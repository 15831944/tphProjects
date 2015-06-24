#include "StdAfx.h"
#include ".\flightplanspecificrouteinsim.h"
#include "ConflictConcernIntersectionInAirspace.h"
#include "FlightPerformanceManager.h"
#include "HeadingAirRouteSegInSim.h "

FlightPlanSpecificRouteInSim::FlightPlanSpecificRouteInSim(void)
{
}

FlightPlanSpecificRouteInSim::~FlightPlanSpecificRouteInSim(void)
{
}

ns_FlightPlan::FlightPlanBase::ProfileType FlightPlanSpecificRouteInSim::getType()
{
	return ns_FlightPlan::FlightPlanBase::Specific_Profile;
}

ElapsedTime FlightPlanSpecificRouteInSim::GetFlightTimeInSeg( AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg )
{
	if(pSeg == NULL)
		return 0L;

	double tNormal = 0.0;
	int nCount = pSeg->GetRoutePointCount();
	if(pSeg->GetFirstConcernIntersection())
	{
		AirRoutePointInSim* pFirstIntersection = pSeg->GetFirstConcernIntersection()->getInputPoint();
		FlightPlanPropertyInSim planProperty = GetRoutePointProperty(pFirstIntersection);
		if (nCount > 0)
		{
			AirRoutePointInSim* pPoint = pSeg->GetRoutePointByIdx(0);
			FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint);
			DistanceUnit dist = pFirstIntersection->GetPosition().distance(pPoint->GetPosition());
			tNormal += 2.0*dist/(planProperty.GetSpeed() +planProperty1.GetSpeed());
		}
	}

	for (int i = 0; i < nCount-1; i++)
	{
		AirRoutePointInSim* pPoint1 = pSeg->GetRoutePointByIdx(i);
		AirRoutePointInSim* pPoint2 = pSeg->GetRoutePointByIdx(i+1);
		FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint1);
		FlightPlanPropertyInSim planProperty2 = GetRoutePointProperty(pPoint2);

		DistanceUnit dist = pPoint1->GetPosition().distance(pPoint2->GetPosition());
		tNormal += 2.0*dist/(planProperty1.GetSpeed() + planProperty2.GetSpeed());		
	}

	if (pSeg->GetSecondConcernIntersection())
	{
		AirRoutePointInSim* pSecondIntersection = pSeg->GetSecondConcernIntersection()->getInputPoint();
		FlightPlanPropertyInSim planProperty = GetRoutePointProperty(pSecondIntersection);
		if (nCount > 0)
		{
			AirRoutePointInSim* pPoint = pSeg->GetRoutePointByIdx(nCount-1);
			FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint);
			DistanceUnit dist = pSecondIntersection->GetPosition().distance(pPoint->GetPosition());
			tNormal += 2.0*dist/(planProperty.GetSpeed()+planProperty1.GetSpeed());
		}
		else
		{   
			AirRoutePointInSim* pPoint = pSeg->GetFirstConcernIntersection()->getInputPoint();
			FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint);
			DistanceUnit dist = pSecondIntersection->GetPosition().distance(pPoint->GetPosition());
			tNormal += 2.0*dist/(planProperty.GetSpeed()+planProperty1.GetSpeed());	
		}
	}


	return (ElapsedTime)tNormal;
}

ElapsedTime FlightPlanSpecificRouteInSim::GetFlightMaxTimeInSeg( AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg )
{
	double tMax = 0.0;
	int nCount = pSeg->GetRoutePointCount();
	if(pSeg->GetFirstConcernIntersection())
	{
		AirRoutePointInSim* pFirstIntersection = pSeg->GetFirstConcernIntersection()->getInputPoint();
		FlightPlanPropertyInSim planProperty = GetRoutePointProperty(pFirstIntersection);
		if (nCount > 0)
		{
			AirRoutePointInSim* pPoint = pSeg->GetRoutePointByIdx(0);
			FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint);
			DistanceUnit dist = pFirstIntersection->GetPosition().distance(pPoint->GetPosition());
			tMax += 2.0*dist/(pFlight->GetMinSpeed(planProperty.GetMode())+pFlight->GetMinSpeed(planProperty1.GetMode()));
		}
	}

	for (int i = 0; i < nCount-1; i++)
	{
		AirRoutePointInSim* pPoint1 = pSeg->GetRoutePointByIdx(i);
		AirRoutePointInSim* pPoint2 = pSeg->GetRoutePointByIdx(i+1);
		FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint1);
		FlightPlanPropertyInSim planProperty2 = GetRoutePointProperty(pPoint2);

		DistanceUnit dist = pPoint1->GetPosition().distance(pPoint2->GetPosition());
		tMax += 2.0*dist/(pFlight->GetMinSpeed(planProperty1.GetMode()) + pFlight->GetMinSpeed(planProperty2.GetMode()));		
	}

	if (pSeg->GetSecondConcernIntersection())
	{
		AirRoutePointInSim* pSecondIntersection = pSeg->GetSecondConcernIntersection()->getInputPoint();
		FlightPlanPropertyInSim planProperty = GetRoutePointProperty(pSecondIntersection);
		if (nCount > 0)
		{
			AirRoutePointInSim* pPoint = pSeg->GetRoutePointByIdx(nCount-1);
			FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint);
			DistanceUnit dist = pSecondIntersection->GetPosition().distance(pPoint->GetPosition());
			tMax += 2.0*dist/(pFlight->GetMinSpeed(planProperty.GetMode())+pFlight->GetMinSpeed(planProperty1.GetMode()));
		}
		else
		{   
			AirRoutePointInSim* pPoint = pSeg->GetFirstConcernIntersection()->getInputPoint();
			FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint);
			DistanceUnit dist = pSecondIntersection->GetPosition().distance(pPoint->GetPosition());
			tMax += 2.0*dist/(pFlight->GetMinSpeed(planProperty.GetMode())+pFlight->GetMinSpeed(planProperty1.GetMode()));	
		}
	}

	return (ElapsedTime)tMax;
}

ElapsedTime FlightPlanSpecificRouteInSim::GetFlightMinTimeInSeg( AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg )
{
	double tMin = 0.0;
	int nCount = pSeg->GetRoutePointCount();
	if(pSeg->GetFirstConcernIntersection())
	{
		AirRoutePointInSim* pFirstIntersection = pSeg->GetFirstConcernIntersection()->getInputPoint();
		FlightPlanPropertyInSim planProperty = GetRoutePointProperty(pFirstIntersection);
		if (nCount > 0)
		{
			AirRoutePointInSim* pPoint = pSeg->GetRoutePointByIdx(0);
			FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint);
			DistanceUnit dist = pFirstIntersection->GetPosition().distance(pPoint->GetPosition());
			tMin += 2.0*dist/(pFlight->GetMaxSpeed(planProperty.GetMode())+pFlight->GetMaxSpeed(planProperty1.GetMode()));
		}
	}

	for (int i = 0; i < nCount-1; i++)
	{
		AirRoutePointInSim* pPoint1 = pSeg->GetRoutePointByIdx(i);
		AirRoutePointInSim* pPoint2 = pSeg->GetRoutePointByIdx(i+1);
		FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint1);
		FlightPlanPropertyInSim planProperty2 = GetRoutePointProperty(pPoint2);

		DistanceUnit dist = pPoint1->GetPosition().distance(pPoint2->GetPosition());
		tMin += 2.0*dist/(pFlight->GetMaxSpeed(planProperty1.GetMode()) + pFlight->GetMaxSpeed(planProperty2.GetMode()));		
	}

	if (pSeg->GetSecondConcernIntersection())
	{
		AirRoutePointInSim* pSecondIntersection = pSeg->GetSecondConcernIntersection()->getInputPoint();
		FlightPlanPropertyInSim planProperty = GetRoutePointProperty(pSecondIntersection);
		if (nCount > 0)
		{
			AirRoutePointInSim* pPoint = pSeg->GetRoutePointByIdx(nCount-1);
			FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint);
			DistanceUnit dist = pSecondIntersection->GetPosition().distance(pPoint->GetPosition());
			tMin += 2.0*dist/(pFlight->GetMaxSpeed(planProperty.GetMode())+pFlight->GetMaxSpeed(planProperty1.GetMode()));
		}
		else
		{   
			AirRoutePointInSim* pPoint = pSeg->GetFirstConcernIntersection()->getInputPoint();
			FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint);
			DistanceUnit dist = pSecondIntersection->GetPosition().distance(pPoint->GetPosition());
			tMin += 2.0*dist/(pFlight->GetMaxSpeed(planProperty.GetMode())+pFlight->GetMaxSpeed(planProperty1.GetMode()));	
		}
	}

	return (ElapsedTime)tMin;
}

ElapsedTime FlightPlanSpecificRouteInSim::CalculateTimeFromLastSegToLogicRunway( AirsideFlightInSim *pFlight )
{
	if (pFlight->IsCircuitFlight())
	{
		if (m_pAirRouteInSim->getRouteType() == CAirRoute::STAR)
		{
			if (pFlight->GetSTAR() != this)
				return -1L;
		}
		else
		{
			if(pFlight->GetLandingCircuit() != this)
				return -1L;
		}
	
	}
	else 
	{
		if (pFlight->GetSTAR() != this)
			return -1L;
	}

	int nCount = m_vItems.size();
	AirRouteSegInSim *pLastSegmentInSim = (AirRouteSegInSim*)m_vItems.at(nCount-2);
	LogicRunwayInSim *pLogicRunway = pFlight->GetLandingRunway();

	ElapsedTime eTimeLastNodeToRunwayPort= ElapsedTime(0L);

	AirRouteIntersectionInSim* pLastFirstConflictIntersection = pLastSegmentInSim->GetFirstConcernIntersection();

	DistanceUnit touchDist = pFlight->GetTouchDownDistAtRuwnay(pLogicRunway);//->GetPerformance()->getDistThresToTouch(pFlight);
	CPoint2008 runwayPortPoint = pLogicRunway->GetDistancePoint(touchDist);
	CPoint2008 lastSegmentNode = pLastFirstConflictIntersection->getInputPoint()->GetPosition();
	CPoint2008 MissApporachPoint = pLogicRunway->GetMissApproachCheckPoint(lastSegmentNode);
	double dSpeed =  GetRoutePointProperty(pLastFirstConflictIntersection->getInputPoint()).GetSpeed();

	double dTimeLastNodeToRunwayPort = 	2.0*(lastSegmentNode.distance(MissApporachPoint) + MissApporachPoint.distance(runwayPortPoint))
										/(dSpeed+ pFlight->GetPerformance()->getLandingSpeed(pFlight));
	eTimeLastNodeToRunwayPort = ElapsedTime(dTimeLastNodeToRunwayPort);
	

	return eTimeLastNodeToRunwayPort;
}

ElapsedTime FlightPlanSpecificRouteInSim::CalculateMinTimeFromLastSegToLogicRunway( AirsideFlightInSim *pFlight )
{
	if (pFlight->GetSTAR() != this)
		return -1L;

	int nCount = m_vItems.size();
	AirRouteSegInSim *pLastSegmentInSim = (AirRouteSegInSim*)m_vItems.at(nCount-2);
	LogicRunwayInSim *pLogicRunway = pFlight->GetLandingRunway();

	ElapsedTime eTimeLastNodeToRunway= ElapsedTime(0L);

	AirRouteIntersectionInSim* pLastFirstConflictIntersection = pLastSegmentInSim->GetFirstConcernIntersection();

	DistanceUnit touchDist = pFlight->GetTouchDownDistAtRuwnay(pLogicRunway);//pFlight->GetPerformance()->getDistThresToTouch(pFlight);
	CPoint2008 runwayPortPoint = pLogicRunway->GetDistancePoint(touchDist);
	CPoint2008 lastSegmentNode = pLastFirstConflictIntersection->getInputPoint()->GetPosition();
	double dSpeed =  pFlight->GetMaxSpeed(OnApproach);

	double dTimeLastNodeToRunwayPort = 	2.0*(runwayPortPoint.distance(lastSegmentNode))/(dSpeed+ pFlight->GetPerformance()->getMaxLandingSpeed(pFlight));
	eTimeLastNodeToRunway = ElapsedTime(dTimeLastNodeToRunwayPort);


	return eTimeLastNodeToRunway;
}



ElapsedTime FlightPlanSpecificRouteInSim::CalculateTakeoffTimeFromRwyToFirstSeg( AirsideFlightInSim *pFlight )
{
	LogicRunwayInSim *pLogicRunway = pFlight->GetAndAssignTakeoffRunway()->GetOtherPort();

	ElapsedTime eTimeLastNodeToRunwayPort= ElapsedTime(0L);
	AirRoutePointInSim *pFirstWP = GetAirRouteInSim()->GetRwyConnectedRoutePoint();

	CPoint2008 runwayPortPoint = pLogicRunway->GetDistancePoint(0.0);
	CPoint2008 lastSegmentNode = pFirstWP->GetPosition();

	double dSpeed =  pFlight->GetPerformance()->getClimboutSpeed(pFlight);

	double dTimeLastNodeToRunwayPort = 	runwayPortPoint.distance(lastSegmentNode)/dSpeed;
	eTimeLastNodeToRunwayPort = ElapsedTime(dTimeLastNodeToRunwayPort);

	return eTimeLastNodeToRunwayPort;
}

ElapsedTime FlightPlanSpecificRouteInSim::GetHeadingMinTimeInSeg( AirsideFlightInSim *pFlight, HeadingAirRouteSegInSim*pSeg, HeadingAirRouteSegInSim*pNextSeg )
{
	double dSped = GetRoutePointProperty(pSeg->GetFirstIntersection()->getInputPoint()).GetSpeed();
	double dSped2 = GetRoutePointProperty(pSeg->GetSecondIntersection()->getInputPoint()).GetSpeed();

	return pSeg->GetMinTimeInSegment(pFlight,dSped,dSped2,pNextSeg);

}

ElapsedTime FlightPlanSpecificRouteInSim::GetHeadingMaxTimeInSeg( AirsideFlightInSim *pFlight, HeadingAirRouteSegInSim*pSeg, HeadingAirRouteSegInSim*pNextSeg )
{

	return ElapsedTime(0L);
}

//double FlightPlanSpecificRouteInSim::GetSpeed(AirRoutePointInSim* pAirPoint)
//{
//	return GetRoutePointProperty(pSeg->GetFirstIntersection()->getInputPoint()).GetSpeed();
//}

























