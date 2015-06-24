#include "StdAfx.h"
#include ".\flightperformancesinsim.h"
#include "FlightPerformanceManager.h"

#define FLT_PERFORM_DEFAULT_VALUE 0.0

FlightPerformancesInSim::FlightPerformancesInSim(FlightPerformanceManager* pPerform)
{
	m_pMan = pPerform;

	m_dMaxCruiseSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dMinCruiseSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dMaxTerminalSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dMinTerminalSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dMaxApproachSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dMinApproachSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dAvgApproachSpeedInKnots = FLT_PERFORM_DEFAULT_VALUE;
	m_dMaxTouchDownSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dMinTouchDownSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dTouchDownSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dMaxTouchDownDist = FLT_PERFORM_DEFAULT_VALUE;
	m_dMinTouchDownDist = FLT_PERFORM_DEFAULT_VALUE;
	m_dMaxLandingDecel = FLT_PERFORM_DEFAULT_VALUE;
	m_dNormalLandingDecel = FLT_PERFORM_DEFAULT_VALUE;
	m_dMaxLandingDist = FLT_PERFORM_DEFAULT_VALUE;
	m_dMinLandingDist = FLT_PERFORM_DEFAULT_VALUE;
	m_dMaxExitSpeedAcute = FLT_PERFORM_DEFAULT_VALUE;
	m_dMaxExitSpeed90deg = FLT_PERFORM_DEFAULT_VALUE;
	m_dMaxExitSpeedHigh = FLT_PERFORM_DEFAULT_VALUE;
	m_dTaxiOutboundMinSeparationInQ = FLT_PERFORM_DEFAULT_VALUE;
	m_dTaxiOutboundMaxSeparationInQ = FLT_PERFORM_DEFAULT_VALUE;
	m_dTaxiOutboundSeparationDistInQ = FLT_PERFORM_DEFAULT_VALUE;
	m_dTaxiInboundMinSeparationInQ = FLT_PERFORM_DEFAULT_VALUE;
	m_dTaxiInboundMaxSeparationInQ = FLT_PERFORM_DEFAULT_VALUE;
	m_dTaxiInboundSeparationDistInQ = FLT_PERFORM_DEFAULT_VALUE;
	m_dMinLiftOff = FLT_PERFORM_DEFAULT_VALUE;
	m_dMaxLiftOff = FLT_PERFORM_DEFAULT_VALUE;
	m_dMinTakeoffRoll = FLT_PERFORM_DEFAULT_VALUE;
	m_dMinHorizontalAccel = FLT_PERFORM_DEFAULT_VALUE;
	m_dMaxHorizontalAccel = FLT_PERFORM_DEFAULT_VALUE;
	m_dMinVerticalSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dMaxVerticalSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dTakeoffPositionTime = FLT_PERFORM_DEFAULT_VALUE;
	m_dLiftoffSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dTakeoffRoll = FLT_PERFORM_DEFAULT_VALUE;
	m_dLandingSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dMaxLandingSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dDecelSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dDistThresToTouch = FLT_PERFORM_DEFAULT_VALUE;
	m_dDistEndToTakeoff = FLT_PERFORM_DEFAULT_VALUE;
	m_dExitSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dTakeoffAcceleration = FLT_PERFORM_DEFAULT_VALUE;
	m_dTaxiInNormalSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dTaxiInMaxSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dTaxiOutNormalSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dTaxiOutMaxSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dApproachSpeedHorizen = FLT_PERFORM_DEFAULT_VALUE;
	m_dClimboutSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dMaxClimboutSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dMinClimboutSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dTaxiInAccelation = FLT_PERFORM_DEFAULT_VALUE;
	m_dTaxiInDeceleration = FLT_PERFORM_DEFAULT_VALUE;
	m_dTaxiOutAccelation = FLT_PERFORM_DEFAULT_VALUE;
	m_dTaxiOutDeceleration = FLT_PERFORM_DEFAULT_VALUE;
	m_dPushBackSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dPushBackAcc = FLT_PERFORM_DEFAULT_VALUE;
	m_dPushBackDec = FLT_PERFORM_DEFAULT_VALUE;
	m_eUnhookandTaxiTime = ElapsedTime(FLT_PERFORM_DEFAULT_VALUE);
	m_dTowingSpeed = FLT_PERFORM_DEFAULT_VALUE;
	m_dTaxiInboundIntersectionBuffer = FLT_PERFORM_DEFAULT_VALUE;
	m_dTaxiOutboundIntersectionBuffer = FLT_PERFORM_DEFAULT_VALUE;
	m_dCurTaxiSpd = FLT_PERFORM_DEFAULT_VALUE;
	m_dCurTaxiAccSpd = FLT_PERFORM_DEFAULT_VALUE;
	m_dCurTaxiDecSpd = FLT_PERFORM_DEFAULT_VALUE;
	m_eStandMinTurnAroundTime = ElapsedTime(FLT_PERFORM_DEFAULT_VALUE);
	m_eEngineStartTime = ElapsedTime(FLT_PERFORM_DEFAULT_VALUE);






}

FlightPerformancesInSim::~FlightPerformancesInSim(void)
{

}

double FlightPerformancesInSim::getMaxCruiseSpeed( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMaxCruiseSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dMaxCruiseSpeed = m_pMan->getMaxCruiseSpeed(pflight);

	return m_dMaxCruiseSpeed;

}

double FlightPerformancesInSim::getMinCruiseSpeed( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMinCruiseSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dMinCruiseSpeed = m_pMan->getMinCruiseSpeed(pflight);

	return m_dMinCruiseSpeed;
}

double FlightPerformancesInSim::getMaxTerminalSpeed( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMaxTerminalSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dMaxTerminalSpeed = m_pMan->getMaxTerminalSpeed(pflight);

	return m_dMaxTerminalSpeed;
}

double FlightPerformancesInSim::getMinTerminalSpeed( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMinTerminalSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dMinTerminalSpeed = m_pMan->getMinTerminalSpeed(pflight);

	return m_dMinTerminalSpeed;
}

double FlightPerformancesInSim::getMaxApproachSpeed( AirsideFlightInSim* pflight )
{
	ASSERT(m_pMan);
	if(m_dMaxApproachSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dMaxApproachSpeed = m_pMan->getMaxApproachSpeed(pflight);

	return m_dMaxApproachSpeed;
}

double FlightPerformancesInSim::getMinApproachSpeed( AirsideFlightInSim* pflight )
{
	ASSERT(m_pMan);
	if(m_dMinApproachSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dMinApproachSpeed = m_pMan->getMinApproachSpeed(pflight);

	return m_dMinApproachSpeed;
}

double FlightPerformancesInSim::getAvgApproachSpeedInKnots( AirsideFlightInSim* pflight )
{
	ASSERT(m_pMan);
	if(m_dAvgApproachSpeedInKnots == FLT_PERFORM_DEFAULT_VALUE)
		m_dAvgApproachSpeedInKnots = m_pMan->getAvgApproachSpeedInKnots(pflight);

	return m_dAvgApproachSpeedInKnots;
}

double FlightPerformancesInSim::getMaxTouchDownSpeed( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMaxTouchDownSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dMaxTouchDownSpeed = m_pMan->getMaxTouchDownSpeed(pflight);

	return m_dMaxTouchDownSpeed;
}

double FlightPerformancesInSim::getMinTouchDownSpeed( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMinTouchDownSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dMinTouchDownSpeed = m_pMan->getMinTouchDownSpeed(pflight);

	return m_dMinTouchDownSpeed;
}

double FlightPerformancesInSim::getMaxTouchDownDist( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMaxTouchDownDist == FLT_PERFORM_DEFAULT_VALUE)
		m_dMaxTouchDownDist = m_pMan->getMaxTouchDownDist(pflight);

	return m_dMaxTouchDownDist;
}

double FlightPerformancesInSim::getMinTouchDownDist( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMinTouchDownDist == FLT_PERFORM_DEFAULT_VALUE)
		m_dMinTouchDownDist = m_pMan->getMinTouchDownDist(pflight);

	return m_dMinTouchDownDist;
}

double FlightPerformancesInSim::getMaxLandingDecel( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMaxLandingDecel == FLT_PERFORM_DEFAULT_VALUE)
		m_dMaxLandingDecel = m_pMan->getMaxLandingDecel(pflight);

	return m_dMaxLandingDecel;
}

double FlightPerformancesInSim::getNormalLandingDecel( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dNormalLandingDecel == FLT_PERFORM_DEFAULT_VALUE)
		m_dNormalLandingDecel = m_pMan->getNormalLandingDecel(pflight);

	return m_dNormalLandingDecel;
}

double FlightPerformancesInSim::getMaxLandingDist( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMaxLandingDist == FLT_PERFORM_DEFAULT_VALUE)
		m_dMaxLandingDist = m_pMan->getMaxLandingDist(pflight);

	return m_dMaxLandingDist;
}

double FlightPerformancesInSim::getMinLandingDist( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);

	if(m_dMinLandingDist == FLT_PERFORM_DEFAULT_VALUE)
		m_dMinLandingDist = m_pMan->getMinLandingDist(pflight);

	return m_dMinLandingDist;
}

double FlightPerformancesInSim::getMaxExitSpeedAcute( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMaxExitSpeedAcute == FLT_PERFORM_DEFAULT_VALUE)
		m_dMaxExitSpeedAcute = m_pMan->getMaxExitSpeedAcute(pflight);

	return m_dMaxExitSpeedAcute;
}

double FlightPerformancesInSim::getMaxExitSpeed90deg( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMaxExitSpeed90deg == FLT_PERFORM_DEFAULT_VALUE)
		m_dMaxExitSpeed90deg = m_pMan->getMaxExitSpeed90deg(pflight);

	return m_dMaxExitSpeed90deg;
}

double FlightPerformancesInSim::getMaxExitSpeedHigh( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMaxExitSpeedHigh == FLT_PERFORM_DEFAULT_VALUE)
		m_dMaxExitSpeedHigh = m_pMan->getMaxExitSpeedHigh(pflight);

	return m_dMaxExitSpeedHigh;
}

double FlightPerformancesInSim::getTaxiOutboundMinSeparationInQ( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dTaxiOutboundMinSeparationInQ == FLT_PERFORM_DEFAULT_VALUE)
		m_dTaxiOutboundMinSeparationInQ = m_pMan->getTaxiOutboundMinSeparationInQ(pflight);

	return m_dTaxiOutboundMinSeparationInQ;
}

double FlightPerformancesInSim::getTaxiOutboundMaxSeparationInQ( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dTaxiOutboundMaxSeparationInQ == FLT_PERFORM_DEFAULT_VALUE)
		m_dTaxiOutboundMaxSeparationInQ = m_pMan->getTaxiOutboundMaxSeparationInQ(pflight);

	return m_dTaxiOutboundMaxSeparationInQ;
}

double FlightPerformancesInSim::getTaxiOutboundSeparationDistInQ( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dTaxiOutboundSeparationDistInQ == FLT_PERFORM_DEFAULT_VALUE)
		m_dTaxiOutboundSeparationDistInQ = m_pMan->getTaxiOutboundSeparationDistInQ(pflight);

	return m_dTaxiOutboundSeparationDistInQ;
}

double FlightPerformancesInSim::getTaxiInboundMinSeparationInQ( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dTaxiInboundMinSeparationInQ == FLT_PERFORM_DEFAULT_VALUE)
		m_dTaxiInboundMinSeparationInQ = m_pMan->getTaxiInboundMinSeparationInQ(pflight);

	return m_dTaxiInboundMinSeparationInQ;
}

double FlightPerformancesInSim::getTaxiInboundMaxSeparationInQ( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dTaxiInboundMaxSeparationInQ == FLT_PERFORM_DEFAULT_VALUE)
		m_dTaxiInboundMaxSeparationInQ = m_pMan->getTaxiInboundMaxSeparationInQ(pflight);

	return m_dTaxiInboundMaxSeparationInQ;
}

double FlightPerformancesInSim::getTaxiInboundSeparationDistInQ( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dTaxiInboundSeparationDistInQ == FLT_PERFORM_DEFAULT_VALUE)
		m_dTaxiInboundSeparationDistInQ = m_pMan->getTaxiInboundSeparationDistInQ(pflight);

	return m_dTaxiInboundSeparationDistInQ;
}

double FlightPerformancesInSim::getMinLiftOff( AirsideFlightInSim* pflight )
{
	ASSERT(m_pMan);
	if(m_dMinLiftOff == FLT_PERFORM_DEFAULT_VALUE)
		m_dMinLiftOff = m_pMan->getMinLiftOff(pflight);

	return m_dMinLiftOff;
}

double FlightPerformancesInSim::getMaxLiftOff( AirsideFlightInSim* pflight )
{
	ASSERT(m_pMan);
	if(m_dMaxLiftOff == FLT_PERFORM_DEFAULT_VALUE)
		m_dMaxLiftOff = m_pMan->getMaxLiftOff(pflight);

	return m_dMaxLiftOff;
}

double FlightPerformancesInSim::getMinTakeoffRoll( AirsideFlightInSim* pflight )
{
	ASSERT(m_pMan);
	if(m_dMinTakeoffRoll == FLT_PERFORM_DEFAULT_VALUE)
		m_dMinTakeoffRoll = m_pMan->getMinTakeoffRoll(pflight);

	return m_dMinTakeoffRoll;
}

double FlightPerformancesInSim::getMinHorizontalAccel( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMinHorizontalAccel == FLT_PERFORM_DEFAULT_VALUE)
		m_dMinHorizontalAccel = m_pMan->getMinHorizontalAccel(pflight);

	return m_dMinHorizontalAccel;
}

double FlightPerformancesInSim::getMaxHorizontalAccel( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMaxHorizontalAccel == FLT_PERFORM_DEFAULT_VALUE)
		m_dMaxHorizontalAccel = m_pMan->getMaxHorizontalAccel(pflight);

	return m_dMaxHorizontalAccel;
}

double FlightPerformancesInSim::getMinVerticalSpeed( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMinVerticalSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dMinVerticalSpeed = m_pMan->getMinVerticalSpeed(pflight);

	return m_dMinVerticalSpeed;
}

double FlightPerformancesInSim::getMaxVerticalSpeed( AirsideFlightInSim* pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMaxVerticalSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dMaxVerticalSpeed = m_pMan->getMaxVerticalSpeed(pflight);

	return m_dMaxVerticalSpeed;
}

double FlightPerformancesInSim::getTakeoffPositionTime( AirsideFlightInSim *pflight ) const
{
	ASSERT(m_pMan);
	if(m_dTakeoffPositionTime == FLT_PERFORM_DEFAULT_VALUE)
		m_dTakeoffPositionTime = m_pMan->getTakeoffPositionTime(pflight);

	return m_dTakeoffPositionTime;
}

double FlightPerformancesInSim::getLiftoffSpeed( AirsideFlightInSim *pflight ) const
{
	ASSERT(m_pMan);
	if(m_dLiftoffSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dLiftoffSpeed = m_pMan->getLiftoffSpeed(pflight);

	return m_dLiftoffSpeed;
}

double FlightPerformancesInSim::getTakeoffRoll( AirsideFlightInSim *pflight ) const
{
	ASSERT(m_pMan);
	if(m_dTakeoffRoll == FLT_PERFORM_DEFAULT_VALUE)
		m_dTakeoffRoll = m_pMan->getTakeoffRoll(pflight);

	return m_dTakeoffRoll;
}

double FlightPerformancesInSim::getLandingSpeed( AirsideFlightInSim *pflight ) const
{
	ASSERT(m_pMan);
	if(m_dLandingSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dLandingSpeed = m_pMan->getLandingSpeed(pflight);

	return m_dLandingSpeed;
}

double FlightPerformancesInSim::getMaxLandingSpeed( AirsideFlightInSim *pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMaxLandingSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dMaxLandingSpeed = m_pMan->getMaxLandingSpeed(pflight);

	return m_dMaxLandingSpeed;
}

double FlightPerformancesInSim::getDecelSpeed( AirsideFlightInSim *pflight ) const
{
	ASSERT(m_pMan);
	if(m_dDecelSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dDecelSpeed = m_pMan->getDecelSpeed(pflight);

	return m_dDecelSpeed;
}

double FlightPerformancesInSim::getDistThresToTouch( AirsideFlightInSim *pflight ) const
{
	ASSERT(m_pMan);
	if(m_dDistThresToTouch == FLT_PERFORM_DEFAULT_VALUE)
		m_dDistThresToTouch = m_pMan->getDistThresToTouch(pflight);

	return m_dDistThresToTouch;
}

double FlightPerformancesInSim::getDistEndToTakeoff( AirsideFlightInSim *pflight ) const
{
	ASSERT(m_pMan);
	if(m_dDistEndToTakeoff == FLT_PERFORM_DEFAULT_VALUE)
		m_dDistEndToTakeoff = m_pMan->getDistEndToTakeoff(pflight);

	return m_dDistEndToTakeoff;
}

double FlightPerformancesInSim::getExitSpeed( AirsideFlightInSim *pflight ) const
{
	ASSERT(m_pMan);
	if(m_dExitSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dExitSpeed = m_pMan->getExitSpeed(pflight);

	return m_dExitSpeed;
}

double FlightPerformancesInSim::getTakeoffAcceleration( AirsideFlightInSim *pflight ) const
{
	ASSERT(m_pMan);
	if(m_dTakeoffAcceleration == FLT_PERFORM_DEFAULT_VALUE)
		m_dTakeoffAcceleration = m_pMan->getTakeoffAcceleration(pflight);

	return m_dTakeoffAcceleration;
}

double FlightPerformancesInSim::getTaxiInNormalSpeed( const AirsideFlightInSim *pflight ) const
{
	ASSERT(m_pMan);
	if(m_dTaxiInNormalSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dTaxiInNormalSpeed = m_pMan->getTaxiInNormalSpeed(pflight);

	return m_dTaxiInNormalSpeed;
}

double FlightPerformancesInSim::getTaxiInMaxSpeed( AirsideFlightInSim* pFlight ) const
{
	ASSERT(m_pMan);
	if(m_dTaxiInMaxSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dTaxiInMaxSpeed= m_pMan->getTaxiInMaxSpeed(pFlight);

	return m_dTaxiInMaxSpeed;
}

double FlightPerformancesInSim::getTaxiOutNormalSpeed( const AirsideFlightInSim *pflight ) const
{
	ASSERT(m_pMan);
	if(m_dTaxiOutNormalSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dTaxiOutNormalSpeed = m_pMan->getTaxiOutNormalSpeed(pflight);

	return m_dTaxiOutNormalSpeed;
}

double FlightPerformancesInSim::getTaxiOutMaxSpeed( AirsideFlightInSim* pFlight ) const
{
	ASSERT(m_pMan);
	if(m_dTaxiOutMaxSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dTaxiOutMaxSpeed = m_pMan->getTaxiOutMaxSpeed(pFlight);

	return m_dTaxiOutMaxSpeed;
}

double FlightPerformancesInSim::getApproachSpeedHorizen( AirsideFlightInSim *pflight ) const
{
	ASSERT(m_pMan);
	if(m_dApproachSpeedHorizen == FLT_PERFORM_DEFAULT_VALUE)
		m_dApproachSpeedHorizen = m_pMan->getApproachSpeedHorizen(pflight);

	return m_dApproachSpeedHorizen;
}

double FlightPerformancesInSim::getClimboutSpeed( AirsideFlightInSim *pflight ) const
{
	ASSERT(m_pMan);
	if(m_dClimboutSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dClimboutSpeed = m_pMan->getClimboutSpeed(pflight);

	return m_dClimboutSpeed;
}

double FlightPerformancesInSim::getMaxClimboutSpeed( AirsideFlightInSim *pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMaxClimboutSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dMaxClimboutSpeed = m_pMan->getMaxClimboutSpeed(pflight);

	return m_dMaxClimboutSpeed;
}

double FlightPerformancesInSim::getMinClimboutSpeed( AirsideFlightInSim *pflight ) const
{
	ASSERT(m_pMan);
	if(m_dMinClimboutSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dMinClimboutSpeed = m_pMan->getMinClimboutSpeed(pflight);

	return m_dMinClimboutSpeed;
}

double FlightPerformancesInSim::getTaxiInAccelation( AirsideFlightInSim * pflight ) const
{
	ASSERT(m_pMan);
	if(m_dTaxiInAccelation == FLT_PERFORM_DEFAULT_VALUE)
		m_dTaxiInAccelation = m_pMan->getTaxiInAccelation(pflight);

	return m_dTaxiInAccelation;
}

double FlightPerformancesInSim::getTaxiInDeceleration( AirsideFlightInSim * pFlight ) const
{
	ASSERT(m_pMan);
	if(m_dTaxiInDeceleration == FLT_PERFORM_DEFAULT_VALUE)
		m_dTaxiInDeceleration = m_pMan->getTaxiInDeceleration(pFlight);

	return m_dTaxiInDeceleration;
}

double FlightPerformancesInSim::getTaxiOutAccelation( AirsideFlightInSim * pflight ) const
{
	ASSERT(m_pMan);
	if(m_dTaxiOutAccelation == FLT_PERFORM_DEFAULT_VALUE)
		m_dTaxiOutAccelation = m_pMan->getTaxiOutAccelation(pflight);

	return m_dTaxiOutAccelation;
}

double FlightPerformancesInSim::getTaxiOutDeceleration( AirsideFlightInSim* pFlight ) const
{
	ASSERT(m_pMan);
	if(m_dTaxiOutDeceleration == FLT_PERFORM_DEFAULT_VALUE)
		m_dTaxiOutDeceleration = m_pMan->getTaxiOutDeceleration(pFlight);

	return m_dTaxiOutDeceleration;
}

double FlightPerformancesInSim::getPushBackSpeed( AirsideFlightInSim* pFlight ) const
{
	ASSERT(m_pMan);
	if(m_dPushBackSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dPushBackSpeed = m_pMan->getPushBackSpeed(pFlight);

	return m_dPushBackSpeed;
}

double FlightPerformancesInSim::getPushBackAcc( AirsideFlightInSim* pFlight ) const
{
	ASSERT(m_pMan);
	if(m_dPushBackAcc == FLT_PERFORM_DEFAULT_VALUE)
		m_dPushBackAcc = m_pMan->getPushBackAcc(pFlight);

	return m_dPushBackAcc;
}

double FlightPerformancesInSim::getPushBackDec( AirsideFlightInSim* pFlight ) const
{
	ASSERT(m_pMan);
	if(m_dPushBackDec == FLT_PERFORM_DEFAULT_VALUE)
		m_dPushBackDec = m_pMan->getPushBackDec(pFlight);

	return m_dPushBackDec;
}

ElapsedTime FlightPerformancesInSim::getUnhookandTaxiTime( AirsideFlightInSim* pFlight ) const
{
	ASSERT(m_pMan);
	if(m_eUnhookandTaxiTime == ElapsedTime(FLT_PERFORM_DEFAULT_VALUE))
		m_eUnhookandTaxiTime = m_pMan->getUnhookandTaxiTime(pFlight);

	return m_eUnhookandTaxiTime;
}

double FlightPerformancesInSim::getTowingSpeed( const AirsideFlightInSim* pFlight ) const
{
	ASSERT(m_pMan);
	if(m_dTowingSpeed == FLT_PERFORM_DEFAULT_VALUE)
		m_dTowingSpeed = m_pMan->getTowingSpeed(pFlight);

	return m_dTowingSpeed;
}

double FlightPerformancesInSim::getTaxiInboundIntersectionBuffer( AirsideFlightInSim* pFlight ) const
{
	ASSERT(m_pMan);
	if(m_dTaxiInboundIntersectionBuffer == FLT_PERFORM_DEFAULT_VALUE)
		m_dTaxiInboundIntersectionBuffer = m_pMan->getTaxiInboundIntersectionBuffer(pFlight);

	return m_dTaxiInboundIntersectionBuffer;
}

double FlightPerformancesInSim::getTaxiOutboundIntersectionBuffer( AirsideFlightInSim* pFlight ) const
{
	ASSERT(m_pMan);
	if(m_dTaxiOutboundIntersectionBuffer == FLT_PERFORM_DEFAULT_VALUE)
		m_dTaxiOutboundIntersectionBuffer = m_pMan->getTaxiOutboundIntersectionBuffer(pFlight);

	return m_dTaxiOutboundIntersectionBuffer;
}

double FlightPerformancesInSim::getCurTaxiSpd( AirsideFlightInSim* pFlight ) const
{
	ASSERT(m_pMan);
	if(m_dCurTaxiSpd == FLT_PERFORM_DEFAULT_VALUE)
		m_dCurTaxiSpd = m_pMan->getCurTaxiSpd(pFlight);

	return m_dCurTaxiSpd;
}

double FlightPerformancesInSim::getCurTaxiAccSpd( AirsideFlightInSim* pFlight ) const
{
	ASSERT(m_pMan);
	if(m_dCurTaxiAccSpd == FLT_PERFORM_DEFAULT_VALUE)
		m_dCurTaxiAccSpd = m_pMan->getCurTaxiAccSpd(pFlight);

	return m_dCurTaxiAccSpd;
}

double FlightPerformancesInSim::getCurTaxiDecSpd( AirsideFlightInSim* pFlight ) const
{
	ASSERT(m_pMan);
	if(m_dCurTaxiDecSpd == FLT_PERFORM_DEFAULT_VALUE)
		m_dCurTaxiDecSpd = m_pMan->getCurTaxiDecSpd(pFlight);

	return m_dCurTaxiDecSpd;
}

ElapsedTime FlightPerformancesInSim::getStandMinTurnAroundTime( AirsideFlightInSim* pFlight ) const
{
	ASSERT(m_pMan);
	if(m_eStandMinTurnAroundTime == ElapsedTime(FLT_PERFORM_DEFAULT_VALUE))
		m_eStandMinTurnAroundTime = m_pMan->getStandMinTurnAroundTime(pFlight);

	return m_eStandMinTurnAroundTime;
}

ElapsedTime FlightPerformancesInSim::getEngineStartTime( AirsideFlightInSim* pFlight ) const
{
	ASSERT(m_pMan);
	if(m_eEngineStartTime == ElapsedTime(FLT_PERFORM_DEFAULT_VALUE))
		m_eEngineStartTime = m_pMan->getEngineStartTime(pFlight);

	return m_eEngineStartTime;
}

