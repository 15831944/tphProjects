#include "StdAfx.h"
#include ".\airspaceinsim.h"

#include "../../Inputs/AirsideInput.h"
#include "../../AirsideInput/FlightPlans.h"

#include "../../AirsideInput/AirRoutes.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside/AirRoute.h"
#include "InputAirside/Hold.h"
#include "InputAirside/WayPoint.h"
#include "InputAirside/Runway.h"
#include "InputAirside/ALTAirspace.h"
#include "FlightInAirsideSimulation.h"
#include "InputAirside/AirRoute.h"

#include "../../Common/ARCUnit.h"
#include "RunwayInSim.h"
#include <math.h>

bool AirspaceInSim::Build()
{

	Clear();


	int nProjID = 0 ;
	int nAirspaceID = InputAirside::GetAirspaceID(nProjID);

	AirRouteSeg airRouteSeg;
	CString strStartID, strEndID;
	std::vector<int> vRouteList;
	ALTAirspace::GetAirRouteList(nAirspaceID,vRouteList);
	for(int i =0;i<(int)vRouteList.size();++i)
	{
		CAirRoute airRoute(vRouteList.at(i));
		//airRoute.ReadObject(vRouteList.at(i));
		airroute * pnewAiroute = new airroute;

		pnewAiroute->setStringID(airRoute.getName());
		pnewAiroute->m_RouteType =airRoute.getRouteType();
		for(int i=0;i<(int)airRoute.getARWaypoint().size()-1;i++)
		{ 
			airRouteSeg.m_pfix1 = (airRoute.getARWaypoint()).at(i);
			airRouteSeg.m_pfix2 = (airRoute.getARWaypoint()).at(i+1);
			pnewAiroute->m_vAirRouteSeg.push_back(&airRouteSeg);
		}
		pnewAiroute->m_RwID = (airRoute.getRunwayObj()).GetObjNameString() ;
		pnewAiroute->m_strID = airRoute.getName();
		m_vroutes.push_back(pnewAiroute);
	}



	std::vector<int> vHoldList;
	ALTAirspace::GetHoldList(nAirspaceID,vHoldList);
	for (int i = 0;i <int(vHoldList.size());++i)
	{
		AirHold airhold;
		airhold.ReadObject(vHoldList.at(i));
		hold * pnewHold = new hold;
		pnewHold->m_pHold = &airhold;
		m_vholds.push_back(pnewHold);

	}


	return true;

}
//
void AirspaceInSim::Clear()
{

	m_vholds.clear();
	m_vroutes.clear();
}

AirsideResource * AirspaceInSim::AllocateNextResource( FlightInAirsideSimulation * pFlight)
{

	if(!GetFlightPlanRoute(pFlight))
		return NULL;

	 

	for (int i =0; i< int( m_vPlanAirRoute.size() ) ;i++)
	{
		for (int j =0;j < m_vPlanAirRoute[i]->GetCount() - 1;j++)
		{
			if (m_vPlanAirRoute[i]->GetItem(j)->getName() == pFlight->GetCurrentResource()->getEndStringID())
			{
				AirWayPoint waypoint;
				Point pos;
				waypoint.ReadObject(m_vPlanAirRoute[i]->GetItem(j)->getID());
				waypoint.SetServicePoint(pos);
				m_pNextResource->m_StartPos.setX(pos.getX());
				m_pNextResource->m_StartPos.setY(pos.getY());
				m_pNextResource->m_StartPos.setZ( m_vPlanAirRoute[i]->GetItem(j)->getAltitude());

				waypoint.ReadObject(m_vPlanAirRoute[i]->GetItem(j+1)->getID());
				waypoint.SetServicePoint(pos);
				m_pNextResource->m_EndPos.setX(pos.getX());
				m_pNextResource->m_EndPos.setY(pos.getY());
				m_pNextResource->m_EndPos.setZ( m_vPlanAirRoute[i]->GetItem(j+1)->getAltitude());
				m_pNextResource->setResourceSpeedLimit( m_vPlanAirRoute[i]->GetItem(j)->getSpeed());
				m_pNextResource->setStringID(m_vPlanAirRoute[i]->GetItem(j)->getName(), m_vPlanAirRoute[i]->GetItem(j+1)->getName());

				if ((m_pNextResource->m_StartPos.getZ() - m_pNextResource->m_EndPos.getZ()) > 0 && m_pNextResource->m_strEndID == GetFlightPlanRunway(pFlight))
				{
					pFlight->setFlightState(OnApproach);
				}
				if ((m_pNextResource->m_StartPos.getZ() - m_pNextResource->m_EndPos.getZ()) > 0 && m_pNextResource->m_strEndID != GetFlightPlanRunway(pFlight))
				{
					pFlight->setFlightState(OnTerminal);
				} 
				if ((m_pNextResource->m_EndPos.getZ() - m_pNextResource->m_StartPos.getZ()) > 0)
				{
					pFlight->setFlightState(Climbout);
				}
				if ((m_pNextResource->m_EndPos.getZ() - m_pNextResource->m_StartPos.getZ()) == 0)
				{
					pFlight->setFlightState(OnCruise);
				}
				break;
			}		
		}	
	}
	return m_pNextResource;
	
}

bool AirspaceInSim::GetFlightPlanRoute(FlightInAirsideSimulation * pFlight)
{
	m_vPlanAirRoute.clear();

	ns_AirsideInput::CFlightPlans _flightPlan;
	FlightDescStruct& fltdesc = pFlight-> GetCFlight()->getLogEntry();
	for(int i=0;i< _flightPlan.GetItemCount();i++)
	{
		CString fltcont = _flightPlan.GetItem(i)->GetFltType() ;
		FlightConstraint fltconstraint ;
		fltconstraint.setConstraint((char*)((LPCTSTR)fltcont),VERSION_CONSTRAINT_CURRENT);
		if( fltconstraint.fits(fltdesc) )
		{
			m_vPlanAirRoute = _flightPlan.GetItem(i)->GetFltRouteList();
			return true;
		}
	}	
	return false;
}

CString AirspaceInSim::GetFlightPlanRunway(FlightInAirsideSimulation * pFlight)
{

	if(!GetFlightPlanRoute(pFlight))
		return "";

	CString RwID;
	int runwayid = m_vPlanAirRoute[0]->GetAirRoute()->getRunwayID();
	Runway runway;
	runway.ReadObject(runwayid);
	RwID = runway.GetObjNameString() ;
	return RwID;
}