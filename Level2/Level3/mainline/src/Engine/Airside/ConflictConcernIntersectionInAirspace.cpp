#include "StdAfx.h"
#include ".\Conflictconcernintersectioninairspace.h"
#include "AirRoutePointInSim.h"
#include "../../Common/ARCUnit.h"
#include "AirsideFlightInSim.h"
#include "Clearance.h"
#include "AirsideResourceManager.h"
#include "AirTrafficController.h"
#include "FlightPerformanceManager.h"
#include "InTrailSeparationInSim.h"
#include ".\AirHoldInInSim.h"
#include "AirspaceHoldInSim.h"
#include "../../Results/AirsideFlightEventLog.h"
#include "../../Results/AirsideFllightRunwayDelayLog.h"
#include "SimulationErrorShow.h"

AirRouteIntersectionInSim::AirRouteIntersectionInSim(AirRoutePointInSim* pIntersection)
{
	m_pConcernIntersection = pIntersection;
	m_pEntrySystemHold = NULL;
}

AirRouteIntersectionInSim::~AirRouteIntersectionInSim(void)
{
	delete m_pEntrySystemHold;
}

bool AirRouteIntersectionInSim::CalculateClearanceAtIntersection(AirsideFlightInSim* pFlight,AirsideResource* pLastSegment,AirsideResource* pNextSegment,FlightRouteInSim* pCurrentRoute, ClearanceItem& lastItem,ClearanceItem& newItem)
{
	//if (pFlight->GetPosition() == m_pConcernIntersection->GetPosition() && pFlight->GetResource())
	//	return true;

	ElapsedTime tLastTime = lastItem.GetTime();
	double vSpeed = lastItem.GetSpeed();

	ElapsedTime tNextTime = newItem.GetTime();
	double vNextSpd = newItem.GetSpeed();

	OccupancyTable OcyTable = m_pConcernIntersection->GetOccupancyTable();
	OccupancyInstance _instance = m_pConcernIntersection->GetOccupyInstance(pFlight);
	if (_instance.GetFlight() == pFlight|| OcyTable.empty())
	{
		if (OcyTable.empty()||(tNextTime >= _instance.GetEnterTime() && tNextTime <= _instance.GetExitTime()))	//the apply time is already get or no flight passed
		{
			m_pConcernIntersection->SetEnterTime(pFlight,tNextTime,newItem.GetMode(),vNextSpd);
			m_pConcernIntersection->SetExitTime(pFlight,tNextTime);
			lastItem = newItem;
			return true;
		}
	}


	AirsideFlightInSim* pLeadFlight = NULL;
	ElapsedTime tEnter = tNextTime;
	ElapsedTime tLeadTime = 0L;
	double dLeadSpd = 0.0;
	if (pLastSegment && (pLastSegment->GetType() == AirsideResource::ResType_AirRouteSegment 
		|| pLastSegment->GetType() == AirsideResource::ResType_HeadingAirRouteSegment))
		pLeadFlight = pLastSegment->GetFlightAhead(pFlight);

	if (pLeadFlight)
	{
		OccupancyInstance LeadInstance = m_pConcernIntersection->GetOccupyInstance(pLeadFlight);
		tLeadTime = LeadInstance.GetEnterTime();
		//dLeadSpd = LeadInstance.GetSpeed();
		AirspaceIntrailSeparationSpeed SepSpeed(pNextSegment,pCurrentRoute);
		tNextTime = m_pConcernIntersection->RecalculateEnterTime(pFlight,pLeadFlight,tLeadTime,tNextTime,SepSpeed);
	}

	pLeadFlight = NULL;
	if ( pLastSegment != pNextSegment&& 
		(pNextSegment->GetType() == AirsideResource::ResType_AirRouteSegment || pNextSegment->GetType() == AirsideResource::ResType_HeadingAirRouteSegment))	
		pLeadFlight = pNextSegment->GetFlightAhead(pFlight);

	if (pLeadFlight)
	{
		OccupancyInstance LeadInstance = m_pConcernIntersection->GetOccupyInstance(pLeadFlight);
		tLeadTime = LeadInstance.GetEnterTime();
		//dLeadSpd = LeadInstance.GetSpeed();
		AirspaceIntrailSeparationSpeed SepSpeed(pNextSegment,pCurrentRoute);
		tNextTime = m_pConcernIntersection->RecalculateEnterTime(pFlight,pLeadFlight,tLeadTime,tNextTime,SepSpeed);
	}

	newItem.SetTime(tNextTime);

	if (tNextTime > tEnter && pLeadFlight && pFlight->IsDepartingOperation() && pFlight->GetMode() <= OnPreTakeoff)		//runway delay
	{
		ElapsedTime tDelay = tNextTime - tEnter;
		ResourceDesc resDesc;
		pFlight->GetAndAssignTakeoffRunway()->getDesc(resDesc);
		AirsideFlightRunwayDelayLog* pLog = new AirsideFlightRunwayDelayLog(resDesc, tEnter.asSeconds(), OnTakeoff, tNextTime.asSeconds(), tDelay.asSeconds(), FlightRunwayDelay::UNKNOWNPOS);
		CString strDetail;
		strDetail.Format("Separated with AC %s at waypoint %s",pLeadFlight->GetCurrentFlightID(),m_pConcernIntersection->PrintResource());
		pLog->sReasonDetail = strDetail.GetString();
		pFlight->LogEventItem(pLog);		
	}


	ElapsedTime tMaxSegTime = 0L;		
	ElapsedTime tMinSegTime = 0L;		
	DistanceUnit dSegDist = 0.0;

	if (pLastSegment && pLastSegment->GetType() == AirsideResource::ResType_LogicRunway)
	{
		LogicRunwayInSim *pLogicRunway = (LogicRunwayInSim *)pLastSegment;
		dSegDist = m_pConcernIntersection->GetPosition().distance(pLogicRunway->GetOtherPort()->GetDistancePoint(0));
		tMaxSegTime = ElapsedTime(dSegDist/pFlight->GetMinSpeed(OnClimbout));
		tMinSegTime = ElapsedTime(dSegDist/pFlight->GetMaxSpeed(OnClimbout));
	}
	else if (pLastSegment && pLastSegment->GetType() == AirsideResource::ResType_AirRouteSegment)
	{
		tMaxSegTime = pCurrentRoute->GetFlightMaxTimeInSeg(pFlight,(AirRouteSegInSim*)pLastSegment);
		tMinSegTime = pCurrentRoute->GetFlightMinTimeInSeg(pFlight,(AirRouteSegInSim*)pLastSegment);
		dSegDist = ((AirRouteSegInSim*)pLastSegment)->GetSegmentDistance();
	}

	if (pLastSegment && ((tNextTime - tLastTime) > tMaxSegTime || (tNextTime - tLastTime) < tMinSegTime))
		return false;

	ElapsedTime tAvailStartTime = 0L;
	ElapsedTime tAvailEndTime = 0L;
	GetAvailableTimeAtIntersection(pFlight,pNextSegment,pCurrentRoute,tNextTime,tAvailStartTime,tAvailEndTime);

	if(tNextTime < tAvailStartTime)
	{
		tNextTime = tAvailStartTime;
		newItem.SetTime(tNextTime);
	}

	if ((tNextTime - tLastTime) > tMaxSegTime || (tNextTime - tLastTime) < tMinSegTime)
		return false;

	if (pLastSegment && pLastSegment->GetType() == AirsideResource::ResType_AirRouteSegment&& dSegDist >0)
	{
		ElapsedTime tTime = tNextTime - tLastTime;
		vNextSpd = 2.0*dSegDist/((1.0 * tTime.getPrecisely())/TimePrecision) - vSpeed;
	}

	m_pConcernIntersection->SetEnterTime(pFlight,tNextTime,newItem.GetMode(),vNextSpd);
	m_pConcernIntersection->SetExitTime(pFlight,tNextTime);
	lastItem = newItem;
	return true;
}

bool AirRouteIntersectionInSim::ConverseCalculateClearanceAtIntersection(AirsideFlightInSim* pFlight,AirsideResource* pLastSegment,AirsideResource* pNextSeg, 
																		 FlightRouteInSim* pCurrentRoute,ClearanceItem& nextItem,ClearanceItem& newItem,ElapsedTime& eMinFltPlanTime)
{
	ElapsedTime tNextItem = nextItem.GetTime();
	double vSpeed = nextItem.GetSpeed();

	ElapsedTime tNextTime = newItem.GetTime();
	double vNextSpd = newItem.GetSpeed();

	OccupancyTable OcyTable = m_pConcernIntersection->GetOccupancyTable();
	if ( OcyTable.empty())
	{
		m_pConcernIntersection->SetEnterTime(pFlight,tNextTime,newItem.GetMode(),vNextSpd);
		m_pConcernIntersection->SetExitTime(pFlight,tNextTime);
		return true;
	
	}

	ElapsedTime tMaxSegTime = 0L;		
	ElapsedTime tMinSegTime = 0L;		
	DistanceUnit dSegDist = 0.0;

	//int nIdx = pCurrentRoute->GetSegmentIndex(pNextSeg);
	//int nSegCount = pCurrentRoute->GetItemCount();

	if (pLastSegment->GetType() == AirsideResource::ResType_LogicRunway)	//nSegCount is resource number of flight route, include route segment and runway, and star's last resource is runway,
	{							
		//dSegDist: the distance from last waypoint to runway touchdown point
		//m_pConcernIntersection: is Waypoint or AirRoute intersection
		//GetDistancePoint(): to get the point position in the resource
		//GetPosition(): to get the intersection's point coordinate
		DistanceUnit touchDist = pFlight->GetTouchDownDistAtRuwnay(pFlight->GetLandingRunway());//pFlight->GetPerformance()->getDistThresToTouch(pFlight);
		CPoint2008 runwayPortPoint = pLastSegment->GetDistancePoint(touchDist);
		CPoint2008 MissApproachPoint = ((LogicRunwayInSim*)pLastSegment)->GetMissApproachCheckPoint(m_pConcernIntersection->GetPosition());
		dSegDist = m_pConcernIntersection->GetPosition().distance(MissApproachPoint) + MissApproachPoint.distance(runwayPortPoint);
		
		double dMinSpeed = (pFlight->GetMinSpeed(OnApproach)+ pFlight->GetPerformance()->getLandingSpeed(pFlight))/2;
		tMaxSegTime = ElapsedTime(dSegDist/dMinSpeed);

		double dMaxSpeed = (pFlight->GetMaxSpeed(OnApproach)+ pFlight->GetPerformance()->getLandingSpeed(pFlight))/2;
		tMinSegTime = ElapsedTime(dSegDist/dMaxSpeed);
	}
	else
	{
		if (pNextSeg->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			tMaxSegTime = pCurrentRoute->GetFlightMaxTimeInSeg(pFlight,(AirRouteSegInSim*)pNextSeg);
			tMinSegTime = pCurrentRoute->GetFlightMinTimeInSeg(pFlight,(AirRouteSegInSim*)pNextSeg);
			dSegDist = ((AirRouteSegInSim*)pNextSeg)->GetSegmentDistance();
		}

		if (pNextSeg->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)	//with same waypoint as next segment enter point or last segment exit point, has already solved conflict
			return true;

	}

	if(tMaxSegTime < eMinFltPlanTime || tMinSegTime >eMinFltPlanTime)
	{
		CString strExption = "The flight performance and the flight plan has conflict, please check the flight performance and flight plan settings.";
		CString strErrorType = "AIRCRAFT TERMINATE";
		throw FlightPlanConflictFlightPerformanceException(strExption, strErrorType);

		return false;
	}


	AirsideFlightInSim* pLeadFlight = NULL;

	//if (pNextSeg->GetType() == AirsideResource::ResType_AirRouteSegment ||pNextSeg->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
	OccupancyInstance leadInstance = m_pConcernIntersection->GetFrontOccupancyInstance(pFlight,tNextTime);
	pLeadFlight = leadInstance.GetFlight();

	ElapsedTime tLeadTime = 0L;
	double dLeadSpd = 0.0;


	if (pLeadFlight && pFlight != pLeadFlight)
	{	
		OccupancyInstance LeadInstance = m_pConcernIntersection->GetOccupyInstance(pLeadFlight);
		tLeadTime = LeadInstance.GetEnterTime();
	/*	dLeadSpd = LeadInstance.GetSpeed();*/
		AirspaceIntrailSeparationSpeed SepSpeed(pNextSeg,pCurrentRoute);
		tNextTime = m_pConcernIntersection->RecalculateEnterTime(pFlight,pLeadFlight,tLeadTime,tNextTime,SepSpeed);
		newItem.SetTime(tNextTime);
	}

	if ((tNextItem - tNextTime) > tMaxSegTime || (tNextItem - tNextTime) < tMinSegTime)
		return false;

	ElapsedTime tAvailStartTime = 0L;
	ElapsedTime tAvailEndTime = 0L;
	GetAvailableTimeAtIntersection(pFlight,pNextSeg,pCurrentRoute,tNextTime,tAvailStartTime,tAvailEndTime);

	tNextTime = tAvailStartTime;
	newItem.SetTime(tNextTime);

	if ((tNextItem - tNextTime) > tMaxSegTime || (tNextItem - tNextTime) < tMinSegTime)
		return false;

	if (pNextSeg && (pNextSeg->GetType() == AirsideResource::ResType_AirRouteSegment|| pNextSeg->GetType() == AirsideResource::ResType_LogicRunway))
	{
		ElapsedTime tTime = tNextItem - tNextTime;
		if(tTime.getPrecisely() != 0)
			vNextSpd = 2.0*dSegDist/((1.0 * tTime.getPrecisely())/TimePrecision) - vSpeed;
		else
			vNextSpd = 0.0;
	}

	m_pConcernIntersection->SetEnterTime(pFlight,tNextTime,newItem.GetMode(),vNextSpd);
	m_pConcernIntersection->SetExitTime(pFlight,tNextTime);
	return true;
}

void AirRouteIntersectionInSim::GetAvailableTimeAtIntersection(AirsideFlightInSim* pFlight,AirsideResource* pNextSeg,FlightRouteInSim* pCurrentRoute,ElapsedTime tEnter,ElapsedTime& tAvailStartTime,ElapsedTime& tAvailEndTime)
{
	OccupancyInstance _instance = m_pConcernIntersection->GetOccupyInstance(pFlight);
	if (_instance.GetFlight() == pFlight)
	{
		if (tEnter >= _instance.GetEnterTime() && tEnter <= _instance.GetExitTime())	//the apply time is already get
		{
			tAvailStartTime = tEnter;
			tAvailEndTime = _instance.GetExitTime();
			return;
		}
	}
	OccupancyTable OcyTable = m_pConcernIntersection->GetOccupancyTable();
	double dAlt = (m_pConcernIntersection->GetAltHigh() + m_pConcernIntersection->GetAltLow())/2.0;
	CInTrailSeparationInSim* pIntrailSeparation = pFlight->GetAirTrafficController()->GetIntrailSeparation();

	if (OcyTable.empty())			//no flight passed
	{
		tAvailStartTime = tEnter;
		tAvailEndTime = -1L;
		return;
	}

	AirsideFlightInSim* pLeadFlight = NULL;

	if ( !m_pConcernIntersection->IsIntersection() || GetConnectHold())
	{
		pLeadFlight = m_pConcernIntersection->GetLastOccupyInstance().GetFlight();
		if(pLeadFlight == pFlight)
		{
			tAvailStartTime = tEnter;
			tAvailEndTime = -1L;
			return;
		}

		/*double dSpd = m_pConcernIntersection->GetLastOccupyInstance().GetSpeed();*/
		ElapsedTime tLeadTime = m_pConcernIntersection->GetLastOccupyInstance().GetExitTime();
		double dMinDist = 0.0;
		ElapsedTime tMinTime = 0L;
		AirspaceIntrailSeparationSpeed SepSpeed(pNextSeg,pCurrentRoute);
		pIntrailSeparation->GetInTrailSeparationDistanceAndTime(pFlight,pLeadFlight,dAlt,dMinDist,tMinTime);
		double dSpd = SepSpeed.GetSparationSpeed(pFlight,dMinDist);
		if (dSpd > (std::numeric_limits<double>::min)())
		{
			tMinTime = tMinTime >= ElapsedTime(dMinDist/dSpd) ? tMinTime :ElapsedTime(dMinDist/dSpd);
		}

		/*tMinTime = tMinTime >= ElapsedTime(dMinDist/dSpd) ? tMinTime :ElapsedTime(dMinDist/dSpd);*/

		if (tLeadTime + tMinTime > tEnter)
			tAvailStartTime = tLeadTime + tMinTime;
		else
			tAvailStartTime = tEnter;

		if (tAvailStartTime > tEnter && pFlight->IsDepartingOperation() && pFlight->GetMode() <= OnPreTakeoff && pLeadFlight)		//runway delay
		{
			ElapsedTime tDelay = tAvailStartTime - tEnter;
			ResourceDesc resDesc;
			pFlight->GetAndAssignTakeoffRunway()->getDesc(resDesc);
			AirsideFlightRunwayDelayLog* pLog = new AirsideFlightRunwayDelayLog(resDesc, tEnter.asSeconds(), OnTakeoff, tAvailStartTime.asSeconds(), tDelay.asSeconds(), FlightRunwayDelay::UNKNOWNPOS);
			CString strDetail;
			strDetail.Format("Separated with AC %s at waypoint %s",pLeadFlight->GetCurrentFlightID(),m_pConcernIntersection->PrintResource());
			pLog->sReasonDetail = strDetail.GetString();
			pFlight->LogEventItem(pLog);		
		}


		tAvailEndTime = -1L;
		return;
	}

	ElapsedTime tAvailEnterTime = tEnter;

	if (pNextSeg->GetType() == AirsideResource::ResType_AirRouteSegment || pNextSeg->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		pLeadFlight = pNextSeg->GetLastOccupyInstance().GetFlight();

	if (pLeadFlight && pLeadFlight != pFlight)
	{
		double dMinDist = 0.0;
		ElapsedTime tMinTime = 0L;
		AirspaceIntrailSeparationSpeed SepSpeed(pNextSeg,pCurrentRoute);
		//double dLeadSpd = /*pNextSeg->GetLastOccupyInstance().GetSpeed()*/
		pIntrailSeparation->GetInTrailSeparationDistanceAndTime(pFlight,pLeadFlight,dAlt,dMinDist,tMinTime);
		double dLeadSpd = SepSpeed.GetSparationSpeed(pLeadFlight,dMinDist);
		ElapsedTime tLeadSepTime = tMinTime >= ElapsedTime(dMinDist/dLeadSpd) ? tMinTime :ElapsedTime(dMinDist/dLeadSpd); 

		OccupancyInstance leadIns = m_pConcernIntersection->GetOccupyInstance(pLeadFlight);

		ASSERT(leadIns.GetFlight()!= NULL);		//must have the lead flight's register in the time table

		if (tAvailEnterTime < leadIns.GetEnterTime()+ tLeadSepTime)
			tAvailEnterTime = leadIns.GetEnterTime()+ tLeadSepTime;

	}

	ElapsedTime dConvergingSepTime = 0L;
	ElapsedTime dDivergingSepTime = 0L;
	AirsideFlightInSim* pSpatialFlight = NULL;
	double dSpatialSpd = 0.0;
	AirspaceIntrailSeparationSpeed SepSpeed(pNextSeg,pCurrentRoute);
	int nSize = int(OcyTable.size());

	DistanceUnit dSpatialConvergentDist = 0.0;
	DistanceUnit dSpatialDivergentDist = 0.0;

	for( int i = 0; i < nSize ;i++)
	{
		if (OcyTable[i].GetEnterTime() <= tAvailEnterTime)
			continue;

		if (i > 0)
		{
			pSpatialFlight = OcyTable[i-1].GetFlight();
			/*dSpatialSpd = OcyTable[i-1].GetSpeed();*/
			{
				double dMinDist = 0.0;
				ElapsedTime tMinTime = 0L;
				if (pSpatialFlight != pLeadFlight)
				{
					pIntrailSeparation->GetInTrailSeparationDistanceAndTime(pFlight,pSpatialFlight,dAlt,dMinDist,tMinTime);
					dSpatialSpd = SepSpeed.GetSparationSpeed(pFlight,dMinDist);
					if (dSpatialSpd > (std::numeric_limits<double>::min)())
					{
						dDivergingSepTime = tMinTime >= ElapsedTime(dMinDist/dSpatialSpd) ? tMinTime :ElapsedTime(dMinDist/dSpatialSpd); 
					}
					/*dDivergingSepTime = tMinTime >= ElapsedTime(dMinDist/dSpatialSpd) ? tMinTime :ElapsedTime(dMinDist/dSpatialSpd); */
				}
			}

			pSpatialFlight = OcyTable[i].GetFlight();
			/*dSpatialSpd = OcyTable[i].GetSpeed();*/

			{
				double dMinDist = 0.0;
				ElapsedTime tMinTime = 0L;
				if (pSpatialFlight != pLeadFlight)
				{
					pIntrailSeparation->GetInTrailSeparationDistanceAndTime (pSpatialFlight,pFlight,dAlt,dMinDist,tMinTime);
					dSpatialSpd = SepSpeed.GetSparationSpeed(pFlight,dMinDist);
					if (dSpatialSpd > (std::numeric_limits<double>::min)())
					{
						dConvergingSepTime = tMinTime >= ElapsedTime(dMinDist/dSpatialSpd) ? tMinTime :ElapsedTime(dMinDist/dSpatialSpd); 
					}
					/*dConvergingSepTime = tMinTime >= ElapsedTime(dMinDist/dSpatialSpd) ? tMinTime :ElapsedTime(dMinDist/dSpatialSpd); */
				}
			}

			// check if the flight can insert front of the spatial flight
			if (OcyTable[i].GetEnterTime() - OcyTable[i-1].GetExitTime() > dConvergingSepTime + dDivergingSepTime && OcyTable[i].GetEnterTime() - dConvergingSepTime >= tAvailEnterTime)
			{
				tAvailStartTime = OcyTable[i-1].GetExitTime() + dDivergingSepTime;
				tAvailEndTime = OcyTable[i].GetEnterTime() - dConvergingSepTime;

				if (OcyTable[i].GetFlight() == pFlight && tAvailStartTime < OcyTable[i].GetEnterTime())
				{
					tAvailStartTime =  OcyTable[i].GetEnterTime();
				}

				if (tAvailStartTime < tEnter && tAvailEndTime >= tEnter)
					tAvailStartTime = tEnter;
				return;
			}
		}
		else	//only one flight in occupancy table, check whether the flight can enter the air route point before the registered flight
		{
			dSpatialConvergentDist = 0.0;

			pSpatialFlight = OcyTable[i].GetFlight();
			dSpatialSpd = OcyTable[i].GetSpeed();
			if (pSpatialFlight != pFlight && pSpatialFlight != pLeadFlight && tAvailEnterTime <= OcyTable[i].GetEnterTime())
				pIntrailSeparation->GetInTrailSeparationSpatialConvergingAndDiverging(pSpatialFlight,pFlight,dAlt,dSpatialConvergentDist,dSpatialDivergentDist);

			dConvergingSepTime = ElapsedTime(dSpatialConvergentDist/dSpatialSpd);

			//if (pLeadFlight == pSpatialFlight)
			//{
			//	double dMinDist = 0.0;
			//	ElapsedTime tMinTime = 0L;
			//	pIntrailSeparation->GetInTrailSeparationDistanceAndTime(pFlight,pLeadFlight,dAlt,dMinDist,tMinTime);
			//	dConvergingSepTime = tMinTime >= ElapsedTime(dMinDist/dSpatialSpd) ? tMinTime :ElapsedTime(dMinDist/dSpatialSpd); 
			//}

			if ( tAvailEnterTime <= OcyTable[i].GetEnterTime() - dConvergingSepTime)		//spatial flight leave waypoint before the flight
			{
				tAvailStartTime = tAvailEnterTime;
				tAvailEndTime = OcyTable[i].GetEnterTime() - dConvergingSepTime;
				return;
			}
		}
	}

	//register time at last
	dSpatialDivergentDist = 0.0;

	pSpatialFlight = m_pConcernIntersection->GetLastOccupyInstance().GetFlight();
	if (pSpatialFlight != pFlight&& pSpatialFlight != pLeadFlight)
		pIntrailSeparation->GetInTrailSeparationSpatialConvergingAndDiverging(pFlight,pSpatialFlight,dAlt,dSpatialConvergentDist,dSpatialDivergentDist);

	double dSpeed = m_pConcernIntersection->GetLastOccupyInstance().GetSpeed();
	ASSERT(dSpeed>0);

	dDivergingSepTime = ElapsedTime(dSpatialConvergentDist/dSpeed);
	tAvailStartTime = m_pConcernIntersection->GetLastOccupyInstance().GetExitTime() + dDivergingSepTime;

	if (tAvailStartTime < tAvailEnterTime)
		tAvailStartTime = tAvailEnterTime;

	if (tAvailStartTime > tEnter && pFlight->IsDepartingOperation() && pFlight->GetMode() <= OnPreTakeoff && pLeadFlight)		//runway delay
	{
		ElapsedTime tDelay = tAvailStartTime - tEnter;
		ResourceDesc resDesc;
		pFlight->GetAndAssignTakeoffRunway()->getDesc(resDesc);
		AirsideFlightRunwayDelayLog* pLog = new AirsideFlightRunwayDelayLog(resDesc, tEnter.asSeconds(), OnTakeoff, tAvailStartTime.asSeconds(), tDelay.asSeconds(), FlightRunwayDelay::UNKNOWNPOS);
		CString strDetail;
		strDetail.Format("Separated with AC %s at waypoint %s",pLeadFlight->GetCurrentFlightID(),m_pConcernIntersection->PrintResource());
		pLog->sReasonDetail = strDetail.GetString();
		pFlight->LogEventItem(pLog);		
	}

	tAvailEndTime = -1L;

}

bool AirRouteIntersectionInSim::ConverseCalculateClearanceWithConvergentSeg(AirsideFlightInSim* pFlight,ElapsedTime tNextItem,ElapsedTime tMaxSegTime, ElapsedTime tMinSegTime, ClearanceItem& newItem)
{
	ElapsedTime tNextTime = newItem.GetTime();
	double vNextSpd = newItem.GetSpeed();

	ElapsedTime tSpatialFlightTime = 0L;
	double dSpatialFlightSpd = 0.0;
	AirsideFlightInSim* pSpatialFlight = NULL;
	DistanceUnit dSpatialConvergentDist = 0.0;
	DistanceUnit dSpatialDivergentDist = 0.0;
	DistanceUnit RadiusOfConcern = 0.0;

	CInTrailSeparationInSim* pIntrailSeparation = pFlight->GetAirTrafficController()->GetIntrailSeparation();
	pIntrailSeparation->GetInTrailSeparationRadiusOfConcern(RadiusOfConcern);

	OccupancyTable OcyTable = m_pConcernIntersection->GetOccupancyTable();

	int nCount = (int)m_vConvergentRouteSegs.size();

	if (nCount ==0)
		return true;

	for (int i = 0; i < nCount; i++)
	{
		pSpatialFlight = (m_vConvergentRouteSegs.at(i))->GetForemostFlightInSeg(tNextTime);
		if (pSpatialFlight == NULL || pSpatialFlight == pFlight)
			continue;

		int nSize = (int)OcyTable.size();
		for (int j =0; j < nSize; j++)
		{
			if ( OcyTable[j].GetFlight() == pSpatialFlight)
			{
				tSpatialFlightTime = OcyTable[j].GetEnterTime();
				dSpatialFlightSpd = OcyTable[j].GetSpeed();
				break;
			}
			continue;
		}

		if (tSpatialFlightTime <= tNextTime)		// the spatial flight arrived at intersection earlier than the flight
		{
			DistanceUnit dDist = dSpatialFlightSpd * (double)(tNextTime - tSpatialFlightTime);
			if (dDist > RadiusOfConcern)		//the distance between two flight exceed the radius of concern 
				continue;

			pIntrailSeparation->GetInTrailSeparationSpatialConvergingAndDiverging(pFlight,pSpatialFlight,newItem.GetAltitude(),dSpatialConvergentDist,dSpatialDivergentDist);

			if (dDist >= dSpatialConvergentDist)
				continue;

			ElapsedTime tDelay = ElapsedTime((dSpatialConvergentDist - dDist)/dSpatialFlightSpd);
			ElapsedTime tDuration =  tNextItem - (tNextTime + tDelay);

			if ( tDuration <= tMaxSegTime && tDuration > tMinSegTime)	// the flight can arrived at intersection after the spatial flight
			{
				tNextTime += tDelay;
				continue;
			}

			return false;
		}
		else								//the spatial flight arrived at intersection latter than the flight
		{
			DistanceUnit dDist = vNextSpd* (double)(tSpatialFlightTime - tNextTime);
			if (dDist > RadiusOfConcern)
				continue;

			pIntrailSeparation->GetInTrailSeparationSpatialConvergingAndDiverging(pSpatialFlight,pFlight,newItem.GetAltitude(),dSpatialConvergentDist,dSpatialDivergentDist );

			if (dDist >= dSpatialConvergentDist)
				continue;

			ElapsedTime tSepTime = ElapsedTime((dSpatialConvergentDist - dDist)/dSpatialFlightSpd);
			ElapsedTime tDuration =  tNextItem - (tNextTime - tSepTime);
			if (tDuration > tMinSegTime && tDuration <= tMaxSegTime)		// the flight can arrived at intersection before the spatial flight
			{
				tNextTime -= tSepTime;
				continue;
			}

			if (tSpatialFlightTime + tSepTime >= tNextItem - tMaxSegTime && tSpatialFlightTime + tSepTime < tNextItem - tMinSegTime)	// the flight can arrived at intersection after the spatial flight
			{
				tNextTime = tSpatialFlightTime + tSepTime;
				continue;
			}

			return false;

		}
	}
	newItem.SetTime(tNextTime);
	return true;
	
}

bool AirRouteIntersectionInSim::ConverseCalculateClearanceWithDivergentSeg(AirsideFlightInSim* pFlight,ElapsedTime tNextItem,ElapsedTime tMaxSegTime,ElapsedTime tMinSegTime, ClearanceItem& newItem)
{
	ElapsedTime tNextTime = newItem.GetTime();
	double vNextSpd = newItem.GetSpeed();

	ElapsedTime tSpatialFlightTime = 0L;
	double dSpatialFlightSpd = 0.0;
	AirsideFlightInSim* pSpatialFlight = NULL;
	DistanceUnit dSpatialConvergentDist = 0.0;
	DistanceUnit dSpatialDivergentDist = 0.0;
	DistanceUnit RadiusOfConcern = 0.0;

	CInTrailSeparationInSim* pIntrailSeparation = pFlight->GetAirTrafficController()->GetIntrailSeparation();
	pIntrailSeparation->GetInTrailSeparationRadiusOfConcern(RadiusOfConcern);

	OccupancyTable OcyTable = m_pConcernIntersection->GetOccupancyTable();

	int nCount = (int)m_vDivergentRouteSegs.size();

	if (nCount ==0)
		return true;

	for (int i = 0; i < nCount; i++)
	{
		pSpatialFlight = (m_vDivergentRouteSegs.at(i))->GetBackmostFlightInSeg(tNextTime);

		if (pSpatialFlight == NULL)
			continue;

		int nSize = (int)OcyTable.size();
		for (int j =0; j < nSize; j++)
		{
			if ( OcyTable[j].GetFlight() == pSpatialFlight)
			{
				tSpatialFlightTime = OcyTable[j].GetExitTime();
				dSpatialFlightSpd = OcyTable[j].GetSpeed();
				break;
			}
			continue;
		}

		// the spatial flight leave intersection earlier than the flight
		DistanceUnit dDist = dSpatialFlightSpd * (double)(tNextTime - tSpatialFlightTime);

		if (dDist > RadiusOfConcern)		//the distance between two flight exceed the radius of concern 
			continue;

		pIntrailSeparation->GetInTrailSeparationSpatialConvergingAndDiverging(pFlight,pSpatialFlight,newItem.GetAltitude(),dSpatialConvergentDist,dSpatialDivergentDist);

		if (dDist >= dSpatialDivergentDist)
			continue;

		ElapsedTime tDelay = (dSpatialDivergentDist - dDist)/dSpatialFlightSpd;
		ElapsedTime tDuration =  tNextItem - (tNextTime + tDelay);

		if (tDuration <= tMaxSegTime && tDuration > tMinSegTime)	// the flight can arrived at intersection after the spatial flight
		{
			tNextTime += tDelay;
			continue;
		}

		return false;

	}
	newItem.SetTime(tNextTime);
	return true;
}

bool AirRouteIntersectionInSim::CalculateClearanceWithConvergentSeg(AirsideFlightInSim* pFlight,ElapsedTime tLastItem,ElapsedTime tMaxSegTime, ElapsedTime tMinSegTime, ClearanceItem& newItem)
{
	ElapsedTime tNextTime = newItem.GetTime();
	double vNextSpd = newItem.GetSpeed();

	ElapsedTime tSpatialFlightTime = 0L;
	double dSpatialFlightSpd = 0.0;
	AirsideFlightInSim* pSpatialFlight = NULL;
	DistanceUnit dSpatialConvergentDist = 0.0;
	DistanceUnit dSpatialDivergentDist = 0.0;
	DistanceUnit RadiusOfConcern = 0.0;

	CInTrailSeparationInSim* pIntrailSeparation = pFlight->GetAirTrafficController()->GetIntrailSeparation();
	pIntrailSeparation->GetInTrailSeparationRadiusOfConcern(RadiusOfConcern);

	OccupancyTable OcyTable = m_pConcernIntersection->GetOccupancyTable();

	int nCount = (int)m_vConvergentRouteSegs.size();

	if (nCount ==0)
		return true;

	for (int i = 0; i < nCount; i++)
	{
		pSpatialFlight = (m_vConvergentRouteSegs.at(i))->GetForemostFlightInSeg(tNextTime);
		if (pSpatialFlight == NULL || pSpatialFlight == pFlight)
			continue;

		int nSize = (int)OcyTable.size();
		for (int j =0; j < nSize; j++)
		{
			if ( OcyTable[j].GetFlight() == pSpatialFlight)
			{
				tSpatialFlightTime = OcyTable[j].GetEnterTime();
				dSpatialFlightSpd = OcyTable[j].GetSpeed();
				break;
			}
			continue;
		}

		if (tSpatialFlightTime <= tNextTime)		// the spatial flight arrived at intersection earlier than the flight
		{
			DistanceUnit dDist = dSpatialFlightSpd * (double)(tNextTime - tSpatialFlightTime);
			if (dDist > RadiusOfConcern)		//the distance between two flight exceed the radius of concern 
				continue;

			pIntrailSeparation->GetInTrailSeparationSpatialConvergingAndDiverging(pFlight,pSpatialFlight,newItem.GetAltitude(),dSpatialConvergentDist,dSpatialDivergentDist);

			if (dDist >= dSpatialConvergentDist)
				continue;

			ElapsedTime tDelay = ElapsedTime((dSpatialConvergentDist - dDist)/dSpatialFlightSpd);
			ElapsedTime tDuration =  tNextTime - tLastItem + tDelay;

			if ( tDuration <= tMaxSegTime && tDuration > tMinSegTime)	// the flight can arrived at intersection after the spatial flight
			{
				tNextTime += tDelay;
				continue;
			}

			return false;
		}
		else								//the spatial flight arrived at intersection latter than the flight
		{
			DistanceUnit dDist = vNextSpd* (double)(tSpatialFlightTime - tNextTime);
			if (dDist > RadiusOfConcern)
				continue;

			pIntrailSeparation->GetInTrailSeparationSpatialConvergingAndDiverging(pSpatialFlight,pFlight,newItem.GetAltitude(),dSpatialConvergentDist,dSpatialDivergentDist );

			if (dDist >= dSpatialConvergentDist)
				continue;

			ElapsedTime tSepTime = ElapsedTime((dSpatialConvergentDist - dDist)/dSpatialFlightSpd);
			ElapsedTime tDuration = tNextTime -  tLastItem - tSepTime;
			if (tDuration > tMinSegTime && tDuration <= tMaxSegTime)		// the flight can arrived at intersection before the spatial flight
			{
				tNextTime -= tSepTime;
				continue;
			}

			if (tSpatialFlightTime + tSepTime > tLastItem + tMinSegTime && tSpatialFlightTime + tSepTime <= tLastItem + tMaxSegTime)	// the flight can arrived at intersection after the spatial flight
			{
				tNextTime = tSpatialFlightTime + tSepTime;
				continue;
			}

			return false;

		}
	}
	newItem.SetTime(tNextTime);
	return true;

}

bool AirRouteIntersectionInSim::CalculateClearanceWithDivergentSeg(AirsideFlightInSim* pFlight,ElapsedTime tLastItem,ElapsedTime tMaxSegTime,ElapsedTime tMinSegTime, ClearanceItem& newItem)
{
	ElapsedTime tNextTime = newItem.GetTime();
	double vNextSpd = newItem.GetSpeed();

	ElapsedTime tSpatialFlightTime = 0L;
	double dSpatialFlightSpd = 0.0;
	AirsideFlightInSim* pSpatialFlight = NULL;
	DistanceUnit dSpatialConvergentDist = 0.0;
	DistanceUnit dSpatialDivergentDist = 0.0;
	DistanceUnit RadiusOfConcern = 0.0;

	CInTrailSeparationInSim* pIntrailSeparation = pFlight->GetAirTrafficController()->GetIntrailSeparation();
	pIntrailSeparation->GetInTrailSeparationRadiusOfConcern(RadiusOfConcern);

	OccupancyTable OcyTable = m_pConcernIntersection->GetOccupancyTable();

	int nCount = (int)m_vDivergentRouteSegs.size();

	if (nCount ==0)
		return true;

	for (int i = 0; i < nCount; i++)
	{
		pSpatialFlight = (m_vDivergentRouteSegs.at(i))->GetBackmostFlightInSeg(tNextTime);

		if (pSpatialFlight == NULL)
			continue;

		int nSize = (int)OcyTable.size();
		for (int j =0; j < nSize; j++)
		{
			if ( OcyTable[j].GetFlight() == pSpatialFlight)
			{
				tSpatialFlightTime = OcyTable[j].GetExitTime();
				dSpatialFlightSpd = OcyTable[j].GetSpeed();
				break;
			}
			continue;
		}

		// the spatial flight leave intersection earlier than the flight
		DistanceUnit dDist = dSpatialFlightSpd * (double)(tNextTime - tSpatialFlightTime);

		if (dDist > RadiusOfConcern)		//the distance between two flight exceed the radius of concern 
			continue;

		pIntrailSeparation->GetInTrailSeparationSpatialConvergingAndDiverging(pFlight,pSpatialFlight,newItem.GetAltitude(),dSpatialConvergentDist,dSpatialDivergentDist);

		if (dDist >= dSpatialDivergentDist)
			continue;

		ElapsedTime tDelay = (dSpatialDivergentDist - dDist)/dSpatialFlightSpd;
		ElapsedTime tDuration =   tNextTime - tLastItem + tDelay;

		if (tDuration <= tMaxSegTime && tDuration > tMinSegTime)	// the flight can arrived at intersection after the spatial flight
		{
			tNextTime += tDelay;
			continue;
		}

		return false;

	}
	newItem.SetTime(tNextTime);
	return true;
}




AirspaceHoldInSim* AirRouteIntersectionInSim::GetConnectHold()
{
	if (m_pConcernIntersection->GetType() == AirsideResource::ResType_WayPoint)
	{
		return ((AirWayPointInSim*)m_pConcernIntersection)->GetHold();
	}
	return NULL;
}

bool AirRouteIntersectionInSim::CalculateClearanceAtIntersectionCircute( AirsideFlightInSim* pFlight,AirsideResource* pLastSegment,AirsideResource* pNextSegment,FlightRouteInSim* pCurrentRoute, ClearanceItem& lastItem,ClearanceItem& newItem )
{
	//if (pFlight->GetPosition() == m_pConcernIntersection->GetPosition() && pFlight->GetResource())
	//	return true;

	ElapsedTime tLastTime = lastItem.GetTime();
	double vSpeed = lastItem.GetSpeed();

	ElapsedTime tNextTime = newItem.GetTime();
	double vNextSpd = newItem.GetSpeed();

	OccupancyTable OcyTable = m_pConcernIntersection->GetOccupancyTable();
	OccupancyInstance _instance = m_pConcernIntersection->GetOccupyInstance(pFlight);
	if (_instance.GetFlight() == pFlight|| OcyTable.empty())
	{
		if (OcyTable.empty()||(tNextTime >= _instance.GetEnterTime() && tNextTime <= _instance.GetExitTime()))	//the apply time is already get or no flight passed
		{
			m_pConcernIntersection->SetEnterTime(pFlight,tNextTime,newItem.GetMode(),vNextSpd);
			m_pConcernIntersection->SetExitTime(pFlight,tNextTime);
			lastItem = newItem;
			return true;
		}
	}


	AirsideFlightInSim* pLeadFlight = NULL;
	ElapsedTime tEnter = tNextTime;
	ElapsedTime tLeadTime = 0L;
	double dLeadSpd = 0.0;
	if (pLastSegment && (pLastSegment->GetType() == AirsideResource::ResType_AirRouteSegment 
		|| pLastSegment->GetType() == AirsideResource::ResType_HeadingAirRouteSegment))
		pLeadFlight = pLastSegment->GetFlightAhead(pFlight);

	if (pLeadFlight)
	{
		OccupancyInstance LeadInstance = m_pConcernIntersection->GetOccupyInstance(pLeadFlight);
		tLeadTime = LeadInstance.GetEnterTime();
		//dLeadSpd = LeadInstance.GetSpeed();
		AirspaceIntrailSeparationSpeed SepSpeed(pNextSegment,pCurrentRoute);
		tNextTime = m_pConcernIntersection->RecalculateEnterTime(pFlight,pLeadFlight,tLeadTime,tNextTime,SepSpeed);
	}

	pLeadFlight = NULL;
	if ( pLastSegment != pNextSegment&& pNextSegment && 
		(pNextSegment->GetType() == AirsideResource::ResType_AirRouteSegment || pNextSegment->GetType() == AirsideResource::ResType_HeadingAirRouteSegment))	
		pLeadFlight = pNextSegment->GetFlightAhead(pFlight);

	if (pLeadFlight)
	{
		OccupancyInstance LeadInstance = m_pConcernIntersection->GetOccupyInstance(pLeadFlight);
		tLeadTime = LeadInstance.GetEnterTime();
		//dLeadSpd = LeadInstance.GetSpeed();
		AirspaceIntrailSeparationSpeed SepSpeed(pNextSegment,pCurrentRoute);
		ElapsedTime tAvTime = m_pConcernIntersection->RecalculateEnterTime(pFlight,pLeadFlight,tLeadTime,tNextTime,SepSpeed);
		if(tAvTime > tNextTime){
			newItem.SetTime(tAvTime);
			return false;
		}
	}

	newItem.SetTime(tNextTime);	

	ElapsedTime tAvailStartTime = 0L;
	ElapsedTime tAvailEndTime = 0L;
	GetAvailableTimeAtIntersection(pFlight,pNextSegment,pCurrentRoute,tNextTime,tAvailStartTime,tAvailEndTime);

	if(tNextTime < tAvailStartTime)
	{
		tNextTime = tAvailStartTime;
		newItem.SetTime(tNextTime);
		return false;
	}
	//if (pLastSegment && pLastSegment->GetType() == AirsideResource::ResType_AirRouteSegment&& dSegDist >0)
	//{
	//	ElapsedTime tTime = tNextTime - tLastTime;
	//	vNextSpd = 2.0*dSegDist/((1.0 * tTime.getPrecisely())/TimePrecision) - vSpeed;
	//}

	m_pConcernIntersection->SetEnterTime(pFlight,tNextTime,newItem.GetMode(),vNextSpd);
	m_pConcernIntersection->SetExitTime(pFlight,tNextTime);
	lastItem = newItem;
	return true;
}

//////////////////////////////////////ConflictConcernIntersectionInAirspaceList////////////////////////////////////////////////////
AirRouteIntersectionInSim* ConflictConcernIntersectionInAirspaceList::GetIntersection(AirRoutePointInSim* pIntersection)
{
	int nCount = GetCount();
	for (int i = 0; i < nCount; i++)
	{
		if (GetIntersection(i)->getInputPoint() == pIntersection )
			return GetIntersection(i);
	}
	return NULL;
}
