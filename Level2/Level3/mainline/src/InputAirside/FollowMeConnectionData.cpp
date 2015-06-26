#include "StdAfx.h"

#include ".\followmeconnectiondata.h"
#include "..\Common\AirportDatabase.h"

void CFollowMeConnectionStand::ReadMeetingPoint(CFollowMeConnectionStand* PStand )
{
	PStand->m_MeetPointData.clear() ;
	CString SQL ;

	SQL.Format(_T("SELECT * FROM TB_FOLLOWMECONNECTION_MEETINGPOINT WHERE PARENT_ID = %d"),PStand->GetID()) ;
	CADORecordset _dataset ;
	long count = 0 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,_dataset) ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}
	int _id = -1 ;
	CString strval ;
	while(!_dataset.IsEOF())
	{
		_dataset.GetFieldValue(_T("MEETINGPOINT_ID"),strval) ;

		PStand->m_MeetPointData.push_back(strval) ;

		_dataset.MoveNextData() ;
	}
}
void CFollowMeConnectionStand::WriteMeetingPoint(CFollowMeConnectionStand* pStand)
{
	CString SQL ;

	SQL.Format(_T("DELETE * FROM TB_FOLLOWMECONNECTION_MEETINGPOINT WHERE PARENT_ID = %d"),pStand->GetID()) ;

	try
	{
		CADODatabase::BeginTransaction();
		CADODatabase::ExecuteSQLStatement(SQL) ;

		for(int i = 0 ; i < (int)(pStand->m_MeetPointData.size()) ;i++)
		{
			SQL.Format(_T("INSERT INTO TB_FOLLOWMECONNECTION_MEETINGPOINT (MEETINGPOINT_ID,PARENT_ID) VALUES('%s',%d)"),
				pStand->m_MeetPointData[i],
				pStand->GetID()) ;

			CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
		}
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		CADODatabase::RollBackTransation() ;
		return ;
	}

}
CString CFollowMeConnectionStand::GetSelectSQL(int _parID)
{
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_FOLLOWMECONNECTION_STANDITEM WHERE PARENT_ID = %d"),_parID) ;
	return SQL ;

}
CString CFollowMeConnectionStand::GetInsertSQL(CFollowMeConnectionStand* _stand ,int _parID)
{
	CString SQL ;
	if(_stand == NULL || _stand->GetID() != -1 || _parID == -1)
		return SQL ;
	SQL.Format(_T("INSERT INTO TB_FOLLOWMECONNECTION_STANDITEM (STAND_ID,ABANDONMENT_NAME,ABANDONMENT_DIS,ABANDONMENT_SELECT,ABANDONMENT_TYPE,PARENT_ID) \
				  VALUES('%s','%s',%0.2f,%d,%d,%d)"),
				  _stand->m_StandInSim,
				  _stand->m_AbandonmentPoint.m_Name,
				  _stand->m_AbandonmentPoint.m_Dis,
				  _stand->m_AbandonmentPoint.m_SelType,
				  _stand->m_AbandonmentPoint.m_PointType,
				  _parID) ;
	return SQL ;
}
CString CFollowMeConnectionStand::GetUpdateSQL( CFollowMeConnectionStand* _stand ,int _parID )
{
	CString SQL ;
	if(_stand == NULL || _parID == -1)
		return SQL ;
	SQL.Format(_T("UPDATE TB_FOLLOWMECONNECTION_STANDITEM SET STAND_ID = '%s' ,ABANDONMENT_NAME = '%s' ,ABANDONMENT_DIS = %0.2f ,ABANDONMENT_SELECT = %d,\
				  ABANDONMENT_TYPE = %d ,PARENT_ID = %d WHERE ID = %d"),
				  _stand->m_StandInSim,
				  _stand->m_AbandonmentPoint.m_Name,
				  _stand->m_AbandonmentPoint.m_Dis,
				  _stand->m_AbandonmentPoint.m_SelType,
				  _stand->m_AbandonmentPoint.m_PointType,
				  _parID,
				  _stand->GetID()); 
	return SQL ;
}
CString CFollowMeConnectionStand::GetDeleteSQL(CFollowMeConnectionStand* _stand )
{
	CString SQL ;

	SQL.Format(_T("DELETE * FROM TB_FOLLOWMECONNECTION_STANDITEM WHERE ID = %d"),_stand->GetID()) ;

	return SQL ;
}

void CFollowMeConnectionStand::ReadData(std::vector<CFollowMeConnectionStand*>& _data ,int _ParID)
{
	if(_ParID == -1)
		return ;
	CString SQL ;
	long count = 0;
	CADORecordset _dataSet ;

	SQL = CFollowMeConnectionStand::GetSelectSQL(_ParID) ;

	try{
		CADODatabase::ExecuteSQLStatement(SQL,count,_dataSet) ;
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CFollowMeConnectionStand* PItemStand = NULL ;
	int _id = 0 ;
	CString StrName ;
	double _disValues = 0 ;
	int _val = 0 ;
	while(!_dataSet.IsEOF())
	{
		PItemStand = new CFollowMeConnectionStand ;
		_dataSet.GetFieldValue(_T("ID"),_id) ;
		PItemStand->SetID(_id) ;

		_dataSet.GetFieldValue(_T("STAND_ID"),StrName) ;
		PItemStand->m_StandInSim = StrName ;

		_dataSet.GetFieldValue(_T("ABANDONMENT_NAME"),StrName) ;
		PItemStand->m_AbandonmentPoint.m_Name = StrName ;

		_dataSet.GetFieldValue(_T("ABANDONMENT_DIS"),_disValues) ;
		PItemStand->m_AbandonmentPoint.m_Dis = _disValues ;

		_dataSet.GetFieldValue(_T("ABANDONMENT_SELECT"),_val) ;
		PItemStand->m_AbandonmentPoint.m_SelType = _val ;

		_dataSet.GetFieldValue(_T("ABANDONMENT_TYPE"),_val) ;
		PItemStand->m_AbandonmentPoint.m_PointType = (CAbandonmentPoint::Point_Type)_val ;

		CFollowMeConnectionStand::ReadMeetingPoint(PItemStand) ;
		_data.push_back(PItemStand) ;

		_dataSet.MoveNextData() ;
	}
}

void CFollowMeConnectionStand::WriteData(std::vector<CFollowMeConnectionStand*>& _data ,int _ParID)
{
	if(_ParID == -1)
		return ;
	CString SQL ;
	CFollowMeConnectionStand* PItemStand = NULL ;
	for (int i = 0 ; i < (int)_data.size() ;i++)
	{
		PItemStand = _data[i] ;
		if(PItemStand->GetID() == -1)
			SQL = CFollowMeConnectionStand::GetInsertSQL(PItemStand,_ParID) ;
		else
			SQL = CFollowMeConnectionStand::GetUpdateSQL(PItemStand,_ParID) ;

		try
		{
			CADODatabase::BeginTransaction() ;
			int id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
			if(PItemStand->GetID() == -1)
				PItemStand->SetID(id) ;
			CFollowMeConnectionStand::WriteMeetingPoint(PItemStand) ;
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
void CFollowMeConnectionStand::DeleteData(std::vector<CFollowMeConnectionStand*>& _data)
{
	CString SQL ;
	CFollowMeConnectionStand* PItemStand = NULL ;
	for (int i = 0 ; i < (int)_data.size() ;i++)
	{
		PItemStand = _data[i] ;

		SQL = GetDeleteSQL(PItemStand) ;

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
void CFollowMeConnectionStand::AddMeetingPoint(const CString& _point)
{
	std::vector<CString>::iterator iter = m_MeetPointData.begin() ;
	for (; iter != m_MeetPointData.end() ;iter++)
	{
		if( _point.CompareNoCase(*iter) == 0 )
			return ;
	}

	m_MeetPointData.push_back(_point) ;
}
void CFollowMeConnectionStand::RemoveMeetingPoint(CString _PointID)
{
	std::vector<CString>::iterator iter = m_MeetPointData.begin() ;
	for ( ; iter != m_MeetPointData.end() ;iter++)
	{
		if( _PointID.CompareNoCase((*iter)) == 0 )
		{
			m_MeetPointData.erase(iter) ;
			break ;
		}
	}
}

CAbandonmentPoint* CFollowMeConnectionStand::GetAbandonmentPoint()
{
	return & m_AbandonmentPoint ;
}

CFollowMeConnectionStand::CFollowMeConnectionStand() 
:m_ID(-1)
{

}

CFollowMeConnectionStand::~CFollowMeConnectionStand()
{

}
//////////////////////////////////////////////////////////////////////////


void CFollowMeConnectionRunway::ClearAll()
{
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		delete m_Data[i] ;
	}
	m_Data.clear() ;
	ClearDelData() ;
}
void CFollowMeConnectionRunway::ClearDelData()
{
	for (int i = 0 ; i < (int)m_DeleteData.size() ;i++)
	{
		delete m_DeleteData[i] ;
	}
	m_DeleteData.clear() ;
}
CString CFollowMeConnectionRunway::GetSelectSQL(int _parID)
{
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_FOLLOWMECONNECTION_RUNWAY WHERE PARENTID = %d"),_parID) ;
	return SQL ;
}
CString CFollowMeConnectionRunway::GetInsertSQL(CFollowMeConnectionRunway* _item , int _parID)
{
	CString SQL ;
	if(_item == NULL || _parID == -1 || _item->GetID() != -1)
		return SQL;

	SQL.Format(_T("INSERT INTO TB_FOLLOWMECONNECTION_RUNWAY (RUNWAYNAME_ID , RUNWAY_ID,RUNWAY_MARKETID,PARENTID) VALUES(%d,'%s',%d,%d)"),_item->GetRunwayID(),_item->GetRunwayName(),_item->GetRunwayMarkerID(),_parID) ;
	return SQL ;
}
CString CFollowMeConnectionRunway::GetUpdateSQL(CFollowMeConnectionRunway* _item , int _parID)
{
		CString SQL ;
	if(_item == NULL || _parID == -1 || _item->GetID() == -1)
		return SQL;
	SQL.Format(_T("UPDATE TB_FOLLOWMECONNECTION_RUNWAY SET RUNWAYNAME_ID =  %d ,RUNWAY_ID = '%s' ,RUNWAY_MARKETID = %d ,PARENTID = %d WHERE ID = %d"),
		_item->GetRunwayID(),
		_item->GetRunwayName(),
		_item->GetRunwayMarkerID(),
		_parID,
		_item->GetID()) ;
	return SQL ;
}
CString CFollowMeConnectionRunway::GetDeleteSQL(CFollowMeConnectionRunway* _item)
{
	CString SQL ;
	if(_item)
	{
		SQL.Format(_T("DELETE * FROM TB_FOLLOWMECONNECTION_RUNWAY WHERE ID = %d") ,_item->GetID()) ;
	}
	return SQL ;
}
void CFollowMeConnectionRunway::ReadData(std::vector<CFollowMeConnectionRunway*>& _data ,int _ParID)
{
	if(_ParID == -1)
		return ;
	CString SQL ;
	SQL = CFollowMeConnectionRunway::GetSelectSQL(_ParID) ;

	long count = 0 ;
	CADORecordset _dataSet ;

	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,_dataSet) ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CFollowMeConnectionRunway* _PItemData = NULL ;
	int _id = -1 ;
	CString str ;
	int RunwayID = -1 ;
	while(!_dataSet.IsEOF())
	{
		_PItemData = new CFollowMeConnectionRunway ;

		_dataSet.GetFieldValue(_T("ID"),_id) ;
		_PItemData->SetID(_id) ;

		_dataSet.GetFieldValue(_T("RUNWAYNAME_ID"),RunwayID) ;
		_PItemData->SetRunwayID(RunwayID) ;

		_dataSet.GetFieldValue(_T("RUNWAY_MARKETID"),RunwayID) ;
		_PItemData->SetRunwayMarketID(RunwayID) ;
		
		_dataSet.GetFieldValue(_T("RUNWAY_ID"),str) ;
		_PItemData->SetRunwayName(str) ;
		CFollowMeConnectionStand::ReadData(_PItemData->m_Data,_PItemData->GetID()) ;

		_data.push_back(_PItemData) ;

		_dataSet.MoveNextData() ;
	}
}
void CFollowMeConnectionRunway::WriteData(std::vector<CFollowMeConnectionRunway*>& _data ,int _ParID)
{
	if(_ParID == -1)
		return ;
	CFollowMeConnectionRunway* runwayItem = NULL ;
	CString SQL ;
	for (int i = 0 ; i < (int)_data.size() ;i++)
	{
		runwayItem = _data[i] ;
		if(runwayItem->GetID() == -1)
			SQL = CFollowMeConnectionRunway::GetInsertSQL(runwayItem,_ParID) ;
		else
			SQL = CFollowMeConnectionRunway::GetUpdateSQL(runwayItem,_ParID) ;

		try
		{
			CADODatabase::BeginTransaction() ;
			int _id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
			if(runwayItem->GetID() == -1)
				runwayItem->SetID(_id) ;
			CFollowMeConnectionStand::WriteData(runwayItem->m_Data,runwayItem->GetID()) ;
			CFollowMeConnectionStand::DeleteData(runwayItem->m_DeleteData) ;
			runwayItem->ClearDelData() ;
			CADODatabase::CommitTransaction() ;
		}
		catch (CADOException& e)
		{
			e.ErrorMessage() ;
			CADODatabase::RollBackTransation( ) ;
			return ;
		}
	}
}
void CFollowMeConnectionRunway::DeleteData(std::vector<CFollowMeConnectionRunway*>& _data)
{
	CFollowMeConnectionRunway* runwayItem = NULL ;
	CString SQL ;
	for (int i = 0 ;i < (int)_data.size() ;i++)
	{
		runwayItem = _data[i] ;
		SQL = GetDeleteSQL(runwayItem) ;

		try
		{
			CADODatabase::BeginTransaction() ;
			CADODatabase::ExecuteSQLStatement(SQL) ;
			CADODatabase::CommitTransaction( ) ;
		}
		catch (CADOException& e)
		{
			e.ErrorMessage() ;
			CADODatabase::RollBackTransation( ) ;
			return ;
		}

	}
}

CFollowMeConnectionStand* CFollowMeConnectionRunway::AddStandItem( const CString& _stand )
{

	CFollowMeConnectionStand* StandItem = NULL ;
	for (int i = 0 ;i < (int)m_Data.size() ;i++)
	{
		StandItem = m_Data[i] ;
		if(StandItem->m_StandInSim.CompareNoCase(_stand) == 0)
			return NULL ;
	}
	StandItem = new CFollowMeConnectionStand ;
	StandItem->m_StandInSim = _stand ;
	m_Data.push_back(StandItem) ;
	return StandItem ;
}

void CFollowMeConnectionRunway::RemoveStandItem( CFollowMeConnectionStand* _dataItem )
{
	std::vector<CFollowMeConnectionStand*>::iterator iter = std::find(m_Data.begin(),m_Data.end(),_dataItem) ;

	if(iter != m_Data.end() )
	{
		m_DeleteData.push_back(*iter) ;
		m_Data.erase(iter) ;
	}
}

CFollowMeConnectionRunway::CFollowMeConnectionRunway() :m_ID(-1),m_RunwayID(-1),m_RunwayMarketID(-1)
{

}

CFollowMeConnectionRunway::~CFollowMeConnectionRunway()
{
	ClearAll() ;
}

int CFollowMeConnectionRunway::GetRwyID()
{
	return m_RunwayID;
}

int CFollowMeConnectionRunway::GetRwyMark()
{
	return m_RunwayMarketID;
}
////////////////////////////////////////////////////////////////////////////////////////


CFollowMeConnectionTimeRangeItem::CFollowMeConnectionTimeRangeItem():m_ID(-1)
{

}
CFollowMeConnectionTimeRangeItem::~CFollowMeConnectionTimeRangeItem()
{
	ClearAll() ;
}
CString CFollowMeConnectionTimeRangeItem::GetSelectSQL(int _parID)
{
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_FOLLOWMECONNECTION_RANAGEITEM WHERE PARENT_ID = %d"),_parID) ;
	return SQL ;
}
CString CFollowMeConnectionTimeRangeItem::GetInsertSQL(CFollowMeConnectionTimeRangeItem* _item ,int _parID)
{
	CString _SQL ;

	if(_item && _item->GetID() == -1)
	{
		_SQL.Format(_T("INSERT INTO TB_FOLLOWMECONNECTION_RANAGEITEM (START_TIME,END_TIME,PARENT_ID) VALUES(%d,%d,%d)"),\
			_item->GetStartTime().asSeconds(),
			_item->GetEndTime().asSeconds(),
			_parID) ;
	}
	return _SQL ;
}
CString CFollowMeConnectionTimeRangeItem::GetUpdateSQL(CFollowMeConnectionTimeRangeItem* _item ,int _parID)
{
	CString _sql ;

	if(_item && _item->GetID() != -1)
	{
		_sql.Format(_T("UPDATE TB_FOLLOWMECONNECTION_RANAGEITEM SET START_TIME = %d ,END_TIME = %d , PARENT_ID = %d WHERE ID = %d"), \
			_item->GetStartTime().asSeconds(),
			_item->GetEndTime().asSeconds(),
			_parID,
			_item->GetID()) ;
	}
	return _sql ;
}
CString CFollowMeConnectionTimeRangeItem::GetDeleteSQL(CFollowMeConnectionTimeRangeItem* _item)
{
	CString SQL ;

	if(_item && _item->GetID() != -1)
		SQL.Format(_T("DELETE * FROM TB_FOLLOWMECONNECTION_RANAGEITEM WHERE ID = %d") ,_item->GetID()) ;
	return SQL ;
}
void CFollowMeConnectionTimeRangeItem::ClearAll()
{
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		delete m_Data[i] ;
	}
	m_Data.clear() ;

	ClearDelData() ;
}
void CFollowMeConnectionTimeRangeItem::ClearDelData()
{
	for (int i = 0 ; i < (int)m_DelData.size() ;i++)
	{
		delete m_DelData[i] ;
	}
	m_DelData.clear() ;
}
void CFollowMeConnectionTimeRangeItem::ReadData(std::vector<CFollowMeConnectionTimeRangeItem*>& _data ,int _parID)
{
	CString SQL ;
	SQL = CFollowMeConnectionTimeRangeItem::GetSelectSQL(_parID) ;
	long count = 0 ;
	CADORecordset _DataSet ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,_DataSet) ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CFollowMeConnectionTimeRangeItem* _item = NULL ;
	int _id = -1 ;
	int _Time = 0 ;
	while(!_DataSet.IsEOF())
	{
		_item = new CFollowMeConnectionTimeRangeItem ;

		_DataSet.GetFieldValue(_T("ID"),_id) ;
		_item->SetID(_id) ;

		_DataSet.GetFieldValue(_T("START_TIME"),_Time) ;
		ElapsedTime startTime ;
		startTime.SetSecond(_Time) ;
		_item->SetStartTime(startTime) ;

		_DataSet.GetFieldValue(_T("END_TIME"),_Time) ;
		ElapsedTime EndTime ;
		EndTime.SetSecond(_Time) ;
		_item->SetEndTime(EndTime) ;

		_data.push_back(_item) ;

		CFollowMeConnectionRunway::ReadData(_item->m_Data,_item->GetID()) ;

		_DataSet.MoveNextData() ;
	}
}
void CFollowMeConnectionTimeRangeItem::WriteData(std::vector<CFollowMeConnectionTimeRangeItem*>& _data ,int _parID)
{
	CString SQL ;
	CFollowMeConnectionTimeRangeItem* _item = NULL ;
	for (int i = 0 ; i < (int)_data.size() ;i++)
	{
		_item = _data[i] ;

		if(_item->GetID() == -1)
			SQL = CFollowMeConnectionTimeRangeItem::GetInsertSQL(_item,_parID) ;
		else
			SQL = CFollowMeConnectionTimeRangeItem::GetUpdateSQL(_item,_parID) ;


		try
		{
			CADODatabase::BeginTransaction() ;

			int id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
			if(_item->GetID() == -1)
				_item->SetID(id) ;

			CFollowMeConnectionRunway::WriteData(_item->m_Data,_item->GetID()) ;
			CFollowMeConnectionRunway::DeleteData(_item->m_DelData) ;
			_item->ClearDelData() ;
			CADODatabase::CommitTransaction() ;
		}
		catch (CADOException& e)
		{
			e.ErrorMessage() ;
			CADODatabase::RollBackTransation( ) ;
			return ;
		}
	}
}
void CFollowMeConnectionTimeRangeItem::DeleteData(std::vector<CFollowMeConnectionTimeRangeItem*>& _data)
{
	CString SQL ;
	CFollowMeConnectionTimeRangeItem* _item = NULL ;
	for (int i = 0 ; i < (int)_data.size() ;i++)
	{
		_item = _data[i] ;

		SQL = GetDeleteSQL(_item) ;

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

CFollowMeConnectionRunway* CFollowMeConnectionTimeRangeItem::AddRunwayItem( const CString& _runway ,int _runwayID ,int _runwayMarkID)
{
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		if(m_Data[i]->GetRunwayID() == _runwayID && m_Data[i]->GetRunwayMarkerID() == _runwayMarkID)
			return NULL ;
	}
	CFollowMeConnectionRunway* PRunwayItem = new CFollowMeConnectionRunway ;
	PRunwayItem->SetRunwayID(_runwayID) ;
	PRunwayItem->SetRunwayMarketID(_runwayMarkID) ;
	PRunwayItem->SetRunwayName(_runway) ;
	m_Data.push_back(PRunwayItem) ;
	return PRunwayItem ;
}

void CFollowMeConnectionTimeRangeItem::RemoveRunwayItem( CFollowMeConnectionRunway* _runway )
{
	std::vector<CFollowMeConnectionRunway*>::iterator iter = m_Data.begin() ;

	iter = std::find(m_Data.begin(),m_Data.end(),_runway) ;

	if(iter != m_Data.end())
	{
		m_DelData.push_back(*iter) ;
		m_Data.erase(iter) ;
	}
}

ElapsedTime CFollowMeConnectionTimeRangeItem::GetStartTime()
{
	return m_startTime ;
}

ElapsedTime CFollowMeConnectionTimeRangeItem::GetEndTime()
{
	return m_EndTime ;
}
//////////////////////////////////////////////////////////////////////////



CString CFollowMeConnectionItem::GetSelectSQL()
{
	CString _SQL ;
	_SQL.Format(_T("%s"),_T("SELECT * FROM TB_FOLLOWMECONNECTION_ITEM"));
	return _SQL ; 
}
CString CFollowMeConnectionItem::GetInsertSQL(CFollowMeConnectionItem* _item)
{
	CString _SQL ;
	TCHAR _strFlightTY[1024] = {0} ;
	_item->GetFlightConstraint().WriteSyntaxStringWithVersion(_strFlightTY) ;
	_SQL.Format(_T("INSERT INTO TB_FOLLOWMECONNECTION_ITEM (FLIGHT_TYPE) VALUES('%s')"),_strFlightTY) ;
	return _SQL ;
}
CString CFollowMeConnectionItem::GetUpdateSQL(CFollowMeConnectionItem* _item)
{
	CString _SQL ;
	TCHAR _strFlightTY[1024] = {0} ;
	_item->GetFlightConstraint().WriteSyntaxStringWithVersion(_strFlightTY) ;
	if(_item->GetID() != -1)
	{
		_SQL.Format(_T("UPDATE TB_FOLLOWMECONNECTION_ITEM SET FLIGHT_TYPE = '%s' WHERE ID = %d"),_strFlightTY,_item->GetID()) ;
	}
	return _SQL ;
}
CString CFollowMeConnectionItem::GetDeleteSQL(CFollowMeConnectionItem* _item)
{
	CString _SQL ;
	if(_item->GetID() != -1)
		_SQL.Format(_T("DELETE * FROM TB_FOLLOWMECONNECTION_ITEM WHERE ID = %d"),_item->GetID()) ;
	return _SQL ;
}
void CFollowMeConnectionItem::ReadData(std::vector<CFollowMeConnectionItem*>& _data ,int _ParID,CAirportDatabase* _AirportDB)
{
	CString _SQL ;
	_SQL = CFollowMeConnectionItem::GetSelectSQL() ;
	long count = 0 ;
	CADORecordset _dataSet ;
	try
	{
		CADODatabase::ExecuteSQLStatement(_SQL,count,_dataSet) ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CFollowMeConnectionItem* _PFollowMeConnectItem = NULL ;
	int _id = -1 ;
	CString StrFlight ;
	while(!_dataSet.IsEOF())
	{
		_PFollowMeConnectItem = new CFollowMeConnectionItem ;
		_dataSet.GetFieldValue(_T("ID"),_id) ;
		_dataSet.GetFieldValue(_T("FLIGHT_TYPE"),StrFlight) ;

		_PFollowMeConnectItem->SetID(_id) ;
		_PFollowMeConnectItem->m_FlightConstraint.SetAirportDB(_AirportDB) ;
		_PFollowMeConnectItem->m_FlightConstraint.setConstraintWithVersion(StrFlight) ;
		CFollowMeConnectionTimeRangeItem::ReadData(*(_PFollowMeConnectItem->GetData()),_id) ;
		_data.push_back(_PFollowMeConnectItem) ;
		_dataSet.MoveNextData() ;
	}

}
void CFollowMeConnectionItem::WriteData(std::vector<CFollowMeConnectionItem*>& _data , int _parID)
{
	CFollowMeConnectionItem* Item = NULL ;
	CString _SQL ;
	for (int i = 0 ; i < (int)_data.size() ;i++)
	{
		Item = _data[i] ;
		if(Item->GetID() == -1)
			_SQL = CFollowMeConnectionItem::GetInsertSQL(Item) ;
		else
			_SQL = CFollowMeConnectionItem::GetUpdateSQL(Item) ;

		try
		{
			CADODatabase::BeginTransaction() ;
			int id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(_SQL) ;
			if(Item->GetID() == -1)
				Item->SetID(id) ;

			CFollowMeConnectionTimeRangeItem::WriteData(*(Item->GetData()),Item->GetID()) ;
			CFollowMeConnectionTimeRangeItem::DeleteData(*(Item->GetDelData())) ;
			Item->ClearDeleteAll() ;
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
void CFollowMeConnectionItem::DeleteData(std::vector<CFollowMeConnectionItem*>& _data)
{
	CFollowMeConnectionItem* Item = NULL ;
	CString _SQL ;
	for (int i = 0 ; i < (int)_data.size() ;i++)
	{
		Item = _data[i] ;
		_SQL = GetDeleteSQL(Item) ;

		try
		{
			CADODatabase::BeginTransaction() ;
			CADODatabase::ExecuteSQLStatement(_SQL) ;
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
void CFollowMeConnectionItem::ClearAll()
{
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		delete m_Data[i] ;
	}
	m_Data.clear() ;

	ClearDeleteAll() ;
}
void CFollowMeConnectionItem::ClearDeleteAll()
{
	for (int i = 0 ; i < (int)m_DelData.size() ;i++)
	{
		delete m_DelData[i] ;
	}
	m_DelData.clear() ;
}

void CFollowMeConnectionItem::RemoveRangeItem( CFollowMeConnectionTimeRangeItem* _data )
{
	std::vector<CFollowMeConnectionTimeRangeItem*>::iterator iter = m_Data.begin() ;

	iter = std::find(m_Data.begin(),m_Data.end(),_data) ;

	if(iter != m_Data.end())
	{
		m_DelData.push_back(*iter) ;
		m_Data.erase(iter) ;
	}
}

CFollowMeConnectionTimeRangeItem* CFollowMeConnectionItem::AddRangeItem( const ElapsedTime& _startTime ,const ElapsedTime& _endTime )
{
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		if(m_Data[i]->GetStartTime() == _startTime && m_Data[i]->GetEndTime() == _endTime)
			return NULL ;
	}	
	CFollowMeConnectionTimeRangeItem* rangeItem = new CFollowMeConnectionTimeRangeItem ;
	rangeItem->SetStartTime(_startTime) ;
	rangeItem->SetEndTime(_endTime) ;
	m_Data.push_back(rangeItem) ;
	return rangeItem ;
}

const FlightConstraint& CFollowMeConnectionItem::GetFlightConstraint()
{
	return m_FlightConstraint ;
}

void CFollowMeConnectionItem::SetFlightConstraint( const FlightConstraint& _fltCon )
{
	m_FlightConstraint = _fltCon ;
}

std::vector<CFollowMeConnectionTimeRangeItem*>* CFollowMeConnectionItem::GetData()
{
	return &m_Data ;
}
//////////////////////////////////////////////////////////////////////////


CFollowMeConnectionItem* CFollowMeConnectionData::AddNewItem(FlightConstraint& _fltCon)
{
	CFollowMeConnectionItem* PFindItem = FindItem(_fltCon) ;
	if(PFindItem == NULL)
	{
		PFindItem = new CFollowMeConnectionItem ;
		PFindItem->SetFlightConstraint(_fltCon)  ;
		m_Data.push_back(PFindItem) ;
	}
	return PFindItem ;
}
CFollowMeConnectionItem* CFollowMeConnectionData::FindItem(FlightConstraint& _fltCon)
{
	CFollowMeConnectionItem* item = NULL ;
	std::vector<CFollowMeConnectionItem*>::iterator iter = m_Data.begin() ;
	for (; iter !=  m_Data.end() ;iter++)
	{
		item =*iter ;
		if(item->GetFlightConstraint().isEqual(&_fltCon))
			return item ;
	}
	return NULL ;
}

void CFollowMeConnectionData::DeleteItem(CFollowMeConnectionItem* _Pitem)
{
	CFollowMeConnectionItem* item = NULL ;
	std::vector<CFollowMeConnectionItem*>::iterator iter = m_Data.begin() ;
	for (; iter !=  m_Data.end() ;iter++)
	{
		item =*iter ;
		if(item == _Pitem)
		{
			m_DeleteData.push_back(item) ;
			m_Data.erase(iter) ;
			break ;
		}
	}
}
void CFollowMeConnectionData::DeleteItem(FlightConstraint& _fltCon)
{
	CFollowMeConnectionItem* item = NULL ;
	std::vector<CFollowMeConnectionItem*>::iterator iter = m_Data.begin() ;
	for (; iter !=  m_Data.end() ;iter++)
	{
		item =*iter ;
		if(item->GetFlightConstraint().isEqual(&_fltCon))
		{
			m_DeleteData.push_back(item) ;
			m_Data.erase(iter) ;
			break ;
		}
	}
}
void CFollowMeConnectionData::ClearAll()
{
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		delete m_Data[i] ;
	}
	m_Data.clear() ;

	for (int i = 0 ; i < (int)m_DeleteData.size() ;i++)
	{
		delete m_DeleteData[i] ;
	}
	m_DeleteData.clear() ;
}
void CFollowMeConnectionData::ReadData()
{
	ClearAll() ;
	CFollowMeConnectionItem::ReadData(m_Data,-1,m_AirportDB) ;
}
void CFollowMeConnectionData::WriteData()
{
	CFollowMeConnectionItem::WriteData(m_Data,-1) ;
	CFollowMeConnectionItem::DeleteData(m_DeleteData) ;

	for (int i = 0 ; i < (int)m_DeleteData.size() ;i++)
	{
		delete m_DeleteData[i] ;
	}
	m_DeleteData.clear() ;
}

const std::vector<CFollowMeConnectionItem*>& CFollowMeConnectionData::GetData()
{
	return m_Data ;
}

CFollowMeConnectionData::CFollowMeConnectionData( CAirportDatabase* _dataBase ) :m_AirportDB(_dataBase)
{

}

CFollowMeConnectionData::~CFollowMeConnectionData( void )
{
	ClearAll() ;
}
//////////////////////////////////////////////////////////////////////////

CAbandonmentPoint::CAbandonmentPoint() :m_ID(-1),m_Dis(0),m_SelType(0),m_PointType(TY_Stand)
{

}
