#include "StdAfx.h"
#include ".\Goal_MoveInRoad.h"
#include "..\LandsideResourceManager.h"
#include "..\ARCportEngine.h"
#include "..\LandsideSimulation.h"
#include "..\CrossWalkInSim.h"
#include "../LandsideIntersectionInSim.h"
#include "../LandsideExternalNodeInSim.h"
#include "../LandsideRoundaboutInSim.h"
#include "Goal_MoveInRoad2.h"
#include "Common/DynamicMovement.h"
#include "Common/ARCTracker.h"


void Goal_MoveInRoad::Process( CARCportEngine* _pEngine )
{	
	
	ActivateIfInactive(_pEngine);
	//
	LandsideVehicleInSim* pVehicle = m_pOwner;
	pVehicle->removeBlock(&m_blockInRoad, pVehicle->curTime() );

	if(_isOutRoute())
	{
		m_iStatus = gs_completed;
		MobileState lastState = m_pOwner->getLastState();
		if(m_bStopAtEnd )
		{
			DistanceUnit dS = m_path.getAllLength() - m_pOwner->GetLength()*0.5 - m_dLastDistInRoute;
			dS = MAX(0,dS);
			
			DoMovementsInPath(_pEngine,lastState.time, dS, lastState.dSpeed,0 );	
		}
		else
		{
			DistanceUnit dS = m_path.getAllLength()-m_dLastDistInRoute;
			dS = MAX(0,dS);
			DoMovementsInPath(_pEngine,lastState.time, dS, lastState.dSpeed,lastState.dSpeed );			
		}
		return ;
	}
	
	if(bCanChangeLane() && ChangeLane(_pEngine))
		return;

	if( MoveInLane2(_pEngine) )
		return ;
	//
	////try overpass if can not move in lane
	/*if(m_lastStateInfo.pConflictVehicle)
	{
		OverPass(m_lastStateInfo.pConflictVehicle,_pEngine);
	}*/
	
}
void Goal_MoveInRoad::ResourceQueueMovement(CARCportEngine* _pEngine, LandsideResourceInSim* pStartRes,LandsideResourceInSim* pEndRes)
{
	PLACE_METHOD_TRACK_STRING();
	//check vehicle whether leave intersection or roundabout queue
	LandsideResourceInSim* pLastCurRes = pEndRes;
	LandsideLayoutObjectInSim* pLayoutObjectInSim = pLastCurRes->getLayoutObject();
	
	if(pLayoutObjectInSim != pStartRes->getLayoutObject())
	{
		//other resource move in current resource queue
		//if (m_pOwner->WaitInResourceQueue(pLayoutObjectInSim))
		//{
		//	LandsideResourceQueueItem* pQueue = new LandsideResourceQueueItem;
		//	pQueue->m_nVehicleID = m_pOwner->getID();
		//	pQueue->m_strVehicleType = m_pOwner->getName().toString();
		//	pQueue->m_nResourceID = pLayoutObjectInSim->getInput()->getID();
		//	pQueue->m_strResName = pLastCurRes->getLayoutObject()->getInput()->getName().GetIDString();
		//	pQueue->m_eTime = m_pOwner->curTime();
		//	pQueue->m_enumOperation = LandsideResourceQueueItem::QO_Exit;
		//	m_pOwner->LeaveResourceQueue(pLayoutObjectInSim);

		//	_pEngine->GetLandsideSimulation()->getOutput()->m_SimLogs.getResourceQueueLog().AddItem(pQueue);
		//}

		//current resource queue move out
		if (m_pOwner->WaitInResourceQueue(pStartRes->getLayoutObject()))
		{
			LandsideResourceQueueItem* pQueue = new LandsideResourceQueueItem;
			pQueue->m_nVehicleID = m_pOwner->getID();
			pQueue->m_strVehicleType = m_pOwner->getName().toString();
			pQueue->m_nResourceID = pStartRes->getLayoutObject()->getInput()->getID();
			pQueue->m_strResName = pStartRes->getLayoutObject()->getInput()->getName().GetIDString();
			pQueue->m_eTime = m_pOwner->curTime();
			pQueue->m_enumOperation = LandsideResourceQueueItem::QO_Exit;
			m_pOwner->LeaveResourceQueue(pStartRes->getLayoutObject());

			_pEngine->GetLandsideSimulation()->getOutput()->m_SimLogs.getResourceQueueLog().AddItem(pQueue);
		}
	}
	
}

void Goal_MoveInRoad::DoMovementsInPath(CARCportEngine* _pEngine, const ElapsedTime& fromTime , double dS,double dStarSpd, double dEndSpd )
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
		mobState.dSpeed = dymMove.GetDistSpeed(dDist);
		mobState.distInRes = pos.distInRes;
		mobState.pRes = pos.pRes;
		mobState.pos = pos.pos;
		mobState.time = fromTime + dymMove.GetDistTime(dDist);
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

//bool Goal_MoveInRoad::MoveInLane( CARCportEngine *_pEngine )
//{
//	m_pOwner->WaitForVehicle(NULL);
//
//	LandsideVehicleInSim* mpVehicle = m_pOwner;
//
//	MobileState curState = mpVehicle->getLastState();
//	curState.time = mpVehicle->curTime();
//
//	double halfLen = mpVehicle->GetLength()*0.5;
//
//	LandsidePosition nextPos = m_path.getDistPosition(m_dLastDistInRoute+100);
//	double dMaxSpd = mpVehicle->getSpeed(nextPos.pRes,nextPos.distInRes);
//	
//	
//
//	double dAccList[TEST_COUNT];
//	OpMoveType dMoveStates[5];
//	dAccList[0] = -MAX_BREAK_ACC;   	dMoveStates[0] = _MAXBREAK;//max brake
//	dAccList[1] = -mpVehicle->mPerform.mDecSpd;   dMoveStates[1]= _SLOWDOWN;//slow down
//	//dAccList[2] = -mpVehicle->mPerform.mDecSpd*.5;   dMoveStates[1]= _SLOWDOWN2;//slow down
//	dAccList[2] = 0 ; dMoveStates[2]=_UNIFORM; //uniform
//	dAccList[3] = 0.5*mpVehicle->mPerform.mAccSpd; dMoveStates[3]=_SLOWACC;//slow acc
//	dAccList[4] = mpVehicle->mPerform.mAccSpd;  dMoveStates[4]=_MAXACC;//max acc
//
//
//	
//	double dS = 0;
//	double dEndSpd = 0;
//	//double dAcc = 0;
//	double dt = (double)LandsideSimulation::tAwarenessTime; //t in seconds
//	StateInfo thisMoveState;
//	thisMoveState.mBeginTime = m_pOwner->curTime();
//	thisMoveState.mState =  _STOP;
//	thisMoveState.distInRoute = m_dLastDistInRoute;
//
//	for(int i=0;i<TEST_COUNT;i++)
//	{
//		double dThisAcc = dAccList[i];
//		double dNextSpeed = curState.dSpeed+dt*dThisAcc;
//		dNextSpeed = MAX(0,dNextSpeed);
//		dNextSpeed = MIN(dNextSpeed,dMaxSpd);
//
//		double dOffsetS = (dNextSpeed + curState.dSpeed)*0.5*dt;		
//		if(i==0) //max brake
//		{
//			dS = dOffsetS;
//			dEndSpd = dNextSpeed;
//			//continue;
//		}
//		
//		{			
//			thisMoveState.pHeadVehicle = CheckHeadVehicle(dOffsetS,dNextSpeed);
//			thisMoveState.pConflictVehicle = CheckVehicleConflict(dOffsetS,dNextSpeed);
//			thisMoveState.pConflictCross = CheckCrossWalkConflict(dOffsetS,dNextSpeed, thisMoveState.mBeginTime);
//			thisMoveState.pConflictIntersect = CheckIntersectionConflict(dOffsetS,dNextSpeed);
//			
//			
//			if(thisMoveState.pConflictIntersect || thisMoveState.pConflictCross || thisMoveState.pConflictVehicle || thisMoveState.pHeadVehicle )
//			{
//				break;
//			}
//			else
//			{
//				dS = dOffsetS;
//				dEndSpd = dNextSpeed;
//				thisMoveState.mState = dMoveStates[i];				
//			}
//			
//		}
//	}
//
//	//ReportDelay(_pEngine,thisMoveState);
////	ReportResourceQueue(_pEngine,thisMoveState);
//
//	//fix
//	double dDistTo = m_dLastDistInRoute+dS;
//	dDistTo = MIN(m_path.getAllLength()-1,dDistTo);
//	dS = dDistTo - m_dLastDistInRoute;
//
//	if(thisMoveState.pConflictVehicle)
//	{	
//		mpVehicle->WaitForVehicle(thisMoveState.pConflictVehicle);
//	}
//
//	if(dS >0)
//	{
//		mpVehicle->StartMove(mpVehicle->curTime());
//		LeaveCrosswalkQueue();
//		RelealseCrossWalk();
//		DoMovementsInPath(_pEngine,curState.time, dS, curState.dSpeed,dEndSpd);	
//		//Update conflict resolve Path
//		//
//		DistanceUnit distSpan = getDecDist(mpVehicle->getLastState().dSpeed) + mpVehicle->GetHalfLength();
//		m_path.getPath(m_dLastDistInRoute, m_dLastDistInRoute + distSpan,  m_pOwner->getSpanPath() );
//
//		return true;
//	}
//	else
//	{
//		curState.time += LandsideSimulation::tAwarenessTime;
//		mpVehicle->UpdateState(curState);
//		return false;
//	}
//
//	return false;
//}
//
//
//
//
#define CONCERN_RADIUS 10000 //500 meters
LandsideVehicleInSim* Goal_MoveInRoad::CheckVehicleConflict( double dOffsetS, double dNextSpeed )
{
	double dendDecDist = getDecDist(dNextSpeed);
		
	double dIndex = m_path.getDistIndex(m_dLastDistInRoute+dOffsetS);
	LandsidePosition nextPos = m_path.getIndexPosition(dIndex);
	ARCVector3 nextDir = m_path.getIndexDir(dIndex);
	
	LandsideResourceInSim* pCurRes = m_pOwner->getLastState().getLandsideRes();	
	LandsideResourceInSim *pNextRes = nextPos.pRes;//GetSegResource(  nNextSetIndex );	
	
	CPath2008 nextPath;
	m_path.getPath(m_dLastDistInRoute+dOffsetS,m_dLastDistInRoute+dOffsetS+dendDecDist, nextPath);
	//ARCPolygon nextPoly;
	//nextPoly.Build()

	LandsideResourceInSim* pItrRes = NULL;
	int fIndex = (int)m_path.getDistIndex(m_dLastDistInRoute);
	for(int iIndex = fIndex; iIndex < m_path.getPtCount();iIndex++ )
	{
		const LandsidePosition& landsidepos =  m_path.PositionAt(iIndex);
		LandsideResourceInSim* pthisRes =landsidepos.pRes;	

		//break if dist too far
		DistanceUnit distInPath = m_path.getIndexDist(iIndex);
		if(distInPath - m_dLastDistInRoute - dOffsetS - dendDecDist > 1000 )
		{
			break;
		}

		if(pthisRes == pItrRes)
			continue;

		pItrRes = pthisRes;
		if(!pthisRes)
			continue;

		
		DistanceUnit dist;
		if(LandsideVehicleInSim* pConflictV = pthisRes->CheckPathConflict(m_pOwner,nextPath,dist) )
		{
			return pConflictV;
		}	
		
	}

	return NULL;
}


bool Goal_MoveInRoad::ChangeLane( CARCportEngine *_pEngine )
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
	

	//search left 
	for(int i=pStretchLane->GetLaneIndex()-1;i>=0;i--)
	{
		LandsideLaneInSim* pOtherLane = pStretch->GetLane(i);
		if(!pOtherLane->IsSameDirToLane(pThisLane))
			break;

		double dChangeLaneDist = pOtherLane->GetPointDist(m_pOwner->getLastState().pos) + m_pOwner->GetLength();
		if(!pOtherLane->isSpaceEmpty(dChangeLaneDist-m_pOwner->GetLength(), dChangeLaneDist+m_pOwner->GetLength()))
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


	for(int i=pStretchLane->GetLaneIndex()+1;i<pStretch->GetLaneCount();i++)
	{
		LandsideLaneInSim* pOtherLane = pStretch->GetLane(i);
		if(!pOtherLane->IsSameDirToLane(pThisLane))
			break;

		double dChangeLaneDist = pOtherLane->GetPointDist(m_pOwner->getLastState().pos) + m_pOwner->GetLength();
		if(!pOtherLane->isSpaceEmpty(dChangeLaneDist-m_pOwner->GetLength(), dChangeLaneDist+m_pOwner->GetLength()))
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
		LandsideVehicleInSim* pOldAhead = pThisLane->GetAheadVehicle(m_pOwner,m_pOwner->getLastState().distInRes,distAheadFromLane);
		LandsideVehicleInSim* pNewAhead = pOtherLane->GetAheadVehicle(m_pOwner, dChangeLaneDist,distAheadToLane);
		LandsideVehicleInSim* pNewBack = pOtherLane->GetBehindVehicle(m_pOwner,dChangeLaneDist);

		
		bool bChangeOK = m_pOwner->GetChangeLaneModel().bChangOk(m_pOwner,false,pOldAhead,dChangeLaneDist,pOtherLane->GetLength(), pNewAhead,pNewBack);
		if(!bChangeOK)
			continue;

		//change lane
		DoChangeLane(_pEngine,pOtherLane,dChangeLaneDist,routepath);
		return true;
	}
	
	return false;
}



void Goal_MoveInRoad::UpdateTotalPath()
{
	m_path.Clear();

	for(int i=0;i<(int)mRoutePath.size()-1;i++)
	{
		_addSeg(mRoutePath.GetNode(i),mRoutePath.GetNode(i+1));
	}
	ASSERT(m_path.getPtCount()>0);
}

Goal_MoveInRoad::Goal_MoveInRoad( LandsideVehicleInSim* pEnt, LandsideResourceInSim* pDestResource,const LandsideLaneNodeList& path, bool bStopAtEnd, bool bChangeLane /*= false*/ ) 
:LandsideVehicleGoal(pEnt)
,m_pDestResource(pDestResource)
,m_blockInRoad(_T("Block in Road"))
{
	mRoutePath = path;
	m_dLastDistInRoute = 0;
	UpdateTotalPath();
	m_bChangeLane = bChangeLane;
	m_bStopAtEnd = bStopAtEnd;

	m_sDesc = "Goal_MoveInRoad";

	m_nChangeLaneCount=0;
}

CCrossWalkInSim* Goal_MoveInRoad::CheckCrossWalkConflict( double dOffsetS, double dNextSpd , const ElapsedTime& eTime)
{	
	double dVehHalfLen = m_pOwner->GetLength()*0.5;
	double dendDecDist = m_pOwner->mPerform.minDecDist(dNextSpd);
	LandsideResourceInSim* pCurRes = m_pOwner->getLastState().getLandsideRes();
	

	LandsideResourceInSim* pItrRes = NULL;
	int fIndex = (int)m_path.getDistIndex(m_dLastDistInRoute);
	for(int iIndex = fIndex; iIndex < m_path.getPtCount();iIndex++ )
	{
		const LandsidePosition& landsidepos =  m_path.PositionAt(iIndex);
		LandsideResourceInSim* pthisRes =landsidepos.pRes;	

		//break if dist too far
		DistanceUnit distInPath = m_path.getIndexDist(iIndex);
		if(distInPath - m_dLastDistInRoute - dOffsetS > dVehHalfLen )
		{
			break;
		}

		//check the first 
		if(pthisRes==pItrRes)continue;

		pItrRes = pthisRes;

		if(!pthisRes)continue;

		DistanceUnit distInRes = landsidepos.distInRes;
		if(pthisRes==pCurRes)
			distInRes = m_pOwner->getLastState().distInRes;

		LandsideLaneInSim* plane = pthisRes->toLane();
		if(!plane)
			continue;

		LandsideStretchLaneInSim* pStretchLane = plane->toStretchLane();
		if(!pStretchLane)
			continue;

		DistanceUnit dCrossDist;
		CCrossWalkInSim*pCross = pStretchLane->GetAheadCross(m_pOwner, distInRes,dCrossDist);
		if(!pCross)
			continue;
		
		DistanceUnit dCrossDistInPath = m_path.getIndexDist(iIndex) + dCrossDist;
		DistanceUnit dDistToCross = dCrossDistInPath - m_dLastDistInRoute - pCross->getHalfWidth() - dVehHalfLen;
		if(dDistToCross < dOffsetS + dendDecDist )
		{
			if(pCross->CanVehicleCross(eTime))
			{
				DistanceUnit distAheadvehicle;
				if( LandsideVehicleInSim* pAheadV = pStretchLane->GetAheadVehicle(m_pOwner,distInRes, distAheadvehicle) )
				{
					DistanceUnit distFree = distAheadvehicle - dCrossDist - pAheadV->GetHalfLength() - pCross->getHalfWidth();
					if(distFree < m_pOwner->GetLength() )
						return pCross;							
				}
			}
			else
			{
				return pCross;
			}
		}		
	}
	return NULL;
}

LandsideIntersectLinkGroupInSim* Goal_MoveInRoad::CheckIntersectionConflict( double dOffsetS, double dNextSpd )
{
	double dVehHalfLen = m_pOwner->GetLength()*0.5;
	double dendDecDist = m_pOwner->mPerform.minDecDist(dNextSpd);
	dendDecDist = dendDecDist+100;


	LandsidePosition nextPos = m_path.getDistPosition(m_dLastDistInRoute+dOffsetS+dendDecDist+dVehHalfLen);
	if(nextPos.pRes)
	{
		if(LandsideIntersectLaneLinkInSim* pLink = nextPos.pRes->toIntersectionLink() )
		{
			if(pLink->IsClosed())
			{
				return pLink->getGroup();
			}
		}
	}
	return NULL;
}

void Goal_MoveInRoad::RelealseCrossWalk()
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

void Goal_MoveInRoad::LeaveCrosswalkQueue()
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

void Goal_MoveInRoad::Activate( CARCportEngine* pEngine )
{
	//m_lastStateInfo.mState = _UNIFORM;
	//m_lastStateInfo.mBeginTime= m_pOwner->curTime();
	m_iStatus = gs_active;;
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
	/*ElapsedTime getDelayTime(LandsideVehicleInSim* pVehilce,LandsideResourceInSim* pRes, StateInfo& stateinfoF, StateInfo& stateinfoT)
	{
		double dist = stateinfoT.distInRoute-stateinfoF.distInRoute;
		double dSpd = pVehilce->getSpeed(pRes,stateinfoF.distInRoute) + pVehilce->getSpeed(pRes,stateinfoT.distInRoute);
		dSpd = dSpd *.5;
		ElapsedTime dT = ElapsedTime(dist/dSpd);
		return stateinfoT.mBeginTime - stateinfoF.mBeginTime - dT;
	}*/
}


void Goal_MoveInRoad::LeaveResourceQueueConflict(CARCportEngine* _pEngine)
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

void Goal_MoveInRoad::ReportConflictEnd(CARCportEngine* _pEngine, ConflictInfo& conflictInfo)
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
void Goal_MoveInRoad::ReportResourceQueue(CARCportEngine* _pEngine, ConflictInfo& conflictInfo)
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

bool Goal_MoveInRoad::OverPass( LandsideVehicleInSim* pFrontVehicle , CARCportEngine *_pEngine )
{
	if(!m_pOwner->getLastState().getLandsideRes() )
		return false;

	LandsideLaneInSim* pThisLane = m_pOwner->getLastState().getLandsideRes()->toLane();
	if(!pThisLane)
		return false;	
	if(pFrontVehicle->getLastState().getLandsideRes() != pThisLane)
		return false;
	if(pFrontVehicle->getLastState().dSpeed > 0 )
		return false;

	DistanceUnit halfLen = m_pOwner->GetLength()*0.5;

	DistanceUnit preVPos = pFrontVehicle->getLastState().distInRes;
	DistanceUnit wantpos = preVPos + pFrontVehicle->GetSafeDistToFront() + halfLen + 100;
	DistanceUnit distMove = wantpos - m_pOwner->getLastState().distInRes;
	if(distMove <=0 || distMove > 20000 )
		return false;
	//check if the want pos exceed the next pt
	int nCurSeg = 0;//GetSegIndex(m_dLastDistInRoute);
	LandsideLaneNodeInSim* pNextNode = mRoutePath.GetNode(nCurSeg+1);
	if(!pNextNode)
		return false;

	if(pNextNode->m_distInlane < wantpos )
	{
		return false;
	}


	if( ! pThisLane->isSpaceEmpty(wantpos - halfLen, wantpos+ m_pOwner->GetSafeDistToFront() ) )
	{	
		DistanceUnit distAhead;
		if( LandsideVehicleInSim* pFrontFrontVehicle = pThisLane->GetAheadVehicle(pFrontVehicle,pFrontVehicle->getLastState().distInRes,distAhead) )
		{
			return OverPass(pFrontFrontVehicle,_pEngine);
		}
		return false;
	}


	if(LandsideLaneInSim* pLeftLane = pThisLane->getLeft())
	{
		DistanceUnit distF = pLeftLane->GetPointDist( m_pOwner->getLastState().pos) + 100;
		DistanceUnit distT = pLeftLane->GetPointDist(  pThisLane->GetDistPoint(wantpos) ) -100;
		if( distF < distT && pLeftLane->isSpaceEmpty(distF,distT) )
		{
			//do overpass
			MobileState tState = m_pOwner->getLastState();
			tState.distInRes = distF;
			tState.pRes = pLeftLane;
			tState.pos = pLeftLane->GetDistPoint(distF);
			tState.dSpeed = m_pOwner->getSpeed(pLeftLane,distF);
			m_pOwner->MoveToPos(tState);

			CPath2008 path = pLeftLane->GetSubPathDist(distF,distT);
			for(int i=1;i<path.getCount();i++)
			{
				tState.distInRes = distF + path.GetIndexDist(i);
				tState.pos = path.getPoint(i);
				m_pOwner->MoveToPos(tState);
			}
	

			tState.distInRes = wantpos;
			tState.pRes = pThisLane;
			tState.pos = pThisLane->GetDistPoint(wantpos);
			m_pOwner->MoveToPos(tState);			
			//
			m_dLastDistInRoute += distMove; 
			return true;
		}
	}
	if(LandsideLaneInSim* pRightLane = NULL /*pThisLane->getRight()*/ )
	{
		DistanceUnit distF = pRightLane->GetPointDist( m_pOwner->getLastState().pos) + 100;
		DistanceUnit distT = pRightLane->GetPointDist(  pThisLane->GetDistPoint(wantpos) )-100;
		if( distF < distT && pRightLane->isSpaceEmpty(distF,distT) )
		{
			//do overpass
			MobileState tState = m_pOwner->getLastState();
			tState.distInRes = distF;
			tState.pRes = pRightLane;
			tState.pos = pRightLane->GetDistPoint(distF);
			tState.dSpeed = m_pOwner->getSpeed(pRightLane,distF);
			m_pOwner->MoveToPos(tState);

			CPath2008 path = pRightLane->GetSubPathDist(distF,distT);
			for(int i=1;i<path.getCount();i++)
			{
				tState.distInRes = distF + path.GetIndexDist(i);
				tState.pos = path.getPoint(i);
				m_pOwner->MoveToPos(tState);
			}


			tState.distInRes = wantpos;
			tState.pRes = pThisLane;
			tState.pos = pThisLane->GetDistPoint(wantpos);
			m_pOwner->MoveToPos(tState);

			m_dLastDistInRoute += distMove; 
			return true;
		}
	}
	return false;
}



void Goal_MoveInRoad::_addSeg( LandsideLaneNodeInSim* pNodePre ,LandsideLaneNodeInSim* pNodeNext )
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

bool Goal_MoveInRoad::_isOutRoute()
{
	return m_dLastDistInRoute > ( m_path.getAllLength() -m_pOwner->GetLength()*0.5);
	return false;
}

void Goal_MoveInRoad::DoChangeLane(CARCportEngine* _pEngine, LandsideLaneInSim* pToLane , DistanceUnit dChangeLaneDist, const LandsideLaneNodeList& routepath)
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



void Goal_MoveToRes::Activate( CARCportEngine* pEngine )
{
	m_iStatus = gs_active;
	RemoveAllSubgoals();

	LandsideResourceInSim* pOrign = m_pOwner->getLastState().getLandsideRes();
	DistanceUnit distInRes = m_pOwner->getLastState().distInRes;
	LandsideResourceManager* pRes = pEngine->GetLandsideSimulation()->GetResourceMannager();
	ASSERT(pOrign);
	ASSERT(mpDest);
	if(pOrign==NULL)
	{
		m_iStatus = gs_completed;
		return;
	}

	LandsideLaneNodeList mRoutePath;
	
	if(LandsideLaneInSim* pAtLane = pOrign->toLane())				
	{		
		bool btrue =  pRes->mRouteGraph.FindRouteStretchPosToRes(pAtLane,distInRes,mpDest,mRoutePath);
		ASSERT(btrue);
		if(!btrue)
		{
			CString str = _T("Can not find Route to Dest");
			if(pOrign && mpDest){
				str.Format("Can not find Route From %s to %s", pOrign->print().GetString(),mpDest->print().GetString());
			}		
			throw SGoalException(str.GetString());
		}
		m_pLaneEntry = new LandsideLaneEntry();
		m_pLaneEntry->SetFromRes(pAtLane);
		m_pLaneEntry->SetPosition(pAtLane,m_pOwner->getLastState().pos);
		mRoutePath.insert(mRoutePath.begin(),m_pLaneEntry);
	}
	else
	{
		bool btrue =  pRes->mRouteGraph.FindRouteResToRes(pOrign,mpDest,mRoutePath);
		ASSERT(btrue);
		if(!btrue)
		{
			CString str = _T("Can not find Route  to Dest");
			if(pOrign && mpDest){
				str.Format("Can not find Route From %s to %s", pOrign->print().GetString(),mpDest->print().GetString());
			}		
			throw SGoalException(str.GetString());
		}
	}
	AddSubgoalFront(new Goal_MoveInRoad(m_pOwner,mpDest,mRoutePath,false,true));
}

Goal_MoveToRes::Goal_MoveToRes( LandsideVehicleInSim* pEnt, LandsideResourceInSim* pDest ) :LandsideVehicleSequenceGoal(pEnt)
{
	mpDest = pDest;		
	m_sDesc.Format(_T("Moving to %s"),pDest->print().GetString());
	m_pLaneEntry = NULL;
}

//////////////////////////////////////////////////////////////////////////
void Goal_BirthAtEntryNode::Process( CARCportEngine* pEngine )
{
	ActivateIfInactive(pEngine);
	ElapsedTime t = m_pOwner->curTime();
	LandsideSimulation* pLandSim = pEngine->GetLandsideSimulation();
	
	MobileState birthState;
	birthState.distInRes = 0;
	birthState.time = t;
	birthState.dSpeed = m_pOwner->getSpeed(m_pNode,0);
	birthState.pRes = m_pNode;
	birthState.pos = m_pNode->getPos();
	//birthState.motion = _stopped;
	m_pOwner->initState(birthState);
	m_iStatus = gs_completed;
	
	
	//return m_iStatus;
}






//
//bool StateInfo::IsDiffer( const StateInfo& s ) const
//{
//	if(mState!=s.mState)return true;
//	if(pConflictCross!=s.pConflictCross)return true;
//	if(pConflictIntersect!=s.pConflictIntersect)return true;
//	if(pConflictVehicle!=s.pConflictVehicle)return true;
//	if(pConflictPax!=s.pConflictPax) return true;
//	if(pHeadVehicle != s.pHeadVehicle) return true;
//	return false;
//}


bool Goal_MoveInRoad::MoveInLane2( CARCportEngine *_pEngine )
{
	m_pOwner->WaitForVehicle(NULL);

	LandsidePosition nextPos = m_path.getDistPosition(m_dLastDistInRoute+100);
	double dMaxSpd = m_pOwner->getSpeed(nextPos.pRes,nextPos.distInRes);	
	
	double dCurSpeed = m_pOwner->getLastState().dSpeed;
	double dt = (double)LandsideSimulation::tAwarenessTime; //t in seconds

	DistanceUnit distRad = getDecDist(dCurSpeed) + 5000;

	ConflictInfo conflictInfo;
	_getConflictInfo(distRad, conflictInfo);
	
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
		DistanceUnit distSpan =  getDecDist(m_pOwner->getLastState().dSpeed);
		m_path.getPath(m_dLastDistInRoute, m_dLastDistInRoute + distSpan + m_pOwner->GetHalfLength(),  m_pOwner->getSpanPath() );
		
	}
	else if (opType == _STOP)
	{
		DoMovementsInPath(_pEngine,curState.time, dOffsetS, curState.dSpeed,dEndSpd);
		//Update conflict resolve Path
		DistanceUnit distSpan =  getDecDist(m_pOwner->getLastState().dSpeed);
		m_path.getPath(m_dLastDistInRoute, m_dLastDistInRoute + distSpan + m_pOwner->GetHalfLength(),  m_pOwner->getSpanPath() );
	}
	
	if(dEndSpd>=dMaxSpd)
	{
		ReportEndDelay(_pEngine, m_pOwner->getLastState().time);
	}
	return true;
}

DistanceUnit Goal_MoveInRoad::getDecDist( double dSpeed )
{
	double dendDecDist = m_pOwner->mPerform.minDecDist(dSpeed);
	dendDecDist += m_pOwner->getIDModel().gap;
	dendDecDist += m_pOwner->getIDModel().T * dSpeed;
	return dendDecDist;
}

LandsideVehicleInSim* Goal_MoveInRoad::CheckHeadVehicle( double dOffsetS, double dNextSpeed )
{
	double dendDecDist = getDecDist(dNextSpeed);
	
	double dIndex = m_path.getDistIndex(m_dLastDistInRoute+dOffsetS);
	LandsidePosition nextPos = m_path.getIndexPosition(dIndex);
	
	LandsideResourceInSim* pCurRes = m_pOwner->getLastState().getLandsideRes();	
	LandsideResourceInSim *pNextRes = nextPos.pRes;

	LandsideResourceInSim* pItrRes = NULL;
	int fIndex = (int)m_path.getDistIndex(m_dLastDistInRoute);
	for(int iIndex = fIndex; iIndex < m_path.getPtCount();iIndex++ )
	{
		const LandsidePosition& landsidepos =  m_path.PositionAt(iIndex);
		LandsideResourceInSim* pthisRes =landsidepos.pRes;	
		//break if dist too far
		DistanceUnit distInPath = m_path.getIndexDist(iIndex);
		if(distInPath - m_dLastDistInRoute - dOffsetS - dendDecDist > 1000 )
		{
			break;
		}

		if(pthisRes == pItrRes)
			continue;

		pItrRes = pthisRes;
		if(!pthisRes)
			continue;

		//keep sep with ahead vehicle			
		{	
			DistanceUnit distInRes = landsidepos.distInRes;
			if(pthisRes==pCurRes)
			{
				distInRes = m_pOwner->getLastState().distInRes;
				//ASSERT(distInRes >= landsidepos.distInRes );
			}

			DistanceUnit distAhead;
			LandsideVehicleInSim* pHeadV = pthisRes->GetAheadVehicle(m_pOwner,distInRes,distAhead);
			if(pHeadV)
			{
				DistanceUnit distHead = distAhead - landsidepos.distInRes + m_path.getIndexDist(iIndex);
				if(distHead - m_dLastDistInRoute < dOffsetS + dendDecDist + m_pOwner->GetHalfLength() + pHeadV->GetHalfLength() )
				{					
					return pHeadV;
				}
			}			
		}		
	}

	return NULL;
}

void Goal_MoveInRoad::_getConflictInfo(DistanceUnit distRad ,ConflictInfo& conflictInfo)
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
				CCrossWalkInSim*pCross = pStretchLane->GetAheadCross(m_pOwner, distInRes,dCrossDist);
				if(pCross)
				{
					DistanceUnit dCrossDistInPath = m_path.getIndexDist(iIndex) + dCrossDist;
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


void Goal_MoveInRoad::ReportDelay( CARCportEngine* _pEngine,const DelayInfo& delay )
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

void Goal_MoveInRoad::ReportEndDelay( CARCportEngine* _pEngine, const ElapsedTime& t )
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

bool Goal_MoveInRoad::bCanChangeLane( ) const
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

void Goal_ParkingExternalNode::Process( CARCportEngine* pEngine )
{
	ActivateIfInactive(pEngine);
	ElapsedTime t = m_pOwner->curTime();
	LandsideSimulation* pLandSim = pEngine->GetLandsideSimulation();

	MobileState deadState;
	deadState.distInRes = 0;
	deadState.time = t;
	deadState.dSpeed = m_pOwner->getSpeed(m_pNode,0);
	deadState.pRes = m_pNode;
	deadState.pos = m_pNode->getPos();
	//birthState.motion = _stopped;
	m_pOwner->initState(deadState);
	m_iStatus = gs_completed;
}
