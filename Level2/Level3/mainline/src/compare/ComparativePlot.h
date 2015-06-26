#pragma once
#include ".\ThreeDChartType.h"
#include "..\AirsideReport\CARC3DChart.h"

class CCmpBaseReport;
class CComparativeQLengthReport;
class CComparativeQTimeReport;
class CComparativeThroughputReport;
class CComparativeSpaceDensityReport;
class CComparativePaxCountReport;
class CComparativeAcOperationReport;
class CComparativeTimeTerminalReport;
class CComparativeDistanceTravelReport;
class CComparativSpaceThroughputReport;
class CComparativeProcUtilizationReport;
class CModelToCompare;

class CComparativePlot
{
public:
	CComparativePlot(ThreeDChartType iType,	CARC3DChart& pChart)
		: m_iType(iType)
		, m_3DChart(pChart)
	{
	}

	bool Draw3DChart(CCmpBaseReport& _reportData,int nSubType);
	bool Update3DChart(ThreeDChartType iType);

private:
	void Init3DChart();
	void UniformAppearances();

	bool Draw3DChart(CComparativeQTimeReport& _reportData,int nSubType);
	bool Draw3DChart(CComparativeQLengthReport& _reportData,int nSubType);
	bool Draw3DChart(CComparativeThroughputReport& _reportData,int nSubType);
	bool Draw3DChart(CComparativeSpaceDensityReport& _reportData,int nSubType);
	bool Draw3DChart(CComparativSpaceThroughputReport& _reportData);
	bool Draw3DChart(CComparativePaxCountReport& _reportData);
	bool Draw3DChart(CComparativeAcOperationReport& _reportData);
	bool Draw3DChart(CComparativeTimeTerminalReport& _reportData);
	bool Draw3DChart(CComparativeDistanceTravelReport & _reportData);
    bool Draw3DChart(CComparativeProcUtilizationReport & _reportData);
	ThreeDChartType m_iType;
	CARC3DChart& m_3DChart;
};

