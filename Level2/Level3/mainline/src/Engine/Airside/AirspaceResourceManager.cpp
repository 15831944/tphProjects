//--------------------------------------------------------------------------------------------------------------------------
//						Creator:		Emily(ARC)
//						Author:			Sky.wen(ARC)
//						Date:			Sep 22, 2012
//						Time:			1:32PM
//						Purpose:		the method doesn't have comment that make other person understand. 
//											So need give some comment to the mothod
//----------------------------------------------------------------------------------------------------------------------------
#include "StdAfx.h"
#include ".\AirspaceResourceManager.h"
#include "../../InputAirside/ALT_BIN.h"
#include "../../InputAirside/ALTAirspace.h"
#include "../../InputAirside/AirWayPoint.h"
#include "AirsideResourceManager.h"
#include "../../Common/ARCUnit.h"
#include "../../InputAirside/AirRoute.h"
#include "../../InputAirside/AirRouteIntersection.h"
#include "AirRoutePointInSim.h"

#include "Clearance.h"
#include "AirsideFlightInSim.h"
#include "AirTrafficController.h"
#include "FlightPerformanceManager.h"
#include "FlightRunwayTakeOffEvent.h"
#include "FlightConflictAtWaypointDelay.h"
#include "../../Results/OutputAirside.h"
#include "../../Common/EngineDiagnoseEx.h"
#include "../../Common/ARCExceptionEx.h"
#include "../../Common/ARCMathCommon.h"
#include "SimulationErrorShow.h"
#include "ArrivalDelayTriggersDelay.h"
#include "FlightConflictAtWaypointDelay.h"
#include ".\AirHoldInInSim.h"
#include ".\FlightGetClearanceEvent.h"
#include "InboundStatus.h"
#include "TakeoffQueueStatus.h"
#include "RunwayOccupancyStatus.h"
#include "NotifyRunwayWaveCrossEvent.h"
#include "ReleaseResourceLockEvent.h"
#include "../../Results/AirsideFllightRunwayDelayLog.h"
#include "../../Common/DynamicMovement.h"
#include "../MobileDynamics.h"
#include "AirsideFlightRunwayDelay.h"
#include "FlightInHoldEvent.h"
#include "FlightPlanSpecificRouteInSim.h"

#define _DEBUGLOG 0


AirRouteNetworkInSim::AirRouteNetworkInSim(void)
{
	m_vAirLaneInSimList.clear();
	m_vHoldInSimList.clear();
}

AirRouteNetworkInSim::~AirRouteNetworkInSim( void )
{
	int nCount = m_vConcernIntersectionList.GetCount();
	for (int i =0; i < nCount; i++)
	{
		AirRouteIntersectionInSim* pIntersection = m_vConcernIntersectionList.GetIntersection(i);
		delete pIntersection;
		pIntersection = NULL;
	}

	nCount = m_vHeadingRouteSegs.GetCount();
	for (int i =0; i < nCount; i++)
	{
		HeadingAirRouteSegInSim* pSeg = m_vHeadingRouteSegs.GetSegment(i);
		delete pSeg;
		pSeg = NULL;
	}
	
	nCount = int(m_vAirLaneInSimList.size());
	for (int i = 0; i < nCount; i++)
	{
		AirRouteInSim* pRoute = m_vAirLaneInSimList[i];
		delete pRoute;
		pRoute = NULL;
	}
	m_vAirLaneInSimList.clear();

	nCount = m_vHoldInSimList.size();
	for (int i = 0; i < nCount; i++)
	{
		AirspaceHoldInSim* pHold = m_vHoldInSimList.at(i);
		delete pHold;
		pHold = NULL;
	}
	m_vHoldInSimList.clear();
}

bool AirRouteNetworkInSim::Init( int nPrjID , AirsideResourceManager * pAirsideRes )
{
	AirportResourceManager* pAirport = pAirsideRes->GetAirportResource();

	m_vRouteList.ReadData(nPrjID);

	int nCount = m_vRouteList.GetAirRouteCount();

	for (int m =0; m < nCount; m ++)		// init air waypoint and air route
	{
		CAirRoute* pRoute = m_vRouteList.GetAirRoute(m);
		int nPointCount = pRoute->GetWayPointCount();
		AirRouteInSim* pRouteInSim = new AirRouteInSim(pRoute);
		for(int n =0; n < nPointCount; n++)
		{
			AirWayPointInSim* pWaypoint = m_vRoutePoints.GetAirWaypointInSim(pRoute->GetWayPointByIdx(n));
			if (pWaypoint)
			{
				pWaypoint->AddIntersectionRoute(pRoute);
			}
			else
			{
				pWaypoint= new AirWayPointInSim(pRoute->GetWayPointByIdx(n)->getWaypoint().getID(),pAirport->GetAirportInfo());
				pWaypoint->AddIntersectionRoute(pRoute);
				m_vRoutePoints.Add(pWaypoint);
			}
			pRouteInSim->AddRoutePoint(n,pWaypoint);
	
		}
		m_vAirLaneInSimList.push_back(pRouteInSim);		
	}

	AirRouteIntersectionList* pIntersectionList = m_vRouteList.GetAirRouteIntersectionList();
	nCount = pIntersectionList->GetAllAirRouteIntersectionCount();

	for (int j = 0; j < nCount; j++)		// init intersections in airspace
	{
		m_vRoutePoints.Add(new AirspaceIntersectionInSim(pIntersectionList->GetAirRouteIntersectionByIdx(j)));
	}

	nCount = (int)m_vAirLaneInSimList.size();
	for (int i = 0; i< nCount; i++)				//init each air route's intersections
	{
		AirRouteInSim* pRouteInSim = m_vAirLaneInSimList[i];
		CAirRoute* pRoute = pRouteInSim->GetAirRouteInput();

		std::vector<CAirRouteIntersection*> vIntersection = pIntersectionList->GetAirRouteIntersection(pRoute);

		int nSize = (int)vIntersection.size();
		for (int k =0; k < nSize; k++)
		{
			AirspaceIntersectionInSim* pIntersectionInSim = m_vRoutePoints.GetRouteIntersectionInSim(vIntersection[k]);
			IntersectedRouteSegment seg = vIntersection[k]->getIntersectedRouteSeg(pRoute);
			int nForeWaypointID = seg.getWaypoint1ID();
			int nAfterWaypointID = seg.getWaypoint2ID();
			int nFrontIdx = pRouteInSim->GetWaypointIdxByID(nForeWaypointID);
			int nAfterIdx = pRouteInSim->GetWaypointIdxByID(nAfterWaypointID);

			if (nFrontIdx >=0 && nAfterIdx >0)
			{	
				pIntersectionInSim->AddIntersectionRoute(pRoute);
				if (nFrontIdx >= 0 && nFrontIdx == nAfterIdx-1)		//only one intersection in seg
					pRouteInSim->AddRoutePoint(nFrontIdx+1,pIntersectionInSim);
				else																		//more than one intersections in seg
				{
					CPoint2008 WPpos = pRouteInSim->GetRoutePointByIdx(nFrontIdx)->GetPosition();
					double dist = pIntersectionInSim->GetPosition().distance(WPpos);

					for (int m = nFrontIdx+1; m <= nAfterIdx; m++)
					{
						AirRoutePointInSim* pPoint =pRouteInSim->GetRoutePointByIdx(m);
						double DistToWP = pPoint->GetPosition().distance(WPpos);
						if (DistToWP > dist)
						{
							pRouteInSim->AddRoutePoint(m,pIntersectionInSim);
							break;
						}
					}
				}
			}

		}
	}
	InitAirspaceHold(nPrjID, pAirport->GetAirportInfo().GetMagnectVariation().GetRealVariation());	
	InitRelations();

	int nHold = m_vHoldInSimList.size();

	for (int i = 0; i < nHold; i++)
	{
		AirspaceHoldInSim* pHoldInSim = m_vHoldInSimList.at(i);
		AirWayPointInSim* pWaypoint  = pHoldInSim->GetWaypointInSim();
		double dDist = 0;

		int nCount = m_vRoutSegs.GetCount();
		for (int j = 0; j < nCount; j++)
		{
			AirRouteSegInSim* pSeg = m_vRoutSegs.GetSegment(j);
			if (pSeg->GetSecondConcernIntersection() && pSeg->GetSecondConcernIntersection()->getInputPoint() == pWaypoint)
			{
				double dDistToHold = pSeg->GetDistLastWaypointToHold();
				if ((dDist > 0 && dDist > dDistToHold )|| dDist <=0)
				{
					dDist = dDistToHold;
				}
			}
		}
		double dAlt = (pWaypoint->getAirWaypointInput()->GetHighLimit() + pWaypoint->getAirWaypointInput()->GetLowLimit())/2.0;
		double dDistToPoint = abs(pHoldInSim->GetHoldInput()->getMaxAltitude() - dAlt)/tan(ARCMath::DegreesToRadians(3.0));

		pHoldInSim->m_dATCManagedDist =  dDist < dDistToPoint? dDist: dDistToPoint;
	}

	return true;
}

void AirRouteNetworkInSim::InitAirspaceHold(int nAirspaceID, double dAirportVar)
{
	std::vector<int> vIDList;
	vIDList.clear();
	ALTAirspace::GetHoldList(nAirspaceID,vIDList);
	int nCount = vIDList.size();
	for (int i =0; i < nCount; i++)
	{
		int nID = vIDList.at(i);
		ALTObject *pALTObj = ALTObject::ReadObjectByID(nID);
		if (pALTObj->GetType() == ALT_HOLD)
		{
			int nWaypointId = ((AirHold*)pALTObj)->GetWatpoint();
			AirWayPointInSim* pWaypoint  = GetAirRoutePointList().GetAirWaypointInSimByID(nWaypointId);

			if(pWaypoint == NULL)		//the waypoint unused in any air route, also neglect the hold
			{
				delete pALTObj;
				pALTObj = NULL;
				continue;
			}

			AirspaceHoldInSim* pHoldInSim = new AirspaceHoldInSim((AirHold*)pALTObj,this, pWaypoint, dAirportVar);
			AirWayPointInSim* pRelateWaypoint = pHoldInSim->GetWaypointInSim();
			pRelateWaypoint->SetHold(pHoldInSim);
			m_vHoldInSimList.push_back(pHoldInSim);
		}
		else
		{
			delete pALTObj;
			pALTObj = NULL;
		}
	}

}


bool AirRouteNetworkInSim::InitRelations()		// init air route segment, heading air route and conflict concern intersection of each air route
{
	int nCount = (int)m_vAirLaneInSimList.size();

	AirRouteIntersectionInSim* pIntersection = NULL;
	AirRouteIntersectionInSim* pSecondIntersection = NULL;

	for (int i =0; i < nCount; i++)
	{
		AirRouteInSim* pRouteInSim = m_vAirLaneInSimList[i];
		CAirRoute* pRoute = pRouteInSim->GetAirRouteInput();

		int nWaypointCount = pRoute->GetWayPointCount();

		AirRouteSegInSim* pSeg = NULL;


		if (nWaypointCount == 1)
		{
			pSeg = new AirRouteSegInSim;
			AirRoutePointInSim* pWaypoint = pRouteInSim->GetRoutePointByIdx(nWaypointCount - 1);
			pIntersection = m_vConcernIntersectionList.GetIntersection(pWaypoint);
			if (pIntersection == NULL)
			{
				pIntersection = new AirRouteIntersectionInSim(pWaypoint);
				m_vConcernIntersectionList.Add(pIntersection);
			}
			pSeg->SetFirstIntersection(pIntersection);

			AirRouteSegInSim* pSegInList = m_vRoutSegs.GetSegmentInList(pSeg);	
			if (pSegInList)			
				pRouteInSim->AddSegment(pSegInList);
			else
				pRouteInSim->AddSegment(pSeg);
			
		}
		
		int IdX = nWaypointCount -1;
		for (int j = 0; j <= IdX-1; j++)
		{
			ARWaypoint* pARWaypoint = pRoute->GetWayPointByIdx(j);
			ARWaypoint* pARWaypoint2 = pRoute->GetWayPointByIdx(j+1);

			int nIdx1 = pRouteInSim->GetRoutePointIdx(pARWaypoint);
			int nIdx2 = pRouteInSim->GetRoutePointIdx(pARWaypoint2);

			AirRoutePointInSim* pWaypoint = pRouteInSim->GetRoutePointByIdx(nIdx1);

			if (nIdx1 >=0 && nIdx2 >0)
			{				
				if (pARWaypoint->getDepartType() == ARWaypoint::Heading)
				{

					AirRoutePointInSim* pWaypoint2 = pRouteInSim->GetRoutePointByIdx(nIdx2);
					/*pIntersection = m_vConcernIntersectionList.GetIntersection(pWaypoint);*/
					if (pIntersection == NULL)
					{
						pIntersection = new AirRouteIntersectionInSim(pWaypoint);
						m_vConcernIntersectionList.Add(pIntersection);
					}

					pSecondIntersection = m_vConcernIntersectionList.GetIntersection(pWaypoint2);
					if (pSecondIntersection == NULL)
					{
						pSecondIntersection = new AirRouteIntersectionInSim(pWaypoint2);
						m_vConcernIntersectionList.Add(pSecondIntersection);
					}

					
					HeadingAirRouteSegInSim* pHeadingSeg = GetHeadingSegmentInList(pIntersection, pSecondIntersection);
					if (pHeadingSeg)
					{
						pRouteInSim->AddSegment(pHeadingSeg);
					}
					else
					{
						pHeadingSeg = new HeadingAirRouteSegInSim(pIntersection,pSecondIntersection);
						pHeadingSeg->SetWPExtentPoint(pARWaypoint->getExtentPoint());

						if (pARWaypoint->getHeadingType() == ARWaypoint::Aligned)
							pHeadingSeg->SetHeadingType(HeadingAirRouteSegInSim::Aligned);
						else
							pHeadingSeg->SetHeadingType(HeadingAirRouteSegInSim::Direct);

						pRouteInSim->AddSegment(pHeadingSeg);
						m_vHeadingRouteSegs.Add(pHeadingSeg);

					}

					pIntersection = pSecondIntersection;
					pSecondIntersection = NULL;

					pSeg = new AirRouteSegInSim;
					pSeg->SetFirstIntersection(pIntersection);
					if (j== IdX -1)	//last waypoint
					{
						AirRouteSegRegular(pSeg);
						AirRouteSegInSim* pSegInList = m_vRoutSegs.GetSegmentInList(pSeg);	
						if (pSegInList)			
							pRouteInSim->AddSegment(pSegInList);
						else
							pRouteInSim->AddSegment(pSeg);
					}

				}
				else
				{
					if( nIdx1 == 0 )			//first route point is assumed as intersection
					{
						pSeg = new AirRouteSegInSim;

						pIntersection = m_vConcernIntersectionList.GetIntersection(pWaypoint);
						if (pIntersection == NULL)
						{
							pIntersection = new AirRouteIntersectionInSim(pWaypoint);
							m_vConcernIntersectionList.Add(pIntersection);
						}
						pSeg->SetFirstIntersection(pIntersection);

					}

					for (int n = nIdx1+1; n <= nIdx2; n++)
					{

						AirRoutePointInSim* pPoint = pRouteInSim->GetRoutePointByIdx(n);
						if (pPoint->IsIntersection()||
							(pPoint->GetType() == AirsideResource::ResType_WayPoint && ((AirWayPointInSim*)pPoint)->GetHold()))
						{
							{
								pSecondIntersection = m_vConcernIntersectionList.GetIntersection(pPoint);
								if (pSecondIntersection == NULL)
								{
									pSecondIntersection = new AirRouteIntersectionInSim(pPoint);
									m_vConcernIntersectionList.Add(pSecondIntersection);
								}
								pSeg->SetSecondIntersection(pSecondIntersection);
								pIntersection = pSecondIntersection;
							}

							AirRouteSegRegular(pSeg);
							AirRouteSegInSim* pSegInList = m_vRoutSegs.GetSegmentInList(pSeg);	
							if (pSegInList)			
								pRouteInSim->AddSegment(pSegInList);
							else
								pRouteInSim->AddSegment(pSeg);

							if (j == IdX -1 && n == nIdx2)		//last waypoint
							{
								pSeg = new AirRouteSegInSim;
								pSeg->SetFirstIntersection(pIntersection);

								pSegInList = m_vRoutSegs.GetSegmentInList(pSeg);	
								if (pSegInList)			
									pRouteInSim->AddSegment(pSegInList);
								else
									pRouteInSim->AddSegment(pSeg);

								continue;
							}

							pSeg = new AirRouteSegInSim;
							pSeg->SetFirstIntersection(pIntersection);

						}
						else
						{
							if ( n == nIdx2 && pARWaypoint2->getDepartType() == ARWaypoint::Heading)
							{
								pSecondIntersection = m_vConcernIntersectionList.GetIntersection(pPoint);
								if (pSecondIntersection == NULL)
								{
									pSecondIntersection = new AirRouteIntersectionInSim(pPoint);
									m_vConcernIntersectionList.Add(pSecondIntersection);
								}
								pSeg->SetSecondIntersection(pSecondIntersection);
								pIntersection = pSecondIntersection;

								AirRouteSegInSim* pSegInList = m_vRoutSegs.GetSegmentInList(pSeg);	
								if (pSegInList)			
									pRouteInSim->AddSegment(pSegInList);
								else
									pRouteInSim->AddSegment(pSeg);

								continue;
							}

							pSeg->AddRoutePoint(pPoint);

							if (j == IdX -1 && n == nIdx2)		// last waypoint of air route is not intersection
							{
								AirRouteSegRegular(pSeg);
								AirRouteSegInSim* pSegInList = m_vRoutSegs.GetSegmentInList(pSeg);	
								if (pSegInList)			
									pRouteInSim->AddSegment(pSegInList);
								else
									pRouteInSim->AddSegment(pSeg);

								//the last point of air route assumed as an air route segment
								pSeg = new AirRouteSegInSim;
								pSeg->AddRoutePoint(pPoint);

								AirRouteSegRegular(pSeg);
								pSegInList = m_vRoutSegs.GetSegmentInList(pSeg);	
								if (pSegInList)			
									pRouteInSim->AddSegment(pSegInList);
								else
									pRouteInSim->AddSegment(pSeg);

							}
						}
					}
				}	
			}
		}
	}

	for (int m = 0; m < m_vConcernIntersectionList.GetCount(); m++ )
	{
		AirRouteIntersectionInSim* pIntersection = m_vConcernIntersectionList.GetIntersection(m);
		CalculateWaypointConnectionSegmentList(pIntersection);
	}

	return true;
}

bool AirRouteNetworkInSim::HasSID()
{
	int nCount = m_vRouteList.GetAirRouteCount();
	for (int i = 0; i < nCount; i++)
	{
		if (m_vRouteList.GetAirRoute(i)->getRouteType() == CAirRoute::SID )
		{
			return true;
		}
	}
	return false;
}

bool AirRouteNetworkInSim::HasSTAR()
{
	int nCount = m_vRouteList.GetAirRouteCount();
	for (int i = 0; i < nCount; i++)
	{
		if (m_vRouteList.GetAirRoute(i)->getRouteType() == CAirRoute::STAR )
		{
			return true;
		}
	}
	return false;
}

void AirRouteNetworkInSim::CalculateWaypointConnectionSegmentList(AirRouteIntersectionInSim* pIntersectedWaypoint)
{
	int nCount =  m_vRoutSegs.GetCount();
	AirRouteSegInSim* pSeg = NULL;
	for (int i = 0; i <nCount; i++)
	{
		pSeg = m_vRoutSegs.GetSegment(i);
		if (pSeg->GetSecondConcernIntersection() == pIntersectedWaypoint )
			pIntersectedWaypoint->AddConvergentRouteSeg(pSeg);

		if (pSeg->GetFirstConcernIntersection() == pIntersectedWaypoint )
			pIntersectedWaypoint->AddDivergentRouteSeg(pSeg);
	}
}

AirRouteInSim* AirRouteNetworkInSim::GetAirLaneInSimByID(int nID)
{
	for ( int i = 0; i < int(m_vAirLaneInSimList.size());i++)
	{
		if (m_vAirLaneInSimList[i]->GetID() == nID )
		{
			return m_vAirLaneInSimList[i];
		}
	}
	return NULL;
}

void AirRouteNetworkInSim::AirRouteSegRegular(AirRouteSegInSim* pSeg)
{
	if ( !pSeg->GetFirstConcernIntersection())
	{
		AirRoutePointInSim* pPoint = pSeg->GetRoutePointByIdx(0);
		if (pPoint)
		{
			AirRouteIntersectionInSim* pIntersection = new AirRouteIntersectionInSim(pPoint);
			m_vConcernIntersectionList.Add(pIntersection);
			pSeg->SetFirstIntersection(pIntersection);
			pSeg->DeleteRoutePoint(0);
		}
	}

	if ( !pSeg->GetSecondConcernIntersection())
	{
		int nCount = pSeg->GetRoutePointCount() -1;
		if (nCount < 0)
			return;

		AirRoutePointInSim* pPoint = pSeg->GetRoutePointByIdx(nCount) ;
		AirRouteIntersectionInSim* pIntersection = new AirRouteIntersectionInSim(pPoint);
		m_vConcernIntersectionList.Add(pIntersection);
		pSeg->SetSecondIntersection(pIntersection);
		pSeg->DeleteRoutePoint(nCount);
	}

}

//---------------------------------------------------------------------------------------------------------------------
//Summary:
//				retrieve route point from start to intersection node and miss apprach
//---------------------------------------------------------------------------------------------------------------------
FlightRouteInSim* AirRouteNetworkInSim::GetMisApproachRoute( AirsideFlightInSim* pFlight)
{
	FlightRouteInSim* pArrivalRoute = pFlight->GetSTAR();
	pArrivalRoute->Clear();
	LogicRunwayInSim* pRunway = pFlight->GetLandingRunway();

	AirRouteInSim* pMissApproach = NULL;

	int nRouteCount = (int) m_vAirLaneInSimList.size();
	for(int i=0 ;i< nRouteCount; i++)
	{
		AirRouteInSim* pRouteInSim = m_vAirLaneInSimList.at(i);
		if(pRouteInSim->GetAirRouteInput()->getRouteType() == CAirRoute::MIS_APPROACH  
			&& pRouteInSim->GetAirRouteInput()->IsConnectRunway(pRunway->GetRunwayInSim()->GetRunwayInput()->getID(), pRunway->getLogicRunwayType()))
		{
			pMissApproach = pRouteInSim;
			int nPointCount = pMissApproach->GetRoutePointCount();
			for (int idx =0; idx < nRouteCount; idx++)
			{
				pRouteInSim = m_vAirLaneInSimList.at(idx);
				if (pRouteInSim == pMissApproach || pRouteInSim->GetAirRouteInput()->getRouteType() != CAirRoute::STAR)
					continue;

				for (int j = 0; j < nPointCount; j++)
				{
					if (pMissApproach->GetRoutePointByIdx(j)->IsIntersectionRoute(pRouteInSim) 
						&& pRouteInSim->GetAirRouteInput()->IsConnectRunway(pRunway->GetRunwayInSim()->GetRunwayInput()->getID(), pRunway->getLogicRunwayType()))
					{
						int nSegCount = pMissApproach->GetRouteSegCount();
						for (int k =0; k < nSegCount; k++)
						{
							AirRouteSegInSim* pSeg = (AirRouteSegInSim*)pMissApproach->GetSeg(k);
							if (pSeg->GetSecondConcernIntersection() && pSeg->GetFirstConcernIntersection()->getInputPoint() == pMissApproach->GetRoutePointByIdx(j))
								break;

							AirRoutePointInSim * pPoint = pSeg->GetFirstConcernIntersection()->getInputPoint();
							FlightPlanPropertyInSim property(pPoint);
							property.SetAlt((pPoint->GetAltHigh() + pPoint->GetAltLow())/2.0) ;
							property.SetArrival();
							property.SetSpeed((pFlight->GetMaxSpeed(OnMissApproach)+pFlight->GetMinSpeed(OnMissApproach))/2.0);
							property.SetMode(OnMissApproach);

							FlightPlanPropertiesInSim& planProperties = pArrivalRoute->GetPlanProperties();
							planProperties.push_back(property);
							pArrivalRoute->AddItem(pSeg);
						}

						int nIdx = pRouteInSim->GetRoutePointIdx(pMissApproach->GetRoutePointByIdx(j));
						AirsideResourceList segList = pRouteInSim->GetAirRouteFromIntersectionPoint(pRouteInSim->GetRoutePointByIdx(nIdx));
			
						nSegCount = segList.size();
						for (int k = 0; k < nSegCount; k++)
							pArrivalRoute->AddItem(segList.at(k));

						pArrivalRoute->AddItem(pRunway);
						return pArrivalRoute;						
					}
				}	
			}	
		}
	}

	if (pMissApproach !=NULL)			//no connected star
	{
		CString strError;
		strError.Format( "No STAR connected with Miss Approach Route of Runway: %s", pRunway->PrintResource());
		CString strErrorType = "DEFINE ERROR";
		AirsideSimErrorShown::SimWarning(pFlight,strError,strErrorType);

		int nCount = pMissApproach->GetRouteSegCount();
		for (int k =0; k < nCount; k++)
			pArrivalRoute->AddItem(pMissApproach->GetSeg(k));
	}

	return pArrivalRoute;
}

//-----------------------------------------------------------------------------------------------------------------------------------
//Summary:
//			generate miss approach clearance item  
//------------------------------------------------------------------------------------------------------------------------------------
void AirRouteNetworkInSim::GetMissApproachClearance(AirsideFlightInSim* pFlight,FlightRouteInSim *pAirRouteInSim, ClearanceItem lastItem,Clearance& newClearance)
{
	AirRoutePointInSim* pRoutePoint = ((AirRouteSegInSim*)pAirRouteInSim->GetItem(0))->GetFirstConcernIntersection()->getInputPoint();

	//landing item
	ClearanceItem missItem(pRoutePoint,OnMissApproach,0);		
	missItem.SetSpeed(pFlight->GetPerformance()->getClimboutSpeed(pFlight));
	missItem.SetPosition(pRoutePoint->GetPosition());
	double dAlt = (pRoutePoint->GetAltHigh() + pRoutePoint->GetAltLow())/2.0;
	missItem.SetAltitude(dAlt);

	CPoint2008 touchPoint = pFlight->GetLandingRunway()->GetFlightTouchdownPoint(pFlight);
	CPoint2008 curPos = lastItem.GetPosition();
	curPos.setZ(lastItem.GetAltitude());
	CPoint2008 nextPos = pRoutePoint->GetPosition();
	nextPos.setZ(dAlt);

	double dPathRate = (curPos.distance(touchPoint) + touchPoint.distance(nextPos))/curPos.distance(nextPos);
	double dT = (2.0* curPos.distance(nextPos))/(lastItem.GetSpeed() + missItem.GetSpeed());
	ElapsedTime dTime = lastItem.GetTime() + ElapsedTime(dT*dPathRate);
	missItem.SetTime(dTime);
	newClearance.AddItem(missItem);
}

//apply arrival delay trigger
bool AirRouteNetworkInSim::IsFlightNeedDelayLandingOnRwy( AirsideFlightInSim* pFlight,const ClearanceItem& lastItem, ElapsedTime& tDelayTime )
{
	ArrivalDelayTriggerInSim* pdelayTrigger = pFlight->GetAirTrafficController()->GetArrivalDelayTrigger();
	CString strReason = "";
	ElapsedTime tDelay = pdelayTrigger->GetDelayTime(pFlight,strReason);

	ClearanceItem CurItem = lastItem;
	LogicRunwayInSim* pLogicRunway = pFlight->GetLandingRunway();
	if (tDelay > 0L )	//delay at last hold because arrival delay trigger
	{
		tDelayTime = tDelay;
		ArrivalDelayTriggersDelay* pDelay = new ArrivalDelayTriggersDelay;
		pDelay->SetFlightID(pFlight->GetUID());
		pDelay->SetStartTime(lastItem.GetTime());

		pDelay->SetReasonDescription(strReason);
		pDelay->SetDelayResult(FltDelayReason_AirHold);
		pFlight->SetDelayed(pDelay);

		//runway delay
		ElapsedTime tAvailableTime = lastItem.GetTime()+ tDelayTime;
		//ElapsedTime tDelay = tDelayTime;
		ResourceDesc resDesc;
		pLogicRunway->getDesc(resDesc);
		AirsideFlightRunwayDelayLog* pLog = new AirsideFlightRunwayDelayLog(resDesc, lastItem.GetTime().asSeconds(), OnLanding, tAvailableTime.asSeconds(), tDelay.asSeconds(), FlightRunwayDelay::LandingRoll);
		CString strDetail;
		strDetail.Format("%d seconds arrival delay triggered", tDelayTime.asSeconds());
		pLog->sReasonDetail = strDetail.GetString();
		pFlight->LogEventItem(pLog);
		
		// hold waiting
		//TakeoffQueueStatus* pStatus = pFlight->GetAirTrafficController()->GetTakeoffQueueStatus();
		//pFlight->GetWakeUpCaller().OberverSubject(pStatus);
		FlightWakeupEvent* pEvent = new FlightWakeupEvent(pFlight);
		pEvent->setTime(pFlight->GetTime() + tDelayTime);
		pEvent->addEvent();

		pFlight->StartDelay(CurItem, NULL, FlightConflict::HOLD_AIR, FlightConflict::RUNWAY,FltDelayReason_AirHold,strDetail);	//holding delay
		return true;

	}
	pFlight->SetDelayed(NULL);

	if (pLogicRunway->HasHasMissedApproachFlight())		//if have frontal flight missed approach, waiting in hold
	{
		pFlight->GetWakeUpCaller().OberverSubject(pLogicRunway);
		pFlight->StartDelay(CurItem, NULL, FlightConflict::HOLD_AIR, FlightConflict::RUNWAY,FltDelayReason_AirHold,_T("Missed approach"));	//holding delay
		return true;
	}

	return false;
}

//to check whether the next hold is available and whether there are flights holding in the hold already
bool AirRouteNetworkInSim::CheckIsFlightNeedWaitingInHold(AirsideFlightInSim* pFlight, AirsideResourceList nextResList,ClearanceItem lastItem)
{	
	AirsideResource* pCurRes = pFlight->GetResource();
	if ( pCurRes!= NULL )		//judge whether flight is at waypoint which connected with hold
	{		
		AirspaceHoldInSim* pHold = NULL;	
		if (pCurRes->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			pHold = ((AirRouteSegInSim*)pCurRes)->GetConnectedHold();
			if (pHold && pFlight->GetPosition() != pHold->GetWaypointInSim()->GetPosition())		//flight is not at hold position
				return false;
		}
		else if (pCurRes->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
			pHold = ((HeadingAirRouteSegInSim*)pCurRes)->GetConnectedHold();
		else if (pCurRes->GetType() == AirsideResource::ResType_WayPoint)
			pHold = ((AirWayPointInSim*)pCurRes)->GetHold();

		if (pHold && pHold->GetHeadFlightCount(pFlight) >0)
		{
			pFlight->GetWakeUpCaller().OberverSubject(pHold);
			pFlight->StartDelay(lastItem, NULL, FlightConflict::HOLD_AIR, FlightConflict::BETWEENWAYPOINTS,FltDelayReason_AirHold,_T("Holding delay"));		//holding delay
			pFlight->WaitingInHold();
			return true;
		}
	
	}

	int nSize = nextResList.size();
	AirsideResource* pRes = nextResList[nSize -1];
	if (pRes->GetType() == AirsideResource::ResType_LogicRunway && pFlight->IsArrivingOperation() && pFlight->GetMode() != OnFinalApproach)		//last segment with hold of STAR
	{
		ElapsedTime tDelay = -1L;
		if (IsFlightNeedDelayLandingOnRwy(pFlight,lastItem,tDelay))
			return true;
	}
	else
	{
		AirspaceHoldInSim* pNextHold = NULL;
		if (pRes->GetType() == AirsideResource::ResType_AirRouteSegment && ((AirRouteSegInSim*)pRes)->GetSecondConcernIntersection())
			pNextHold = ((AirRouteSegInSim*)pRes)->GetSecondConcernIntersection()->GetConnectHold();

		if (pNextHold)
		{
			if (!pNextHold->GetLock(pFlight))
			{
				pFlight->GetWakeUpCaller().OberverSubject(pNextHold);
				pFlight->StartDelay(lastItem,NULL,FlightConflict::HOLD_AIR, FlightConflict::BETWEENWAYPOINTS,FltDelayReason_AirHold,_T("Holding delay"));	//holding delay
				return true;
			}
		}
	}

	return false;
}

//to check whether exist conflicts to next hold
bool AirRouteNetworkInSim::IsFlightNeedHolding(AirsideFlightInSim* pFlight, AirspaceHoldInSim* pHold,ClearanceItem& lastItem,Clearance& newClearance)
{
	ASSERT(pHold);		//error

	FlightRouteInSim* pRoute = NULL;
	if (pFlight->IsArrivingOperation())
		pRoute = pFlight->GetSTAR();
	else
		pRoute = pFlight->GetSID();

	AirsideResourceList resList;

	pRoute->GetConcernRouteSegs(pHold,resList);

	int nSize = resList.size();
	AirsideResource* pRes = resList[nSize -1];
	if (pRes->GetType() == AirsideResource::ResType_LogicRunway && pFlight->IsArrivingOperation())		//last segment with hold of STAR
	{
		ElapsedTime tDelay = -1L;
		if (IsFlightNeedDelayLandingOnRwy(pFlight,lastItem,tDelay))
		{
			ElapsedTime tHoldingTime = lastItem.GetTime();
			if(tDelay > 0L)		//if need delay, waiting a delay time in hold
				tHoldingTime = tDelay + lastItem.GetTime();

			FlightInHoldEvent* pEvent = new FlightInHoldEvent(pFlight, pHold, tHoldingTime);
			pEvent->setTime(lastItem.GetTime());
			pEvent->addEvent();	

			return false;
		}

	}

	return 	FlightPrearrangeOnAirRouteExcepteTakeoffPart(pFlight,pRoute,resList,lastItem,newClearance);

}


bool AirRouteNetworkInSim::FindClearanceInConcern(AirsideFlightInSim * pFlight,FlightRouteInSim* pFlightRoute, ClearanceItem& lastItem, Clearance& newClearance )
{
	//ASSERT(pFlight->GetUID() != 29);
	if( pFlightRoute->GetItemCount() == 0)
		return false;

	AirsideResourceList resList;
	AirsideResource* pCurrentRes = lastItem.GetResource();

	if (pCurrentRes == NULL)	//birth
	{
		AirEntrySystemHoldInInSim* pEntryHold = pFlightRoute->GetEntrySystemHold();
		if( pEntryHold && pEntryHold->GetInQFlightCount() && !pEntryHold->isFlightInQ(pFlight)) //if the hold is not empty, and pFlight is not in hold,add pFlight to the hold and wait
		{
			pEntryHold->OnFlightEnter(pFlight,lastItem.GetTime() );				
			pEntryHold->AddFlightToQueue(pFlight);
			return true;
		}
		else if(pEntryHold && !pEntryHold->GetOccupyInstance(pFlight).IsValid() )
		{
			pEntryHold->AddFlightToQueue(pFlight);
			pEntryHold->OnFlightEnter(pFlight,lastItem.GetTime() );
		}

	}

	pFlightRoute->GetConcernRouteSegs(pCurrentRes,resList);

	int nSize = (int)resList.size();

	if (nSize == 0)	//end of route
	{
		if (pFlight->IsArrivingOperation() && pCurrentRes->GetType() != AirsideResource::ResType_LogicRunway)	// only enroute
		{
			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(pFlight->GetTime());
			newClearance.AddItem(newItem);

			pFlight->PerformClearance(newClearance);
			return true;
		}

		return false;
	}
	if (pFlight->GetRunwayExit() == NULL && pFlight->IsArrivingOperation())
	{				
		pFlight->GetAirTrafficController()->AssignRunwayExit(pFlight);

		if (pFlight->GetRunwayExit() == NULL)		// no exit
		{
			CString strWarn;
			strWarn.Format("No available runway exit for the flight on %s!", pFlight->GetLandingRunway()->PrintResource()) ;
			CString strError = _T("FLLIGHT TERMINATE");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);	
			pFlight->GetLandingRunway()->RemoveElementOccupancyInfo(pFlight);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastItem.GetTime());
			newClearance.AddItem(newItem);
			pFlight->PerformClearance(newClearance);
			return true;

		}
	}

	if ( CheckIsFlightNeedWaitingInHold(pFlight,resList, lastItem))
	{
		if(pFlight->WaitingInHold()== false && pFlight->GetMode() != OnBirth)//cannot hold the flight, terminate it
		{
			CString strError = "Flight cannot land for there has no hold in the STAR to hold the flight";
			CString strErrorType = "AIRCRAFT TERMINATE";
			AirsideSimErrorShown::SimWarning(pFlight,strError,strErrorType);

			pFlight->GetLandingRunway()->RemoveElementOccupancyInfo(pFlight);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastItem.GetTime());
			newClearance.AddItem(newItem);

			pFlight->PerformClearance(newClearance);
		}
		return true;
	}

	AirsideResource* pRes = resList.at(nSize-1);

	if (pCurrentRes)
	{

		if (lastItem.GetMode() == OnFinalApproach)		//at miss approach point to check whether runway is cleared
		{
			if (pFlight->GetLandingRunway()->IsRunwayClear(pFlight))
			{
				CRunwaySystem *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();
				ClearanceItem TouchdownItem = pRunwaysController->GetRunwayTouchdownClearanceItem(pFlight,lastItem.GetTime(),lastItem);
				newClearance.AddItem(TouchdownItem);


				pFlight->GetAirTrafficController()->GetRunwaysController()->GetRunwayLandingClearance(pFlight,TouchdownItem,newClearance);
				return true;
			}
			else
			{
				pFlight->GetLandingRunway()->SetHasMissedApproachFlight();

				FlightRouteInSim* pAirRoute = GetMisApproachRoute(pFlight);
				if (pAirRoute->IsEmpty())
				{
					CString strError = "Flight cannot land and there is no available Miss Approach air route to re-enter STAR";
					CString strErrorType = "AIRCRAFT TERMINATE";
					AirsideSimErrorShown::SimWarning(pFlight,strError,strErrorType);

					pFlight->GetLandingRunway()->RemoveElementOccupancyInfo(pFlight);

					ClearanceItem newItem(NULL,OnTerminate,0);
					newItem.SetTime(lastItem.GetTime());
					newClearance.AddItem(newItem);	

					pFlight->PerformClearance(newClearance);
					return true;
				}

				AirsideFlightInSim* pConflictFlt = pFlight->GetLandingRunway()->GetLastInResourceFlight();
				pFlight->StartDelay(lastItem,pConflictFlt,FlightConflict::MISSAPPROACH,FlightConflict::RUNWAY,FltDelayReason_AirHold,_T("Missed approach"));	//Miss approach delay
				GetMissApproachClearance(pFlight,pAirRoute, lastItem,newClearance);
				return true;
			}

		}

		AirspaceHoldInSim* pHold = NULL;
		if (pCurrentRes->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			pHold = ((AirRouteSegInSim*)pCurrentRes)->GetConnectedHold();
			//need judge whether need waiting in hold at an ATC point in the segment, for smooth fling to enter the hold
			//if flight at ATC managed point in the segment, give it next clearance directly,because the clearance of next waypoint already be calculated when flight prepare enter the segment
			if (pHold != NULL && 
				((AirRouteSegInSim*)pCurrentRes)->GetSecondConcernIntersection() != NULL &&
				((AirRouteSegInSim*)pCurrentRes)->GetATCManagedPoint(pFlight) == pFlight->GetPosition())		
			{

				AirRoutePointInSim* pSecondIntersection = ((AirRouteSegInSim*)pCurrentRes)->GetSecondConcernIntersection()->getInputPoint();
				FlightPlanPropertyInSim Pointprop(NULL);
				ElapsedTime tSegTime = 0L;
				if (pFlight->IsArrivingOperation())
				{
					Pointprop = pFlight->GetSTAR()->GetRoutePointProperty(pSecondIntersection);
					tSegTime = pFlight->GetSTAR()->GetFlightTimeInSeg(pFlight,(AirRouteSegInSim*)pCurrentRes);
				}
				else
				{
					Pointprop = pFlight->GetSID()->GetRoutePointProperty(pSecondIntersection);
					tSegTime = pFlight->GetSID()->GetFlightTimeInSeg(pFlight,(AirRouteSegInSim*)pCurrentRes);
				}
				ElapsedTime tLastWaypointTime = ((AirRouteSegInSim*)pCurrentRes)->GetFirstConcernIntersection()->getInputPoint()->GetOccupyInstance(pFlight).GetEnterTime();
				ClearanceItem nextItem(pCurrentRes,Pointprop.GetMode(),0);
				nextItem.SetPosition(pSecondIntersection->GetPosition());
				nextItem.SetSpeed(Pointprop.GetSpeed());
				nextItem.SetTime(tLastWaypointTime + tSegTime);

				if (pHold->GetHeadFlightCount(pFlight) >0)		// there are flights holding 
				{
					nextItem.SetAltitude(pHold->getAvailableHeight(pFlight));
					//pHold->AddHoldingFlight(pFlight);
					pFlight->PerformClearanceItem(nextItem);
					pFlight->StartDelay(nextItem, NULL, FlightConflict::HOLD_AIR, FlightConflict::BETWEENWAYPOINTS,FltDelayReason_AirHold,_T("Holding delay"));	//holding delay
					pFlight->WaitingInHold(nextItem.GetTime());
					//newClearance.AddItem(nextItem);
					return true;
				}	

				CRunwaySystem *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();
				ClearanceItem preItem = nextItem;
				if (pFlight->IsArrivingOperation() && pRes->GetType() == AirsideResource::ResType_LogicRunway)		//landing phase
				{
					bool bDelay = false;
					ElapsedTime tDelay = -1L;

					std::vector<CRunwaySystem::RunwayOccupyInfo> vRunwayArrangeInfo;
					TimeRange eLandingTimeRange(ElapsedTime(0L),ElapsedTime(0L));

					if (IsFlightNeedDelayLandingOnRwy(pFlight,preItem,tDelay))
						bDelay = true;
					else
					{
						ElapsedTime eTotalTime = CalculateOptimisticTimeThrouhRoute(pFlight,pFlightRoute,resList);

						if(!pRunwaysController->ApplyForLandingTime(pFlight,preItem.GetTime() + eTotalTime,eLandingTimeRange,vRunwayArrangeInfo))
							bDelay = true;
					}

					if(bDelay)
					{
						nextItem.SetAltitude(pHold->getAvailableHeight(pFlight));
						pFlight->PerformClearanceItem(nextItem);

						pFlight->GetLandingRunway()->GetRunwayInSim()->AddWaitingFlight(pFlight);
						pFlight->StartDelay(nextItem, NULL, FlightConflict::HOLD_AIR, FlightConflict::RUNWAY,FltDelayReason_AirHold,_T("Holding delay"));	//holding delay
						pFlight->WaitingInHold(tDelay + pFlight->GetTime());

						return true;
					}


					try
					{
						FlightSchemeOnAirRouteLanding(pFlight,pFlightRoute,resList,preItem,lastItem,eLandingTimeRange,preItem.GetTime(),vRunwayArrangeInfo);
					}
					catch (FlightPlanConflictFlightPerformanceException& e)
					{
						AirsideSimErrorShown::SimWarning(pFlight,e.GetExcepDesc(),e.GetExcepType());

						newClearance.ClearAllItems();
						ClearanceItem newItem(NULL,OnTerminate,0);
						newItem.SetTime(lastItem.GetTime());
						newClearance.AddItem(newItem);	
						pFlight->PerformClearance(newClearance);
						return true;	

					}
	
				}
				else	//enroute or terminal part 
				{
					//AirspaceHoldInSim* pNextHold = NULL;
					//if (pRes->GetType() == AirsideResource::ResType_AirRouteSegment && ((AirRouteSegInSim*)pRes)->GetSecondConcernIntersection())
					//	pNextHold = ((AirRouteSegInSim*)pRes)->GetSecondConcernIntersection()->GetConnectHold();

					//if (pNextHold)
					//{
					//	if (!pNextHold->GetLock(pFlight))
					//	{
					//		pFlight->GetWakeUpCaller().OberverSubject(pNextHold);
					//		return true;
					//	}
					//}
					FlightSchemeOnAirRoute(pFlight,pFlightRoute,resList,lastItem,lastItem);
				}

				//to get altitude
				if (nextItem.GetTime() == pSecondIntersection->GetOccupyInstance(pFlight).GetEnterTime())		//without holding time
				{
					nextItem.SetAltitude(Pointprop.GetAlt());
					pFlight->PerformClearanceItem(nextItem);
					{
						ReleaseResourceLockEvent * pReleseEvent = new ReleaseResourceLockEvent(pFlight, pHold);
						pReleseEvent->setTime(nextItem.GetTime());
						pReleseEvent->addEvent();			
					}
					GenerateFlightClearance(pFlight, pFlightRoute, resList, nextItem, newClearance);

					if (pFlight->IsArrivingOperation() && pRes->GetType() == AirsideResource::ResType_LogicRunway)
					{
						Clearance runwayClearence;

						LogicRunwayInSim* pPort = pFlight->GetLandingRunway();
						double landspd = pFlight->GetPerformance()->getLandingSpeed(pFlight);	
						DistanceUnit touchDist = pFlight->GetTouchDownDistAtRuwnay(pPort);

						//landing item
						ClearanceItem landingItem(pPort,OnLanding,touchDist);		
						landingItem.SetSpeed(landspd);
						landingItem.SetPosition(pPort->GetFlightTouchdownPoint(pFlight));
						landingItem.SetTime(pPort->GetOccupyInstance(pFlight).GetEnterTime());
						pRunwaysController->WriteRunwayLogs(pFlight,true,landingItem,runwayClearence);		
					}
				}
				else									//need holding
				{				
					nextItem.SetAltitude(pHold->getAvailableHeight(pFlight));
					pFlight->PerformClearanceItem(nextItem);
					pFlight->StartDelay(nextItem, NULL, FlightConflict::HOLD_AIR, FlightConflict::BETWEENWAYPOINTS,FltDelayReason_AirHold,_T("Holding delay"));	//holding delay
					pFlight->WaitingInHold(pSecondIntersection->GetOccupyInstance(pFlight).GetEnterTime());
				}

				return true;
			}
			
		}
	}

	try
	{

		bool bEnterSystem  = FlightPrearrangeOnAirRoute(pFlight,pFlightRoute,resList,lastItem,newClearance);
					
		if(bEnterSystem && newClearance.GetItemCount() && pFlight->GetMode() == OnBirth) // flight get the time enter the system
		{
			AirEntrySystemHoldInInSim* pEntryHold = pFlightRoute->GetEntrySystemHold();
			if (pEntryHold == NULL)
				return true;

			ElapsedTime tEnterSysTime = newClearance.GetItem(0).GetTime();
			pEntryHold->writeFlightLog(pFlight,tEnterSysTime);
			CFlightDelayOnHoldEvent* pHoldDelayEvent = new CFlightDelayOnHoldEvent(pFlight,pEntryHold);
// 			if (pEntryHold->GetOccupancyTable().size() > 1)
// 			{
// 				tEnterSysTime += ElapsedTime(10L);
// 			}
			pHoldDelayEvent->setTime(tEnterSysTime);
			pHoldDelayEvent->addEvent();
		/*	pEntryHold->OnFlightExit(pFlight,tEnterSysTime);*/
		}

		if ( !bEnterSystem)
			newClearance.ClearItems();
	}
	catch (AirsideACTypeMatchError* pError)
	{
		AirsideDiagnose* pDiagnose =static_cast<AirsideDiagnose*>(pError->GetDiagnose());
		CString& strErrorType = pDiagnose->GetErrorType();
		CString& strError = pDiagnose->GetDetails();
		AirsideSimErrorShown::SimWarning(pFlight,strError,strErrorType);

		delete pError;
		pError = NULL;

		ClearanceItem newItem(NULL,OnTerminate,0);
		newItem.SetTime(pFlight->GetTime());
		newClearance.AddItem(newItem);

		pFlight->PerformClearance(newClearance);
	}
	catch(AirsideHeadingException* pError)
	{
		AirsideDiagnose* pDiagnose =static_cast<AirsideDiagnose*>(pError->GetDiagnose());
		CString& strErrorType = pDiagnose->GetErrorType();
		CString& strError = pDiagnose->GetDetails();
		AirsideSimErrorShown::SimWarning(pFlight,strError,strErrorType);

		delete pError;
		pError = NULL;

		ClearanceItem newItem(NULL,OnTerminate,0);
		newItem.SetTime(pFlight->GetTime());
		newClearance.AddItem(newItem);

		pFlight->PerformClearance(newClearance);
	}
	return true;

}

bool AirRouteNetworkInSim::FlightPrearrangeOnAirRouteExcepteTakeoffPart(AirsideFlightInSim* pFlight,FlightRouteInSim *pAirRouteInSim,AirsideResourceList& ResourceList,ClearanceItem& lastItem,Clearance& newClearance)
{

	CRunwaySystem *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();
	std::vector<CRunwaySystem::RunwayOccupyInfo> vRunwayArrangeInfo;
	vRunwayArrangeInfo.clear();

	TimeRange eLandingTimeRange(ElapsedTime(0L),ElapsedTime(0L));

	int nCount = (int)ResourceList.size();
	AirsideResource* pResource = ResourceList.at(nCount -1);

	ClearanceItem preItem = lastItem;
	if (pFlight->IsArrivingOperation() && pResource->GetType() == AirsideResource::ResType_LogicRunway)		//landing
	{	
		//ASSERT(pFlight->GetUID() != 80);
		if (pFlight->GetLandingRunway() == NULL)		// no landing runway
		{
			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastItem.GetTime());
			newClearance.AddItem(newItem);

			pFlight->PerformClearance(newClearance);
			return true;
		}

		ElapsedTime tRwyEnter = 0L;
		if (pFlight->GetMode() == OnWaitInHold && pFlight->GetResource()->GetType() == AirsideResource::ResType_WayPoint)	//holding finished
		{
			OccupancyInstance _instance = pFlight->GetLandingRunway()->GetOccupyInstance(pFlight);
			//ASSERT(_instance.GetFlight() != NULL);			//error
			ElapsedTime tMinTotalTime = CalculateMinTimeThrouhRoute(pFlight,pAirRouteInSim,ResourceList);
			if (_instance.GetFlight() == pFlight && tMinTotalTime < _instance.GetEnterTime() - lastItem.GetTime())		//has applied already
			{
				eLandingTimeRange.SetStartTime(_instance.GetEnterTime());
				eLandingTimeRange.SetEndTime(_instance.GetExitTime());
			}

		}

		if (eLandingTimeRange.GetStartTime() == 0L)		//not get available landing time
		{
			ElapsedTime eTotalTime = CalculateOptimisticTimeThrouhRoute(pFlight,pAirRouteInSim,ResourceList);
			tRwyEnter = lastItem.GetTime() + eTotalTime;

			if(!pRunwaysController->ApplyForLandingTime(pFlight,tRwyEnter,eLandingTimeRange,vRunwayArrangeInfo))
			{
				pFlight->GetLandingRunway()->GetRunwayInSim()->AddWaitingFlight(pFlight);
				bool bCanHold = pFlight->WaitingInHold();

				if (!bCanHold)		//no hold to holding
				{
					CString strError;
					CString strAtRes;
					if(preItem.GetResource())
					{
						strAtRes = preItem.GetResource()->PrintResource();
					}
					else
					{
						strAtRes = _T("SystemHold");
					}
					strError.Format("Flight conflict with other flight at %s, and without Hold to holding",strAtRes );
					CString strErrorType = "AIRCRAFT TERMINATE";
					AirsideSimErrorShown::SimWarning(pFlight,strError,strErrorType);

					newClearance.ClearAllItems();
					ClearanceItem newItem(NULL,OnTerminate,0);
					newItem.SetTime(lastItem.GetTime());
					newClearance.AddItem(newItem);	
					pFlight->PerformClearance(newClearance);
					return true;
				}
				pFlight->StartDelay(lastItem, NULL, FlightConflict::HOLD_AIR, FlightConflict::RUNWAY,FltDelayReason_AirHold,_T("Holding delay"));	//holding delay
				return false;
			}
		}

		try
		{
			FlightSchemeOnAirRouteLanding(pFlight,pAirRouteInSim,ResourceList,lastItem,preItem, eLandingTimeRange,pFlight->GetTime(),vRunwayArrangeInfo);
			GenerateFlightClearance(pFlight, pAirRouteInSim, ResourceList, preItem, newClearance);

		}
		catch (FlightPlanConflictFlightPerformanceException& e)
		{
			AirsideSimErrorShown::SimWarning(pFlight,e.GetExcepDesc(),e.GetExcepType());

			newClearance.ClearAllItems();
			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastItem.GetTime());
			newClearance.AddItem(newItem);	
			pFlight->PerformClearance(newClearance);
			return true;	

		}


	}
	else	//enroute or terminal part
	{
		FlightSchemeOnAirRoute(pFlight,pAirRouteInSim,ResourceList,lastItem, preItem);
		GenerateFlightClearance(pFlight, pAirRouteInSim, ResourceList, preItem, newClearance);
	}

	//if (newClearance.GetItemCount() >0 && preItem.GetTime() < newClearance.ItemAt(0).GetTime() && preItem.GetMode() > OnBirth)			//with delay at waypoint
	//{
	//	ClearanceItem firstItem = newClearance.ItemAt(0);
	//	//ASSERT(pFlight->GetUID() != 31);
	//	bool bCanHold = pFlight->WaitingInHold(firstItem.GetTime());
	//	newClearance.ClearAllItems();
	//	if (!bCanHold)		//no hold to holding
	//	{
	//		CString strError;
	//		strError.Format("Flight conflict with other flight at %s, and without hold to holding",preItem.GetResource()->PrintResource());
	//		CString strErrorType = "AIRCRAFT TERMINATE";
	//		AirsideSimErrorShown::SimWarning(pFlight,strError,strErrorType);

	//		ClearanceItem newItem(NULL,OnTerminate,0);
	//		newItem.SetTime(pFlight->GetTime());
	//		newClearance.AddItem(newItem);	
	//		pFlight->PerformClearance(newClearance);
	//		return true;
	//	}
	//	pFlight->StartDelay(preItem, NULL, FlightConflict::HOLD_AIR, FlightConflict::BETWEENWAYPOINTS,FltDelayReason_AirHold,_T("Holding delay"));	//holding delay
	//	return false;

	//}

	AirspaceHoldInSim* pHold = NULL;
	if (pFlight->GetResource()&& pFlight->GetResource()->GetType() == AirsideResource::ResType_AirRouteSegment)
	{
		pHold = ((AirRouteSegInSim*)pFlight->GetResource())->GetConnectedHold();
	}
	else if (pFlight->GetResource()&& pFlight->GetResource()->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
	{
		pHold = ((HeadingAirRouteSegInSim*)pFlight->GetResource())->GetConnectedHold();
	}
	else if (pFlight->GetResource()&& pFlight->GetResource()->GetType() == AirsideResource::ResType_WayPoint)
	{
		pHold = ((AirWayPointInSim*)pFlight->GetResource())->GetHold();
	}
	else if (pFlight->GetResource()&& pFlight->GetResource()->GetType() == AirsideResource::ResType_AirspaceHold)
	{
		pHold = (AirspaceHoldInSim*)pFlight->GetResource();
	}

	/*if (pHold)
	{
		ReleaseResourceLockEvent * pReleseEvent = new ReleaseResourceLockEvent(pFlight, pHold);
		pReleseEvent->setTime(preItem.GetTime());
		pReleseEvent->addEvent();				
	}*/

	if (pFlight->IsArrivingOperation() && pResource->GetType() == AirsideResource::ResType_LogicRunway)		//landing
	{
		Clearance runwayClearence;

		LogicRunwayInSim* pPort = pFlight->GetLandingRunway();
		double landspd = pFlight->GetPerformance()->getLandingSpeed(pFlight);	
		DistanceUnit touchDist = pFlight->GetTouchDownDistAtRuwnay(pPort);//pFlight->GetPerformance()->getDistThresToTouch(pFlight);

		//landing item
		ClearanceItem landingItem(pPort,OnLanding,touchDist);		
		landingItem.SetSpeed(landspd);
		landingItem.SetPosition(pPort->GetFlightTouchdownPoint(pFlight));
		landingItem.SetTime(pPort->GetOccupyInstance(pFlight).GetEnterTime());
		pRunwaysController->WriteRunwayLogs(pFlight,true,landingItem,runwayClearence);		
	}

	return true;
	
}

bool AirRouteNetworkInSim::CircuitFlightTakeoffOnAirRoute(AirsideCircuitFlightInSim* pFlight,
															   FlightRouteInSim *pAirRouteInSim, 
															   AirsideResourceList& ResourceList,
															   const ClearanceItem& currentItem,
															   Clearance& newClearance,
															   AirsideCircuitFlightInSim::LandingOperation emType
	)
{
	ClearanceItem flightItem = currentItem;

	if ( ResourceList.at(0)->GetType() == AirsideResource::ResType_LogicRunway)	//take off	
	{	
		if (pFlight->GetAndAssignTakeoffRunway() == NULL)		// no takeoff runway
		{
			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(currentItem.GetTime());
			newClearance.AddItem(newItem);

			pFlight->PerformClearance(newClearance);
			return true;
		}

		CRunwaySystem *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();
		std::vector<CRunwaySystem::RunwayOccupyInfo> vRunwayArrangeInfo;
		vRunwayArrangeInfo.clear();
		bool bNeedCreateTakeOffEvent = false;

		ElapsedTime dTimeToRwy;
		AirsideResource * pRes = currentItem.GetResource(); // add at the end of the taxiway item
		if (pRes)
		{
			if(pRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
			{
				CPath2008 filletPath = pFlight->GetFilletPathFromTaxiwayToRwyport(pFlight->GetAndAssignTakeoffRunway()->getLogicRunwayType());
				MobileTravelTrace travelTrace(pFlight->mGroundPerform,filletPath.GetTotalLength(),currentItem.GetSpeed(),0 );
				travelTrace.BeginUse();
				dTimeToRwy = travelTrace.getEndTime();
			}
			else if (pRes->GetType() == AirsideResource::ResType_AirRouteSegment)
			{
				ClearanceItem landingItem = pRunwaysController->GetRunwayTouchdownClearanceItem(pFlight,flightItem.GetTime(),flightItem);
				dTimeToRwy = landingItem.GetTime() - currentItem.GetTime();
			}
		}
		ElapsedTime tMoveTime =0L;
		bool bTakeoff = FlightSchemeOnAirRouteTakeOff(pFlight, pAirRouteInSim, ResourceList, currentItem, dTimeToRwy, tMoveTime, vRunwayArrangeInfo);

		if (!bTakeoff)
			return false;

		if (currentItem.GetTime() < tMoveTime)
			bNeedCreateTakeOffEvent = true;


		flightItem.SetTime(tMoveTime);
		//pRunwaysController->GetCircuitFlightTakeoffClearance(pFlight,dTimeToRwy,flightItem,newClearance,emType);
		pRunwaysController->WriteCircuitFlightRunwayTakeoffLogs(pFlight,vRunwayArrangeInfo,newClearance);
		ClearanceItem RwyLiftOffItem = newClearance.GetItem(newClearance.GetItemCount() -1);

		GenerateFlightClearance(pFlight, pAirRouteInSim, ResourceList, RwyLiftOffItem, newClearance);

		if (bNeedCreateTakeOffEvent)
		{
			//wait log
			//start waiting event
			ClearanceItem StartwaitItem = currentItem;	
			StartwaitItem.SetMode(OnTakeOffWaitEnterRunway);

			//wait item
			ClearanceItem waitItem = currentItem;
			//ElapsedTime tMoveTime = max((eTakeOffTime - dTimeToRwy), lastItem.GetTime());
			waitItem.SetTime(tMoveTime);

			ElapsedTime tDelay = tMoveTime - currentItem.GetTime()/* -dTimeToRwy*/;
			if (tDelay > 0L)		//delay when takeoff
			{
				int nDelayId = -1;
				CAirsideFlightRunwayDelay* pDelay = new CAirsideFlightRunwayDelay;
				if (pFlight->GetAndAssignTakeoffRunway())
				{
					int nRunwayID = pFlight->GetAndAssignTakeoffRunway()->GetRunwayInSim()->GetRunwayInput()->getID();
					pDelay->SetFlightID(pFlight->GetUID());
					pDelay->SetDelayResult(FltDelayReason_RunwayHold);
					pDelay->SetDelayTime(tDelay);
					pDelay->SetStartTime(currentItem.GetTime());
					if (pFlight->GetOutput())
					{
						nDelayId = pFlight->GetOutput()->LogFlightDelay(pDelay);
					}
				}
				waitItem.SetDelayId(nDelayId);
			}
			waitItem.SetMode(OnTakeOffWaitEnterRunway);
			pFlight->PerformClearanceItem(StartwaitItem);
			pFlight->PerformClearanceItem(waitItem);

			//create take off event, take off at takeoff time
			if(0)
			{
				newClearance.ClearItems();
				FlightGetClearanceEvent * pNextEvent = new FlightGetClearanceEvent(pFlight);
				pNextEvent->setTime(tMoveTime);
				pNextEvent->addEvent();
			}
			else
			{
				CFlightDelayTakeoffOnRwyEvent * pTakeOffEvent = new CFlightDelayTakeoffOnRwyEvent(pFlight,newClearance);
				pTakeOffEvent->setTime(tMoveTime);
				pTakeOffEvent->addEvent();
				newClearance.ClearItems();
			}
		}
	}
	return true;
}

bool AirRouteNetworkInSim::FlightPrearrangeOnAirRoute(AirsideFlightInSim* pFlight,
													  FlightRouteInSim *pAirRouteInSim, 
													  AirsideResourceList& ResourceList,
													  const ClearanceItem& currentItem,
													  Clearance& newClearance)
{	

	ClearanceItem flightItem = currentItem;

	if (pFlight->IsDepartingOperation() && ResourceList.at(0)->GetType() == AirsideResource::ResType_LogicRunway)	//take off	
	{	
		if (pFlight->GetAndAssignTakeoffRunway() == NULL)		// no takeoff runway
		{
			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(currentItem.GetTime());
			newClearance.AddItem(newItem);

			pFlight->PerformClearance(newClearance);
			return true;
		}

		CRunwaySystem *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();
		std::vector<CRunwaySystem::RunwayOccupyInfo> vRunwayArrangeInfo;
		vRunwayArrangeInfo.clear();
		bool bNeedCreateTakeOffEvent = false;

		//FlightGroundRouteDirectSegInSim* pSeg = (FlightGroundRouteDirectSegInSim*)flightItem.GetResource();
		//DistanceUnit travelDist = pSeg->GetEndDist()-pSeg->GetExitHoldDist();
// 		ClearanceItem rwyItem(pFlight->GetTakeoffRunway(),OnPreTakeoff, pFlight->GetTakeoffPosition()->GetEnterRunwayPos());
// 		rwyItem.SetSpeed(0);
		CPath2008 filletPath = pFlight->GetFilletPathFromTaxiwayToRwyport(pFlight->GetAndAssignTakeoffRunway()->getLogicRunwayType());
		MobileTravelTrace travelTrace(pFlight->mGroundPerform,filletPath.GetTotalLength(),flightItem.GetSpeed(),0 );
		travelTrace.BeginUse();
		ElapsedTime dTimeToRwy = travelTrace.getEndTime();

		ElapsedTime tMoveTime =0L;
		bool bTakeoff = FlightSchemeOnAirRouteTakeOff(pFlight, pAirRouteInSim, ResourceList, currentItem, dTimeToRwy, tMoveTime, vRunwayArrangeInfo);

		if (!bTakeoff)
			return false;

		if (currentItem.GetTime() < tMoveTime)
			bNeedCreateTakeOffEvent = true;
		

		flightItem.SetTime(tMoveTime);
		pRunwaysController->WriteRunwayTakeOffLogs(pFlight,vRunwayArrangeInfo,tMoveTime,dTimeToRwy, flightItem,newClearance);
		ClearanceItem RwyLiftOffItem = newClearance.GetItem(newClearance.GetItemCount() -1);

		GenerateFlightClearance(pFlight, pAirRouteInSim, ResourceList, RwyLiftOffItem, newClearance);

		if (bNeedCreateTakeOffEvent)
		{
			//wait log
			//start waiting event
			ClearanceItem StartwaitItem = currentItem;	
			StartwaitItem.SetMode(OnTakeOffWaitEnterRunway);

			//wait item
			ClearanceItem waitItem = currentItem;
			//ElapsedTime tMoveTime = max((eTakeOffTime - dTimeToRwy), lastItem.GetTime());
			waitItem.SetTime(tMoveTime);

			ElapsedTime tDelay = tMoveTime - currentItem.GetTime()/* -dTimeToRwy*/;
			if (tDelay > 0L)		//delay when takeoff
			{
				int nDelayId = -1;
				CAirsideFlightRunwayDelay* pDelay = new CAirsideFlightRunwayDelay;
				if (pFlight->GetAndAssignTakeoffRunway())
				{
					int nRunwayID = pFlight->GetAndAssignTakeoffRunway()->GetRunwayInSim()->GetRunwayInput()->getID();
					pDelay->SetFlightID(pFlight->GetUID());
					pDelay->SetDelayResult(FltDelayReason_RunwayHold);
					pDelay->SetDelayTime(tDelay);
					pDelay->SetStartTime(currentItem.GetTime());
					if (pFlight->GetOutput())
					{
						nDelayId = pFlight->GetOutput()->LogFlightDelay(pDelay);
					}
				}
				waitItem.SetDelayId(nDelayId);
			}
			waitItem.SetMode(OnTakeOffWaitEnterRunway);
			pFlight->PerformClearanceItem(StartwaitItem);
			pFlight->PerformClearanceItem(waitItem);
			



//////////////////////////////////////////////////////////////////////////
#if _DEBUGLOG
			{
				Clearance clearance;	
				clearance.AddItem(StartwaitItem);
				clearance.AddItem(waitItem);
				//log to flight file

				CString debugfileName;
				debugfileName.Format( "C:\\FlightDebug\\%d.txt", pFlight->GetUID() );
				std::ofstream outfile;
				outfile.open(debugfileName,std::ios::app);
				outfile << pFlight->GetTime()<<"," << long(pFlight->GetTime())<<std::endl;
				outfile << pFlight->GetPosition().getX() <<" ," <<pFlight->GetPosition().getY() << std::endl;
				outfile << clearance;
				outfile.close();
				//log to all file
				CString allfile = "C:\\FlightDebug\\AllFlight.txt";
				std::ofstream alloutfile;
				alloutfile.open(allfile,std::ios::app);		
				alloutfile<<pFlight->GetCurrentFlightID().GetString()<<"("<<pFlight->GetUID()<<"),"<<pFlight->GetTime()<<"," << long(pFlight->GetTime())<<std::endl;
				alloutfile<<clearance;		
				alloutfile.close();
			}
#endif
//////////////////////////////////////////////////////////////////////////

			//create take off event, take off at takeoff time
			if(0)
			{
				newClearance.ClearItems();
				FlightGetClearanceEvent * pNextEvent = new FlightGetClearanceEvent(pFlight);
				pNextEvent->setTime(tMoveTime);
				pNextEvent->addEvent();
			}
			else
			{
				CFlightDelayTakeoffOnRwyEvent * pTakeOffEvent = new CFlightDelayTakeoffOnRwyEvent(pFlight,newClearance);
				pTakeOffEvent->setTime(tMoveTime);
				//pFlight->SetTime(tMoveTime);
				pTakeOffEvent->addEvent();
				newClearance.ClearItems();
			}
		}
	}
	else
	{
		return FlightPrearrangeOnAirRouteExcepteTakeoffPart(pFlight,pAirRouteInSim,ResourceList,flightItem,newClearance);
	}

	return true;
}

void AirRouteNetworkInSim::GenerateFlightClearance(AirsideFlightInSim* pFlight,FlightRouteInSim *pAirRouteInSim,AirsideResourceList& ResourceList,ClearanceItem& lastItem,Clearance& newClearance)
{
	ClearanceItem preItem = lastItem;
	int nCount = (int)ResourceList.size();
	for (int i =0; i < nCount; i++)
	{
		AirsideResource* pResource = ResourceList.at(i);
		if (pFlight->IsArrivingOperation() && pResource->GetType() == AirsideResource::ResType_LogicRunway && nCount >1)
		{
			AirRoutePointInSim* pFirstIntersection = ((AirRouteSegInSim*)ResourceList.at(i-1))->GetFirstConcernIntersection()->getInputPoint();
			CPoint2008 MissApproachCheckPoint = pFlight->GetLandingRunway()->GetMissApproachCheckPoint(pFirstIntersection->GetPosition());
			if (MissApproachCheckPoint == pFirstIntersection->GetPosition())
			{
				int nNum = newClearance.GetItemCount();
				preItem = newClearance.GetItem(nNum-1);
				preItem.SetMode(OnFinalApproach);
				newClearance.RemoveItem(nNum-1);
				newClearance.AddItem(preItem);
				return;
			}

			double dDistToWaypoint = pFirstIntersection->GetPosition().distance(MissApproachCheckPoint);
			double dTouchDist = pFlight->GetTouchDownDistAtRuwnay((LogicRunwayInSim*)pResource); //pFlight->GetPerformance()->getDistThresToTouch(pFlight);
			double dSegDist = pFirstIntersection->GetPosition().distance(MissApproachCheckPoint) 
							+ MissApproachCheckPoint.distance(pFlight->GetLandingRunway()->GetDistancePoint(dTouchDist));
			preItem.SetPosition(MissApproachCheckPoint);
			double dElevation = pFlight->GetAirTrafficController()->GetAirsideResourceManager()->GetAirportResource()->GetAirportInfo().getElevation();
			double dAlt = (preItem.GetAltitude() - dElevation)*(1- dDistToWaypoint/dSegDist) + dElevation;
			preItem.SetAltitude(dAlt);

			double dLandSpd = pFlight->GetPerformance()->getLandingSpeed(pFlight);
			DynamicMovement dynamicMoveCalc(preItem.GetSpeed(),dLandSpd, dSegDist);
			ElapsedTime tTime = preItem.GetTime() + dynamicMoveCalc.GetDistTime(dDistToWaypoint);			
			preItem.SetTime(tTime);
			preItem.SetSpeed(dynamicMoveCalc.GetDistSpeed(dDistToWaypoint));
			preItem.SetMode(OnFinalApproach);
			newClearance.AddItem(preItem);
			return;
		}

		if (pResource->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			((HeadingAirRouteSegInSim*)pResource)->CalculateClearanceItems(pFlight,pAirRouteInSim,preItem,ResourceList.at(i+1),newClearance);
		}

		if (pResource->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			AirRoutePointInSim* pFirstIntersection = ((AirRouteSegInSim*)pResource)->GetFirstConcernIntersection()->getInputPoint();
			FlightPlanPropertyInSim property = pAirRouteInSim->GetRoutePointProperty(pFirstIntersection);
			OccupancyInstance instance = pFirstIntersection->GetOccupyInstance(pFlight);
			ElapsedTime tDelayAtPoint = instance.GetEnterTime() - preItem.GetTime();

			int nDelayId = -1;
			if (preItem.GetResource() == NULL && tDelayAtPoint > 0L)		//delay at first waypoint when arrival
			{
				FlightConflictAtWaypointDelay* pDelay = new FlightConflictAtWaypointDelay;
				//pDelay->SetAtObjectID(pFirstIntersection->getID());
				pDelay->SetFlightID(pFlight->GetUID());
				pDelay->SetDelayResult(FltDelayReason_AirHold);
				pDelay->SetDelayTime(tDelayAtPoint);
				pDelay->SetStartTime(preItem.GetTime());
				if (pFlight->GetOutput())
				{
					nDelayId = pFlight->GetOutput()->LogFlightDelay(pDelay);
				}
			}

			ClearanceItem newItem(pResource,property.GetMode(),0);
			newItem.SetPosition(pFirstIntersection->GetPosition());

			if (preItem.GetPosition() == pFirstIntersection->GetPosition() && ((AirRouteSegInSim*)pResource)->GetFirstConcernIntersection()->GetConnectHold())
				newItem.SetAltitude(preItem.GetAltitude());
			else
				newItem.SetAltitude(property.GetAlt());

			newItem.SetSpeed(instance.GetSpeed());
			newItem.SetTime(instance.GetEnterTime());
			if (nDelayId > -1)
				newItem.SetDelayId(nDelayId);

			if (newItem.GetMode() != preItem.GetMode() && preItem.GetMode() == OnMissApproach)		//miss approach operation finish, so delay finish
			{
				pFlight->EndDelay(newItem);		//end miss approach delay
			}

			preItem = newItem;
			newClearance.AddItem(preItem);

			ElapsedTime tExit = -1L;
			if (((AirRouteSegInSim*)pResource)->GetSecondConcernIntersection())
			{
				AirRoutePointInSim* pSecondIntersection = ((AirRouteSegInSim*)pResource)->GetSecondConcernIntersection()->getInputPoint();
				tExit = pSecondIntersection->GetOccupyInstance(pFlight).GetEnterTime();
			}
			if(tExit < 0L)
				tExit = preItem.GetTime() + pAirRouteInSim->GetFlightTimeInSeg(pFlight,(AirRouteSegInSim*)pResource);

			((AirRouteSegInSim*)pResource)->CalculateClearanceItems(pFlight,pAirRouteInSim,preItem,tExit,newClearance);

			if ( ((AirRouteSegInSim*)pResource)->GetSecondConcernIntersection())
			{	
				AirRouteIntersectionInSim* pIntersectionNode = ((AirRouteSegInSim*)pResource)->GetSecondConcernIntersection();
				AirRoutePointInSim* pSecondIntersection = pIntersectionNode->getInputPoint();

				FlightPlanPropertyInSim Pointprop = pAirRouteInSim->GetRoutePointProperty(pSecondIntersection);
				instance = pSecondIntersection->GetOccupyInstance(pFlight);

				if (pIntersectionNode->GetConnectHold())
				{
					CPoint2008 ATCManagedPoint = ((AirRouteSegInSim*)pResource)->GetATCManagedPoint(pFlight);
					preItem = newClearance.GetItem(newClearance.GetItemCount()-1);
					if (preItem.GetPosition() == ATCManagedPoint)
						return;

					double dRateInSegment = ATCManagedPoint.distance(preItem.GetPosition())/(preItem.GetPosition().distance(pSecondIntersection->GetPosition()));
					ElapsedTime dT = pAirRouteInSim->GetFlightTimeInSeg(pFlight,(AirRouteSegInSim*)pResource);
					ElapsedTime tTime = ElapsedTime(dT.asSeconds()*dRateInSegment) + preItem.GetTime();
					double dAlt = (1-dRateInSegment) * (preItem.GetAltitude() - Pointprop.GetAlt()) + Pointprop.GetAlt();
					preItem.SetAltitude(dAlt);
					preItem.SetPosition(ATCManagedPoint);
					preItem.SetTime(tTime);
					newClearance.AddItem(preItem);
					return;

				}
				else
				{

					ClearanceItem secondItem(pResource,Pointprop.GetMode(),0);
					secondItem.SetPosition(pSecondIntersection->GetPosition());
					secondItem.SetAltitude(Pointprop.GetAlt());
					if (instance.GetFlight())
					{
						secondItem.SetSpeed(instance.GetSpeed());
						secondItem.SetTime(instance.GetEnterTime());
					}
					else
					{
						secondItem.SetSpeed(Pointprop.GetSpeed());
						secondItem.SetTime(tExit);
					}
					preItem = secondItem;
					newClearance.AddItem(preItem);					
				}


			}
		}
	}
}

bool AirRouteNetworkInSim::FlightSchemeOnAirRoute(AirsideFlightInSim* pFlight,FlightRouteInSim *pAirRouteInSim,AirsideResourceList& ResourceList,ClearanceItem& lastItem, const ClearanceItem& flightState)
{
	//ClearanceItem lastItem(pFlight->GetResource(),OnBirth,0.0);
	//lastItem.SetTime(tNewEnterTime);
	//lastItem.SetSpeed(pFlight->GetSpeed());

	ElapsedTime tDelay = 0L;
	int nSegmentCount = ResourceList.size();
	//initialization
	std::vector<SegmentTimeInfo> vSegTimeInfo;
	for (int i = 0; i<= nSegmentCount; ++i)		//used to record available time at intersection point
		vSegTimeInfo.push_back(SegmentTimeInfo());

	int nSeg = 0;
	while (nSeg <= nSegmentCount)		//last segment need judge conflict of second intersection
	{	
		AirsideResource* pNextRes = NULL;
		if (nSeg < nSegmentCount)
			pNextRes = ResourceList.at(nSeg);
		else
			pNextRes = ResourceList.at(nSegmentCount -1);

		AirsideResource* pPreRes = lastItem.GetResource();
		if (nSeg >0)
			pPreRes = ResourceList.at(nSeg -1);

		ElapsedTime eTimeEstmated = -1L;

		if (pNextRes->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			HeadingAirRouteSegInSim* pNextSeg = (HeadingAirRouteSegInSim*)pNextRes;

			ClearanceItem newItem = lastItem;

			if(pPreRes->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
			{
				CString strTime;
				CString strID;
				AirsideDiagnose* pNewDiagnose = new AirsideDiagnose(strTime, strID);
				CString strRoute;
				AirRouteInSim *pRouteInSim = pAirRouteInSim->GetAirRouteInSim();
				if (pRouteInSim)
				{
					CAirRoute* pRoute = pRouteInSim->GetAirRouteInput();
					if (pRoute)
					{
						strRoute= pRoute->getName();
					}
				}
			
				CString strDetals;
				strDetals.Format(_T("Heading cannot been defined in the first segment or the last segment of the Air Route: %s "),strRoute);
				pNewDiagnose->SetDetails(strDetals);
				throw new AirsideHeadingException(pNewDiagnose);
			}

			bool bFit = pNextSeg->CalculateFlightClearance(pFlight,pPreRes,pAirRouteInSim,newItem,lastItem.GetTime());
			if (bFit)
			{
				vSegTimeInfo[nSeg].curTime = newItem.GetTime();
				lastItem = newItem;
				nSeg = nSeg +1;
				continue;
			}

			eTimeEstmated = newItem.GetTime();
			pNextSeg->GetFirstIntersection()->GetAvailableTimeAtIntersection(pFlight,pNextRes,pAirRouteInSim,eTimeEstmated,vSegTimeInfo[nSeg].minTime,vSegTimeInfo[nSeg].maxTime);

			tDelay = vSegTimeInfo[nSeg].minTime - eTimeEstmated;
			if (tDelay == ElapsedTime(0L))
				tDelay = ElapsedTime(1L);
		}
		else
		{
			AirRouteIntersectionInSim* pConflictIntersection = NULL;
			if (nSeg < nSegmentCount)
				pConflictIntersection = ((AirRouteSegInSim *)pNextRes)->GetFirstConcernIntersection();
			else
			{
				if (((AirRouteSegInSim *)pNextRes)->GetSecondConcernIntersection() == NULL)	//last segment only one waypoint
					return true;

				pConflictIntersection = ((AirRouteSegInSim *)pNextRes)->GetSecondConcernIntersection();
				if (pConflictIntersection->GetConnectHold())
					return true;

			}

			ElapsedTime eSegTime = 0L;
			if ((pPreRes && pPreRes->GetType() == AirsideResource::ResType_AirRouteSegment)&& lastItem.GetPosition() != pConflictIntersection->getInputPoint()->GetPosition())
				eSegTime = pAirRouteInSim->GetFlightTimeInSeg(pFlight,(AirRouteSegInSim *)pPreRes);	

			ClearanceItem newItem(pNextRes,OnBirth,0.0);
			newItem.SetTime(lastItem.GetTime() + eSegTime);
			double dSpeed = pAirRouteInSim->GetRoutePointProperty(pConflictIntersection->getInputPoint()).GetSpeed();
			newItem.SetSpeed(dSpeed);

			//to adjust time difference
			if (pConflictIntersection->GetConnectHold()&& flightState.GetPosition() == pConflictIntersection->getInputPoint()->GetPosition() && flightState.GetTime() > newItem.GetTime())
			{
				newItem.SetTime(pFlight->GetTime());
			}

			bool bFit = pConflictIntersection->CalculateClearanceAtIntersection(pFlight,pPreRes,pNextRes,pAirRouteInSim,lastItem,newItem);
			if (bFit)
			{
				//pConflictIntersection->GetAvailableTimeAtIntersection(pFlight,pNextRes,lastItem.GetTime() + eSegTime,vSegTimeInfo[nSeg].minTime,vSegTimeInfo[nSeg].maxTime);
				vSegTimeInfo[nSeg].curTime = newItem.GetTime();
				lastItem = newItem;

				nSeg = nSeg +1;
				continue;
			}

			eTimeEstmated = newItem.GetTime();
			pConflictIntersection->GetAvailableTimeAtIntersection(pFlight,pNextRes,pAirRouteInSim,eTimeEstmated,vSegTimeInfo[nSeg].minTime,vSegTimeInfo[nSeg].maxTime);

			//adjust the time range
			tDelay = eTimeEstmated - (lastItem.GetTime() + eSegTime);
			if (tDelay == ElapsedTime(0L))
				tDelay = ElapsedTime(1L);

			lastItem.SetTime(lastItem.GetTime()+tDelay);
			FlightSchemeOnAirRoute(pFlight,pAirRouteInSim,ResourceList,lastItem,flightState);
		}

		break;

	}

	return true; 
}


bool AirRouteNetworkInSim::FlightSchemeOnAirRouteLanding(AirsideFlightInSim* pFlight,FlightRouteInSim *pAirRouteInSim,AirsideResourceList& ResourceList,ClearanceItem& lastItem,const ClearanceItem& flightState, TimeRange& eLandingTimeRange,
													 ElapsedTime tNewEnterTime,std::vector<CRunwaySystem::RunwayOccupyInfo>& vRunwayArrangeInfo)
{	


	ElapsedTime tDelay = 0L;
	CRunwaySystem *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();

	//ClearanceItem FlightStatusItem = lastItem;
	size_t nSegmentCount = ResourceList.size();

	// the last resource is runway
	LogicRunwayInSim *pLogicRunway = pFlight->GetLandingRunway();
	ClearanceItem runwayItem( pLogicRunway,OnLanding,0);
	runwayItem.SetTime(eLandingTimeRange.GetStartTime());
	runwayItem.SetSpeed(pFlight->GetPerformance()->getLandingSpeed(pFlight));
	runwayItem.SetAltitude(0.0);
	lastItem = runwayItem;
	//ClearanceItem nextItem = runwayItem;

	//initialization
	std::vector<ClearanceItem > vClearanceItem;
	std::vector<SegmentTimeInfo> vSegTimeInfo;
	for (size_t i = 0; i< nSegmentCount; ++i)
	{
		vClearanceItem.push_back(ClearanceItem(NULL,OnBirth,0.0));
		vSegTimeInfo.push_back(SegmentTimeInfo());
	}

	//write the logic runway info
	vClearanceItem[nSegmentCount-1] = lastItem;
	vSegTimeInfo[nSegmentCount-1].minTime = eLandingTimeRange.GetStartTime();
	vSegTimeInfo[nSegmentCount-1].maxTime = eLandingTimeRange.GetEndTime();
	vSegTimeInfo[nSegmentCount-1].curTime = eLandingTimeRange.GetStartTime();



	int nSeg = (int)nSegmentCount -2;
	while (nSeg >= 0)
	{	
		AirsideResource* pNextRes = ResourceList.at(nSeg);
		AirsideResource* pLastSeg = ResourceList.at(nSeg+1);
		ElapsedTime eTimeEstmated = -1L;

		if (pNextRes->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			HeadingAirRouteSegInSim* pNextSeg = (HeadingAirRouteSegInSim*)pNextRes;

			ASSERT(nSeg != (int)nSegmentCount - 2);	//the heading segment shouldn't be last segment
			ClearanceItem newItem = lastItem;

			bool bFit = pNextSeg->ConverseCalculateFlightClearance(pFlight,pLastSeg,pAirRouteInSim,lastItem,newItem,ResourceList,tNewEnterTime);
			if (bFit)
			{
				vSegTimeInfo[nSeg].curTime = newItem.GetTime();
				vClearanceItem[nSeg] = newItem;
				lastItem = newItem;

				nSeg = nSeg -1;
				continue;
			}

			eTimeEstmated = newItem.GetTime();
			pNextSeg->GetFirstIntersection()->GetAvailableTimeAtIntersection(pFlight,pNextRes,pAirRouteInSim,eTimeEstmated,vSegTimeInfo[nSeg].minTime,vSegTimeInfo[nSeg].maxTime);

			tDelay = vSegTimeInfo[nSeg].minTime - eTimeEstmated;
			if(tDelay == ElapsedTime(0L))
				tDelay = ElapsedTime(1L);
		}
		else
		{
			AirRouteSegInSim* pCurrentSegment =(AirRouteSegInSim *)pNextRes;

			ElapsedTime eSegTime;
			if (nSeg == (int)nSegmentCount -2)//the last segment Item 
			{
				eSegTime = pAirRouteInSim->CalculateTimeFromLastSegToLogicRunway(pFlight);
			}
			else
			{
				eSegTime = pAirRouteInSim->GetFlightTimeInSeg(pFlight,pCurrentSegment);
			}

			AirRouteIntersectionInSim* pConflictIntersection = pCurrentSegment->GetFirstConcernIntersection();

			ClearanceItem newItem(/*pConflictIntersection->getInputPoint()*/pCurrentSegment,OnBirth,0.0);
			newItem.SetTime(lastItem.GetTime() - eSegTime);

			double dSpeed = pAirRouteInSim->GetRoutePointProperty(pConflictIntersection->getInputPoint()).GetSpeed();
			newItem.SetSpeed(dSpeed);

			//to adjust time difference
			if (pConflictIntersection->GetConnectHold() && flightState.GetPosition() == pConflictIntersection->getInputPoint()->GetPosition())
			{
				if ((flightState.GetTime() > newItem.GetTime())||( flightState.GetTime() < newItem.GetTime() 
					&& (lastItem.GetTime() - pFlight->GetTime()) <pAirRouteInSim->GetFlightMaxTimeInSeg(pFlight,pCurrentSegment)))
					newItem.SetTime(pFlight->GetTime());			
			}

			bool bFit = pConflictIntersection->ConverseCalculateClearanceAtIntersection(pFlight,pLastSeg,pCurrentSegment,pAirRouteInSim,lastItem,newItem, eSegTime);
			if (bFit)
			{
				vSegTimeInfo[nSeg].curTime = newItem.GetTime();
				vClearanceItem[nSeg] = newItem;
				lastItem = newItem;

				nSeg = nSeg -1;
				continue;
			}

			eTimeEstmated = newItem.GetTime();
			pConflictIntersection->GetAvailableTimeAtIntersection(pFlight,pCurrentSegment,pAirRouteInSim,eTimeEstmated,vSegTimeInfo[nSeg].minTime,vSegTimeInfo[nSeg].maxTime);

			tDelay = vSegTimeInfo[nSeg].minTime - (lastItem.GetTime() - eSegTime);
			if(tDelay == ElapsedTime(0L))
				tDelay = ElapsedTime(1L);
		}

		//adjust the time range
		if (eTimeEstmated <= vSegTimeInfo[nSeg].minTime)
		{
			if (AdjustMinTime(vSegTimeInfo,vClearanceItem,nSeg))
			{

				nSeg = nSeg -1;
				continue;			
			}
			else
			{
				//recalculate
				ElapsedTime nSegsTime = CalculateOptimisticTimeThrouhRoute(pFlight,pAirRouteInSim,ResourceList);
				ElapsedTime eEstmatedRunwayTime =  tNewEnterTime + nSegsTime + tDelay;

				pRunwaysController->ApplyForLandingTime(pFlight,eEstmatedRunwayTime,eLandingTimeRange,vRunwayArrangeInfo);


				FlightSchemeOnAirRouteLanding(pFlight,pAirRouteInSim,ResourceList,lastItem,flightState, eLandingTimeRange,tNewEnterTime + tDelay,vRunwayArrangeInfo);
				break;
			}
		}
		else if(eTimeEstmated >= vSegTimeInfo[nSeg].maxTime)
		{
			if (AdjustMaxTime(vSegTimeInfo,vClearanceItem,nSeg))
			{
				nSeg = nSeg -1;
				continue;	
			}
			else
			{	
				ElapsedTime nSegsTime = CalculateOptimisticTimeThrouhRoute(pFlight,pAirRouteInSim,ResourceList);
				ElapsedTime eEstmatedRunwayTime =  tNewEnterTime + nSegsTime + tDelay;
				pRunwaysController->ApplyForLandingTime(pFlight,eEstmatedRunwayTime,eLandingTimeRange,vRunwayArrangeInfo);

				FlightSchemeOnAirRouteLanding(pFlight,pAirRouteInSim,ResourceList,lastItem,flightState, eLandingTimeRange,tNewEnterTime + tDelay,vRunwayArrangeInfo);

				break;
			}

		}
	}


	return true;

}

bool AirRouteNetworkInSim::FlightSchemeOnAirRouteTakeOff(AirsideFlightInSim* pFlight,
														 FlightRouteInSim *pAirRouteInSim,
														 AirsideResourceList& ResourceList,
														 const ClearanceItem& currentItem, 
														 const ElapsedTime& tTimeToRwy, 
														 ElapsedTime& tMoveTime,
														 std::vector<CRunwaySystem::RunwayOccupyInfo>& vRunwayArrangeInfo)
{

	CRunwaySystem *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();

	ElapsedTime eLiftNeedTime = 0L;
	ElapsedTime tEnterRwy = currentItem.GetTime()+ tTimeToRwy;
	ClearanceItem flightItem = currentItem;
	TimeRange eTakeingOffTime(0L,0L);

	AirsideFlightRunwayDelayLog* pLog = NULL;
	if(!pRunwaysController->ApplyForTakeoffTime(pFlight, tTimeToRwy,tEnterRwy, eTakeingOffTime, vRunwayArrangeInfo))
	{
		pFlight->GetAndAssignTakeoffRunway()->GetRunwayInSim()->AddWaitingFlight(pFlight);

		ResourceDesc resDesc;
		ResourceList.at(0)->getDesc(resDesc);
		pLog = new AirsideFlightRunwayDelayLog(resDesc,currentItem.GetTime().asSeconds(), OnTakeoff, 0l, 0l, FlightRunwayDelay::AtHoldShort);
		pLog->sReasonDetail = "Wave crossings"; 
		return false;
	}

	if (pLog != NULL)
	{
		pLog->mAvailableTime = eTakeingOffTime.GetStartTime().asSeconds();
		pLog->mDelayTime = (pLog->mAvailableTime - pLog->time);
		pFlight->LogEventItem(pLog);
	}

	Clearance rwyClearce;
	tMoveTime = max((eTakeingOffTime.GetStartTime() - tTimeToRwy), currentItem.GetTime());
	flightItem.SetTime(tMoveTime);
	pRunwaysController->GetFlightTakeOffClearance(pFlight,tTimeToRwy, flightItem, rwyClearce);

	//get lift off time
	if(rwyClearce.GetItemCount()>0)
	{
		flightItem = rwyClearce.ItemAt(rwyClearce.GetItemCount()-1);
		eLiftNeedTime = flightItem.GetTime() - eTakeingOffTime.GetStartTime()/* + tTimeToRwy*/;
	}

	ElapsedTime tRelatedNodeDelay = pRunwaysController->GetFlightDelayTimeAtRunwayRelatedIntersections(pFlight, flightItem.GetTime(), flightItem.GetDistInResource());

	if (tRelatedNodeDelay > 0L)
	{
		ClearanceItem newCurrentItem =currentItem;
		newCurrentItem.SetTime(currentItem.GetTime() + tRelatedNodeDelay);

		FlightSchemeOnAirRouteTakeOff(pFlight,pAirRouteInSim,ResourceList,newCurrentItem,tTimeToRwy,tMoveTime, vRunwayArrangeInfo);		
	}
	else
	{
		size_t nSegmentCount = ResourceList.size();
		std::vector<ClearanceItem > vClearanceItem;
		std::vector<SegmentTimeInfo> vSegTimeInfo;
		for (size_t i = 0; i< nSegmentCount; ++i)
		{
			vClearanceItem.push_back(ClearanceItem(NULL,OnBirth,0.0));
			vSegTimeInfo.push_back(SegmentTimeInfo());
		}

		//write the logic runway info
		vClearanceItem[0] = flightItem;

		int nSeg = 1;//start from the second one
		ClearanceItem preItem = flightItem;
		while (nSeg < (int)nSegmentCount)
		{	
			AirRouteSegInSim* pCurrentSegment =(AirRouteSegInSim *)ResourceList.at(nSeg);
			AirsideResource* pLastSeg = ResourceList.at(nSeg -1);
			ElapsedTime eSegTime;

			if (nSeg == 1)//the first segment with only one waypoint
				eSegTime = pAirRouteInSim->CalculateTakeoffTimeFromRwyToFirstSeg(pFlight);
			else
				eSegTime = pAirRouteInSim->GetFlightTimeInSeg(pFlight,(AirRouteSegInSim*)pLastSeg);

			AirRouteIntersectionInSim* pConflictIntersection = NULL;
			pConflictIntersection = pCurrentSegment->GetFirstConcernIntersection();

			ClearanceItem newItem(/*pConflictIntersection->getInputPoint()*/pCurrentSegment,OnTakeoff,0.0);
			newItem.SetTime(preItem.GetTime() + eSegTime);
			double dSpeed = pAirRouteInSim->GetRoutePointProperty(pConflictIntersection->getInputPoint()).GetSpeed();
			newItem.SetSpeed(dSpeed);

			bool bFit = pConflictIntersection->CalculateClearanceAtIntersection(pFlight,pLastSeg,pCurrentSegment,pAirRouteInSim,preItem,newItem);
			if (bFit )
			{
				vSegTimeInfo[nSeg].curTime = newItem.GetTime();
				vClearanceItem[nSeg] = newItem;

				nSeg = nSeg +1;
				continue;
			}
			else
			{
				ElapsedTime tAvailableTime = newItem.GetTime();
				pConflictIntersection->GetAvailableTimeAtIntersection(pFlight,pCurrentSegment,pAirRouteInSim,tAvailableTime,vSegTimeInfo[nSeg].minTime,vSegTimeInfo[nSeg].maxTime);
					
				ElapsedTime nRouteSegsTime = CalculateOptimisticTimeThrouhRouteTakeOff(pFlight,pAirRouteInSim,ResourceList,nSeg);

				ClearanceItem newCurrentItem = currentItem;
				ElapsedTime tNewEnterRwy =  vSegTimeInfo[nSeg].minTime+ ElapsedTime(1L)  - nRouteSegsTime- eLiftNeedTime ;
				newCurrentItem.SetTime(tNewEnterRwy - tTimeToRwy);

				//pRunwaysController->ApplyForTakeoffTime(pFlight,tNewEnterRwy,eTakeingOffTime,vRunwayArrangeInfo);

				//Clearance takeOffClearce;
				//ElapsedTime tMoveTime = max((eTakeingOffTime - tTimeToRwy), RwyLiftOffItem.GetTime()); 

				//pRunwaysController->GetFlightTakeOffClearance(pFlight,eTakeingOffTime.GetStartTime(),tTimeToRwy, lastItemCopy,takeOffClearce);

				//if(takeOffClearce.GetItemCount()>0)		//get lift off time again
				//{
				//	eLiftNeedTime = takeOffClearce.ItemAt(takeOffClearce.GetItemCount()-1).GetTime() - takeOffClearce.ItemAt(0).GetTime() + tTimeToRwy;
				//}

				////take off	
				//lastItemCopy.SetTime(eTakeingOffTime.GetStartTime() + eLiftNeedTime);
				//if (eTakeingOffTime.GetEndTime() > ElapsedTime(0L))
				//{
				//	eTakeingOffTime.SetEndTime(eTakeingOffTime.GetEndTime() + eLiftNeedTime);
				//}
				FlightSchemeOnAirRouteTakeOff(pFlight,pAirRouteInSim,ResourceList,newCurrentItem,tTimeToRwy,tMoveTime, vRunwayArrangeInfo);

				break;
			}
		}


	}

	return true;
}

bool AirRouteNetworkInSim::AdjustMinTime(std::vector<SegmentTimeInfo>& vSegTimeInfo, std::vector<ClearanceItem >& vClearancItem,int nCurSeg)
{
	return false;

	ElapsedTime eTimeSep = vSegTimeInfo[nCurSeg].minTime - vSegTimeInfo[nCurSeg].curTime;

	//calculate the max adjust time
	ElapsedTime eMaxAdjustTime= ElapsedTime(0L);
	for (int i = nCurSeg + 1; i < (int)vSegTimeInfo.size(); ++ i)
	{
		eMaxAdjustTime += vSegTimeInfo[i].curTime - vSegTimeInfo[i].minTime;
	}

	if (eMaxAdjustTime < eTimeSep)
		return false;

	//adjust time
	vSegTimeInfo[nCurSeg].curTime = vSegTimeInfo[nCurSeg].minTime;

	for (int nSeg = nCurSeg + 1; nSeg < (int)vSegTimeInfo.size(); ++nSeg)
	{
		if (vSegTimeInfo[nSeg].curTime -eTimeSep - vSegTimeInfo[nSeg].minTime < ElapsedTime(0L)) // cann't adjust
			return false;

		ElapsedTime nSegTimeAdjust = vSegTimeInfo[nSeg].curTime - vSegTimeInfo[nSeg].minTime;



		if (nSegTimeAdjust > eTimeSep)
		{
			vSegTimeInfo[nSeg].curTime = vSegTimeInfo[nSeg].curTime - eTimeSep;
			break;
		}
		else
		{
			vSegTimeInfo[nSeg].curTime = vSegTimeInfo[nSeg].minTime;

			eTimeSep = eTimeSep - nSegTimeAdjust;
		}

	}

	return true;
}

bool AirRouteNetworkInSim::AdjustMaxTime(std::vector<SegmentTimeInfo>& vSegTimeInfo, std::vector<ClearanceItem >& vClearancItem,int nCurSeg)
{
	return false;
	ElapsedTime eTimeSep = vSegTimeInfo[nCurSeg].curTime- vSegTimeInfo[nCurSeg].maxTime ;

	//calculate the max adjust time
	ElapsedTime eMaxAdjustTime= ElapsedTime(0L);
	for (int i = nCurSeg + 1; i < (int)vSegTimeInfo.size(); ++ i)
	{
		eMaxAdjustTime += vSegTimeInfo[i].maxTime - vSegTimeInfo[i].curTime;
	}

	if (eMaxAdjustTime < eTimeSep)
		return false;

	//adjust time
	vSegTimeInfo[nCurSeg].curTime = vSegTimeInfo[nCurSeg].minTime;

	for (int nSeg = nCurSeg + 1; nSeg < (int)vSegTimeInfo.size(); ++nSeg)
	{	

		if ( vSegTimeInfo[nSeg].curTime + eTimeSep - vSegTimeInfo[nSeg].maxTime> ElapsedTime(0L)) // cann't adjust
			return false;

		ElapsedTime nSegTimeAdjust =vSegTimeInfo[nSeg].maxTime - vSegTimeInfo[nSeg].curTime;

		if (nSegTimeAdjust > eTimeSep)
		{
			vSegTimeInfo[nSeg].curTime = vSegTimeInfo[nSeg].curTime - eTimeSep;
			break;
		}
		else
		{
			vSegTimeInfo[nSeg].curTime = vSegTimeInfo[nSeg].minTime;

			eTimeSep = eTimeSep - nSegTimeAdjust;
		}

	}

	return true;
}

ElapsedTime AirRouteNetworkInSim::CalculateOptimisticTimeThrouhRoute(AirsideFlightInSim *pFlight, FlightRouteInSim *pAirRoute,AirsideResourceList& ResourceList)
{
	ElapsedTime eTotalTime(0L);

	size_t nSegmentCount = ResourceList.size();
	if (nSegmentCount < 2)
		return eTotalTime;

	//the last item is runway,ignore it
	for (int nSeg  = 0; nSeg < (int)nSegmentCount -2; ++ nSeg)
	{
		AirsideResource * pTheRes  = ResourceList.at(nSeg); 
		if( pTheRes && pTheRes->GetType() == AirsideResource::ResType_AirRouteSegment )
		{
			AirRouteSegInSim* pCurrentSegment =(AirRouteSegInSim *)pTheRes;
			ElapsedTime eSegTime = pAirRoute->GetFlightTimeInSeg(pFlight,pCurrentSegment);
			eTotalTime += eSegTime;
		}
		if(pTheRes && pTheRes->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			HeadingAirRouteSegInSim* pSeg = (HeadingAirRouteSegInSim*)pTheRes;
			double dSped = pAirRoute->GetRoutePointProperty(pSeg->GetFirstIntersection()->getInputPoint()).GetSpeed();
			double dSped2 = pAirRoute->GetRoutePointProperty(pSeg->GetSecondIntersection()->getInputPoint()).GetSpeed();

			eTotalTime += pSeg->GetMinTimeInSegment(pFlight,dSped,dSped2,ResourceList.at(nSeg+1));
		}
	}
	//if (nSegmentCount >=2)
	//{
		AirsideResource * pLastSegResource = ResourceList.at(nSegmentCount -2);
		if( pLastSegResource && pLastSegResource->GetType()== AirsideResource::ResType_AirRouteSegment )
		{
			AirRouteSegInSim* pLastSegment =(AirRouteSegInSim *)pLastSegResource;
			ElapsedTime eLastTime = pAirRoute->CalculateTimeFromLastSegToLogicRunway(pFlight);
			eTotalTime += eLastTime;
		}
	//}
	return eTotalTime;
}

ElapsedTime AirRouteNetworkInSim::CalculateMinTimeThrouhRoute(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRoute,AirsideResourceList& ResourceList)
{
	ElapsedTime eTotalTime(0L);

	size_t nSegmentCount = ResourceList.size();
	if (nSegmentCount < 2)
		return eTotalTime;

	//the last item is runway,ignore it
	for (int nSeg  = 0; nSeg < (int)nSegmentCount -2; ++ nSeg)
	{
		AirsideResource * pTheRes  = ResourceList.at(nSeg); 
		if( pTheRes && pTheRes->GetType() == AirsideResource::ResType_AirRouteSegment )
		{
			AirRouteSegInSim* pCurrentSegment =(AirRouteSegInSim *)pTheRes;
			ElapsedTime eSegTime = pAirRoute->GetFlightMinTimeInSeg(pFlight,pCurrentSegment);
			eTotalTime += eSegTime;
		}
		if(pTheRes && pTheRes->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			HeadingAirRouteSegInSim* pSeg = (HeadingAirRouteSegInSim*)pTheRes;
			////ElapsedTime eSegTime =  pAirRoute->GetHeadingMinTimeInSeg(pFlight,pSeg,ResourceList.at(nSeg+1));

			double dSped = pAirRoute->GetRoutePointProperty(pSeg->GetFirstIntersection()->getInputPoint()).GetSpeed();
			double dSped2 = pAirRoute->GetRoutePointProperty(pSeg->GetSecondIntersection()->getInputPoint()).GetSpeed();

			ElapsedTime eSegTime =  pSeg->GetMinTimeInSegment(pFlight,dSped,dSped2,ResourceList.at(nSeg+1));

			eTotalTime += eSegTime;
		}
	}

	AirsideResource * pLastSegResource = ResourceList.at(nSegmentCount -2);
	if( pLastSegResource && pLastSegResource->GetType()== AirsideResource::ResType_AirRouteSegment )
	{
		AirRouteSegInSim* pLastSegment =(AirRouteSegInSim *)pLastSegResource;
		ElapsedTime eLastTime = pAirRoute->CalculateMinTimeFromLastSegToLogicRunway(pFlight);
		eTotalTime += eLastTime;
	}

	return eTotalTime;
}

ElapsedTime AirRouteNetworkInSim::CalculateOptimisticTimeThrouhRouteTakeOff(AirsideFlightInSim *pFlight, FlightRouteInSim *pAirRoute,AirsideResourceList& ResourceList,int nEndSeg)
{
	ElapsedTime eTotalTime(0L);

	size_t nSegmentCount = ResourceList.size();
	if (nSegmentCount < 2)
		return eTotalTime;

	if (nEndSeg >= (int)nSegmentCount)
	{
		ASSERT(0);
		nEndSeg = nSegmentCount;
	}


	//the last item is runway,ignore it
	for (int nCurSeg  = 1; nCurSeg <= nEndSeg ;++nCurSeg)
	{
		if (nCurSeg == 1)
		{
			ElapsedTime eRunwayPortToFirstNode = pAirRoute->CalculateTakeoffTimeFromRwyToFirstSeg(pFlight);
			eTotalTime += eRunwayPortToFirstNode;
		}
		else
		{
			AirsideResource * pTheRes  = ResourceList.at(nCurSeg-1); 
			if( pTheRes && pTheRes->GetType() == AirRouteSegInSim::ResType_AirRouteSegment )
			{
				AirRouteSegInSim* pCurrentSegment =(AirRouteSegInSim *)pTheRes;
				ElapsedTime eSegTime = pAirRoute->GetFlightTimeInSeg(pFlight,pCurrentSegment);
				eTotalTime += eSegTime;
			}
		}


	}

	return eTotalTime;
}

ElapsedTime AirRouteNetworkInSim::CalculateTimeFromOneSegToNextSeg(AirsideFlightInSim *pFlight,FlightRouteInSim *pAirRouteInSim,AirRouteSegInSim *pCurrentSeg,AirRouteSegInSim *pNextSeg)
{

	ElapsedTime eSegTime = 	pAirRouteInSim->GetFlightTimeInSeg(pFlight,pCurrentSeg);

	return eSegTime;
}


FlightRouteInSim* AirRouteNetworkInSim::GetFlightEnroute(AirsideFlightInSim* pFlight, int nEntryWaypointID, int nExitWaypointID)
{
	FlightRouteInSim* pRoute = NULL;
	AirWayPointInSim* pEntry = m_vRoutePoints.GetAirWaypointInSimByID(nEntryWaypointID);
	AirWayPointInSim* pExit = m_vRoutePoints.GetAirWaypointInSimByID(nExitWaypointID);
	if (pEntry == NULL || pExit == NULL)
		return pRoute;
	AirsideResourceList vSegs;
	vSegs.clear();

	int nCount = m_vAirLaneInSimList.size();
	for (int i =0; i < nCount; i++)
	{
		AirRouteInSim* pAirRoute = m_vAirLaneInSimList.at(i);
		vSegs = pAirRoute->GetRouteSegmentList(pEntry,pExit);
		if (!vSegs.empty())
			break;
	}

	nCount = vSegs.size();

	if (nCount ==0)
		return pRoute;

	if (pRoute == NULL)
		pRoute = new FlightPlanSpecificRouteInSim;

	FlightPlanPropertiesInSim& planProperties = pRoute->GetPlanProperties();	

	for (int j =0; j < nCount; j++)
	{
		double dSpeed = (pFlight->GetPerformance()->getMaxCruiseSpeed(pFlight)+pFlight->GetPerformance()->getMinCruiseSpeed(pFlight))/2.0;
		AirsideResource* pRes = vSegs.at(j);
		if (pRes->GetType() == AirsideResource::ResType_AirRouteSegment )
		{
			AirRouteSegInSim* pSeg = (AirRouteSegInSim*)pRes;

			AirRoutePointInSim* pFirst = pSeg->GetFirstConcernIntersection()->getInputPoint();
			FlightPlanPropertyInSim property1(pFirst);
			property1.SetAlt((pFirst->GetAltHigh() + pFirst->GetAltLow())/2.0) ;
			property1.SetSpeed(dSpeed) ;
			property1.SetMode(OnCruiseThrough);
			planProperties.push_back(property1);

			AirRoutePointInSim* pSecond = pSeg->GetSecondConcernIntersection()->getInputPoint();
			FlightPlanPropertyInSim property2(pSecond);
			property2.SetAlt((pSecond->GetAltHigh() + pSecond->GetAltLow())/2.0) ;
			property2.SetSpeed(dSpeed) ;
			property2.SetMode(OnCruiseThrough);
			planProperties.push_back(property2);

			pRoute->AddItem(pSeg);

		}
		if (pRes->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			HeadingAirRouteSegInSim* pSeg = (HeadingAirRouteSegInSim*)pRes;

			AirRoutePointInSim* pFirst = pSeg->GetFirstIntersection()->getInputPoint();
			FlightPlanPropertyInSim property1(pFirst);
			property1.SetAlt((pFirst->GetAltHigh() + pFirst->GetAltLow())/2.0) ;
			property1.SetSpeed(dSpeed) ;
			property1.SetMode(OnCruiseThrough);
			planProperties.push_back(property1);

			AirRoutePointInSim* pSecond = pSeg->GetSecondIntersection()->getInputPoint();
			FlightPlanPropertyInSim property2(pSecond);
			property2.SetAlt((pSecond->GetAltHigh() + pSecond->GetAltLow())/2.0) ;
			property2.SetSpeed(dSpeed) ;
			property2.SetMode(OnCruiseThrough);
			planProperties.push_back(property2);

			pRoute->AddItem(pSeg);
		}
	}
	return pRoute;
}

void AirRouteNetworkInSim::ReleaseFlightHoldLock(AirsideFlightInSim* pFlight)
{
	int nCount = m_vHoldInSimList.size();
	for (int i =0; i < nCount; i++)
	{
		AirspaceHoldInSim* pHold = m_vHoldInSimList.at(i);
		pHold->RemoveLockFlight(pFlight);
	}
}

HeadingAirRouteSegInSim* AirRouteNetworkInSim::GetHeadingSegmentInList( AirRouteIntersectionInSim* pWayPt1, AirRouteIntersectionInSim* pWayPt2 )
{
	int nCount = m_vHeadingRouteSegs.GetCount() ;
	for (int i =0; i < nCount; i++)
	{
		HeadingAirRouteSegInSim* pSeg = m_vHeadingRouteSegs.GetSegment(i);
		if (pSeg->GetFirstIntersection() == pWayPt1 && pSeg->GetSecondIntersection() == pWayPt2)
			return pSeg;
	}

	return NULL;
}

ElapsedTime AirRouteNetworkInSim::GetLandingFlightTimeFromPointToRwy( AirsideFlightInSim* pFlight,FlightRouteInSim *pAirRouteInSim, const CPoint2008& point )
{
	LogicRunwayInSim* pPort = pFlight->GetLandingRunway();
	AirRoutePointInSim* pLastWP = pAirRouteInSim->GetAirRouteInSim()->GetRwyConnectedRoutePoint();
	DistanceUnit touchDist = pFlight->GetPerformance()->getDistThresToTouch(pFlight);

	double dRate = point.distance(pPort->GetDistancePoint(touchDist))/pLastWP->GetPosition().distance(pPort->GetDistancePoint(touchDist));

	ElapsedTime tSegTime = pAirRouteInSim->CalculateTimeFromLastSegToLogicRunway(pFlight);

	return ElapsedTime(tSegTime.asSeconds()*dRate);
}

ElapsedTime AirRouteNetworkInSim::GetTakeoffFlightTimeFromRwyToPoint( AirsideFlightInSim* pFlight,FlightRouteInSim *pAirRouteInSim, const CPoint2008& point )
{
	LogicRunwayInSim* pPort = pFlight->GetAndAssignTakeoffRunway()->GetOtherPort();
	AirRoutePointInSim* pLastWP = pAirRouteInSim->GetAirRouteInSim()->GetRwyConnectedRoutePoint();

	double dRate = point.distance(pPort->GetDistancePoint(0))/pLastWP->GetPosition().distance(pPort->GetDistancePoint(0));

	ElapsedTime tSegTime = pAirRouteInSim->CalculateTakeoffTimeFromRwyToFirstSeg(pFlight);

	return ElapsedTime(tSegTime.asSeconds()*dRate);

}

ElapsedTime AirRouteNetworkInSim::GetDelayTimeByEncroachTrailSeperationOnSameSID( AirsideFlightInSim* pFlight,const ElapsedTime& tCurTime)
{
	FlightRouteInSim* pSID = pFlight->GetSID();
	CRunwaySystem *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();

	ElapsedTime tTime = pRunwaysController->getFlightTakeoffTime(pFlight);
	ElapsedTime tAtWaypoint = tCurTime + tTime;

	ElapsedTime tAvailStartTime = -1L;
	ElapsedTime tAvailEndTime = -1L;
	int nCount = pSID->GetAirRouteInSim()->GetRouteSegCount();
	for (int i =0; i < nCount; i++)
	{
		AirRouteSegInSim* pSeg = (AirRouteSegInSim*)pSID->GetAirRouteInSim()->GetSeg(i);
		if (i == 0)
			tAtWaypoint += pSID->CalculateTakeoffTimeFromRwyToFirstSeg(pFlight);
		else
			tAtWaypoint += pSID->GetFlightTimeInSeg(pFlight, pSeg);

		AirRouteIntersectionInSim* pNode = pSeg->GetFirstConcernIntersection();
		pNode->GetAvailableTimeAtIntersection(pFlight, pSeg, pSID,tAtWaypoint, tAvailStartTime, tAvailEndTime);

		if (tAvailStartTime - tAtWaypoint > 0L)
			return (tAvailStartTime - tAtWaypoint);
	}

	return 0L;
}

ElapsedTime AirRouteNetworkInSim::GetFirstDelayTimeAtWaypoint( AirsideFlightInSim* pFlight,const ElapsedTime& tCurTime )
{
	CRunwaySystem *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();
	std::vector<CRunwaySystem::RunwayOccupyInfo> vRunwayArrangeInfo;
	TimeRange timeRange(0L,0L);
	ElapsedTime tTime = tCurTime;
	pRunwaysController->ApplyForTakeoffTime(pFlight,15L,tTime, timeRange,vRunwayArrangeInfo);

	return timeRange.GetStartTime() - tTime;
}

bool AirRouteNetworkInSim::FindCircuitFlightLandingClearance( AirsideCircuitFlightInSim * pFlight, FlightRouteInSim* pFlightRoute,ClearanceItem& lastItem, Clearance& newClearance,AirsideCircuitFlightInSim::LandingOperation emType )
{
	if( pFlightRoute->GetItemCount() == 0)
		return false;

	AirsideResourceList resList;
	AirsideResource* pCurrentRes = lastItem.GetResource();

	if (pCurrentRes == NULL)	//birth
	{
		AirEntrySystemHoldInInSim* pEntryHold = pFlightRoute->GetEntrySystemHold();
		if( pEntryHold && pEntryHold->GetInQFlightCount() && !pEntryHold->isFlightInQ(pFlight)) //if the hold is not empty, and pFlight is not in hold,add pFlight to the hold and wait
		{
			pEntryHold->OnFlightEnter(pFlight,lastItem.GetTime() );				
			pEntryHold->AddFlightToQueue(pFlight);
			return true;
		}
		else if(pEntryHold && !pEntryHold->GetOccupyInstance(pFlight).IsValid() )
		{
			pEntryHold->AddFlightToQueue(pFlight);
			pEntryHold->OnFlightEnter(pFlight,lastItem.GetTime() );
		}

	}

	pFlightRoute->GetConcernRouteSegs(pCurrentRes,resList);

	if (resList.empty())
	{
		if (pCurrentRes->GetType() != AirsideResource::ResType_LogicRunway)	// only enroute
		{
			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(pFlight->GetTime());
			newClearance.AddItem(newItem);

			pFlight->PerformClearance(newClearance);
			return true;
		}

		return false;
	}

	if (pFlight->GetRunwayExit() == NULL)
	{
		pFlight->GetAirTrafficController()->AssignRunwayExit(pFlight);

		if (pFlight->GetRunwayExit() == NULL)		// no exit
		{
			CString strWarn;
			strWarn.Format("No available runway exit for the flight on %s!", pFlight->GetLandingRunway()->PrintResource()) ;
			CString strError = _T("DEFINE ERROR");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);	
			pFlight->GetLandingRunway()->RemoveElementOccupancyInfo(pFlight);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastItem.GetTime());
			newClearance.AddItem(newItem);
			pFlight->PerformClearance(newClearance);
			return true;
		}
	}

	if ( CheckIsFlightNeedWaitingInHold(pFlight,resList, lastItem))
	{
		if(pFlight->WaitingInHold()== false)//cannot hold the flight, terminate it
		{
			CString strError = "Flight cannot land for there has no hold in the STAR to hold the flight";
			CString strErrorType = "AIRCRAFT TERMINATE";
			AirsideSimErrorShown::SimWarning(pFlight,strError,strErrorType);

			pFlight->GetLandingRunway()->RemoveElementOccupancyInfo(pFlight);

			ClearanceItem newItem(NULL,OnTerminate,0);
			newItem.SetTime(lastItem.GetTime());
			newClearance.AddItem(newItem);

			pFlight->PerformClearance(newClearance);
		}
		return true;
	}

	if (pCurrentRes)
	{
		if (lastItem.GetMode() == OnFinalApproach)		//at miss approach point to check whether runway is cleared
		{
			if (pFlight->GetLandingRunway()->IsRunwayClear(pFlight))//available to takeoff
			{
				CRunwaySystem *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();
				FlightRouteInSim* pTakeoffCircuitRoute = pFlight->GetTakeoffCircuit();
				if (emType != AirsideCircuitFlightInSim::ForceAndGo_Operation&&CheckIsFlightCanTakeoffOperation(pFlight,pTakeoffCircuitRoute,lastItem,newClearance))
				{
					if (emType == AirsideCircuitFlightInSim::LowAndOver_Operation)
					{
						FlightRouteInSim* pCircuitRoute = pFlight->GetLandingCircuit();
						if (pCircuitRoute->IsEmpty())
						{
							CString strError = "Flight cannot land and there is no available circuit air route to re-enter";
							CString strErrorType = "AIRCRAFT TERMINATE";
							AirsideSimErrorShown::SimWarning(pFlight,strError,strErrorType);

							pFlight->GetLandingRunway()->RemoveElementOccupancyInfo(pFlight);

							ClearanceItem newItem(NULL,OnTerminate,0);
							newItem.SetTime(lastItem.GetTime());
							newClearance.AddItem(newItem);	

							pFlight->PerformClearance(newClearance);
							return true;
						}
					}
					return false;
				}
			
				return false;
			}
			else
			{
				pFlight->GetLandingRunway()->SetHasMissedApproachFlight();

				FlightRouteInSim* pAirRoute = GetMisApproachRoute(pFlight);
				if (pAirRoute->IsEmpty())
				{
					CString strError = "Flight cannot land and there is no available Miss Approach air route to re-enter STAR";
					CString strErrorType = "AIRCRAFT TERMINATE";
					AirsideSimErrorShown::SimWarning(pFlight,strError,strErrorType);

					pFlight->GetLandingRunway()->RemoveElementOccupancyInfo(pFlight);

					ClearanceItem newItem(NULL,OnTerminate,0);
					newItem.SetTime(lastItem.GetTime());
					newClearance.AddItem(newItem);	

					pFlight->PerformClearance(newClearance);
					return true;
				}

				AirsideFlightInSim* pConflictFlt = pFlight->GetLandingRunway()->GetLastInResourceFlight();
				pFlight->StartDelay(lastItem,pConflictFlt,FlightConflict::MISSAPPROACH,FlightConflict::RUNWAY,FltDelayReason_AirHold,_T("Missed approach"));	//Miss approach delay
				GetMissApproachClearance(pFlight,pAirRoute, lastItem,newClearance);
				return true;
			}

		}
		AirspaceHoldInSim* pHold = NULL;
		if (pCurrentRes->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			pHold = ((AirRouteSegInSim*)pCurrentRes)->GetConnectedHold();
			//need judge whether need waiting in hold at an ATC point in the segment, for smooth fling to enter the hold
			//if flight at ATC managed point in the segment, give it next clearance directly,because the clearance of next waypoint already be calculated when flight prepare enter the segment
			if (pHold != NULL && 
				((AirRouteSegInSim*)pCurrentRes)->GetSecondConcernIntersection() != NULL &&
				((AirRouteSegInSim*)pCurrentRes)->GetATCManagedPoint(pFlight) == pFlight->GetPosition())		
			{
				AirRoutePointInSim* pSecondIntersection = ((AirRouteSegInSim*)pCurrentRes)->GetSecondConcernIntersection()->getInputPoint();
				FlightPlanPropertyInSim Pointprop(NULL);
				ElapsedTime tSegTime = 0L;
				Pointprop = pFlightRoute->GetRoutePointProperty(pSecondIntersection);
				tSegTime = pFlightRoute->GetFlightTimeInSeg(pFlight,(AirRouteSegInSim*)pCurrentRes);
				
		
				ElapsedTime tLastWaypointTime = ((AirRouteSegInSim*)pCurrentRes)->GetFirstConcernIntersection()->getInputPoint()->GetOccupyInstance(pFlight).GetEnterTime();
				ClearanceItem nextItem(pCurrentRes,Pointprop.GetMode(),0);
				nextItem.SetPosition(pSecondIntersection->GetPosition());
				nextItem.SetSpeed(Pointprop.GetSpeed());
				nextItem.SetTime(tLastWaypointTime + tSegTime);

				if (pHold->GetHeadFlightCount(pFlight) >0)		// there are flights holding 
				{
					nextItem.SetAltitude(pHold->getAvailableHeight(pFlight));
					pHold->AddHoldingFlight(pFlight);
					newClearance.AddItem(nextItem);
					return true;
				}	

				CRunwaySystem *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();
				ClearanceItem preItem = nextItem;
				AirsideResource* pRes = resList.back();
				if ( pRes->GetType() == AirsideResource::ResType_LogicRunway)		//landing phase
				{
					bool bDelay = false;
					ElapsedTime tDelay = -1L;

					std::vector<CRunwaySystem::RunwayOccupyInfo> vRunwayArrangeInfo;
					TimeRange eLandingTimeRange(ElapsedTime(0L),ElapsedTime(0L));

					if (IsFlightNeedDelayLandingOnRwy(pFlight,preItem,tDelay))
						bDelay = true;
					else
					{
						ElapsedTime eTotalTime = CalculateOptimisticTimeThrouhRoute(pFlight,pFlightRoute,resList);

						if(!pRunwaysController->ApplyForLandingTime(pFlight,preItem.GetTime() + eTotalTime,eLandingTimeRange,vRunwayArrangeInfo))
							bDelay = true;
					}

					if(bDelay)
					{
						nextItem.SetAltitude(pHold->getAvailableHeight(pFlight));
						pFlight->PerformClearanceItem(nextItem);

						pFlight->GetLandingRunway()->GetRunwayInSim()->AddWaitingFlight(pFlight);
						pFlight->StartDelay(nextItem, NULL, FlightConflict::HOLD_AIR, FlightConflict::RUNWAY,FltDelayReason_AirHold,_T("Holding delay"));	//holding delay
						pFlight->WaitingInHold(tDelay + pFlight->GetTime());

						return true;
					}


					try
					{
						FlightSchemeOnAirRouteLanding(pFlight,pFlightRoute,resList,preItem,lastItem,eLandingTimeRange,preItem.GetTime(),vRunwayArrangeInfo);
					}
					catch (FlightPlanConflictFlightPerformanceException& e)
					{
						AirsideSimErrorShown::SimWarning(pFlight,e.GetExcepDesc(),e.GetExcepType());

						newClearance.ClearAllItems();
						ClearanceItem newItem(NULL,OnTerminate,0);
						newItem.SetTime(lastItem.GetTime());
						newClearance.AddItem(newItem);	
						pFlight->PerformClearance(newClearance);
						return true;	

					}

				}
				else	//enroute or terminal part 
				{
					FlightSchemeOnAirRoute(pFlight,pFlightRoute,resList,lastItem,lastItem);
				}

				//to get altitude
				if (nextItem.GetTime() == pSecondIntersection->GetOccupyInstance(pFlight).GetEnterTime())		//without holding time
				{
					nextItem.SetAltitude(Pointprop.GetAlt());
					pFlight->PerformClearanceItem(nextItem);
					{
						ReleaseResourceLockEvent * pReleseEvent = new ReleaseResourceLockEvent(pFlight, pHold);
						pReleseEvent->setTime(nextItem.GetTime());
						pReleseEvent->addEvent();			
					}
					GenerateFlightClearance(pFlight, pFlightRoute, resList, nextItem, newClearance);

					if ( pRes->GetType() == AirsideResource::ResType_LogicRunway)
					{
						Clearance runwayClearence;

						LogicRunwayInSim* pPort = pFlight->GetLandingRunway();
						double landspd = pFlight->GetPerformance()->getLandingSpeed(pFlight);	
						DistanceUnit touchDist = pFlight->GetTouchDownDistAtRuwnay(pPort);

						//landing item
						ClearanceItem landingItem(pPort,OnLanding,touchDist);		
						landingItem.SetSpeed(landspd);
						landingItem.SetPosition(pPort->GetFlightTouchdownPoint(pFlight));
						landingItem.SetTime(pPort->GetOccupyInstance(pFlight).GetEnterTime());
						pRunwaysController->WriteRunwayLogs(pFlight,true,landingItem,runwayClearence);		
					}
				}
				else									//need holding
				{				
					nextItem.SetAltitude(pHold->getAvailableHeight(pFlight));
					pFlight->PerformClearanceItem(nextItem);
					pFlight->StartDelay(nextItem, NULL, FlightConflict::HOLD_AIR, FlightConflict::BETWEENWAYPOINTS,FltDelayReason_AirHold,_T("Holding delay"));	//holding delay
					pFlight->WaitingInHold(pSecondIntersection->GetOccupyInstance(pFlight).GetEnterTime());
				}

				return true;
			}

		}
	}

	try
	{

		bool bEnterSystem  = FlightPrearrangeOnAirRoute(pFlight,pFlightRoute,resList,lastItem,newClearance);

		if(bEnterSystem && newClearance.GetItemCount() && pFlight->GetMode() == OnBirth) // flight get the time enter the system
		{
			AirEntrySystemHoldInInSim* pEntryHold = pFlightRoute->GetEntrySystemHold();
			if (pEntryHold == NULL)
				return true;

			ElapsedTime tEnterSysTime = newClearance.GetItem(0).GetTime();
			pEntryHold->writeFlightLog(pFlight,tEnterSysTime);
			CFlightDelayOnHoldEvent* pHoldDelayEvent = new CFlightDelayOnHoldEvent(pFlight,pEntryHold);

			pHoldDelayEvent->setTime(tEnterSysTime);
			pHoldDelayEvent->addEvent();
		}
		if ( !bEnterSystem)
			newClearance.ClearItems();
	}
	catch (AirsideACTypeMatchError* pError)
	{
		AirsideDiagnose* pDiagnose =static_cast<AirsideDiagnose*>(pError->GetDiagnose());
		CString& strErrorType = pDiagnose->GetErrorType();
		CString& strError = pDiagnose->GetDetails();
		AirsideSimErrorShown::SimWarning(pFlight,strError,strErrorType);

		delete pError;
		pError = NULL;

		ClearanceItem newItem(NULL,OnTerminate,0);
		newItem.SetTime(pFlight->GetTime());
		newClearance.AddItem(newItem);

		pFlight->PerformClearance(newClearance);
	}

	return true;
}

bool AirRouteNetworkInSim::FindCircuitFlightTakeoffClearance( AirsideCircuitFlightInSim * pFlight, FlightRouteInSim* pFlightRoute,ClearanceItem& lastItem, Clearance& newClearance,AirsideCircuitFlightInSim::LandingOperation emType)
{
	if( pFlightRoute->GetItemCount() == 0)
		return false;

	AirsideResourceList resList;
	AirsideResource* pCurrentRes = lastItem.GetResource();
	if (pCurrentRes == NULL)
		return false;

	AirRouteInSim* pAirRouteInSim = pFlightRoute->GetAirRouteInSim();
	
	pFlightRoute->GetConcernCircuitRouteSegs(pCurrentRes,resList);
	
	
	if (resList.empty())
	{
		return false;
	}

	if (pCurrentRes)
	{
		AirspaceHoldInSim* pHold = NULL;
		if (pCurrentRes->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			pHold = ((AirRouteSegInSim*)pCurrentRes)->GetConnectedHold();
			//need judge whether need waiting in hold at an ATC point in the segment, for smooth fling to enter the hold
			//if flight at ATC managed point in the segment, give it next clearance directly,because the clearance of next waypoint already be calculated when flight prepare enter the segment
			if (pHold != NULL && 
				((AirRouteSegInSim*)pCurrentRes)->GetSecondConcernIntersection() != NULL &&
				((AirRouteSegInSim*)pCurrentRes)->GetATCManagedPoint(pFlight) == pFlight->GetPosition())		
			{

				AirRoutePointInSim* pSecondIntersection = ((AirRouteSegInSim*)pCurrentRes)->GetSecondConcernIntersection()->getInputPoint();
				FlightPlanPropertyInSim Pointprop(NULL);
				ElapsedTime tSegTime = 0L;
				Pointprop = pFlightRoute->GetRoutePointProperty(pSecondIntersection);
				tSegTime =pFlightRoute->GetFlightTimeInSeg(pFlight,(AirRouteSegInSim*)pCurrentRes);


				ElapsedTime tLastWaypointTime = ((AirRouteSegInSim*)pCurrentRes)->GetFirstConcernIntersection()->getInputPoint()->GetOccupyInstance(pFlight).GetEnterTime();
				ClearanceItem nextItem(pCurrentRes,Pointprop.GetMode(),0);
				nextItem.SetPosition(pSecondIntersection->GetPosition());
				nextItem.SetSpeed(Pointprop.GetSpeed());
				nextItem.SetTime(tLastWaypointTime + tSegTime);

				if (pHold->GetHeadFlightCount(pFlight) >0)		// there are flights holding 
				{
					nextItem.SetAltitude(pHold->getAvailableHeight(pFlight));
					pHold->AddHoldingFlight(pFlight);
					newClearance.AddItem(nextItem);
					return true;
				}	

				CRunwaySystem *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();
				ClearanceItem preItem = nextItem;
				AirsideResource* pRes = resList.back();
				
				FlightSchemeOnAirRoute(pFlight,pFlightRoute,resList,lastItem,lastItem);
				

				//to get altitude
				if (nextItem.GetTime() == pSecondIntersection->GetOccupyInstance(pFlight).GetEnterTime())		//without holding time
				{
					nextItem.SetAltitude(Pointprop.GetAlt());
					pFlight->PerformClearanceItem(nextItem);
					{
						ReleaseResourceLockEvent * pReleseEvent = new ReleaseResourceLockEvent(pFlight, pHold);
						pReleseEvent->setTime(nextItem.GetTime());
						pReleseEvent->addEvent();			
					}
					GenerateFlightClearance(pFlight, pFlightRoute, resList, nextItem, newClearance);
				}
				else									//need holding
				{				
					nextItem.SetAltitude(pHold->getAvailableHeight(pFlight));
					pFlight->PerformClearanceItem(nextItem);
					pFlight->StartDelay(nextItem, NULL, FlightConflict::HOLD_AIR, FlightConflict::BETWEENWAYPOINTS,FltDelayReason_AirHold,_T("Holding delay"));	//holding delay
					pFlight->WaitingInHold(pSecondIntersection->GetOccupyInstance(pFlight).GetEnterTime());
				}

				return true;
			}

		}
	}

	try
	{

		bool bEnterSystem  = CircuitFlightTakeoffOnAirRoute(pFlight,pFlightRoute,resList,lastItem,newClearance,emType);

		if ( !bEnterSystem)
			newClearance.ClearItems();
	}
	catch (AirsideACTypeMatchError* pError)
	{
		AirsideDiagnose* pDiagnose =static_cast<AirsideDiagnose*>(pError->GetDiagnose());
		CString& strErrorType = pDiagnose->GetErrorType();
		CString& strError = pDiagnose->GetDetails();
		AirsideSimErrorShown::SimWarning(pFlight,strError,strErrorType);

		delete pError;
		pError = NULL;

		ClearanceItem newItem(NULL,OnTerminate,0);
		newItem.SetTime(pFlight->GetTime());
		newClearance.AddItem(newItem);

		pFlight->PerformClearance(newClearance);
	}

	return true;
}

bool AirRouteNetworkInSim::CheckIsFlightCanTakeoffOperation( AirsideCircuitFlightInSim * pFlight, FlightRouteInSim* pFlightRoute,ClearanceItem& lastItem, Clearance& newClearance )
{
	if (pFlightRoute == NULL)
		return true;
	
	if( pFlightRoute->GetItemCount() == 0)
		return true;

	AirsideResourceList resList;
	AirsideResource* pCurrentRes = lastItem.GetResource();

	ClearanceItem currentItem = lastItem;

	pFlightRoute->GetConcernCircuitRouteSegs(pCurrentRes,resList);

	if ( resList.at(0)->GetType() == AirsideResource::ResType_LogicRunway)	//take off	
	{	
		CRunwaySystem *pRunwaysController =  pFlight->GetAirTrafficController()->GetRunwaysController();
		std::vector<CRunwaySystem::RunwayOccupyInfo> vRunwayArrangeInfo;
		vRunwayArrangeInfo.clear();
		bool bNeedCreateTakeOffEvent = false;

		ElapsedTime dTimeToRwy;
		AirsideResource * pRes = currentItem.GetResource(); // add at the end of the taxiway item
		if (pRes)
		{
			if(pRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
			{
				CPath2008 filletPath = pFlight->GetFilletPathFromTaxiwayToRwyport(pFlight->GetAndAssignTakeoffRunway()->getLogicRunwayType());
				MobileTravelTrace travelTrace(pFlight->mGroundPerform,filletPath.GetTotalLength(),currentItem.GetSpeed(),0 );
				travelTrace.BeginUse();
				dTimeToRwy = travelTrace.getEndTime();
			}
			else if (pRes->GetType() == AirsideResource::ResType_AirRouteSegment)
			{
				ClearanceItem landingItem = pRunwaysController->GetRunwayTouchdownClearanceItem(pFlight,currentItem.GetTime(),currentItem);
				dTimeToRwy = landingItem.GetTime() - currentItem.GetTime();
			}
		}
	

		ElapsedTime tMoveTime =0L;
		bool bTakeoff = FlightSchemeOnAirRouteTakeOff(pFlight, pFlightRoute, resList, currentItem, dTimeToRwy, tMoveTime, vRunwayArrangeInfo);

		if (!bTakeoff)
			return false;

		if (lastItem.GetTime() < tMoveTime)
		return false;
	}
	return true;
}

AirspaceIntrailSeparationSpeed::AirspaceIntrailSeparationSpeed( AirsideResource* pCurrentRes,FlightRouteInSim* pAirRouteInSim )
:m_pCurrentRes(pCurrentRes)
,m_pAirRouteInSim(pAirRouteInSim)
{

}

AirspaceIntrailSeparationSpeed::~AirspaceIntrailSeparationSpeed()
{

}

double AirspaceIntrailSeparationSpeed::GetSparationSpeed( AirsideFlightInSim* pFlight,double dDist )const
{
	if (m_pAirRouteInSim == NULL)
		return 0.0;

	if (m_pCurrentRes == NULL)
		return 0.0;

	if (pFlight->IsArrivingOperation())
	{
		return GetLandingSparationSpeed(pFlight,dDist);
	}

	return GetTakeoffSparationSpeed(pFlight,dDist);
}

double AirspaceIntrailSeparationSpeed::GetSegmentSparationTime( AirsideFlightInSim* pFlight,AirRouteSegInSim* pSeg,double dDist ) const
{
	int nRouteCount = pSeg->GetRoutePointCount();
	double tNormal = 0.0;
	DistanceUnit dist = 0.0;
	if (pSeg->GetSecondConcernIntersection())
	{
		AirRoutePointInSim* pSecondIntersection = pSeg->GetSecondConcernIntersection()->getInputPoint();
		FlightPlanPropertyInSim planProperty = m_pAirRouteInSim->GetRoutePointProperty(pSecondIntersection);
		if (nRouteCount > 0)
		{
			AirRoutePointInSim* pPoint = pSeg->GetRoutePointByIdx(nRouteCount-1);
			FlightPlanPropertyInSim planProperty1 = m_pAirRouteInSim->GetRoutePointProperty(pPoint);
			dist = pSecondIntersection->GetPosition().distance(pPoint->GetPosition());
			if (dist >= dDist)
			{
				DynamicMovement dyNamicMove(planProperty.GetSpeed(),planProperty1.GetSpeed(),dist);
				double dTime = dyNamicMove.GetDistTime(dDist).getPrecisely()/100.0;
				return tNormal + dTime;
			}
			tNormal += 2.0*dist/(planProperty.GetSpeed()+planProperty1.GetSpeed());
			dDist -= dist;
		}
		else
		{   
			AirRoutePointInSim* pPoint = pSeg->GetFirstConcernIntersection()->getInputPoint();
			FlightPlanPropertyInSim planProperty1 = m_pAirRouteInSim->GetRoutePointProperty(pPoint);
			dist = pSecondIntersection->GetPosition().distance(pPoint->GetPosition());
			if (dist >= dDist)
			{
				DynamicMovement dyNamicMove(planProperty.GetSpeed(),planProperty1.GetSpeed(),dist);
				double dTime = dyNamicMove.GetDistTime(dDist).getPrecisely()/100.0;
				return tNormal + dTime;
			}
			tNormal += 2.0*dist/(planProperty.GetSpeed()+planProperty1.GetSpeed());	
			dDist -= dist;
		}
	}

	for (int i = nRouteCount - 2; i >= 0; i--)
	{
		AirRoutePointInSim* pPoint1 = pSeg->GetRoutePointByIdx(i);
		AirRoutePointInSim* pPoint2 = pSeg->GetRoutePointByIdx(i+1);
		FlightPlanPropertyInSim planProperty1 = m_pAirRouteInSim->GetRoutePointProperty(pPoint1);
		FlightPlanPropertyInSim planProperty2 = m_pAirRouteInSim->GetRoutePointProperty(pPoint2);

		DistanceUnit dist = pPoint1->GetPosition().distance(pPoint2->GetPosition());
		if (dist >= dDist)
		{
			DynamicMovement dyNamicMove(planProperty2.GetSpeed(),planProperty1.GetSpeed(),dist);
			double dTime = dyNamicMove.GetDistTime(dDist).getPrecisely()/100.0;
			return tNormal + dTime;
		}
		dDist -= dist;
		tNormal += 2.0*dist/(planProperty1.GetSpeed() + planProperty2.GetSpeed());		
	}

	if(pSeg->GetFirstConcernIntersection())
	{
		AirRoutePointInSim* pFirstIntersection = pSeg->GetFirstConcernIntersection()->getInputPoint();
		FlightPlanPropertyInSim planProperty = m_pAirRouteInSim->GetRoutePointProperty(pFirstIntersection);
		if (nRouteCount > 0)
		{
			AirRoutePointInSim* pPoint = pSeg->GetRoutePointByIdx(0);
			FlightPlanPropertyInSim planProperty1 = m_pAirRouteInSim->GetRoutePointProperty(pPoint);
			DistanceUnit dist = pFirstIntersection->GetPosition().distance(pPoint->GetPosition());
			if (dist >= dDist)
			{
				DynamicMovement dyNamicMove(planProperty1.GetSpeed(),planProperty.GetSpeed(),dist);
				double dTime = dyNamicMove.GetDistTime(dDist).getPrecisely()/100.0;
				return tNormal + dTime;
			}
			tNormal += 2.0*dist/(planProperty.GetSpeed() +planProperty1.GetSpeed());
		}
	}
	return tNormal;
}

double AirspaceIntrailSeparationSpeed::GetLandingSparationSpeed( AirsideFlightInSim* pFlight,double dDist ) const
{
	double dMoveDist = dDist;
	double tSparationTime = 0.0;

	int idx = m_pAirRouteInSim->GetSegmentIndex(m_pCurrentRes);
	if (idx == -1)
		return 0.0;

	int nCount = m_pAirRouteInSim->GetItemCount();
	if (idx >= nCount)
		return 0.0;

	for (int i = idx; i >= 0; i--)
	{
		AirsideResource* pRes = m_pAirRouteInSim->GetItem(i);

		if (pRes->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			AirRouteSegInSim* pSeg = (AirRouteSegInSim*)pRes;
			double dSegDist = pSeg->GetSegmentDistance();

			if (dSegDist > 0)
			{
				if (dSegDist >= dDist)
				{
					double dTime = GetSegmentSparationTime(pFlight,pSeg,dDist);
					tSparationTime += dTime;
					if (tSparationTime > (std::numeric_limits<double>::min)())
						return dMoveDist/tSparationTime;
					return 0.0;
				}
				else 
				{
					ElapsedTime tSegTime = m_pAirRouteInSim->GetFlightTimeInSeg(pFlight,pSeg);
					double dSegTime = tSegTime.getPrecisely()/100.0;
					tSparationTime += dSegTime;
					dDist -= dSegDist;
				}
			}
		}

		if (pRes->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			HeadingAirRouteSegInSim* pSeg = (HeadingAirRouteSegInSim*)pRes;
			double dSegDist = pSeg->GetExpectDistance();
			if (dSegDist > 0)
			{
				if (dSegDist >= dDist)
				{
					double dTime = pSeg->GetDistExpectTimeInSegment(m_pAirRouteInSim,pFlight,dDist).getPrecisely()/100.0;
					tSparationTime += dTime;
					if (tSparationTime > (std::numeric_limits<double>::min)())
						return dMoveDist/tSparationTime;
					return 0.0;
				}
				else 
				{
					ElapsedTime tSegTime = pSeg->GetExpectTimeInSegment(m_pAirRouteInSim,pFlight);
					double dSegTime = tSegTime.getPrecisely()/100.0;
					tSparationTime += dSegTime;
					dDist -= dSegDist;
				}
			}
		}

		if (pRes->GetType() == AirsideResource::ResType_LogicRunway)
		{
			double dSegDist = m_pAirRouteInSim->CalculateDistanceFromLastSegToLogicRunway(pFlight);
			if (dSegDist > 0)
			{
				if (dSegDist < dDist)
				{
					ElapsedTime tSegTime = m_pAirRouteInSim->CalculateTimeFromLastSegToLogicRunway(pFlight);
					double dSegTime = tSegTime.getPrecisely()/100.0;
					tSparationTime += dSegTime;
					dDist -= dSegDist;
				}
				else
				{
					AirRouteSegInSim *pLastSegmentInSim = (AirRouteSegInSim*)m_pAirRouteInSim->GetItem(nCount-2);
					AirRouteIntersectionInSim* pLastFirstConflictIntersection = pLastSegmentInSim->GetFirstConcernIntersection();
					double dSpeed = m_pAirRouteInSim-> GetRoutePointProperty(pLastFirstConflictIntersection->getInputPoint()).GetSpeed();

					double dAvgSpeed =pFlight->GetPerformance()->getLandingSpeed(pFlight);

					DynamicMovement dyNamicMove(dAvgSpeed,dSpeed,dSegDist);
					tSparationTime += dyNamicMove.GetDistTime(dDist).getPrecisely()/100.0;
					if (tSparationTime > (std::numeric_limits<double>::min)())
						return dMoveDist/tSparationTime;
					return 0.0;
				}
			}
		}
	}

	if (nCount)
	{
		AirsideResource* pRes = m_pAirRouteInSim->GetItem(0);
		if (pRes->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			AirRouteSegInSim* pSeg = (AirRouteSegInSim*)pRes;
			if(pSeg->GetFirstConcernIntersection())
			{
				AirRoutePointInSim* pFirstIntersection = pSeg->GetFirstConcernIntersection()->getInputPoint();
				FlightPlanPropertyInSim planProperty = m_pAirRouteInSim->GetRoutePointProperty(pFirstIntersection);

				double dSpeed = planProperty.GetSpeed();
				tSparationTime += (dDist/dSpeed);
				if (tSparationTime > (std::numeric_limits<double>::min)())
					return dMoveDist/tSparationTime;
				return 0.0;
			}
		}

		if (pRes->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			HeadingAirRouteSegInSim* pSeg = (HeadingAirRouteSegInSim*)pRes;
			if(pSeg->GetFirstIntersection())
			{
				AirRoutePointInSim* pFirstIntersection = pSeg->GetFirstIntersection()->getInputPoint();
				FlightPlanPropertyInSim planProperty = m_pAirRouteInSim->GetRoutePointProperty(pFirstIntersection);

				double dSpeed = planProperty.GetSpeed();
				if (dSpeed > (std::numeric_limits<double>::min)())
				{
					tSparationTime += (dDist/dSpeed);
				}
				if (tSparationTime > (std::numeric_limits<double>::min)())
					return dMoveDist/tSparationTime;
				return 0.0;
			}
		}
	}

	return 0.0;
}

double AirspaceIntrailSeparationSpeed::GetTakeoffSparationSpeed( AirsideFlightInSim* pFlight,double dDist ) const
{
	double dMoveDist = dDist;
	double tSparationTime = 0.0;

	int idx = m_pAirRouteInSim->GetSegmentIndex(m_pCurrentRes);
	if (idx == -1)
		return 0.0;

	int nCount = m_pAirRouteInSim->GetItemCount();
	if (idx >= nCount)
		return 0.0;

	for (int i = idx; i >= 0; i--)
	{
		AirsideResource* pRes = m_pAirRouteInSim->GetItem(i);

		if (pRes->GetType() == AirsideResource::ResType_AirRouteSegment)
		{
			AirRouteSegInSim* pSeg = (AirRouteSegInSim*)pRes;
			double dSegDist = pSeg->GetSegmentDistance();

			if (dSegDist > 0)
			{
				if (dSegDist >= dDist)
				{
					double dTime = GetSegmentSparationTime(pFlight,pSeg,dDist);
					tSparationTime += dTime;
					if (tSparationTime > (std::numeric_limits<double>::min)())
						return dMoveDist/tSparationTime;
					return 0.0;
				}
				else 
				{
					ElapsedTime tSegTime = m_pAirRouteInSim->GetFlightTimeInSeg(pFlight,pSeg);
					double dSegTime = tSegTime.getPrecisely()/100.0;
					tSparationTime += dSegTime;
					dDist -= dSegDist;
				}
			}
		}
		if (pRes->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			HeadingAirRouteSegInSim* pSeg = (HeadingAirRouteSegInSim*)pRes;
			double dSegDist = pSeg->GetExpectDistance();
			if (dSegDist > 0)
			{
				if (dSegDist >= dDist)
				{
					double dTime = pSeg->GetDistExpectTimeInSegment(m_pAirRouteInSim,pFlight,dDist).getPrecisely()/100.0;
					tSparationTime += dTime;
					if (tSparationTime > (std::numeric_limits<double>::min)())
						return dMoveDist/tSparationTime;
					return 0.0;
				}
				else 
				{
					ElapsedTime tSegTime = pSeg->GetExpectTimeInSegment(m_pAirRouteInSim,pFlight);
					double dSegTime = tSegTime.getPrecisely()/100.0;
					tSparationTime += dSegTime;
					dDist -= dSegDist;
				}
			}
		}
		if (pRes->GetType() == AirsideResource::ResType_LogicRunway)
		{
			double dSpeed =  pFlight->GetPerformance()->getClimboutSpeed(pFlight);

			if (dSpeed > (std::numeric_limits<double>::min)())
			{
				tSparationTime += (dDist/dSpeed);
			}
			if (tSparationTime > (std::numeric_limits<double>::min)())
				return dMoveDist/tSparationTime;
			return 0.0;
		}
	}

	return 0.0;
}