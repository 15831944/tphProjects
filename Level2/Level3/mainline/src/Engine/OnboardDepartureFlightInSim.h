#pragma once
#include "onboardflightinsim.h"
class OnboardDepartureFlightInSim :
	public OnboardFlightInSim
{
public:
	OnboardDepartureFlightInSim(Flight *pFlight, COnboardFlight *pOnboardFlight);
	~OnboardDepartureFlightInSim(void);
};

