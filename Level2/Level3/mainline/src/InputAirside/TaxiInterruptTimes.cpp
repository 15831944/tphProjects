#include "StdAfx.h"
#include ".\taxiinterrupttimes.h"
#include "../Database/ADODatabase.h"
#include <algorithm>

CTaxiInterruptTimeItem::CTaxiInterruptTimeItem()
:m_ID(-1)
,m_FlightTyID(-1)
,m_ShortLineID(-1)
,m_Direction(-1)
,m_bLinkedRunway(false)
,m_Startime(0L)
,m_EndTime(24*60*60L -1L)
{
	m_vLinkedRunwayData.clear();
	m_vDelLinkedRunwayData.clear();
}
CTaxiInterruptTimeItem::~CTaxiInterruptTimeItem()
{
	int nCount = (int)m_vLinkedRunwayData.size();
	for (int i =0; i < nCount ; i++)
	{
		delete m_vLinkedRunwayData.at(i);
	}
	m_vLinkedRunwayData.clear();

	nCount = (int)m_vDelLinkedRunwayData.size();
	for (int i =0; i <nCount; i++)
	{
		CTaxiLinkedRunwayData* pData = m_vDelLinkedRunwayData.at(i);
		delete pData;
		pData = NULL;
	}
	m_vDelLinkedRunwayData.clear();

}
void CTaxiInterruptTimeItem::ReadData(std::vector<CTaxiInterruptTimeItem*>& _vectorData , int FlightTyID)
{
	if(FlightTyID == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_TAXI_INTERRUPT_ITEM WHERE FLIGHTTYID = %d") ,FlightTyID) ;
	CADORecordset DataSet ;
	long count = -1 ;

	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,DataSet) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CTaxiInterruptTimeItem*  interruptTime = NULL ;
	int id = -1 ;
	long _time = 0 ;
	int holdtime = 0 ;
	DOUBLE directory = 0 ;
	while (!DataSet.IsEOF())
	{
		interruptTime = new CTaxiInterruptTimeItem ;
		DataSet.GetFieldValue(_T("ID"),id) ;
		interruptTime->SetID(id) ;

		DataSet.GetFieldValue(_T("SHORTLINE_ID"),id) ;
		interruptTime->SetShortLineID(id) ;

		DataSet.GetFieldValue(_T("START_TIME"),_time) ;
		interruptTime->SetStartTime(ElapsedTime(_time)) ;

		DataSet.GetFieldValue(_T("END_TIME"),_time) ;
		interruptTime->SetEndTime(ElapsedTime(_time)) ;

		DataSet.GetFieldValue(_T("HOLD_TIME"),holdtime) ;
		interruptTime->GetHoldTime()->ReadData(holdtime) ;

		DataSet.GetFieldValue(_T("DIRECTION"),directory) ;
		interruptTime->SetDirection((int)directory) ;

		int nLink = 0;
		DataSet.GetFieldValue(_T("LINKEDRUNWAY"),nLink) ;
		interruptTime->m_bLinkedRunway = nLink >0?true:false;

		interruptTime->SetFlightTyID(FlightTyID) ;

		if (interruptTime->m_bLinkedRunway)
			interruptTime->ReadLinkedRunwayData();

		_vectorData.push_back(interruptTime) ;

		DataSet.MoveNextData() ;
	}
}

void CTaxiInterruptTimeItem::SaveData(int FlightTyID)
{
	GetHoldTime()->SaveData() ;
	if(FlightTyID == -1 || GetShortLineID() == -1 )
		return ;

	if(GetID() > -1)
	{
		UpdateData() ;
	}
	else
	{
		CString SQL ; 
		int id = -1 ;
		int nValue = m_bLinkedRunway? 1:0;
		SQL.Format(_T("INSERT INTO TB_TAXI_INTERRUPT_ITEM (FLIGHTTYID,SHORTLINE_ID,START_TIME,END_TIME,HOLD_TIME,DIRECTION, LINKEDRUNWAY) \
					  VALUES (%d,%d,%d,%d,%d,%f,%d)"),
					  FlightTyID , m_ShortLineID , m_Startime.asSeconds() ,m_EndTime.asSeconds(), m_HoldTime.getID() ,(double)m_Direction,nValue);

		try
		{
			id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
		}
		catch (CADOException e)
		{
			e.ErrorMessage() ;
			return ;
		}

		SetID(id) ;
	}

	int nCount = (int)m_vLinkedRunwayData.size();
	for (int i =0; i <nCount; i++)
	{
		m_vLinkedRunwayData.at(i)->SaveData(m_ID);
	}

	nCount = (int)m_vDelLinkedRunwayData.size();
	for (int i =0; i <nCount; i++)
	{
		m_vDelLinkedRunwayData.at(i)->DeleteData();
	}

	for (int i =0; i <nCount; i++)
	{
		CTaxiLinkedRunwayData* pData = m_vDelLinkedRunwayData.at(i);
		delete pData;
		pData = NULL;
	}
	m_vDelLinkedRunwayData.clear();
}

void CTaxiInterruptTimeItem::DelData()
{
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_TAXI_INTERRUPT_ITEM WHERE ID = %d") , GetID()) ;
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

void CTaxiInterruptTimeItem::UpdateData()
{
	if(GetShortLineID() == -1 )
		return ;

	CString SQL ; 
	int nValue = m_bLinkedRunway? 1:0;
	SQL.Format(_T("UPDATE TB_TAXI_INTERRUPT_ITEM SET SHORTLINE_ID = %d , START_TIME = %d ,END_TIME = %d ,  \
				  HOLD_TIME = %d , DIRECTION = %f, LINKEDRUNWAY = %d WHERE ID = %d ") ,
				m_ShortLineID , m_Startime.asSeconds() ,m_EndTime.asSeconds(), m_HoldTime.getID() , (double)m_Direction,nValue ,m_ID) ;

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

void CTaxiInterruptTimeItem::SetLinkedRunway( bool bLinked )
{
	m_bLinkedRunway = bLinked;
}

bool CTaxiInterruptTimeItem::IsLinkedRunway()
{
	return m_bLinkedRunway;
}

CTaxiLinkedRunwayData* CTaxiInterruptTimeItem::GetLinkedRunwayDataByIdx( int idx )
{
	if (idx < 0|| idx >= GetLinkedRunwayDataCount())
		return NULL;

	return m_vLinkedRunwayData.at(idx);
}

int CTaxiInterruptTimeItem::GetLinkedRunwayDataCount()
{
	return (int)m_vLinkedRunwayData.size();
}

CTaxiLinkedRunwayData* CTaxiInterruptTimeItem::GetLinkedRunwayData( int nRwyID, int nRwyMark )
{
	int nCount = (int)m_vLinkedRunwayData.size();
	for (int i =0; i < nCount; i++)
	{
		CTaxiLinkedRunwayData* pRwyData = m_vLinkedRunwayData.at(i);
		if (pRwyData->GetLinkRwyID() == nRwyID && pRwyData->GetLinkRwyMark() == nRwyMark)
			return pRwyData;
	}

	return NULL;
}

void CTaxiInterruptTimeItem::ReadLinkedRunwayData()
{
	CString SQL ;
	SQL.Format(_T("SELECT * FROM AS_TAXI_LINKEDRUNWAY WHERE PARENTID = %d") ,m_ID) ;
	CADORecordset adoRecordset ;
	long nCount = -1 ;

	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,nCount,adoRecordset) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}

	while (!adoRecordset.IsEOF())
	{
		CTaxiLinkedRunwayData* pData = new CTaxiLinkedRunwayData;
		pData->ReadData(adoRecordset);
		m_vLinkedRunwayData.push_back(pData);

		adoRecordset.MoveNextData();
	}
}

void CTaxiInterruptTimeItem::AddLinkedRunwayData( CTaxiLinkedRunwayData* pData )
{
	if (std::find(m_vLinkedRunwayData.begin(),m_vLinkedRunwayData.end(),pData) != m_vLinkedRunwayData.end())
		return;

	m_vLinkedRunwayData.push_back(pData);
}

void CTaxiInterruptTimeItem::DelLinkedRunwayData( CTaxiLinkedRunwayData* pData )
{
	std::vector<CTaxiLinkedRunwayData*>::iterator iter = std::find(m_vLinkedRunwayData.begin(),m_vLinkedRunwayData.end(),pData);
	
	if (iter != m_vLinkedRunwayData.end())
	{
		m_vDelLinkedRunwayData.push_back(*iter);
		m_vLinkedRunwayData.erase(iter);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////
void CTaxiInterruptFlightData::ReadData()
{
	CTaxiInterruptTimeItem::ReadData(m_DataSet,m_FlightTyID) ;
}

void CTaxiInterruptFlightData::SaveData() 
{
	CString SQL ;
	TCHAR str[1024] = {0} ;
	GetFlightTy()->WriteSyntaxStringWithVersion(str) ;
	int id = -1 ;
	if(GetFlightTyID() <0)
	{
		SQL.Format(_T("INSERT INTO TB_TAXI_INTERRUPT_FLIGHTTY (FLIGHTTY) VALUES('%s')"),str) ;
		try
		{
			id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
			SetFlightTyID(id) ;
		}
		catch (CADOException e)
		{
			e.ErrorMessage() ;
			return ;
		}

	}
	else
	{
		SQL.Format(_T("UPDATE TB_TAXI_INTERRUPT_FLIGHTTY SET FLIGHTTY = '%s' WHERE ID = %d"),str,GetFlightTyID()) ;
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

	INTERRUPTTIMEITER iter = m_DataSet.begin() ;
	for ( ; iter != m_DataSet.end() ;iter++)
	{
		(*iter)->SaveData(m_FlightTyID);
	}

	iter = m_DelDataSet.begin();
	for ( ; iter != m_DelDataSet.end() ;iter++)
	{
		(*iter)->DelData();
	}

	ClearDelData() ;
}
CTaxiInterruptFlightData::CTaxiInterruptFlightData():m_FlightTyID(-1)
{

}
CTaxiInterruptFlightData::~CTaxiInterruptFlightData()
{
	ClearAllData() ;
}
void CTaxiInterruptFlightData::ClearAllData()
{
	INTERRUPTTIMEITER iter = m_DataSet.begin() ;
	for ( ; iter != m_DataSet.end() ;iter++)
	{
		delete *iter ;
	}
	m_DataSet.clear() ;

	ClearDelData();
}
void CTaxiInterruptFlightData::ClearDelData()
{
	INTERRUPTTIMEITER iter = m_DelDataSet.begin() ;
	for ( ; iter != m_DelDataSet.end() ;iter++)
	{
		delete *iter ;
	}
	m_DelDataSet.clear() ;
}
void CTaxiInterruptFlightData::AddTaxiInterruptTimeItem(CTaxiInterruptTimeItem* _item)
{
	INTERRUPTTIMEITER iter = std::find(m_DataSet.begin(),m_DataSet.end(),_item) ;
	if(iter == m_DataSet.end())
		m_DataSet.push_back(_item) ;
}
void CTaxiInterruptFlightData::DelTaxiInterruptTimeItem(CTaxiInterruptTimeItem* _item)
{
	INTERRUPTTIMEITER iter = std::find(m_DataSet.begin(),m_DataSet.end(),_item) ;
	if(iter != m_DataSet.end())
	{
		m_DelDataSet.push_back(*iter) ;
		m_DataSet.erase(iter) ;
	}
}
void CTaxiInterruptFlightData::DelAllData()
{
	INTERRUPTTIMEITER iter = m_DataSet.begin() ;
	for ( ; iter != m_DataSet.end() ;iter++)
	{
		m_DelDataSet.push_back(*iter) ;
	}
	m_DataSet.clear() ;
}

int CTaxiInterruptFlightData::GetCount() const
{
	return (int)m_DataSet.size();
}

FlightConstraint* CTaxiInterruptFlightData::GetFlightTy()
{
	return &m_FlightTy ;
}

CTaxiInterruptTimeItem* CTaxiInterruptFlightData::GetItem( int idx ) const
{
	if (idx < 0 || idx >= GetCount())
		return NULL;

	return m_DataSet.at(idx);
}

void CTaxiInterruptFlightData::DelData()
{
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_TAXI_INTERRUPT_ITEM WHERE FLIGHTTYID = %d") ,m_FlightTyID) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}

	SQL.Format(_T("DELETE * FROM TB_TAXI_INTERRUPT_FLIGHTTY WHERE ID = %d") ,GetFlightTyID()) ;
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTaxiInterruptTimes::ReadData()
{
	CString SQL ;
	SQL.Format(_T("SELECT * FROM TB_TAXI_INTERRUPT_FLIGHTTY")) ;
	CADORecordset DataSet ;
	long count = 0 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,DataSet) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	int id  = -1 ; 
	CTaxiInterruptFlightData* Pair = NULL ;
	CString str ; 
	while(!DataSet.IsEOF())
	{
		Pair = new CTaxiInterruptFlightData ;
		DataSet.GetFieldValue(_T("ID"),id) ;
		Pair->SetFlightTyID(id) ;

		DataSet.GetFieldValue(_T("FLIGHTTY"),str) ;
		Pair->GetFlightTy()->SetAirportDB(m_airportDB) ;
		Pair->GetFlightTy()->setConstraintWithVersion(str) ;

		Pair->ReadData() ;

		m_DataSet.push_back(Pair) ;
		DataSet.MoveNextData() ;
	}
}
void CTaxiInterruptTimes::SaveData()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		TY_DATA_FLIGHT_ITER iter = m_DataSet.begin() ;
		for ( ; iter != m_DataSet.end() ;iter++)
		{
			(*iter)->SaveData();
		}

		iter = m_DelDataSet.begin() ;
		for ( ; iter != m_DelDataSet.end() ;iter++)
		{
			(*iter)->DelData() ;
		}
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
		return ;
	}
	
	clearDelData() ;
}
CTaxiInterruptFlightData* CTaxiInterruptTimes::AddFlightTy(FlightConstraint _constraint ) 
{
	CTaxiInterruptFlightData* pair = FindByFlightID(_constraint) ;
	TCHAR str[1024] = {0} ;
	if(pair == NULL)
	{
		pair = new CTaxiInterruptFlightData ;
		*pair->GetFlightTy() = _constraint;
		m_DataSet.push_back(pair) ;
	}
	return pair ;
}
CTaxiInterruptFlightData* CTaxiInterruptTimes::FindByFlightID(FlightConstraint flightTy) 
{
	TY_DATA_FLIGHT_ITER iter = m_DataSet.begin() ;
	for ( ; iter != m_DataSet.end() ;iter++)
	{
		if((*iter)->GetFlightTy()->isEqual(&flightTy) )
			return *iter ;
	}
	return NULL ;
}
void CTaxiInterruptTimes::AddInterruptTimes(CTaxiInterruptTimeItem* _item , FlightConstraint flightTy)
{
	CTaxiInterruptFlightData* pair = FindByFlightID(flightTy) ;
	TCHAR str[1024] = {0} ;
	if(pair == NULL)
	{
		pair = new CTaxiInterruptFlightData ;
		*pair->GetFlightTy() = flightTy ;
		m_DataSet.push_back(pair) ;
	}
	pair->AddTaxiInterruptTimeItem(_item) ;
}
void CTaxiInterruptTimes::DelInterruptTimes(CTaxiInterruptTimeItem* _item , FlightConstraint flightTy)
{
	CTaxiInterruptFlightData* pair = FindByFlightID(flightTy) ;
	if(pair != NULL)
	{
		pair->DelTaxiInterruptTimeItem(_item) ;
	}
}
void CTaxiInterruptTimes::DelFlightData( CTaxiInterruptFlightData* _pair)
{
	TY_DATA_FLIGHT_ITER iter = m_DataSet.begin() ;
	for ( ; iter != m_DataSet.end() ;iter++)
	{
		if((*iter) == _pair )
		{
			m_DelDataSet.push_back(*iter) ;
			m_DataSet.erase(iter) ;
			return;
		}
	}
}
CTaxiInterruptTimes::CTaxiInterruptTimes(CAirportDatabase *airportDB):m_airportDB(airportDB)
{

}
CTaxiInterruptTimes::~CTaxiInterruptTimes()
{
	clearAllData() ;
}
void CTaxiInterruptTimes::clearDelData()
{
	TY_DATA_FLIGHT_ITER iter = m_DelDataSet.begin() ;
	for ( ; iter != m_DelDataSet.end() ;iter++)
	{
		delete *iter ;
	}
	m_DelDataSet.clear() ;
}
void CTaxiInterruptTimes::clearAllData()
{
	TY_DATA_FLIGHT_ITER iter = m_DataSet.begin() ;
	for ( ; iter != m_DataSet.end() ;iter++)
	{
		delete *iter ;
	}
	m_DataSet.clear() ;	
	clearDelData() ;
}

int CTaxiInterruptTimes::GetCount() const
{
	return (int)m_DataSet.size();
}