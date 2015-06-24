#pragma once
#include <Common/Types.hpp>
#include <set>
#include "./AirsideResource.h"


#define SET(T) std::set<T>

class RouteDirPath;
typedef std::vector<RouteDirPath*> RouteDirPathList;
class ARCMobileElement;
#include "SimAgent.h"
//////////////////////////////////////////////////////////////////////////
//Route resource are the mobile element are going to enter can hold many mobile at the same time
//
class RouteResource : public SimAgent
{
	DynamicClassStatic(RouteResource)
public:
	class QueueItem
	{
	public:
		QueueItem(ARCMobileElement* pmob, RouteDirPath* pPath):mpMob(pmob),mpPath(pPath){}
		ARCMobileElement* mpMob;
		RouteDirPath* mpPath;
	};
	typedef std::vector<QueueItem> ResourceQueue;
public: //functions used by mobile
	virtual bool IsActiveFor(ARCMobileElement* pmob,const RouteDirPath* pPath)const=0;
	virtual bool bMayHaveConflict(const RouteDirPath *pPath)const=0;
	virtual bool bLocked(ARCMobileElement* pmob,const RouteDirPath* pPath)const=0;
	virtual bool bMobileInResource(ARCMobileElement* pmob)const=0;

	//always active for it will for whole simulation
	virtual bool IsActive()const{ return true; }	

	void OnMobileEnter(const ElapsedTime& t){  mCurTime = t; NotifyOtherAgents( SimMessage().setTime(t) ); } 
	void OnMobielExit(const ElapsedTime& t){ mCurTime = t; NotifyOtherAgents( SimMessage().setTime(t));  }
	void OnNotify( SimAgent* pAgent, SimMessage& msg ){} //it wont be notified;
	ElapsedTime mCurTime;
	ElapsedTime getCurTime()const{ return mCurTime; } 



	void addQueueItem(ARCMobileElement* pMob, RouteDirPath* pPath)
	{
		if(!bInQueue(pMob))
			mQueue.push_back(QueueItem(pMob,pPath));
	}
	void removeQueueItem(ARCMobileElement* pmob)
	{
		for(ResourceQueue::iterator itr=mQueue.begin();itr!=mQueue.end();++itr)
		{
			if( itr->mpMob== pmob)
			{
				mQueue.erase(itr);
				break;
			}
		}
	}
	bool bInQueue(ARCMobileElement* pmob)const
	{ 
		for(ResourceQueue::const_iterator itr=mQueue.begin();itr!=mQueue.end();++itr)
		{
			if( itr->mpMob== pmob)
			{
				return true;
			}
		}
		return false;
	}
	ARCMobileElement* getFirstQueueMobile()const
	{
		if(!mQueue.empty())
		{
			return mQueue.begin()->mpMob;
		}
		return 0;
	}

	virtual bool bDirConflictPaths(RouteDirPath* pPath1, RouteDirPath* pPath2);
protected:
	ResourceQueue mQueue;
};

//////////////////////////////////////////////////////////////////////////
//the route dir path that are laid on the route resource, mobiles walks on this guid line
//
class RouteDirPath : public AirsideResource
{	
	DynamicClassStatic(RouteDirPath)
	RegisterInherit(RouteDirPath,AirsideResource)
public:
	//RouteDirPath(RouteResource* pResAt,RouteResource* pSrcRes, RouteResource* pDestRes):mpSrcRes(pSrcRes),mpResAt(pResAt),mpDestRes(pDestRes){}
	virtual bool IsActiveFor(ARCMobileElement* pmob)const{ return getRouteResource()->IsActiveFor(pmob,this); }
	virtual bool bMayHaveConflict()const{ return getRouteResource()->bMayHaveConflict(this); }
	//get the lead mob of the mob
	ARCMobileElement* getLeadMobile(ARCMobileElement* pmob)const;
	virtual ARCMobileElement* getAdjacencyLeadMobile(ARCMobileElement* pmob,double& dDist);

	void addPlanMobile(ARCMobileElement* pmob);
	void removePlanMobile(ARCMobileElement* pmob);
	bool bMobilePlaned(ARCMobileElement* pmob)const;
	std::set<ARCMobileElement*> getPlanMobiles()const{ return mvPlanMobiles; }
	int getPlanMobileCount()const{ return (int)mvPlanMobiles.size(); }

	void addInPathMobile(ARCMobileElement* pmob);
	void removeInPathMobile(ARCMobileElement* pmob);
	bool bMobileInPath(ARCMobileElement* pmob)const;
	std::vector<ARCMobileElement*> getInPathMobile()const {return vInPathMobiles;}
	bool bHaveInPathMoibleExcept(ARCMobileElement* pmob = NULL)const;

	//the will conflict other dir path
	bool IsDirConflictWith(RouteDirPath* pPath);	

public: //functions used by route controller
	virtual void addQueueMobile(ARCMobileElement* pmob);
	virtual void removeQueueMobile(ARCMobileElement* pmob);
	ARCMobileElement* getFirstQueueMobile()const;;
	virtual RouteResource* getRouteResource()const=0; 
	virtual bool adjacencyNegativeConflict(RouteDirPath* pPath,ARCMobileElement* pmob,ARCMobileElement* ptheMob);

protected:

	std::vector<ARCMobileElement*> vInPathMobiles;//mobile entered
	std::set<ARCMobileElement*> mvPlanMobiles; //mobiles plan to enter
};
