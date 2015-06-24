#include "StdAfx.h"
#include ".\landsidepaxsericeinsim.h"
#include "LandsideBaseBehavior.h"
#include "PaxLandsideBehavior.h"
#include "PERSON.H"
#include "ARCportEngine.h"

LandsidePaxSericeInSim::LandsidePaxSericeInSim(void)
{
}

LandsidePaxSericeInSim::~LandsidePaxSericeInSim(void)
{
}

void LandsidePaxSericeInSim::FlushOnVehiclePaxLog(CARCportEngine* pEngine, const ElapsedTime& t )
{
	for(int i=0;i<(int)m_vWaitingPax.size();++i)
	{
		Person* p = (Person*) pEngine->m_simBobileelemList.GetAvailableElement(m_vWaitingPax.at(i));//[i];
	
		if(p->getState()!=Death)
		{
			p->flushLog(t);
			p->setState(Death);
		}		
	}
}

void LandsidePaxSericeInSim::AddWaitingPax( int PaxId )
{
	PaxList::const_iterator itr = std::find(m_vWaitingPax.begin(),m_vWaitingPax.end(),PaxId);
	if(itr==m_vWaitingPax.end())
		m_vWaitingPax.push_back(PaxId);
	
}

void LandsidePaxSericeInSim::DelWaitingPax( int PaxId )
{
	PaxList::iterator itr = std::find(m_vWaitingPax.begin(),m_vWaitingPax.end(),PaxId);
	if(itr!=m_vWaitingPax.end())
		m_vWaitingPax.erase(itr);
}

int LandsidePaxSericeInSim::GetWaitingPaxNum() const
{
	return m_vWaitingPax.size();
}
