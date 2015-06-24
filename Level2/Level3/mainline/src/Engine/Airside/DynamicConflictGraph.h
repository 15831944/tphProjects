#pragma once


#include "ARCMobileElement.h"
#include "RouteResource.h"
class AirsideFlightInSim;
class IntersectionNodeInSim;
class MobileConfliction;
//////////////////////////////////////////////////////////////////////////
class DynamicConflictGraph
{
public:
	//mobile going to enter next few path
	 void OnMobilePlaneRoute(ARCMobileElement* pmob, const RouteDirPathList& vResList, const ElapsedTime& t,bool bGetRes = false); 
	 void OnMobileGoingToEnter(ARCMobileElement* pmob, RouteDirPath* pPath,const ElapsedTime& t);
	 void OnMobileEnter(ARCMobileElement* pmob, RouteDirPath* pRes,const ElapsedTime& t);
	 void OnMobileLeave(ARCMobileElement* pElm, RouteDirPath* pRes,const ElapsedTime& t);	
	 void OnMobileUnPlanRoute(ARCMobileElement* pElm, const ElapsedTime& t); //clean up registry of the the mobile
	
	 ARCMobileElement* bMobileForbidTo(ARCMobileElement* pMob, RouteDirPath* pPath);  //the mobile is forbidden to go to the path by conflict ion with the return mobile
	 void setMobileGetTheConflictLock(ARCMobileElement* pmob, RouteDirPath* pNextPath); //let the mobile get the control of the conflict resource

	 void OnFlightEnterNode(AirsideFlightInSim* pFlt, IntersectionNodeInSim* pNode,const ElapsedTime& t);
	 void OnFlightExitNode(AirsideFlightInSim* pFlt, IntersectionNodeInSim* pNode,const ElapsedTime& t);

	//static DynamicConflictGraph& getInstance();
	
	bool SetAFollowB(ARCMobileElement* pFollowMob, ARCMobileElement* pLeadMob);
	bool IsAFollowB(ARCMobileElement* pmobA, ARCMobileElement* pmobB)const;
	ARCMobileElement* GetLeadMobile(ARCMobileElement* pmobA);
	bool SetAWaitForB(ARCMobileElement* pmobA, ARCMobileElement* pmobB);
	bool CanAWaitForB(ARCMobileElement* pmobA, ARCMobileElement* pmobB)const;
	//only can use after called bMobileFobidTo
	//ARCMobileElement* mpConflictMob;
	//ARCMobileElement* TryGetAllConflictLock(ARCMobileElement* pMob);
	//bool GetAllConflictLock(ARCMobileElement* pMob);
	bool bTryPlanRoute(ARCMobileElement* pmob, const RouteDirPathList& vResList,bool bGetRes = false)const;
protected:
    std::vector<MobileConfliction*> mvMobileConflictRoute;  //two mobile conflict 
	typedef std::pair<ARCMobileElement* , RouteDirPathList> MobPairRoute;
	std::vector<MobPairRoute> mMobileRouteMap;

	typedef std::map<ARCMobileElement* , std::set<ARCMobileElement*> > TypeWaitMap;
	typedef std::map<ARCMobileElement*, ARCMobileElement*> LeadWaitMap; //first follow second

	TypeWaitMap mWaitingMap; //mobiles waiting for direction lock
	LeadWaitMap mLeadWaitingMap;
	void SetMobAUnWaitForB(ARCMobileElement* pmobA, ARCMobileElement* pmobB);
	bool bCanLetBWaitForA(ARCMobileElement* pmobA, ARCMobileElement* pmobB)const;
	bool IsAWaitForBRecusive(ARCMobileElement* pmobA, ARCMobileElement* pmobB)const;
	void ClearMobileWaitRecord(ARCMobileElement* pMob);

	void DeleteMobileRoute(ARCMobileElement* pmob);
	RouteDirPathList GetMobileRoute(ARCMobileElement* pmob);
};

