#include "StdAfx.h"
#include ".\airsidevehicleoperaterreport.h"
#include "../Common/Point2008.h"
#include "../Reports/StatisticalTools.h"
#include "AirsideVehicleOperParameter.h"
#include "../InputAirside/VehicleDefine.h"
#include "../Engine/Airside/CommonInSim.h"
#include "../Common/CARCUnit.h"
#include "../InputAirside/ARCUnitManager.h"
CAirsideVehicleOperaterReport::CAirsideVehicleOperaterReport(CBGetLogFilePath pFunc,CAirsideVehicleOperParameter* _param):CAirsideBaseReport(pFunc),m_pParam(_param)
{
	m_IntervalSize = 0 ;
	m_BaseResult = NULL ;
}

CAirsideVehicleOperaterReport::~CAirsideVehicleOperaterReport(void)
{

	for (int i = 0 ; i < (int)m_SummaryResult.size() ;i++)
	{
		delete m_SummaryResult[i] ;
	}
	m_SummaryResult.clear() ;
}
void CAirsideVehicleOperaterReport::RefreshReport(CParameters * parameter)
{
	CAirsideVehicleOperParameter* Paramter = (CAirsideVehicleOperParameter*) parameter;
  if (m_BaseResult != NULL)
	    delete m_BaseResult ;
  m_BaseResult = NULL ;
  m_pParam = Paramter ;
   switch(Paramter->GetSubType())
   {
   case DETAIL_ONROAD_TIME:
	   m_BaseResult = new CAirsideVehicleOpeDetailReportResForOnRoadTime(&m_DetailResult,Paramter) ;
   	break;
   case DETAIL_IDEL_TIME:
	   m_BaseResult = new CAirsideVehicleOpeDetailReportResForIdleTime(&m_DetailResult,Paramter) ;
   	break;
   case DETAIL_TOTALTIME_INSERVER:
	   m_BaseResult = new CAirsideVehicleOpeDetailReportResForServerTime(&m_DetailResult,Paramter) ;
       break;
   case DETAIL_NUMBER_SERVER:
	    m_BaseResult = new CAirsideVehicleOpeDetailReportResForServerNumber(&m_DetailResult,Paramter) ;
       break;
   case DETAIL_NUMBER_TOPOOL:
	   m_BaseResult = new CAirsideVehicleOpeDetailReportResForPoolNumber(&m_DetailResult,Paramter) ;
	   break;
   case DETAIL_NUMBER_FORGAS:
	   m_BaseResult = new CAirsideVehicleOpeDetailReportResForGas(&m_DetailResult,Paramter);
	   break;
   case DETAIL_TIME_INLOTS:
	   m_BaseResult = new CAirsideVehicleOpeDetailReportResForIdleTime(&m_DetailResult,Paramter);
	   break;
   case DETAIL_FUEL_CONSUMED:
	   m_BaseResult = new CAirsideVehicleOpeDetailReportResForFuelConsumed(&m_DetailResult,Paramter);
	   break;
   case DETAIL_AVG_SPEED:
	   m_BaseResult = new CAirsideVehicleOpeDetailReportResForAvgSpeed(&m_DetailResult,Paramter);
	   break;
   case DETAIL_MAX_SPEED:
	   m_BaseResult = new CAirsideVehicleOpeDetailReportResForMaxSpeed(&m_DetailResult,Paramter);
	   break;
   case DETAIL_VEHICLE_SERVICE:
	   m_BaseResult = new CAirsideVehicleOpeDetailReportResForServerPerForm(&m_DetailResult,Paramter);
	   break;
   case SUMMARY_ON_ROAD:
	   m_BaseResult = new CAirsideVehicleOpeSummaryReportResForOnRoadTime(&m_FinalResultSummary,Paramter);
	   break;
   case SUMMARY_SPEED:
	   m_BaseResult = new CAirsideVehicleOpeSummaryReportResForSpeed(&m_FinalResultSummary,Paramter);
	   break;
   case SUMMARY_IN_SERVER:
	   m_BaseResult = new CAirsideVehicleOpeSummaryReportResForOnServerTime(&m_FinalResultSummary,Paramter);
	   break;
   case SUMMARY_FUEL:
	   m_BaseResult = new CAirsideVehicleOpeSummaryReportResForFuelConsumed(&m_FinalResultSummary,Paramter);
	   break;
   case SUMMARY_DISTANCE:
	   m_BaseResult = new CAirsideVehicleOpeSummaryReportResForDis(&m_FinalResultSummary,Paramter);
	   break;
   case SUMMARY_UTILIZATION:
	   m_BaseResult = new CAirsideVehicleOpeSummaryReportResForUtilization(&m_FinalResultSummary,Paramter) ;
	   break;
   default:
       break;
   }
   if(m_BaseResult)
		m_BaseResult->GenerateResult() ;
}
void CAirsideVehicleOperaterReport::GenerateReport(CParameters * parameter)
{

    m_DetailResult.clear() ;
	for (int i = 0 ; i < (int)m_SummaryResult.size() ;i++)
	{
		delete m_SummaryResult[i] ;
	}
	
	m_SummaryResult.clear() ;
	m_FinalResultSummary.clear() ;
	CAirsideVehicleOperParameter *pParam = (CAirsideVehicleOperParameter *)parameter;
	m_pParam = pParam ;
	ASSERT(pParam != NULL);


	long lStartTime = (long)pParam->getStartTime();
	long lEndTime = (long)pParam->getEndTime();

	//read log
	CString strDescFilepath = (*m_pCBGetLogFilePath)(AirsideVehicleDescFile);
	CString strEventFilePath = (*m_pCBGetLogFilePath)(AirsideVehicleEventFile);

	CString FlightstrDescFilepath = (*m_pCBGetLogFilePath)(AirsideDescFile);
	if (FlightstrDescFilepath.IsEmpty() )
		return;
	m_fltLog.LoadDataIfNecessary(FlightstrDescFilepath);


	AirsideVehicleLog VehicleLog;
	VehicleLog.SetEventLog(&m_AirsideVehicleEventLog); // set evet log .
	VehicleLog.LoadDataIfNecessary(strDescFilepath); // read AirsideVehicleDescStructs
	VehicleLog.LoadEventDataIfNecessary(strEventFilePath); //read AirsideVehicleEventStructs


	for (int i = 0 ; i < VehicleLog.getCount() ; i++)
	{
		VehicleLog.getItem(logEntry,i) ;
		logEntry.SetEventLog(&m_AirsideVehicleEventLog);
		AirsideVehicleDescStruct descStruct = logEntry.GetAirsideDesc() ;
		//filter by vehicle type 
		if(pParam->CheckVehicleType(CString(descStruct.vehicleType))&&pParam->CheckPoolId(descStruct.poolNO))
		{
			pParam->AddVehicleTypeForShow(descStruct.m_VecileTypeID) ;
			if(pParam->getReportType() == ASReportType_Detail)
				AnalysisDetailReportItem(descStruct,pParam) ;
			else
			{
				CalSummaryInterval(pParam) ;
				AnalysisSummaryReportItem(descStruct,pParam) ;
			}
		}
	}
	if(pParam->getReportType() == ASReportType_Summary)
	{
		if(m_SummaryResult.empty())
			return ;
		std::map<int , std::vector<CVehicleOperaterSummaryReportItem*> >  m_MapData ;

		for ( int i = 0 ; i < (int)m_SummaryResult.size() ; i++)
		{
			m_MapData[m_SummaryResult[i]->m_vehicleType].push_back(m_SummaryResult[i]) ;
		}

		std::map<int , std::vector<CVehicleOperaterSummaryReportItem*> >::iterator iter = m_MapData.begin() ;
		for ( ; iter != m_MapData.end() ;iter++)
		{
			AnalysisReslut((*iter).first , (*iter).second ) ;
		}
	}
	if(pParam->getReportType() == ASReportType_Detail)
        pParam->setSubType(DETAIL_ONROAD_TIME);
	else
		pParam->setSubType(SUMMARY_ON_ROAD);

	SaveReportData() ;
}
BOOL CAirsideVehicleOperaterReport::ExportListData(ArctermFile& _file,CParameters * parameter)
{
	if(parameter->getReportType() == ASReportType_Detail)
	{
		ExportListDetailData(_file,parameter) ;
	}else
		ExportListSummaryData(_file,parameter) ;
  return TRUE ;
}
void CAirsideVehicleOperaterReport::ExportListSummaryData(ArctermFile& _file,CParameters * parameter)
{
	for (int i = 0 ; i < (int)m_FinalResultSummary.size() ;i++)
	{
		CSummaryFinialResult finalResult ;
		finalResult = m_FinalResultSummary[i] ;
		CString NodeName ;
		NodeName.Format(_T("%s-%s"),finalResult.m_StartTime.PrintDateTime(),finalResult.m_EndTime.PrintDateTime()) ;
		_file.writeField(NodeName);

		NodeName.Format(_T("%s"),GetVehicleTypeNameById(finalResult.m_VehicleType)) ;
		_file.writeField(NodeName) ;
		ElapsedTime time ;
		time.setPrecisely(finalResult.m_MinTimeInRoad) ;

		NodeName.Format(_T("%s"),time.printTime()) ;
		_file.writeField(NodeName) ;

		time.setPrecisely(finalResult.m_AvgTimeInRoad) ;
		NodeName.Format(_T("%s"),time.printTime()) ;
		_file.writeField(NodeName) ;

		time.setPrecisely(finalResult.m_MaxTimeInRoad) ;
		NodeName.Format(_T("%s"),time.printTime()) ;
		_file.writeField(NodeName) ;
		//Q1
		time.setPrecisely(finalResult.m_TimeInRoadQ1) ;
		NodeName.Format(_T("%s"),time.printTime()) ;
		_file.writeField(NodeName) ;
		//Q2
		time.setPrecisely(finalResult.m_TimeInRoadQ2) ;
		NodeName.Format(_T("%s"),time.printTime());
		_file.writeField(NodeName) ;
		//Q3;
		time.setPrecisely(finalResult.m_TimeInRoadQ3) ;
		NodeName.Format(_T("%s"),time.printTime());
		_file.writeField(NodeName) ;
		//p1
		time.setPrecisely(finalResult.m_TimeInRoadP1) ;
		NodeName.Format(_T("%s"),time.printTime());
		_file.writeField(NodeName) ;
		//p5
		time.setPrecisely(finalResult.m_TimeInRoadP5) ;
		NodeName.Format(_T("%s"),time.printTime());
		_file.writeField(NodeName) ;
		//p90
		time.setPrecisely(finalResult.m_TimeInRoadP90) ;
		NodeName.Format(_T("%s"),time.printTime());
		_file.writeField(NodeName) ;
		//p95
		time.setPrecisely(finalResult.m_TimeInRoadP95) ;
		NodeName.Format(_T("%s"),time.printTime());
		_file.writeField(NodeName) ;
		//p99
		time.setPrecisely(finalResult.m_TimeInRoadP99) ;
		NodeName.Format(_T("%s"),time.printTime());
		_file.writeField(NodeName) ;


		//////////////////////////////////////////////////////////////////////////
		time.setPrecisely(finalResult.m_MinTimeInServer) ;
		NodeName.Format(_T("%s"),time.printTime()) ;
		_file.writeField(NodeName) ;

		time.setPrecisely(finalResult.m_AvgTimeInServer) ;
		NodeName.Format(_T("%s"),time.printTime()) ;
		_file.writeField(NodeName) ;

		time.setPrecisely(finalResult.m_MaxTimeInServer) ;
		NodeName.Format(_T("%s"),time.printTime()) ;
		_file.writeField(NodeName) ;
		//Q1
		time.setPrecisely(finalResult.m_TimeInServerQ1) ;
		NodeName.Format(_T("%s"),time.printTime());
		_file.writeField(NodeName) ;
		//Q2
		time.setPrecisely(finalResult.m_TimeInServerQ2) ;
		NodeName.Format(_T("%s"),time.printTime());
		_file.writeField(NodeName) ;
		//Q3
		time.setPrecisely(finalResult.m_TimeInServerQ3) ;
		NodeName.Format(_T("%s"),time.printTime());
		_file.writeField(NodeName) ;
		//p1
		time.setPrecisely(finalResult.m_TimeInServerP1) ;
		NodeName.Format(_T("%s"),time.printTime());
		_file.writeField(NodeName) ;
		//p5
		time.setPrecisely(finalResult.m_TimeInServerP5) ;
		NodeName.Format(_T("%s"),time.printTime());
		_file.writeField(NodeName) ;
		//p90
		time.setPrecisely(finalResult.m_TimeInServerP90) ;
		NodeName.Format(_T("%s"),time.printTime());
		_file.writeField(NodeName) ;
		//p95
		time.setPrecisely(finalResult.m_TimeInServerP95) ;
		NodeName.Format(_T("%s"),time.printTime());
		_file.writeField(NodeName) ;
		//p99
		time.setPrecisely(finalResult.m_TimeInServerP99) ;
		NodeName.Format(_T("%s"),time.printTime());
		_file.writeField(NodeName) ;

		//////////////////////////////////////////////////////////////////////////

		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_MinDis) ) ;
		_file.writeField(NodeName) ;
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_AvgDis )) ;
		_file.writeField(NodeName) ;
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_MaxDis)) ;
		_file.writeField(NodeName) ;
		//Q1
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_DisQ1));
		_file.writeField(NodeName) ;
		//Q2
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_DisQ2));
		_file.writeField(NodeName) ;
		//Q3
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_DisQ3));
		_file.writeField(NodeName) ;
		//p1
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_DisP1));
		_file.writeField(NodeName) ;
		//p5
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_Disp5));
		_file.writeField(NodeName) ;
		//p90
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_DisP90));
		_file.writeField(NodeName) ;
		//p95
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_DisP95));
		_file.writeField(NodeName) ;
		//p99
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_DisP99));
		_file.writeField(NodeName) ;

		//////////////////////////////////////////////////////////////////////////


		NodeName.Format(_T("%0.2f"),finalResult.m_MinFuel) ;
		_file.writeField(NodeName) ;
		NodeName.Format(_T("%0.2f"),finalResult.m_AvgFuel) ;
		_file.writeField(NodeName) ;
		NodeName.Format(_T("%0.2f"),finalResult.m_MaxFuel) ;
		_file.writeField(NodeName) ;
		//Q1
		NodeName.Format(_T("%0.2f"),finalResult.m_FuelQ1);
		_file.writeField(NodeName) ;
		//Q2
		NodeName.Format(_T("%0.2f"),finalResult.m_FuelQ2);
		_file.writeField(NodeName) ;
		//Q3
		NodeName.Format(_T("%0.2f"),finalResult.m_FuelQ3);
		_file.writeField(NodeName) ;
		//p1
		NodeName.Format(_T("%0.2f"),finalResult.m_FuelP1);
		_file.writeField(NodeName) ;
		//p5
		NodeName.Format(_T("%0.2f"),finalResult.m_FuelP5);
		_file.writeField(NodeName) ;
		//p90
		NodeName.Format(_T("%0.2f"),finalResult.m_FuelP90);
		_file.writeField(NodeName) ;
		//p95
		NodeName.Format(_T("%0.2f"),finalResult.m_FuelP95);
		_file.writeField(NodeName) ;
		//p99
		NodeName.Format(_T("%0.2f"),finalResult.m_FuelP99);
		_file.writeField(NodeName) ;
		//////////////////////////////////////////////////////////////////////////


		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_MinSpeed*36/1000) );
		_file.writeField(NodeName) ;
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_AvgSpeed*36/1000)) ;
		_file.writeField(NodeName) ;
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_MaxSpeed*36/1000)) ;
		_file.writeField(NodeName) ;
		//Q1
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_SpeedQ1*36/1000));
		_file.writeField(NodeName) ;
		//Q2
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_SpeedQ2*36/1000));
		_file.writeField(NodeName) ;
		//Q3
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_SpeedQ3*36/1000));
		_file.writeField(NodeName) ;
		//p1
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_SpeedP1*36/1000));
		_file.writeField(NodeName) ;
		//p5
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_SpeedP5*36/1000));
		_file.writeField(NodeName) ;
		//p90
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_SpeedP90*36/1000));
		_file.writeField(NodeName) ;
		//p95
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_SpeedP95*36/1000));
		_file.writeField(NodeName) ;
		//p99
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_SpeedP99*36/1000));
		_file.writeField(NodeName) ;
		_file.writeLine() ;
	}
}
void CAirsideVehicleOperaterReport::ExportListDetailData(ArctermFile& _file,CParameters * parameter)
{
	if(m_DetailResult.empty())
		return  ;
	CVehicleOperaterDetailReportItem reportItem  ;
	CString ItemName ;
	for (int i = 0 ; i < (int)m_DetailResult.size() ;i++)
	{    
		ElapsedTime time ;
		reportItem = m_DetailResult[i] ;
		ItemName.Format(_T("%d"),i+1) ;
		_file.writeField(ItemName) ;
		_file.writeField(reportItem.m_VehicleType);
		_file.writeField(reportItem.m_StartServerTime.PrintDateTime()) ;
		_file.writeField(reportItem.m_EndServerTime.PrintDateTime()) ;
		time.setPrecisely(reportItem.m_TotalOnRouteTime) ;
		_file.writeField(time.printTime()) ;

		time.setPrecisely(reportItem.m_TotalIdelTime) ;
		_file.writeField(time.printTime()) ;

		time.setPrecisely(reportItem.m_ServerTime) ;
		_file.writeField(time.printTime()) ;

		ItemName.Format(_T("%d"),reportItem.m_ServerNumber) ;
		_file.writeField(ItemName) ;
		ItemName.Format(_T("%d"),reportItem.m_NumberToPool) ;
		_file.writeField(ItemName) ;
		_file.writeField(CString(_T("0"))) ;
		time.setPrecisely(reportItem.m_TotalIdelTime) ;
		_file.writeField(time.printTime()) ;

		ItemName.Format(_T("%d"),reportItem.m_FuelConsumed) ;
		_file.writeField(ItemName) ;

		ItemName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),reportItem.m_AvgSpeed*36/1000)) ;
		_file.writeField(ItemName) ;

		ItemName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),reportItem.m_MaxSpeed*36/1000));
		_file.writeField(ItemName) ;
		double disance = CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),reportItem.m_TotalDistance) ;
		ItemName.Format(_T("%0.2f"),disance) ;
		_file.writeField(ItemName) ;
		_file.writeLine() ;
	}
}
//////////////////////////////////////////////////////////////////////////
//detail report generate function 
void CAirsideVehicleOperaterReport::GenerateDetailReport(CAirsideVehicleOperParameter *pParam )
{
	
}
void CAirsideVehicleOperaterReport::AnalysisDetailReportItem(AirsideVehicleDescStruct& _VehicleDesStruct,CAirsideVehicleOperParameter *pParam )
{
	CVehicleOperaterDetailReportItem vehicleOperateDetailItem ;
	vehicleOperateDetailItem.m_vehicleID = _VehicleDesStruct.IndexID ;
	vehicleOperateDetailItem.m_VehicleType = _VehicleDesStruct.vehicleType ;
	CDetailAnalysis* detailAnsly = NULL ;
	detailAnsly = new CDetailAnalysis(&vehicleOperateDetailItem,&logEntry,pParam) ;
	detailAnsly->SetFlightLog(&m_fltLog) ;
	detailAnsly->AnalysisVehicleForDetail() ;
	delete detailAnsly ;
	m_DetailResult.push_back(vehicleOperateDetailItem) ;
}
//////////////////////////////////////////////////////////////////////////
void CAirsideVehicleOperaterReport::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC)
{
	if(reportType == ASReportType_Detail)
		InitDetailListHead(cxListCtrl,piSHC) ;
	else
		InitSummaryListHead(cxListCtrl,piSHC) ;
}
void CAirsideVehicleOperaterReport::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{

	if(parameter->getReportType() == ASReportType_Detail)
		InitDetaliList(cxListCtrl) ;
	else
		InitSummaryList(cxListCtrl) ;
}
void CAirsideVehicleOperaterReport::InitDetailListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("Vehicle ID"), LVCFMT_LEFT, 100, -1);

	cxListCtrl.InsertColumn(1, _T("Vehicle Type"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(2, _T("Time Start First Service"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(3, _T("Time End last Service"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(4, _T("Total on Road Time"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(5, _T("Total idle time"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(6,_T("Total Service Time"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(7,_T("Number of Services"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(8,_T("Number of Returns to Pool"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(9,_T("Number of Stops for Gas"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(10,_T("Time spent in parking lots"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(11,_T("Fuel consumed(liter)"), LVCFMT_LEFT, 100, -1) ;
	CString StrSpeed ;
	StrSpeed.Format(_T("Average speed(%s)"),CARCVelocityUnit::GetVelocityUnitString(CARCUnitManager::GetInstance().GetCurSelVelocityUnit()));
	cxListCtrl.InsertColumn(12,StrSpeed, LVCFMT_LEFT, 100, -1) ;

	StrSpeed.Format(_T("Max speed(%s)"),CARCVelocityUnit::GetVelocityUnitString(CARCUnitManager::GetInstance().GetCurSelVelocityUnit()));
	cxListCtrl.InsertColumn(13,StrSpeed, LVCFMT_LEFT, 100, -1) ;

	CString unit ;
	unit.Format(_T("(%s)"),CARCLengthUnit::GetLengthUnitString(CARCUnitManager::GetInstance().GetCurSelLengthUnit()));

	cxListCtrl.InsertColumn(14,_T("Total distance" + unit ), LVCFMT_LEFT, 100, -1) ;

	if (piSHC)
	{
		piSHC->ResetAll();
		piSHC->SetDataType(0,dtINT);
		piSHC->SetDataType(1,dtSTRING);
		piSHC->SetDataType(2,dtSTRING);
		piSHC->SetDataType(3,dtSTRING);
		piSHC->SetDataType(4,dtTIME);
		piSHC->SetDataType(5,dtTIME);
		piSHC->SetDataType(6,dtTIME);
		piSHC->SetDataType(7,dtINT);
		piSHC->SetDataType(8,dtINT);
		piSHC->SetDataType(9,dtINT);
		piSHC->SetDataType(10,dtTIME);
		piSHC->SetDataType(11,dtINT);
		piSHC->SetDataType(12,dtDEC);
		piSHC->SetDataType(13,dtDEC);
		piSHC->SetDataType(14,dtDEC);
	}

}
void CAirsideVehicleOperaterReport::InitSummaryListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("Interval"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(1, _T("Vehicle Type"), LVCFMT_LEFT, 100, -1);

	cxListCtrl.InsertColumn(2, _T("Min time on road"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(3, _T("Average time on road"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(4, _T("Max time on road"), LVCFMT_LEFT, 100, -1);
    cxListCtrl.InsertColumn(5,_T("Time on road Q1"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(6,_T("Time on road Q2"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(7,_T("Time on road Q3"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(8,_T("Time on road P1"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(9,_T("Time on road P5"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(10,_T("Time on road P90"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(11,_T("Time on road P95"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(12,_T("Time on road P99"),LVCFMT_LEFT, 100, -1);

	cxListCtrl.InsertColumn(13,_T("Min time in service"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(14,_T("Average time in service"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(15,_T("Max time in service"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(16,_T("Time in service Q1"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(17,_T("Time in service Q2"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(18,_T("Time in service Q3"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(19,_T("Time in service P1"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(20,_T("Time in service P5"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(21,_T("Time in service P90"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(22,_T("Time in service P95"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(23,_T("Time in service P99"),LVCFMT_LEFT, 100, -1);

	CString UnitName ;
	UnitName.Format(_T("(%s)"),CARCLengthUnit::GetLengthUnitString(CARCUnitManager::GetInstance().GetCurSelLengthUnit()));

	cxListCtrl.InsertColumn(24,_T("Min distance"+UnitName), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(25,_T("Average distance"+UnitName), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(26,_T("Max distance"+UnitName), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(27,_T("Distance Q1"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(28,_T("Distance Q2"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(29,_T("Distance Q3"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(30,_T("Distance P1"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(31,_T("Distance P5"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(32,_T("Distance P90"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(33,_T("Distance P95"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(34,_T("Distance P99"),LVCFMT_LEFT, 100, -1);

	cxListCtrl.InsertColumn(35,_T("Min fuel(liter)"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(36,_T("Average fuel(liter)"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(37,_T("Max fuel(liter)"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(38,_T("Fuel Q1"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(39,_T("Fuel Q2"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(40,_T("Fuel Q3"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(41,_T("Fuel P1"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(42,_T("Fuel P5"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(43,_T("Fuel P90"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(44,_T("Fuel P95"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(45,_T("Fuel P99"),LVCFMT_LEFT, 100, -1);

	CString StrSpeed ;
	StrSpeed.Format(_T("(%s)"),CARCVelocityUnit::GetVelocityUnitString(CARCUnitManager::GetInstance().GetCurSelVelocityUnit() ));

	cxListCtrl.InsertColumn(46,_T("Min speed+StrSpeed"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(47,_T("Average speed + StrSpeed"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(48,_T("Max speed + StrSpeed"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(49,_T("Speed Q1"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(50,_T("Speed Q2"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(51,_T("Speed Q3"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(52,_T("Speed P1"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(53,_T("Speed P5"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(54,_T("Speed P90"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(55,_T("Speed P95"),LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(56,_T("Speed P99"),LVCFMT_LEFT, 100, -1);

	if (piSHC)
	{
		piSHC->ResetAll();
		piSHC->SetDataType(0,dtSTRING);
		piSHC->SetDataType(1,dtSTRING);
		piSHC->SetDataType(2,dtTIME);
		piSHC->SetDataType(3,dtTIME);
		piSHC->SetDataType(4,dtTIME);
		piSHC->SetDataType(5,dtTIME);
		piSHC->SetDataType(6,dtTIME);
		piSHC->SetDataType(7,dtTIME);
		piSHC->SetDataType(8,dtTIME);
		piSHC->SetDataType(9,dtTIME);
		piSHC->SetDataType(10,dtTIME);
		piSHC->SetDataType(11,dtTIME);
		piSHC->SetDataType(12,dtTIME);
		piSHC->SetDataType(13,dtTIME);
		piSHC->SetDataType(14,dtTIME);
		piSHC->SetDataType(15,dtTIME);
		piSHC->SetDataType(16,dtTIME);
		piSHC->SetDataType(17,dtTIME);
		piSHC->SetDataType(18,dtTIME);
		piSHC->SetDataType(19,dtTIME);
		piSHC->SetDataType(20,dtTIME);
		piSHC->SetDataType(21,dtTIME);
		piSHC->SetDataType(22,dtTIME);
		piSHC->SetDataType(23,dtTIME);
		piSHC->SetDataType(24,dtDEC);
		piSHC->SetDataType(25,dtDEC);
		piSHC->SetDataType(26,dtDEC);
		piSHC->SetDataType(27,dtDEC);
		piSHC->SetDataType(28,dtDEC);
		piSHC->SetDataType(29,dtDEC);
		piSHC->SetDataType(30,dtDEC);
		piSHC->SetDataType(31,dtDEC);
		piSHC->SetDataType(32,dtDEC);
		piSHC->SetDataType(33,dtDEC);
		piSHC->SetDataType(34,dtDEC);
		piSHC->SetDataType(35,dtDEC);
		piSHC->SetDataType(36,dtDEC);
		piSHC->SetDataType(37,dtDEC);
		piSHC->SetDataType(38,dtDEC);
		piSHC->SetDataType(39,dtDEC);
		piSHC->SetDataType(40,dtDEC);
		piSHC->SetDataType(41,dtDEC);
		piSHC->SetDataType(42,dtDEC);
		piSHC->SetDataType(43,dtDEC);
		piSHC->SetDataType(44,dtDEC);
		piSHC->SetDataType(45,dtDEC);
		piSHC->SetDataType(46,dtDEC);
		piSHC->SetDataType(47,dtDEC);
		piSHC->SetDataType(48,dtDEC);
		piSHC->SetDataType(49,dtDEC);
		piSHC->SetDataType(50,dtDEC);
		piSHC->SetDataType(51,dtDEC);
		piSHC->SetDataType(52,dtDEC);
		piSHC->SetDataType(53,dtDEC);
		piSHC->SetDataType(54,dtDEC);
		piSHC->SetDataType(55,dtDEC);
		piSHC->SetDataType(56,dtDEC);
	}
}
void CAirsideVehicleOperaterReport::InitDetaliList(CXListCtrl& cxListCtrl)
{
	cxListCtrl.DeleteAllItems() ;
	CVehicleOperaterDetailReportItem reportItem  ;
	CString ItemName ;
	for (int i = 0 ; i < (int)m_DetailResult.size() ;i++)
	{    
		ElapsedTime time ;
		reportItem = m_DetailResult[i] ;
        ItemName.Format(_T("%d"),i+1) ;
		cxListCtrl.InsertItem(i,ItemName) ;
		cxListCtrl.SetItemText(i,1,reportItem.m_VehicleType);
		cxListCtrl.SetItemText(i,2,reportItem.m_StartServerTime.PrintDateTime()) ;
        cxListCtrl.SetItemText(i,3,reportItem.m_EndServerTime.PrintDateTime()) ;
		time.setPrecisely(reportItem.m_TotalOnRouteTime) ;
		cxListCtrl.SetItemText(i,4,time.printTime()) ;

		time.setPrecisely(reportItem.m_TotalIdelTime) ;
		cxListCtrl.SetItemText(i,5,time.printTime()) ;

		time.setPrecisely(reportItem.m_ServerTime) ;
		cxListCtrl.SetItemText(i,6,time.printTime()) ;

		ItemName.Format(_T("%d"),reportItem.m_ServerNumber) ;
		cxListCtrl.SetItemText(i,7,ItemName) ;
		ItemName.Format(_T("%d"),reportItem.m_NumberToPool) ;
		cxListCtrl.SetItemText(i,8,ItemName) ;
		cxListCtrl.SetItemText(i,9,CString(_T("0"))) ;
		time.setPrecisely(reportItem.m_TotalIdelTime) ;
		cxListCtrl.SetItemText(i,10,time.printTime()) ;

		ItemName.Format(_T("%d"),reportItem.m_FuelConsumed) ;
		cxListCtrl.SetItemText(i,11,ItemName) ;

		ItemName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_MPS,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),reportItem.m_AvgSpeed / 100)) ;
		cxListCtrl.SetItemText(i,12,ItemName) ;

		ItemName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_MPS,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),reportItem.m_MaxSpeed / 100)) ;
		cxListCtrl.SetItemText(i,13,ItemName) ;
		double disance =CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),reportItem.m_TotalDistance) ;
		ItemName.Format(_T("%0.2f"),disance) ;
		cxListCtrl.SetItemText(i,14,ItemName) ;
	}
}
void CAirsideVehicleOperaterReport::InitSummaryList(CXListCtrl& cxListCtrl)
{

	for (int i = 0 ; i < (int)m_FinalResultSummary.size() ;i++)
	{
		CSummaryFinialResult finalResult ;
		finalResult = m_FinalResultSummary[i] ;
        CString NodeName ;
		NodeName.Format(_T("%s-%s"),finalResult.m_StartTime.PrintDateTime(),finalResult.m_EndTime.PrintDateTime()) ;
		cxListCtrl.InsertItem(i,NodeName);

		NodeName.Format(_T("%s"),GetVehicleTypeNameById(finalResult.m_VehicleType)) ;
		cxListCtrl.SetItemText(i,1,NodeName) ;
        ElapsedTime time ;
		time.setPrecisely(finalResult.m_MinTimeInRoad) ;

		NodeName.Format(_T("%s"),time.printTime()) ;
		cxListCtrl.SetItemText(i,2,NodeName) ;

		time.setPrecisely(finalResult.m_AvgTimeInRoad) ;
		NodeName.Format(_T("%s"),time.printTime()) ;
		cxListCtrl.SetItemText(i,3,NodeName) ;

		time.setPrecisely(finalResult.m_MaxTimeInRoad) ;
		NodeName.Format(_T("%s"),time.printTime()) ;
		cxListCtrl.SetItemText(i,4,NodeName) ;
		//Q1
		time.setPrecisely(finalResult.m_TimeInRoadQ1) ;
		NodeName.Format(_T("%s"),time.printTime()) ;
		cxListCtrl.SetItemText(i,5,NodeName) ;
		//Q2
		time.setPrecisely(finalResult.m_TimeInRoadQ2) ;
		NodeName.Format(_T("%s"),time.printTime());
		cxListCtrl.SetItemText(i,6,NodeName) ;
		//Q3;
		time.setPrecisely(finalResult.m_TimeInRoadQ3) ;
		NodeName.Format(_T("%s"),time.printTime());
		cxListCtrl.SetItemText(i,7,NodeName) ;
		//p1
		time.setPrecisely(finalResult.m_TimeInRoadP1) ;
		NodeName.Format(_T("%s"),time.printTime());
		cxListCtrl.SetItemText(i,8,NodeName) ;
		//p5
		time.setPrecisely(finalResult.m_TimeInRoadP5) ;
		NodeName.Format(_T("%s"),time.printTime());
		cxListCtrl.SetItemText(i,9,NodeName) ;
		//p90
		time.setPrecisely(finalResult.m_TimeInRoadP90) ;
		NodeName.Format(_T("%s"),time.printTime());
		cxListCtrl.SetItemText(i,10,NodeName) ;
		//p95
		time.setPrecisely(finalResult.m_TimeInRoadP95) ;
		NodeName.Format(_T("%s"),time.printTime());
		cxListCtrl.SetItemText(i,11,NodeName) ;
		//p99
		time.setPrecisely(finalResult.m_TimeInRoadP99) ;
		NodeName.Format(_T("%s"),time.printTime());
		cxListCtrl.SetItemText(i,12,NodeName) ;


    //////////////////////////////////////////////////////////////////////////
		time.setPrecisely(finalResult.m_MinTimeInServer) ;
		NodeName.Format(_T("%s"),time.printTime()) ;
		cxListCtrl.SetItemText(i,13,NodeName) ;

		time.setPrecisely(finalResult.m_AvgTimeInServer) ;
		NodeName.Format(_T("%s"),time.printTime()) ;
		cxListCtrl.SetItemText(i,14,NodeName) ;

		time.setPrecisely(finalResult.m_MaxTimeInServer) ;
		NodeName.Format(_T("%s"),time.printTime()) ;
		cxListCtrl.SetItemText(i,15,NodeName) ;
		//Q1
		time.setPrecisely(finalResult.m_TimeInServerQ1) ;
		NodeName.Format(_T("%s"),time.printTime());
		cxListCtrl.SetItemText(i,16,NodeName) ;
		//Q2
		time.setPrecisely(finalResult.m_TimeInServerQ2) ;
		NodeName.Format(_T("%s"),time.printTime());
			cxListCtrl.SetItemText(i,17,NodeName) ;
		//Q3
		time.setPrecisely(finalResult.m_TimeInServerQ3) ;
		NodeName.Format(_T("%s"),time.printTime());
			cxListCtrl.SetItemText(i,18,NodeName) ;
		//p1
		time.setPrecisely(finalResult.m_TimeInServerP1) ;
		NodeName.Format(_T("%s"),time.printTime());
			cxListCtrl.SetItemText(i,19,NodeName) ;
		//p5
		time.setPrecisely(finalResult.m_TimeInServerP5) ;
		NodeName.Format(_T("%s"),time.printTime());
			cxListCtrl.SetItemText(i,20,NodeName) ;
		//p90
		time.setPrecisely(finalResult.m_TimeInServerP90) ;
		NodeName.Format(_T("%s"),time.printTime());
			cxListCtrl.SetItemText(i,21,NodeName) ;
		//p95
			time.setPrecisely(finalResult.m_TimeInServerP95) ;
		NodeName.Format(_T("%s"),time.printTime());
			cxListCtrl.SetItemText(i,22,NodeName) ;
		//p99
			time.setPrecisely(finalResult.m_TimeInServerP99) ;
		NodeName.Format(_T("%s"),time.printTime());
			cxListCtrl.SetItemText(i,23,NodeName) ;

//////////////////////////////////////////////////////////////////////////
     
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_MinDis )) ;
		cxListCtrl.SetItemText(i,24,NodeName) ;
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_AvgDis )) ;
		cxListCtrl.SetItemText(i,25,NodeName) ;
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_MaxDis)) ;
		cxListCtrl.SetItemText(i,26,NodeName) ;
		//Q1
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_DisQ1));
			cxListCtrl.SetItemText(i,27,NodeName) ;
		//Q2
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_DisQ2));
			cxListCtrl.SetItemText(i,28,NodeName) ;
		//Q3
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_DisQ3));
			cxListCtrl.SetItemText(i,29,NodeName) ;
		//p1
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_DisP1));
			cxListCtrl.SetItemText(i,30,NodeName) ;
		//p5
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_Disp5));
			cxListCtrl.SetItemText(i,31,NodeName) ;
		//p90
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_DisP90));
			cxListCtrl.SetItemText(i,32,NodeName) ;
		//p95
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_DisP95));
			cxListCtrl.SetItemText(i,33,NodeName) ;
		//p99
		NodeName.Format(_T("%0.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),finalResult.m_DisP99));
			cxListCtrl.SetItemText(i,34,NodeName) ;

//////////////////////////////////////////////////////////////////////////


		NodeName.Format(_T("%0.2f"),finalResult.m_MinFuel) ;
		cxListCtrl.SetItemText(i,35,NodeName) ;
		NodeName.Format(_T("%0.2f"),finalResult.m_AvgFuel) ;
		cxListCtrl.SetItemText(i,36,NodeName) ;
		NodeName.Format(_T("%0.2f"),finalResult.m_MaxFuel) ;
		cxListCtrl.SetItemText(i,37,NodeName) ;
		//Q1
		NodeName.Format(_T("%0.2f"),finalResult.m_FuelQ1);
			cxListCtrl.SetItemText(i,38,NodeName) ;
		//Q2
		NodeName.Format(_T("%0.2f"),finalResult.m_FuelQ2);
			cxListCtrl.SetItemText(i,39,NodeName) ;
		//Q3
		NodeName.Format(_T("%0.2f"),finalResult.m_FuelQ3);
			cxListCtrl.SetItemText(i,40,NodeName) ;
		//p1
		NodeName.Format(_T("%0.2f"),finalResult.m_FuelP1);
			cxListCtrl.SetItemText(i,41,NodeName) ;
		//p5
		NodeName.Format(_T("%0.2f"),finalResult.m_FuelP5);
			cxListCtrl.SetItemText(i,42,NodeName) ;
		//p90
		NodeName.Format(_T("%0.2f"),finalResult.m_FuelP90);
			cxListCtrl.SetItemText(i,43,NodeName) ;
		//p95
		NodeName.Format(_T("%0.2f"),finalResult.m_FuelP95);
			cxListCtrl.SetItemText(i,44,NodeName) ;
		//p99
		NodeName.Format(_T("%0.2f"),finalResult.m_FuelP99);
			cxListCtrl.SetItemText(i,45,NodeName) ;
//////////////////////////////////////////////////////////////////////////


		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_MinSpeed*36/1000)) ;
		cxListCtrl.SetItemText(i,46,NodeName) ;
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_AvgSpeed*36/1000)) ;
		cxListCtrl.SetItemText(i,47,NodeName) ;
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_MaxSpeed*36/1000)) ;
		cxListCtrl.SetItemText(i,48,NodeName) ;
		//Q1
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_SpeedQ1*36/1000));
			cxListCtrl.SetItemText(i,49,NodeName) ;
		//Q2
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_SpeedQ2*36/1000));
			cxListCtrl.SetItemText(i,50,NodeName) ;
		//Q3
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_SpeedQ3*36/1000));
			cxListCtrl.SetItemText(i,51,NodeName) ;
		//p1
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_SpeedP1*36/1000));
			cxListCtrl.SetItemText(i,52,NodeName) ;
		//p5
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_SpeedP5*36/1000));
			cxListCtrl.SetItemText(i,53,NodeName) ;
		//p90
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_SpeedP90*36/1000));
			cxListCtrl.SetItemText(i,54,NodeName) ;
		//p95
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_SpeedP95*36/1000));
			cxListCtrl.SetItemText(i,55,NodeName) ;
		//p99
		NodeName.Format(_T("%0.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),finalResult.m_SpeedP99*36/1000));
			cxListCtrl.SetItemText(i,56,NodeName) ;
	}
}
void CAirsideVehicleOperaterReport::AnalysisReslut(int Type , std::vector<CVehicleOperaterSummaryReportItem*> & _data)
{
	if(_data.empty())
		return ;

	int intervalSize = (int)((_data[0])->m_SubItem).size() ;
	for (int i= 0 ; i < intervalSize ; i++)
	{
		CSummaryFinialResult finalResult ;
		memset(&finalResult,0,sizeof(CSummaryFinialResult)) ;
		finalResult.m_VehicleType = Type ;
		CStatisticalTools<float> SpeedTool;
		CStatisticalTools<long> timeServerTool;
		CStatisticalTools<long> timeRoadTool;
		CStatisticalTools<double> DisTool;
		CStatisticalTools<float> FuelTool;
		float max_Seed = 0; 
		float totalSpeed = 0;
		float minSpeed = 0 ; 

		long maxTimeServer = 0 ;
		long minTimeServer = 0 ;
		long totalTimeServer = 0 ;

		long maxTimeInRoad = 0 ;
		long minTimeInRood = 0 ;
		long totalTimeInRood = 0 ;

		double maxDis = 0 ;
		double minDis = 0 ;
		double totalDis = 0 ;

		float maxFuel = 0 ;
		float minFuel = 0 ;
		float totalFuel = 0 ;
		for (int j = 0 ; j < (int)_data.size() ;j++)
		{
			CVehicleOperaterSummaryReportItem* reportItem = NULL;
			CVehicleOperaterSummaryReportSubItem* subItem = NULL;
			reportItem = _data[j] ;
			subItem = reportItem->m_SubItem[i] ; 
			finalResult.m_StartTime = subItem->m_StartTime ;
			finalResult.m_EndTime = subItem->m_EndTime ;

			//calculate maxspeed 
			max_Seed = MAX(max_Seed,subItem->m_speed) ;
            totalSpeed = totalSpeed + subItem->m_speed ;
			if(minSpeed == 0 )
				minSpeed = subItem->m_speed ;
			minSpeed = MIN(minSpeed,subItem->m_speed) ;
			SpeedTool.AddNewData(subItem->m_speed) ;

			//calculate time server 
			maxTimeServer = MAX(maxTimeServer,subItem->m_serverTime) ;
			totalTimeServer = totalTimeServer + subItem->m_serverTime ;
			if(minTimeServer == 0 )
				minTimeServer = subItem->m_serverTime ;
			minTimeServer = MIN(minTimeServer,subItem->m_serverTime) ;
			timeServerTool.AddNewData(subItem->m_serverTime ) ;

			//calculate time on road
			maxTimeInRoad = MAX(maxTimeInRoad,subItem->m_OnRoadTime) ;
			totalTimeInRood = totalTimeInRood + subItem->m_OnRoadTime ;
			if(minTimeInRood == 0)
				minTimeInRood = subItem->m_OnRoadTime ;
            minTimeInRood = MIN(minTimeInRood,subItem->m_OnRoadTime) ;
			timeRoadTool.AddNewData(subItem->m_OnRoadTime) ;

			//calculate dis 
			maxDis = MAX(maxDis,subItem->m_Distance) ;
			totalDis = totalDis + subItem->m_Distance ;
            if(minDis == 0 )
				minDis = subItem->m_Distance ;
			minDis = MIN(minDis,subItem->m_Distance) ;
			DisTool.AddNewData(subItem->m_Distance) ;

			//calculate fuel 
			maxFuel = MAX(maxFuel,subItem->m_Fuel) ;
			totalFuel = totalFuel + subItem->m_Fuel ;
			if(minFuel == 0)
				minFuel = subItem->m_Fuel ;
			minFuel = MIN(minFuel,subItem->m_Fuel) ;
			FuelTool.AddNewData(subItem->m_Fuel) ;
		}
		long  count =_data.size() ;
        finalResult.m_MinDis = minDis ;
		finalResult.m_MaxDis = maxDis ;
		finalResult.m_AvgDis = totalDis/count ;
		finalResult.m_DisQ1 = DisTool.GetPercentile(25) ;
		finalResult.m_DisQ2 = DisTool.GetPercentile(50) ;
		finalResult.m_DisQ3 = DisTool.GetPercentile(75) ;
		finalResult.m_DisP1 = DisTool.GetPercentile(1) ;
		finalResult.m_Disp5 = DisTool.GetPercentile(5) ;
		finalResult.m_DisP90 = DisTool.GetPercentile(90) ;
		finalResult.m_DisP95 = DisTool.GetPercentile(95) ;
		finalResult.m_DisP99 = DisTool.GetPercentile(99) ;

		finalResult.m_MaxFuel = maxFuel ;
		finalResult.m_MinFuel = minFuel ;
		finalResult.m_AvgFuel = totalFuel/count ;
		finalResult.m_FuelQ1 = FuelTool.GetPercentile(25) ;
		finalResult.m_FuelQ2 = FuelTool.GetPercentile(50) ;
		finalResult.m_FuelQ3 = FuelTool.GetPercentile(75) ;
		finalResult.m_FuelP1 = FuelTool.GetPercentile(1) ;
		finalResult.m_FuelP5 = FuelTool.GetPercentile(5) ;
		finalResult.m_FuelP90 = FuelTool.GetPercentile(90) ;
		finalResult.m_FuelP95 = FuelTool.GetPercentile(95) ;
		finalResult.m_FuelP99 = FuelTool.GetPercentile(99) ;

		finalResult.m_MaxSpeed = max_Seed ;
		finalResult.m_MinSpeed = minSpeed ;
		finalResult.m_AvgSpeed = totalSpeed/count ;
		finalResult.m_SpeedQ1 = SpeedTool.GetPercentile(25) ;
		finalResult.m_SpeedQ2 = SpeedTool.GetPercentile(50) ;
		finalResult.m_SpeedQ3 = SpeedTool.GetPercentile(75) ;
		finalResult.m_SpeedP1 = SpeedTool.GetPercentile(1) ;
		finalResult.m_SpeedP5 = SpeedTool.GetPercentile(5) ;
		finalResult.m_SpeedP90 = SpeedTool.GetPercentile(90) ;
		finalResult.m_SpeedP95 = SpeedTool.GetPercentile(95) ;
		finalResult.m_SpeedP99 = SpeedTool.GetPercentile(99) ;

		finalResult.m_MaxTimeInRoad = maxTimeInRoad ;
		finalResult.m_MinTimeInRoad = minTimeInRood ;
		finalResult.m_AvgTimeInRoad = totalTimeInRood/count ;
		finalResult.m_TotalInRoadTime = totalTimeInRood ;
		finalResult.m_TimeInRoadQ1 = timeRoadTool.GetPercentile(25) ;
		finalResult.m_TimeInRoadQ2 = timeRoadTool.GetPercentile(50) ;
		finalResult.m_TimeInRoadQ3 = timeRoadTool.GetPercentile(75) ;
		finalResult.m_TimeInServerP1 = timeRoadTool.GetPercentile(1) ;
		finalResult.m_TimeInServerP5 = timeRoadTool.GetPercentile(5);
		finalResult.m_TimeInServerP90 = timeRoadTool.GetPercentile(90) ;
		finalResult.m_TimeInServerP95 = timeRoadTool.GetPercentile(95) ;
		finalResult.m_TimeInServerP99 = timeRoadTool.GetPercentile(99) ;
		
		finalResult.m_MaxTimeInServer = maxTimeServer ;
		finalResult.m_MinTimeInServer = minTimeServer ;
		finalResult.m_AvgTimeInServer  = totalTimeServer/count ;
		finalResult.m_totalTimeServer = totalTimeServer ;
        finalResult.m_TimeInServerQ1 = timeServerTool.GetPercentile(25) ;
		finalResult.m_TimeInServerQ2 = timeServerTool.GetPercentile(50) ;
		finalResult.m_TimeInServerQ3 = timeServerTool.GetPercentile(75) ;
		finalResult.m_TimeInServerP1 = timeServerTool.GetPercentile(1) ;
		finalResult.m_TimeInServerP5 = timeServerTool.GetPercentile(5) ;
		finalResult.m_TimeInServerP90 = timeServerTool.GetPercentile(90) ;
		finalResult.m_TimeInServerP95 = timeServerTool.GetPercentile(95) ;
		finalResult.m_TimeInServerP99 = timeServerTool.GetPercentile(99) ;
		m_FinalResultSummary.push_back(finalResult) ;
	}
}
//////////////////////////////////////////////////////////////////////////
//the interface foe analysis detail item form event log 
CVehicleEventLogAnalysis::CVehicleEventLogAnalysis( AirsideVehicleLogEntry* _vehicleEntry ,CAirsideVehicleOperParameter *pParam)
:m_vehicleEntry(_vehicleEntry),m_pParam(pParam),m_fltLog(NULL)
{
	m_descStruct = &(m_vehicleEntry->GetAirsideDesc()) ;
}
void CVehicleEventLogAnalysis::AnalysisEventLog()
{
	if(m_descStruct->m_BaseType == VehicleType_PaxTruck)
		AnalysisPaxVehicle();
	else if(m_descStruct->m_BaseType == VehicleType_FollowMeCar)
		AnalysisFollowmeCar();
	else
		AnalysisGeneralVehicle();
}
BOOL CVehicleEventLogAnalysis::GetFlightDesStructByID(AirsideFlightDescStruct& destStruct , int _ID)
{
	AirsideFlightLogEntry logentry ;
	for (int i = 0 ; i < m_fltLog->getCount() ;i++)
	{
		m_fltLog->getItem(logentry,_ID) ;
		if(logentry.GetAirsideDesc().id == _ID)
		{
			destStruct = logentry.GetAirsideDesc() ;
			return TRUE ;
		}
	}
	return FALSE ;
}
void CVehicleEventLogAnalysis::AnalysisGeneralVehicle()
{
	if (m_descStruct->startPos == -1 || m_descStruct->endPos == -1)
		return ; 
	AirsideVehicleEventStruct eventStruct ;
	for (int i = 0 ; i < m_vehicleEntry->getCount();i++)
	{
		 eventStruct = m_vehicleEntry->getEvent( i) ;
		//filter by start time and end time 
		 ElapsedTime time ;
		 time.setPrecisely(eventStruct.time) ;
		if(time < m_pParam->getStartTime() )
			continue ;
		time.setPrecisely(eventStruct.time) ;
		if( time > m_pParam->getEndTime())
			break ;

		//the first event to server 
		if(eventStruct.mode == OnBeginToService )
		{
			//check the flight
			AirsideFlightDescStruct flightCon ; 
			if(GetFlightDesStructByID(flightCon,eventStruct.m_ServerFlightID))
			{
				if(!m_pParam->CheckServerFlightID(flightCon))
					continue ;
			}
			CServerIndex serverindex ; 
			serverindex.m_moveToServerIndex = i ; // the index of log which moveto server 
			for (int j = i+1 ; j < m_vehicleEntry->getCount() ; j++) //analysis the log form next log index
			{
				eventStruct =  m_vehicleEntry->getEvent( j) ;
				if(eventStruct.mode == OnMoveToRingRoute)
					serverindex.m_BeginServerIndex = j ;
				if(eventStruct.mode == OnLeaveRingRoute)
					serverindex.m_endServerIndex = j ;
				if(eventStruct.mode == OnParkingPool||eventStruct.mode == OnBeginToService ) 
				{
					if(eventStruct.mode != OnParkingPool)
					{
						serverindex.m_ReturnPool = FALSE ;
						serverindex.m_OnRoadEndindex = j - 1 ;
					}else
						serverindex.m_OnRoadEndindex = j ; 
					i = serverindex.m_OnRoadEndindex;
					m_ServerS.push_back(serverindex) ;  //one server completed
					break ;
				}
			}
			
		}
	}
}
void CVehicleEventLogAnalysis::AnalysisPaxVehicle()
{
	if (m_descStruct->startPos == -1 || m_descStruct->endPos == -1)
		return ; 
	AirsideVehicleEventStruct ServereventStruct ;
	for (int i = 0 ; i < m_vehicleEntry->getCount();i++)
	{
		ServereventStruct =  m_vehicleEntry->getEvent( i) ;

		//filter by start time and end time 
		ElapsedTime time ;
		time.setPrecisely(ServereventStruct.time) ;
		if(time < m_pParam->getStartTime() )
			continue ;
		time.setPrecisely(ServereventStruct.time) ;
		if(time  > m_pParam->getEndTime())
			break ;

		//the first event to server 
		if(ServereventStruct.mode == OnBeginToService || ServereventStruct.mode == OnBeginToServiceToGate ) 
		{
			//check the flight 
			AirsideFlightDescStruct flightCon ; 
			if(GetFlightDesStructByID(flightCon,ServereventStruct.m_ServerFlightID))
			{
				if(!m_pParam->CheckServerFlightID(flightCon))
					continue ;
			}
			CServerIndex serverindex ; 
			serverindex.m_moveToServerIndex = i ;
			AirsideVehicleEventStruct eventStruct ;
			for (int j = i+1 ; j < m_vehicleEntry->getCount(); j++)
			{
				if((ServereventStruct.mode == OnBeginToService ))
				{
					// for arrival flight
					eventStruct =  m_vehicleEntry->getEvent( j);
					if(eventStruct.mode == OnMoveToRingRoute)
						serverindex.m_BeginServerIndex = j ;
					if(eventStruct.mode == OnLeaveGate)
						serverindex.m_endServerIndex = j ;
				}
				if((ServereventStruct.mode == OnBeginToServiceToGate ))
				{
					// for dep flight 
					eventStruct = m_vehicleEntry->getEvent( j);
					if(eventStruct.mode == OnArriveAtGate)
						serverindex.m_BeginServerIndex = j ;
					if(eventStruct.mode == OnLeaveRingRoute)
						serverindex.m_endServerIndex = j ;
				}
				if(eventStruct.mode == OnParkingPool||eventStruct.mode == OnBeginToService || eventStruct.mode == OnBeginToServiceToGate) 
				{
					if(eventStruct.mode != OnParkingPool)
					{
						serverindex.m_ReturnPool = FALSE ;
						serverindex.m_OnRoadEndindex = j - 1 ;
					}else
						serverindex.m_OnRoadEndindex = j ; 
					i = serverindex.m_OnRoadEndindex ;
					m_ServerS.push_back(serverindex) ;  // one server completed 
					break ;
				}
			}
			
		}
	}
}

void CVehicleEventLogAnalysis::AnalysisFollowmeCar()
{
	if (m_descStruct->startPos == -1 || m_descStruct->endPos == -1)
		return ; 
	AirsideVehicleEventStruct eventStruct ;
	for (int i = 0 ; i < m_vehicleEntry->getCount();i++)
	{
		eventStruct = m_vehicleEntry->getEvent( i) ;
		//filter by start time and end time 
		ElapsedTime time ;
		time.setPrecisely(eventStruct.time) ;
		if(time < m_pParam->getStartTime() )
			continue ;
		time.setPrecisely(eventStruct.time) ;
		if( time > m_pParam->getEndTime())
			break ;

		//the first event to server 
		if(eventStruct.mode == OnBeginToService )
		{
			//check the flight
			AirsideFlightDescStruct flightCon ; 
			if(GetFlightDesStructByID(flightCon,eventStruct.m_ServerFlightID))
			{
				if(!m_pParam->CheckServerFlightID(flightCon))
					continue ;
			}
			CServerIndex serverindex ; 
			serverindex.m_moveToServerIndex = i; // the index of log which moveto server 
			for (int j = i+1 ; j < m_vehicleEntry->getCount() ; j++) //analysis the log form next log index
			{
				eventStruct =  m_vehicleEntry->getEvent( j) ;
				if(serverindex.m_BeginServerIndex == -1 && eventStruct.mode == OnService)
				{
					serverindex.m_BeginServerIndex = j ;
				}
					
				if(serverindex.m_endServerIndex == -1 && eventStruct.mode == OnBackPool)
				{
					serverindex.m_endServerIndex = j ;
				}

				if(eventStruct.mode == OnParkingPool||eventStruct.mode == OnBeginToService ) 
				{
					if(eventStruct.mode != OnParkingPool)
					{
						serverindex.m_ReturnPool = FALSE ;
						serverindex.m_OnRoadEndindex = j - 1 ;
					}else
						serverindex.m_OnRoadEndindex = j ; 
					i = serverindex.m_OnRoadEndindex;
					m_ServerS.push_back(serverindex) ;  //one server completed
					break ;
				}
			}

		}
	}
}

CDetailAnalysis::CDetailAnalysis(CVehicleOperaterDetailReportItem* _item , AirsideVehicleLogEntry* _vehicleEntry ,CAirsideVehicleOperParameter *pParam)
:CVehicleEventLogAnalysis(_vehicleEntry,pParam),m_DetailReportItem(_item)
{

}
void CDetailAnalysis::AnalysisVehicleForDetail()
{
	AnalysisEventLog() ;
	CalculateStartTime()  ;
	CalculateEndTime()  ;
	CalculateTotalOnRoadTime() ;
	CalculateIdelTime()  ;
	CalculateServerTime()  ;
	CalculateNumberOfServer()  ;
	CalculateNumberToPool() ;
	CalculateNumForGas()  ;
	CalculateTotalDistance() ;
	CalculateFuelConsumed()  ;
	CalculateMaxSpeed() ;
	CalculateAvgSpeed() ;
	CalculateServerTimes() ;
}
//the class for general vehicle 
void CDetailAnalysis::CalculateStartTime()
{
		if(m_ServerS.empty())
			m_DetailReportItem->m_StartServerTime.SetSecond(0) ; 
		else
		{
			int ndx  = m_ServerS[0].m_moveToServerIndex  ;
			if(ndx != -1)
			{
				AirsideVehicleEventStruct eventStruct ;
				eventStruct = m_vehicleEntry->getEvent( ndx);
				m_DetailReportItem->m_StartServerTime.setPrecisely( eventStruct.time) ; 
			}
		}
			
}
void CDetailAnalysis::CalculateEndTime()
{
	if(m_ServerS.empty())
		m_DetailReportItem->m_EndServerTime.SetSecond(0) ;
	else
	{
		int ndx  = m_ServerS[(m_ServerS.size() -1)].m_OnRoadEndindex  ;
		if(ndx != -1)
		{
			AirsideVehicleEventStruct eventStruct ;
			eventStruct = m_vehicleEntry->getEvent( ndx) ;
			m_DetailReportItem->m_EndServerTime.setPrecisely(eventStruct.time) ; 
		}
	}
}
void CDetailAnalysis::CalculateTotalOnRoadTime()
{
	long _timeOnRoadTime = 0 ; 
    for (int i = 0 ; i < (int)m_ServerS.size() ;i++)
    {
		CServerIndex serverindex ;
		serverindex =  m_ServerS[i] ;
        AirsideVehicleEventStruct BegineventStruct ;
		AirsideVehicleEventStruct EndeventStruct ;
		if(serverindex.m_BeginServerIndex != -1 && serverindex.m_moveToServerIndex != -1)
		{
			BegineventStruct = m_vehicleEntry->getEvent( serverindex.m_moveToServerIndex)  ;
			EndeventStruct = m_vehicleEntry->getEvent(serverindex.m_BeginServerIndex)  ;
			_timeOnRoadTime =_timeOnRoadTime + (EndeventStruct.time - BegineventStruct.time) ;
		}
		if(serverindex.m_endServerIndex != -1 && serverindex.m_OnRoadEndindex != -1)
		{
			BegineventStruct =  m_vehicleEntry->getEvent( serverindex.m_endServerIndex) ;
			EndeventStruct =  m_vehicleEntry->getEvent( serverindex.m_OnRoadEndindex)  ;
			_timeOnRoadTime =_timeOnRoadTime + (EndeventStruct.time - BegineventStruct.time) ;
		}
    }
	m_DetailReportItem->m_TotalOnRouteTime = _timeOnRoadTime ;
}
void CDetailAnalysis::CalculateIdelTime()
{
    long IdelTime = 0 ;
	AirsideVehicleEventStruct FirstEventStruct ;
	 AirsideVehicleEventStruct BirthEventStruct ;
	 AirsideVehicleEventStruct NextEventStruct ;
    if(m_ServerS.empty())
	{
		m_DetailReportItem->m_TotalIdelTime = m_descStruct->endTime - m_descStruct->startTime ;
		return ;
	}
	for (int i =0 ; i < m_vehicleEntry->getCount() ;i++)
	{
	   NextEventStruct =  m_vehicleEntry->getEvent( i) ;
	   if(NextEventStruct.mode == OnVehicleBirth ) // from birth -----  first server ,it's parking time
	   {
		   BirthEventStruct = NextEventStruct ;
		   for (int j = i+1 ; j < m_vehicleEntry->getCount() ;j++ )
		   {
				NextEventStruct = m_vehicleEntry->getEvent( j) ;
				if(NextEventStruct.mode == OnBeginToService || NextEventStruct.mode == OnBeginToServiceToGate)
				{
					IdelTime = IdelTime + (NextEventStruct.time - BirthEventStruct.time) ;
					 i = j ;
					break ;
				}
		   }  
	   }
	   if(NextEventStruct.mode == OnParkingPool) // from parking ------ next server ,it's parking time 
	   {
		   FirstEventStruct = NextEventStruct ;
		   int j = i+1 ;
		   for ( ; j < m_vehicleEntry->getCount() ;j++ )
		   {
			   NextEventStruct = m_vehicleEntry->getEvent( j);
			   if(NextEventStruct.mode != OnParkingPool)
			   {
				   IdelTime = IdelTime + (NextEventStruct.time - FirstEventStruct.time) ;
				   i = j ;
				   break ;
			   }
		   }  
		   if(j == m_vehicleEntry->getCount())
		   {
				IdelTime = IdelTime + (NextEventStruct.time - FirstEventStruct.time) ; // the last time at pool 
				break ;
		   }
	   }
	}
    m_DetailReportItem->m_TotalIdelTime = IdelTime ;
}
void CDetailAnalysis::CalculateServerTime()
{
	long ServerTime = 0 ;
	CServerIndex serverIndex ;
	AirsideVehicleEventStruct BeginServerEventStruct ;
	AirsideVehicleEventStruct EndServerEventStruct ;
	for (int i = 0 ; i < (int)m_ServerS.size() ;i++)
	{
		serverIndex = m_ServerS[i] ;
		if(serverIndex.m_BeginServerIndex != -1 && serverIndex.m_endServerIndex != -1)
		{
			BeginServerEventStruct = m_vehicleEntry->getEvent( serverIndex.m_BeginServerIndex) ;
			EndServerEventStruct = m_vehicleEntry->getEvent(serverIndex.m_endServerIndex) ;
			ServerTime = ServerTime + (EndServerEventStruct.time - BeginServerEventStruct.time) ;
		}

	}
	m_DetailReportItem->m_ServerTime = ServerTime;
}
void CDetailAnalysis::CalculateNumberOfServer()
{
	m_DetailReportItem->m_ServerNumber = m_ServerS.size() ;
}
void CDetailAnalysis::CalculateNumberToPool()
{
	int number = 0 ;
	for (int i = 0 ; i < (int)m_ServerS.size() ;i++)
	{
		if(m_ServerS[i].m_ReturnPool == TRUE)
			number++ ;
	}
	m_DetailReportItem->m_NumberToPool = number ;
}
void CDetailAnalysis::CalculateNumForGas()
{
	
}
void CDetailAnalysis::CalculateTotalDistance()
{
	double totalDis = 0  ;
	long lTotalTime = 0l;
	AirsideVehicleEventStruct FirstEventStruct ;
	memset(&FirstEventStruct,0,sizeof(AirsideVehicleEventStruct) ) ;
	AirsideVehicleEventStruct nextEventStruct ;
	CPoint2008 currentPos ;
	CPoint2008 lastPos ;
	for (int i = 0 ; i < (int)m_ServerS.size() ; i++)
	{
       if(m_ServerS[i].m_moveToServerIndex == -1 ||  m_ServerS[i].m_OnRoadEndindex == -1 )
		   continue ;
	   FirstEventStruct = m_vehicleEntry->getEvent(m_ServerS[i].m_moveToServerIndex) ;
	   lastPos.setX(FirstEventStruct.x) ;
	   lastPos.setY(FirstEventStruct.y) ;
	   lastPos.setZ(FirstEventStruct.z) ;
	   for (int j = m_ServerS[i].m_moveToServerIndex ; j <= m_ServerS[i].m_OnRoadEndindex ;j++)
	   {
           nextEventStruct = m_vehicleEntry->getEvent(j) ;
		   ElapsedTime time ;
		   time.setPrecisely(nextEventStruct.time) ;
		   if(time > m_pParam->getEndTime())
			   break ;
		   currentPos.setX(nextEventStruct.x) ;
		   currentPos.setY(nextEventStruct.y) ;
		   currentPos.setZ(nextEventStruct.z) ;
		   totalDis = totalDis + currentPos.distance(lastPos) ;
		   lTotalTime += nextEventStruct.time - FirstEventStruct.time;
           //set last point 
		   lastPos = currentPos ;
		   FirstEventStruct = nextEventStruct;
	   }
	}
	m_DetailReportItem->m_TotalDistance = totalDis ;
	if (lTotalTime > 0l)
	{
		m_DetailReportItem->m_AvgSpeed = static_cast<float>((totalDis*100) / lTotalTime);
	}
}
void CDetailAnalysis::CalculateFuelConsumed()
{
   m_DetailReportItem->m_FuelConsumed =  (long)(m_descStruct->m_FuelConsumed * m_DetailReportItem->m_TotalDistance/100000) ;
}
void CDetailAnalysis::CalculateMaxSpeed()
{
	double maxspeed = 0 ;
	AirsideVehicleEventStruct EventStruct ;
	for (int i =  0 ; i < (int)m_ServerS.size() ; i++)
	{
		if(m_ServerS[i].m_moveToServerIndex == -1 ||  m_ServerS[i].m_OnRoadEndindex == -1 )
			continue ;
		for (int j = m_ServerS[i].m_moveToServerIndex ; j <= m_ServerS[i].m_OnRoadEndindex ;j++)
		{
			EventStruct = m_vehicleEntry->getEvent(j)  ;
			ElapsedTime time ;
			time.setPrecisely(EventStruct.time) ;
			if(time > m_pParam->getEndTime())
				continue ;
			maxspeed = EventStruct.speed > maxspeed ? EventStruct.speed:maxspeed ;
		}
	}
	m_DetailReportItem->m_MaxSpeed = (float)maxspeed ;
}
void CDetailAnalysis::CalculateAvgSpeed()
{
 if(m_DetailReportItem->m_StartServerTime.asSeconds() == 0)
	 return ;
ElapsedTime time ;
time.setPrecisely(m_DetailReportItem->m_TotalOnRouteTime) ;
// m_DetailReportItem->m_AvgSpeed = (float)(m_DetailReportItem->m_TotalDistance/ time.asSeconds() ) ;

}

void CDetailAnalysis::CalculateServerTimes()
{
	m_DetailReportItem->m_ServerSTimes.clear() ;
	AirsideVehicleEventStruct EventStruct ;
	for (int i = 0 ; i < (int)m_ServerS.size() ;i++ )
	{
		int ndx = m_ServerS[i].m_moveToServerIndex ;
		if(ndx != -1)
		{
			EventStruct = m_vehicleEntry->getEvent(ndx) ;
            m_DetailReportItem->m_ServerSTimes.push_back(EventStruct.time) ;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CAirsideVehicleOperaterReport::GenerateSummaryReport(CAirsideVehicleOperParameter *pParam )
{

}
void CAirsideVehicleOperaterReport::CalSummaryInterval(CAirsideVehicleOperParameter *pParam )
{
	if(pParam == NULL)
		return ;
	ElapsedTime totalTime ;
	totalTime = pParam->getEndTime() - pParam->getStartTime() ;
	ElapsedTime time ;
	time.SetSecond( totalTime.asSeconds() / 6 );
	m_IntervalSize = 6 ;
	pParam->setInterval(time.asSeconds()) ;
	while( (m_IntervalSize * time.asSeconds()) < pParam->getEndTime().asSeconds() )
	{
		m_IntervalSize++ ;
	}
}
void CAirsideVehicleOperaterReport::AnalysisSummaryReportItem(AirsideVehicleDescStruct& _VehicleDesStruct,CAirsideVehicleOperParameter *pParam )
{
	CVehicleOperaterSummaryReportItem* Item = new CVehicleOperaterSummaryReportItem;
	Item->m_vehicleType = _VehicleDesStruct.m_VecileTypeID  ;
	ElapsedTime startTime = pParam->getStartTime() ;
	CVehicleOperaterSummaryReportSubItem* subitem = NULL;
	
	for ( int i = 0 ; i < (int)m_IntervalSize ;i++)
	{
		subitem = new CVehicleOperaterSummaryReportSubItem ;
		subitem->m_StartTime = startTime ;
		subitem->m_EndTime = startTime + pParam->getInterval() ;
        startTime = subitem->m_EndTime ;
		Item->m_SubItem.push_back(subitem) ;
	}
	CSummaryAnalysis analysis (Item,&logEntry,pParam) ;
	analysis.SetFlightLog(&m_fltLog) ;
	analysis.AnalysisEventLog() ;
	m_SummaryResult.push_back(Item) ;
}
//the analysis for summary 
CSummaryAnalysis::CSummaryAnalysis(CVehicleOperaterSummaryReportItem* _item  , AirsideVehicleLogEntry* _vehicleEntry ,CAirsideVehicleOperParameter *pParam)
:m_item(_item),CVehicleEventLogAnalysis(_vehicleEntry,pParam)
{
  m_descStruct = &(m_vehicleEntry->GetAirsideDesc());
}
void CSummaryAnalysis::AnalysisEventLog()
{
	CVehicleEventLogAnalysis::AnalysisEventLog() ;
	 CalculateOnroadTime() ;
	 CalculateServerTime() ;
	 CalculateDistance() ;
	 CalculateFuelConsumed() ;
	 CalculateSpeed() ;
}
void CSummaryAnalysis::CalculateOnroadTime()
{
	long onRoadTime = 0 ;
	AirsideVehicleEventStruct OnroadEventStruct ;
	AirsideVehicleEventStruct ServerEventStruct ;

       for (int j = 0 ; j < (int)m_ServerS.size() ; j++)
       {
		   for (int i = 0 ; i < (int)m_item->m_SubItem.size() ;i++)
		   {
			   onRoadTime = 0 ;
			   CVehicleOperaterSummaryReportSubItem subitem ;
			   subitem.m_StartTime = m_item->m_SubItem[i]->m_StartTime ;
			   subitem.m_EndTime =  m_item->m_SubItem[i]->m_EndTime ;
              long beginTime = 0 ;
			  long endtime = 0 ;
		     if(m_ServerS[j].m_moveToServerIndex == -1 )
			   continue ;
		     if(m_ServerS[j].m_BeginServerIndex != -1)
				{
					//move -----  server place 
						OnroadEventStruct = m_vehicleEntry->getEvent(m_ServerS[j].m_moveToServerIndex)   ;
						ServerEventStruct = m_vehicleEntry->getEvent(m_ServerS[j].m_BeginServerIndex)  ;
					
						//check the time 
					if(min((long)subitem.m_EndTime,ServerEventStruct.time) - max(OnroadEventStruct.time,(long)subitem.m_StartTime) > 0)
					{
						beginTime = max(OnroadEventStruct.time,(long)subitem.m_StartTime) ;
						ElapsedTime time ;
						time.setPrecisely(ServerEventStruct.time) ;
						if(time < subitem.m_EndTime )
							onRoadTime = onRoadTime + (ServerEventStruct.time - beginTime) ;
						else
						{
							for (int ndx = m_ServerS[j].m_moveToServerIndex + 1 ; ndx < m_ServerS[j].m_BeginServerIndex ;ndx++)
							{
								ServerEventStruct = m_vehicleEntry->getEvent(ndx) ;
								time.setPrecisely(ServerEventStruct.time) ;
								if(time > subitem.m_EndTime)
								{
									onRoadTime = onRoadTime + (ServerEventStruct.time -beginTime) ;
									break ;
								}
							}
						}
					}
				}
				//leave server --->
				if(m_ServerS[j].m_endServerIndex == -1 )
					continue ;
				if(m_ServerS[j].m_OnRoadEndindex != -1)
				{
					OnroadEventStruct = m_vehicleEntry->getEvent( m_ServerS[j].m_endServerIndex ) ;
					ServerEventStruct = m_vehicleEntry->getEvent( m_ServerS[j].m_OnRoadEndindex )  ;
					//check the time 
					if(min((long)subitem.m_EndTime,ServerEventStruct.time) - max(OnroadEventStruct.time,(long)subitem.m_StartTime) > 0)
					{
						beginTime = max(OnroadEventStruct.time,(long)subitem.m_StartTime) ;
						ElapsedTime time ;
						time.setPrecisely(ServerEventStruct.time)  ;
						if(time < subitem.m_EndTime )
							onRoadTime = onRoadTime + (ServerEventStruct.time - beginTime) ;
						else
						{
							for (int ndx = m_ServerS[j].m_endServerIndex + 1 ; ndx < m_ServerS[j].m_OnRoadEndindex ;ndx++)
							{
								ServerEventStruct = m_vehicleEntry->getEvent(ndx) ;
								time.setPrecisely(ServerEventStruct.time) ;
								if(time > subitem.m_EndTime)
								{
									onRoadTime = onRoadTime + (ServerEventStruct.time - beginTime) ;
									break ;
								}
							}
						}

					}	
				}
					m_item->m_SubItem[i]->m_OnRoadTime =  m_item->m_SubItem[i]->m_OnRoadTime + onRoadTime ;
			}
	}
}
void CSummaryAnalysis::CalculateServerTime()
{
   long ServerTime = 0 ;
   AirsideVehicleEventStruct ServerEventStruct ;
   AirsideVehicleEventStruct LeverEventStruct ;
   //long serverTime = 0 ;
   for (int j = 0 ; j < (int)m_ServerS.size() ; j++)
   {
       if(m_ServerS[j].m_BeginServerIndex == -1 || m_ServerS[j].m_endServerIndex == -1)
		   continue ;
		ServerEventStruct = m_vehicleEntry->getEvent( m_ServerS[j].m_BeginServerIndex) ;
		LeverEventStruct  = m_vehicleEntry->getEvent( m_ServerS[j].m_endServerIndex) ;
		ElapsedTime startTime ;
		ElapsedTime EndTime ;
	   for (int i = 0 ; i < (int)m_item->m_SubItem.size() ;i++)
	   {
          startTime = m_item->m_SubItem[i]->m_StartTime ;
		  EndTime = m_item->m_SubItem[i]->m_EndTime ;

		  ServerTime = min((long)EndTime,LeverEventStruct.time) -  max((long)startTime,ServerEventStruct.time)  ;
		  if(ServerTime >  0)
		  {
			m_item->m_SubItem[i]->m_serverTime = m_item->m_SubItem[i]->m_serverTime + ServerTime ;
			continue ;
		  }
	   }
   }
}
void CSummaryAnalysis::CalculateSpeed()
{
	for (int i = 0 ; i < (int)m_item->m_SubItem.size() ;i++)
	{
        ElapsedTime time ;
		time.setPrecisely(m_item->m_SubItem[i]->m_OnRoadTime) ;
		if(m_item->m_SubItem[i]->m_Distance > 0)
		 m_item->m_SubItem[i]->m_speed = (float)(m_item->m_SubItem[i]->m_Distance / time.asSeconds() ) ;
		else
		m_item->m_SubItem[i]->m_speed = 0 ;
	}
}
void CSummaryAnalysis::CalculateFuelConsumed()
{
	for (int i = 0 ; i <(int)m_item->m_SubItem.size() ;i++)
	{

		 m_item->m_SubItem[i]->m_Fuel =(float)( m_item->m_SubItem[i]->m_Distance/100000 * (m_descStruct->m_FuelConsumed) );
	}
}
void CSummaryAnalysis::CalculateDistance()
{
	long ServerTime = 0 ;
	AirsideVehicleEventStruct beginEventStruct ;
	AirsideVehicleEventStruct LeverEventStruct ;
	long serverTime = 0 ;
	for (int j = 0 ; j < (int)m_ServerS.size() ; j++)
	{
		if(m_ServerS[j].m_BeginServerIndex == -1 || m_ServerS[j].m_endServerIndex == -1)
			continue ;
		beginEventStruct = m_vehicleEntry->getEvent(m_ServerS[j].m_moveToServerIndex)   ;
		LeverEventStruct  =  m_vehicleEntry->getEvent(m_ServerS[j].m_OnRoadEndindex)  ;


		ElapsedTime startTime ;
		ElapsedTime EndTime ;
		int ndx = m_ServerS[j].m_moveToServerIndex ;
		for (int i = 0 ; i < (int)m_item->m_SubItem.size() ;i++)
		{

			startTime = m_item->m_SubItem[i]->m_StartTime ;
			EndTime = m_item->m_SubItem[i]->m_EndTime ;

            long firstTime =  max(beginEventStruct.time,(long)startTime) ;
			long secondTime = min((long)EndTime,LeverEventStruct.time) ;
			if(secondTime - firstTime <= 0)
               continue ;
			CPoint2008 firstPoint ;
			firstPoint.setX(beginEventStruct.x) ;
			firstPoint.setY(beginEventStruct.y) ;
			firstPoint.setZ(beginEventStruct.z) ;
			CPoint2008 NextPoint ;
			AirsideVehicleEventStruct EventStruct ;
		     for ( ; ndx <= m_ServerS[j].m_OnRoadEndindex ;ndx++)
		     {
                   EventStruct = m_vehicleEntry->getEvent(ndx) ;
				   NextPoint.setX(EventStruct.x) ;
				   NextPoint.setY(EventStruct.y) ;
				   NextPoint.setZ(EventStruct.z) ;
				   if(EventStruct.time < firstTime )
				   {
                       firstPoint = NextPoint ;
					   continue ;
				   }
				   if(EventStruct.time > secondTime )
				   {
					   beginEventStruct = EventStruct ;
					   break ;
				   }
				    m_item->m_SubItem[i]->m_Distance = m_item->m_SubItem[i]->m_Distance + NextPoint.distance(firstPoint) ;
                    firstPoint = NextPoint ; 
		     }
			 if(ndx > m_ServerS[j].m_OnRoadEndindex)
				 break ;
		}
	}
}
BOOL CAirsideVehicleOperaterReport::ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType)
{ 
	if(reportType == ASReportType_Detail)
	{
		_file.writeInt((int)m_DetailResult.size()) ;
		_file.writeLine() ;
		for (int i =0 ;i < (int)m_DetailResult.size() ;i++)
		{
			m_DetailResult[i].ExportFile(_file) ;
		}
	}else
	{
		_file.writeInt((int)m_FinalResultSummary.size()) ;
		_file.writeLine() ;
		for (int i = 0 ;i < (int)m_FinalResultSummary.size() ;i++)
		{
			m_FinalResultSummary[i].ExportFile(_file) ;
		}
	}
	return TRUE ;
} ;
BOOL CAirsideVehicleOperaterReport::ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType )  
{
	int size = 0 ;
	if(!_file.getInteger(size) )
	{
		Errmsg.Format(_T("Read size error")) ;
		return FALSE ;
	}
	if(reportType == ASReportType_Detail)
	{

		m_DetailResult.clear() ;
		for (int i = 0 ; i < size ;i++)
		{
			_file.getLine() ;
			CVehicleOperaterDetailReportItem  reportitem ;
			reportitem.ImportFile(_file) ;
			m_DetailResult.push_back(reportitem) ;
		}

	}else
	{
		m_FinalResultSummary.clear() ;
		for (int i= 0 ;i < size;i++)
		{
			_file.getLine() ;
			CSummaryFinialResult  reportitem ;
			reportitem.ImportFile(_file) ;
			m_FinalResultSummary.push_back(reportitem) ;
		}
	}
	return TRUE ;
}

CString CAirsideVehicleOperaterReport::GetReportFileName()
{
	return "Vehicle\\AirsideVehicleOperaterReport.rep" ;
}

BOOL CAirsideVehicleOperaterReport::WriteReportData( ArctermFile& _file )
{
	_file.writeField("RunwayExit Report");
	_file.writeLine();
	_file.writeField("Paramerate");
	_file.writeLine();
	m_pParam->ExportFile(_file) ;
	enumASReportType_Detail_Summary type = m_pParam->getReportType() ;
	_file.writeInt(type) ;
	_file.writeLine() ;
	return ExportReportData(_file,CString(),type) ;
}

BOOL CAirsideVehicleOperaterReport::ReadReportData( ArctermFile& _file )
{
	_file.getLine() ;
	m_pParam->ImportFile(_file) ;
	int type = 0 ;
	_file.getLine() ;
	if(!_file.getInteger(type))
		return FALSE;
	_file.getLine() ;
	return ImportReportData(_file,CString(),(enumASReportType_Detail_Summary)type) ;
}
//////////////////////////////////////////////////////////////////////////