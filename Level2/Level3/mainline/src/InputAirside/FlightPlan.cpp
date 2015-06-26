#include "StdAfx.h"
#include "FlightPlan.h"
#include "../Database/ADODatabase.h"
#include "AirRoute.h"
#include "InputAirside/AirsideImportExportManager.h"
#include "../Common/FLT_CNST.H"
#include "../Common/AirportDatabase.h"


using namespace ns_FlightPlan;

// **********************************************************************************************************
// WayPointProperty

WayPointProperty::WayPointProperty(const ARWaypoint& wayPoint, int nAirRoutePropID)
 : m_nTableID(-1)
 , m_wayPoint(wayPoint)
 , m_nAirRoutePropID(nAirRoutePropID)
 , m_fAltitude(.0f)
 , m_fSpeed(.0f)
 , m_phase(Phase_cruise)
{


}
WayPointProperty::WayPointProperty(int nAirRoutePropID)
: m_nTableID(-1)
, m_nAirRoutePropID(nAirRoutePropID)
, m_fAltitude(.0f)
, m_fSpeed(.0f)
, m_phase(Phase_cruise)
{

}
void WayPointProperty::SetDefaultAltitude()
{
	m_fAltitude = m_wayPoint.getDefaultAltitude();
}
CString WayPointProperty::GetPhaseNameString()
{
	ASSERT(m_phase >= Phase_cruise && m_phase < Phase_Count);
	return Phase_NameString[m_phase];
}
WayPointProperty::WayPointProperty(CADORecordset& adoRecordset, CAirRoute& airRoute)
{
	adoRecordset.GetFieldValue(_T("ID"), m_nTableID);
	adoRecordset.GetFieldValue(_T("AIRROUTEPROPID"), m_nAirRoutePropID);

	int nWayPointID;
	adoRecordset.GetFieldValue(_T("WAYPOINTID"), nWayPointID);
	ARWaypoint* pWayPoint = airRoute.getARWaypoint(nWayPointID);
	if (pWayPoint == NULL)
	{
		m_fAltitude = 0.0;
		m_fSpeed = 0.0;
		m_phase = Phase_cruise;
		return;
	}
	m_wayPoint = *pWayPoint;
	m_ExtendPoint = pWayPoint->getExtentPoint();
	m_wayPoint.setExtentPoint(m_ExtendPoint);

	adoRecordset.GetFieldValue(_T("ALTITUDE"), m_fAltitude);
	adoRecordset.GetFieldValue(_T("SPEED"), m_fSpeed);

	int nPhase = 0;
	adoRecordset.GetFieldValue(_T("PHASE"), nPhase);
	m_phase = Phase(nPhase);
}

void WayPointProperty::Save()
{
	if (m_nAirRoutePropID < 0 )
		return;

	CString strSQL;
	
	if (m_nTableID < 0)
	{
		strSQL.Format(_T("INSERT INTO FLIGHTPLANWAYPOINTPROP (WAYPOINTID, AIRROUTEPROPID, ALTITUDE, SPEED, PHASE) VALUES (%d,%d,%f,%f,%d)"),
		m_wayPoint.getID(), m_nAirRoutePropID, m_fAltitude, m_fSpeed, m_phase);

		m_nTableID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		strSQL.Format(_T("UPDATE FLIGHTPLANWAYPOINTPROP SET WAYPOINTID = %d, AIRROUTEPROPID = %d, ALTITUDE = %f, SPEED =%f, PHASE =%d WHERE ID = %d"),
						 m_wayPoint.getID(), m_nAirRoutePropID, m_fAltitude, m_fSpeed, (int)m_phase ,m_nTableID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}
}

void WayPointProperty::DeleteFromDB()
{
	CString strSQL;
	strSQL.Format(_T("DELETE FROM FLIGHTPLANWAYPOINTPROP WHERE ID = %d"), m_nTableID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

// WayPointProperty
// **********************************************************************************************************


// **********************************************************************************************************
// AirRouteProperty

AirRouteProperty::AirRouteProperty(const CAirRoute& airRoute, int nFlightPlanID)
 : m_nTableID(-1)
 , m_nFlightPlanID(nFlightPlanID)
 , m_airRoute(airRoute)
{
	const std::vector<ARWaypoint*>& wayPoints = m_airRoute.getARWaypoint();
	for (std::vector<ARWaypoint*>::const_iterator iter = wayPoints.begin(); iter != wayPoints.end(); ++iter)
	{
		m_wayPointProperties.push_back(new WayPointProperty((**iter), m_nTableID));
	}
}
AirRouteProperty::AirRouteProperty(int nFlightPlanID)
: m_nTableID(-1)
, m_nFlightPlanID(nFlightPlanID)
{

}
AirRouteProperty::AirRouteProperty(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"), m_nTableID);
	adoRecordset.GetFieldValue(_T("FLIGHTPLANID"), m_nFlightPlanID);

	int nAirRouteID;
	adoRecordset.GetFieldValue(_T("AIRROUTEID"), nAirRouteID);
	//if(ALTObject::IsObjectExists(nAirRouteID) == false)
	//{	
	//	m_nTableID = -1;
	//	return;
	//}
	//else
	//{
		m_airRoute.ReadData(nAirRouteID);
	//}
		if(m_airRoute.getName().IsEmpty())
			return;
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM FLIGHTPLANWAYPOINTPROP WHERE (AIRROUTEPROPID = %d)"), m_nTableID);

	long nRecordCount = -1;
	CADORecordset adoRecordsetWayPointProp;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordsetWayPointProp);

	m_airRoute.CalWaypointExtentPoint();
	while (!adoRecordsetWayPointProp.IsEOF())
	{
		m_wayPointProperties.push_back(new WayPointProperty(adoRecordsetWayPointProp, m_airRoute));
		adoRecordsetWayPointProp.MoveNextData();
	}
}

void AirRouteProperty::AddItem(WayPointProperty* wayPointProperty)
{
	wayPointProperty->SetAirRoutePropID(m_nTableID);
	m_wayPointProperties.push_back(wayPointProperty);
}

void AirRouteProperty::DeleteItem(WayPointProperty* wayPointProperty)
{
	size_t nItemIndex = -1;
	size_t nCount = m_wayPointProperties.size();
	for (size_t i = 0; i < nCount; ++i)
	{
		if (wayPointProperty == m_wayPointProperties.at(i))
		{
			nItemIndex = i;
			break;
		}
	}

	if (nItemIndex >= 0)
		m_wayPointProperties.erase(m_wayPointProperties.begin() + nItemIndex);
}

void AirRouteProperty::Save()
{
	if (m_nFlightPlanID < 0)
		return;

	CString strSQL;
	if (m_nTableID < 0)
	{
		strSQL.Format(_T("INSERT INTO FLIGHTPLANAIRROUTEPROP (FLIGHTPLANID, AIRROUTEID) VALUES (%d, %d)"), m_nFlightPlanID, m_airRoute.getID());
		m_nTableID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		strSQL.Format(_T("UPDATE FLIGHTPLANAIRROUTEPROP SET FLIGHTPLANID = %d, AIRROUTEID = %d WHERE ID = %d"),
						m_nFlightPlanID, m_airRoute.getID(), m_nTableID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}

	for (std::vector<WayPointProperty *>::iterator iter = m_wayPointProperties.begin();
		 iter != m_wayPointProperties.end(); ++iter)
	{
		(*iter)->SetAirRoutePropID(m_nTableID);
		(*iter)->Save();
	}
}

void AirRouteProperty::DeleteFromDB()
{
	CString strSQL;
	strSQL.Format(_T("DELETE FROM FLIGHTPLANAIRROUTEPROP WHERE ID = %d"), m_nTableID);
	CADODatabase::ExecuteSQLStatement(strSQL);

	for (std::vector<WayPointProperty *>::iterator iter = m_wayPointProperties.begin();
		iter != m_wayPointProperties.end(); ++iter)
	{
		(*iter)->DeleteFromDB();
	}
}
void AirRouteProperty::DeleteFromPosToEnd(int nWayPointPos)
{
	size_t i =0;
	for(; i < GetWayPointCount(); ++i)
	{
		if(GetItem(i)->GetWayPoint().getWaypoint().getID() == nWayPointPos)
		{
			break;
		}
	}

	m_wayPointPropertiesNeedDel.insert(m_wayPointPropertiesNeedDel.begin(),m_wayPointProperties.begin() + i,m_wayPointProperties.end());
	m_wayPointProperties.erase(m_wayPointProperties.begin() + i,m_wayPointProperties.end());

}
void AirRouteProperty::DeleteBeginToPos(int nWayPointPos)
{
	size_t i =0;
	for(; i < GetWayPointCount(); ++i)
	{
		if(GetItem(i)->GetWayPoint().getWaypoint().getID() == nWayPointPos)
		{
			break;
		}
	}
	m_wayPointPropertiesNeedDel.insert(m_wayPointPropertiesNeedDel.begin(),m_wayPointProperties.begin(),m_wayPointProperties.begin() +i);
	m_wayPointProperties.erase(m_wayPointProperties.begin(),m_wayPointProperties.begin() + i);
}
// AirRouteProperty
// **********************************************************************************************************


// **********************************************************************************************************
// FlightPlan

FlightPlanSpecific::FlightPlanSpecific(int nProjectID)
{
	m_nProjectID = nProjectID;

}

FlightPlanSpecific::FlightPlanSpecific(CADORecordset& adoRecordset)
{
	adoRecordset.GetFieldValue(_T("ID"), m_nTableID);
	adoRecordset.GetFieldValue(_T("PROJECTID"), m_nProjectID);
	adoRecordset.GetFieldValue(_T("FLIGHTTYPE"), m_strFlightType);

	int nOperationType = 0;
	adoRecordset.GetFieldValue(_T("OPERATIONTYPE"), nOperationType);
	m_operationType = OperationType(nOperationType);

	CString strSQL;
	strSQL.Format(_T("SELECT * FROM FLIGHTPLANAIRROUTEPROP WHERE (FLIGHTPLANID = %d)"), m_nTableID);

	long nRecordCount = -1;
	CADORecordset adoRecordsetAirRouteProp;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordsetAirRouteProp);

	while (!adoRecordsetAirRouteProp.IsEOF())
	{
		m_airRouteProperties.push_back(new AirRouteProperty(adoRecordsetAirRouteProp));
		adoRecordsetAirRouteProp.MoveNextData();
	}

	//check the data is valid
	for (int i =0; i < (int)m_airRouteProperties.size(); ++i)
	{
		if(m_airRouteProperties[i]->getTableID() == -1)
		{
			m_airRoutePropertiesNeedDel.insert(m_airRoutePropertiesNeedDel.begin(),
				m_airRouteProperties.begin() + i,m_airRouteProperties.end());
			break;
		}
	}

}

void FlightPlanSpecific::AddItem(AirRouteProperty* airRouteProperty)
{
	if(GetOperationType() == takeoff)
	{
		if(GetAirRouteCount() == 0 &&airRouteProperty->GetAirRoute().getRouteType() == CAirRoute::SID)
		{
			size_t nWpCount = airRouteProperty->GetWayPointCount();
			//if (nWpCount > 0)
			//{
			//	airRouteProperty->GetItem(0)->SetPhase(WayPointProperty::Phase_takeoff);
			//	airRouteProperty->GetItem(0)->SetDefaultAltitude();
			//}
			for (size_t i=0; i < nWpCount; ++ i)
			{
				airRouteProperty->GetItem(i)->SetPhase(WayPointProperty::Phase_climbout);
				//airRouteProperty->GetItem(i)->SetDefaultAltitude();
				airRouteProperty->GetItem(i)->SetAltitude(1000);
				airRouteProperty->GetItem(i)->SetSpeed(200);
			}
		}
		else//the second or more
		{
			CAirRoute *pAirRoute = &airRouteProperty->GetAirRoute();
			size_t nAirRouteCount = GetAirRouteCount();
			if (nAirRouteCount > 0)
			{
			//get the last air route in the vector
				size_t nAirRouteCount = GetAirRouteCount();
				int nIntWayPointID = m_airRouteProperties.at(nAirRouteCount-1)->GetAirRoute().HasIntersection(pAirRoute);
			
				m_airRouteProperties.at(nAirRouteCount-1)->DeleteFromPosToEnd(nIntWayPointID);
				airRouteProperty->DeleteBeginToPos(nIntWayPointID);
			}

			size_t nWpCount = airRouteProperty->GetWayPointCount();
			for (size_t i=0; i < nWpCount; ++ i)
			{
				airRouteProperty->GetItem(i)->SetPhase(WayPointProperty::Phase_cruise);
				//airRouteProperty->GetItem(i)->SetDefaultAltitude();
				airRouteProperty->GetItem(i)->SetAltitude(3000);
				airRouteProperty->GetItem(i)->SetSpeed(250);

			}
		}
	}
	
	if (GetOperationType() == enroute)
	{	
		size_t nAirRouteCount = GetAirRouteCount();
		if (nAirRouteCount > 0)
		{
			CAirRoute *pAirRoute = &airRouteProperty->GetAirRoute();
			int nIntWayPointID = m_airRouteProperties.at(nAirRouteCount-1)->GetAirRoute().HasIntersection(pAirRoute);
			m_airRouteProperties.at(nAirRouteCount-1)->DeleteFromPosToEnd(nIntWayPointID);
			airRouteProperty->DeleteBeginToPos(nIntWayPointID);
		}

		size_t nWpCount = airRouteProperty->GetWayPointCount();
		for (size_t i=0; i < nWpCount; ++ i)
		{
			airRouteProperty->GetItem(i)->SetPhase(WayPointProperty::Phase_cruise);
			//airRouteProperty->GetItem(i)->SetDefaultAltitude();
			airRouteProperty->GetItem(i)->SetAltitude(3000);
			airRouteProperty->GetItem(i)->SetSpeed(250);

		}
	}

	if (GetOperationType() == landing)
	{


		if(airRouteProperty->GetAirRoute().getRouteType() == CAirRoute::STAR)
		{
			size_t nAirRouteCount = GetAirRouteCount();
			if (nAirRouteCount > 0)
			{
				CAirRoute *pAirRoute = &airRouteProperty->GetAirRoute();

				int nIntWayPointID = m_airRouteProperties.at(nAirRouteCount-1)->GetAirRoute().HasIntersection(pAirRoute);
				m_airRouteProperties.at(nAirRouteCount-1)->DeleteFromPosToEnd(nIntWayPointID);
				airRouteProperty->DeleteBeginToPos(nIntWayPointID);

			}

			int nWpCount = static_cast<int>(airRouteProperty->GetWayPointCount());
			
			int i = 0;
			for (; i < nWpCount -1; ++ i)
			{
				airRouteProperty->GetItem(i)->SetPhase(WayPointProperty::Phase_cruise);
				//airRouteProperty->GetItem(i)->SetDefaultAltitude();
				airRouteProperty->GetItem(i)->SetAltitude(3000);
				airRouteProperty->GetItem(i)->SetSpeed(250);

			}
			if (nWpCount -1 >= 0 )
			{
				airRouteProperty->GetItem(nWpCount -1)->SetPhase(WayPointProperty::Phase_approach);
				//airRouteProperty->GetItem(nWpCount -1)->SetDefaultAltitude();
				airRouteProperty->GetItem(i)->SetAltitude(300);
				airRouteProperty->GetItem(i)->SetSpeed(120);
			}
		}
		else
		{	
			size_t nAirRouteCount = GetAirRouteCount();
			if (nAirRouteCount > 0)
			{
				CAirRoute *pAirRoute = &airRouteProperty->GetAirRoute();

				int nIntWayPointID = m_airRouteProperties.at(nAirRouteCount-1)->GetAirRoute().HasIntersection(pAirRoute);
				m_airRouteProperties.at(nAirRouteCount-1)->DeleteFromPosToEnd(nIntWayPointID);
				airRouteProperty->DeleteBeginToPos(nIntWayPointID);

			}

			size_t nWpCount = airRouteProperty->GetWayPointCount();
			for (size_t i=0; i < nWpCount; ++ i)
			{
				airRouteProperty->GetItem(i)->SetPhase(WayPointProperty::Phase_cruise);
				//airRouteProperty->GetItem(i)->SetDefaultAltitude();
				airRouteProperty->GetItem(i)->SetAltitude(3000);
				airRouteProperty->GetItem(i)->SetSpeed(250);
			}
		}

	}

//	airRouteProperty->SetFlightPlanID(m_nTableID);
	m_airRouteProperties.push_back(airRouteProperty);
}

void FlightPlanSpecific::DeleteItem(AirRouteProperty* airRouteProperty)
{
	size_t nItemIndex = -1;
	size_t nCount = m_airRouteProperties.size();
	for (size_t i = 0; i < nCount; ++i)
	{
		if (airRouteProperty == m_airRouteProperties.at(i))
		{
			nItemIndex = i;
			break;
		}
	}

	if (nItemIndex >= 0)
		DeleteItem(nItemIndex);
}

void FlightPlanSpecific::DeleteItem(size_t nIndex)
{
//	m_airRouteProperties.at(nIndex).DeleteFromDB();
	m_airRoutePropertiesNeedDel.push_back(m_airRouteProperties.at(nIndex));
	m_airRouteProperties.erase(m_airRouteProperties.begin() + nIndex);
}

void FlightPlanSpecific::Save()
{
	CString strSQL;
	int protype = 1;
	if (m_nTableID < 0)
	{
		strSQL.Format(_T("INSERT INTO FLIGHTPLAN (PROJECTID, FLIGHTTYPE, OPERATIONTYPE,PROFILETYPE) VALUES (%d, '%s', %d,%d)"),
					  m_nProjectID, m_strFlightType,  m_operationType,protype);
		m_nTableID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		strSQL.Format(_T("UPDATE FLIGHTPLAN SET PROJECTID = %d, FLIGHTTYPE = '%s', OPERATIONTYPE = %d ,PROFILETYPE = %d WHERE ID = %d"),
						 m_nProjectID, m_strFlightType, m_operationType,protype, m_nTableID);
		CADODatabase::ExecuteSQLStatement(strSQL);
	}

	for (std::vector<AirRouteProperty *>::iterator iter = m_airRouteProperties.begin();
		iter != m_airRouteProperties.end(); ++iter)
	{
		(*iter)->SetFlightPlanID(m_nTableID);
		(*iter)->Save();
	}
	for (std::vector<AirRouteProperty *>::iterator iterDel = m_airRoutePropertiesNeedDel.begin();
		iterDel!= m_airRoutePropertiesNeedDel.end(); ++iterDel)
	{
		(*iterDel)->DeleteFromDB();
	}
	m_airRoutePropertiesNeedDel.clear();
}

void FlightPlanSpecific::DeleteFromDB()
{
	CString strSQL;
	strSQL.Format(_T("DELETE FROM FLIGHTPLAN WHERE ID = %d"), m_nTableID);
	CADODatabase::ExecuteSQLStatement(strSQL);

	for (std::vector<AirRouteProperty *>::iterator iter = m_airRouteProperties.begin();
		iter != m_airRouteProperties.end(); ++iter)
	{
		(*iter)->DeleteFromDB();
	}
}

// FlightPlan
// **********************************************************************************************************


// **********************************************************************************************************
// FlightPlans
FlightPlans::FlightPlans(int nProjectID)
{
	m_nProjectID = nProjectID;
	m_pAirportDB = NULL;
}
FlightPlans::FlightPlans(int nProjectID,CAirportDatabase* pAirportDB)
:m_nProjectID(nProjectID)
,m_pAirportDB(pAirportDB)
{

}
//FlightPlan* FlightPlans::AddItem(FlightPlan* flightPlan)
//{
//	m_flightPlans.push_back(flightPlan);
//	return /*m_flightPlans.back()*/NULL;
//}
//
//void FlightPlans::DeleteItem(FlightPlan* flightPlan)
//{
//
//	size_t nItemIndex = -1;
//	size_t nCount = m_flightPlans.size();
//	for (size_t i = 0; i < nCount; ++i)
//	{
//		if (flightPlan == m_flightPlans.at(i))
//		{
//			nItemIndex = i;
//			break;
//		}
//	}
//
//	if (nItemIndex >= 0)
//	{
//		m_flightPlansNeedDel.push_back(m_flightPlans.at(nItemIndex));
//		m_flightPlans.erase(m_flightPlans.begin() + nItemIndex);
//	
//	}
//
//
//}

void FlightPlans::Read()
{
	m_flightPlans.clear();

	CString strSQL;
	strSQL.Format(_T("SELECT * FROM FLIGHTPLAN WHERE (PROJECTID = %d)"), m_nProjectID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
 		int profileType = 0;
 		adoRecordset.GetFieldValue(_T("PROFILETYPE"),profileType);
 		FlightPlanBase* pProfileProperty = NULL;

		if (profileType)
		{
 			pProfileProperty = new FlightPlanSpecific(adoRecordset);
		}
 		else
 		{
 			pProfileProperty= new FlightPlanGeneric(m_nProjectID);
			FlightPlanGeneric* pProfileGeneric = (FlightPlanGeneric*)pProfileProperty;
 			pProfileGeneric->InitFromDBRecordset(adoRecordset);
 		}

		AddFlightPlan(pProfileProperty);
		adoRecordset.MoveNextData();
	}
}

void FlightPlans::Save()
{
	for (std::vector<FlightPlanBase *>::iterator iter = m_flightPlans.begin();
		iter != m_flightPlans.end(); ++iter)
	{
		(*iter)->Save();
	}

	for (std::vector<FlightPlanBase *>::iterator iterDel = m_flightPlansNeedDel.begin();
		iterDel != m_flightPlansNeedDel.end(); ++iterDel)
	{
		(*iterDel)->DeleteFromDB();
	}
}
















ns_FlightPlan::WayPointProperty::Phase ns_FlightPlan::WayPointProperty::GetPhase() const
{
	return m_phase;
}

double ns_FlightPlan::WayPointProperty::GetSpeed() const
{
	return m_fSpeed;
}

double ns_FlightPlan::WayPointProperty::GetAltitude() const
{
	return m_fAltitude;
}

const ARWaypoint& ns_FlightPlan::WayPointProperty::GetWayPoint() const
{
	return m_wayPoint;
}

ARWaypoint& ns_FlightPlan::WayPointProperty::GetWayPoint()
{
	return m_wayPoint;
}
size_t ns_FlightPlan::AirRouteProperty::GetWayPointCount() const
{
	return m_wayPointProperties.size();
}

const CAirRoute& ns_FlightPlan::AirRouteProperty::GetAirRoute() const
{
	return m_airRoute;
}

CAirRoute& ns_FlightPlan::AirRouteProperty::GetAirRoute()
{
	return m_airRoute;
}

WayPointProperty* ns_FlightPlan::AirRouteProperty::GetItem( size_t nIndex )
{
	return m_wayPointProperties.at(nIndex);
}

const WayPointProperty* ns_FlightPlan::AirRouteProperty::GetItem( size_t nIndex ) const
{
	return m_wayPointProperties.at(nIndex);
}
//size_t ns_FlightPlan::FlightPlans::GetFlightPlanCount() const
//{
//	return /*m_flightPlans.size()*/0;
//}
//
//const FlightPlan* ns_FlightPlan::FlightPlans::GetItem( size_t nIndex ) const
//{
//	return /*m_flightPlans.at(nIndex)*/NULL;
//}
//
//FlightPlan* ns_FlightPlan::FlightPlans::GetItem( size_t nIndex )
//{
//	return /*m_flightPlans.at(nIndex)*/NULL;
//}

ns_FlightPlan::FlightPlans::~FlightPlans()
{
	for (size_t i = 0; i < m_flightPlans.size(); i++)
	{
		delete m_flightPlans[i];
	}
	m_flightPlans.clear();

	for (int ii = 0; ii < static_cast<int>(m_flightPlansNeedDel.size()); ii++)
	{
		delete m_flightPlansNeedDel[ii];
	}
	m_flightPlansNeedDel.clear();
}
//LPCTSTR ns_FlightPlan::FlightPlan::GetFlightType() const
//{
//	return m_strFlightType;
//}
//
//ns_FlightPlan::FlightPlan::OperationType ns_FlightPlan::FlightPlan::GetOperationType() const
//{
//	return m_operationType;
//}

size_t ns_FlightPlan::FlightPlanSpecific::GetAirRouteCount() const
{
	return m_airRouteProperties.size();
}

const AirRouteProperty* ns_FlightPlan::FlightPlanSpecific::GetItem( size_t nIndex ) const
{
	return m_airRouteProperties.at(nIndex);
}

AirRouteProperty* ns_FlightPlan::FlightPlanSpecific::GetItem( size_t nIndex )
{
	return m_airRouteProperties.at(nIndex);
}

int ns_FlightPlan::FlightPlans::GetCount() const
{
	return (int)m_flightPlans.size();
}

FlightPlanBase* ns_FlightPlan::FlightPlans::GetFlightPlan( int nidx )
{
	ASSERT(nidx >= 0 && nidx < GetCount());
	return m_flightPlans[nidx];
}

void ns_FlightPlan::FlightPlans::AddFlightPlan( FlightPlanBase* pFlightPlan )
{
	ASSERT(pFlightPlan);
	if (pFlightPlan)
	{
		pFlightPlan->SetAirportDB(m_pAirportDB);
		m_flightPlans.push_back(pFlightPlan);
		SortFlightPlan();
	}
}

void ns_FlightPlan::FlightPlans::DeleteFlightPlan( FlightPlanBase* pFlightPlan )
{
	std::vector<FlightPlanBase*>::iterator iter = std::find(m_flightPlans.begin(),m_flightPlans.end(),pFlightPlan);
	if (iter != m_flightPlans.end())
	{
		m_flightPlans.erase(iter);
		m_flightPlansNeedDel.push_back(pFlightPlan);
	}
}

void ns_FlightPlan::FlightPlans::SortFlightPlan()
{
	std::sort(m_flightPlans.begin(),m_flightPlans.end(),SortCompare);
}

bool ns_FlightPlan::FlightPlans::SortCompare(FlightPlanBase* pFlightPlanSource,FlightPlanBase* pFlightPlanDest)
{
	return (*pFlightPlanSource) < (*pFlightPlanDest);
}

int ns_FlightPlan::FlightPlans::GetIndex( FlightPlanBase* pFlightPlan )
{
	for (int i = 0; i < (int)m_flightPlans.size(); i++)
	{
		if (pFlightPlan == m_flightPlans[i])
		{
			return i;
		}
	}
	return -1;
}

ns_FlightPlan::FlightPlanBase::FlightPlanBase()
:m_operationType(takeoff)
{
m_nTableID = -1;
m_nProjectID = -1;
}

ns_FlightPlan::FlightPlanBase::~FlightPlanBase()
{

}
bool ns_FlightPlan::FlightPlanBase::operator < (const FlightPlanBase& flightProfile)const
{
	//fill own
	FlightConstraint fltCon1;
	fltCon1.SetAirportDB( m_pAirportDB );
	fltCon1.setConstraintWithVersion(m_strFlightType);

	//fill other
	FlightConstraint fltCon2;
	fltCon2.SetAirportDB(m_pAirportDB);
	fltCon2.setConstraintWithVersion(flightProfile.m_strFlightType);

	if(fltCon1.fits(fltCon2))
		return false;
	else if(fltCon2.fits(fltCon1))
		return true;

	return false;
}

LPCTSTR ns_FlightPlan::FlightPlanBase::GetFlightType() const
{
	return m_strFlightType;
}

ns_FlightPlan::FlightPlanBase::OperationType ns_FlightPlan::FlightPlanBase::GetOperationType() const
{
	return m_operationType;
}


ns_FlightPlan::FlightPlanGeneric::FlightPlanGeneric(int nProjectID)
{
	m_nProjectID = nProjectID;
	m_pAirportDB = NULL;
	m_pGenericPhases = new FlightPlanGenericPhaseList();
}

ns_FlightPlan::FlightPlanGeneric::~FlightPlanGeneric()
{
	if (m_pGenericPhases)
	{
		delete m_pGenericPhases;
		m_pGenericPhases = NULL;
	}
}

void ns_FlightPlan::FlightPlanGeneric::SaveData( int nForeignID )
{
	CString strSQL;
	if (m_nTableID < 0)
	{
		strSQL.Format(_T("INSERT INTO FLIGHTPLAN (PROJECTID, FLIGHTTYPE, OPERATIONTYPE,PROFILETYPE) VALUES (%d, '%s', %d,0)"),
			m_nProjectID, m_strFlightType,  m_operationType);
		m_nTableID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);

		m_pGenericPhases->SaveData(m_nTableID);
	}
	else
	{
		UpdateData();
	}
}

void ns_FlightPlan::FlightPlanGeneric::UpdateData()
{
	m_pGenericPhases->SaveData(m_nTableID);

	CString strSQL;
	strSQL.Format(_T("UPDATE FLIGHTPLAN SET PROJECTID = %d, FLIGHTTYPE = '%s', OPERATIONTYPE = %d ,PROFILETYPE = 0 WHERE ID = %d"),
		m_nProjectID, m_strFlightType, m_operationType,m_nTableID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void ns_FlightPlan::FlightPlanGeneric::DeleteData()
{
	m_pGenericPhases->DeleteData();

	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM FLIGHTPLAN WHERE (ID = %d);"),m_nTableID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void ns_FlightPlan::FlightPlanGeneric::InitFromDBRecordset( CADORecordset& recordset )
{
	recordset.GetFieldValue(_T("ID"), m_nTableID);
	recordset.GetFieldValue(_T("PROJECTID"), m_nProjectID);
	recordset.GetFieldValue(_T("FLIGHTTYPE"), m_strFlightType);

	int nOperationType = 0;
	recordset.GetFieldValue(_T("OPERATIONTYPE"), nOperationType);
	m_operationType = OperationType(nOperationType);

	m_pGenericPhases->InitFromDBRecordset(recordset);
}

int ns_FlightPlan::FlightPlanGeneric::GetIndex(FlightPlanGenericPhaseBase* pGenericComponent)
{
	for (int i = 0; i < GetCount(); i++)
	{
		if (pGenericComponent == GetItem(i))
		{
			return i;
		}
	}
	return 0;
}

void ns_FlightPlan::FlightPlanGeneric::Save()
{
	SaveData(-1);
}

void ns_FlightPlan::FlightPlanGeneric::DeleteFromDB()
{
	DeleteData();
}

void ns_FlightPlan::FlightPlanGeneric::AddItem( FlightPlanGenericPhase* pItem )
{
	m_pGenericPhases->AddItem(pItem);
}

FlightPlanGenericPhase* ns_FlightPlan::FlightPlanGeneric::GetItem( int nIdx ) const
{
	return (FlightPlanGenericPhase*)m_pGenericPhases->GetItem(nIdx);
}

void ns_FlightPlan::FlightPlanGeneric::DeleteItem( FlightPlanGenericPhase* pItem )
{
	m_pGenericPhases->DeleteItem(pItem);
}

int ns_FlightPlan::FlightPlanGeneric::GetCount() const
{
	return m_pGenericPhases->GetCount();
}

void ns_FlightPlan::FlightPlanGeneric::FillDefaultValue()
{
	
}

double ns_FlightPlan::FlightPlanGeneric::GetSpeed( double dDistToRunway ) const
{
	double dSamll = (std::numeric_limits<double>::max)();

	FlightPlanGenericPhase *phaseSamll;


	for (int nItem = 0; nItem < GetCount(); ++nItem)
	{
		FlightPlanGenericPhase* pPhase  = GetItem(nItem);
		ASSERT(pPhase != NULL);

		if(dDistToRunway - pPhase->GetDistanceToRunway() >= 0.0 &&
			dDistToRunway - pPhase->GetDistanceToRunway()  < dSamll)
		{
			dSamll =dDistToRunway  - pPhase->GetDistanceToRunway() ;
			phaseSamll = pPhase;
		}
	}
	if(phaseSamll != NULL)
	{
        return (phaseSamll->GetUpSpeed() + phaseSamll->GetDownSpeed()) * 0.5;
	}

	ASSERT(0);
	return -1.0;

}

double ns_FlightPlan::FlightPlanGeneric::GetMinSpeed( double dDistToRunway ) const
{
	double dSamll = (std::numeric_limits<double>::max)();

	FlightPlanGenericPhase *phaseSamll;


	for (int nItem = 0; nItem < GetCount(); ++nItem)
	{
		FlightPlanGenericPhase* pPhase  = GetItem(nItem);
		ASSERT(pPhase != NULL);

		if(dDistToRunway - pPhase->GetDistanceToRunway() >= 0.0 &&
			dDistToRunway - pPhase->GetDistanceToRunway()  < dSamll)
		{
			dSamll =dDistToRunway  - pPhase->GetDistanceToRunway() ;
			phaseSamll = pPhase;
		}
	}
	if(phaseSamll != NULL)
	{
		return phaseSamll->GetDownSpeed();
	}

	ASSERT(0);
	return -1.0;

}

double ns_FlightPlan::FlightPlanGeneric::GetMaxSpeed( double dDistToRunway ) const
{
	double dSamll = (std::numeric_limits<double>::max)();

	FlightPlanGenericPhase *phaseSamll;


	for (int nItem = 0; nItem < GetCount(); ++nItem)
	{
		FlightPlanGenericPhase* pPhase  = GetItem(nItem);
		ASSERT(pPhase != NULL);

		if(dDistToRunway - pPhase->GetDistanceToRunway() >= 0.0 &&
			dDistToRunway - pPhase->GetDistanceToRunway()  < dSamll)
		{
			dSamll =dDistToRunway  - pPhase->GetDistanceToRunway() ;
			phaseSamll = pPhase;
		}
	}
	if(phaseSamll != NULL)
	{
		return phaseSamll->GetUpSpeed();
	}

	ASSERT(0);
	return -1.0;

}

void ns_FlightPlan::FlightPlanGeneric::FillLandingValue()
{
	//First data
	FlightPlanGenericPhase* pInitData = new FlightPlanGenericPhase();
	{
		pInitData->SetPhase(FlightPlanGenericPhase::Phase_cruise);
		pInitData->SetDistancetoRunway(120.0);
		pInitData->SetUpAltitude(33000.0);
		pInitData->SetDownAltitude(30000.0);
		pInitData->SetAltitudeSeparate(1000.0);
		pInitData->SetDownSpeed(400.0);
		pInitData->SetUpSpeed(460.0);
	}
	m_pGenericPhases->AddItem(pInitData);

	//Change 1
	FlightPlanGenericPhase* pChange1Data = new FlightPlanGenericPhase();
	{
		pChange1Data->SetPhase(FlightPlanGenericPhase::Phase_Terminal);
		pChange1Data->SetDistancetoRunway(90.0);
		pChange1Data->SetUpAltitude(30000.0);
		pChange1Data->SetDownAltitude(27000.0);
		pChange1Data->SetAltitudeSeparate(1000.0);
		pChange1Data->SetDownSpeed(380.0);
		pChange1Data->SetUpSpeed(420.0);
	}
	m_pGenericPhases->AddItem(pChange1Data);

	//Change 2
	FlightPlanGenericPhase* pChange2Data = new FlightPlanGenericPhase();
	{
		pChange2Data->SetPhase(FlightPlanGenericPhase::Phase_Terminal);
		pChange2Data->SetDistancetoRunway(60.0);
		pChange2Data->SetUpAltitude(23000.0);
		pChange2Data->SetDownAltitude(20000.0);
		pChange2Data->SetAltitudeSeparate(1000.0);
		pChange2Data->SetDownSpeed(350.0);
		pChange2Data->SetUpSpeed(400.0);
	}
	m_pGenericPhases->AddItem(pChange2Data);

	//Change 3
	FlightPlanGenericPhase* pChange3Data = new FlightPlanGenericPhase();
	{
		pChange3Data->SetPhase(FlightPlanGenericPhase::Phase_Terminal);
		pChange3Data->SetDistancetoRunway(30.0);
		pChange3Data->SetUpAltitude(13000.0);
		pChange3Data->SetDownAltitude(10000.0);
		pChange3Data->SetAltitudeSeparate(1000.0);
		pChange3Data->SetDownSpeed(230.0);
		pChange3Data->SetUpSpeed(250.0);
	}
	m_pGenericPhases->AddItem(pChange3Data);

	//Change 4
	FlightPlanGenericPhase* pChange4Data = new FlightPlanGenericPhase();
	{
		pChange4Data->SetPhase(FlightPlanGenericPhase::Phase_approach);
		pChange4Data->SetDistancetoRunway(15.0);
		pChange4Data->SetUpAltitude(4000);
		pChange4Data->SetDownAltitude(3000.0);
		pChange4Data->SetAltitudeSeparate(1000.0);
		pChange4Data->SetDownSpeed(180.0);
		pChange4Data->SetUpSpeed(200.0);
	}
	m_pGenericPhases->AddItem(pChange4Data);
}

void ns_FlightPlan::FlightPlanGeneric::FillTakeoffValue()
{
	//Change 4
	FlightPlanGenericPhase* pChange4Data = new FlightPlanGenericPhase();
	{
		pChange4Data->SetPhase(FlightPlanGenericPhase::Phase_climbout);
		pChange4Data->SetDistancetoRunway(15.0);
		pChange4Data->SetUpAltitude(4000);
		pChange4Data->SetDownAltitude(3000.0);
		pChange4Data->SetAltitudeSeparate(1000.0);
		pChange4Data->SetDownSpeed(180.0);
		pChange4Data->SetUpSpeed(200.0);
	}
	m_pGenericPhases->AddItem(pChange4Data);
	
	//Change 3
	FlightPlanGenericPhase* pChange3Data = new FlightPlanGenericPhase();
	{
		pChange3Data->SetPhase(FlightPlanGenericPhase::Phase_climbout);
		pChange3Data->SetDistancetoRunway(30.0);
		pChange3Data->SetUpAltitude(13000.0);
		pChange3Data->SetDownAltitude(10000.0);
		pChange3Data->SetAltitudeSeparate(1000.0);
		pChange3Data->SetDownSpeed(230.0);
		pChange3Data->SetUpSpeed(250.0);
	}
	m_pGenericPhases->AddItem(pChange3Data);

	//Change 2
	FlightPlanGenericPhase* pChange2Data = new FlightPlanGenericPhase();
	{
		pChange2Data->SetPhase(FlightPlanGenericPhase::Phase_cruise);
		pChange2Data->SetDistancetoRunway(60.0);
		pChange2Data->SetUpAltitude(23000.0);
		pChange2Data->SetDownAltitude(20000.0);
		pChange2Data->SetAltitudeSeparate(1000.0);
		pChange2Data->SetDownSpeed(350.0);
		pChange2Data->SetUpSpeed(400.0);
	}
	m_pGenericPhases->AddItem(pChange2Data);

}
//////////////////////////////////////////////////////////////////////////
//
ns_FlightPlan::FlightPlanGenericPhaseList::FlightPlanGenericPhaseList()
{
	
}

ns_FlightPlan::FlightPlanGenericPhaseList::~FlightPlanGenericPhaseList()
{
	for (int i = 0; i < GetCount(); i++)
	{
		FlightPlanGenericPhaseBase* pItem = m_vPhase[i];
		delete pItem;
	}
	m_vPhase.clear();

	for (int ii = 0; ii < (int)m_vDelPhase.size(); ii++)
	{
		FlightPlanGenericPhaseBase* pItem = m_vDelPhase[ii];
		delete pItem;
	}
	m_vDelPhase.clear();
}

void ns_FlightPlan::FlightPlanGenericPhaseList::AddItem( FlightPlanGenericPhaseBase* pItem )
{
	ASSERT(pItem);
	if (pItem)
	{
		m_vPhase.push_back(pItem);
	}
}

FlightPlanGenericPhaseBase* ns_FlightPlan::FlightPlanGenericPhaseList::GetItem( int nIdx )
{
	ASSERT(nIdx >=0 && nIdx < GetCount());
	return m_vPhase[nIdx];
}

void ns_FlightPlan::FlightPlanGenericPhaseList::DeleteItem( FlightPlanGenericPhaseBase* pItem )
{
	ASSERT(pItem);
	if (pItem == NULL)
		return;
	
	std::vector<FlightPlanGenericPhaseBase*>::iterator iter = std::find(m_vPhase.begin(),m_vPhase.end(),pItem);
	if (iter != m_vPhase.end())
	{
		m_vPhase.erase(iter);
		m_vDelPhase.push_back(pItem);
	}
}

int ns_FlightPlan::FlightPlanGenericPhaseList::GetCount()
{
	return (int)m_vPhase.size();
}

void ns_FlightPlan::FlightPlanGenericPhaseList::SaveData(int nFlightPlanID)
{
	for (std::vector<FlightPlanGenericPhaseBase *>::iterator iter = m_vPhase.begin();
		iter != m_vPhase.end(); ++iter)
	{
		(*iter)->SaveData(nFlightPlanID);
	}

	for (std::vector<FlightPlanGenericPhaseBase *>::iterator iterDel = m_vDelPhase.begin();
		iterDel != m_vDelPhase.end(); ++iterDel)
	{
		(*iterDel)->DeleteData();
	}
}

void ns_FlightPlan::FlightPlanGenericPhaseList::UpdateData()
{

}

void ns_FlightPlan::FlightPlanGenericPhaseList::DeleteData()
{
	for (std::vector<FlightPlanGenericPhaseBase *>::iterator iter = m_vPhase.begin();
		iter != m_vPhase.end(); ++iter)
	{
		(*iter)->DeleteData();
	}

	for (std::vector<FlightPlanGenericPhaseBase *>::iterator iterDel = m_vDelPhase.begin();
		iterDel != m_vDelPhase.end(); ++iterDel)
	{
		(*iterDel)->DeleteData();
	}
}

void ns_FlightPlan::FlightPlanGenericPhaseList::InitFromDBRecordset( CADORecordset& recordset )
{
	int nFLIGHTPLANID = -1;
	recordset.GetFieldValue(_T("ID"), nFLIGHTPLANID);
	CString strSQL;
	strSQL.Format(_T("SELECT * FROM GENERICPROPERTY WHERE FLIGHTPLANID = %d"), nFLIGHTPLANID);

	long nRecordCount = -1;
	CADORecordset adoRecordset;
	CADODatabase::ExecuteSQLStatement(strSQL, nRecordCount, adoRecordset);

	while (!adoRecordset.IsEOF())
	{
		FlightPlanGenericPhaseBase* pItem = new FlightPlanGenericPhase();
		pItem->InitFromDBRecordset(adoRecordset);
		m_vPhase.push_back(pItem);
		adoRecordset.MoveNextData();
	}
}

ns_FlightPlan::FlightPlanGenericPhase::FlightPlanGenericPhase()
:m_dDistToRunway(0.0)
,m_dUpAltitude(0.0)
,m_dDownAltitude(0.0)
,m_dAltitudeSep(0.0)
,m_dUpSpeed(0.0)
,m_dDownSpeed(0.0)
,m_phase(Phase_cruise)
{
	m_nTableID = -1;
}

ns_FlightPlan::FlightPlanGenericPhase::~FlightPlanGenericPhase()
{

}

void ns_FlightPlan::FlightPlanGenericPhase::SetDistancetoRunway( double dDistance )
{
	m_dDistToRunway = dDistance;
}

double ns_FlightPlan::FlightPlanGenericPhase::GetDistanceToRunway() const
{
	return m_dDistToRunway;
}

void ns_FlightPlan::FlightPlanGenericPhase::SetUpAltitude( double dAltitude )
{
	m_dUpAltitude = dAltitude;
}

double ns_FlightPlan::FlightPlanGenericPhase::GetUpAltitude() const
{
	return m_dUpAltitude;
}

void ns_FlightPlan::FlightPlanGenericPhase::SetDownAltitude( double dAltitude )
{
	m_dDownAltitude = dAltitude;
}

double ns_FlightPlan::FlightPlanGenericPhase::GetDownAltitude() const
{
	return m_dDownAltitude;
}

void ns_FlightPlan::FlightPlanGenericPhase::SetAltitudeSeparate( double dSep )
{
	m_dAltitudeSep = dSep;
}

double ns_FlightPlan::FlightPlanGenericPhase::GetAltitudeSeparate() const
{
	return m_dAltitudeSep;
}

void ns_FlightPlan::FlightPlanGenericPhase::SetUpSpeed( double dSpeed )
{
	m_dUpSpeed = dSpeed;
}

double ns_FlightPlan::FlightPlanGenericPhase::GetUpSpeed() const
{
	return m_dUpSpeed;
}

void ns_FlightPlan::FlightPlanGenericPhase::SetDownSpeed( double dSpeed )
{
	m_dDownSpeed = dSpeed;
}

double ns_FlightPlan::FlightPlanGenericPhase::GetDownSpeed() const
{
	return m_dDownSpeed;
}	

void ns_FlightPlan::FlightPlanGenericPhase::SetPhase( Phase emPhase )
{
	m_phase = emPhase;
}

ns_FlightPlan::FlightPlanGenericPhase::Phase ns_FlightPlan::FlightPlanGenericPhase::GetPhase() const
{
	return m_phase;
}

void ns_FlightPlan::FlightPlanGenericPhase::SaveData(int nFlightPlanID)
{
	CString strSQL;
	if (m_nTableID < 0)
	{
		strSQL.Format(_T("INSERT INTO GENERICPROPERTY (DISTTORUNWAY, UPALTITUDE, DOWNALTITUDE,ALTITUDESEP,\
						 UPSPEED,DOWNSPEED,PHASE,FLIGHTPLANID) VALUES (%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%d)"),
			m_dDistToRunway, m_dUpAltitude, m_dDownAltitude,m_dAltitudeSep,m_dUpSpeed,m_dDownSpeed,\
			m_phase,nFlightPlanID);
		m_nTableID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(strSQL);
	}
	else
	{
		UpdateData();
	}
}

void ns_FlightPlan::FlightPlanGenericPhase::UpdateData()
{
	CString strSQL;
	strSQL.Format(_T("UPDATE GENERICPROPERTY SET DISTTORUNWAY = %.2f, UPALTITUDE = %.2f,DOWNALTITUDE = %.2f,\
					 ALTITUDESEP = %.2f,UPSPEED = %.2f,DOWNSPEED = %.2f,PHASE = %d WHERE ID = %d"),
		m_dDistToRunway, m_dUpAltitude, m_dDownAltitude, m_dAltitudeSep,m_dUpSpeed,m_dDownSpeed,m_phase,m_nTableID);
	CADODatabase::ExecuteSQLStatement(strSQL);
}

void ns_FlightPlan::FlightPlanGenericPhase::DeleteData()
{
	CString strSQL = _T("");
	strSQL.Format(_T("DELETE FROM GENERICPROPERTY WHERE (ID = %d);"),m_nTableID);

	CADODatabase::ExecuteSQLStatement(strSQL);
}

void ns_FlightPlan::FlightPlanGenericPhase::InitFromDBRecordset( CADORecordset& recordset )
{
	if (!recordset.IsEOF())
	{
		recordset.GetFieldValue(_T("ID"),m_nTableID);
		recordset.GetFieldValue(_T("DISTTORUNWAY"),m_dDistToRunway);
		recordset.GetFieldValue(_T("UPALTITUDE"),m_dUpAltitude);
		recordset.GetFieldValue(_T("DOWNALTITUDE"),m_dDownAltitude);
		recordset.GetFieldValue(_T("ALTITUDESEP"),m_dAltitudeSep);
		recordset.GetFieldValue(_T("UPSPEED"),m_dUpSpeed);
		recordset.GetFieldValue(_T("DOWNSPEED"),m_dDownSpeed);
		int nType = 0;
		recordset.GetFieldValue(_T("PHASE"),nType);
		m_phase = (Phase)nType;
	}
}

ns_FlightPlan::FlightPlanGenericPhaseBase::FlightPlanGenericPhaseBase()
{
	
}

ns_FlightPlan::FlightPlanGenericPhaseBase::~FlightPlanGenericPhaseBase()
{

}

//void ns_FlightPlan::GenericProfileComponent::AddItem( GenericProfileComponent* pItem )
//{
//
//}
//
//int ns_FlightPlan::GenericProfileComponent::GetTableID() const
//{
//	return m_nTableID;
//}
//
//GenericProfileComponent* ns_FlightPlan::GenericProfileComponent::GetItem( int nIdx )
//{
//	return NULL;
//}
//
//void ns_FlightPlan::GenericProfileComponent::DeleteItem( GenericProfileComponent* pItem )
//{
//
//}
//
//int ns_FlightPlan::GenericProfileComponent::GetCount()
//{
//	return 0;
//}