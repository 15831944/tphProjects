#include "StdAfx.h"
#include ".\landsidepaxsericeinsim.h"
#include "LandsideBaseBehavior.h"
#include "PaxLandsideBehavior.h"

LandsidePaxSericeInSim::LandsidePaxSericeInSim(void)
{
}

LandsidePaxSericeInSim::~LandsidePaxSericeInSim(void)
{
}

void LandsidePaxSericeInSim::FlushOnVehiclePaxLog( const ElapsedTime& t )
{
	for(int i=0;i<m_vWaitingPax.GetCount();++i)
	{
		PaxLandsideBehavior* pLandsidepax = m_vWaitingPax.at(i);//[i];
		pLandsidepax->terminate(t);
	}
}
