#include "StdAfx.h"
#include ".\headingairrouteseginsim.h"
#include "AirsideFlightInSim.h"
#include "AirspaceResource.h"
#include "Clearance.h"
#include "../../Common/Line2008.h"
#include "../../Common/Point2008.h"
#include "../../Common/DynamicMovement.h"
#include "ConflictConcernIntersectionInAirspace.h"
#include "AirspaceResource.h"

HeadingAirRouteSegInSim::HeadingAirRouteSegInSim(void)
{
}

HeadingAirRouteSegInSim::~HeadingAirRouteSegInSim(void)
{
}

CString HeadingAirRouteSegInSim::PrintResource() const
{
	CString strName;
	strName.Format("%s-%s", ((AirWayPointInSim*)m_pFirstIntersection->getInputPoint())->PrintResource(), ((AirWayPointInSim*)m_pSecondIntersection->getInputPoint())->PrintResource() );
	return strName;
}

CPoint2008 HeadingAirRouteSegInSim::GetDistancePoint(double dist)const
{
	if (dist == 0)
		return m_pFirstIntersection->getInputPoint()->GetPosition();

	CPoint2008 pointPos;

	return pointPos;
}

bool HeadingAirRouteSegInSim::operator ==(const HeadingAirRouteSegInSim& routeSeg)const
{
	return (m_pFirstIntersection== routeSeg.m_pFirstIntersection) && (m_pSecondIntersection == routeSeg.m_pSecondIntersection) 
		&& (m_HeadingType == routeSeg.m_HeadingType) &&( m_WPExtentPoint == routeSeg.m_WPExtentPoint);
}

AirspaceHoldInSim* HeadingAirRouteSegInSim::GetConnectedHold()
{
	if (m_pSecondIntersection)
	{
		return m_pSecondIntersection->GetConnectHold();
	}
	return NULL;
}

bool HeadingAirRouteSegInSim::ConverseCalculateFlightClearance(AirsideFlightInSim* pFlight,AirsideResource* pLastSegment,FlightRouteInSim* pCurrentRoute, 
															   ClearanceItem& nextItem,ClearanceItem& newItem,AirsideResourceList& ResourceList,ElapsedTime tEnterTime)
{

	ElapsedTime tEarliestArrTime = tEnterTime;
	ElapsedTime tArrTime = tEarliestArrTime;
	for (int i = 0; i < (int)ResourceList.size(); i++)
	{
		AirsideResource* pRes = ResourceList.at(i);
		if (pRes == this)
			break;
		tEarliestArrTime += pCurrentRoute->GetFlightMinTimeInSeg(pFlight,(AirRouteSegInSim*)pRes);
		tArrTime += pCurrentRoute->GetFlightTimeInSeg(pFlight,(AirRouteSegInSim*)pRes);
	}

	ClearanceItem newItem2(this,newItem.GetMode(),0);
	newItem2.SetSpeed(newItem.GetSpeed());
	newItem2.SetTime(tArrTime);
	newItem = newItem2;
	
	ElapsedTime eMaxTime = GetMaxTimeInSegment(pFlight,pCurrentRoute->GetRoutePointProperty(m_pFirstIntersection->getInputPoint()).GetSpeed(),nextItem.GetSpeed(), pLastSegment);

	ElapsedTime eMinTime = GetMinTimeInSegment(pFlight,pCurrentRoute->GetRoutePointProperty(m_pFirstIntersection->getInputPoint()).GetSpeed(),nextItem.GetSpeed(), pLastSegment);

	if(eMaxTime < eMinTime)
	{
		swap(eMinTime,eMaxTime);
	}
	ElapsedTime tEarliestEnter = nextItem.GetTime() - eMaxTime;
	ElapsedTime tLatestEnter = nextItem.GetTime() - eMinTime;

	if (tEarliestArrTime > tLatestEnter)
		return false;

	ElapsedTime tStartTime = -1L;
	ElapsedTime tEndTime = -1L;
	m_pFirstIntersection->GetAvailableTimeAtIntersection(pFlight,this,pCurrentRoute,tEarliestEnter,tStartTime,tEndTime);



	if (tStartTime > tLatestEnter)	// available time is later than  the latest enter segment time
	{
		newItem.SetTime(tLatestEnter);
		return false;
	}

	if (tStartTime < tArrTime)
		tStartTime = tArrTime;

	newItem.SetTime(tStartTime);

	m_pFirstIntersection->getInputPoint()->SetEnterTime(pFlight,tStartTime,OnTerminal,nextItem.GetSpeed());
	m_pFirstIntersection->getInputPoint()->SetExitTime(pFlight,tStartTime);

	return true;
}

bool HeadingAirRouteSegInSim::CalculateFlightClearance(AirsideFlightInSim* pFlight,AirsideResource* pPreSegment,FlightRouteInSim* pCurrentRoute, ClearanceItem& newItem,ElapsedTime tEnterTime)
{

	ElapsedTime tEarliestArrTime = tEnterTime + pCurrentRoute->GetFlightMinTimeInSeg(pFlight,(AirRouteSegInSim*)pPreSegment);
	ElapsedTime tLatestArrTime = tEnterTime + pCurrentRoute->GetFlightMaxTimeInSeg(pFlight,(AirRouteSegInSim*)pPreSegment);

	ElapsedTime tArrTime = tEnterTime + pCurrentRoute->GetFlightTimeInSeg(pFlight,(AirRouteSegInSim*)pPreSegment);

	ElapsedTime tStartEnter = -1L;
	ElapsedTime tEndEnter = -1L;
	m_pFirstIntersection->GetAvailableTimeAtIntersection(pFlight,this,pCurrentRoute,tEarliestArrTime,tStartEnter,tEndEnter);

	if (tStartEnter > tLatestArrTime)	// available time is later than  the latest enter segment time
		return false;

	if (tStartEnter < tArrTime )
		tStartEnter = tArrTime;

	if (tEndEnter > 0L && tStartEnter > tEndEnter )
		return false;

	double dSpeed1 = pCurrentRoute->GetRoutePointProperty(m_pFirstIntersection->getInputPoint()).GetSpeed();
	double dSpeed = pCurrentRoute->GetRoutePointProperty(m_pSecondIntersection->getInputPoint()).GetSpeed();

	m_pFirstIntersection->getInputPoint()->SetEnterTime(pFlight,tStartEnter,OnTerminal,dSpeed1);
	m_pFirstIntersection->getInputPoint()->SetExitTime(pFlight,tStartEnter);

	int nIdx = pCurrentRoute->GetSegmentIndex(this);

	ElapsedTime eMaxTime = GetMaxTimeInSegment(pFlight, dSpeed1, dSpeed, pCurrentRoute->GetItem(nIdx+1));
	ElapsedTime eMinTime = GetMinTimeInSegment(pFlight,dSpeed1,dSpeed, pCurrentRoute->GetItem(nIdx+1));
	if(eMaxTime < eMinTime)
	{
		swap(eMaxTime, eMinTime);
	}
	ElapsedTime tEarliestExit = tStartEnter + eMinTime;
	ElapsedTime tLatestExit = tStartEnter + eMaxTime;


	ElapsedTime tStartExit = -1L;
	ElapsedTime tEndExit = -1L;
	m_pSecondIntersection->GetAvailableTimeAtIntersection(pFlight,this,pCurrentRoute,tEarliestExit,tStartExit,tEndExit);
	
	if (tStartExit > tLatestExit)	// available time is later than  the latest enter segment time
		return false;

	if (tStartExit < tEarliestExit)
		tStartExit = tEarliestExit;

	ClearanceItem newItem2(this,newItem.GetMode(),0);
	newItem2.SetPosition(m_pSecondIntersection->getInputPoint()->GetPosition());
	newItem2.SetTime(tStartExit);
	newItem2.SetSpeed(dSpeed);
	newItem = newItem2;

	m_pSecondIntersection->getInputPoint()->SetEnterTime(pFlight,tStartExit,OnTerminal,dSpeed);
	m_pSecondIntersection->getInputPoint()->SetExitTime(pFlight,tStartExit);

	return true;
}

void HeadingAirRouteSegInSim::CalculateClearanceItems(AirsideFlightInSim* pFlight,FlightRouteInSim* pRoute, ClearanceItem& lastItem, AirsideResource* pNextRes, Clearance& newClearance)
{
	ElapsedTime tEnter = m_pFirstIntersection->getInputPoint()->GetOccupyInstance(pFlight).GetEnterTime();
	ElapsedTime tExit = m_pSecondIntersection->getInputPoint()->GetOccupyInstance(pFlight).GetEnterTime();
	double dSped1 = m_pFirstIntersection->getInputPoint()->GetOccupyInstance(pFlight).GetSpeed();
	double dSped2 = m_pSecondIntersection->getInputPoint()->GetOccupyInstance(pFlight).GetSpeed();
	double dAlt1 = lastItem.GetAltitude();
	double dAlt2 = pRoute->GetRoutePointProperty(m_pSecondIntersection->getInputPoint()).GetAlt();

	ASSERT(dSped1 == lastItem.GetSpeed());	//the speed should be same as last item
	ASSERT(tEnter == lastItem.GetTime());	//the time should be same as last item

	SetEnterTime(pFlight, tEnter,OnTerminal, dSped1);
	SetExitTime(pFlight,tExit);

	AirRoutePointInSim* pPoint = m_pFirstIntersection->getInputPoint();

	CPoint2008 nextPos = GetVectorAvailablePosition(tEnter, tExit, dSped1, dSped2, pNextRes, pFlight);

	CPoint2008 curPos = lastItem.GetPosition();
	if (nextPos != m_pSecondIntersection->getInputPoint()->GetPosition())
	{
		double dDistToNext = curPos.distance(nextPos);
		if (m_HeadingType == Direct )
		{

			double dRate = dDistToNext/(dDistToNext + nextPos.distance(m_pSecondIntersection->getInputPoint()->GetPosition()));

			ClearanceItem newItem(this,lastItem.GetMode(),0);
			newItem.SetPosition(nextPos);
			double dNextSpd = dSped1 + (dSped2 - dSped1)*dRate;
			newItem.SetSpeed(dNextSpd);
			ElapsedTime dT =  ElapsedTime(2.0* dDistToNext/(lastItem.GetSpeed() + dNextSpd)) + lastItem.GetTime();
			newItem.SetTime(dT);
			double dNextAlt = dAlt1 + (dAlt2 - dAlt1)* dRate;
			newItem.SetAltitude(dNextAlt);
			lastItem = newItem;
			newClearance.AddItem(newItem);
		}
		else
		{
			CPoint2008 projectPoint = GetProjectionPosition(nextPos,pNextRes,pFlight);
			double dTotalDist = dDistToNext + nextPos.distance(projectPoint) + projectPoint.distance(m_pSecondIntersection->getInputPoint()->GetPosition());
			
			double dRate = dDistToNext/dTotalDist;
			ClearanceItem newItem(this,lastItem.GetMode(),0);
			newItem.SetPosition(nextPos);
			double dNextSpd = dSped1 + (dSped2 - dSped1)*dRate;
			newItem.SetSpeed(dNextSpd);
			ElapsedTime dT = ElapsedTime(double(tExit - tEnter)*dRate) + lastItem.GetTime();
			newItem.SetTime(dT);

			double dNextAlt = dAlt1/* + (dAlt2 - dAlt1)* dRate*/;
			newItem.SetAltitude(dNextAlt);
			lastItem = newItem;
			newClearance.AddItem(newItem);


			double dRate2 = (dDistToNext + nextPos.distance(projectPoint))/dTotalDist;
			ClearanceItem nextItem = lastItem;
			nextItem.SetPosition(projectPoint);
			dNextSpd = dSped1 + (dSped2 - dSped1)*dRate2;
			nextItem.SetSpeed(dNextSpd);
			dT = ElapsedTime(double(tExit - tEnter)*(dRate2 - dRate)) + lastItem.GetTime();
			nextItem.SetTime(dT);
			ASSERT(dT < tExit);
			dNextAlt = dAlt1 + (dAlt2 - dAlt1)* (dRate2 - dRate)/(1 - dRate);
			nextItem.SetAltitude(dNextAlt);
			lastItem = nextItem;
			newClearance.AddItem(nextItem);
		}


	}

	pPoint = m_pSecondIntersection->getInputPoint();
	ClearanceItem newItem2(this,lastItem.GetMode(),0);
	newItem2.SetPosition(pPoint->GetPosition());
	newItem2.SetSpeed(dSped2);
	newItem2.SetTime(tExit);
	newItem2.SetAltitude(dAlt2);
	lastItem = newItem2;
	newClearance.AddItem(newItem2);


}

CPoint2008 HeadingAirRouteSegInSim::GetVectorAvailablePosition(ElapsedTime tEnter,ElapsedTime tExit, double dSped1, double dSped2, AirsideResource* pNextRes,AirsideFlightInSim* pFlight)
{
	CPoint2008 FirstPoint = m_pFirstIntersection->getInputPoint()->GetPosition();
	CPoint2008 SecondPoint = m_pSecondIntersection->getInputPoint()->GetPosition();
	ElapsedTime tTime = ElapsedTime(FirstPoint.distance(SecondPoint)/dSped1);

	if (tTime >= tExit - tEnter )
	{
		return SecondPoint;
	}

	double dDist = (dSped1+dSped2) * (tExit.asSeconds() - tEnter.asSeconds())/2;

	if (m_HeadingType == Direct)
	{
		CLine2008 routeline(FirstPoint, m_WPExtentPoint);
		CPoint2008 projectPoint = routeline.getProjectPoint(SecondPoint);
		double dDistToSecond = projectPoint.distance(SecondPoint);
		double dDistToFirst = projectPoint.distance(FirstPoint);

		double dLength = 0;
		if (projectPoint.distance(m_WPExtentPoint) < FirstPoint.distance(m_WPExtentPoint)) // projection point in the line segment
		{
			double dValue = dDist - dDistToFirst;
			dLength = ( dValue * dValue + dDistToSecond* dDistToSecond)/(2.0 * dValue);
		}
		else
		{
			double dValue = dDist + dDistToFirst;
			dLength = (dValue * dValue + dDistToSecond* dDistToSecond)/(2.0*dValue);
		}

		return routeline.getInlinePoint(dLength/routeline.GetLineLength());
	}

	//Aligned
	CPoint2008 projectPoint = GetProjectionPosition(FirstPoint,pNextRes,pFlight);
	CLine2008 projectLine = GetAlignProjectLine((AirRouteSegInSim*)pNextRes,pFlight);

	double dDistToProject = FirstPoint.distance(projectPoint);		// the vertical distance from first waypoint to next segment
	double dDistProjectToSecondPoint = projectPoint.distance(SecondPoint);		// the distance from project point of first waypoint on the line of next segment to the second waypoint

	double dMinDist = dDistToProject + dDistProjectToSecondPoint;
	double dRate = (dDist- dMinDist)/2.0;

	if (dRate < 0)
	{
		dRate = 0;
		dDist = dMinDist;
	}
	
	int nIn = projectLine.contains(projectPoint);
	if (nIn >0 || ( nIn <=0 && projectPoint.distance(projectLine.GetPoint1()) > projectPoint.distance(projectLine.GetPoint2())))
	{
		dRate += dDistProjectToSecondPoint;
	}

	CLine2008 routeLine(FirstPoint, m_WPExtentPoint);
	CPoint2008 PropPoint = routeLine.getInlinePoint(dRate/routeLine.GetLineLength());
	projectPoint = projectLine.getProjectPoint(PropPoint);

	return PropPoint;
}

double HeadingAirRouteSegInSim::GetExpectDistance()
{
	CPoint2008 pointPos = m_pFirstIntersection->getInputPoint()->GetPosition();
	return pointPos.distance(m_pSecondIntersection->getInputPoint()->GetPosition());
}

ElapsedTime HeadingAirRouteSegInSim::GetDistExpectTimeInSegment(FlightRouteInSim* pFlightRoute,AirsideFlightInSim* pFlight,double dDist)
{
	ASSERT(m_pFirstIntersection);
	ASSERT(m_pSecondIntersection);
	AirRoutePointInSim* pFirstIntersection = m_pFirstIntersection->getInputPoint();
	FlightPlanPropertyInSim planProperty1 = pFlightRoute->GetRoutePointProperty(pFirstIntersection);
	double dSpeed1 = planProperty1.GetSpeed();

	AirRoutePointInSim* pSecondIntersection = m_pSecondIntersection->getInputPoint();
	FlightPlanPropertyInSim planProperty2 = pFlightRoute->GetRoutePointProperty(pSecondIntersection);
	double dSpeed2 = planProperty2.GetSpeed();

	DynamicMovement dyNamicMove(dSpeed2,dSpeed1,GetExpectDistance());
	return dyNamicMove.GetDistTime(dDist);
}

ElapsedTime HeadingAirRouteSegInSim::GetExpectTimeInSegment(FlightRouteInSim* pFlightRoute,AirsideFlightInSim* pFlight)
{
	ASSERT(m_pFirstIntersection);
	ASSERT(m_pSecondIntersection);
	AirRoutePointInSim* pFirstIntersection = m_pFirstIntersection->getInputPoint();
	FlightPlanPropertyInSim planProperty1 = pFlightRoute->GetRoutePointProperty(pFirstIntersection);
	double dSpeed1 = planProperty1.GetSpeed();

	AirRoutePointInSim* pSecondIntersection = m_pSecondIntersection->getInputPoint();
	FlightPlanPropertyInSim planProperty2 = pFlightRoute->GetRoutePointProperty(pSecondIntersection);
	double dSpeed2 = planProperty2.GetSpeed();

	DynamicMovement dyNamicMove(dSpeed1,dSpeed2,GetExpectDistance());
	return dyNamicMove.GetDistTime(GetExpectDistance());
}

ElapsedTime HeadingAirRouteSegInSim::GetMaxTimeInSegment(AirsideFlightInSim* pFlight, double dSped1,  double dSped2, AirsideResource* pNextRes)
{
	double dDist = m_WPExtentPoint.distance(m_pFirstIntersection->getInputPoint()->GetPosition()) ;

	if (m_HeadingType == Direct)
	{
		dDist += m_WPExtentPoint.distance(m_pFirstIntersection->getInputPoint()->GetPosition());
	}
	else
	{
		CPoint2008 extentPoint = GetProjectionPosition(m_WPExtentPoint, pNextRes,pFlight);
		dDist += extentPoint.distance(m_WPExtentPoint);
		dDist += extentPoint.distance(m_pSecondIntersection->getInputPoint()->GetPosition());
	}
	return ElapsedTime(2.0*dDist/(dSped1+dSped2));

}

ElapsedTime HeadingAirRouteSegInSim::GetMinTimeInSegment(AirsideFlightInSim* pFlight,  double dSped1,  double dSped2, AirsideResource* pNextRes)
{
	CPoint2008 pointPos = m_pFirstIntersection->getInputPoint()->GetPosition();
	double dDist = 0;
	if (m_HeadingType == Direct)
	{
		dDist = pointPos.distance(m_pSecondIntersection->getInputPoint()->GetPosition());
	}
	else
	{
		CPoint2008 extentPoint = GetProjectionPosition(pointPos, pNextRes,pFlight);
		dDist += extentPoint.distance(pointPos);
		dDist += extentPoint.distance(m_pSecondIntersection->getInputPoint()->GetPosition());
	}
		
	return ElapsedTime( 2.0*(dDist)/(dSped1+dSped2) );

}

CPoint2008 HeadingAirRouteSegInSim::GetProjectionPosition(CPoint2008 CurrentPos, AirsideResource* pNextRes, AirsideFlightInSim* pFlight)
{

	if (m_HeadingType == Direct)
	{
		return m_pSecondIntersection->getInputPoint()->GetPosition();
	}

	CPoint2008 WPPos1 = m_pSecondIntersection->getInputPoint()->GetPosition();

	if (pNextRes->GetType() == AirsideResource::ResType_AirRouteSegment )		
	{
		CLine2008 projectLine = GetAlignProjectLine((AirRouteSegInSim*)pNextRes,pFlight);
		return projectLine.getProjectPoint(CurrentPos);
	}

	return WPPos1;												

}

CLine2008 HeadingAirRouteSegInSim::GetAlignProjectLine(AirRouteSegInSim* pNextSeg,AirsideFlightInSim* pFlight)
{
	CPoint2008 WPPos1 = m_pSecondIntersection->getInputPoint()->GetPosition();
	if (pNextSeg->GetSecondConcernIntersection())		//not final segment 
	{
		CPoint2008 WPPos2 = pNextSeg->GetSecondConcernIntersection()->getInputPoint()->GetPosition();

		CLine2008 routeLine(WPPos1,WPPos2);
		return routeLine;
	}

	LogicRunwayInSim* pPort = pFlight->GetLandingRunway();
	if (pFlight->IsDepartingOperation())
	{
		pPort = pFlight->GetAndAssignTakeoffRunway();
	}

	CPoint2008 WPPos2 = pPort->GetDistancePoint(0);

	CLine2008 routeLine(WPPos1,WPPos2);
	return routeLine;			
	
}

#include "..\..\Results\AirsideFlightEventLog.h"
void HeadingAirRouteSegInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = 0;
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource();
}

void HeadingAirRouteSegInSim::GenerateClearanceItems( AirsideFlightInSim* pFlight,FlightRouteInSim* pRoute, AirsideResource* pNextRes , ClearanceItem& lastItem, Clearance& newClearance )
{
	OccupancyInstance ocyInst = GetOccupyInstance(pFlight);
	ASSERT(ocyInst.GetFlight());


	OccupancyInstance ocyInstExitWaypoint = m_pSecondIntersection->getInputPoint()->GetOccupyInstance(pFlight);

	ElapsedTime tEnter = ocyInst.GetEnterTime();
	ElapsedTime tExit = ocyInstExitWaypoint.GetEnterTime();

	double dSped1 = ocyInst.GetSpeed();
	double dSped2 = ocyInstExitWaypoint.GetSpeed();

	double dAlt1 = lastItem.GetAltitude();
	double dAlt2 = pRoute->GetRoutePointProperty(m_pSecondIntersection->getInputPoint()).GetAlt();

	ASSERT(dSped1 == lastItem.GetSpeed());	//the speed should be same as last item
	ASSERT(tEnter == lastItem.GetTime());	//the time should be same as last item

	//SetEnterTime(pFlight, tEnter,OnTerminal, dSped1);
	//SetExitTime(pFlight,tExit);

	DistanceUnit distNeedTo = (dSped1+dSped2)*.5* double(tExit-tEnter);
	CPoint2008 endPoint = m_pSecondIntersection->getInputPoint()->GetPosition();
	DistanceUnit distDirectTo = lastItem.GetPosition().distance3D(endPoint);
	if(distNeedTo > distDirectTo) //add middle points
	{
		
		DistanceUnit diffDist = distNeedTo - distDirectTo;
		double dHalfDist = 0.5*diffDist;
		ARCVector3 dir = (m_WPExtentPoint - lastItem.GetPosition());
		CPoint2008 nextPos = lastItem.GetPosition() + dir.GetLengtDir(dHalfDist);

		DynamicMovement dynMove(dSped1,dSped2,distNeedTo);
		ElapsedTime startTime = lastItem.GetTime();
		


		ClearanceItem newItem(this,lastItem.GetMode(),0);
		newItem.SetPosition(nextPos);
		newItem.SetTime(startTime + dynMove.GetDistTime(dHalfDist) );
		newItem.SetSpeed(dynMove.GetDistSpeed(dHalfDist));	
		double dNextAlt = dAlt1 + (dAlt2 - dAlt1)* dynMove.getTrate(dHalfDist);
		newItem.SetAltitude(dNextAlt);
		lastItem = newItem;
		newClearance.AddItem(newItem);

		if(m_HeadingType!=Direct && pNextRes->GetType() == AirsideResource::ResType_AirRouteSegment){
			double dAlignLen = 0.5*(distNeedTo - nextPos.distance3D(endPoint) - dHalfDist);
				

			CLine2008 lineseg =  GetAlignProjectLine((AirRouteSegInSim*)pNextRes,pFlight);
			ARCVector3 dirAlign = (lineseg.GetPoint1()-lineseg.GetPoint2());
			dirAlign.SetLength(dAlignLen);


			CPoint2008 item2pos = dirAlign + endPoint;
			newItem.SetPosition(item2pos);
			newItem.SetTime( dynMove.GetDistTime(distNeedTo - dAlignLen)+startTime );
			newItem.SetSpeed(dynMove.GetDistSpeed(distNeedTo - dAlignLen));				
			newItem.SetAltitude(dNextAlt);
			lastItem = newItem;
			newClearance.AddItem(lastItem);
		}
	}

	
	AirRoutePointInSim* pPoint = m_pSecondIntersection->getInputPoint();
	ClearanceItem newItem2(this,lastItem.GetMode(),0);
	newItem2.SetPosition(pPoint->GetPosition());
	newItem2.SetSpeed(dSped2);
	newItem2.SetTime(tExit);
	newItem2.SetAltitude(dAlt2);
	lastItem = newItem2;
	newClearance.AddItem(newItem2);
}
