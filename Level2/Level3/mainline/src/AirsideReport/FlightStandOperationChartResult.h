#pragma once
#include <vector>
#include "CARC3DChart.h"
#include "Parameters.h"

class CStandOperationReportData;
///for graph draw///////////////////////////////////////////////////////////////////////////////////////////
///base class//////////////////////////////////////////////////////////////////////////////////////////////
class CFlightStandOperationChartResult
{
public:
	CFlightStandOperationChartResult();
	virtual ~CFlightStandOperationChartResult();
public:
	virtual void GenerateResult(std::vector<CStandOperationReportData*>& vResult,CParameters *pParameter) = 0;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;
	void SetUnuseActualStandCount(int iCount);
	void SetUnuseScheduleStandCount(int Count);
protected:
	std::vector<CString> m_vXAxisTitle;
	int m_nUnuseScheduleStandCount;
	int m_nUnuseActualStandCount;
};


