#pragma once
#include "AirsideFlightInSim.h"
#include "../../InputAirside/VehicleDefine.h"

class Clearance;
class AirsideTowTruckInSim;
class ENGINE_TRANSFER AirsideTowingMobileElement: public AirsideFlightInSim
{
public:
	AirsideTowingMobileElement(AirsideFlightInSim* pFlight, AirsideTowTruckInSim* pVehicle);
	~AirsideTowingMobileElement(void);

	virtual double GetLength() const;
	//virtual void PerformClearance(const Clearance& clearance);
	virtual void PerformClearanceItem(const ClearanceItem& clearanceItem);
	//virtual void UpdateState(const AirsideFlightState& fltState);
	virtual void WakeUp(const ElapsedTime& tTime);

	bool EnterStand(ElapsedTime tParkingTime);
	bool ExitStand();
	bool TowingToDestination();
	bool EnterRunway();
	bool EnterDeicePad();

	void ServiceFinish();

	ElapsedTime GetStandParkingTime();
	TaxiRouteInSim* GetFlightOutboundRoute();

	TOWOPERATIONTYPE GetTowOperationType(){ return m_pTowTruck->GetServiceType(); }
	AirsideResource* GetReleasePoint(){ return m_pTowTruck->GetReleasePoint(); }

	bool IsTowingFinish(){ return m_bServiceFinished; }
	bool IsNeedWaiting(){ return m_bWaiting; }

private:
	AirsideFlightInSim* m_pTowingFlight;
	AirsideTowTruckInSim* m_pTowTruck;
	bool m_bServiceFinished;
	bool m_bWaiting;

};
