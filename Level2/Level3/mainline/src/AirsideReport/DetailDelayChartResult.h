#pragma once
#include "AirsideReportBaseResult.h"

class CParameters;
class AirsideFlightRunwayDelayData;
class DetailDelayChartResult : public CAirsideReportBaseResult
{
public:
	DetailDelayChartResult(void);
	~DetailDelayChartResult(void);

public:
	void GenerateResult(AirsideFlightRunwayDelayData* pData);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

private:
	AirsideFlightRunwayDelayData*  m_pData;
};
