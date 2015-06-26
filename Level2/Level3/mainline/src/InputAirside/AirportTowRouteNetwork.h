#pragma once
#include <vector>
#include <set>
#include "IntersectionNodesInAirport.h"
#include "IntersectionNode.h"
#include "InputAirsideAPI.h"

using namespace std;
class Runway;
class Taxiway;
class CTowingRouteList;
class CTowingRoute;
class CTaxiwayNode;
class ALTObject;
class ALTObjectGroup;
class Stand;

typedef std::vector<IntersectionNode> IntersectionNodeList;

enum RUNWAY_MARK;

class INPUTAIRSIDE_API CAirportTowRouteNetwork
{
public:
	CAirportTowRouteNetwork(int nPrjID);
	~CAirportTowRouteNetwork(void);



public:
	void GetRunwayIntersectGroundRoutes(Runway *pRunway, RUNWAY_MARK runwayMark, std::vector<CTowingRoute *>  *pIntersectGRVector);
	void GetTaxiwayIntersectGroundRoutes(Taxiway *pTaxiway, std::vector<CTowingRoute *>  *pIntersectGRVector);
	void GetGroundRouteIntersectGroundRoutes(CTowingRoute *pGroundRoute,  std::vector<CTowingRoute *> *pIntersectGRVector);


	void GetGroundRouteTaxiwayIntersectNodeList(CTowingRoute* pGroundRoute, ALTObject *pTaxiway, IntersectionNodeList *pIntersectNodeList);
	void GetTaxiwayGroundRouteIntersectNodeList(Taxiway *pTaxiway, CTowingRoute *pGroundRoute, IntersectionNodeList *pIntersectNodeList);

	void GetGroundRouteGroundRouteIntersectNodeList(CTowingRoute *pFirstRoute, CTowingRoute *pSecondRoute, IntersectionNodeList *pIntersectNodeList);

	void GetRunwayGroundRouteIntersectNodeList(Runway *pRunway, CTowingRoute *pGroundRoute, IntersectionNodeList *pIntersectNodeList);
	void GetGroundRouteRunwayIntersectNodeList(CTowingRoute *pGroundRoute, Runway *pRunway, IntersectionNodeList *pIntersectNodeList);


	void GetRunwayTaxiwayIntersectNodeList(Runway *pRunway, Taxiway *pTaxiway, IntersectionNodeList& pIntersectNodeList);
	void GetTaxiwayTaxiwayIntersectNodeList(ALTObject *pFirstTaxiway, ALTObject *pSecondTaxiway, IntersectionNodeList& pIntersectNodeList);

	void GetStandList(int nAirportID);
	void GetTaxiwayList(int nAirportID);
	void GetRunwayList(int nAirportID);
	void GetIntersectedTaxiwayRunway(ALTObject* pAltObject, std::set<ALTObject*>& vTaxiwayList, std::set<ALTObject*>& vRunwayist);
	void GetIntersectedGroundRoutes(ALTObject* pAltObject,std::vector<CTowingRoute *>& vGroundRouteList);

	BOOL IsALTObjIntersectWithGroundRoute(int nALTObjID, CTowingRoute *pGroundRoute);
	BOOL IsGroundRouteIntersectWithGroundRoute(CTowingRoute *pFirstGroundRoute, CTowingRoute *pSecondGroundRoute);
	BOOL IsIntersectedGrouteRouteItems(CTaxiwayNode *pItem1, CTaxiwayNode *pItem2);

	ALTObject* GetTaxiwayByID(int nID);
	ALTObject* GetRunwayByID(int nID);


private:
	ALTObjectList m_vStands;
	ALTObjectList m_vTaxiways;
	ALTObjectList m_vRunways;


	CTowingRouteList* m_pTowingRoutes;
	IntersectionNodesInAirport m_vIntersectionNodeList;


};
