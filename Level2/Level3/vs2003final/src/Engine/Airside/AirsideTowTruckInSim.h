#pragma once
#include "AirsideVehicleInSim.h"
#include "../../InputAirside/VehicleDefine.h"
#include "VehicleRouteInSim.h"
#include "FlightRouteSegInSim.h"

class ClearanceItem;
class TowTruckServiceRequest;

class ENGINE_TRANSFER AirsideTowTruckInSim : public AirsideVehicleInSim
{
public:
	AirsideTowTruckInSim(int id,int nPrjID,CVehicleSpecificationItem *pVehicleSpecItem);
	~AirsideTowTruckInSim(void);

	void SetServiceType(TOWOPERATIONTYPE _type);
	TOWOPERATIONTYPE GetServiceType();

	void SetServiceFlight(AirsideFlightInSim* pFlight, double vStuffPercent);

	void SetTowTruckServiceRequest(TowTruckServiceRequest* pRequest);
	TowTruckServiceRequest* GetTowTruckServiceRequest();

	bool IsReadyToService(AirsideFlightInSim* pFlight);

	void GetNextCommand();

	void ServicingFlight();
	
	void SetReturnRoute(const VehicleRouteInSim& pReturnRoute);
	void SetReturnToStretch(const FlightGroundRouteDirectSegList& vSegment);

	void SetReleasePoint(AirsideResource* pResource);
	AirsideResource* GetReleasePoint();

	void UpdateState(const ClearanceItem& item);

	virtual void WakeUp(const ElapsedTime& tTime);

	virtual void ReturnVehiclePool(ProbabilityDistribution* pTurnAroundDistribute);
	//void _ChangeToService();
	void GetCompleteReturnRouteToPool();

private:
	bool GetRouteToServiceStand( StandInSim* pServiceStand, VehicleRouteInSim& resultRoute );

private:
	TOWOPERATIONTYPE m_ServiceType;
	VehicleRouteInSim m_ReturnRoute;
	FlightGroundRouteDirectSegList m_vSegments;//get return route the same as complement return pool
	AirsideResource* m_pReleasePoint;
	TowTruckServiceRequest* m_pServiceRequest;
};
