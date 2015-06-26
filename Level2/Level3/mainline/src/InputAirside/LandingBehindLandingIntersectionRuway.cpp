#include "stdafx.h"
#include "LandingBehindLandingIntersectionRuway.h"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"
#include "ALTAirport.h"
#include "InputAirside.h"


LandingBehindLandingIntersectionRuwayNONLAHSO::LandingBehindLandingIntersectionRuwayNONLAHSO(int nProjID)
:AircraftClassificationBasisApproachSeparationItem(nProjID,LandingBehindLandingOnIntersectingRunway_NON_LAHSO)
{
}

LandingBehindLandingIntersectionRuwayNONLAHSO::~LandingBehindLandingIntersectionRuwayNONLAHSO(void)
{
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//LandingBehindLandingIntersectionRuwayLAHSO
LandingBehindLandingIntersectionRuwayLAHSO::LandingBehindLandingIntersectionRuwayLAHSO(int nProjID)
:ApproachSeparationItem(nProjID,LandingBehindLandingOnIntersectingRunway_LAHSO)
{
}

LandingBehindLandingIntersectionRuwayLAHSO::~LandingBehindLandingIntersectionRuwayLAHSO(void)
{
	Clear();
}

void LandingBehindLandingIntersectionRuwayLAHSO::ReadData()
{
	CString strSQL;
	strSQL.Format("SELECT * FROM APPROACHSEPARATIONCRITERIA WHERE PROJID = %d AND APPSEPTYPE = %d ORDER BY ID",
		m_nProjID, (int)m_emType);
	CADORecordset classADORecordSet;
	long lEffect = 0L;
	CADODatabase::ExecuteSQLStatement(strSQL,lEffect,classADORecordSet);

	Clear();

	std::vector<int>::iterator iterRunwayID;
	std::vector<int> vAirportIds;
	ALTAirport airport;
	std::vector<int> vRunways;
	std::vector<int>::iterator iterAirportID;
	if(classADORecordSet.IsEOF())
	{//if it is first time read DB,and the table must be empty.
		
		InputAirside::GetAirportList(m_nProjID, vAirportIds);
		for (iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
		{		
			airport.ReadAirport(*iterAirportID);

			vRunways.clear();
			ALTAirport::GetRunwaysIDs(*iterAirportID, vRunways);			
			for (iterRunwayID = vRunways.begin(); iterRunwayID != vRunways.end(); ++iterRunwayID)
			{
				MatchIntersectRunway * pNode = new MatchIntersectRunway;
				pNode->nRunwayID = *iterRunwayID;
				pNode->markLogicRunway = RUNWAYMARK_FIRST;
				pNode->bHadMatch = false;
				pNode->nIntersectRunwayID = -1;
				pNode->markIntersectLogicRunway = RUNWAYMARK_FIRST;
				m_vectSelected.push_back(pNode);
			}
		}
	}
	else
	{
		std::vector<int> vrRunwayTotal;
		int iPort = -1;

		InputAirside::GetAirportList(m_nProjID, vAirportIds);
		for (iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
		{			
			airport.ReadAirport(*iterAirportID);
			
			vRunways.clear();
			ALTAirport::GetRunwaysIDs(*iterAirportID, vRunways);	
			for (iterRunwayID = vRunways.begin();iterRunwayID != vRunways.end(); ++iterRunwayID)
			{
				MatchIntersectRunway * pNode = new MatchIntersectRunway; 

				strSQL.Format("SELECT * FROM APPROACHSEPARATIONCRITERIA WHERE PROJID = %d AND APPSEPTYPE = %d AND RUNWAYID = %d ",\
					          m_nProjID, (int)m_emType,*iterRunwayID);
				CADODatabase::ExecuteSQLStatement(strSQL,lEffect,classADORecordSet);
				if(!classADORecordSet.IsEOF())
				{
					classADORecordSet.GetFieldValue(_T("RUNWAYID"),pNode->nRunwayID);
					classADORecordSet.GetFieldValue(_T("RUNWAYPORT"),iPort);
					pNode->markLogicRunway = (RUNWAY_MARK)iPort;
					classADORecordSet.GetFieldValue(_T("INTRRUNWAYID"),pNode->nIntersectRunwayID);
					classADORecordSet.GetFieldValue(_T("INTRRUNWAYPORT"),iPort);
					pNode->markIntersectLogicRunway = (RUNWAY_MARK)iPort;
					pNode->bHadMatch = pNode->nIntersectRunwayID == -1?false:true;
				}
				else
				{
					pNode->nRunwayID = *iterRunwayID;
					pNode->markLogicRunway = RUNWAYMARK_FIRST;
					pNode->bHadMatch = false;
					pNode->nIntersectRunwayID = -1;
					pNode->markIntersectLogicRunway = RUNWAYMARK_FIRST;
				}
				
				m_vectSelected.push_back(pNode);
			}
		}		
	}
}

CString LandingBehindLandingIntersectionRuwayLAHSO::GetRunwayName(int nRunwayID)
{
	Runway runway;
	runway.ReadObject(nRunwayID);	
	return(runway.GetObjNameString());
}
CString LandingBehindLandingIntersectionRuwayLAHSO::GetRunwayFirstPort(int nRunwayID)
{
	Runway runway;
	runway.ReadObject(nRunwayID);	
	return(runway.GetMarking1().c_str());
}
CString LandingBehindLandingIntersectionRuwayLAHSO::GetRunwaySecondPort(int nRunwayID)
{
	Runway runway;
	runway.ReadObject(nRunwayID);	
	return(runway.GetMarking2().c_str());
}

void LandingBehindLandingIntersectionRuwayLAHSO::SaveData()
{
	ApproachSeparationItem::SaveData();

	CString strSQL;	
	for (std::vector<MatchIntersectRunway *>::iterator iterMatchWay = m_vectSelected.begin();iterMatchWay != m_vectSelected.end();iterMatchWay++)
	{
		strSQL.Format(_T("INSERT INTO APPROACHSEPARATIONCRITERIA \
						(PROJID,\
						APPSEPTYPE,\
						ARCCLASSTYPE,\
						TRAILID,\
						LEADID,\
						MINSEPARATION,\
						RUNWAYID,\
						RUNWAYPORT,\
						INTRRUNWAYID,\
						INTRRUNWAYPORT,\
						TIMEDIFFINTRFLTPATH,\
						TIMEDIFFINTRRUNWAY,\
						DISDIFFINTRFLTPATH,\
						DISDIFFINTRRUNWAY,\
						LEADDISFROMTHRESHOLD) \
						VALUES (%d,%d,%d,%d,%d,%f,%d,%d,%d,%d,%f,%f,%f,%f,%f)"), \
						m_nProjID,\
						(int)LandingBehindLandingOnIntersectingRunway_LAHSO,\
						-1,\
						-1,\
						-1,\
						-1.0,\
						(*iterMatchWay)->nRunwayID,\
						int((*iterMatchWay)->markLogicRunway),\
						(*iterMatchWay)->nIntersectRunwayID,\
						int((*iterMatchWay)->markIntersectLogicRunway),\
						-1.0,\
						-1.0,\
						-1.0,\
						-1.0,\
						-1.0);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}
void LandingBehindLandingIntersectionRuwayLAHSO::UpdateData()
{
	return;
}
void LandingBehindLandingIntersectionRuwayLAHSO::ImportData(CAirsideImportFile& importFile)
{
	ApproachSeparationItem::ImportData(importFile);
	Clear();
	int iCount = -1;
	int iTemp = -1;
	importFile.getFile().getInteger(iCount);
	for (int i = 0;i < iCount;i ++)
	{
		MatchIntersectRunway * pNode = new MatchIntersectRunway;
		importFile.getFile().getInteger(pNode->nRunwayID);
		importFile.getFile().getInteger(iTemp);
		pNode->markLogicRunway = (RUNWAY_MARK)iTemp;
		importFile.getFile().getInteger(pNode->nIntersectRunwayID);
		importFile.getFile().getInteger(iTemp);
		pNode->markIntersectLogicRunway = (RUNWAY_MARK)iTemp;	
		pNode->bHadMatch = pNode->nIntersectRunwayID == -1?false:true;		
		m_vectSelected.push_back(pNode);
	}
	importFile.getFile().getLine();
}
void LandingBehindLandingIntersectionRuwayLAHSO::ExportData(CAirsideExportFile& exportFile)
{
	ApproachSeparationItem::ExportData(exportFile);
	exportFile.getFile().writeInt(int(m_vectSelected.size()) );
	for (std::vector<MatchIntersectRunway *>::iterator iterMatchWay = m_vectSelected.begin();iterMatchWay != m_vectSelected.end();iterMatchWay++)
	{
		exportFile.getFile().writeInt((*iterMatchWay)->nRunwayID);
		exportFile.getFile().writeInt(int((*iterMatchWay)->markLogicRunway));
		exportFile.getFile().writeInt((*iterMatchWay)->nIntersectRunwayID);
		exportFile.getFile().writeInt(int((*iterMatchWay)->markIntersectLogicRunway));		
	}
	exportFile.getFile().writeLine();
}

void LandingBehindLandingIntersectionRuwayLAHSO::Clear()
{
	for (int i = 0; i < (int)m_vectSelected.size(); i++)
	{
		if(m_vectSelected[i])
			delete  m_vectSelected[i];
	}
	m_vectSelected.clear();
}