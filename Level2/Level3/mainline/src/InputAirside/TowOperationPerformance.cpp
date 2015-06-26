#include "StdAfx.h"
#include ".\towoperationperformance.h"
#include "../Common/ProbDistEntry.h"
#include "../Common/ProDistrubutionData.h"
#include "../Database/ADODatabase.h"
#include "../Inputs/IN_TERM.H"
#include "..\Common\AirportDatabase.h"
#include "AirsideImportExportManager.h"
#include <algorithm>
#include "..\Database\DBElementCollection_Impl.h"
using namespace ADODB;

TowOperationPerformance::TowOperationPerformance(void)
:m_nID(-1)
,m_pAirportDB(NULL)
{
	m_pPushbackSpeedDist = new CProDistrubution;
	m_pUnhookAndTaxiTimeDist = new CProDistrubution;
	m_pHookupTime = new CProDistrubution;
	m_pAttachedAccelerationDist = new CProDistrubution;
	m_pAttachedDecelerationDist = new CProDistrubution;
	m_pTowSpeedDist = new CProDistrubution;
	InitDistValue();

}

TowOperationPerformance::~TowOperationPerformance(void)
{
	delete m_pPushbackSpeedDist;
	m_pPushbackSpeedDist = NULL;

	delete m_pUnhookAndTaxiTimeDist;
	m_pUnhookAndTaxiTimeDist = NULL;

	delete m_pHookupTime;
	m_pHookupTime = NULL;

	delete m_pAttachedAccelerationDist;
	m_pAttachedAccelerationDist = NULL;

	delete m_pAttachedDecelerationDist;
	m_pAttachedDecelerationDist = NULL;

	delete m_pTowSpeedDist;
	m_pTowSpeedDist = NULL;
}

void TowOperationPerformance::InitDistValue()
{
	m_pPushbackSpeedDist->setDistName(_T("U[1~5]"));
	m_pPushbackSpeedDist->setProType(UNIFORM);
	m_pPushbackSpeedDist->setPrintDist(_T("Uniform:1;5"));
	m_pPushbackSpeedDist->initProbilityDistribution();

	m_pUnhookAndTaxiTimeDist->setDistName(_T("U[3~5]"));
	m_pUnhookAndTaxiTimeDist->setProType(UNIFORM);
	m_pUnhookAndTaxiTimeDist->setPrintDist(_T("Uniform:3;5"));
	m_pUnhookAndTaxiTimeDist->initProbilityDistribution();

	m_pHookupTime->setDistName(_T("U[3~5]"));
	m_pHookupTime->setProType(UNIFORM);
	m_pHookupTime->setPrintDist(_T("Uniform:3;5"));
	m_pHookupTime->initProbilityDistribution();

	m_pAttachedAccelerationDist->setDistName(_T("B[0~3]"));
	m_pAttachedAccelerationDist->setProType(BETA);
	m_pAttachedAccelerationDist->setPrintDist(_T("Beta:0;3"));
	m_pAttachedAccelerationDist->initProbilityDistribution();

	m_pAttachedDecelerationDist->setDistName(_T("B[1~5]"));
	m_pAttachedDecelerationDist->setProType(BETA);
	m_pAttachedDecelerationDist->setPrintDist(_T("Beta:1;5"));
	m_pAttachedDecelerationDist->initProbilityDistribution();

	m_pTowSpeedDist->setDistName(_T("U[5~8]"));
	m_pTowSpeedDist->setProType(UNIFORM);
	m_pTowSpeedDist->setPrintDist(_T("Uniform:5;8"));
	m_pTowSpeedDist->initProbilityDistribution();
}

void TowOperationPerformance::InitFromDBRecordset(CADORecordset& recordset)
{
	if(!recordset.IsEOF()){
		recordset.GetFieldValue(_T("ID"),m_nID);

		CString strFltType;
		recordset.GetFieldValue(_T("FLTTYPE"),strFltType);
		if(m_pAirportDB)
		{
			m_fltType.SetAirportDB(m_pAirportDB);
			m_fltType.setConstraintWithVersion(strFltType.GetBuffer(strFltType.GetLength()+1));
			strFltType.ReleaseBuffer(strFltType.GetLength()+1);
		}

		int nProbID = -1;

		recordset.GetFieldValue(_T("PUSHSPEED"),nProbID);	
		m_pPushbackSpeedDist->ReadData(nProbID);

		recordset.GetFieldValue(_T("UNHOOKTIME"),nProbID);
		m_pUnhookAndTaxiTimeDist->ReadData(nProbID);

		recordset.GetFieldValue(_T("HOOKUPTIME"),nProbID);
		m_pHookupTime->ReadData(nProbID);

		recordset.GetFieldValue(_T("ACCELERATION"),nProbID);
		m_pAttachedAccelerationDist->ReadData(nProbID);

		recordset.GetFieldValue(_T("DECELERATION"),nProbID);
		m_pAttachedDecelerationDist->ReadData(nProbID);

		recordset.GetFieldValue(_T("TOWSPEED"),nProbID);
		m_pTowSpeedDist->ReadData(nProbID);
	}
}

void TowOperationPerformance::GetSelectSQL(int nID,CString& strSQL)const
{
	strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM AS_TOWOPERATIONPERFORMANCE WHERE (ID = %d);"),nID);
}

void TowOperationPerformance::GetInsertSQL(int nParentID,CString& strSQL) const
{
	strSQL = _T("");
	char szFltType[1024] = {0};
	CString strFltType = _T("");

	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strFltType = szFltType;
	

	strSQL.Format(_T("INSERT INTO  AS_TOWOPERATIONPERFORMANCE (FLTTYPE,PUSHSPEED,UNHOOKTIME,HOOKUPTIME,ACCELERATION,DECELERATION,TOWSPEED)  \
					 VALUES ('%s',%d, %d, %d, %d, %d, %d)"),\
		strFltType,m_pPushbackSpeedDist->getID(),m_pUnhookAndTaxiTimeDist->getID(),m_pHookupTime->getID(),m_pAttachedAccelerationDist->getID(),
		m_pAttachedDecelerationDist->getID(), m_pTowSpeedDist->getID());

}

void TowOperationPerformance::GetUpdateSQL(CString& strSQL) const
{
	strSQL = _T("");
	char szFltType[1024] = {0};
	CString strFltType = _T("");

	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strFltType = szFltType;
	
	strSQL.Format(_T("UPDATE AS_TOWOPERATIONPERFORMANCE SET FLTTYPE = '%s',PUSHSPEED = '%d',UNHOOKTIME = %d, HOOKUPTIME = %d,ACCELERATION = '%d',DECELERATION = '%d',TOWSPEED = %d  \
					 WHERE (ID = %d);"), \
					 strFltType,m_pPushbackSpeedDist->getID(),m_pUnhookAndTaxiTimeDist->getID(),m_pHookupTime->getID(),m_pAttachedAccelerationDist->getID(),
					 m_pAttachedDecelerationDist->getID(), m_pTowSpeedDist->getID(),m_nID);
}

void TowOperationPerformance::GetDeleteSQL(CString& strSQL) const
{
	strSQL = _T("");
	strSQL.Format(_T("DELETE FROM AS_TOWOPERATIONPERFORMANCE WHERE (ID = %d);"),m_nID);
}

void TowOperationPerformance::SaveData()
{
	if (m_nID >=0)
	{
		m_pPushbackSpeedDist->UpdateData();
		m_pUnhookAndTaxiTimeDist->UpdateData();
		m_pHookupTime->UpdateData();
		m_pAttachedAccelerationDist->UpdateData();
		m_pAttachedDecelerationDist->UpdateData();
		m_pTowSpeedDist->UpdateData();
	}
	else
	{
		m_pPushbackSpeedDist->SaveData();
		m_pUnhookAndTaxiTimeDist->SaveData();
		m_pHookupTime->SaveData();
		m_pAttachedAccelerationDist->SaveData();
		m_pAttachedDecelerationDist->SaveData();
		m_pTowSpeedDist->SaveData();
	}
	CString strSQL;

	if (m_nID < 0)
	{
		GetInsertSQL(-1,strSQL);

		if (strSQL.IsEmpty())
			return;

		m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		GetUpdateSQL(strSQL);
		if (strSQL.IsEmpty())
			return;

		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void TowOperationPerformance::SaveData(int nParentID)
{

}

void TowOperationPerformance::RemoveData()
{
	DeleteData();
	m_pPushbackSpeedDist->RemoveData();
	m_pUnhookAndTaxiTimeDist->RemoveData();
	m_pHookupTime->RemoveData();
	m_pAttachedAccelerationDist->RemoveData();
	m_pAttachedDecelerationDist->RemoveData();
	m_pTowSpeedDist->RemoveData();
}

void TowOperationPerformance::ImportData(CAirsideImportFile& importFile)
{

}

void TowOperationPerformance::ExportData(CAirsideExportFile& exportFile)
{

}

/////////////////////////////////////////////////////////////////////////////////////
TowOperationPerformanceList::TowOperationPerformanceList(CAirportDatabase* pAirportDB)
:m_pInputTerminal(NULL)
,m_pAirportDB(pAirportDB)
{

}

TowOperationPerformanceList::~TowOperationPerformanceList()
{
}

void TowOperationPerformanceList::GetSelectElementSQL(int nParentID,CString& strSQL) const
{
	strSQL = _T("");
	strSQL.Format(_T("SELECT * FROM AS_TOWOPERATIONPERFORMANCE "));
}

void TowOperationPerformanceList::ReadData(int nParentID)
{
	CString strSelectSQL;
	GetSelectElementSQL(nParentID,strSelectSQL);
	strSelectSQL.Trim();
	if (strSelectSQL.IsEmpty())
		return;

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSelectSQL, nRecordCount, adoRecordset);

	m_dataList.clear();
	while (!adoRecordset.IsEOF())
	{
		TowOperationPerformance* element = new TowOperationPerformance;
		element->SetAirportDB(m_pAirportDB);
		element->InitFromDBRecordset(adoRecordset);
		m_dataList.push_back(element);
		adoRecordset.MoveNextData();
	}
}

void TowOperationPerformanceList::SaveData()
{
	std::vector<TowOperationPerformance*>::iterator iter = m_dataList.begin();
	for (; iter != m_dataList.end(); iter++)
	{
		(*iter)->SaveData();
	}

	iter = m_deleteddataList.begin();
	for (; iter != m_deleteddataList.end(); iter++)
	{
		(*iter)->RemoveData();
	}
	m_deleteddataList.clear();
}

bool FlightTypeCompare(TowOperationPerformance* fItem,TowOperationPerformance* sItem)
{
	if (fItem->GetFltType().fits(sItem->GetFltType()))
		return false;
	else if(sItem->GetFltType().fits(fItem->GetFltType()))
		return true;

	return false;
}


void TowOperationPerformanceList::SortFlightType()
{
	std::sort(m_dataList.begin(),m_dataList.end(),FlightTypeCompare);
}
