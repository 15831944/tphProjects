#pragma once
class CAirportDatabase;
#include "InputAirside/SurfacesInitalEvent.h"
class AirsideFlightInSim;
class ElapsedTime;
#include "InputAirside/AircraftDeicingAndAnti_icingsSettings.h"

class CAircraftInitSurfaceSettingInSim
{
public:
	CAircraftInitSurfaceSettingInSim(CAirportDatabase* pArpDb);

	~CAircraftInitSurfaceSettingInSim();

	//get flight surface set at the time
	AircraftSurfacecondition getFlightSurfaceCondition(AirsideFlightInSim* pFlight, const ElapsedTime& time);


	CAircraftSurfaceSetting m_settingInput;
};
