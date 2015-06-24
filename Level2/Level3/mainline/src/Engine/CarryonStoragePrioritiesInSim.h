#pragma once

#include "../InputOnBoard/CarryonStoragePriorities.h"


class FlightCarryonPriorityInSim
{
public:
	FlightCarryonPriorityInSim(CFlightCarryonStoragePriorities *pPriority);
	~FlightCarryonPriorityInSim();

	
	int GetDevicePriority(const CPassengerType& paxType, int nTypeIndex) const;

protected:

protected:
	CFlightCarryonStoragePriorities *m_pPriority;
private:
};


class CarryonStoragePrioritiesInSim
{
public:
	CarryonStoragePrioritiesInSim(void);
	~CarryonStoragePrioritiesInSim(void);

public:
	void Initialize(StringDictionary *pStrDict);

	CFlightCarryonStoragePriorities* GetDevicePriority(int nOnboardFltID);

protected:
	CCarryonStoragePriorities m_carryonPriority;
};
