#pragma once
#include "../../InputAirside/VehicleDefine.h"
#include "../EngineDll.h"
#include "TaxiwayResource.h"
#include "../ARCportEngine.h"

class CTowOperationSpecification;
class CTowOperationSpecFltTypeData;
class AirsideResource;
class StandInSim;
class AirsideFlightInSim;
class CAirportDatabase;
class LogicRunwayInSim;
class IntersectionNodeInSim;
class TowTruckServiceRequest;
class CReleasePoint;
class CTowOperationRoute;
class AirportResourceManager;
class CTowingRoute;
class CTowingRouteList;
class DeicePadInSim;
class VehicleRouteInSim;
class VehicleRouteNodePairDist;
class AirsideVehicleInSim;
class TaxiwayToNearestStretchRouteFinder;

class ENGINE_TRANSFER Push_TowOperationSpecInSim
{
public:
	Push_TowOperationSpecInSim(int nPrjID,AirportResourceManager* pAirportRes, CAirportDatabase* pAirportDB);
	~Push_TowOperationSpecInSim(void);

public:
	//TOWOPERATIONTYPE GetStandOperationType(StandInSim* pStand, AirsideFlightInSim* pFlight);
	TowTruckServiceRequest* GetFlightTowingServiceRequest(AirsideFlightInSim* pFlight,AirsideResource* pCurRes/*, TowTruckServiceRequest* pRequest*/);

	void GetTowingRoute(CReleasePoint* pData,AirsideFlightInSim* pFlight, StandInSim* pOrigin,AirsideResource* pDest, FlightGroundRouteDirectSegList& vSegments);
	bool GetTowtruckReturnRoute(CReleasePoint* pData,AirsideFlightInSim* pFlight, AirsideResource* pOrigin,TowTruckServiceRequest* pRequest);


private:
	CTowOperationSpecFltTypeData* GetFlightTowTruckOperationSpecs(StandInSim* pOriginStand, AirsideFlightInSim* pFlight);

	CReleasePoint* GetTowTruckOperationSpecs(StandInSim* pOriginStand, StandInSim* pDestStand, AirsideFlightInSim* pFlight);
	CReleasePoint* GetTowTruckOperationSpecs(StandInSim* pOriginStand, LogicRunwayInSim* pDepRunway, AirsideFlightInSim* pFlight);
	CReleasePoint* GetTowTruckOperationSpecs(StandInSim* pOriginStand, TaxiInterruptLineInSim* pInterruptLine, AirsideFlightInSim* pFlight);
	CReleasePoint* GetTowTruckOperationSpecs(StandInSim* pOriginStand, DeicePadInSim* pDeicePad, AirsideFlightInSim* pFlight);
	CReleasePoint* GetTowTruckOperationSpecs(StandInSim* pOriginStand, StartPositionInSim* pStartPos, AirsideFlightInSim* pFlight);

	void InitRequestRouteInfo(CReleasePoint* pData,AirsideFlightInSim* pFlight, StandInSim* pOrigin,AirsideResource* pDest, TowTruckServiceRequest* pRequest);
	
	IntersectionNodeInSim* GetRouteItemList(CTowOperationRoute* pRoute, FlightGroundRouteDirectSegList& vRouteItems);
	IntersectionNodeInSim* GetReturnRouteItemList(CTowOperationRoute* pRoute, FlightGroundRouteDirectSegList& vRouteItems);
	void GetRouteItemsFromTowingRoute(CTowingRoute* pGroundRoute, int nIntersectionID, FlightGroundRouteDirectSegList& vRouteItems, int& nLastTaxiwayID);
	void GetReturnRoute(CReleasePoint* pData,AirsideFlightInSim* pFlight,AirsideResource* pOrigin, FlightGroundRouteDirectSegList& vSegments,FlightGroundRouteDirectSegList& vTowSegments);
private:
	AirportResourceManager* m_pAirportRes;
	CTowOperationSpecification* m_pPushTowOperationSpec;
	CTowingRouteList* m_pTowingRouteList;
	TaxiwayToNearestStretchRouteFinder* m_pTaxiwayToStretchRouteFinder;
};
