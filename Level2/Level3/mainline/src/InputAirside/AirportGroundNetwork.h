#pragma once
#include <vector>
#include <set>
#include "IntersectionNodesInAirport.h"
#include "InputAirsideAPI.h"

using namespace std;
class Runway;
class Taxiway;
class GroundRoutes;
class GroundRoute;
class GroundRouteItem;
class ALTObject;
class ALTObjectGroup;
class Stand;
#include "IntersectionNode.h"
typedef std::vector<IntersectionNode> IntersectionNodeList;
typedef std::vector<GroundRoute*> GroundRouteVector;
enum RUNWAY_MARK;

class INPUTAIRSIDE_API CAirportGroundNetwork
{
public:
	CAirportGroundNetwork(int nPrjID);
	~CAirportGroundNetwork(void);

public:
	void GetRunwayIntersectGroundRoutes(Runway *pRunway, RUNWAY_MARK runwayMark, GroundRouteVector *pIntersectGRVector);
	void GetTaxiwayIntersectGroundRoutes(Taxiway *pTaxiway, GroundRouteVector *pIntersectGRVector);
	void GetGroundRouteIntersectGroundRoutes(GroundRoute *pGroundRoute,  GroundRouteVector *pIntersectGRVector);


	void GetGroundRouteTaxiwayIntersectNodeList(GroundRoute* pGroundRoute, ALTObject *pTaxiway, IntersectionNodeList *pIntersectNodeList);
	void GetTaxiwayGroundRouteIntersectNodeList(Taxiway *pTaxiway, GroundRoute *pGroundRoute, IntersectionNodeList *pIntersectNodeList);

	void GetGroundRouteGroundRouteIntersectNodeList(GroundRoute *pFirstRoute, GroundRoute *pSecondRoute, IntersectionNodeList *pIntersectNodeList);

	void GetRunwayGroundRouteIntersectNodeList(Runway *pRunway, GroundRoute *pGroundRoute, IntersectionNodeList *pIntersectNodeList);
	void GetGroundRouteRunwayIntersectNodeList(GroundRoute *pGroundRoute, Runway *pRunway, IntersectionNodeList *pIntersectNodeList);


	void GetRunwayTaxiwayIntersectNodeList(Runway *pRunway, Taxiway *pTaxiway, IntersectionNodeList& pIntersectNodeList);
	void GetTaxiwayTaxiwayIntersectNodeList(ALTObject *pFirstTaxiway, ALTObject *pSecondTaxiway, IntersectionNodeList& pIntersectNodeList);

	void GetStandList(int nAirportID);
	void GetTaxiwayList(int nAirportID);
	void GetRunwayList(int nAirportID);
	void GetIntersectedTaxiwayRunway(ALTObject* pAltObject, std::set<ALTObject*>& vTaxiwayList, std::set<ALTObject*>& vRunwayist);
	void GetIntersectedGroundRoutes(ALTObject* pAltObject,GroundRouteVector& vGroundRouteList);

	BOOL IsALTObjIntersectWithGroundRoute(int nALTObjID, GroundRoute *pGroundRoute);
	BOOL IsGroundRouteIntersectWithGroundRoute(GroundRoute *pFirstGroundRoute, GroundRoute *pSecondGroundRoute);
	BOOL IsIntersectedGrouteRouteItems(GroundRouteItem *pItem1, GroundRouteItem *pItem2);

	ALTObject* GetTaxiwayByID(int nID);
	ALTObject* GetRunwayByID(int nID);

	//BOOL IsALTObjIntersectWithGroundRouteItem(int nALTObjID, GroundRouteItem *pGRItem);
	//BOOL IsALTObjInGroundRoute(int nALTObjID, GroundRoute *pGroundRoute);
	//BOOL IsALTObjEqualWithGroundRouteItem(int nALTObjID, GroundRouteItem *pGroundRouteItem);

	//BOOL IsGroundRouteItemIntersectWithGroundRoute(GroundRouteItem *pGroundRouteItem, GroundRoute *pOtherGroundRoute);

	////get the taxiway nodelist which is intersect with the ALTObjectGroup
	//void GetStandGroupTaxiwayIntersectNodeList(ALTObjectGroup *pALTObjGroup, IntersectionNodeList *pIntersectNodeList, vector<int>* vStandID);
	//void GetStandGroupGroundRouteIntersectNodeList(ALTObjectGroup *pALTObjGroup, GroundRoute *pGroundRoute, IntersectionNodeList *pIntersectNodeList);
	//void GetStandGroupIntersectGroundRoutes(ALTObjectGroup *pALTObjGroup, GroundRoutes *pAllGroundRoutes, GroundRouteVector *pIntersectGRVector);

	//void GetStandOutNodeList(Stand *pStand, IntersectionNodeList& outNodeList, vector<int>* vStandID);
	//void GetStandInNodeList(Stand *pStand, IntersectionNodeList& inNodeList);



private:

	ALTObjectList m_vStands;
	ALTObjectList m_vTaxiways;
	ALTObjectList m_vRunways;


	GroundRoutes* m_pGroundRoutes;
	IntersectionNodesInAirport m_vIntersectionNodeList;

};
