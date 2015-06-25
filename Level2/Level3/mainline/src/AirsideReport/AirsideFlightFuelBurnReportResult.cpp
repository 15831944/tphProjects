#include "StdAfx.h"
#include ".\airsideflightfuelburnreportresult.h"
#include "AirsideFlightFuelBurnReport.h"
#include "AirsideFlightBurnParmater.h"
CAirsideFlightFuelBurnReportResult::CAirsideFlightFuelBurnReportResult(CAirsideFlightFuelBurnParmater* _parameter):CAirsideReportBaseResult(),m_parameter(_parameter)
{
}

CAirsideFlightFuelBurnReportResult::~CAirsideFlightFuelBurnReportResult(void)
{

}
CAirsideFlightFuelBurnDetailReportResult::CAirsideFlightFuelBurnDetailReportResult(std::vector<CFlightFuelBurnReportItem>& _reportData ,CAirsideFlightFuelBurnParmater* _parameter):CAirsideFlightFuelBurnReportResult(_parameter),m_DetailreportData(_reportData)
{

}
void CAirsideFlightFuelBurnDetailReportResult::GenerateResult()
{
	//calculate fuel interval 
	CalculateFuelBurnInterval(m_DetailreportData) ;

	for (int i = 0 ; i < (int)m_DetailreportData.size() ;i++)
	{
		AddReportFuelBurnItem(m_DetailreportData[i]) ;
	}
}
void CAirsideFlightFuelBurnDetailReportResult::CalculateFuelBurnInterval(std::vector<CFlightFuelBurnReportItem>& _ReportData)
{
	//calculate the max fuel burn 
	double _maxFuelBurn = 0 ;
	double totalFuelBurn = 0 ;
	CNumberOfFlightType* NumberOfFlightType = NULL ;
	CReportResult* PReportResult = NULL ;
	for (int i = 0 ; i < (int)_ReportData.size() ;i++)
	{
		totalFuelBurn = _ReportData[i].m_FuleBurnDep + _ReportData[i].m_FuelBurnArrival ;
		_maxFuelBurn = _maxFuelBurn > totalFuelBurn ?_maxFuelBurn: totalFuelBurn ;
	}
	int  interval = (int)(_maxFuelBurn / 6 );
	if(interval == 0)
	{
		PReportResult = new CReportResult ;
		PReportResult->m_FromFuel = 0 ;
		PReportResult->m_ToFule = _maxFuelBurn ;
		for (int num = 0 ; num < (int)m_parameter->getFlightConstraintCount() ;num++)
		{
			NumberOfFlightType = new CNumberOfFlightType ;
			NumberOfFlightType->m_FlightType = m_parameter->getFlightConstraint(num) ;
			PReportResult->m_DataResult.push_back(NumberOfFlightType) ;
		}
		m_ReportData.push_back(PReportResult) ;
	}else
	{
		int IntervalNum = 6 ;
		while((double)interval*IntervalNum < _maxFuelBurn)
			IntervalNum++ ;
		double _Startinterval = 0 ;
		for (int num = 0 ; num < IntervalNum ;num++)
		{
			PReportResult = new CReportResult ;
			PReportResult->m_FromFuel = _Startinterval ;
			_Startinterval = _Startinterval + interval ;
			PReportResult->m_ToFule = _Startinterval ;
			for (int num = 0 ; num < (int)m_parameter->getFlightConstraintCount() ;num++)
			{
				NumberOfFlightType = new CNumberOfFlightType ;
				NumberOfFlightType->m_FlightType = m_parameter->getFlightConstraint(num) ;
				PReportResult->m_DataResult.push_back(NumberOfFlightType) ;
			}
			m_ReportData.push_back(PReportResult) ;
		}
	}
}
void CAirsideFlightFuelBurnDetailReportResult::AddReportFuelBurnItem(const CFlightFuelBurnReportItem& _reportItem)
{
	CReportResult* pReportRes = NULL ;
	double _FurlBurn = 0 ;
	for (int i =0 ; i < (int)m_ReportData.size() ;i++)
	{
		pReportRes = m_ReportData[i] ;
		_FurlBurn = _reportItem.m_FuelBurnArrival + _reportItem.m_FuleBurnDep ;

		if(_FurlBurn >= pReportRes->m_FromFuel && _FurlBurn < pReportRes->m_ToFule)
			pReportRes->AddFlightReportItem(_reportItem) ;
	}
}

CAirsideFlightFuelBurnDetailReportResult::~CAirsideFlightFuelBurnDetailReportResult()
{
	for (int i = 0 ; i < (int)m_ReportData.size() ;i++)
	{
		delete m_ReportData[i] ;
	}
}

void CAirsideFlightFuelBurnDetailReportResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	if(pParameter == NULL)
		return ;
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle.Format(_T("Flight Fuel Burn Detail"))  ;

	c2dGraphData.m_strYtitle = _T("Number of aircrafts");
	c2dGraphData.m_strXtitle = _T("Fuel Burn(lbs)") ;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "),_T("Flight Fuel Burn Detail"), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;

	CReportResult* pReportRes = NULL ;
	CNumberOfFlightType* pNumberOfFlightType = NULL ;
	CString StrFuelInterval ;
	for (int i = 0 ; i <(int) m_ReportData.size() ;i++)
	{
		pReportRes = m_ReportData[i] ;
		StrFuelInterval.Format(_T("%0.2f-%0.2f"),pReportRes->m_FromFuel,pReportRes->m_ToFule) ;
		c2dGraphData.m_vrXTickTitle.push_back(StrFuelInterval) ;
	}
	for (int i = 0 ; i < (int)pParameter->getFlightConstraintCount(); i++)
	{
		CString FlightType ;
		pParameter->getFlightConstraint(i).screenPrint(FlightType) ;
		c2dGraphData.m_vrLegend.push_back(FlightType) ;
	}
	TY_DOUBLE_DOUBLE Segmentdatas ;
	Segmentdatas.resize(pParameter->getFlightConstraintCount()) ;

	for (int ndx = 0 ; ndx < (int)m_ReportData.size() ;ndx++)
	{
			pReportRes = m_ReportData[ndx] ;

			for (int i = 0 ; i < (int)pReportRes->m_DataResult.size();i++)
			{
				pNumberOfFlightType = pReportRes->m_DataResult[i] ;
					Segmentdatas[ i ].push_back(pNumberOfFlightType->m_NumOfAircraft) ; 
			}
	}

	for (int datasize = 0 ; datasize < (int)Segmentdatas.size() ;datasize++)
	{
		c2dGraphData.m_vr2DChartData.push_back(Segmentdatas.at(datasize));
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CAirsideFlightFuelBurnSummaryReportResult::GenerateResult()
{
	
}

void CAirsideFlightFuelBurnSummaryReportResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	if(pParameter == NULL)
		return ;
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle.Format(_T("Flight Fuel Burn Summary"))  ;

	c2dGraphData.m_strYtitle = _T("Fuel Burn(lbs)");
	c2dGraphData.m_strXtitle = _T("Flight Type") ;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "),_T("Flight Fuel Burn Summary"), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;

	c2dGraphData.m_vrLegend.push_back("Minimum") ;
	c2dGraphData.m_vrLegend.push_back(_T("Average")) ;
	c2dGraphData.m_vrLegend.push_back(_T("Maximum")) ;
	c2dGraphData.m_vrLegend.push_back(_T("Q1")) ;
	c2dGraphData.m_vrLegend.push_back(_T("Q2")) ;
	c2dGraphData.m_vrLegend.push_back(_T("Q3")) ;
	c2dGraphData.m_vrLegend.push_back(_T("P1")) ;
	c2dGraphData.m_vrLegend.push_back(_T("P5")) ;
	c2dGraphData.m_vrLegend.push_back(_T("P10")) ;
	c2dGraphData.m_vrLegend.push_back(_T("P90")) ;
	c2dGraphData.m_vrLegend.push_back(_T("P95")) ;
	c2dGraphData.m_vrLegend.push_back(_T("P99")) ;


	std::vector<double> _MinFuel ;
	std::vector<double> _AvgFuel ;
	std::vector<double> _MaxFuel ;
	std::vector<double> _Q1 ;
	std::vector<double> _Q2 ;
	std::vector<double> _Q3 ;
	std::vector<double> _P1 ;
	std::vector<double> _P5 ;
	std::vector<double> _P10 ;
	std::vector<double> _P90 ;
	std::vector<double> _P95 ;
	std::vector<double> _P99 ;
	CFlightSummaryFuelBurnReportItem SummaryFuelBurn ;
	for (int i = 0 ; i < (int)m_SummaryData.size() ;i++)
	{
		CString strFlt ;
		SummaryFuelBurn = m_SummaryData[i]  ;
		SummaryFuelBurn.m_FlightType.screenPrint(strFlt) ;
		c2dGraphData.m_vrXTickTitle.push_back(strFlt) ;

		_MinFuel.push_back(SummaryFuelBurn.m_minFuelBurn) ;
		_AvgFuel.push_back(SummaryFuelBurn.m_AvgFuelBurn) ;
		_MaxFuel.push_back(SummaryFuelBurn.m_MaxFuelBurn) ;
		_Q1.push_back(SummaryFuelBurn.m_Q1) ;
		_Q2.push_back(SummaryFuelBurn.m_Q2) ;
		_Q3.push_back(SummaryFuelBurn.m_Q3) ;
		_P1.push_back(SummaryFuelBurn.m_P1) ;
		_P5.push_back(SummaryFuelBurn.m_P5) ;
		_P10.push_back(SummaryFuelBurn.m_p10) ;
		_P90.push_back(SummaryFuelBurn.m_p90) ;
		_P95.push_back(SummaryFuelBurn.m_P95) ;
		_P99.push_back(SummaryFuelBurn.m_P99) ;
	}

	c2dGraphData.m_vr2DChartData.push_back(_MinFuel);
	c2dGraphData.m_vr2DChartData.push_back(_AvgFuel);
	c2dGraphData.m_vr2DChartData.push_back(_MaxFuel);
	c2dGraphData.m_vr2DChartData.push_back(_Q1);
	c2dGraphData.m_vr2DChartData.push_back(_Q2);
	c2dGraphData.m_vr2DChartData.push_back(_Q3);
	c2dGraphData.m_vr2DChartData.push_back(_P1);
	c2dGraphData.m_vr2DChartData.push_back(_P5);
	c2dGraphData.m_vr2DChartData.push_back(_P10);
	c2dGraphData.m_vr2DChartData.push_back(_P90);
	c2dGraphData.m_vr2DChartData.push_back(_P95);
	c2dGraphData.m_vr2DChartData.push_back(_P99);
	chartWnd.DrawChart(c2dGraphData);
}

void CReportResult::AddFlightReportItem( const CFlightFuelBurnReportItem& _reportItem )
{
	for (int i = 0 ; i < (int)m_DataResult.size() ;i++)
	{
		CNumberOfFlightType* numberFlightTy ;
		numberFlightTy = m_DataResult[i] ;

		if( numberFlightTy->m_FlightType.fits(_reportItem.m_fltDesc) )
			numberFlightTy->m_NumOfAircraft++ ;
	}
}