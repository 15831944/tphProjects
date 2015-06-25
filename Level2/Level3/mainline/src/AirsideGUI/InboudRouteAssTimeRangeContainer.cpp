#include "StdAfx.h"
#include ".\inboudrouteasstimerangecontainer.h"
#include <algorithm>
#include "../Database/DBElementCollection_Impl.h"

CRouteAssTimeRangeContainer::CRouteAssTimeRangeContainer(int proID ,int FlightTy_ID,int classTY):m_proID(proID),m_flightTyID(FlightTy_ID),class_TY(classTY)
{

}
CRouteAssTimeRangeContainer::~CRouteAssTimeRangeContainer()
{
	TIMERANGE_ITER  iter = list_TimeRange.begin() ;
	for(; iter != list_TimeRange.end() ;++iter)
	{
		if(*iter != NULL)
			delete *iter ;
	}
	for(iter = del_list_TimeRange.begin() ; iter != del_list_TimeRange.end() ; ++iter)
	{
		if(*iter != NULL)
			delete *iter ;
	}
}
void CRouteAssTimeRangeContainer::SetFlightID(int flightID)
{
	m_flightTyID = flightID ;
	TIMERANGE_ITER iter = list_TimeRange.begin() ;
	for ( ; iter != list_TimeRange.end() ;++iter)
	{
	   (*iter)->SetFlightTyID(flightID) ;
	}		
}
void CRouteAssTimeRangeContainer::AddTimeRange(CRouteAssTimeRange* P_timerange)
{
	if(P_timerange != NULL)
		list_TimeRange.push_back(P_timerange) ;
}
void CRouteAssTimeRangeContainer::DelTimeRange(CRouteAssTimeRange* P_timeRange)
{
	if(P_timeRange != NULL)
	{
         TIMERANGE_ITER iter = list_TimeRange.begin() ;
		 for ( ; iter != list_TimeRange.end() ;++iter)
		 {
			 if((*iter) == P_timeRange)
			 {
               del_list_TimeRange.push_back(P_timeRange) ;
			   list_TimeRange.erase(iter);
			   break ;
			 }
		 }		
	}
}
 CRouteAssTimeRangeContainer::TIMERANGE_VECTOR& CRouteAssTimeRangeContainer::GetTimeRanges()
{
	return list_TimeRange ;
}
void CRouteAssTimeRangeContainer::ReadTimeRangeFromDB()
{
list_TimeRange.clear() ;
	if(m_flightTyID == -1)
		return  ;
	CString SQL ;
	SQL.Format(_T("SELECT * FROM INBOUD_ROUTE_ASS_TIMERANGE WHERE PROID = %d and FLIGHTTYID = %d and CLASSTYPE =%d"),
		m_proID,
		m_flightTyID,
		class_TY) ;
	CADORecordset  recordset ;
	long cout ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,cout,recordset) ;
		InitTimeRanges(recordset) ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}

}
void CRouteAssTimeRangeContainer::InitTimeRanges(CADORecordset& record)
{
	int id = -1 ;
	long time = 0 ;
	while (!record.IsEOF())
	{
        CRouteAssTimeRange* timerange = NULL ;
		if(class_TY == INBOUD_ROUTE_FLIGHT)
		    timerange = new CInboudRouteAssTimeRange(m_proID,m_flightTyID) ;
		else
			timerange = new COutboundRouteAssTimeRange(m_proID,m_flightTyID) ;
		record.GetFieldValue(_T("ID"),id) ;
		timerange->SetTimeRangeID(id) ;
		record.GetFieldValue(_T("FLIGHTTYID") ,id) ;
		timerange->SetFlightTyID(id) ;

		record.GetFieldValue(_T("BEGINTIME") ,time) ;
		timerange->SetStartTime(time) ;

		record.GetFieldValue(_T("ENDTIME") ,time) ;
		timerange->SetEndTime(time) ;
		timerange->GetInboudRouteAssignment()->ReadData(m_proID) ;
		list_TimeRange.push_back(timerange) ;
		record.MoveNextData() ;
	}
}
void CRouteAssTimeRangeContainer::OnSaveData()
{

		TIMERANGE_ITER iter = list_TimeRange.begin() ;
		for(; iter != list_TimeRange.end() ;++iter)
		{
			(*iter)->OnSaveData() ;
		}
		for(iter = del_list_TimeRange.begin() ; iter != del_list_TimeRange.end() ;++iter)
		{
			(*iter)->DelTimeRangeFromDB() ;
			(*iter)->SetTimeRangeID(-1) ;
		}
}
void CRouteAssTimeRangeContainer::DelAllTimeRangesFromDB()
{

		TIMERANGE_ITER iter = list_TimeRange.begin() ;
		for(; iter != list_TimeRange.end() ;++iter)
		{
			(*iter)->DelTimeRangeFromDB() ;
			(*iter)->SetTimeRangeID(-1) ;
		}
	//for(iter = del_list_TimeRange.begin() ; iter != del_list_TimeRange.end() ;++iter)
	//{
	//	(*iter)->DelTimeRangeFromDB() ;
	//	(*iter)->SetTimeRangeID(-1) ;
	//}
}
CInboudRouteAssTimeRangeContainer::CInboudRouteAssTimeRangeContainer(int proID ,int FlightTy_ID):CRouteAssTimeRangeContainer(proID,FlightTy_ID,INBOUD_ROUTE_FLIGHT)
{

}
CInboudRouteAssTimeRangeContainer::~CInboudRouteAssTimeRangeContainer(void)
{

}
COutboundRouteAssTimeRangeContaniner::COutboundRouteAssTimeRangeContaniner(int proID ,int FlightTy_ID):CRouteAssTimeRangeContainer(proID,FlightTy_ID,OUTBOUD_ROUTE_FLIGHT)
{
	
}
COutboundRouteAssTimeRangeContaniner::~COutboundRouteAssTimeRangeContaniner(void)
{


}