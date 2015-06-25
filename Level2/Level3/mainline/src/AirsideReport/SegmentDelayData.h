#pragma once
#include "AirsideFlightDelayReport.h"
#include <vector>
using namespace std;

class CSegmentDelayData
{
public:
	CSegmentDelayData(void);
	~CSegmentDelayData(void);

public:
	CAirsideFlightDelayReport::FltDelaySegment     m_DelaySegmentType;
	vector<int>                                    m_vData;
};
