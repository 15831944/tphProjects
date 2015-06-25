#pragma once
#include "airside\airsidevehicleinsim.h"

class AirsideBaggageTugInSim :
	public AirsideVehicleInSim
{
public:
	AirsideBaggageTugInSim(int id,int nPrjID,CVehicleSpecificationItem *pVehicleSpecItem);
	~AirsideBaggageTugInSim(void);
};
