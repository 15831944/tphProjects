#include "StdAfx.h"
#include "RunwayExitDiscription.h"
#include "Runway.h"
#include "Taxiway.h"

RunwayExitDescription::RunwayExitDescription()
:m_nID(-1)
,m_side(RunwayExit::EXIT_LEFT)
,m_rwMark(RUNWAYMARK_FIRST)
,m_strName(_T(""))
,m_dAngle(0.0)
,m_nRunwayID(-1)
,m_sRunway(_T(""))
,m_nRunwayOrTaxiwayID(-1)
,m_sTaxiway(_T(""))
,m_nIndex(-1)
{
	m_vResult.clear();
	
}

RunwayExitDescription::~RunwayExitDescription()
{

}

void RunwayExitDescription::SaveData()
{
	if (m_nID >=0 )
	{
		return UpdateData();
	}
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_RUNWAY_RUNWAYEXITDISCRIPTION\
					 (RUNWAYID, MARKINDEX, EXITSIDE, NAME, ANGLE, TAXIWAYID,RUNWAYNAME,TAXIWAYNAME,NODEINDEX)\
					 VALUES (%d,%d,%d,'%s',%f,%d,'%s','%s',%d)"),
					 m_nRunwayID,(int)m_rwMark,(int)m_side,m_strName,m_dAngle, m_nRunwayOrTaxiwayID,m_sRunway,m_sTaxiway,m_nIndex);


	m_nID =  CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

}

void RunwayExitDescription::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_RUNWAY_RUNWAYEXITDISCRIPTION\
					 SET MARKINDEX =%d, EXITSIDE =%d, NAME ='%s', ANGLE = %f, TAXIWAYID = %d,RUNWAYID = %d, NODEINDEX =%d,\
					 RUNWAYNAME = '%s',TAXIWAYNAME = '%s' WHERE (ID = %d)"),
					 (int)m_rwMark,(int)m_side,m_strName,m_dAngle,m_nRunwayOrTaxiwayID,m_nRunwayID,m_nIndex,m_sRunway,m_sTaxiway,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void RunwayExitDescription::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_RUNWAY_RUNWAYEXITDISCRIPTION WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void RunwayExitDescription::ReadData(int nID)
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT *\
					 FROM IN_RUNWAY_RUNWAYEXITDISCRIPTION\
					 WHERE (ID = %d)"),nID);
	long lRecordCount = 0;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL,lRecordCount,adoRecordset);
	if(!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue(_T("ID"),m_nID);
		adoRecordset.GetFieldValue(_T("RUNWAYID"),m_nRunwayID);
		adoRecordset.GetFieldValue(_T("RUNWAYNAME"),m_sRunway);
		if (m_sRunway.IsEmpty())
		{
			Runway runway;
			runway.ReadObject(m_nRunwayID);
			m_sRunway = runway.GetObjectName().GetIDString();
		}
		
		int nMarkIndex = RUNWAYMARK_FIRST;
		adoRecordset.GetFieldValue(_T("MARKINDEX"),nMarkIndex);
		if (nMarkIndex != RUNWAYMARK_FIRST)
			nMarkIndex = RUNWAYMARK_SECOND;
		m_rwMark = (RUNWAY_MARK)nMarkIndex;

		int nExitSide = RunwayExit::EXIT_LEFT;
		adoRecordset.GetFieldValue(_T("EXITSIDE"),nExitSide);
		if (nExitSide != RunwayExit::EXIT_LEFT)
		{
			nExitSide = RunwayExit::EXIT_RIGHT;
		}
		m_side = (RunwayExit::ExitSide)nExitSide;

		adoRecordset.GetFieldValue(_T("ANGLE"),m_dAngle);

		adoRecordset.GetFieldValue(_T("NAME"), m_strName );
		adoRecordset.GetFieldValue(_T("NODEINDEX"),m_nIndex);

		adoRecordset.GetFieldValue(_T("TAXIWAYID"),m_nRunwayOrTaxiwayID);
		adoRecordset.GetFieldValue(_T("TAXIWAYNAME"),m_sTaxiway); 
		if (m_sTaxiway.IsEmpty())
		{
			Taxiway taxiway;
			taxiway.ReadObject(m_nRunwayOrTaxiwayID);
			m_sTaxiway = taxiway.GetObjectName().GetIDString();
		}
	}
	
}

void RunwayExitDescription::InitData(const RunwayExit& rwyExit)
{
	m_side = rwyExit.GetSideType();
	m_rwMark = rwyExit.GetRunwayMark();
	m_strName = rwyExit.GetName();
	m_dAngle = rwyExit.GetAngle();
	m_nRunwayID = rwyExit.GetRunwayID();
	m_nRunwayOrTaxiwayID = rwyExit.GetTaxiwayID();
	m_nIndex = rwyExit.GetIntesectionNode().GetIndex();
	Taxiway taxiway;
	taxiway.ReadObject(m_nRunwayOrTaxiwayID);
	m_sTaxiway = taxiway.GetObjectName().GetIDString();

	Runway runway;
	runway.ReadObject(m_nRunwayID);
	m_sRunway = runway.GetObjectName().GetIDString();
}

void RunwayExitDescription::SetID(int nID)
{
	m_nID = nID;
}

int RunwayExitDescription::GetID()const
{
	return m_nID;
}

void RunwayExitDescription::SetName(CString strName)
{
	m_strName = strName;
}

void RunwayExitDescription::SetRunwayName(const CString& sRunway)
{
	m_sRunway = sRunway;
}

void RunwayExitDescription::SetTaxiwayName(const CString& sTaxiway)
{
	m_sTaxiway = sTaxiway;
}

const CString& RunwayExitDescription::GetRunwayName()const
{
	return m_sRunway;
}

const CString& RunwayExitDescription::GetTaxiwayName()const
{
	return m_sTaxiway;
}

CString RunwayExitDescription::GetName()const
{
	return m_strName;
}

void RunwayExitDescription::SetRunwayID(int nID)
{
	m_nRunwayID = nID;
}

int RunwayExitDescription::GetRunwayID()const
{
	return m_nRunwayID;
}

void RunwayExitDescription::SetIntersectRunwayOrTaxiwayID(int nID)
{
	m_nRunwayOrTaxiwayID = nID;
}

int RunwayExitDescription::GetTaxiwayID()const
{
	return m_nRunwayOrTaxiwayID;
}

void RunwayExitDescription::SetExitSide(RunwayExit::ExitSide side)
{
	m_side = side;
}

RunwayExit::ExitSide RunwayExitDescription::GetSideType()const
{
	return m_side;
}

void RunwayExitDescription::SetAngle(double dAngle)
{
	m_dAngle = dAngle;
}

double RunwayExitDescription::GetAngle()const
{
	return m_dAngle;
}

void RunwayExitDescription::SetRunwayMark(RUNWAY_MARK rwMark)
{
	m_rwMark = rwMark;
}

int RunwayExitDescription::GetIndex()const
{
	return m_nIndex;
}

RUNWAY_MARK RunwayExitDescription::GetRunwayMark()const
{
	return m_rwMark;
}

bool RunwayExitDescription::EqualToRunwayExit(const RunwayExit& runwayExit)const
{
	if(m_nRunwayID == runwayExit.GetRunwayID()&&
		m_nRunwayOrTaxiwayID == runwayExit.GetTaxiwayID()&&
		m_rwMark == runwayExit.GetRunwayMark()&&
		m_side == runwayExit.GetSideType()&&
		m_nIndex == runwayExit.GetIntesectionNode().GetIndex())
	{
		return true;
	}
	
	return false;
}