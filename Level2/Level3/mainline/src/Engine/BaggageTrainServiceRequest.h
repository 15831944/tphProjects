#pragma once
#include "Airside\VehicleServiceRequest.h"
#include "Airside\AirsideFlightInSim.h"


class Processor;
class CBagCartsParkingSpotInSim;


class BaggageTrainServiceRequest :
	public VehicleServiceRequest
{
public:
	BaggageTrainServiceRequest(void);
	~BaggageTrainServiceRequest(void);


public:
	void setBaggageCount(int nTotalBagNeedToHandle);

	void setUnserviceBaggageCount(int nLeft);

	int GetBaggageLeft() const;

	virtual bool IsCompleteService();
	virtual enumVehicleBaseType GetType()const;

	FlightOperation getFltOperation() const;
	void setFltOperation(FlightOperation enumOp);

//	bool IsArrival();

	void SetTermProcList(std::vector<Processor *>& vTermProcs);
	void SetPakringSpotList(std::vector<CBagCartsParkingSpotInSim *>& vParkingSpot);
	
	Processor *getTermPocessor();
	CBagCartsParkingSpotInSim *getParkingSpot();
protected:
	//the count of baggage need to handle
	int m_nBaggageCount;
	
	//the count of baggage which has no serviced
	int m_nBaggageLeft;
	
	//flight operation
	FlightOperation m_enumFltOperation;

	//the data in those 2 vector should be in pair
	std::vector<Processor *> m_vTermProcs;
	std::vector<CBagCartsParkingSpotInSim *> m_vParkingSpot;

};
