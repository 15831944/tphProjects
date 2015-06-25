#include "StdAfx.h"
#include ".\inboudrouteassflightty.h"
#include "../Database/DBElementCollection_Impl.h"


CRouteAssFlightTY::CRouteAssFlightTY(int m_projectID) : m_proID(m_projectID),FlightTy_id(-1),CNodeType(FLIGHTTY_NODE)
{
	P_timeranges = NULL ;
}
CRouteAssFlightTY::CRouteAssFlightTY(int m_projectID,int flightTy_ID) : m_proID(m_projectID),FlightTy_id(flightTy_ID),CNodeType(FLIGHTTY_NODE)
{
	P_timeranges = NULL ;
}
CRouteAssFlightTY::~CRouteAssFlightTY()
{
	if(P_timeranges != NULL)
		delete P_timeranges ;
}
CString CRouteAssFlightTY::GetFlightType()
{
	return flightTy_Name ;
}
void CRouteAssFlightTY::SetFlightType(const CString& name)
{
	flightTy_Name.Format(_T("%s"),name) ;
}
void CRouteAssFlightTY::SetFlightTyID(int ID)
{
	FlightTy_id = ID ;
	P_timeranges->SetFlightID(FlightTy_id) ;
}
int CRouteAssFlightTY::GetFlightTyID()
{
	return FlightTy_id ;
}
CRouteAssTimeRange*  CRouteAssFlightTY::AddTimeRange(TimeRange* timerange)
{
	CRouteAssTimeRange*  P_time = NULL ;
	if(class_TY == INBOUD_ROUTE_FLIGHT)
	   P_time =new CInboudRouteAssTimeRange(m_proID,FlightTy_id,timerange) ;
	if(class_TY == OUTBOUD_ROUTE_FLIGHT)
		P_time = new COutboundRouteAssTimeRange(m_proID,FlightTy_id,timerange) ;
	P_timeranges->AddTimeRange(P_time) ;
	return P_time ;
}
void CRouteAssFlightTY::DelTimeRange(CRouteAssTimeRange* timerange) 
{
	if(timerange != NULL)
		P_timeranges->DelTimeRange(timerange) ;
}
void CRouteAssFlightTY::ReadTimeTanges()
{
	P_timeranges->ReadTimeRangeFromDB() ;
}
 CRouteAssTimeRangeContainer::TIMERANGE_VECTOR& CRouteAssFlightTY::GetTimeRanges() 
{
	return P_timeranges->GetTimeRanges() ;
}
void CRouteAssFlightTY::SaveFlightTyToDB()
{
	CString SQL ;
	if(FlightTy_id == -1 )
	{
		SQL.Format(_T("INSERT INTO INBOUDROUTEASSIGN_FLIGHTTY(PROID ,CLASSTY,FLIGHTTY)VALUES(%d,%d,'%s')") ,
			m_proID,
			class_TY,
			flightTy_Name) ;
		SetFlightTyID(CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) );
	}else
	{
		SQL.Format(_T("UPDATE INBOUDROUTEASSIGN_FLIGHTTY SET PROID = %d ,FLIGHTTY = '%s' WHERE ID = %d"),
			m_proID,
			flightTy_Name,
			FlightTy_id) ;
		CADODatabase::ExecuteSQLStatement(SQL) ;
	}
	
}
void CRouteAssFlightTY::DelFlightTyToDB()
{
	CString SQL ;
	if(FlightTy_id == -1 )
		return ;
	else
	{
		SQL.Format(_T("DELETE FROM INBOUDROUTEASSIGN_FLIGHTTY WHERE ID = %d "),
			FlightTy_id) ;
		CADODatabase::ExecuteSQLStatement(SQL);
		P_timeranges->DelAllTimeRangesFromDB() ;
	}
}
void CRouteAssFlightTY::OnSaveData()
{
	SaveFlightTyToDB() ;
	P_timeranges->OnSaveData() ;
}
CInboudRouteAssFlightTY::CInboudRouteAssFlightTY(int m_projectID,int flightTy_ID) : CRouteAssFlightTY(m_projectID,flightTy_ID)
{
	P_timeranges = new CInboudRouteAssTimeRangeContainer(m_projectID ,flightTy_ID) ;
	class_TY = INBOUD_ROUTE_FLIGHT ;
}
CInboudRouteAssFlightTY::CInboudRouteAssFlightTY(int m_projectID) : CRouteAssFlightTY(m_projectID)
{
	P_timeranges = new CInboudRouteAssTimeRangeContainer(m_projectID ,FlightTy_id) ;
	class_TY = INBOUD_ROUTE_FLIGHT ;
}
CInboudRouteAssFlightTY::~CInboudRouteAssFlightTY()
{

}

COutboundRouteAssFilghtTY::COutboundRouteAssFilghtTY(int m_projectID) : CRouteAssFlightTY(m_projectID)
{
	P_timeranges = new COutboundRouteAssTimeRangeContaniner(m_projectID,FlightTy_id) ;
	class_TY = OUTBOUD_ROUTE_FLIGHT ;
}
COutboundRouteAssFilghtTY::COutboundRouteAssFilghtTY(int m_projectID,int flightTy_ID) : CRouteAssFlightTY(m_projectID,flightTy_ID)
{
	P_timeranges = new COutboundRouteAssTimeRangeContaniner(m_projectID,flightTy_ID) ;
	class_TY = OUTBOUD_ROUTE_FLIGHT ;
}
COutboundRouteAssFilghtTY::~COutboundRouteAssFilghtTY()
{

}