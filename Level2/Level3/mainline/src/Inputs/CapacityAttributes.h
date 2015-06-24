#pragma once
#include <vector>
#include <algorithm>
#include "MobileElemTypeStrDB.h"



//class to handler Processor's Capacity for different mobileElement type
class InputTerminal;

class CapacityAttribute;
class CapacityAttributes
{
public:
	enum CapacityType { TY_Normal,TY_AllNO_PAX,TY_Combination};
public:
	CapacityAttributes(void);
	~CapacityAttributes(void);
	
	typedef std::vector<CapacityAttribute*> Attributes;
	typedef std::vector<CapacityAttribute*>::iterator AttributesIter;

	int GetCount(){	return (int)m_vectAttribute.size(); }
	CapacityAttribute* GetItem(int nIndex);

	void Clear();
	
	void AddCapacityAttribute(CapacityAttribute* pAttr){	m_vectAttribute.push_back(pAttr); }

	void RemoveCapacityAttribute(CapacityAttribute* nMobElement);

	void UpdateCapacityAttribute(CapacityAttribute* nMobElement,  int nCapacity);

private:
	Attributes m_vectAttribute;
};
class CapacityAttribute
{
public:
	CapacityAttribute():m_nMobElementIndex(-1), m_nCapacity(-1),m_Type(CapacityAttributes::TY_Normal){}
	int     m_nMobElementIndex;
	int     m_nCapacity;
	CapacityAttributes::CapacityType m_Type ;
	CString m_Name ;
	std::vector<CString> m_ContainData ;
public:
	void AddCombinationContainTypeString(CString& _str)
	{
		if(std::find(m_ContainData.begin(),m_ContainData.end(),_str) == m_ContainData.end())
			m_ContainData.push_back(_str) ;
	}
	void RemoveCombinationContainTypeString(CString& _str)
	{
		std::vector<CString>::iterator iter = std::find(m_ContainData.begin(),m_ContainData.end(),_str) ;
		if(iter != m_ContainData.end())
			m_ContainData.erase(iter) ;
	}
};