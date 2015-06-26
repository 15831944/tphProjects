#include "stdafx.h"
#include "ApproachSeparationItem.h"
#include "AirsideImportExportManager.h"
#include "../Database/ADODatabase.h"
#include "../Common/ARCUnit.h"

void AircraftClassificationBasisItem::ImportData(CAirsideImportFile& importFile)
{
	int nProjID = -1;
	importFile.getFile().getInteger(nProjID);
	m_nProjID = importFile.getNewProjectID();

	int nType;
	importFile.getFile().getInteger(nType);
	if ( nType >= WingspanBased && nType <= CategoryType_Count)
	{
		m_emClassifiBasis = (FlightClassificationBasisType)nType;
	}

	int nID = -1;
	importFile.getFile().getInteger(nID);
	m_aciTrail.m_nID = importFile.GetAircraftClassificationsNewID(nID);
	char szName[_MAX_PATH] = {0};	
	importFile.getFile().getField(szName,_MAX_PATH);
	m_aciTrail.m_strName = szName;

	nID = -1;
	importFile.getFile().getInteger(nID);
	m_aciLead.m_nID = importFile.GetAircraftClassificationsNewID(nID);
	memset(szName,0,_MAX_PATH);
	importFile.getFile().getField(szName,_MAX_PATH);
	m_aciLead.m_strName = szName;

	importFile.getFile().getFloat(m_dMinSeparation);
	importFile.getFile().getFloat(m_dTimeDiffInFltPathIntersecting);
	importFile.getFile().getFloat(m_dTimeDiffOnRunwayIntersecting);
	importFile.getFile().getFloat(m_dDistanceDiffInFltPathIntersecting);
	importFile.getFile().getFloat(m_dDistanceDiffOnRunwayIntersecting);
	importFile.getFile().getLine();
}
void AircraftClassificationBasisItem::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt((int)m_emClassifiBasis);

	exportFile.getFile().writeInt(m_aciTrail.m_nID);
	exportFile.getFile().writeField(m_aciTrail.m_strName.GetBuffer(_MAX_PATH));
	m_aciTrail.m_strName.ReleaseBuffer(_MAX_PATH);

	exportFile.getFile().writeInt(m_aciLead.m_nID);
	exportFile.getFile().writeField(m_aciLead.m_strName.GetBuffer(_MAX_PATH));
	m_aciLead.m_strName.ReleaseBuffer(_MAX_PATH);	

	exportFile.getFile().writeDouble(m_dMinSeparation);
	exportFile.getFile().writeDouble(m_dTimeDiffInFltPathIntersecting);
	exportFile.getFile().writeDouble(m_dTimeDiffOnRunwayIntersecting);
	exportFile.getFile().writeDouble(m_dDistanceDiffInFltPathIntersecting);
	exportFile.getFile().writeDouble(m_dDistanceDiffOnRunwayIntersecting);
	exportFile.getFile().writeLine();
}
void AircraftClassificationBasisItem::SaveData(ApproachSeparationType emType)
{
	CString strSQL;
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
					 (int)emType,\
					 (int)m_emClassifiBasis,\
					 m_aciTrail.m_nID,\
					 m_aciLead.m_nID,\
					 m_dMinSeparation,\
					 -1,\
					 -1,\
					 -1,\
					 -1,\
					 m_dTimeDiffInFltPathIntersecting,\
					 m_dTimeDiffOnRunwayIntersecting,\
					 m_dDistanceDiffInFltPathIntersecting,\
					 m_dDistanceDiffOnRunwayIntersecting,\
					 -1.0);
	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
} 

AircraftClassificationBasisApproachSeparationItem::AircraftClassificationBasisApproachSeparationItem(int nProjID, ApproachSeparationType emType,FlightClassificationBasisType emFltTypeDefault /*= WingspanBased*/)
:ApproachSeparationItem(nProjID,emType)
,m_emFltClassBasis(emFltTypeDefault)
{
}

AircraftClassificationBasisApproachSeparationItem::~AircraftClassificationBasisApproachSeparationItem(void)
{
	Clear();
}

bool AircraftClassificationBasisApproachSeparationItem::DeleteRecord(int nID)
{
	CString strSQL;
	strSQL.Format(_T("DELETE FROM APPROACHSEPARATIONCRITERIA WHERE  ID = %d AND PROJID = %d AND APPSEPTYPE = %d "),nID,m_nProjID,m_emType);
	CADODatabase::ExecuteSQLStatement(strSQL);
	return (true);
}

//if table "AIRCRAFTCLASSIFICATION" has no record's id match  TRAILID or LEADID ,then ReadData() delete this record;
void AircraftClassificationBasisApproachSeparationItem::ReadData()
{
	CString strSQL;
	strSQL.Format("SELECT * FROM APPROACHSEPARATIONCRITERIA WHERE PROJID = %d AND APPSEPTYPE = %d ORDER BY ID",
		m_nProjID, (int)m_emType);
	CADORecordset pADORecordSet;
	long lEffect = 0L;
	CADODatabase::ExecuteSQLStatement(strSQL,lEffect,pADORecordSet);

	Clear();

	if(pADORecordSet.IsEOF())
	{//if it is first time read DB,and the table must be empty.
		ShowItemsByFltType(WakeVortexWightBased);
	}
	else
	{

		while (!pADORecordSet.IsEOF())
		{
			AircraftClassificationBasisItem* pNewItem = new AircraftClassificationBasisItem;

			pADORecordSet.GetFieldValue(_T("ID"),pNewItem->m_nID);

			pNewItem->m_nProjID = m_nProjID;
			
			int nAircraftClassificationType = -1;
			pADORecordSet.GetFieldValue(_T("ARCCLASSTYPE"),nAircraftClassificationType);
			pNewItem->m_emClassifiBasis = (FlightClassificationBasisType)nAircraftClassificationType;

			m_emFltClassBasis = (FlightClassificationBasisType)nAircraftClassificationType;//it is same type for all records.

			break;
		}

		AircraftClassifications * pFltType = 0;
		pFltType = new AircraftClassifications(m_nProjID,m_emFltClassBasis);
		if(!pFltType)return;
		pFltType->ReadData();
		int iSize = pFltType->GetCount();
		int iID = 1;
		for(int i = 0;i < iSize;i++)
			for (int j = 0;j < iSize;j++)
			{
				AircraftClassificationBasisItem* pNewItem = new AircraftClassificationBasisItem;
				if(!pNewItem)
				{
					Clear();
					return;
				}

				pNewItem->m_nID = iID++;
				pNewItem->m_nProjID = m_nProjID;
				pNewItem->m_emClassifiBasis = m_emFltClassBasis;			
				pNewItem->m_aciTrail = * (pFltType->GetItem(i) );
				pNewItem->m_aciLead = * (pFltType->GetItem(j) );

				strSQL.Format("SELECT * FROM APPROACHSEPARATIONCRITERIA WHERE PROJID = %d AND APPSEPTYPE = %d AND \
							  ARCCLASSTYPE = %d AND TRAILID = %d AND LEADID = %d ",	m_nProjID, (int)m_emType,\
							  m_emFltClassBasis,pFltType->GetItem(i)->m_nID,pFltType->GetItem(j)->m_nID);
				CADODatabase::ExecuteSQLStatement(strSQL,lEffect,pADORecordSet);

				if(!pADORecordSet.IsEOF())
				{
					pADORecordSet.GetFieldValue(_T("MINSEPARATION"),pNewItem->m_dMinSeparation);
					pADORecordSet.GetFieldValue(_T("TIMEDIFFINTRFLTPATH"),pNewItem->m_dTimeDiffInFltPathIntersecting);
					pADORecordSet.GetFieldValue(_T("TIMEDIFFINTRRUNWAY"),pNewItem->m_dTimeDiffOnRunwayIntersecting);
					pADORecordSet.GetFieldValue(_T("DISDIFFINTRFLTPATH"),pNewItem->m_dDistanceDiffInFltPathIntersecting);
					pADORecordSet.GetFieldValue(_T("DISDIFFINTRRUNWAY"),pNewItem->m_dDistanceDiffOnRunwayIntersecting);
				}
				else
				{
					//assign default.
					if(pNewItem->m_emClassifiBasis == WakeVortexWightBased)
					{
						if(pNewItem->m_aciLead.getName() == _T("Light"))
							pNewItem->m_dMinSeparation = 555600.0;
						else if(pNewItem->m_aciLead.getName() == _T("Medium"))
							pNewItem->m_dMinSeparation = 740800.0;
						else if(pNewItem->m_aciLead.getName() == _T("Heavy"))
							pNewItem->m_dMinSeparation = 926000.0;
					}
					else
						pNewItem->m_dMinSeparation = 926000.0;
					pNewItem->m_dTimeDiffInFltPathIntersecting = 120.0;
					pNewItem->m_dTimeDiffOnRunwayIntersecting = 120.0;
					pNewItem->m_dDistanceDiffInFltPathIntersecting = 10000.0;
					pNewItem->m_dDistanceDiffOnRunwayIntersecting = 10000.0;
				}

				m_vectClassfiBasis.push_back(pNewItem);
			}
		delete pFltType;
	}
}

void AircraftClassificationBasisApproachSeparationItem::SaveData()
{	
	ApproachSeparationItem::SaveData();
	for (int i=0; i< (int)m_vectClassfiBasis.size(); i++)
		m_vectClassfiBasis[i]->SaveData(m_emType);
}

//it reads record from table "AIRCRAFTCLASSIFICATION" , and make a data struct with emFltType ,
//if had been ReadData(),it do nothing.
bool AircraftClassificationBasisApproachSeparationItem::ShowItemsByFltType(FlightClassificationBasisType emFltType)
{
	if(m_emFltClassBasis == emFltType)
		return (true);

	AircraftClassifications * pFltType = 0;
	pFltType = new AircraftClassifications(m_nProjID,emFltType);
	if(!pFltType)return(false);

	Clear();
	pFltType->ReadData();
	int iSize = pFltType->GetCount();
	int iID = 1;
	for(int i = 0;i < iSize;i++)
		for (int j = 0;j < iSize;j++)
		{
			AircraftClassificationBasisItem* pNewItem = new AircraftClassificationBasisItem;
			if(!pNewItem)return(false);

			pNewItem->m_nID = iID++;
			pNewItem->m_nProjID = m_nProjID;
			pNewItem->m_emClassifiBasis = emFltType;			
			pNewItem->m_aciTrail = * (pFltType->GetItem(i) );
			pNewItem->m_aciLead = * (pFltType->GetItem(j) );

			//assign default.
			if(emFltType == WakeVortexWightBased)
			{
				if(pNewItem->m_aciLead.getName() == _T("Light"))
					pNewItem->m_dMinSeparation = 555600.0;
				else if(pNewItem->m_aciLead.getName() == _T("Medium"))
					pNewItem->m_dMinSeparation = 740800.0;
				else if(pNewItem->m_aciLead.getName() == _T("Heavy"))
					pNewItem->m_dMinSeparation = 926000.0;
			}
			else
				pNewItem->m_dMinSeparation = 926000.0;
			pNewItem->m_dTimeDiffInFltPathIntersecting = 120.0;
			pNewItem->m_dTimeDiffOnRunwayIntersecting = 120.0;
			pNewItem->m_dDistanceDiffInFltPathIntersecting = 10000.0;
			pNewItem->m_dDistanceDiffOnRunwayIntersecting = 10000.0;

			m_vectClassfiBasis.push_back(pNewItem);
		}
	delete pFltType;
	m_emFltClassBasis = emFltType;
	return(true);
} 

void AircraftClassificationBasisApproachSeparationItem::Clear()
{
	for (int i=0; i< (int)m_vectClassfiBasis.size(); i++)
	{
		if(m_vectClassfiBasis[i])
			delete  m_vectClassfiBasis[i];
	}

	m_vectClassfiBasis.clear();
}
void AircraftClassificationBasisApproachSeparationItem::ImportData(CAirsideImportFile& importFile)
{
	ApproachSeparationItem::ImportData(importFile);
	Clear();
	int nACBasisCount = 0;
	importFile.getFile().getInteger(nACBasisCount);
	importFile.getFile().getLine();

	for (int i =0; i < nACBasisCount ; ++ i)
	{
		AircraftClassificationBasisItem * pNewItem = new AircraftClassificationBasisItem ;
		pNewItem->ImportData(importFile);
		m_vectClassfiBasis.push_back(pNewItem);
	}
}
void AircraftClassificationBasisApproachSeparationItem::ExportData(CAirsideExportFile& exportFile)
{
	ApproachSeparationItem::ExportData(exportFile);

	int nDataSize = static_cast<int>(m_vectClassfiBasis.size());
	exportFile.getFile().writeInt(nDataSize);
	exportFile.getFile().writeLine();

	for (ACBasisItemIter iter = m_vectClassfiBasis.begin();
		iter != m_vectClassfiBasis.end(); iter++)
	{
		(*iter)->ExportData(exportFile);	
	}
}

AircraftClassificationBasisItem * AircraftClassificationBasisApproachSeparationItem::GetItem( const AircraftClassificationItem& leadAc, const AircraftClassificationItem& trailAC ) const
{
	for(int i =0;i< (int)m_vectClassfiBasis.size();i++)
	{
		AircraftClassificationBasisItem * thisItem = m_vectClassfiBasis.at(i);
		if(leadAc.getID() ==thisItem->m_aciLead.getID() && trailAC.getID() == thisItem->m_aciTrail.getID())
		{
			return thisItem;
		}
	}
	return NULL;
}

ApproachSeparationItem::ApproachSeparationItem(int nProjID,ApproachSeparationType emType)
:m_nProjID(nProjID),
m_emType(emType)
{
}

ApproachSeparationItem::~ApproachSeparationItem(void)
{	
}

void ApproachSeparationItem::ImportData(CAirsideImportFile& importFile)
{
	importFile.getFile().getInteger(m_nProjID);
	m_nProjID = importFile.getNewProjectID();
	int iAppSepType = -1;
	importFile.getFile().getInteger(iAppSepType);
	m_emType = (ApproachSeparationType)iAppSepType;
	importFile.getFile().getLine();
}
void ApproachSeparationItem::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nProjID);
	exportFile.getFile().writeInt(m_emType);
	exportFile.getFile().writeLine();
}

void ApproachSeparationItem::ReadData()
{	
	ASSERT(FALSE);
	return;	
}

void ApproachSeparationItem::SaveData()
{
	CString strSQL;
	strSQL.Format(_T("DELETE FROM APPROACHSEPARATIONCRITERIA WHERE PROJID = %d AND APPSEPTYPE = %d"),m_nProjID,m_emType);
	CADODatabase::ExecuteSQLStatement(strSQL);
}
