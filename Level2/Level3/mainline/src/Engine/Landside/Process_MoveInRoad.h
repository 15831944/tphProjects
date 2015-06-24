#pragma once


#include "../LandsideVehicleInSim.h"
#include "..\core\SGoal.h"
#include "..\LandsideVehicleInSim.h"
#include "Goal_LandsideVehicle.h"



class Process_MoveToDest  : public LandsideVehicleGoal
{
public:
	Process_MoveToDest(LandsideVehicleInSim* pEnt, LandsideLayoutObjectInSim* pDest );	
	void Process(CARCportEngine* pEngine);
	virtual void Activate(CARCportEngine* pEngine);
	
	
	//


	MovingTrace m_trace;

	
protected:
	LandsideLayoutObjectInSim* mpDest;
	LandsideLaneNodeInSim* m_pNextNode;
};