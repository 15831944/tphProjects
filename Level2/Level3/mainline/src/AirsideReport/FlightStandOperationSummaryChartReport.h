#pragma once
#include "FlightStandOperationChartResult.h"

class CStandOperationReportData;
class CSummaryStandOperationData;
class CSummaryStandOperationChartResult: public CFlightStandOperationChartResult
{
public:
	CSummaryStandOperationChartResult();
	virtual ~CSummaryStandOperationChartResult();

	virtual void GenerateResult(std::vector<CStandOperationReportData*>& vResult,CParameters *pParameter);
	void GenerateResult(std::vector<CSummaryStandOperationData*>& vSummaryResult);

	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) =0;

protected:
	std::vector<CSummaryStandOperationData*> m_vResult;
};


/////summary conflict///////////////////////////////////////////////////////////////////////////////////////
class CSummaryStandConflictChartResult : public CSummaryStandOperationChartResult
{
public:
	CSummaryStandConflictChartResult();
	~CSummaryStandConflictChartResult();

public:
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

};

//////summary delay/////////////////////////////////////////////////////////////////////////////////////////
class CSummaryStandDelayChartResult : public CSummaryStandOperationChartResult
{
public:
	CSummaryStandDelayChartResult();
	~CSummaryStandDelayChartResult();

public:
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

};

///////summary sched utilize///////////////////////////////////////////////////////////////////////////////////////////
class CSummarySchedStandUtilizationChartResult : public CSummaryStandOperationChartResult
{
public:
	CSummarySchedStandUtilizationChartResult();
	~CSummarySchedStandUtilizationChartResult();

public:
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
};

//////////summary actual utilize///////////////////////////////////////////////////////////////////////////////////
class CSummaryActualStandUtilizationChartResult : public CSummaryStandOperationChartResult
{
public:
	CSummaryActualStandUtilizationChartResult();
	~CSummaryActualStandUtilizationChartResult();

public:
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

};

///////summary sched idle///////////////////////////////////////////////////////////////////////////////////////////
class CSummarySchedStandIdleChartResult : public CSummaryStandOperationChartResult
{
public:
	CSummarySchedStandIdleChartResult();
	~CSummarySchedStandIdleChartResult();

public:
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
};

//////////summary actual idle///////////////////////////////////////////////////////////////////////////////////
class CSummaryActualStandIdleChartResult : public CSummaryStandOperationChartResult
{
public:
	CSummaryActualStandIdleChartResult();
	~CSummaryActualStandIdleChartResult();

public:
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

};