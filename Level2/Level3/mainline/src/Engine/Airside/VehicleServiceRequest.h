#pragma once
#include "common/elaptime.h"
#include "Common/ProbabilityDistribution.h"
#include "../../InputAirside/VehicleSpecifications.h"
class AirsideFlightInSim;
class AirsideVehicleInSim;

class DeiceVehicleServiceRequest;

class VehicleServiceRequest
{
public:
	VehicleServiceRequest(void);
	virtual ~VehicleServiceRequest(void);

	virtual DeiceVehicleServiceRequest* toDeiceRequest(){ return NULL; }
	
	void SetServiceFlight(AirsideFlightInSim* pFlight){ m_pServiceFlight = pFlight;}
	AirsideFlightInSim* GetServiceFlight(){ return m_pServiceFlight;}
	void SetServiceVehicleTypeID(int nID){ m_nServiceVehicleTypeID = nID;}
	int GetServiceVehicleTypeID(){ return m_nServiceVehicleTypeID;}


	void SetServiceCount(int nCount){ m_nServiceCount = nCount;}
	int GetServiceCount(){ return m_nServiceCount;}
	void DecreaseServiceCount(int n=1){ m_nServiceCount-=n; }
	void SetServiceTime(ProbabilityDistribution* pTimeDistribution){ m_pServiceTimeDistribution = pTimeDistribution;}
	ProbabilityDistribution* GetServiceTime(){ return m_pServiceTimeDistribution;}
	void SetSubServiceTime(ProbabilityDistribution* pTimeDistribution){ m_pSubServiceTimeDistribution = pTimeDistribution;}
	ProbabilityDistribution* GetSubServiceTime(){ return m_pSubServiceTimeDistribution;}

	void AddServiceVehicle(AirsideVehicleInSim * pVehicle);
	virtual void RemoveServiceVehicle(AirsideVehicleInSim *pVehicle);
	bool bHasVehicle(AirsideVehicleInSim* pVehicle)const;
	int GetServiceVehicleCount()const{ return (int)m_vServiceVehicle.size(); }
	void WakeupAllPaxBusCompleteService(AirsideFlightInSim* pFlight);

	virtual enumVehicleBaseType GetType()const=0;
	virtual bool IsCompleteService();	
	//need to be sort by this time
	virtual ElapsedTime getRequestTime();	

	void SetProceed(bool bProceed);

	bool IsProceed() const;


	static bool OrderLess(VehicleServiceRequest* q1, VehicleServiceRequest* q2);

protected:
	AirsideFlightInSim* m_pServiceFlight;
	//the vehicle dispatched 
	std::vector<AirsideVehicleInSim *> m_vServiceVehicle; // assigned service vehicle
	int m_nServiceVehicleTypeID;
	ProbabilityDistribution* m_pServiceTimeDistribution;
	ProbabilityDistribution* m_pSubServiceTimeDistribution;
	int m_nServiceCount;  //unserviced count left //unassigned vehicle service point
	bool m_bProceed;//check the request has been proceed or not, TRUE, yes
};

class GeneralVehicleServiceRequest : public VehicleServiceRequest
{
public:
	GeneralVehicleServiceRequest();
	virtual enumVehicleBaseType GetType()const{ return VehicleType_General; }	
};


