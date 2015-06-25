#include "StdAfx.h"
#include ".\DynamicConflictGraph.h"
#include ".\IntersectionNodeInSim.h"
#include "..\..\Common\FileOutPut.h"
#include "Airsideevent.h"
#include "AirsideFlightInSim.h"
#include ".\FlightGroundRouteResourceManager.h"




//class MobileGetConflictionLockEvent : public AirsideEvent
//{
//public:
//	MobileGetConflictionLockEvent(ARCMobileElement* pmob, RouteDirPath* pNextPath,const ElapsedTime& t)
//	{
//		mpMob = pmob;
//		mpPath = pNextPath;
//		setTime(t);
//	}
//	//It returns event's name
//	virtual const char *getTypeName (void) const { return _T("GetMobileConflictLock"); };
//
//	//It returns event type
//	virtual int getEventType (void) const { return -1; }
//
//
//	int Process(){ DynamicConflictGraph::getInstance().setMobileGetTheConflictLock(mpMob,mpPath); return 1; }
//protected:
//	ARCMobileElement* mpMob;
//	RouteDirPath* mpPath;
//
//};
//
//
//
class FlightExitNodeEvent: public AirsideEvent
{
public:
	FlightExitNodeEvent(AirsideFlightInSim* pFlit, IntersectionNodeInSim* pNode, const ElapsedTime& t){
		mpFlight = pFlit;
		mpNode = pNode;
		setTime(t);
	}
	//It returns event's name
	virtual const char *getTypeName (void) const { return _T("Release Node"); };

	//It returns event type
	virtual int getEventType (void) const { return -1; }


	int Process(){ mpNode->OnFlightExit(mpFlight,getTime());; return 1; }


protected:
	AirsideFlightInSim* mpFlight;
	IntersectionNodeInSim* mpNode;
	
};

void DynamicConflictGraph::OnFlightExitNode( AirsideFlightInSim* pFlt, IntersectionNodeInSim* pNode ,const ElapsedTime& t)
{
	FlightExitNodeEvent* pExitNode = new FlightExitNodeEvent(pFlt,pNode,t);
	pExitNode->addEvent();
}

RouteDirPath* bConflictInRoute(ARCMobileElement* pmob,ARCMobileElement* ptheMob,RouteDirPath* pPath, const RouteDirPathList& route)
{
	for(int i=0;i<(int)route.size();i++)
	{
		if(pPath->IsDirConflictWith(route.at(i)) || pPath->adjacencyNegativeConflict(route.at(i),pmob,ptheMob))
			return route.at(i);
	}
	return NULL;
}

//#include <FileOutPut.h>
// mobile A , mobile b will conflict at these Resource, let which one go first
class RouteResource;
class MobileConfliction
{
	friend class DynamicConflictGraph;
public:
	MobileConfliction(ARCMobileElement* pMobA, ARCMobileElement* pMobB, const RouteDirPathList& vMobARoute, const RouteDirPathList& vMobBRoute){ 
		mpMobileConflictA=pMobA;
		mpMobileConflictB = pMobB;
		mpGetResourceMob = NULL;
		mvMobARoute = vMobARoute;//mob A's sequence route
		mvMobBRoute = vMobBRoute;

	}

	void SetGetResMob(ARCMobileElement* pmob)
	{ 
		ASSERT(mpGetResourceMob== NULL);
        mpGetResourceMob = pmob;
	}

	bool IsMobileConfliction(ARCMobileElement* pmob,RouteDirPath* pPath)const; //one of the mobile is the pmob
	bool IsMobileConfliction(ARCMobileElement* pmob)const; //one of the mobile is the pmob
	bool IsMobileConfliction(ARCMobileElement* pmob, IntersectionNodeInSim* pNode)const
	{
		if(mpMobileConflictA == pmob)
		{
			for(std::vector<RouteDirPath*>::const_iterator itr = mvMobARoute.begin();itr!=mvMobARoute.end();++itr)
			{
				RouteDirPath* ptPath= *itr;
				FlightGroundRouteDirectSegInSim* pDirSeg = (FlightGroundRouteDirectSegInSim*)ptPath;
				if(pDirSeg->GetEntryNode() == pNode )
					return true;
				if(pDirSeg->GetExitNode() == pNode)
					return true;
			}		
		}
		else if(mpMobileConflictB == pmob)
		{
			for(std::vector<RouteDirPath*>::const_iterator itr = mvMobBRoute.begin();itr!=mvMobBRoute.end();++itr)
			{
				RouteDirPath* ptPath= *itr;
				FlightGroundRouteDirectSegInSim* pDirSeg = (FlightGroundRouteDirectSegInSim*)ptPath;
				if(pDirSeg->GetEntryNode() == pNode )
					return true;
				if(pDirSeg->GetExitNode() == pNode)
					return true;
			}		
		}
		return false;
	}

	ARCMobileElement* getOtherMobile(ARCMobileElement* pmob)const;

	RouteDirPath* getEndPath(ARCMobileElement* pMob)const;
	//
	//bool bHasDirPath(RouteDirPath* pPath)const;
	//bool bHasNegDirPath(RouteDirPath* pPath)const;
	//is the mobile in the resources
	//bool bMoibleInResources(ARCMobileElement* pMob)const;

	CString Print()const;
protected:
    ARCMobileElement* mpGetResourceMob; //the mobile which get this ResourceRoute
	std::vector<RouteDirPath*> mvMobARoute;
	std::vector<RouteDirPath*> mvMobBRoute;
	ARCMobileElement* mpMobileConflictA;
	ARCMobileElement* mpMobileConflictB;

};


//
void LogMobilePlanRoute(ARCMobileElement* pmob, const RouteDirPathList& vPathList,const ElapsedTime& t )
{
	/*if(!pmob->IsKindof(typeof(AirsideFlightInSim))){
		return ;
	}*/


	AirsideFlightInSim* pFlt= (AirsideFlightInSim* )pmob;
	const static CString strFile = _T("c:\\taxiconflict\\routeassignLog.txt");
	CFileOutput filtout(strFile);
	CString strOut;

	CString strPath;
	for(int i=0;i<(int)vPathList.size();i++)
	{
		RouteDirPath* pPath = vPathList[i];
		if(pPath && pPath->IsKindof(typeof(FlightGroundRouteDirectSegInSim)))
		{
			FlightGroundRouteDirectSegInSim* pSeg = (FlightGroundRouteDirectSegInSim*)pPath;
			CString strTmp;
			strTmp.Format("(%d,%d)",pSeg->GetEntryNode()->GetID(),pSeg->GetExitNode()->GetID());
			strPath += strTmp;
		}
	}
	strOut.Format(_T("%s plan route %s at %s"),pFlt->GetCurrentFlightID().GetString(),strPath.GetString(),t.printTime().GetString() );
	filtout.LogLine(strOut);
};



//////////////////////////////////////////////////////////////////////////
void DynamicConflictGraph::OnMobilePlaneRoute( ARCMobileElement* pmob, const RouteDirPathList& vPathList,const ElapsedTime& t,bool bGetRes )
{
	//un plan old Route 
	OnMobileUnPlanRoute(pmob,t);

	//notifyTempFlights(vPathList,t);

	//add Registry of the mobile 
//	mMobileRouteMap[pmob] = vPathList;
	mMobileRouteMap.push_back(std::make_pair(pmob,vPathList));

	{//print route
		//LogMobilePlanRoute(pmob,vPathList,t);		
	}


	//update conflict resource map
	for(std::vector<MobPairRoute>::iterator itr = mMobileRouteMap.begin();itr!=mMobileRouteMap.end();++itr)
	{
		ARCMobileElement* pOtherMob = itr->first;
		if(pOtherMob==pmob)continue;
		//find sequence conflict resources
		RouteDirPathList& otherMobRoute = itr->second;
		
		RouteDirPathList vSeqRes;		
		RouteDirPathList vOtherSeqRes;
		bool bOtherMobileInres = false;
		bool bMobileInRes = false;

		for(int i=0;i<(int)vPathList.size();i++)
		{
			RouteDirPath* pPath = vPathList.at(i);

			if( RouteDirPath* pConflictPath = bConflictInRoute(pmob,pOtherMob,pPath,otherMobRoute) ) //add to buffer list
			{
				vSeqRes.push_back(pPath);
				vOtherSeqRes.push_back(pConflictPath);
				//if(  ptheMob->getCurDirPath()  )//the mob is in the res already
				{					
					if( pConflictPath->getRouteResource()->bMobileInResource(pOtherMob) )////	ptheMob->getCurDirPath()->getRouteResource() == pPath->getRouteResource()
					{				
						bOtherMobileInres = true;
						break;
					}
					if(pConflictPath->getRouteResource()->bMobileInResource(pmob))
					{
						bMobileInRes = true;
					}					
				}
			}
			else//not conflict
			{
				if(!vSeqRes.empty())
				{
					if(bOtherMobileInres && bMobileInRes)
					{
						ASSERT(false);
					}

					MobileConfliction* pNewConflict = new MobileConfliction(pmob,pOtherMob,vSeqRes, vOtherSeqRes); 
					if(bOtherMobileInres) //if the other mobile is in the resource list already let it go first
					{
						pNewConflict->SetGetResMob(pOtherMob);
						bool bTRUE = SetAWaitForB(pmob,pOtherMob);
						ASSERT(bTRUE);
					}
					if(bMobileInRes || bGetRes)
					{
						pNewConflict->SetGetResMob(pmob);
						bool bTRUE = SetAWaitForB(pOtherMob,pmob);
						ASSERT(bTRUE);
					}
					
					mvMobileConflictRoute.push_back(pNewConflict);			
				}
				vSeqRes.clear();
				vOtherSeqRes.clear();
				bOtherMobileInres = false;
				bMobileInRes = false;
			}
		}

		//the last
		if(!vSeqRes.empty())
		{
			if(bOtherMobileInres && bMobileInRes)
			{
				ASSERT(false);
			}

			MobileConfliction* pNewConflict = new MobileConfliction(pmob,pOtherMob,vSeqRes,vOtherSeqRes); 
			if(bOtherMobileInres) //if the other mobile is in the resource list already let it go first
			{
				pNewConflict->SetGetResMob(pOtherMob);
				bool bTRUE = SetAWaitForB(pmob,pOtherMob);
				ASSERT(bTRUE);
			}
			if(bMobileInRes || bGetRes)
			{
				pNewConflict->SetGetResMob(pmob);
				bool bTRUE = SetAWaitForB(pOtherMob,pmob);
				ASSERT(bTRUE);
			}

			mvMobileConflictRoute.push_back(pNewConflict);			
		}		
		
	}

}

void DynamicConflictGraph::OnMobileUnPlanRoute( ARCMobileElement* pElm, const ElapsedTime& t )
{
	//remove conflict map
	for(int i=0;i<(int)mvMobileConflictRoute.size();i++){
		MobileConfliction* pconfRes = mvMobileConflictRoute.at(i);
		if(pconfRes->IsMobileConfliction(pElm))
		{
			//notify the other flight
			ARCMobileElement* pOtherMob = pconfRes->getOtherMobile(pElm);
			SimMessage messageLeave;messageLeave.setTime(t);
			pOtherMob->OnNotify(pElm,messageLeave);
			mvMobileConflictRoute.erase(mvMobileConflictRoute.begin()+i);
			ClearMobileWaitRecord(pElm);
			delete pconfRes;
			--i;
		}
	}
	//remove route
	//mMobileRouteMap.erase(pElm);
	DeleteMobileRoute(pElm);
}

#ifdef _DEBUG
const CString strMobileRoutecConflict = _T("c:\\taxiconflict\\routeconflictdebug.txt");
const CString strMobileEnterLeave = _T("c:\\taxiconflict\\enterleavedebug.txt");
#else
const CString strMobileRoutecConflict = _T("c:\\taxiconflict\\routeconflictrelease.txt");
const CString strMobileEnterLeave = _T("c:\\taxiconflict\\enterleaverelease.txt");
#endif

void LogConflictMap(ARCMobileElement* pMob, ARCMobileElement*pconflictMob ,bool RouteBeOcy ,MobileConfliction* pconfRes)
{
	if(pMob->IsKindof(typeof(AirsideFlightInSim))  && pconflictMob->IsKindof(typeof(AirsideFlightInSim)))
	{
		CFileOutput fileout(strMobileRoutecConflict);  	
		AirsideFlightInSim* pFlight = (AirsideFlightInSim*)pMob;
		AirsideFlightInSim* pConflictFlight = (AirsideFlightInSim*)pconflictMob;
		CString strOUt;
		if(!RouteBeOcy)
			strOUt.Format(_T("%s can not let %s wait at %s (%s)"),pFlight->GetCurrentFlightID(),pConflictFlight->GetCurrentFlightID(),pconfRes->Print().GetString() ,pFlight->GetTime().printTime().GetString());
		else
			strOUt.Format(_T("%s 's route is ocuppyed by %s at %s (%s)"),pFlight->GetCurrentFlightID(),pConflictFlight->GetCurrentFlightID(),pconfRes->Print().GetString(),pFlight->GetTime().printTime().GetString() );
		fileout.LogLine(strOUt);
	}
}
void LogWaitMap(ARCMobileElement* pMobA, ARCMobileElement*pMoB)
{
	if(pMobA->IsKindof(typeof(AirsideFlightInSim))  && pMoB->IsKindof(typeof(AirsideFlightInSim)))
	{
		CFileOutput fileout(strMobileRoutecConflict);  	
		AirsideFlightInSim* pFlight = (AirsideFlightInSim*)pMobA;
		AirsideFlightInSim* pConflictFlight = (AirsideFlightInSim*)pMoB;
		CString strOUt;
		strOUt.Format(_T("%s(%d) wait for %s(%d)"),pFlight->GetCurrentFlightID(),pFlight->GetUID(),pConflictFlight->GetCurrentFlightID(),pConflictFlight->GetUID() );
		fileout.LogLine(strOUt);
	}
}

//mobile f
ARCMobileElement* DynamicConflictGraph::bMobileForbidTo( ARCMobileElement* pMob, RouteDirPath* pPath )
{
	//mpConflictMob = NULL;
	for(int i=0;i<(int)mvMobileConflictRoute.size();i++){
		MobileConfliction* pconfRes = mvMobileConflictRoute.at(i);
		if( !pconfRes->IsMobileConfliction(pMob,pPath))continue;	

		if(pconfRes->mpGetResourceMob != pMob)
		{
			if( pconfRes->mpGetResourceMob!=NULL)			
			{				
				ARCMobileElement* pconflictMob =  pconfRes->getOtherMobile(pMob);//for debug
				//LogConflictMap(pMob,pconflictMob,true,pconfRes);
				return pconflictMob;		
			}
			else if( !bCanLetBWaitForA(pMob,pconfRes->getOtherMobile(pMob)) )
			{					
				ARCMobileElement* pconflictMob =  pconfRes->getOtherMobile(pMob);//for debug
				//LogConflictMap(pMob,pconflictMob,false,pconfRes);
				return pconflictMob;
			}			
		}
		
	}
	return NULL;
}


//mobile wait at the hold ,need to wait for 5minutes and add to enter
void DynamicConflictGraph::OnMobileGoingToEnter( ARCMobileElement* pmob, RouteDirPath* pPath,const ElapsedTime& t )
{
	//if(bMobileForbidTo(pmob,pPath))
	//{
	//	//add event after five minute to set the dir lock to me
	//	//MobileGetConflictionLockEvent* pEvent = new MobileGetConflictionLockEvent(pmob,pPath,t+ElapsedTime(5*60L));
	//	//pEvent->addEvent();
	//}
	//else
	//{
	//	 DynamicConflictGraph::getInstance().setMobileGetTheConflictLock(pmob,pPath);
	//}
}


void DynamicConflictGraph::setMobileGetTheConflictLock( ARCMobileElement* pmob, RouteDirPath* pNextPath )
{
	//set all unset 
	for(int i=0;i<(int)mvMobileConflictRoute.size();i++)
	{
		MobileConfliction* pconfRes = mvMobileConflictRoute.at(i);
		if(pconfRes->mpGetResourceMob)
			continue;
		if(!pconfRes->IsMobileConfliction(pmob,pNextPath))
			continue;

		ARCMobileElement* theOtherMob= pconfRes->getOtherMobile(pmob);
		if( SetAWaitForB(theOtherMob,pmob) )
		{
			pconfRes->SetGetResMob(pmob);
		}	
	}		
}

/****/
void LogMobileEnter(ARCMobileElement* pElm, RouteDirPath* pPath,const ElapsedTime& t )
{
	CFileOutput fileout(strMobileEnterLeave);  	
	AirsideFlightInSim* pFlight = (AirsideFlightInSim*)pElm;	
	
	CString strOUt;
	strOUt.Format(_T("E %s(%d) enter for %s at %s"),pFlight->GetCurrentFlightID(),pFlight->GetUID(), pPath->PrintResource().GetString(), t.printTime().GetString() );
	fileout.LogLine(strOUt);
}
void LogMobileLeave(ARCMobileElement* pElm, RouteDirPath* pPath,const ElapsedTime& t )
{
	CFileOutput fileout(strMobileEnterLeave);  	
	AirsideFlightInSim* pFlight = (AirsideFlightInSim*)pElm;	

	CString strOUt;
	strOUt.Format(_T("L %s(%d) leave for %s at %s"),pFlight->GetCurrentFlightID(),pFlight->GetUID(), pPath->PrintResource().GetString(), t.printTime().GetString() );
	fileout.LogLine(strOUt);

}
void DynamicConflictGraph::DeleteMobileRoute(ARCMobileElement* pmob)
{
	for(std::vector<MobPairRoute>::iterator iter = mMobileRouteMap.begin(); iter != mMobileRouteMap.end(); ++iter)
	{
		if ((*iter).first == pmob)
		{
			mMobileRouteMap.erase(iter);
			break;
		}
	}
}

RouteDirPathList DynamicConflictGraph::GetMobileRoute(ARCMobileElement* pmob)
{
	for(std::vector<MobPairRoute>::iterator iter = mMobileRouteMap.begin(); iter != mMobileRouteMap.end(); ++iter)
	{
		if ((*iter).first == pmob)
		{
			return (*iter).second;
		}
	}
	return RouteDirPathList();
}

/***/
void DynamicConflictGraph::OnMobileLeave( ARCMobileElement* pElm, RouteDirPath* pPath,const ElapsedTime& t )
{

	//clear plan route
	RouteDirPathList route;
	route = GetMobileRoute(pElm);

	//RouteDirPathList& route = mMobileRouteMap[pElm];
	RouteDirPathList::iterator itrFind = std::find(route.begin(),route.end(),pPath);
	if(itrFind!=route.end())
	{
		itrFind++;
		route.erase(route.begin(),itrFind);	
	}
	//clear conflict route map if the pelm leave the last conflict res
	for(int i=0;i<(int)mvMobileConflictRoute.size();i++)
	{
		MobileConfliction* pconfRes = mvMobileConflictRoute.at(i);
		if(!pconfRes->IsMobileConfliction(pElm,pPath))continue;

		if( pconfRes->getEndPath(pElm) == pPath )
		{
			//notify the other flight
			ARCMobileElement* pOtherMob = pconfRes->getOtherMobile(pElm);
			SimMessage messageLeave;messageLeave.setTime(t);
			pOtherMob->OnNotify(pElm,messageLeave);
			//remove from the list
			mvMobileConflictRoute.erase(mvMobileConflictRoute.begin()+i);
			SetMobAUnWaitForB(pOtherMob,pconfRes->mpGetResourceMob );
			
			delete pconfRes;
			--i;
		}
	}	
	pPath->removeInPathMobile(pElm);
	//LogMobileLeave(pElm,pPath,t);
}

void DynamicConflictGraph::OnMobileEnter( ARCMobileElement* pmob, RouteDirPath* pRes,const ElapsedTime& t )
{
	if(!pRes)return;

	setMobileGetTheConflictLock(pmob,pRes);
	pRes->addInPathMobile(pmob);
	//LogMobileEnter(pmob,pRes,t);
}

//DynamicConflictGraph& DynamicConflictGraph::getInstance()
//{
//	static DynamicConflictGraph Instance;
//	return Instance;
//}


bool DynamicConflictGraph::IsAWaitForBRecusive( ARCMobileElement* pmobA, ARCMobileElement* pmobB ) const
{
	if(pmobA == pmobB)
		return true;

	TypeWaitMap::const_iterator itr = mWaitingMap.find(pmobA);
	if(itr!=mWaitingMap.end())
	{
		const std::set<ARCMobileElement*>& waitlist = itr->second;
		if( waitlist.find(pmobB)!= waitlist.end() )
			return true;

		for( std::set<ARCMobileElement*>::const_iterator itrS = waitlist.begin();itrS!=waitlist.end();itrS++)
		{	
			if(IsAWaitForBRecusive(*itrS,pmobB))
				return true;
		}
	}	
	return false;
}

bool DynamicConflictGraph::bCanLetBWaitForA( ARCMobileElement* pmobA, ARCMobileElement* pmobB ) const
{
	if(IsAWaitForBRecusive(pmobA,pmobB))
		return false;
	return true;
}


bool DynamicConflictGraph::SetAWaitForB( ARCMobileElement* pmobA, ARCMobileElement* pmobB )
{
	if(bCanLetBWaitForA(pmobB,pmobA))
	{
		//LogWaitMap(pmobA,pmobB);
		mWaitingMap[pmobA].insert(pmobB);
		return true;
	}
	return false;
}

void DynamicConflictGraph::ClearMobileWaitRecord( ARCMobileElement* pMob )
{
	mWaitingMap.erase(pMob);
	for(TypeWaitMap::iterator itr=mWaitingMap.begin();itr!=mWaitingMap.end();itr++)
	{
		itr->second.erase(pMob);
	}
}

void DynamicConflictGraph::SetMobAUnWaitForB( ARCMobileElement* pmobA, ARCMobileElement* pmobB )
{
	mWaitingMap[pmobA].erase(pmobB);	
}



bool DynamicConflictGraph::SetAFollowB( ARCMobileElement* pFollowMob, ARCMobileElement* pLeadMob )
{
	LeadWaitMap::iterator itr = mLeadWaitingMap.find(pFollowMob);
	if(itr!=mLeadWaitingMap.end())
	{
		if(itr->second == pLeadMob)
			return true;

		SetMobAUnWaitForB(pFollowMob,itr->second);	
		mLeadWaitingMap[pFollowMob] = NULL;
	}

	if(pFollowMob && pLeadMob)
	{		
		if(SetAWaitForB(pFollowMob,pLeadMob)/*bCanLetBWaitForA(pLeadMob,pFollowMob)*/)
		{		
			mLeadWaitingMap[pFollowMob] = pLeadMob;
		}
		else
		{
			//ASSERT(FALSE);
			return false;
		}
	}	
	return true;
}

bool DynamicConflictGraph::IsAFollowB( ARCMobileElement* pmobA, ARCMobileElement* pmobB ) const
{
	LeadWaitMap::const_iterator itr = mLeadWaitingMap.find(pmobA);
	if(itr!=mLeadWaitingMap.end())
	{
		return itr->second == pmobB;
	}
	return false;
}

ARCMobileElement* DynamicConflictGraph::GetLeadMobile( ARCMobileElement* pmobA )
{
	LeadWaitMap::const_iterator itr = mLeadWaitingMap.find(pmobA);
	if(itr!=mLeadWaitingMap.end())
	{
		return itr->second;
	}
	return NULL;
}

//ARCMobileElement* DynamicConflictGraph::TryGetAllConflictLock( ARCMobileElement* pMob )
//{
//	for(int i=0;i<(int)mvMobileConflictRoute.size();i++)
//	{
//		MobileConfliction* pconfRes = mvMobileConflictRoute.at(i);
//		if( !pconfRes->IsMobileConfliction(pMob))continue;	
//
//		if(pconfRes->mpGetResourceMob != pMob)
//		{
//			if( pconfRes->mpGetResourceMob!=NULL)			
//			{	
//				return pconfRes->mpGetResourceMob;		
//			}			
//		}
//	}
//	return NULL;
//}

//bool DynamicConflictGraph::GetAllConflictLock( ARCMobileElement* pMob )
//{
//	for(int i=0;i<(int)mvMobileConflictRoute.size();i++)
//	{
//		MobileConfliction* pconfRes = mvMobileConflictRoute.at(i);
//		if( !pconfRes->IsMobileConfliction(pMob))continue;			
//		
//		if( pconfRes->mpGetResourceMob==NULL)			
//		{	
//			pconfRes->mpGetResourceMob = pMob;		
//		}	
//		else if(pconfRes->mpGetResourceMob != pMob)
//		{
//			return false;
//		}
//		
//	}
//	return true;
//}

//bool MobileConfliction::bHasDirPath( RouteDirPath* pPath ) const
//{
//	return std::find(vSequenceResource.begin(),vSequenceResource.end(),pPath)!=vSequenceResource.end();
//}

ARCMobileElement* MobileConfliction::getOtherMobile( ARCMobileElement* pmob ) const
{
	if(mpMobileConflictA==pmob)return mpMobileConflictB; 
	if(mpMobileConflictB==pmob)return mpMobileConflictA;
	return NULL;
}

bool MobileConfliction::IsMobileConfliction( ARCMobileElement* pmob,RouteDirPath* pPath ) const
{
	if(mpMobileConflictA == pmob)
	{
		for(std::vector<RouteDirPath*>::const_iterator itr = mvMobARoute.begin();itr!=mvMobARoute.end();++itr)
		{
			RouteDirPath* ptPath= *itr;
			if(ptPath->getRouteResource() == pPath->getRouteResource())
				return true;
		}		
	}
	else if(mpMobileConflictB == pmob)
	{
		for(std::vector<RouteDirPath*>::const_iterator itr = mvMobBRoute.begin();itr!=mvMobBRoute.end();++itr)
		{
			RouteDirPath* ptPath= *itr;
			if(ptPath->getRouteResource() == pPath->getRouteResource())
				return true;
		}		
	}
	return false;
}

bool MobileConfliction::IsMobileConfliction( ARCMobileElement* pmob ) const
{
	return (mpMobileConflictA==pmob || mpMobileConflictB==pmob);
}

//bool MobileConfliction::bMoibleInResources( ARCMobileElement* pMob ) const
//{
//	if(pMob==mpMobileConflictA)
//	for(int i=0;i<(int)vSequenceResource.size();++i)
//	{
//		RouteResource *pRes = vSequenceResource.at(i)->getRouteResource();
//		if(pRes->bMobileInResource(pMob))
//			return true;		
//	}
//	return false;
//}

RouteDirPath* MobileConfliction::getEndPath( ARCMobileElement* pMob ) const
{
	
	if(pMob == mpMobileConflictA)
	{
		if(!mvMobARoute.empty())
			return *mvMobARoute.rbegin();
	}
	if(pMob == mpMobileConflictB)
	{
		if(!mvMobBRoute.empty())
		return (*mvMobBRoute.begin());
	}
	return NULL;
}

CString MobileConfliction::Print() const
{
	CString strRet;
	/*for(int i=0;i<(int)vSequenceResource.size();i++)
	{
		RouteResource* pRes = vSequenceResource[i]->getRouteResource();
		if(pRes && pRes->IsKindof(typeof(FlightGroundRouteSegInSim)) )
		{
			FlightGroundRouteSegInSim* pSeg = (FlightGroundRouteSegInSim*)pRes;
			CString strTmp;
			strTmp.Format("(%d,%d)",pSeg->GetNode1()->GetID(),pSeg->GetNode2()->GetID());
			strRet+=strTmp;
		}
	}*/
	return strRet;	
}


void DynamicConflictGraph::OnFlightEnterNode( AirsideFlightInSim* pFlt, IntersectionNodeInSim* pNode,const ElapsedTime& t )
{
	pNode->OnFlightEnter(pFlt,t);
	//set all unset 
	for(int i=0;i<(int)mvMobileConflictRoute.size();i++)
	{
		MobileConfliction* pconfRes = mvMobileConflictRoute.at(i);
		if(pconfRes->mpGetResourceMob!=NULL)
			continue;
		if(!pconfRes->IsMobileConfliction(pFlt,pNode))
			continue;

		ARCMobileElement* theOtherMob= pconfRes->getOtherMobile(pFlt);
		
		if( SetAWaitForB(theOtherMob,pFlt) )
		{
			pconfRes->SetGetResMob(pFlt);
		}		
	}	

}

bool DynamicConflictGraph::bTryPlanRoute( ARCMobileElement* pmob, const RouteDirPathList& vPathList,bool bGetRes /*= false*/ )const
{
	for(std::vector<MobPairRoute>::const_iterator itr = mMobileRouteMap.begin();itr!=mMobileRouteMap.end();++itr)
	{
		ARCMobileElement* pOtherMob = itr->first;
		if(pOtherMob==pmob)continue;
		//find sequence conflict resources
		const RouteDirPathList& otherMobRoute = itr->second;

		RouteDirPathList vSeqRes;		
		RouteDirPathList vOtherSeqRes;
		bool bOtherMobileInres = false;
		bool bMobileInRes = false;

		for(int i=0;i<(int)vPathList.size();i++)
		{
			RouteDirPath* pPath = vPathList.at(i);

			if( RouteDirPath* pConflictPath = bConflictInRoute(pmob,pOtherMob,pPath,otherMobRoute) ) //add to buffer list
			{
				vSeqRes.push_back(pPath);
				vOtherSeqRes.push_back(pConflictPath);
				//if(  ptheMob->getCurDirPath()  )//the mob is in the res already
				{					
					if( pConflictPath->getRouteResource()->bMobileInResource(pOtherMob) )////	ptheMob->getCurDirPath()->getRouteResource() == pPath->getRouteResource()
					{				
						bOtherMobileInres = true;
						break;
					}
					if(pConflictPath->getRouteResource()->bMobileInResource(pmob))
					{
						bMobileInRes = true;
					}					
				}
			}
			else//not conflict
			{
				if(!vSeqRes.empty())
				{
					if(bOtherMobileInres && bMobileInRes)
					{
						return false;
					}

					//MobileConfliction* pNewConflict = new MobileConfliction(pmob,pOtherMob,vSeqRes, vOtherSeqRes); 
					if(bOtherMobileInres) //if the other mobile is in the resource list already let it go first
					{
						//pNewConflict->SetGetResMob(pOtherMob);
						if( !bCanLetBWaitForA(pOtherMob,pmob) )
							return false;//  SetAWaitForB(pmob,pOtherMob);
						//ASSERT(bTRUE);
					}
					if(bMobileInRes || bGetRes)
					{
						//pNewConflict->SetGetResMob(pmob);
						if(!bCanLetBWaitForA(pmob,pOtherMob))
							return false; //SetAWaitForB(pOtherMob,pmob);
						//ASSERT(bTRUE);
					}

					//mvMobileConflictRoute.push_back(pNewConflict);			
				}
				vSeqRes.clear();
				vOtherSeqRes.clear();
				bOtherMobileInres = false;
				bMobileInRes = false;
			}
		}

		//the last
		if(!vSeqRes.empty())
		{
			if(bOtherMobileInres && bMobileInRes)
			{
				return false; //ASSERT(false);
			}

			if(bOtherMobileInres) //if the other mobile is in the resource list already let it go first
			{
				//pNewConflict->SetGetResMob(pOtherMob);
				if( !bCanLetBWaitForA(pOtherMob,pmob) )
					return false;//  SetAWaitForB(pmob,pOtherMob);
				//ASSERT(bTRUE);
			}
			if(bMobileInRes || bGetRes)
			{
				//pNewConflict->SetGetResMob(pmob);
				if(!bCanLetBWaitForA(pmob,pOtherMob))
					return false; //SetAWaitForB(pOtherMob,pmob);
				//ASSERT(bTRUE);
			}			
		}		

	}
	return true;
}

bool DynamicConflictGraph::CanAWaitForB( ARCMobileElement* pmobA, ARCMobileElement* pmobB ) const
{
	if(bCanLetBWaitForA(pmobB,pmobA))
		return true;
	return false;
}

void DynamicConflictGraph::addTempParkingFlight( AirsideFlightInSim* pFlight )
{
	std::vector<AirsideFlightInSim*>::iterator itr = 
		std::find(m_vTempParkFlights.begin(),m_vTempParkFlights.end(),pFlight);
	if(itr==m_vTempParkFlights.end())
		m_vTempParkFlights.push_back(pFlight);
}

void DynamicConflictGraph::removeTempParkingFlight( AirsideFlightInSim* pFlight )
{
	std::vector<AirsideFlightInSim*>::iterator itr = 
		std::find(m_vTempParkFlights.begin(),m_vTempParkFlights.end(),pFlight);
	if(itr!=m_vTempParkFlights.end())
		m_vTempParkFlights.erase(itr);
}

void DynamicConflictGraph::notifyTempFlights( const RouteDirPathList& vResList,const ElapsedTime& t )
{
	for(size_t i=0;i<m_vTempParkFlights.size();++i)
	{
		AirsideFlightInSim* pFlight = m_vTempParkFlights.at(i);

		if(RouteDirPath* seg = pFlight->getCurDirPath() )
		{
			bool bFind = std::find(vResList.begin(), vResList.end(), seg)!= vResList.end();
			if(bFind)
			{
				if(TempParkingInSim* temppark = pFlight->GetTemporaryParking() )
				{
					temppark->notifyCirculate(pFlight,t);
				}
			}

		}
	}
}

bool DynamicConflictGraph::needCirculate( AirsideFlightInSim* pTempFlight )
{
	for(size_t i =0 ;i< mMobileRouteMap.size();++i)
	{
		MobPairRoute& mobRoute  = mMobileRouteMap[i];
		if(mobRoute.first==pTempFlight)
			continue;
		RouteDirPathList& route =mobRoute.second;
		RouteDirPath* mobPos = mobRoute.first->getCurDirPath();
		if(!mobPos){
			continue;
		}

		RouteDirPathList::iterator itr = std::find(route.begin(),route.end(),mobPos);
		for(itr;itr!=route.end();++itr)
		{
			if(pTempFlight->getCurDirPath()==*itr)
				return true;
		}		
	}
	return false;
}
