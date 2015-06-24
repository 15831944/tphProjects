#include "StdAfx.h"
#include ".\runwaycontroller.h"

#include "AirportTrafficController.h"
#include "FlightInAirsideSimulation.h"
#include "../../Common/elaptime.h"
#include "StandTrafficController.h"
#include "StandInSim.h"
#include "RunwayInSim.h"
#include "../../Common/ARCUnit.h"
#include "FlightMovementsOnResource.h"
#include <cmath>
#include "FlightTakeoffOnRunway.h"
#include "AirsideController.h"

RunwayTrafficController::RunwayTrafficController( AirportTrafficController * pAirportController )
{
	m_pAirportController = pAirportController;
	m_pRunwayRes = NULL;
	
}

bool RunwayTrafficController::Init( int prjID, RunwayResourceManager * pRunwayRes ,CAirportDatabase* pAirportDatabase,StandResourceManager* pStandRes)
{
	m_pRunwayRes = pRunwayRes;
	m_pStandRes = pStandRes;
	m_pAirportDatabase = pAirportDatabase;
	m_RunwayExitAssignmentStrategies.Init(prjID,pAirportDatabase);
	m_RunwayAssignmentStrategies.Init(prjID,pAirportDatabase);
	return true;
}


RunwayExitInSim * RunwayTrafficController::ChooseRunwayExit(FlightInAirsideSimulation * pFlight, RunwayPortInSim * pRunway)
{
	
	if(pFlight->GetRunwayExit()) return pFlight->GetRunwayExit();

	StandInSim  * pStand  = m_pAirportController->GetStandTrafficController()->AssignStand(pFlight);
	ASSERT(pStand);
	IntersectionNodeInSim * destNode = pStand->GetInNode();
	ASSERT(destNode);
	//according to flight state choose a exit
	ASSERT(pRunway->GetExitCount()>0);
	//return pRunway->GetExit(pRunway->GetExitCount()-1);
	return m_RunwayExitAssignmentStrategies.GetRunwayExitByFlightType(pFlight,pRunway,m_pAirportDatabase,m_pStandRes);

}

RunwayPortInSim* RunwayTrafficController::AssignLandRunway( FlightInAirsideSimulation* pFlight )
{
	if(NULL == m_pRunwayRes)
		return NULL;

	if(m_pRunwayRes->GetRunwayCount())
	{
		return  m_RunwayAssignmentStrategies.GetArrivalRunway(pFlight,m_pRunwayRes);
	}
	return NULL;
}


Clearance* RunwayTrafficController::AssignNextClearance(FlightInAirsideSimulation * pFlight )
{
	if(pFlight->GetFlightMode() == OnBirth)
	{
		if(pFlight->IsArrival())
		{
			RunwayPortInSim * landRunway = AssignLandRunway(pFlight);
			
			if(landRunway)
			{			
				Clearance * newClearance = new Clearance;
				newClearance->m_nMode = OnLanding;
				newClearance->m_pResource = landRunway;
				newClearance->m_tTime = pFlight->GetCurrentTime();
				newClearance->m_vSpeed = pFlight->getFlightPerformanceManager()->getLandingSpeed(pFlight->GetCFlight());
				newClearance->m_dDist = 0;				
				m_pAirportController->AmendClearance(pFlight,newClearance);
				return  newClearance;			
			}
		}
		else if(pFlight->IsDeparture())
		{
			RunwayPortInSim * takeoffRunway = AssignTakeoffRunway(pFlight);
			if(takeoffRunway)
			{
				Clearance * newClearance = new Clearance;
				newClearance->m_nMode = OnTakeoff;
				newClearance->m_pResource = takeoffRunway;
				newClearance->m_tTime = pFlight->GetCurrentTime();
				m_pAirportController->AmendClearance(pFlight,newClearance);
				return newClearance;
			}
		}
		return NULL;

	}
	


	//Estimate exit time and exit speed
	if(pFlight->GetFlightMode() == OnLanding) // on landing choose a exit
	{		
		RunwayPortInSim * pRunway = (RunwayPortInSim * )pFlight->GetCurrentResource();

		RunwayExitInSim * rwExit = ChooseRunwayExit(pFlight,pRunway);
		ASSERT(rwExit);
		TaxiwayDirectSegInSim * pDirSeg = rwExit->m_pTaxiwayDirSeg.get();
		ASSERT(pDirSeg);	

		Clearance * newClearance  = new Clearance;
		newClearance->m_pResource = pDirSeg;
		newClearance->m_nMode = OnTaxiToStand;
		newClearance->m_tTime = pFlight->GetCurrentTime();
		newClearance->m_dDist = 0;
		m_pAirportController->AmendClearance(pFlight,newClearance);
		return newClearance;
		//return m_pAirportController->GetEnterResourceClearance(pFlight,landingOnRunway.GetEndState());
	}

	if(pFlight->GetFlightMode() == OnPreTakeoff)
	{
		RunwayPortInSim * pRunway = (RunwayPortInSim * )pFlight->GetCurrentResource();

		Clearance * newClearance = new Clearance(pFlight->GetFlightState());
		newClearance->m_pResource = pRunway;
		newClearance->m_nMode = OnTakeoff;		
		m_pAirportController->AmendClearance(pFlight,newClearance);
		return newClearance;			
	}
	
	return NULL;
}

RunwayPortInSim* RunwayTrafficController::AssignTakeoffRunway( FlightInAirsideSimulation* pFlight )
{
	AirspaceTrafficController * pAirspaceCtrl = m_pAirportController->GetAirsideTrafficController()->GetAirspaceController();	
	
	RunwayPortInSim * pPlanRunway = NULL;
	if(pAirspaceCtrl)
	{
		return m_RunwayAssignmentStrategies.GetDepartureRunway(pFlight,m_pRunwayRes);
	} 
	
	//default the first runway;
	if(m_pRunwayRes->GetRunwayCount())
	{
		return m_pRunwayRes->GetRunwayByIndex(0)->GetPort2();
	}
	return NULL;
}

IntersectionNodeInSim * RunwayTrafficController::GetTakeoffPosition( FlightInAirsideSimulation * pFlight ,RunwayPortInSim * pRunway)
{
	if( pRunway->GetExitCount() > 0)
	{
		return pRunway->GetExitAt(0)->m_pTaxiwayDirSeg->GetEntryNode();
	}	
	return NULL;
}

bool RunwayTrafficController::AmendClearance( FlightInAirsideSimulation* pFlight, Clearance * pClearance )
{
	ASSERT(pClearance && pClearance->m_pResource);

	//amend landing clearance
	if(pClearance->m_nMode == OnLanding && pClearance->m_pResource->GetType() == AirsideResource::ResType_RunwayPort)
	{
		RunwayPortInSim * pRunway =  (RunwayPortInSim*) pClearance->m_pResource;
		ASSERT(pRunway->IsActive());   // 
	
		OccupancyInstance lastOcy = pRunway->GetLastOccupyInstance();
		if(lastOcy.IsValid())
		{
			ElapsedTime sepT(1L);
			if(lastOcy.GetOccupyType() == OccupancyInstance::Takeoff){
				sepT = GetSeparationTakeoffBehindTakeoff(lastOcy.GetFlight(),pFlight);
			}
			if(lastOcy.GetOccupyType() == OccupancyInstance::Landing)
			{
				sepT = GetSeparationTakeoffBehindLanding(lastOcy.GetFlight(),pFlight);
			}
			if(lastOcy.IsExitTimeValid())
			{
				ElapsedTime dT = pClearance->m_tTime - lastOcy.GetExitTime();
				if(dT < sepT) 
				{
					pClearance->m_tTime += (sepT -dT);
				}		
			}
			else
			{
				pClearance->m_tTime = pFlight->GetCurrentTime() + sepT;
				pClearance->m_nMode = pFlight->GetFlightMode();
				pClearance->m_pResource = pFlight->GetCurrentResource();
				pClearance->m_dDist = pFlight->GetDistInResource();
			}
					
		}
		//check all the intersection nodes in the runway
		for(int i =0; i < pRunway->GetIntersectionList().GetNodeCount();i++)
		{
			IntersectionNodeInSim * pNode = pRunway->GetIntersectionList().GetNode(i);
			OccupancyInstance lastOcy = pNode->GetLastOccupyInstance();
			if(lastOcy.IsValid() && lastOcy.IsExitTimeValid())
			{
				ElapsedTime sepT(1L);
				if(lastOcy.GetOccupyType() == OccupancyInstance::Takeoff){
					sepT = GetSeparationLandingBehindTakeoff(lastOcy.GetFlight(),pFlight);
				}
				if(lastOcy.GetOccupyType() == OccupancyInstance::Landing)
				{
					sepT = GetSeparationLandingBehindLanding(lastOcy.GetFlight(),pFlight);
				}
				if(lastOcy.GetOccupyType() == OccupancyInstance::Taxiing)
				{
					
				}
				ElapsedTime dT = pClearance->m_tTime - lastOcy.GetExitTime();
				if(dT < sepT) 
				{
					pClearance->m_tTime += (sepT -dT);
				}				
			}
		}
		return true;		
	}
	//amend takeoff clearance
	if(pClearance->m_nMode == OnTakeoff && pClearance->m_pResource->GetType() == AirsideResource::ResType_RunwayPort)
	{
		RunwayPortInSim * pRunway = (RunwayPortInSim*)pClearance->m_pResource;
		//ASSERT(pRunway->IsActive());
		
		OccupancyInstance lastOcy = pRunway->GetLastOccupyInstance();
		if(lastOcy.IsValid() && lastOcy.IsExitTimeValid())
		{
			ElapsedTime sepT(1L);
			if(lastOcy.GetOccupyType() == OccupancyInstance::Takeoff){
				sepT = GetSeparationTakeoffBehindTakeoff(lastOcy.GetFlight(),pFlight);
			}
			if(lastOcy.GetOccupyType() == OccupancyInstance::Landing)
			{
				sepT = GetSeparationTakeoffBehindLanding(lastOcy.GetFlight(),pFlight);
			}	

			if(lastOcy.IsExitTimeValid())
			{
				ElapsedTime dT = pClearance->m_tTime - lastOcy.GetExitTime();
				if(dT < sepT) 
				{
					pClearance->m_tTime += (sepT -dT);
				}
			}
			else
			{
				pClearance->m_tTime = pFlight->GetCurrentTime() + sepT;
				pClearance->m_nMode = pFlight->GetFlightMode();
				pClearance->m_pResource = pFlight->GetCurrentResource();
				pClearance->m_dDist = pFlight->GetDistInResource();
			}
					
		}
		//
		for(int i=0;i<pRunway->GetIntersectionList().GetNodeCount();i++)
		{
			IntersectionNodeInSim * pNode = pRunway->GetIntersectionList().GetNode(i);
			OccupancyInstance lastOcy = pNode->GetLastOccupyInstance();
			if(lastOcy.IsValid() && lastOcy.IsExitTimeValid())
			{
				ElapsedTime sepT;
				if(lastOcy.GetOccupyType() == OccupancyInstance::Takeoff){
					sepT = GetSeparationTakeoffBehindTakeoff(lastOcy.GetFlight(),pFlight);
				}
				if(lastOcy.GetOccupyType() == OccupancyInstance::Landing)
				{
					sepT = GetSeparationTakeoffBehindLanding(lastOcy.GetFlight(),pFlight);
				}
				if(lastOcy.GetOccupyType() == OccupancyInstance::Taxiing)
				{

				}
				ElapsedTime dT = pClearance->m_tTime - lastOcy.GetExitTime();
				if(dT < sepT) 
				{
					pClearance->m_tTime += (sepT -dT);
				}				
			}

		}
		return true;		
	}
	//amend pre-takeoff clearance 
	if(pClearance->m_nMode == OnPreTakeoff && pClearance->m_pResource->GetType() == AirsideResource::ResType_RunwayPort) 
	{
		RunwayPortInSim * pRunway = (RunwayPortInSim*)pClearance->m_pResource;
		
		//	
		OccupancyInstance lastOcy = pRunway->GetLastOccupyInstance();
		if(lastOcy.IsValid() )
		{
			ElapsedTime sepT(1L);
			if(lastOcy.GetOccupyType() == OccupancyInstance::Takeoff){
				sepT = GetSeparationTakeoffBehindTakeoff(lastOcy.GetFlight(),pFlight);
			}
			if(lastOcy.GetOccupyType() == OccupancyInstance::Landing)
			{
				sepT = GetSeparationTakeoffBehindLanding(lastOcy.GetFlight(),pFlight);
			}	
			
			if( lastOcy.IsExitTimeValid()) 
			{
				ElapsedTime dT = pClearance->m_tTime - lastOcy.GetExitTime();
				if(dT < sepT) 
				{
					pClearance->m_tTime += (sepT -dT);
				}			
			}else
			{
				ElapsedTime dT  = pClearance->m_tTime - lastOcy.GetEnterTime();
				
				pClearance->m_pResource = pFlight->GetCurrentResource();
				pClearance->m_nMode = pFlight->GetFlightMode();
				pClearance->m_dDist = pFlight->GetDistInResource();
				if(dT < sepT) pClearance->m_tTime += sepT - dT;
				else pClearance->m_tTime += sepT;
			}
					
		}
		return true;		
	}
	return false;
}

ElapsedTime RunwayTrafficController::GetSeparationLandingBehindLanding( FlightInAirsideSimulation * leadflight, FlightInAirsideSimulation * trailflight, bool intersectRw /*= false*/ )
{
	return ElapsedTime(1L);
}

ElapsedTime RunwayTrafficController::GetSeparationLandingBehindTakeoff( FlightInAirsideSimulation * leadflight, FlightInAirsideSimulation * trailflight, bool intersectRw /*= false*/ )
{
	return ElapsedTime(1L);
}

ElapsedTime RunwayTrafficController::GetSeparationTakeoffBehindLanding( FlightInAirsideSimulation * leadflight, FlightInAirsideSimulation * trailflight, bool intersectRw /*= false*/ )
{
	return ElapsedTime(1L);
}

ElapsedTime RunwayTrafficController::GetSeparationTakeoffBehindTakeoff( FlightInAirsideSimulation * leadflight, FlightInAirsideSimulation * trailflight, bool intersectRw /*= false*/ )
{
	return ElapsedTime(1L);
}

RunwayPortInSim * RunwayTrafficController::GetLandingRunway( FlightInAirsideSimulation * pFlight )
{
	if (pFlight->GetPlanningFlow().GetLandingRunwayPort())
		return pFlight->GetPlanningFlow().GetLandingRunwayPort();

	return AssignLandRunway(pFlight);
}

RunwayPortInSim * RunwayTrafficController::GetTakeoffRunway( FlightInAirsideSimulation * pFlight )
{
	if (pFlight->GetPlanningFlow().GetTakeoffRunwayPort())
		return pFlight->GetPlanningFlow().GetTakeoffRunwayPort();
	return AssignTakeoffRunway(pFlight);
}

