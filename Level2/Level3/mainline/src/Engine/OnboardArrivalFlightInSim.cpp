#include "StdAfx.h"
#include "OnboardArrivalFlightInSim.h"


OnboardArrivalFlightInSim::OnboardArrivalFlightInSim(Flight *pFlight, COnboardFlight *pOnboardFlight)
	:OnboardFlightInSim(pFlight, true, pOnboardFlight)
{
}


OnboardArrivalFlightInSim::~OnboardArrivalFlightInSim(void)
{
}
