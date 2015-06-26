#include "StdAfx.h"
#include "RunwayExitStandStrategyItem.h"
#include "TimeRangeRunwayExitStrategyItem.h"
#include "ALTObjectGroup.h"
#include "Stand.h"

#include "../Database/ADODatabase.h"
#include "Common/FLT_CNST.H"

#include "AirsideImportExportManager.h"

#include <algorithm>

RunwayExitStandStrategyItem::RunwayExitStandStrategyItem( int nExitStrategyID )
	: m_nID(-1)
	, m_nExitStrategyID(nExitStrategyID)
	, m_nStandFamilyID(-1)
{
	InitStandName();
}

RunwayExitStandStrategyItem::~RunwayExitStandStrategyItem( void )
{
	RemoveAll();
}

void RunwayExitStandStrategyItem::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, TYPE, STARTTIME, ENDTIME, MANAGED_EXIT_MODE\
					 FROM IN_RUNWAY_EXIT_TIME2\
					 WHERE (RWSTANDSTRAID = %d)"),m_nID);

	CADORecordset adoRecordset;
	long nRecordAfflect = 0L;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordAfflect,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		TimeRangeRunwayExitStrategyItem *pItem = new TimeRangeRunwayExitStrategyItem(m_nID);

		int nID = -1;
		adoRecordset.GetFieldValue(_T("ID"),nID);
		pItem->setID(nID);

		int nType = -1;
		adoRecordset.GetFieldValue(_T("TYPE"),nType);
		pItem->setStrategyType((RunwayExitStrategyType)nType);

		long lStartTime = 0;
		adoRecordset.GetFieldValue(_T("STARTTIME"),lStartTime);
		pItem->setStartTime(lStartTime);

		long lEndTime = 0;
		adoRecordset.GetFieldValue(_T("ENDTIME"),lEndTime);
		pItem->setEndTime(lEndTime);

		int nMode = TimeRangeRunwayExitStrategyItem::ProbabilityBased;
		adoRecordset.GetFieldValue(_T("MANAGED_EXIT_MODE"),nMode);
		pItem->setStrategyManagedExitMode((TimeRangeRunwayExitStrategyItem::StrategyMode)nMode);

		if(nType = (int)ManagedExit)
			pItem->ReadData();

		m_vectTimeRangeRunwayExitStrategy.push_back(pItem);

		adoRecordset.MoveNextData();
	}
}

void RunwayExitStandStrategyItem::SaveData()
{
	if (m_nID != -1)
	{
		return UpdateData();
	}
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_RUNWAY_STAND_STRATEGY\
					 (RWEXITSTRAID, STANDID) VALUES (%d,%d)")
					 , m_nExitStrategyID
					 , m_nStandFamilyID);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	for (TimeRangeStrategyItemIter iter = m_vectTimeRangeRunwayExitStrategy.begin(); 
		iter != m_vectTimeRangeRunwayExitStrategy.end(); iter++)
	{
		if(-1 == (*iter)->getID())
		{
			(*iter)->SetStandStrategyID(m_nID);
			(*iter)->SaveData();
		}
		else
			(*iter)->UpdateData();
	}

	DeleteDataFromDatabase();
}

void RunwayExitStandStrategyItem::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_RUNWAY_STAND_STRATEGY SET STANDID =%d WHERE (ID = %d)")
		, m_nStandFamilyID
		, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	for (TimeRangeStrategyItemIter iter = m_vectTimeRangeRunwayExitStrategy.begin(); 
		iter != m_vectTimeRangeRunwayExitStrategy.end(); iter++)
	{
		if(-1 == (*iter)->getID())
		{
			(*iter)->SetStandStrategyID(m_nID);
			(*iter)->SaveData();
		}
		else
			(*iter)->UpdateData();
	}

	DeleteDataFromDatabase();
}

void RunwayExitStandStrategyItem::DeleteData()
{
	for (TimeRangeStrategyItemIter iter = m_vectTimeRangeRunwayExitStrategy.begin(); 
		iter != m_vectTimeRangeRunwayExitStrategy.end(); iter++)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vectTimeRangeRunwayExitStrategy.clear();

	DeleteDataFromDatabase();

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_RUNWAY_STAND_STRATEGY\
					 WHERE (ID = %d)"), m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);

}

void RunwayExitStandStrategyItem::ExportData( CAirsideExportFile& exportFile )
{
	exportFile.getFile().writeInt(m_nID);
// 	exportFile.getFile().writeInt(m_nExitStrategyID);
	exportFile.getFile().writeInt(m_nStandFamilyID);
	exportFile.getFile().writeLine();

	exportFile.getFile().writeInt(static_cast<int>(m_vectTimeRangeRunwayExitStrategy.size()));
	TimeRangeStrategyItemIter iter = m_vectTimeRangeRunwayExitStrategy.begin();
	for (; iter!=m_vectTimeRangeRunwayExitStrategy.end(); iter++)
	{
		(*iter)->ExportData(exportFile);
	}
}

void RunwayExitStandStrategyItem::ImportData( CAirsideImportFile& importFile, int nExitStrategyID )
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

// 	importFile.getFile().getInteger(m_nExitStrategyID);
	m_nExitStrategyID = nExitStrategyID;
	importFile.getFile().getInteger(m_nStandFamilyID);
	importFile.getFile().getLine();

	SaveData();

	int nTimeRangeStrategyItemCount = 0;
	importFile.getFile().getInteger(nTimeRangeStrategyItemCount);
	for (int i =0; i < nTimeRangeStrategyItemCount; ++i)
	{
		TimeRangeRunwayExitStrategyItem data(m_nID);
		data.ImportData(importFile,m_nID);
	}
}

void RunwayExitStandStrategyItem::DeleteTimeRangeRWAssignStrategyItem( TimeRangeRunwayExitStrategyItem* pItem )
{
	TimeRangeStrategyItemIter iter = 
		std::find(m_vectTimeRangeRunwayExitStrategy.begin(), m_vectTimeRangeRunwayExitStrategy.end(), pItem);
	if (iter == m_vectTimeRangeRunwayExitStrategy.end())
		return;

	m_vectNeedDelStrategy.push_back(*iter);
	m_vectTimeRangeRunwayExitStrategy.erase(iter);
}

void RunwayExitStandStrategyItem::RemoveAll()
{
	for (TimeRangeStrategyItemIter iter = m_vectTimeRangeRunwayExitStrategy.begin();
		iter != m_vectTimeRangeRunwayExitStrategy.end(); iter++) 
	{
		delete(*iter);
	}
	m_vectTimeRangeRunwayExitStrategy.clear();

	for (TimeRangeStrategyItemIter iter = m_vectNeedDelStrategy.begin();
		iter != m_vectNeedDelStrategy.end(); iter++) 
	{
		delete(*iter);
	}
	m_vectNeedDelStrategy.clear();
}

void RunwayExitStandStrategyItem::DeleteDataFromDatabase()
{
	for (TimeRangeStrategyItemIter iter = m_vectNeedDelStrategy.begin(); iter != m_vectNeedDelStrategy.end(); iter++)
	{
		if( (*iter)->getID() != -1)
		{
			(*iter)->RemoveAll();
			(*iter)->DeleteData();
		}
		delete (*iter);
		(*iter) = NULL;
	}
}

void RunwayExitStandStrategyItem::InitStandName()
{
	if (-1 == m_nStandFamilyID)
	{
		m_strStandFamilyName = _T("All");
		return;
	}
	ALTObjectGroup altObjGroup;
	altObjGroup.ReadData(m_nStandFamilyID);
	m_strStandFamilyName = altObjGroup.getName().GetIDString();
}

BOOL RunwayExitStandStrategyItem::CheckExitPercent() const
{
	for (TimeRangeStrategyItemIter_const iter = m_vectTimeRangeRunwayExitStrategy.begin();
		iter!=m_vectTimeRangeRunwayExitStrategy.end();iter++)
	{
		if (FALSE == (*iter)->checkExitPercent())
		{
			return FALSE;
		}
	}
	return TRUE;
}

void RunwayExitStandStrategyItem::setStandFamilyID( int val )
{
	m_nStandFamilyID = val; InitStandName();
}

CString RunwayExitStandStrategyItem::getStandFamilyName() const
{
	return m_strStandFamilyName;
}

TimeRangeRunwayExitStrategyItem* RunwayExitStandStrategyItem::GetTimeRangeStrategyItem( int nIndex )
{
	return m_vectTimeRangeRunwayExitStrategy[nIndex];
}

int RunwayExitStandStrategyItem::GetTimeRangeStrategyItemCount()
{
	return (int)m_vectTimeRangeRunwayExitStrategy.size();
}

void RunwayExitStandStrategyItem::AddTimeRangeRWAssignStrategyItem( TimeRangeRunwayExitStrategyItem* pItem )
{
	m_vectTimeRangeRunwayExitStrategy.push_back(pItem);
}