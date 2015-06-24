#include "StdAfx.h"


#include "FlightMovementsOnResource.h"


#include "TaxiwayResource.h"
#include "../../Common/GetIntersection.h"
#include "StandInSim.h"
#include "RunwayInSim.h"
#include "./AirspaceResource.h"
#include <cmath>


ElapsedTime FlightMovements::GetElapsedTime() const
{
	if(m_vMovements.size()>0){
		return m_vMovements.back().m_tTime - m_beginState.m_tTime;
	}
	else 
		return ElapsedTime(0L);

}

void FlightMovements::BeginState( const FlightState& _fltstate )
{
	m_beginState = _fltstate;
	m_vMovements.clear();
}

void FlightMovements::End( Clearance * pClearance )
{	
	FlightState fltstate(pClearance);
	
	m_vMovements.push_back(fltstate);		
}

FlightState FlightMovements::GetEndState() const
{
	if(m_vMovements.size()<1) 
		return m_beginState;
	else
		return m_vMovements.back();
}
// movements from begin to end at a clearance
void FlightLanding::End( Clearance * pClearance )
{
	AirsideResource * nextRes = pClearance->m_pResource;
	RunwayPortInSim * pRunway = (RunwayPortInSim*)(m_pFlight->GetCurrentResource());

	DistanceUnit TouchDist = m_pFlight->getFlightPerformanceManager()->getDistThresToTouch(m_pFlight->GetCFlight());	

	FlightState fltstate = m_pFlight->GetFlightState();
	//touch down what ever
	Point touchPos = pRunway->GetDistancePoint(TouchDist);
	ARCVector3 vDir = touchPos - fltstate.m_pPosition;
	vDir[VZ] = 0;
	DistanceUnit dDist = vDir.length();
	ElapsedTime dT = ElapsedTime(dDist/fltstate.m_vSpeed);
	fltstate.m_tTime += dT;
	fltstate.m_vDirection = vDir;
	fltstate.m_pPosition = touchPos;
	fltstate.m_dist = TouchDist;
	fltstate.m_pResource = pRunway;
	m_vMovements.push_back(fltstate);
	
	//Cleared to the enter taxiway
	if( nextRes && nextRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
	{
		TaxiwayDirectSegInSim * pTaxiSeg = (TaxiwayDirectSegInSim *)nextRes;

		//decel to exit		
		IntersectionNodeInSim * pExitNode = pTaxiSeg->GetEntryNode();
		Point exitPoint = pExitNode->GetDistancePoint(0);
		double exitspd = m_pFlight->getFlightPerformanceManager()->getExitSpeed(m_pFlight->GetCFlight());
		double avgspd = (exitspd  + fltstate.m_vSpeed) * 0.5;
		vDir = exitPoint - fltstate.m_pPosition;
		vDir[VZ] = 0 ;
		dDist = vDir.length();
		dT = ElapsedTime(dDist/avgspd);
		fltstate.m_pPosition = exitPoint;
		fltstate.m_vSpeed = exitspd;
		fltstate.m_tTime += dT;
		fltstate.m_dist += dDist;
		m_vMovements.push_back(fltstate);
		//wait at the node until the clearance
		if(fltstate.m_tTime < pClearance->m_tTime)
		{
			fltstate.m_tTime = pClearance->m_tTime;
			m_vMovements.push_back(fltstate);
		}

		//end to the begin of exit
		Point enterPoint = pTaxiSeg->GetDistancePoint(0);
		vDir = enterPoint - fltstate.m_pPosition;
		dDist = vDir.length();
		dT = ElapsedTime(dDist/fltstate.m_vSpeed);
		fltstate.m_tTime += dT;
		fltstate.m_vDirection = vDir;
		fltstate.m_pPosition = enterPoint;
		fltstate.m_fltMode = OnTaxiToStand;
		fltstate.m_pResource = pTaxiSeg;
		fltstate.m_dist = 0;
		m_vMovements.push_back(fltstate);
	}

	

}

void FlightFinalApproach::End( Clearance * pClearance )
{
	ASSERT( pClearance->m_pResource && pClearance->m_pResource->GetType() == AirsideResource::ResType_RunwayPort ) ;

	RunwayPortInSim * pRunway = (RunwayPortInSim*)pClearance->m_pResource;

	double landingSpd = m_pFlight->getFlightPerformanceManager()->getLandingSpeed(m_pFlight->GetCFlight());
	DistanceUnit touchDist = m_pFlight->getFlightPerformanceManager()->getDistThresToTouch(m_pFlight->GetCFlight());
	DistanceUnit ApprochDistToRunway = 100;
	Point ApproachPos = pRunway->GetDistancePoint(-ApprochDistToRunway);
	ApproachPos.setZ((ApprochDistToRunway + touchDist) * std::tan(ARCMath::DegreesToRadians(3)));
	ARCVector3 ApproachDir = pRunway->GetDirection();
	ApproachDir[VZ] =  -std::tan(ARCMath::DegreesToRadians(3));

	FlightState fltstate = m_beginState;

	if(fltstate.m_fltMode == OnBirth)  //start state is on birth ,assume some position
	{
		fltstate.m_fltMode = OnFinalApproach;
		fltstate.m_pPosition = ApproachPos;
		fltstate.m_vSpeed = landingSpd;
		fltstate.m_vDirection = ApproachDir;		
		m_vMovements.push_back(fltstate);
	}
	//else if(fltstate.m_fltMode == OnFinalApproach)    // to the final approach position
	//{
	//	//arrival to approach dist
	//	double avgSpd = (fltstate.m_vSpeed + landingSpd) *0.5;
	//	ARCVector3 dVector = ApproachPos - fltstate.m_pPosition;
	//	DistanceUnit dDist = dVector.length();
	//	ElapsedTime dT = ElapsedTime(dDist /avgSpd);
	//	fltstate.m_pPosition = ApproachPos;
	//	fltstate.m_pResource = pRunway;
	//	fltstate.m_tTime += dT;
	//	fltstate.m_vDirection = ApproachDir;
	//	fltstate.m_vSpeed = landingSpd;
	//	m_vMovements.push_back(fltstate);
	//}

	//end at the threshold
	Point landingPos = pRunway->GetDistancePoint(0);
	DistanceUnit dDist = fltstate.GetPosition().distance(landingPos);
	ElapsedTime dT = ElapsedTime(dDist/landingSpd);
	fltstate.m_fltMode = OnLanding;
	fltstate.m_pPosition = pRunway->GetDistancePoint(0);
	fltstate.m_pPosition.setZ(touchDist *  std::tan(ARCMath::DegreesToRadians(3)) );
	fltstate.m_pResource = pRunway;
	fltstate.m_tTime += dT;
	m_vMovements.push_back(fltstate);

	//adjust delay time
	ElapsedTime delayT  = pClearance->m_tTime - fltstate.m_tTime;
	
	ASSERT(GetStateCount());
	//GetState(0).m_tTime += delayT;

}