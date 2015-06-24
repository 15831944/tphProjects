#include "StdAfx.h"
#include ".\flightplangenericrouteinsim.h"
#include ".\ConflictConcernIntersectionInAirspace.h"
#include "FlightPerformanceManager.h"
#include "../../common/arcunit.h"
#include "HeadingAirRouteSegInSim.h"
#include "SimulationErrorShow.h"

FlightPlanGenericRouteInSim::FlightPlanGenericRouteInSim( ns_FlightPlan::FlightPlanGeneric *pGenericPlan )
:m_pGenericPlan( new FlightPlanGenericInSim(pGenericPlan))
{
m_bInitialzied = false;
}
FlightPlanGenericRouteInSim::~FlightPlanGenericRouteInSim(void)
{
}

ns_FlightPlan::FlightPlanBase::ProfileType FlightPlanGenericRouteInSim::getType()
{
	return ns_FlightPlan::FlightPlanBase::Generic_Profile;
}

void FlightPlanGenericRouteInSim::CheckFlightPlan( AirsideFlightInSim *pFlight , bool bArrival)
{
	if(m_pGenericPlan)
	{
		int nPhaseCount = static_cast< int>(m_pGenericPlan->m_vPhases.size());
		for (int nPhase = 0; nPhase < nPhaseCount; ++nPhase)
		{
			FlightPlanGenericPhaseInSim& phase = m_pGenericPlan->m_vPhases[nPhase];

			AirsideMobileElementMode mode = getPhaseMode(phase,bArrival);

			double dMinSpeed = pFlight->GetMinSpeed(mode);
			double dMaxSpeed = pFlight->GetMaxSpeed(mode);

			if(phase.GetDownSpeed()< dMinSpeed || phase.GetDownSpeed() > dMaxSpeed)
			{
				phase.SetDownSpeed(dMinSpeed);

				CString strError;
				strError.Format("The flight plan minimum speed exceed the flight performance speed limitation at mode %s!",AIRSIDEMOBILE_MODE_NAME[mode]);
				CString strWarn = _T("DEFINITION ERROR");
				AirsideSimErrorShown::SimWarning(pFlight,strError,strWarn);
			}

			if(phase.GetUpSpeed() < dMinSpeed || phase.GetUpSpeed() > dMaxSpeed)
			{
				phase.SetUpSpeed(dMaxSpeed);

				CString strError;
				strError.Format("The flight plan maximum speed exceed the flight performance speed limitation at mode %s!",AIRSIDEMOBILE_MODE_NAME[mode]);
				CString strWarn = _T("DEFINITION ERROR");
				AirsideSimErrorShown::SimWarning(pFlight,strError,strWarn);

			}
		}
	}




}

void FlightPlanGenericRouteInSim::InitializeSegments(AirsideFlightInSim *pFlight)
{
	//only initialized for one time
	if(m_bInitialzied)
		return;
	m_bInitialzied = true;



	CAirRoute::RouteType rtType = m_pAirRouteInSim->getRouteType();
	if(rtType == CAirRoute::STAR)
	{
		CheckFlightPlan(pFlight, true);
		InitializeSTAR(pFlight);	
	}
	else if(rtType ==  CAirRoute::SID)
	{
		CheckFlightPlan(pFlight,false);
		InitializeSID(pFlight);
	}
	else if(rtType == CAirRoute::EN_ROUTE)
	{
	}
	else if(rtType == CAirRoute::MIS_APPROACH)
	{
	}

}

ElapsedTime FlightPlanGenericRouteInSim::GetFlightTimeInSeg( AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg )
{
	return FlightRouteInSim::GetFlightTimeInSeg(pFlight, pSeg );

	if(pSeg == NULL)
		return 0L;

	ElapsedTime tNormal(0L);
	int nCount = pSeg->GetRoutePointCount();
	if(pSeg->GetFirstConcernIntersection())
	{
		AirRoutePointInSim* pFirstIntersection = pSeg->GetFirstConcernIntersection()->getInputPoint();
		FlightPlanPropertyInSim planProperty = GetRoutePointProperty(pFirstIntersection);
		//get speed
		double dDistFrom = pFirstIntersection->getDistToRunway();
		if (nCount > 0)
		{
			AirRoutePointInSim* pPoint = pSeg->GetRoutePointByIdx(0);
			FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint);
			DistanceUnit dist = pFirstIntersection->GetPosition().distance(pPoint->GetPosition());

			tNormal += m_pGenericPlan->GetFlightTimeInSeg(pFlight, dDistFrom, pPoint->getDistToRunway());
		}
	}

	for (int i = 0; i < nCount-1; i++)
	{
		AirRoutePointInSim* pPoint1 = pSeg->GetRoutePointByIdx(i);
		AirRoutePointInSim* pPoint2 = pSeg->GetRoutePointByIdx(i+1);
		FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint1);
		FlightPlanPropertyInSim planProperty2 = GetRoutePointProperty(pPoint2);

		DistanceUnit dist = pPoint1->GetPosition().distance(pPoint2->GetPosition());
		//tNormal += 2.0*dist/(planProperty1.GetSpeed() + planProperty2.GetSpeed());	
		tNormal +=m_pGenericPlan->GetFlightTimeInSeg(pFlight, pPoint1->getDistToRunway(), pPoint2->getDistToRunway());

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
			//tNormal += 2.0*dist/(planProperty.GetSpeed()+planProperty1.GetSpeed());
			tNormal += m_pGenericPlan->GetFlightTimeInSeg(pFlight, pPoint->getDistToRunway(), pSecondIntersection->getDistToRunway());

		}
		else
		{   
			AirRoutePointInSim* pPoint = pSeg->GetFirstConcernIntersection()->getInputPoint();
			FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint);
			DistanceUnit dist = pSecondIntersection->GetPosition().distance(pPoint->GetPosition());
			//tNormal += 2.0*dist/(planProperty.GetSpeed()+planProperty1.GetSpeed());	
			tNormal += m_pGenericPlan->GetFlightTimeInSeg(pFlight, pPoint->getDistToRunway(), pSecondIntersection->getDistToRunway());

		}
	}

	return tNormal;
}

ElapsedTime FlightPlanGenericRouteInSim::GetFlightMaxTimeInSeg( AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg )
{
	return FlightRouteInSim::GetFlightMaxTimeInSeg( pFlight, pSeg);


	if(pSeg == NULL)
		return 0L;

	ElapsedTime tNormal(0L);
	int nCount = pSeg->GetRoutePointCount();
	if(pSeg->GetFirstConcernIntersection())
	{
		AirRoutePointInSim* pFirstIntersection = pSeg->GetFirstConcernIntersection()->getInputPoint();
		FlightPlanPropertyInSim planProperty = GetRoutePointProperty(pFirstIntersection);
		//get speed
		double dDistFrom = pFirstIntersection->getDistToRunway();
		if (nCount > 0)
		{
			AirRoutePointInSim* pPoint = pSeg->GetRoutePointByIdx(0);
			FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint);
			DistanceUnit dist = pFirstIntersection->GetPosition().distance(pPoint->GetPosition());

			tNormal += m_pGenericPlan->GetFlightMaxTimeInSeg(pFlight, dDistFrom, pPoint->getDistToRunway());
		}
	}

	for (int i = 0; i < nCount-1; i++)
	{
		AirRoutePointInSim* pPoint1 = pSeg->GetRoutePointByIdx(i);
		AirRoutePointInSim* pPoint2 = pSeg->GetRoutePointByIdx(i+1);
		FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint1);
		FlightPlanPropertyInSim planProperty2 = GetRoutePointProperty(pPoint2);

		DistanceUnit dist = pPoint1->GetPosition().distance(pPoint2->GetPosition());
		//tNormal += 2.0*dist/(planProperty1.GetSpeed() + planProperty2.GetSpeed());	
		tNormal +=m_pGenericPlan->GetFlightMaxTimeInSeg(pFlight, pPoint1->getDistToRunway(), pPoint2->getDistToRunway());

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
			//tNormal += 2.0*dist/(planProperty.GetSpeed()+planProperty1.GetSpeed());
			tNormal += m_pGenericPlan->GetFlightMaxTimeInSeg(pFlight, pPoint->getDistToRunway(), pSecondIntersection->getDistToRunway());

		}
		else
		{   
			AirRoutePointInSim* pPoint = pSeg->GetFirstConcernIntersection()->getInputPoint();
			FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint);
			DistanceUnit dist = pSecondIntersection->GetPosition().distance(pPoint->GetPosition());
			//tNormal += 2.0*dist/(planProperty.GetSpeed()+planProperty1.GetSpeed());	
			tNormal += m_pGenericPlan->GetFlightMaxTimeInSeg(pFlight, pPoint->getDistToRunway(), pSecondIntersection->getDistToRunway());

		}
	}

	return tNormal;
}

ElapsedTime FlightPlanGenericRouteInSim::GetFlightMinTimeInSeg( AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg )
{

	return FlightRouteInSim::GetFlightMinTimeInSeg(pFlight, pSeg);

	if(pSeg == NULL)
		return 0L;

	ElapsedTime tNormal(0L);
	int nCount = pSeg->GetRoutePointCount();
	if(pSeg->GetFirstConcernIntersection())
	{
		AirRoutePointInSim* pFirstIntersection = pSeg->GetFirstConcernIntersection()->getInputPoint();
		FlightPlanPropertyInSim planProperty = GetRoutePointProperty(pFirstIntersection);
		//get speed
		double dDistFrom = pFirstIntersection->getDistToRunway();
		if (nCount > 0)
		{
			AirRoutePointInSim* pPoint = pSeg->GetRoutePointByIdx(0);
			FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint);
			DistanceUnit dist = pFirstIntersection->GetPosition().distance(pPoint->GetPosition());

			tNormal += m_pGenericPlan->GetFlightMinTimeInSeg(pFlight, dDistFrom, pPoint->getDistToRunway());
		}
	}

	for (int i = 0; i < nCount-1; i++)
	{
		AirRoutePointInSim* pPoint1 = pSeg->GetRoutePointByIdx(i);
		AirRoutePointInSim* pPoint2 = pSeg->GetRoutePointByIdx(i+1);
		FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint1);
		FlightPlanPropertyInSim planProperty2 = GetRoutePointProperty(pPoint2);

		DistanceUnit dist = pPoint1->GetPosition().distance(pPoint2->GetPosition());
		//tNormal += 2.0*dist/(planProperty1.GetSpeed() + planProperty2.GetSpeed());	
		tNormal +=m_pGenericPlan->GetFlightMinTimeInSeg(pFlight, pPoint1->getDistToRunway(), pPoint2->getDistToRunway());

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
			//tNormal += 2.0*dist/(planProperty.GetSpeed()+planProperty1.GetSpeed());
			tNormal += m_pGenericPlan->GetFlightMinTimeInSeg(pFlight, pPoint->getDistToRunway(), pSecondIntersection->getDistToRunway());

		}
		else
		{   
			AirRoutePointInSim* pPoint = pSeg->GetFirstConcernIntersection()->getInputPoint();
			FlightPlanPropertyInSim planProperty1 = GetRoutePointProperty(pPoint);
			DistanceUnit dist = pSecondIntersection->GetPosition().distance(pPoint->GetPosition());
			//tNormal += 2.0*dist/(planProperty.GetSpeed()+planProperty1.GetSpeed());	
			tNormal += m_pGenericPlan->GetFlightMinTimeInSeg(pFlight, pPoint->getDistToRunway(), pSecondIntersection->getDistToRunway());

		}
	}

	return tNormal;
}

ElapsedTime FlightPlanGenericRouteInSim::CalculateTimeFromLastSegToLogicRunway( AirsideFlightInSim *pFlight )
{
	return FlightRouteInSim::CalculateTimeFromLastSegToLogicRunway(pFlight );


	if (pFlight->GetSTAR() != this)
		return -1L;

	int nCount = m_vItems.size();
	AirRouteSegInSim *pLastSegmentInSim = (AirRouteSegInSim*)m_vItems.at(nCount-2);
	LogicRunwayInSim *pLogicRunway = pFlight->GetLandingRunway();

	ElapsedTime eTimeLastNodeToRunwayPort= ElapsedTime(0L);

	AirRouteIntersectionInSim* pLastFirstConflictIntersection = pLastSegmentInSim->GetFirstConcernIntersection();

	DistanceUnit touchDist = pFlight->GetTouchDownDistAtRuwnay(pLogicRunway);
	CPoint2008 runwayPortPoint = pLogicRunway->GetDistancePoint(touchDist);


	CPoint2008 lastSegmentNode = pLastFirstConflictIntersection->getInputPoint()->GetPosition();
	CPoint2008 MissApporachPoint = pLogicRunway->GetMissApproachCheckPoint(lastSegmentNode);
	double dSpeed =  GetRoutePointProperty(pLastFirstConflictIntersection->getInputPoint()).GetSpeed();

	double dDistanceToRunway = lastSegmentNode.distance(MissApporachPoint) + MissApporachPoint.distance(runwayPortPoint);
	//double dTimeLastNodeToRunwayPort = 	2.0*()
	//	/(dSpeed+ pFlight->GetPerformance()->getLandingSpeed(pFlight));

	//eTimeLastNodeToRunwayPort = ElapsedTime(dTimeLastNodeToRunwayPort);

	eTimeLastNodeToRunwayPort = m_pGenericPlan->CalculateLastSegToRunway(pFlight,pFlight->GetPerformance()->getLandingSpeed(pFlight),dDistanceToRunway);
	return eTimeLastNodeToRunwayPort;
}

ElapsedTime FlightPlanGenericRouteInSim::CalculateMinTimeFromLastSegToLogicRunway( AirsideFlightInSim *pFlight )
{
	return CalculateMinTimeFromLastSegToLogicRunway( pFlight );


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

	double dDistanceToRunway = runwayPortPoint.distance(lastSegmentNode);

	//double dTimeLastNodeToRunwayPort = 	2.0*(runwayPortPoint.distance(lastSegmentNode))/(dSpeed+ pFlight->GetPerformance()->getMaxLandingSpeed(pFlight));
	//eTimeLastNodeToRunway = ElapsedTime(dTimeLastNodeToRunwayPort);
	
	eTimeLastNodeToRunway = m_pGenericPlan->CalculateMinTimeLastSegToRunway(pFlight,pFlight->GetPerformance()->getMaxLandingSpeed(pFlight), dDistanceToRunway);

	return eTimeLastNodeToRunway;
}


ElapsedTime FlightPlanGenericRouteInSim::CalculateTakeoffTimeFromRwyToFirstSeg( AirsideFlightInSim *pFlight )
{
	return FlightRouteInSim::CalculateTakeoffTimeFromRwyToFirstSeg( pFlight );

	LogicRunwayInSim *pLogicRunway = pFlight->GetAndAssignTakeoffRunway()->GetOtherPort();

	ElapsedTime eTimeLastNodeToRunwayPort= ElapsedTime(0L);
	AirRoutePointInSim *pFirstWP = GetAirRouteInSim()->GetRwyConnectedRoutePoint();

	CPoint2008 runwayPortPoint = pLogicRunway->GetDistancePoint(0.0);
	CPoint2008 lastSegmentNode = pFirstWP->GetPosition();

	double dClimboutSpeed =  pFlight->GetPerformance()->getClimboutSpeed(pFlight);

	//double dTimeLastNodeToRunwayPort = 	runwayPortPoint.distance(lastSegmentNode)/dSpeed;
	//eTimeLastNodeToRunwayPort = ElapsedTime(dTimeLastNodeToRunwayPort);
	

	double dDistanceTravel = runwayPortPoint.distance(lastSegmentNode);

	return eTimeLastNodeToRunwayPort;



}


void FlightPlanGenericRouteInSim::UpdatePointProperties( AirRoutePointInSim *pPoint, FlightPlanGenericPhaseInSim& phase ,bool bArrival)
{
	int nCount = (int)m_FlightPlanProperties.size();

	for (int i = 0; i < nCount; i++ )
	{
		AirRoutePointInSim* pTmpPoint =( m_FlightPlanProperties.at(i)).GetRoutePoint();
		if ( pPoint == pTmpPoint)
		{
			FlightPlanPropertyInSim& prop = m_FlightPlanProperties[i];

			prop.SetSpeed((phase.GetUpSpeed() + phase.GetDownSpeed()) *0.5);
			prop.SetArrival();
			prop.SetMode(getPhaseMode(phase, bArrival));
			prop.SetAlt((phase.GetDownAltitude() + phase.GetUpAltitude())*0.5);
		}
	}

	pPoint->SetAltHigh(phase.GetUpAltitude());
	pPoint->SetAltLow(phase.GetDownAltitude());

}

AirsideMobileElementMode FlightPlanGenericRouteInSim::getPhaseMode(const FlightPlanGenericPhaseInSim& phase , bool bArrival) const
{
	switch (phase.GetPhase())
	{
	case ns_FlightPlan::FlightPlanGenericPhase::Phase_cruise:
		{
			if(bArrival)
				return OnCruiseArr;
			else
				return OnCruiseDep;
		}
		break;
	case ns_FlightPlan::FlightPlanGenericPhase::Phase_Terminal:
		{
			return OnTerminal;
		}
		break;
	case ns_FlightPlan::FlightPlanGenericPhase::Phase_approach:
		{
			return OnApproach;
		}
		break;
	case ns_FlightPlan::FlightPlanGenericPhase::Phase_Landing:
		{
			return OnLanding;
		}
		break;

	case ns_FlightPlan::FlightPlanGenericPhase::Phase_takeoff:
		{
			return OnTakeoff;
		}
		break;
	case ns_FlightPlan::FlightPlanGenericPhase::Phase_climbout:
		{
			return OnClimbout;
		}
		break;
	default:
		{
			ASSERT(0);
		}
	}

	return OnBirth;
}

void FlightPlanGenericRouteInSim::InitializeSTAR( AirsideFlightInSim *pFlight )
{
	CAirRoute::RouteType rtType = m_pAirRouteInSim->getRouteType();
	if(rtType == CAirRoute::STAR)//Seg->Seg->...->runway
	{	
		int nSegCount = GetItemCount();
		ASSERT(nSegCount > 0);
		//calculate the distance, runway to last segment
		double dDistanceToRunway = 0.0;
		CPoint2008 ptPtreLocation;

		AirRouteSegInSim *pNewSegment = NULL;
		if(nSegCount > 0)
		{
			LogicRunwayInSim * pLandingRunway  = (LogicRunwayInSim *)m_vItems[nSegCount - 1];
			ASSERT(pLandingRunway != NULL);

			CPoint2008 ptRunwayPort = pLandingRunway->GetDistancePoint(0.0);
			ptPtreLocation = ptRunwayPort;	
			if(nSegCount > 1)
			{
				AirRouteSegInSim *pLastSegmentInSim = (AirRouteSegInSim*)m_vItems.at(nSegCount-2);

				AirRouteIntersectionInSim* pLastFirstConflictIntersection = pLastSegmentInSim->GetFirstConcernIntersection();
				CPoint2008 lastSegmentNode = pLastFirstConflictIntersection->getInputPoint()->GetPosition();
				//CPoint2008 MissApporachPoint = pLandingRunway->GetMissApproachCheckPoint(lastSegmentNode);
				//double dRunwayDist = lastSegmentNode.distance(MissApporachPoint) + MissApporachPoint.distance(ptRunwayPort);
				double dRunwayDist = lastSegmentNode.distance(ptRunwayPort);

				pLastFirstConflictIntersection->getInputPoint()->setDistToRunway(dRunwayDist);
				ptPtreLocation = lastSegmentNode;

				//check if need to add a segment 
				std::vector<FlightPlanGenericPhaseInSim> vPhases =m_pGenericPlan->getConcernPhase(0,dRunwayDist);
				if (vPhases.size() > 0)
				{
					//add new segment
					std::vector<AirRoutePointInSim * > vAirRoutePoint;
					int nPhaseCount = static_cast<int>(vPhases.size());
					for (int nPhase = nPhaseCount - 1; nPhase >= 0; --nPhase)
					{
						FlightPlanGenericPhaseInSim& phase = vPhases[nPhase];
						CPoint2008 ptPhasePosition = ptRunwayPort.GetDistancePoint(lastSegmentNode,phase.GetDistanceToRunway());

						FlightPlanPhasePointInSim *pPhasepoint = new FlightPlanPhasePointInSim;
						pPhasepoint->SetAltHigh(phase.GetUpAltitude());
						pPhasepoint->SetAltLow(phase.GetDownAltitude());
						pPhasepoint->SetPosition(ptPhasePosition);

						vAirRoutePoint.push_back(pPhasepoint);

						FlightPlanPropertyInSim prop(pPhasepoint);
						prop.SetSpeed((phase.GetUpSpeed() + phase.GetDownSpeed()) *0.5);
						prop.SetArrival();
						prop.SetMode(getPhaseMode(phase,true));
						prop.SetAlt((phase.GetDownAltitude() + phase.GetUpAltitude())*0.5);

						m_FlightPlanProperties.insert(m_FlightPlanProperties.begin(),prop);
					}

					//regroup this segment
					//first nod

					for (int nPoint = 0; nPoint < static_cast<int>(vAirRoutePoint.size()) -1; ++nPoint)
					{
						pLastSegmentInSim->AddRoutePoint(vAirRoutePoint[nPoint]);
					}
					//
					AirRouteIntersectionInSim* pSecondIntersection = new AirRouteIntersectionInSim(vAirRoutePoint[vAirRoutePoint.size() -1]);			
					pLastSegmentInSim->SetSecondIntersection(pSecondIntersection);

					//new the new last segment
					pNewSegment = new AirRouteSegInSim;
					AirRouteIntersectionInSim* pNewFirst = new AirRouteIntersectionInSim(vAirRoutePoint[vAirRoutePoint.size() -1]);			
					pNewSegment->SetFirstIntersection(pNewFirst);
				}




				pLastFirstConflictIntersection->getInputPoint()->setDistToRunway(dRunwayDist);
				ptPtreLocation = lastSegmentNode;

				dDistanceToRunway += dRunwayDist;

			}

		}

		//the last non-runway segment, start
		//runway, ingore
		//last segment, handled
		//start with 3
		for (int nSeg = nSegCount - 3; nSeg >= 0 ; -- nSeg)
		{
			AirsideResource *pResource = GetItem(nSeg);
			if(pResource == NULL)
				continue;

			if(pResource->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
			{
				HeadingAirRouteSegInSim* pHeadingSeg = (HeadingAirRouteSegInSim*)pResource;

				//second intersection
				if(pHeadingSeg->GetSecondIntersection())
				{
					FlightPlanGenericPhaseInSim tmpPhase;
					if(m_pGenericPlan->GetSTARPhase(dDistanceToRunway, tmpPhase))
					{
						UpdatePointProperties(pHeadingSeg->GetSecondIntersection()->getInputPoint(),tmpPhase,true);
					}
					
					ptPtreLocation = pHeadingSeg->GetSecondIntersection()->getInputPoint()->GetPosition();
				}

				//first intersection
				if(pHeadingSeg->GetFirstIntersection())
				{
					DistanceUnit dist = ptPtreLocation.distance(pHeadingSeg->GetFirstIntersection()->getInputPoint()->GetPosition());
					dDistanceToRunway += dist;


					FlightPlanGenericPhaseInSim tmpPhase;
					if(m_pGenericPlan->GetSTARPhase(dDistanceToRunway, tmpPhase))
					{
						UpdatePointProperties(pHeadingSeg->GetFirstIntersection()->getInputPoint(),tmpPhase,true);
					}
					ptPtreLocation = pHeadingSeg->GetFirstIntersection()->getInputPoint()->GetPosition();
				}

				continue;
			}
			if(pResource->GetType() != AirsideResource::ResType_AirRouteSegment)
				continue;
			

			AirRouteSegInSim *pSeg =  (AirRouteSegInSim *)pResource;
			ASSERT(pSeg != NULL);

			AirRouteIntersectionInSim*pSecondInt = pSeg->GetSecondConcernIntersection();
			if(pSecondInt == NULL)
				continue;

			//AirRoutePointInSim *pPrePoint = pSecondInt->getInputPoint();
			ptPtreLocation = pSecondInt->getInputPoint()->GetPosition();

			FlightPlanGenericPhaseInSim tmpPhase;
			if(m_pGenericPlan->GetSTARPhase(dDistanceToRunway, tmpPhase))
			{
				UpdatePointProperties(pSecondInt->getInputPoint(),tmpPhase,true);
			}

			int nPointCount = pSeg->GetRoutePointCount();
			for (int nPoint = nPointCount - 1; nPoint >= 0; --nPoint)
			{
				AirRoutePointInSim* pPoint = pSeg->GetRoutePointByIdx(nPoint);
				DistanceUnit dist = ptPtreLocation.distance(pPoint->GetPosition());

				//get the middle phases
				std::vector<FlightPlanGenericPhaseInSim> vConcernPhase = m_pGenericPlan->getConcernPhase(dDistanceToRunway, dDistanceToRunway + dist);
				if(vConcernPhase.size() > 0)
				{
					AirRoutePointInSim *pPrePoint = pPoint;
					int nPhaseCount = static_cast<int>(vConcernPhase.size());
					for (int nPhase = nPhaseCount - 1; nPhase >= 0; --nPhase)
					{
						FlightPlanGenericPhaseInSim& phase = vConcernPhase[nPhase];
						CPoint2008 ptPhasePosition = ptPtreLocation.GetDistancePoint(pPoint->GetPosition(), phase.GetDistanceToRunway() - dDistanceToRunway);


						FlightPlanPhasePointInSim *pPhasepoint = new FlightPlanPhasePointInSim;
						pPhasepoint->SetAltHigh(phase.GetUpAltitude());
						pPhasepoint->SetAltLow(phase.GetDownAltitude());
						pPhasepoint->SetPosition(ptPhasePosition);
						pPhasepoint->setDistToRunway(phase.GetDistanceToRunway());

						pSeg->InsertRouteAfterPoint(pPrePoint, pPhasepoint);

						FlightPlanPropertyInSim prop(pPhasepoint);
						prop.SetSpeed((phase.GetUpSpeed() + phase.GetDownSpeed()) *0.5);
						prop.SetArrival();
						prop.SetMode(getPhaseMode(phase, true));
						prop.SetAlt((phase.GetDownAltitude() + phase.GetUpAltitude())*0.5);

						m_FlightPlanProperties.push_back(prop);

						pPrePoint = pPhasepoint;

					}

				}

				dDistanceToRunway += dist;
				//update itself
				FlightPlanGenericPhaseInSim tmpPhase;
				if(m_pGenericPlan->GetSTARPhase(dDistanceToRunway, tmpPhase))
				{
					UpdatePointProperties(pPoint,tmpPhase,true);
				}
				ptPtreLocation = pPoint->GetPosition();
				pPoint->setDistToRunway(dDistanceToRunway);
			}
			//calculate to first intersection
			AirRouteIntersectionInSim* pFirstIntersection = pSeg->GetFirstConcernIntersection();
			if(pFirstIntersection)
			{
				CPoint2008 ptFirst = pFirstIntersection->getInputPoint()->GetPosition();
				DistanceUnit dist = ptPtreLocation.distance(ptFirst);

				std::vector<FlightPlanGenericPhaseInSim> vConcernPhase = m_pGenericPlan->getConcernPhase(dDistanceToRunway, dDistanceToRunway + dist);
				if(vConcernPhase.size() > 0)
				{
					AirRoutePointInSim *pPrePoint = pFirstIntersection->getInputPoint();
					int nPhaseCount = static_cast<int>(vConcernPhase.size());
					for (int nPhase = nPhaseCount - 1; nPhase >= 0; --nPhase)
					{
						FlightPlanGenericPhaseInSim& phase = vConcernPhase[nPhase];
						CPoint2008 ptPhasePosition = ptPtreLocation.GetDistancePoint(ptFirst, phase.GetDistanceToRunway() - dDistanceToRunway);

						FlightPlanPhasePointInSim *pPhasepoint = new FlightPlanPhasePointInSim;
						pPhasepoint->SetAltHigh(phase.GetUpAltitude());
						pPhasepoint->SetAltLow(phase.GetDownAltitude());
						pPhasepoint->SetPosition(ptPhasePosition);
						pPhasepoint->setDistToRunway(phase.GetDistanceToRunway());

						pSeg->InsertRouteAfterPoint(pPrePoint, pPhasepoint);

						FlightPlanPropertyInSim prop(pPhasepoint);
						prop.SetSpeed((phase.GetUpSpeed() + phase.GetDownSpeed()) *0.5);
						prop.SetArrival();
						prop.SetMode(getPhaseMode(phase,true));
						prop.SetAlt((phase.GetDownAltitude() + phase.GetUpAltitude())*0.5);

						m_FlightPlanProperties.push_back(prop);

						pPrePoint = pPhasepoint;

					}
				}
				dDistanceToRunway += dist;

				//update itself
				FlightPlanGenericPhaseInSim tmpPhase;
				if(m_pGenericPlan->GetSTARPhase(dDistanceToRunway, tmpPhase))
				{
					UpdatePointProperties(pFirstIntersection->getInputPoint(),tmpPhase,true);
				}

				pFirstIntersection->getInputPoint()->setDistToRunway(dDistanceToRunway);
			}
		}

		if(pNewSegment)//add this segment to the route
		{
			m_vItems.insert(m_vItems.end() - 1, pNewSegment);
		}
		////{
		////	//print all the segment and node
		////	int nSegCount = GetItemCount();
		////	for (int nSeg = 0; nSeg <nSegCount -2 ; ++ nSeg)
		////	{
		////		AirRouteSegInSim *pSeg =  (AirRouteSegInSim *)GetItem(nSeg);
		////		ASSERT(pSeg != NULL);
		////		AirRouteIntersectionInSim* pFirst = pSeg->GetFirstConcernIntersection();

		////		if(pFirst != NULL)
		////		{
		////			AirRoutePointInSim *pPoint = pFirst->getInputPoint();
		////			TRACE(_T("\r\n Seg: %d,Dist: %f ,AL: %f, AH: %f, %s"),nSeg,
		////				pPoint->getDistToRunway() ,
		////				pPoint->GetAltLow(),
		////				pPoint->GetAltHigh(),
		////				pPoint->PrintResource());
		////		}
		////		int nPointCount = pSeg->GetRoutePointCount();
		////		for (int nPoint = 0; nPoint < nPointCount ; ++nPoint)
		////		{
		////			AirRoutePointInSim* pPoint = pSeg->GetRoutePointByIdx(nPoint);
		////			TRACE(_T("\r\n Seg: %d,Dist: %f ,AL: %f, AH: %f, %s"),nSeg,
		////				pPoint->getDistToRunway() ,
		////				pPoint->GetAltLow(),
		////				pPoint->GetAltHigh(),
		////				pPoint->PrintResource());
		////		}

		////		AirRouteIntersectionInSim* pSecond = pSeg->GetSecondConcernIntersection();

		////		if(pSecond != NULL)
		////		{
		////			AirRoutePointInSim *pPoint = pSecond->getInputPoint();
		////			TRACE(_T("\r\n Seg: %d,Dist: %f ,AL: %f, AH: %f, %s"),nSeg,
		////				pPoint->getDistToRunway() ,
		////				pPoint->GetAltLow(),
		////				pPoint->GetAltHigh(),
		////				pPoint->PrintResource());
		////		}
		////		TRACE("\r\n");

		////	}
		////}
	}
}

void FlightPlanGenericRouteInSim::InitializeSID( AirsideFlightInSim *pFlight )
{
	CAirRoute::RouteType rtType = m_pAirRouteInSim->getRouteType();
	if(rtType ==  CAirRoute::SID)//runway -> seg -> seg ->...
	{
		//the first segment is runway
		int nSegCount = GetItemCount();
		ASSERT(nSegCount > 0);
		//calculate the distance, runway to last segment
		double dDistanceToRunway = 0.0;
		CPoint2008 ptPtreLocation;
		AirRouteSegInSim *pNewSegment = NULL;
		if(nSegCount > 0)
		{
			LogicRunwayInSim * pTakeOffRunway  = (LogicRunwayInSim *)m_vItems[0];
			ASSERT(pTakeOffRunway != NULL);
			CPoint2008 ptRunwayPort = pTakeOffRunway->GetOtherPort()->GetDistancePoint(0.0);
			ptPtreLocation = ptRunwayPort;
			if(nSegCount > 1)
			{
				AirRouteSegInSim *pLastSegmentInSim = (AirRouteSegInSim*)m_vItems.at(1);

				AirRouteIntersectionInSim* pLastFirstConflictIntersection = pLastSegmentInSim->GetFirstConcernIntersection();
				CPoint2008 lastSegmentNode = pLastFirstConflictIntersection->getInputPoint()->GetPosition();
				double dRunwayDist = lastSegmentNode.distance(ptRunwayPort);

				pLastFirstConflictIntersection->getInputPoint()->setDistToRunway(dDistanceToRunway);


				//add new segment if necessary
				std::vector<FlightPlanGenericPhaseInSim> vPhases =m_pGenericPlan->getConcernPhase(0,dRunwayDist);
				if (vPhases.size() > 0)
				{
					//add new segment
					std::vector<AirRoutePointInSim * > vAirRoutePoint;
					int nPhaseCount = static_cast<int>(vPhases.size());
					for (int nPhase = 0 ; nPhase < nPhaseCount; ++nPhase)
					{
						FlightPlanGenericPhaseInSim& phase = vPhases[nPhase];
						CPoint2008 ptPhasePosition = ptRunwayPort.GetDistancePoint(lastSegmentNode,phase.GetDistanceToRunway());

						FlightPlanPhasePointInSim *pPhasepoint = new FlightPlanPhasePointInSim;
						pPhasepoint->SetAltHigh(phase.GetUpAltitude());
						pPhasepoint->SetAltLow(phase.GetDownAltitude());
						pPhasepoint->SetPosition(ptPhasePosition);
						pPhasepoint->setDistToRunway(phase.GetDistanceToRunway());
						vAirRoutePoint.push_back(pPhasepoint);

						FlightPlanPropertyInSim prop(pPhasepoint);
						prop.SetSpeed((phase.GetUpSpeed() + phase.GetDownSpeed()) *0.5);
						prop.SetMode(getPhaseMode(phase,false));
						prop.SetAlt((phase.GetDownAltitude() + phase.GetUpAltitude())*0.5);
						m_FlightPlanProperties.insert(m_FlightPlanProperties.begin(),prop);

					}

					//regroup this segment
					//first node

					//new the new last segment
					pNewSegment = new AirRouteSegInSim;
					AirRouteIntersectionInSim* pNewFirst = new AirRouteIntersectionInSim(vAirRoutePoint[0]);			
					pNewSegment->SetFirstIntersection(pNewFirst);

					for (int nPoint = 1; nPoint < static_cast<int>(vAirRoutePoint.size()); ++nPoint)
					{
						pNewSegment->AddRoutePoint(vAirRoutePoint[nPoint]);
					}
					AirRouteIntersectionInSim* pSecondIntersection = new AirRouteIntersectionInSim(pLastFirstConflictIntersection->getInputPoint());			
					pNewSegment->SetSecondIntersection(pSecondIntersection);

				}

				ptPtreLocation = lastSegmentNode;
				dDistanceToRunway += dRunwayDist;
			}
		}

		//the first non-runway segment, start
		for (int nSeg = 1; nSeg < nSegCount ; ++ nSeg)
		{
			AirsideResource *pResource = GetItem(nSeg);
			if(pResource == NULL)
				continue;

			if(pResource->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
			{
				HeadingAirRouteSegInSim* pHeadingSeg = (HeadingAirRouteSegInSim*)pResource;
				//first intersection
				if(pHeadingSeg->GetFirstIntersection())
				{
					FlightPlanGenericPhaseInSim tmpPhase;
					if(m_pGenericPlan->GetSIDPhase(dDistanceToRunway, tmpPhase))
					{
						UpdatePointProperties(pHeadingSeg->GetFirstIntersection()->getInputPoint(),tmpPhase,true);
					}
					ptPtreLocation = pHeadingSeg->GetFirstIntersection()->getInputPoint()->GetPosition();
				}

				//second intersection
				if(pHeadingSeg->GetSecondIntersection())
				{
					DistanceUnit dist = ptPtreLocation.distance(pHeadingSeg->GetSecondIntersection()->getInputPoint()->GetPosition());
					dDistanceToRunway += dist;

					ptPtreLocation = pHeadingSeg->GetSecondIntersection()->getInputPoint()->GetPosition();

					FlightPlanGenericPhaseInSim tmpPhase;
					if(m_pGenericPlan->GetSIDPhase(dDistanceToRunway, tmpPhase))
					{
						UpdatePointProperties(pHeadingSeg->GetSecondIntersection()->getInputPoint(),tmpPhase,true);
					}
				}

				continue;
		
			}
			if(pResource->GetType() != AirsideResource::ResType_AirRouteSegment)
				continue;

			AirRouteSegInSim *pSeg =  (AirRouteSegInSim *)pResource;
			ASSERT(pSeg != NULL);

			AirRouteIntersectionInSim* pFirstInt = pSeg->GetFirstConcernIntersection();

			//update first point
			FlightPlanGenericPhaseInSim tmpPhase;
			if(m_pGenericPlan->GetSIDPhase(dDistanceToRunway, tmpPhase))
			{
				UpdatePointProperties(pFirstInt->getInputPoint(),tmpPhase,false);
			}
			pFirstInt->getInputPoint()->setDistToRunway(dDistanceToRunway);

			ptPtreLocation = pFirstInt->getInputPoint()->GetPosition();

			AirRoutePointInSim *pPrePoint = pFirstInt->getInputPoint();

			for (int nPoint = 0; nPoint < pSeg->GetRoutePointCount(); ++ nPoint)
			{
				AirRoutePointInSim* pPoint = pSeg->GetRoutePointByIdx(nPoint);
				DistanceUnit dist = ptPtreLocation.distance(pPoint->GetPosition());

				std::vector<FlightPlanGenericPhaseInSim> vConcernPhase = m_pGenericPlan->getConcernPhase(dDistanceToRunway, dDistanceToRunway + dist);
				if(vConcernPhase.size() > 0)
				{
					int nPhaseCount = static_cast<int>(vConcernPhase.size());
					for (int nPhase = 0; nPhase < nPhaseCount ; ++nPhase)
					{
						FlightPlanGenericPhaseInSim& phase = vConcernPhase[nPhase];
						CPoint2008 ptPhasePosition = ptPtreLocation.GetDistancePoint(pPoint->GetPosition(), phase.GetDistanceToRunway() - dDistanceToRunway);


						FlightPlanPhasePointInSim *pPhasepoint = new FlightPlanPhasePointInSim;
						pPhasepoint->SetAltHigh(phase.GetUpAltitude());
						pPhasepoint->SetAltLow(phase.GetDownAltitude());
						pPhasepoint->SetPosition(ptPhasePosition);
						pPhasepoint->setDistToRunway(phase.GetDistanceToRunway());

						pSeg->InsertRouteAfterPoint(pPrePoint, pPhasepoint);

						FlightPlanPropertyInSim prop(pPhasepoint);
						prop.SetSpeed((phase.GetUpSpeed() + phase.GetDownSpeed()) *0.5);
						prop.SetArrival();
						prop.SetMode(getPhaseMode(phase, true));
						prop.SetAlt((phase.GetDownAltitude() + phase.GetUpAltitude())*0.5);

						m_FlightPlanProperties.push_back(prop);

						pPrePoint = pPhasepoint;
						nPoint += 1;
					}
				}


				dDistanceToRunway += dist;
				//update itself
				FlightPlanGenericPhaseInSim tmpPhase;
				if(m_pGenericPlan->GetSIDPhase(dDistanceToRunway, tmpPhase))
				{
					UpdatePointProperties(pPoint,tmpPhase,false);
				}

				ptPtreLocation = pPoint->GetPosition();	
				pPoint->setDistToRunway(dDistanceToRunway);
				pPrePoint = pPoint;

			}

			//calculate to first intersection
			AirRouteIntersectionInSim* pSecondIntersection = pSeg->GetSecondConcernIntersection();
			if(pSecondIntersection)
			{
				CPoint2008 ptSecond = pSecondIntersection->getInputPoint()->GetPosition();
				DistanceUnit dist = ptPtreLocation.distance(ptSecond);

				std::vector<FlightPlanGenericPhaseInSim> vConcernPhase = m_pGenericPlan->getConcernPhase(dDistanceToRunway, dDistanceToRunway + dist);
				if(vConcernPhase.size() > 0)
				{
					int nPhaseCount = static_cast<int>(vConcernPhase.size());
					for (int nPhase = 0; nPhase < nPhaseCount; ++nPhase)
					{
						FlightPlanGenericPhaseInSim& phase = vConcernPhase[nPhase];
						CPoint2008 ptPhasePosition = ptPtreLocation.GetDistancePoint(ptSecond, phase.GetDistanceToRunway() - dDistanceToRunway);

						FlightPlanPhasePointInSim *pPhasepoint = new FlightPlanPhasePointInSim;
						pPhasepoint->SetAltHigh(phase.GetUpAltitude());
						pPhasepoint->SetAltLow(phase.GetDownAltitude());
						pPhasepoint->SetPosition(ptPhasePosition);
						pPhasepoint->setDistToRunway(phase.GetDistanceToRunway());

						pSeg->InsertRouteAfterPoint(pPrePoint, pPhasepoint);

						FlightPlanPropertyInSim prop(pPhasepoint);
						prop.SetSpeed((phase.GetUpSpeed() + phase.GetDownSpeed()) *0.5);
						prop.SetArrival();
						prop.SetMode(getPhaseMode(phase, false));
						prop.SetAlt((phase.GetDownAltitude() + phase.GetUpAltitude())*0.5);

						m_FlightPlanProperties.push_back(prop);
						pPrePoint = pPhasepoint;
					}
				}

				dDistanceToRunway += dist;
				//update second intersection
				FlightPlanGenericPhaseInSim tmpPhase;
				if(m_pGenericPlan->GetSIDPhase(dDistanceToRunway, tmpPhase))
				{
					UpdatePointProperties(pSecondIntersection->getInputPoint(),tmpPhase,false);
				}

				pSecondIntersection->getInputPoint()->setDistToRunway(dDistanceToRunway);
			}
		}

		if(pNewSegment != NULL)
		{
			m_vItems.insert(m_vItems.begin() + 1, pNewSegment);
		}

		//////{
		//////	//print all the segment and node
		//////	int nSegCount = GetItemCount();
		//////	for (int nSeg = 1; nSeg < nSegCount ; ++ nSeg)
		//////	{
		//////		AirRouteSegInSim *pSeg =  (AirRouteSegInSim *)GetItem(nSeg);
		//////		ASSERT(pSeg != NULL);
		//////		AirRouteIntersectionInSim* pFirst = pSeg->GetFirstConcernIntersection();

		//////		if(pFirst != NULL)
		//////		{
		//////			AirRoutePointInSim *pPoint = pFirst->getInputPoint();
		//////			TRACE(_T("\r\n Seg: %d,Dist: %f ,AL: %f, AH: %f, %s"),nSeg,
		//////				pPoint->getDistToRunway() ,
		//////				pPoint->GetAltLow(),
		//////				pPoint->GetAltHigh(),
		//////				pPoint->PrintResource());
		//////		}
		//////		int nPointCount = pSeg->GetRoutePointCount();
		//////		for (int nPoint = 0; nPoint < nPointCount ; ++nPoint)
		//////		{
		//////			AirRoutePointInSim* pPoint = pSeg->GetRoutePointByIdx(nPoint);
		//////			TRACE(_T("\r\n Seg: %d,Dist: %f ,AL: %f, AH: %f, %s"),nSeg,
		//////				pPoint->getDistToRunway() ,
		//////				pPoint->GetAltLow(),
		//////				pPoint->GetAltHigh(),
		//////				pPoint->PrintResource());
		//////		}

		//////		AirRouteIntersectionInSim* pSecond = pSeg->GetSecondConcernIntersection();

		//////		if(pSecond != NULL)
		//////		{
		//////			AirRoutePointInSim *pPoint = pSecond->getInputPoint();
		//////			TRACE(_T("\r\n Seg: %d,Dist: %f ,AL: %f, AH: %f, %s"),nSeg,
		//////				pPoint->getDistToRunway() ,
		//////				pPoint->GetAltLow(),
		//////				pPoint->GetAltHigh(),
		//////				pPoint->PrintResource());
		//////		}
		//////		TRACE("\r\n");

		//////	}
		//////}

	}
}

double FlightPlanGenericRouteInSim::GetSpeed( AirsideFlightInSim *pFlight, const FlightPlanGenericPhaseInSim& phase ) const
{
	return 100;
}



FlightPlanGenericInSim::FlightPlanGenericInSim( ns_FlightPlan::FlightPlanGeneric *pGenericPlan )
:m_pGenericPlan(pGenericPlan)
{
	Initialize();
}

FlightPlanGenericInSim::~FlightPlanGenericInSim()
{

}

void FlightPlanGenericInSim::Initialize()
{
	//sort the data
	int nItemCount = m_pGenericPlan->GetCount();
	for (int nItem = 0; nItem < nItemCount; ++nItem)
	{
		m_vPhases.push_back(FlightPlanGenericPhaseInSim(*(m_pGenericPlan->GetItem(nItem))));
	}
	std::sort(m_vPhases.begin(), m_vPhases.end(), FlightPlanGenericInSim::CompareItem);



}

bool FlightPlanGenericInSim::CompareItem(const FlightPlanGenericPhaseInSim& phase1,const FlightPlanGenericPhaseInSim&phase2 )
{
	if(phase1.GetDistanceToRunway() < phase2.GetDistanceToRunway())
		return true;

	return false;
}

ElapsedTime FlightPlanGenericInSim::GetFlightTimeInSeg( AirsideFlightInSim* pFlight, double dDistFrom, double dDistTo )
{
	if(m_vPhases.size() == 0)
	{
		if(pFlight && pFlight->GetSpeed() > 0.0)
			return ElapsedTime(abs(dDistFrom - dDistTo)/pFlight->GetSpeed());
		else
			return ElapsedTime(0L);
	}

	if(dDistFrom > dDistTo)//to runway
	{	
		std::vector<FlightPlanGenericPhaseInSim> vPhases = m_vPhases;
		std::reverse(vPhases.begin(), vPhases.end());//big->little
		
		if(dDistFrom > vPhases[0].GetDistanceToRunway() && dDistTo > vPhases[0].GetDistanceToRunway())//all out of range
		{
			double dTime = (dDistFrom - dDistTo)/getSpeed(vPhases[0]);
			return ElapsedTime(dTime);
		}

		double dDistTravel = dDistFrom - dDistTo;

		double dTime = 0.0;
		if(dDistFrom > vPhases[0].GetDistanceToRunway())//dDitanceTo is in the range
		{
			//calculate the out of range
			dTime += (dDistFrom - vPhases[0].GetDistanceToRunway())/getSpeed(vPhases[0]);
			
			dDistTravel -= dDistFrom - vPhases[0].GetDistanceToRunway();
			
			int nCount = static_cast<int>(vPhases.size());
			for (int nItem = 0; nItem < nCount - 1; ++ nItem)
			{
				double dRange1 = vPhases[nItem].GetDistanceToRunway();
				double dRange2 = vPhases[nItem + 1].GetDistanceToRunway();

				if(dRange1 - dRange2 > dDistTravel)
				{
					dTime += (dRange1 - dRange2)/getSpeed(vPhases[nItem]);
					dDistTravel -= dRange1 - dRange2;
				}
				else
				{
					dTime += dDistTravel/getSpeed(vPhases[nItem]);
					dDistTravel = 0.0;
					break;
				}
			}

			if(dDistTravel > 0.0)//still have 
			{
				dTime += dDistTravel/getSpeed(vPhases[nCount-1]);
			}
		}
		else
		{	
			//calculate time in the range
			int nStartItem = 0;
			int nCount = static_cast<int>(vPhases.size());
			for (int nItem = 0; nItem < nCount - 1; ++ nItem)
			{
				if(vPhases[nItem + 1].GetDistanceToRunway() < dDistFrom )
				{
					dTime += (dDistFrom - vPhases[nItem + 1].GetDistanceToRunway())/getSpeed(vPhases[nItem]); 
					dDistTravel -= dDistFrom - vPhases[nItem + 1].GetDistanceToRunway();

					nStartItem = nItem + 1;
					break;
				}
			}
			if(nStartItem == -1)//direct to last phase
				nStartItem = nCount;

			for (; nStartItem < nCount - 1; ++ nStartItem)
			{
				double dRange1 = vPhases[nItem].GetDistanceToRunway();
				double dRange2 = vPhases[nItem + 1].GetDistanceToRunway();

				if(dRange1 - dRange2 > dDistTravel)
				{
					dTime += (dRange1 - dRange2)/getSpeed(vPhases[nItem]);
					dDistTravel -= dRange1 - dRange2;
				}
				else
				{
					dTime += dDistTravel/getSpeed(vPhases[nItem]);
					dDistTravel = 0.0;
					break;
				}
			}
			if(dDistTravel > 0.0)//still have 
			{
				dTime += dDistTravel/getSpeed(vPhases[nCount-1]);
			}
		}

		return ElapsedTime(dTime);
	}
	else//leave runway
	{	
		double dDistTravel = dDistTo - dDistFrom;
		
		double dTime = 0.0;
		if(dDistTo < m_vPhases[0].GetDistanceToRunway())
		{
			dTime += dDistTravel/getSpeed(m_vPhases[0]);
			return ElapsedTime(dTime);
		}
		
		if(dDistFrom < m_vPhases[0].GetDistanceToRunway())
		{
			dTime += (m_vPhases[0].GetDistanceToRunway() - dDistFrom)/getSpeed(m_vPhases[0]);
			dDistTravel -= m_vPhases[0].GetDistanceToRunway() - dDistFrom;
			
			int nCount = static_cast<int>(m_vPhases.size());
			for (int nItem = 0; nItem < nCount - 1; ++ nItem)
			{
				double dRange1 = m_vPhases[nItem].GetDistanceToRunway();
				double dRange2 = m_vPhases[nItem + 1].GetDistanceToRunway();

				if(dRange1 - dRange2 > dDistTravel)
				{
					dTime += (dRange1 - dRange2)/getSpeed(m_vPhases[nItem]);
					dDistTravel -= dRange1 - dRange2;
				}
				else
				{
					dTime += dDistTravel/getSpeed(m_vPhases[nItem]);
					dDistTravel = 0.0;
					break;
				}
			}

			if(dDistTravel > 0.0)//still have 
			{
				dTime += dDistTravel/getSpeed(m_vPhases[nCount-1]);
			}
		}
		else
		{
			int nStartItem = 0;

			int nCount = static_cast<int>(m_vPhases.size());
			for (int nItem = 0; nItem < nCount - 1; ++ nItem)
			{
				if(dDistFrom < m_vPhases[nItem + 1].GetDistanceToRunway() )
				{
					dTime += (m_vPhases[nItem + 1].GetDistanceToRunway() - dDistFrom )/getSpeed(m_vPhases[nItem]); 
					dDistTravel -=  m_vPhases[nItem + 1].GetDistanceToRunway() - dDistFrom;

					nStartItem = nItem + 1;
					break;
				}
			}
			if(nStartItem == -1)//direct to last phase
				nStartItem = nCount;
			for (; nStartItem < nCount - 1; ++ nStartItem)
			{
				double dRange1 = m_vPhases[nItem].GetDistanceToRunway();
				double dRange2 = m_vPhases[nItem + 1].GetDistanceToRunway();

				if(dRange1 - dRange2 > dDistTravel)
				{
					dTime += (dRange1 - dRange2)/getSpeed(m_vPhases[nItem]);
					dDistTravel -= dRange1 - dRange2;
				}
				else
				{
					dTime += dDistTravel/getSpeed(m_vPhases[nItem]);
					dDistTravel = 0.0;
					break;
				}
			}
			if(dDistTravel > 0.0)//still have 
			{
				dTime += dDistTravel/getSpeed(m_vPhases[nCount-1]);
			}
		}
		return ElapsedTime(dTime);
	}

}

ElapsedTime FlightPlanGenericInSim::GetFlightMinTimeInSeg( AirsideFlightInSim* pFlight, double dDistFrom, double dDistTo )
{
	if(m_vPhases.size() == 0)
	{
		if(pFlight && pFlight->GetSpeed() > 0.0)
			return ElapsedTime(abs(dDistFrom - dDistTo)/pFlight->GetSpeed());
		else
			return ElapsedTime(0L);
	}

	if(dDistFrom > dDistTo)//to runway
	{	
		std::vector<FlightPlanGenericPhaseInSim> vPhases = m_vPhases;
		std::reverse(vPhases.begin(), vPhases.end());//big->little
		
		if(dDistFrom > vPhases[0].GetDistanceToRunway() && dDistTo > vPhases[0].GetDistanceToRunway())//all out of range
		{
			double dTime = (dDistFrom - dDistTo)/getMaxSpeed(vPhases[0]);
			return ElapsedTime(dTime);
		}

		double dDistTravel = dDistFrom - dDistTo;

		double dTime = 0.0;
		if(dDistFrom > vPhases[0].GetDistanceToRunway())//dDitanceTo is in the range
		{
			//calculate the out of range
			dTime += (dDistFrom - vPhases[0].GetDistanceToRunway())/getMaxSpeed(vPhases[0]);
			
			dDistTravel -= dDistFrom - vPhases[0].GetDistanceToRunway();
			
			int nCount = static_cast<int>(vPhases.size());
			for (int nItem = 0; nItem < nCount - 1; ++ nItem)
			{
				double dRange1 = vPhases[nItem].GetDistanceToRunway();
				double dRange2 = vPhases[nItem + 1].GetDistanceToRunway();

				if(dRange1 - dRange2 > dDistTravel)
				{
					dTime += (dRange1 - dRange2)/getMaxSpeed(vPhases[nItem]);
					dDistTravel -= dRange1 - dRange2;
				}
				else
				{
					dTime += dDistTravel/getMaxSpeed(vPhases[nItem]);
					dDistTravel = 0.0;
					break;
				}
			}

			if(dDistTravel > 0.0)//still have 
			{
				dTime += dDistTravel/getMaxSpeed(vPhases[nCount-1]);
			}
		}
		else
		{	
			//calculate time in the range
			int nStartItem = 0;
			int nCount = static_cast<int>(vPhases.size());
			for (int nItem = 0; nItem < nCount - 1; ++ nItem)
			{
				if(vPhases[nItem + 1].GetDistanceToRunway() < dDistFrom )
				{
					dTime += (dDistFrom - vPhases[nItem + 1].GetDistanceToRunway())/getMaxSpeed(vPhases[nItem]); 
					dDistTravel -= dDistFrom - vPhases[nItem + 1].GetDistanceToRunway();

					nStartItem = nItem + 1;
					break;
				}
			}
			if(nStartItem == -1)//direct to last phase
				nStartItem = nCount;
			for (; nStartItem < nCount - 1; ++ nStartItem)
			{
				double dRange1 = vPhases[nItem].GetDistanceToRunway();
				double dRange2 = vPhases[nItem + 1].GetDistanceToRunway();

				if(dRange1 - dRange2 > dDistTravel)
				{
					dTime += (dRange1 - dRange2)/getMaxSpeed(vPhases[nItem]);
					dDistTravel -= dRange1 - dRange2;
				}
				else
				{
					dTime += dDistTravel/getMaxSpeed(vPhases[nItem]);
					dDistTravel = 0.0;
					break;
				}
			}
			if(dDistTravel > 0.0)//still have 
			{
				dTime += dDistTravel/getMaxSpeed(vPhases[nCount-1]);
			}
		}

		return ElapsedTime(dTime);
	}
	else//leave runway
	{	
		double dDistTravel = dDistTo - dDistFrom;
		
		double dTime = 0.0;
		if(dDistTo < m_vPhases[0].GetDistanceToRunway())
		{
			dTime += dDistTravel/getMaxSpeed(m_vPhases[0]);
			return ElapsedTime(dTime);
		}
		
		if(dDistFrom < m_vPhases[0].GetDistanceToRunway())
		{
			dTime += (m_vPhases[0].GetDistanceToRunway() - dDistFrom)/getMaxSpeed(m_vPhases[0]);
			dDistTravel -= m_vPhases[0].GetDistanceToRunway() - dDistFrom;
			
			int nCount = static_cast<int>(m_vPhases.size());
			for (int nItem = 0; nItem < nCount - 1; ++ nItem)
			{
				double dRange1 = m_vPhases[nItem].GetDistanceToRunway();
				double dRange2 = m_vPhases[nItem + 1].GetDistanceToRunway();

				if(dRange1 - dRange2 > dDistTravel)
				{
					dTime += (dRange1 - dRange2)/getMaxSpeed(m_vPhases[nItem]);
					dDistTravel -= dRange1 - dRange2;
				}
				else
				{
					dTime += dDistTravel/getMaxSpeed(m_vPhases[nItem]);
					dDistTravel = 0.0;
					break;
				}
			}

			if(dDistTravel > 0.0)//still have 
			{
				dTime += dDistTravel/getMaxSpeed(m_vPhases[nCount-1]);
			}
		}
		else
		{
			int nStartItem = 0;

			int nCount = static_cast<int>(m_vPhases.size());
			for (int nItem = 0; nItem < nCount - 1; ++ nItem)
			{
				if(dDistFrom < m_vPhases[nItem + 1].GetDistanceToRunway() )
				{
					dTime += (m_vPhases[nItem + 1].GetDistanceToRunway() - dDistFrom )/getMaxSpeed(m_vPhases[nItem]); 
					dDistTravel -=  m_vPhases[nItem + 1].GetDistanceToRunway() - dDistFrom;

					nStartItem = nItem + 1;
					break;
				}
			}
			if(nStartItem == -1)//direct to last phase
				nStartItem = nCount;

			for (; nStartItem < nCount - 1; ++ nStartItem)
			{
				double dRange1 = m_vPhases[nItem].GetDistanceToRunway();
				double dRange2 = m_vPhases[nItem + 1].GetDistanceToRunway();

				if(dRange1 - dRange2 > dDistTravel)
				{
					dTime += (dRange1 - dRange2)/getMaxSpeed(m_vPhases[nItem]);
					dDistTravel -= dRange1 - dRange2;
				}
				else
				{
					dTime += dDistTravel/getMaxSpeed(m_vPhases[nItem]);
					dDistTravel = 0.0;
					break;
				}
			}
			if(dDistTravel > 0.0)//still have 
			{
				dTime += dDistTravel/getMaxSpeed(m_vPhases[nCount-1]);
			}
		}
		return ElapsedTime(dTime);
	}

}

ElapsedTime FlightPlanGenericInSim::GetFlightMaxTimeInSeg( AirsideFlightInSim* pFlight, double dDistFrom, double dDistTo )
{
	if(m_vPhases.size() == 0)
	{
		if(pFlight && pFlight->GetSpeed() > 0.0)
			return ElapsedTime(abs(dDistFrom - dDistTo)/pFlight->GetSpeed());
		else
			return ElapsedTime(0L);
	}

	if(dDistFrom > dDistTo)//to runway
	{	
		std::vector<FlightPlanGenericPhaseInSim> vPhases = m_vPhases;
		std::reverse(vPhases.begin(), vPhases.end());//big->little
		
		if(dDistFrom > vPhases[0].GetDistanceToRunway() && dDistTo > vPhases[0].GetDistanceToRunway())//all out of range
		{
			double dTime = (dDistFrom - dDistTo)/getMinSpeed(vPhases[0]);
			return ElapsedTime(dTime);
		}

		double dDistTravel = dDistFrom - dDistTo;

		double dTime = 0.0;
		if(dDistFrom > vPhases[0].GetDistanceToRunway())//dDitanceTo is in the range
		{
			//calculate the out of range
			dTime += (dDistFrom - vPhases[0].GetDistanceToRunway())/getMinSpeed(vPhases[0]);
			
			dDistTravel -= dDistFrom - vPhases[0].GetDistanceToRunway();
			
			int nCount = static_cast<int>(vPhases.size());
			for (int nItem = 0; nItem < nCount - 1; ++ nItem)
			{
				double dRange1 = vPhases[nItem].GetDistanceToRunway();
				double dRange2 = vPhases[nItem + 1].GetDistanceToRunway();

				if(dRange1 - dRange2 > dDistTravel)
				{
					dTime += (dRange1 - dRange2)/getMinSpeed(vPhases[nItem]);
					dDistTravel -= dRange1 - dRange2;
				}
				else
				{
					dTime += dDistTravel/getMinSpeed(vPhases[nItem]);
					dDistTravel = 0.0;
					break;
				}
			}

			if(dDistTravel > 0.0)//still have 
			{
				dTime += dDistTravel/getMinSpeed(vPhases[nCount-1]);
			}
		}
		else
		{	
			//calculate time in the range
			int nStartItem = 0;
			int nCount = static_cast<int>(vPhases.size());
			for (int nItem = 0; nItem < nCount - 1; ++ nItem)
			{
				if(vPhases[nItem + 1].GetDistanceToRunway() < dDistFrom )
				{
					dTime += (dDistFrom - vPhases[nItem + 1].GetDistanceToRunway())/getMinSpeed(vPhases[nItem]); 
					dDistTravel -= dDistFrom - vPhases[nItem + 1].GetDistanceToRunway();

					nStartItem = nItem + 1;
					break;
				}
			}
			if(nStartItem == -1)//direct to last phase
				nStartItem = nCount;

			for (; nStartItem < nCount - 1; ++ nStartItem)
			{
				double dRange1 = vPhases[nItem].GetDistanceToRunway();
				double dRange2 = vPhases[nItem + 1].GetDistanceToRunway();

				if(dRange1 - dRange2 > dDistTravel)
				{
					dTime += (dRange1 - dRange2)/getMinSpeed(vPhases[nItem]);
					dDistTravel -= dRange1 - dRange2;
				}
				else
				{
					dTime += dDistTravel/getMinSpeed(vPhases[nItem]);
					dDistTravel = 0.0;
					break;
				}
			}
			if(dDistTravel > 0.0)//still have 
			{
				dTime += dDistTravel/getMinSpeed(vPhases[nCount-1]);
			}
		}

		return ElapsedTime(dTime);
	}
	else//leave runway
	{	
		double dDistTravel = dDistTo - dDistFrom;
		
		double dTime = 0.0;
		if(dDistTo < m_vPhases[0].GetDistanceToRunway())
		{
			dTime += dDistTravel/getMinSpeed(m_vPhases[0]);
			return ElapsedTime(dTime);
		}
		
		if(dDistFrom < m_vPhases[0].GetDistanceToRunway())
		{
			dTime += (m_vPhases[0].GetDistanceToRunway() - dDistFrom)/getMinSpeed(m_vPhases[0]);
			dDistTravel -= m_vPhases[0].GetDistanceToRunway() - dDistFrom;
			
			int nCount = static_cast<int>(m_vPhases.size());
			for (int nItem = 0; nItem < nCount - 1; ++ nItem)
			{
				double dRange1 = m_vPhases[nItem].GetDistanceToRunway();
				double dRange2 = m_vPhases[nItem + 1].GetDistanceToRunway();

				if(dRange1 - dRange2 > dDistTravel)
				{
					dTime += (dRange1 - dRange2)/getMinSpeed(m_vPhases[nItem]);
					dDistTravel -= dRange1 - dRange2;
				}
				else
				{
					dTime += dDistTravel/getMinSpeed(m_vPhases[nItem]);
					dDistTravel = 0.0;
					break;
				}
			}

			if(dDistTravel > 0.0)//still have 
			{
				dTime += dDistTravel/getMinSpeed(m_vPhases[nCount-1]);
			}
		}
		else
		{
			int nStartItem = -1;

			int nCount = static_cast<int>(m_vPhases.size());
			for (int nItem = 0; nItem < nCount - 1; ++ nItem)
			{
				if(dDistFrom < m_vPhases[nItem + 1].GetDistanceToRunway() )
				{
					dTime += (m_vPhases[nItem + 1].GetDistanceToRunway() - dDistFrom )/getMinSpeed(m_vPhases[nItem]); 
					dDistTravel -=  m_vPhases[nItem + 1].GetDistanceToRunway() - dDistFrom;

					nStartItem = nItem + 1;
					break;
				}
			}
			if(nStartItem == -1)//out of phases, last phase
				nStartItem = nCount;

			for (; nStartItem < nCount - 1; ++ nStartItem)
			{
				double dRange1 = m_vPhases[nItem].GetDistanceToRunway();
				double dRange2 = m_vPhases[nItem + 1].GetDistanceToRunway();

				if(dRange1 - dRange2 > dDistTravel)
				{
					dTime += (dRange1 - dRange2)/getMinSpeed(m_vPhases[nItem]);
					dDistTravel -= dRange1 - dRange2;
				}
				else
				{
					dTime += dDistTravel/getMinSpeed(m_vPhases[nItem]);
					dDistTravel = 0.0;
					break;
				}
			}
			if(dDistTravel > 0.0)//still have 
			{
				dTime += dDistTravel/getMinSpeed(m_vPhases[nCount-1]);
			}
		}
		return ElapsedTime(dTime);
	}

}

double FlightPlanGenericInSim::getSpeed( FlightPlanGenericPhaseInSim& phase ) const
{
	return (getMinSpeed(phase) + getMaxSpeed(phase)) * 0.5;
}

double FlightPlanGenericInSim::getMinSpeed( FlightPlanGenericPhaseInSim& phase ) const
{
	return phase.GetDownSpeed();
}

double FlightPlanGenericInSim::getMaxSpeed( FlightPlanGenericPhaseInSim& phase ) const
{
	return phase.GetUpSpeed();
}

ElapsedTime FlightPlanGenericInSim::CalculateLastSegToRunway( AirsideFlightInSim *pFlight,double dLandingSpeed,double dDistanceToRunway )
{	
	//STAR
	if(m_vPhases.size() == 0)
	{
		return ElapsedTime(2.0 * dDistanceToRunway/ (pFlight->GetSpeed() + dLandingSpeed));
	}


	std::vector<FlightPlanGenericPhaseInSim> vPhases = m_vPhases;
	std::reverse(vPhases.begin(), vPhases.end());//big->little
	
	double dTime = 0.0;
	if(dDistanceToRunway > vPhases[0].GetDistanceToRunway())
	{
		//through all phase

		dTime += (dDistanceToRunway - vPhases[0].GetDistanceToRunway())/getSpeed(vPhases[0]);

		int nCount = static_cast<int>(vPhases.size());
		for (int nItem = 0; nItem < nCount-1; ++ nItem)
		{
			double dRange1 = vPhases[nItem].GetDistanceToRunway();
			double dRange2 = vPhases[nItem + 1].GetDistanceToRunway();

			dTime += (dRange1 - dRange2)/getSpeed(vPhases[nItem]);
			dDistanceToRunway -= dRange1 - dRange2;
		}
        ASSERT(dDistanceToRunway >0.0);
		//last phase
		dTime += 2.0 *dDistanceToRunway/(getSpeed(vPhases[nCount -1]) + dLandingSpeed);
	}
	else//dDistance is in range
	{
		//find the start
		int nStartPhase = -1;
		int nCount = static_cast<int>(vPhases.size());
		for (int nItem = 0; nItem < nCount-1; ++ nItem)
		{
			double dRange = vPhases[nItem + 1].GetDistanceToRunway();
			if(dRange < dDistanceToRunway)
			{
				dTime +=  (dDistanceToRunway - dRange)/getSpeed(vPhases[nItem]);
				dDistanceToRunway -= dDistanceToRunway - dRange;
				nStartPhase = nItem + 1;
				break;
			}
		}
		if(nStartPhase == -1)//not find, last phase
			nStartPhase = nCount;

		for (; nStartPhase < nCount; ++nStartPhase)
		{
			double dRange1 = vPhases[nItem].GetDistanceToRunway();
			double dRange2 = vPhases[nItem + 1].GetDistanceToRunway();

			dTime += (dRange1 - dRange2)/getSpeed(vPhases[nItem]);
			dDistanceToRunway -= dRange1 - dRange2;
		}

        ASSERT(dDistanceToRunway >0.0);
		//last phase
		dTime += 2.0 *dDistanceToRunway/(getSpeed(vPhases[nCount -1]) + dLandingSpeed);

	}

	return ElapsedTime(dTime);

}

ElapsedTime FlightPlanGenericInSim::CalculateMinTimeLastSegToRunway( AirsideFlightInSim *pFlight,double dLandingSpeed ,double dDistanceToRunway )
{
	//STAR
	if(m_vPhases.size() == 0)
	{
		return ElapsedTime(2.0 * dDistanceToRunway/ (pFlight->GetSpeed() + dLandingSpeed));
	}


	std::vector<FlightPlanGenericPhaseInSim> vPhases = m_vPhases;
	std::reverse(vPhases.begin(), vPhases.end());//big->little

	double dTime = 0.0;
	if(dDistanceToRunway > vPhases[0].GetDistanceToRunway())
	{
		//through all phase

		dTime += (dDistanceToRunway - vPhases[0].GetDistanceToRunway())/getSpeed(vPhases[0]);

		int nCount = static_cast<int>(vPhases.size());
		for (int nItem = 0; nItem < nCount-1; ++ nItem)
		{
			double dRange1 = vPhases[nItem].GetDistanceToRunway();
			double dRange2 = vPhases[nItem + 1].GetDistanceToRunway();

			dTime += (dRange1 - dRange2)/getMaxSpeed(vPhases[nItem]);
			dDistanceToRunway -= dRange1 - dRange2;
		}
		ASSERT(dDistanceToRunway >0.0);
		//last phase
		dTime += 2.0 *dDistanceToRunway/(getMaxSpeed(vPhases[nCount -1]) + dLandingSpeed);
	}
	else//dDistance is in range
	{
		//find the start
		int nStartPhase = -1;
		int nCount = static_cast<int>(vPhases.size());
		for (int nItem = 0; nItem < nCount-1; ++ nItem)
		{
			double dRange = vPhases[nItem + 1].GetDistanceToRunway();
			if(dRange < dDistanceToRunway)
			{
				dTime +=  (dDistanceToRunway - dRange)/getMaxSpeed(vPhases[nItem]);
				dDistanceToRunway -= dDistanceToRunway - dRange;
				nStartPhase = nItem + 1;
				break;
			}
		}
		if(nStartPhase == -1)//not find, last phase
			nStartPhase = nCount;

		for (; nStartPhase < nCount; ++nStartPhase)
		{
			double dRange1 = vPhases[nItem].GetDistanceToRunway();
			double dRange2 = vPhases[nItem + 1].GetDistanceToRunway();

			dTime += (dRange1 - dRange2)/getMaxSpeed(vPhases[nItem]);
			dDistanceToRunway -= dRange1 - dRange2;
		}

		ASSERT(dDistanceToRunway >0.0);
		//last phase
		dTime += 2.0 *dDistanceToRunway/(getMaxSpeed(vPhases[nCount -1]) + dLandingSpeed);
	}
	return ElapsedTime(dTime);
}



ElapsedTime FlightPlanGenericInSim::CalculateTakeoffTimeFromRwyToFirstSeg( AirsideFlightInSim *pFlight, double dClimboutSpeed, double dDistanceTravel )
{
	//dDistance Travel
	if(m_vPhases.size() == 0)
	{
		return ElapsedTime(dDistanceTravel/dClimboutSpeed);
	}

	//
	if(m_vPhases[0].GetDistanceToRunway() > dDistanceTravel)
	{
		double dTime = 2.0 * dDistanceTravel/(dClimboutSpeed + getSpeed(m_vPhases[0]));
		return ElapsedTime(dTime);
	}
	else
	{
		//runway -> first phase
		double dTime = 2.0 * m_vPhases[0].GetDistanceToRunway()/(dClimboutSpeed + getSpeed(m_vPhases[0]));
		dDistanceTravel -= m_vPhases[0].GetDistanceToRunway();

		//phase segment
		int nCount = static_cast<int>(m_vPhases.size());
		for ( int nItem = 0; nItem < nCount; ++ nItem)
		{
			double dRange1 = m_vPhases[nItem].GetDistanceToRunway();
			double dRange2 = m_vPhases[nItem + 1].GetDistanceToRunway();
			if(dRange2 - dRange1 < dDistanceTravel)
			{
				dTime += (dRange2 - dRange1)/getSpeed(m_vPhases[nItem]);
				dDistanceTravel -= dRange2 - dRange1;
			}
			else
			{
				dTime += (dDistanceTravel -dRange1)/getSpeed(m_vPhases[nItem]);
                dDistanceTravel = 0.0;
				break;
			}
		}

		if(dDistanceTravel > 0)
		{
			dTime += dDistanceTravel/getSpeed(m_vPhases[nCount -1]);
		}

		return ElapsedTime(dTime);
	}

}
//the data has been sort, from small to big
bool FlightPlanGenericInSim::GetSTARPhase( double dDistanceToRunway, FlightPlanGenericPhaseInSim& phase) const
{
	int nCount = static_cast<int>(m_vPhases.size());
	if(nCount == 0)
		return false;

	if(m_vPhases[0].GetDistanceToRunway() > dDistanceToRunway)
	{
		phase = m_vPhases[0];
		return true;
	}

	for (int nItem = 1; nItem < nCount; ++ nItem)//first matched
	{
		if(m_vPhases[nItem].GetDistanceToRunway() > dDistanceToRunway)
		{
			phase = m_vPhases[nItem -1];
			return true;
		}
	}
	//return the last one
	phase = m_vPhases[nCount -1];
	return true;
}

bool FlightPlanGenericInSim::GetSIDPhase( double dDistanceToRunway, FlightPlanGenericPhaseInSim& phase ) const
{
	int nCount = static_cast<int>(m_vPhases.size());
	if(nCount == 0)
		return false;

	for (int nItem = 0; nItem < nCount; ++ nItem)//first matched
	{
		if(m_vPhases[nItem].GetDistanceToRunway() > dDistanceToRunway)
		{
			if(nItem == 0)
                phase = m_vPhases[nItem];
			else
				phase = m_vPhases[nItem -1];//the pre item
			return true;
		}
	}
	//return the last one
	phase = m_vPhases[nCount - 1];
	return true;
}

std::vector<FlightPlanGenericPhaseInSim> FlightPlanGenericInSim::getConcernPhase( double dDistFrom, double dDistanceTo ) const
{
	std::vector<FlightPlanGenericPhaseInSim> vConcern;

	double dDistStart = dDistFrom;
	if(dDistFrom > dDistanceTo)
	{
		dDistStart  = dDistanceTo;
		dDistanceTo = dDistFrom;
	}

	int nCount = static_cast<int>(m_vPhases.size());
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		if(m_vPhases[nItem].GetDistanceToRunway() > dDistStart && m_vPhases[nItem].GetDistanceToRunway() < dDistanceTo )
			vConcern.push_back(m_vPhases[nItem]);
	}

	return vConcern;

}

























FlightPlanGenericPhaseInSim::FlightPlanGenericPhaseInSim( ns_FlightPlan::FlightPlanGenericPhase phase )
{
	m_phase = phase;

	Initialize();
}

FlightPlanGenericPhaseInSim::FlightPlanGenericPhaseInSim()
{

}
FlightPlanGenericPhaseInSim::~FlightPlanGenericPhaseInSim()
{

}
void FlightPlanGenericPhaseInSim::Initialize()
{
	m_dDistToRunway = ARCUnit::ConvertLength(m_phase.GetDistanceToRunway(),ARCUnit::NM,ARCUnit::CM);
	m_dUpAltitude	= ARCUnit::ConvertLength(m_phase.GetUpAltitude(),ARCUnit::FEET,ARCUnit::CM);
	m_dDownAltitude = ARCUnit::ConvertLength(m_phase.GetDownAltitude(),ARCUnit::FEET,ARCUnit::CM);
	m_dAltitudeSep	= ARCUnit::ConvertLength(m_phase.GetAltitudeSeparate(),ARCUnit::FEET,ARCUnit::CM);
	m_dUpSpeed		= ARCUnit::ConvertVelocity(m_phase.GetUpSpeed(),ARCUnit::KNOT,ARCUnit::CMpS);
	m_dDownSpeed	= ARCUnit::ConvertVelocity(m_phase.GetDownSpeed(),ARCUnit::KNOT,ARCUnit::CMpS);
}
double FlightPlanGenericPhaseInSim::GetDistanceToRunway() const
{
	return m_dDistToRunway;
}

double FlightPlanGenericPhaseInSim::GetUpAltitude() const
{
	return m_dUpAltitude;
}

double FlightPlanGenericPhaseInSim::GetDownAltitude() const
{
	return m_dDownAltitude;
}

double FlightPlanGenericPhaseInSim::GetAltitudeSeparate() const
{
	return m_dAltitudeSep;
}

double FlightPlanGenericPhaseInSim::GetUpSpeed() const
{
	return m_dUpSpeed;
}

double FlightPlanGenericPhaseInSim::GetDownSpeed() const
{
	return m_dDownSpeed;
}

ns_FlightPlan::FlightPlanGenericPhase::Phase FlightPlanGenericPhaseInSim::GetPhase() const
{
	return m_phase.GetPhase();
}

void FlightPlanGenericPhaseInSim::SetDistanceToRunway( double dDist )
{
	m_dDistToRunway = dDist;
}

void FlightPlanGenericPhaseInSim::SetUpAltitude( double dDist )
{
	m_dUpAltitude = dDist;
}

void FlightPlanGenericPhaseInSim::SetDownAltitude( double dDist )
{
	m_dDownAltitude = dDist;
}

void FlightPlanGenericPhaseInSim::SetAltitudeSeparate( double dDist )
{
	m_dAltitudeSep = dDist;
}

void FlightPlanGenericPhaseInSim::SetUpSpeed( double dDist )
{
	m_dUpAltitude =dDist;
}

void FlightPlanGenericPhaseInSim::SetDownSpeed( double dDist )
{
	m_dDownSpeed =dDist;
}