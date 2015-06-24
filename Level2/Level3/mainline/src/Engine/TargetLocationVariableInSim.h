#pragma once

#include "../inputonboard/targetlocations.h"


class TargetLocationVariableInSim
{
public:
	TargetLocationVariableInSim(void);
	~TargetLocationVariableInSim(void);

public:

	void Initialize(int nProjID, StringDictionary *pStrDict);

	//+, 0, -
	double GetOffsetDist(const CPassengerType& paxType) const;
	
	double GetSearchRadius(const CPassengerType& paxType) const;

protected:
	CTargetLocationsList m_targetLocation;


};
