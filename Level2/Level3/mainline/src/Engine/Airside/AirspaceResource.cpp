//--------------------------------------------------------------------------------------------------------------------------
//						Creator:		Emily(ARC)
//						Author:			Sky.wen(ARC)
//						Date:			Sep 22, 2012
//						Time:			1:32PM
//						Purpose:		the method doesn't have comment that make other person understand. 
//											So need give some comment to the mothod
//----------------------------------------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include "AirspaceResource.h"
#include "../../Common/ARCUnit.h"
#include <stdlib.h>
#include "AirsideFlightInSim.h"
#include "Clearance.h"
#include "AirsideResourceManager.h"
#include "AirspaceResourceManager.h"
#include "../../InputAirside/FlightPlan.h"
#include "AirTrafficController.h"
#include <limits>
#include "../../InputAirside/GoAroundCriteriaDataProcess.h"
#include "FlightPerformanceManager.h"
#include "../../InputAirside/AirRoute.h"
#include "../../Common/line.h"
#include "ConflictConcernIntersectionInAirspace.h"
//#include "HeadingAirRouteSegInSim.h"
//#include "../../Common/EngineDiagnoseEx.h"
//#include "../../Common/ARCExceptionEx.h"
//#include "SimulationErrorShow.h"
//#include "ArrivalDelayTriggersDelay.h"
#include "FlightConflictAtWaypointDelay.h"
#include "../../Results/OutputAirside.h"
#include ".\AirHoldInInSim.h"
#include "../../Common/ARCMathCommon.h"
static DistanceUnit StepDistinAir = 100000;  // 1000 meters


/////////////////////////////////AirRouteSegInSim///////////////////////////////////
FlightRouteInSim::FlightRouteInSim()
{
	m_vItems.clear();m_pAirRouteInSim = NULL;
}

FlightRouteInSim::~FlightRouteInSim()
{

}

DistanceUnit AirRouteSegInSim::GetSegmentDistance() const
{

	DistanceUnit SegDist = 0;
	int nSize = m_vRoutePoints.size();
	if (m_pFirstIntersection && nSize > 0)
	{
		SegDist = m_vRoutePoints[0]->GetPosition().distance(m_pFirstIntersection->getInputPoint()->GetPosition());
	}

	for (int i =0; i < nSize -1; i++)
	{
		SegDist += m_vRoutePoints[i]->GetPosition().distance(m_vRoutePoints[i+1]->GetPosition());
	}

	if (m_pSecondIntersection)
	{
		if (nSize > 0)
			SegDist += m_vRoutePoints[nSize-1]->GetPosition().distance(m_pSecondIntersection->getInputPoint()->GetPosition());
		else
			SegDist += m_pSecondIntersection->getInputPoint()->GetPosition().distance(m_pFirstIntersection->getInputPoint()->GetPosition());
	}

	return SegDist;
}
CPoint2008 AirRouteSegInSim::GetDistancePoint(double dist) const
{
	CPoint2008 pos;
	return pos;
}

void AirRouteSegInSim::CalculateClearanceItems(AirsideFlightInSim* pFlight,FlightRouteInSim* pRoute,ClearanceItem& lastItem, ElapsedTime tExit, Clearance& newClearance )
{
	int nPointCount = int(m_vRoutePoints.size());
	if (nPointCount <= 0)
		return;

	ElapsedTime tEnter = lastItem.GetTime();
	ElapsedTime tDuration = tExit - tEnter;
	double dSegDist = GetSegmentDistance();
	ClearanceItem preItem = lastItem;

	CInTrailSeparationInSim* pIntrailSeparation = pFlight->GetAirTrafficController()->GetIntrailSeparation();

	double dDist = 0;
	for (int i =0; i < nPointCount; i++)
	{	
		AirRoutePointInSim* pPoint = (AirRoutePointInSim*)m_vRoutePoints[i];
		FlightPlanPropertyInSim property = pRoute->GetRoutePointProperty(pPoint);
		double dSpd1 = preItem.GetSpeed();
		double dSpd2 = property.GetSpeed();
		double dAlt = property.GetAlt();

		double dDistance = preItem.GetPosition().distance(pPoint->GetPosition());
		double tNormal = 2.0*dDistance/(dSpd1+dSpd2);
		ElapsedTime tAtPoint = ElapsedTime(tNormal)+preItem.GetTime();

		//AirsideFlightInSim* pLeadFlight = pPoint->GetLastOccupyInstance().GetFlight();
		//if (pLeadFlight)
		//{
		//	ElapsedTime tLeadTime = pPoint->GetLastOccupyInstance().GetEnterTime();
		//	double dLeadSpd = pPoint->GetLastOccupyInstance().GetSpeed();
		//	ElapsedTime tNewTime = pPoint->RecalculateEnterTime(pFlight,pLeadFlight,tLeadTime,tAtPoint,dLeadSpd);

		//	tAtPoint = tNewTime;
		//}

		pPoint->SetEnterTime(pFlight,tAtPoint,property.GetMode(),dSpd2);
		pPoint->SetExitTime(pFlight,tAtPoint);

		ClearanceItem newItem(this,property.GetMode(),0);
		newItem.SetPosition(pPoint->GetPosition());
		newItem.SetSpeed(dSpd2);
		newItem.SetAltitude(dAlt);
		newItem.SetTime(tAtPoint);

		//int nDelayId = -1;
		//if(tDelayAtPoint > 0L)
		//{
		//	FlightConflictAtWaypointDelay* pDelay = new FlightConflictAtWaypointDelay;
		//	//pDelay->SetAtObjectID(pPoint->getID());
		//	pDelay->SetFlightID(pFlight->GetUID());
		//	pDelay->SetDelayResult(FltDelayReason_Slowed);
		//	pDelay->SetDelayTime(tDelayAtPoint);
		//	pDelay->SetStartTime(tAtPoint - tDelayAtPoint);
		//	if (pFlight->GetOutput())
		//	{
		//		nDelayId = pFlight->GetOutput()->LogFlightDelay(pDelay);
		//	}
		//}

		//if (nDelayId > -1)
		//	newItem.SetDelayId(nDelayId);

		newClearance.AddItem(newItem);
		preItem = newItem;

	}

	SetEnterTime(pFlight,tEnter,lastItem.GetMode(), lastItem.GetSpeed() );
	SetExitTime(pFlight,tExit);

}

AirRouteIntersectionInSim* AirRouteSegInSim::GetFirstConcernIntersection()
{
	return m_pFirstIntersection;
}

AirRouteIntersectionInSim* AirRouteSegInSim::GetSecondConcernIntersection()
{
	return m_pSecondIntersection;
}

void AirRouteSegInSim::SetSecondIntersection(AirRouteIntersectionInSim* pIntersection)
{
	m_pSecondIntersection = pIntersection;
}

double AirRouteSegInSim::GetDistLastWaypointToHold()
{
	AirRoutePointInSim* pPrePoint = GetRoutePointByIdx(GetRoutePointCount()-1);
	if (pPrePoint == NULL)
	{
		return m_pFirstIntersection->getInputPoint()->GetPosition().distance(m_pSecondIntersection->getInputPoint()->GetPosition());
	}

	return pPrePoint->GetPosition().distance(m_pSecondIntersection->getInputPoint()->GetPosition());
		
}

//calculate flight with the ATC communicate point before arriving hold,
//only air route segment which with airspace hold has the ATC managed point
//if the distance between airspace hold and previous waypoint is long enough, the flight climb angle not exceed 6 degree from ATC managed point to hold
//if  the distance between airspace hold and previous waypoint is short,the ATC managed point is previous waypoint
CPoint2008 AirRouteSegInSim::GetATCManagedPoint(AirsideFlightInSim* pFlight)
{
	CPoint2008 _point;
	if (m_pSecondIntersection)
	{
		AirspaceHoldInSim* pHold = m_pSecondIntersection->GetConnectHold();
		if (pHold)
		{
			//FlightPlanPropertyInSim Pointprop(NULL);
			//if (pFlight->IsArrivingOperation())
			//	Pointprop = pFlight->GetSTAR()->GetRoutePointProperty(m_pSecondIntersection->getInputPoint());
			//else
			//	Pointprop = pFlight->GetSID()->GetRoutePointProperty(m_pSecondIntersection->getInputPoint());
			//double dAlt = Pointprop.GetAlt();
			//double dDistToPoint = abs((pHold->GetHoldInput()->getMaxAltitude() - dAlt)/tan(ARCMath::DegreesToRadians(3.0)));
			//CPoint2008 SecPos = m_pSecondIntersection->getInputPoint()->GetPosition();
			//
			double dDistToHold = pHold->GetATCManagedDistance();
			AirRoutePointInSim* pPrePoint = GetRoutePointByIdx(GetRoutePointCount()-1);
			if (pPrePoint == NULL)
			{
				//double dDistToFirst = m_pFirstIntersection->getInputPoint()->GetPosition().distance(SecPos) - dDistToPoint;
				//if (dDistToFirst <=0)
				//{
				//	return m_pFirstIntersection->getInputPoint()->GetPosition();
				//}

				CPath2008 path;
				path.init(2);
				path[0] = m_pSecondIntersection->getInputPoint()->GetPosition();
				path[1] = m_pFirstIntersection->getInputPoint()->GetPosition();

				_point = path.GetDistPoint(dDistToHold);
			
			}
			else
			{
				//double dDistToLast = pPrePoint->GetPosition().distance(SecPos) - dDistToPoint;
				//if (dDistToLast <=0)
				//{
				//	return pPrePoint->GetPosition();
				//}

				CPath2008 path;
				path.init(2);
				path[0] = m_pSecondIntersection->getInputPoint()->GetPosition();
				path[1] = pPrePoint->GetPosition();
				_point =  path.GetDistPoint(dDistToHold);				
			}
			
		}
	}

	return _point;
}

void AirRouteSegInSim::DeleteRoutePoint(int nIdx)
{
	if (nIdx >= GetRoutePointCount())
		return;

	m_vRoutePoints.erase(m_vRoutePoints.begin() + nIdx);
}

CString AirRouteSegInSim::PrintResource() const
{
	CString strName;
	if(m_pFirstIntersection && m_pSecondIntersection)
		strName.Format("%s-%s", m_pFirstIntersection->getInputPoint()->PrintResource(), m_pSecondIntersection->getInputPoint()->PrintResource() );
	else
		strName.Format("%s", m_pFirstIntersection->getInputPoint()->PrintResource());

	return strName;
}

bool AirRouteSegInSim::operator ==(const AirRouteSegInSim& otherSeg) const
{
	if (m_pFirstIntersection == otherSeg.m_pFirstIntersection && m_pSecondIntersection == otherSeg.m_pSecondIntersection && m_vRoutePoints == otherSeg.m_vRoutePoints)
	{
		return true;
	}
	return false;
}

AirsideFlightInSim* AirRouteSegInSim::GetForemostFlightInSeg(ElapsedTime tTime)
{
	int nCount = (int)m_vOccupancyTable.size();
	for (int i =0; i < nCount; i ++)
	{
		OccupancyInstance Instance = m_vOccupancyTable[i];
		if (Instance.GetEnterTime() <= tTime && Instance.GetExitTime()>= tTime)
			return Instance.GetFlight();
	}
	return NULL;
}

AirsideFlightInSim* AirRouteSegInSim::GetBackmostFlightInSeg(ElapsedTime tTime)
{
	int nCount = (int)m_vOccupancyTable.size();
	for (int i = nCount -1; i >= 0; i --)
	{
		OccupancyInstance Instance = m_vOccupancyTable[i];
		if (Instance.GetEnterTime() <= tTime && Instance.GetExitTime()>= tTime)
			return Instance.GetFlight();
	}
	return NULL;
}

AirRoutePointInSim* AirRouteSegInSim::GetRoutePointByIdx(int nIdx)
{
	if (GetRoutePointCount() > nIdx && nIdx >=0)
	{
		return m_vRoutePoints[nIdx];
	}
	return NULL;
}

AirspaceHoldInSim* AirRouteSegInSim::GetConnectedHold()
{
	if (m_pSecondIntersection)
	{
		return m_pSecondIntersection->GetConnectHold();
	}
	else if (m_pFirstIntersection)			//final segment
	{
		return m_pFirstIntersection->GetConnectHold();
	}

	return NULL;
}

AirsideFlightInSim* AirRouteSegInSim::GetLeadFlight(AirsideFlightInSim* pFlight)
{
	if (m_vOccupancyTable.empty()|| m_vOccupancyTable[0].GetFlight() == pFlight)
		return NULL;

	int nCount = (int)m_vOccupancyTable.size();
	for (int i = nCount -1 ; i >0 ; i++)
	{
		OccupancyInstance Instance = m_vOccupancyTable[i];
		if (Instance.GetFlight() == pFlight)
			return m_vOccupancyTable[i-1].GetFlight();
	}

	return m_vOccupancyTable[nCount-1].GetFlight();
}

#include "..\..\Results\AirsideFlightEventLog.h"
void AirRouteSegInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = 0;
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource();
}

void AirRouteSegInSim::InsertRouteAfterPoint( AirRoutePointInSim *pAfter, AirRoutePointInSim* pInsert )
{
	if(m_vRoutePoints.size() == 0)
	{
		AddRoutePoint(pInsert);
		return;
	}
	if(pAfter == NULL)
	{
		m_vRoutePoints.insert(m_vRoutePoints.begin(), pInsert);
		return;
	}
	if(pAfter == m_pFirstIntersection->getInputPoint())
	{
		m_vRoutePoints.insert(m_vRoutePoints.begin(), pInsert);
		return;
	}
	if(pAfter == m_pSecondIntersection->getInputPoint())
	{
		AddRoutePoint(pInsert);
		return;
	}

	std::vector<AirRoutePointInSim*>::iterator iterFind =
											std::find(m_vRoutePoints.begin(), m_vRoutePoints.end(), pAfter);
	if(iterFind != m_vRoutePoints.end())
	{
		m_vRoutePoints.insert(iterFind + 1,pInsert);
	}
	else
	{
		m_vRoutePoints.push_back(pInsert);
	}

	
}

void AirRouteSegInSim::InsertRouteBeforePoint( AirRoutePointInSim *pBefore, AirRoutePointInSim* pInsert )
{

}

////////////////////////AirRouteSegInSimList////////////////////////////////////
AirRouteSegInSim* AirRouteSegInSimList::GetSegmentInList( AirRouteSegInSim* pSegment )
{
	int nCount = GetCount();
	for (int i = 0; i < nCount ; i++)
	{
		AirRouteSegInSim* pSegInList =GetSegment(i);
		if (*pSegInList == *pSegment)
		{
			return pSegInList;
		}
	}
	m_vRouteSegs.push_back(pSegment);
	return NULL;
}


///////////////////////////////FlightRouteInSim//////////////////////////////////////////////////
AirsideResource* FlightRouteInSim::GetItem(int idx)
{
	if (idx <0 || idx > GetItemCount()-1)
		return NULL;

	return m_vItems[idx];
}
double FlightRouteInSim::CalculateFlightDecel(double dForeSpd,double dAftSpd,double dDist, double dSep)
{
	if(dForeSpd >= dAftSpd)
		return 0;

	double tTime = 2.0*(dDist - dSep)/(dAftSpd - dForeSpd);

	return (dAftSpd - dForeSpd)/tTime;
}

double FlightRouteInSim::RecalculateFlightSpeed(double dSpeed,double dDecel,ElapsedTime tTime)
{
	return dSpeed + dDecel* double(tTime);
}

bool FlightRouteInSim::IsFlightInAirspace(AirsideFlightInSim* pFlight)
{
	if ((pFlight->GetMode() > OnBirth && pFlight->GetMode() < OnLanding) || (pFlight->GetMode() > OnTakeoff && pFlight->GetMode() < OnTerminate))
		return true;

	return false;
}




int FlightRouteInSim::GetSegmentIndex( AirsideResource * pResource ) const
{
	if(pResource == NULL )
		return -1;

	int nCount = GetItemCount();

	for(int i=0;i < nCount;i++)
	{
		if( m_vItems[i] == pResource )
		{
			return i;
		}
		
		if (m_vItems[i]->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			AirRouteSegInSim* pSeg = (AirRouteSegInSim*)m_vItems.at(i);

			if (pResource->GetType() == AirsideResource::ResType_AirspaceHold 
				&& ((AirspaceHoldInSim*)pResource)->GetWaypointInSim() == pSeg->GetFirstConcernIntersection()->getInputPoint())
				return i-1;

			if(pSeg->GetFirstConcernIntersection()->getInputPoint() == pResource )
				return i-1;
		}

		if (m_vItems[i]->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			HeadingAirRouteSegInSim* pSeg = (HeadingAirRouteSegInSim*)m_vItems.at(i);
			if(pSeg->GetFirstIntersection()->getInputPoint() == pResource )
				return i-1;

			if (pResource->GetType() == AirsideResource::ResType_AirspaceHold 
				&& ((AirspaceHoldInSim*)pResource)->GetWaypointInSim() == pSeg->GetSecondIntersection()->getInputPoint())
				return i;
		}
	}
	return -1;	
}

//-----------------------------------------------------------------------------------------------------------------------------
//Summary:
//			retrieve route point property
//---------------------------------------------------------------------------------------------------------------------------
FlightPlanPropertyInSim FlightRouteInSim::GetRoutePointProperty( AirRoutePointInSim * pWaypt )
{
	int nCount = (int)m_FlightPlanProperties.size();
	FlightPlanPropertyInSim property(pWaypt);		
	if(nCount>0)
	{
		FlightPlanPropertyInSim NearProperty = m_FlightPlanProperties[0];
		for (int i = 0; i < nCount; i++ )
		{
			AirRoutePointInSim* pPoint =( m_FlightPlanProperties.at(i)).GetRoutePoint();
			if ( pPoint == pWaypt)
				return m_FlightPlanProperties.at(i);

			if (pWaypt->GetPosition().distance(pPoint->GetPosition()) < pWaypt->GetPosition().distance(NearProperty.GetRoutePoint()->GetPosition()))
			{
				NearProperty = m_FlightPlanProperties.at(i);
			}

		}
		
		property.SetAlt((pWaypt->GetAltHigh() + pWaypt->GetAltLow())/2.0);
		property.SetMode(NearProperty.GetMode());		//if not find the property of point, give the nearest point's property
		property.SetSpeed(NearProperty.GetSpeed());

		if (NearProperty.IsArrival())
			property.SetArrival();
	}else
	{
		property.SetMode(OnTerminal);		//if not find the property of point, give the nearest point's property
		property.SetSpeed(10000);

	}
	
	return property;


}
//------------------------------------------------------------------------------------------------------------------
//Summary:
//			make sure circuit route less or equal 2 resource
//------------------------------------------------------------------------------------------------------------------
void FlightRouteInSim::GetConcernCircuitRouteSegs(AirsideResource* pCurrentResource, AirsideResourceList& ResourceList)
{
//	int nIdx = GetSegmentIndex(pCurrentResource);

	for (int i = 0; i < GetItemCount(); i++)
	{
		if (i <0)
			continue;

		int nCount = (int)ResourceList.size();
		if (nCount >= 2)
			return;
		
		AirsideResource* pResource = GetItem(i);
		if (pResource)
		{
			if (pResource->GetType() == AirsideResource::ResType_AirRouteSegment)
			{
				AirRouteIntersectionInSim* pIntersection = ((AirRouteSegInSim*)pResource)->GetSecondConcernIntersection();
				if ( pIntersection == NULL && i == GetItemCount() -1
					&& pResource == pCurrentResource)	//last segment is current resource
				{
					ResourceList.clear();
					return;
				}
				if ( pIntersection && pIntersection->getInputPoint()->GetType() == AirsideResource::ResType_WayPoint && ((AirWayPointInSim*)pIntersection->getInputPoint())->GetHold())
				{
					ResourceList.push_back(pResource);
					return;
				}
			}
			if (pResource->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
			{
				AirWayPointInSim* pWaypoint = (AirWayPointInSim*)((HeadingAirRouteSegInSim*)pResource)->GetSecondIntersection()->getInputPoint();
				if (pWaypoint->GetHold())
				{
					ResourceList.push_back(pResource);
					return;
				}
			}
			ResourceList.push_back(pResource);
		}

	}
}

void FlightRouteInSim::GetResouceRouteSegs(AirsideResource* pCurrentResource, AirsideResourceList& ResourceList)
{
//	int nIdx = GetSegmentIndex(pCurrentResource);

	for (int i = 0; i < GetItemCount(); i++)
	{
		if (i <0)
			continue;

		AirsideResource* pResource = GetItem(i);
		if (pResource)
		{
			if (pResource->GetType() == AirsideResource::ResType_AirRouteSegment)
			{

				AirRouteIntersectionInSim* pIntersection = ((AirRouteSegInSim*)pResource)->GetSecondConcernIntersection();
				if ( pIntersection == NULL && i == GetItemCount() -1
					&& pResource == pCurrentResource)	//last segment is current resource
				{
					ResourceList.clear();
					return;
				}
				if ( pIntersection && pIntersection->getInputPoint()->GetType() == AirsideResource::ResType_WayPoint && ((AirWayPointInSim*)pIntersection->getInputPoint())->GetHold())
				{
					ResourceList.push_back(pResource);
					return;
				}
			}
			if (pResource->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
			{
				AirWayPointInSim* pWaypoint = (AirWayPointInSim*)((HeadingAirRouteSegInSim*)pResource)->GetSecondIntersection()->getInputPoint();
				if (pWaypoint->GetHold())
				{
					ResourceList.push_back(pResource);
					return;
				}
			}
			ResourceList.push_back(pResource);
		}

	}
}
//-------------------------------------------------------------------------------------------------------------------
//Summary:
//			find next resource after current resource such waypoint has hold and header airroute
//-------------------------------------------------------------------------------------------------------------------
void FlightRouteInSim::GetConcernRouteSegs(AirsideResource* pCurrentResource,AirsideResourceList& ResourceList)		//divided by hold
{
	int nIdx = GetSegmentIndex(pCurrentResource);

	for (int i = nIdx+1; i < GetItemCount(); i++)
	{
		if (i <0)
			continue;

		AirsideResource* pResource = GetItem(i);
		if (pResource)
		{
			if (pResource->GetType() == AirsideResource::ResType_AirRouteSegment)
			{

				AirRouteIntersectionInSim* pIntersection = ((AirRouteSegInSim*)pResource)->GetSecondConcernIntersection();
				if ( pIntersection == NULL && i == GetItemCount() -1
					&& pResource == pCurrentResource)	//last segment is current resource
				{
					ResourceList.clear();
					return;
				}
				if ( pIntersection && pIntersection->getInputPoint()->GetType() == AirsideResource::ResType_WayPoint && ((AirWayPointInSim*)pIntersection->getInputPoint())->GetHold())
				{
					ResourceList.push_back(pResource);
					return;
				}
			}
			if (pResource->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
			{
				AirWayPointInSim* pWaypoint = (AirWayPointInSim*)((HeadingAirRouteSegInSim*)pResource)->GetSecondIntersection()->getInputPoint();
				if (pWaypoint->GetHold())
				{
					ResourceList.push_back(pResource);
					return;
				}
			}
			ResourceList.push_back(pResource);
		}

	}

}
//--------------------------------------------------------------------------------------------------------------------------------------------
//Summary:
//				calculate max time between two intersection node on airroute
//			every route point has min and max speed	
//--------------------------------------------------------------------------------------------------------------------------------------------
ElapsedTime FlightRouteInSim::GetFlightMaxTimeInSeg(AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg)
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

//--------------------------------------------------------------------------------------------------------------------------------------------
//Summary:
//				calculate min time between two intersection node on airroute
//			every route point has min and max speed	
//--------------------------------------------------------------------------------------------------------------------------------------------
ElapsedTime FlightRouteInSim::GetFlightMinTimeInSeg(AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg)
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

//------------------------------------------------------------------------------------------------------------------
//Summary:
//			consider of separation time and time flight move to runway to get flight whether can go to runway
//--------------------------------------------------------------------------------------------------------------------
ElapsedTime FlightRouteInSim::GetLatestRunwayAvalableTime(AirsideFlightInSim* pFlight,ElapsedTime tSepartionTime, double dSeparationDistance)
{
	LogicRunwayInSim* pRuwnay = GetRelateRunway();
	if (pRuwnay == NULL || pFlight->GetSTAR() != this)
		return -1L;

	AirWayPointInSim* pWaypoint = GetLastHoldConnectedWaypoint();

	if (pWaypoint == NULL)
		return -1L;

	AirsideResourceList reslist;
	GetConcernRouteSegs(pWaypoint,reslist);

	int nCount = reslist.size();
	ElapsedTime tTime = 0L;
	for(int i =0; i< nCount-1; i++)
	{
		AirsideResource* pRes = reslist.at(i);
		if (pRes->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			tTime += GetFlightTimeInSeg(pFlight, (AirRouteSegInSim*)pRes);
		}
		if (pRes->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			double dSpeed1 = GetRoutePointProperty(((HeadingAirRouteSegInSim*)pRes)->GetFirstIntersection()->getInputPoint()).GetSpeed();
			double dSpeed2 = GetRoutePointProperty(((HeadingAirRouteSegInSim*)pRes)->GetSecondIntersection()->getInputPoint()).GetSpeed();
			tTime += ((HeadingAirRouteSegInSim*)pRes)->GetMinTimeInSegment(pFlight,dSpeed1,dSpeed2,reslist.at(i+1));
		}

	}
	tTime += CalculateTimeFromLastSegToLogicRunway(pFlight);

	ElapsedTime tDistSepTime = 2*dSeparationDistance/(pFlight->GetSpeed() + pFlight->GetPerformance()->getLandingSpeed(pFlight));

	tSepartionTime = tSepartionTime > tDistSepTime? tSepartionTime: tDistSepTime;

	if (tTime >= tSepartionTime)
		return pFlight->GetTime();

	return pFlight->GetTime() - (tSepartionTime -tTime);
	

}

double FlightRouteInSim::CalculateDistanceFromLastSegToLogicRunway( AirsideFlightInSim *pFlight )
{
	if (pFlight->GetSTAR() != this)
		return -1L;

	int nCount = m_vItems.size();
	if(nCount<2)
		return 0; 

	AirRouteSegInSim *pLastSegmentInSim = (AirRouteSegInSim*)m_vItems.at(nCount-2);
	LogicRunwayInSim *pLogicRunway = pFlight->GetLandingRunway();

	ElapsedTime eTimeLastNodeToRunwayPort= ElapsedTime(0L);

	AirRouteIntersectionInSim* pLastFirstConflictIntersection = pLastSegmentInSim->GetFirstConcernIntersection();

	DistanceUnit touchDist = pFlight->GetTouchDownDistAtRuwnay(pLogicRunway);//->GetPerformance()->getDistThresToTouch(pFlight);
	CPoint2008 runwayPortPoint = pLogicRunway->GetDistancePoint(touchDist);
	CPoint2008 lastSegmentNode = pLastFirstConflictIntersection->getInputPoint()->GetPosition();
	CPoint2008 MissApporachPoint = pLogicRunway->GetMissApproachCheckPoint(lastSegmentNode);
	double dSpeed =  GetRoutePointProperty(pLastFirstConflictIntersection->getInputPoint()).GetSpeed();


	double dDistance = lastSegmentNode.distance(MissApporachPoint) + MissApporachPoint.distance(runwayPortPoint);
	return dDistance;
}

//-------------------------------------------------------------------------------------------------------------------------------
//Summary:
//			calculate time between start and touch down point 
//-----------------------------------------------------------------------------------------------------------------------------
ElapsedTime FlightRouteInSim::CalculateTimeFromLastSegToLogicRunway(AirsideFlightInSim *pFlight)
{
	if (pFlight->GetSTAR() != this)
		return -1L;

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


	double dDistance = lastSegmentNode.distance(MissApporachPoint) + MissApporachPoint.distance(runwayPortPoint);
	double dAvgSpeed = (dSpeed+ pFlight->GetPerformance()->getLandingSpeed(pFlight))/2;
	double dTimeLastNodeToRunwayPort = 	dDistance/dAvgSpeed;


	eTimeLastNodeToRunwayPort = ElapsedTime(dTimeLastNodeToRunwayPort);


	return eTimeLastNodeToRunwayPort;
}

//-------------------------------------------------------------------------------------------------------------------------------
//Summary:
//			calculate time between start and touch down point  use max speed
//-----------------------------------------------------------------------------------------------------------------------------
ElapsedTime FlightRouteInSim::CalculateMinTimeFromLastSegToLogicRunway(AirsideFlightInSim *pFlight)
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


AirWayPointInSim* FlightRouteInSim::GetLastHoldConnectedWaypoint()
{
	int nCount = m_vItems.size();
	AirWayPointInSim* pWaypoint = NULL;

	for (int i = nCount -1; i >= 0; i--)
	{
		AirsideResource* pResource = m_vItems.at(i);

		if (pResource->GetType() == AirsideResource::ResType_AirRouteSegment)
		{

			AirRouteIntersectionInSim* pIntersection = ((AirRouteSegInSim*)pResource)->GetSecondConcernIntersection();
			if ( pIntersection && pIntersection->getInputPoint()->GetType() == AirsideResource::ResType_WayPoint 
				&& ((AirWayPointInSim*)pIntersection->getInputPoint())->GetHold())
			{
				return (AirWayPointInSim*)pIntersection->getInputPoint();
			}
			if (i == 0)		//to get first waypoint of route
			{
				pWaypoint = (AirWayPointInSim*)((AirRouteSegInSim*)pResource)->GetFirstConcernIntersection()->getInputPoint();
			}
		}
		if (pResource->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			AirWayPointInSim* pWaypoint = (AirWayPointInSim*)((HeadingAirRouteSegInSim*)pResource)->GetSecondIntersection()->getInputPoint();
			if (pWaypoint->GetHold())
			{
				return pWaypoint;
			}
		}
	}

	return pWaypoint;
}

//-------------------------------------------------------------------------------------
//Summary:
//			retrieve time between tow intersection node
//---------------------------------------------------------------------------------------
ElapsedTime FlightRouteInSim::GetFlightTimeInSeg(AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg)
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

LogicRunwayInSim* FlightRouteInSim::GetRelateRunway()
{

	int nSize = m_vItems.size();
	for (int i = 0; i < nSize; i++)
	{
		if (m_vItems[i]->GetType() == AirsideResource::ResType_LogicRunway)
			return (LogicRunwayInSim*)m_vItems[i];
	}

	return NULL;
}

void FlightRouteInSim::SetRelateRunway(LogicRunwayInSim* pLogicRunway)
{
	if (m_vItems.empty())
		m_vItems.push_back(pLogicRunway);

	int nSize = m_vItems.size();
	for (int i = 0; i < nSize; i++)
	{
		if (m_vItems[i]->GetType() == AirsideResource::ResType_LogicRunway)
			m_vItems[i] = pLogicRunway;
	}

}

AirEntrySystemHoldInInSim* FlightRouteInSim::GetEntrySystemHold()
{
	if(GetItemCount())
	{
		AirsideResource* pStartSeg = GetItem(0);
		if (pStartSeg->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			AirRouteIntersectionInSim* pIntersection  = ((AirRouteSegInSim*)pStartSeg)->GetFirstConcernIntersection();
			if(pIntersection && pIntersection->GetEntrySysHold() )
			{
				return pIntersection->GetEntrySysHold();
			}
			else if(pIntersection)
			{
				DistanceUnit dAlt = 0.5*(pIntersection->getInputPoint()->GetAltHigh() + pIntersection->getInputPoint()->GetAltLow() );
				

				CPoint2008 pt1 = pIntersection->getInputPoint()->GetPosition() + CPoint2008(0,0,dAlt);


				AirRouteIntersectionInSim* pSecInt = ((AirRouteSegInSim*)pStartSeg)->GetSecondConcernIntersection();
				ARCVector3 vDir(10,0,0);
				if(pSecInt)
				{
					vDir = pSecInt->getInputPoint()->GetPosition() - pIntersection->getInputPoint()->GetPosition();
					vDir.SetLength(10);
				}
				else if(GetRelateRunway())
				{
					vDir = GetRelateRunway()->GetDistancePoint(0) - pIntersection->getInputPoint()->GetPosition();
					vDir.SetLength(10);
				}
				AirEntrySystemHoldInInSim * pnewEntrSysHold = new AirEntrySystemHoldInInSim( vDir, pt1 );
				pIntersection->SetEntrySystemHold(pnewEntrSysHold);
				return pnewEntrSysHold;
			}
		}
	}
	return NULL;
}

void FlightRouteInSim::SetAirRouteInSim( AirRouteInSim *pAirRouteInSim )
{
	ASSERT(pAirRouteInSim != NULL);
	m_pAirRouteInSim = pAirRouteInSim;
}

AirRouteInSim * FlightRouteInSim::GetAirRouteInSim()
{
	return m_pAirRouteInSim;
}

bool FlightRouteInSim::IsPointAtFlightRouteToRwyPhase( AirsideFlightInSim* pFlight, const CPoint2008& point )
{
	LogicRunwayInSim* pPort1 = NULL;
	AirRoutePointInSim* pLastWP1 = m_pAirRouteInSim->GetRwyConnectedRoutePoint();

	if (pFlight->IsArrivingOperation())
		pPort1 = pFlight->GetLandingRunway();
	else
		pPort1 = pFlight->GetAndAssignTakeoffRunway()->GetOtherPort();

	if (point.within(pPort1->GetDistancePoint(0),pLastWP1->GetPosition()) >0)
		return true;

	return false;
}

void FlightRouteInSim::AddItem( AirsideResource* pItem )
{
	if (m_vItems.empty())
	{
		m_vItems.push_back(pItem);
		return;
	}

	if (m_vItems.at(GetItemCount() -1) == pItem)	//repeated
		return;

	if (std::find(m_vItems.begin(),m_vItems.end(),pItem) != m_vItems.end())	//already exist, error
	{
		ASSERT(0);
	}

	m_vItems.push_back(pItem);
}

//---------------------------------------------------------------------------------------------------------------
//Summary:
//			retrieve time flight takeoff from runway point to SID 
//----------------------------------------------------------------------------------------------------------------
ElapsedTime FlightRouteInSim::CalculateTakeoffTimeFromRwyToFirstSeg( AirsideFlightInSim *pFlight )
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

ElapsedTime FlightRouteInSim::GetHeadingMinTimeInSeg( AirsideFlightInSim *pFlight, HeadingAirRouteSegInSim*pSeg, HeadingAirRouteSegInSim*pNextSeg )
{
	double dSped = GetRoutePointProperty(pSeg->GetFirstIntersection()->getInputPoint()).GetSpeed();
	double dSped2 = GetRoutePointProperty(pSeg->GetSecondIntersection()->getInputPoint()).GetSpeed();

	return pSeg->GetMinTimeInSegment(pFlight,dSped,dSped2,pNextSeg);

}

ElapsedTime FlightRouteInSim::GetHeadingMaxTimeInSeg( AirsideFlightInSim *pFlight, HeadingAirRouteSegInSim*pSeg, HeadingAirRouteSegInSim*pNextSeg )
{
	ASSERT(0);
	return ElapsedTime(0L);
}


///////////////////////////////////////////AirRouteInSim///////////////////////////////////
AirRouteInSim::AirRouteInSim(CAirRoute* pRoute)
{
	m_pRouteInput = pRoute;
	m_nID = m_pRouteInput->getID();
}
AirRouteInSim::~AirRouteInSim()
{
	m_pRouteInput = NULL;
}

void AirRouteInSim::AddRoutePoint(int Idx,AirRoutePointInSim* pPoint)
{
	m_RoutePointList.InsertPoint(Idx,pPoint);
}

AirRoutePointInSim* AirRouteInSim::GetRoutePointByIdx(int nIdx)
{
	if (nIdx <0 ||nIdx > GetRoutePointCount()-1)
		return NULL;

	return m_RoutePointList.GetRoutePointByIdx(nIdx);
}

void AirRouteInSim::AddSegment(AirsideResource* pSeg)
{
	int nSize = GetRouteSegCount();
	for (int i = nSize -1; i >=0; i--)
	{
		if (GetSeg(i) == pSeg)
			return;
	}

	m_vRouteSegList.push_back(pSeg);
}

int AirRouteInSim::GetRoutePointIdx(AirRoutePointInSim* pPoint)
{
	if (pPoint == NULL)
		return -1;

	int nCount = m_RoutePointList.GetRoutePointCount();
	for (int i =0; i < nCount; i++)
	{
		if (m_RoutePointList.GetRoutePointByIdx(i) == pPoint)
		{
			return i;
		}
	}
	return -1;
}

int AirRouteInSim::GetRoutePointIdx(ARWaypoint* pWaypoint)
{
	int nCount = m_RoutePointList.GetRoutePointCount();
	for (int i =0; i < nCount; i++ )
	{
		AirRoutePointInSim* pPoint = m_RoutePointList.GetRoutePointByIdx(i);
		if (pPoint->GetType() == AirsideResource::ResType_WayPoint && pPoint->getID() == pWaypoint->getWaypoint().getID())
			return i;
	}
	return -1;
}

AirsideResourceList AirRouteInSim::GetRouteSegmentList(AirRoutePointInSim* pFirstIntersection, AirRoutePointInSim* pSecondIntersection)
{
// two proper types: 1.first point is intersection and second point is intersection too
//					 2.first point is intersection and without second point 


	AirsideResourceList resourceList;
	resourceList.clear();

	if (pFirstIntersection == NULL || pSecondIntersection == NULL)
		return resourceList;

	int nCount = GetRouteSegCount();
	bool bStart = false;
	for (int i = 0; i < nCount; i++)
	{
		AirsideResource* pSeg = GetSeg(i);
		if (pSeg->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			if (((AirRouteSegInSim*)pSeg)->GetFirstConcernIntersection()->getInputPoint() == pFirstIntersection)
				bStart = true;
		}

		if (!bStart)	//if the point(not first point) is not intersection need be skipped
			continue;

		if (pSeg->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			if (((AirRouteSegInSim*)pSeg)->GetFirstConcernIntersection()->getInputPoint() == pSecondIntersection)
			{
				if (((AirRouteSegInSim*)pSeg)->GetSecondConcernIntersection())
					return resourceList;

				resourceList.push_back(pSeg);
				return resourceList;
			}
		}

		if (pSeg->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			if (((HeadingAirRouteSegInSim*)pSeg)->GetFirstIntersection()->getInputPoint() == pSecondIntersection)
				return resourceList;
		}

		resourceList.push_back(pSeg);	
	}
	return resourceList;


	//int nStart = GetRoutePointIdx(pFirstIntersection);
	//int nSecond = GetRoutePointIdx(pSecondIntersection);

	//AirRoutePointInSim* pStart = pFirstIntersection;
	//AirRoutePointInSim* pSecond = NULL;;
	//for (int i = nStart; i <= nSecond; i++)
	//{
	//	if(i <0)
	//		continue;

	//	if (i >0 && i < GetRoutePointCount()-1 && GetRoutePointByIdx(i)->IsIntersection() == false)	//if the point(not first point) is not intersection need be skipped
	//		continue;
	//	

	//	if (pStart && GetRoutePointByIdx(i) != pStart)
	//		pSecond = GetRoutePointByIdx(i);

	//	if (pStart && pSecond)
	//	{
	//		AirsideResource* pSeg = GetRouteSegment(pStart,pSecond);
	//		if (pSeg)
	//			resourceList.push_back(pSeg);
	//	}

	//	if (pSecond)
	//		pStart = pSecond;
	//}

	//if (nSecond >=0 && GetRoutePointByIdx(nSecond)->IsIntersection() == false)
	//{
	//	pSecond = pSecondIntersection;
	//	if (pStart && pSecond)
	//	{
	//		AirsideResource* pSeg = GetRouteSegment(pStart,pSecond);
	//		if (pSeg)
	//			resourceList.push_back(pSeg);
	//	}
	//}

	//if (nSecond == GetRoutePointCount()-1)
	//{
	//	pStart = GetRoutePointByIdx(nSecond);
	//	AirsideResource* pSeg = GetRouteSegment(pStart,NULL);
	//	if (pSeg)
	//		resourceList.push_back(pSeg);
	//}

	//return resourceList;
}

AirsideResourceList AirRouteInSim::GetAirRouteFromIntersectionPoint(AirRoutePointInSim* pFirstIntersection)
{
	AirsideResourceList resourceList;
	resourceList.clear();

	if (pFirstIntersection == NULL || pFirstIntersection->IsIntersection() == false)
		return resourceList;

	int nCount = GetRouteSegCount();
	bool bStart = false;
	for (int i = 0; i < nCount; i++)
	{
		AirsideResource* pSeg = GetSeg(i);
		if (pSeg->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			if (((AirRouteSegInSim*)pSeg)->GetFirstConcernIntersection()->getInputPoint() == pFirstIntersection)
				bStart = true;
		}

		if (!bStart)	//if the point(not first point) is not intersection need be skipped
			continue;

		resourceList.push_back(pSeg);	
	}
	return resourceList;
}

//AirsideResource* AirRouteInSim::GetRouteSegment(AirRouteIntersectionInSim* pFirstInt,AirRouteIntersectionInSim* pSecondInt)
//{
//	for (int i = 0; i < (int)m_vRouteSegList.size(); i++)
//	{
//		if (m_vRouteSegList[i]->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
//		{
//			HeadingAirRouteSegInSim* pSeg = (HeadingAirRouteSegInSim*)m_vRouteSegList[i];
//			if (pSeg->GetFirstIntersection() == pFirstInt && pSeg->GetSecondIntersection() == pSecondInt )
//				return pSeg;
//		}
//
//		if (m_vRouteSegList[i]->GetType() == AirsideResource::ResType_AirRouteSegment)
//		{
//			AirRouteSegInSim* pSeg = (AirRouteSegInSim*)m_vRouteSegList[i];
//			if (pSeg->GetFirstConcernIntersection() == pFirstInt)
//			{
//				if (pSeg->GetSecondConcernIntersection() && pSeg->GetSecondConcernIntersection() == pSecondInt)
//					return pSeg;
//
//				if (pSeg->GetSecondConcernIntersection() == NULL)
//					return pSeg;
//			}
//		}
//	}
//	return NULL;
//}

AirRoutePointInSim* AirRouteInSim::GetRwyConnectedRoutePoint()
{
	int nCount = m_RoutePointList.GetRoutePointCount();

	if (m_pRouteInput->getRouteType() == CAirRoute::SID || m_pRouteInput->getRouteType() == CAirRoute::CIRCUIT)
		return m_RoutePointList.GetRoutePointByIdx(0);
	else
		return m_RoutePointList.GetRoutePointByIdx(nCount -1);

}

CAirRoute::RouteType AirRouteInSim::getRouteType() const
{
	return m_pRouteInput->getRouteType();
}