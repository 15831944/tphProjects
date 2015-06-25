#pragma once
#include "../Engine/Airside/CommonInSim.h"
#include "SummaryData.h"

class CSummaryComponentData
{
public:
	CSummaryComponentData(void);
	~CSummaryComponentData(void);

public:
public:
	FltDelayReason      m_DelayReason;
	CSummaryData        m_SummaryDelayData;
};
