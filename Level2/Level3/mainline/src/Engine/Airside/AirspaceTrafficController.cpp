#include "StdAfx.h"

#include "AirsideResource.h"
#include "AirspaceResourceManager.h"
#include "AirspaceTrafficController.h"
#include "FlightInAirsideSimulation.h"
#include "../../Common/ARCUnit.h"
#include "../../Common/elaptime.h"
#include "AirsideController.h"
#include "RunwayController.h"
#include "AirspaceResource.h"
#include "AircraftSeparationManager.h"
#include "../../InputAirside/FlightPlan.h"
#include "../../Common/AirportDatabaseList.h"
#include "./AirsideResourceManager.h"


AirspaceTrafficController::AirspaceTrafficController( AirsideTrafficController * pAirsideController )
{
	m_pAirsideController = pAirsideController;
	m_pflightPlanInput = NULL;
	m_pAirportDatabase = NULL;
}



bool AirspaceTrafficController::Init(int nPrjID, AirspaceResourceManager* pAirspaceRes, CAirportDatabase *pAirportDatabase)
{
	ASSERT(pAirportDatabase);
	m_pAirportDatabase = pAirportDatabase;

	m_pAirspaceRes = pAirspaceRes;	
	
	// 
	m_pflightPlanInput = new ns_FlightPlan::FlightPlans(nPrjID);
	m_pflightPlanInput->Read();

	m_SID_STARAssignment.Init(nPrjID);
    
	return true;
}


Clearance * AirspaceTrafficController::AssignNextClearance( FlightInAirsideSimulation * pFlight )
{
	std::vector<FlightPlanInSim::AtWayPointInfo> vRouteList;	
	if(pFlight->GetFlightMode() < OnHeldAtStand )
	{
		FlightPlanInSim& flightPlan = GetArrivalFlightPlan(pFlight);

		int nIntersectIdx = -1;

		for(int i =0 ;i < flightPlan.GetWayPointCount();i ++)
		{
			FlightPlanInSim::AtWayPointInfo& atWayptInfo = flightPlan.GetAtWayPointInfo(i);
			if(atWayptInfo.bArried)continue;
			if (atWayptInfo.fltMode == OnTerminal)
			{
				RunwayPortInSim* pPort = m_pAirsideController->GetAirportController()->GetRunwayTrafficController()->GetLandingRunway(pFlight);
				vRouteList = GetSTARWayPointList(pFlight,pPort);
				nIntersectIdx = GetIntersectWaypointIdx(vRouteList, flightPlan);
				if (nIntersectIdx >=0)
					break;
			}

			DistanceUnit dDist = pFlight->GetCurrentPosition().distance(atWayptInfo.pWayPoint->GetPosition());
			double  avgSpd = (pFlight->GetFlightState().m_vSpeed +  atWayptInfo.vSpeed ) * 0.5;
			ElapsedTime dT = dDist / avgSpd;

			Clearance * newClearance = new Clearance;
			newClearance->m_pResource = atWayptInfo.pWayPoint;
			newClearance->m_dAlt = atWayptInfo.dAlt;
			newClearance->m_nMode = atWayptInfo.fltMode;
			newClearance->m_tTime = pFlight->GetCurrentTime() +dT;
			newClearance->m_vSpeed = atWayptInfo.vSpeed;
			newClearance->m_dDist = 0;

			atWayptInfo.bArried = true;
			return newClearance;
		}
		if (nIntersectIdx >=0)
		{
			for (int i = nIntersectIdx; i>= 0; i++ )
			{
				FlightPlanInSim::AtWayPointInfo& atWayptInfo = vRouteList[i];

				DistanceUnit dDist = pFlight->GetCurrentPosition().distance(atWayptInfo.pWayPoint->GetPosition());
				double  avgSpd = (pFlight->GetFlightState().m_vSpeed +  atWayptInfo.vSpeed ) * 0.5;
				ElapsedTime dT = dDist / avgSpd;

				Clearance * newClearance = new Clearance;
				newClearance->m_pResource = atWayptInfo.pWayPoint;
				newClearance->m_dAlt = atWayptInfo.dAlt;
				newClearance->m_nMode = atWayptInfo.fltMode;
				newClearance->m_tTime = pFlight->GetCurrentTime() +dT;
				newClearance->m_vSpeed = atWayptInfo.vSpeed;
				newClearance->m_dDist = 0;

				atWayptInfo.bArried = true;
				return newClearance;
			}
		}

		if(pFlight->GetFlightMode()!= OnBirth  && pFlight->GetFlightMode() != OnFinalApproach)
		{ 
			Clearance * newClearnce = new Clearance;
			newClearnce->m_nMode = OnFinalApproach;
			newClearnce->m_pResource = pFlight->GetCurrentResource();
			newClearnce->m_tTime = pFlight->GetCurrentTime();
			newClearnce->m_dAlt = pFlight->GetFlightState().m_pPosition.getZ();
			newClearnce->m_vSpeed = pFlight->GetFlightState().m_vSpeed;
			newClearnce->m_dDist = pFlight->GetDistInResource();
			return newClearnce;
		}
		else if(pFlight->GetFlightMode() == OnFinalApproach || pFlight->GetFlightMode()== OnBirth)
		{
			RunwayPortInSim * pLandrunway = NULL;
			pLandrunway = m_pAirsideController->GetAirportController()->GetRunwayTrafficController()->GetLandingRunway(pFlight);
			
			if(pLandrunway)
			{
				Clearance * newClearnce  = new Clearance;

				newClearnce->m_nMode = OnLanding;
				newClearnce->m_pResource = pLandrunway;
				newClearnce->m_tTime = pFlight->GetCurrentTime();

				m_pAirsideController->AmendClearance(pFlight,newClearnce);

				return newClearnce;
			}
		}
	}
	
	if(pFlight->GetFlightMode() > OnHeldAtStand )
	{
		vRouteList = GetSIDWayPointList(pFlight,m_pAirsideController->GetAirportController()->GetRunwayTrafficController()->GetTakeoffRunway(pFlight));
		for (int i = 0; i <= int(vRouteList.size()); i++)
		{
			FlightPlanInSim::AtWayPointInfo& atWayptInfo = vRouteList[i];
			if(atWayptInfo.bArried)continue;

			DistanceUnit dDist = pFlight->GetCurrentPosition().distance(atWayptInfo.pWayPoint->GetPosition());
			double  avgSpd = (pFlight->GetFlightState().m_vSpeed +  atWayptInfo.vSpeed ) * 0.5;
			ElapsedTime dT = dDist / avgSpd;

			Clearance * newClearance = new Clearance;
			newClearance->m_pResource = atWayptInfo.pWayPoint;
			newClearance->m_dAlt = atWayptInfo.dAlt;
			newClearance->m_nMode = atWayptInfo.fltMode;
			newClearance->m_tTime = pFlight->GetCurrentTime() +dT;
			newClearance->m_vSpeed = atWayptInfo.vSpeed;
			newClearance->m_dDist = 0;

			atWayptInfo.bArried = true;
			return newClearance;
		}

		FlightPlanInSim& flightPlan = GetDepartureFlightPlan(pFlight);
		int nIntersectIdx = GetIntersectWaypointIdx(vRouteList, flightPlan);
		if (nIntersectIdx >=0)
		{
			for(int i = nIntersectIdx ;i < flightPlan.GetWayPointCount();i ++)
			{
				FlightPlanInSim::AtWayPointInfo& atWayptInfo = flightPlan.GetAtWayPointInfo(i);
				if(atWayptInfo.bArried)continue;

				DistanceUnit dDist = pFlight->GetCurrentPosition().distance(atWayptInfo.pWayPoint->GetPosition());
				double  avgSpd = (pFlight->GetFlightState().m_vSpeed +  atWayptInfo.vSpeed ) * 0.5;
				ElapsedTime dT = dDist / avgSpd;

				Clearance * newClearance = new Clearance;
				newClearance->m_pResource = atWayptInfo.pWayPoint;
				newClearance->m_dAlt = atWayptInfo.dAlt;
				newClearance->m_nMode = atWayptInfo.fltMode;
				newClearance->m_tTime = pFlight->GetCurrentTime() +dT;
				newClearance->m_vSpeed = atWayptInfo.vSpeed;
				newClearance->m_dDist = 0;

				atWayptInfo.bArried = true;
				return newClearance;
			}
		}


	}


	Clearance * newClearance = new Clearance;
	newClearance->m_tTime = pFlight->GetCurrentTime();
	newClearance->m_nMode = OnTerminate;
	return newClearance;
}



ElapsedTime AirspaceTrafficController::GetInTrailSeperationTime( FlightInAirsideSimulation* pLeadFlight, FlightInAirsideSimulation* pTrailFlight, FlightMode mode )
{
	CString ACTypeLead;
	CString ACTypeTrail;
	if(pLeadFlight){ pLeadFlight->GetCFlight()->getACType(ACTypeLead.GetBuffer(255)); }
	if(pTrailFlight){ pTrailFlight->GetCFlight()->getACType(ACTypeTrail.GetBuffer(255)); }

	DistanceUnit sepDist;
	ElapsedTime sepTime;
	if(mode == OnCruiseThrough || mode == OnCruiseArr || mode == OnCruiseDep ) // Cruise
	{
		sepDist = m_pSeparationManager->GetCruiseInTrailSeparationDist(ACTypeLead,ACTypeTrail);
		sepTime = m_pSeparationManager->GetCruiseInTrailSeparationTime(ACTypeLead,ACTypeTrail);		
	}
	if(mode == OnTerminal)
	{
		sepDist = m_pSeparationManager->GetTerminalInTrailSeparationDist(ACTypeLead,ACTypeTrail);
		sepTime = m_pSeparationManager->GetTerminalInTrailSeparationTime(ACTypeLead,ACTypeTrail);
	}
	if(mode == OnApproach)
	{
		sepDist = m_pSeparationManager->GetApproachInTrailSeparationDist(ACTypeLead,ACTypeTrail);
		sepTime = m_pSeparationManager->GetApproachInTrailSeparationTime(ACTypeLead,ACTypeTrail);
	}
	if(mode == OnFinalApproach)
	{
		sepDist = m_pSeparationManager->GetApproachInTrailSeparationDist(ACTypeLead,ACTypeTrail);
		sepTime = m_pSeparationManager->GetApproachInTrailSeparationTime(ACTypeLead,ACTypeTrail);
	}
	return max( sepTime, ElapsedTime(sepDist/pTrailFlight->GetFlightState().m_vSpeed) );
	
}
// decide next controller of the flight
TrafficController * AirspaceTrafficController::GetNextController( FlightInAirsideSimulation* pFlight )
{
	if(pFlight->GetFlightMode() == OnFinalApproach)
	{
		return m_pAirsideController->GetAirportController()->GetRunwayTrafficController();
	}
	return NULL;
}

//Flight Plan
FlightPlanInSim::FlightPlanInSim()
{
	m_pRunway = NULL;
	bInitialized = false;
}

AirspaceTrafficController::~AirspaceTrafficController()
{
	delete m_pflightPlanInput;
}

bool AirspaceTrafficController::AmendClearance( FlightInAirsideSimulation * pFlight, Clearance* pClearance )
{
	if(pClearance->m_pResource->GetType()== AirsideResource::ResType_WayPoint ){
		AirWayPointInSim * pWayPoint = (AirWayPointInSim*)pClearance->m_pResource;
		OccupancyInstance lastOcy  = pWayPoint->GetLastOccupyInstance();
		if(lastOcy.IsValid())
		{
			ElapsedTime sepT(1L);
			FlightInAirsideSimulation *pPreFlight = lastOcy.GetFlight();
			sepT = GetInTrailSeperationTime(pPreFlight,pFlight,pFlight->GetFlightMode());
			
			ElapsedTime dFlightSep = pClearance->m_tTime - lastOcy.GetEnterTime();
			if(dFlightSep < sepT)
			{
				pClearance->m_tTime = lastOcy.GetEnterTime() + sepT;
			}			
		}
	}
	return true;
}

FlightPlanInSim& AirspaceTrafficController::GetArrivalFlightPlan( FlightInAirsideSimulation * pFlight )
{
	if( ! m_vArrivalFlightPlans[pFlight].IsInitialized() )
	{

		for(int i=0 ;i< (int)m_pflightPlanInput->GetFlightPlanCount();i++)
		{
			
			ns_FlightPlan::FlightPlan * pfltPlan  = m_pflightPlanInput->GetItem(i);			
			if(pfltPlan->GetOperationType() != ns_FlightPlan::FlightPlan::landing) continue;

			FlightConstraint fltCnst;

			//fltCnst.SetAirportDB( AIRPORTDBLIST->getAirportDBByIndex(0) );
			ASSERT(m_pAirportDatabase);
			fltCnst.SetAirportDB(m_pAirportDatabase);
			fltCnst.setConstraint(pfltPlan->GetFlightType(),VERSION_CONSTRAINT_CURRENT);
			

			if(	fltCnst.fits(pFlight->GetCFlight()->getLogEntry()) )
			{
				m_vArrivalFlightPlans[pFlight].Init(*pfltPlan,m_pAirsideController->GetAirsideResource(),true);
			}

		}	

	}
	if ( !pFlight->GetPlanningFlow().GetArrivalAirRoute() == NULL)
	{
		pFlight->GetPlanningFlow().SetArrivalAirRoute( m_vArrivalFlightPlans[pFlight]);
	}
	return m_vArrivalFlightPlans[pFlight];
}

FlightPlanInSim& AirspaceTrafficController::GetDepartureFlightPlan( FlightInAirsideSimulation * pFlight )
{
	if( ! m_vDepartureFlightPlans[pFlight].IsInitialized() )
	{

		for(int i=0 ;i< (int)m_pflightPlanInput->GetFlightPlanCount();i++)
		{
			ns_FlightPlan::FlightPlan * pfltPlan  = m_pflightPlanInput->GetItem(i);	
			if(pfltPlan->GetOperationType() != ns_FlightPlan::FlightPlan::takeoff ) continue;

			FlightConstraint fltCnst;

			//fltCnst.SetAirportDB( AIRPORTDBLIST->getAirportDBByIndex(0) );
			ASSERT(m_pAirportDatabase);
			fltCnst.SetAirportDB(m_pAirportDatabase);
			fltCnst.setConstraint(pfltPlan->GetFlightType(),VERSION_CONSTRAINT_CURRENT);

			if(	fltCnst.fits(pFlight->GetCFlight()->getLogEntry()) )
			{
				m_vDepartureFlightPlans[pFlight].Init(*pfltPlan,m_pAirsideController->GetAirsideResource(),false);
			}
		}	

	}
	if ( pFlight->GetPlanningFlow().GetDepartureAirRoute() == NULL)
	{
		pFlight->GetPlanningFlow().SetDepartureAirRoute( m_vDepartureFlightPlans[pFlight]);
	}
	return m_vDepartureFlightPlans[pFlight];
}

AtWayPointList& AirspaceTrafficController::GetSTARWayPointList(FlightInAirsideSimulation* pFlight,RunwayPortInSim* pRunwayPort)
{
	CAirRoute* pSTAR = pFlight->GetPlanningFlow().GetSTAR();
	if (pSTAR ==NULL)
	{	
		std::vector<FlightPlanInSim::AtWayPointInfo> vSTARWayPointList; 
		vSTARWayPointList.clear();
		AirWayPointInSimList WayPointList;

		pSTAR = m_SID_STARAssignment.GetSTARRoute(pFlight,pRunwayPort,m_pAirportDatabase,m_pAirspaceRes);
		int nCount = pSTAR->GetWayPointCount();	

		double fltSpeed = (pFlight->getFlightPerformanceManager()->getMaxTerminalSpeed(pFlight->GetCFlight())
			- pFlight->getFlightPerformanceManager()->getMinTerminalSpeed(pFlight->GetCFlight()))/3.0 
			+ pFlight->getFlightPerformanceManager()->getMinTerminalSpeed(pFlight->GetCFlight()) ;

		double DecelStep = double((pFlight->GetFlightState().m_vSpeed - fltSpeed)/nCount);
		if (DecelStep < 0.0)
			DecelStep = 0.0;

		for (int i =0; i < nCount; i++)
		{
			int nID = pSTAR->GetWayPointByIdx(i)->getWaypoint().getID() ;
			FlightPlanInSim::AtWayPointInfo atWayPoint(WayPointList.GetWayPoint(nID));
			atWayPoint.dAlt = (pSTAR->GetWayPointByIdx(i)->getMaxHeight() + pSTAR->GetWayPointByIdx(i)->getMinHeight())/2.0;
			if (atWayPoint.dAlt <= 0)
				atWayPoint.dAlt = (pSTAR->GetWayPointByIdx(i)->getWaypoint().GetLowLimit() +  pSTAR->GetWayPointByIdx(i)->getWaypoint().GetHighLimit())/2.0;
			atWayPoint.vSpeed = pFlight->GetFlightState().m_vSpeed - DecelStep * i;
			if (i < nCount -1)
				atWayPoint.fltMode = OnTerminal;
			if (i == nCount -1)
				atWayPoint.fltMode = OnFinalApproach;

			vSTARWayPointList.push_back(atWayPoint);

		}
		m_vSTARRoute.insert(map<CAirRoute*,AtWayPointList>::value_type(pSTAR,vSTARWayPointList));
		pFlight->GetPlanningFlow().SetSTAR(pSTAR);
	}


	return m_vSTARRoute[pSTAR];

}

AtWayPointList& AirspaceTrafficController::GetSIDWayPointList(FlightInAirsideSimulation* pFlight,RunwayPortInSim* pRunwayPort)
{
	CAirRoute* pSID = pFlight->GetPlanningFlow().GetSID();

	if (pSID ==NULL)
	{	
		std::vector<FlightPlanInSim::AtWayPointInfo> vSIDWayPointList;
		vSIDWayPointList.clear();
		AirWayPointInSimList WayPointList;

		pSID = m_SID_STARAssignment.GetSIDRoute(pFlight,pRunwayPort,m_pAirportDatabase,m_pAirspaceRes);
		int nCount = pSID->GetWayPointCount();	

		double ClimbSpeed = pFlight->getFlightPerformanceManager()->getClimboutSpeed(pFlight->GetCFlight()) ;
		double CruiseSpeed = (pFlight->getFlightPerformanceManager()->getMinCruiseSpeed(pFlight->GetCFlight()) 
			+ pFlight->getFlightPerformanceManager()->getMaxCruiseSpeed(pFlight->GetCFlight()))/2.0;

		for (int i =0; i < nCount; i++)
		{
			int nID = pSID->GetWayPointByIdx(i)->getWaypoint().getID() ;
			FlightPlanInSim::AtWayPointInfo atWayPoint(WayPointList.GetWayPoint(nID));

			atWayPoint.dAlt = (pSID->GetWayPointByIdx(i)->getMaxHeight() + pSID->GetWayPointByIdx(i)->getMinHeight())/2.0;
			if (atWayPoint.dAlt <= 0)
				atWayPoint.dAlt = (pSID->GetWayPointByIdx(i)->getWaypoint().GetLowLimit() +  pSID->GetWayPointByIdx(i)->getWaypoint().GetHighLimit())/2.0;
			if (i = 0)
			{
				atWayPoint.vSpeed = ClimbSpeed;
				atWayPoint.fltMode = OnClimbout;
			}
			if (i > 0)
			{
				atWayPoint.vSpeed = CruiseSpeed;
				atWayPoint.fltMode = OnCruiseDep;
			}
			vSIDWayPointList.push_back(atWayPoint);

		}
		m_vSIDRoute.insert(map<CAirRoute*,AtWayPointList>::value_type(pSID,vSIDWayPointList));
		pFlight->GetPlanningFlow().SetSID(pSID);

	}

	return m_vSIDRoute[pSID] ;

}

int AirspaceTrafficController::GetIntersectWaypointIdx(std::vector<FlightPlanInSim::AtWayPointInfo>& vRoute, FlightPlanInSim& PlanRoute)
{
	int idx = -1;
	if (vRoute[0].fltMode == OnTerminal)							//STAR
	{
		AirWayPointInSim* pItem = NULL;
		for (int i = PlanRoute.GetWayPointCount()-1; i >= 0; i--)
		{
			if (PlanRoute.GetAtWayPointInfo(i).fltMode == OnTerminal)
			{
				pItem = PlanRoute.GetAtWayPointInfo(i).pWayPoint;
				break;
			}
		}
		if (pItem)
		{

			for (int i = 0; i < int(vRoute.size()); i++)
			{
				if (vRoute[i].pWayPoint == pItem)
					return i;
				
			}
		}
	}

	if (vRoute[0].fltMode == OnClimbout)					//SID
	{
		int num = int(vRoute.size());
		AirWayPointInSim* pItem = vRoute[num-1].pWayPoint;
		for (int i = 0; i < PlanRoute.GetWayPointCount(); i++)
		{
			if (PlanRoute.GetAtWayPointInfo(i).pWayPoint == pItem)
				return i;
		}
	}

	return -1;

}