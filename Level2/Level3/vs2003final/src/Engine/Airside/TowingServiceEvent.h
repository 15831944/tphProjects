#pragma once
#include "AirsideMobileElementEvent.h"

class AirsideTowTruckInSim;
class ENGINE_TRANSFER TowingServiceEvent: public CAirsideMobileElementEvent
{
public:
	TowingServiceEvent(CAirsideMobileElement* pMobileElement);
	~TowingServiceEvent(void);

	int Process();

	virtual const char *getTypeName (void) const { return "TowTruckService";}
	virtual int getEventType (void) const {return TowTruckService;}

protected:
	bool EnterStand();
	bool ExitStand();
	bool TowingToDestination();
	bool EnterRunway();
	bool EnterDeicePad();
	void ServiceFinish();

protected:
	AirsideTowTruckInSim* m_pTowTruck;
	bool m_bWaiting;
};
