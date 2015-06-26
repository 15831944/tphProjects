#include "stdafx.h"
#include ".\SurfacesInitalEvent.h"
#include "../Database/ADODatabase.h"

CConditionItem::CConditionItem():m_ID(-1)
{

}
CConditionItem::~CConditionItem()
{

}
void CConditionItem::ReadData(std::vector<CConditionItem*>* _datas ,int _eventID,CAirportDatabase* _database)
{
	if(_eventID == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("SELECT * FROM AS_Initial_condition WHERE EVENT_ID = %d"),_eventID) ;
	CADORecordset dataset ;
	long count = 0 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,dataset) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CConditionItem* _conditionItem = NULL ;
	CString flightty ;
	CString condition ;
	int id = -1 ;
	while (!dataset.IsEOF())
	{
		_conditionItem = new CConditionItem ;
		dataset.GetFieldValue(_T("FLIGHT_TYPE"),flightty) ;
		_conditionItem->GetFlightType()->SetAirportDB(_database) ;
		_conditionItem->GetFlightType()->setConstraintWithVersion(flightty) ;

		dataset.GetFieldValue(_T("CONDITION"),condition) ;
		_conditionItem->SetCondition(condition) ;

		dataset.GetFieldValue(_T("ID"),id) ;
		_conditionItem->SetID(id) ;

		_datas->push_back(_conditionItem) ;
		dataset.MoveNextData() ;
	}
}
void CConditionItem::WriteData(std::vector<CConditionItem*>* _Datas , int _eventID)
{
	if(_eventID == -1)
		return ;
	std::vector<CConditionItem*> ::iterator iter = _Datas->begin() ;
	for ( ; iter != _Datas->end() ;iter++)
	{
		CConditionItem::WriteData(*iter,_eventID) ;
	}
}
void CConditionItem::DelData(std::vector<CConditionItem*>* _Datas)
{
	std::vector<CConditionItem*> ::iterator iter = _Datas->begin() ;
	for ( ; iter != _Datas->end() ;iter++)
	{
		CConditionItem::DelData(*iter) ;
	}
}
void CConditionItem::WriteData(CConditionItem* _item,int _eventID)
{
	if(_item == NULL || _eventID == -1)
		return ;
	CString SQL ;
	int id = -1 ;
	TCHAR flightTy[1024] = {0} ;
	_item->GetFlightType()->WriteSyntaxStringWithVersion(flightTy) ;
	try
	{
		if(_item->GetID() == -1)
		{
			SQL.Format(_T("INSERT INTO AS_Initial_condition (FLIGHT_TYPE,CONDITION,EVENT_ID) VALUES('%s','%s',%d)"),
				flightTy,
				_item->GetCondition(),
				_eventID) ;
			id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
			_item->SetID(id) ;
		}else
		{
			SQL.Format(_T("UPDATE AS_Initial_condition SET FLIGHT_TYPE = '%s' , CONDITION = '%s'  WHERE ID = %d AND EVENT_ID = %d"),
				flightTy,
				_item->GetCondition(),
				_item->GetID(),
				_eventID) ;
			CADODatabase::ExecuteSQLStatement(SQL) ;
		}
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
	}
}
void CConditionItem::DelData(CConditionItem* _item)
{
	if(_item == NULL || _item->GetID() == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("DELETE * FROM AS_Initial_condition WHERE ID = %d") , _item->GetID()) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
	}
}

CSurfacesInitalEvent::CSurfacesInitalEvent():m_ID(-1)
{

}
CSurfacesInitalEvent::~CSurfacesInitalEvent()
{
	ClearData() ;
	ClearDelData() ;
}
void CSurfacesInitalEvent::ClearData()
{
	TY_DATA_ITER iter = m_Data.begin() ;
	for ( ; iter != m_Data.end() ;iter++)
	{
		delete *iter ;
	}
	m_Data.clear() ;
}
void CSurfacesInitalEvent::ClearDelData()
{
	TY_DATA_ITER iter = m_DelData.begin() ;
	for ( ; iter != m_DelData.end() ; iter++)
	{
		delete *iter ;
	}
	m_DelData.clear() ;
}
CConditionItem* CSurfacesInitalEvent::AddConditionItem(FlightConstraint& _constraint)
{
	CConditionItem* condition = new CConditionItem() ;
	condition->SetFlightTy(_constraint) ;
	m_Data.push_back(condition) ;
	return condition ;
}
void CSurfacesInitalEvent::DelConditionItem(CConditionItem* _item)
{
	TY_DATA_ITER iter = m_Data.begin() ;
	for ( ; iter != m_Data.end() ;iter++)
	{
		if(*iter == _item)
		{
			m_DelData.push_back(*iter) ;
			m_Data.erase(iter);
			break ;
		}
	}
}
void CSurfacesInitalEvent::DelAllConditionItems()
{
	TY_DATA_ITER iter = m_Data.begin() ;
	for ( ; iter != m_Data.end() ;iter++)
	{
		m_DelData.push_back(*iter) ;
	}
	m_Data.clear() ;
}
 void CSurfacesInitalEvent::ReadData(std::vector<CSurfacesInitalEvent*>* _datas,CAirportDatabase* _database)
 {
	CString SQL ;
	SQL.Format(_T("SELECT * FROM AS_Flight_Initial_surface_Event"));
	CADORecordset dataset ;
	long count ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,dataset) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CSurfacesInitalEvent* _evnet = NULL ;
	int id = -1 ;
	CString str ;
	long val = 0;
	while (!dataset.IsEOF())
	{
		_evnet = new CSurfacesInitalEvent ;
		dataset.GetFieldValue(_T("ID"),id) ;
		_evnet->SetID(id) ;

		dataset.GetFieldValue(_T("START_TIME"),val) ;
		_evnet->SetStartTime(ElapsedTime(val)) ;

		dataset.GetFieldValue(_T("END_TIME"),val) ;
		_evnet->SetEndTime(ElapsedTime(val)) ;

		dataset.GetFieldValue(_T("EVENT_NAME"),str) ;
		_evnet->SetName(str) ;

		_datas->push_back(_evnet) ;
		CConditionItem::ReadData(_evnet->GetConditions(),_evnet->GetID() , _database) ;
		dataset.MoveNextData() ;
	}
}
void CSurfacesInitalEvent::WriteData(std::vector<CSurfacesInitalEvent*>* _datas)
{
	std::vector<CSurfacesInitalEvent*>::iterator iter = _datas->begin() ;
	for ( ; iter != _datas->end() ;iter++)
	{
		WriteData(*iter) ;
	}
}
void CSurfacesInitalEvent::DelData(std::vector<CSurfacesInitalEvent*>* _datas)
{
	std::vector<CSurfacesInitalEvent*>::iterator iter = _datas->begin() ;
	for ( ; iter != _datas->end() ;iter++)
	{
		DelData(*iter) ;
	}
}
void CSurfacesInitalEvent::WriteData(CSurfacesInitalEvent* _event )
{
	if(_event == NULL)
		return  ;
	CString SQL ;
	int id = -1 ;
	try
	{
		if(_event->GetID() == -1)
		{
			SQL.Format(_T("INSERT INTO AS_Flight_Initial_surface_Event (START_TIME,END_TIME,EVENT_NAME) VALUES(%d,%d,'%s')"),
				_event->GetStartTime().asSeconds(),
				_event->GetEndTime().asSeconds() ,
				_event->GetName()) ;
			id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
			_event->SetID(id) ;
		}else
		{
			SQL.Format(_T("UPDATE AS_Flight_Initial_surface_Event SET START_TIME = %d ,END_TIME = %d ,EVENT_NAME = '%s' WHERE ID = %d "),
				_event->GetStartTime().asSeconds(),
				_event->GetEndTime().asSeconds() ,
				_event->GetName(),
				_event->GetID()) ;
			CADODatabase::ExecuteSQLStatement(SQL) ;
		}
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CConditionItem::WriteData(_event->GetConditions(),_event->GetID()) ;
	CConditionItem::DelData(_event->GetDelCondition()) ;
}
void CSurfacesInitalEvent::DelData(CSurfacesInitalEvent* _event)
{
	if(_event == NULL || _event->GetID() == -1)
		return ; 
	CString SQL ;
	SQL.Format(_T("DELETE * FROM AS_Flight_Initial_surface_Event WHERE ID = %d") , _event->GetID()) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
}
CAircraftSurfaceSetting::~CAircraftSurfaceSetting()
{
	ClearData() ;
	ClearDelData() ;
}
void CAircraftSurfaceSetting::ClearData()
{
	TY_DATA_ITER iter = m_Data.begin() ;
	for ( ; iter != m_Data.end() ;iter++)
	{
		delete *iter ;
	}
	m_Data.clear() ;
}
void CAircraftSurfaceSetting::ClearDelData()
{
	TY_DATA_ITER iter = m_DelData.begin() ;
	for ( ; iter != m_DelData.end() ;iter++)
	{
		delete *iter ;
	}
	m_DelData.clear() ;
}
CSurfacesInitalEvent*  CAircraftSurfaceSetting::AddSurfacesInitalEvent(CString _Name)
{
	CSurfacesInitalEvent* event = new CSurfacesInitalEvent ;
	event->SetName(_Name) ;
	m_Data.push_back(event) ;
	return event ;
}
void CAircraftSurfaceSetting::DelSurfacesInitalEvent(CSurfacesInitalEvent* _event) 
{
	TY_DATA_ITER iter = m_Data.begin() ;
	for ( ; iter != m_Data.end() ;iter++)
	{
		if(*iter == _event)
		{
			m_DelData.push_back(*iter) ;
			m_Data.erase(iter) ;
			break ;
		}
	}
}
void CAircraftSurfaceSetting::ReadData()
{
	CSurfacesInitalEvent::ReadData(GetData(),m_AirportDataBase) ;
}
void CAircraftSurfaceSetting::SaveData()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		CSurfacesInitalEvent::WriteData(GetData()) ;
		CSurfacesInitalEvent::DelData(&m_DelData) ;
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
		return ;
	}

	ClearDelData() ;
}
void CAircraftSurfaceSetting::ReSetData()
{
	TY_DATA_ITER iter = m_Data.begin() ;
	for ( ; iter != m_Data.end() ;iter++)
	{
			m_DelData.push_back(*iter) ;
	}
	m_Data.clear() ;
}

CAircraftSurfaceSetting::CAircraftSurfaceSetting( CAirportDatabase* _database ) :m_AirportDataBase(_database)
{

}