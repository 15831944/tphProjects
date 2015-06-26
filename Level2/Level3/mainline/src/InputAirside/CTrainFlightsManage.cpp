#include "StdAfx.h"
#include "..\Database\ADODatabase.h"
#include "TakeOffTimeRangeRunwayAssignItem.h"
#include "CTrainFlightsManage.h"
#include "TrainingFlightContainer.h"
#include "CTrainFlightsManage.h"

class CTrainingFlightTypeManage;
class CTrainingTimeRangeManage;

CTrainingFlightTypeManage* CTrainingFlightsManage::AddTrainFlightType(CTrainingFlightData& flightType)
{
	CTrainingFlightTypeManage* _flighttype = FindFlightTypeItem(flightType);
	if (_flighttype)
		return NULL;
	_flighttype = new CTrainingFlightTypeManage;
	_flighttype->SetFlightType(flightType.GetName());
	_flighttype->SetID(flightType.GetID());
	m_FlightData.push_back(_flighttype);
	return _flighttype;
}

CTrainingFlightTypeManage* CTrainingFlightsManage::FindFlightTypeItem(CTrainingFlightData& flightType)
{
	
	TY_DATA_ITER iter = m_FlightData.begin();
	for ( ; iter != m_FlightData.end(); iter++)
	{
		if((*iter)->GetID() == flightType.GetID())
			return *iter;
	}
	return NULL;
}

void CTrainingFlightsManage::ReadData()
{	
	m_Flightyp.ReadData(m_FlightData,-1);
	CTrainingFlightContainer m_trainFlightContainer;
	m_trainFlightContainer.ReadData();
	for (unsigned i = 0; i < m_trainFlightContainer.GetElementCount(); i++)
	{
		CTrainingFlightData* pTrainFlight = (CTrainingFlightData*)m_trainFlightContainer.GetItem(i);
		AddTrainFlightType(*pTrainFlight);
	}
}

void CTrainingFlightsManage::WriteData()
{
	m_Flightyp.WriteData(m_FlightData,-1);
}

void CTrainingFlightsManage::ClearData()
{
	for (int i = 0; i < (int)m_FlightData.size(); i++)
	{
		delete m_FlightData[i];
	}
	m_FlightData.clear();
}

void CTrainingFlightsManage::RemoveAllData()
{
	for (int i = 0; i < (int)m_FlightData.size();i++)
	{
		delete m_FlightData[i];
	}
	m_FlightData.clear();
}

CTrainingFlightsManage::~CTrainingFlightsManage()
{
	RemoveAllData();
}


/////////////////////////////////////////////////////////////////////////////////////////////////


void CTrainingFlightTypeManage::RemoveAllData()
{
	for (int i = 0 ; i < (int)m_TimeRangeList.size() ;i++)
	{
		delete m_TimeRangeList[i];
	}
	m_TimeRangeList.clear();

	RemoveDeleteData();
}

void CTrainingFlightTypeManage::RemoveDeleteData()
{
	for (int i = 0; i < (int)m_DelTimeRange.size(); i++)
	{
		delete m_DelTimeRange[i];
	}
	m_DelTimeRange.clear();
}

void CTrainingFlightTypeManage::ReadData(std::vector<CTrainingFlightTypeManage*>& _data, int _ParID)
{
	CString SQL;
	SQL = CTrainingFlightTypeManage::GetSelectSQL(_ParID);
	long count =0;
	CADORecordset dataSet;
	try{
		CADODatabase::BeginTransaction();
		CADODatabase::ExecuteSQLStatement(SQL,count,dataSet);
		CADODatabase::CommitTransaction();
	}catch(CADOException& e)
	{
		e.ErrorMessage();
		return;
	}
	CTrainingFlightTypeManage* Pitem = NULL;
	CString strType;
	int strval;
	int _id = -1;
	while(!dataSet.IsEOF())
	{
		dataSet.GetFieldValue(_T("FLT_TYPE"),strType);
		dataSet.GetFieldValue(_T("FLT_ID"),strval);
		dataSet.GetFieldValue(_T("ID"),_id);
		Pitem = new CTrainingFlightTypeManage;
		Pitem->SetFlightType(strType);
		Pitem->SetID(strval);
		Pitem->m_ID = _id;
		m_TimeRange.ReadData(*(Pitem->GetTimeRangeData()) ,_id);
		_data.push_back(Pitem);
		dataSet.MoveNextData();
	}
}

void CTrainingFlightTypeManage::WriteData( std::vector<CTrainingFlightTypeManage*>&_data,int _parID)
{
	CTrainingFlightTypeManage* Pitem = NULL;
	for(int i = 0 ; i < (int)_data.size() ;i++)
	{
		Pitem = _data[i];
		CString SQL;
		if(Pitem->m_ID == -1)
			SQL = CTrainingFlightTypeManage::GetInsertSQL(Pitem,_parID);
		else
			SQL = CTrainingFlightTypeManage::GetUpdateSQL(Pitem,_parID);

		try
		{
			CADODatabase::BeginTransaction( );
			int _id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL);
			if(Pitem->m_ID == -1)
				Pitem->m_ID = _id;
			m_TimeRange.WriteData(*(Pitem->GetTimeRangeData()),Pitem->m_ID);
			m_TimeRange.DeleteData(Pitem->m_DelTimeRange);
			Pitem->RemoveDeleteData();
			CADODatabase::CommitTransaction();
		}catch (CADOException& e)
		{
			e.ErrorMessage();
			CADODatabase::RollBackTransation();
			return;
		}
	}
}

void CTrainingFlightTypeManage::DeleteData(std::vector<CTrainingFlightTypeManage*>& _data)
{
	CTrainingFlightTypeManage* Pitem = NULL;
	for (int i = 0; i < (int)_data.size(); i++)
	{
		Pitem = _data[i];

		CString SQL;

		SQL = CTrainingFlightTypeManage::GetDeleteSQL(Pitem);

		try
		{
			CADODatabase::BeginTransaction();
			CADODatabase::ExecuteSQLStatement(SQL);
			CADODatabase::CommitTransaction();
		}
		catch (CADOException& e)
		{
			e.ErrorMessage();
			CADODatabase::RollBackTransation();
			return;
		}
	}
}

CString CTrainingFlightTypeManage::GetSelectSQL(int _parID)
{
	CString SQL;
	SQL.Format(_T("SELECT * FROM TB_TRAIN_FLIGHTTYPE_MANAGEMENT WHERE PAR_ID = %d"), _parID);
	return SQL;
}

CString CTrainingFlightTypeManage::GetInsertSQL(CTrainingFlightTypeManage* _TrainFlight, int _ParID)
{
	CString SQL;
	SQL.Format(_T("INSERT INTO TB_TRAIN_FLIGHTTYPE_MANAGEMENT (FLT_ID,FLT_TYPE,PAR_ID) VALUES(%d,'%s',%d)"),_TrainFlight->GetID(), _TrainFlight->GetFlightType(), _ParID);
	return SQL;
}

CString CTrainingFlightTypeManage::GetUpdateSQL(CTrainingFlightTypeManage* _TrainFlight,int  _ParID)
{
	CString SQL;
	SQL.Format(_T("UPDATE TB_TRAIN_FLIGHTTYPE_MANAGEMENT SET FLT_ID = %d ,FLT_TYPE = '%s',PAR_ID = %d WHERE ID = %d"), _TrainFlight->GetID(), _TrainFlight->GetFlightType(), _ParID, _TrainFlight->m_ID);
	return SQL;
}

CString CTrainingFlightTypeManage::GetDeleteSQL(CTrainingFlightTypeManage* _TrainFlight)
{
	CString SQL;
	if(_TrainFlight == NULL)
		return SQL;
	SQL.Format(_T("DELETE * FROM TB_TRAIN_FLIGHTTYPE_MANAGEMENT WHERE ID = %d"),_TrainFlight->m_ID);
	return SQL;
}

CTrainingTimeRangeManage* CTrainingFlightTypeManage::AddTrainingTimeRange(TimeRange& _flt)
{
	CTrainingTimeRangeManage* pTimeRange = FindTrainingTimeRangee(_flt);
	if(pTimeRange)
		return NULL;
	pTimeRange = new CTrainingTimeRangeManage;
	pTimeRange->SetStartTime(_flt.GetStartTime());
	pTimeRange->SetEndTime(_flt.GetEndTime());
	m_TimeRangeList.push_back(pTimeRange);
	return pTimeRange;
}

CTrainingTimeRangeManage* CTrainingFlightTypeManage::FindTrainingTimeRangee( TimeRange& _flt)
{
	CTrainingTimeRangeManage* PTimeItem = NULL;
	for (int i = 0; i < (int)m_TimeRangeList.size(); i++)
	{
		PTimeItem  = m_TimeRangeList[i];
		if(!(_flt.GetStartTime()> PTimeItem->GetEndTime() || _flt.GetEndTime() < PTimeItem->GetStartTime()))
			return m_TimeRangeList[i];
	}
	return NULL;
}

void CTrainingFlightTypeManage::RemoveTrainingTimeRangeeItem(CTrainingTimeRangeManage* _data) 
{
	TY_DATA_ITER iter = std::find(m_TimeRangeList.begin(),m_TimeRangeList.end(),_data);
	if(iter != m_TimeRangeList.end())
	{
		m_TimeRangeList.erase(iter);
		m_DelTimeRange.push_back(_data);
	}
}

int CTrainingFlightTypeManage::GetCount() const
{
	return (int)m_TimeRangeList.size();
}

CTrainingTimeRangeManage* CTrainingFlightTypeManage::GetItem( int idx ) const
{
	if (idx < 0 || idx >= GetCount())
		return NULL;
	return m_TimeRangeList.at(idx);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTrainingTimeRangeManage::RemoveAllData()
{
	for (int i = 0; i < (int)m_TakeOffPositionList.size(); i++)
	{
		delete m_TakeOffPositionList[i];
	}
	m_TakeOffPositionList.clear();
	RemoveDelData();
}

void CTrainingTimeRangeManage::RemoveDelData()
{
	for (int i = 0; i < (int)m_DelTakeOffPosition.size(); i++)
	{
		delete m_DelTakeOffPosition[i];
	}
	m_DelTakeOffPosition.clear();
}

CTrainingTimeRangeManage::~CTrainingTimeRangeManage()
{
	RemoveAllData();
}

CTrainingTimeRangeManage::CTrainingTimeRangeManage() 
:m_ID(-1)

{

}

void CTrainingTimeRangeManage::ReadData(std::vector<CTrainingTimeRangeManage*>& _data,int _ParID)
{
	CString SQL;
	if(_ParID == -1)
		return;
	SQL = CTrainingTimeRangeManage::GetSelectSQL(_ParID);
	long count = 0;
	CADORecordset recordSet;
	try{
		CADODatabase::ExecuteSQLStatement(SQL,count,recordSet);
	}catch(CADOException& e)
	{
		e.ErrorMessage();
		return;
	}
	int val = 0;
	CTrainingTimeRangeManage*  PItemData = NULL;
	while(!recordSet.IsEOF())
	{
		PItemData = new CTrainingTimeRangeManage;
		ElapsedTime timeStart, timeEnd;
		recordSet.GetFieldValue(_T("START_TIME"),val);
		timeStart.SetSecond(val);
		PItemData->SetStartTime(timeStart);

		recordSet.GetFieldValue(_T("END_TIME"),val);
		timeEnd.SetSecond(val);
		PItemData->SetEndTime(timeEnd);
		
		recordSet.GetFieldValue(_T("ID"),val);
		PItemData->SetID(val);
		m_TakeOffPosition.ReadData(*(PItemData->GetTakeOffPosData()),val);
		_data.push_back(PItemData);
		recordSet.MoveNextData();
	}
}
void CTrainingTimeRangeManage::WriteData(std::vector<CTrainingTimeRangeManage*>& _data,int _parID)
{
	CTrainingTimeRangeManage* PtimeRangeItem = NULL;
	for (int i = 0 ; i < (int)_data.size() ;i++)
	{
		PtimeRangeItem = _data[i];
		CString SQL;
		if(PtimeRangeItem->GetID() == -1)
			SQL = CTrainingTimeRangeManage::GetInsertSQL(PtimeRangeItem,_parID);
		else
			SQL = CTrainingTimeRangeManage::GetUpdateSQL(PtimeRangeItem,_parID);

		try
		{
			CADODatabase::BeginTransaction( );
			int _id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL);
			if(PtimeRangeItem->GetID() == -1)
				PtimeRangeItem->SetID(_id);
			m_TakeOffPosition.WriteData(PtimeRangeItem->m_TakeOffPositionList,PtimeRangeItem->GetID());
			m_TakeOffPosition.DeleteDate(PtimeRangeItem->m_DelTakeOffPosition);
			PtimeRangeItem->RemoveDelData();
			CADODatabase::CommitTransaction();
		}
		catch (CADOException& e)
		{
			CADODatabase::RollBackTransation();
			e.ErrorMessage();
			return;
		}
	}
}
void CTrainingTimeRangeManage::DeleteData(std::vector<CTrainingTimeRangeManage*>& _data)
{
	CTrainingTimeRangeManage* PTimeRange = NULL;
	for (int i = 0; i < (int)_data.size(); i++)
	{
		PTimeRange = _data[i];

		CString SQL;
		SQL = CTrainingTimeRangeManage::GetDeleteSQL(PTimeRange);

		try
		{
			CADODatabase::BeginTransaction();
			CADODatabase::ExecuteSQLStatement(SQL);
			CADODatabase::CommitTransaction();
		}
		catch (CADOException& e)
		{
			CADODatabase::RollBackTransation();
			e.ErrorMessage();
			return;
		}
	}
}

CString CTrainingTimeRangeManage::GetSelectSQL(int _parID)
{
	CString SQL;
	SQL.Format(_T("SELECT * FROM TB_TRAINFLIGHTMANAGEMENT_TIMERANGE WHERE PAR_ID = %d"),_parID);
	return SQL;
}

CString CTrainingTimeRangeManage::GetInsertSQL(CTrainingTimeRangeManage* _timeRange ,int _parID)
{
	CString SQL;
	SQL.Format(_T("INSERT INTO TB_TRAINFLIGHTMANAGEMENT_TIMERANGE (START_TIME,END_TIME,PAR_ID) VALUES(%d,%d,%d)"),
		_timeRange->GetStartTime().asSeconds(),
		_timeRange->GetEndTime().asSeconds(),
		_parID);
	return SQL;
}

CString CTrainingTimeRangeManage::GetUpdateSQL(CTrainingTimeRangeManage* _timeRange ,int _parID)
{
	CString SQL;
	SQL.Format(_T("UPDATE TB_TRAINFLIGHTMANAGEMENT_TIMERANGE SET START_TIME = %d,END_TIME = %d,PAR_ID = %d WHERE ID = %d"),
		_timeRange->GetStartTime().asSeconds(),
		_timeRange->GetEndTime().asSeconds(),
		_parID,
		_timeRange->GetID());
	return SQL;
}

CString CTrainingTimeRangeManage::GetDeleteSQL(CTrainingTimeRangeManage* _timeRange )
{
	CString SQL;
	SQL.Format(_T("DELETE * FROM TB_TRAINFLIGHTMANAGEMENT_TIMERANGE WHERE ID = %d"),_timeRange->GetID());
	return SQL;
}

CTrainingTakeOffPosition* CTrainingTimeRangeManage::AddTrainTakeoffPosition(const std::vector<RunwayExit> _exitlist)
{
	CTrainingTakeOffPosition* PTrainItem = new CTrainingTakeOffPosition;
	if (!PTrainItem)
	{
		return NULL;
	}
	for (int i = 0; i < (int)_exitlist.size(); i++)
	{
		PTrainItem->m_runwayIdList.push_back(_exitlist.at(i).GetID());
	}	
	m_TakeOffPositionList.push_back(PTrainItem);
	return PTrainItem;
}

CTrainingTakeOffPosition* CTrainingTimeRangeManage::FindTakeoffPosition(const std::vector<RunwayExit>& exit)
{
	for (int i = 0; i < (int)m_TakeOffPositionList.size(); i++)
	{
		CTrainingTakeOffPosition* pTakeoffPos = m_TakeOffPositionList.at(i);
		if(pTakeoffPos->m_runwayIdList.size() == exit.size())
		{
			int count = 0;
			for (int i = 0; i < (int)exit.size(); i++)
			{
				if (pTakeoffPos->m_runwayIdList.at(i) == exit.at(i).GetID())
				{
					count++;
					continue;
				}
				else
				{
					break;
				}
			}
			if (count == exit.size())
			{
				return pTakeoffPos;		
			}			
		}				
	}
	return NULL;
}

void CTrainingTimeRangeManage::RemoveTrainTakeoffPosition( CTrainingTakeOffPosition* postion )
{
	std::vector<CTrainingTakeOffPosition*>::iterator iter = std::find(m_TakeOffPositionList.begin(), m_TakeOffPositionList.end(), postion) ;
	if(iter != m_TakeOffPositionList.end())
	{
		m_TakeOffPositionList.erase(iter);
		m_DelTakeOffPosition.push_back(postion);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTrainingTakeOffPosition::CTrainingTakeOffPosition()
{
	m_nID = -1;
	m_ApproachSequence= TRUE;
	m_TakeOffSequence = TRUE;
	m_ApproachPax = 0;
	m_TakeOffPax = 0;
}

void CTrainingTakeOffPosition::ReadData(std::vector<CTrainingTakeOffPosition*>&_data ,int _parID)
{
	CString SQL;
	SQL = CTrainingTakeOffPosition::GetSelectSQL(_parID,TB_POSITION);
	long count =0;
	CADORecordset dataSet;
	try{
		CADODatabase::ExecuteSQLStatement(SQL,count,dataSet);
	}catch(CADOException& e)
	{
		e.ErrorMessage();
		return;
	}
	CTrainingTakeOffPosition* Pitem = NULL;
	CString _name;
	bool strval;
	int paxval;
	int _id;
	while(!dataSet.IsEOF())
	{
		Pitem = new CTrainingTakeOffPosition;		
		dataSet.GetFieldValue(_T("APPROACHSEQUENCE"),strval);
		Pitem->m_ApproachSequence = strval;
		dataSet.GetFieldValue(_T("APPROACHPAX"),paxval);
		Pitem->m_ApproachPax = paxval;
		dataSet.GetFieldValue(_T("TAKEOFFSEQUENCE"),strval);
		Pitem->m_TakeOffSequence = strval;
		dataSet.GetFieldValue(_T("TAKEOFFPAX"),paxval);
		Pitem->m_TakeOffPax = paxval;
		dataSet.GetFieldValue(_T("ID"),_id);
		Pitem->m_nID = _id;
		Pitem->ReadData(Pitem->m_runwayIdList,Pitem->m_nID);
		_data.push_back(Pitem);
		dataSet.MoveNextData();
	}	
}

void CTrainingTakeOffPosition::WriteData(std::vector<CTrainingTakeOffPosition*>&_data ,int _parID)
{
	CString SQL;
	CTrainingTakeOffPosition* Pitem = NULL;
	try
	{
		CADODatabase::BeginTransaction( );

		for(int i = 0; i < (int)_data.size(); i++)
		{
		   Pitem = _data[i];
		   if(Pitem->m_nID == -1)
				SQL = CTrainingTakeOffPosition::GetInsertSQL(Pitem,_parID);
		   else
				SQL = CTrainingTakeOffPosition::GetUpdateSQL(Pitem,_parID);
		   int _id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL);
		   if(Pitem->GetID() == -1)
				Pitem->SetID(_id);
		   CTrainingTakeOffPosition::WriteData(Pitem->m_runwayIdList,Pitem->GetID());
        }	

		CADODatabase::CommitTransaction();
	}catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		return;
	}
}

void CTrainingTakeOffPosition::WriteData( std::vector<int>&_data,int _id )
{
	CTrainingTakeOffPosition::DeleteDate(_id);
	CString SQL;
	try
	{
		CADODatabase::BeginTransaction();
		for (int i = 0; i < (int)_data.size(); i++)
		{
			SQL = CTrainingTakeOffPosition::GetInsertSQL(_data.at(i),_id);
			CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL);
		}

		CADODatabase::CommitTransaction();
	}catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		return;
	}
}

void CTrainingTakeOffPosition::ReadData( std::vector<int>&_data,int _id )
{
	CString SQL;
	SQL = CTrainingTakeOffPosition::GetSelectSQL(_id,TB_POSITIONLIST);
	long count =0;
	CADORecordset dataSet;
	try{
		CADODatabase::ExecuteSQLStatement(SQL,count,dataSet);
	}catch(CADOException& e)
	{
		e.ErrorMessage();
		return;
	}
	int ID;
	while(!dataSet.IsEOF())
	{	
		dataSet.GetFieldValue(_T("RUNWAY_ID"),ID);
		_data.push_back(ID);
		dataSet.MoveNextData();
	}
}
void CTrainingTakeOffPosition::DeleteDate(std::vector<CTrainingTakeOffPosition*>&_data)
{
	CTrainingTakeOffPosition* Pitem = NULL;
	for (int i = 0; i < (int)_data.size(); i++)
	{
		Pitem = _data[i];
		CString SQL;
		SQL = CTrainingTakeOffPosition::GetDeleteSQL(Pitem->GetID(),TB_POSITION);
		try
		{
			CADODatabase::BeginTransaction();
			CADODatabase::ExecuteSQLStatement(SQL);
			CADODatabase::CommitTransaction();
		}
		catch (CADOException& e)
		{
			e.ErrorMessage();
			CADODatabase::RollBackTransation();
			return;
		}
	}
}

void CTrainingTakeOffPosition::DeleteDate( int _id )
{
	CString SQL;
	SQL = CTrainingTakeOffPosition::GetDeleteSQL(_id,TB_POSITIONLIST);
	try
	{
		CADODatabase::BeginTransaction();
		CADODatabase::ExecuteSQLStatement(SQL);
		CADODatabase::CommitTransaction();
	}catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		return;
	}
}
CString CTrainingTakeOffPosition::GetSelectSQL(int _parID,PosOrList _Type)
{
	CString SQL;
	if (_Type == TB_POSITION)
	{
		SQL.Format(_T("SELECT * FROM TB_TRAINFLIGHT_POSITION WHERE PAR_ID = %d"),_parID);
	}
	else
	{
		SQL.Format(_T("SELECT * FROM TB_TRAINFLIGHT_POSITIONLIST WHERE PAR_ID = %d"),_parID);
	}
	return SQL;
}

CString CTrainingTakeOffPosition::GetInsertSQL( CTrainingTakeOffPosition* _item ,int _parID)
{
	CString SQL;
	SQL.Format(_T("INSERT INTO TB_TRAINFLIGHT_POSITION (APPROACHSEQUENCE,APPROACHPAX,TAKEOFFSEQUENCE,TAKEOFFPAX,PAR_ID) VALUES(%d,%d,%d,%d,%d)"),
	_item->m_ApproachSequence,
	_item->m_ApproachPax,
	_item->m_TakeOffSequence,
	_item->m_TakeOffPax,
	_parID);
	return SQL;
}

CString CTrainingTakeOffPosition::GetInsertSQL( int _id,int _parID )
{
	CString SQL;
	SQL.Format(_T("INSERT INTO TB_TRAINFLIGHT_POSITIONLIST(PAR_ID,RUNWAY_ID) VALUES(%d,%d)"),_parID,_id);
	return SQL;
}
CString CTrainingTakeOffPosition::GetUpdateSQL( CTrainingTakeOffPosition* _item ,int _parID)
{
	   CString SQL ;
	if(_item == NULL || _parID == -1)
		return SQL;	
	SQL.Format(_T("UPDATE TB_TRAINFLIGHT_POSITION SET APPROACHSEQUENCE = %d,APPROACHPAX =%d,TAKEOFFSEQUENCE =%d,TAKEOFFPAX =%d,PAR_ID =%d WHERE ID = %d"),
		_item->m_ApproachSequence,
		_item->m_ApproachPax,
		_item->m_TakeOffSequence,
		_item->m_TakeOffPax,
		_parID,
		_item->m_nID);
	return SQL;
}

CString CTrainingTakeOffPosition::GetDeleteSQL( int _id,PosOrList _type)
{
	CString SQL;
	if (_type == TB_POSITION)
	{
		SQL.Format(_T("DELETE * FROM TB_TRAINFLIGHT_POSITION WHERE ID = %d"),_id);
	}
	else
	{
		SQL.Format(_T("DELETE * FROM TB_TRAINFLIGHT_POSITIONLIST WHERE PAR_ID = %d"),_id);
	}
	return SQL;
}

void CTrainingTakeOffPosition::RemoveData()
{
	m_runwayIdList.clear();
}

CTrainingTakeOffPosition::~CTrainingTakeOffPosition()
{
	RemoveData();
}