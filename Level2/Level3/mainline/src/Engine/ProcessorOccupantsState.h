#pragma once

#include <map>

//store Processor's Occupants information in certain time
//
class ProcessorOccupantsState
{
public:
	ProcessorOccupantsState(void);
	~ProcessorOccupantsState(void);

public:
	//key: mobile element type index, occupants count
	typedef std::map<int,int> Occupants;
	typedef Occupants::iterator OccupantsIter;

	int GetCount();
	Occupants& GetOccupants(){	return m_mapOccupants; }

	void AddMobileElementOccupant(int nMobElementTypeIndex, int m_nOccupants = 1);

private:
	Occupants m_mapOccupants;
};
