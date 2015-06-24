#include "stdafx.h"


#include "MobileRouteInSim.h"

#include ".\DynamicConflictGraph.h"
#include ".\GroundIntersection.h"
RouteDirPathList getPathList(const MobileRouteInSim& route){
	RouteDirPathList ret;
	ret.reserve(route.GetItemCount());
	for(int i=0;i<route.GetItemCount();++i)
	{
		ret.push_back(route.ItemAt(i).GetPath());
	}
	return ret;
}

boost::tuple<ARCMobileElement*,RouteDirPath* > MobileRouteInSim::bCanCrossTheIntersection( IntersectionInRoute* pHold ) const
{
	if(pHold)
	{
		if( !pHold->m_pNode->IsAvailable(mpMobile) )
		{			
			return  boost::make_tuple(pHold->m_pNode->getInMobile(),(RouteDirPath*)NULL);
		}
		
		int curItemidx;DistanceUnit curItemDist;
		int nextItemidx;DistanceUnit nextItemDist;
		boost::tie(curItemidx,curItemDist) = GetItemIndexAndDist(pHold->m_dEntryDistInRoute);
		boost::tie(nextItemidx,nextItemDist) = GetItemIndexAndDist(pHold->m_dExitDistInRoute);
		for(int i=min(GetItemCount()-1,curItemidx+1);i<=min(GetItemCount()-1,nextItemidx);++i)
		{
			ARCMobileElement* pMob;
			if(pMob = DynamicConflictGraph::getInstance().bMobileForbidTo(mpMobile,ItemAt(i).GetPath()) )
			{
				return boost::make_tuple(pMob,ItemAt(i).GetPath());
			}
		}				
	}
	return boost::make_tuple((ARCMobileElement*)NULL,(RouteDirPath*)NULL);
}

void MobileRouteInSim::Init( ARCMobileElement* pMobile ,const ElapsedTime&t)
{
	if(mpMobile!=pMobile)
	{
		mpMobile = pMobile;
		updateCachedata();
		DynamicConflictGraph::getInstance().OnMobilePlaneRoute(mpMobile,getPathList(*this),t);
	}
}

boost::tuple<ARCMobileElement*, DistanceUnit> MobileRouteInSim::getLeadMobile( DistanceUnit distInRoute ) const
{
	int nBeginItemIdx;
	DistanceUnit dDistInItem;
	boost::tie(nBeginItemIdx,dDistInItem) = GetItemIndexAndDist(distInRoute);

	//continue searching next route items
	for(int i=max(0,nBeginItemIdx);i<GetItemCount();i++)
	{		
		RouteDirPath* rDPath = ItemAt(i).GetPath();
		ARCMobileElement* lMob = rDPath->getLeadMobile(mpMobile);
		if(lMob && lMob->getCurDirPath()==rDPath) //find the mob
		{
			DistanceUnit distInRoute = GetDistInRoute(i,lMob->getCurDistInDirPath());
			return boost::make_tuple(lMob,distInRoute);
		}
		
	}
	ARCMobileElement* pLead = NULL;
	DistanceUnit dist = 0;
	return boost::make_tuple(pLead,dist);

}

boost::tuple<DistanceUnit,ARCMobileElement*> MobileRouteInSim::checkConflictWithLeadMobile( DistanceUnit mCurDistInRoute,DistanceUnit dRadOfConcern )
{
	//find the lead mobile element
	ARCMobileElement* pLeadMobile=NULL;DistanceUnit leadMobDist;

	boost::tie(pLeadMobile,leadMobDist) = getLeadMobile(mCurDistInRoute);

	if(pLeadMobile ) //can see a flight ahead
	{		

		DistanceUnit dSeperation  = pLeadMobile->getCurSeparation() + pLeadMobile->GetLength()*0.5 + mpMobile->GetLength()*0.5;

		double dNormalSpd = mpMobile->getMobilePerform().getNormalSpd();
		DistanceUnit dSafeDist = leadMobDist - dSeperation;

		double dLeadSpd = min(pLeadMobile->getCurSpeed(),dNormalSpd );
		DistanceUnit dSpdChageDist = mpMobile->getCurSpdChangeDist(dLeadSpd);


		IntersectionInRoute* pHWaitHold = mIntersectionList.IsDistForbidParking(dSafeDist);
		if(pHWaitHold)
			dSafeDist = pHWaitHold->m_dEntryDistInRoute;

		if(mCurDistInRoute + dSpdChageDist < dSafeDist)
		{
			return boost::make_tuple( dSafeDist-dSpdChageDist, dLeadSpd , (ARCMobileElement*)NULL);
		}
		else if(mCurDistInRoute < dSafeDist)
		{
			return boost::make_tuple( dSafeDist, 0, (ARCMobileElement*)NULL );
		}
		else
		{
			return boost::make_tuple( mCurDistInRoute, 0, pLeadMobile);
		}			
	}

	//after all try to use normal speed
	return boost::make_tuple( GetEndDist(),mpMobile->getMobilePerform().getNormalSpd(),(ARCMobileElement*)NULL);
}

boost::tuple<DistanceUnit,double, ARCMobileElement*, IntersectionInRoute*, RouteDirPath* > MobileRouteInSim::checkConflictWithNextHold(  DistanceUnit mCurDistInRoute )
{
	double dMoveSpd =  mpMobile->getMobilePerform().getNormalSpd();
	DistanceUnit dDecDistToStatic = mpMobile->getMobilePerform().getSpdChangeDist(dMoveSpd,0); //dist to stop

	std::vector<IntersectionInRoute*> vNodeHoldList= mIntersectionList.GetNextIntersetions(mCurDistInRoute);


	for(int idxHD=0;idxHD<(int)vNodeHoldList.size();++idxHD)
	{
		IntersectionInRoute* pNextHold = vNodeHoldList.at(idxHD);
		if(pNextHold)
		{
			DistanceUnit distCheckHold = pNextHold->m_dEntryDistInRoute - dDecDistToStatic;
			if( int(mCurDistInRoute) < int(distCheckHold) ) //flight can move to the checkpoint
			{
				if( !mIntersectionList.IsDistForbidParking(distCheckHold) )
					return boost::make_tuple(distCheckHold, dMoveSpd,(ARCMobileElement*)NULL, pNextHold, (RouteDirPath*)NULL);	
			}

			ARCMobileElement *pConflictMobile;
			RouteDirPath* pPath;
			boost::tie(pConflictMobile,pPath) = bCanCrossTheIntersection(pNextHold);
			if(pConflictMobile)
			{
				DistanceUnit nextDist = pNextHold->m_dEntryDistInRoute;
				IntersectionInRoute* pWaitHold = mIntersectionList.IsDistForbidParking(nextDist);
				if(pWaitHold)
					nextDist = pWaitHold->m_dEntryDistInRoute;
				if( int(mCurDistInRoute) < int(nextDist) )
				{
					return  boost::make_tuple(nextDist,0,pConflictMobile,pNextHold,pPath);
				}
				else
				{
					return boost::make_tuple(mCurDistInRoute,0,pConflictMobile,pNextHold,pPath);
				}
			}

		}
	}	

	return boost::make_tuple( GetEndDist(),dMoveSpd,(ARCMobileElement*)NULL,(IntersectionInRoute*)NULL,(RouteDirPath*)0);
}



void MobileRouteInSim::MobileMove( const DistanceUnit& distF, const DistanceUnit& distT, const ElapsedTime& tF,const double& spdF,const double&  spdT )
{
	//no movement
	if(distT == distF)
		return;

	DistanceUnit dDist = distT - distF;
	double dAvgSpd = (spdF+spdT)*0.5;
	ElapsedTime dTAll = (dDist)/dAvgSpd;
	dTAll = max(ElapsedTime(1L),dTAll);
	
	//enter intersections
	IntersectionInRoute* pIntersectFar = NULL;
	std::vector<IntersectionInRoute*> vEntryIntersections = mIntersectionList.GetEntryIntersections(distF,distT);
	for(int i=0;i<(int)vEntryIntersections.size();++i)
	{
		IntersectionInRoute* pIntersection = vEntryIntersections.at(i);
		if(!pIntersectFar){
			pIntersectFar = pIntersection;
		}
		else if(pIntersectFar->m_dExitDistInRoute < pIntersection->m_dExitDistInRoute)
		{
			pIntersectFar = pIntersection;
		}
		double dTravelDist = pIntersection->m_dEntryDistInRoute - distF;
		double dRatio = getTimeBetweenRat( dTravelDist/dDist,spdF,spdT );
		ElapsedTime atTime = tF + ElapsedTime((double)dTAll*dRatio);
		MobileEnter(pIntersection,atTime);
	}
	//exit intersections
	std::vector<IntersectionInRoute*> vExitIntesections = mIntersectionList.GetExitIntersections(distF,distT);
	for(int i=0;i<(int)vExitIntesections.size();++i)
	{
		IntersectionInRoute* pIntersection = vEntryIntersections.at(i);
		double dTravelDist = pIntersection->m_dExitDistInRoute - distF;
		double dRatio = getTimeBetweenRat( dTravelDist/dDist,spdF,  spdT );
		ElapsedTime atTime = tF + ElapsedTime((double)dTAll*dRatio);
		MobileExit(pIntersection,atTime);
	}

	//
	//enter exit dir path
	int nCurItemIdx;DistanceUnit curDistInItem;
	boost::tie(nCurItemIdx,curDistInItem) = GetItemIndexAndDist(distF);
	int nNextItemIdx; DistanceUnit nextDistInItem;
	boost::tie(nNextItemIdx,nextDistInItem) = GetItemIndexAndDist(distT);
	//at the same time enter the edge to the next hold
	int nNextNodeItemidx;DistanceUnit nexNodeItemDist;
	if(pIntersectFar)
		boost::tie(nNextNodeItemidx,nexNodeItemDist) = GetItemIndexAndDist(pIntersectFar->m_dExitDistInRoute);
	else
		nexNodeItemDist = nNextItemIdx;

	//enter items	
	for(int i=max(0,nCurItemIdx);i<= min( nexNodeItemDist,GetItemCount()-1);i++)
	{
		DistanceUnit dTravlDist = GetItemBeginDist(i) - distF;
		double dR = getTimeBetweenRat(dTravlDist/dDist, spdF,spdT);
		ElapsedTime atTime =  tF + ElapsedTime( (double)dTAll * dR);
		//exit preItem		
		DynamicConflictGraph::getInstance().OnMobileEnter(mpMobile,ItemAt(i).GetPath(),atTime);	
	}	
	//exit items
	for(int i=max(1,nCurItemIdx+1);i<= min( nNextItemIdx,GetItemCount()-1);i++)
	{
		DistanceUnit dTravlDist = GetItemBeginDist(i) - distF;
		double dR = getTimeBetweenRat(dTravlDist/dDist, spdF,spdT);
		ElapsedTime atTime =  tF + ElapsedTime( (double)dTAll * dR);
		//exit preItem		
		DynamicConflictGraph::getInstance().OnMobileLeave(mpMobile,ItemAt(i-1).GetPath(),atTime);
	}		


}


void MobileRouteInSim::MobileExitRoute( const ElapsedTime& t )
{
	//exit from the intersections
	for(int i=0;i<mIntersectionList.GetCount();++i)
	{
		if( mIntersectionList.ItemAt(i)->IsMobileIn(mpMobile) )
		{
			MobileExit(mIntersectionList.ItemAt(i),t);
		}
	}
	
	//exit the route
	for(int i=0;i< GetItemCount();i++)
	{
		RouteDirPath* pPath = ItemAt(i).GetPath();	
		if(pPath->bMobileInPath(mpMobile))
            DynamicConflictGraph::getInstance().OnMobileLeave(mpMobile,pPath,t);
	}
	DynamicConflictGraph::getInstance().OnMobileUnPlanRoute(mpMobile,t);    

}

void MobileRouteInSim::MobileEnter( IntersectionInRoute* pIntersection,const ElapsedTime&t )
{
	if(pIntersection && pIntersection->m_pNode)
		pIntersection->m_pNode->setLockMobile(mpMobile);
}

//mobile exit intersection
#include "Airsideevent.h"
class MobileExitIntersectionEvent: public AirsideEvent
{
public:
	MobileExitIntersectionEvent(ARCMobileElement* pmob, GroundIntersectionInSim* pNode, const ElapsedTime& t){
		mpFlight = pmob;
		mpNode = pNode;
		setTime(t);
	}
	//It returns event's name
	virtual const char *getTypeName (void) const { return _T("Release Node"); };

	//It returns event type
	virtual int getEventType (void) const { return -1; }

	int Process()
	{ 
		mpNode->setLockMobile(NULL);
		mpNode->NotifyOtherAgents(SimMessage().setTime(getTime()));
		return 1; 
	}


protected:
	ARCMobileElement* mpFlight;
	GroundIntersectionInSim* mpNode;

};
void MobileRouteInSim::MobileExit( IntersectionInRoute* pIntersection,const ElapsedTime& t )
{
	MobileExitIntersectionEvent* pExitNode = new MobileExitIntersectionEvent(mpMobile,pIntersection->m_pNode,t);
	pExitNode->addEvent();
}

void MobileRouteInSim::updateCachedata()
{
//get cache item length
	mvCacheItemLength.resize(GetItemCount()+1);
	mvCacheItemLength[0]=0;
	double dLen = 0;
	for(int i=0;i<GetItemCount();++i)
	{
		dLen+= ItemAt(i).GetDistTo()-ItemAt(i).GetDistFrom();
		mvCacheItemLength[i+1] = dLen;
	}
}

void MobileRouteInSim::MobileEnterRoute( const ElapsedTime& t )
{
	
}

boost::tuple<int,DistanceUnit> MobileRouteInSim::GetItemIndexAndDist( const DistanceUnit& dist ) const
{
	ASSERT( mvCacheItemLength.size() == GetItemCount()+1 );

	if(dist < * mvCacheItemLength.begin()){
		return boost::make_tuple(-1,dist - *mvCacheItemLength.begin() );
	}	

	for(int i=0;i< (int)mvCacheItemLength.size()-1;i++)
	{
		if( dist < mvCacheItemLength[i+1] && dist >= mvCacheItemLength[i] )
		{
			int nItemIdx = i;
			DistanceUnit distInItem = dist - mvCacheItemLength[i];
			return boost::make_tuple(nItemIdx,distInItem);
		}
	}	

	int nItemIdx = GetItemCount();
	DistanceUnit distInItem = dist - *mvCacheItemLength.rbegin();
	return boost::make_tuple(nItemIdx,distInItem);
}

DistanceUnit MobileRouteInSim::GetDistInRoute( const int& nItemIdx , const DistanceUnit& distInItem ) const
{
	ASSERT(mvCacheItemLength.size() == GetItemCount()+1);
	if(nItemIdx < 0){
		return -1;
	}
	if(nItemIdx==0)
		return distInItem;

	if(nItemIdx >= GetItemCount() )
	{
		return *mvCacheItemLength.rbegin() + distInItem; 
	}

	return mvCacheItemLength[nItemIdx] + distInItem;
}

bool IntersectionInRoute::IsMobileIn(ARCMobileElement* pmob) const
{
	return m_pNode->getInMobile() == pmob;
}

bool IntersectionInRoute::IsRuwnayIntersection() const
{
	ASSERT(false);
	return false;
}
