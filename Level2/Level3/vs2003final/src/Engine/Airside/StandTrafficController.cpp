#include "StdAfx.h"

#include "StandTrafficController.h"
#include "StandResourceManager.h"
#include "FlightInAirsideSimulation.h"
#include "AirsideResource.h"
#include "../../Common/elaptime.h"
#include "AirportTrafficController.h"

StandTrafficController::StandTrafficController( AirportTrafficController * pAirportController )
{
	m_pAirportController = pAirportController;
}



StandInSim * StandTrafficController::AssignStand( FlightInAirsideSimulation* pFlight )
{

	if(pFlight->GetPlanningFlow().GetParkingStand())
		return pFlight->GetPlanningFlow().GetParkingStand();

	ALTObjectID standID = pFlight->GetCFlight()->getStand();
	StandInSim * pStand = m_pStandRes->GetStandByName(standID);

	if(pStand){
		pFlight->GetPlanningFlow().SetParkingStand(pStand);
		return pStand;
	}

	//get a free stand
	for(int i=0 ;i< m_pStandRes->GetStandCount();i++)
	{
		StandInSim * pStand = m_pStandRes->GetStandByIdx(i);
		OccupancyInstance ocyInstance = pStand->GetLastOccupyInstance();
		if( !ocyInstance.IsValid() )
		{
			pFlight->GetPlanningFlow().SetParkingStand(pStand);
			return pStand;
		}
		else if(ocyInstance.IsExitTimeValid())
		{
			pFlight->GetPlanningFlow().SetParkingStand(pStand);
			return pStand;
		}
	}
		
	if(m_pStandRes->GetStandCount()>0)
	{
		pFlight->GetPlanningFlow().SetParkingStand(m_pStandRes->GetStandByIdx(0));
		m_pStandRes->GetStandByIdx(0);
	}	

	return NULL;
	
}

Clearance* StandTrafficController::AssignNextClearance( FlightInAirsideSimulation * pFlight )
{
	
	if(pFlight->GetFlightMode() == OnBirth)
	{
		Clearance * newClearance = new Clearance;
		StandInSim * pStand = AssignStand(pFlight);		
		if(pFlight->IsArrival())
		{				
			newClearance->m_nMode = OnHeldAtStand;
			newClearance->m_pResource = pStand;
			newClearance->m_tTime = pFlight->GetCurrentTime();				
		}
		else if(pFlight->IsDeparture())
		{			
			
			newClearance->m_nMode = OnHeldAtStand;
			newClearance->m_tTime = pFlight->GetCurrentTime();
			newClearance->m_pResource = pStand;
			//get exit taxiway segment
			
		}
		m_pAirportController->AmendClearance(pFlight,newClearance);			
		
		return newClearance;
	}

	if(pFlight->GetFlightMode() == OnHeldAtStand)
	{
		StandInSim *pStand = (StandInSim*)pFlight->GetCurrentResource();
		if(pFlight->IsDeparture())
		{		
			Clearance *newClearance = new Clearance;
			newClearance->m_tTime = max(pFlight->GetCurrentTime(), pFlight->GetCFlight()->getDepTime() );
			newClearance->m_nMode = OnExitStand;
			newClearance->m_pResource = pStand;
			m_pAirportController->AmendClearance(pFlight,newClearance);
			return newClearance;
		}else 
		{
			Clearance *newClearance = new Clearance;
			newClearance->m_tTime = pFlight->GetCurrentTime() + pFlight->GetCFlight()->getServiceTime();
			newClearance->m_nMode = OnTerminate;			
			return newClearance;
		}
	}
	if(pFlight->GetFlightMode() == OnExitStand)
	{
		StandInSim *pStand = (StandInSim*)pFlight->GetCurrentResource();
		Clearance *newClearance = new Clearance;
		newClearance->m_tTime = pFlight->GetCurrentTime();
		newClearance->m_nMode = OnTaxiToRunway;
		m_pAirportController->AmendClearance(pFlight,newClearance);
		return newClearance;
	}
	

	//if(pFlight->GetFlightMode() == OnEnterStand)
	//{
	//	StandInSim * pStand = (StandInSim * )pFlight->GetCurrentResource();
	//	double dDist = pStand->GetInPath().GetTotalLength();
	//	double avgspd = pFlight->GetFlightState().m_vSpeed * 0.5;
	//	ElapsedTime dT = ElapsedTime(dDist /avgspd );

	//	Clearance * pNewClearance  = new Clearance;
	//	pNewClearance->m_dDist = pStand->GetHeldDist();
	//	pNewClearance->m_pResource = pFlight->GetFlightState().m_pResource.get();
	//	pNewClearance->m_nMode = OnHeldAtStand;

	//	pNewClearance->m_tTime = pFlight->GetCurrentTime() + dT;
	//	pNewClearance->m_vSpeed = 0;
	//	pNewClearance->m_dAlt = 0;
	//	return pNewClearance;
	//}
	//if(pFlight->GetFlightMode() == OnHeldAtStand)
	//{
	//	if(pFlight->IsDeparture())
	//	{
	//		StandInSim * pStand = (StandInSim *) pFlight->GetCurrentResource();
	//		Clearance * pNewClearance  = new Clearance;
	//		pNewClearance->m_dDist = pStand->GetOutDist();
	//		pNewClearance->m_pResource = pFlight->GetFlightState().m_pResource.get();
	//		pNewClearance->m_nMode = OnExitStand;
	//		pNewClearance->m_tTime = max( pFlight->GetCFlight()->getDepTime(),pFlight->GetCurrentTime()) ;
	//		pNewClearance->m_vSpeed = 0;
	//		pNewClearance->m_dAlt = 0;
	//		return pNewClearance;
	//	}else 
	//	{
	//		return NULL;
	//	}
	//}

	//if(pFlight->GetFlightMode() == OnExitStand)
	//{
	//	StandInSim  * pStand = (StandInSim*)pFlight->GetCurrentResource();
	//	AirsideResource * nextRes = pStand->GetOutNode();

	//	FlightState nextState;
	//	nextState.m_pResource = pStand->GetOutNode();
	//	//nextState.m_dAlt = 0 ;
	//	nextState.m_dist = 0;
	//	nextState.m_fltMode = OnTaxiToRunway;
	//	nextState.m_tTime = pFlight->GetCurrentTime() + GetPushBackTime(pFlight);
	//	nextState.m_vSpeed = GetPushBackSpeed(pFlight);

	//	return m_pAirportController->GetEnterResourceClearance(pFlight,nextState);

	//
	//}
	
	
	
	return NULL;
}

bool StandTrafficController::Init( int nprjId,StandResourceManager * pStandRes )
{
	m_pStandRes = pStandRes;
	return true;
}



ElapsedTime StandTrafficController::GetStandBufferTime(FlightInAirsideSimulation* pLeadFlight, FlightInAirsideSimulation* pFollowFlight)
{
	ElapsedTime tSeparation = 20L;

	return tSeparation;
}

ElapsedTime StandTrafficController::GetPushBackTime(FlightInAirsideSimulation* pFlight)
{
	ElapsedTime tPushBack;
	
	return ElapsedTime(120L);

	return tPushBack;
}

//Clearance * StandTrafficController::GetEnterResourceClearance( FlightInAirsideSimulation* pFlight,const FlightState& fltstate )
//{	
//	const AirsideResource * pRes = fltstate.m_pResource.get();
//	ASSERT(pRes); if(!pRes) return NULL;
//	if(pRes->GetType() == AirsideResource::ResType_Stand)
//	{
//		StandInSim * pStand = (StandInSim*)pRes;
//
//		EnterSolution entersolution = GetEnterStandSolution(pFlight,pStand,fltstate.m_tTime);
//		ASSERT(entersolution.m_tEnterWaitTime>ElapsedTime(0L));
//
//		if(entersolution.IsEnterApproved())
//		{
//			Clearance * newClearance = new Clearance(fltstate);
//			newClearance->m_tTime = entersolution.GetEnterTime();
//			ModifyOcuppyTable(pFlight,pFlight->GetCurrentResource(),pStand,newClearance->m_tTime);
//			return newClearance;
//		}
//		else
//		{
//			Clearance * newClearance = new Clearance(pFlight->GetFlightState());
//			newClearance->m_tTime += entersolution.GetWaitTime();
//			return newClearance;	
//		}	
//	}			
//	return NULL;
//}
//
//
double StandTrafficController::GetPushBackSpeed( FlightInAirsideSimulation *pFlight )
{
	return  pFlight->getFlightPerformanceManager()->getTaxiOutSpeed(pFlight->GetCFlight());
}

bool StandTrafficController::AmendClearance( FlightInAirsideSimulation * pFlight, Clearance* pClearance )
{
	
	
	if(pClearance->m_nMode == OnHeldAtStand)
	{
		StandInSim * pStand = (StandInSim*)pClearance->m_pResource;
		OccupancyInstance lastOcy = pStand->GetLastOccupyInstance();
		if(lastOcy.IsValid() )
		{
			if(lastOcy.GetFlight() == pFlight) return true;
			ElapsedTime sepT = GetStandBufferTime(lastOcy.GetFlight(),pFlight);
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
				pClearance->m_pResource = pFlight->GetCurrentResource();
				pClearance->m_dDist = pFlight->GetDistInResource();
				pClearance->m_nMode = pFlight->GetFlightMode();
				pClearance->m_tTime += sepT;
			}		
		}
	}	
	
	return true;
}



//EnterSolution StandTrafficController::GetEnterStandSolution( FlightInAirsideSimulation * pFlight, StandInSim * pStand,const ElapsedTime& enterT )
//{
//	EnterSolution reslt;
//	reslt.m_bEnterApproval = true ;
//	reslt.m_tEnterWaitTime = enterT;
//
//	OccupancyInstance ocyInstance = pStand->GetLastOccupyInstance();
//	if(ocyInstance.IsValid())  // last flight landing on this Runway
//	{		
//		ElapsedTime sepT = GetStandBufferTime(ocyInstance.GetFlight(),pFlight);		
//		if(!ocyInstance.IsExitTimeValid())  // still not exit
//		{
//			reslt.m_bEnterApproval = false;
//			reslt.m_tEnterWaitTime = sepT + ocyInstance.GetFlight()->GetNextClearance()->m_tTime - enterT;
//		}else    // already know exit time 
//		{			
//			ElapsedTime dT =  enterT - ocyInstance.GetExitTime();		
//			reslt.m_bEnterApproval = true;	
//			if(dT < sepT){			
//				reslt.m_tEnterWaitTime = sepT + ocyInstance.GetExitTime();
//			}			
//		}
//	}	
//	ASSERT(reslt.m_tEnterWaitTime > ElapsedTime(0L));
//	return reslt;
//}
