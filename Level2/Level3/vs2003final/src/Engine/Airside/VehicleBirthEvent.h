#pragma once
#include "AirsideMobileElementEvent.h"

class AirsideVehicleInSim;
class ENGINE_TRANSFER VehicleBirthEvent : public CAirsideMobileElementEvent
{
public:
	explicit VehicleBirthEvent(AirsideVehicleInSim* vehicle);
	~VehicleBirthEvent(void);

	virtual int Process();
	
	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "VehicleBirth";}

	virtual int getEventType (void) const {return VehicleBirth;}
	
private:
	AirsideVehicleInSim* m_pVehicle;
};
