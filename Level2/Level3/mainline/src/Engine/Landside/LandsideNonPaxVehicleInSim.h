#pragma once

#include <list>
#include "..\LandsideVehicleInSim.h"
#include "..\LandsideVehicleEntryInfo.h"

class ResidentVehicleRoute;
class ResidentVehicleRouteList;
class NonResidentVehicleTypePlan;


//////////////////////////////////////////////////////////////////////////
class ResidentVehicleRoute;
class LandsideNonPaxVehicleInSim : public LandsideVehicleInSim
{
public:
	LandsideNonPaxVehicleInSim(const NonPaxVehicleEntryInfo& entryInfo);
	virtual ResidentVehiclePlan* getRoutePlan(){ return m_entryInfo.pPlan; }

	bool InitBirth(CARCportEngine*pEngine);
	virtual void SuccessParkInLotSpot(LandsideParkingSpotInSim* plot);
	virtual bool ProceedToNextFcObject(CARCportEngine* pEngine);

	NonPaxVehicleEntryInfo& getEntryInfo(){ return m_entryInfo; }
	ElapsedTime GetParkingLotWaitTime(CARCportEngine* pEngine,const ALTObjectID& parkLot);

protected:
	NonPaxVehicleEntryInfo m_entryInfo;
};
