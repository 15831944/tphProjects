#pragma once
#include "AirsideFlightDelayReport.h"
#include <vector>
#include "SummaryData.h"
using namespace std;

class CSummarySegDelayData
{
public:
	CSummarySegDelayData(void);
	~CSummarySegDelayData(void);

public:
public:
	CAirsideFlightDelayReport::FltDelaySegment     m_DelaySegmentType;
	CSummaryData                                   m_SummaryDelayData;
};
