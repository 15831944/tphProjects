#pragma once
#include "../EngineDll.h"
#include ".\InputAirside\FlightPerformanceCruise.h"
#include ".\InputAirside\FlightPerformanceTerminal.h"
#include ".\InputAirside\PerformLandings.h"
#include ".\InputAirside\DepClimbOut.h"
#include ".\InputAirside\PerformanceTakeoffs.h"
#include ".\InputAirside\ApproachLand.h"
#include ".\InputAirside\TaxiInbound.h"
#include ".\InputAirside\TaxiOutbound.h"
#include ".\InputAirside\EngineParameters.h"

class AirsideFlightInSim;
class ElapsedTime;
class CAirportDatabase;


class TowOperationPerformanceList;
class TowOperationPerformance;
class StandServiceList;
class StandService;
class ENGINE_TRANSFER FlightPerformanceManager
{
public:
	FlightPerformanceManager();
	~FlightPerformanceManager();

	bool Init(int nPrjID, CAirportDatabase* pAirportDB);

	//get cruise performance
	double getMaxCruiseSpeed(AirsideFlightInSim* pflight)const;
	double getMinCruiseSpeed(AirsideFlightInSim* pflight)const;
	//double getMaxCruiseAccel(AirsideFlightInSim* pflight);
	//double getMaxCruiseDecel(AirsideFlightInSim* pflight);

	////get terminal performance
	double getMaxTerminalSpeed(AirsideFlightInSim* pflight)const;
	double getMinTerminalSpeed(AirsideFlightInSim* pflight)const;
	//double getMaxTerminalAccel(AirsideFlightInSim* pflight);
	//double getMaxTerminalDecel(AirsideFlightInSim* pflight);
	//double getMaxHoldingSpeed(AirsideFlightInSim* pflight);
	//double getMinHoldingSpeed(AirsideFlightInSim* pflight);
	//double getMaxUpVerticalSpeed(AirsideFlightInSim* pflight);
	//double getMaxDownVerticalSpeed(AirsideFlightInSim* pflight);

	////get approach to land performance
	double getMaxApproachSpeed(AirsideFlightInSim* pflight);
	double getMinApproachSpeed(AirsideFlightInSim* pflight);
	double getAvgApproachSpeedInKnots(AirsideFlightInSim* pflight);
	//double getMinApproachLength(AirsideFlightInSim* pflight);


	////get landing performance

	double getMaxTouchDownSpeed(AirsideFlightInSim* pflight)const;
	double getMinTouchDownSpeed(AirsideFlightInSim* pflight)const;
	double getTouchDownSpeed(AirsideFlightInSim* pflight)const;

	double getMaxTouchDownDist(AirsideFlightInSim* pflight)const;
	double getMinTouchDownDist(AirsideFlightInSim* pflight)const;

	double getMaxLandingDecel(AirsideFlightInSim* pflight)const;
	double getNormalLandingDecel(AirsideFlightInSim* pflight)const;
	double getMaxLandingDist(AirsideFlightInSim* pflight)const;
	double getMinLandingDist(AirsideFlightInSim* pflight)const;
	double getMaxExitSpeedAcute(AirsideFlightInSim* pflight)const;
	double getMaxExitSpeed90deg(AirsideFlightInSim* pflight)const;
	double getMaxExitSpeedHigh(AirsideFlightInSim* pflight)const;

	////get taxi inbound performance
	//double getMaxTaxiInSpeed(AirsideFlightInSim* pflight);
	//double getNormalTaxiInSpeed(AirsideFlightInSim* pflight);
	//double getTaxiInInterSectionBuffer(AirsideFlightInSim* pflight);

	////get push back performance
	//ElapsedTime getMaxPushBackOptTime(AirsideFlightInSim* pflight);
	//ElapsedTime getMinPushBackOptTime(AirsideFlightInSim* pflight);

	////get taxi outbound performance
	//double getMaxTaxiOutSpeed(AirsideFlightInSim* pflight);
	//double getNormalTaxiOutSpeed(AirsideFlightInSim* pflight);
	//double getTaxiOutInterSectionBuffer(AirsideFlightInSim* pflight);
	double getTaxiOutboundMinSeparationInQ(AirsideFlightInSim* pflight)const;
	double getTaxiOutboundMaxSeparationInQ(AirsideFlightInSim* pflight)const;
	double getTaxiOutboundSeparationDistInQ(AirsideFlightInSim* pflight)const;

	double getTaxiInboundMinSeparationInQ(AirsideFlightInSim* pflight)const;
	double getTaxiInboundMaxSeparationInQ(AirsideFlightInSim* pflight)const;
	double getTaxiInboundSeparationDistInQ(AirsideFlightInSim* pflight)const;

	////get takeoff performance
	//double getMinTakeoffAccel(AirsideFlightInSim* pflight);
	//double getMaxTakeoffAccel(AirsideFlightInSim* pflight);
	double getMinLiftOff(AirsideFlightInSim* pflight);
	double getMaxLiftOff(AirsideFlightInSim* pflight);
	//double getMinPositionTime(AirsideFlightInSim* pflight);
	//double getMaxPositionTime(AirsideFlightInSim* pflight);
	double getMinTakeoffRoll(AirsideFlightInSim* pflight);
	//double getMaxTakeoffRoll(AirsideFlightInSim* pflight);
	//

	////get climbout performance
	double getMinHorizontalAccel(AirsideFlightInSim* pflight)const;
	double getMaxHorizontalAccel(AirsideFlightInSim* pflight)const;
	double getMinVerticalSpeed(AirsideFlightInSim* pflight)const;
	double getMaxVerticalSpeed(AirsideFlightInSim* pflight)const;

	//get takeoff position time
	double getTakeoffPositionTime(AirsideFlightInSim *pflight) const;
	//get lift off speed
	double getLiftoffSpeed(AirsideFlightInSim *pflight) const;
	//get take off roll
	double getTakeoffRoll(AirsideFlightInSim *pflight) const;

	//get landing speed 
	double getLandingSpeed(AirsideFlightInSim *pflight)const;
	double getMaxLandingSpeed(AirsideFlightInSim *pflight)const;

	//deceleration speed
	double getDecelSpeed(AirsideFlightInSim *pflight)const;

	// get distance from threshold to touchdown
	double getDistThresToTouch(AirsideFlightInSim *pflight)const;

	//takeoff rolling distance
	double getDistEndToTakeoff(AirsideFlightInSim *pflight )const;

	//get Exit speed of a specified flight
	double getExitSpeed (AirsideFlightInSim *pflight)const;

	//takeoff accelerate speed
	double getTakeoffAcceleration(AirsideFlightInSim *pflight)const;  

	//takeoff ready time
//	ElapsedTime getReadyTime(AirsideFlightInSim *pflight)const;

	//taxiway in speed
	double getTaxiInNormalSpeed(const AirsideFlightInSim *pflight)const;
	
	double getTaxiInMaxSpeed(AirsideFlightInSim* pFlight)const;

	//taxiway out speed
	double getTaxiOutNormalSpeed(const AirsideFlightInSim *pflight)const;

	double getTaxiOutMaxSpeed(AirsideFlightInSim* pFlight)const;

	//Approach speed
	double getApproachSpeedHorizen(AirsideFlightInSim *pflight)const;    

//	double getApproachSpeedVertical(AirsideFlightInSim *pflight)const; 

	//climbout speed
	double getClimboutSpeed(AirsideFlightInSim *pflight)const;
	double getMaxClimboutSpeed(AirsideFlightInSim *pflight)const;
	double getMinClimboutSpeed(AirsideFlightInSim *pflight)const;

//	double getClimboutSpeedVertical(AirsideFlightInSim *pflight)const;
	//taxi in acceleration
	double getTaxiInAccelation(AirsideFlightInSim * pflight)const;
	double getTaxiInDeceleration(AirsideFlightInSim * pFlight)const;
	//taxi out acceleration
	double getTaxiOutAccelation(AirsideFlightInSim * pflight)const;
	double getTaxiOutDeceleration(AirsideFlightInSim* pFlight)const;

	double getPushBackSpeed(AirsideFlightInSim* pFlight)const;
	double getPushBackAcc(AirsideFlightInSim* pFlight)const;
	double getPushBackDec(AirsideFlightInSim* pFlight)const;
	ElapsedTime getUnhookandTaxiTime(AirsideFlightInSim* pFlight)const;
	double getTowingSpeed(const AirsideFlightInSim* pFlight) const;

	double getTaxiInboundIntersectionBuffer(AirsideFlightInSim* pFlight) const;
	double getTaxiOutboundIntersectionBuffer(AirsideFlightInSim* pFlight) const;

	
	double getCurTaxiSpd(AirsideFlightInSim* pFlight)const;
	double getCurTaxiAccSpd(AirsideFlightInSim* pFlight)const;
	double getCurTaxiDecSpd(AirsideFlightInSim* pFlight)const;

	ElapsedTime getStandMinTurnAroundTime(AirsideFlightInSim* pFlight) const;

	ElapsedTime getEngineStartTime(AirsideFlightInSim* pFlight) const;
private:
	ns_AirsideInput::CFlightPerformanceCruise* GetFlightPerformanceCruiseData(AirsideFlightInSim *pflight)const;
	ns_AirsideInput::CFlightPerformanceTerminal* GetFlightPerformanceTerminalData(AirsideFlightInSim *pflight)const;
	PerformLanding* GetPerformLandingData(AirsideFlightInSim *pflight)const;
	DepClimbOut* GetDepClimbOutData(AirsideFlightInSim *pflight)const;
	PerformTakeOff* GetPerformTakeOffData(AirsideFlightInSim *pflight)const;
	ApproachLand* GetApproachLandData(AirsideFlightInSim *pflight)const;
	CTaxiInboundNEC* GetTaxiInboundNECData(const AirsideFlightInSim *pflight)const;
	CTaxiOutboundNEC* GetTaxiOutboundNECData(const AirsideFlightInSim *pflight)const;
	TowOperationPerformance * GetTowingData(const AirsideFlightInSim* pFlight)const;
	StandService* GetStandServiceData(AirsideFlightInSim* pFlight) const;
	CEngineParametersItem* GetEngineStartData(AirsideFlightInSim* pFlight) const;

private:
	ns_AirsideInput::vFlightPerformanceCruise* m_pvFlightPerformanceCruise;

	ns_AirsideInput::vFlightPerformanceTerminal* m_pvFlightPerformanceTerminal;

	PerformLandings* m_pPerformLandings;
	
	DepClimbOuts* m_pDepClimbOuts;

	PerformTakeOffs* m_pPerformTakeOffs;

	ApproachLands* m_pApproachLands;

	CTaxiInbound* m_pTaxiInbound;

	CTaxiOutbound* m_pTaxiOutbound;

	TowOperationPerformanceList * m_pTowingPerform;

	StandServiceList* m_pStandServiceList;

	CEngineParametersList* m_pEngineStartList;
};
