#pragma once
#include "airsidereportbaseresult.h"
class CFlightFuelBurnReportItem ;
class CFlightSummaryFuelBurnReportItem;
class CAirsideFlightFuelBurnParmater ;
class CNumberOfFlightType
{
public:
	FlightConstraint m_FlightType ;
	int m_NumOfAircraft ;
public:
	CNumberOfFlightType():m_NumOfAircraft(0) {} ;
};
class CReportResult
{
public:
	double m_FromFuel ;
	double m_ToFule ;
	std::vector<CNumberOfFlightType*> m_DataResult ;
public:
	void AddFlightReportItem(const CFlightFuelBurnReportItem& _reportItem ) ;
public:
	~CReportResult()
	{
		for (int i = 0 ; i < (int)m_DataResult.size() ;i++)
		{
			delete m_DataResult[i] ;
		}
		m_DataResult.clear() ;
	}
};
class CAirsideFlightFuelBurnReportResult :
	public CAirsideReportBaseResult
{
public:
	CAirsideFlightFuelBurnReportResult(CAirsideFlightFuelBurnParmater* _parameter);
	virtual ~CAirsideFlightFuelBurnReportResult(void);
public:
	virtual void GenerateResult() = 0;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;
protected:
	CAirsideFlightFuelBurnParmater* m_parameter ;

};
class CAirsideFlightFuelBurnDetailReportResult : public CAirsideFlightFuelBurnReportResult
{
protected:
		std::vector<CReportResult*> m_ReportData ;
		std::vector<CFlightFuelBurnReportItem>& m_DetailreportData ;
		typedef std::vector<double>  TY_DOUBLE ;
		typedef std::vector<TY_DOUBLE > TY_DOUBLE_DOUBLE ;
public:
	CAirsideFlightFuelBurnDetailReportResult(std::vector<CFlightFuelBurnReportItem>& _reportData ,CAirsideFlightFuelBurnParmater* _parameter) ;
	~CAirsideFlightFuelBurnDetailReportResult() ;
public:
	virtual void GenerateResult( ) ;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) ;
protected:
	void CalculateFuelBurnInterval(std::vector<CFlightFuelBurnReportItem>& _reportData) ;
	void AddReportFuelBurnItem(const CFlightFuelBurnReportItem& _reportItem) ;
};

class CAirsideFlightFuelBurnSummaryReportResult : public CAirsideFlightFuelBurnReportResult
{
protected:
	std::vector<CFlightSummaryFuelBurnReportItem>& m_SummaryData ;
public:
	CAirsideFlightFuelBurnSummaryReportResult(std::vector<CFlightSummaryFuelBurnReportItem>& _SummaryData ,CAirsideFlightFuelBurnParmater* _parameter):m_SummaryData(_SummaryData),CAirsideFlightFuelBurnReportResult(_parameter) {};
	~CAirsideFlightFuelBurnSummaryReportResult() {};
public:
	virtual void GenerateResult() ;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) ;
};