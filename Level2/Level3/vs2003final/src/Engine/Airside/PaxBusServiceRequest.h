#pragma once
#include "vehicleservicerequest.h"

class AirsideVehicleInSim;

class CPaxBusServiceRequest :
	public VehicleServiceRequest
{
public:
	CPaxBusServiceRequest(void);
	~CPaxBusServiceRequest(void);

public:
	void SetArrival(bool bArrival);
	bool IsArrival();


	void SetPaxCount(int nCount);
	int GetAllPaxCount();

	int GetLeftPaxCount();
	void ServicedPaxCount(int nServicePaxCount);

	virtual bool IsCompleteService();
	virtual enumVehicleBaseType GetType()const{ return VehicleType_PaxTruck; };


protected:
	bool m_bArrival;

	int m_nPaxCount;

	

	int m_nPaxLoad;

	int m_nLeftPaxCount;

};
