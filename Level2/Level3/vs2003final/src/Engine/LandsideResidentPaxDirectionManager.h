#pragma once

#include "Landside/LandsideResidentVehilceRoute.h"
class ResidentRelatedVehiclTypeContainer;
class LandsideResourceManager;
class LandsideBusStationInSim;
class ResidentVehicleRoute;
class ResidenRelatedtVehicleTypeData;
class ResidentRelatedVehicleTypePlan;
class LandsideLayoutObjectInSim;


class LandsideResidentPaxDirectionManager
{
public:
	LandsideResidentPaxDirectionManager(void);
	~LandsideResidentPaxDirectionManager(void);

public:

	void initialize(ResidentRelatedVehiclTypeContainer*  plstResidentFlow, LandsideResourceManager* pResourceManager);

	std::map<int, LandsideBusStationInSim *> m_mapBusStation;

protected:
	void ProcessVehicleType( ResidenRelatedtVehicleTypeData* pVehicleTypeData, LandsideResourceManager* pResourceManager );
	void ProcessVehicleTypePlan(ResidentRelatedVehicleTypePlan* pVehicleTypePlan, LandsideResourceManager* pResourceManager);
	void ProcessRouteData(ResidentVehicleRoute *pOriginalRoute,ResidentVehicleRouteFlow* pVehicleRouteFlow, LandsideResourceManager* pResourceManager, std::vector<LandsideLayoutObjectInSim *>& vBusStationDirections,std::vector<LandsideFacilityObject> vFacilityObject,ResidentRelatedVehicleTypePlan* pVehicleTypePlan );



};
