#include "StdAfx.h"
#include "FlightTypeRunwayExitStrategyItem.h"
#include "TimeRangeRunwayExitStrategyItem.h"
#include "AirsideImportExportManager.h"
#include "RunwayExitStandStrategyItem.h"
#include "Common/FLT_CNST.H"
#include "../Database/ADODatabase.h"
#include "../Common/ALTObjectID.h"


#include <algorithm>

FlightTypeRunwayExitStrategyItem::FlightTypeRunwayExitStrategyItem(int nProjectID)
	: m_nProjectID(nProjectID)
	, m_nID(-1)
	, m_pAirportDatabase(NULL)
{
}

FlightTypeRunwayExitStrategyItem::~FlightTypeRunwayExitStrategyItem(void)
{
	RemoveAll();
}

bool StandCompare(RunwayExitStandStrategyItem* fItem,RunwayExitStandStrategyItem* sItem)
{
	ALTObjectID firstObjectID(fItem->getStandFamilyName());
	ALTObjectID secondObjectID(sItem->getStandFamilyName());
	if (firstObjectID.idFits(secondObjectID))
		return true;

	return false;
}
void FlightTypeRunwayExitStrategyItem::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, RWEXITSTRAID, STANDID\
		FROM IN_RUNWAY_STAND_STRATEGY\
		WHERE (RWEXITSTRAID = %d)"),m_nID);

	CADORecordset adoRecordset;
	long nRecordAfflect = 0L;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordAfflect,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		RunwayExitStandStrategyItem *pItem = new RunwayExitStandStrategyItem(m_nID);

		int nID = -1;
		adoRecordset.GetFieldValue(_T("ID"),nID);
		pItem->setID(nID);

		int nExitStrategyID = -1;
		adoRecordset.GetFieldValue(_T("RWEXITSTRAID"),nExitStrategyID);
		pItem->setExitStrategyID(nExitStrategyID);

		int nStandID = -1;
		adoRecordset.GetFieldValue(_T("STANDID"),nStandID);
		pItem->setStandFamilyID(nStandID);

		pItem->ReadData();

		m_vectRunwayExitStandStrategyItems.push_back(pItem);

		adoRecordset.MoveNextData();
	}

	std::sort(m_vectRunwayExitStandStrategyItems.begin(), m_vectRunwayExitStandStrategyItems.end(),StandCompare);
}

void FlightTypeRunwayExitStrategyItem::SaveData()
{
	if (m_nID != -1)
	{
		return UpdateData();
	}

	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_RUNWAY_EXITSTRATEGY\
		(PROJID, FLTTYPE)\
		VALUES (%d,'%s')"),m_nProjectID,szFltType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	for (RunwayExitStandStrategyItemIter iter = m_vectRunwayExitStandStrategyItems.begin(); 
		iter != m_vectRunwayExitStandStrategyItems.end(); iter++)
	{
		if(-1 == (*iter)->getID())
		{
			(*iter)->setExitStrategyID(m_nID);
			(*iter)->SaveData();
		}
		else
			(*iter)->UpdateData();
	}

	DeleteDataFromDatabase();
}

void FlightTypeRunwayExitStrategyItem::ImportSaveData(CString strFltType)
{

	if (m_nID != -1)
	{
		return UpdateData();
	}
	char szFltType[1024] = {0};
	strcpy_s(szFltType,strFltType);
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_RUNWAY_EXITSTRATEGY\
					 (PROJID, FLTTYPE)\
					 VALUES (%d,'%s')"),m_nProjectID,szFltType);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	for (RunwayExitStandStrategyItemIter iter = m_vectRunwayExitStandStrategyItems.begin(); 
		iter != m_vectRunwayExitStandStrategyItems.end(); iter++)
	{
		if(-1 == (*iter)->getID())
		{
			(*iter)->setExitStrategyID(	m_nID);
			(*iter)->SaveData();
		}
		else
			(*iter)->UpdateData();
	}

	DeleteDataFromDatabase();
}
void FlightTypeRunwayExitStrategyItem::ImportUpdateData(CString strFltType)
{
	char szFltType[1024] = {0};
	strcpy_s(szFltType,strFltType);
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_RUNWAY_EXITSTRATEGY\
					 SET FLTTYPE ='%s'\
					 WHERE (ID = %d)"),szFltType,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	for (RunwayExitStandStrategyItemIter iter = m_vectRunwayExitStandStrategyItems.begin(); 
		iter != m_vectRunwayExitStandStrategyItems.end(); iter++)
	{
		if(-1 == (*iter)->getID())
		{
			(*iter)->setExitStrategyID(m_nID);
			(*iter)->SaveData();
		}
		else
			(*iter)->UpdateData();
	}

	DeleteDataFromDatabase();
}
void FlightTypeRunwayExitStrategyItem::UpdateData()
{
	CString strSQL = _T("");
	char szFltType[1024] = {0};
	m_fltType.WriteSyntaxStringWithVersion(szFltType);
	strSQL.Format(_T("UPDATE IN_RUNWAY_EXITSTRATEGY\
		SET FLTTYPE ='%s'\
		WHERE (ID = %d)"),szFltType,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	for (RunwayExitStandStrategyItemIter iter = m_vectRunwayExitStandStrategyItems.begin(); 
		iter != m_vectRunwayExitStandStrategyItems.end(); iter++)
	{
		if(-1 == (*iter)->getID())
		{
			(*iter)->setExitStrategyID(m_nID);
			(*iter)->SaveData();
		}
		else
			(*iter)->UpdateData();
	}

	DeleteDataFromDatabase();

}

void FlightTypeRunwayExitStrategyItem::DeleteData()
{
	for (RunwayExitStandStrategyItemIter iter = m_vectRunwayExitStandStrategyItems.begin();
		iter != m_vectRunwayExitStandStrategyItems.end(); iter++)
	{
		if( (*iter)->getID() != -1)
		{
			(*iter)->DeleteData();
		}
		delete (*iter);
	}
	m_vectRunwayExitStandStrategyItems.clear();

	DeleteDataFromDatabase();

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_RUNWAY_EXITSTRATEGY\
		WHERE (ID = %d)"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);

}

RunwayExitStandStrategyItem* FlightTypeRunwayExitStrategyItem::GetRunwayExitStandStrategyItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex<(int)m_vectRunwayExitStandStrategyItems.size());
	return m_vectRunwayExitStandStrategyItems[nIndex];
}

void FlightTypeRunwayExitStrategyItem::AddRunwayExitStandStrategyItem(RunwayExitStandStrategyItem* pItem)
{
	m_vectRunwayExitStandStrategyItems.push_back(pItem);
}

void FlightTypeRunwayExitStrategyItem::DeleteRunwayExitStandStrategyItem(RunwayExitStandStrategyItem* pItem)
{
	RunwayExitStandStrategyItemIter iter = 
		std::find(m_vectRunwayExitStandStrategyItems.begin(), m_vectRunwayExitStandStrategyItems.end(), pItem);
	if (iter == m_vectRunwayExitStandStrategyItems.end())
		return;
	m_vectNeedDelStrategyItems.push_back(*iter);
	m_vectRunwayExitStandStrategyItems.erase(iter);
}


void FlightTypeRunwayExitStrategyItem::DeleteDataFromDatabase()
{
	for (RunwayExitStandStrategyItemIter iter = m_vectNeedDelStrategyItems.begin(); iter != m_vectNeedDelStrategyItems.end(); iter++)
	{
		if( (*iter)->getID() != -1)
		{
			(*iter)->DeleteData();
		}
		delete (*iter);
	}
	m_vectNeedDelStrategyItems.clear();
}

void FlightTypeRunwayExitStrategyItem::RemoveAll()
{
	for (RunwayExitStandStrategyItemIter iter = m_vectRunwayExitStandStrategyItems.begin();
		iter != m_vectRunwayExitStandStrategyItems.end(); iter++) 
	{
		delete(*iter);
	}
	m_vectRunwayExitStandStrategyItems.clear();

	for (RunwayExitStandStrategyItemIter iter = m_vectNeedDelStrategyItems.begin();
		iter != m_vectNeedDelStrategyItems.end(); iter++) 
	{
		delete(*iter);
	}
	m_vectNeedDelStrategyItems.clear();
}

//void FlightTypeRunwayExitStrategyItem::setFlightType(const CString& strFltType)
//{
//	 m_strFltType = strFltType;
//
//	 if (m_pAirportDatabase)
//	 {
//		 m_FlightType.SetAirportDB(m_pAirportDatabase);
//		 m_FlightType.setConstraint(m_strFltType,VERSION_CONSTRAINT_CURRENT);
//	 }
//}

void FlightTypeRunwayExitStrategyItem::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);
	CString strFltType;
	m_fltType.screenPrint(strFltType);
	exportFile.getFile().writeField(strFltType);
	exportFile.getFile().writeLine();

	exportFile.getFile().writeInt(static_cast<int>(m_vectRunwayExitStandStrategyItems.size()));
	RunwayExitStandStrategyItemIter iter = m_vectRunwayExitStandStrategyItems.begin();
	for (; iter!=m_vectRunwayExitStandStrategyItems.end(); iter++)
	{
		(*iter)->ExportData(exportFile);
	}
}

void FlightTypeRunwayExitStrategyItem::ImportData(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	CString strFltType;
	importFile.getFile().getField(strFltType.GetBuffer(1024),1024);

	m_nProjectID = importFile.getNewProjectID();
	ImportSaveData(strFltType);

	importFile.getFile().getLine();

	int nTimeRangeStrategyItemCount = 0;
	importFile.getFile().getInteger(nTimeRangeStrategyItemCount);
	for (int i =0; i < nTimeRangeStrategyItemCount; ++i)
	{
		RunwayExitStandStrategyItem data(m_nProjectID);
		data.ImportData(importFile,m_nID);
	}
}

BOOL FlightTypeRunwayExitStrategyItem::CheckExitPercent() const
{
	for (RunwayExitStandStrategyItemIter_const iter = m_vectRunwayExitStandStrategyItems.begin();
		iter!=m_vectRunwayExitStandStrategyItems.end();iter++)
	{
		if (FALSE == (*iter)->CheckExitPercent())
		{
			return FALSE;
		}
	}
	return TRUE;
}