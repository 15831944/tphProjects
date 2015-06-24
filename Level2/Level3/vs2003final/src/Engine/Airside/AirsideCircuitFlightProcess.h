#pragma once
#include "Clearance.h"
#include "AirsideCircuitFlightInSim.h"
class AirTrafficController;



class ENGINE_TRANSFER AirsideCircuitFlightProcess
{
public:
	AirsideCircuitFlightProcess(AirsideCircuitFlightInSim* pFlight,ClearanceItem& lastItem, Clearance& newClearance);
	~AirsideCircuitFlightProcess(void);

	//-------------------------------------------------------------
	//Summary:
	//			Process order: start-->intension-->end
	//-------------------------------------------------------------
	void Process();
	Clearance GetClearance()const {return m_newClearance;}
private:
	bool StartBegine()const;
	bool SIDEnd()const;

	bool ExecuteBegineStart(AirTrafficController* pATC);
	bool ExecuteBegineStand(AirTrafficController* pATC);

	bool ExecuteEndSID(AirTrafficController* pATC);
	bool ExecuteEndStand(AirTrafficController* pATC);

	bool ExecuteIntensions(AirTrafficController* pATC);

	bool ExecuteLowAndOver(AirTrafficController* pATC);
	bool ExecuteTouchGo(AirTrafficController* pATC);
	bool ExecuteStopGo(AirTrafficController* pATC);
	bool ExecuteForceGo(AirTrafficController* pATC);

	bool MoveOnCircuitRoute(AirTrafficController* pATC,AirsideCircuitFlightInSim::LandingOperation emTyp);

	bool GetForceGoNextClearance(AirTrafficController* pATC);

	bool CheckOperationComplete(AirsideMobileElementMode mode)const;
	bool CheckIsTakeoffDelay(AirTrafficController* pATC,FlightRouteInSim *pAirRouteInSim);
private:
	AirsideCircuitFlightInSim* m_pFlight;
	ClearanceItem m_lastItem;
	Clearance m_newClearance;
};





