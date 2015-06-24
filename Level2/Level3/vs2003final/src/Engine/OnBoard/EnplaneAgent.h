#pragma once
/*
	need improvement in the future, add more multi-agent methodology to agent object.
*/

#include "MobileAgent.h"

class carrierEntryDoor;
class carrierEmergencyDoor;
class carrierStair;
class carrierStorage;


class EnplaneAgent : public MobileAgent
{
public:
	EnplaneAgent(MobLogEntry& p_entry, CARCportEngine *pEngine );
	~EnplaneAgent();

	virtual void assignParticularConfig();

    virtual bool findMovementPath();
	virtual ElapsedTime moveTime (void) const;

	carrierEntryDoor* getEntryDoor();
	carrierEmergencyDoor* getEmergencyDoor();
	carrierStair* getStair();
	carrierStorage* getStorage();
    
private:


};
