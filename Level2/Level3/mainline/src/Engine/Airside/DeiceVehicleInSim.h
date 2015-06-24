#pragma once

#include ".\AirsideVehicleInSim.h"
class DeiceVehicleInSim : public AirsideVehicleInSim
{
public:
	DeiceVehicleInSim(int id,int nPrjID,CVehicleSpecificationItem *pVehicleSpecItem);
	void ResetServiceCapacity();
	
	//vehicle assign the flight to service
	void SetServiceFlight(AirsideFlightInSim* pFlight, double vStuffPercent);

	//call when vehicle run off the road and to service
	void _ChangeToService();
	void _continueService();
	void ServiceFlight(const ElapsedTime& tTime);

	double getFluidAvaiable()const{ return m_fluidleft; }
	void decreaseFluid(double d){ m_fluidleft-=d; }

	
	double m_fluidleft;

	enum ServState{ 
		NoService,
		OnEnterDeiceArea,//first state
		WaitForFlightAtTemp, // stay at the deice area wait for flight come
		ReadyForService, //wait for multi vehicle service
		ServiceStart,  // service start
		ServiceEnd, //service end
	}; 	
	enum ServType
	{
		DeIce,
		AntiIce,
	};
	void SetServType(ServType t){
		mServType = t;
	}
	void SetServFluid(double d){ mdServFluid =d; }
	ServState mServState;
	ServType mServType;
	double mdServFluid;
};
#pragma once

