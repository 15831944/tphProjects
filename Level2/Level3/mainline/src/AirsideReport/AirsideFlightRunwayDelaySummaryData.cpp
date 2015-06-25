#include "StdAfx.h"
#include ".\airsideflightrunwaydelaysummarydata.h"
#include "../Engine/Airside/CommonInSim.h"

AirsideFlightRunwayDelaySummaryData::AirsideFlightRunwayDelaySummaryData()
:m_tStart(ElapsedTime(0L))
,m_tEnd(ElapsedTime(0L))
,m_nPosition(FlightRunwayDelay::UNKNOWNPOS)
,m_nDelayCount(0)
,m_lTotal(0l)
,m_lMin(0l)
,m_lMax(0l)
,m_lAvg(0l)
{
}

AirsideFlightRunwayDelaySummaryData::~AirsideFlightRunwayDelaySummaryData(void)
{
}
