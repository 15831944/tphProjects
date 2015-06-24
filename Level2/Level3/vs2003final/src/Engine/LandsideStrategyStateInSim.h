
#pragma once
#include "Common\elaptime.h"
#include "Common\ALTObjectID.h"

class LandsideCurbsideSettingData;
class CARCportEngine;
class LandsideLayoutObjectInSim;
class CurbsideStrategyStateInSim
{
public:
	CurbsideStrategyStateInSim(LandsideCurbsideSettingData* pdata);
	ElapsedTime getMaxWaitSpotTime()const{ return m_maxStopTime; }
	//go to next Priority
	LandsideLayoutObjectInSim* GoNextPriority(CARCportEngine* pEngine);
	LandsideLayoutObjectInSim* GoNextLoop(CARCportEngine* pEngine);

	bool isFinalState()const{ return m_bFinalState; }
public:
	LandsideCurbsideSettingData* m_pData;
	int m_nCurPriority;
	ElapsedTime m_maxStopTime;  //stop of current priority
	int m_nLoopedCount;
	bool m_bFinalState;
};


class ParkinglotStrategyStateInSim
{

};
