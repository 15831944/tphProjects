#pragma once
#include "../compare/Cmp3DChartWnd.h"
#include "../compare/ThreeDChartType.h"
#include "../AirsideReport/CARC3DChart.h"

class CComparativeReportResult;
class CComparativeQLengthReport;
class CComparativeQTimeReport;
class CComparativeThroughputReport;
class CComparativeSpaceDensityReport;
class CComparativePaxCountReport;
class CComparativeAcOperationReport;
class CComparativeTimeTerminalReport;
class CComparativeDistanceTravelReport;
class CModelToCompare;

class CComparativePlot
{
public:
	CComparativePlot(ThreeDChartType iType,	N3DCHARTLib::IDualChartPtr& pChart, const std::vector<CModelToCompare*>& vModelList)
	 : m_iType(iType)
	 , m_pChart(pChart)
	 , m_vModelList(vModelList)
	{
	}

	bool Draw3DChart(const CComparativeReportResult& _reportData);
	bool Update3DChart(ThreeDChartType iType);

private:
	void Init3DChart();
	void UniformAppearances();

	bool Draw3DChart(const CComparativeQTimeReport& _reportData);
	bool Draw3DChart(const CComparativeQLengthReport& _reportData);
	bool Draw3DChart(const CComparativeThroughputReport& _reportData);
	bool Draw3DChart(const CComparativeSpaceDensityReport& _reportData);
	bool Draw3DChart(const CComparativePaxCountReport& _reportData);
	bool Draw3DChart(const CComparativeAcOperationReport& _reportData);
	bool Draw3DChart(const CComparativeTimeTerminalReport& _reportData);
	bool Draw3DChart(const CComparativeDistanceTravelReport & _reportData);

	ThreeDChartType m_iType;
	N3DCHARTLib::IDualChartPtr& m_pChart;
	const std::vector<CModelToCompare*>& m_vModelList;
};

class CComparativePlot_new
{
public:
	CComparativePlot_new(ThreeDChartType iType,	CARC3DChart& pChart)
		: m_iType(iType)
		, m_3DChart(pChart)
	{
	}

	bool Draw3DChart(CComparativeReportResult& _reportData);
	bool Update3DChart(ThreeDChartType iType);

private:
	void Init3DChart();
	void UniformAppearances();

	bool Draw3DChart(CComparativeQTimeReport& _reportData);
	bool Draw3DChart(CComparativeQLengthReport& _reportData);
	/*	bool Draw3DChart(const CComparativeThroughputReport& _reportData);
	bool Draw3DChart(const CComparativeSpaceDensityReport& _reportData);
	bool Draw3DChart(const CComparativePaxCountReport& _reportData);
	bool Draw3DChart(const CComparativeAcOperationReport& _reportData);
	bool Draw3DChart(const CComparativeTimeTerminalReport& _reportData);
	bool Draw3DChart(const CComparativeDistanceTravelReport & _reportData);
*/
	ThreeDChartType m_iType;
	CARC3DChart& m_3DChart;
};

