#pragma once
#include "SummaryData.h"

class CSummaryNodeDelayDataItem
{
public:
	CSummaryNodeDelayDataItem(void);
	~CSummaryNodeDelayDataItem(void);

public:
	int             m_nNodeID;
	int             m_nNodeType;
	CSummaryData    m_summaryData;
};
