#pragma once
#include "EngineDll.h"
class CAirportDatabase;
class FlightConstraint;
class ALTObjectID;
class ACTypeDoor;
class OperatingDoorSpec;
class ENGINE_TRANSFER FltOperatingDoorSpecInSim
{
public:
	FltOperatingDoorSpecInSim(CAirportDatabase* pAirportDB);
	~FltOperatingDoorSpecInSim(void);

	void getFlightOpDoors(const FlightConstraint& fltcont,const ALTObjectID& standName, std::vector<ACTypeDoor*>& vOpenDoors);
private:
	OperatingDoorSpec* m_pOperatingDoorSpec;
	CAirportDatabase* m_pAirportDB;
};
