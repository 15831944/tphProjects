#pragma once
#include "EngineDll.h"
#include "Common\ACTypeDoor.h"
class CAirportDatabase;
class FlightConstraint;
class ALTObjectID;
class ACTypeDoor;
class OperatingDoorSpec;

struct ACTypeDoorOpen
{
	ACTypeDoor* pDoor;
	ACTypeDoor::DoorDir openSide;
	int index;
	bool operator<(const ACTypeDoorOpen& other)const;
};

struct OpenDoorsOrder
{
	bool operator()(ACTypeDoor* door1, ACTypeDoor*door2);
};

class StandOperatingDoorData;
class ENGINE_TRANSFER FltOperatingDoorSpecInSim
{
public:
	FltOperatingDoorSpecInSim(CAirportDatabase* pAirportDB);
	~FltOperatingDoorSpecInSim(void);

	//void getFlightOpDoors(const FlightConstraint& fltcont,const ALTObjectID& standName, std::vector<ACTypeDoor*>& vOpenDoors);

	std::vector<ACTypeDoorOpen> getFlightOpenDoors(const FlightConstraint& fltcont,const ALTObjectID& standName)const;
	std::vector<ACTypeDoorOpen> getAllFlightDoors(const FlightConstraint& fltcont)const;

private:
	OperatingDoorSpec* m_pOperatingDoorSpec;
	CAirportDatabase* m_pAirportDB;

	StandOperatingDoorData* _getFitData( const FlightConstraint& fltcont,const ALTObjectID& standName)const;
};
