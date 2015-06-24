#pragma once

#include "../../Common/elaptime.h"

class AirsideFlightInSim;
class FlightPerformanceManager;


class FlightPerformancesInSim
{
public:
	FlightPerformancesInSim(FlightPerformanceManager* pPerform);
	~FlightPerformancesInSim(void);

public:
	//get cruise performance
	double getMaxCruiseSpeed(AirsideFlightInSim* pflight)const;
	mutable double m_dMaxCruiseSpeed;

	double getMinCruiseSpeed(AirsideFlightInSim* pflight)const;
	mutable double m_dMinCruiseSpeed;


	//double getMaxCruiseAccel(AirsideFlightInSim* pflight);
	//double getMaxCruiseDecel(AirsideFlightInSim* pflight);

	////get terminal performance
	double getMaxTerminalSpeed(AirsideFlightInSim* pflight)const;
	mutable double m_dMaxTerminalSpeed;

	double getMinTerminalSpeed(AirsideFlightInSim* pflight)const;
	mutable double m_dMinTerminalSpeed;

	//double getMaxTerminalAccel(AirsideFlightInSim* pflight);
	//double getMaxTerminalDecel(AirsideFlightInSim* pflight);
	//double getMaxHoldingSpeed(AirsideFlightInSim* pflight);
	//double getMinHoldingSpeed(AirsideFlightInSim* pflight);
	//double getMaxUpVerticalSpeed(AirsideFlightInSim* pflight);
	//double getMaxDownVerticalSpeed(AirsideFlightInSim* pflight);

	////get approach to land performance
	double getMaxApproachSpeed(AirsideFlightInSim* pflight);
	mutable double m_dMaxApproachSpeed;

	double getMinApproachSpeed(AirsideFlightInSim* pflight);
	mutable double m_dMinApproachSpeed;


	double getAvgApproachSpeedInKnots(AirsideFlightInSim* pflight);
	mutable double m_dAvgApproachSpeedInKnots;
	//double getMinApproachLength(AirsideFlightInSim* pflight);


	////get landing performance

	double getMaxTouchDownSpeed(AirsideFlightInSim* pflight)const;
	mutable double m_dMaxTouchDownSpeed;
	double getMinTouchDownSpeed(AirsideFlightInSim* pflight)const;
	mutable double m_dMinTouchDownSpeed;
	double getTouchDownSpeed(AirsideFlightInSim* pflight)const;
	mutable double m_dTouchDownSpeed;

	double getMaxTouchDownDist(AirsideFlightInSim* pflight)const;
	mutable double m_dMaxTouchDownDist;
	double getMinTouchDownDist(AirsideFlightInSim* pflight)const;
	mutable double m_dMinTouchDownDist;

	double getMaxLandingDecel(AirsideFlightInSim* pflight)const;
	mutable double m_dMaxLandingDecel;
	double getNormalLandingDecel(AirsideFlightInSim* pflight)const;
	mutable double m_dNormalLandingDecel;
	double getMaxLandingDist(AirsideFlightInSim* pflight)const;
	mutable double m_dMaxLandingDist;
	double getMinLandingDist(AirsideFlightInSim* pflight)const;
	mutable double m_dMinLandingDist;
	double getMaxExitSpeedAcute(AirsideFlightInSim* pflight)const;
	mutable double m_dMaxExitSpeedAcute;
	double getMaxExitSpeed90deg(AirsideFlightInSim* pflight)const;
	mutable double m_dMaxExitSpeed90deg;
	double getMaxExitSpeedHigh(AirsideFlightInSim* pflight)const;
	mutable double m_dMaxExitSpeedHigh;

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
	mutable double m_dTaxiOutboundMinSeparationInQ;

	double getTaxiOutboundMaxSeparationInQ(AirsideFlightInSim* pflight)const;
	mutable double m_dTaxiOutboundMaxSeparationInQ;
	double getTaxiOutboundSeparationDistInQ(AirsideFlightInSim* pflight)const;
	mutable double m_dTaxiOutboundSeparationDistInQ;

	double getTaxiInboundMinSeparationInQ(AirsideFlightInSim* pflight)const;
	mutable double m_dTaxiInboundMinSeparationInQ;
	double getTaxiInboundMaxSeparationInQ(AirsideFlightInSim* pflight)const;
	mutable double m_dTaxiInboundMaxSeparationInQ;
	double getTaxiInboundSeparationDistInQ(AirsideFlightInSim* pflight)const;
	mutable double m_dTaxiInboundSeparationDistInQ;

	////get takeoff performance
	//double getMinTakeoffAccel(AirsideFlightInSim* pflight);
	//double getMaxTakeoffAccel(AirsideFlightInSim* pflight);
	double getMinLiftOff(AirsideFlightInSim* pflight);
	mutable double m_dMinLiftOff;
	double getMaxLiftOff(AirsideFlightInSim* pflight);
	mutable double m_dMaxLiftOff;
	//double getMinPositionTime(AirsideFlightInSim* pflight);
	//double getMaxPositionTime(AirsideFlightInSim* pflight);
	double getMinTakeoffRoll(AirsideFlightInSim* pflight);
	mutable double m_dMinTakeoffRoll;
	//double getMaxTakeoffRoll(AirsideFlightInSim* pflight);
	//

	////get climbout performance
	double getMinHorizontalAccel(AirsideFlightInSim* pflight)const;
	mutable double m_dMinHorizontalAccel;	
	double getMaxHorizontalAccel(AirsideFlightInSim* pflight)const;
	mutable double m_dMaxHorizontalAccel;
	double getMinVerticalSpeed(AirsideFlightInSim* pflight)const;
	mutable double m_dMinVerticalSpeed;
	double getMaxVerticalSpeed(AirsideFlightInSim* pflight)const;
	mutable double m_dMaxVerticalSpeed;

	//get takeoff position time
	double getTakeoffPositionTime(AirsideFlightInSim *pflight) const;
	mutable double m_dTakeoffPositionTime;
	//get lift off speed
	double getLiftoffSpeed(AirsideFlightInSim *pflight) const;
	mutable double m_dLiftoffSpeed;
	//get take off roll
	double getTakeoffRoll(AirsideFlightInSim *pflight) const;
	mutable double m_dTakeoffRoll;

	//get landing speed 
	double getLandingSpeed(AirsideFlightInSim *pflight)const;
	mutable double m_dLandingSpeed;
	double getMaxLandingSpeed(AirsideFlightInSim *pflight)const;
	mutable double m_dMaxLandingSpeed;

	//deceleration speed
	double getDecelSpeed(AirsideFlightInSim *pflight)const;
	mutable double m_dDecelSpeed;

	// get distance from threshold to touchdown
	double getDistThresToTouch(AirsideFlightInSim *pflight)const;
	mutable double m_dDistThresToTouch;

	//takeoff rolling distance
	double getDistEndToTakeoff(AirsideFlightInSim *pflight )const;
	mutable double m_dDistEndToTakeoff;

	//get Exit speed of a specified flight
	double getExitSpeed (AirsideFlightInSim *pflight)const;
	mutable double m_dExitSpeed;

	//takeoff accelerate speed
	double getTakeoffAcceleration(AirsideFlightInSim *pflight)const;  
	mutable double m_dTakeoffAcceleration;

	//takeoff ready time
	//	ElapsedTime getReadyTime(AirsideFlightInSim *pflight)const;

	//taxiway in speed
	double getTaxiInNormalSpeed(const AirsideFlightInSim *pflight)const;
	mutable double m_dTaxiInNormalSpeed;

	double getTaxiInMaxSpeed(AirsideFlightInSim* pFlight)const;
	mutable double m_dTaxiInMaxSpeed;

	//taxiway out speed
	double getTaxiOutNormalSpeed(const AirsideFlightInSim *pflight)const;
	mutable double m_dTaxiOutNormalSpeed;

	double getTaxiOutMaxSpeed(AirsideFlightInSim* pFlight)const;
	mutable double m_dTaxiOutMaxSpeed;

	//Approach speed
	double getApproachSpeedHorizen(AirsideFlightInSim *pflight)const;   
	mutable double m_dApproachSpeedHorizen;

	//	double getApproachSpeedVertical(AirsideFlightInSim *pflight)const; 

	//climbout speed
	double getClimboutSpeed(AirsideFlightInSim *pflight)const;
	mutable double m_dClimboutSpeed;

	double getMaxClimboutSpeed(AirsideFlightInSim *pflight)const;
	mutable double m_dMaxClimboutSpeed;

	double getMinClimboutSpeed(AirsideFlightInSim *pflight)const;
	mutable double m_dMinClimboutSpeed;

	//	double getClimboutSpeedVertical(AirsideFlightInSim *pflight)const;
	//taxi in acceleration
	double getTaxiInAccelation(AirsideFlightInSim * pflight)const;
	mutable double m_dTaxiInAccelation;

	double getTaxiInDeceleration(AirsideFlightInSim * pFlight)const;
	mutable double m_dTaxiInDeceleration;
	//taxi out acceleration
	double getTaxiOutAccelation(AirsideFlightInSim * pflight)const;
	mutable double m_dTaxiOutAccelation;
	double getTaxiOutDeceleration(AirsideFlightInSim* pFlight)const;
	mutable double m_dTaxiOutDeceleration;

	double getPushBackSpeed(AirsideFlightInSim* pFlight)const;
	mutable double m_dPushBackSpeed;
	double getPushBackAcc(AirsideFlightInSim* pFlight)const;
	mutable double m_dPushBackAcc;
	double getPushBackDec(AirsideFlightInSim* pFlight)const;
	mutable double m_dPushBackDec;

	ElapsedTime getUnhookandTaxiTime(AirsideFlightInSim* pFlight)const;
	mutable ElapsedTime m_eUnhookandTaxiTime;

	double getTowingSpeed(const AirsideFlightInSim* pFlight) const;
	mutable double m_dTowingSpeed;

	double getTaxiInboundIntersectionBuffer(AirsideFlightInSim* pFlight) const;
	mutable double m_dTaxiInboundIntersectionBuffer;

	double getTaxiOutboundIntersectionBuffer(AirsideFlightInSim* pFlight) const;
	mutable double m_dTaxiOutboundIntersectionBuffer;


	double getCurTaxiSpd(AirsideFlightInSim* pFlight)const;
	mutable double m_dCurTaxiSpd;

	double getCurTaxiAccSpd(AirsideFlightInSim* pFlight)const;
	mutable double m_dCurTaxiAccSpd;

	double getCurTaxiDecSpd(AirsideFlightInSim* pFlight)const;
	mutable double m_dCurTaxiDecSpd;



	ElapsedTime getStandMinTurnAroundTime(AirsideFlightInSim* pFlight) const;
	mutable ElapsedTime m_eStandMinTurnAroundTime;

	ElapsedTime getEngineStartTime(AirsideFlightInSim* pFlight) const;
	mutable ElapsedTime m_eEngineStartTime;

protected:
	FlightPerformanceManager *m_pMan;














};
