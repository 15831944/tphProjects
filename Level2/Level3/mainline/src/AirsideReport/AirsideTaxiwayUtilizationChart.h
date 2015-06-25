#pragma once
#include <vector>
#include "CARC3DChart.h"
#include "Parameters.h"

class CTaxiwayUtilizationData;
class TaxiwayUtilizationData;
///for graph draw///////////////////////////////////////////////////////////////////////////////////////////
///base class//////////////////////////////////////////////////////////////////////////////////////////////
class CCTaxiwayUtilizationBaseChart
{
public:
	CCTaxiwayUtilizationBaseChart();
	virtual ~CCTaxiwayUtilizationBaseChart();

	class TaxiwayUtilizationData
	{
	public:
		TaxiwayUtilizationData()
		{

		}
		~TaxiwayUtilizationData()
		{

		}
	public:
		CString m_sFlightInfo;
		std::vector<double>m_Data;
	};
public:
	virtual void GenerateResult(std::vector<CTaxiwayUtilizationData*>& vResult,CParameters *pParameter) = 0;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;

protected:
	std::vector<CString>m_vTimeRange;
	std::vector<TaxiwayUtilizationData>m_vTaxiwayDatal;
};

//////////////////detail graph//////////////////////////////////////////////////////////////////
/////////detail movement graph/////////////////////////////////////////////////////////////////
class CTaxiwayUtilizationDetailMovementChart : public CCTaxiwayUtilizationBaseChart
{
public:
	CTaxiwayUtilizationDetailMovementChart();
	~CTaxiwayUtilizationDetailMovementChart();

	void GenerateResult(std::vector<CTaxiwayUtilizationData*>& vResult,CParameters *pParameter);
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
};

//////////detail speed graph///////////////////////////////////////////////////////////////////
class CTaxiwayUtilizationDetailSpeedChart : public CCTaxiwayUtilizationBaseChart
{
public:
	CTaxiwayUtilizationDetailSpeedChart();
	~CTaxiwayUtilizationDetailSpeedChart();

	void GenerateResult(std::vector<CTaxiwayUtilizationData*>& vResult,CParameters *pParameter);
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
};

/////////detail occupancy graph//////////////////////////////////////////////////////////////
class CTaxiwayUtilizationDetailOccupancyChart : public CCTaxiwayUtilizationBaseChart
{
public:
	CTaxiwayUtilizationDetailOccupancyChart();
	~CTaxiwayUtilizationDetailOccupancyChart();

	void GenerateResult(std::vector<CTaxiwayUtilizationData*>& vResult,CParameters *pParameter);
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
};
///////////////////summary graph//////////////////////////////////////////////////////////////
////////////summary movement graph///////////////////////////////////////////////////////////
class CTaxiwayUtilizationSummaryMovementChart : public CCTaxiwayUtilizationBaseChart
{
public:
	CTaxiwayUtilizationSummaryMovementChart();
	~CTaxiwayUtilizationSummaryMovementChart();

	void GenerateResult(std::vector<CTaxiwayUtilizationData*>& vResult,CParameters *pParameter);
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

protected:
	std::vector<CTaxiwayUtilizationData*>m_vResult;
};

///////////////////summary speed graph///////////////////////////////////////////////////////
class CTaxiwayUtilizationSummarySpeedChart : public CCTaxiwayUtilizationBaseChart
{
public:
	CTaxiwayUtilizationSummarySpeedChart();
	~CTaxiwayUtilizationSummarySpeedChart();

	void GenerateResult(std::vector<CTaxiwayUtilizationData*>& vResult,CParameters *pParameter);
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

protected:
	std::vector<CTaxiwayUtilizationData*>m_vResult;
};

/////////////////summary occupancy graph//////////////////////////////////////////////////////
class CTaxiwayUtilizationSummaryOccupancyChart : public CCTaxiwayUtilizationBaseChart
{
public:
	CTaxiwayUtilizationSummaryOccupancyChart();
	~CTaxiwayUtilizationSummaryOccupancyChart();

	void GenerateResult(std::vector<CTaxiwayUtilizationData*>& vResult,CParameters *pParameter);
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

protected:
	std::vector<CTaxiwayUtilizationData*>m_vResult;
};