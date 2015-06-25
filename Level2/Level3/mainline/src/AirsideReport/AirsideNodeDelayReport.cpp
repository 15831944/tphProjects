#include "StdAfx.h"
#include ".\airsidenodedelayreport.h"
#include "AirsideNodeDelayReportParameter.h"
#include "AirsideFlightDelayReport.h"
#include "DetailNodeDelaydResult.h"
#include "SummaryNodeDelayResult.h"
#include "CommonMethod.h"

CAirsideNodeDelayReport::CAirsideNodeDelayReport(CBGetLogFilePath pFunc)
:CAirsideBaseReport(pFunc)
, m_pNodeDelayResult(NULL)
{
}

CAirsideNodeDelayReport::~CAirsideNodeDelayReport(void)
{
	if (NULL != m_pNodeDelayResult)
	{
		delete m_pNodeDelayResult;
		m_pNodeDelayResult = NULL;
	}
}

void CAirsideNodeDelayReport::GenerateReport(CParameters * parameter)
{
	m_vResult.clear();

	CAirsideNodeDelayReportParameter *pParam = (CAirsideNodeDelayReportParameter*)parameter;
	ASSERT(pParam != NULL);

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

	for (int i=0; i<(int)pParam->getReportNodeCount(); i++)
	{
		nodeDelay reportNodeDelay(m_AirportDB);
		reportNodeDelay.node = pParam->getReportNode(i);
		reportNodeDelay.delayTime = 0;

		m_vResult.push_back(reportNodeDelay);
	}

	int nFltLogCount = fltLog.getCount();
	for (int i =0; i < nFltLogCount; ++i)
	{
		AirsideFlightLogEntry logEntry;
		logEntry.SetEventLog(&airsideFlightEventLog);
		fltLog.getItem(logEntry,i);	
		HandleFlightLog(logEntry,pParam);
	}
}

void CAirsideNodeDelayReport::RefreshReport(CParameters * parameter)
{
	CAirsideNodeDelayReportParameter* pParam = (CAirsideNodeDelayReportParameter*)parameter;
	ASSERT(pParam != NULL);

	if (NULL != m_pNodeDelayResult)
	{
		delete m_pNodeDelayResult;
		m_pNodeDelayResult = NULL;
	}

	switch(pParam->getSubType())
	{
	case CAirsideFlightDelayReport::SRT_DETAIL_NODEDELAY:
		{
			m_pNodeDelayResult = new CDetailNodeDelaydResult;
			CDetailNodeDelaydResult* pResult = (CDetailNodeDelaydResult*)m_pNodeDelayResult;
			pResult->GenerateResult(m_vResult,parameter);
		}
		break;

	case CAirsideFlightDelayReport::SRT_SUMMARY_NODEDELAY:
		{
			m_pNodeDelayResult   = new CSummaryNodeDelayResult;
			CSummaryNodeDelayResult* pResult = (CSummaryNodeDelayResult*)m_pNodeDelayResult;
			pResult->GenerateResult(m_vResult,parameter);
		}
		break;

	default:
		break;
	}
}

void CAirsideNodeDelayReport::HandleFlightLog(AirsideFlightLogEntry& logEntry,CAirsideNodeDelayReportParameter * parameter)
{
	ASSERT(parameter);

	AirsideFlightDescStruct	fltDesc = logEntry.GetAirsideDescStruct();

	if (IsValidTime(fltDesc,parameter) == false)
		return;

	FlightConstraint flightConstraint;
	if (IsFitConstraint(fltDesc,parameter, flightConstraint) == false)
		return;

	long lDelayTime = 0;

	nodeDelayItem nodeDelayItemObj(m_AirportDB);
	nodeDelayItemObj.fltDesc = logEntry.GetAirsideDesc();
	nodeDelayItemObj.m_fltCons = flightConstraint;

	for (int nNodeIndex=0; nNodeIndex<(int)parameter->getReportNodeCount(); nNodeIndex++)
	{
		CAirsideReportNode airsideReportNode = parameter->getReportNode(nNodeIndex);

		vector<nodeDelayItem> vNodeDelayItem;

		int nLastEventIndex = 0;
		bool bNeedPushback = false;
		bool bFirstItem = true;

		int nLastArrivalOrDept = 0;  //0:arrival 1:dept
		for (int i=0; i<logEntry.getCount(); i++)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(i);

			//node not equal
			if (!IsNodeEqual(event, airsideReportNode))
			{
				continue;
			}

			//arrival
			if (event.mode < OnHeldAtStand)
			{
				if ((bNeedPushback && (nLastArrivalOrDept!=0))                 //change arrival or dept
					|| (bNeedPushback && (nLastEventIndex != (i-1))))          //not sequential
				{
					//save prior item
					vNodeDelayItem.push_back(nodeDelayItemObj);
					bNeedPushback = false;
					bFirstItem = true;
				}
				nodeDelayItemObj.fltDesc = logEntry.GetAirsideDesc();
				ZeroMemory(nodeDelayItemObj.fltDesc.depID, sizeof(nodeDelayItemObj.fltDesc.depID));
				nLastArrivalOrDept = 0;
			}
			//deptarture
			else
			{
				if ((bNeedPushback && (nLastArrivalOrDept != 1))          //change arrival or dept
					|| (bNeedPushback && (nLastEventIndex != (i-1))))     //not sequential
				{
					//save prior item
					vNodeDelayItem.push_back(nodeDelayItemObj);
					bNeedPushback = false;
					bFirstItem = true;
				}
				nodeDelayItemObj.fltDesc = logEntry.GetAirsideDesc();
				ZeroMemory(nodeDelayItemObj.fltDesc.arrID, sizeof(nodeDelayItemObj.fltDesc.arrID));
				nLastArrivalOrDept = 1;
			}

			//first item
			if (bFirstItem)
			{
				bNeedPushback = true;
				bFirstItem    = false;
				//set new item
				nodeDelayItemObj.delayTime = event.delayTime;
				nodeDelayItemObj.lEventTime = event.time;
				nodeDelayItemObj.lPlanArrivalTime = nodeDelayItemObj.lEventTime - nodeDelayItemObj.delayTime;
			}
			//consequential event
			else if (nLastEventIndex == i-1)
			{
				bNeedPushback = true;

				nodeDelayItemObj.delayTime += event.delayTime;
				if (nodeDelayItemObj.lEventTime < event.time)
				{
					nodeDelayItemObj.lEventTime = event.time;
				}				
				nodeDelayItemObj.lPlanArrivalTime = nodeDelayItemObj.lEventTime - nodeDelayItemObj.delayTime;
			}
			//next item
			else
			{
				bNeedPushback = true;

				//save prior item
				vNodeDelayItem.push_back(nodeDelayItemObj);

				//set new item
				nodeDelayItemObj.delayTime = event.delayTime;
				nodeDelayItemObj.lEventTime = event.time;
				nodeDelayItemObj.lPlanArrivalTime = nodeDelayItemObj.lEventTime - nodeDelayItemObj.delayTime;
			}

			//set last event index
			nLastEventIndex = i;
		}

		if (bNeedPushback)
		{
			vNodeDelayItem.push_back(nodeDelayItemObj);
		}

		//add to result
		for (int j=0; j<(int)m_vResult.size(); j++)
		{
			nodeDelay reportNodeDelay = m_vResult[j];

			if (reportNodeDelay.node.EqualTo( airsideReportNode))
			{
				for (int m=0; m<(int)vNodeDelayItem.size(); m++)
				{
					m_vResult[j].delayTime += vNodeDelayItem[m].delayTime;
					m_vResult[j].vDelayItem.push_back(vNodeDelayItem[m]);
				}
			}
		}
	}
}

//if node is equal return true, haven't selected return false
bool CAirsideNodeDelayReport::IsNodeEqual(AirsideFlightEventStruct& event, CAirsideReportNode& airsideReportNode)
{
	if(airsideReportNode.GetNodeType() == CAirsideReportNode::ReportNodeType_All)
		return true;

	if (airsideReportNode.GetNodeType() == CAirsideReportNode::ReportNodeType_WayPointStand
		&& airsideReportNode.IncludeObj(event.nodeNO))
	{
		return true;
	}
	else if (airsideReportNode.GetNodeType() == CAirsideReportNode::ReportNodeType_Intersection
		&& airsideReportNode.IncludeObj(event.intNodeID))
	{
		return true;
	}
	else if(airsideReportNode.GetNodeType() == CAirsideReportNode::ReportNodeType_Taxiway
		&& airsideReportNode.IncludeObj(event.intNodeID))
	{
		return true;
	}

	return false;
}

bool CAirsideNodeDelayReport::IsValidTime(AirsideFlightDescStruct& fltDesc,CAirsideNodeDelayReportParameter * parameter)
{
	//time valid
	if (fltDesc.startTime == 0 && fltDesc.endTime == 0)
		return false;

	if (fltDesc.endTime < parameter->getStartTime().asSeconds()*100 || fltDesc.startTime> parameter->getEndTime().asSeconds()*100)//not in the time range
		return false;

	return true;
}

bool CAirsideNodeDelayReport::IsFitConstraint(AirsideFlightDescStruct& fltDesc,CAirsideNodeDelayReportParameter * parameter, FlightConstraint& fltConstraint)
{
	size_t nFltConsCount = parameter->getFlightConstraintCount();
	for (size_t nfltCons =0; nfltCons < nFltConsCount; ++ nfltCons)
	{
		FlightConstraint fltCst = parameter->getFlightConstraint(nfltCons);

		if (fltCst.fits(fltDesc))
		{
			fltConstraint = fltCst;
			return true;
		}
	}

	return false;
}

void CAirsideNodeDelayReport::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	switch(reportType)
	{
	case ASReportType_Detail:
		{
			cxListCtrl.InsertColumn(0, _T("Flight"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(1, _T("Flight Type"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(2, _T("Operation"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(3, _T("Node"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(4, _T("T Node(plan)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(5, _T("T Node(act)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(6, _T("Node Delay(mins)"), LVCFMT_LEFT, 100);

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
			}

		}
		break;

	case ASReportType_Summary:
		{
			cxListCtrl.InsertColumn(0, _T("Node"), LVCFMT_LEFT, 100);

			//Node delay
			cxListCtrl.InsertColumn(1, _T("Total Delay(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(2, _T("Min Delay(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(3, _T("Max Delay(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(4, _T("Mean Delay(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(5, _T("Q1(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(6, _T("Q2(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(7, _T("Q3(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(8, _T("P1(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(9, _T("P5(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(10, _T("P10(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(11, _T("P90(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(12, _T("P95(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(13, _T("P99(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(14, _T("Std dev(mins)"), LVCFMT_LEFT, 100);

			if (piSHC)
			{
				piSHC->ResetAll();
				piSHC->SetDataType(0,dtSTRING);
				piSHC->SetDataType(1,dtTIME);
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
			}

		}
		break;

	default:
		ASSERT(false);
		break;
	}
}

void CAirsideNodeDelayReport::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	ASSERT(parameter);
	CAirsideNodeDelayReportParameter* pAirsideNodeDelayReportParameter = (CAirsideNodeDelayReportParameter*)parameter;
	ASSERT(pAirsideNodeDelayReportParameter);

	switch(parameter->getReportType())
	{
	case ASReportType_Detail:
		FillDetailListContent(cxListCtrl, parameter);
		break;
	case ASReportType_Summary:
		FillSummaryListContent(cxListCtrl, parameter);
	    break;
	default:
		ASSERT(false);
	    break;
	}
}

BOOL CAirsideNodeDelayReport::ExportListData(ArctermFile& _file,CParameters * parameter) 
{
	if(parameter == NULL)
		return FALSE ;
	CAirsideNodeDelayReportParameter* pAirsideNodeDelayReportParameter = (CAirsideNodeDelayReportParameter*)parameter;
	ASSERT(pAirsideNodeDelayReportParameter);

	switch(parameter->getReportType())
	{
	case ASReportType_Detail:
		ExportListDetail(_file, parameter);
		break;
	case ASReportType_Summary:
		ExportListSummary(_file, parameter);
		break;
	default:
		ASSERT(false);
		break;
	}
	return TRUE ;
}
void CAirsideNodeDelayReport::ExportListSummary(ArctermFile& _file, CParameters * parameter)
{
	ASSERT(parameter);

	CSummaryNodeDelayResult       summaryNodeDelayResult;

	summaryNodeDelayResult.GenerateResult(m_vResult, parameter);
	vector<CSummaryNodeDelayDataItem>  summaryNodeDelayData;
	summaryNodeDelayResult.GetSummaryResult(summaryNodeDelayData);

	for (int i=0; i<(int)summaryNodeDelayData.size(); i++)
	{
		//Node
		_file.writeField( CommonMethod::GetObjectName(summaryNodeDelayData[i].m_nNodeType, summaryNodeDelayData[i].m_nNodeID));

		//total delay
		_file.writeField( summaryNodeDelayData[i].m_summaryData.m_estTotal.printTime());
		//Min Delay(mins)
		_file.writeField( summaryNodeDelayData[i].m_summaryData.m_estMin.printTime());
		//Max Delay(mins)
		_file.writeField( summaryNodeDelayData[i].m_summaryData.m_estMax.printTime());
		//Mean Delay(mins)
		_file.writeField( summaryNodeDelayData[i].m_summaryData.m_estAverage.printTime());
		//Q1(mins)
		_file.writeField( summaryNodeDelayData[i].m_summaryData.m_estQ1.printTime());
		//Q2(mins)
		_file.writeField( summaryNodeDelayData[i].m_summaryData.m_estQ2.printTime());
		//Q3(mins)
		_file.writeField( summaryNodeDelayData[i].m_summaryData.m_estQ3.printTime());
		//P1(mins)
		_file.writeField( summaryNodeDelayData[i].m_summaryData.m_estP1.printTime());
		//P5(mins)
		_file.writeField( summaryNodeDelayData[i].m_summaryData.m_estP5.printTime());
		//P10(mins)
		_file.writeField( summaryNodeDelayData[i].m_summaryData.m_estP10.printTime());
		//P90(mins)
		_file.writeField( summaryNodeDelayData[i].m_summaryData.m_estP90.printTime());
		//P95(mins)
		_file.writeField( summaryNodeDelayData[i].m_summaryData.m_estP95.printTime());
		//P99(mins)
		_file.writeField( summaryNodeDelayData[i].m_summaryData.m_estP99.printTime());
		//Std dev(mins)
		_file.writeField( summaryNodeDelayData[i].m_summaryData.m_estSigma.printTime());
		_file.writeLine() ;
	}
}
void CAirsideNodeDelayReport::ExportListDetail(ArctermFile& _file, CParameters * parameter)
{
	ASSERT(parameter);
	CAirsideNodeDelayReportParameter* pAirsideNodeDelayReportParameter = (CAirsideNodeDelayReportParameter*)parameter;
	ASSERT(pAirsideNodeDelayReportParameter);

	int nRowCount = 0;
	for (size_t i =0; i < m_vResult.size(); ++i )
	{
		for (size_t j=0; j<m_vResult[i].vDelayItem.size(); j++)
		{
			//arrival
			if (strlen(m_vResult[i].vDelayItem[j].fltDesc.arrID) > 0)
			{
				//flight
				CString strFlight(_T(""));
				strFlight.Format("%s%s", m_vResult[i].vDelayItem[j].fltDesc._airline.GetValue(), m_vResult[i].vDelayItem[j].fltDesc.arrID);
				_file.writeField(strFlight);

				//flight type
				CString strFlightType;
				m_vResult[i].vDelayItem[j].m_fltCons.screenPrint(strFlightType.GetBuffer(1024));
				_file.writeField(strFlightType);

				//Operation
				_file.writeField( _T("A"));

				//Node
				CString strNode(_T("Node"));
				//strNode.Format(_T("%s"), CommonMethod::GetObjectName((int)m_vResult[i].node.GetNodeType(), m_vResult[i].node.GetNodeID()));
				strNode.Format(_T("%s"), m_vResult[i].node.GetNodeName());
				_file.writeField(strNode);

				//Node Plan time
				ElapsedTime estNodePlan(long(m_vResult[i].vDelayItem[j].lPlanArrivalTime/100.0+0.5));
				CString strNodePlan(_T(""));
				strNodePlan.Format(_T("Day%d %02d:%02d:%02d"), estNodePlan.GetDay(), estNodePlan.GetHour(), estNodePlan.GetMinute(), estNodePlan.GetSecond());
				_file.writeField(strNodePlan);

				//Node act time
				ElapsedTime estNodeAct(long(m_vResult[i].vDelayItem[j].lEventTime/100.0+0.5));
				CString strNodeAct(_T(""));
				strNodeAct.Format(_T("Day%d %02d:%02d:%02d"), estNodeAct.GetDay(), estNodeAct.GetHour(), estNodeAct.GetMinute(), estNodeAct.GetSecond());
				_file.writeField( strNodeAct);

				//total delays
				ElapsedTime estFlightDelayInNode(long(m_vResult[i].vDelayItem[j].delayTime/100.0+0.5));
				_file.writeField( estFlightDelayInNode.printTime());
                _file.writeLine() ;
				nRowCount++;
			}

			//depature
			if (strlen(m_vResult[i].vDelayItem[j].fltDesc.depID) > 0)
			{
				//flight
				CString strFlight(_T(""));
				strFlight.Format("%s%s", m_vResult[i].vDelayItem[j].fltDesc._airline.GetValue(), m_vResult[i].vDelayItem[j].fltDesc.depID);
				_file.writeField(strFlight);

				//flight type
				CString strFlightType;
				m_vResult[i].vDelayItem[j].m_fltCons.screenPrint(strFlightType.GetBuffer(1024));
				_file.writeField(strFlightType);

				//Operation
				_file.writeField(_T("D"));

				//Node
				CString strNode(_T("Node"));
				//strNode.Format(_T("%s"), CommonMethod::GetObjectName((int)m_vResult[i].node.GetNodeType(), m_vResult[i].node.GetNodeID()));
				strNode.Format(_T("%s"), m_vResult[i].node.GetNodeName());
				_file.writeField( strNode);

				//Node Plan time
				ElapsedTime estNodePlan(long(m_vResult[i].vDelayItem[j].lPlanArrivalTime/100.0+0.5));
				CString strNodePlan(_T(""));
				strNodePlan.Format(_T("Day%d %02d:%02d:%02d"), estNodePlan.GetDay(), estNodePlan.GetHour(), estNodePlan.GetMinute(), estNodePlan.GetSecond());
				_file.writeField( strNodePlan);

				//Node act time
				ElapsedTime estNodeAct(long(m_vResult[i].vDelayItem[j].lEventTime/100.0+0.5));
				CString strNodeAct(_T(""));
				strNodeAct.Format(_T("Day%d %02d:%02d:%02d"), estNodeAct.GetDay(), estNodeAct.GetHour(), estNodeAct.GetMinute(), estNodeAct.GetSecond());
				_file.writeField(strNodeAct);

				//total delays
				ElapsedTime estFlightDelayInNode(long(m_vResult[i].vDelayItem[j].delayTime/100.0+0.5));
				_file.writeField(estFlightDelayInNode.printTime());
                _file.writeLine() ;
				nRowCount++;
			}
		}
	}
}
void CAirsideNodeDelayReport::FillDetailListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	ASSERT(parameter);
	CAirsideNodeDelayReportParameter* pAirsideNodeDelayReportParameter = (CAirsideNodeDelayReportParameter*)parameter;
	ASSERT(pAirsideNodeDelayReportParameter);

	int nRowCount = 0;
	for (size_t i =0; i < m_vResult.size(); ++i )
	{
		for (size_t j=0; j<m_vResult[i].vDelayItem.size(); j++)
		{
			//arrival
			if (strlen(m_vResult[i].vDelayItem[j].fltDesc.arrID) > 0)
			{
				//flight
				CString strFlight(_T(""));
				strFlight.Format("%s%s", m_vResult[i].vDelayItem[j].fltDesc._airline.GetValue(), m_vResult[i].vDelayItem[j].fltDesc.arrID);
				cxListCtrl.InsertItem(nRowCount, strFlight);

				//flight type
				CString strFlightType;
				m_vResult[i].vDelayItem[j].m_fltCons.screenPrint(strFlightType.GetBuffer(1024));
				cxListCtrl.SetItemText(nRowCount, 1,strFlightType);

				//Operation
				cxListCtrl.SetItemText(nRowCount, 2, _T("A"));

				//Node
				CString strNode(_T("Node"));
				//strNode.Format(_T("%s"), CommonMethod::GetObjectName((int)m_vResult[i].node.GetNodeType(), m_vResult[i].node.GetNodeID()));
				strNode.Format(_T("%s"), m_vResult[i].node.GetNodeName());
				cxListCtrl.SetItemText(nRowCount, 3, strNode);

				//Node Plan time
				ElapsedTime estNodePlan(long(m_vResult[i].vDelayItem[j].lPlanArrivalTime/100.0+0.5));
				CString strNodePlan(_T(""));
				strNodePlan.Format(_T("Day%d %02d:%02d:%02d"), estNodePlan.GetDay(), estNodePlan.GetHour(), estNodePlan.GetMinute(), estNodePlan.GetSecond());
				cxListCtrl.SetItemText(nRowCount, 4, strNodePlan);

				//Node act time
				ElapsedTime estNodeAct(long(m_vResult[i].vDelayItem[j].lEventTime/100.0+0.5));
				CString strNodeAct(_T(""));
				strNodeAct.Format(_T("Day%d %02d:%02d:%02d"), estNodeAct.GetDay(), estNodeAct.GetHour(), estNodeAct.GetMinute(), estNodeAct.GetSecond());
				cxListCtrl.SetItemText(nRowCount, 5, strNodeAct);

				//total delays
				ElapsedTime estFlightDelayInNode(long(m_vResult[i].vDelayItem[j].delayTime/100.0+0.5));
				cxListCtrl.SetItemText(nRowCount, 6, estFlightDelayInNode.printTime());

				nRowCount++;
			}

			//depature
			if (strlen(m_vResult[i].vDelayItem[j].fltDesc.depID) > 0)
			{
				//flight
				CString strFlight(_T(""));
				strFlight.Format("%s%s", m_vResult[i].vDelayItem[j].fltDesc._airline.GetValue(), m_vResult[i].vDelayItem[j].fltDesc.depID);
				cxListCtrl.InsertItem(nRowCount, strFlight);

				//flight type
				CString strFlightType;
				m_vResult[i].vDelayItem[j].m_fltCons.screenPrint(strFlightType.GetBuffer(1024));
				cxListCtrl.SetItemText(nRowCount, 1,strFlightType);

				//Operation
				cxListCtrl.SetItemText(nRowCount, 2, _T("D"));

				//Node
				CString strNode(_T("Node"));
				//strNode.Format(_T("%s"), CommonMethod::GetObjectName((int)m_vResult[i].node.GetNodeType(), m_vResult[i].node.GetNodeID()));
				strNode.Format(_T("%s"), m_vResult[i].node.GetNodeName());
				cxListCtrl.SetItemText(nRowCount, 3, strNode);

				//Node Plan time
				ElapsedTime estNodePlan(long(m_vResult[i].vDelayItem[j].lPlanArrivalTime/100.0+0.5));
				CString strNodePlan(_T(""));
				strNodePlan.Format(_T("Day%d %02d:%02d:%02d"), estNodePlan.GetDay(), estNodePlan.GetHour(), estNodePlan.GetMinute(), estNodePlan.GetSecond());
				cxListCtrl.SetItemText(nRowCount, 4, strNodePlan);

				//Node act time
				ElapsedTime estNodeAct(long(m_vResult[i].vDelayItem[j].lEventTime/100.0+0.5));
				CString strNodeAct(_T(""));
				strNodeAct.Format(_T("Day%d %02d:%02d:%02d"), estNodeAct.GetDay(), estNodeAct.GetHour(), estNodeAct.GetMinute(), estNodeAct.GetSecond());
				cxListCtrl.SetItemText(nRowCount, 5, strNodeAct);

				//total delays
				ElapsedTime estFlightDelayInNode(long(m_vResult[i].vDelayItem[j].delayTime/100.0+0.5));
				cxListCtrl.SetItemText(nRowCount, 6, estFlightDelayInNode.printTime());

				nRowCount++;
			}
		}
	}
}

void CAirsideNodeDelayReport::FillSummaryListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	ASSERT(parameter);

	CSummaryNodeDelayResult       summaryNodeDelayResult;

	summaryNodeDelayResult.GenerateResult(m_vResult, parameter);
	vector<CSummaryNodeDelayDataItem>  summaryNodeDelayData;
	summaryNodeDelayResult.GetSummaryResult(summaryNodeDelayData);

	for (int i=0; i<(int)summaryNodeDelayData.size(); i++)
	{
		//Node
		cxListCtrl.InsertItem(i, CommonMethod::GetObjectName(summaryNodeDelayData[i].m_nNodeType, summaryNodeDelayData[i].m_nNodeID));

		//total delay
		cxListCtrl.SetItemText(i, 1,summaryNodeDelayData[i].m_summaryData.m_estTotal.printTime());
		//Min Delay(mins)
		cxListCtrl.SetItemText(i, 2,summaryNodeDelayData[i].m_summaryData.m_estMin.printTime());
		//Max Delay(mins)
		cxListCtrl.SetItemText(i, 3,summaryNodeDelayData[i].m_summaryData.m_estMax.printTime());
		//Mean Delay(mins)
		cxListCtrl.SetItemText(i, 4,summaryNodeDelayData[i].m_summaryData.m_estAverage.printTime());
		//Q1(mins)
		cxListCtrl.SetItemText(i, 5,summaryNodeDelayData[i].m_summaryData.m_estQ1.printTime());
		//Q2(mins)
		cxListCtrl.SetItemText(i, 6,summaryNodeDelayData[i].m_summaryData.m_estQ2.printTime());
		//Q3(mins)
		cxListCtrl.SetItemText(i, 7,summaryNodeDelayData[i].m_summaryData.m_estQ3.printTime());
		//P1(mins)
		cxListCtrl.SetItemText(i, 8,summaryNodeDelayData[i].m_summaryData.m_estP1.printTime());
		//P5(mins)
		cxListCtrl.SetItemText(i, 9,summaryNodeDelayData[i].m_summaryData.m_estP5.printTime());
		//P10(mins)
		cxListCtrl.SetItemText(i, 10,summaryNodeDelayData[i].m_summaryData.m_estP10.printTime());
		//P90(mins)
		cxListCtrl.SetItemText(i, 11,summaryNodeDelayData[i].m_summaryData.m_estP90.printTime());
		//P95(mins)
		cxListCtrl.SetItemText(i, 12,summaryNodeDelayData[i].m_summaryData.m_estP95.printTime());
		//P99(mins)
		cxListCtrl.SetItemText(i, 13,summaryNodeDelayData[i].m_summaryData.m_estP99.printTime());
		//Std dev(mins)
		cxListCtrl.SetItemText(i, 14,summaryNodeDelayData[i].m_summaryData.m_estSigma.printTime());
	}
}

BOOL CAirsideNodeDelayReport::ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType )
{
   int size = (int)m_vResult.size() ;
   _file.writeInt(size) ;
   _file.writeLine() ;
   for (int i = 0 ; i < size ;i++)
   {
		if(!m_vResult[i].ExportFile(_file))
			return FALSE ;
   }
   return TRUE ;
}
BOOL CAirsideNodeDelayReport::ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType )
{
   int size = 0  ;
   if(!_file.getInteger(size))
	   return FALSE ;
   for (int i = 0 ; i < size ;i++)
   {
		_file.getLine() ;
		nodeDelay _nodedelay(m_AirportDB) ;
		if(!_nodedelay.ImportFile(_file))
			return FALSE ;
		m_vResult.push_back(_nodedelay) ;
   }
   return TRUE ;
}
BOOL CAirsideNodeDelayReport::nodeDelay::ExportFile(ArctermFile& _file)
{
	_file.writeInt(delayTime) ;
	_file.writeLine() ;
	node.ExportFile(_file) ;
    int size = (int)vDelayItem.size() ;
    _file.writeInt(size) ;
	_file.writeLine() ;
	for (int i = 0 ; i < size ;i++)
	{
      if(!vDelayItem[i].ExportFile(_file))
		  return FALSE ;
	} 
	return TRUE ;
}
BOOL CAirsideNodeDelayReport::nodeDelay::ImportFile(ArctermFile& _file)
{
    _file.getInteger(delayTime) ;
	_file.getLine() ;
	node.ImportFile(_file) ;
	_file.getLine() ;

	int size = 0 ;
	if(!_file.getInteger(size))
		return FALSE ;
	for (int i = 0 ; i < size ;i++)
	{
		_file.getLine() ;
		nodeDelayItem delayitem(m_AirportDB) ;
		delayitem.ImportFile(_file) ;
		vDelayItem.push_back(delayitem) ;
	}
	return TRUE ;
}
BOOL CAirsideNodeDelayReport::nodeDelayItem::ExportFile(ArctermFile& _file)
{
	_file.writeInt(delayTime) ;
	_file.writeInt(lEventTime) ;
	_file.writeInt(lPlanArrivalTime) ;
	TCHAR th[1024] = {0} ;
	m_fltCons.WriteSyntaxStringWithVersion(th) ;
	_file.writeField(th) ;
    _file.writeLine() ;

	fltDesc.ExportFile(_file) ;
	return TRUE ;
}
BOOL CAirsideNodeDelayReport::nodeDelayItem::ImportFile(ArctermFile& _file)
{
	_file.getInteger(delayTime) ;
	_file.getInteger(lEventTime) ;
	_file.getInteger(lPlanArrivalTime) ;
		TCHAR th[1024] = {0} ;
	_file.getField(th,1024) ;
	m_fltCons.setConstraintWithVersion(th) ;

	_file.getLine() ;

	fltDesc.ImportFile(_file) ;
	return TRUE ;
}