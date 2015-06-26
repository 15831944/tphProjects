#include "stdafx.h"
#include "TimeRangeRunwayAssignmentStrategyItem.h"
#include "../Database/ADODatabase.h"
#include "inputAirside/AirsideImportExportManager.h"
//
//////////////////////////////////////////////////////////////////////////

//RunwayAssignmentPercentItem

void RunwayAssignmentPercentItem::SaveData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_RUNWAY_ASGNDATA\
		(RWASNTIMEID, RUNWAYID, MARKINDEX, FPERCENT)\
		VALUES (%d,%d,%d,%f)"),m_nTimeRangeItemID,
		m_nRunwayID,m_nRunwayMarkIndex,m_dPercent);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);


}
void RunwayAssignmentPercentItem::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_RUNWAY_ASGNDATA\
		SET RUNWAYID =%d, MARKINDEX =%d, FPERCENT =%f\
		WHERE (ID = %d)"),
		m_nRunwayID,m_nRunwayMarkIndex,m_dPercent,m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}
void RunwayAssignmentPercentItem::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_RUNWAY_ASGNDATA\
		WHERE (ID = %d)"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void RunwayAssignmentPercentItem::ExportRunwayAssignmentPercent(CAirsideExportFile& exportFile)
{
//		m_nID(-1)
//		m_dPercent(0.0)
//		m_nRunwayMarkIndex(0)
//		m_nRunwayID(-1)
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeDouble(m_dPercent);
	exportFile.getFile().writeInt(m_nRunwayID);
	exportFile.getFile().writeInt(m_nRunwayMarkIndex);
	exportFile.getFile().writeLine();


}
void RunwayAssignmentPercentItem::ImportRunwayAssignmentPercent(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);
	importFile.getFile().getFloat(m_dPercent);
	int nOldRunwayID = -1;
	importFile.getFile().getInteger(nOldRunwayID);
	m_nRunwayID = importFile.GetObjectNewID(nOldRunwayID);
	importFile.getFile().getInteger(m_nRunwayMarkIndex);
	importFile.getFile().getLine();

	SaveData();
}
//////////////////////////////////////////////////////////////////////////
//TimeRangeRunwayAssignmentStrategyItem
TimeRangeRunwayAssignmentStrategyItem::TimeRangeRunwayAssignmentStrategyItem(int nFltTypeItemID)
:m_nFltTypeItemID(nFltTypeItemID)
,m_nID(-1)
,m_emStrategyType(FlightPlannedRunway)
{
}

TimeRangeRunwayAssignmentStrategyItem::~TimeRangeRunwayAssignmentStrategyItem(void)
{
	RemoveAll();
}

void TimeRangeRunwayAssignmentStrategyItem::ReadData()
{
	if(m_emStrategyType != ManagedAssignment)
		return;

	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, RUNWAYID, MARKINDEX,FPERCENT\
		FROM IN_RUNWAY_ASGNDATA\
		WHERE (RWASNTIMEID = %d)"),m_nID);

	long lItemAfflect = 0L;
	CADORecordset  adoRecordset;
	
	CADODatabase::ExecuteSQLStatement(strSQL,lItemAfflect,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		RunwayAssignmentPercentItem *pItem = new RunwayAssignmentPercentItem(m_nID);
		adoRecordset.GetFieldValue(_T("ID"),pItem->m_nID);
		
		adoRecordset.GetFieldValue(_T("RUNWAYID"), pItem->m_nRunwayID);

		adoRecordset.GetFieldValue(_T("MARKINDEX"),pItem->m_nRunwayMarkIndex);

		adoRecordset.GetFieldValue(_T("FPERCENT"),pItem->m_dPercent);

		m_vectManagedAssignmentRWAssignPercent.push_back(pItem);

		adoRecordset.MoveNextData();
	}

}


void TimeRangeRunwayAssignmentStrategyItem::RefreshRunwayAssignPercentItems(RunwayAssignPercentVector& vec, int nID)
{
	for (RunwayAssignPercentIter iter = vec.begin();
		iter != vec.end(); iter++)
	{
		if ( -1 == (*iter)->getID()) 
		{
			(*iter)->SetTimeRangeID(nID);
			(*iter)->SaveData();
		}
		else
			(*iter)->UpdateData();
	}
}

void TimeRangeRunwayAssignmentStrategyItem::SaveData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_RUNWAY_ASGN_TIME\
		(RWLANDASNID, TYPE, STARTTIME, ENDTIME)\
		VALUES (%d,%d,%d,%d)"),m_nFltTypeItemID,(int)m_emStrategyType,
		m_dtTimeRange.GetStartTime().asSeconds(),
		m_dtTimeRange.GetEndTime().asSeconds());

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	RefreshRunwayAssignPercentItems(m_vectManagedAssignmentRWAssignPercent, m_nID);

	DeleteDataFromDatabase();
}

void TimeRangeRunwayAssignmentStrategyItem::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_RUNWAY_ASGN_TIME\
		SET TYPE=%d, STARTTIME =%d, ENDTIME =%d\
		WHERE (ID = %d)"),
		(int)m_emStrategyType,
		m_dtTimeRange.GetStartTime().asSeconds(),
		m_dtTimeRange.GetEndTime().asSeconds(),
		m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
	RefreshRunwayAssignPercentItems(m_vectManagedAssignmentRWAssignPercent, m_nID);

	DeleteDataFromDatabase();

}

void TimeRangeRunwayAssignmentStrategyItem::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_RUNWAY_ASGN_TIME\
		WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	DeleteDataFromDatabase();
}




void TimeRangeRunwayAssignmentStrategyItem::AddRunwayAssignPercentItem(RunwayAssignmentPercentItem* pItem)
{
	if (m_emStrategyType == ManagedAssignment)
		m_vectManagedAssignmentRWAssignPercent.push_back(pItem);
}

void TimeRangeRunwayAssignmentStrategyItem::DeleteRunwayAssignPercentItem(RunwayAssignmentPercentItem* pItem)
{
	if (m_emStrategyType == ManagedAssignment)
	{
		RunwayAssignPercentIter iter = std::find(m_vectManagedAssignmentRWAssignPercent.begin(), m_vectManagedAssignmentRWAssignPercent.end(),pItem);
		m_vectManagedAssignmentRWAssignPercent.erase(iter);
		if( (*iter)->getID() != -1)
			m_vectNeedDelStrategy.push_back(*iter);
	}
	
}

void TimeRangeRunwayAssignmentStrategyItem::UpdateRunwayAssignPercentItem(RunwayAssignmentPercentItem* pItem)
{

}

void TimeRangeRunwayAssignmentStrategyItem::UpdateRunwayAssignPercentItemType(RunwayAssignmentStrategyType rwType)
{
	if (m_emStrategyType == ManagedAssignment && rwType != ManagedAssignment) 
	{
		for (RunwayAssignPercentIter iter = m_vectNeedDelStrategy.begin(); iter != m_vectNeedDelStrategy.end(); iter++)
		{
			m_vectNeedDelStrategy.push_back(*iter);
		}
	}
	m_vectManagedAssignmentRWAssignPercent.clear();
	m_emStrategyType = rwType;

}

void TimeRangeRunwayAssignmentStrategyItem::DeleteDataFromDatabase()
{
	for (RunwayAssignPercentIter iter = m_vectNeedDelStrategy.begin(); iter != m_vectNeedDelStrategy.end(); iter++)
	{
		if( (*iter)->getID() != -1)
			(*iter)->DeleteData();
		delete (*iter);
		(*iter) = NULL;
	}
}

void TimeRangeRunwayAssignmentStrategyItem::RemoveAll()
{

	for (RunwayAssignPercentIter iter = m_vectManagedAssignmentRWAssignPercent.begin();
		iter != m_vectManagedAssignmentRWAssignPercent.end(); iter++)
	{
		delete(*iter);
		(*iter) = NULL;
	}
	m_vectManagedAssignmentRWAssignPercent.clear();
}
void TimeRangeRunwayAssignmentStrategyItem::ExportTimeRangeRunwayAssignment(CAirsideExportFile& exportFile)
{
	//int m_nID;
	//int m_nFltTypeItemID;

	//RunwayAssignmentStrategyType m_emStrategyType;
	//TimeRange m_dtTimeRange;
	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(m_nFltTypeItemID);
	exportFile.getFile().writeInt((int)m_emStrategyType);
	exportFile.getFile().writeTime(m_dtTimeRange.GetStartTime(),TRUE);
	exportFile.getFile().writeTime(m_dtTimeRange.GetEndTime(),TRUE);
	
	int nAsnCount = static_cast<int>(m_vectManagedAssignmentRWAssignPercent.size());
	exportFile.getFile().writeInt( nAsnCount);

	exportFile.getFile().writeLine();

	for (RunwayAssignPercentIter iter = m_vectManagedAssignmentRWAssignPercent.begin();
		iter != m_vectManagedAssignmentRWAssignPercent.end(); iter++)
	{
		(*iter)->ExportRunwayAssignmentPercent(exportFile);		
	}

}
void TimeRangeRunwayAssignmentStrategyItem::ImportTimeRangeRunwayAssignment(CAirsideImportFile& importFile)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	int nOldFltTypeID = -1;
	importFile.getFile().getInteger(nOldFltTypeID);

	int nStrategyType = 0;
	importFile.getFile().getInteger(nStrategyType);

	m_emStrategyType = (RunwayAssignmentStrategyType)nStrategyType;
	
	ElapsedTime startTime;
	importFile.getFile().getTime(startTime,TRUE);
	m_dtTimeRange.SetStartTime(startTime);

	ElapsedTime endTime;
	importFile.getFile().getTime(endTime,TRUE);
	m_dtTimeRange.SetEndTime(endTime);
	

	int nAsnCount = 0;
	importFile.getFile().getInteger(nAsnCount);

	importFile.getFile().getLine();
	SaveData();

	for (int i =0 ; i< nAsnCount; ++i)
	{
		RunwayAssignmentPercentItem asnItem(m_nID);
		asnItem.ImportRunwayAssignmentPercent(importFile);
	}

	

}

