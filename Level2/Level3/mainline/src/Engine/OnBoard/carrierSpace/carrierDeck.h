#pragma once
#include "Common/containr.h"
#include "carrierGround.h"

class LocatorSite;
class MobAgentMovementPath;
class MobileAgent;
class carrierDeck
{
public:
	carrierDeck(void);
	~carrierDeck(void);

	bool isValid();
	carrierGround& getCarrierGround();

	bool getMovementPath(MobileAgent* pMobAgent, MobAgentMovementPath& _movePath);
	
private:
	carrierGround mGround;
};




