#pragma once
#include "LSVehicleTotalDelayResult.h"
#include <map>

class LANDSIDEREPORT_API LSVehicleTotalDelayDetailResult : public LSVehicleTotalDelayResult
{
public:
	enum ChartType
	{
		CT_TOTALDELAY = 0,
		CT_ACTUAL,
		CT_IDEAL,
	};

	LSVehicleTotalDelayDetailResult(void);
	~LSVehicleTotalDelayDetailResult(void);

	virtual void Draw3DChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter);
	virtual void GenerateResult(CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde);
	virtual void RefreshReport(LandsideBaseParam * parameter);
	virtual void InitListHead( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ );

	virtual void FillListContent( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter );

	virtual LSGraphChartTypeList GetChartList() const;


protected:
	class ChartResult
	{
	public:
		ChartResult(void);
		~ChartResult(void);

	public:
		void AddTime(const CString& strVehicleType,const ElapsedTime& eTime);

	public:
		void Draw3DChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter );

	protected:
		//get count in the range
		int GetCount(std::vector<ElapsedTime>& vTime,ElapsedTime& eMinTime, ElapsedTime& eMaxTime) const;
		CString GetXtitle(ElapsedTime& eMinTime, ElapsedTime& eMaxTime) const;

		ElapsedTime GetMaxTime();
		ElapsedTime GetMaxTime(std::vector<ElapsedTime>& vTime)const;
	private:
		CString GetChartTitle(int iChart)const;
		CString GetXTitle(int iChart)const;
		CString GetYTitle(int iChart)const;

		void SortChartData();
	protected:
		std::map<CString, std::vector<ElapsedTime> >m_mapDelayResult;
	};

};
