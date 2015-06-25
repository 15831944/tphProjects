#pragma once
#include "../common/elaptime.h"
#include "../Reports/StatisticalTools.h"
class AirsideFlightRunwayDelaySummaryData
{
public:
	AirsideFlightRunwayDelaySummaryData(void);
	~AirsideFlightRunwayDelaySummaryData(void);

	ElapsedTime m_tStart;
	ElapsedTime m_tEnd;

	int m_nPosition;
	int m_nDelayCount;

	long m_lTotal;
	long m_lMin;
	long m_lMax;
	long m_lAvg;

	CStatisticalTools<double> m_statisticalTool;

};
