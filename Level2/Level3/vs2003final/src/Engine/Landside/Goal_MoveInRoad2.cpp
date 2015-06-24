#include "StdAfx.h"
#include ".\goal_moveinroad2.h"
#include "..\LandsideResourceManager.h"
#include "..\ARCportEngine.h"
#include "..\LandsideSimulation.h"
#include "..\CrossWalkInSim.h"
#include "../LandsideIntersectionInSim.h"
#include "../LandsideExternalNodeInSim.h"
#include "../LandsideRoundaboutInSim.h"

//algorithm 2
//Goal_MoveInRoad2::Goal_MoveInRoad2( LandsideVehicleInSim* pEnt, const LandsideLaneNodeList& path, bool bStopAtEnd, bool bChangeLane /*= false*/ )
//:LandsideVehicleGoal(pEnt)
//{
//	m_bChangeLane = bChangeLane;
//	m_bStopAtEnd = bStopAtEnd;
//	m_sDesc = "Goal_MoveInRoad";
//	m_rpath = path;
//	m_plastNode = NULL;
//	reverse_copy(path.begin(),path.end(),m_rpath.begin());
//	UpdatePathMap();
//}
//
//void Goal_MoveInRoad2::Activate( CARCportEngine* pEngine )
//{
//	//init next steps
//	/*MobileState ptinfo;
//	ptinfo.isConerPt = true;
//	ptinfo.pRes = m_pOwner->getLastState().mAtRes;
//	ptinfo.dSpeed =  m_pOwner->getLastState().dSpeed;
//
//	MovingTrace& paststeps = m_pOwner->pastSteps();
//	paststeps.Clear();
//	paststeps.AddPoint(m_pOwner->getLastState().pos, ptinfo);*/
//
//	//m_plastNode = m_rpath.back();
//	//m_rpath.pop_back();
//
//	m_iStatus = gs_active;
//}
//
//
//
//void Goal_MoveInRoad2::Process( CARCportEngine* pEngine )
//{
//	ActivateIfInactive(pEngine);
//	//check if move out of route
//	if(m_rpath.empty())
//	{
//		m_iStatus = gs_completed;
//		return;
//	}
//	//check if need change lane
//	if(ChangeLane()){ return ; }
//	//normal move in lanes
//	MoveInLane();
//}
//
//const CPath2008* Goal_MoveInRoad2::GetSegPath( LandsideLaneNodeInSim* pNodePre, LandsideLaneNodeInSim* pNodeNext, CPath2008& subPath ) const
//{	
//
//	LandsideResourceInSim* pResource = NULL;
//	//get resource and next path
//	if(pNodePre && pNodeNext )
//	{
//		//ASSERT(pNodePre->getToRes() == pNodeNext->getFromRes());
//
//		pResource = pNodePre->getToRes();
//		if(LandsideLaneInSim* pLane = pResource->toLane()) // in lane
//		{			
//			if(pNodePre->mpLane!=pNodeNext->mpLane)
//			{
//				subPath.push_back(pNodePre->m_pos);
//				subPath.push_back(pNodeNext->m_pos);
//				return &subPath;
//			}
//			else
//			{
//				//const ARCPath3& respath = pLane->getPath();
//				subPath = pLane->GetSubPathDist(pNodePre->m_distInlane, pNodeNext->m_distInlane);
//				return &subPath;
//			}
//		}		
//		else if(LandsideIntersectionInSim* pIntersect = pResource->toIntersection() ) //intersection node
//		{
//			LandsideLaneExit* pExit = pNodePre->toExit();
//			LandsideLaneEntry* pEntry = pNodeNext->toEntry();
//			if(pEntry && pExit)
//			{
//				LandsideIntersectLaneLinkInSim* pLink = pIntersect->GetLinkage(pExit,pEntry);
//				if(pLink)
//				{
//					const CPath2008& respath = pLink->m_path;				
//					return &respath;							
//				}
//			}
//		}		
//		else if(LandsideCurbSideInSim* pCurb  = pResource->toCurbSide())
//		{
//			ASSERT(pNodePre->mpLane == pNodeNext->mpLane);
//			//const ARCPath3& respath = pNodePre->mpLane->getPath();
//
//			subPath = pLane->GetSubPathDist( pNodePre->m_distInlane, pNodeNext->m_distInlane );
//			return &subPath;	
//		}
//		else if(LandsideRoundaboutInSim* pAbout = pResource->toAbout() )
//		{
//			if( pAbout->GetLinkPath(pNodePre,pNodeNext,subPath) )
//			{
//				return &subPath;	
//			}
//			ASSERT(FALSE);
//		}
//		else
//		{
//			ASSERT(FALSE);
//		}
//	}
//	else if(pNodePre)
//	{
//		pResource = pNodePre->getToRes();
//		if(LandsideCurbSideInSim* pCurb = pResource->toCurbSide())
//		{
//			subPath = pNodePre->mpLane->GetPathLeft(pNodePre->m_distInlane);
//			return &subPath;
//		}
//	}
//	ASSERT(FALSE);
//	return NULL;
//}
//
//const CPath2008* Goal_MoveInRoad2::GetSegPath( LandsideLaneNodeInSim* pNode1, LandsideLaneNodeInSim* pNode2 ) const
//{
//	PathMap::const_iterator itr =  m_pathMap.find( NodePair(pNode1,pNode2) );
//	if(itr!=m_pathMap.end())
//	{
//		return &(itr->second);
//	}
//	return NULL;
//}
//
//
//
//void Goal_MoveInRoad2::GetNextSteps( const MobileState& lastState,DistanceUnit dRadS ,MovingTrace& nextsteps)const
//{
//	if(m_rpath.empty())
//		return;
//	
//	//add future end point first ;
//	LandsideLaneNodeInSim* pPreNode = m_plastNode;
//	LandsideLaneNodeList rpath = m_rpath;
//
//	MobileState state = lastState;
//	state.pathIndex = 0;
//	nextsteps.AddPoint(state);
//
//	int pathindex = 0;
//	while(!rpath.empty())
//	{			
//		//
//		LandsideLaneNodeInSim* pNextNode = rpath.back();
//
//		if(pPreNode && pNextNode)
//		{			
//			const CPath2008* ppath =  GetSegPath(pPreNode,pNextNode);
//			if(ppath && ppath->getCount()>0){
//				int iStart = 0;
//				if(pPreNode == m_plastNode){
//					iStart = (int)(ppath->GetPointIndex(lastState.pos)+1);					
//				}
//
//				for (; iStart < ppath->getCount()-1; iStart++)
//				{
//					state.pos = ppath->getPoint(iStart);
//					state.isConerPt = true;
//					state.mAtRes = pNextNode->mpLane;
//					state.pathIndex = pathindex;
//					state.distInRes = pPreNode->mpLane->GetPointDist(state.pos);
//
//					nextsteps.AddPoint(state);			
//					if(nextsteps.GetLength()>=dRadS){
//						return ;
//					}
//				}			
//
//				//add end point
//				state.pos = ppath->getPoint( ppath->getCount()-1 );
//				state.isConerPt = true;
//				state.mAtRes = pNextNode->mpLane;
//				state.pathIndex = pathindex+1;
//				state.distInRes = pPreNode->mpLane->GetPointDist(state.pos);
//
//				nextsteps.AddPoint(state);	
//				if(nextsteps.GetLength()>=dRadS){
//					return ;
//				}
//			}			
//		}		
//		rpath.pop_back(); 
//		pPreNode = pNextNode;	
//		pathindex++;
//	}
//	
//}
//
//void Goal_MoveInRoad2::MoveInLane()
//{
//	MobileState curState = m_pOwner->getLastState();
//	double dMaxSpd = m_pOwner->getSpeed(curState.getLandsideRes(),curState.distInRes);
//	double dCurSpd = m_pOwner->getLastState().dSpeed; 
//	double dt = .2;//t in seconds
//
//
// 
//	//5 options
//	double dAccList[5];
//	double dSpdList[5];
//	double dOffsetList[5];
//	OpMoveType dMoveStates[5];
//	dAccList[0] = -m_pOwner->mPerform.mDecSpd;   	dMoveStates[0] = _MAXBREAK;//max brake
//	dAccList[1] = -0.2*m_pOwner->mPerform.mDecSpd;   dMoveStates[1]= _SLOWDOWN;//slow down
//	dAccList[2] = 0 ; dMoveStates[2]=_UNIFORM; //uniform
//	dAccList[3] = 0.2*m_pOwner->mPerform.mAccSpd; dMoveStates[3]=_SLOWACC;//slow acc
//	dAccList[4] = m_pOwner->mPerform.mAccSpd;  dMoveStates[4]=_MAXACC;//max acc
//	for(int i=0;i<5;i++){
//		double dNextSpeed = dCurSpd + dt*dAccList[i];
//		dNextSpeed = MIN(dNextSpeed,dMaxSpd);
//		dNextSpeed = MAX(0,dNextSpeed);
//		dSpdList[i] = dNextSpeed;
//		dOffsetList[i] = (dNextSpeed+dCurSpd)*0.5*dt;
//	}
//	
//	
//	MovingTrace  mvpath;
//	GetNextSteps(m_pOwner->getLastState(),dOffsetList[4]+500,mvpath);
//	
//	double dS = 0;
//	double dEndSpd = 0;
//
//	StateInfo thisMoveState;
//	thisMoveState.mBeginTime = m_pOwner->curTime();
//	thisMoveState.mState =  _STOP;
//
//	for(int i=0;i<5;++i){
//		double dNextSpeed = dSpdList[i];
//		double dOffsetS = dOffsetList[i];
//
//		if(dOffsetS >0){
//			thisMoveState.pConflictVehicle = CheckVehicleConflict(mvpath, dOffsetS,dNextSpeed);
//			thisMoveState.pConflictPax = CheckPaxConflict(mvpath, dOffsetS,dNextSpeed);
//			thisMoveState.pConflictCross = CheckCrossWalkConflict(curState,dEndSpd,dOffsetS);
//
//			if(thisMoveState.isConflict() )
//			{
//				break;
//			}
//			else
//			{
//				dS = dOffsetS;
//				dEndSpd = dNextSpeed;
//				thisMoveState.mState = dMoveStates[i];
//			}
//		}
//	}
//
//	
//
//	//
//	if(dS>0){
//		m_pOwner->StartMove(m_pOwner->curTime());
//		MovingTrace mv1,mv2;
//		mvpath.Split(dS,mv1,mv2);
//		//
//		mv1.CalculateTime(m_pOwner->curTime(),m_pOwner->curTime()+ ElapsedTime(dt), curState.dSpeed, dEndSpd);
//
//		int ptIdx = 0;
//		for(int i=1;i<mv1.GetCount();i++){
//			m_pOwner->UpdateState(mv1.at(i));
//			ptIdx = mv1.at(i).pathIndex;
//		}
//		popPathNodes(ptIdx);
//		
//	}
//	else{ //wait for the seconds;
//
//		m_pOwner->UpdateState(curState);
//		curState.time += ElapsedTime(dt);
//		m_pOwner->UpdateState(curState);
//	}
//
//	m_lastStateInfo = thisMoveState;
//}
//
//
//CCrossWalkInSim* Goal_MoveInRoad2::CheckCrossWalkConflict(const MobileState&lastState,  double dNextSpd, double dOffsetS)
//{	
//	double dVehHalfLen = m_pOwner->GetLength()*0.5;
//	double dendDecDist = m_pOwner->mPerform.minDecDist(dNextSpd);
//	//dendDecDist = dendDecDist+20;
//
//	LandsideResourceInSim* pres = GetSegPathResource(m_plastNode, m_rpath.back() );
//	if(pres)
//	{
//		if(LandsideLaneInSim* plane = pres->toLane())
//		{
//			double dCrossDist = 0;
//			if(LandsideStretchLaneInSim* pStretchlane  = plane->toStretchLane())
//			{
//				DistanceUnit distInSegRes = pStretchlane->GetPointDist(lastState.pos)  ;
//				if(CCrossWalkInSim*pCross = pStretchlane->GetAheadCross(m_pOwner, distInSegRes,dCrossDist) )
//				{
//					if( (dOffsetS + dendDecDist+dVehHalfLen+pCross->getHalfWidth()) > (dCrossDist-distInSegRes) )
//					{
//						if(!pCross->CanVehicleCross(lastState.time))
//							return pCross;
//					}
//				}
//			}
//
//		}
//	}
//	return NULL;
//}
//
//LandsideResourceInSim* Goal_MoveInRoad2::GetSegPathResource( LandsideLaneNodeInSim* pNodePre, LandsideLaneNodeInSim* pNodeNext ) const
//{
//	LandsideResourceInSim* pResource = NULL;
//	//get resource and next path
//	if(pNodePre && pNodeNext )
//	{
//		//ASSERT(pNodePre->getToRes() == pNodeNext->getFromRes());
//
//		pResource = pNodePre->getToRes();
//		if(LandsideLaneInSim* pLane = pResource->toLane()) // in lane
//		{			
//			return pLane;				
//		}		
//		else if(LandsideIntersectionInSim* pIntersect = pResource->toIntersection() ) //intersection node
//		{
//			LandsideLaneExit* pExit = pNodePre->toExit();
//			LandsideLaneEntry* pEntry = pNodeNext->toEntry();
//			if(pEntry && pExit)
//			{
//				LandsideIntersectLaneLinkInSim* pLink = pIntersect->GetLinkage(pExit,pEntry);
//				if(pLink)
//				{
//					return pLink;					
//				}
//			}
//		}		
//		else if(LandsideCurbSideInSim* pCurb  = pResource->toCurbSide())
//		{
//			ASSERT(pNodePre->mpLane == pNodeNext->mpLane);
//			return pNodePre->mpLane;			
//		}
//	}
//	else if(pNodePre)
//	{
//		pResource = pNodePre->getToRes();
//		if(LandsideCurbSideInSim* pCurb = pResource->toCurbSide())
//		{
//			return pNodePre->mpLane;			
//		}
//	}
//	return NULL;
//}
//
//LandsideVehicleInSim* Goal_MoveInRoad2::CheckVehicleConflict(const MovingTrace& nextpath, double dOffsetS, double dNextSpd )
//{
//	if(nextpath.IsEmpty())
//		return NULL;
//
//	double dendDecDist = m_pOwner->mPerform.minDecDist(dNextSpd);
//	dendDecDist = dendDecDist+100;
//	double halfLen = m_pOwner->GetLength()*0.5;	
//
//	
//	MobileState nextPtInfo;
//	nextpath.GetDistPos(dOffsetS,nextPtInfo);
//
//	LandsideResourceInSim* pNextRes = nextPtInfo.getLandsideRes();
//	LandsideResourceInSim* pCurRes = m_pOwner->getLastState().getLandsideRes();
//	bool bIgnoreCurrent = false;
//	
//	//if(pCurRes &&  pCurRes == pNextRes) //move in the same lane
//	//{
//	//	if(LandsideLaneInSim* pLane = pCurRes->toLane())
//	//	{
//	//		double distInSegRes = m_pOwner->getLastState().distInRes;
//	//		LandsideVehicleInSim* pHeadV = pLane->GetVehicleAhead(m_pOwner);
//	//		if(pHeadV)
//	//		{
//	//			double distHead = pHeadV->getLastState().distInRes;
//	//			if(distHead-distInSegRes < dOffsetS + dendDecDist + halfLen + pHeadV->GetLength()*0.5)
//	//			{
//	//				if(m_pOwner->bCanWaitFor(pHeadV))
//	//					return pHeadV;
//	//			}
//	//		}	
//	//		bIgnoreCurrent = true;			
//	//	}
//	//}
//	//
//	ARCPolygon nextPoly;
//	nextPoly.Build(nextPtInfo.pos,nextPtInfo.dir, halfLen+dendDecDist,dOffsetS-halfLen,m_pOwner->GetWidth());
//
//	
//	
//	if(!bIgnoreCurrent)
//	{
//		if(LandsideVehicleInSim* pConflict = GetConflictVehicle(nextPoly,pCurRes)){
//			return pConflict;
//		}
//	}
//	LandsideResourceInSim* pRes = pCurRes;	
//	for(int i=0;i<nextpath.GetCount();i++)
//	{
//		const MobileState& s = nextpath.at(i);
//		if(LandsideResourceInSim* pResNext = s.getLandsideRes() )
//		{
//			if(pRes != pResNext){
//				if(LandsideVehicleInSim* pConflict = GetConflictVehicle(nextPoly,pRes)){
//					return pConflict;
//				}
//			}			
//			pRes = pResNext;
//		}
//	}
//	
//	return NULL;
//}
//
//LandsideVehicleInSim * Goal_MoveInRoad2::GetConflictVehicle( const ARCPolygon& poly,LandsideResourceInSim* pRes )
//{
//	LandsideVehicleInSim* pConflictVehcile = NULL;
//	for(int i=0;i<pRes->GetInResVehicleCount();i++)
//	{
//		LandsideVehicleInSim* pOtherVeh = pRes->GetInResVehicle(i);
//		if(m_pOwner == pOtherVeh)
//			continue;
//		if( !m_pOwner->bCanWaitFor(pOtherVeh) )
//			continue;
//
//		if( poly.IsOverlapWithOtherPolygon(pOtherVeh->getBBox(m_pOwner->curTime())) )
//			return pOtherVeh;
//
//	}
//	return NULL;
//}
//
//PaxLandsideBehavior* Goal_MoveInRoad2::CheckPaxConflict( const MovingTrace& nextpath,double dOffsetS,double dNextSpd )
//{
//
//	return NULL;
//}
//
//void Goal_MoveInRoad2::popPathNodes( int nNum )
//{
//	for(int i=0;i<nNum;++i)
//	{
//		m_plastNode = m_rpath.back();
//		m_rpath.pop_back();
//	}
//}
//
//void Goal_MoveInRoad2::UpdatePathMap()
//{
//	LandsideLaneNodeList rpath = m_rpath;
//	if(rpath.empty())
//		return;
//	LandsideLaneNodeInSim* preNode = rpath.back();
//	rpath.pop_back();
//	while(!rpath.empty()){
//		LandsideLaneNodeInSim* nextNode = rpath.back();
//		CPath2008& path = m_pathMap[NodePair(preNode,nextNode)];
//		const CPath2008* retpath = GetSegPath(preNode,nextNode,path);
//		if(&path!=retpath && retpath)
//			path = *retpath;
//
//		preNode = nextNode;
//		rpath.pop_back();
//	}
//}
