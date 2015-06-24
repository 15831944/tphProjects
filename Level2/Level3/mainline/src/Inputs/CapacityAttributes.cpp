#include "StdAfx.h"
#include "CapacityAttributes.h"

#include "in_term.h"

CapacityAttributes::CapacityAttributes(void)
{
}

CapacityAttributes::~CapacityAttributes(void)
{
	Clear();
}

CapacityAttribute* CapacityAttributes::GetItem(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < (int)m_vectAttribute.size());
	return m_vectAttribute[nIndex];
}

void CapacityAttributes::UpdateCapacityAttribute(CapacityAttribute* nMobElement,  int nCapacity)
{
	AttributesIter iter = m_vectAttribute.begin();
	while (iter != m_vectAttribute.end())
	{
		if ((*iter) == nMobElement)
		{
			if (nCapacity < 1)
			{
				delete (*iter);
				m_vectAttribute.erase(iter);
				return;
			}
			else
				(*iter)->m_nCapacity = nCapacity;		
			return;
		}
		iter++;
	}
}

void CapacityAttributes::RemoveCapacityAttribute(CapacityAttribute* nMobElement)
{
	/* ASSERT(nMobElementIndex < ?);*/

	AttributesIter iter = m_vectAttribute.begin();
	while (iter != m_vectAttribute.end())
	{
		if ((*iter) == nMobElement)
		{
			delete (*iter);
			m_vectAttribute.erase(iter);
			return;
		}
		iter++;
	}
}

void CapacityAttributes::Clear()
{
	int n = m_vectAttribute.size();
	for (int i=0; i<n; i++){
		delete m_vectAttribute[i];
	}
	m_vectAttribute.clear();
}
