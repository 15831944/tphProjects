#pragma once
// handler person query processor's capacity
// need	Initialization when runSimulation
//

#include "ProcessorOccupantsState.h"
#include <algorithm>
#include <map>

class MobileElement;
class Person;
class MiscData;
class CapacityAttribute;
class CapacityAttributes;

class ProcessorCapacity;
class SortedMobileElementList;

class Terminal;



class ProcessorCapacityInSimulation
{
	class ProcessorCombinationCapactity
	{
	public:

		ProcessorCombinationCapactity()
		{
			m_nCapacity = 0;
		}
		//Capacity
		int m_nCapacity;

		//the combination types
		std::vector<int> m_vMobType;

	};

public:
	ProcessorCapacityInSimulation();
	~ProcessorCapacityInSimulation(void);

	typedef std::map<int, int> CapacitiesMap;
	typedef CapacitiesMap::const_iterator CapacitiesMapIter;

	void Initialization(Terminal *pTernimal,ProcessorCapacity* pProcCapacity);

	bool IsVacant(MobileElement* pMobElement,
		ProcessorOccupantsState& currentProcessorOccupants, 
		ProcessorOccupantsState& queristIncreasedOccupants) const;

private:

	void Clear();
	CapacityAttributes* m_pAttributes;
	
	//store mobile element capacity informations , detected 
	CapacitiesMap m_mapMobElementCapacity;

	std::vector<ProcessorCombinationCapactity *> m_vCombinationCapacity;
	
	int m_nAllNonPaxCapacity;
};
