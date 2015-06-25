#pragma once
//#include "ResourceManager.h"

#include "VehicleStretchInSim.h"
#include "VehicleRoadIntersectionInSim.h"
#include "VehicleRouteInSim.h"
#include "VehicleRouteUserDefineInSim.h"
#include "VehicleRoutePathFinder.h"


class StandResourceManager;
class VehiclePoolResourceManager;
class TaxiwayResourceManager;
class VehicleRouteInSim;
class AirsideResource;
class CPaxBusParkingResourceManager;
class DeiceResourceManager;
class AirsideMeetingPointInSim;
class CBagCartsParkingSpotResourceManager;

class VehicleRouteResourceManager
{
public:
	bool Init(int nPrjID, int nAirportID, IntersectionNodeInSimList& NodeList);	
	bool InitRelations(StandResourceManager& standres,
		DeiceResourceManager& deiceRes,
		VehiclePoolResourceManager& poolRes,
		TaxiwayResourceManager& taxiwayRes,
		CPaxBusParkingResourceManager& paxBusRes,
		CBagCartsParkingSpotResourceManager& pBagCartsParkingSpotResManager);
	bool InitRelationsWithMeetingPoint(const std::vector<AirsideMeetingPointInSim*>& vMeetingPoints);	

	//initialize the dij shortest path
	void InitDIJShortesRoute();

	int GetStretchCount()const{ return (int)m_vStretches.size(); }  
	int GetIntersectionCount()const{ return (int)m_vIntersections.size(); }
	VehicleStretchInSim * GetStretch(int idx)const{ return (VehicleStretchInSim * ) m_vStretches.at(idx); }

	VehicleRoadIntersectionInSim * GetIntersection(int idx)const{ return  m_vIntersections.at(idx); }
	VehicleStretchInSim * GetStretchByID(int nID);


	bool GetVehicleRoute(AirsideResource * pResFrom, AirsideResource* pResTo, VehicleRouteInSim& resltRoute);

	bool GetVehicleDefineRoute(AirsideResource * pResFrom, AirsideResource* pResTo, VehicleRouteInSim& resltRoute);

	VehicleRoadIntersectionInSim * GetRelatedIntersectionWithLane(VehicleLaneInSim* pExitLane,VehicleLaneInSim *pEntryLane);


	~VehicleRouteResourceManager();

protected:
	bool GetShortestPathWithUserDefineRoutes( AirsideResource * pResFrom, AirsideResource* pResTo, VehicleRouteInSim& resltRoute );
	bool GetShortestPath(AirsideResource * pResFrom, AirsideResource* pResTo,int nPRI, VehicleRouteInSim& resltRoute);
	void findShortestPath(VehicleRouteNode* pEntry, VehicleRouteNode* pExit);


	bool InitRelationsWithPaxBusParking(CPaxBusParkingResourceManager& paxBusRes);
	bool InitRelationsWithVehiclePool(VehiclePoolResourceManager& poolRes);
	bool InitRelationsWithStand(StandResourceManager& standres);
	bool InitRelationsWithDeice(DeiceResourceManager& deiceRes);

	bool InitIntersectionWithTaxiways(TaxiwayResourceManager& taxiwayRes);
	void InitTaxiwayLanes(TaxiwayResourceManager& taxiRes);
	
	bool InitRelationsWithBagCartsParkingSpot(CBagCartsParkingSpotResourceManager& pBagCartsParkingSpotResManager);	
protected:
	std::vector<VehicleStretchInSim*> m_vStretches;
	std::vector<VehicleRoadIntersectionInSim*> m_vIntersections;	
	
	VehicleRouteNodeSet m_vRouteNodeSet;

	CVehicleRoutePathFinder m_pathFinder;
	
	//std::map< VehicleRouteNodePair, dij_VechileRoute> m_ShortestRoutes;
	VehicleStrethRouteUserDefineInSimList m_userDefineRoutes;
	int m_nPrjId;
	int m_nAptID;

	std::map<std::pair<AirsideResource *, AirsideResource*>, VehicleRouteInSim > m_mapResourceRoute;

};
