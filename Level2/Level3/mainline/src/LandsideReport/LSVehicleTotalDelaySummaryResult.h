#pragma once
#include "LSVehicleTotalDelayResult.h"
#include <map>

class LANDSIDEREPORT_API LSVehicleTotalDelaySummaryResult : public LSVehicleTotalDelayResult
{
public:
	enum ChartType
	{
		CT_S_TotalDelayTime = 0,
	};
	LSVehicleTotalDelaySummaryResult(void);
	~LSVehicleTotalDelaySummaryResult(void);

	virtual void Draw3DChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter);
	virtual void GenerateResult(CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde);
	virtual void RefreshReport(LandsideBaseParam * parameter);

	virtual void InitListHead( CListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ );

	virtual void FillListContent( CListCtrl& cxListCtrl, LandsideBaseParam * parameter );

	virtual LSGraphChartTypeList GetChartList() const;

	virtual BOOL ReadReportData( ArctermFile& _file );
	virtual BOOL WriteReportData( ArctermFile& _file );

protected:
	std::map<CString,LandsideSummaryTime> m_vTimeSummary;
};
