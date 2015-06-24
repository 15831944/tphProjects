#include "stdafx.h"


#include "FlightTaxiingMovements.h"

#include "TaxiwayResource.h"

#include "RunwayInSim.h"

#include "StandInSim.h"
#include <cmath>


void FlightTaxiing::EndInSeg( TaxiwayDirectSegInSim * pSeg, double dist, double vSpeed )
{
	AirsideResource * pBeginRes = m_beginState.m_pResource.get();
	FlightState fltstate = m_beginState;


	if(pBeginRes!=pSeg) // not in the same segment
	{
		if(pBeginRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
		{
			TaxiwayDirectSegInSim * preSeg = (TaxiwayDirectSegInSim* )pBeginRes;
			fltstate.m_pResource = preSeg;
			double enddist = preSeg->GetEndDist();
			if(enddist > fltstate.m_dist ){
				double dDist = enddist - fltstate.m_dist;
				ElapsedTime dT = ElapsedTime(dDist/fltstate.m_vSpeed);
				fltstate.m_tTime += dT;
				fltstate.m_dist = enddist;
				m_vMovements.push_back(fltstate);
			}

			Point endPt = fltstate.m_pResource->GetDistancePoint(fltstate.m_dist);
			Point toPt = pSeg->GetDistancePoint(0);
			double dDist = endPt.distance(toPt);
			ElapsedTime dT = ElapsedTime( dDist/fltstate.m_vSpeed );
			fltstate.m_tTime += dT;

			fltstate.m_pResource = pSeg;
			fltstate.m_dist = 0;

			m_vMovements.push_back(fltstate);			
		}
	}else
	{
		if(fltstate.m_dist < dist)
		{
			double dDist = dist - fltstate.m_dist;
			double avgSpd = (fltstate.m_vSpeed + vSpeed)*0.5;
			ElapsedTime dT = ElapsedTime(dDist/avgSpd);
			fltstate.m_tTime += dT;
			fltstate.m_dist = dist;		
			m_vMovements.push_back(fltstate);
		}
	}


}


//end movements
void FlightTaxiing::End( Clearance * pClearance )
{
	
	FlightState fltstate = m_pFlight->GetFlightState();

	fltstate.m_tTime = pClearance->m_tTime;
	double taxispd = m_pFlight->getFlightPerformanceManager()->getTaxiInSpeed(m_pFlight->GetCFlight());

	if(fltstate.m_pResource.get() && fltstate.m_pResource->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
	{
		TaxiwayDirectSegInSim * pSeg = (TaxiwayDirectSegInSim *) fltstate.m_pResource.get();
		//stay at the position until the clearance
		fltstate.m_vSpeed = 0;
		fltstate.m_tTime = pClearance->m_tTime;
		m_vMovements.push_back(fltstate);
	}
	//taxi to the dest
	Path taxiPath;
	GetTaxiPath(pClearance, taxiPath);
	ElapsedTime startT = fltstate.m_tTime;
	for(int i =0 ;i<taxiPath.getCount();i++)
	{
		Point nextPt = taxiPath.getPoint(i);
		ARCVector3 dir = nextPt - fltstate.m_pPosition;
		DistanceUnit dDist = dir.Magnitude();

		if(dDist> ARCMath::EPSILON){
			fltstate.m_vDirection = dir.Normalize();
		}

		fltstate.m_pPosition = nextPt;
		ElapsedTime dT = ElapsedTime(dDist/ taxispd);
		fltstate.m_tTime += dT;		
		m_vMovements.push_back(fltstate);
	}
	fltstate.m_pResource = pClearance->m_pResource;
	fltstate.m_fltMode = pClearance->m_nMode;
	fltstate.m_dist = pClearance->m_dDist;
	m_vMovements.push_back(fltstate);	
	
	
	

}

void FlightTaxiing::GetTaxiPath( Clearance * pClearance , Path& path )
{
	AirsideResource * pStartRes = m_pFlight->GetCurrentResource();
	AirsideResource * pEndRes = pClearance->m_pResource;

	if(pStartRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
	{
		TaxiwayDirectSegInSim * pStartSeg  =(TaxiwayDirectSegInSim*)pStartRes;
		if(pStartRes == pEndRes) // in the same taxiway segment
		{
			path = GetSubPath(pStartSeg->GetPath(),m_pFlight->GetFlightState().m_dist, pClearance->m_dDist);
		}
		else if(pEndRes && pEndRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg) // to the other taxiway
		{
			TaxiwayDirectSegInSim * pEndSeg  =(TaxiwayDirectSegInSim*)pEndRes;
			Path othersegpath = GetSubPath(pEndSeg->GetPath(),0,pClearance->m_dDist);
			path.init(othersegpath.getCount()+1);
			path[0] = pStartSeg->GetExitNode()->GetDistancePoint(0); 
			for(int i=0;i<othersegpath.getCount();i++)
			{
				path[i+1] = othersegpath[i];
			}
		}
		else if(pEndRes && pEndRes->GetType() == AirsideResource::ResType_RunwayPort)  // to the runway
		{
			RunwayPortInSim * pRunway = (RunwayPortInSim*)pEndRes;
			path.init(2);
			path[0] = pStartSeg->GetExitNode()->GetDistancePoint(0);
			path[1] =  pRunway->GetDistancePoint(pClearance->m_dDist);
		}
		else if(pEndRes && pEndRes->GetType() == AirsideResource::ResType_Stand)
		{
			StandInSim * pStand = (StandInSim *)pEndRes;
			path = pStand->GetInPath(); 
		}
	}

}

void FlightPushOutStand::End( Clearance * pClearance )
{
	ASSERT(pClearance);
	FlightState fltstate = m_pFlight->GetFlightState();
	double pushoutspd = m_pFlight->getFlightPerformanceManager()->getTaxiOutSpeed(m_pFlight->GetCFlight());
	if(m_pFlight->GetCurrentResource() && m_pFlight->GetCurrentResource()->GetType() == AirsideResource::ResType_Stand)
	{
		StandInSim * pStand = (StandInSim*)m_pFlight->GetCurrentResource();
		//first wait until the clearance time
		fltstate.m_tTime = pClearance->m_tTime;
		m_vMovements.push_back(fltstate);
		//push back
		//if(pStand->GetStandInput()->IsUsePushBack())
		{
			Path pushoutPath ;
			GetPushOutPath(pClearance,pushoutPath);
			for(int i =0 ;i<pushoutPath.getCount();i++)
			{
				Point nextPt = pushoutPath.getPoint(i);
				ARCVector3 dir = nextPt - fltstate.m_pPosition;
				DistanceUnit dDist = dir.Magnitude();

				if(dDist> ARCMath::EPSILON){
					fltstate.m_vDirection = dir.Normalize();
				}

				fltstate.m_pPosition = nextPt;
				ElapsedTime dT = ElapsedTime(dDist/ pushoutspd);
				fltstate.m_tTime += dT;		
				m_vMovements.push_back(fltstate);
			}
		}
		//to the next segment
		if( pClearance->m_pResource)
		{
			Point nextPt = pClearance->m_pResource->GetDistancePoint(0);
			ARCVector3 dir = nextPt - fltstate.m_pPosition;
			DistanceUnit dDist = dir.Magnitude();
			if(dDist> ARCMath::EPSILON){
				fltstate.m_vDirection = dir.Normalize();
			}
			fltstate.m_pPosition = nextPt;
			fltstate.m_tTime += ElapsedTime(dDist/pushoutspd);
			fltstate.m_pResource = pClearance->m_pResource;
			fltstate.m_fltMode = pClearance->m_nMode;
			m_vMovements.push_back(fltstate);
		}
		else
		{
			fltstate.m_fltMode = pClearance->m_nMode;	
			fltstate.m_pResource = pClearance->m_pResource;
			m_vMovements.push_back(fltstate);
		}
	}
	
}

void FlightPushOutStand::GetPushOutPath( Clearance *pClearance, Path& path )
{
	StandInSim * pStand = (StandInSim*)m_pFlight->GetCurrentResource();
	Path outpath = pStand->GetOutPath();
	path = outpath;

	if(pClearance->m_pResource && pClearance->m_pResource->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
	{
		TaxiwayDirectSegInSim *taxiseg = (TaxiwayDirectSegInSim*)pClearance->m_pResource;
		if(pStand->GetStandInput()->IsUsePushBack())
		{
			Point segDir = taxiseg->GetDistancePoint(1) - taxiseg->GetDistancePoint(0);
			path.init(outpath.getCount() +1, &outpath[0]);
			path[path.getCount()-1] = outpath[outpath.getCount()-1] - segDir; 
		}
	}
	
}

void FlightBirthing::End( Clearance * pClearace )
{
	if(pClearace->m_nMode == OnLanding)
	{
		RunwayPortInSim * pRunway = (RunwayPortInSim * )pClearace->m_pResource;
		double landingSpd = m_pFlight->getFlightPerformanceManager()->getLandingSpeed(m_pFlight->GetCFlight());
		DistanceUnit touchDist = m_pFlight->getFlightPerformanceManager()->getDistThresToTouch(m_pFlight->GetCFlight());
		DistanceUnit ApprochDistToRunway = 50000;
		Point ApproachPos = pRunway->GetDistancePoint(-ApprochDistToRunway);
		ApproachPos.setZ((ApprochDistToRunway + touchDist) * std::tan(ARCMath::DegreesToRadians(3)));
		ARCVector3 ApproachDir = pRunway->GetDirection();
		ApproachDir[VZ] =  -std::tan(ARCMath::DegreesToRadians(3));		
		
		FlightState fltstate;
		fltstate.m_fltMode = OnFinalApproach;
		fltstate.m_pPosition = ApproachPos;
		fltstate.m_vSpeed = landingSpd;
		fltstate.m_vDirection = ApproachDir;		
		fltstate.m_tTime = pClearace->m_tTime;
		m_vMovements.push_back(fltstate);

		fltstate.m_tTime += ElapsedTime(ApprochDistToRunway/landingSpd);
		fltstate.m_pResource = pRunway;
		fltstate.m_pPosition = pRunway->GetDistancePoint(0);
		fltstate.m_pPosition.setZ(touchDist * std::tan(ARCMath::DegreesToRadians(3)) );
		fltstate.m_vSpeed = landingSpd;
		fltstate.m_fltMode = OnLanding;

		m_vMovements.push_back(fltstate);
	}
	else if(pClearace->m_nMode == OnHeldAtStand)
	{
		StandInSim * pStand = (StandInSim*) pClearace->m_pResource;
		FlightState fltstate;
		fltstate.m_pResource = pStand;
		Path inPath = pStand->GetInPath();
		if(inPath.getCount() > 1)
		{
			fltstate.m_pPosition = inPath.getPoint(inPath.getCount() -1);
			ARCVector3 vDir = inPath.getPoint(inPath.getCount()-1) - inPath.getPoint(inPath.getCount()-2);
			vDir.Normalize();
			fltstate.m_vDirection = vDir;			
		}
		else 
		{
			fltstate.m_pPosition = pStand->GetStandInput()->GetServicePoint();			
		}
		fltstate.m_tTime = pClearace->m_tTime;
		fltstate.m_fltMode = pClearace->m_nMode;
		fltstate.m_dist = pStand->GetHeldDist();
		m_vMovements.push_back(fltstate);
	}
	else
		FlightMovements::End(pClearace);
}

void FlightHeldAtStand::End( Clearance *pClearance )
{
	FlightState fltstate = m_pFlight->GetFlightState();
	fltstate.m_fltMode = pClearance->m_nMode;
	fltstate.m_tTime = pClearance->m_tTime;
	fltstate.m_pResource = pClearance->m_pResource;
	m_vMovements.push_back(fltstate);
}