#pragma once
#include "onboardflightinsim.h"
class OnboardArrivalFlightInSim :
	public OnboardFlightInSim
{
public:
	OnboardArrivalFlightInSim(Flight *pFlight, COnboardFlight *pOnboardFlight);
	~OnboardArrivalFlightInSim(void);
};

