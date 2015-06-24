#pragma once
/*
	need improvement in the future, add more multi-agent methodology to agent object.
*/

#include "MobileAgent.h"

class carrierExitDoor;
class carrierEmergencyDoor;
class carrierStair;
class carrierStorage;
class DeplaneAgent: public MobileAgent
{
public:
	DeplaneAgent(MobLogEntry& p_entry, CARCportEngine* _pEngine );
	~DeplaneAgent();

	virtual void assignParticularConfig();

	virtual bool findMovementPath();

	carrierExitDoor* getExitDoor();
	carrierEmergencyDoor* getEmergencyDoor();
	carrierStair* getStair();
	carrierStorage* getStorage();
};