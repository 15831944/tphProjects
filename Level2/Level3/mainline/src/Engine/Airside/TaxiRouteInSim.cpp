#include "StdAfx.h"
#include "TaxiRouteInSim.h"
#include "AirsideFlightInSim.h"
#include "Clearance.h"
#include "FlightPerformanceManager.h"
#include "AirTrafficController.h"
#include "TaxiwayResource.h"
#include "./ResourceManager.h"
#include "./AirspaceResourceManager.h"
#include "./AirportResourceManager.h"
#include "AirsideResourceManager.h"
#include "TaxiwayConflictResolutionInEngine.h"
#include "../MobileDynamics.h"
#include "ReleaseResourceLockEvent.h"
#include "StandLeadOutLineInSim.h"
#include "StandLeadInLineInSim.h"
#include "./TempParkingSegmentsInSim.h"
#include "./TaxiwayDirectionLockDelay.h"
#include "../../Results/AirsideFlightEventLog.h"
#include "DynamicConflictGraph.h"
#include "..\MobileDynamics.h"
#include ".\FlightGetClearanceEvent.h"
#include "AirsideMeetingPointInSim.h"
#include "QueueToTakeoffPos.h"
#include <Common/ARCTracker.h>
#include "FlightPerformancesInSim.h"

//#include "FlightInTaxiRouteSeperation.h"
#include "RunwaySegInSim.h"
const static DistanceUnit minTravelDist = 100;

#define _DEBUGWAVECROSSLOG 0


#define _DEBUGLOG 0
#if _DEBUGLOG
#include "../../Common/FileOutPut.h"

#ifdef _DEBUG
const CString ConflictDebugFile = "C:\\taxiconflict\\taxiwaydebug.log";
#else
const CString ConflictDebugFile = "C:\\taxiconflict\\taxiwayrelease.log";
#endif

#endif
//////////////////////////////////////////////////////////////////////////
TaxiRouteItemInSim::TaxiRouteItemInSim( AirsideResource * pRes, DistanceUnit distF, DistanceUnit distT )
{
	m_pRes = pRes;
	m_distFrom = distF;
	m_distTo = distT;

	//ASSERT(m_distTo >= m_distFrom);
}

bool TaxiRouteItemInSim::IsDistInItem( DistanceUnit dist ) const
{
	return dist <= m_distTo && dist>= m_distFrom;
}

bool TaxiRouteItemInSim::operator==( const TaxiRouteItemInSim& oItem ) const
{
	return m_pRes==oItem.m_pRes;
}
//////////////////////////////////////////////////////////////////////////
//get the dist in Route in which item and the item dist
//
void TaxiRouteInSim::GetItemIndexAndDistInRoute(const DistanceUnit& dist, int& nItemIdx, DistanceUnit& distInItem)
{
	ASSERT( m_vdAccLength.size() == GetItemCount()+1 );
		
	if(dist < * m_vdAccLength.begin()){
		nItemIdx = -1;
		distInItem = -1;
		return;
	}	
	
	for(int i=0;i< (int)m_vdAccLength.size()-1;i++)
	{
		if( dist < m_vdAccLength[i+1] && dist >= m_vdAccLength[i] )
		{
			nItemIdx = i;
			distInItem = dist - m_vdAccLength[i];
			return;
		}
	}	

	nItemIdx = GetItemCount();
	distInItem = dist - *m_vdAccLength.rbegin();
	return;

}
// get the dist in route of the item and dist
DistanceUnit TaxiRouteInSim::GetDistInRoute( const int& nItemIdx , const DistanceUnit& distInItem) const
{
	ASSERT(m_vdAccLength.size() == GetItemCount()+1);
	if(nItemIdx < 0){
		return -1;
	}
	if(nItemIdx==0)
		return distInItem;

	if(nItemIdx >= GetItemCount() )
	{
		return *m_vdAccLength.rbegin() + distInItem; 
	}

	return m_vdAccLength[nItemIdx] + distInItem;
}

class MobileLeavePathEvent : public AirsideEvent
{
public:
public:
	MobileLeavePathEvent(DynamicConflictGraph* pConflictGraph, ARCMobileElement* pmob, RouteDirPath* pthePath,const ElapsedTime& t)
	{
		m_pConflictgraph = pConflictGraph;
		mpMob = pmob;
		mpPath = pthePath;
		pthePath->removeInPathMobile(pmob);
		setTime(t);
	}
	//It returns event's name
	virtual const char *getTypeName (void) const { return _T("Leave DirPath"); };

	//It returns event type
	virtual int getEventType (void) const { return -1; }


	int Process(){
		m_pConflictgraph->OnMobileLeave(mpMob,mpPath,getTime());
		return 1; 
	}
protected:
	ARCMobileElement* mpMob;
	RouteDirPath* mpPath;
	DynamicConflictGraph* m_pConflictgraph;

};

#include "../../Results/AirsideFlightNodeLog.h"
FlightCrossNodeLog* CreatFlightCrossNodeLog(AirsideFlightInSim* pFlt, HoldInTaxiRoute& pNodeHold,const ElapsedTime& atHoldTime,TaxiRouteInSim& theRoute)
{
	FlightCrossNodeLog* pNewLog = new FlightCrossNodeLog;
	pNewLog->time = atHoldTime;
	pNewLog->mNodeId = pNodeHold.m_pNode->GetID();
	pNewLog->mNodeName  = pNodeHold.m_pNode->GetNodeInput().GetName();
	pNewLog->mFlightMode = pFlt->IsArrivingOperation()?'A':'D';
	pNewLog->mdDist = pNodeHold.m_dDistInRoute;
	
	
	pNewLog->mEnterStand = 0;
	
	

	
	pNewLog->mEnterStand = 0;
	
	

	//To Node
	IntersectionNodeInSim* pNextNode = NULL;
	if(pNodeHold.mLinkDirSeg)
		pNextNode = pNodeHold.mLinkDirSeg->GetExitNode();
	if(pNextNode)
	{
		pNewLog->mNodeToId = pNextNode->GetID();
		pNewLog->mNodeToName = pNextNode->PrintResource();
	}
	//from node
	IntersectionNodeInSim* pPreNode = NULL;
	if(pNodeHold.m_pDirSeg)
		pPreNode= pNodeHold.m_pDirSeg->GetEntryNode();
	if(pPreNode)
	{
		pNewLog->mNodeFromId = pPreNode->GetID();
		pNewLog->mNodeFromName = pPreNode->PrintResource();
	}
	
	switch(pNodeHold.m_hHoldType)
	{
	case HoldInTaxiRoute::ENTRYNODE:
		pNewLog->meType=FlightCrossNodeLog::IN_NODE;
		break;
	case HoldInTaxiRoute::EXITNODE:
		pNewLog->meType = FlightCrossNodeLog::OUT_NODE;
		break;
	case HoldInTaxiRoute::EXITNODE_BUFFER:
		pNewLog->meType = FlightCrossNodeLog::OUT_NODEBUFFER;
		break;
	default:
		ASSERT(FALSE);
	}
	//get original to dest
	if(theRoute.mpResOrig){
		pNewLog->mGlobalOrigin = theRoute.mpResOrig->PrintResource();
	}
	if(theRoute.mpResDest)
	{
		pNewLog->mGlobalDest = theRoute.mpResDest->PrintResource();
	}
	return pNewLog;
}

FlightStartNodeDelayLog* createStartNodeDelayLog(AirsideFlightInSim* pFlight,IntersectionNodeInSim* pNode, const ElapsedTime& atTime, AirsideFlightInSim* pConflictFlt)
{
	FlightStartNodeDelayLog* pNewLog = new FlightStartNodeDelayLog();
	pNewLog->time = atTime;
	pNewLog->mNodeId = pNode->GetID();
	pNewLog->mNodeName  = pNode->GetNodeInput().GetName();
	pNewLog->mFlightMode = pFlight->IsArrivingOperation()?'A':'D';

	pNewLog->miConflictFlightID = pConflictFlt->GetUID();
	pNewLog->msConflictFlight = pConflictFlt->GetCurrentFlightID();
	CString acType; 
	pConflictFlt->getACType(acType.GetBuffer(255));
	pNewLog->msConflictFlightType = acType.GetString();
	
	//
	FlightGroundRouteDirectSegInSim* pDirSeg = NULL;
	if(pConflictFlt&& pConflictFlt->GetResource())
	{
		if(pConflictFlt->GetResource()->IsKindof(typeof(FlightGroundRouteDirectSegInSim)))
		{
			pDirSeg = (FlightGroundRouteDirectSegInSim*)pConflictFlt->GetResource();
		}
	}
	if(pDirSeg)
	{
		IntersectionNodeInSim* pFromNdoe = pDirSeg->GetEntryNode();
		IntersectionNodeInSim* pToNode = pDirSeg->GetExitNode();

		pNewLog->miConflictFltFromNodeID = pFromNdoe->GetID();
		pNewLog->msConflictFltFromNode = pFromNdoe->GetNodeInput().GetName();
		pNewLog->miConflictFltToNodeID = pToNode->GetID();
		pNewLog->msConflictFltToNode = pToNode->GetNodeInput().GetName();
	}
	
	TaxiRouteInSim*route = pConflictFlt->GetCurrentTaxiRoute();
	if(route)
	{
		if(route->mpResOrig){
			pNewLog->msConflictFltOrig = route->mpResOrig->PrintResource();
		}
		if(route->mpResDest)
		{
			pNewLog->msConflictFltDest = route->mpResDest->PrintResource();
		}
	}

	return pNewLog;
}

DynamicConflictGraph* getConflictGraph(AirsideFlightInSim* pFlt)
{
	return pFlt->GetAirTrafficController()->getConflictGraph();
}
//
void DoMoveToNextDistInRoute(MobileTravelTrace& travelTrace, const ElapsedTime& startTime, AirsideFlightInSim* pFlight,TaxiRouteInSim& theRoute,const DistanceUnit& curDist,  const DistanceUnit& nextDist)
{
	//entry exit hold
	int iEndIdx = theRoute.GetHoldList().GetHoldUntil(nextDist);	
	for(int i=0;i<(int)iEndIdx;i++ )
	{
		HoldInTaxiRoute& theHold = theRoute.GetHoldList().ItemAt(i);
		if(theHold.m_bCrossed)
			continue; //ignore crossed holds
		
		double dTravelDist = theHold.m_dDistInRoute - curDist;		
		ElapsedTime atHoldTime = startTime + travelTrace.getDistTime(dTravelDist);

		if( theHold.m_hHoldType == HoldInTaxiRoute::ENTRYNODE )
		{
			theHold.m_pNode->SetEnterTime(pFlight, atHoldTime,theRoute.GetMode());
			//enter runway log
			if (theHold.m_pNode->GetNodeInput().GetRunwayIntersectItemList().size()> 0)
			{
				RunwayIntersectItem* pRwItem = theHold.m_pNode->GetNodeInput().GetRunwayIntersectItemList()[0];
				int nRwID = pRwItem->GetObjectID();
				AirsideRunwayCrossigsLog* pEnterRunwayCrossingLog = new AirsideRunwayCrossigsLog();
				
				pEnterRunwayCrossingLog->speed = travelTrace.getDistSpeed(dTravelDist);
				if (theHold.m_pDirSeg->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
				{
					pEnterRunwayCrossingLog->sTaxiway = ((TaxiwayDirectSegInSim*)theHold.m_pDirSeg)->GetTaxiwaySeg()->m_pTaxiway->GetTaxiwayInput()->GetObjNameString();
				}
				pEnterRunwayCrossingLog->sNodeName = theHold.m_pNode->GetNodeInput().GetName();
				pEnterRunwayCrossingLog->time = atHoldTime.getPrecisely();
				pEnterRunwayCrossingLog->sRunway = pRwItem->GetNameString();
				pEnterRunwayCrossingLog->state = AirsideRunwayCrossigsLog::OnEnterRunway;
				pFlight->LogEventItem(pEnterRunwayCrossingLog);
			}

			getConflictGraph(pFlight)->OnFlightEnterNode(pFlight,theHold.m_pNode, atHoldTime);
			getConflictGraph(pFlight)->OnMobileEnter(pFlight,theHold.mLinkDirSeg,atHoldTime);
		}		


		if( theHold.m_hHoldType == HoldInTaxiRoute::EXITNODE_BUFFER  )
		{
			double dTravelDist = theHold.m_dDistInRoute - curDist;		
			ElapsedTime atHoldTime = startTime + travelTrace.getDistTime(dTravelDist);

			if(!theHold.m_pNode->GetOccupyInstance(pFlight).IsExitTimeValid())
				theHold.m_pNode->SetExitTime(pFlight, atHoldTime );	

			if(theHold.m_pNode->GetLockedFlight() == pFlight)
			{
				getConflictGraph(pFlight)->OnFlightExitNode(pFlight,theHold.m_pNode,atHoldTime);
				//Flight crossed runway
				if(pFlight->GetWaitRunwayHold() && pFlight->GetWaitRunwayHold()->GetNode() == theHold.m_pNode )
				{
					ElapsedTime eCurTime = pFlight->GetTime();
					ElapsedTime eNotifyTime =  atHoldTime;//theHold.m_pNode->GetOccupyInstance(pFlight).GetEnterTime();
					if(eNotifyTime >= ElapsedTime(0L))
						pFlight->SetTime(eNotifyTime);//notify landing or take off runway
					pFlight->QuitCrossRunwayQueue();
					pFlight->SetTime(eCurTime);
				}	
			}

			if(theHold.mLinkDirSeg && theHold.mLinkDirSeg->bMobileInPath(pFlight))
			{				
				(new MobileLeavePathEvent(getConflictGraph(pFlight),pFlight,theHold.mLinkDirSeg,atHoldTime))->addEvent();				
			}			
			if (theHold.m_pNode->GetNodeInput().GetRunwayIntersectItemList().size()> 0)
			{
				RunwayIntersectItem* pRwItem = theHold.m_pNode->GetNodeInput().GetRunwayIntersectItemList()[0];
				int nRwID = pRwItem->GetObjectID();
				//exit runway log
				AirsideRunwayCrossigsLog* pExitRunwayCrossingLog = new AirsideRunwayCrossigsLog();
				pExitRunwayCrossingLog->speed = travelTrace.getDistSpeed(dTravelDist);
				if (theHold.m_pDirSeg->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
				{
					pExitRunwayCrossingLog->sTaxiway = ((TaxiwayDirectSegInSim*)theHold.m_pDirSeg)->GetTaxiwaySeg()->m_pTaxiway->GetTaxiwayInput()->GetObjNameString();
				}
				pExitRunwayCrossingLog->sNodeName = theHold.m_pNode->GetNodeInput().GetName();
				pExitRunwayCrossingLog->time = atHoldTime.getPrecisely();
				pExitRunwayCrossingLog->sRunway = pRwItem->GetNameString();
				pExitRunwayCrossingLog->state = AirsideRunwayCrossigsLog::OnExitRunway;
				pFlight->LogEventItem(pExitRunwayCrossingLog);
			}
		}	
		//create log
		FlightCrossNodeLog* pCrossNodeLog = CreatFlightCrossNodeLog(pFlight,theHold,atHoldTime,theRoute);
		pFlight->LogEventItem(pCrossNodeLog);
		theHold.m_bCrossed = true;
	}	

	//entry exit lane points
	std::vector<LaneIntersectionNotifyPoint> vNotPts = theRoute.GetLaneNotifyPtList().GetNotifyPointsBetween(curDist,nextDist);
	for(int i=0;i<(int)vNotPts.size();++i)
	{
		LaneIntersectionNotifyPoint& thePoint = vNotPts[i];
		double dTravleDist = thePoint.mDistInRoute - curDist;		
		ElapsedTime atDistTime = startTime + travelTrace.getDistTime(dTravleDist);
		if(thePoint.m_type == LaneIntersectionNotifyPoint::ENTRY_POINT)
		{
			thePoint.mpNode->OnFlightEnter(pFlight,atDistTime);
		}

	}
	std::vector<LaneIntersectionNotifyPoint> vExitNotPts = theRoute.GetLaneNotifyPtList().GetNotifyPointsUntil(nextDist);
	for(int i=0;i<(int)vExitNotPts.size();++i)
	{
		LaneIntersectionNotifyPoint& thePoint = vExitNotPts[i];
		if(thePoint.m_type == LaneIntersectionNotifyPoint::EXIT_POINT && thePoint.mpNode->bMobileInResource(pFlight) )
		{		
			double dTravleDist = thePoint.mDistInRoute - curDist;		
			ElapsedTime atDistTime = startTime + travelTrace.getDistTime(dTravleDist);
			thePoint.mpNode->OnFlightExit(pFlight,atDistTime);
		}

	}

}


//do the operation: such as release direction , set enter resource time, exit time
void TravelToNextDistInRoute(AirsideFlightInSim* pFlight,
							 TaxiRouteInSim& theRoute,
							 const DistanceUnit& curDist, 
							 const DistanceUnit& nextDist, 
							 double dEndSpd, 
							 ClearanceItem& lastClearanceItem, 
							 Clearance& newClearance,
							 bool bPushback = false)
{
	if(curDist==nextDist)
		return; 

	double dStartSpd = lastClearanceItem.GetSpeed();
	double dDist = nextDist - curDist;
	const ElapsedTime startTime= lastClearanceItem.GetTime();

	MobileTravelTrace travelTrace(pFlight->mGroundPerform,dDist,dStartSpd,dEndSpd);
	travelTrace.BeginUse();

	double dAcc;
	if(dStartSpd<dEndSpd){
		dAcc = pFlight->mGroundPerform.getAccSpd();
	}
	else if(dStartSpd>dEndSpd)
	{
		dAcc = -pFlight->mGroundPerform.getDecSpd();
	}
	else
	{
		dAcc = 0;
	}

	DoMoveToNextDistInRoute(travelTrace,startTime,pFlight,theRoute,curDist,nextDist);

	//get clearance items
	//add clearance items	
	std::vector<FiletPointInTaxiRoute> midFiltPts = theRoute.m_vFiletPts.GetFiletPointsBetween(curDist, nextDist);
	DistanceUnit lastItemDistInRoute = curDist;
	MobileTravelTrace modifytrace = travelTrace;
	if(midFiltPts.size())
		modifytrace.removeDistItem(midFiltPts.begin()->GetDistInRoute()-curDist,midFiltPts.rbegin()->GetDistInRoute()-curDist);
	if(curDist<0)
		modifytrace.removeMidItems();

	for(int i=0;i< (int)midFiltPts.size();i++)
	{
		FiletPointInTaxiRoute& theFtPt = midFiltPts[i];
		DistanceUnit distInRoute  = theFtPt.GetDistInRoute();
		DistanceUnit dTravelDist = distInRoute - curDist;
		modifytrace.addDistItem(dTravelDist, travelTrace.getDistSpeed(dTravelDist) ); //add fillet points
	}

	ElapsedTime beginTime = lastClearanceItem.GetTime();
	modifytrace.BeginUse();
	for(int i=1;i<modifytrace.getItemCount();++i)
	{
		MobileTravelTrace::DistSpeedTime& distSpdT = modifytrace.ItemAt(i);
		DistanceUnit dPosDist = curDist+ distSpdT.mdist; //curdist;
		DistanceUnit dPosSpeed =  distSpdT.mdSpd;
		ElapsedTime tPosTime = beginTime+distSpdT.mtTime;

		//add items
		int nItem; DistanceUnit dDist;
		theRoute.GetItemIndexAndDistInRoute(dPosDist, nItem, dDist);
		if(nItem<theRoute.GetItemCount() && nItem>=0 )
		{
			ClearanceItem endItem(theRoute.ItemAt(nItem).GetResource(), theRoute.GetMode(), dDist);
			endItem.SetSpeed(dPosSpeed);
			endItem.SetTime(tPosTime);
			endItem.SetAcceleration(dAcc);
			endItem.SetPushback(bPushback);
			lastClearanceItem = endItem;
			newClearance.AddItem(lastClearanceItem);
		}

	}


}

void MinTimeTravelToNexDist(AirsideFlightInSim* pFlight,
							TaxiRouteInSim& theRoute,
							const DistanceUnit& curDist,  
							const DistanceUnit& nextDist, 
							double dEndSpd, 
							ClearanceItem& lastClearanceItem, 
							Clearance& newClearance,
							bool bPushback)
{
	TravelToNextDistInRoute(pFlight, theRoute, curDist, nextDist, dEndSpd, lastClearanceItem, newClearance, bPushback);
	if(!newClearance.GetItemCount()) //add ending item
	{
		newClearance.AddItem(lastClearanceItem);
	}
}

//check if the flight is going to cross the runway
bool IsGoingCrossRunway(AirsideFlightInSim* pFlight,const FlightHoldListInTaxiRoute& taxiRouteHolds, const DistanceUnit& curDist, HoldInTaxiRoute& dHold1, HoldInTaxiRoute& dHold2)
{
	std::vector<HoldInTaxiRoute*> vEntryHoldList = taxiRouteHolds.GetNextRunwayEntryHoldList(curDist);
	if(vEntryHoldList.size())
	{
		const HoldInTaxiRoute& firstHold = *(*vEntryHoldList.begin());
		dHold1 = firstHold;

		dHold2 = *taxiRouteHolds.GetExitHold(firstHold.m_pNode);
		
		for(int i= 1;i<(int)vEntryHoldList.size();i++)//check close intersection
		{
			HoldInTaxiRoute dNextHold1 = *vEntryHoldList[i];
			if( dNextHold1.m_dDistInRoute < dHold2.m_dDistInRoute ) //two Intersection are close , check it also
			{
				dHold2 = *taxiRouteHolds.GetExitHold(dHold1.m_pNode);
			}
			else
			{
				return true;
			}
		}
		return true;
	}
	return false;
}

RouteDirPathList TaxiRouteInSim::getNextPathList(int nBeginIdx)const
{
	RouteDirPathList reslt;
	for(int i=max(0,nBeginIdx+1);i<GetItemCount();i++)
	{
		AirsideResource *theRes = ItemAt(i).GetResource();
		if( theRes && theRes->IsKindof(typeof(RouteDirPath)) )
		{	
			reslt.push_back((RouteDirPath*)theRes);
		}
	}
	return reslt;
}


AirsideFlightInSim* pConflictFlt = NULL;
IntersectionNodeInSim* pConflictNode =NULL;


boost::tuple<ARCMobileElement*, DistanceUnit> getLeadMobile(AirsideFlightInSim* pFlt ,TaxiRouteInSim& theRoute, DistanceUnit distInRoute);

//void LogConflictAtNode(AirsideFlightInSim* pFlight, const ElapsedTime& time )
//{
//#if _DEBUGLOG
//	if(pFlight && pConflictFlt)
//	{
//		CString strLog;
//		strLog.Format("%s wait for %s, (%d - %d) at node%s" , 
//			pFlight->GetCurrentFlightID().GetString(), pConflictFlt->GetCurrentFlightID().GetString(), 
//			pFlight->GetUID(),
//			pConflictFlt->GetUID(),
//			pConflictNode?pConflictNode->PrintResource():"Null");
//
//		CFileOutput logFile(ConflictDebugFile);
//		logFile.LogLine(time.printTime());
//		logFile.LogLine(strLog);
//	}
//#endif
//}

double TaxiRouteInSim::GetFlightOnTaxiwaySegSpeed( const AirsideFlightInSim* pFlight ) const
{
	double dSpeed = pFlight->GetSpeedByMode(GetMode());
	double dSpeedConstrained = 20.0;
	if (pFlight->GetTaxiwayConstrainedSpeed(dSpeedConstrained) && dSpeedConstrained<dSpeed)
	{
		dSpeed = dSpeedConstrained;
	}
	return dSpeed;
}

int GetQueueFlightCount(RunwayResourceManager *pRunwayResManager)
{
	int nCount = 0;
	for (int i = 0; i < pRunwayResManager->GetRunwayCount(); i++)
	{
		RunwayInSim* pRunwayInSim = pRunwayResManager->GetRunwayByIndex(i);
		LogicRunwayInSim* pLogicRwy1 = pRunwayInSim->GetLogicRunway1();
		LogicRunwayInSim* pLogicRwy2 = pRunwayInSim->GetLogicRunway2();

		nCount += pLogicRwy1->GetQueueList().GetAllFlightCount();
		nCount += pLogicRwy2->GetQueueList().GetAllFlightCount();
	}
	return nCount;
}

void LogTakeoffProcess(AirsideFlightInSim * pFlight,RunwayExitInSim* pTakeoffPos,ClearanceItem& lastClearanceItem,TaxiRouteItemInSim& theItem)
{
	
	//if(!pTakeoffPos->GetLogicRunway()->IsFlightInQueueToTakeoffPos(pFlight,pTakeoffPos))//flight does not exist takeoff queue
	{
		AirTrafficController* pATC = pFlight->GetAirTrafficController();
		ASSERT(pATC);
		RunwayResourceManager *pRunwayResManager = pATC->GetAirsideResourceManager()->GetAirportResource()->getRunwayResource();
		ASSERT(pRunwayResManager);
		QueueToTakeoffPos* pQueue = pTakeoffPos->GetLogicRunway()->GetQueueList().GetAddQueue(pTakeoffPos);
		AirsideTakeoffProcessLog* pTakeoffLog = new AirsideTakeoffProcessLog();
		pTakeoffLog->time = lastClearanceItem.GetTime();
		pTakeoffLog->m_lCount = pQueue->GetItemCount();
		pTakeoffLog->m_lTotalCount = GetQueueFlightCount(pRunwayResManager);
		pTakeoffLog->m_sTakeoffPoistion = pTakeoffPos->GetExitInput().GetName().GetString();
		pTakeoffLog->m_dDist = lastClearanceItem.GetDistInResource();
		pTakeoffLog->m_speed = pFlight->GetSpeed();
		pTakeoffLog->x = pFlight->GetPosition().getX();
		pTakeoffLog->y = pFlight->GetPosition().getY();
		pTakeoffLog->z = pFlight->GetPosition().getZ();
 		if( theItem.GetResource() && theItem.GetResource()->IsKindof(typeof(FlightGroundRouteDirectSegInSim)) )
		{
 			FlightGroundRouteDirectSegInSim* pSeg = (FlightGroundRouteDirectSegInSim*)theItem.GetResource();
 			IntersectionNodeInSim* pEnterNode = pSeg->GetEntryNode();
 			if (pEnterNode)
 			{
 				pTakeoffLog->m_sStartNode = pEnterNode->GetNodeInput().GetName().GetString();
 			}
 
 			IntersectionNodeInSim* pExitNode = pSeg->GetExitNode();
 			if (pExitNode)
 			{
 				pTakeoffLog->m_sEndNode = pExitNode->GetNodeInput().GetName().GetString();
 			}
 		}
		pFlight->LogEventItem(pTakeoffLog);
	}
}

//get the clearance of the flight
bool TaxiRouteInSim::FindClearanceInConcern( AirsideFlightInSim * pFlight,ClearanceItem& lastClearanceItem,DistanceUnit radiusOfConcern, Clearance& newClearance )
{
	if( GetItemCount() < 1) 
		return false;
	
	InitRoute(pFlight,lastClearanceItem.GetTime());
	
	bool bPushbackByTowTruck = false;

	if (GetMode() == OnTowToDestination && pFlight->GetTowingServiceVehicle() && pFlight->GetTowingServiceVehicle()->GetServiceType() == TOWOPERATIONTYPE_PUSHBACKTORELEASEPOINT)
		bPushbackByTowTruck = true;

	
	bool bGetDelay = (GetMode() != OnTaxiToTempParking ); //no delay for temp parking

	const int nBeginItemIdx = GetItemIndex( lastClearanceItem.GetResource() );
	const DistanceUnit dCurDistInRoute = GetDistInRoute( nBeginItemIdx , lastClearanceItem.GetDistInResource() );
	
	updateCurItemIndex(nBeginItemIdx);	
	
	
	DistanceUnit dExitRouteDist = GetExitRouteDist(pFlight);	
	HoldInTaxiRoute* lastEntryHold = GetlastEntryHold();
	//double dDecDist = pFlight->mGroundPerform.getSpdChangeDist(pFlight->mGroundPerform.mNoramlSpd,0);
	
	{//check to see whether the flight arrival at the end of the Route
		if(dExitRouteDist <= dCurDistInRoute+minTravelDist  && dCurDistInRoute>=0 )//flight arrival at the end 
		{		
			if(pFlight->GetMode() == OnTaxiToRunway && lastEntryHold && lastEntryHold->IsRunwayHold() )//if flight is take off add it to the queue
			{
				RunwayExitInSim* pTakeoffPos = pFlight->GetAndAssignTakeoffPosition();
				if(pTakeoffPos)
				{
					int nCurentIdx = max(0,nBeginItemIdx);
					LogTakeoffProcess(pFlight,pTakeoffPos,lastClearanceItem,m_vItems[nCurentIdx]);
					pTakeoffPos->GetLogicRunway()->AddFlightToTakeoffQueue(pFlight,pTakeoffPos,lastClearanceItem.GetTime());
					QueueToTakeoffPos* pQueue = pTakeoffPos->GetLogicRunway()->GetQueueList().GetAddQueue(pTakeoffPos);
					pQueue->SetHeadFlight(pFlight);
				}
			}	
			

			return false;
		}
	} 

	//lead mobile
	ARCMobileElement *pLeadMob=NULL;
	DistanceUnit distToLead=0;
	boost::tie(pLeadMob,distToLead) = getLeadMobile(pFlight,*this,dCurDistInRoute);
	bool bTheSameLead = getConflictGraph(pFlight)->IsAFollowB(pFlight,pLeadMob);
	bool bJustLanding = false;
    bJustLanding = (dCurDistInRoute<0 && pFlight->GetMode() == OnExitRunway);
	if(!bJustLanding) //ignore just landing flight
	{
		//flight found lead flight 
		getConflictGraph(pFlight)->SetAFollowB(pFlight,pLeadMob);
	}

	//occupy first node
	if(bJustLanding)
	{
		int iEndIdx = m_vHoldList.GetHoldUntil(0);	
		for(int i=0;i<(int)iEndIdx;i++ )
		{
			HoldInTaxiRoute& theHold = m_vHoldList.ItemAt(i);
			if(theHold.m_bCrossed)
				continue; //ignore crossed holds	
			

			if( theHold.m_hHoldType == HoldInTaxiRoute::ENTRYNODE )
			{
				getConflictGraph(pFlight)->OnFlightEnterNode(pFlight,theHold.m_pNode, lastClearanceItem.GetTime());
				getConflictGraph(pFlight)->OnMobileEnter(pFlight,theHold.mLinkDirSeg,lastClearanceItem.GetTime());
			}
			theHold.m_bCrossed = true;
		}
		
	}
	//
	double dNomalSpd = pFlight->GetSpeedByMode(GetMode());
	// apply taxiway speed constraint to the normal speed
	// 1. according to whether the flight is about to enter one hold, to use next or this AirsideResource
	// 2. if the AirsideResource is TaxiwayDirectSegInSim, try to get the constrained speed of the flight
	// 3. if the constrained speed is more restrict, apply it
	PLACE_TRACK_STRING("2010223-10:55:34");
	AirsideResource* pNextRes = NULL;
	double dMoveSpd =  pFlight->getMobilePerform().getNormalSpd();
	DistanceUnit dDecDistToStatic = pFlight->getMobilePerform().getSpdChangeDist(dMoveSpd,0); //dist to stop
	DistanceUnit distToStatic = dCurDistInRoute + dDecDistToStatic; // distance to static if dec immediately
	HoldInTaxiRoute* pNextHold = m_vHoldList.GetNextNodeEntryHold(dCurDistInRoute);
	if (pNextHold)
	{
		if (m_pLastAcrossHold && GetItemIndex(pNextHold->mLinkDirSeg) < GetItemIndex(m_pLastAcrossHold->mLinkDirSeg))
		{
			pNextRes = m_pLastAcrossHold->mLinkDirSeg;
		}
		else if(distToStatic + ARCMath::EPSILON >= pNextHold->m_dDistInRoute) //flight can move to the checkpoint
		{
			pNextRes = pNextHold->mLinkDirSeg;
			m_pLastAcrossHold = pNextHold;
		}
		else
		{
			pNextRes = lastClearanceItem.GetResource();
		}

		if (pNextRes && pNextRes->IsKindof(typeof(TaxiwayDirectSegInSim)))
		{
			TaxiwayDirectSegInSim* pNextTaxiwayDirSeg = (TaxiwayDirectSegInSim*)pNextRes;
			double dConstrainedSpeed;
			if (pNextTaxiwayDirSeg->GetTaxiwaySeg()->GetTaxiwayConstrainedSpeed(pFlight, dConstrainedSpeed) && dConstrainedSpeed< dNomalSpd)
			{
				dNomalSpd = dConstrainedSpeed;
			}
		}
	}
	else
	{
		m_pLastAcrossHold = NULL;
	}
	

	//the flight should stop for lead speed constrain
	bool bNeedStopForLeadReseaon= false;
	double dStopDistInRoute = GetEndDist();
	//check 
	if( pLeadMob )
	{
		//------------------------------------------------------------START-----------------------------------------------------------
		//--------------------------Two Solutions to Calculate Normal Speed when Lead Mobile Presents----------------------------------
#define __TO_USE_SOLUTION_A__
#ifdef __TO_USE_SOLUTION_A__
		// Solution A(Default):
		DistanceUnit dSeperation  = pLeadMob->getCurSeparation() + pLeadMob->GetLength()*0.5 + pFlight->GetLength()*0.5;
		DistanceUnit dSpdChageDist = pFlight->mGroundPerform.getSpdChangeDist(pFlight->mGroundPerform.getNormalSpd(),0);
		ElapsedTime leadMobTime = pLeadMob->getCurTime();
	
		if(leadMobTime < lastClearanceItem.GetTime()){}		
		else
		{
			ElapsedTime timeoffset = leadMobTime - lastClearanceItem.GetTime();
		
			if( distToLead - timeoffset.asSeconds() * pLeadMob->mGroundPerform.mNoramlSpd < (dCurDistInRoute + dSpdChageDist+dSeperation) )
			{
				pFlight->mGroundPerform.mNoramlSpd = min(pLeadMob->mGroundPerform.mNoramlSpd, dNomalSpd );
			}			
			else//can use normal speed
			{	
				double overheadDist = timeoffset.asSeconds()*dNomalSpd + dCurDistInRoute - (distToLead-dSeperation-dSpdChageDist);
				if( overheadDist>0 &&  dNomalSpd > pLeadMob->mGroundPerform.mNoramlSpd) //if travel with the time
				{
					double dNormalDistCanTraval = timeoffset.asSeconds()*dNomalSpd - overheadDist*dNomalSpd/(dNomalSpd - pLeadMob->mGroundPerform.mNoramlSpd);
					if(dNormalDistCanTraval>0)
					{
						dStopDistInRoute = dCurDistInRoute+dNormalDistCanTraval;
						bNeedStopForLeadReseaon = true;
					}
				}
				pFlight->mGroundPerform.mNoramlSpd = dNomalSpd;	
			}
		}
#else
		// Solution B:
		DistanceUnit dSeperation  = pLeadMob->getCurSeparation() + pLeadMob->GetLength()*0.5 + pFlight->GetLength()*0.5;
		DistanceUnit dSpdChageDist = pFlight->mGroundPerform.getSpdChangeDist(pFlight->mGroundPerform.getNormalSpd(),0);
		DistanceUnit dEstimateCriticalDist = distToLead - dCurDistInRoute - dSeperation;

		ElapsedTime timeTolead = ElapsedTime( dEstimateCriticalDist/dNomalSpd );
		ElapsedTime diffTime = pLeadMob->getCurTime() < lastClearanceItem.GetTime()
			? ElapsedTime(0L) : pLeadMob->getCurTime() - lastClearanceItem.GetTime();

		if (timeTolead <= diffTime || dEstimateCriticalDist <= dSpdChageDist || !bTheSameLead) // the sub control limit or another lead mobile
		{
			pFlight->mGroundPerform.mNoramlSpd = min(pLeadMob->mGroundPerform.mNoramlSpd, dNomalSpd );
		}
		else if (timeTolead*2L >= diffTime ||/*&&*/ dEstimateCriticalDist >= dSpdChageDist*2) // the super control limit
		{
			pFlight->mGroundPerform.mNoramlSpd = dNomalSpd;
		}
		else
		{
			// do nothing, just keep the hysteresis curve
		}
		//------------------------------------------------------------
#endif
	}
	else
	{
		pFlight->mGroundPerform.mNoramlSpd = dNomalSpd;
	}
	

	//
	if(pLeadMob&&pLeadMob->IsKindof(typeof(AirsideFlightInSim)))
	{

	}
	//check the separations the flight should take to account
	boost::tuple<DistanceUnit, double, SimAgent*,DistanceUnit> checkResultLead = checkConflictWithLeadMobile(pFlight,dCurDistInRoute,radiusOfConcern);
	SimAgent* pAgent = checkResultLead.get<2>();
	boost::tuple<DistanceUnit, double, SimAgent*,DistanceUnit> checkResultHold = checkConflictWithNextHold(pFlight,(CAirsideMobileElement*)pAgent,dCurDistInRoute);
	boost::tuple<DistanceUnit, double, TaxiInterruptLineInSim*,DistanceUnit,ElapsedTime> checkInterruptLine = checkConflictWithInterruptLine(pFlight,dCurDistInRoute);


	//////////////////////////////conflict and stop////////////////////////////////////////////
	//conflict with hold
	bool bCheckHold = true;
	if( checkResultHold.get<2>() && int(checkResultHold.get<0>()) <= max(0,int(dCurDistInRoute)+minTravelDist) )
	{
		SimAgent* pAgent = checkResultHold.get<2>();
		
		RouteDirPathList routeList = getNextPathList(nBeginItemIdx);		
		if(!routeList.empty())
		{
			getConflictGraph(pFlight)->OnMobileGoingToEnter(pFlight,*routeList.begin(),lastClearanceItem.GetTime()); //need to notify I am going to next after 5 minutes
		}

		bool bNewDelay = pFlight->StartDelay(lastClearanceItem,pConflictFlt,FlightConflict::STOPPED,FlightConflict::APPROACHINTERSECTION,FltDelayReason_Stop,_T("Taxiway delay"));		//taxi delay
		
		if (pFlight->GetResource() && pFlight->GetResource()->GetType() == AirsideResource::ResType_StandLeadOutLine)
		{
			//flight stand operation delay log by taxiway occupied
			StandLeadOutLineInSim* pLeadOut = (StandLeadOutLineInSim*)pFlight->GetResource();
			AirsideFlightStandOperationLog* pDelayLog = new AirsideFlightStandOperationLog(lastClearanceItem.GetTime().getPrecisely(), pFlight, pLeadOut->GetStandInSim()->GetID(),
				pLeadOut->GetStandInSim()->GetStandInput()->GetObjectName(), AirsideFlightStandOperationLog::StandDelay);
			pDelayLog->m_eDelayReason = AirsideFlightStandOperationLog::TaxiwayOccupiedDelay;
			pFlight->LogEventItem(pDelayLog);
		}

		//write a speed ==0 log
		if(newClearance.GetItemCount()==0 && pFlight->getCurSpeed()>0)
		{
			pFlight->SetSpeed(0);
			pFlight->WriteLog();
		}
		
		//debug log
		//LogConflictAtNode(pFlight,lastClearanceItem.GetTime()); 
		if(pConflictNode) //node delay
		{
			pFlight->DependOnAgents(pConflictNode);			
			pFlight->LogEventItem( createStartNodeDelayLog(pFlight,pConflictNode,lastClearanceItem.GetTime(),pConflictFlt) );			
			return true;
		}
		else
		{
			if(pConflictFlt)
			{
				if(pFlight->DependOnAgents(pConflictFlt))
				{
					IntersectionNodeInSim* pNextNode = NULL;
					HoldInTaxiRoute* pNextHold = m_vHoldList.GetNextNodeEntryHold(dCurDistInRoute);
					if(pNextHold)pNextNode = pNextHold->m_pNode;
					if(pNextNode)
						pFlight->LogEventItem( createStartNodeDelayLog(pFlight,pNextNode,lastClearanceItem.GetTime(),pConflictFlt) );			
					return true;
				}				
			}			
			bCheckHold = false;
		}

		/*if(pConflictFlt)
		{
			DynamicConflictGraph::getInstance().SetAWaitForB(pFlight,pConflictFlt);
		}*/
		{ // flight stop check lead flight in queue to runway hold
			SimAgent* pAgent = checkResultLead.get<2>();
			if(pAgent && pAgent->IsKindof(typeof(AirsideFlightInSim)) )
			{
				AirsideFlightInSim * sepLeadFlight = (AirsideFlightInSim*)pAgent;
				if(sepLeadFlight && sepLeadFlight->GetWaitRunwayHold() ) //if lead flight is waiting for cross runway, 
				{
					pFlight->EnterQueueToRunwayHold( sepLeadFlight->GetWaitRunwayHold(),false );
				}
			}
		}

		//return true;
	}	
	//conflict with lead flight
	if( checkResultLead.get<2>() && int(checkResultLead.get<0>()) <= max(0,int(dCurDistInRoute)+minTravelDist) )//conflict with lead 
	{
		SimAgent* pAgent = checkResultLead.get<2>();

		pFlight->DependOnAgents(checkResultLead.get<2>());
	
		if( pAgent->IsKindof(typeof(AirsideFlightInSim)) )	
		{
			AirsideFlightInSim * sepLeadFlight = (AirsideFlightInSim*)pAgent;
			//DynamicConflictGraph::getInstance().SetAFollowB(pFlight,sepLeadFlight);
			
			if(newClearance.GetItemCount()==0)//write a speed ==0 log
			{
				pFlight->SetSpeed(0);
				pFlight->WriteLog();
			}
#if _DEBUGLOG
			CString strLog;
			strLog.Format("%s wait for %s, (%d - %d)" , pFlight->GetCurrentFlightID().GetString(), sepLeadFlight->GetCurrentFlightID().GetString(), pFlight->GetUID(), sepLeadFlight->GetUID() );
			CFileOutput logFile(ConflictDebugFile);
			logFile.LogLine(lastClearanceItem.GetTime().printTime());
			logFile.LogLine(strLog);
#endif
			//////////////////////////////////////////////////////////////////////////delay log
			pFlight->StartDelay(lastClearanceItem, sepLeadFlight, FlightConflict::STOPPED, FlightConflict::BETWEENINTERSECTIONS,FltDelayReason_Stop,_T("Taxiway delay"));	//taxi delay
			/////////////////////////////////////////////////////////////////////////

			
			if(bGetDelay)
				pFlight->SetDelayed(new TaxiLeadFlightDelay(sepLeadFlight, lastClearanceItem.GetTime()));				
			
			if(sepLeadFlight && sepLeadFlight->GetWaitRunwayHold() ) //if lead flight is waiting for cross runway, 
			{
				HoldPositionInSim* pHold = sepLeadFlight->GetWaitRunwayHold();
  				AirsideRunwayCrossigsLog* pRunwayCrossingLog = new AirsideRunwayCrossigsLog();
  				pRunwayCrossingLog->speed = pFlight->GetSpeed();
				TaxiwayInSim* pTaxiway = pFlight->GetAirTrafficController()->GetAirsideResourceManager()->GetAirportResource()->getTaxiwayResource()->GetTaxiwayByID(pHold->GetTaxiwayID());
  				pRunwayCrossingLog->time = pFlight->GetTime().getPrecisely();
 				pRunwayCrossingLog->sRunway = pHold->GetRunwayInSim()->GetRunwayInput()->GetObjNameString();
				pRunwayCrossingLog->sNodeName = _T("");

 				pRunwayCrossingLog->sTaxiway = pTaxiway->GetTaxiwayInput()->GetObjNameString();
 				pRunwayCrossingLog->state = AirsideRunwayCrossigsLog::OnWaitingCrossRunway;
  				pFlight->LogEventItem(pRunwayCrossingLog);

				pFlight->EnterQueueToRunwayHold( sepLeadFlight->GetWaitRunwayHold(),false );
			}

			if((GetMode() == OnTaxiToRunway||GetMode() == OnTowToDestination) ) // if flight is going to take off, add it to queue
			{
				RunwayExitInSim* pTakeoffPos = pFlight->GetAndAssignTakeoffPosition();	
				if(pTakeoffPos )
				{	
					int nCurentIdx = max(0,nBeginItemIdx);

					if(pTakeoffPos->GetLogicRunway()->IsFlightInQueueToTakeoffPos(sepLeadFlight, pTakeoffPos) 
						&& !pTakeoffPos->GetLogicRunway()->IsFlightInQueueToTakeoffPos(pFlight,pTakeoffPos))
					{
						pTakeoffPos->GetLogicRunway()->AddFlightToTakeoffQueue(pFlight,pTakeoffPos,lastClearanceItem.GetTime());
						LogTakeoffProcess(pFlight,pTakeoffPos,lastClearanceItem,m_vItems[nCurentIdx]);			

					}
				}
			}
		}	
		
		return true;
	}

	//conflict interrupt line
	if( checkInterruptLine.get<2>() && int(checkInterruptLine.get<0>()) <= max(0,int(dCurDistInRoute)+minTravelDist) )
	{
		TaxiInterruptLineInSim* pInterruptLine = checkInterruptLine.get<2>();
		pInterruptLine->SetWaitFlight(pFlight,lastClearanceItem.GetTime());
		FlightGetClearanceEvent* newEvent  = new FlightGetClearanceEvent(pFlight);
		newEvent->setTime(lastClearanceItem.GetTime()+checkInterruptLine.get<4>() );
		newEvent->addEvent();
		return true;
	}
	//////////////////////////////////////////////////////////////////////////

	//no conflict
	pFlight->EndDelay(lastClearanceItem);			//end taxi delay
	if (lastClearanceItem.GetMode() == OnExitStand && pFlight->GetResource())
	{
		StandInSim* pLeavingStand = NULL;
		// airside flight leaving stand operation
		if (pFlight->GetResource()->GetType() == AirsideResource::ResType_StandLeadOutLine)
		{
			StandLeadOutLineInSim* pLeadOut = (StandLeadOutLineInSim*)pFlight->GetResource();
			pLeavingStand = pLeadOut->GetStandInSim();
		}
		else if (pFlight->GetResource()->GetType() == AirsideResource::ResType_StandLeadInLine)
		{
			StandLeadInLineInSim* pLeadIn = (StandLeadInLineInSim*)pFlight->GetResource();
			pLeavingStand = pLeadIn->GetStandInSim();
		}
		if (pLeavingStand)
		{
			AirsideFlightStandOperationLog* pLeaveLog = new AirsideFlightStandOperationLog(lastClearanceItem.GetTime().getPrecisely(), pFlight,pLeavingStand->GetID(), pLeavingStand->GetStandInput()->GetObjectName(),
				AirsideFlightStandOperationLog::LeavingStand);
			pFlight->LogEventItem(pLeaveLog);
		}
	}



	

	////////////////////////get next safe dist//////////////////////////////////////////////////
	double dMinBlockPos;
	double dNextSafeDistInRoute;
	double dEndSpd;
	if(checkResultHold.get<3>() < checkResultLead.get<3>() && bCheckHold ) //compare the hold or the lead flight which can be see
	{
		dNextSafeDistInRoute = checkResultHold.get<0>();
		dEndSpd = checkResultHold.get<1>();
		dMinBlockPos = checkResultHold.get<3>();
	}
	else{
		dNextSafeDistInRoute = checkResultLead.get<0>();
		dEndSpd = checkResultLead.get<1>();
		dMinBlockPos = checkResultLead.get<3>();
	}	
	if(dMinBlockPos > checkInterruptLine.get<3>() )
	{
		dNextSafeDistInRoute = checkInterruptLine.get<0>();
		dEndSpd  = checkInterruptLine.get<1>();
	}

	if(dNextSafeDistInRoute>=dExitRouteDist && GetMode() == OnTowToDestination)
	{
		dEndSpd = 0;
	}



	//if((GetMode() == OnTaxiToRunway||GetMode() == OnTowToDestination) && pLeadMob && pLeadMob->IsKindof(typeof(AirsideFlightInSim)) && dEndSpd < ARCMath::EPSILON ) // if flight is going to take off, add it to queue
	//{
	//	RunwayExitInSim* pTakeoffPos = pFlight->GetAndAssignTakeoffPosition();	
	//	if(pTakeoffPos )
	//	{	
	//		int nCurentIdx = max(0,nBeginItemIdx);

	//		if(pTakeoffPos->GetLogicRunway()->IsFlightInQueueToTakeoffPos((AirsideFlightInSim*)pLeadMob, pTakeoffPos) )
	//		{
	//			pTakeoffPos->GetLogicRunway()->AddFlightToTakeoffQueue(pFlight,pTakeoffPos,lastClearanceItem.GetTime());
	//			LogTakeoffProcess(pFlight,pTakeoffPos,lastClearanceItem,m_vItems[nCurentIdx]);				

	//		}
	//	}
	//}

	if(bNeedStopForLeadReseaon){
		dNextSafeDistInRoute = min(dNextSafeDistInRoute,dStopDistInRoute);
	}
	//////////////////////////////////////////////////////////////////////////
	///
	if(bGetDelay)pFlight->SetDelayed(NULL);

	//if( int(dNextSafeDistInRoute)!=int(GetEndDist()) )
	//	ASSERT( !m_vHoldList.IsDistInNoParkingNodeRange(dNextSafeDistInRoute) );
	
	//////////////////////////////////////////////////////////////////////////
	//double taxispd = pFlight->getMobilePerform().getNormalSpd();	
	if( dNextSafeDistInRoute >  0 && dCurDistInRoute < 0 ) //add first item
	{	
		//set enter time of the node		
		bool bNeedUnhook = false;
		if(lastClearanceItem.IsPushback() && !bPushbackByTowTruck)
		{
			bNeedUnhook = true;
		}
		
		if(bNeedUnhook)//add Unhook time
		{
			//
			DistanceUnit lastResDist = 0;
			StandLeadOutLineInSim * pStandOutLine = NULL;
			if(lastClearanceItem.GetResource()&& lastClearanceItem.GetResource()->GetType() == AirsideResource::ResType_StandLeadInLine)
			{
				StandLeadInLineInSim* pStandInLine = (StandLeadInLineInSim*)lastClearanceItem.GetResource();
				lastResDist = pStandInLine->GetEndDist();
			}
			if(lastClearanceItem.GetResource()&& lastClearanceItem.GetResource()->GetType() == AirsideResource::ResType_StandLeadOutLine)
			{
				pStandOutLine = (StandLeadOutLineInSim *)lastClearanceItem.GetResource();
				lastResDist = pStandOutLine->GetEndDist();
			}
			
			MobileGroundPerform pushbackPerform;
			pushbackPerform.mNoramlSpd = pFlight->GetPerformance()->getPushBackSpeed(pFlight);
			pushbackPerform.mAccSpd = pFlight->GetPerformance()->getPushBackAcc(pFlight);
			pushbackPerform.mDecSpd = pFlight->GetPerformance()->getPushBackDec(pFlight);
			

			MobileTravelTrace travelTrace(pushbackPerform,pStandOutLine->GetEndDist()-lastClearanceItem.GetDistInResource()+1000, 0,0);
			travelTrace.BeginUse();
			DoMoveToNextDistInRoute(travelTrace,lastClearanceItem.GetTime(),pFlight,*this,dCurDistInRoute,0);

			
			
			ElapsedTime unhookTime = 0L;
			if (pFlight->GetTowingServiceVehicle() ==NULL)
			{
				unhookTime = pFlight->GetUnhookTime();
			}	

			ClearanceItem firstRouteItem( ItemAt(0).GetResource(), GetMode(), ItemAt(0).GetDistFrom() );
			firstRouteItem.SetPushback(lastClearanceItem.IsPushback());
			ElapsedTime dT;
			if (pStandOutLine && pStandOutLine->IsReleasePoint())
			{
				
				firstRouteItem.SetSpeed(m_dTaxiSpd);
				firstRouteItem.SetDistInResource(firstRouteItem.GetDistInResource()+3000);
				dT = pFlight->GetTimeBetween(lastClearanceItem, firstRouteItem);
				firstRouteItem.SetTime(lastClearanceItem.GetTime()+dT);
				newClearance.AddItem(firstRouteItem);

				lastClearanceItem = firstRouteItem;			
				lastClearanceItem.SetDistInResource( lastClearanceItem.GetDistInResource() + 1);//
				lastClearanceItem.SetSpeed(m_dTaxiSpd*0.5);
				dT = pFlight->GetTimeBetween(firstRouteItem,lastClearanceItem);
				lastClearanceItem.SetTime(lastClearanceItem.GetTime() + dT);
				newClearance.AddItem(lastClearanceItem);
			}
			else
			{
				/*ClearanceItem midItem(lastClearanceItem.GetResource(), lastClearanceItem.GetMode(), lastResDist*0.5);
				double dTowSpeed =  pFlight->GetPushbackSpeed();
				midItem.SetSpeed(dTowSpeed);
				dT = pFlight->GetTimeBetween(lastClearanceItem,midItem);
				midItem.SetTime(lastClearanceItem.GetTime()+dT);
				lastClearanceItem = midItem;
				newClearance.AddItem(midItem);*/
				ClearanceItem beginItem = lastClearanceItem;
				/*for( int i=1;i< travelTrace.getItemCount()-1;i++)
				{
					ClearanceItem midItem = beginItem;
					midItem.SetDistInResource(beginItem.GetDistInResource() + travelTrace.ItemAt(i).mdist);
					midItem.SetSpeed(travelTrace.ItemAt(i).mdSpd);
					midItem.SetTime(travelTrace.ItemAt(i).mtTime + beginItem.GetTime());
					lastClearanceItem = midItem;
					newClearance.AddItem(midItem);
				}*/

				firstRouteItem.SetSpeed(0);
				firstRouteItem.SetDistInResource(firstRouteItem.GetDistInResource()-1000);
				dT = travelTrace.getEndTime();
				firstRouteItem.SetTime( beginItem.GetTime()+dT);	
				firstRouteItem.SetPushback(true);
				newClearance.AddItem(firstRouteItem);
				firstRouteItem.SetPushback(false);
				newClearance.AddItem(firstRouteItem);

				lastClearanceItem = firstRouteItem;			
				lastClearanceItem.SetDistInResource( lastClearanceItem.GetDistInResource() + 1);//
				lastClearanceItem.SetTime(lastClearanceItem.GetTime() + unhookTime);CString strtmp = lastClearanceItem.GetTime().printTime();
				lastClearanceItem.SetSpeed(0);
				lastClearanceItem.SetPushback(false);
				newClearance.AddItem(lastClearanceItem);
			
			}
			//ClearanceItem toNodeItem = lastClearanceItem;
			//toNodeItem.SetDistInResource(lastClearanceItem.GetDistInResource() + 1000);
			//toNodeItem.SetSpeed(0);
			/*dT = pFlight->GetTimeBetween(lastClearanceItem,toNodeItem);
			toNodeItem.SetTime(lastClearanceItem.GetTime() + dT);*/
			//lastClearanceItem = toNodeItem;
			//newClearance.AddItem(lastClearanceItem);			
			return true;

		}	
		else
		{			

			DistanceUnit dEntryDist = 1000; //default 10m
			dEntryDist = min(dEntryDist,dNextSafeDistInRoute);
			if(dEntryDist>=GetEndDist())
			{
				dEntryDist = (dCurDistInRoute+GetEndDist())*0.5;
			}
			

			MobileTravelTrace travelTrace(pFlight->mGroundPerform,dEntryDist, lastClearanceItem.GetSpeed(),dEndSpd);
			travelTrace.BeginUse();
			DoMoveToNextDistInRoute(travelTrace,lastClearanceItem.GetTime(),pFlight,*this,dCurDistInRoute,dEntryDist);

			if(lastClearanceItem.GetResource() && (/*lastClearanceItem.GetResource()->GetType() == AirsideResource::ResType_StandLeadOutLine*/lastClearanceItem.GetMode() == OnExitStand
				|| lastClearanceItem.GetResource()->GetType() == AirsideResource::ResType_DeiceStation )) //can only add one item 
			{
				int nItem;
				DistanceUnit distInItem;
				GetItemIndexAndDistInRoute(dEntryDist,nItem,distInItem);
				ASSERT(nItem<GetItemCount());

				ClearanceItem entryRouteItem( ItemAt(nItem).GetResource(),GetMode(), distInItem );
				entryRouteItem.SetSpeed(dEndSpd);
				ElapsedTime dT = pFlight->GetTimeBetween(lastClearanceItem, entryRouteItem);
				entryRouteItem.SetTime( lastClearanceItem.GetTime() + dT );
				entryRouteItem.SetPushback(bPushbackByTowTruck);

				lastClearanceItem = entryRouteItem;
				newClearance.AddItem(lastClearanceItem);
				lastClearanceItem.SetDistInResource( lastClearanceItem.GetDistInResource() + 1);//
				newClearance.AddItem(lastClearanceItem);
			}
			else
			{				
				if( lastClearanceItem.GetMode() == OnExitRunway )
				{
					dEntryDist = pFlight->GetRunwayExit()->GetEnterTaxiwayPos();
	
					MinTimeTravelToNexDist(pFlight,*this,-1,dEntryDist,dEndSpd, lastClearanceItem,newClearance, bPushbackByTowTruck);
				}
				else
					MinTimeTravelToNexDist(pFlight,*this,0,dEntryDist,dEndSpd, lastClearanceItem,newClearance, bPushbackByTowTruck);

				if(dExitRouteDist < dEntryDist)
				{
					return false; 
				}
			}
			return true;
		}				
	}

	
	//get clearanceItems from curdist  to next safedist 
	std::vector<HoldInTaxiRoute> vHolds = m_vHoldList.GetHoldFromTo(dCurDistInRoute, dNextSafeDistInRoute);	
	//DistanceUnit preDistInRoute = dCurDistInRoute;
	for(int i=0;i< (int)vHolds.size();i++)
	{
		HoldInTaxiRoute& theHold = vHolds[i];	
		///flight is at the last hold
		if(lastEntryHold && lastEntryHold->m_pNode == theHold.m_pNode && (GetMode() == OnTaxiToRunway||GetMode() == OnTowToDestination) )
		{
			RunwayExitInSim* pTakeoffPos = pFlight->GetAndAssignTakeoffPosition();	
			if(pTakeoffPos )
			{	
				int nCurentIdx = max(0,nBeginItemIdx);
				LogTakeoffProcess(pFlight,pTakeoffPos,lastClearanceItem,m_vItems[nCurentIdx]);				
				pTakeoffPos->GetLogicRunway()->AddFlightToTakeoffQueue(pFlight,pTakeoffPos,lastClearanceItem.GetTime());
				QueueToTakeoffPos* pQueue = pTakeoffPos->GetLogicRunway()->GetQueueList().GetAddQueue(pTakeoffPos);
				pQueue->SetHeadFlight(pFlight);
			}			
		}
		//the hold is cross runway hold
		if( theHold.m_hHoldType == HoldInTaxiRoute::ENTRYNODE && theHold.IsRunwayHold() )
		{
			//calculate the time that the flight could go through the node
			DistanceUnit dThroughNodeDist;
			HoldInTaxiRoute* pExitHold = m_vHoldList.GetExitHold(theHold);
			if(pExitHold)
			{
				dThroughNodeDist = pExitHold->m_dDistInRoute - dCurDistInRoute;	
				ASSERT(dThroughNodeDist>=0);
			}	
			MobileTravelTrace travelTrace(pFlight->mGroundPerform,dThroughNodeDist,lastClearanceItem.GetSpeed(),pFlight->mGroundPerform.getNormalSpd() );
			travelTrace.BeginUse();

			ElapsedTime dThroughNodeTime = travelTrace.getEndTime();
			ElapsedTime dNextTime = lastClearanceItem.GetTime();

			ElapsedTime AvaEnterTime ;
			if(theHold.mLinkDirSeg && theHold.mLinkDirSeg->GetType()== AirsideResource::ResType_RunwayDirSeg)
			{
				AvaEnterTime  = MAX(dNextTime, theHold.m_pNode->GetLastOcyTime());
			}
			else
			{
				//available entry time
				AvaEnterTime = theHold.m_pNode->GetAvailableEntryTime(pFlight, dNextTime ,dNextTime + dThroughNodeTime );
			}

			//if the flight in runway wave crossing node, 
			HoldPositionInSim* pHoldCrossRunway = NULL;
			RunwayIntersectItem* pRwItem = theHold.m_pNode->GetNodeInput().GetRunwayIntersectItemList()[0];
			int nRwID = pRwItem->GetObjectID();
			RunwayInSim* pRunway = pFlight->GetAirTrafficController()->GetAirsideResourceManager()->GetAirportResource()->getRunwayResource()->GetRunwayByID(nRwID);
			pHoldCrossRunway = pRunway->GetHoldPosition( theHold.m_nHoldId );

			if(pHoldCrossRunway != NULL && pHoldCrossRunway->IsWaveCrossHold())
			{//wave crossing node//wave cross exceptions

				//check waving crossing allow the flight go through or not
				if(pRunway && !pRunway->bCanFlightCross(pFlight,lastClearanceItem.GetTime()) ) //flight cross the runway failed
				{
                    //if cannot go through

					//check the runway is on idle or not
					
					if((pRunway->GetLogicRunway1() && pRunway->GetLogicRunway1()->GetDepartureQueueLength()) ||
						(pRunway->GetLogicRunway2() && pRunway->GetLogicRunway2()->GetDepartureQueueLength()))
					{//if the runway is not free, the flight should wait its time 
						pHoldCrossRunway = pRunway->GetHoldPosition( theHold.m_nHoldId );
						pFlight->EnterQueueToRunwayHold(pHoldCrossRunway,true);

						//the flight has been at hold, no need event, stop and waiting
						if((int)theHold.m_dDistInRoute < int(dCurDistInRoute)+minTravelDist)
						{
							if(newClearance.GetItemCount() == 0)//add the waiting item,
							{
								lastClearanceItem.SetSpeed(0);
								newClearance.AddItem(lastClearanceItem);
								if(pFlight->GetTime() >= AvaEnterTime)
									AvaEnterTime = pFlight->GetTime() + ElapsedTime(1L);
								lastClearanceItem.SetTime(AvaEnterTime);
								newClearance.AddItem(lastClearanceItem);
							}

							return true;
						}
						//move to hold
						MinTimeTravelToNexDist(pFlight, *this,dCurDistInRoute, theHold.m_dDistInRoute, 0, lastClearanceItem, newClearance,bPushbackByTowTruck);
						//if(lastClearanceItem.GetTime() < AvaEnterTime)
						//{
						//	lastClearanceItem.SetSpeed(0);
						//	newClearance.AddItem(lastClearanceItem);
						//	lastClearanceItem.SetTime(AvaEnterTime);
						//	newClearance.AddItem(lastClearanceItem);
						//}
						return true;
						
					}
					else
					{//on idle, the flight could pass the runway node

						if( lastClearanceItem.GetTime() < AvaEnterTime )//flight can not cross the node
						{	
							//the flight has been at hold
							if((int)theHold.m_dDistInRoute < int(dCurDistInRoute)+minTravelDist)
							{
								pHoldCrossRunway = pRunway->GetHoldPosition( theHold.m_nHoldId );
								pFlight->EnterQueueToRunwayHold(pHoldCrossRunway,true);

								if(newClearance.GetItemCount() == 0)//add the waiting item,
								{
									lastClearanceItem.SetSpeed(0);
									newClearance.AddItem(lastClearanceItem);
									if(pFlight->GetTime() >= AvaEnterTime)
										AvaEnterTime = pFlight->GetTime() + ElapsedTime(1L);
									lastClearanceItem.SetTime(AvaEnterTime);
									newClearance.AddItem(lastClearanceItem);
								}

								return true;
							}
							
							//move to hold
							if(dCurDistInRoute < theHold.m_dDistInRoute)
								MinTimeTravelToNexDist(pFlight, *this,dCurDistInRoute, theHold.m_dDistInRoute, 0, lastClearanceItem, newClearance, bPushbackByTowTruck);
							else 
							{
								if(newClearance.GetItemCount() == 0)//add the waiting item,
								{
									lastClearanceItem.SetSpeed(0);
									newClearance.AddItem(lastClearanceItem);
									if(pFlight->GetTime() >= AvaEnterTime)
										AvaEnterTime = pFlight->GetTime() + ElapsedTime(1L);
									lastClearanceItem.SetTime(AvaEnterTime);
									newClearance.AddItem(lastClearanceItem);
								}
							}
							//if(lastClearanceItem.GetTime() < AvaEnterTime)
							//{
							//	lastClearanceItem.SetSpeed(0);
							//	newClearance.AddItem(lastClearanceItem);
							//	lastClearanceItem.SetTime(AvaEnterTime);
							//	newClearance.AddItem(lastClearanceItem);
							//}					
							return true;
						}
						else
						{
							HoldInTaxiRoute *pHoldExit = m_vHoldList.GetExitHold(theHold.m_pNode);
							//the flight go thought the node at one time
							if(pHoldExit != NULL && dNextSafeDistInRoute < pHoldExit->m_dDistInRoute)
							{
								if(dCurDistInRoute < pHoldExit->m_dDistInRoute)
								{
									RunwayInSim* pRunway = pFlight->GetAirTrafficController()->GetAirsideResourceManager()->GetAirportResource()->getRunwayResource()->GetRunwayByID(nRwID);
									MinTimeTravelToNexDist(pFlight, *this,dCurDistInRoute, pHoldExit->m_dDistInRoute, 0, lastClearanceItem, newClearance, bPushbackByTowTruck);
									if(lastClearanceItem.GetTime()<AvaEnterTime)
									{
										if(pFlight->GetTime() >= AvaEnterTime)
											AvaEnterTime = pFlight->GetTime() + ElapsedTime(1L);
										lastClearanceItem.SetTime(AvaEnterTime);
										newClearance.AddItem(lastClearanceItem);
									}
								}
								else
								{
									if(newClearance.GetItemCount() == 0)//add the waiting item,
									{
										lastClearanceItem.SetSpeed(0);
										newClearance.AddItem(lastClearanceItem);
										if(pFlight->GetTime() >= AvaEnterTime)
											AvaEnterTime = pFlight->GetTime() + ElapsedTime(1L);
										lastClearanceItem.SetTime(AvaEnterTime);
										newClearance.AddItem(lastClearanceItem);
									}
								}

								//pRunway->WakeUpRunwayLanding(lastClearanceItem.GetTime());
								return true;
							}
						}


					}
				}
				else
				{
					HoldInTaxiRoute *pHoldExit = m_vHoldList.GetExitHold(theHold.m_pNode);
					//the flight go thought the node at one time
					if(pHoldExit != NULL && dNextSafeDistInRoute < pHoldExit->m_dDistInRoute)
					{
						if(dCurDistInRoute < theHold.m_dDistInRoute)
						{
							RunwayInSim* pRunway = pFlight->GetAirTrafficController()->GetAirsideResourceManager()->GetAirportResource()->getRunwayResource()->GetRunwayByID(nRwID);
							MinTimeTravelToNexDist(pFlight, *this,dCurDistInRoute, theHold.m_dDistInRoute, 0, lastClearanceItem, newClearance,bPushbackByTowTruck);
							if(lastClearanceItem.GetTime()<AvaEnterTime)
							{
								if(pFlight->GetTime() >= AvaEnterTime)
									AvaEnterTime = pFlight->GetTime() + ElapsedTime(1L);
								lastClearanceItem.SetTime(AvaEnterTime);
								newClearance.AddItem(lastClearanceItem);
							}
						}
						else
						{
							if(newClearance.GetItemCount() == 0)//add the waiting item,
							{
								lastClearanceItem.SetSpeed(0);
								newClearance.AddItem(lastClearanceItem);
								if(pFlight->GetTime() >= AvaEnterTime)
									AvaEnterTime = pFlight->GetTime() + ElapsedTime(1L);
								lastClearanceItem.SetTime(AvaEnterTime);
								newClearance.AddItem(lastClearanceItem);
							}
						}
						//pRunway->WakeUpRunwayLanding(lastClearanceItem.GetTime());
						return true;
					}	
				}
			} 
			else //
			{
				//no wave crossing node,  no special treatment
				if(lastClearanceItem.GetTime() < AvaEnterTime)
				{
					pHoldCrossRunway = pRunway->GetHoldPosition( theHold.m_nHoldId );
					pFlight->EnterQueueToRunwayHold(pHoldCrossRunway,true);

					////the flight has been at hold
					//if((int)theHold.m_dDistInRoute < int(dCurDistInRoute)+minTravelDist)
					//{
					//	return true;
					//}

					MinTimeTravelToNexDist(pFlight, *this,dCurDistInRoute, theHold.m_dDistInRoute, 0, lastClearanceItem, newClearance,bPushbackByTowTruck);
					if(theHold.IsTakeoffPosHold(pFlight->GetTakeoffPosition()) && GetMode()==OnTaxiToRunway)
					{
						lastClearanceItem.SetMode(OnTakeOffWaitEnterRunway);
					}
					//runway hold delay
					pFlight->StartDelay(lastClearanceItem, NULL, FlightConflict::STOPPED, FlightConflict::BETWEENINTERSECTIONS,FltDelayReason_RunwayHold,_T("Runway hold delay"));	//taxi delay
					lastClearanceItem.SetSpeed(0);
					newClearance.AddItem(lastClearanceItem);
					lastClearanceItem.SetTime(AvaEnterTime);
					newClearance.AddItem(lastClearanceItem);
				//	pFlight->EndDelay(lastClearanceItem);	
					
					
					return true;
				}
			}
		}
		//normal entry hold, stop here for next clearance
		if(theHold.m_hHoldType == HoldInTaxiRoute::ENTRYNODE
			&& dCurDistInRoute + minTravelDist < theHold.m_dDistInRoute )
		{
			if( theHold.mLinkDirSeg  )
			{
				if( theHold.m_pNode->IsNoParking() && !m_vHoldList.IsDistInNoParkingNodeRange(theHold.m_dDistInRoute) )
				{
					MinTimeTravelToNexDist(pFlight, *this,dCurDistInRoute, theHold.m_dDistInRoute, dNomalSpd, lastClearanceItem, newClearance, bPushbackByTowTruck);
					return true;
				}
			}
		}
	}
	//move to the safe dist
	dNextSafeDistInRoute = min(dNextSafeDistInRoute, dExitRouteDist);
	MinTimeTravelToNexDist(pFlight, *this, dCurDistInRoute,dNextSafeDistInRoute,dEndSpd,lastClearanceItem, newClearance, bPushbackByTowTruck);
	return true;	
}

//ElapsedTime TaxiRouteInSim::GetFlightOcupancyRunwayTime( std::vector<HoldInTaxiRoute> vHolds,AirsideFlightInSim* pFlight ) const
//{
//
//	for (int i = nCurentIdx; i < GetItemCount(); i++)
//	{
//		const TaxiRouteItemInSim& routeItem = ItemAt(i);
//		AirsideResource *pResouce = routeItem.GetResource();
//		if (pResouce && pResouce->GetType() != AirsideResource::ResType_RunwayDirSeg)
//		{
//			RunwayDirectSegInSim* pRunwaySeg = (RunwayDirectSegInSim*)pResouce;
//			double dDist = routeItem.GetDistTo() - routeItem.GetDistFrom();
//
//			eOccupancyRunwayTime +=  ElapsedTime(dDist / dSpeed);
//		}
//		else
//			break;
//	}
//	
//	return eOccupancyRunwayTime;
//}

bool TaxiRouteInSim::IsLockDirectionOfWholeRoute()
{
	return false;
}

bool TaxiRouteInSim::CheckFlightConflictInResource(AirsideFlightInSim *pFlight,AirsideResource *pResouce,AirsideResource *pNextResource,ElapsedTime& nextEventTime)
{
	if(pResouce->GetType() != AirsideResource::ResType_TaxiwayDirSeg)
		return false;

	////get the flights that lock the resource
	//std::vector<AirsideFlightInSim *> vLockedFlight = pResouce->GetLockedFlight();
	//
	////remove itself in the list
	//std::vector<AirsideFlightInSim *>::iterator iter = std::find(vLockedFlight.begin(),vLockedFlight.end(),pFlight);
	//if (iter != vLockedFlight.end())
	//{
	//	vLockedFlight.erase(iter);
	//}

	////get the flights in the resource
	//std::vector<CAirsideMobileElement *> vInResourceFlight = pResouce->GetInResouceMobileElement();

	////remove the items in the resource
	//for (size_t i = 0; i < vInResourceFlight.size(); ++i)
	//{
	//	for (size_t j = 0; j < vLockedFlight.size(); ++j)
	//	{
	//		if(vInResourceFlight[i] == vLockedFlight[j])
	//		{
	//			vLockedFlight.erase(vLockedFlight.begin()+ j);
	//			break;
	//		}
	//	}
	//}

	TaxiwayDirectSegInSim *pNextDirectSegment =	reinterpret_cast<TaxiwayDirectSegInSim*>(pNextResource);
	TaxiwayDirectSegInSim *pDirectSegment =	reinterpret_cast<TaxiwayDirectSegInSim*>(pResouce);
	FlightGroundRouteResourceManager * pGroundRouteRes =  pFlight->GetAirTrafficController()->GetAirsideResourceManager()->GetAirportResource()->getGroundRouteResourceManager();

	FlightGroundRouteDirectSegList taxiwaySegmets;
	IntersectionNodeInSim *pIntersectionNode = NULL;
	//if (pDirectSegment->IsPositiveDir())
	//{
	//	pIntersectionNode =pDirectSegment->GetPhysicalSeg()->GetNode2();
	//}
	//else
	//{
	//	pIntersectionNode =pDirectSegment->GetPhysicalSeg()->GetNode1();
	//}
	pIntersectionNode =pDirectSegment->GetExitNode();

	taxiwaySegmets =pGroundRouteRes->GetAllLinkedDirectSegments(pIntersectionNode);

	size_t nSegCount = taxiwaySegmets.size();

	for (size_t i =0; i < nSegCount; ++i)
	{
		//			  ||left
		//			from	  ||        to
		//			=========> * =========>
		//					  ||
		//					  ||right
		//					  
		//
		FlightGroundRouteDirectSegInSim* pTempSegment =  taxiwaySegmets[i];
		if (taxiwaySegmets[i]->GetRouteSegInSim() == pDirectSegment->GetRouteSegInSim()) //ingore *->from  & from ->*
			continue;

		if (taxiwaySegmets[i]->GetRouteSegInSim() == pNextDirectSegment->GetRouteSegInSim())//remove *->To  &  To->*
			continue;

		if (taxiwaySegmets[i]->GetLockedFlight() != NULL)//ingore the segment has not locked, ingore left->* or right ->* 
			continue;

		//if (taxiwaySegmets[i]->IsPositiveDir() == true && taxiwaySegmets[i]->GetPhysicalSeg()->GetNode2() != pIntersectionNode)//ingore *->left
		//	continue;
		//if (taxiwaySegmets[i]->IsPositiveDir() == false && taxiwaySegmets[i]->GetPhysicalSeg()->GetNode1() != pIntersectionNode)//ingore *->right
		//	continue;
		if (taxiwaySegmets[i]->GetExitNode() != pIntersectionNode)
			continue;


		//only right->* or left->* remaining
		std::vector<CAirsideMobileElement *> vInFlight = taxiwaySegmets[i]->GetInResouceMobileElement();
		if (vInFlight.size()  == 0)
			continue;


		AirsideFlightInSim *pInFlight =  reinterpret_cast<AirsideFlightInSim *>(vInFlight[0]);			
		//OccupancyInstance  OccuInstance = taxiwaySegmets[i]->GetFirstOccupyInstance();
		//if (OccuInstance.GetElement())//
		if (pInFlight)
		{
			//check the flight's next resource
			{
				bool bHasNextRes = false;
				for (size_t j = 0; j < nSegCount ; ++ j)
				{
					if (pInFlight->GetResource() != NULL && 
						pInFlight->GetResource() != taxiwaySegmets[j] &&
						taxiwaySegmets[j]->bMobileInPath(pInFlight))
					{
						bHasNextRes = true;
						break;
					}
				}

				if ( bHasNextRes == false)
					return false;

			}

			if(pDirectSegment->IsDirectSegementOnRight(taxiwaySegmets[i]))//at intersection in right
			{
				CTaxiwayConflictResolutionInEngine *pConflictResolution =  pFlight->GetAirTrafficController()->GetTaxiwayConflictResolution();
				if (pConflictResolution->IsConflictRightFlight(pFlight,pInFlight))
				{
					nextEventTime = pInFlight->GetTime();
					return true;
				}

			}

			//first arrive at the taxiway has the right
			if(pNextDirectSegment->bMobileInPath(pInFlight))
			{
				CTaxiwayConflictResolutionInEngine *pConflictResolution =  pFlight->GetAirTrafficController()->GetTaxiwayConflictResolution();
				if (pConflictResolution->IsConflictNextTaxiway(pFlight,pInFlight))
				{	
					nextEventTime = pInFlight->GetTime()+ ElapsedTime(0.2);
					return true;
				}
			}

			//specified taxiway
			{
				FlightGroundRouteDirectSegInSim *pFlightGroundRouteSeg = taxiwaySegmets[i];
				if(pFlightGroundRouteSeg->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
				{
					CTaxiwayConflictResolutionInEngine *pConflictResolution =  pFlight->GetAirTrafficController()->GetTaxiwayConflictResolution();

					if (pConflictResolution->IsConflictSpecifiedTaxiway(((TaxiwayDirectSegInSim *)pFlightGroundRouteSeg)->GetTaxiwayID(),pFlight,pInFlight))
					{
						nextEventTime = pInFlight->GetTime()+ ElapsedTime(0.2);
						return true;
					}
				}

			}

		}


	}

	return false;

}



void TaxiRouteInSim::AddTaxiwaySegList(const FlightGroundRouteDirectSegList& segList ,bool isCyclicRoute)
{
	int nCount = (int)segList.size();
	for(int i=0;i<nCount;i++)
	{
		FlightGroundRouteDirectSegInSim * pSeg = segList.at(i);	
		if (GetMode() == OnTaxiToMeetingPoint && i == nCount -1)
		{
			double dDist = pSeg->GetEndDist();
			TaxiwaySegInSim* pTaxiSeg = ((TaxiwayDirectSegInSim*)pSeg)->GetTaxiwaySeg();
			if (pTaxiSeg)
			{
				ASSERT(!pTaxiSeg->GetMeetingPoints().empty());
				AirsideMeetingPointInSim* pAttachedPoint = pTaxiSeg->GetMeetingPoints().at(0);
				dDist = pSeg->GetPath().GetPointDist(pAttachedPoint->GetPosition());
			}
			TaxiRouteItemInSim newItem(pSeg,0,dDist);		
			AddItem(newItem,isCyclicRoute);
		}
		else
		{
			TaxiRouteItemInSim newItem(pSeg,0,pSeg->GetEndDist());		
			AddItem(newItem,isCyclicRoute);
		}
	}
	UpdateData();
}

//void TaxiRouteInSim::AddTaxiwaySegsToTempParking( TaxiwayDirectSegList& segList )
//{
//	for(int i=0;i<(int)segList.size();i++)
//	{
//		TaxiwayDirectSegInSim * pSeg = segList.at(i);
//        m_vItems.push_back(TaxiRouteItemInSim(pSeg,0,pSeg->GetEndDist()));		
//	}
//	UpdateData();
//}

TaxiRouteInSim::TaxiRouteInSim( AirsideMobileElementMode mode,AirsideResource* pOrign, AirsideResource* pDest )
{
	m_mode = mode;
	m_nCurItemIdxInRoute = -1;
	m_dTaxiSpd = 0.0;
	mpResOrig = pOrign;
	mpResDest = pDest;

	m_pLastAcrossHold = NULL;
}

TaxiRouteInSim::~TaxiRouteInSim()
{

}

int TaxiRouteInSim::GetItemIndex( AirsideResource * pRes ) const
{
	for(int i =0;i<GetItemCount();i++)
	{
		if(ItemAt(i).m_pRes == pRes && i>= m_nCurItemIdxInRoute )
			return i;
	}
	return -1;
}

std::vector<IntersectionNodeInSim*> TaxiRouteInSim::GetRouteNodeList() 
{
	std::vector<IntersectionNodeInSim* > vNodeList;
	for( int i= 0;i < GetItemCount();i++)
	{
		TaxiRouteItemInSim& theItem = ItemAt(i);
		if( theItem.GetResource() && theItem.GetResource()->IsKindof(typeof(FlightGroundRouteDirectSegInSim)) )
		{
			FlightGroundRouteDirectSegInSim* pSeg = (FlightGroundRouteDirectSegInSim*)theItem.GetResource();
			IntersectionNodeInSim* pEnterNode = pSeg->GetEntryNode();
			if( vNodeList.end() == std::find(vNodeList.begin(),vNodeList.end(),pEnterNode) )
			{
				vNodeList.push_back(pEnterNode);
			}
			IntersectionNodeInSim* pExitNode = pSeg->GetExitNode();
			if( vNodeList.end() == std::find(vNodeList.begin(),vNodeList.end(),pExitNode) )
			{
				vNodeList.push_back(pExitNode);
			}
		}

	}
	return vNodeList;
}

void TaxiRouteInSim::FlightExitRoute( AirsideFlightInSim* pFlight, const ElapsedTime& releaseTime )
{
	DynamicConflictGraph* pConflictGraph = getConflictGraph(pFlight);
	ASSERT(pConflictGraph);
	//clear every thing
	std::vector<IntersectionNodeInSim*> vNodes = GetRouteNodeList();
	for(int i=0;i<(int) vNodes.size();i++)
	{
		IntersectionNodeInSim* pTheNode = vNodes.at(i);
		OccupancyInstance fligthOCy = pTheNode->GetOccupyInstance(pFlight);
		if( fligthOCy.IsValid() && !fligthOCy.IsExitTimeValid() )
		{			
			pTheNode->SetExitTime(pFlight, releaseTime);
		}	
		if(pTheNode->GetLockedFlight() == pFlight)
		{
			pTheNode->OnFlightExit(pFlight,releaseTime);		
		}
	}
	for(int i=0;i< GetItemCount();i++)
	{
		AirsideResource* pRes = ItemAt(i).GetResource();
		if(pRes && pRes->IsKindof( typeof(FlightGroundRouteDirectSegInSim) ) )
		{
			FlightGroundRouteDirectSegInSim* pSeg = (FlightGroundRouteDirectSegInSim*)pRes;
			pConflictGraph->OnMobileLeave(pFlight,pSeg,releaseTime);//->ReleaseDirLock(pFlight, releaseTime);
			
		}
	}
	pConflictGraph->OnMobileUnPlanRoute(pFlight,releaseTime);


	
	for(int i=0;i<(int)m_vHoldList.GetCount();i++ )
	{
		HoldInTaxiRoute& theHold = m_vHoldList.ItemAt(i);
		if(theHold.m_bCrossed)
			continue; //ignore crossed holds
		//create log
		FlightCrossNodeLog* pCrossNodeLog = CreatFlightCrossNodeLog(pFlight,theHold,releaseTime,*this);
		pFlight->LogEventItem(pCrossNodeLog);
		theHold.m_bCrossed = true;
	}	

	m_LaneNotifyPtList.clearFlightLocks(pFlight,releaseTime);    

	//
	pFlight->NotifyOtherAgents(SimMessage().setTime(releaseTime));
}

void TaxiRouteInSim::UpdateData()
{
	DistanceUnit dLength = 0;
	m_vdAccLength.clear();	
	m_vdAccLength.push_back(0);

	for(int i=0;i< GetItemCount();i++)
	{
		const TaxiRouteItemInSim& theItem = ItemAt(i);
		DistanceUnit theItemDist =0;
		AirsideResource* pRes = theItem.GetResource();
		/*if( pRes && pRes->IsKindof(typeof(FlightGroundRouteDirectSegInSim)) )
		{
			FlightGroundRouteDirectSegInSim* pSeg = (FlightGroundRouteDirectSegInSim*) theItem.GetResource();
			theItemDist = pSeg->GetLength();
		}
		else*/
		{
			theItemDist = theItem.GetDistTo() - theItem.GetDistFrom();
		}
		dLength += theItemDist;		
		m_vdAccLength.push_back(dLength);
	}
	m_vFiletPts.Init(*this);	
}


int TaxiRouteInSim::AddTaxiRouteClearanceItems( AirsideFlightInSim* pFlight,  const DistanceUnit nextDist,double endSpd, double dAngle, ClearanceItem& lastClearanceItem, Clearance& newClearance )
{
	int nAddItemCount = 0;
	
	TaxiRouteInSim& theRoute = *this;
	int nNextItemIdx; DistanceUnit nextDistInItem;
	theRoute.GetItemIndexAndDistInRoute(nextDist,  nNextItemIdx, nextDistInItem);
	const int nCurItemIndex = theRoute.GetItemIndex(lastClearanceItem.GetResource());
	const DistanceUnit dCurDistInItem = lastClearanceItem.GetDistInResource();

	DistanceUnit curDistInRoute = theRoute.GetDistInRoute(nCurItemIndex, dCurDistInItem);

	std::vector<FiletPointInTaxiRoute> midFiltPts = theRoute.m_vFiletPts.GetFiletPointsBetween(curDistInRoute, nextDist);
	std::vector<HoldInTaxiRoute> midHolds = theRoute.GetHoldList().GetHoldFromTo(curDistInRoute,nextDist);
	std::vector<DistanceUnit> vDistInRoute;
	for(int i=0;i< (int)midFiltPts.size();i++)
		vDistInRoute.push_back( midFiltPts[i].GetDistInRoute() );
	for( int i=0;i< (int)midHolds.size();i++)
		vDistInRoute.push_back( midHolds[i].m_dDistInRoute );

	std::sort(vDistInRoute.begin(),vDistInRoute.end());
	DistanceUnit lastItemDistInRoute = curDistInRoute;
	
	double avgspd = (lastClearanceItem.GetSpeed() + endSpd)*0.5;
	ElapsedTime dT = ElapsedTime((nextDist - curDistInRoute)/avgspd);
	ElapsedTime endTime = lastClearanceItem.GetTime() + dT;
	for(int i=0;i< (int)vDistInRoute.size();i++)
	{
		//FiletPointInTaxiRoute& theFtPt = midFiltPts[i];
		DistanceUnit distInRoute  = vDistInRoute.at(i);
		
		double dAlpha = (distInRoute - curDistInRoute)/(nextDist - curDistInRoute);
		int nItem; DistanceUnit dDist;
		theRoute.GetItemIndexAndDistInRoute(distInRoute, nItem, dDist);
		double dSpd = lastClearanceItem.GetSpeed()*(1-dAlpha) + endSpd * dAlpha;
		ElapsedTime atTime = lastClearanceItem.GetTime() + ElapsedTime(dAlpha*(double)dT);
		ClearanceItem newItem(theRoute.ItemAt(nItem).GetResource(), theRoute.GetMode(), dDist);
		newItem.SetSpeed(dSpd);
		newItem.SetTime(atTime);		
		lastClearanceItem = newItem;
		newClearance.AddItem(lastClearanceItem);	
		nAddItemCount++;
	}
	//add last item
	int nItem; DistanceUnit dDist;
	theRoute.GetItemIndexAndDistInRoute(nextDist, nItem, dDist);
	ClearanceItem lastItem(theRoute.ItemAt(nItem).GetResource(), theRoute.GetMode(), dDist);
	lastItem.SetSpeed(endSpd);
	lastItem.SetTime(endTime);
	lastItem.SetOffsetAngle(dAngle);
	lastClearanceItem = lastItem;
	newClearance.AddItem(lastClearanceItem);
	nAddItemCount++;
	
	return nAddItemCount;
}

void TaxiRouteInSim::GetTaxiInterruptLinesInRoute(std::vector<TaxiInterruptLineInSim*>& vInterruptLines)
{
	int nCount = m_vItems.size();
	for (int i =0; i < nCount; i++)
	{
		TaxiRouteItemInSim* pItem = &m_vItems.at(i);
		if (pItem->GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
		{
			TaxiwaySegInSim* pSeg = ((TaxiwayDirectSegInSim*)pItem->GetResource())->GetTaxiwaySeg();
			std::vector<TaxiInterruptLineInSim*> vLines = pSeg->GetHoldShortLine();

			if (vLines.empty())
				continue;

			vInterruptLines.insert(vInterruptLines.end(),vLines.begin(),vLines.end());
		}

	}
}

//Temperately parking 
bool TaxiRouteInSim::FindClearanceAsTempParking( AirsideFlightInSim* pFlight, ClearanceItem& lastClearanceItem, Clearance& newClearance, double offsetAngle )
{
	DistanceUnit dOperateAnglePosDistance = 2000;
	DistanceUnit dOperateUnAnglePosDistance = 2000;
	const double taxispd = ( (m_mode<OnHeldAtStand)?pFlight->GetPerformance()->getTaxiInNormalSpeed(pFlight): pFlight->GetPerformance()->getTaxiOutNormalSpeed(pFlight) );

	if(offsetAngle == 0 ) //as the acc and dec distance
	{
		dOperateUnAnglePosDistance = 1000;
		dOperateAnglePosDistance = 2000;
	}
	//
	if( GetItemCount() < 1) 
		return false;

	const int nBeginItemIdx = GetItemIndex( lastClearanceItem.GetResource() );
	const DistanceUnit dCurDistInRoute = GetDistInRoute( nBeginItemIdx , lastClearanceItem.GetDistInResource() );
	m_vHoldList.Init(*this, pFlight);

	DistanceUnit dLastEntryHoldDist = *m_vdAccLength.rbegin() - pFlight->GetLength();
	HoldInTaxiRoute* lastEntryHold =  m_vHoldList.GetLastEntryHold() ;
	if(lastEntryHold)
	{
		dLastEntryHoldDist = lastEntryHold->m_dDistInRoute;
	}
	
	if(dLastEntryHoldDist <= dCurDistInRoute )//check to see the flight arrival at the end of the Route
		return false;
	
	//first enter the hold 
	DistanceUnit dEntryDist = 0;
	HoldInTaxiRoute* pFirstExitHold = m_vHoldList.GetFirstExitHold();
	if(pFirstExitHold)
	{
		dEntryDist = pFirstExitHold->m_dDistInRoute;
	}

	if( dCurDistInRoute < dEntryDist ) //add first item
	{
		ClearanceItem firstRouteItem( ItemAt(0).GetResource(), GetMode(), ItemAt(0).GetDistFrom() );
		firstRouteItem.SetSpeed(taxispd);
		ElapsedTime dT = pFlight->GetTimeBetween(lastClearanceItem, firstRouteItem);
		firstRouteItem.SetTime(lastClearanceItem.GetTime()+dT);

		int nItem; DistanceUnit distInItem;
		GetItemIndexAndDistInRoute(dEntryDist,nItem,distInItem);
		ClearanceItem entryRouteItem( ItemAt(nItem).GetResource(),GetMode(), distInItem );
		entryRouteItem.SetSpeed(taxispd);
		dT = ElapsedTime( dEntryDist/taxispd );
		entryRouteItem.SetTime( firstRouteItem.GetTime() + dT );

		lastClearanceItem = entryRouteItem;
		newClearance.AddItem(lastClearanceItem);
		return true;	
	}

	//TaxiRouteSeperationWithLeadFlight sepLeadFlight(*this, pFlight, dCurDistInRoute);
	ARCMobileElement* pLeadMob; DistanceUnit distOfLead;
	boost::tie(pLeadMob,distOfLead) = getLeadMobile(pFlight,*this,dCurDistInRoute);
	if( pLeadMob ) //there is flight ahead
	{
		AirsideFlightInSim* pLeadFlight = (AirsideFlightInSim*)pLeadMob;	
		{
			DistanceUnit distSep = ( pLeadFlight->GetLength() + pFlight->GetLength() )* cos(offsetAngle)*0.5 + TempParkingSegmentsInSim::defaultParkingSep;
			DistanceUnit parkingDist = distOfLead - distSep;
			//DistanceUnit curSeperation = parkingDist - dCurDistInRoute;
			if(dCurDistInRoute+minTravelDist<=parkingDist)
			{
				DistanceUnit nextDist = parkingDist;
				TravelToNextDistInRoute(pFlight,*this,dCurDistInRoute,nextDist,0,lastClearanceItem,newClearance);//AddTaxiRouteClearanceItems(pFlight, nextDist, 0, offsetAngle,lastClearanceItem,newClearance );
				return true;
			}
			else
			{
				pFlight->DependOnAgents(pLeadFlight);//pFlight->GetWakeUpCaller().OberverSubject(pLeadFlight);
				return true;
			}			
		}		
		
	}
	else //walk to the last hold dist straight 
	{
		//with speed taxi speed
		//DistanceUnit dNextDsit1 = dLastEntryHoldDist - dOperateAnglePosDistance; 
		//TravelToNextDistInRoute(pFlight,*this,dCurDistInRoute,nextDist,taxispd,lastClearanceItem,newClearance);
		//AddTaxiRouteClearanceItems( pFlight, dNextDsit1, taxispd, 0,lastClearanceItem,newClearance );
		//with speed 0
		DistanceUnit dNextDist2 = dLastEntryHoldDist;
		TravelToNextDistInRoute(pFlight,*this,dCurDistInRoute,dNextDist2,taxispd,lastClearanceItem,newClearance);
		//AddTaxiRouteClearanceItems( pFlight, dNextDist2, 0, offsetAngle,lastClearanceItem,newClearance );		
		return true;
	}
	return true;
}

void TaxiRouteInSim::AddItem( const TaxiRouteItemInSim& newItem ,bool isCyclicRoute)
{
	if (!isCyclicRoute)
	{
		FlightGroundRouteDirectSegInSim* pNewSeg = (FlightGroundRouteDirectSegInSim*)newItem.GetResource();//newItem.GetResource();

		std::vector<TaxiRouteItemInSim>::iterator itrFind = m_vItems.begin();	
		for(;itrFind!=m_vItems.end();itrFind++)
		{
			FlightGroundRouteDirectSegInSim* pOldSeg = (FlightGroundRouteDirectSegInSim*)itrFind->GetResource();
			if( pOldSeg->GetEntryNode() == pNewSeg->GetExitNode() )
				break;
		}

		if(itrFind!=m_vItems.end())
		{
			m_vItems.erase(itrFind,m_vItems.end());
		}
		else
		{
			m_vItems.push_back(newItem);
		}
	}else
	{
		m_vItems.push_back(newItem);
	}
	

		
}

void TaxiRouteInSim::TravelRunwaySegInRoute( AirsideFlightInSim* pFlight,TaxiRouteInSim& theRoute,double taxiSpd, const DistanceUnit& curDist, ClearanceItem& lastClearanceItem, Clearance& newClearance ,const ElapsedTime& supEndTime )
{
	//get current segment
	int nCurItemIdx;
	DistanceUnit curDistInItem;
	theRoute.GetItemIndexAndDistInRoute(curDist,nCurItemIdx,curDistInItem);
	//int nNextItemIdx; 
	DistanceUnit nextDist = curDist;
	//theRoute.GetItemIndexAndDistInRoute(nextDist,nNextItemIdx, nextDistInItem);
	//const ElapsedTime startTime= lastClearanceItem.GetTime();
	
	//find the runway segments 
	int nRunwaySegEndIndex = nCurItemIdx;
	int nItemCount = theRoute.GetItemCount();

	for (int nItem = nCurItemIdx + 1; nItem < nItemCount; ++ nItem)
	{
		TaxiRouteItemInSim taxiRouteItem = theRoute.GetItem(nItem);

		AirsideResource *pRunwaySegResource = theRoute.ItemAt(nCurItemIdx).GetResource();
		if(taxiRouteItem.GetResource()->GetType() == AirsideResource::ResType_RunwayDirSeg)
		{
			RunwayDirectSegInSim *pCurRunwaySeg = (RunwayDirectSegInSim *)pRunwaySegResource;	
			double dSegDistance = pCurRunwaySeg->GetEndDist();
			nextDist += dSegDistance;
		}
		else
		{
			break;
		}
	}
	TravelToNextDistInRoute(pFlight,theRoute,taxiSpd,curDist,nextDist,lastClearanceItem,newClearance);

	////travel from current resource to nRunwaySegIndex
	////check the current item exit node available or not
	//Clearance crossRunwaySegClearance;
	//ElapsedTime eCurTime = lastClearanceItem.GetTime();
	//AirsideResource *pCurrentResource = theRoute.ItemAt(nCurItemIdx).GetResource();
	//if(pCurrentResource->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
	//{
	//	TaxiwayDirectSegInSim *pCurTaxiwaySeg = (TaxiwayDirectSegInSim *)pCurrentResource;
	//	ElapsedTime AvaEnterTime = pCurTaxiwaySeg->GetExitNode()->GetAvailableEntryTime(pFlight, eCurTime ,eCurTime + ElapsedTime(10L) );		
	//	lastClearanceItem.SetTime(AvaEnterTime);
	//	crossRunwaySegClearance.AddItem(lastClearanceItem);
	//}

	//for (int i = nCurItemIdx + 1; i <= nRunwaySegEndIndex; ++ i)
	//{	
	//	
	//	AirsideResource *pRunwaySegResource = theRoute.ItemAt(nCurItemIdx).GetResource();
	//	if(pCurrentResource->GetType() == AirsideResource::ResType_RunwayDirSeg)
	//	{	
	//		RunwayDirectSegInSim *pCurRunwaySeg = (RunwayDirectSegInSim *)pCurrentResource;
	//		//from pre item to current item time
	//		CPoint2008 ptEntryHold = pCurRunwaySeg->GetDistancePoint(pCurRunwaySeg->GetEntryHoldDist());
	//
	//		ElapsedTime dTurnArrountTime = ElapsedTime(lastClearanceItem.GetPosition().getDistanceTo(ptEntryHold)/taxiSpd);
	//		
	//		ElapsedTime eTimeAtEnd = ElapsedTime((pCurRunwaySeg->GetExitHoldDist()- pCurRunwaySeg->GetEntryHoldDist())/taxiSpd);
	//		eCurTime = lastClearanceItem.GetTime() + dTurnArrountTime + eTimeAtEnd;

	//		ElapsedTime AvaEnterTime = pCurRunwaySeg->GetExitNode()->GetAvailableEntryTime(pFlight, eCurTime ,eCurTime + ElapsedTime(10L) );		
	//		lastClearanceItem.SetTime(AvaEnterTime);
	//		crossRunwaySegClearance.AddItem(lastClearanceItem);

	//	}
	//	else
	//	{

	//		TRACE("\nException .. TravelRunwaySegInRoute \n");
	//	}
	//}
}

bool TaxiRouteInSim::IsNextResourceIsRunwaySegment( TaxiRouteInSim& theRoute,const DistanceUnit& curDist )
{
	int nCurItemIdx;
	DistanceUnit curDistInItem;
	theRoute.GetItemIndexAndDistInRoute(curDist,nCurItemIdx,curDistInItem);
	int nItemCount = theRoute.GetItemCount();   
	if(nCurItemIdx + 1 < nItemCount)
	{
		TaxiRouteItemInSim taxiRouteItem = theRoute.GetItem(nCurItemIdx + 1);

		if(taxiRouteItem.GetResource()->GetType() == AirsideResource::ResType_RunwayDirSeg)
			return true;
	}

	return false;
}
//taxiway adjacency lead mobile
boost::tuple<ARCMobileElement*, DistanceUnit> getAdjacencyLeadMobile(int idx,AirsideFlightInSim* pFlt,TaxiRouteInSim& theRoute)
{
	int itemIdx = max(0,idx);
	TaxiRouteItemInSim& theItem = theRoute.ItemAt(itemIdx);
	AirsideResource* theResource = theItem.GetResource();
	if(theResource->IsKindof(typeof(RouteDirPath)) )
	{
		RouteDirPath* rDPath = (RouteDirPath*)theResource;
		double dDist = 0.0;
		ARCMobileElement* lMob = rDPath->getAdjacencyLeadMobile(pFlt,dDist);
		if (lMob && lMob != pFlt)
		{
			DistanceUnit distInRoute = theRoute.GetDistInRoute(itemIdx,dDist);
			return boost::make_tuple(lMob,distInRoute);
		}
	}

	ARCMobileElement* pLead = NULL;
	DistanceUnit dist = 0;
	return boost::make_tuple(pLead,dist);
}

//conflict with lead mobile
boost::tuple<ARCMobileElement*, DistanceUnit> getLeadMobile(AirsideFlightInSim* pFlt ,TaxiRouteInSim& theRoute, DistanceUnit distInRoute)
{
	int nBeginItemIdx;
	DistanceUnit dDistInItem;
	theRoute.GetItemIndexAndDistInRoute(distInRoute,nBeginItemIdx,dDistInItem);
//find adjacency flight that mapping to current route
	ARCMobileElement* pLeadFlight = NULL;
	double dDist = 0.0;

	//continue searching next route items
	for(int i=max(0,nBeginItemIdx);i<theRoute.GetItemCount();i++)
	{
		TaxiRouteItemInSim& theItem = theRoute.ItemAt(i);
		AirsideResource* theResource = theItem.GetResource();
		if(theResource->IsKindof(typeof(RouteDirPath)) )
		{

			RouteDirPath* rDPath = (RouteDirPath*)theResource;
			ARCMobileElement* lMob = rDPath->getLeadMobile(pFlt);
			if(!lMob && i==(theRoute.GetItemCount()-1) ) //check the last node 
			{
				if( theResource->IsKindof(typeof(FlightGroundRouteDirectSegInSim)) )
				{
					FlightGroundRouteDirectSegInSim* pSeg = (FlightGroundRouteDirectSegInSim*)theResource;
					AirsideFlightInSim* pInNodeFlt = pSeg->GetExitNode()->GetLockedFlight();
					if(pInNodeFlt && pInNodeFlt!=pFlt)
					{
						return boost::make_tuple(pInNodeFlt,theRoute.GetEndDist());
					}
				}
			}

			boost::tie(pLeadFlight,dDist) = getAdjacencyLeadMobile(i,pFlt,theRoute);
			if(lMob && lMob!=pFlt  ) //find the mob
			{
				DistanceUnit lmobDist = 0;
				
				if( lMob->getCurDirPath() == rDPath)
				{
					lmobDist = theRoute.GetDistInRoute(i,lMob->getCurDistInDirPath());
				}
				else
				{
					OccupancyInstance inst = rDPath->GetOccupyInstance((CAirsideMobileElement*)lMob);
					if(inst.IsEnterTimeValid())
					{
						lmobDist = theRoute.GetDistInRoute(i+1,0);
					}
					else
					{
						lmobDist = theRoute.GetDistInRoute(i,0);
					}
				}

				if(lmobDist>distInRoute)
				{
					if (!pLeadFlight || dDist > lmobDist)
					{
						pLeadFlight = lMob;
						dDist = lmobDist;
					}
				}
			}

			if (pLeadFlight)
			{
				return boost::make_tuple(pLeadFlight,dDist);
			}
		}
	}
	return boost::make_tuple(pLeadFlight,dDist);
}

boost::tuple<DistanceUnit,double, SimAgent*,DistanceUnit> TaxiRouteInSim::checkConflictWithLeadMobile(AirsideFlightInSim* pFlight,DistanceUnit mCurDistInRoute, DistanceUnit dRadOfConcern)
{	
	//find the lead mobile element
	ARCMobileElement* pLeadMobile=NULL;DistanceUnit leadMobDist;

	boost::tie(pLeadMobile,leadMobDist) = getLeadMobile(pFlight,*this, mCurDistInRoute);

	if(pLeadMobile ) //can see a flight ahead
	{		
		
		DistanceUnit dSeperation  = pLeadMobile->getCurSeparation() + pLeadMobile->GetLength()*0.5 + pFlight->GetLength()*0.5;

		double dNormalSpd = pFlight->getMobilePerform().getNormalSpd();
		DistanceUnit dSafeDist = leadMobDist - dSeperation;
		
		//double dLeadSpd = min(pLeadMobile->getCurSpeed(),dNormalSpd );
		DistanceUnit dSpdChageDist = pFlight->mGroundPerform.getSpdChangeDist(pFlight->mGroundPerform.getNormalSpd(),0);
		
		DistanceUnit dSafeDistNoSpdChange = dSafeDist - dSpdChageDist;

		if(pFlight->GetMode()==OnExitRunway)
		{
			FiletPointInTaxiRoute *pWaitfilet = m_vFiletPts.IsDistInFilet(dSafeDist);
			if(pWaitfilet)
				dSafeDist = pWaitfilet->m_distInRoute;
		}
		else
		{
			const HoldInTaxiRoute* pHWaitHold = m_vHoldList.GetWaitHold(dSafeDist);
			if(pHWaitHold)
				dSafeDist = pHWaitHold->m_dDistInRoute;

			pHWaitHold = m_vHoldList.GetWaitHold(dSafeDistNoSpdChange);
			if(pHWaitHold)
				dSafeDistNoSpdChange = pHWaitHold->m_dDistInRoute;
		}

		if( int(mCurDistInRoute)< int(dSafeDistNoSpdChange) && dSafeDistNoSpdChange >= 0)
		{
			return boost::make_tuple( dSafeDistNoSpdChange, dNormalSpd , (ARCMobileElement*)NULL,leadMobDist-dSeperation);
		}
		else if( int(mCurDistInRoute) < int(dSafeDist) && dSafeDist >= 0)
		{
			return boost::make_tuple( dSafeDist, 0, pLeadMobile,leadMobDist-dSeperation );
		}
		else
		{
			return boost::make_tuple( mCurDistInRoute, 0, pLeadMobile,leadMobDist-dSeperation);
		}			
	}

	//after all try to use normal speed
	return boost::make_tuple( GetEndDist(),pFlight->getMobilePerform().getNormalSpd(),(ARCMobileElement*)NULL,GetEndDist());
}

//////////////////////////////////////////////////////////////////////////
SimAgent* _bCanCrossTheHold(AirsideFlightInSim* pFlight, ARCMobileElement*pLeadFligth, TaxiRouteInSim&theRoute, HoldInTaxiRoute* pHold)
{
	pConflictFlt = NULL;
	pConflictNode = NULL;

	if(pHold)
	{
		
		AirsideFlightInSim* pInHoldFlt = pHold->m_pNode->GetLockedFlight();
		if(pInHoldFlt!=pLeadFligth && pInHoldFlt)
		{	
			if(getConflictGraph(pFlight)->CanAWaitForB(pFlight,pInHoldFlt))
			{
				pConflictNode = pHold->m_pNode;
				pConflictFlt = pInHoldFlt;
				return pHold->m_pNode;
			}			
		}
		
		for(int i=0;i<theRoute.GetItemCount();i++)
		{
			TaxiRouteItemInSim& theItem = theRoute.ItemAt(i);
			if(theItem.m_pRes && theItem.m_pRes->IsKindof( typeof(FlightGroundRouteDirectSegInSim) ) )
			{
				FlightGroundRouteDirectSegInSim* pSeg = (FlightGroundRouteDirectSegInSim*)theItem.m_pRes;
				if( pSeg == pHold->mLinkDirSeg )
				{
					ARCMobileElement* pConflictMob = getConflictGraph(pFlight)->bMobileForbidTo(pFlight,pSeg);
					if(pConflictMob && pConflictMob!=pFlight)
					{						
						if(pConflictMob && pConflictMob->IsKindof(typeof(AirsideFlightInSim)))
						{
							pConflictFlt = (AirsideFlightInSim*)pConflictMob;
							pConflictNode = NULL;
						}
						return pHold->m_pNode;
					}
					break;
				}
				
			}
		}		
	}
	return NULL;
}

boost::tuple<DistanceUnit, double ,SimAgent*,DistanceUnit> TaxiRouteInSim::checkConflictWithNextHold(AirsideFlightInSim* mpFlight, ARCMobileElement* pleadFlight, DistanceUnit mCurDistInRoute)
{
	double dMoveSpd =  mpFlight->getMobilePerform().getNormalSpd();
	DistanceUnit dDecDistToStatic = mpFlight->getMobilePerform().getSpdChangeDist(dMoveSpd,0); //dist to stop

	std::vector<HoldInTaxiRoute> vNodeHoldList= m_vHoldList.GetNextRouteNodeHoldList(mCurDistInRoute);


	for(int idxHD=0;idxHD<(int)vNodeHoldList.size();++idxHD)
	{
		HoldInTaxiRoute* pNextHold = &vNodeHoldList.at(idxHD);
		if(pNextHold && pNextHold->m_hHoldType==HoldInTaxiRoute::ENTRYNODE)
		{
			DistanceUnit distCheckHold = pNextHold->m_dDistInRoute;
			distCheckHold -= dDecDistToStatic;

			if( int(mCurDistInRoute) < int(distCheckHold) && distCheckHold >=0 ) //flight can move to the checkpoint
			{
				if( !m_vHoldList.GetWaitHold(distCheckHold) )
					return boost::make_tuple(distCheckHold, dMoveSpd, (SimAgent*)NULL, pNextHold->m_dDistInRoute);	
			}
			
			SimAgent *pConflictAgent = _bCanCrossTheHold(mpFlight, pleadFlight,*this, pNextHold);
			//bool bDelay = mpFlight->getCurTime() < pNextHold->m_pNode->GetAvailableEntryTime(mpFlight,mpFlight->getCurTime(),mpFlight->getCurTime()+ElapsedTime(1L));
			if(pConflictAgent)
			{
				DistanceUnit nextDist = pNextHold->m_dDistInRoute;
				const HoldInTaxiRoute* pWaitHold = m_vHoldList.GetWaitHold(nextDist);
				if(pWaitHold)
					nextDist = pWaitHold->m_dDistInRoute;
				if( int(mCurDistInRoute) < int(nextDist) && nextDist>= 0 )
				{
					return  boost::make_tuple(nextDist,0,(SimAgent*)NULL,nextDist);
				}
				else
				{
					return boost::make_tuple(mCurDistInRoute,0,pConflictAgent,nextDist);
				}
			}
			
			
		}
	}	

	return boost::make_tuple( GetEndDist(),dMoveSpd,(SimAgent*)NULL,GetEndDist());
}

// get the entry hold that the flight is about to enter at this moment,
// it means the flight is now at the position near the hold,
// NULL if not found
bool TaxiRouteInSim::IsFlightToEnterHold(AirsideFlightInSim* mpFlight, DistanceUnit mCurDistInRoute)
{
	//double dMoveSpd =  mpFlight->getMobilePerform().getNormalSpd();
	//DistanceUnit dDecDistToStatic = mpFlight->getMobilePerform().getSpdChangeDist(dMoveSpd,0); //dist to stop
	//std::vector<HoldInTaxiRoute> vNodeHoldList= m_vHoldList.GetNextRouteNodeHoldList(mCurDistInRoute);
	//HoldInTaxiRoute* pNextHold = m_vHoldList.GetNextNodeEntryHold(mCurDistInRoute);
	//DistanceUnit distCheckHold = pNextHold->m_dDistInRoute - dDecDistToStatic;
	//if( abs(distCheckHold - mCurDistInRoute) < ARCMath::EPSILON && distCheckHold >= 0.0 ) //flight can move to the checkpoint
	//{
	//	HoldInTaxiRoute& nextHold = vNodeHoldList.at(idxHD);
	//	if(nextHold.m_hHoldType==HoldInTaxiRoute::ENTRYNODE)
	//	{
	//		DistanceUnit distCheckHold = nextHold.m_dDistInRoute - dDecDistToStatic;  

	return false;
}


//check conflict with the interrupt line returns < endDist, end speed, Interrupt line,line dist>
boost::tuple<DistanceUnit,double , TaxiInterruptLineInSim*, DistanceUnit, ElapsedTime> TaxiRouteInSim::checkConflictWithInterruptLine( AirsideFlightInSim*pFligt, DistanceUnit mCurDistInRoute )
{
	std::vector<FlightInterruptPosInRoute> vInterruptLines = m_InterruptLineList.GetNextItems(mCurDistInRoute);
	for(int idxIL =0; idxIL < (int)vInterruptLines.size();idxIL++)
	{
		FlightInterruptPosInRoute & interPos = vInterruptLines[idxIL];
		if( interPos.mpInterruptLine->GetWaitFlight() == pFligt)
			continue;

		ElapsedTime holdTime;
		if( interPos.mpInterruptLine->bInterruptFlight(pFligt,interPos.m_startNodeId,holdTime) )
		{
			return boost::make_tuple( interPos.m_distInRoute, 0, interPos.mpInterruptLine, interPos.m_distInRoute,holdTime);
		}
	}
	return boost::make_tuple(GetEndDist(), 0, (TaxiInterruptLineInSim*)NULL, GetEndDist(),ElapsedTime(0L));
}

AirsideMeetingPointInSim* TaxiRouteInSim::GetFirstMeetingPointInRoute( const std::vector<ALTObjectID>& vMeetingPoints )
{			
	AirsideMeetingPointInSim* pMeetingPoint = NULL;

	int nCount = GetItemCount();
	for (int i =0; i < nCount; i++)
	{
		AirsideResource* pRes = ItemAt(i).GetResource();
		if (pRes->IsA(typeof(TaxiwayDirectSegInSim)))
		{
			pMeetingPoint = ((TaxiwayDirectSegInSim*)pRes)->GetTaxiwaySeg()->GetMeetingPointInSeg(vMeetingPoints);

			if (pMeetingPoint != NULL)
				return pMeetingPoint;
		}

	}

	return pMeetingPoint;
}

bool TaxiRouteInSim::GetSubRouteToMeetingPoint( const AirsideMeetingPointInSim* pMeetingPoint, FlightGroundRouteDirectSegList& segList )
{
	TaxiwaySegInSim* pAttachedSeg = (const_cast<AirsideMeetingPointInSim*>(pMeetingPoint))->GetAttachedTaxiwaySegment();

	int nCount = GetItemCount();
	for (int i =0; i < nCount; i++)
	{
		AirsideResource* pRes = ItemAt(i).GetResource();
		segList.push_back((FlightGroundRouteDirectSegInSim*)pRes);

		if (pRes->IsA(typeof(TaxiwayDirectSegInSim)))
		{
			if (((TaxiwayDirectSegInSim*)pRes)->GetTaxiwaySeg() == pAttachedSeg)
				return true;			
		}

	}
	return false;
}

void TaxiRouteInSim::PlanRoute(AirsideFlightInSim* pFlt,const ElapsedTime& t)
{
	RouteDirPathList routeList = getNextPathList(-1); //all route
	getConflictGraph(pFlt)->OnMobilePlaneRoute(pFlt,routeList,t);
}

void TaxiRouteInSim::GetTaxiStartPositionsInRoute( std::vector<StartPositionInSim*>& vStartPositions )
{
	size_t nCount = m_vItems.size();
	for (size_t i =0; i < nCount; i++)
	{
		TaxiRouteItemInSim* pItem = &m_vItems.at(i);
		if (pItem->GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
		{
			TaxiwaySegInSim* pSeg = ((TaxiwayDirectSegInSim*)pItem->GetResource())->GetTaxiwaySeg();
			std::vector<StartPositionInSim*> vStartpos = pSeg->GetStartPostions();

			if (vStartpos.empty())
				continue;

			vStartPositions.insert(vStartPositions.end(),vStartpos.begin(),vStartpos.end());
		}

	}
}

IntersectionNodeInSim* TaxiRouteInSim::GetLastNode()
{
	
	if(GetItemCount())
	{
		TaxiRouteItemInSim& theItem = ItemAt(GetItemCount()-1);
		if( theItem.GetResource() && theItem.GetResource()->IsKindof(typeof(FlightGroundRouteDirectSegInSim)) )
		{
			FlightGroundRouteDirectSegInSim* pSeg = (FlightGroundRouteDirectSegInSim*)theItem.GetResource();				
			IntersectionNodeInSim* pExitNode = pSeg->GetExitNode();
			return pExitNode;
		}

	}
	return NULL;
}

DistanceUnit TaxiRouteInSim::GetExitRouteDist(AirsideFlightInSim* pFlight) 
{
	DistanceUnit dExitRouteDist = GetEndDist() - pFlight->GetLength();//default exit route dist

	HoldInTaxiRoute* lastEntryHold =  GetlastEntryHold();
	if(lastEntryHold)
	{ 
		dExitRouteDist = min(dExitRouteDist,lastEntryHold->m_dDistInRoute);
	}
	return dExitRouteDist;
}

HoldInTaxiRoute* TaxiRouteInSim::GetlastEntryHold()
{
	HoldInTaxiRoute* lastEntryHold =  m_vHoldList.GetLastEntryHold() ; //find last node entry hold
	if(mpResDest && mpResDest->GetType() == AirsideResource::ResType_LogicRunway)
	{
		LogicRunwayInSim* pLogicRunway = (LogicRunwayInSim*)mpResDest; 
		HoldInTaxiRoute* lastRunayHold= m_vHoldList.GetLastRunwayEntryHold();
		if( lastRunayHold && lastRunayHold->m_pNode->GetNodeInput().GetRunwayItem(pLogicRunway->GetRunwayInSim()->GetId()) )
		{
			lastEntryHold = lastRunayHold;
		}		
	}
	return lastEntryHold;
}

void TaxiRouteInSim::InitRoute( AirsideFlightInSim* pFlight,const ElapsedTime&t  )
{
	if(m_vHoldList.m_pFlight==pFlight)
		return;

	UpdateData();
	m_vHoldList.Init(*this, pFlight,pFlight->GetMode() == OnExitRunway);
	m_LaneNotifyPtList.Init(*this,pFlight);
	m_InterruptLineList.Init(*this,pFlight);
	PlanRoute(pFlight,t);

	if( GetMode()==OnTowToDestination)
	{
		pFlight->mGroundPerform.mNoramlSpd = pFlight->GetPerformance()->getTowingSpeed(pFlight);
		pFlight->mGroundPerform.mAccSpd = pFlight->GetPerformance()->getPushBackAcc(pFlight);
		pFlight->mGroundPerform.mDecSpd = pFlight->GetPerformance()->getPushBackDec(pFlight);

	}
	if(GetMode() == OnTaxiToRunway || GetMode() == OnTaxiToDeice)
	{
		pFlight->mGroundPerform.mNoramlSpd = pFlight->GetPerformance()->getTaxiOutNormalSpeed(pFlight);
		pFlight->mGroundPerform.mAccSpd = pFlight->GetPerformance()->getTaxiOutAccelation(pFlight);
		pFlight->mGroundPerform.mDecSpd = pFlight->GetPerformance()->getTaxiOutDeceleration(pFlight);
	}
	if(GetMode() == OnTaxiToStand || GetMode() == OnTaxiToMeetingPoint || GetMode() == OnTaxiToTempParking)
	{
		pFlight->mGroundPerform.mNoramlSpd = pFlight->GetPerformance()->getTaxiInNormalSpeed(pFlight);
		pFlight->mGroundPerform.mAccSpd = pFlight->GetPerformance()->getTaxiInAccelation(pFlight);
		pFlight->mGroundPerform.mDecSpd = pFlight->GetPerformance()->getTaxiInDeceleration(pFlight);
	}
}


bool TaxiRouteInSim::IsRouteValide( AirsideFlightInSim* pFlt ) const
{
	return pFlt->GetAirTrafficController()->getConflictGraph()->bTryPlanRoute(pFlt,getNextPathList(-1));	
}