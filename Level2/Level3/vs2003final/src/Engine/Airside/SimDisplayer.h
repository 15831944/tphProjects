#ifndef __AIRSIDE_SIMDISPLAYER_DEF
#define __AIRSIDE_SIMDISPLAYER_DEF
#include "../EngineDll.h"
#include "../EngineConfig.h"
#include "../SimClock.h"
#include "../../Common/ARCVector.h"
// 
//
class CAirsideSimLogs;
NAMESPACE_AIRSIDEENGINE_BEGINE
class Simulation;
class SimFlight;

class SimDisplayer{
public:
	
	bool getFlightState(SimFlight* pFlight,const SimClock::TimeType& curTime, ARCVector3& pos, ARCVector3& dir );
	
	bool WirteLog(CAirsideSimLogs& simlog,int ProjectId);

	SimDisplayer();
	SimDisplayer(Simulation * pSimulation);

	void SetSimulation(Simulation * pSimulation);
private:
	
	Simulation * m_pSimulation;
	long m_nStartTime;
	long m_nEndTime;
};







NAMESPACE_AIRSIDEENGINE_END
#endif