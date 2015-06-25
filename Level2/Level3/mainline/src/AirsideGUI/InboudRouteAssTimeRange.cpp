#include "StdAfx.h"
#include ".\inboudrouteasstimerange.h"
#include "../Database/DBElementCollection_Impl.h"

CRouteAssTimeRange::CRouteAssTimeRange(void): m_flightty_ID(-1),m_timerange_ID(-1),m_proID(-1) ,CNodeType(TIMERANGE_NODE)
{

}
CRouteAssTimeRange::CRouteAssTimeRange(int m_pro): m_flightty_ID(-1),m_timerange_ID(-1),m_proID(m_pro),CNodeType(TIMERANGE_NODE)
{

}
CRouteAssTimeRange::CRouteAssTimeRange(int m_pro ,int flightID ) :m_proID(m_pro),m_flightty_ID(-1),CNodeType(TIMERANGE_NODE)
  {

  }
CRouteAssTimeRange::CRouteAssTimeRange(int m_pro ,int flightID ,TimeRange* time_range)
:m_proID(m_pro),m_flightty_ID(flightID),m_timerange_ID(-1),CNodeType(TIMERANGE_NODE)
{
	timeRange.SetStartTime(time_range->GetStartTime());
	timeRange.SetEndTime(time_range->GetEndTime()) ;
}
CRouteAssTimeRange::~CRouteAssTimeRange()
{

}
CString CRouteAssTimeRange::GetTimeRangeName()
{
	CString strDay ;
	CString strTimeRange ;
	ElapsedTime etInsert = timeRange.GetStartTime();
	long lSecond = etInsert.asSeconds();
	strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400 + 1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
	strTimeRange = strDay + _T(" - ");
	etInsert = timeRange.GetEndTime();
	lSecond = etInsert.asSeconds();
	strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400 + 1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
	strTimeRange += strDay;
	return strTimeRange ;
}
void CRouteAssTimeRange::SetFlightTyID(int flight_id)
{
	m_flightty_ID = flight_id ;
}
int CRouteAssTimeRange::GetFlightTyID()
{
	return m_flightty_ID ;
}
void CRouteAssTimeRange::SetTimeRangeID(int id)
{
	m_timerange_ID = id ;
	Inboud_Route.SetTimeRangeID(id) ;
}
int CRouteAssTimeRange::GetTimeRangeID()
{
	return m_timerange_ID ;
}
void CRouteAssTimeRange::SetStartTime(ElapsedTime time)
{
	timeRange.SetStartTime(time) ;
}
void CRouteAssTimeRange::SetEndTime(ElapsedTime time )
{
	timeRange.SetEndTime(time ) ;
}
 CInboundRouteAssignment* CRouteAssTimeRange::GetInboudRouteAssignment() 
{
	return &Inboud_Route ;
}
void CRouteAssTimeRange::SaveTimeRangeToDB()
{
	CString SQL ;
	if(m_timerange_ID == -1 )
	{
		SQL.Format(_T("INSERT INTO INBOUD_ROUTE_ASS_TIMERANGE (FLIGHTTYID ,BEGINTIME ,ENDTIME,CLASSTYPE ,PROID)VALUES( %d,%i,%i,%d,%d)"),
		GetFlightTyID(),
		timeRange.GetStartTime().asSeconds(),
		timeRange.GetEndTime().asSeconds(),
		class_ty,
		m_proID) ;
		SetTimeRangeID(CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL)) ;
	}
	else
	{
		SQL.Format(_T("UPDATE INBOUD_ROUTE_ASS_TIMERANGE SET FLIGHTTYID = %d,BEGINTIME= %i,ENDTIME = %i,PROID= %d WHERE ID = %d"),
		GetFlightTyID(),
		timeRange.GetStartTime().asSeconds(),
		timeRange.GetEndTime().asSeconds(),
		m_proID,
		m_timerange_ID) ;
		CADODatabase::ExecuteSQLStatement(SQL) ;
	}
}
void CRouteAssTimeRange::DelTimeRangeFromDB()
{
if(m_timerange_ID == -1 )
			return ;
	CString SQL ;
	SQL.Format(_T("DELETE FROM INBOUD_ROUTE_ASS_TIMERANGE WHERE ID=%d"),
        m_timerange_ID) ;
	CADODatabase::ExecuteSQLStatement(SQL) ;
	Inboud_Route.DeleteData() ;
}
void CRouteAssTimeRange::OnSaveData()
{
	SaveTimeRangeToDB() ;
	Inboud_Route.SaveData(m_proID) ;
}
CInboudRouteAssTimeRange::CInboudRouteAssTimeRange(int m_pro ,int flightID ) : CRouteAssTimeRange(m_pro,flightID)
{
	class_ty = INBOUD_ROUTE_FLIGHT ;
}
CInboudRouteAssTimeRange::CInboudRouteAssTimeRange(int m_pro ,int flightID ,TimeRange* time_range):
 CRouteAssTimeRange(m_pro,flightID,time_range)
{
	class_ty = INBOUD_ROUTE_FLIGHT ;
}
CInboudRouteAssTimeRange::CInboudRouteAssTimeRange():CRouteAssTimeRange()
{
	class_ty = INBOUD_ROUTE_FLIGHT ;
}
CInboudRouteAssTimeRange::CInboudRouteAssTimeRange(int m_pro):CRouteAssTimeRange(m_pro)
{
	class_ty = INBOUD_ROUTE_FLIGHT ;
}
CInboudRouteAssTimeRange::~CInboudRouteAssTimeRange()
{

}
COutboundRouteAssTimeRange::COutboundRouteAssTimeRange(int m_pro):CRouteAssTimeRange(m_pro)
{
	class_ty = OUTBOUD_ROUTE_FLIGHT ;
}
COutboundRouteAssTimeRange::COutboundRouteAssTimeRange() :CRouteAssTimeRange()
{
   class_ty = OUTBOUD_ROUTE_FLIGHT ;
}
COutboundRouteAssTimeRange::COutboundRouteAssTimeRange(int m_pro ,int flightID ,TimeRange* time_range) :CRouteAssTimeRange(m_pro,flightID,time_range)
{
	class_ty = OUTBOUD_ROUTE_FLIGHT ;
}
COutboundRouteAssTimeRange::COutboundRouteAssTimeRange(int m_pro ,int flightID ) :CRouteAssTimeRange(m_pro,flightID)
{
class_ty = OUTBOUD_ROUTE_FLIGHT ;
}
COutboundRouteAssTimeRange::~COutboundRouteAssTimeRange()
{

}