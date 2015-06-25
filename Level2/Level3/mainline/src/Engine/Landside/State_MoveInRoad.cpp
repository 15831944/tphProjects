#include "StdAfx.h"
#include ".\State_MoveInRoad.h"
#include "..\LandsideResourceManager.h"
#include "..\ARCportEngine.h"
#include "..\LandsideSimulation.h"
#include "..\CrossWalkInSim.h"
#include "../LandsideIntersectionInSim.h"
#include "../LandsideExternalNodeInSim.h"
#include "../LandsideRoundaboutInSim.h"
#include "Common/DynamicMovement.h"
#include "Common/ARCTracker.h"
#include "LandsideNonPaxVehicleInSim.h"
#include "Common/FileOutPut.h"
#include "../LandsideParkingLotInSim.h"
#define  MAX_BREAK_ACC 100*100


void State_MoveInRoad::Execute( CARCportEngine* _pEngine )
{	
	if(m_path.getPtCount()==0)
	{
		ASSERT(false);
		return OnMoveOutRoute(_pEngine);
	}

	LandsideVehicleInSim* pVehicle = getVehicle();
	
	if(_isOutRoute())
	{		
		MobileState lastState = pVehicle->getLastState();
		if(m_bStopAtEnd )
		{
			DistanceUnit dS = m_path.getAllLength() - pVehicle->GetLength()*0.5 - m_dLastDistInRoute;
			dS = MAX(0,dS);			
			DoMovementsInPath(_pEngine,lastState.time, dS, lastState.dSpeed,0 );	
		}
		else
		{
			DistanceUnit dS = m_path.getAllLength()-m_dLastDistInRoute;
			dS = MAX(0,dS);
			DoMovementsInPath(_pEngine,lastState.time, dS, lastState.dSpeed,lastState.dSpeed );			
		}
		return OnMoveOutRoute(_pEngine);
	}
	
	if( bCanChangeLane())
	{
		if( ChangeLane(_pEngine) )
		{
			pVehicle->Continue();
			return;
		}
	}

	if( MoveInLane2(_pEngine) )
	{
		pVehicle->Continue();
		return ;		
	}
	pVehicle->StepTime(_pEngine);	
}

void State_MoveInRoad::ResourceQueueMovement(CARCportEngine* _pEngine, LandsideResourceInSim* pStartRes,LandsideResourceInSim* pEndRes)
{
	PLACE_METHOD_TRACK_STRING();
	//check vehicle whether leave intersection or roundabout queue
	LandsideResourceInSim* pLastCurRes = pEndRes;
	LandsideLayoutObjectInSim* pLayoutObjectInSim = pLastCurRes->getLayoutObject();
	
	LandsideLayoutObjectInSim* pStartObject = NULL;
	if(pStartRes)
	{
		pStartObject = pStartRes->getLayoutObject();
	}
	if(!pStartObject)
		return;
	
	if(pLayoutObjectInSim != pStartObject)
	{
		//current resource queue move out
		if (getVehicle()->WaitInResourceQueue(pStartObject))
		{
			LandsideResourceQueueItem* pQueue = new LandsideResourceQueueItem;
			pQueue->m_nVehicleID = m_pOwner->getID();
			pQueue->m_strVehicleType = m_pOwner->getName().toString();
			pQueue->m_nResourceID = pStartRes->getLayoutObject()->getInput()->getID();
			pQueue->m_strResName = pStartRes->getLayoutObject()->getInput()->getName().GetIDString();
			pQueue->m_eTime = m_pOwner->curTime();
			pQueue->m_enumOperation = LandsideResourceQueueItem::QO_Exit;
			m_pOwner->LeaveResourceQueue(pStartObject);

			_pEngine->GetLandsideSimulation()->getOutput()->m_SimLogs.getResourceQueueLog().AddItem(pQueue);
		}
	}
	
}

void State_MoveInRoad::DoMovementsInPath(CARCportEngine* _pEngine, const ElapsedTime& fromTime , double dS,double dStarSpd, double dEndSpd )
{
	double dT = dS*2.0/(dStarSpd+dEndSpd);
	double dAcc = 0;
	if(dT > 0)
		dAcc = (dEndSpd - dStarSpd)/dT;

	double dFromIndex = m_path.getDistIndex(m_dLastDistInRoute);
	double dToIndex = m_path.getDistIndex(m_dLastDistInRoute+dS);	 

	DynamicMovement dymMove(dStarSpd,dEndSpd, dS);
	for(int iIndex = (int)dFromIndex+1;iIndex < dToIndex;iIndex++)
	{
		double dDist = m_path.getIndexDist(iIndex) -m_dLastDistInRoute;	
		const LandsidePosition& pos = m_path.PositionAt(iIndex);

		MobileState mobState;
		mobState.dAcc = dAcc;
		mobState.distInRes = pos.distInRes;
		mobState.dSpeed = dymMove.GetDistSpeed(dDist);
		mobState.pRes = pos.pRes;
		mobState.time = fromTime + dymMove.GetDistTime(dDist);
		mobState.pos = pos.pos;
		mobState.isConerPt = true;

		MobileState lastState = m_pOwner->getLastState();
		ResourceQueueMovement(_pEngine,lastState.getLandsideRes(),pos.pRes);
		m_pOwner->UpdateState(mobState);

	}

	//double dDist = m_path.getIndexDist(dToIndex)-m_distInPath;	
	LandsidePosition pos = m_path.getIndexPosition(dToIndex);	
	//the last point
	MobileState mobState;
	mobState.dAcc = dAcc;
	mobState.distInRes = pos.distInRes;
	mobState.dSpeed = dEndSpd;
	mobState.pRes = pos.pRes;
	mobState.time = fromTime + dymMove.GetDistTime(dS);
	mobState.isConerPt = (dToIndex==(int)dToIndex);
	mobState.pos = pos.pos;

	MobileState lastState = m_pOwner->getLastState();
	ResourceQueueMovement(_pEngine,lastState.getLandsideRes(),pos.pRes);
	m_pOwner->UpdateState(mobState);
	m_dLastDistInRoute += dS;
}


#define TEST_COUNT 5
//
#define CONCERN_RADIUS 10000 //500 meters

struct StretchChangeLaneSorter
{
public:
	StretchChangeLaneSorter(int nCurLaneIndex, bool bLeftDrive)
	{
		m_nCurLane = nCurLaneIndex;
		m_bLeftDrive = bLeftDrive;
	}
	bool operator()(LandsideStretchLaneInSim * pLane1, LandsideStretchLaneInSim* pLane2)
	{
		bool bLane1Left = pLane1->GetLaneIndex()<m_nCurLane;
		bool bLane2Left = pLane2->GetLaneIndex()<m_nCurLane;
		
		if(bLane1Left!=bLane2Left)
		{
			if(m_bLeftDrive)
			{
				return bLane1Left;
			} 
			else 
				return bLane2Left;
		}
		else
		{
			return abs(pLane1->GetLaneIndex()-m_nCurLane)<abs(pLane2->GetLaneIndex()-m_nCurLane);
		}
	}
	int m_nCurLane;
	bool m_bLeftDrive;
};


bool State_MoveInRoad::ChangeLane( CARCportEngine *_pEngine )
{
	LandsideResourceInSim* pCurRes = m_pOwner->getLastState().getLandsideRes();
	if(!pCurRes)
		return false;

	LandsideLaneInSim* pThisLane = pCurRes->toLane();
	if(!pThisLane)
		return false;

	LandsideStretchLaneInSim*pStretchLane = pThisLane->toStretchLane();
	if(!pStretchLane)
		return false;
	
	LandsideStretchInSim* pStretch = (LandsideStretchInSim*)pStretchLane->getLayoutObject();
	LandsideResourceManager* pRes = _pEngine->GetLandsideSimulation()->GetResourceMannager();
	LandsideLaneNodeInSim* pLastNode = *mRoutePath.rbegin();
	LandsideResourceInSim* pDest = pLastNode->getToRes();
	
	UpdateChangeLanes(pStretchLane,_pEngine->GetLandsideSimulation()->IsLeftDrive());
	//search left 
	for(int i=0;i<(int)m_vChangeLanes.size();i++)
	{
		LandsideStretchLaneInSim* pOtherLane = m_vChangeLanes.at(i);
		if(!pOtherLane->IsSameDirToLane(pThisLane))
			break;

		double dChangeLaneDist = pOtherLane->GetPointDist(m_pOwner->getLastState().pos) + m_pOwner->GetLength();
		if(!pOtherLane->isSpaceEmpty(getVehicle(),dChangeLaneDist-m_pOwner->GetLength(), dChangeLaneDist+m_pOwner->GetLength()))
			break;

		if(dChangeLaneDist > pOtherLane->GetLength() - 1000 )
			break;
		
		LandsideLaneNodeList routepath;
		bool leadtoDest = pRes->mRouteGraph.FindRouteStretchPosToRes(pOtherLane,dChangeLaneDist, pDest, routepath);
		if(!leadtoDest)
			continue;

		double dNewLaneSpeed = m_pOwner->getSpeed(pOtherLane,dChangeLaneDist);

		DistanceUnit distAheadFromLane;
		DistanceUnit distAheadToLane;
		LandsideVehicleInSim* pOldAhead = pThisLane->GetAheadVehicle(m_pOwner,  m_pOwner->getLastState().distInRes,distAheadFromLane);
		LandsideVehicleInSim* pNewAhead = pOtherLane->GetAheadVehicle(m_pOwner, dChangeLaneDist, distAheadToLane);
		LandsideVehicleInSim* pNewBack  = pOtherLane->GetBehindVehicle(m_pOwner,dChangeLaneDist);

		bool bChangeOK = m_pOwner->GetChangeLaneModel().bChangOk(m_pOwner,true,pOldAhead,dChangeLaneDist,pOtherLane->GetLength(), pNewAhead,pNewBack);
		if(!bChangeOK)
			continue;

		//change lane
		DoChangeLane(_pEngine,pOtherLane,dChangeLaneDist,routepath);
		return true;

	}	
	return false;
}



void State_MoveInRoad::UpdateTotalPath()
{
	m_path.Clear();

	for(int i=0;i<(int)mRoutePath.size()-1;i++)
	{
		_addSeg(mRoutePath.GetNode(i),mRoutePath.GetNode(i+1));
	}
	ASSERT(m_path.getPtCount()>0);
}

State_MoveInRoad::State_MoveInRoad( LandsideVehicleInSim* pEnt) 
:State_LandsideVehicle<LandsideVehicleInSim>(pEnt)
{	
	m_pDestResource = NULL;
	m_bChangeLane = false;
	m_bStopAtEnd = true;//init list of acc operations 
	m_dLastDistInRoute = 0;
	m_nChangeLaneCount = 0;

	m_plastStretchLane= NULL;
}



void State_MoveInRoad::RelealseCrossWalk()
{
	LandsideResourceInSim* pRes=  m_pOwner->getLastState().getLandsideRes();
	if(pRes)
	{
		if(LandsideLaneInSim* plane = pRes->toLane())
		{
			if(LandsideStretchLaneInSim* pstretchLane = plane->toStretchLane())
			{
				pstretchLane->ReleaseCrossOcys(m_pOwner,m_pOwner->getLastState().distInRes);			
			}
		}
	}
}

void State_MoveInRoad::LeaveCrosswalkQueue()
{
	LandsideResourceInSim* pRes=  m_pOwner->getLastState().getLandsideRes();
	if(pRes)
	{
		if(LandsideLaneInSim* plane = pRes->toLane())
		{
			if(LandsideStretchLaneInSim* pstretchLane = plane->toStretchLane())
			{
				pstretchLane->LeaveCrosswalkQueue(m_pOwner,m_pOwner->getLastState().distInRes);			
			}
		}
	}
}



namespace {
	LandsideVehicleDelayLog::EnumDelayOperation getLogOp(OpMoveType stateenum)
	{
		if(stateenum==_STOP){ return LandsideVehicleDelayLog::DO_Stop; }
		return LandsideVehicleDelayLog::DO_SlowDown;
	}
	LandsideVehicleDelayLog::EnumDelayReason getDelayReson(ConflictInfo& cfInfo)
	{
		if(cfInfo.nearestConflict == cf_cross){ return LandsideVehicleDelayLog::DR_ForCrossWalk;} 
		if(cfInfo.nearestConflict == cf_intersection){ return LandsideVehicleDelayLog::DR_TrafficLight; }
		if(cfInfo.nearestConflict == cf_conflictVehicle || cfInfo.nearestConflict == cf_followingVehicle){ return LandsideVehicleDelayLog::DR_ForeheadCar; }
		return LandsideVehicleDelayLog::DR_YeildSign;
	}
	
}


void State_MoveInRoad::LeaveResourceQueueConflict(CARCportEngine* _pEngine)
{
	PLACE_METHOD_TRACK_STRING();
	LandsideResourceQueueItem* pQueue = new LandsideResourceQueueItem;
	landsideResourceQueue* pQueueData = m_pOwner->GetLastResourceQueue();
	pQueue->m_nVehicleID = m_pOwner->getID();


	pQueue->m_strVehicleType = m_pOwner->getName().toString();
	if (pQueueData->bResourceQueue())
	{
		LandsideLayoutObjectInSim* pRes = pQueueData->GetLandsideRes();
		pQueue->m_nResourceID = pRes->getInput()->getID();
		pQueue->m_strResName = pRes->getInput()->getName().GetIDString();
		m_pOwner->LeaveResourceQueue(pRes);
	}
	else
	{
		CCrossWalkInSim* pCrosswalk = pQueueData->GetCrosswalk();
		pQueue->m_nResourceID = pCrosswalk->GetCrosswalk()->getID();
		pQueue->m_strResName = pCrosswalk->GetCrosswalk()->getName().GetIDString();
		m_pOwner->LeaveCrosswalkQueue(pQueueData->GetCrosswalk());
	}

	pQueue->m_eTime = m_pOwner->curTime();
	pQueue->m_enumOperation = LandsideResourceQueueItem::QO_Exit;

	_pEngine->GetLandsideSimulation()->getOutput()->m_SimLogs.getResourceQueueLog().AddItem(pQueue);
}

void State_MoveInRoad::ReportConflictEnd(CARCportEngine* _pEngine, ConflictInfo& conflictInfo)
{
	//last state has conflict and current state doesn't have conflict. vehicle should leave resource queue
	if(m_pOwner->GetWaitResourceQueueFlag())
	{
		landsideResourceQueue* pQueueData = m_pOwner->GetLastResourceQueue();
		LandsideResourceInSim* pRes = m_pOwner->getLastState().getLandsideRes();

		//if vehicle is still on the waiting resource, vehicle doesn't leave resource queue 
		if (pQueueData->GetLandsideRes() && pRes)
		{
			if (pQueueData->GetLandsideRes() == pRes->getLayoutObject())
				return;
		}
		
		if (m_pDestResource)
		{
			if(m_pOwner->WaitInResourceQueue(m_pDestResource->getLayoutObject()))
				return;
		}

		if (!conflictInfo.isConflict())
		{
			double dFromIndex = m_path.getDistIndex(m_dLastDistInRoute);
			int iNextIndex = (int)dFromIndex;
			if (pQueueData->GetLandsideRes())
			{
				if (m_path.ExistResourcePoint(iNextIndex,pQueueData->GetLandsideRes()->IsResource()) == false)
				{
					LeaveResourceQueueConflict(_pEngine);
				}
			}
			else //cross walk queue leave
			{
				LeaveResourceQueueConflict(_pEngine);
			}
		}
		else
		{
			if (conflictInfo.pConflictIntersect)
			{
				LandsideResourceInSim* pRes = conflictInfo.pConflictIntersect->getIntersection()->IsResource();
				LandsideLayoutObjectInSim* pObject = pRes->getLayoutObject();
				if(!m_pOwner->WaitInResourceQueue(pObject))
				{
					LeaveResourceQueueConflict(_pEngine);
				}
			}
			else if (conflictInfo.pConflictCross)
			{
				CCrossWalkInSim* pCrosswalk = conflictInfo.pConflictCross;
				if (!m_pOwner->WaitInCrosswalkQueue(pCrosswalk))
				{
					LeaveResourceQueueConflict(_pEngine);
				}
			}
			else if (conflictInfo.pConflictVehicle)
			{
				if (conflictInfo.pConflictVehicle->GetWaitResourceQueueFlag())
				{
					landsideResourceQueue* pQueueData = conflictInfo.pConflictVehicle->GetLastResourceQueue();
					if (pQueueData->bResourceQueue())
					{
						LandsideLayoutObjectInSim* pObject = pQueueData->GetLandsideRes();
						if (!m_pOwner->WaitInResourceQueue(pObject))
						{
							LeaveResourceQueueConflict(_pEngine);
						}
					}
					else
					{
						if (!m_pOwner->WaitInCrosswalkQueue(pQueueData->GetCrosswalk()))
						{
							LeaveResourceQueueConflict(_pEngine);
						}
					}
				}
				else
				{
					double dFromIndex = m_path.getDistIndex(m_dLastDistInRoute);
					int iNextIndex = (int)dFromIndex;
					if (pQueueData->GetLandsideRes())
					{
						if (m_path.ExistResourcePoint(iNextIndex,pQueueData->GetLandsideRes()->IsResource()) == false)
						{
							LeaveResourceQueueConflict(_pEngine);
						}
					}
				}
				
			}
			else if (conflictInfo.pFollowingVehicle)
			{
				if (conflictInfo.pFollowingVehicle->GetWaitResourceQueueFlag())
				{
					landsideResourceQueue* pQueueData = conflictInfo.pFollowingVehicle->GetLastResourceQueue();
					if (pQueueData->bResourceQueue())
					{
						LandsideLayoutObjectInSim* pObject = pQueueData->GetLandsideRes();
						if (!m_pOwner->WaitInResourceQueue(pObject))
						{
							LeaveResourceQueueConflict(_pEngine);
						}
					}
					else
					{
						if (!m_pOwner->WaitInCrosswalkQueue(pQueueData->GetCrosswalk()))
						{
							LeaveResourceQueueConflict(_pEngine);
						}
					}
				}
				else
				{
					double dFromIndex = m_path.getDistIndex(m_dLastDistInRoute);
					int iNextIndex = (int)dFromIndex;
					if (pQueueData->GetLandsideRes())
					{
						if (m_path.ExistResourcePoint(iNextIndex,pQueueData->GetLandsideRes()->IsResource()) == false)
						{
							LeaveResourceQueueConflict(_pEngine);
						}
					}
				}
			}
		}
		
	}
}
void State_MoveInRoad::ReportResourceQueue(CARCportEngine* _pEngine, ConflictInfo& conflictInfo)
{
	PLACE_METHOD_TRACK_STRING();
	if (m_pOwner->GetWaitResourceQueueFlag())
		return;
	

	//vehicle enter resource queue
	if ((conflictInfo.pConflictVehicle && conflictInfo.nearestConflict == cf_conflictVehicle) 
		|| (conflictInfo.pFollowingVehicle && conflictInfo.nearestConflict == cf_followingVehicle))
	{
		LandsideVehicleInSim* pConflictVehicle = NULL;
		if (conflictInfo.nearestConflict == cf_followingVehicle)
		{
			pConflictVehicle = conflictInfo.pFollowingVehicle;
		}
		else
		{
			pConflictVehicle = conflictInfo.pConflictVehicle;
		}
		
		if (pConflictVehicle == NULL)
			return;
		
		if (pConflictVehicle->GetWaitResourceQueueFlag())
		{
			LandsideResourceQueueItem* pQueue = new LandsideResourceQueueItem;
			landsideResourceQueue* pQueueData = pConflictVehicle->GetLastResourceQueue();
			pQueue->m_nVehicleID = m_pOwner->getID();
			pQueue->m_strVehicleType = m_pOwner->getName().toString();
			
			if (pQueueData->bResourceQueue())
			{
				LandsideLayoutObjectInSim* pRes = pQueueData->GetLandsideRes();
				pQueue->m_nResourceID =pRes->getInput()->getID();
				pQueue->m_strResName = pRes->getInput()->getName().GetIDString();
				m_pOwner->EnterResourceQueue(pRes);
			}
			else
			{
				pQueue->m_nResourceID = pQueueData->GetCrosswalk()->GetCrosswalk()->getID();
				pQueue->m_strResName = pQueueData->GetCrosswalk()->GetCrosswalk()->getName().GetIDString();
				m_pOwner->EnterCrosswalkQueue(pQueueData->GetCrosswalk());
			}
		
			pQueue->m_eTime = m_pOwner->curTime();
			pQueue->m_enumOperation = LandsideResourceQueueItem::QO_Entry;

			_pEngine->GetLandsideSimulation()->getOutput()->m_SimLogs.getResourceQueueLog().AddItem(pQueue);
		}
		else
		{
			//check conflict vehicle current resource whether is intersection node or roundabout
			LandsideResourceInSim* pConflictRes = pConflictVehicle->getLastState().getLandsideRes();
			LandsideResourceInSim* pLastRes = m_pOwner->getLastState().getLandsideRes();

			if (pConflictRes)
			{
				if (pConflictRes->toIntersection() || pConflictRes->toAbout())
				{
					LandsideResourceQueueItem* pQueue = new LandsideResourceQueueItem;
					pQueue->m_nVehicleID = m_pOwner->getID();
					pQueue->m_strVehicleType = m_pOwner->getName().toString();
					pQueue->m_nResourceID = pConflictRes->getLayoutObject()->getInput()->getID();
					pQueue->m_strResName = pConflictRes->getLayoutObject()->getInput()->getName().GetIDString();
					pQueue->m_eTime = m_pOwner->curTime();
					pQueue->m_enumOperation = LandsideResourceQueueItem::QO_Entry;
					m_pOwner->EnterResourceQueue(pConflictRes->getLayoutObject());

					_pEngine->GetLandsideSimulation()->getOutput()->m_SimLogs.getResourceQueueLog().AddItem(pQueue);
				}
			}
			else if (pLastRes)
			{
				if (pLastRes->toIntersection() || pLastRes->toAbout())
				{
					LandsideResourceQueueItem* pQueue = new LandsideResourceQueueItem;
					pQueue->m_nVehicleID = m_pOwner->getID();
					pQueue->m_strVehicleType = m_pOwner->getName().toString();
					pQueue->m_nResourceID = pLastRes->getLayoutObject()->getInput()->getID();
					pQueue->m_strResName = pLastRes->getLayoutObject()->getInput()->getName().GetIDString();
					pQueue->m_eTime = m_pOwner->curTime();
					pQueue->m_enumOperation = LandsideResourceQueueItem::QO_Entry;
					m_pOwner->EnterResourceQueue(pLastRes->getLayoutObject());

					_pEngine->GetLandsideSimulation()->getOutput()->m_SimLogs.getResourceQueueLog().AddItem(pQueue);
				}
			}
		}
	}
	else if (conflictInfo.pConflictIntersect && conflictInfo.nearestConflict == cf_intersection)
	{
		LandsideResourceInSim* pRes = conflictInfo.pConflictIntersect->getIntersection()->IsResource();
		
		LandsideResourceQueueItem* pQueue = new LandsideResourceQueueItem;
		pQueue->m_nVehicleID = m_pOwner->getID();
		pQueue->m_strVehicleType = m_pOwner->getName().toString();
		pQueue->m_nResourceID = pRes->getLayoutObject()->getInput()->getID();
		pQueue->m_strResName = pRes->getLayoutObject()->getInput()->getName().GetIDString();
		pQueue->m_eTime = m_pOwner->curTime();
		pQueue->m_enumOperation = LandsideResourceQueueItem::QO_Entry;
		m_pOwner->EnterResourceQueue(pRes->getLayoutObject());

		_pEngine->GetLandsideSimulation()->getOutput()->m_SimLogs.getResourceQueueLog().AddItem(pQueue);
	}
	else if (conflictInfo.pConflictCross && conflictInfo.nearestConflict == cf_cross)
	{
		LandsideResourceQueueItem* pQueue = new LandsideResourceQueueItem;
		pQueue->m_nVehicleID = m_pOwner->getID();
		pQueue->m_strVehicleType = m_pOwner->getName().toString();
		pQueue->m_nResourceID = conflictInfo.pConflictCross->GetCrosswalk()->getID();
		pQueue->m_strResName = conflictInfo.pConflictCross->GetCrosswalk()->getName().GetIDString();
		pQueue->m_eTime = m_pOwner->curTime();
		pQueue->m_enumOperation = LandsideResourceQueueItem::QO_Entry;


		m_pOwner->EnterCrosswalkQueue(conflictInfo.pConflictCross);
		_pEngine->GetLandsideSimulation()->getOutput()->m_SimLogs.getResourceQueueLog().AddItem(pQueue);
	}
}




void State_MoveInRoad::_addSeg( LandsideLaneNodeInSim* pNodePre ,LandsideLaneNodeInSim* pNodeNext )
{

	LandsideResourceInSim* pResource = NULL;
	const CPath2008* pPath = NULL;
	CPath2008 subPath;
	DistanceUnit distFrom = 0;

	//get resource and next path	
	if( pNodePre && pNodeNext )
	{
		pResource = pNodePre->getToRes();
		if(LandsideLaneInSim* pLane = pResource->toLane()) // in lane
		{		
			subPath = pLane->GetSubPathDist(pNodePre->m_distInlane, pNodeNext->m_distInlane);
			distFrom = pNodePre->m_distInlane;
			pPath = &subPath;			
		}		
		else if(LandsideIntersectionInSim* pIntersect = pResource->toIntersection() ) //intersection node
		{
			LandsideLaneExit* pExit = pNodePre->toExit();
			LandsideLaneEntry* pEntry = pNodeNext->toEntry();
			if(pEntry && pExit)
			{
				LandsideIntersectLaneLinkInSim* pLink = pIntersect->GetLinkage(pExit,pEntry);
				if(pLink)
				{
					const CPath2008& respath = pLink->m_path;				
					pPath = &respath;	
					pResource = pLink;
				}
			}
		}		
		else if(LandsideCurbSideInSim* pCurb  = pResource->toCurbSide())
		{
			ASSERT(pNodePre->mpLane == pNodeNext->mpLane);
			//const ARCPath3& respath = pNodePre->mpLane->getPath();

			subPath = pLane->GetSubPathDist( pNodePre->m_distInlane, pNodeNext->m_distInlane );
			pPath = &subPath;
		}
		else if(LandsideRoundaboutInSim* pAbout = pResource->toAbout() )
		{
			if( pAbout->GetLinkPath(pNodePre,pNodeNext,subPath) )
			{
				pPath = &subPath;
			}
			else
				ASSERT(FALSE);
		}
		else
		{
			subPath.push_back(pNodePre->m_pos);
			subPath.push_back(pNodeNext->m_pos);				
			pPath = &subPath;
			//ASSERT(FALSE);
		}
	}
	else if(pNodePre)
	{
		pResource = pNodePre->getToRes();
		if(LandsideCurbSideInSim* pCurb = pResource->toCurbSide())
		{
			subPath = pNodePre->mpLane->GetPathLeft(pNodePre->m_distInlane);
			pPath = &subPath;
		}
	}
	ASSERT(pPath);

	if(pPath)
	{
		m_path.addPath(pResource,pPath,distFrom);
	}
}

bool State_MoveInRoad::_isOutRoute()
{
	return m_dLastDistInRoute > ( m_path.getAllLength() -m_pOwner->GetLength()*0.5);
	return false;
}

void State_MoveInRoad::DoChangeLane(CARCportEngine* _pEngine, LandsideLaneInSim* pToLane , DistanceUnit dChangeLaneDist, const LandsideLaneNodeList& routepath)
{
	ReportEndDelay(_pEngine,m_pOwner->curTime());
	//do change lane need smooth path
	MobileState nextstat = m_pOwner->getLastState();
	nextstat.pRes = pToLane;
	nextstat.distInRes = dChangeLaneDist;
	nextstat.pos = pToLane->GetDistPoint(dChangeLaneDist);
	nextstat.dSpeed = m_pOwner->getSpeed(pToLane,dChangeLaneDist);

	double dSpeed = (nextstat.dSpeed + m_pOwner->getLastState().dSpeed)*0.5;
	
	DistanceUnit dDist = m_pOwner->getLastState().pos.distance3D(nextstat.pos);
	nextstat.time += ElapsedTime(dDist/dSpeed);
	m_pOwner->UpdateState(nextstat);

	//re find the path
	LandsideLaneEntry* pEntry= pToLane->GetPreLaneEntry(dChangeLaneDist);
	mRoutePath.clear();
	mRoutePath.push_back(pEntry);
	mRoutePath.insert(mRoutePath.end(),routepath.begin(),routepath.end());
	UpdateTotalPath();
	m_dLastDistInRoute = dChangeLaneDist - pEntry->m_distInlane;

	m_nChangeLaneCount++;
	m_tLastChangeLane = m_pOwner->curTime();
}


bool State_MoveInRoad::MoveInLane2( CARCportEngine *_pEngine )
{
	m_pOwner->WaitForVehicle(NULL);

	LandsidePosition nextPos = m_path.getDistPosition(m_dLastDistInRoute+100);
	double dMaxSpd = m_pOwner->getSpeed(nextPos.pRes,nextPos.distInRes);	
	double dMaxAcc = m_pOwner->mPerform.mAccSpd;

	double dCurSpeed = m_pOwner->getLastState().dSpeed;
	double dt = (double)LandsideSimulation::tAwarenessTime; //t in seconds

	DistanceUnit distRad = getDecDist(dCurSpeed) + 5000;
	DistanceUnit sepToEnd = m_path.getAllLength() - m_pOwner->GetLength()*0.5 - m_dLastDistInRoute;

	ConflictInfo conflictInfo;
	_getConflictInfo(distRad, conflictInfo);

	MoveOperationTable& opTable = m_pOwner->m_opTable;

	double dEndSpd = 0;
	double dOffsetS = 0;
	OpMoveType opType = _STOP;
	for(int iopType = _MAXBREAK; iopType<_OpMoveTypeCount; iopType++)
	{		
		MoveOperationTable& opTable = m_pOwner->m_opTable;
		double opAcc=  opTable[(OpMoveType)iopType];

		double _dEndSpd = dCurSpeed + opAcc * dt;
		_dEndSpd = MAX(0,_dEndSpd);
		_dEndSpd = MIN(dMaxSpd,_dEndSpd);


		DistanceUnit _dOffsetS = 0;
		if(opAcc!=0 && dCurSpeed!=_dEndSpd)
			_dOffsetS = (_dEndSpd*_dEndSpd-dCurSpeed*dCurSpeed)*0.5/opAcc;
		else 
			_dOffsetS = (_dEndSpd+dCurSpeed)*0.5*dt;

		DistanceUnit decDist  = getDecDist(_dEndSpd);
		if(_dOffsetS + decDist > conflictInfo.sepToConflict )
			break;

		dEndSpd = _dEndSpd;
		dOffsetS = _dOffsetS;
		opType = (OpMoveType)iopType;

		//max speed 
		if(dEndSpd>=dMaxSpd)
			break;
	}
	//
	if(conflictInfo.pConflictVehicle)
	{
		m_pOwner->WaitForVehicle(conflictInfo.pConflictVehicle);
	}
	//
	
	if(opType<_UNIFORM)
	{
		DelayInfo delay;
		delay.conflictInfo = conflictInfo;
		delay.distStartDelay = m_dLastDistInRoute;
		delay.dSpeedNeed = dMaxSpd;
		delay.timeStartDelay = m_pOwner->curTime();
		delay.delayRes = m_pOwner->getLastState().getLandsideRes();
		delay.operation = opType;
		ReportDelay(_pEngine,delay);
		ReportResourceQueue(_pEngine,conflictInfo);
	}
//	else
	{
		ReportConflictEnd(_pEngine,conflictInfo);
	}


	MobileState curState = m_pOwner->getLastState();
	curState.time = m_pOwner->curTime();
	if(dOffsetS >0)
	{
		m_pOwner->StartMove(m_pOwner->curTime());
		LeaveCrosswalkQueue();
		RelealseCrossWalk();
		DoMovementsInPath(_pEngine,curState.time, dOffsetS, curState.dSpeed,dEndSpd);	
		//Update conflict resolve Path
		UpdateConflictResolvePath();
	}
	else 
	{
		return false;
	}
	
	if(dEndSpd>=dMaxSpd)
	{
		ReportEndDelay(_pEngine, m_pOwner->getLastState().time);
	}
	return true;
}

DistanceUnit State_MoveInRoad::getDecDist( double dSpeed )
{
	double dendDecDist = m_pOwner->mPerform.minDecDist(dSpeed);
	dendDecDist += m_pOwner->getIDModel().gap;
	dendDecDist += m_pOwner->getIDModel().T * dSpeed;
	return dendDecDist;

}

void State_MoveInRoad::_getConflictInfo(DistanceUnit distRad ,ConflictInfo& conflictInfo)
{
	LandsideResourceInSim* pCurRes = m_pOwner->getLastState().getLandsideRes();
	LandsideResourceInSim* pItrRes = NULL;
	int fIndex = (int)m_path.getDistIndex(m_dLastDistInRoute);
	

	for(int iIndex = fIndex; iIndex < m_path.getPtCount();iIndex++ )
	{
		if(m_path.getIndexDist(iIndex) - m_dLastDistInRoute > distRad )
			break;

		const LandsidePosition& landsidepos =  m_path.PositionAt(iIndex);
		LandsideResourceInSim* pthisRes = landsidepos.pRes;	
		if(pthisRes == pItrRes)continue;
		pItrRes = pthisRes;
		if(!pthisRes){ ASSERT(FALSE); continue; }

		//break if dist too far			
		DistanceUnit distInRes = landsidepos.distInRes;
		if(pthisRes == pCurRes)
			distInRes = m_pOwner->getLastState().distInRes;		

		//get conflict in resource
		DistanceUnit dist;
		LandsideVehicleInSim* pConflictV = pthisRes->CheckPathConflict(m_pOwner,m_pOwner->getSpanPath(),dist);
		if(pConflictV)
		{
			DistanceUnit sep = dist;
			if( sep < conflictInfo.sepToConflict )
			{
				conflictInfo.nearestConflict = cf_conflictVehicle;
				conflictInfo.sepToConflict = sep;
				conflictInfo.pConflictVehicle = pConflictV;
			}			
		}
		//get following vehicle in resource
		DistanceUnit distAhead;
		LandsideVehicleInSim* pHeadV = pthisRes->GetAheadVehicle(m_pOwner,distInRes,distAhead);
		if(pHeadV)
		{
			DistanceUnit distHeadInPath = distAhead - landsidepos.distInRes + m_path.getIndexDist(iIndex);
			DistanceUnit sep = distHeadInPath - m_dLastDistInRoute - m_pOwner->GetHalfLength() - pHeadV->GetHalfLength();
			if( sep < conflictInfo.sepToConflict )
			{
				conflictInfo.nearestConflict = cf_followingVehicle;
				conflictInfo.sepToConflict = sep;
				conflictInfo.pFollowingVehicle = pHeadV;
			}	
		}
		//check intersection
		if(LandsideIntersectLaneLinkInSim* pLink = pthisRes->toIntersectionLink() )
		{
			if(pLink!=pCurRes && pLink->IsClosed())
			{
				DistanceUnit sep = m_path.getIndexDist(iIndex) - m_dLastDistInRoute - m_pOwner->GetHalfLength();
				if(sep < conflictInfo.sepToConflict )
				{
					conflictInfo.nearestConflict = cf_intersection;
					conflictInfo.sepToConflict = sep;
					conflictInfo.pConflictIntersect = pLink->getGroup();
				}				
			}
		}
		//check cross walk
		if(LandsideLaneInSim* plane = pthisRes->toLane() )
		{
			if( LandsideStretchLaneInSim* pStretchLane = plane->toStretchLane() )
			{
				DistanceUnit dCrossDist;
				CCrossWalkInSim*pCross = pStretchLane->GetAheadCross(m_pOwner, distInRes+m_pOwner->GetHalfLength(),dCrossDist);
				if(pCross)
				{
					DistanceUnit dCrossDistInPath = m_path.getIndexDist(iIndex) + dCrossDist- m_path.PositionAt(iIndex).distInRes;
					DistanceUnit sep = dCrossDistInPath - m_dLastDistInRoute - pCross->getHalfWidth() - m_pOwner->GetHalfLength();

					bool bCanCross = pCross->CanVehicleCross(m_pOwner->curTime());
					if(bCanCross)
					{
						DistanceUnit distAheadvehicle;
						if( LandsideVehicleInSim* pAheadV = pStretchLane->GetAheadVehicle(m_pOwner,distInRes, distAheadvehicle) )
						{
							DistanceUnit distFree = distAheadvehicle - dCrossDist - pAheadV->GetHalfLength() - pCross->getHalfWidth();
							if(distFree < m_pOwner->GetLength() )							
							{
								bCanCross = false;
							}
						}
					}
					if(!bCanCross)
					{
						if(sep < conflictInfo.sepToConflict )
						{
							conflictInfo.nearestConflict = cf_cross;
							conflictInfo.sepToConflict = sep;
							conflictInfo.pConflictCross = pCross;
						}	
					}									
				}					
			}
		}//end check cross walk
		
	}
	return ;
}


void State_MoveInRoad::ReportDelay( CARCportEngine* _pEngine,const DelayInfo& delay )
{
	if(!delay.IsDelay())
		return;

	if(delay.IsSameDelay(m_theDelayInfo))
	{
		return;
	}
	ReportEndDelay(_pEngine, delay.timeStartDelay);
	m_theDelayInfo = delay;
}

void State_MoveInRoad::ReportEndDelay( CARCportEngine* _pEngine, const ElapsedTime& t )
{
	if(m_theDelayInfo.IsDelay())
	{
		DistanceUnit distInDelay = m_dLastDistInRoute - m_theDelayInfo.distStartDelay;
		ElapsedTime delayTime = t - m_theDelayInfo.timeStartDelay - ElapsedTime(distInDelay/m_theDelayInfo.dSpeedNeed);
		if(delayTime<=ElapsedTime(0L))
		{
			m_theDelayInfo = DelayInfo();
			return;
		}

		LandsideResourceInSim* pRes = m_theDelayInfo.delayRes;
		if (pRes == NULL)
		{
			m_theDelayInfo = DelayInfo();
			return;
		}
		
		LandsideVehicleDelayItem* pNewItem = new LandsideVehicleDelayItem;
		pNewItem->m_nVehicleID = m_pOwner->getID();
		pNewItem->m_nResourceID = pRes->getLayoutObject()->getID();
		pNewItem->m_strResName = /*pRes->print()*/pRes->getLayoutObject()->getInput()->getName().GetIDString();

		pNewItem->m_eStartTime = m_theDelayInfo.timeStartDelay;
		pNewItem->m_eEndTime = t;
		pNewItem->m_enumOperation = getLogOp(m_theDelayInfo.operation);
		pNewItem->m_enumReason = getDelayReson(m_theDelayInfo.conflictInfo);
		pNewItem->m_nVehicleType = m_pOwner->getType();
		pNewItem->m_eDelayTime = delayTime;
		pNewItem->m_strDesc = "";

		_pEngine->GetLandsideSimulation()->getOutput()->m_SimLogs.getDelayLog().AddItem(pNewItem);
		m_theDelayInfo = DelayInfo();
	}
}

void State_MoveInRoad::SetPath( const LandsideLaneNodeList& routePath )
{
	mRoutePath = routePath; UpdateTotalPath();
}

bool State_MoveInRoad::bCanChangeLane() const
{
	if(!m_bChangeLane)
		return false;
	if(m_nChangeLaneCount>0)
	{
		if(m_pOwner->curTime() - m_tLastChangeLane < ElapsedTime(10*60L) )
			return false;
	}
	return true;
}

void State_MoveInRoad::UpdateConflictResolvePath()
{
	DistanceUnit distSpan =  getDecDist(m_pOwner->getLastState().dSpeed);
	m_path.getPath(m_dLastDistInRoute, m_dLastDistInRoute + distSpan + m_pOwner->GetHalfLength(),  m_pOwner->getSpanPath() );
}

void State_MoveInRoad::UpdateChangeLanes( LandsideStretchLaneInSim* pthislane ,bool bLeftDrive)
{
	if(m_plastStretchLane == pthislane)
		return;
	m_plastStretchLane = pthislane;

	m_vChangeLanes.clear();

	LandsideStretchInSim* pStretch = (LandsideStretchInSim*)pthislane->getLayoutObject();
	for(int i=0;i<pStretch->GetLaneCount();i++)
	{
		LandsideStretchLaneInSim* pLane = pStretch->GetLane(i);
		if(pLane!=pthislane)
		{
			if(!pLane->HasParkingSpot())
			{
				m_vChangeLanes.push_back(pLane);
			}
		}
	}
	std::sort( m_vChangeLanes.begin(),m_vChangeLanes.end(),StretchChangeLaneSorter(pthislane->GetLaneIndex(),bLeftDrive) );
}


//////////////////////////////////////////////////////////////////////////
void State_MoveToDest::Entry( CARCportEngine* pEngine )
{
	bool bSameDest  = m_pOwner->bToSameDest();

	LandsideResourceManager* pResMan = pEngine->GetLandsideSimulation()->GetResourceMannager();
	LandsideResourceInSim* pOrign = m_pOwner->getLastState().getLandsideRes();
	if(LandsideParkingLotInSim* pLot = m_pDestObject->toParkLot())
	{
		LandsideParkingLotDoorInSim* pDoor = pLot->getNearestEntryDoor(pResMan,m_pOwner);
		if(!pDoor)
		{
			CString sError;
			sError.Format(_T("Can not Find a Entry Door of  %s"),pLot->getName().GetIDString().GetString() );
			m_pOwner->ShowError(sError,"Simulation Error");
			m_pOwner->Terminate(pEngine);
			return;
		}
		m_pDestResource = pDoor;	
	}
	else
	{
		m_pDestResource = m_pDestObject->IsResource();
	}

	DistanceUnit distInRes = m_pOwner->getLastState().distInRes;
	
	if(pOrign==NULL||m_pDestResource==NULL)
	{
		ASSERT(FALSE);
		m_pOwner->Terminate(pEngine);
		return;
	}

	LandsideLaneInSim* pAtLane = pOrign->toLane();
	if(LandsideIntersectLaneLinkInSim* pLink = pOrign->toIntersectionLink())
	{
		pAtLane = pLink->getToNode()->mpLane;
		distInRes = pLink->getToNode()->m_distInlane;
	}
	if(pAtLane)				
	{		
		bool btrue =  pResMan->mRouteGraph.FindRouteStretchPosToRes(pAtLane, bSameDest?pAtLane->GetLength():distInRes,m_pDestResource,mRoutePath);
		//ASSERT(btrue);
		if(!btrue)
		{
			CString str = _T("Can not find Route to Dest");
			if(pOrign && m_pDestResource){
				str.Format("Can not find Route From %s to %s", pOrign->print().GetString(),m_pDestResource->print().GetString());
			}		
			m_pOwner->ShowError(str, "Simulation Error");
			return m_pOwner->Terminate(pEngine);
		}

		m_pLaneEntry = new LandsideLaneEntry();
		m_pLaneEntry->SetFromRes(pAtLane);
		m_pLaneEntry->SetPosition(pAtLane,m_pOwner->getLastState().pos);
		mRoutePath.insert(mRoutePath.begin(),m_pLaneEntry);
	}
	else
	{
		bool btrue =  pResMan->mRouteGraph.FindRouteResToRes(pOrign,m_pDestResource,mRoutePath);
		//ASSERT(btrue);
		if(!btrue)
		{
			CString str = _T("Can not find Route  to Dest");
			if(pOrign && m_pDestResource){
				str.Format("Can not find Route From %s to %s", pOrign->print().GetString(),m_pDestResource->print().GetString());
			}		
			m_pOwner->ShowError(str, "Simulation Error");
			return m_pOwner->Terminate(pEngine);
		}
	}

	UpdateTotalPath();
	Execute(pEngine);
}

void State_MoveToDest::OnMoveOutRoute( CARCportEngine* _pEngine )
{
	return m_pOwner->ArrivalLayoutObject(m_pDestObject,m_pDestResource,_pEngine);
}

State_MoveToDest::State_MoveToDest( LandsideVehicleInSim* pEnt,LandsideLayoutObjectInSim* pDest )
:State_MoveInRoad(pEnt)
,m_pDestObject(pDest)
{
	if(pDest->toExtNode())
		m_bStopAtEnd= false;
	m_pLaneEntry = NULL;
	m_bChangeLane = true;
}

//////////////////////////////////////////////////////////////////////////

bool ConflictInfo::isSameConflict( const ConflictInfo& cf ) const
{
	if(nearestConflict==cf.nearestConflict)
	{
		switch(nearestConflict)
		{
		case cf_cross:
			return pConflictCross == cf.pConflictCross;
		case cf_followingVehicle :
			return pFollowingVehicle == cf.pFollowingVehicle;
		case cf_conflictVehicle :
			return pConflictVehicle == cf.pConflictVehicle;
		case cf_intersection : 
			return pConflictIntersect == cf.pConflictIntersect;
		case cf_pax:
			return pConflictPax == cf.pConflictPax;
		}
	}
	return false;
}

ConflictInfo::ConflictInfo()
{
	pFollowingVehicle = NULL;
	pConflictVehicle = NULL;
	pConflictCross = NULL; 
	pConflictIntersect=NULL;
	pConflictPax = NULL; 
	nearestConflict = cf_none;
	sepToConflict = ARCMath::DISTANCE_INFINITE;
}

bool DelayInfo::IsSameDelay( const DelayInfo& delay ) const
{
	if(IsDelay()!=delay.IsDelay())
		return false;
	if(!IsDelay())
	{
		return true;
	}
	return conflictInfo.isSameConflict(delay.conflictInfo) && dSpeedNeed == delay.dSpeedNeed;
}
