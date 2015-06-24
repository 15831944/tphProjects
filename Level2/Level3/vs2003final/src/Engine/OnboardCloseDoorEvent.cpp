#include "StdAfx.h"
#include ".\onboardclosedoorevent.h"
#include "OnboardDoorInSim.h"
#include "ARCportEngine.h"

OnboardCloseDoorEvent::OnboardCloseDoorEvent(OnboardDoorInSim* pDoorInSim)
:m_pOnboardDoorInSim(pDoorInSim)
{
}

OnboardCloseDoorEvent::~OnboardCloseDoorEvent(void)
{
}

int OnboardCloseDoorEvent::process(CARCportEngine* pEngine )
{
	if (m_pOnboardDoorInSim)
		return m_pOnboardDoorInSim->ProcessCloseDoor(time);

	return 0;
}