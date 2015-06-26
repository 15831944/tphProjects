#include "stdafx.h"
#include "TimeRangeRunwayExitStrategyItem.h"
#include "../Database/ADODatabase.h"
#include "AirsideImportExportManager.h"

#include <limits>

//////////////////////////////////////////////////////////////////////////
//
RunwayExitPercentItem::RunwayExitPercentItem(int nTimeRangeItemID)
:m_nID(-1)
,m_fPercent(0.0)
,m_nRunwayID(-1)
,m_nExitID(-1)
,m_nRunwayMarkIndex(-1)
,m_nTimeRangeItemID(nTimeRangeItemID)
,m_bBacktrackOp(false)
{
}

void RunwayExitPercentItem::SaveData()
{
	CString strSQL = _T("");

	int nBacktrack = 0;
	if (m_bBacktrackOp)
		nBacktrack = 1;

	strSQL.Format(_T("INSERT INTO IN_RUNWAY_EXITDATA2\
		(RWEXITTIMEID, RUNWAYID, MARKINDEX, EXITID, FPERCENT, BACKTRACK)\
		VALUES (%d,%d,%d,%d,%f, %d)"),m_nTimeRangeItemID,
		m_nRunwayID,m_nRunwayMarkIndex,m_nExitID,m_fPercent,nBacktrack );

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}
void RunwayExitPercentItem::UpdateData()
{
	int nBacktrack = 0;
	if (m_bBacktrackOp)
		nBacktrack = 1;

	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_RUNWAY_EXITDATA2\
					 SET RUNWAYID =%d, MARKINDEX = %d,EXITID =%d, FPERCENT =%f, BACKTRACK =%d\
					 WHERE (ID = %d)"),
					 m_nRunwayID,m_nRunwayMarkIndex,m_nExitID,m_fPercent,nBacktrack, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

}
void RunwayExitPercentItem::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_RUNWAY_EXITDATA2\
					 WHERE (ID = %d)"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void RunwayExitPercentItem::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);	
	exportFile.getFile().writeInt(m_nRunwayID);
	exportFile.getFile().writeInt(m_nRunwayMarkIndex);
	exportFile.getFile().writeInt(m_nExitID);
	exportFile.getFile().writeFloat(m_fPercent);
	exportFile.getFile().writeLine();
}

void RunwayExitPercentItem::ImportData(CAirsideImportFile& importFile,int nTimeRangeItemID)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	int nOldRunwayID = -1;
	importFile.getFile().getInteger(nOldRunwayID);
	m_nRunwayID = importFile.GetObjectNewID(nOldRunwayID);
	//importFile.getFile().getInteger(m_nRunwayID);

	importFile.getFile().getInteger(m_nRunwayMarkIndex);

	int nOldExitID = -1;
	importFile.getFile().getInteger(nOldExitID);
	m_nExitID = importFile.GetRunwayExitIndexMap(nOldExitID);
	//importFile.getFile().getInteger(m_nExitID);

	importFile.getFile().getFloat(m_fPercent);

	m_nTimeRangeItemID = nTimeRangeItemID;
	SaveData();

	importFile.getFile().getLine();
}
//////////////////////////////////////////////////////////////////////////

TimeRangeRunwayExitStrategyItem::TimeRangeRunwayExitStrategyItem(int nStandStrategyID)
	: m_nStandStrategyID(nStandStrategyID)
	, m_nID(-1)
	, m_dtTimeRange(ElapsedTime(0L), ElapsedTime((24*60*60L - 1L)))
	, m_emStrategyType(MaxBrakingFirstOnEitherSide)
	, m_strategyManagedExitMode(ProbabilityBased)
{
}

TimeRangeRunwayExitStrategyItem::~TimeRangeRunwayExitStrategyItem(void)
{
	RemoveAll();
}


void TimeRangeRunwayExitStrategyItem::AddToProbabilityStructure(RunwayExitPercentItem* pItem)
{
	RunwayExitStrategyPercentItem* pPriorities = GetRunwayExitPercentItem(pItem->m_nRunwayID,pItem->m_nRunwayMarkIndex);
	pPriorities->AddItem(pItem);
}

void TimeRangeRunwayExitStrategyItem::AddToPrioritiesStructure(RunwayExitPriorityItem* pItem)
{
	RunwayExitStrategyPriorityItem* pPriorities = GetRunwayExitPriorityItem(pItem->m_nRunwayID,pItem->m_nRunwayMarkIndex);
	pPriorities->AddItem(pItem);
}

void TimeRangeRunwayExitStrategyItem::ReadData()
{
	if(m_emStrategyType != ManagedExit)
		return;

	if (ProbabilityBased == m_strategyManagedExitMode)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("SELECT ID, RUNWAYID,MARKINDEX, EXITID, FPERCENT, BACKTRACK\
			FROM IN_RUNWAY_EXITDATA2\
			WHERE (RWEXITTIMEID = %d)"),m_nID);
		CADORecordset adoRecordset;
		long nRecordAfflect = 0L;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordAfflect,adoRecordset);
	
		while (!adoRecordset.IsEOF())
		{
			RunwayExitPercentItem *pItem = new RunwayExitPercentItem(m_nID);
			adoRecordset.GetFieldValue(_T("ID"),pItem->m_nID);
	
			adoRecordset.GetFieldValue(_T("RUNWAYID"),pItem->m_nRunwayID);
			adoRecordset.GetFieldValue(_T("MARKINDEX"),pItem->m_nRunwayMarkIndex);
			adoRecordset.GetFieldValue(_T("EXITID"),pItem->m_nExitID);
			double dPercent;
			adoRecordset.GetFieldValue(_T("FPERCENT"),dPercent);
			pItem->m_fPercent = (float)dPercent;
			int nBackground;
			adoRecordset.GetFieldValue(_T("BACKTRACK"), nBackground);
			if (nBackground > 0)
			{
				pItem->SetBacktrack(true);
			}
			else
			{
				pItem->SetBacktrack(false);
			}
			
			AddToProbabilityStructure(pItem);
	
			adoRecordset.MoveNextData();
		}
	}
	else if (PriorityBased == m_strategyManagedExitMode)
	{
		CString strSQL = _T("");
		strSQL.Format(_T("SELECT ID, RWEXITTIMEID, RUNWAYID, MARKINDEX, EXITID, BACKTRACK, PRIORITY\
						 FROM IN_RUNWAY_EXIT_PRIORITYDATA\
						 WHERE (RWEXITTIMEID = %d)"), m_nID);
		CADORecordset adoRecordset;
		long nRecordAfflect = 0L;
		CADODatabase::ExecuteSQLStatement(strSQL,nRecordAfflect,adoRecordset);

		while (!adoRecordset.IsEOF())
		{
			RunwayExitPriorityItem* pItem = new RunwayExitPriorityItem(m_nID);
			adoRecordset.GetFieldValue(_T("ID"),pItem->m_nID);

			adoRecordset.GetFieldValue(_T("RUNWAYID"),pItem->m_nRunwayID);
			adoRecordset.GetFieldValue(_T("MARKINDEX"),pItem->m_nRunwayMarkIndex);
			adoRecordset.GetFieldValue(_T("EXITID"),pItem->m_nExitID);
			adoRecordset.GetFieldValue(_T("PRIORITY"),pItem->m_nPriorityIndex);
			int nTmp = 0;
			adoRecordset.GetFieldValue(_T("BACKTRACK"),nTmp);
			pItem->m_bBacktrack = (nTmp != 0);

			pItem->GetConditionData().ReadData(pItem->m_nID);
			AddToPrioritiesStructure(pItem);

			adoRecordset.MoveNextData();
		}

		SortPriorities();
	}
	else
	{
		ASSERT(FALSE);
	}

}

void TimeRangeRunwayExitStrategyItem::SaveData()
{
	if (m_nID != -1)
	{
		return UpdateData();
	}
	CString strSQL = _T("");
	strSQL.Format(_T("INSERT INTO IN_RUNWAY_EXIT_TIME2\
		(RWSTANDSTRAID, TYPE, STARTTIME, ENDTIME, MANAGED_EXIT_MODE)\
		VALUES (%d,%d,%d,%d,%d)")
		, m_nStandStrategyID
		, (int)m_emStrategyType
		, m_dtTimeRange.GetStartTime().asSeconds()
		, m_dtTimeRange.GetEndTime().asSeconds()
		, (int)m_strategyManagedExitMode);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	SaveVectorData(m_nID);
}

void TimeRangeRunwayExitStrategyItem::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_RUNWAY_EXIT_TIME2\
		SET TYPE=%d, STARTTIME =%d, ENDTIME =%d, MANAGED_EXIT_MODE = %d\
		WHERE (ID = %d)")
		, (int)m_emStrategyType
		, m_dtTimeRange.GetStartTime().asSeconds()
		, m_dtTimeRange.GetEndTime().asSeconds()
		, (int)m_strategyManagedExitMode
		, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
	SaveVectorData(m_nID);

//	DeleteDataFromDatabase();
}
void TimeRangeRunwayExitStrategyItem::DeleteData()
{
	DeleteDataFromDatabase();

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_RUNWAY_EXIT_TIME2\
		WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void TimeRangeRunwayExitStrategyItem::AddRunwayExitPercentItem(RunwayExitPercentItem* pItem)
{
	if(m_emStrategyType == ManagedExit)
		//m_vectManagedExitRWExitPercent.push_back(pItem);
		AddToProbabilityStructure(pItem);
}

void TimeRangeRunwayExitStrategyItem::DeleteRunwayExitPercentItem(int nRunwayID,int nRunwayMark)
{
	//if (m_emStrategyType == ManagedAssignment)
	//{
//		RunwayExitPercentIter iter = std::find(m_vectManagedExitRWExitPercent.begin(), m_vectManagedExitRWExitPercent.end(),pItem);
//
//		m_vectManagedExitRWExitPercent.erase(iter);
//		m_vectManagedExitRWExitPercent.push_back(*iter);

		RunwayExitStrategyPercentItem *pRunwayExitItem = GetRunwayExitPercentItem(nRunwayID,nRunwayMark);
		//pRunwayExitItem->DeleteData();
		//delete
		DeleteRunwayMarkExitItem(pRunwayExitItem);


		//RunwayExitPercentVectorIter iter = m_mapRunwayExitVector.begin();
		//for(;iter != m_mapRunwayExitVector.end(); iter++)
		//{
		//	if(iter->first == nRunwayID)
		//	{
		//		RunwayExitPercentVector& vec = iter->second;

		//		for (RunwayExitPercentIter it=vec.begin(); it!=vec.end(); it++)
		//		{
		//			if( (*it)->getID() != -1)
		//				(*it)->DeleteData();
		//			delete(*it);
		//			(*it) = NULL;
		//		}
		//		break;
		//	}
		//}
		//if(iter != m_mapRunwayExitVector.end())
		//	m_mapRunwayExitVector.erase(iter);
	//}
}

void TimeRangeRunwayExitStrategyItem::AddRunwayExitPriority(RunwayExitPriorityItem* pItem)
{
	ASSERT(pItem);
	if(m_emStrategyType == ManagedExit)
	{
		AddToPrioritiesStructure(pItem);
	}
}

void TimeRangeRunwayExitStrategyItem::DeleteRunwayExitPriority(int nRunwayID,int nRunwayMark)
{
	RunwayExitStrategyPriorityItem *pItem = GetRunwayExitPriorityItem(nRunwayID,nRunwayMark);
	if (pItem)
	{
		pItem->DeleteData();
	}
}

BOOL TimeRangeRunwayExitStrategyItem::checkExitPercent()
{
	if (ProbabilityBased == m_strategyManagedExitMode)
	{
		RunwayMarkExitStrategyIter iter = m_vRunwayMarkItems.begin();
		for (;iter != m_vRunwayMarkItems.end(); ++iter)
		{
			if((*iter)->checkExitPercent() == false)
				return false;
		}
	}
   return TRUE ;
}


void TimeRangeRunwayExitStrategyItem::DeleteDataFromDatabase()
{
	RunwayMarkExitStrategyIter iter = m_vRunwayMarkItems.begin();
	for (;iter != m_vRunwayMarkItems.end(); ++iter)
	{
		(*iter)->DeleteData();
		delete *iter;
	}
	m_vRunwayMarkItems.clear();

	RunwayExitStrategyPrioritiesIter iter2 = m_vPriorityItems.begin();
	for (;iter2 != m_vPriorityItems.end(); ++iter2)
	{
		(*iter2)->DeleteData();
		delete *iter2;
	}
	m_vPriorityItems.clear();
}

void TimeRangeRunwayExitStrategyItem::RemoveAll()
{
	RunwayMarkExitStrategyIter iter = m_vRunwayMarkItems.begin();
	for (;iter != m_vRunwayMarkItems.end(); ++iter)
	{
		(*iter)->clear();
		delete *iter;
	}
	m_vRunwayMarkItems.clear();

	RunwayExitStrategyPrioritiesIter iter2 = m_vPriorityItems.begin();
	for (;iter2 != m_vPriorityItems.end(); ++iter2)
	{
		(*iter2)->clear();
		delete *iter2;
	}
	m_vPriorityItems.clear();

}

void TimeRangeRunwayExitStrategyItem::RefreshRunwayExitPercentItems( int nTimeRangeID )
{
	RunwayMarkExitStrategyIter iter = m_vRunwayMarkItems.begin();
	for (;iter != m_vRunwayMarkItems.end(); ++iter)
	{
		(*iter)->RefreshRunwayExitPercentItems(nTimeRangeID);
	}
}

void TimeRangeRunwayExitStrategyItem::RefreshRunwayExitAllPriorities( int nTimeRangeID )
{
	RunwayExitStrategyPrioritiesIter iter = m_vPriorityItems.begin();
	for (;iter != m_vPriorityItems.end(); ++iter)
	{
		(*iter)->RefreshRunwayExitPriorities(nTimeRangeID);
	}

}


void TimeRangeRunwayExitStrategyItem::ExportData(CAirsideExportFile& exportFile)
{
	//exportFile.getFile().writeInt(m_nID);
	//exportFile.getFile().writeInt((int)m_emStrategyType);
	//exportFile.getFile().writeInt(m_dtTimeRange.GetStartTime().asSeconds());
	//exportFile.getFile().writeInt(m_dtTimeRange.GetEndTime().asSeconds());
	//exportFile.getFile().writeLine();

	//int i = 0;
	//for(RunwayExitPercentVectorIter iter = m_mapRunwayExitVector.begin();
	//	iter != m_mapRunwayExitVector.end(); iter++)
	//{
	//	RunwayExitPercentVector& vec = iter->second;

	//	for (RunwayExitPercentIter it=vec.begin(); it!=vec.end(); it++)
	//	{
	//		//	(*iter)->ExportData(exportFile);
	//		i++;
	//	}
	//}
	//exportFile.getFile().writeInt(i);
	//for(RunwayExitPercentVectorIter iter = m_mapRunwayExitVector.begin();
	//	iter != m_mapRunwayExitVector.end(); iter++)
	//{
	//	RunwayExitPercentVector& vec = iter->second;

	//	for (RunwayExitPercentIter it=vec.begin(); it!=vec.end(); it++)
	//	{
	//		(*it)->ExportData(exportFile);
	//	}
	//}
}
void TimeRangeRunwayExitStrategyItem::ImportData(CAirsideImportFile& importFile,int nStandStrategyID)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	int nStrategyType;
	importFile.getFile().getInteger(nStrategyType);
	m_emStrategyType = (RunwayExitStrategyType)nStrategyType;


	long lstartTime = 0L;
	importFile.getFile().getInteger(lstartTime);
	ElapsedTime startTime;
	startTime.set(lstartTime);

	long lendtime = 0L;
	importFile.getFile().getInteger(lendtime);
	ElapsedTime endTime;
	endTime.set(lendtime);

	m_dtTimeRange.SetStartTime(startTime);
	m_dtTimeRange.SetEndTime(endTime);

	m_nStandStrategyID = nStandStrategyID;
	SaveData();

	importFile.getFile().getLine();

	// Probability Based
	int nRunwayExitStrategyPercentItemCount = 0;
	importFile.getFile().getInteger(nRunwayExitStrategyPercentItemCount);

	for (int i =0; i < nRunwayExitStrategyPercentItemCount; ++i)
	{
		RunwayExitPercentItem data(m_nID);
		data.ImportData(importFile,m_nID);
	}

	// Priority Based
	int nRunwayExitStrategyPrioritiesCount = 0;
	importFile.getFile().getInteger(nRunwayExitStrategyPrioritiesCount);

	for (int i =0; i < nRunwayExitStrategyPrioritiesCount; ++i)
	{
		RunwayExitPriorityItem data(m_nID);
		data.ImportData(importFile,m_nID);
	}
}

RunwayExitStrategyPercentItem * TimeRangeRunwayExitStrategyItem::FindRunwaExitPercentItem( int nRunwayID, int nRunwayMark )
{
	RunwayExitStrategyPercentItem * pRunwayMarkItem = NULL;
	int nCount = static_cast<int >(m_vRunwayMarkItems.size());
	for (int nItem = 0;nItem < nCount;++nItem)
	{
		RunwayExitStrategyPercentItem * pTempRunwayMarkItem = m_vRunwayMarkItems.at(nItem);
		if(pTempRunwayMarkItem->IsSame(nRunwayID,nRunwayMark))
		{
			pRunwayMarkItem = pTempRunwayMarkItem;
			break;
		}
	}
	return pRunwayMarkItem;
}

RunwayExitStrategyPercentItem * TimeRangeRunwayExitStrategyItem::GetRunwayExitPercentItem( int nRunwayID, int nRunwayMark )
{
	RunwayExitStrategyPercentItem * pRunwayMarkItem = FindRunwaExitPercentItem(nRunwayID, nRunwayMark);
	if(pRunwayMarkItem == NULL)
	{
		pRunwayMarkItem = new RunwayExitStrategyPercentItem(nRunwayID,nRunwayMark);
		m_vRunwayMarkItems.push_back(pRunwayMarkItem);
	}

	return pRunwayMarkItem;
}

RunwayExitStrategyPriorityItem* TimeRangeRunwayExitStrategyItem::FindRunwayExitPriorityItem( int nRunwayID, int nRunwayMark )
{
	size_t nCount = m_vPriorityItems.size();
	for (size_t i = 0;i < nCount;++i)
	{
		RunwayExitStrategyPriorityItem * pPriorities = m_vPriorityItems.at(i);
		if(pPriorities->IsSame(nRunwayID,nRunwayMark))
		{
			return pPriorities;
		}
	}
	return NULL;
}

RunwayExitStrategyPriorityItem* TimeRangeRunwayExitStrategyItem::GetRunwayExitPriorityItem( int nRunwayID, int nRunwayMark )
{
	RunwayExitStrategyPriorityItem* pPriorities = FindRunwayExitPriorityItem(nRunwayID, nRunwayMark);
	if (NULL == pPriorities)
	{
		pPriorities = new RunwayExitStrategyPriorityItem(nRunwayID,nRunwayMark);
		m_vPriorityItems.push_back(pPriorities);
		ASSERT(pPriorities);
	}

	return pPriorities;
}

void TimeRangeRunwayExitStrategyItem::DeleteRunwayMarkExitItem( RunwayExitStrategyPercentItem * pItem )
{
	RunwayMarkExitStrategyIter iter = std::find(m_vRunwayMarkItems.begin(),m_vRunwayMarkItems.end(),pItem);
	if(iter != m_vRunwayMarkItems.end())
	{
		(*iter)->DeleteData();
		delete *iter;
		m_vRunwayMarkItems.erase(iter);
	}
}

void TimeRangeRunwayExitStrategyItem::DeleteRunwayMarkExitPriorities(RunwayExitStrategyPriorityItem * pItem)
{
	RunwayExitStrategyPrioritiesIter iter = std::find(m_vPriorityItems.begin(),m_vPriorityItems.end(),pItem);
	if(iter != m_vPriorityItems.end())
	{
		(*iter)->DeleteData();
		delete *iter;
		m_vPriorityItems.erase(iter);
	}
}

void TimeRangeRunwayExitStrategyItem::SaveVectorData( int nID )
{
	BOOL bSaveProbabilityItem = ManagedExit == m_emStrategyType && ProbabilityBased == m_strategyManagedExitMode;
	BOOL bSavePriorityItem = ManagedExit == m_emStrategyType && PriorityBased == m_strategyManagedExitMode;

	if (bSaveProbabilityItem)
	{
		RefreshRunwayExitPercentItems(nID); // refresh and save
	}
	else
	{
		RunwayMarkExitStrategyIter ite = m_vRunwayMarkItems.begin();
		RunwayMarkExitStrategyIter iteEn = m_vRunwayMarkItems.end();
		for (;ite!= iteEn;ite++)
		{
			(*ite)->DeleteData();
			delete *ite;
		}
		m_vRunwayMarkItems.clear();
	}

	if (bSavePriorityItem)
	{
		RefreshRunwayExitAllPriorities(nID);
	} 
	else
	{
		RunwayExitStrategyPrioritiesIter ite = m_vPriorityItems.begin();
		RunwayExitStrategyPrioritiesIter iteEn = m_vPriorityItems.end();
		for (;ite!= iteEn;ite++)
		{
			(*ite)->DeleteData();
			delete *ite;
		}
		m_vPriorityItems.clear();
	}
}

void TimeRangeRunwayExitStrategyItem::SortPriorities()
{
	RunwayExitStrategyPrioritiesIter ite = m_vPriorityItems.begin();
	RunwayExitStrategyPrioritiesIter iteEn = m_vPriorityItems.end();
	for (;ite!=iteEn;ite++)
	{
		(*ite)->ResortPriorities();
	}
}
RunwayExitStrategyPercentItem::RunwayExitStrategyPercentItem(int nRunwayID, int nRunwayMark)
{
	m_nRunwayID = nRunwayID;
	m_nRunwayMark = nRunwayMark;

}

RunwayExitStrategyPercentItem::~RunwayExitStrategyPercentItem()
{
	clear();
}

bool RunwayExitStrategyPercentItem::IsSame( int nRunwayID, int nRunwayMark )
{
	if(m_nRunwayID == nRunwayID && m_nRunwayMark == nRunwayMark)
		return true;
	return false;
}

int RunwayExitStrategyPercentItem::getRunwayID()
{
	return m_nRunwayID;
}

int RunwayExitStrategyPercentItem::getMarkIndex()
{
	return m_nRunwayMark;
}

void RunwayExitStrategyPercentItem::clear()
{
	RunwayExitPercentIter iter = m_vItems.begin();
	for(;iter != m_vItems.end(); ++iter)
	{
		delete *iter;
	}

	m_vItems.clear();

}

void RunwayExitStrategyPercentItem::DeleteData()
{
	RunwayExitPercentIter iter = m_vItems.begin();
	for(;iter != m_vItems.end(); ++iter)
	{
		if((*iter)->getID() != -1)
			(*iter)->DeleteData();

		delete *iter;
	}

	m_vItems.clear();
}

void RunwayExitStrategyPercentItem::RefreshRunwayExitPercentItems( int nTimeRangeID )
{
	RunwayExitPercentIter iter = m_vItems.begin();
	for(;iter != m_vItems.end(); ++iter)
	{
		if ( -1 == (*iter)->getID()) 
		{
			(*iter)->SetTimeRangeID(nTimeRangeID);
			(*iter)->SaveData();
		}
		else
			(*iter)->UpdateData();
	}
}

bool RunwayExitStrategyPercentItem::checkExitPercent()
{
	float percent = 0 ;
	RunwayExitPercentIter iter = m_vItems.begin();
	for(;iter != m_vItems.end(); ++iter)
	{
		RunwayExitPercentItem* pItemTemp = *iter;
		percent +=pItemTemp->m_fPercent;
	}
	if((int)percent > 100 || (int)percent < 98)
		return false;
	else
		percent = 0;

	return true;
}

void RunwayExitStrategyPercentItem::AddItem( RunwayExitPercentItem * pItem )
{
	m_vItems.push_back(pItem);
}

const RunwayExitPercentVector& RunwayExitStrategyPercentItem::getItems()
{
	return m_vItems;
}

//////////////////////////////////////////////////////////////////////////////////////
void RunwayChangeConditionData::SaveData( int nParentID )
{
	CString strSQL;

	strSQL.Format(_T("INSERT INTO IN_RUNWAY_EXIT_CONDITIONDATA\
					 (PARENTID, OPERATIONONTAXIWAY, ARRSTANDOCCUPIED, TAXIWAYSEGDATA)\
					 VALUES (%d,%d,%d,'%s')")
					 , nParentID
					 , m_bOperationOnTaxiway?1:0
					 , m_bArrStandOccupied?1:0
					 , MakeDatabaseString()
					 );

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
}

void RunwayChangeConditionData::UpdateData()
{
	CString strSQL;
	strSQL.Format(_T("UPDATE IN_RUNWAY_EXIT_CONDITIONDATA\
					 SET OPERATIONONTAXIWAY = %d, ARRSTANDOCCUPIED =%d, TAXIWAYSEGDATA = '%s' \
					 WHERE (ID = %d)")
					 , m_bOperationOnTaxiway?1:0
					 , m_bArrStandOccupied?1:0
					 , MakeDatabaseString()
					 , m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void RunwayChangeConditionData::DeleteData()
{
	CString strSQL;
	strSQL.Format(_T("DELETE FROM IN_RUNWAY_EXIT_CONDITIONDATA\
					 WHERE (ID = %d)"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void RunwayChangeConditionData::ReadData( int nParentID )
{
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM IN_RUNWAY_EXIT_CONDITIONDATA WHERE PARENTID = %d"),nParentID);

	CADORecordset adoRecordset;
	long nRecordAfflect = 0L;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordAfflect,adoRecordset);

	if (!adoRecordset.IsEOF())
	{
		adoRecordset.GetFieldValue("ID",m_nID);
		int iOperation = 0;
		adoRecordset.GetFieldValue("OPERATIONONTAXIWAY",iOperation);
		m_bOperationOnTaxiway = iOperation ? true : false;

		int iArrStand = 0;
		adoRecordset.GetFieldValue("ARRSTANDOCCUPIED",iArrStand);
		m_bArrStandOccupied = iArrStand ? true : false;

		CString strDatabase;
		adoRecordset.GetFieldValue("TAXIWAYSEGDATA",strDatabase);
		ParseDatabaseString(strDatabase);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
RunwayExitPriorityItem::RunwayExitPriorityItem( int nTimeRangeItemID )
: m_nID(-1)
, m_nRunwayID(-1)
, m_nExitID(-1)
, m_nRunwayMarkIndex(-1)
, m_nTimeRangeItemID(nTimeRangeItemID)
, m_nPriorityIndex(0)
{

}

void RunwayExitPriorityItem::ExportData(CAirsideExportFile& exportFile)
{
	exportFile.getFile().writeInt(m_nID);	
	exportFile.getFile().writeInt(m_nRunwayID);
	exportFile.getFile().writeInt(m_nRunwayMarkIndex);
	exportFile.getFile().writeInt(m_nExitID);
	exportFile.getFile().writeInt(m_nPriorityIndex);
	exportFile.getFile().writeLine();
}

void RunwayExitPriorityItem::ImportData(CAirsideImportFile& importFile,int nTimeRangeItemID)
{
	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	int nOldRunwayID = -1;
	importFile.getFile().getInteger(nOldRunwayID);
	m_nRunwayID = importFile.GetObjectNewID(nOldRunwayID);

	importFile.getFile().getInteger(m_nRunwayMarkIndex);

	int nOldExitID = -1;
	importFile.getFile().getInteger(nOldExitID);
	m_nExitID = importFile.GetRunwayExitIndexMap(nOldExitID);

	importFile.getFile().getInteger(m_nPriorityIndex);

	m_nTimeRangeItemID = nTimeRangeItemID;
	SaveData();

	importFile.getFile().getLine();
}



void RunwayExitPriorityItem::SaveData()
{
	CString strSQL;

	strSQL.Format(_T("INSERT INTO IN_RUNWAY_EXIT_PRIORITYDATA\
					 (RWEXITTIMEID, RUNWAYID, MARKINDEX, EXITID, BACKTRACK, PRIORITY)\
					 VALUES (%d,%d,%d,%d,%d,%d)")
					 , m_nTimeRangeItemID
					 , m_nRunwayID
					 , m_nRunwayMarkIndex
					 , m_nExitID
					 , m_bBacktrack ? 1 : 0
					 , m_nPriorityIndex);

	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	m_conditionData.SaveData(m_nID);
}
void RunwayExitPriorityItem::UpdateData()
{
	CString strSQL;
	strSQL.Format(_T("UPDATE IN_RUNWAY_EXIT_PRIORITYDATA\
					 SET RWEXITTIMEID = %d, RUNWAYID =%d, MARKINDEX = %d, EXITID =%d, BACKTRACK = %d, PRIORITY =%d\
					 WHERE (ID = %d)")
					 , m_nTimeRangeItemID
					 , m_nRunwayID
					 , m_nRunwayMarkIndex
					 , m_nExitID
					 , m_bBacktrack ? 1 : 0
					 , m_nPriorityIndex
					 , m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);
	m_conditionData.UpdateData();
}
void RunwayExitPriorityItem::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_RUNWAY_EXIT_PRIORITYDATA\
					 WHERE (ID = %d)"),m_nID);
	CADODatabase::ExecuteSQLStatement(strSQL);

	m_conditionData.DeleteData();
}

////////////////////////////////////////////////////////////////////////////////////////////

RunwayExitStrategyPriorityItem::RunwayExitStrategyPriorityItem( int nRunwayID, int nRunwayMark )
: m_nRunwayID(nRunwayID)
, m_nRunwayMark(nRunwayMark)
{

}

RunwayExitStrategyPriorityItem::~RunwayExitStrategyPriorityItem()
{
	clear();
}

void RunwayExitStrategyPriorityItem::clear()
{
	RunwayExitStrategyPriorityIter iter = m_vItems.begin();
	for(;iter != m_vItems.end(); ++iter)
	{
		delete *iter;
	}

	m_vItems.clear();

}

void RunwayExitStrategyPriorityItem::DeleteData()
{
	RunwayExitStrategyPriorityIter iter = m_vItems.begin();
	for(;iter != m_vItems.end(); ++iter)
	{
		if((*iter)->getID() != -1)
			(*iter)->DeleteData();

		delete *iter;
	}
	m_vItems.clear();

	RunwayExitStrategyPriorityIter iter2 = m_vDelItems.begin();
	for(;iter2 != m_vDelItems.end(); ++iter2)
	{
		if((*iter2)->getID() != -1)
			(*iter2)->DeleteData();

		delete *iter2;
	}
	m_vDelItems.clear();
}

void RunwayExitStrategyPriorityItem::RefreshRunwayExitPriorities( int nTimeRangeID )
{
	RunwayExitStrategyPriorityIter iter = m_vItems.begin();
	for(;iter != m_vItems.end(); ++iter)
	{
		if ( -1 == (*iter)->getID()) 
		{
			(*iter)->SetTimeRangeID(nTimeRangeID);
			(*iter)->SaveData();
		}
		else
			(*iter)->UpdateData();
	}

	// Delete un-wanted data
	RunwayExitStrategyPriorityIter iter2 = m_vDelItems.begin();
	for(;iter2 != m_vDelItems.end(); ++iter2)
	{
		if((*iter2)->getID() != -1)
			(*iter2)->DeleteData();

		delete *iter2;
	}
	m_vDelItems.clear();
}

class StrategyPrioritySorter
{
public:
	bool operator()(RunwayExitPriorityItem* lhs, RunwayExitPriorityItem* rhs) const
	{
		return lhs->getPriorityIndex() < rhs->getPriorityIndex();
	}
};

class StrategyPriorityIndexer
{
public:
	StrategyPriorityIndexer(int nIndex = 1) : m_nIndex(nIndex) {}
	void operator()(RunwayExitPriorityItem* pItem)
	{
		pItem->setPriorityIndex(m_nIndex++);
	}

private:
	int m_nIndex;
};

void RunwayExitStrategyPriorityItem::ResortPriorities()
{
	std::sort(m_vItems.begin(), m_vItems.end(), StrategyPrioritySorter());
	std::for_each(m_vItems.begin(), m_vItems.end(), StrategyPriorityIndexer());
}

void RunwayExitStrategyPriorityItem::AddItem( RunwayExitPriorityItem* pItem )
{
	m_vItems.push_back(pItem);
}

void RunwayExitStrategyPriorityItem::DeleteItem( RunwayExitPriorityItem* pItem )
{
	RunwayExitStrategyPriorityIter iter = std::find(m_vItems.begin(), m_vItems.end(), pItem);
	if (iter != m_vItems.end())
	{
		m_vDelItems.push_back(*iter);
		m_vItems.erase(iter);
	}

}

void RunwayExitStrategyPriorityItem::MoveUp( RunwayExitPriorityItem* pItem )
{
	RunwayExitStrategyPriorityIter iter = std::find(m_vItems.begin(), m_vItems.end(), pItem);
	if (m_vItems.end() == iter)
	{
		return;
	}
	if (m_vItems.begin() == iter) // ignore if the first
	{
		return;
	}
	RunwayExitPriorityItem* pItemExchange = *(--iter);
	int nTmp = pItemExchange->getPriorityIndex();
	pItemExchange->setPriorityIndex(pItem->getPriorityIndex());
	pItem->setPriorityIndex(nTmp);

	ResortPriorities();
}

void RunwayExitStrategyPriorityItem::MoveDown( RunwayExitPriorityItem* pItem )
{
	RunwayExitStrategyPriorityIter iter = std::find(m_vItems.begin(), m_vItems.end(), pItem);
	if (m_vItems.end() == iter)
	{
		return;
	}
	if (m_vItems.end() == ++iter) // ignore if the last
	{
		return;
	}
	RunwayExitPriorityItem* pItemExchange = *iter;
	int nTmp = pItemExchange->getPriorityIndex();
	pItemExchange->setPriorityIndex(pItem->getPriorityIndex());
	pItem->setPriorityIndex(nTmp);

	ResortPriorities();
}

void RunwayExitStrategyPriorityItem::MoveTop( RunwayExitPriorityItem* pItem )
{
	pItem->setPriorityIndex(-1);
	ResortPriorities();
}

void RunwayExitStrategyPriorityItem::MoveBottom( RunwayExitPriorityItem* pItem )
{
	pItem->setPriorityIndex((std::numeric_limits<int>::max)());
	ResortPriorities();
}

void RunwayExitStrategyPriorityItem::InsertItem( RunwayExitPriorityItem* pItem, int nIndex )
{
	RunwayExitStrategyPriorityIter iter = std::find(m_vItems.begin(), m_vItems.end(), pItem);
	if (m_vItems.end() != iter)
	{
		m_vItems.erase(iter);
	}

	int nCount = (int)m_vItems.size();
	nIndex = nIndex>=0 ? (nIndex<nCount+1 ? nIndex : nCount+1) : 0;
	m_vItems.insert(m_vItems.begin() + nIndex, pItem);
	std::for_each(m_vItems.begin(), m_vItems.end(), StrategyPriorityIndexer());
}

bool RunwayExitStrategyPriorityItem::ExchangeItem( RunwayExitPriorityItem* pItem, int nIndex )
{
	size_t sIndex = (size_t)nIndex;
	if (sIndex>=m_vItems.size())
	{
		return false;
	}
	RunwayExitStrategyPriorityIter iter = std::find(m_vItems.begin(), m_vItems.end(), pItem);
	if (m_vItems.end() == iter)
	{
		return false;
	}

	RunwayExitStrategyPriorityIter iter2 = m_vItems.begin() + sIndex;
	*iter = *iter2;
	*iter2 = pItem;

	std::for_each(m_vItems.begin(), m_vItems.end(), StrategyPriorityIndexer());
	return true;
}

const RunwayExitStrategyPriorityVector& RunwayExitStrategyPriorityItem::getItems()
{
	return m_vItems;
}

