#pragma once
#include "AirsideMobileElementEvent.h"

class AirsideFollowMeCarInSim;
class ENGINE_TRANSFER AirsideFollowMeCarServiceEvent :public CAirsideMobileElementEvent
{
public:
	AirsideFollowMeCarServiceEvent(CAirsideMobileElement* pMobileElement);
	~AirsideFollowMeCarServiceEvent(void);

	int Process();

	virtual const char *getTypeName (void) const { return "FollowMeCarService";}
	virtual int getEventType (void) const {return FollowMeCarService;}

protected:
	bool LeadingToDestination();
	void ServiceFinish();

private:
	AirsideFollowMeCarInSim* m_pServiceVehicle;
	bool m_bWaiting;

};
