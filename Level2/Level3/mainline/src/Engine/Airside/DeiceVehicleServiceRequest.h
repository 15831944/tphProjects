#pragma once
#include ".\VehicleServiceRequest.h"
#include ".\DeiceVehicleInSim.h"
#include "AirsideFlightDeiceRequirment.h"

class DeiceVehicleServiceRequest : public VehicleServiceRequest
{
public:
	DeiceVehicleServiceRequest(AirsideFlightInSim* pFlight,const FlightDeiceRequirment& deiceRequir,const ElapsedTime& requestT);
	virtual DeiceVehicleServiceRequest* toDeiceRequest(){ return this; };
	enumVehicleBaseType GetType()const{ return VehicleType_DeicingTruck; } 
	
	//the request has been fulfilled
	bool isRequestHandled()const;
	
	void AssignVehicle(DeiceVehicleInSim* pVehicle);
	FlightDeiceRequirment mDeiceRequir;

	int getVehicleCount()const{ return (int)m_vServiceVehicle.size(); }
	DeiceVehicleInSim* getVehicle(int idx)const;	

	int geVehicleIndex(AirsideVehicleInSim* pVehicle)const;

	//call all vehicle start deice
	void flightCallDeice(const ElapsedTime& t);

	//call all Vehicle start antiice
	void flightCallAntiIce(const ElapsedTime& t);

	virtual ElapsedTime getRequestTime() { return m_requestTime; }

	virtual bool IsCompleteService();
protected:
	ElapsedTime m_requestTime;
	double m_dDeiceFluidUnHandled;
	double m_dAntiFliudUnHandled;
	int m_nDeiceCount;
	int m_nAntiIceCount;
};


class InspectionVehicleServiceRequest  : public VehicleServiceRequest
{
public:
	InspectionVehicleServiceRequest(AirsideFlightInSim* pFlight)
	{
		SetServiceFlight(pFlight);
	}

	enumVehicleBaseType GetType()const{ return VehicleType_InspectionTruck; }


};

