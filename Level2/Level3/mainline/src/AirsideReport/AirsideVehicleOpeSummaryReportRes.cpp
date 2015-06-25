#include "StdAfx.h"
#include ".\airsidevehicleopesummaryreportres.h"
#include "AirsideVehicleOperaterReport.h"
#include "airsidevehicleoperparameter.h"
#include "../Common/UnitsManager.h"
#include "../InputAirside/ARCUnitManager.h"
void CAirsideVehicleOpeSummaryReportRes::InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter)
{
	CString strLabel = _T("");
	c2dGraphData.m_strChartTitle.Format(_T("%s "),m_TittleName)  ;
	c2dGraphData.m_strYtitle = _T("Duration activity(Minute)");
	c2dGraphData.m_strXtitle = _T(_T("Time In Day")) ;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s Vehicle Type:%s"),m_TittleName, pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime(),\
		CAirsideVehicleOperaterReport::GetVehicleTypeNameById(((CAirsideVehicleOperParameter*)m_pParameter)->GetSelVehicleType()));
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;
}
void CAirsideVehicleOpeSummaryReportRes::InitGtDataItem(C2DChartData& c2dGraphData)
{
	CString Timeranger ;
	if(m_GtResult.m_Data.empty())
		return ;
	for (int i = 0 ; i < (int)m_GtResult.m_Data.size() ;i++)
	{

		Timeranger.Format(_T("%s-%s"),(m_GtResult.m_Data[i].m_StartTime).printTime(),m_GtResult.m_Data[i].m_EndTime.printTime()) ;
		c2dGraphData.m_vrXTickTitle.push_back(Timeranger) ;
	}

	c2dGraphData.m_vrLegend.push_back(_T("Min"));
	c2dGraphData.m_vrLegend.push_back(_T("Avg"));
	c2dGraphData.m_vrLegend.push_back(_T("Max"));
	c2dGraphData.m_vrLegend.push_back(_T("Q1"));
	c2dGraphData.m_vrLegend.push_back(_T("Q2"));
	c2dGraphData.m_vrLegend.push_back(_T("Q3"));
	c2dGraphData.m_vrLegend.push_back(_T("P1"));
	c2dGraphData.m_vrLegend.push_back(_T("P5"));
	c2dGraphData.m_vrLegend.push_back(_T("P90"));
	c2dGraphData.m_vrLegend.push_back(_T("P95"));
	c2dGraphData.m_vrLegend.push_back(_T("P99"));
}

void CAirsideVehicleOpeSummaryReportRes::GenerateResult()
{
	if(m_FinalResultSummary->empty())
		return ;
	int vehicletype = ((CAirsideVehicleOperParameter*)m_pParameter)->GetSelVehicleType() ;
	for (int i = 0 ; i < (int)m_FinalResultSummary->size() ;i++)
	{
		if((*m_FinalResultSummary)[i].m_VehicleType == vehicletype)
			m_GtResult.m_Data.push_back((*m_FinalResultSummary)[i]);
	}
	m_GtResult.m_VehicleType = CAirsideVehicleOperaterReport::GetVehicleTypeNameById(vehicletype) ;
}
void CAirsideVehicleOpeSummaryReportRes::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	C2DChartData c2dGraphData;
	InitGtChartShowMessage(c2dGraphData,pParameter) ; 
	InitGtDataItem(c2dGraphData) ;
	InitGtIntervalData(chartWnd,c2dGraphData) ;
	DrawGt(chartWnd,c2dGraphData) ;
}
void CAirsideVehicleOpeSummaryReportRes::DrawGt(CARC3DChart& chartWnd,C2DChartData& c2dGraphData)
{
	c2dGraphData.m_vr2DChartData.push_back(vSegmentData_Min);
	c2dGraphData.m_vr2DChartData.push_back(vSegmentData_Avg);
	c2dGraphData.m_vr2DChartData.push_back(vSegmentData_Max);
	c2dGraphData.m_vr2DChartData.push_back(vSegmentData_Q1);
	c2dGraphData.m_vr2DChartData.push_back(vSegmentData_Q2);
	c2dGraphData.m_vr2DChartData.push_back(vSegmentData_Q3);
	c2dGraphData.m_vr2DChartData.push_back(vSegmentData_p1);
	c2dGraphData.m_vr2DChartData.push_back(vSegmentData_p5);
	c2dGraphData.m_vr2DChartData.push_back(vSegmentData_p90);
	c2dGraphData.m_vr2DChartData.push_back(vSegmentData_p95);
	c2dGraphData.m_vr2DChartData.push_back(vSegmentData_p99);
	chartWnd.DrawChart(c2dGraphData);
}
//////////////////////////////////////////////////////////////////////////

void CAirsideVehicleOpeSummaryReportResForOnRoadTime::InitGtIntervalData(CARC3DChart& chartWnd,C2DChartData& c2dGraphData)
{
	int nSegmentCount = (int)m_GtResult.m_Data.size();
	vector<double> vSegmentData;
	double count = 0 ; 
	ElapsedTime time ;
	for (int i=0; i<nSegmentCount; i++)
	{
		CSummaryFinialResult FinialResult = m_GtResult.m_Data[i] ;
		time.setPrecisely(FinialResult.m_MinTimeInRoad) ;
		vSegmentData_Min.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_AvgTimeInRoad) ;
		vSegmentData_Avg.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_MaxTimeInRoad) ;
		vSegmentData_Max.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_TimeInRoadQ1) ;
		vSegmentData_Q1.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_TimeInRoadQ2) ;
		vSegmentData_Q2.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_TimeInRoadQ3) ;
		vSegmentData_Q3.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_TimeInRoadP1) ;
		vSegmentData_p1.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_TimeInRoadP5) ;
		vSegmentData_p5.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_TimeInRoadP90) ;
		vSegmentData_p90.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_TimeInRoadP95) ;
		vSegmentData_p95.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_TimeInRoadP99) ;
		vSegmentData_p99.push_back(time.asMinutes());
	}

}

//////////////////////////////////////////////////////////////////////////

void CAirsideVehicleOpeSummaryReportResForOnServerTime::InitGtIntervalData(CARC3DChart& chartWnd,C2DChartData& c2dGraphData)
{
	int nSegmentCount = (int)m_GtResult.m_Data.size();
	vector<double> vSegmentData;
	double count = 0 ; 
	for (int i=0; i<nSegmentCount; i++)
	{
		ElapsedTime time ;

		CSummaryFinialResult FinialResult = m_GtResult.m_Data[i] ;

		time.setPrecisely(FinialResult.m_MinTimeInServer) ;
		vSegmentData_Min.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_AvgTimeInServer) ;
		vSegmentData_Avg.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_MaxTimeInServer) ;
		vSegmentData_Max.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_TimeInServerQ1) ;
		vSegmentData_Q1.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_TimeInServerQ2) ;
		vSegmentData_Q2.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_TimeInServerQ3) ;
		vSegmentData_Q3.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_TimeInServerP1) ;
		vSegmentData_p1.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_TimeInServerP5) ;
		vSegmentData_p5.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_TimeInServerP90) ;
		vSegmentData_p90.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_TimeInServerP95) ;
		vSegmentData_p95.push_back(time.asMinutes());

		time.setPrecisely(FinialResult.m_TimeInServerP99) ;
		vSegmentData_p99.push_back(time.asMinutes());
	}
}
//////////////////////////////////////////////////////////////////////////
void CAirsideVehicleOpeSummaryReportResForDis::InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter)
{
	
	CString strLabel = _T("");
	c2dGraphData.m_strChartTitle.Format(_T("%s "),m_TittleName)  ;
	c2dGraphData.m_strYtitle.Format(_T("Total Distance(%s)"),CARCLengthUnit::GetLengthUnitString(CARCUnitManager::GetInstance().GetCurSelLengthUnit()));
	c2dGraphData.m_strXtitle = _T(_T("Time In Day")) ;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s Vehicle Type:%s"),m_TittleName, pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime(),CAirsideVehicleOperaterReport::GetVehicleTypeNameById(((CAirsideVehicleOperParameter*)m_pParameter)->GetSelVehicleType()));
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;
}
void CAirsideVehicleOpeSummaryReportResForDis::InitGtIntervalData(CARC3DChart& chartWnd,C2DChartData& c2dGraphData)
{
	int nSegmentCount = (int)m_GtResult.m_Data.size();
	vector<double> vSegmentData;
	double count = 0 ; 
	for (int i=0; i<nSegmentCount; i++)
	{
		CSummaryFinialResult FinialResult = m_GtResult.m_Data[i] ;
		vSegmentData_Min.push_back(CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),FinialResult.m_MinDis));
		vSegmentData_Avg.push_back(CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),FinialResult.m_AvgDis));
		vSegmentData_Max.push_back(CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),FinialResult.m_MaxDis));
		vSegmentData_Q1.push_back(CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),FinialResult.m_DisQ1));
		vSegmentData_Q2.push_back(CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),FinialResult.m_DisQ2));
		vSegmentData_Q3.push_back(CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),FinialResult.m_DisQ3));
		vSegmentData_p1.push_back(CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),FinialResult.m_DisP1));
		vSegmentData_p5.push_back(CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),FinialResult.m_Disp5));
		vSegmentData_p90.push_back(CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),FinialResult.m_DisP90));
		vSegmentData_p95.push_back(CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),FinialResult.m_DisP95));
		vSegmentData_p99.push_back(CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),FinialResult.m_DisP99));

	}
}
//////////////////////////////////////////////////////////////////////////
void CAirsideVehicleOpeSummaryReportResForFuelConsumed::InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter)
{
	CString strLabel = _T("");
	c2dGraphData.m_strChartTitle.Format(_T("%s "),m_TittleName)  ;
	c2dGraphData.m_strYtitle.Format(_T("Total Fuel Consumed(%s)"),_T("liter"));
	c2dGraphData.m_strXtitle = _T(_T("Time In Day")) ;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s Vehicle Type:%s"),m_TittleName, pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime(),CAirsideVehicleOperaterReport::GetVehicleTypeNameById(((CAirsideVehicleOperParameter*)m_pParameter)->GetSelVehicleType()));
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;
}
void CAirsideVehicleOpeSummaryReportResForFuelConsumed::InitGtIntervalData(CARC3DChart& chartWnd,C2DChartData& c2dGraphData)
{
	int nSegmentCount = (int)m_GtResult.m_Data.size();
	vector<double> vSegmentData;
	double count = 0 ; 
	for (int i=0; i<nSegmentCount; i++)
	{
		CSummaryFinialResult FinialResult = m_GtResult.m_Data[i] ;
		vSegmentData_Min.push_back(FinialResult.m_MinFuel);
		vSegmentData_Avg.push_back(FinialResult.m_AvgFuel);
		vSegmentData_Max.push_back(FinialResult.m_MaxFuel);
		vSegmentData_Q1.push_back(FinialResult.m_FuelQ1);
		vSegmentData_Q2.push_back(FinialResult.m_FuelQ2);
		vSegmentData_Q3.push_back(FinialResult.m_FuelQ3);
		vSegmentData_p1.push_back(FinialResult.m_FuelP1);
		vSegmentData_p5.push_back(FinialResult.m_FuelP5);
		vSegmentData_p90.push_back(FinialResult.m_FuelP90);
		vSegmentData_p95.push_back(FinialResult.m_FuelP95);
		vSegmentData_p99.push_back(FinialResult.m_FuelP99);
	}
}
//////////////////////////////////////////////////////////////////////////
void CAirsideVehicleOpeSummaryReportResForSpeed::InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter)
{

	CString strLabel = _T("");
	c2dGraphData.m_strChartTitle.Format(_T("%s "),m_TittleName)  ;
	c2dGraphData.m_strYtitle.Format(_T("Speed(%s)"), CARCVelocityUnit::GetVelocityUnitString(CARCUnitManager::GetInstance().GetCurSelVelocityUnit()));
	c2dGraphData.m_strXtitle = _T(_T("Time In Day")) ;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s Vehicle Type:%s"),m_TittleName, pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime(),CAirsideVehicleOperaterReport::GetVehicleTypeNameById(((CAirsideVehicleOperParameter*)m_pParameter)->GetSelVehicleType()));
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;
}
void CAirsideVehicleOpeSummaryReportResForSpeed::InitGtIntervalData(CARC3DChart& chartWnd,C2DChartData& c2dGraphData)
{
	int nSegmentCount = (int)m_GtResult.m_Data.size();
	vector<double> vSegmentData;
	double count = 0 ; 
	for (int i=0; i<nSegmentCount; i++)
	{
		CSummaryFinialResult FinialResult = m_GtResult.m_Data[i] ;
		vSegmentData_Min.push_back(CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),FinialResult.m_MinSpeed*36/1000));
		vSegmentData_Avg.push_back(CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),FinialResult.m_AvgSpeed*36/1000));
		vSegmentData_Max.push_back(CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),FinialResult.m_MaxSpeed*36/1000));
		vSegmentData_Q1.push_back(CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),FinialResult.m_SpeedQ1*36/1000));
		vSegmentData_Q2.push_back(CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),FinialResult.m_SpeedQ2*36/1000));
		vSegmentData_Q3.push_back(CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),FinialResult.m_SpeedQ3*36/1000));
		vSegmentData_p1.push_back(CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),FinialResult.m_SpeedP1*36/1000));
		vSegmentData_p5.push_back(CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),FinialResult.m_SpeedP5*36/1000));
		vSegmentData_p90.push_back(CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),FinialResult.m_SpeedP90*36/1000));
		vSegmentData_p95.push_back(CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),FinialResult.m_SpeedP95*36/1000));
		vSegmentData_p99.push_back(CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),FinialResult.m_SpeedP99*36/1000));
	}
}
void CAirsideVehicleOpeSummaryReportResForUtilization::InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter)
{
	CString strLabel = _T("");
	c2dGraphData.m_strChartTitle.Format(_T("%s "),m_TittleName)  ;
	c2dGraphData.m_strYtitle.Format(_T("Total Time(%s)"),_T("Minute"));
	c2dGraphData.m_strXtitle = _T(_T("Time In Day")) ;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s Vehicle Type:%s"),m_TittleName, pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime(),CAirsideVehicleOperaterReport::GetVehicleTypeNameById(((CAirsideVehicleOperParameter*)m_pParameter)->GetSelVehicleType()));
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;
}
void CAirsideVehicleOpeSummaryReportResForUtilization::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	C2DChartData c2dGraphData;
	InitGtChartShowMessage(c2dGraphData,pParameter) ;
	CString Timeranger ;
	if(m_GtResult.m_Data.empty())
		return ;
	for (int i = 0 ; i < (int)m_GtResult.m_Data.size() ;i++)
	{

		Timeranger.Format(_T("%s-%s"),(m_GtResult.m_Data[i].m_StartTime).printTime(),m_GtResult.m_Data[i].m_EndTime.printTime()) ;
		c2dGraphData.m_vrXTickTitle.push_back(Timeranger) ;
	}

	c2dGraphData.m_vrLegend.push_back(_T("On Road"));
	c2dGraphData.m_vrLegend.push_back(_T("In Service"));
	int nSegmentCount = (int)m_GtResult.m_Data.size();

	vector<double> TotalOnRoad;
	vector<double> TotalInService;
	ElapsedTime time ;
	for (int i=0; i<nSegmentCount; i++)
	{
		CSummaryFinialResult FinialResult = m_GtResult.m_Data[i] ;
		time.setPrecisely(FinialResult.m_TotalInRoadTime) ;
		TotalOnRoad.push_back(time.asMinutes());
		time.setPrecisely(FinialResult.m_totalTimeServer) ;
		TotalInService.push_back(time.asMinutes());
	}
	c2dGraphData.m_vr2DChartData.push_back(TotalOnRoad) ;
	c2dGraphData.m_vr2DChartData.push_back(TotalInService) ;
	chartWnd.DrawChart(c2dGraphData);
}