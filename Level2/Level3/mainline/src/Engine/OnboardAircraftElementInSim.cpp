#include "StdAfx.h"
#include ".\onboardaircraftelementinsim.h"

OnboardAircraftElementInSim::OnboardAircraftElementInSim(OnboardFlightInSim* pFlightInSim)
:m_pFlightInSim(pFlightInSim)
{
	
}

OnboardAircraftElementInSim::~OnboardAircraftElementInSim(void)
{
}

OnboardCellInSim * OnboardAircraftElementInSim::GetEntryCell() const
{
	return NULL;
}

void OnboardAircraftElementInSim::NotifyPaxLeave( OnboardCellInSim *pCell, const ElapsedTime& eTime  )
{
	ASSERT(0);
}