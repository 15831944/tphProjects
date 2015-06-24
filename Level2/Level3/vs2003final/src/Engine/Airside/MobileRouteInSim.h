#pragma once

#include <Boost/tuple/tuple.hpp>
#include <Common/Point2008.h>
#include ".\simagent.h"

//
//Mobile Move on the dir path
class RouteDirPath;
class MobileRouteItem
{
public:
	MobileRouteItem(RouteDirPath* pPath, DistanceUnit distFrom, DistanceUnit distTo){
		mpPath = pPath;
		m_distFrom = distFrom;
		m_distTo = distTo;
	}
	RouteDirPath * GetPath()const{ return mpPath; }
	DistanceUnit GetDistFrom()const{ return m_distFrom; }
	DistanceUnit GetDistTo()const{ return m_distTo; }

	bool IsDistInItem(DistanceUnit dist)const;
	RouteDirPath* mpPath;
	DistanceUnit m_distFrom;
	DistanceUnit m_distTo;


};


//the intersection need to check in this route
class GroundIntersectionInSim;
class ARCMobileElement;
class IntersectionInRoute
{
public:
	DistanceUnit m_dEntryDistInRoute;
	DistanceUnit m_dExitDistInRoute;
	GroundIntersectionInSim* m_pNode;	
	bool IsRuwnayIntersection()const;
	bool IsMobileIn(ARCMobileElement* pmob)const;
	DistanceUnit getEntryDist()const{ return m_dEntryDistInRoute; }
	DistanceUnit getExitDist()const{ return m_dExitDistInRoute; }
public:
	IntersectionInRoute(const DistanceUnit&entrydist, const DistanceUnit& exitdist, GroundIntersectionInSim* pNode ){		
		m_dEntryDistInRoute = entrydist;
		m_dExitDistInRoute = exitdist;
		m_pNode = pNode;
	}
};



class MobileRouteInSim;
class ARCMobileElement;
//hold dist in the route the enter the intersection
class MobileIntersectionListInRoute
{
public:
	MobileIntersectionListInRoute(){}
	void Init(const MobileRouteInSim& theRoute,ARCMobileElement*pMob);
	int GetCount()const{ return (int)m_vDataList.size(); }

	IntersectionInRoute* ItemAt(int idx){ return m_vDataList[idx]; }
	const IntersectionInRoute* ItemAt(int idx)const{ return m_vDataList[idx]; }

	IntersectionInRoute* GetLastEntryHold()const;
	IntersectionInRoute* GetFirstEntryHold()const;
	IntersectionInRoute* GetFirstExitHold()const;

	
	IntersectionInRoute* GetNextIntersection(DistanceUnit distInRoute)const;
	std::vector<IntersectionInRoute*> GetNextIntersetions(DistanceUnit dist)const;

	//return the first dir path after the dist
	RouteDirPath* getNextDirPath(DistanceUnit dist)const;

	std::vector<IntersectionInRoute*> GetEntryIntersections(DistanceUnit distF,DistanceUnit distT)const;
	std::vector<IntersectionInRoute*> GetExitIntersections(DistanceUnit distF,DistanceUnit distT)const;

	//return the hold intersection
	IntersectionInRoute* IsDistForbidParking(const DistanceUnit& dist)const;
public:
	std::vector<IntersectionInRoute*> m_vDataList; //list sorted by entrydist
	//std::vector<IntersectionInRoute*> m_vDataListExit; //list sorted by exitdist;
};


//mobile move on the route
class ClearanceItem;
class Clearance;
class MobileRouteInSim
{
public:
	MobileRouteInSim(){ mpMobile = NULL; }

	void Init(ARCMobileElement* pMobile,const ElapsedTime& t);

	int  GetItemCount()const{ return (int)m_vItems.size(); }
	MobileRouteItem & ItemAt(int idx){ return m_vItems[idx]; }
	const MobileRouteItem& ItemAt(int idx)const{ return m_vItems[idx]; }
	void AddItem(const MobileRouteItem& newitem){ m_vItems.push_back(newitem); }
	DistanceUnit GetEndDist()const{ return *mvCacheItemLength.rbegin(); }//total length of the route
	DistanceUnit GetItemBeginDist(int idx)const{ return mvCacheItemLength[idx]; }
	DistanceUnit GetItemEndDist(int idx)const{ return mvCacheItemLength[idx+1]; }

protected:
	MobileRouteItem GetItem(int idx){ return m_vItems[idx]; }
	boost::tuple<int,DistanceUnit> GetItemIndexAndDist(const DistanceUnit& dist)const;
	DistanceUnit GetDistInRoute( const int& nItemIdx , const DistanceUnit& distInItem)const;

	//do the things on mobile move from distF to distT,release the resource, get the resource
	virtual void MobileMove( const DistanceUnit& distF, const DistanceUnit& distT, const ElapsedTime& tF,const double& spdF,const double&  spdT );
	virtual void MobileEnter(IntersectionInRoute* pIntersection,const ElapsedTime&t );
	virtual void MobileExit(IntersectionInRoute* pIntersection,const ElapsedTime& t);
	virtual void MobileExitRoute(const ElapsedTime& t); //exit the whole route
	virtual void MobileEnterRoute(const ElapsedTime& t); //enter route first time

	//return conflict mobile , if null, then can across the entry hold
	boost::tuple<ARCMobileElement*,RouteDirPath* > bCanCrossTheIntersection( IntersectionInRoute* pHold)const;
	//get the lead mobile from current  position
	boost::tuple<ARCMobileElement*, DistanceUnit> getLeadMobile( DistanceUnit distInRoute)const;

	//check conflict with lead mob. return endDist,lead mobile
	boost::tuple<DistanceUnit,ARCMobileElement*> checkConflictWithLeadMobile(DistanceUnit mCurDistInRoute);
	//check conflict with the next hold have conflict,wait agent,and conflict mobile, wait hold, need to ignore lead mobile
	boost::tuple<DistanceUnit,ARCMobileElement*, IntersectionInRoute*,RouteDirPath*> checkConflictWithNextHold( DistanceUnit mCurDistInRoute, ARCMobileElement* pLeadMobile);

	typedef std::vector<MobileRouteItem> RouteItemList;
	RouteItemList m_vItems;

	ARCMobileElement* mpMobile; //this route belongs to this mobile
	MobileIntersectionListInRoute mIntersectionList; //the intersections list in the route, need to check conflict with other mobile
	//cache data
	virtual void updateCachedata(); //update the cached item length list
	std::vector<DistanceUnit> mvCacheItemLength;

};


