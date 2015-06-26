#pragma once
#include "lsvehicledelayresult.h"

class LANDSIDEREPORT_API LSVehicleDelayDetailResult :
	public LSVehicleDelayResult
{

public:
	enum ChartType
	{
		CT_DelayTime = 0,
		CT_Reason,
		CT_Operation,
	};
public:
	LSVehicleDelayDetailResult(void);
	~LSVehicleDelayDetailResult(void);


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
	virtual void InitListHead( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ );

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		insert result of onboard report into list ctrl
	//Parameter:
	//		cxListCtrl: output parameter and target list ctrl to operate
	//		parameter: input parameter let knows how to fill list ctrl
	//------------------------------------------------------------------------------------------
	virtual void FillListContent( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter );

	virtual LSGraphChartTypeList GetChartList() const;



protected:
	class ChartResult
	{
	public:
		ChartResult(void);
		~ChartResult(void);

	public:
	//	void AddTime(const ElapsedTime& eTime);
		void AddChartData(const CString& strVehicleType,int iData);
	public:
		void Draw3DChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter );

	protected:
		void DrawDelay3DChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter);
		void DrawReason3DChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter);
		void DrawOperation3DChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter);

		//get count in the range
		int GetCount(std::vector<int>& vTime,ElapsedTime& eMinTime, ElapsedTime& eMaxTime) const;
		CString GetXtitle(ElapsedTime& eMinTime, ElapsedTime& eMaxTime) const;
		ElapsedTime GetMaxTime();
		ElapsedTime GetMaxTime(std::vector<int>& vTime)const;

		int GetReasonCount(std::vector<int>& vReason,int iReason)const;
		CString GetReasonTitle(LandsideVehicleDelayLog::EnumDelayReason iReason)const;

		int GetOperationCount(std::vector<int>& vOperation,int iOperaton)const;
		CString GetOperationTitle(LandsideVehicleDelayLog::EnumDelayOperation iOperation)const;

		void SortChartData();
	protected:
		//std::vector< ElapsedTime> m_vTime;
		std::map<CString, std::vector<int> >m_mapDelayResult;
	};


};
