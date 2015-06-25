#pragma once
#include "airsidevehicleopereportresult.h"
class CSummaryFinialResult ;
class CSummaryGTResult
{
public:
	CString m_VehicleType ;
	std::vector<CSummaryFinialResult> m_Data ;
};

class CAirsideVehicleOpeSummaryReportRes :
	public CAirsideVehicleOpeReportResult
{
public:
	CAirsideVehicleOpeSummaryReportRes(std::vector<CSummaryFinialResult>* _FinalResultSummary ,CParameters *pParameter)
		:CAirsideVehicleOpeReportResult(),m_FinalResultSummary(_FinalResultSummary) ,m_pParameter(pParameter)
	{

	}
	virtual ~CAirsideVehicleOpeSummaryReportRes(void){};
	void GenerateResult() ;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) ;
protected:
	std::vector<CSummaryFinialResult>* m_FinalResultSummary ;
	CParameters* m_pParameter ;
	CString m_TittleName ;
	CSummaryGTResult m_GtResult ;

	//
	vector<double> vSegmentData_Min;
	vector<double> vSegmentData_Max;
	vector<double> vSegmentData_Avg;
	vector<double> vSegmentData_Q1;
	vector<double> vSegmentData_Q2;
	vector<double> vSegmentData_Q3;
	vector<double> vSegmentData_p1;
	vector<double> vSegmentData_p5;
	vector<double> vSegmentData_p90;
	vector<double> vSegmentData_p95;
	vector<double> vSegmentData_p99;
	//
protected:
	virtual void InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter)  ;
	virtual void InitGtDataItem(C2DChartData& c2dGraphData) ;
	virtual void InitGtIntervalData(CARC3DChart& chartWnd,C2DChartData& c2dGraphData) = 0;
	void DrawGt(CARC3DChart& chartWnd,C2DChartData& c2dGraphData) ;
};

//////////////////////////////////////////////////////////////////////////
class CAirsideVehicleOpeSummaryReportResForOnRoadTime : public CAirsideVehicleOpeSummaryReportRes
{
public:
	CAirsideVehicleOpeSummaryReportResForOnRoadTime(std::vector<CSummaryFinialResult>* _FinalResultSummary ,CParameters *pParameter)
		:CAirsideVehicleOpeSummaryReportRes(_FinalResultSummary,pParameter)
	{
		m_TittleName.Format(_T("On Road Time(Summary)")) ;
	}
	~CAirsideVehicleOpeSummaryReportResForOnRoadTime() {} ;
protected:
	void InitGtIntervalData(CARC3DChart& chartWnd,C2DChartData& c2dGraphData) ;
};
class CAirsideVehicleOpeSummaryReportResForOnServerTime : public CAirsideVehicleOpeSummaryReportRes
{
public:
	CAirsideVehicleOpeSummaryReportResForOnServerTime(std::vector<CSummaryFinialResult>* _FinalResultSummary ,CParameters *pParameter)
		:CAirsideVehicleOpeSummaryReportRes(_FinalResultSummary,pParameter)
	{
		m_TittleName.Format(_T("On Server Time(Summary)")) ;
	}
	~CAirsideVehicleOpeSummaryReportResForOnServerTime() {} ;
protected:
	void InitGtIntervalData(CARC3DChart& chartWnd,C2DChartData& c2dGraphData) ;
};
class CAirsideVehicleOpeSummaryReportResForDis : public CAirsideVehicleOpeSummaryReportRes
{
public:
	CAirsideVehicleOpeSummaryReportResForDis(std::vector<CSummaryFinialResult>* _FinalResultSummary ,CParameters *pParameter)
		:CAirsideVehicleOpeSummaryReportRes(_FinalResultSummary,pParameter)
	{
		m_TittleName.Format(_T("Total Distance(Summary)")) ;
	}
	~CAirsideVehicleOpeSummaryReportResForDis() {} ;
protected:
	void InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter) ;
	void InitGtIntervalData(CARC3DChart& chartWnd,C2DChartData& c2dGraphData) ;
};
class CAirsideVehicleOpeSummaryReportResForFuelConsumed : public CAirsideVehicleOpeSummaryReportRes
{
public:
	CAirsideVehicleOpeSummaryReportResForFuelConsumed(std::vector<CSummaryFinialResult>* _FinalResultSummary ,CParameters *pParameter)
		:CAirsideVehicleOpeSummaryReportRes(_FinalResultSummary,pParameter)
	{
		m_TittleName.Format(_T("Total Fuel Consumed(Summary)")) ;
	}
	~CAirsideVehicleOpeSummaryReportResForFuelConsumed() {} ;
protected:
	void InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter) ;
	void InitGtIntervalData(CARC3DChart& chartWnd,C2DChartData& c2dGraphData) ;
};
class CAirsideVehicleOpeSummaryReportResForSpeed : public CAirsideVehicleOpeSummaryReportRes
{
public:
	CAirsideVehicleOpeSummaryReportResForSpeed(std::vector<CSummaryFinialResult>* _FinalResultSummary ,CParameters *pParameter)
		:CAirsideVehicleOpeSummaryReportRes(_FinalResultSummary,pParameter)
	{
		m_TittleName.Format(_T("Speed(Summary)")) ;
	}
	~CAirsideVehicleOpeSummaryReportResForSpeed() {} ;
protected:
	void InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter) ;
	void InitGtIntervalData(CARC3DChart& chartWnd,C2DChartData& c2dGraphData) ;
};

class CAirsideVehicleOpeSummaryReportResForUtilization : public CAirsideVehicleOpeSummaryReportRes
{
public:
	CAirsideVehicleOpeSummaryReportResForUtilization(std::vector<CSummaryFinialResult>* _FinalResultSummary ,CParameters *pParameter)
		:CAirsideVehicleOpeSummaryReportRes(_FinalResultSummary,pParameter)
	{
		m_TittleName.Format(_T("Utilization(Summary)")) ;
	}
	~CAirsideVehicleOpeSummaryReportResForUtilization() {} ;
public:
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) ;
	void InitGtIntervalData(CARC3DChart& chartWnd,C2DChartData& c2dGraphData){} ;
	void InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter) ;
};
