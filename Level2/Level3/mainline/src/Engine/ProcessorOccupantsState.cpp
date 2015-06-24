#include "StdAfx.h"
#include "ProcessorOccupantsState.h"

ProcessorOccupantsState::ProcessorOccupantsState(void)
{
}

ProcessorOccupantsState::~ProcessorOccupantsState(void)
{
}

int ProcessorOccupantsState::GetCount()
{
	return (int)m_mapOccupants.size();
}


void ProcessorOccupantsState::AddMobileElementOccupant(int nMobElementTypeIndex, int nOccupants)
{
	OccupantsIter iter = m_mapOccupants.find(nMobElementTypeIndex);
	if (iter == m_mapOccupants.end())
		m_mapOccupants.insert( std::make_pair(nMobElementTypeIndex, nOccupants));
	else
		iter->second += nOccupants;
}
