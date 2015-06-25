#pragma once
#include "ResourceManager.h"
#include "RunwayResourceManager.h"
#include "TaxiwayResourceManager.h"
#include "StandResourceManager.h"
#include "DeiceResourceManager.h"
#include "../../Common/point.h"
#include "../../InputAirside/ALTAirport.h"
#include "VehiclePoolResourceManager.h"
#include "VehicleRouteResourceManager.h"
//#include "VehicleStrechRouteInSim.h"
#include "TakeoffQueueInSim.h"
#include "PaxBusParkingResourceManager.h"
#include "FlightGroundRouteResourceManager.h"
#include "../BagCartsParkingSpotResourceManager.h"


class AirsideSimConfig;
class AirsideMeetingPointInSim;
class TaxiwayToNearestStretchRouteFinder;
class AircraftClassificationManager;
class StartPositionInSim;
class ENGINE_TRANSFER AirportResourceManager : public ResourceManager
{
friend class AirportController;

public:
	AirportResourceManager();
	~AirportResourceManager();

	//Initialize all Resources in the Airport; 

	bool Init(int projID, int nAirportID,CAirportDatabase* pAirportDB, AircraftClassificationManager* pAircraftClassification, const AirsideSimConfig& simconf);

	bool InitGroundRouteResourceManager(TemporaryParkingInSim* tempParkingsegs);
	
	void InitMeetingPoints(int projID, int nAirportID);
	void InitStartPositions(int nPrjID, int nAirportID);


	RunwayResourceManager* getRunwayResource(){ return &m_RunwayRes; }
	TaxiwayResourceManager* getTaxiwayResource(){ return &m_TaxiwayRes; }
	StandResourceManager* getStandResource(){ return &m_StandRes; }
	DeiceResourceManager* getDeiceResource(){ return &m_DeiceRes; }
	IntersectionNodeInSimList& GetIntersectionNodeList(){ return m_vIntersectionNodes; }
	VehiclePoolResourceManager* getVehicleResource(){ return & m_VehiclePoolRes; }
	VehicleRouteResourceManager * getVehicleRouteRes(){ return &m_VehicleRouteRes; }
	TakeoffQueuesManager * getTakeoffQueuesManager(){ return &m_TakeoffQueueManager; } 
	
	CPaxBusParkingResourceManager * getPaxParkingResouceManager(){ return & m_paxParkingResouceManager;}

	FlightGroundRouteResourceManager *getGroundRouteResourceManager();

	CBagCartsParkingSpotResourceManager *getBagCartsParkingSpotResManager();


	ALTAirport& GetAirportInfo(){ return m_AirportInfo; }

	TaxiwayToNearestStretchRouteFinder* GetTaxiwayToNearestStretchRouteFinder();
	void GetVehicleReturnRouteFromTaxiRoute( const FlightGroundRouteDirectSegList& vSegments, std::vector<VehicleRouteNodePairDist>& vNodePairs);
	void GetVehiclePartOfReturnRouteFromTaxiRoute(const FlightGroundRouteDirectSegList& vSegments, std::vector<VehicleRouteNodePairDist>& vNodePairs);
	void GetVehicleLastRouteFromTaxiRoute( const FlightGroundRouteDirectSegList& vSegments,std::vector<VehicleRouteNodePairDist>& vNodePairs);

	const TaxiInterruptLineInSimList& GetTaxiInterruptLineList(){ return m_InterruptLineList; }
	void GetStartPosition(const ALTObjectID& objID, std::vector<StartPositionInSim*>& vStartPos);

protected:

	IntersectionNodeInSimList m_vIntersectionNodes;
	TaxiInterruptLineInSimList m_InterruptLineList;
	std::vector<AirsideMeetingPointInSim*> m_vMeetingPoints;
	std::vector<StartPositionInSim*> m_vStartPositions;
	ALTAirport m_AirportInfo;

	RunwayResourceManager m_RunwayRes;
	TaxiwayResourceManager m_TaxiwayRes;
	StandResourceManager m_StandRes;
	DeiceResourceManager m_DeiceRes;
	VehiclePoolResourceManager m_VehiclePoolRes;
	VehicleRouteResourceManager m_VehicleRouteRes;
	//VehicleStretchRouteInSim m_VehicleStretchRouteInSim;
	TakeoffQueuesManager m_TakeoffQueueManager;

	CPaxBusParkingResourceManager m_paxParkingResouceManager;

	FlightGroundRouteResourceManager m_flightGroundRouteManager;

	TaxiwayToNearestStretchRouteFinder* m_pTaxiwayToNearestStretchRouteFinder;

	CBagCartsParkingSpotResourceManager m_pBagCartsParkingSpotResManager;
	
};
