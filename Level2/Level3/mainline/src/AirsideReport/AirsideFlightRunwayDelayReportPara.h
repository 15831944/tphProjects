#pragma once
#include "parameters.h"
#include "../common/elaptime.h"
#include "AirsideReportNode.h"
#include "AirsideRunwayUtilizationReportParam.h"
#include "AirsideFlightRunwayDelayData.h"

class AirsideFlightRunwayDelayLog;
class AirsideFlightRunwayDelaySummaryData;
class AIRSIDEREPORT_API AirsideFlightRunwayDelayReportPara: public CAirsideRunwayUtilizationReportParam
{
public:
	AirsideFlightRunwayDelayReportPara(void);
	~AirsideFlightRunwayDelayReportPara(void);

	bool IsDelayLogFitPara(const AirsideFlightRunwayDelayLog* pLog, const AirsideFlightDescStruct& fltDesc);
	bool IsDetailDataFitChartType(const AirsideFlightRunwayDelayData::RunwayDelayItem* pData);
	bool IsSummaryDataFitChartType(const AirsideFlightRunwayDelaySummaryData* pData);
	char getFltTypeMode();

protected:
	virtual CString GetReportParamName();
};
