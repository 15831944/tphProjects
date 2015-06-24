#include "StdAfx.h"


#include "FlightTakeoffOnRunway.h"
#include "AirspaceResource.h"



void FlightTakeoff::End( Clearance * pClearance )
{
	RunwayPortInSim  *pRunway  = (RunwayPortInSim*)m_pFlight->GetCurrentResource();
	FlightState fltstate = m_pFlight->GetFlightState();	
	ARCVector3 vRwDir = pRunway->GetDirection();

	ElapsedTime dT;
	DistanceUnit dDist;

	if(pClearance->m_nMode == OnTerminate)
	{	
		//Climb out
		double climbspdV = m_pFlight->getFlightPerformanceManager()->getClimboutSpeedVertical(m_pFlight->GetCFlight());
		double climbspdH = m_pFlight->getFlightPerformanceManager()->getClimboutSpeed(m_pFlight->GetCFlight());

		double climbAltitude = 25000;   // determinant later
		dT = ElapsedTime(climbAltitude / climbspdV) ; 

		double avgSpdH = (climbspdH + fltstate.m_vSpeed) * 0.5;

		dDist = avgSpdH * (double)dT;           // dist to climbout    

		fltstate.m_tTime += dT;
		fltstate.m_dist += dDist;
		fltstate.m_pPosition += Point(vRwDir[VX],vRwDir[VY],0) * dDist;
		fltstate.m_pPosition.setZ(climbAltitude);
		fltstate.m_vSpeed = climbspdH;
		fltstate.m_fltMode = OnClimbout;
		m_vMovements.push_back(fltstate);	
	}
	else
	{
		FlightMovements::End(pClearance);
	}
	

}



void FlightTakeoff::EndAtClimbUp()
{
	// pre conditions
	AirsideResource * pCurRes = m_beginState.m_pResource.get();
	ASSERT(pCurRes->GetType() ==AirsideResource::ResType_RunwayPort);
	RunwayPortInSim * pRunway = (RunwayPortInSim*)pCurRes;
	//

	FlightState fltstate = m_beginState;
	//taxi to takeoff position
	fltstate.m_tTime += ElapsedTime(10L);
	m_vMovements.push_back(fltstate);	

	//start take off at time
	fltstate.m_tTime += ElapsedTime(10L);
	m_vMovements.push_back(fltstate);

	//lift off 
	double acelspd  = m_pFlight->getFlightPerformanceManager()->getTakeoffAcceleration(m_pFlight->GetCFlight());
	double liftspd  = m_pFlight->getFlightPerformanceManager()->getClimboutSpeed(m_pFlight->GetCFlight());
	ElapsedTime dT = ElapsedTime( (liftspd - fltstate.m_vSpeed) / acelspd );
	double dDist = (liftspd + fltstate.m_vSpeed) * 0.5 * dT.asSeconds();
	fltstate.m_dist += dDist;
	fltstate.m_vSpeed = liftspd;
	//fltstate.m_dAlt = 0;
	fltstate.m_tTime +=dT;

	m_vMovements.push_back(fltstate);

	// start climb out	

	double climbspdV = m_pFlight->getFlightPerformanceManager()->getClimboutSpeedVertical(m_pFlight->GetCFlight());
	double climbspdH = m_pFlight->getFlightPerformanceManager()->getClimboutSpeed(m_pFlight->GetCFlight());

	double climbAltitude = 2500;   // determinant later
	dT = ElapsedTime(climbAltitude / climbspdV) ; 

	double avgSpdH = (climbspdH + fltstate.m_vSpeed) * 0.5;

	dDist = avgSpdH * (double)dT;           // dist to climbout    

	fltstate.m_tTime += dT;
	fltstate.m_dist += dDist;
	//fltstate.m_dAlt = climbAltitude;
	fltstate.m_vSpeed = climbspdH;

	m_vMovements.push_back(fltstate);

}



void FlightPreTakeoff::End( Clearance * pClearance )
{
	
	if(pClearance->m_pResource && pClearance->m_pResource->GetType() == AirsideResource::ResType_RunwayPort)
	{
		double taxispd = m_pFlight->getFlightPerformanceManager()->getTaxiInSpeed(m_pFlight->GetCFlight());
		RunwayPortInSim * pRunway = (RunwayPortInSim * )pClearance->m_pResource;
		FlightState fltstate = m_pFlight->GetFlightState();
		//move to the clearance position
		Point nextPos = pRunway->GetDistancePoint(pClearance->m_dDist);
		ARCVector3 vDir = nextPos - fltstate.m_pPosition;
		DistanceUnit dDist = vDir.Magnitude();
		ElapsedTime dT = ElapsedTime(dDist/taxispd);
		fltstate.m_tTime += dT;
		fltstate.m_pPosition = nextPos;
		fltstate.m_dist = pClearance->m_dDist;
		fltstate.m_vDirection = vDir;
		m_vMovements.push_back(fltstate);
		//wait until the clearance time
		if(fltstate.m_tTime < pClearance->m_tTime)
		{
			fltstate.m_tTime = pClearance->m_tTime;
			m_vMovements.push_back(fltstate);
		}	
		//
		if(pClearance->m_nMode == OnTakeoff)
		{
			//accelerate to lift
			ARCVector3 vRwDir = pRunway->GetDirection();

			double acelspd  = m_pFlight->getFlightPerformanceManager()->getTakeoffAcceleration(m_pFlight->GetCFlight());
			double liftspd  = m_pFlight->getFlightPerformanceManager()->getClimboutSpeed(m_pFlight->GetCFlight());
			ElapsedTime dT = ElapsedTime( (liftspd - fltstate.m_vSpeed) / acelspd );
			double dDist = (liftspd + fltstate.m_vSpeed) * 0.5 * dT.asSeconds();
			fltstate.m_dist += dDist;
			fltstate.m_vSpeed = liftspd;
			fltstate.m_pPosition += Point(vRwDir[VX],vRwDir[VY],0) * dDist;	
			fltstate.m_tTime +=dT;
			m_vMovements.push_back(fltstate);

		}
		fltstate.m_fltMode = pClearance->m_nMode;	
		fltstate.m_pResource = pClearance->m_pResource;
		m_vMovements.push_back(fltstate);

		//
	}
}

