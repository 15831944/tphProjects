#include "StdAfx.h"
#include "FlightTypeRunwayAssignmentStrategyItem.h"
#include "Common/FLT_CNST.H"
#include "TimeRangeRunwayAssignmentStrategyItem.h"
#include "../Database/ADODatabase.h"
#include "InputAirside/AirsideImportExportManager.h"
#include "Common/AirportDatabase.h"
#include "TakeOffTimeRangeRunwayAssignItem.h"
FlightTypeRunwayAssignmentStrategyItem::FlightTypeRunwayAssignmentStrategyItem(int nProjID)
:m_nProjectID(nProjID)
,m_nID(-1)
,m_pAirportDatabase(NULL)
{
}

FlightTypeRunwayAssignmentStrategyItem::~FlightTypeRunwayAssignmentStrategyItem(void)
{
	RemoveAll();
}

void FlightTypeRunwayAssignmentStrategyItem::ReadData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("SELECT ID, STARTTIME, ENDTIME, TYPE\
		FROM IN_RUNWAY_ASGN_TIME\
		WHERE (RWLANDASNID = %d)"),m_nID);

	CADORecordset adoRecordset;
	long nRecordAfflect = 0L;
	CADODatabase::ExecuteSQLStatement(strSQL,nRecordAfflect,adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		TimeRangeRunwayAssignmentStrategyItem *pItem = new TimeRangeRunwayAssignmentStrategyItem(m_nID);

		int nID = -1;
		adoRecordset.GetFieldValue(_T("ID"),nID);
		pItem->setID(nID);
		
		int nType;
		adoRecordset.GetFieldValue(_T("TYPE"),nType);
		pItem->setStrategyType((RunwayAssignmentStrategyType)nType);

		long lStartTime = 0;
		adoRecordset.GetFieldValue(_T("STARTTIME"),lStartTime);
		pItem->setStartTime(lStartTime);

		long lEndTime = 0;
		adoRecordset.GetFieldValue(_T("ENDTIME"),lEndTime);
		pItem->setEndTime(lEndTime);
		
		if(nType = (int)ManagedAssignment)
			pItem->ReadData();

		m_vectTimeRangeRunwayAssignmentStrategy.push_back(pItem);

		adoRecordset.MoveNextData();
	}

}

void FlightTypeRunwayAssignmentStrategyItem::SaveData(const RunwayAssignmentType& rwAssignType)
{

	if (m_nID != -1)
	{
		return UpdateData();
	}
	CString strSQL = _T("");

	strSQL.Format(_T("INSERT INTO IN_RUNWAY_ASGN\
		(FLTTYPE,PROJID,ASGNTYPE)\
		VALUES ('%s',%d,%d)"),m_strFltType,m_nProjectID,(int)rwAssignType);


	m_nID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

	for (TimeRangeStrategyItemIter iter = m_vectTimeRangeRunwayAssignmentStrategy.begin(); 
		iter != m_vectTimeRangeRunwayAssignmentStrategy.end(); iter++)
	{
		if(-1 == (*iter)->getID())
		{
			(*iter)->SetFltTypeItemID(m_nID);
			(*iter)->SaveData();
		}
		else
			(*iter)->UpdateData();
	}

	DeleteDataFromDatabase();
}

void FlightTypeRunwayAssignmentStrategyItem::UpdateData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("UPDATE IN_RUNWAY_ASGN\
		SET FLTTYPE ='%s', PROJID =%d\
		WHERE (ID = %d)"), m_strFltType, m_nProjectID, m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	for (TimeRangeStrategyItemIter iter = m_vectTimeRangeRunwayAssignmentStrategy.begin(); 
		iter != m_vectTimeRangeRunwayAssignmentStrategy.end(); iter++)
	{
		if(-1 == (*iter)->getID())
		{
			(*iter)->SetFltTypeItemID(m_nID);
			(*iter)->SaveData();
		}
		else
			(*iter)->UpdateData();
	}

	DeleteDataFromDatabase();
}

void FlightTypeRunwayAssignmentStrategyItem::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM IN_RUNWAY_ASGN\
		WHERE (ID = %d)"),m_nID);

	CADODatabase::ExecuteSQLStatement(strSQL);

	DeleteDataFromDatabase();
}

int FlightTypeRunwayAssignmentStrategyItem::GetTimeRangeStrategyItemCount()
{
	return (int)m_vectTimeRangeRunwayAssignmentStrategy.size();
}

TimeRangeRunwayAssignmentStrategyItem* FlightTypeRunwayAssignmentStrategyItem::GetTimeRangeStrategyItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex<(int)m_vectTimeRangeRunwayAssignmentStrategy.size());
	return m_vectTimeRangeRunwayAssignmentStrategy[nIndex];
}

void FlightTypeRunwayAssignmentStrategyItem::AddTimeRangeRWAssignStrategyItem(TimeRangeRunwayAssignmentStrategyItem* pItem)
{
	m_vectTimeRangeRunwayAssignmentStrategy.push_back(pItem);
}

void FlightTypeRunwayAssignmentStrategyItem::DeleteTimeRangeRWAssignStrategyItem(TimeRangeRunwayAssignmentStrategyItem* pItem)
{
	TimeRangeStrategyItemIter iter = 
		std::find(m_vectTimeRangeRunwayAssignmentStrategy.begin(), m_vectTimeRangeRunwayAssignmentStrategy.end(), pItem);
	if (iter == m_vectTimeRangeRunwayAssignmentStrategy.end())
		return;
	m_vectNeedDelStrategy.push_back(*iter);
	m_vectTimeRangeRunwayAssignmentStrategy.erase(iter);
}

void FlightTypeRunwayAssignmentStrategyItem::UpdateTimeRangeRWAssignStrategyItem(TimeRangeRunwayAssignmentStrategyItem* pItem)
{

}

void FlightTypeRunwayAssignmentStrategyItem::DeleteDataFromDatabase()
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

void FlightTypeRunwayAssignmentStrategyItem::RemoveAll()
{
	for (TimeRangeStrategyItemIter iter = m_vectTimeRangeRunwayAssignmentStrategy.begin();
		iter != m_vectTimeRangeRunwayAssignmentStrategy.end(); iter++) 
	{
		delete(*iter);
		(*iter) = NULL;
	}
	m_vectTimeRangeRunwayAssignmentStrategy.clear();

}
void FlightTypeRunwayAssignmentStrategyItem::ExportFlightTypeRunwayAssignment(CAirsideExportFile& exportFile,int nAssignType)
{

	exportFile.getFile().writeInt(m_nID);
	exportFile.getFile().writeInt(nAssignType);
	exportFile.getFile().writeField(m_strFltType);

	exportFile.getFile().writeInt(GetTimeRangeStrategyItemCount());
	exportFile.getFile().writeLine();
	
	for (TimeRangeStrategyItemIter iter = m_vectTimeRangeRunwayAssignmentStrategy.begin();
		iter != m_vectTimeRangeRunwayAssignmentStrategy.end(); iter++) 
	{
		(*iter)->ExportTimeRangeRunwayAssignment(exportFile);
	}



}
void FlightTypeRunwayAssignmentStrategyItem::ImportFlightTypeRunwayAssignment(CAirsideImportFile& importFile)
{

	int nOldID = -1;
	importFile.getFile().getInteger(nOldID);

	int nAssignType =0;
	importFile.getFile().getInteger(nAssignType);

	CString strFltType = _T("");
	importFile.getFile().getField(strFltType.GetBuffer(1024),1024);
	setFlightType(strFltType);

	int nTimeRangeCount = 0;

	importFile.getFile().getInteger(nTimeRangeCount);
	importFile.getFile().getLine();


	SaveData((RunwayAssignmentType)nAssignType);

	for (int i =0; i < nTimeRangeCount; ++i)
	{
		TimeRangeRunwayAssignmentStrategyItem timeItem(getID());
//		timeItem.SetFltTypeItemID(GetID());
		timeItem.ImportTimeRangeRunwayAssignment(importFile);
	}
}

void FlightTypeRunwayAssignmentStrategyItem::setFlightType(const CString& strFltType)
{
	 m_strFltType = strFltType;

	 if (m_pAirportDatabase)
	 {
		 m_FlightType.SetAirportDB(m_pAirportDatabase);
		 m_FlightType.setConstraintWithVersion(m_strFltType);
	 }


}

bool FlightTypeRunwayAssignmentStrategyItem::operator > ( FlightTypeRunwayAssignmentStrategyItem& element)  
{ 
	int nFit = m_FlightType.fits(element.GetFlightType());
	return   nFit > 0?false:true;
}

//////////////////////////////////////////////////////////////////////////

void CTakeoffFlightTypeRunwayAssignment::RemoveAllData()
{
	for (int i = 0 ; i < (int)m_TimeRange.size() ;i++)
	{
		delete m_TimeRange[i] ;
	}
	m_TimeRange.clear() ;

	RemoveDeleteData() ;
}
void CTakeoffFlightTypeRunwayAssignment::RemoveDeleteData()
{
	for (int i = 0 ; i < (int)m_DelTimeRange.size() ;i++)
	{
		delete m_DelTimeRange[i] ;
	}
	m_DelTimeRange.clear() ;
}
void CTakeoffFlightTypeRunwayAssignment::ReadData(std::vector<CTakeoffFlightTypeRunwayAssignment*>& _data, int _ParID,CAirportDatabase* _pAirportDatabase)
{
	CString SQL ;
	SQL = CTakeoffFlightTypeRunwayAssignment::GetSelectSQL(_ParID) ;
	long count =0 ;
	CADORecordset dataSet ;
	try{
		CADODatabase::ExecuteSQLStatement(SQL,count,dataSet) ;
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CTakeoffFlightTypeRunwayAssignment* Pitem = NULL ;
	CString strval ;
	int _id = 0 ;
	while(!dataSet.IsEOF())
	{
		dataSet.GetFieldValue(_T("FLT_TYPE"),strval) ;
		dataSet.GetFieldValue(_T("ID"),_id) ;
		Pitem = new CTakeoffFlightTypeRunwayAssignment ;
		Pitem->GetFlightType().SetAirportDB(_pAirportDatabase) ;
		Pitem->GetFlightType().setConstraintWithVersion(strval) ;
		Pitem->SetID(_id) ;
		CTakeOffTimeRangeRunwayAssignItem::ReadData(*(Pitem->GetTimeRangeData()) ,_id) ;
		_data.push_back(Pitem) ;
		dataSet.MoveNextData() ;
	}
}
void CTakeoffFlightTypeRunwayAssignment::WriteData( std::vector<CTakeoffFlightTypeRunwayAssignment*>&_data ,int _parID)
{
	 CTakeoffFlightTypeRunwayAssignment* Pitem = NULL ;
	for(int i = 0 ; i < (int)_data.size() ;i++)
	{
		Pitem = _data[i] ;
		CString SQL ;
		if(Pitem->GetID() == -1)
			SQL = CTakeoffFlightTypeRunwayAssignment::GetInsertSQL(Pitem,_parID) ;
		else
			SQL = CTakeoffFlightTypeRunwayAssignment::GetUpdateSQL(Pitem,_parID) ;

		try
		{
			CADODatabase::BeginTransaction( ) ;
			int _id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
			if(Pitem->GetID() == -1)
				Pitem->SetID(_id) ;
			CTakeOffTimeRangeRunwayAssignItem::WriteData(*(Pitem->GetTimeRangeData()),Pitem->GetID()) ;
			CTakeOffTimeRangeRunwayAssignItem::DeleteData(Pitem->m_DelTimeRange) ;
			Pitem->RemoveDeleteData() ;
			CADODatabase::CommitTransaction() ;
		}catch (CADOException& e)
		{
			e.ErrorMessage() ;
			CADODatabase::RollBackTransation() ;
			return ;
		}
	}
}
void CTakeoffFlightTypeRunwayAssignment::DeleteData(std::vector<CTakeoffFlightTypeRunwayAssignment*>& _data)
{
	CTakeoffFlightTypeRunwayAssignment* Pitem = NULL ;
	for (int i = 0 ; i < (int)_data.size() ;i++)
	{
		Pitem = _data[i] ;

		CString SQL ;

		SQL = CTakeoffFlightTypeRunwayAssignment::GetDeleteSQL(Pitem) ;

		try
		{
			CADODatabase::BeginTransaction() ;
			CADODatabase::ExecuteSQLStatement(SQL) ;
			CADODatabase::CommitTransaction() ;
		}
		catch (CADOException& e)
		{
			e.ErrorMessage() ;
			CADODatabase::RollBackTransation() ;
			return ;
		}
	}
}
CString CTakeoffFlightTypeRunwayAssignment::GetSelectSQL(int _parID)
{
	CString SQL ;
	if(_parID == -1)
		return SQL ;
	SQL.Format(_T("SELECT * FROM TB_TAKEOFFRUNWAY_FLIGHTTY WHERE PAR_ID = %d") ,_parID) ;

	return SQL ;
}
CString CTakeoffFlightTypeRunwayAssignment::GetInsertSQL(CTakeoffFlightTypeRunwayAssignment* _timeRange ,int _ParID)
{
	CString SQL ;
	if(_ParID == -1 || _timeRange == NULL)
		return SQL;
	TCHAR strVal[1024] = {0} ;
	_timeRange->GetFlightType().WriteSyntaxStringWithVersion(strVal) ;
	SQL.Format(_T("INSERT INTO TB_TAKEOFFRUNWAY_FLIGHTTY (FLT_TYPE,PAR_ID) VALUES('%s',%d)"),strVal,_ParID) ;
	return SQL ;
}
CString CTakeoffFlightTypeRunwayAssignment::GetUpdateSQL(CTakeoffFlightTypeRunwayAssignment* _timeRange ,int _ParID)
{
	CString SQL ;
	if(_ParID == -1 || _timeRange == NULL)
		return SQL ;
	TCHAR strVal[1024] = {0} ;
	_timeRange->GetFlightType().WriteSyntaxStringWithVersion(strVal) ;
	SQL.Format(_T("UPDATE TB_TAKEOFFRUNWAY_FLIGHTTY SET FLT_TYPE = '%s' ,PAR_ID = %d WHERE ID = %d"),strVal,_ParID,_timeRange->GetID()) ;
	return SQL ;
}
CString CTakeoffFlightTypeRunwayAssignment::GetDeleteSQL(CTakeoffFlightTypeRunwayAssignment* _timeRange)
{
	CString SQL ;
	if(_timeRange == NULL)
		return SQL;
	SQL.Format(_T("DELETE * FROM TB_TAKEOFFRUNWAY_FLIGHTTY WHERE ID = %d"),_timeRange->GetID()) ;
	return SQL ;
}
CTakeOffTimeRangeRunwayAssignItem* CTakeoffFlightTypeRunwayAssignment::AddTakeoffTimeRangeeItem(TimeRange& _flt)
{
	CTakeOffTimeRangeRunwayAssignItem* pTimeRange = FindTakeoffTimeRangeeItem(_flt) ;
	if(pTimeRange)
		return NULL ;

	pTimeRange = new CTakeOffTimeRangeRunwayAssignItem ;
	pTimeRange->SetStartTime(_flt.GetStartTime()) ;
	pTimeRange->SetEndTime(_flt.GetEndTime()) ;
	m_TimeRange.push_back(pTimeRange) ;
	return pTimeRange ;
}
CTakeOffTimeRangeRunwayAssignItem* CTakeoffFlightTypeRunwayAssignment::FindTakeoffTimeRangeeItem( TimeRange& _flt)
{
	CTakeOffTimeRangeRunwayAssignItem* PTimeItem = NULL ;
	for (int i = 0 ;i < (int)m_TimeRange.size() ;i++)
	{
		PTimeItem  = m_TimeRange[i] ;
		if(!(_flt.GetStartTime()> PTimeItem->GetEndTime() || _flt.GetEndTime() < PTimeItem->GetStartTime()))
			return m_TimeRange[i] ;
	}
	return NULL ;
}
void CTakeoffFlightTypeRunwayAssignment::RemoveTakeoffTimeRangeeItem(CTakeOffTimeRangeRunwayAssignItem* _data) 
{
	TY_DATA_ITER iter = std::find(m_TimeRange.begin(),m_TimeRange.end(),_data) ;
	if(iter != m_TimeRange.end())
	{
		m_TimeRange.erase(iter) ;
		m_DelTimeRange.push_back(_data) ;
	}
}


