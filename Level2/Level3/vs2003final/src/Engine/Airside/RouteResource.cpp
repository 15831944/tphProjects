#include "StdAfx.h"

#include "RouteResource.h"
#include <algorithm>



//////////////////////////////////////////////////////////////////////////
void RouteDirPath::addPlanMobile( ARCMobileElement* pmob )
{
	mvPlanMobiles.insert(pmob);
}

void RouteDirPath::removePlanMobile( ARCMobileElement* pmob )
{
	mvPlanMobiles.erase(pmob);
}

bool RouteDirPath::bMobilePlaned( ARCMobileElement* pmob ) const
{
	return mvPlanMobiles.find(pmob)!=mvPlanMobiles.end();
}


//////////////////////////////////////////////////////////////////////////
void RouteDirPath::addQueueMobile( ARCMobileElement* pmob )
{
	getRouteResource()->addQueueItem(pmob,this);
}

void RouteDirPath::removeQueueMobile( ARCMobileElement* pmob )
{
	getRouteResource()->removeQueueItem(pmob);
}

ARCMobileElement* RouteDirPath::getFirstQueueMobile() const
{
	return getRouteResource()->getFirstQueueMobile();
}


//////////////////////////////////////////////////////////////////////////
bool RouteDirPath::bHaveInPathMoibleExcept(ARCMobileElement* pmob) const
{
	for(int i=0;i<(int)vInPathMobiles.size();i++)
	{
		if(vInPathMobiles.at(i)!=pmob)
			return true;
	}
	return false;
}
void RouteDirPath::removeInPathMobile( ARCMobileElement* pmob )
{
	std::vector<ARCMobileElement*>::iterator itrFind  = std::find(vInPathMobiles.begin(),vInPathMobiles.end(),pmob);
	if(itrFind!=vInPathMobiles.end()) 
		vInPathMobiles.erase(itrFind);
}

void RouteDirPath::addInPathMobile( ARCMobileElement* pmob )
{
	if(!bMobileInPath(pmob))
		vInPathMobiles.push_back(pmob);
}
bool RouteDirPath::bMobileInPath( ARCMobileElement* pmob ) const
{
	return std::find(vInPathMobiles.begin(),vInPathMobiles.end(),pmob)!=vInPathMobiles.end();
}

ARCMobileElement* RouteDirPath::getLeadMobile( ARCMobileElement* pmob ) const
{
	std::vector<ARCMobileElement*>::const_iterator itrFind = std::find(vInPathMobiles.begin(),vInPathMobiles.end(),pmob);
	if(itrFind == vInPathMobiles.end() && !vInPathMobiles.empty())
	{
		return *vInPathMobiles.rbegin();
	}	
	if(itrFind!= vInPathMobiles.end() && itrFind!= vInPathMobiles.begin() )
	{
		return *(--itrFind);
	}
	return 0;
}

ARCMobileElement* RouteDirPath::getAdjacencyLeadMobile(ARCMobileElement* pmob,double& dDist)
{
	return NULL;
}

bool RouteDirPath::IsDirConflictWith( RouteDirPath* pPath )
{
	if(pPath->getRouteResource() == getRouteResource() ){ 
		return getRouteResource()->bDirConflictPaths(this,pPath);
	}
	return false;
}

bool RouteResource::bDirConflictPaths( RouteDirPath* pPath1, RouteDirPath* pPath2 )
{
	return pPath1!=pPath2;
}

bool RouteDirPath::adjacencyNegativeConflict(RouteDirPath* pPath,ARCMobileElement* pmob,ARCMobileElement* ptheMob)
{
	return false;
}
