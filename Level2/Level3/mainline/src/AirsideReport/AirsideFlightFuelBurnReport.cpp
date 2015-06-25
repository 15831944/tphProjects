#include "StdAfx.h"
#include ".\airsideflightfuelburnreport.h"
#include "AirsideFlightFuelBurnReport.h"
#include "AirsideFlightFuelBurnReportResult.h"
#include "AirsideFlightBurnParmater.h"
#include "../Common/AIRSIDE_BIN.h"
#include "..\Common\AirportDatabase.h"
#include "../InputAirside/ARCUnitManager.h"
#include "../Reports/StatisticalTools.h"

#include "../InputAirside/FlightPerformanceCruise.h"
#include "../InputAirside/FlightPerformanceTerminal.h"
#include "../InputAirside/ApproachLand.h"
#include "../InputAirside/PerformLandings.h"
#include "../InputAirside/TaxiInbound.h"
#include "../InputAirside/TaxiOutbound.h"
#include "../InputAirside/PerformanceTakeoffs.h"
#include "../InputAirside/DepClimbOut.h"
CFlightFuelBurningCalculator::CFlightFuelBurningCalculator(CAirportDatabase* _airportDB,int _proid):m_AirportDB(_airportDB),m_proID(_proid)
{

}
CFlightFuelBurningCalculator::FlightFuelState CFlightFuelBurningCalculator::JuedgeWhichStateBelongTo(AirsideMobileElementMode _mode)
{
	CFlightCrusieFuelBurnState CrusuieFuelBurn ;
	if(CrusuieFuelBurn.IsAtThisArea(_mode))
		return STATE_crusie ;
	CFlightTerminalFuelBurnState TerminalFuelBurn ;
	if(TerminalFuelBurn.IsAtThisArea(_mode))
		return STATE_terminal;
	CFlightApproachToLandFuelBurnState TolandFuelBurn ;
	if(TolandFuelBurn.IsAtThisArea(_mode))
		return STATE_Approachtoland ;

	CFlightLandingFuelBurnState landingFuelBurn ;
	if(landingFuelBurn.IsAtThisArea(_mode))
		return STATE_landing ;

	CFlightTaxiInBound inboundFuelBurn ;
	if(inboundFuelBurn.IsAtThisArea(_mode))
		return STATE_inbound ;

	CFlightTaxiOutboundFuelBurnState OutBoundFuelBurn ;
	if(OutBoundFuelBurn.IsAtThisArea(_mode))
		return STATE_outbound ;

	CFlightTakeoffFuelBurnState takeoffFuelBurn ;
	if(takeoffFuelBurn.IsAtThisArea(_mode))
		return STATE_takeoff ;

	CFlightClimoutFuelBurnState ClimoutFuelBurn ;
	if(ClimoutFuelBurn.IsAtThisArea(_mode))
		return STATE_climbout ;
	return STATE_UNKNOW ;
}
double CFlightFuelBurningCalculator::GetFuelBurnAtEachStateByFlightType(CFlightFuelBurningCalculator::FlightFuelState _state,AirsideFlightDescStruct& _fltType)
{
	using namespace ns_AirsideInput;
	switch(_state)
	{
	case STATE_crusie:
		{
			vFlightPerformanceCruise performanceCruise(m_AirportDB) ;
			performanceCruise.ReadData(m_proID) ;
			return performanceCruise.GetFuelBurnByFlightType(_fltType) ;
			break ;
		}
	case STATE_terminal:
		{
			vFlightPerformanceTerminal performanceTerminal(m_AirportDB) ;
			performanceTerminal.ReadData(m_proID) ;
			return performanceTerminal.GetFuelBurnByFlightTy(_fltType) ;
		}
	case STATE_Approachtoland:
		{
			ApproachLands performanceLands(m_AirportDB) ;
			performanceLands.ReadData(m_proID) ;
			return performanceLands.GetFuelBurnByFlightType(_fltType) ;
		}
	case STATE_landing:
		{
			PerformLandings performanceLanding(m_AirportDB) ;
			performanceLanding.ReadData(m_proID) ;
			return performanceLanding.GetFuelBurnByFlightType(_fltType) ;
		}
	case STATE_inbound:
		{
			CTaxiInbound performanceInbound(m_AirportDB) ;
			performanceInbound.ReadData(m_proID) ;
			return performanceInbound.GetFuelBurnByFlightType(_fltType) ;
		}
	case STATE_server:
		return 0 ;
	case STATE_Pushback_tow:
		{
			return 0 ;
		}
	case STATE_outbound:
		{
			CTaxiOutbound performanceOutbound(m_AirportDB) ;
			performanceOutbound.ReadData(m_proID) ;
			return performanceOutbound.GetFuelBurnByFlightType(_fltType) ;
		}
	case STATE_takeoff:
		{
			PerformTakeOffs performanceTakeoff(m_AirportDB) ;
			performanceTakeoff.ReadData(m_proID) ;
			return performanceTakeoff.GetFuelBurnByFlightType(_fltType) ;
		}
	case STATE_climbout:
		{
			DepClimbOuts performanceClimbouts(m_AirportDB) ;
			performanceClimbouts.ReadData(m_proID) ;
			return performanceClimbouts.GetFuelBurnByFlightType(_fltType) ;
		}
	}
	return 0 ;
}

double CFlightFuelBurningCalculator::CalculateFuelBurnByEvenFlightLog( AirsideFlightLogEntry& logEntry ,int _fromIndx,int _toIndex ,AirsideFlightDescStruct& _flttype,CFlightFuelBurnReportItem& _reportItem)
{
	ARCEventLog* pLog = NULL ;
	FlightFuelState _Atstate ;
	FlightFuelState _PriAtState = STATE_UNKNOW ;
	AirsideFlightEventStruct _PriFlightEventlog  ;
	double totalFuelBurning = 0 ;
	double stateFuelBurn = 0 ;
	BOOL _arrBeginTime = FALSE;
	BOOL _arrEndTIme = FALSE;
	BOOL _DepBeginTime = FALSE;
	BOOL _dependTime = FALSE;
	for (int i = _fromIndx ; i <=_toIndex;i++)
	{
 		AirsideFlightEventStruct event = logEntry.getEvent(i);
		
		_Atstate = JuedgeWhichStateBelongTo((AirsideMobileElementMode)event.mode) ;

			if(event.mode <= OnHeldAtStand)
			{
				if(!_arrBeginTime)
				{
					_reportItem.m_ArrFromTime.setPrecisely(event.time) ;
					_arrBeginTime = TRUE ;
				}
				if(i == _toIndex)
				{
					_reportItem.m_ArrToTime.setPrecisely(event.time) ;
				}
			}else
			{
				if(_arrBeginTime && _arrEndTIme == FALSE)
				{
					_reportItem.m_ArrToTime.setPrecisely(event.time) ;
					_arrEndTIme = TRUE ;
				}
				if(!_DepBeginTime)
				{
					_reportItem.m_DepFromTime.setPrecisely(event.time) ;
					_DepBeginTime  = TRUE ;
				}
				if(i == _toIndex)
				{
					_reportItem.m_DepToTime.setPrecisely(event.time) ;
				}
			}
		if(_Atstate != _PriAtState)
		{
			if(_PriAtState != STATE_UNKNOW) //another state ,calculate fuel 
			{
				stateFuelBurn = GetFuelBurnAtEachStateByFlightType(_PriAtState,_flttype) ;
				ElapsedTime intervaltime ;
				intervaltime.setPrecisely(event.time - _PriFlightEventlog.time) ;
				if(event.mode < OnHeldAtStand)
				{
					_reportItem.m_FuelBurnArrival += stateFuelBurn/3600 * intervaltime.asSeconds() ;
				}
				else
				{
					_reportItem.m_FuleBurnDep += stateFuelBurn/3600 * intervaltime.asSeconds() ;
				}
			}
			_PriAtState = _Atstate ;
			_PriFlightEventlog = event ;
		}else
		{
			if(_PriAtState != STATE_UNKNOW && i == _toIndex) //all the log are in the one state .
			{
				stateFuelBurn = GetFuelBurnAtEachStateByFlightType(_PriAtState,_flttype) ;
				ElapsedTime intervaltime ;
				intervaltime.setPrecisely(event.time - _PriFlightEventlog.time) ;
				if(event.mode < OnHeldAtStand)
				{
					_reportItem.m_FuelBurnArrival += stateFuelBurn/3600 * intervaltime.asSeconds() ;
				}
				else
				{
					_reportItem.m_DepToTime.setPrecisely(event.time) ;
					_reportItem.m_FuleBurnDep += stateFuelBurn/3600 * intervaltime.asSeconds() ;
				}
			}
		}

	}
	return _reportItem.m_FuleBurnDep + _reportItem.m_FuelBurnArrival ;
}
//////////////////////////////////////////////////////////////////////////
CAirsideFlightFuelBurnReport::~CAirsideFlightFuelBurnReport(void)
{
}

void CAirsideFlightFuelBurnReport::GenerateReport( CParameters * parameter )
{
	ClearAllData() ;
	//read log
	CString strDescFilepath = (*m_pCBGetLogFilePath)(AirsideDescFile);
	CString strEventFilePath = (*m_pCBGetLogFilePath)(AirsideEventFile);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;
	EventLog<AirsideFlightEventStruct> airsideFlightEventLog;
	AirsideFlightLog fltLog;
	fltLog.SetEventLog(&airsideFlightEventLog);
	fltLog.LoadDataIfNecessary(strDescFilepath);
	fltLog.LoadEventDataIfNecessary(strEventFilePath);

	CFlightFuelBurningCalculator flightfuelBurnCal(m_AirportDB,m_proid) ;
	int nFltLogCount = fltLog.getCount();
	for (int i =0; i < nFltLogCount; ++i)
	{
		AirsideFlightLogEntry logEntry;
		logEntry.SetEventLog(&airsideFlightEventLog);
		fltLog.getItem(logEntry,i);	
		

		AirsideFlightDescStruct	fltDesc = logEntry.GetAirsideDescStruct();

		if (IsValidTime(fltDesc,parameter) == false)
			continue;

		if (IsFitConstraint(fltDesc,parameter) == false)
			continue;

		int nStartPos = 0, nEndPos = 0;
		long lDelayTime = 0;
		if (GetFitFlightEventLogByStartAndEndPostion(logEntry,nStartPos,nEndPos) == false )
			continue;
		if(nEndPos <= nStartPos)
			continue ;
		CFlightFuelBurnReportItem reportitem ;
		reportitem.m_Actype = fltDesc._acType ;
		reportitem.m_Airline = fltDesc._airline ;
		reportitem.m_ArrivalID = fltDesc._arrID ;
		reportitem.m_DepID = fltDesc.depID ;
		flightfuelBurnCal.CalculateFuelBurnByEvenFlightLog(logEntry,nStartPos,nEndPos,fltDesc,reportitem) ;
		reportitem.m_fltDesc = fltDesc ;
		m_ReportItemData.push_back(reportitem) ;
	}
	if(parameter->getReportType() == ASReportType_Detail)
		GenerateDetailReport() ;
	else
		GenerateSummaryReport() ;

	SaveReportData() ;
}
bool CAirsideFlightFuelBurnReport::IsValidTime(AirsideFlightDescStruct& fltDesc,CParameters * parameter)
{

	//time valid
	if (fltDesc.startTime == 0 && fltDesc.endTime == 0)
		return false;


	if (fltDesc.endTime < parameter->getStartTime().asSeconds()*100 || fltDesc.startTime> parameter->getEndTime().asSeconds()*100)//not in the time range
		return false;

	return true;
}
bool CAirsideFlightFuelBurnReport::IsFitConstraint(AirsideFlightDescStruct& fltDesc,CParameters * parameter)
{	
	int size = parameter->getFlightConstraintCount() ;
	for (int i = 0 ;i < size; i++)
	{
		if(parameter->getFlightConstraint(i).fits(fltDesc))
		{
			return true;
		}
	}
	return false;
}
BOOL CAirsideFlightFuelBurnReport::GetFitFlightEventLogByStartAndEndPostion(AirsideFlightLogEntry& logEntry,int& _startPos ,int& _endPos )
{
	int nEventCount = logEntry.getCount();

	CAirsideReportNode startNode = m_Parameter->GetFromNode();
	CAirsideReportNode endNode = m_Parameter->GetToNode();

	//default
	if (startNode.IsDefault() && endNode.IsDefault())
	{
		_startPos = 0;
		_endPos = nEventCount-1;
		return true;
	}
	else if (startNode.IsDefault())
	{
		for (int nEvent = 0; nEvent < nEventCount; ++nEvent)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(nEvent);
			if ((m_Parameter->GetToNode().GetNodeType() == CAirsideReportNode::ReportNodeType_WayPointStand && m_Parameter->GetToNode().IncludeObj(event.nodeNO))||
				(m_Parameter->GetToNode().GetNodeType() == CAirsideReportNode::ReportNodeType_Intersection&&m_Parameter->GetToNode().IncludeObj(event.intNodeID))||
				(m_Parameter->GetToNode().GetNodeType() == CAirsideReportNode::ReportNodeType_Taxiway&&m_Parameter->GetToNode().IncludeObj(event.intNodeID)))
			{
				_startPos = 0;
				_endPos = nEvent;
				return true;
			}
		}
	}
	else if ( endNode.IsDefault())
	{
		for (int nEvent = 0; nEvent < nEventCount; ++nEvent)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(nEvent);
			if ((m_Parameter->GetFromNode().GetNodeType() == CAirsideReportNode::ReportNodeType_WayPointStand && m_Parameter->GetFromNode().IncludeObj(event.nodeNO))||
				(m_Parameter->GetFromNode().GetNodeType() == CAirsideReportNode::ReportNodeType_Intersection&&m_Parameter->GetFromNode().IncludeObj(event.intNodeID))||
				(m_Parameter->GetFromNode().GetNodeType() == CAirsideReportNode::ReportNodeType_Taxiway&&m_Parameter->GetFromNode().IncludeObj(event.intNodeID)))

			{
				_startPos = nEvent;
				_endPos = nEventCount-1;
				return true;
			}
		}

	}
	else
	{
		//find start pos
		bool  bFindStartPos = false;
		for (int nStartEvent = 0; nStartEvent < nEventCount; ++nStartEvent)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(nStartEvent);
			if ((m_Parameter->GetFromNode().GetNodeType() == CAirsideReportNode::ReportNodeType_WayPointStand && m_Parameter->GetFromNode().IncludeObj(event.nodeNO))||
				(m_Parameter->GetFromNode().GetNodeType() == CAirsideReportNode::ReportNodeType_Intersection&&m_Parameter->GetFromNode().IncludeObj(event.intNodeID))||
				(m_Parameter->GetFromNode().GetNodeType() == CAirsideReportNode::ReportNodeType_Taxiway&&m_Parameter->GetFromNode().IncludeObj(event.intNodeID)))
			{
				_startPos = nStartEvent;
				bFindStartPos = true;
				break;
			}
		}

		if (bFindStartPos == false)
			return false;

		//find endpos
		bool  bFindEndPos = false;
		for (int nEndEvent = 0; nEndEvent < nEventCount; ++nEndEvent)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(nEndEvent);
			if ((m_Parameter->GetToNode().GetNodeType() == CAirsideReportNode::ReportNodeType_WayPointStand && m_Parameter->GetToNode().IncludeObj(event.nodeNO))||
				(m_Parameter->GetToNode().GetNodeType() == CAirsideReportNode::ReportNodeType_Intersection&&m_Parameter->GetToNode().IncludeObj(event.intNodeID))||
				(m_Parameter->GetToNode().GetNodeType() == CAirsideReportNode::ReportNodeType_Taxiway&&m_Parameter->GetToNode().IncludeObj(event.intNodeID)))
			{
				_endPos = nEndEvent;
				bFindEndPos = true;
				break;
			}
		}

		if (bFindEndPos == false)
			return false;


		return true;

	}

	return false;
}

void CAirsideFlightFuelBurnReport::InitListHead( CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC )
{
	if(reportType == ASReportType_Detail)
		InitDetailListHead(cxListCtrl,piSHC) ;
	else
		InitSummaryListHead(cxListCtrl,piSHC) ;
}
void CAirsideFlightFuelBurnReport::InitDetailListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);
	cxListCtrl.InsertColumn(0,_T("Airline"), LVCFMT_LEFT, 100) ;
	cxListCtrl.InsertColumn(1,_T("FlightID"), LVCFMT_LEFT, 100) ;
	cxListCtrl.InsertColumn(2,_T("Actype"),LVCFMT_LEFT,100) ;
	cxListCtrl.InsertColumn(3,_T("Operation"),LVCFMT_LEFT,100) ;
	cxListCtrl.InsertColumn(4,_T("From"),LVCFMT_LEFT,100) ;
	cxListCtrl.InsertColumn(5,_T("To"),LVCFMT_LEFT,100) ;
	cxListCtrl.InsertColumn(6,_T("Time From"),LVCFMT_LEFT,100) ;
	cxListCtrl.InsertColumn(7,_T("Time To"),LVCFMT_LEFT,100) ;
	cxListCtrl.InsertColumn(8,_T("Duration(mins)"),LVCFMT_LEFT,100) ;
	CString fuelUnit ;
	fuelUnit = CARCWeightUnit::GetWeightUnitString(AU_WEIGHT_POUND) ;
	cxListCtrl.InsertColumn(9,_T("Fuel Burn(" + fuelUnit + _T(")")),LVCFMT_LEFT,100) ;

	if (piSHC)
	{
		piSHC->ResetAll();
		piSHC->SetDataType(0,dtSTRING);
		piSHC->SetDataType(1,dtSTRING);
		piSHC->SetDataType(2,dtSTRING);
		piSHC->SetDataType(3,dtSTRING);
		piSHC->SetDataType(4,dtSTRING);
		piSHC->SetDataType(5,dtSTRING);
		piSHC->SetDataType(6,dtTIME);
		piSHC->SetDataType(7,dtTIME);
		piSHC->SetDataType(8,dtINT);
		piSHC->SetDataType(9,dtDEC);
	}
}
void CAirsideFlightFuelBurnReport::InitSummaryListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);
	cxListCtrl.InsertColumn(0,_T("Flight Type"),LVCFMT_LEFT,100) ;
	cxListCtrl.InsertColumn(1,_T("Number Of Aircrafts"),LVCFMT_LEFT,100) ;

	CString fuelUnit ;
	fuelUnit = CARCWeightUnit::GetWeightUnitString(AU_WEIGHT_POUND) ;
	cxListCtrl.InsertColumn(2,_T("Total Fuel(" + fuelUnit + ")"),LVCFMT_LEFT,100) ;

	cxListCtrl.InsertColumn(3,_T("Min Fuel("+ fuelUnit + ")"),LVCFMT_LEFT,100) ;

	cxListCtrl.InsertColumn(4,_T("Avg Fuel(" + fuelUnit + ")"),LVCFMT_LEFT,100) ;

	cxListCtrl.InsertColumn(5,_T("Max Fuel(" + fuelUnit + ")"),LVCFMT_LEFT,100) ;

	cxListCtrl.InsertColumn(6,_T("Q1"),LVCFMT_LEFT,100) ;
	cxListCtrl.InsertColumn(7,_T("Q2"),LVCFMT_LEFT,100) ;
	cxListCtrl.InsertColumn(8,_T("Q3"),LVCFMT_LEFT,100) ;

	cxListCtrl.InsertColumn(9,_T("P1"),LVCFMT_LEFT,100) ;
	cxListCtrl.InsertColumn(10,_T("P5"),LVCFMT_LEFT,100) ;
	cxListCtrl.InsertColumn(11,_T("P10"),LVCFMT_LEFT,100) ;
	cxListCtrl.InsertColumn(12,_T("P90"),LVCFMT_LEFT,100) ;
	cxListCtrl.InsertColumn(13,_T("P95"),LVCFMT_LEFT,100) ;
	cxListCtrl.InsertColumn(14,_T("P99"),LVCFMT_LEFT,100) ;

	if (piSHC)
	{
		piSHC->ResetAll();
		piSHC->SetDataType(0,dtSTRING);
		piSHC->SetDataType(1,dtINT);
		piSHC->SetDataType(2,dtDEC);
		piSHC->SetDataType(3,dtDEC);
		piSHC->SetDataType(4,dtDEC);
		piSHC->SetDataType(5,dtDEC);
		piSHC->SetDataType(6,dtDEC);
		piSHC->SetDataType(7,dtDEC);
		piSHC->SetDataType(8,dtDEC);
		piSHC->SetDataType(9,dtDEC);
		piSHC->SetDataType(10,dtDEC);
		piSHC->SetDataType(11,dtDEC);
		piSHC->SetDataType(12,dtDEC);
		piSHC->SetDataType(13,dtDEC);
		piSHC->SetDataType(14,dtDEC);
	}

}

void CAirsideFlightFuelBurnReport::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter )
{
		if(parameter->getReportType() == ASReportType_Detail)
			FillDetailListContent(cxListCtrl) ;
		else
			FillSummaryListContent(cxListCtrl) ;
}
void CAirsideFlightFuelBurnReport::FillDetailListContent (CXListCtrl& cxListCtrl)
{
	
		for (int i = 0 ; i < (int)m_DetailData.size() ;i++)
		{
			CFlightDetailFuelBurnReportItem detailFuelBurnReport ;
			detailFuelBurnReport = m_DetailData[i] ;
			int ndx = cxListCtrl.InsertItem(0,detailFuelBurnReport.m_Airline) ;
			cxListCtrl.SetItemText(ndx,1,detailFuelBurnReport.m_FlightID) ;
			cxListCtrl.SetItemText(ndx,2,detailFuelBurnReport.m_Actype) ;
			cxListCtrl.SetItemText(ndx,3,detailFuelBurnReport.m_Operate) ;
			cxListCtrl.SetItemText(ndx,4,m_Parameter->GetFromNode().GetNodeName()) ;
			cxListCtrl.SetItemText(ndx,5,m_Parameter->GetToNode().GetNodeName()) ;
			cxListCtrl.SetItemText(ndx,6,detailFuelBurnReport.m_fromTime.printTime()) ;
			cxListCtrl.SetItemText(ndx,7,detailFuelBurnReport.m_toTime.printTime()) ;
			CString min ;
			min.Format(_T("%d"),detailFuelBurnReport.m_Duration.asMinutes()) ;
			cxListCtrl.SetItemText(ndx,8,min) ;

			CString fuelburn ;
			fuelburn.Format(_T("%0.2f"),GetFuelBurnByCurrentUnitWeight(detailFuelBurnReport.m_FuelBurn)) ;
			cxListCtrl.SetItemText(ndx,9,fuelburn) ;
		}
}
void CAirsideFlightFuelBurnReport::FillSummaryListContent(CXListCtrl& cxListCtrl)
{
	for (int i= 0 ; i < (int)m_SummaryData.size() ;i++)
	{
		CFlightSummaryFuelBurnReportItem summaryFuelBurnItem ;
		summaryFuelBurnItem = m_SummaryData[i] ;
		CString strFlt ;
		summaryFuelBurnItem.m_FlightType.screenPrint(strFlt) ;
		int ndx = cxListCtrl.InsertItem(0,strFlt) ;
		strFlt.Format(_T("%d"),summaryFuelBurnItem.m_NumOfAircraft) ;
		cxListCtrl.SetItemText(ndx,1,strFlt) ;
		strFlt.Format(_T("%0.2f"),GetFuelBurnByCurrentUnitWeight(summaryFuelBurnItem.m_TotalFuelBurn)) ;
		cxListCtrl.SetItemText(ndx,2,strFlt) ;

		strFlt.Format(_T("%0.2f"),GetFuelBurnByCurrentUnitWeight(summaryFuelBurnItem.m_minFuelBurn)) ;
		cxListCtrl.SetItemText(ndx,3,strFlt) ;

		strFlt.Format(_T("%0.2f"),GetFuelBurnByCurrentUnitWeight(summaryFuelBurnItem.m_AvgFuelBurn)) ;
		cxListCtrl.SetItemText(ndx,4,strFlt) ;

		strFlt.Format(_T("%0.2f"),GetFuelBurnByCurrentUnitWeight(summaryFuelBurnItem.m_MaxFuelBurn)) ;
		cxListCtrl.SetItemText(ndx,5,strFlt) ;

		strFlt.Format(_T("%0.2f"),GetFuelBurnByCurrentUnitWeight(summaryFuelBurnItem.m_Q1)) ;
		cxListCtrl.SetItemText(ndx,6,strFlt) ;

		strFlt.Format(_T("%0.2f"),GetFuelBurnByCurrentUnitWeight(summaryFuelBurnItem.m_Q2)) ;
		cxListCtrl.SetItemText(ndx,7,strFlt) ;

		strFlt.Format(_T("%0.2f"),GetFuelBurnByCurrentUnitWeight(summaryFuelBurnItem.m_Q3)) ;
		cxListCtrl.SetItemText(ndx,8,strFlt) ;

		strFlt.Format(_T("%0.2f"),GetFuelBurnByCurrentUnitWeight(summaryFuelBurnItem.m_P1)) ;
		cxListCtrl.SetItemText(ndx,9,strFlt) ;

		strFlt.Format(_T("%0.2f"),GetFuelBurnByCurrentUnitWeight(summaryFuelBurnItem.m_P5)) ;
		cxListCtrl.SetItemText(ndx,10,strFlt) ;

		strFlt.Format(_T("%0.2f"),GetFuelBurnByCurrentUnitWeight(summaryFuelBurnItem.m_p10)) ;
		cxListCtrl.SetItemText(ndx,11,strFlt) ;

		strFlt.Format(_T("%0.2f"),GetFuelBurnByCurrentUnitWeight(summaryFuelBurnItem.m_p90)) ;
		cxListCtrl.SetItemText(ndx,12,strFlt) ;

		strFlt.Format(_T("%0.2f"),GetFuelBurnByCurrentUnitWeight(summaryFuelBurnItem.m_P95)) ;
		cxListCtrl.SetItemText(ndx,13,strFlt) ;

		strFlt.Format(_T("%0.2f"),GetFuelBurnByCurrentUnitWeight(summaryFuelBurnItem.m_P99)) ;
		cxListCtrl.SetItemText(ndx,14,strFlt) ;
	}
}
double CAirsideFlightFuelBurnReport::GetFuelBurnByCurrentUnitWeight(double _fuelBurn)
{
	return CARCWeightUnit::ConvertWeight(AU_WEIGHT_POUND,AU_WEIGHT_POUND,_fuelBurn) ;
}
void CAirsideFlightFuelBurnReport::GenerateDetailReport()
{
	CFlightFuelBurnReportItem reportItem ;
	for (int i = 0 ; i < (int)m_ReportItemData.size() ;i++)
	{
		reportItem = m_ReportItemData[i] ;
		if(reportItem.m_FuleBurnDep != 0)
		{
			CFlightDetailFuelBurnReportItem detailFuelBurnReport ;
			detailFuelBurnReport.m_Actype = reportItem.m_Actype ;
			detailFuelBurnReport.m_Airline = reportItem.m_Airline ;
			detailFuelBurnReport.m_FlightID.Format(_T("%s%s"),reportItem.m_Airline,reportItem.m_DepID) ;
			detailFuelBurnReport.m_Duration = reportItem.m_DepToTime - reportItem.m_DepFromTime ;
			detailFuelBurnReport.m_fromTime = reportItem.m_DepFromTime ;
			detailFuelBurnReport.m_toTime = reportItem.m_DepToTime ;
			detailFuelBurnReport.m_FuelBurn = reportItem.m_FuleBurnDep ;
			detailFuelBurnReport.m_Operate = _T("D") ;
			m_DetailData.push_back(detailFuelBurnReport) ;
		}
		if(reportItem.m_FuelBurnArrival != 0)
		{
			CFlightDetailFuelBurnReportItem detailFuelBurnReport ;
			detailFuelBurnReport.m_Actype = reportItem.m_Actype ;
			detailFuelBurnReport.m_Airline = reportItem.m_Airline ;
			detailFuelBurnReport.m_FlightID.Format(_T("%s%s"),reportItem.m_Airline,reportItem.m_ArrivalID) ;
			detailFuelBurnReport.m_Duration = reportItem.m_ArrToTime -  reportItem.m_ArrFromTime;
			detailFuelBurnReport.m_fromTime = reportItem.m_ArrFromTime ;
			detailFuelBurnReport.m_toTime = reportItem.m_ArrToTime ;
			detailFuelBurnReport.m_FuelBurn = reportItem.m_FuelBurnArrival ;
			detailFuelBurnReport.m_Operate = _T("A") ;
			m_DetailData.push_back(detailFuelBurnReport) ;
		}
	}
}

void CAirsideFlightFuelBurnReport::GenerateSummaryReport()
{
	CDataSummary* Pdata = NULL ;
		for (int ndx = 0 ; ndx < (int)m_Parameter->getFlightConstraintCount() ;ndx++)
		{
			 Pdata = new CDataSummary;
			Pdata->m_FltType = m_Parameter->getFlightConstraint(ndx) ;
			m_SummaryMiddleData.push_back(Pdata) ;
		}

		for (int i = 0 ; i < (int)m_ReportItemData.size() ;i++)
		{
			CFlightFuelBurnReportItem FuelBurn = m_ReportItemData[i] ;

			for (int fltnum = 0 ; fltnum < (int)m_SummaryMiddleData.size() ;fltnum++)
			{
				Pdata = m_SummaryMiddleData[fltnum] ;
				if(Pdata->m_FltType.fits(FuelBurn.m_fltDesc))
					Pdata->m_Data.push_back(FuelBurn) ;			
			}
		}
		for (int count = 0 ; count < (int)m_SummaryMiddleData.size() ;count++)
		{
			CalculateSummaryData(m_SummaryMiddleData[count]) ;
		}

}
void CAirsideFlightFuelBurnReport::CalculateSummaryData(CDataSummary* PDataSummary)
{
	if(PDataSummary == NULL)
		return ;
	CStatisticalTools<double> FuelBurnTool ;
	double maxFuel = 0 ;
	double minFuel = 0xFFFFFFFF ;
	double avgFuel = 0 ;
	double totalfuelBurn = 0 ;
	for (int ndx = 0 ; ndx < (int)PDataSummary->m_Data.size() ;ndx++)
	{
		double fuelBurn = PDataSummary->m_Data[ndx].m_FuleBurnDep + PDataSummary->m_Data[ndx].m_FuelBurnArrival ;

		maxFuel = maxFuel > fuelBurn ? maxFuel:fuelBurn ;
		minFuel = minFuel < fuelBurn ? minFuel : fuelBurn ;
		FuelBurnTool.AddNewData(fuelBurn) ;
		totalfuelBurn += fuelBurn ;
	}
	CFlightSummaryFuelBurnReportItem summaryFuelBurnItem ;
	summaryFuelBurnItem.m_FlightType = PDataSummary->m_FltType ;
	summaryFuelBurnItem.m_MaxFuelBurn = maxFuel ;
	summaryFuelBurnItem.m_minFuelBurn = minFuel ;
	summaryFuelBurnItem.m_TotalFuelBurn = totalfuelBurn ;
	summaryFuelBurnItem.m_AvgFuelBurn = totalfuelBurn / (int)PDataSummary->m_Data.size() ;
	summaryFuelBurnItem.m_NumOfAircraft = (int)PDataSummary->m_Data.size() ;
	summaryFuelBurnItem.m_P1 = FuelBurnTool.GetPercentile(1) ;
	summaryFuelBurnItem.m_P5 = FuelBurnTool.GetPercentile(5) ;
	summaryFuelBurnItem.m_p10 = FuelBurnTool.GetPercentile(10) ;
	summaryFuelBurnItem.m_p90 = FuelBurnTool.GetPercentile(90) ;
	summaryFuelBurnItem.m_P95 = FuelBurnTool.GetPercentile(95) ;
	summaryFuelBurnItem.m_P99 = FuelBurnTool.GetPercentile(99) ;
	summaryFuelBurnItem.m_Q1 = FuelBurnTool.GetPercentile(25) ;
	summaryFuelBurnItem.m_Q2 = FuelBurnTool.GetPercentile(50) ;
	summaryFuelBurnItem.m_Q3 = FuelBurnTool.GetPercentile(75) ;
	m_SummaryData.push_back(summaryFuelBurnItem) ;
}

void CAirsideFlightFuelBurnReport::RefreshReport( CParameters * parameter )
{
	if(m_BaseResult)
		delete m_BaseResult ;
	m_BaseResult = NULL ;
	if(parameter->getReportType() == ASReportType_Detail)
		m_BaseResult = new CAirsideFlightFuelBurnDetailReportResult(m_ReportItemData,m_Parameter) ;
	else
		m_BaseResult = new CAirsideFlightFuelBurnSummaryReportResult(m_SummaryData,m_Parameter) ;
	m_BaseResult->GenerateResult() ;
}

void CAirsideFlightFuelBurnReport::ClearAllData()
{
	m_ReportItemData.clear() ;
	m_DetailData.clear();
	m_SummaryData.clear() ;
	for (int i = 0 ; i < (int)m_SummaryMiddleData.size() ;i++)
	{
		delete m_SummaryMiddleData[i] ;
	}
	m_SummaryMiddleData.clear() ;
}

BOOL CAirsideFlightFuelBurnReport::ExportReportData( ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType /*= ASReportType_Detail*/ )
{
	int count = 0 ;
	count = (int) m_ReportItemData.size() ;
	_file.writeInt(count) ;
	_file.writeLine() ;
	for (int i = 0 ; i < count ;i++)
	{
		m_ReportItemData[i].ExportFile(_file) ;
	}
	return TRUE ;
}

BOOL CAirsideFlightFuelBurnReport::ImportReportData( ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType /*= ASReportType_Detail*/ )
{

	int count = 0 ;
	if(!_file.getInteger(count))
	{
		Errmsg.Format(_T("Read report Data Item count error")) ;
		return FALSE ;
	}
	ClearAllData();
	_file.getLine() ;
	for (int i = 0 ; i < count ;i++)
	{
		CFlightFuelBurnReportItem  item ;
		item.ImportFile(_file) ;
		m_ReportItemData.push_back(item) ;
	}
	if(reportType == ASReportType_Detail)
		GenerateDetailReport() ;
	else
	{
		m_SummaryData.clear() ;
		for (int i = 0 ; i < (int)m_SummaryMiddleData.size() ;i++)
		{
			delete m_SummaryMiddleData[i] ;
		}
		m_SummaryMiddleData.clear() ;
		GenerateSummaryReport() ;
	}

	return TRUE ;
}

CString CAirsideFlightFuelBurnReport::GetReportFileName()
{
	return "FlightFuel\\AirsideFlightFuelBurnReport.rep" ;
}

BOOL CAirsideFlightFuelBurnReport::WriteReportData( ArctermFile& _file )
{
	_file.writeField("Fuel burn Report");
	_file.writeLine();
	_file.writeField("Paramerate");
	_file.writeLine();
	m_Parameter->ExportFile(_file) ;
	_file.writeInt(m_Parameter->getReportType()) ;
	_file.writeLine() ;
	return ExportReportData(_file,CString()) ;
}

BOOL CAirsideFlightFuelBurnReport::ReadReportData( ArctermFile& _file )
{
	_file.getLine() ;
	m_Parameter->ImportFile(_file) ;
	_file.getLine() ;
	int reportType ;
	_file.getInteger(reportType) ;
	_file.getLine() ;
	return ImportReportData(_file,CString(),(enumASReportType_Detail_Summary)reportType) ;
}