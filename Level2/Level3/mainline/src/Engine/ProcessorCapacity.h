#pragma once

#include <vector>
#include "../inputs/CapacityAttributes.h"

class MobileElementCapacityItem
{
public:
	int m_nMobElementTypeIndex;
	int m_nCapacity;
	CapacityAttributes::CapacityType m_Type ;
	CString m_Name ;
	std::vector<CString> m_ContainData ;
};

class ProcessorCapacity
{
public:
	ProcessorCapacity();
	~ProcessorCapacity(void);
	
	void Initialization(CapacityAttributes* pAttributes);

	int GetCount()const {	return (int)m_vectMobElementCapacity.size(); }
	MobileElementCapacityItem GetItem(int nIndex);
	void Clear();
	

	std::vector<MobileElementCapacityItem> m_vectMobElementCapacity;
};
