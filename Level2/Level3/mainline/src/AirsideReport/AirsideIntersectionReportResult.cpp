#include "StdAfx.h"
#include ".\airsideintersectionreportresult.h"
#include "Results\AirsideReportLog.h"
#include "AirsideIntersectionReportParam.h"
#include "Results\AirsideFlightNodeLog.h"
#include "AirsideIntersectionSummaryChartResult.h"
#include "ArisdieIntersectionDetailChartResult.h"
#include "Common/ProgressBar.h"


CAirsideIntersectionReportBaseResult::CAirsideIntersectionReportBaseResult(void)
{
}

CAirsideIntersectionReportBaseResult::~CAirsideIntersectionReportBaseResult(void)
{
}
void CAirsideIntersectionReportBaseResult::setLoadFromFile( bool bLoad )
{
	m_bLoadFromFile = bLoad;
}

bool CAirsideIntersectionReportBaseResult::IsLoadFromFile()
{
	return m_bLoadFromFile;

}

void CAirsideIntersectionReportBaseResult::SetCBGetFilePath( CBGetLogFilePath pFunc )
{
	m_pCBGetLogFilePath= pFunc;
}

void CAirsideIntersectionReportBaseResult::GenerateResult( CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter )
{
	m_vResult.clear();
	if (pParameter == NULL)
		return;
	CAirsideIntersectionReportParam* pParam = (CAirsideIntersectionReportParam*)pParameter;
	if(pParam == NULL)
		return;

	CString strDescFilepath = (*pCBGetLogFilePath)(AirsideFlightDescFileReport);
	CString strEventFilePath = (*pCBGetLogFilePath)(AirsideFlightEventFileReport);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;


	ElapsedTime eStartTime = pParam->getStartTime();
	ElapsedTime eEndTime = pParam->getEndTime();

	pParam->InitTaxiNodeIndex();
	//the log sequence of the intersection log
	//Start Delay Log -> Start Delay Log -> Cross log(in) - > Crossing log (out) -> Cross log (buffer)

	AirsideReportLog::AirsideFlightLogData mFlightLogData;
	mFlightLogData.OpenInput(strDescFilepath.GetString() ,strEventFilePath.GetString());
	int nCount = mFlightLogData.getItemCount();
	CProgressBar progressBar( "Generating report ", 100, nCount, TRUE, 1 );

	std::list<AirsideFlightNodeLog*>vList;
	
	for (int i = 0; i < nCount; i++)
	{
		progressBar.StepIt();

		mFlightLogData.LoadItem(mFlightLogData.ItemAt(i));
		AirsideFlightLogItem item = mFlightLogData.ItemAt(i);
		int nLogCount = (int)item.mpEventList->mEventList.size();

		int nCurrentNodeID = -1;
		std::vector< AirsideFlightNodeLog *> vNodeLog;

// 		FlightCrossNodeLog * pLogIn = NULL;
// 		FlightCrossNodeLog * pLogOut = NULL;
// 		FlightCrossNodeLog * pLogBuffer = NULL;
// 		std::vector< FlightStartNodeDelayLog *> vDelayNodeLog;

		for (int j = 0; j < nLogCount; j++)
		{
			ARCEventLog* pLog = item.mpEventList->mEventList.at(j);
			if(pLog == NULL)
				continue;

			if(pLog->time < eStartTime.getPrecisely())
				continue;
			if(pLog->time > eEndTime.getPrecisely())
				break;
				
			if(pLog->IsA(typeof(FlightStartNodeDelayLog)) ||
				pLog->IsA(typeof(FlightCrossNodeLog)))
			{
				AirsideFlightNodeLog *pNodeLog = (AirsideFlightNodeLog *)pLog;
				if(pNodeLog && pParam->IsNodeSelected(pNodeLog->mNodeId))
				{
					vList.push_back(pNodeLog);
				}
			}
		}
		HandleIntersectionNode(vList,item,pParam);
	}
}


void CAirsideIntersectionReportBaseResult::HandleIntersectionNode(std::list<AirsideFlightNodeLog*>& vNodeLog,const AirsideFlightLogItem& flightItem, CParameters *pParameter)
{
	FlightCrossNodeLog * pLogIn = NULL;
	FlightCrossNodeLog * pLogOut = NULL;
	FlightCrossNodeLog * pLogBuffer = NULL;
	FlightStartNodeDelayLog *pDelayLog = NULL;
	std::vector< FlightStartNodeDelayLog *> vDelayNodeLog;
	CAirsideIntersectionReportParam *pParam = (CAirsideIntersectionReportParam *)pParameter;

	if (vNodeLog.empty())
	{
		return;
	}
	else
	{
		AirsideFlightNodeLog* pStartLog = NULL;
		while(!vNodeLog.empty())
		{
			AirsideFlightNodeLog* pLog = vNodeLog.front();
			if(pLog->IsA(typeof(FlightStartNodeDelayLog)))
			{
				pStartLog = pLog;
				pDelayLog = (FlightStartNodeDelayLog *)pLog;
				vDelayNodeLog.push_back(pDelayLog);
			}

			if (pLog->IsA(typeof(FlightCrossNodeLog)))
			{
				FlightCrossNodeLog*pFlightCrossLog =(FlightCrossNodeLog*)pLog;
				if(pFlightCrossLog)
				{
					//IN_NODE , OUT_NODE , OUT_NODEBUFFER 
					if(pFlightCrossLog->meType == FlightCrossNodeLog::IN_NODE)
					{
						pStartLog = pLog;
						pLogIn = pFlightCrossLog;
						vNodeLog.pop_front();
						break;
					}
				}
			}
			vNodeLog.pop_front();
		}
		

		std::list<AirsideFlightNodeLog*>::iterator iter = vNodeLog.begin();
		for (; iter != vNodeLog.end(); ++iter)
		{
			AirsideFlightNodeLog *pNodeLog = (*iter);
			if (pStartLog && pNodeLog->mNodeId == pStartLog->mNodeId)
			{
				if (pNodeLog->IsA(typeof(FlightCrossNodeLog)))
				{
					FlightCrossNodeLog*pFlightCrossLog =(FlightCrossNodeLog*)pNodeLog;
					if(pFlightCrossLog)
					{
						//IN_NODE , OUT_NODE , OUT_NODEBUFFER 
						if(pFlightCrossLog->meType == FlightCrossNodeLog::OUT_NODE)
						{
							//ASSERT(pLogIn == NULL);
							pLogOut = pFlightCrossLog;
						}
						else if(pFlightCrossLog->meType == FlightCrossNodeLog::OUT_NODEBUFFER)
						{
							//ASSERT(pLogIn == NULL);
							pLogBuffer = pFlightCrossLog;
						}
					}
				}

				if (pLogIn && pLogOut && pLogBuffer)
				{
					ResultItem resultItem;

					resultItem.m_nIndex = pParam->GetNodeIndex(pLogIn->mNodeId);
					resultItem.m_nNodeID = pLogIn->mNodeId;
					resultItem.m_strNodeName = pParam->GetNodeName(pLogIn->mNodeId);//pLogIn->mNodeName.c_str();
					if(pLogIn->mFlightMode == 'A')
						resultItem.m_strFlight = CString(flightItem.mFltDesc.sAirline.c_str()) + CString(flightItem.mFltDesc.sArrID.c_str());
					else
						resultItem.m_strFlight = CString(flightItem.mFltDesc.sAirline.c_str()) + CString(flightItem.mFltDesc.sDepID.c_str());

					resultItem.m_strACType = flightItem.mFltDesc.sAcType.c_str();

					ElapsedTime eDateTimeIn;
					eDateTimeIn.setPrecisely(pLogIn->time);
					resultItem.m_eTimeIn = eDateTimeIn;

					ElapsedTime eDateTimeOut;
					eDateTimeOut.setPrecisely(pLogOut->time);
					resultItem.m_eTimeOut = eDateTimeOut;

					ElapsedTime eDateTimeBuffer;
					eDateTimeBuffer.setPrecisely(pLogBuffer->time);
					resultItem.m_eTimeBuffer = eDateTimeBuffer;

					resultItem.m_strNodeFrom = pLogIn->mNodeFromName.c_str();
					resultItem.m_strOrigin = pLogIn->mGlobalOrigin.c_str();

					resultItem.m_strNodeTo = pLogIn->mNodeToName.c_str();
					resultItem.m_strDest = pLogIn->mGlobalDest.c_str();

					int nConflictCount = static_cast<int>(vDelayNodeLog.size());
					for (int nConflict = 0; nConflict < nConflictCount; ++nConflict)
					{	
						long lDlayEndTime = 0;
						FlightStartNodeDelayLog* pConflictStartItem = vDelayNodeLog[nConflict];
						if(pConflictStartItem == NULL)
							continue;
						if(nConflict == nConflictCount -1)
						{
							lDlayEndTime = pLogIn->time;
						}
						else
						{
							FlightStartNodeDelayLog* pConflictEndItem = vDelayNodeLog[nConflict + 1];
							if(pConflictEndItem)
								lDlayEndTime = pConflictEndItem->time;
						}

						//conflict data
						ConflictItem conflictItem;
						ElapsedTime eDalayTime;
						eDalayTime.setPrecisely(lDlayEndTime - pConflictStartItem->time);
						conflictItem.m_eTimeDealy = eDalayTime;
						conflictItem.m_strFlight = pConflictStartItem->msConflictFlight.c_str(); //conflict flight
						conflictItem.m_strType = pConflictStartItem->msConflictFlightType.c_str();//conflict type

						conflictItem.m_strNodeFrom = pConflictStartItem->msConflictFltFromNode.c_str();
						conflictItem.m_strOrigin = pConflictStartItem->msConflictFltOrig.c_str();

						conflictItem.m_strNodeTo = pConflictStartItem->msConflictFltToNode.c_str();
						conflictItem.m_strDest = pConflictStartItem->msConflictFltDest.c_str();

						resultItem.m_vConflicts.push_back(conflictItem);
					}

					m_vResult.push_back(resultItem);

					HandleIntersectionNode(vNodeLog,flightItem,pParameter);
					return;
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//
CAirsideIntersectionDetailResult::CAirsideIntersectionDetailResult()
{
	m_pChartResult = NULL;
}

CAirsideIntersectionDetailResult::~CAirsideIntersectionDetailResult()
{

}

void CAirsideIntersectionDetailResult::RefreshReport( CParameters * parameter )
{
	CAirsideIntersectionReportParam* pParam = (CAirsideIntersectionReportParam*)parameter;
	ASSERT(pParam != NULL);

	if (NULL != m_pChartResult)
	{
		delete m_pChartResult;
		m_pChartResult = NULL;
	}
	//DT_CROSSING = 0,
	//	DT_BUSYFACTOR,
	//	DT_DELAY,
	//	DT_CONFLICT,
	switch(pParam->getSubType())
	{
	case DT_CROSSING:
		{
			m_pChartResult = new CArisideIntersectionDetailCrossingChartResult;
			CArisideIntersectionDetailCrossingChartResult* pResult = (CArisideIntersectionDetailCrossingChartResult*)m_pChartResult;
			pResult->GenerateResult(m_vResult, parameter);
		}
		break;
	case DT_BUSYFACTOR:
		{
			m_pChartResult = new CArisideIntersectionDetailBusyChartResult;
			CArisideIntersectionDetailBusyChartResult* pResult = (CArisideIntersectionDetailBusyChartResult*)m_pChartResult;
			pResult->GenerateResult(m_vResult, parameter);
		}
		break;
	case DT_DELAY:
		{
			m_pChartResult = new CArisideIntersectionDetailDelayChartResult;
			CArisideIntersectionDetailDelayChartResult* pResult = (CArisideIntersectionDetailDelayChartResult*)m_pChartResult;
			pResult->GenerateResult(m_vResult, parameter);
		}
		break;
	case DT_CONFLICT:
		{
			m_pChartResult = new CArisideIntersectionDetailConflictChartResult;
			CArisideIntersectionDetailConflictChartResult* pResult = (CArisideIntersectionDetailConflictChartResult*)m_pChartResult;
			pResult->GenerateResult(m_vResult, parameter);
		}
		break;


	default:
		{
			m_pChartResult = new CArisideIntersectionDetailCrossingChartResult;
			CArisideIntersectionDetailCrossingChartResult* pResult = (CArisideIntersectionDetailCrossingChartResult*)m_pChartResult;
			pResult->GenerateResult(m_vResult, parameter);
			ASSERT(0);
		}
		break;
	}
}

void CAirsideIntersectionDetailResult::GenerateResult( CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter )
{
	CAirsideIntersectionReportBaseResult::GenerateResult(pCBGetLogFilePath,pParameter);
}

void CAirsideIntersectionDetailResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	if(m_pChartResult)
		m_pChartResult->Draw3DChart(chartWnd,pParameter);

}

void CAirsideIntersectionDetailResult::InitListHead( CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC )
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);
	cxListCtrl.InsertColumn(0, _T("Index"), LVCFMT_LEFT, 50);
	piSHC->SetDataType(0,dtINT);

	cxListCtrl.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 100);	
	piSHC->SetDataType(1,dtSTRING);

	cxListCtrl.InsertColumn(2, _T("Flight ID"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(2,dtSTRING);

	cxListCtrl.InsertColumn(3, _T("AC Type"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(3,dtSTRING);


	cxListCtrl.InsertColumn(4, _T("Date In"),LVCFMT_LEFT,100);
	piSHC->SetDataType(4,dtSTRING);

	cxListCtrl.InsertColumn(5, _T("Time In"),LVCFMT_LEFT,50);
	piSHC->SetDataType(5,dtSTRING);

	cxListCtrl.InsertColumn(6, _T("Date Out"),LVCFMT_LEFT,100);
	piSHC->SetDataType(6,dtSTRING);

	cxListCtrl.InsertColumn(7, _T("Time Out"),LVCFMT_LEFT,100);
	piSHC->SetDataType(7,dtSTRING);

	cxListCtrl.InsertColumn(8, _T("Buffer Time"),LVCFMT_LEFT,80);
	piSHC->SetDataType(8,dtSTRING);

	cxListCtrl.InsertColumn(9, _T("Duration Busy"),LVCFMT_LEFT,80);
	piSHC->SetDataType(9,dtSTRING);

	cxListCtrl.InsertColumn(10, _T("Intersection From"),LVCFMT_LEFT,100);
	piSHC->SetDataType(10,dtSTRING);

	cxListCtrl.InsertColumn(11, _T("Global Origin"),LVCFMT_LEFT,100);
	piSHC->SetDataType(11,dtSTRING);

	cxListCtrl.InsertColumn(12, _T("Intersection To"),LVCFMT_LEFT,100);
	piSHC->SetDataType(12,dtSTRING);

	cxListCtrl.InsertColumn(13, _T("Global Dest"),LVCFMT_LEFT,100);
	piSHC->SetDataType(13,dtSTRING);

	cxListCtrl.InsertColumn(14, _T("In Conflict"),LVCFMT_LEFT,100);
	piSHC->SetDataType(14,dtSTRING);

	cxListCtrl.InsertColumn(15, _T("Conflict Delay(mm:hh:ss)"),LVCFMT_LEFT,120);
	piSHC->SetDataType(15,dtSTRING);

	cxListCtrl.InsertColumn(16, _T("Conflict ID"),LVCFMT_LEFT,100);
	piSHC->SetDataType(16,dtSTRING);

	cxListCtrl.InsertColumn(17, _T("AC Type"),LVCFMT_LEFT,100);
	piSHC->SetDataType(17,dtSTRING);

	cxListCtrl.InsertColumn(18, _T("From Intersection"),LVCFMT_LEFT,100);
	piSHC->SetDataType(18,dtSTRING);

	cxListCtrl.InsertColumn(19, _T("Global Origin"),LVCFMT_LEFT,100);
	piSHC->SetDataType(19,dtSTRING);

	cxListCtrl.InsertColumn(20, _T("To Intersection"),LVCFMT_LEFT,100);
	piSHC->SetDataType(20,dtSTRING);

	cxListCtrl.InsertColumn(21, _T("Global Dest"),LVCFMT_LEFT,100);
	piSHC->SetDataType(21,dtSTRING);


}

void CAirsideIntersectionDetailResult::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter )
{
	int nIndex = 0;
	for (int nResult = 0; nResult < static_cast<int>(m_vResult.size()); ++nResult)
	{
		ResultItem& resultItem = m_vResult[nResult];


		if(resultItem.m_vConflicts.size() > 0)
		{
			for (int nConflict = 0; nConflict < static_cast<int>(resultItem.m_vConflicts.size()); ++ nConflict)
			{
				InsertItemToList(cxListCtrl,nIndex,resultItem,resultItem.m_vConflicts[nConflict]);
				nIndex += 1;
			}
		}
		else
		{
			InsertItemToList(cxListCtrl,nIndex,resultItem,ConflictItem());
			nIndex += 1;

		}
	}




}
void CAirsideIntersectionDetailResult::InsertItemToList(CXListCtrl& cxListCtrl,int nIndex, const ResultItem& resultItem, const ConflictItem& conflictItem )
{
	//int m_nNodeID;
	//CString m_strNodeName;
	//CString m_strFlight;

	//ElapsedTime m_eTimeIn;

	//ElapsedTime m_eTimeOut;

	//ElapsedTime m_eTimeBuffer;

	//CString m_strNodeFrom;
	//CString m_strOrigin;

	//CString m_strNodeTo;
	//CString m_strDest;

	//node id
	CString strNodeID;
	strNodeID.Format(_T("%d"),resultItem.m_nIndex);
	cxListCtrl.InsertItem(nIndex, strNodeID);

	//node name
	int nSubItem = 1;
	cxListCtrl.SetItemText(nIndex, nSubItem,resultItem.m_strNodeName);

	//flight
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem,resultItem.m_strFlight);

	//ac type
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem,resultItem.m_strACType);

	//date in
	int nDateIn = resultItem.m_eTimeIn.GetDay();
	CString strDayIn;
	strDayIn.Format(_T("Day %d"),nDateIn);
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem,strDayIn);
	//time in
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem,resultItem.m_eTimeIn.printTime());

	//date out
	int nDateOut = resultItem.m_eTimeIn.GetDay();
	CString strDateOut;
	strDateOut.Format(_T("Day %d"),nDateOut);
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem,strDateOut);
	//time out
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem,resultItem.m_eTimeOut.printTime());

	//buffer time
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem, resultItem.getBufferTime().printTime());

	//duration busy
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem,resultItem.getDurationBusy().printTime());

	//intersection from
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem,resultItem.m_strNodeFrom);
	
	//global origin
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem, resultItem.m_strOrigin);

	//intersection to
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem, resultItem.m_strNodeTo);

	//global dest
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem,resultItem.m_strDest);

	//conflict item, Yes or no
	nSubItem += 1;
	CString strConflict = _T("No");
	if(resultItem.m_vConflicts.size() > 0)
		strConflict = _T("Yes");
	cxListCtrl.SetItemText(nIndex, nSubItem,strConflict);
	
	if(resultItem.m_vConflicts.size() == 0)
		return;

	//conflict delay
	nSubItem += 1;
	CString strDelayTime;
	strDelayTime.Format(_T("%02d:%02d:%02d"),conflictItem.m_eTimeDealy.GetHour(),conflictItem.m_eTimeDealy.GetMinute(),conflictItem.m_eTimeDealy.GetSecond());
	cxListCtrl.SetItemText(nIndex, nSubItem,strDelayTime);

	//conflict id
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem,conflictItem.m_strFlight);

	//conflict ac type
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem,conflictItem.m_strType);

	//conflict from intersection
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem,conflictItem.m_strNodeFrom);

	//conflict global origin
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem, conflictItem.m_strOrigin);

	//conflict intersection to
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem, conflictItem.m_strNodeTo);

	//conflict global dest
	nSubItem += 1;
	cxListCtrl.SetItemText(nIndex, nSubItem,conflictItem.m_strDest);


}
BOOL CAirsideIntersectionDetailResult::ExportReportData( ArctermFile& _file,CString& Errmsg )
{
	int nCount = (int)m_vResult.size();
	_file.writeInt(nCount);
	_file.writeLine();
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		ResultItem& item = m_vResult[nIndex];
		_file.writeInt(item.m_nIndex);
		_file.writeInt(item.m_nNodeID);
		_file.writeField(item.m_strNodeName);
		_file.writeField(item.m_strFlight);
		_file.writeField(item.m_strACType);
		_file.writeTime(item.m_eTimeIn,TRUE);
		_file.writeTime(item.m_eTimeOut,TRUE);
		_file.writeTime(item.m_eTimeBuffer,TRUE);
		_file.writeField(item.m_strNodeFrom);
		_file.writeField(item.m_strOrigin);
		_file.writeField(item.m_strNodeTo);
		_file.writeField(item.m_strDest);

		//conflict items
		_file.writeLine();
		_file.writeInt(static_cast<int>(item.m_vConflicts.size()));
		_file.writeLine();
		for (int nConflict = 0; nConflict < static_cast<int>(item.m_vConflicts.size()); ++ nConflict)
		{
			_file.writeTime(item.m_vConflicts[nConflict].m_eTimeDealy,TRUE);
			_file.writeField(item.m_vConflicts[nConflict].m_strFlight);
			_file.writeField(item.m_vConflicts[nConflict].m_strType);
			_file.writeField(item.m_vConflicts[nConflict].m_strNodeFrom);
			_file.writeField(item.m_vConflicts[nConflict].m_strOrigin);
			_file.writeField(item.m_vConflicts[nConflict].m_strNodeTo);
			_file.writeField(item.m_vConflicts[nConflict].m_strDest);
			_file.writeLine();
		}

	}	
	return TRUE;
}

BOOL CAirsideIntersectionDetailResult::ImportReportData( ArctermFile& _file,CString& Errmsg )
{
	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		ResultItem item;
		_file.getInteger(item.m_nIndex);
		_file.getInteger(item.m_nNodeID);
		_file.getField(item.m_strNodeName.GetBuffer(1024),1024);
		_file.getField(item.m_strFlight.GetBuffer(1024),1024);
		_file.getField(item.m_strACType.GetBuffer(1024),1024);
		_file.getTime(item.m_eTimeIn,TRUE);
		_file.getTime(item.m_eTimeOut,TRUE);
		_file.getTime(item.m_eTimeBuffer,TRUE);
		_file.getField(item.m_strNodeFrom.GetBuffer(1024),1024);
		_file.getField(item.m_strOrigin.GetBuffer(1024),1024);
		_file.getField(item.m_strNodeTo.GetBuffer(1024),1024);
		_file.getField(item.m_strDest.GetBuffer(1024),1024);

		//conflict items
		int nConflictCount = 0;
		_file.getLine();
		_file.getInteger(nConflictCount);
		_file.getLine();
		for (int nConflict = 0; nConflict < nConflictCount; ++ nConflict)
		{
			ConflictItem conflictItem;
			_file.getTime(conflictItem.m_eTimeDealy,TRUE);
			_file.getField(conflictItem.m_strFlight.GetBuffer(1024),1024);
			_file.getField(conflictItem.m_strType.GetBuffer(1024),1024);
			_file.getField(conflictItem.m_strNodeFrom.GetBuffer(1024),1024);
			_file.getField(conflictItem.m_strOrigin.GetBuffer(1024),1024);
			_file.getField(conflictItem.m_strNodeTo.GetBuffer(1024),1024);
			_file.getField(conflictItem.m_strDest.GetBuffer(1024),1024);
			_file.getLine();
			item.m_vConflicts.push_back(conflictItem);
		}


		m_vResult.push_back(item);
	}




	return TRUE;
}

BOOL CAirsideIntersectionDetailResult::ExportListData( ArctermFile& _file,CParameters * parameter )
{


	return FALSE;
}

BOOL CAirsideIntersectionDetailResult::WriteReportData( ArctermFile& _file )
{
	_file.writeField("Airside Intersection Report");
	_file.writeLine();

	_file.writeInt(ASReportType_Detail);
	_file.writeLine();

	CString strEsg;
	ExportReportData(_file,strEsg);
	return TRUE;
}

BOOL CAirsideIntersectionDetailResult::ReadReportData( ArctermFile& _file )
{	
	CString strEsg;
	ImportReportData(_file,strEsg);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//CAirsideIntersectionSummarylResult
CAirsideIntersectionSummarylResult::CAirsideIntersectionSummarylResult()
{
	m_pChartResult = NULL;
}

CAirsideIntersectionSummarylResult::~CAirsideIntersectionSummarylResult()
{

}

void CAirsideIntersectionSummarylResult::RefreshReport( CParameters * parameter )
{
	CAirsideIntersectionReportParam* pParam = (CAirsideIntersectionReportParam*)parameter;
	ASSERT(pParam != NULL);

	if (NULL != m_pChartResult)
	{
		delete m_pChartResult;
		m_pChartResult = NULL;
	}
	//SM_CROSSING,
	//	SM_DELAY
	switch(pParam->getSubType())
	{
	case SM_CROSSING:
		{
			m_pChartResult = new CAirsideIntersectionSummaryCrossingTimeChartResult;
			CAirsideIntersectionSummaryCrossingTimeChartResult* pResult = (CAirsideIntersectionSummaryCrossingTimeChartResult*)m_pChartResult;
			pResult->GenerateResult(m_vSummaryResult, parameter);		
		}
		break;
	case SM_DELAY:
		{
			m_pChartResult = new CAirsideIntersectionSummaryCrossingDelayChartResult;
			CAirsideIntersectionSummaryCrossingDelayChartResult* pResult = (CAirsideIntersectionSummaryCrossingDelayChartResult*)m_pChartResult;
			pResult->GenerateResult(m_vSummaryResult, parameter);		
		}
		break;

	default:
		{
			m_pChartResult = new CAirsideIntersectionSummaryCrossingTimeChartResult;
			CAirsideIntersectionSummaryCrossingTimeChartResult* pResult = (CAirsideIntersectionSummaryCrossingTimeChartResult*)m_pChartResult;
			pResult->GenerateResult(m_vSummaryResult, parameter);		

			ASSERT(0);
		}
	}
}

void CAirsideIntersectionSummarylResult::GenerateResult( CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter )
{
	CAirsideIntersectionReportBaseResult::GenerateResult(pCBGetLogFilePath,pParameter);

	m_vSummaryResult.clear();
	if(pParameter == NULL)
		return;

	//split the report time
	ElapsedTime eStartTime = pParameter->getStartTime();
	ElapsedTime eEndTime = pParameter->getEndTime();
	ElapsedTime eInterval = pParameter->getInterval();

	NodeSummaryMiddleResult sampleMiddleResult;
	sampleMiddleResult.InitResult(eStartTime,eEndTime,eInterval);

	//generate middle result
	//sort the result item by node
	int nResultCount = static_cast<int>(m_vResult.size());
	for (int nResult = 0; nResult < nResultCount; ++ nResult)
	{
		ResultItem& nodeResultItem = m_vResult[nResult];
		
		std::map<int, NodeSummaryMiddleResult >::iterator iterFind = m_mapNodeMiddleResult.find(nodeResultItem.m_nNodeID);
		if(iterFind != m_mapNodeMiddleResult.end())
		{
			(*iterFind).second.AddResultItem(nodeResultItem);
		}
		else
		{
			NodeSummaryMiddleResult middleResult = sampleMiddleResult;
			middleResult.m_nNodeID = nodeResultItem.m_nNodeID;
			middleResult.m_strNodeName = nodeResultItem.m_strNodeName;
			middleResult.AddResultItem(nodeResultItem);
			m_mapNodeMiddleResult[nodeResultItem.m_nNodeID] = middleResult;
		}
	}

	//generate summary result
	GenerateSummaryResult();

}
void CAirsideIntersectionSummarylResult::GenerateSummaryResult()
{
	std::map<int, NodeSummaryMiddleResult >::iterator iter = m_mapNodeMiddleResult.begin();
	for (; iter != m_mapNodeMiddleResult.end(); ++iter)
	{
		SummaryResultItem summaryItem;
		(*iter).second.GenerateSummaryResult(m_vSummaryResult);
	}

}
void CAirsideIntersectionSummarylResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	if(m_pChartResult)
		m_pChartResult->Draw3DChart(chartWnd,pParameter);

}

void CAirsideIntersectionSummarylResult::InitListHead( CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC )
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(0,dtSTRING);

	cxListCtrl.InsertColumn(1, _T("Interval"), LVCFMT_LEFT, 200);
	piSHC->SetDataType(1,dtSTRING);

	cxListCtrl.InsertColumn(2, _T("Crossings"), LVCFMT_LEFT, 100);
	piSHC->SetDataType(2,dtINT);

	cxListCtrl.InsertColumn(3, _T("Min Time"),LVCFMT_LEFT,60);
	piSHC->SetDataType(3,dtSTRING);

	cxListCtrl.InsertColumn(4, _T("Avg Time"),LVCFMT_LEFT,60);
	piSHC->SetDataType(4,dtSTRING);

	cxListCtrl.InsertColumn(5, _T("Max Time"),LVCFMT_LEFT,60);
	piSHC->SetDataType(5,dtSTRING);

	cxListCtrl.InsertColumn(6, _T("Q1 Time"),LVCFMT_LEFT,60);
	piSHC->SetDataType(6,dtSTRING);

	cxListCtrl.InsertColumn(7, _T("Q2 Time"),LVCFMT_LEFT,60);
	piSHC->SetDataType(7,dtSTRING);

	cxListCtrl.InsertColumn(8, _T("Q3 Time"),LVCFMT_LEFT,60);
	piSHC->SetDataType(8,dtSTRING);

	cxListCtrl.InsertColumn(9, _T("P1 Time"),LVCFMT_LEFT,60);
	piSHC->SetDataType(9,dtSTRING);

	cxListCtrl.InsertColumn(10, _T("P5 Time"),LVCFMT_LEFT,60);
	piSHC->SetDataType(10,dtSTRING);

	cxListCtrl.InsertColumn(11, _T("P10 Time"),LVCFMT_LEFT,60);
	piSHC->SetDataType(11,dtSTRING);

	cxListCtrl.InsertColumn(12, _T("P90 Time"),LVCFMT_LEFT,60);
	piSHC->SetDataType(12,dtSTRING);

	cxListCtrl.InsertColumn(13, _T("P95 Time"),LVCFMT_LEFT,60);
	piSHC->SetDataType(13,dtSTRING);

	cxListCtrl.InsertColumn(14, _T("P99 Time"),LVCFMT_LEFT,60);
	piSHC->SetDataType(14,dtSTRING);

	cxListCtrl.InsertColumn(15, _T("Sigma Time"),LVCFMT_LEFT,60);
	piSHC->SetDataType(15,dtSTRING);

	
	cxListCtrl.InsertColumn(16, _T("Conflict count"),LVCFMT_LEFT,100);
	piSHC->SetDataType(16,dtINT);


	cxListCtrl.InsertColumn(17, _T("Min Delay"),LVCFMT_LEFT,60);
	piSHC->SetDataType(17,dtSTRING);

	cxListCtrl.InsertColumn(18, _T("Avg Delay"),LVCFMT_LEFT,60);
	piSHC->SetDataType(18,dtSTRING);

	cxListCtrl.InsertColumn(19, _T("Max Delay"),LVCFMT_LEFT,60);
	piSHC->SetDataType(19,dtSTRING);

	cxListCtrl.InsertColumn(20, _T("Q1 Delay"),LVCFMT_LEFT,60);
	piSHC->SetDataType(20,dtSTRING);

	cxListCtrl.InsertColumn(21, _T("Q2 Delay"),LVCFMT_LEFT,60);
	piSHC->SetDataType(21,dtSTRING);

	cxListCtrl.InsertColumn(22, _T("Q3 Delay"),LVCFMT_LEFT,60);
	piSHC->SetDataType(22,dtSTRING);

	cxListCtrl.InsertColumn(23, _T("P1 Delay"),LVCFMT_LEFT,60);
	piSHC->SetDataType(23,dtSTRING);

	cxListCtrl.InsertColumn(24, _T("P5 Delay"),LVCFMT_LEFT,60);
	piSHC->SetDataType(24,dtSTRING);

	cxListCtrl.InsertColumn(25, _T("P10 Delay"),LVCFMT_LEFT,60);
	piSHC->SetDataType(25,dtSTRING);

	cxListCtrl.InsertColumn(26, _T("P90 Delay"),LVCFMT_LEFT,60);
	piSHC->SetDataType(26,dtSTRING);

	cxListCtrl.InsertColumn(27, _T("P95 Delay"),LVCFMT_LEFT,60);
	piSHC->SetDataType(27,dtSTRING);

	cxListCtrl.InsertColumn(28, _T("P99 Delay"),LVCFMT_LEFT,60);
	piSHC->SetDataType(28,dtSTRING);

	cxListCtrl.InsertColumn(29, _T("Sigma Delay"),LVCFMT_LEFT,60);
	piSHC->SetDataType(29,dtSTRING);


}

void CAirsideIntersectionSummarylResult::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter )
{
	int nIndex = 0;
	for (int nResult = 0; nResult < static_cast<int>(m_vSummaryResult.size()); ++nResult)
	{
		NodeSummaryResultItem& nodeSummaryItem = m_vSummaryResult[nResult];

		for (int nInterval = 0; nInterval < static_cast<int>(nodeSummaryItem.m_vIntervalItem.size()); ++nInterval)
		{
			SummaryResultItem& summaryItem = nodeSummaryItem.m_vIntervalItem[nInterval];
			//node name
			cxListCtrl.InsertItem(nIndex, summaryItem.m_strNodeName);

			//interval
			int nSubItem = 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.GetInterval());

			//crossing
			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,FormatString(summaryItem.m_nCrossingCount));

			//crossing time include buff
			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryCrossing.m_minTime.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryCrossing.m_avgTime.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryCrossing.m_maxTime.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryCrossing.m_nQ1.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryCrossing.m_nQ2.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryCrossing.m_nQ3.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryCrossing.m_nP1.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryCrossing.m_nP5.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryCrossing.m_nP10.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryCrossing.m_nP90.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryCrossing.m_nP95.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryCrossing.m_nP99.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryCrossing.m_nStdDev.printTime());

			//conflict
			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,FormatString(summaryItem.m_nConflictCount));
			

			//delay time
			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryDelay.m_minTime.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryDelay.m_avgTime.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryDelay.m_maxTime.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryDelay.m_nQ1.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryDelay.m_nQ2.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryDelay.m_nQ3.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryDelay.m_nP1.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryDelay.m_nP5.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryDelay.m_nP10.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryDelay.m_nP90.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryDelay.m_nP95.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryDelay.m_nP99.printTime());

			nSubItem += 1;
			cxListCtrl.SetItemText(nIndex, nSubItem,summaryItem.summaryDelay.m_nStdDev.printTime());
			
			
			nIndex+=1;

		}

	}

}

BOOL CAirsideIntersectionSummarylResult::ExportReportData( ArctermFile& _file,CString& Errmsg )
{
	int nCount = (int)m_vSummaryResult.size();
	_file.writeInt(nCount);
	_file.writeLine();
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		NodeSummaryResultItem& nodeSummaryItem = m_vSummaryResult[nIndex];

		_file.writeInt(nodeSummaryItem.m_nNodeID);
		_file.writeField(nodeSummaryItem.m_strNodeName);

		_file.writeInt( static_cast<int>(nodeSummaryItem.m_vIntervalItem.size()));
	
		_file.writeLine();

		for (int nInterval = 0; nInterval < static_cast<int>(nodeSummaryItem.m_vIntervalItem.size()); ++nInterval)
		{
			SummaryResultItem& item = nodeSummaryItem.m_vIntervalItem[nInterval];

			_file.writeInt(item.m_nNodeID);
			_file.writeField(item.m_strNodeName);

			_file.writeTime(item.m_eStartTime,TRUE);
			_file.writeTime(item.m_eEndTime,TRUE);

			_file.writeInt(item.m_nCrossingCount);

			item.summaryCrossing.WirteFile(_file);

			_file.writeInt(item.m_nConflictCount);

			item.summaryDelay.WirteFile(_file);

			_file.writeLine();

		}
	}

	return TRUE;
}

BOOL CAirsideIntersectionSummarylResult::ImportReportData( ArctermFile& _file,CString& Errmsg )
{

	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		
		//read node
		NodeSummaryResultItem nodeSummaryResultItem;

		_file.getInteger(nodeSummaryResultItem.m_nNodeID);
		_file.getField(nodeSummaryResultItem.m_strNodeName.GetBuffer(1024),1024);

		int nIntervalCount = 0;
		_file.getInteger(nIntervalCount);
		_file.getLine();

		for (int nInterval = 0; nInterval < nIntervalCount; ++ nInterval)
		{
			SummaryResultItem item;
			_file.getInteger(item.m_nNodeID);
			_file.getField(item.m_strNodeName.GetBuffer(1024),1024);

			_file.getTime(item.m_eStartTime,TRUE);
			_file.getTime(item.m_eEndTime,TRUE);

			_file.getInteger(item.m_nCrossingCount);

			item.summaryCrossing.ReadFile(_file);

			_file.getInteger(item.m_nConflictCount);

			item.summaryDelay.ReadFile(_file);

			nodeSummaryResultItem.m_vIntervalItem.push_back(item);

			_file.getLine();
		}
		 m_vSummaryResult.push_back(nodeSummaryResultItem);
	}
	return FALSE;
}

BOOL CAirsideIntersectionSummarylResult::ExportListData( ArctermFile& _file,CParameters * parameter )
{
	return FALSE;
}

BOOL CAirsideIntersectionSummarylResult::WriteReportData( ArctermFile& _file )
{
	_file.writeField("Airside Intersection Report");
	_file.writeLine();

	_file.writeInt(ASReportType_Summary);
	_file.writeLine();

	CString strEsg;
	ExportReportData(_file,strEsg);
	return TRUE;

}

BOOL CAirsideIntersectionSummarylResult::ReadReportData( ArctermFile& _file )
{
	CString strEsg;
	ImportReportData(_file,strEsg);
	return TRUE;
}

