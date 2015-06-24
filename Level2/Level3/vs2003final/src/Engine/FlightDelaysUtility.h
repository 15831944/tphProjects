#pragma once

class PaxLog;
class FlightSchedule;
class Flight;
class FlightsBoardingCallManager;

class CProgressBar;

class FlightDelaysUtility
{
public:
	static void impactFlightDelaysEffectToMobElement(PaxLog* pAllPaxLog, const FlightSchedule* pSchedule/*, CProgressBar& bar*/);
	static void impactFlightDelaysEffectToBoardingCall(FlightsBoardingCallManager*_pBoardingcallMgr, const FlightSchedule* pSchedule);

};