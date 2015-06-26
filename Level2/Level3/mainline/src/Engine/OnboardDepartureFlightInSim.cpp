#include "StdAfx.h"
#include "OnboardDepartureFlightInSim.h"


OnboardDepartureFlightInSim::OnboardDepartureFlightInSim(Flight *pFlight, COnboardFlight *pOnboardFlight)
	:OnboardFlightInSim(pFlight, false, pOnboardFlight)
{
}


OnboardDepartureFlightInSim::~OnboardDepartureFlightInSim(void)
{
}
