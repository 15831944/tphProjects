#pragma once
#include "FlightStandOperationChartResult.h"


struct StandOccupancyData
{
	CString m_sName;
	std::vector<double>m_vStandData;
};
///////detail occupancy utilization/////////////////////////////////////////////////////////////////////////
class CDetailStandOccupancyUtilizationChartResult : public CFlightStandOperationChartResult
{
public:
	CDetailStandOccupancyUtilizationChartResult();
	virtual ~CDetailStandOccupancyUtilizationChartResult();

	virtual void GenerateResult(std::vector<CStandOperationReportData*>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

private:
	std::vector<StandOccupancyData> m_vOccupancyData;
};


///////////detail percentage////////////////////////////////////////////////////////////////////////////////
class CDetailStandUtilizationPercentageChartResult : public CFlightStandOperationChartResult
{
public:
	CDetailStandUtilizationPercentageChartResult();
	virtual ~CDetailStandUtilizationPercentageChartResult();

	virtual void GenerateResult(std::vector<CStandOperationReportData*>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

private:
	std::vector<StandOccupancyData> m_vOccupancyData;
};

//////////detail idle time utilization//////////////////////////////////////////////////////////////////////
class CDetailStandIdleTimeUtiliztionChartResult : public CFlightStandOperationChartResult
{
public:
	CDetailStandIdleTimeUtiliztionChartResult();
	virtual ~CDetailStandIdleTimeUtiliztionChartResult();

	virtual void GenerateResult(std::vector<CStandOperationReportData*>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

private:
	std::vector<StandOccupancyData> m_vOccupancyData;
};

//////////detail conflict////////////////////////////////////////////////////////////////////////////////////
class CDetailStandConflictChartResult : public CFlightStandOperationChartResult
{
public:
	CDetailStandConflictChartResult();
	virtual ~CDetailStandConflictChartResult();

public:
	virtual void GenerateResult(std::vector<CStandOperationReportData*>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
private:
	std::vector<StandOccupancyData> m_vOccupancyData;
};


//////////detail delay//////////////////////////////////////////////////////////////////////////////////////
class CDetailStandDelayChartResult : public CFlightStandOperationChartResult
{
public:
	CDetailStandDelayChartResult();
	virtual ~CDetailStandDelayChartResult();
public:
	virtual void GenerateResult(std::vector<CStandOperationReportData*>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
private:
	std::vector<StandOccupancyData> m_vOccupancyData;
};

