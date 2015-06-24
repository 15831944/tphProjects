#include "StdAfx.h"
#include "ProcessorCapacity.h"

ProcessorCapacity::ProcessorCapacity()
{
}



ProcessorCapacity::~ProcessorCapacity(void)
{
	Clear();
}

void ProcessorCapacity::Initialization(CapacityAttributes* pAttributes)
{
	Clear();

	for (int i=0; i< pAttributes->GetCount(); i++)
	{
		MobileElementCapacityItem newItem;
		newItem.m_nMobElementTypeIndex = pAttributes->GetItem(i)->m_nMobElementIndex;
		newItem.m_nCapacity = pAttributes->GetItem(i)->m_nCapacity;
		newItem.m_Type =  pAttributes->GetItem(i)->m_Type ;
		for (int j = 0 ;j < (int)pAttributes->GetItem(i)->m_ContainData.size() ;j++)
		{
			newItem.m_ContainData.push_back(pAttributes->GetItem(i)->m_ContainData[j]) ;
		}
		m_vectMobElementCapacity.push_back(newItem);
	}

}
MobileElementCapacityItem ProcessorCapacity::GetItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vectMobElementCapacity.size());

	return m_vectMobElementCapacity[nIndex];
}

void ProcessorCapacity::Clear()
{
	m_vectMobElementCapacity.clear();
}