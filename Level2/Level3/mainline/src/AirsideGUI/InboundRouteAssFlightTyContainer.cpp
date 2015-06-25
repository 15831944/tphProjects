#include "StdAfx.h"
#include ".\inboundrouteassflighttycontainer.h"
#include <algorithm>
#include "../Database/DBElementCollection_Impl.h"


CRouteAssFlightTyContainer::CRouteAssFlightTyContainer(int Pro_ID,int classTY): class_TY(classTY),proid(Pro_ID)
{
	ReadFlightTYFromDB() ;
}
CRouteAssFlightTyContainer::~CRouteAssFlightTyContainer(void)
{

	FLIGHTTY_LIST_ITER iter = list_flightTY.begin() ;
	for( ; iter != list_flightTY.end() ;++iter)
	{
		if((*iter) != NULL)
			delete *iter ;
	}
	iter = del_list_flightTY.begin() ;
	for( ; iter != del_list_flightTY.end() ;++iter )
	{
		if((*iter) != NULL)
			delete *iter ;
	}
}
CRouteAssFlightTY* CRouteAssFlightTyContainer::AddFlightTY(CString flightTY)
{
	CRouteAssFlightTY* p_flightty = NULL ;
	if(class_TY == INBOUD_ROUTE_FLIGHT)
	   p_flightty = new CInboudRouteAssFlightTY(proid) ;
	if(class_TY == OUTBOUD_ROUTE_FLIGHT)
       p_flightty = new COutboundRouteAssFilghtTY(proid) ;
	p_flightty->SetFlightType(flightTY) ;
	if(p_flightty != NULL)
		list_flightTY.push_back(p_flightty) ;
	return p_flightty ;
}
void CRouteAssFlightTyContainer::DelFlightTY(CRouteAssFlightTY* flightTY)
{
	if(flightTY != NULL)
	{
		FLIGHTTY_LIST_ITER iter= list_flightTY.begin();
		for( ; iter != list_flightTY.end() ; ++iter)
		{
			if((*iter) == flightTY)
			{
				del_list_flightTY.push_back(flightTY) ;
				list_flightTY.erase(iter) ;
				break ;
			}
		}
	}

}
void CRouteAssFlightTyContainer::OnSaveData()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		FLIGHTTY_LIST_ITER iter = list_flightTY.begin() ;
		for( ; iter != list_flightTY.end() ;++iter )
		{
			(*iter)->OnSaveData() ;
		}
		iter = del_list_flightTY.begin() ;
		for( ; iter != del_list_flightTY.end() ;++iter)
		{
			(*iter)->DelFlightTyToDB() ;
			(*iter)->SetFlightTyID(-1) ;
		}
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
}
void CRouteAssFlightTyContainer::DelAllFlightTYFromDB()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		FLIGHTTY_LIST_ITER iter = list_flightTY.begin() ;
		for( ; iter != list_flightTY.end() ;++iter )
		{
			(*iter)->DelFlightTyToDB() ;
			(*iter)->SetFlightTyID(-1) ;
		}
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}


	/*iter = del_list_flightTY.begin() ;
	for( ; iter != del_list_flightTY.end() ;++iter)
	{
	(*iter)->DelFlightTyToDB() ;
	(*iter)->SetFlightTyID(-1) ;
	}*/
}
void CRouteAssFlightTyContainer::ReadFlightTYFromDB()
{
	list_flightTY.clear() ;
	CString SQL ;
	SQL.Format(_T("SELECT * FROM INBOUDROUTEASSIGN_FLIGHTTY WHERE PROID =%d and CLASSTY = %d") ,
		proid,
		class_TY) ;
	CADORecordset DataSet ;
	long cout = 0 ;
	int ID ;
	CString flight_TY ;
	try{
		CADODatabase::ExecuteSQLStatement(SQL,cout,DataSet) ;
	}catch(CADOException& e)
	{
		e.ErrorMessage() ;
		return ;
	}
	while (!DataSet.IsEOF())
	{
	    CRouteAssFlightTY* flightTY = NULL ;
		if(class_TY == INBOUD_ROUTE_FLIGHT)
		   flightTY = new CInboudRouteAssFlightTY(proid) ;
		if(class_TY == OUTBOUD_ROUTE_FLIGHT)
           flightTY = new COutboundRouteAssFilghtTY(proid) ;
		DataSet.GetFieldValue(_T("ID"),ID) ;
		flightTY->SetFlightTyID(ID) ;
		DataSet.GetFieldValue(_T("FLIGHTTY"),flight_TY) ;
		flightTY->SetFlightType(flight_TY) ;
		flightTY->ReadTimeTanges() ;
		list_flightTY.push_back(flightTY) ;
		DataSet.MoveNextData() ;
	}
}
CInboundRouteAssFlightTyContainer::CInboundRouteAssFlightTyContainer(int Pro_ID) : CRouteAssFlightTyContainer(Pro_ID,INBOUD_ROUTE_FLIGHT)
{

}

CInboundRouteAssFlightTyContainer::~CInboundRouteAssFlightTyContainer(void)
{

}
COutboundRouteAssFlightTyContainer::COutboundRouteAssFlightTyContainer(int Pro_ID) : CRouteAssFlightTyContainer(Pro_ID,OUTBOUD_ROUTE_FLIGHT)
{

}

COutboundRouteAssFlightTyContainer::~COutboundRouteAssFlightTyContainer(void)
{

}