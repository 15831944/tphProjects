#include "StdAfx.h"
#include ".\landsidevehiclemoveinroad.h"
#include "LandsideSimulation.h"
#include "ARCportEngine.h"
#include "LandsideVehicleInSim.h"
#include "LandsideParkingLotInSim.h"
#include "LandsideVehicleInSpotState.h"
#include "LandsideResourceManager.h"
#include "LandsideVehicleInSim.h"
#include "common/DistanceLineLine.h"
#include "LandsideIntersectionInSim.h"
#include "LandsideIntersectionInSim.h"
#include "LandsideExternalNodeInSim.h"

#define CONCERN_RADIUS 10000 //500 meters
//////////////////////////////////////////////////////////////////////////
IVehicleStateMovement* LandsideVehicleMoveInRoad::proceedTo( CARCportEngine *_pEngine )
{
	//get last at seg
	/*double dLen = mpVehicle->GetLength();
	if( m_dLastDistInRoute < *nSegChangeDist.rbegin() - dLen*2 )
	{
		if(ChangeLane(_pEngine))
			return this;
	}*/
	MoveInLane(_pEngine);

	if(IsOutRoute()) //done current phase
	{
		return gotoNextState(_pEngine);
	}	
	return this;	
}
//change to next state
IVehicleStateMovement * LandsideVehicleMoveInRoad::gotoNextState( CARCportEngine *_pEngine )
{
	ElapsedTime t = mpVehicle->curTime();
	LandsideSimulation* pLaneSim = _pEngine->GetLandsideSimulation();

	if(!mpDest)
	{	
		mpVehicle->Terminate(t);
		return NULL;
	}
	if(LandsideCurbSideInSim* pDestCurb = mpDest->toCurbSide() )
	{
		LandsideVehicleMoveInCurbside* pNextMove = new LandsideVehicleMoveInCurbside(mpVehicle,pDestCurb, *mRoutePath.rbegin() );	
		return pNextMove;
	}
	if(LandsideParkingLotDoorInSim* pEntryDoor = mpDest->toParkLotDoor() )
	{				
		LandsideParkingLotInSim* pLot = pEntryDoor->getParkingLot();
		LandsideParkingSpotInSim* pFreeSpot = pLot->getFreeSpot(mpVehicle);
		if(pFreeSpot) //get a spot park there
		{
			VehicleEnterParkingLot* pNextState = new VehicleEnterParkingLot(mpVehicle,pEntryDoor,pFreeSpot);
			if(mpVehicle->IsDepPaxVehicle())
			{
				mpVehicle->setDropPaxPlace(pFreeSpot);
			}
			else
			{
				mpVehicle->setPickPaxPlace(pFreeSpot);
			}
			return pNextState;
		}
		else //go to other parking lot
		{
			LandsideParkingLotInSim* pOtherLot = pLaneSim->AssignLongTermParkingLot(mpVehicle,pLot);
			if(pOtherLot)
			{
				LandsideParkingLotDoorInSim* pDoor = pLot->getEntryDoor(mpVehicle);
				if(pDoor)
				{
					LandsideVehicleMoveInRoad* pMoveToOtherLot = new LandsideVehicleMoveInRoad(mpVehicle);
					if( pMoveToOtherLot->SetOrignDest(pEntryDoor,pDoor,_pEngine) )
					{
						return pMoveToOtherLot;
					}
					else
					{
						delete pMoveToOtherLot;
					}
					
				}
			}
			//no other lot terminate
			mpVehicle->Terminate(t);
			return NULL;
		}
	}
	if(mpDest->toExtNode())
	{
		mpVehicle->Terminate(t);
		return NULL;
	}

	ASSERT(FALSE);
	return this;
}

bool LandsideVehicleMoveInRoad::SetOrignDest( LandsideResourceInSim* pOrign, LandsideResourceInSim* pDest,CARCportEngine *_pEngine )
{
	mpOrign = pOrign;
	mpDest = pDest;
	//
	LandsideResourceManager* pRes = _pEngine->GetLandsideSimulation()->GetResourceMannager();
	mRoutePath.clear();
	bool btrue =  pRes->mRouteGraph.FindRoute(pOrign,pDest,mRoutePath);
	UpdateTotalPath();
	return btrue;
}

bool LandsideVehicleMoveInRoad::SetOrignDest( LandsideLaneNodeInSim* pOrign, LandsideResourceInSim* pDest,CARCportEngine *_pEngine )
{
	mpOrign = pOrign->getFromRes();
	mpDest = pDest;
	//
	LandsideResourceManager* pRes = _pEngine->GetLandsideSimulation()->GetResourceMannager();
	mRoutePath.clear();
	bool bret = pRes->mRouteGraph.FindRoute(pOrign,pDest,mRoutePath);
	UpdateTotalPath();
	return bret;
}


LandsideVehicleMoveInRoad::LandsideVehicleMoveInRoad( LandsideVehicleInSim*pVehicle ) :IVehicleStateMovement(pVehicle)
{
	mpOrign = NULL;
	mpDest = NULL;
	//m_iLastSegIndex = -1;
	m_dLastDistInRoute = -1;
}



		

#include "Common/DynamicMovement.h"
void LandsideVehicleMoveInRoad::DoMovementsInPath( double dEndSpd,double dS,MobileState lastmvPt,const ElapsedTime& dEndT )
{
	double dDistFrom = m_dLastDistInRoute;
	double dDistTo = m_dLastDistInRoute+dS;

	ElapsedTime dStartT = lastmvPt.time;
	double dFromDist = lastmvPt.distInRes;
	//DynamicMovement dyMove(lastmvPt.dSpeed,dEndSpd,dDistTo-dDistFrom);
	//double dIndexFrom = mTotalPath.GetDistIndex(dDistFrom);
	double dIndexTo = mTotalPath.GetDistIndex(dDistTo);

	//
	int iSegIndex = GetSegIndex(dDistTo);
	if(dS>0 && iSegIndex>=0)
	{
		lastmvPt.mAtRes = GetSegResource(iSegIndex);
		lastmvPt.distInRes = GetSegStartDistInRes(iSegIndex) + dDistTo - nSegChangeDist[iSegIndex];
		lastmvPt.dSpeed = dEndSpd;
		lastmvPt.pos = mTotalPath.GetIndexPos(dIndexTo);
		lastmvPt.time = dEndT;
		mpVehicle->addNextState(lastmvPt);
	}
	else
	{
		lastmvPt.time = dEndT;
		mpVehicle->addNextState(lastmvPt);
	}
	m_dLastDistInRoute += dS;
}



double LandsideVehicleMoveInRoad::GetSegStartDistInRes( int nSegIndex ) const
{
	if(nSegIndex < (int)mRoutePath.size()-1  && nSegIndex>=0 )
	{
		LandsideLaneNodeInSim* pNode = mRoutePath[nSegIndex];
		if(LandsideLaneEntry* pEntry =pNode->toEntry())
		{
			return pEntry->m_distInlane;
		}		
	}	
	return 0;
}

#include "common/ARCRay.h"
LandsideVehicleInSim * LandsideVehicleMoveInRoad::GetAheadVehicle(LandsideResourceInSim* pRes, double& dMinDist )const
{
	LandsideVehicleInSim* pConflictVehcile = NULL;
	for(int i=0;i<pRes->GetInResVehicleCount();i++)
	{
		LandsideVehicleInSim* pOtherVeh = pRes->GetInResVehicle(i);
		if(mpVehicle == pOtherVeh)
			continue;

		const ARCPoint3& pos = pOtherVeh->getState().pos;
		double dCarLen = (pOtherVeh->GetLength() + mpVehicle->GetLength())*0.5;
		ARCPathIntersect intersect;
		if( intersect.Get(mTotalPath,pos,dCarLen,m_dLastDistInRoute,true) )
		{
			double dDistToOther = *intersect.pathDistList.begin()-m_dLastDistInRoute;
			if(!pConflictVehcile || dDistToOther < dMinDist )
			{
				dMinDist = dDistToOther;
				pConflictVehcile = pOtherVeh;
			}
				
		}	
		
	} 
	return pConflictVehcile;
}

bool LandsideVehicleMoveInRoad::GetAheadBlock( double& distToHead, double& dAheadSpd )
{
	
	int nCurSeg = GetSegIndex(m_dLastDistInRoute);
	int nNextSeg = GetSegIndex(m_dLastDistInRoute+CONCERN_RADIUS);

	//search in the route for the ahead flight
	for(int i=nCurSeg;i<nNextSeg+1;i++)
	{
		LandsideResourceInSim* pResource = GetSegResource(i);
		if(!pResource)
			continue;

		double dSep = 0;
		if (LandsideVehicleInSim* pOtherV = GetAheadVehicle(pResource, dSep) )
		{
			distToHead = dSep;
			dAheadSpd = pOtherV->getState().dSpeed;
			return true;
		}	
	}
	return false;	 
}



LandsideResourceInSim* LandsideVehicleMoveInRoad::GetSegResource( int nSegIndex ) const
{
	if(nSegIndex< (int)mRoutePath.size() && nSegIndex >=0 )
	{
		LandsideLaneNodeInSim* pNode =mRoutePath[nSegIndex];
		return pNode->getToRes();
	}
	return NULL;
}

const ARCPath3* LandsideVehicleMoveInRoad::GetSegPath( int nSegIndex, ARCPath3& subPath )const
{
	LandsideLaneNodeInSim* pNodePre = NULL;
	LandsideLaneNodeInSim* pNodeNext = NULL;
	if(nSegIndex < 0)
	{
		pNodeNext = mRoutePath[0];
	}
	else if(nSegIndex < (int)mRoutePath.size()-1 )
	{
		pNodePre = mRoutePath[nSegIndex];
		pNodeNext = mRoutePath[nSegIndex +1];
	}
	else if(nSegIndex == mRoutePath.size()-1)
	{
		pNodePre = mRoutePath[nSegIndex];
	}	
	
	
	LandsideResourceInSim* pResource = NULL;
	//get resource and next path
	if(pNodePre && pNodeNext )
	{
		ASSERT(pNodePre->getToRes() == pNodeNext->getFromRes());

		pResource = pNodePre->getToRes();
		if(LandsideLaneInSim* pLane = pResource->toLane()) // in lane
		{			
			//const ARCPath3& respath = pLane->getPath();
			subPath = pLane->GetSubPathDist( pNodePre->m_distInlane, pNodeNext->m_distInlane );
			return &subPath;						
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
					const ARCPath3& respath = pLink->m_path;				
					return &respath;							
				}
			}
		}		
		else if(LandsideCurbSideInSim* pCurb  = pResource->toCurbSide())
		{
			ASSERT(pNodePre->mpLane == pNodeNext->mpLane);
			//const ARCPath3& respath = pNodePre->mpLane->getPath();
			
			subPath = pLane->GetSubPathDist( pNodePre->m_distInlane, pNodeNext->m_distInlane );
			return &subPath;	
		}
	}
	else if(pNodePre)
	{
		pResource = pNodePre->getToRes();
		if(LandsideCurbSideInSim* pCurb = pResource->toCurbSide())
		{
			subPath = pNodePre->mpLane->GetPathLeft(pNodePre->m_distInlane);
			return &subPath;
		}
	}

	return NULL;
}

//void LandsideVehicleMoveInRoad::translate(double dAcc, const ElapsedTime& dt)
//{
//	MobileState state = mpVehicle->getState();
//	double dNextSpd = dAcc*(double)dt  + state.dSpeed;
//	dNextSpd = MAX(0,dNextSpd);
//	dNextSpd = MIN(mpVehicle->GetIDM().v0,dNextSpd);
//
//	double dS = (dNextSpd + state.dSpeed)*(double)dt*0.5;
//	DoMovementsInPath(dNextSpd,dS,state, state.time + dt);	
//
//	m_dLastDistInRoute +=dS;
//}

void LandsideVehicleMoveInRoad::UpdateTotalPath()
{
	mTotalPath.clear();
	nSegChangeDist.clear();
	nSegChangePtIndex.clear();

	double dLastDist = 0;
	int dLastIndex =0;
	nSegChangeDist.push_back(dLastDist);
	nSegChangePtIndex.push_back(dLastIndex);

	for(int i=0;i<(int)mRoutePath.size()-1;i++)
	{
		ARCPath3 subPath;
		const ARCPath3* retPath = GetSegPath(i,subPath);
		ASSERT(retPath);
		double dSegLen = retPath->GetLength();
		mTotalPath.Append(*retPath);
		nSegChangeDist.push_back(dLastDist+=dSegLen);
		nSegChangePtIndex.push_back(dLastIndex+=retPath->size());		
	}
}

int LandsideVehicleMoveInRoad::GetSegIndex( double dDist ) const
{
	for(int i=0;i<(int)nSegChangeDist.size();i++)
	{
		if(nSegChangeDist[i]> dDist)
			return i-1;
	}
	return i-1;
}

bool LandsideVehicleMoveInRoad::IsOutRoute() const
{
	return m_dLastDistInRoute  > *nSegChangeDist.rbegin();
}

//bool LandsideVehicleMoveInRoad::ChangeLane( CARCportEngine *_pEngine )
//{	
//	if(!mpVehicle->getState().mAtRes)
//		return false;
//
//	LandsideLaneInSim* pThisLane = mpVehicle->getState().mAtRes->toLane();
//	if(!pThisLane)
//		return false;
//
//	if(LandsideLaneInSim* pLeft = pThisLane->getLeft())
//	{
//		if(ChangeToLane(pThisLane,pLeft,_pEngine,true))
//			return true;
//	}
//	if(LandsideLaneInSim* pRight = pThisLane->getRight() )
//	{
//		if(ChangeToLane(pThisLane,pRight,_pEngine,false))
//			return true;
//	}
//	return false;
//}


//bool LandsideVehicleMoveInRoad::ChangeToLane( LandsideLaneInSim* pFromLane, LandsideLaneInSim* pToLane, CARCportEngine *_pEngine,bool bLeft )
//{
//	if(pFromLane->IsPositiveDir()==pToLane->IsPositiveDir())
//	{
//		double dDist = pToLane->m_path.getPointDistance(mpVehicle->getState().pos);
//		double dChangeLaneDist = dDist + mpVehicle->GetLength();
//		double dIterDist = dChangeLaneDist;
//		while(LandsideLaneExit* pNextExit  = pToLane->GetNextLaneExit(dIterDist) )
//		{
//			dIterDist = pNextExit->m_distInlane;
//
//			LandsideResourceManager* pRes = _pEngine->GetLandsideSimulation()->GetResourceMannager();
//			LandsideLaneNodeList routepath;
//			if( !pRes->mRouteGraph.FindRoute(pNextExit, mpDest, routepath) )
//			{
//				continue;
//			}
//			
//			ASSERT(pNextExit == *routepath.begin());
//			//
//			LandsideVehicleInSim* pOldAhead = pFromLane->GetAheadVehicle(mpVehicle,mpVehicle->getState().distInRes);
//			LandsideVehicleInSim* pNewAhead = pToLane->GetAheadVehicle(mpVehicle, dChangeLaneDist);
//			LandsideVehicleInSim* pNewBack = pToLane->GetBehindVehicle(mpVehicle,dChangeLaneDist);
//
//			ARCVector3 nextLanePos = pToLane->m_path.GetDistPos(dChangeLaneDist);
//			ARCVector3 nextLaneDir = pToLane->m_path.GetDistPos(dChangeLaneDist);
//			double dendDecDist = mpVehicle->GetIDM().decDist(mpVehicle->GetIDM().v0);
//
//			double dhalflen = mpVehicle->GetLength()*0.5;
//			ARCPolygon nextPoly;
//			nextPoly.Build(nextLanePos.xy(), nextLaneDir.xy(),dhalflen , dhalflen,mpVehicle->GetWidth());
//
//			if( !CheckConflict(nextPoly,nextLaneDir,pToLane)  
//				&& mpVehicle->GetChangeLaneModel().bChangOk(mpVehicle,bLeft,pOldAhead,dChangeLaneDist,pNewAhead,pNewBack) 
//				)
//			{
//
//				mpVehicle->WriteLog(mpVehicle->getState());
//				//write change lane log
//				MobileState nextstat = mpVehicle->getState();
//				nextstat.mAtRes = pToLane;
//				nextstat.distInRes = dChangeLaneDist;
//				nextstat.pos = nextLanePos;
//				double dDist= nextstat.pos.DistanceTo(mpVehicle->getState().pos);
//				nextstat.time += ElapsedTime(dDist/mpVehicle->GetIDM().v0);
//				mpVehicle->addNextState(nextstat);
//
//				//re find the path
//				LandsideLaneEntry* pEntry= pToLane->GetFirstLaneEntry();
//				mRoutePath.clear();
//				mRoutePath.push_back(pEntry);
//				mRoutePath.insert(mRoutePath.end(),routepath.begin(),routepath.end());
//				UpdateTotalPath();
//				m_dLastDistInRoute = dChangeLaneDist - pEntry->m_distInlane;
//				return true;
//			}
//			else
//				break;
//		}
//	}
//	return false;
//}
//


bool LandsideVehicleMoveInRoad::MoveInLane( CARCportEngine *_pEngine )
{
	
	double dt = (double)LandsideSimulation::tAwarenessTime;

	MobileState curState = mpVehicle->getState();
	double halfLen = mpVehicle->GetLength()*0.5;
	LandsideVehicleInSim* pConflictVehicle = NULL;


	double dAccList[5];
	dAccList[0] = -mpVehicle->mPerform.dMaxDec;   //max brake
	dAccList[1] = -0.1*mpVehicle->mPerform.dMaxDec; //slow down
	dAccList[2] = 0 ;  //uniform
	dAccList[3] = 0.1*mpVehicle->mPerform.dMaxAcc; //slow acc
	dAccList[4] = mpVehicle->mPerform.dMaxAcc;  //max acc
	

	double dS = 0;
	double dEndSpd = 0;
	for(int i=0;i<5;i++)
	{
		double dAcc = dAccList[i];
		double dNextSpeed = curState.dSpeed+dt*dAcc;
		dNextSpeed = MIN(dNextSpeed,mpVehicle->mPerform.dMaxSpeed);
		dNextSpeed = MAX(0,dNextSpeed);

		double dOffsetS = (dNextSpeed+curState.dSpeed)*0.5*dt;
		double dendDecDist = mpVehicle->mPerform.minDecDist(dNextSpeed);
		dendDecDist = MAX(dendDecDist,100);
		
		if(dOffsetS>0)
		{
			double nextdistIndex = mTotalPath.GetDistIndex(dOffsetS+m_dLastDistInRoute);
			ARCPoint3 nextPos = mTotalPath.GetIndexPos(nextdistIndex);
			ARCVector3 nextDir = mTotalPath.GetIndexDir(nextdistIndex);

			ARCPolygon nextPoly;
			nextPoly.Build(nextPos.xy(),nextDir.xy(), halfLen+dendDecDist,dOffsetS-halfLen,mpVehicle->GetWidth());
			if(pConflictVehicle = CheckNextConflict(nextPoly,nextDir))
			{
				break;
			}
			else 
			{
				dS = dOffsetS;
				dEndSpd = dNextSpeed;
			}
		}
	}


	if(dS >0)
	{
		DoMovementsInPath(dEndSpd,dS, curState,curState.time + LandsideSimulation::tAwarenessTime);
		mpVehicle->StartMove(mpVehicle->curTime());
		mpVehicle->WaitForVehicle(NULL);		
		return true;
	}
	else
	{		
		ASSERT(pConflictVehicle);
		//curState.time+=LandsideSimulation::tAwarenessTime;
		//mpVehicle->addNextState(curState);
		if(mpVehicle!=pConflictVehicle)
		{	
			mpVehicle->WaitForVehicle(pConflictVehicle);
			mpVehicle->SetActive(false);
		}
		else
		{
			curState.time+= LandsideSimulation::tAwarenessTime;
			mpVehicle->addNextState(curState);
		}
		return false;
	}
	
	return true;
}

LandsideVehicleInSim * LandsideVehicleMoveInRoad::CheckConflict( const ARCPolygon& poly, const ARCVector3& dir,LandsideResourceInSim* pRes )
{
	LandsideVehicleInSim* pConflictVehcile = NULL;
	for(int i=0;i<pRes->GetInResVehicleCount();i++)
	{
		LandsideVehicleInSim* pOtherVeh = pRes->GetInResVehicle(i);
		if(mpVehicle == pOtherVeh)
			continue;
		if( !mpVehicle->bCanWaitFor(pOtherVeh) )
			continue;

		if( poly.IsOverlapWithOtherPolygon(pOtherVeh->getBBox()) )
			return pOtherVeh;
	
		//ARCVector3 dirToHead = pOtherVeh->getState().pos-mpVehicle->getState().pos;
		//if(dir.dot(dirToHead)>0 )
		//{
			/*if(pOtherVeh && poly.size()>1 && pOtherVeh->getBBox().IsOverLap(poly[0],poly[1]) )
			{
				return pOtherVeh;
			}*/
		//}
	}
	return NULL;		
}

LandsideVehicleInSim* LandsideVehicleMoveInRoad::CheckNextConflict( const ARCPolygon& poly, const ARCVector3& dir )
{
	int nCurSeg = GetSegIndex(m_dLastDistInRoute);
	int nNextSeg = GetSegIndex(m_dLastDistInRoute+CONCERN_RADIUS);

	//search in the route for the ahead flight
	for(int i=nCurSeg;i<nNextSeg+1;i++)
	{
		LandsideResourceInSim* pResource = GetSegResource(i);
		if(!pResource)
			continue;
		
		if (LandsideVehicleInSim* pOtherV = CheckConflict(poly,dir, pResource) )
		{
			return pOtherV;
		}	
	}
	return false;	 
}


//////////////////////////////////////////////////////////////////////////
IVehicleStateMovement* LandsideVehicleMoveInCurbside::proceedTo( CARCportEngine *_pEngine )
{
	if(!IsOutRoute())
	{
	
		//MobileState lastpos = mpVehicle->getState();
		if(! mpVehicle->IsInLoadUnLoadPax() )
		{
			bool bStoped = !MoveInLane(_pEngine);		

			if(bStoped && !bReadyToLeave() ) //stop for unload pax
			{
				//mpVehicle->setTime(t);
				
				doPickDrop(mpVehicle->curTime(),_pEngine);
				mpVehicle->SetActive(false); //wait for pax notify
				mpVehicle->SetInLoadUnLoadPax(true);
				return this;
			}
		}
		else //in loading or unload pax
		{
			//mpVehicle->setTime(t);			
			if(!bReadyToLeave())
			{
				mpVehicle->SetActive(false);
				return this;
			}
			else
			{
				mpVehicle->SetInLoadUnLoadPax(false);
				mpVehicle->WriteLog(mpVehicle->getState());
			}
		}		
		//AddPathMotionMovements(path,dEndDist,tAwarenessTime,dCurMotion,lastpos);	
		return this;
	}

	//get a exit node
	LandsideLaneNodeInSim* pExitNode = mRoutePath[1];
	//go next state
	LandsideSimulation *pLandSim = _pEngine->GetLandsideSimulation();
	LandsideExternalNodeInSim* pDest = pLandSim->AssignDestNode(mpVehicle);
	if(pDest)
	{
		LandsideVehicleMoveInRoad* pNextState =new LandsideVehicleMoveInRoad(mpVehicle);
		if(!pNextState->SetOrignDest(pExitNode,pDest,_pEngine))
		{
			delete pNextState;			
			return NULL;
		}
		return pNextState;
	}
	else //
	{		
		return NULL;
	}	
	return this;
}
//
//
LandsideVehicleMoveInCurbside::LandsideVehicleMoveInCurbside( LandsideVehicleInSim*pVehicle, LandsideCurbSideInSim* pCurb, LandsideLaneNodeInSim* pFromNode )
:LandsideVehicleMoveInRoad(pVehicle)
{
	m_pCurbside = pCurb;
	
//	bPickedPax = false;

	//m_iLastSegIndex = 0;
	//bUnloadPickingPax = false;

	mRoutePath.push_back(pFromNode);
	LandsideLaneNodeInSim* pNextNode = m_pCurbside->GetExitNode(pFromNode);
	ASSERT(pNextNode->mpLane == pFromNode->mpLane);

	mRoutePath.push_back(pNextNode);

	UpdateTotalPath();
}

LandsideVehicleInSim* LandsideVehicleMoveInCurbside::CheckNextConflict( const ARCPolygon& poly, const ARCVector3& dir )
{
	if(LandsideVehicleInSim* pVehicle = __super::CheckNextConflict(poly,dir) )
	{
		return pVehicle;
	}

	//check end point
	if(!bReadyToLeave())
	{
		ARCVector3 endPos = *mTotalPath.rbegin();	
		ARCPolygon endPoly;
		endPoly.Build(endPos.xy(),dir.xy(), mpVehicle->GetLength(),mpVehicle->GetLength(),mpVehicle->GetWidth());
		if(endPoly.IsOverLap(poly[0],poly[1]) )
		{
			return mpVehicle;
		}
		
	}
	return NULL;
}

LandsideResourceInSim* LandsideVehicleMoveInCurbside::GetSegResource( int nSegIndex ) const
{
	if(nSegIndex< (int)mRoutePath.size() && nSegIndex >=0 )
	{
		LandsideLaneNodeInSim* pNode =mRoutePath[nSegIndex];
		return pNode->getToRes();
	}
	return NULL;
}

bool LandsideVehicleMoveInCurbside::bReadyToLeave() const
{
	if(mpVehicle->IsDepPaxVehicle())
	{
		return !mpVehicle->HaveOnPax();
	}
	else
		return mpVehicle->AllPaxGetOn();
}



void LandsideVehicleMoveInCurbside::doPickDrop( const ElapsedTime& t, CARCportEngine* _pEngine )
{
	if(mpVehicle->IsDepPaxVehicle())
	{
		mpVehicle->UnLoadOnPax(t);
	}
	else
		mpVehicle->CallPaxGetOnVehicle(t,_pEngine);		

}

