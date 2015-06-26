#pragma once
#include "lsvehicledelayresult.h"

class LANDSIDEREPORT_API LSVehicleDelaySummaryResult :
	public LSVehicleDelayResult
{
public:
	enum ChartType
	{
		CT_S_DelayTime = 0,



	};
public:
	LSVehicleDelaySummaryResult(void);
	~LSVehicleDelaySummaryResult(void);

	virtual void Draw3DChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter);
	virtual void GenerateResult(CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde);
	virtual void RefreshReport(LandsideBaseParam * parameter);
	//-----------------------------------------------------------------------------------------
	//Summary:
	//		create list control header and set list has sortable
	//Parameter:
	//		cxListCtrl: reference pass, target list ctrl to init
	//		parameter: user input that knows how to create list ctrl header
	//		piSHC: make list ctrl sortable
	//-----------------------------------------------------------------------------------------
	virtual void InitListHead( CListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ );

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		insert result of onboard report into list ctrl
	//Parameter:
	//		cxListCtrl: output parameter and target list ctrl to operate
	//		parameter: input parameter let knows how to fill list ctrl
	//------------------------------------------------------------------------------------------
	virtual void FillListContent( CListCtrl& cxListCtrl, LandsideBaseParam * parameter );

	virtual LSGraphChartTypeList GetChartList() const;


	virtual BOOL ReadReportData( ArctermFile& _file );
	virtual BOOL WriteReportData( ArctermFile& _file );
protected:
	//LandsideSummaryTime m_timeSummary;
	std::map<CString,LandsideSummaryTime> m_vTimeSummary;
};
