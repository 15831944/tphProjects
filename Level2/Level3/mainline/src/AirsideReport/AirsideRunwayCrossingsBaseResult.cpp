#include "StdAfx.h"
#include "AirsideRunwayCrossingsBaseResult.h"
#include "AirsideRunwayCrossingBaseDetailResult.h"
#include "AirsideRunwayCrossingsBaseSummaryResult.h"
#include "AirsideRunwayCrossingsPara.h"
#include "../Results/AirsideFlightEventLog.h"
#include "../Results/AirsideFlightLogItem.h"
#include "../Results/ARCBaseLog.h"
#include "../Common/ARCUnit.h"
#include "../Reports/StatisticalTools.h"



CAirsideRunwayCrossingsBaseResult::CAirsideRunwayCrossingsBaseResult()
{

}

CAirsideRunwayCrossingsBaseResult::~CAirsideRunwayCrossingsBaseResult()
{

}
void CAirsideRunwayCrossingsBaseResult::setLoadFromFile( bool bLoad )
{
	m_bLoadFromFile = bLoad;
}

bool CAirsideRunwayCrossingsBaseResult::IsLoadFromFile()
{
	return m_bLoadFromFile;

}

void CAirsideRunwayCrossingsBaseResult::SetCBGetFilePath( CBGetLogFilePath pFunc )
{
	m_pCBGetLogFilePath= pFunc;
}

///////////////////////////////////////////////////////////////////////////////////////////////
CAirsideRunwayCrossingBaseDetailResult::CAirsideRunwayCrossingBaseDetailResult()
:m_pChartResult(NULL)
{

}

CAirsideRunwayCrossingBaseDetailResult::~CAirsideRunwayCrossingBaseDetailResult()
{

}

void CAirsideRunwayCrossingBaseDetailResult::GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter)
{
	if (pParameter == NULL)
		return;
	AirsideRunwayCrossingsParameter* pRunwayCrossingsPara = (AirsideRunwayCrossingsParameter*)pParameter;

	CString strDescFilepath = (*pCBGetLogFilePath)(AirsideFlightDescFileReport);
	CString strEventFilePath = (*pCBGetLogFilePath)(AirsideFlightEventFileReport);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	m_vResult.clear();
	ARCBaseLog<AirsideFlightLogItem> mFlightLogData;
	mFlightLogData.OpenInput(strDescFilepath.GetString(), strEventFilePath.GetString());

	std::vector<AirsideRunwayCrossigsLog*> vFitRunwayCrossingLog;
	int nCount = mFlightLogData.getItemCount();
	for (int i = 0; i < nCount; i++)
	{
		vFitRunwayCrossingLog.clear();
		mFlightLogData.LoadItem(mFlightLogData.ItemAt(i));
		AirsideFlightLogItem item = mFlightLogData.ItemAt(i);
		int nLogCount = (int)item.mpEventList->mEventList.size();
		for (int j = 0; j < nLogCount; j++)
		{
			ARCEventLog* pLog = item.mpEventList->mEventList.at(j);
			if (!pLog->IsA(typeof(AirsideRunwayCrossigsLog)))
			    continue;
			
			AirsideRunwayCrossigsLog* pRunwayCrossingsLog = (AirsideRunwayCrossigsLog*)pLog;
			if (pRunwayCrossingsPara->fit(pRunwayCrossingsLog))
			{
				vFitRunwayCrossingLog.push_back(pRunwayCrossingsLog);
			}
		}
		
		AirsideFlightLogDesc fltdesc = item.mFltDesc;
		AirsideFlightDescStruct descStruct;
		fltdesc.getFlightDescStruct(descStruct);
		std::vector<AirsideRunwayCrossigsLog*> vRunwayCrossingsLog;
		vRunwayCrossingsLog.clear();
		for (int nIndex = 0; nIndex < (int)vFitRunwayCrossingLog.size();nIndex++)
		{
			AirsideRunwayCrossigsLog* pLog = vFitRunwayCrossingLog.at(nIndex);
			if (pLog && pLog->state != AirsideRunwayCrossigsLog::OnExitRunway)
			{
				vRunwayCrossingsLog.push_back(pLog);
			}
			else
			{
				vRunwayCrossingsLog.push_back(pLog);
				DetailRunwayCrossingsItem runwayItem;
				runwayItem.m_sRunway = pLog->sRunway.c_str();					

				if (descStruct._arrID.HasValue() && descStruct._depID.HasValue())
				{
					runwayItem.m_sFlightID.Format(_T("%s%s/%s"),descStruct._airline.GetValue(),descStruct._arrID.GetValue(),descStruct._depID.GetValue());
				}
				else
				{
					runwayItem.m_sFlightID.Format(_T("%s%s%s"),descStruct._airline.GetValue(),descStruct._arrID.GetValue(),descStruct._depID.GetValue());
				}

				long time = 0;
				double dSpeed = 0.0;
				runwayItem.m_sActype = descStruct._acType.GetValue();

				bool bHaveEnterLog = false;
				bool bHaveWaitLog = false;

				for (int nIdx = 0; nIdx < (int)vRunwayCrossingsLog.size(); nIdx++)
				{
					AirsideRunwayCrossigsLog* pCrossingLog = vRunwayCrossingsLog[nIdx];
					
					switch(pCrossingLog->state)
					{
					case AirsideRunwayCrossigsLog::OnWaitingCrossRunway:
						{
							bHaveWaitLog = true;
							if (time)
							{
								time = min(time,pCrossingLog->time);
							}
							else
							{
								time = pCrossingLog->time;
							}
						}
						break;
					case AirsideRunwayCrossigsLog::OnEnterRunway:
						{
							bHaveEnterLog = true;
							runwayItem.m_lEnterRunwayTime = pCrossingLog->time/100;
							if (bHaveWaitLog)
							{
								runwayItem.m_lWaitTimeFroRunwayCrossings = (pCrossingLog->time - time)/100;
							}
							else
							{
								runwayItem.m_lWaitTimeFroRunwayCrossings = 0;
							}
							dSpeed = pCrossingLog->speed;
							runwayItem.m_sEnterTaxiway = pCrossingLog->sTaxiway.c_str();
							runwayItem.m_sNodeName = pCrossingLog->sNodeName.c_str();
						}
						break;
					case AirsideRunwayCrossigsLog::OnExitRunway:
						{
							runwayItem.m_lExitRunwayTime = pCrossingLog->time/100;
							runwayItem.m_lOccupancyTime = (runwayItem.m_lExitRunwayTime - runwayItem.m_lEnterRunwayTime);
							runwayItem.m_dRunwayCrossingsSpeed = ARCUnit::ConvertVelocity((pCrossingLog->speed + dSpeed)/2,ARCUnit::CMpS,ARCUnit::MpS);
							runwayItem.m_sExitTaxiway = pCrossingLog->sTaxiway.c_str();
						}
						break;
					default:
						ASSERT(0);
						break;
					}
				}
				if(bHaveEnterLog)
				{
					m_vResult.push_back(runwayItem);
					CString strLog;
					strLog.Format("%s,%s,%s,%s,%s,%s,%d,%d,%d,%5d,%.2f\r\n",
					runwayItem.m_sRunway,
					runwayItem.m_sEnterTaxiway,
					runwayItem.m_sExitTaxiway,
					runwayItem.m_sFlightID,
					runwayItem.m_sActype,
					runwayItem.m_sNodeName,
					runwayItem.m_lEnterRunwayTime,
					runwayItem.m_lExitRunwayTime,
					runwayItem.m_lOccupancyTime,
					runwayItem.m_lWaitTimeFroRunwayCrossings,
					runwayItem.m_dRunwayCrossingsSpeed);
					TRACE(strLog);
				}
				vRunwayCrossingsLog.clear();
			}
		}
		
	}
	RefreshReport(pRunwayCrossingsPara);
}


void CAirsideRunwayCrossingBaseDetailResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if (m_pChartResult)
	{
		m_pChartResult->Draw3DChart(chartWnd,pParameter);
	}
}

BOOL CAirsideRunwayCrossingBaseDetailResult::WriteReportData(ArctermFile& _file)
{
	_file.writeField("Runway Crossings Report");
	_file.writeLine();

	_file.writeInt(ASReportType_Detail);
	_file.writeLine();

	int nCount = (int)m_vResult.size();
	_file.writeInt(nCount);
	_file.writeLine();
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		DetailRunwayCrossingsItem& item = m_vResult[nIndex];
		_file.writeField(item.m_sRunway);
		_file.writeField(item.m_sEnterTaxiway);
		_file.writeField(item.m_sExitTaxiway);
		_file.writeField(item.m_sFlightID);
		_file.writeField(item.m_sActype);
		_file.writeField(item.m_sNodeName);
		_file.writeInt(item.m_lEnterRunwayTime);
		_file.writeInt(item.m_lExitRunwayTime);
		_file.writeInt(item.m_lOccupancyTime);
		_file.writeInt(item.m_lWaitTimeFroRunwayCrossings);
		_file.writeFloat((float)item.m_dRunwayCrossingsSpeed);
		_file.writeLine();
	}
	return TRUE;
}	

BOOL CAirsideRunwayCrossingBaseDetailResult::ReadReportData(ArctermFile& _file)
{
	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		DetailRunwayCrossingsItem item;
		_file.getField(item.m_sRunway.GetBuffer(1024),1024);
		_file.getField(item.m_sEnterTaxiway.GetBuffer(1024),1024);
		_file.getField(item.m_sExitTaxiway.GetBuffer(1024),1024);
		_file.getField(item.m_sFlightID.GetBuffer(1024),1024);
		_file.getField(item.m_sActype.GetBuffer(1024),1024);
		_file.getField(item.m_sNodeName.GetBuffer(1024),1024);
		_file.getInteger(item.m_lEnterRunwayTime);
		_file.getInteger(item.m_lExitRunwayTime);
		_file.getInteger(item.m_lOccupancyTime);
		_file.getInteger(item.m_lWaitTimeFroRunwayCrossings);
		_file.getFloat(item.m_dRunwayCrossingsSpeed);
		_file.getLine();
		m_vResult.push_back(item);
	}
	return TRUE;
}

void CAirsideRunwayCrossingBaseDetailResult::RefreshReport(CParameters * parameter)
{
	AirsideRunwayCrossingsParameter* pParam = (AirsideRunwayCrossingsParameter*)parameter;
	ASSERT(pParam != NULL);

	if (NULL != m_pChartResult)
	{
		delete m_pChartResult;
		m_pChartResult = NULL;
	}

	switch(pParam->getSubType())
	{
	case DT_COUNT:
		{
			m_pChartResult = new CRunwayCrossingDetailCountResult;
			CRunwayCrossingDetailCountResult* pResult = (CRunwayCrossingDetailCountResult*)m_pChartResult;
			pResult->GenerateResult(m_vResult, parameter);
		}
		break;
	case DT_WAITTIME:
		{
			m_pChartResult = new CRunwayCrossingDetailWaitTimeResult;
			CRunwayCrossingDetailWaitTimeResult* pResult = (CRunwayCrossingDetailWaitTimeResult*)m_pChartResult;
			pResult->GenerateResult(m_vResult, parameter);
		}
		break;
	case DT_TAXINODE:
		{
			m_pChartResult = new CRunwayCrossingDetailRunwayBayTaxiwayResult;
			CRunwayCrossingDetailRunwayBayTaxiwayResult* pResult = (CRunwayCrossingDetailRunwayBayTaxiwayResult*)m_pChartResult;
			pResult->GenerateResult(m_vResult, parameter);
		}
		break;
	default:
		break;
	}
}

void CAirsideRunwayCrossingBaseDetailResult::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("Date"), LVCFMT_LEFT, 50);	
	cxListCtrl.InsertColumn(1, _T("Runway"), LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(2, _T("Time enter runway(hh:mm:ss)"), LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(3, _T("Time exit runway(hh:mm:ss)"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(4, _T("Runway occupancy(secs)"),LVCFMT_LEFT,50);
	cxListCtrl.InsertColumn(5, _T("Taxiway@enter"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(6, _T("Taxiway@exit"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(7, _T("Flight ID"),LVCFMT_LEFT,80);
	cxListCtrl.InsertColumn(8, _T("AC Type"),LVCFMT_LEFT,80);
	cxListCtrl.InsertColumn(9, _T("Wait time to cross(secs)"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(10, _T("Average speed of crossing(kps/mph)"),LVCFMT_LEFT,100);
}

void CAirsideRunwayCrossingBaseDetailResult::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	for (int i=0; i<(int)m_vResult.size(); i++)
	{
		DetailRunwayCrossingsItem runwayCrossingsItem = m_vResult[i];

		CString strDate(_T(""));
		ElapsedTime estEntryTime(runwayCrossingsItem.m_lEnterRunwayTime);
		strDate.Format(_T("Day%d"),estEntryTime.GetDay());
		cxListCtrl.InsertItem(i, strDate);

		cxListCtrl.SetItemText(i, 1,runwayCrossingsItem.m_sRunway);

		CString strEnterRunwayTime(_T(""));
		ElapsedTime estEntryRunway(runwayCrossingsItem.m_lEnterRunwayTime);
		strEnterRunwayTime.Format(_T("%02d:%02d:%02d"),estEntryRunway.GetHour(),estEntryRunway.GetMinute(),estEntryRunway.GetSecond());
		cxListCtrl.SetItemText(i, 2,strEnterRunwayTime);
		
		CString strExitRunwayTime(_T(""));
		ElapsedTime estExitRunway(runwayCrossingsItem.m_lExitRunwayTime);
		strExitRunwayTime.Format(_T("%02d:%02d:%02d"),estExitRunway.GetHour(),estExitRunway.GetMinute(),estExitRunway.GetSecond());
		cxListCtrl.SetItemText(i, 3,strExitRunwayTime);

		CString strRunwayOccupancyTime(_T(""));
		strRunwayOccupancyTime.Format(_T("%d"),runwayCrossingsItem.m_lOccupancyTime);
		cxListCtrl.SetItemText(i, 4,strRunwayOccupancyTime);

		cxListCtrl.SetItemText(i, 5,runwayCrossingsItem.m_sEnterTaxiway);
		cxListCtrl.SetItemText(i, 6,runwayCrossingsItem.m_sExitTaxiway);

		cxListCtrl.SetItemText(i, 7,runwayCrossingsItem.m_sFlightID);
		cxListCtrl.SetItemText(i, 8,runwayCrossingsItem.m_sActype);

		CString strWaitTime(_T(""));
		strWaitTime.Format(_T("%d"),runwayCrossingsItem.m_lWaitTimeFroRunwayCrossings);
		cxListCtrl.SetItemText(i, 9,strWaitTime);

		CString strSpeed(_T(""));
		strSpeed.Format(_T("%.2f"),runwayCrossingsItem.m_dRunwayCrossingsSpeed);
		cxListCtrl.SetItemText(i, 10,strSpeed);
	}
}

BOOL CAirsideRunwayCrossingBaseDetailResult::ExportListData(ArctermFile& _file,CParameters * parameter)
{
	return TRUE;
}

BOOL CAirsideRunwayCrossingBaseDetailResult::ImportReportData(ArctermFile& _file,CString& Errmsg)
{
	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		DetailRunwayCrossingsItem item;
		_file.getField(item.m_sRunway.GetBuffer(1024),1024);
		_file.getField(item.m_sEnterTaxiway.GetBuffer(1024),1024);
		_file.getField(item.m_sExitTaxiway.GetBuffer(1024),1024);
		_file.getField(item.m_sFlightID.GetBuffer(1024),1024);
		_file.getField(item.m_sActype.GetBuffer(1024),1024);
		_file.getField(item.m_sNodeName.GetBuffer(1024),1024);
		_file.getInteger(item.m_lEnterRunwayTime);
		_file.getInteger(item.m_lExitRunwayTime);
		_file.getInteger(item.m_lOccupancyTime);
		_file.getInteger(item.m_lWaitTimeFroRunwayCrossings);
		_file.getFloat(item.m_dRunwayCrossingsSpeed);
		_file.getLine();
		m_vResult.push_back(item);
	}
	return TRUE;
}

BOOL CAirsideRunwayCrossingBaseDetailResult::ExportReportData(ArctermFile& _file,CString& Errmsg)
{
	int nCount = (int)m_vResult.size();
	_file.writeInt(nCount);
	_file.writeLine();
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		DetailRunwayCrossingsItem& item = m_vResult[nIndex];
		_file.writeField(item.m_sRunway);
		_file.writeField(item.m_sEnterTaxiway);
		_file.writeField(item.m_sExitTaxiway);
		_file.writeField(item.m_sFlightID);
		_file.writeField(item.m_sActype);
		_file.writeField(item.m_sNodeName);
		_file.writeInt(item.m_lEnterRunwayTime);
		_file.writeInt(item.m_lExitRunwayTime);
		_file.writeInt(item.m_lOccupancyTime);
		_file.writeInt(item.m_lWaitTimeFroRunwayCrossings);
		_file.writeFloat((float)item.m_dRunwayCrossingsSpeed);
		_file.writeLine();
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////////////
CAirsideRunwayCrossingsBaseSummaryResult::CAirsideRunwayCrossingsBaseSummaryResult()
:m_pChartResult(NULL)
{

}

CAirsideRunwayCrossingsBaseSummaryResult::~CAirsideRunwayCrossingsBaseSummaryResult()
{

}

void CAirsideRunwayCrossingsBaseSummaryResult::GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter)
{
	if (pParameter == NULL)
		return;
	AirsideRunwayCrossingsParameter* pRunwayCrossingsPara = (AirsideRunwayCrossingsParameter*)pParameter;

	CString strDescFilepath = (*pCBGetLogFilePath)(AirsideFlightDescFileReport);
	CString strEventFilePath = (*pCBGetLogFilePath)(AirsideFlightEventFileReport);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	m_vResult.clear();
	ARCBaseLog<AirsideFlightLogItem> mFlightLogData;
	mFlightLogData.OpenInput(strDescFilepath.GetString(), strEventFilePath.GetString());

	std::vector<AirsideRunwayCrossigsLog*> vFitRunwayCrossingLog;
	vFitRunwayCrossingLog.clear();

	std::vector<SummaryRunwayItem>vResult;
	vResult.clear();


	int nCount = mFlightLogData.getItemCount();
	for (int i = 0; i < nCount; i++)
	{
		vFitRunwayCrossingLog.clear();
		mFlightLogData.LoadItem(mFlightLogData.ItemAt(i));
		AirsideFlightLogItem item = mFlightLogData.ItemAt(i);
		int nLogCount = (int)item.mpEventList->mEventList.size();
		for (int j = 0; j < nLogCount; j++)
		{
			ARCEventLog* pLog = item.mpEventList->mEventList.at(j);
			if (!pLog->IsA(typeof(AirsideRunwayCrossigsLog)))
				continue;

			AirsideRunwayCrossigsLog* pRunwayCrossingsLog = (AirsideRunwayCrossigsLog*)pLog;
			if (pRunwayCrossingsPara->fit(pRunwayCrossingsLog))
			{
				vFitRunwayCrossingLog.push_back(pRunwayCrossingsLog);
			}
		}

		

		AirsideFlightLogDesc fltdesc = item.mFltDesc;
		AirsideFlightDescStruct descStruct;
		fltdesc.getFlightDescStruct(descStruct);
		std::vector<AirsideRunwayCrossigsLog*> vRunwayCrossingsLog;
		vRunwayCrossingsLog.clear();
		
		for (int nIndex = 0; nIndex < (int)vFitRunwayCrossingLog.size();nIndex++)
		{
			AirsideRunwayCrossigsLog* pLog = vFitRunwayCrossingLog.at(nIndex);
			if (pLog && pLog->state != AirsideRunwayCrossigsLog::OnExitRunway)
			{
				vRunwayCrossingsLog.push_back(pLog);
			}
			else
			{
				vRunwayCrossingsLog.push_back(pLog);
				SummaryRunwayItem runwayItem;
				runwayItem.m_sRunway = pLog->sRunway.c_str();
				
				long time = 0;
				double dSpeed = 0.0;
				bool bHaveEnterLog = false;   //have OnEnterRunway log
				bool bHaveWaitLog = false;  // have OnWaitingCrossRunway log

				for (int nIdx = 0; nIdx < (int)vRunwayCrossingsLog.size(); nIdx++)
				{
					AirsideRunwayCrossigsLog* pCrossingLog = vRunwayCrossingsLog[nIdx];
					switch(pCrossingLog->state)
					{
					case AirsideRunwayCrossigsLog::OnWaitingCrossRunway:
						{
							bHaveWaitLog = true;
							if (time)
							{
								time = min(time,pCrossingLog->time);
							}
							else
							{
								time = pCrossingLog->time;
							}
						}
						break;
					case AirsideRunwayCrossigsLog::OnEnterRunway:
						{
							bHaveEnterLog = true;
							if (bHaveWaitLog)
							{
								runwayItem.m_lWaitTime = (pCrossingLog->time - time)/100;
							}
							else
							{
								runwayItem.m_lWaitTime = 0;
							}
							runwayItem.m_lEnterRunway = pCrossingLog->time/100;
							runwayItem.m_sNodeName = pCrossingLog->sNodeName.c_str();
							runwayItem.m_sEnterTaxiway = pLog->sTaxiway.c_str();
						}
						break;
					case AirsideRunwayCrossigsLog::OnExitRunway:
						{
							//runwayItem.m_ = pLog->sTaxiway.c_str();
						}
						break;
					default:
						ASSERT(0);
						break;
					}
				}
				if(bHaveEnterLog)
				{
					vResult.push_back(runwayItem);
					CString strLog;
					strLog.Format("%s, %s, %s, %5d, %d\r\n", 
						runwayItem.m_sRunway, 
						runwayItem.m_sNodeName, 
						runwayItem.m_sEnterTaxiway, 
						runwayItem.m_lWaitTime, 
						runwayItem.m_lEnterRunway);
					TRACE(strLog);
				}
				vRunwayCrossingsLog.clear();
			}
		}		
	}

	std::vector<std::pair<CString,ALTObjectID> >vRunwayList = GetRunwayList(vResult); 
	for (int i = 0; i < (int)vRunwayList.size(); i++)
	{
		SummaryRunwayCrossingsItem summaryItem;
		summaryItem.m_sRunway = vRunwayList[i].second.GetIDString();
		summaryItem.m_sNodeName = vRunwayList[i].first;
		summaryItem.m_sEnterTaxiway = GetTaxiwayString(summaryItem,vResult);
		GetMinCrossingsCountAndIntervalIndex(pRunwayCrossingsPara,summaryItem,vResult);
		GetMaxCrossingsCountAndIntervalIndex(pRunwayCrossingsPara,summaryItem,vResult);
		GetMinWaitTimeAndIntervalMinIndex(pRunwayCrossingsPara,summaryItem,vResult);
		GetMaxWaitTimeAndItervalMaxIndex(pRunwayCrossingsPara,summaryItem,vResult);
		GetAverageCrossingsCountAndTotal(pRunwayCrossingsPara,summaryItem,vResult);
		GetAverageWaitTimeAndTotal(pRunwayCrossingsPara,summaryItem,vResult);

		InitStaticData(pRunwayCrossingsPara,SummaryRunwayCrossingsItem::SM_CROSSSINGS,summaryItem,vResult);
		InitStaticData(pRunwayCrossingsPara,SummaryRunwayCrossingsItem::SM_WAITTIME,summaryItem,vResult);
		m_vResult.push_back(summaryItem);
	}
	RefreshReport(pRunwayCrossingsPara);
}

CString CAirsideRunwayCrossingsBaseSummaryResult::GetTaxiwayString(SummaryRunwayCrossingsItem& item,std::vector<SummaryRunwayItem>&vResult)
{
	for (int i = 0; i < (int)vResult.size(); i++)
	{
		CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayItem runwayItem = vResult[i];
		if (!item.m_sNodeName.CompareNoCase(runwayItem.m_sNodeName))
		{
			return runwayItem.m_sEnterTaxiway;
		}
	}
	return _T("");
}

void CAirsideRunwayCrossingsBaseSummaryResult::InitStaticData(CParameters* pPara,SummaryRunwayCrossingsItem::SummaryRunwayCrossingType emType,SummaryRunwayCrossingsItem& item,std::vector<SummaryRunwayItem>&vResult)
{
// 	std::vector<std::pair<CString,ALTObjectID> >vRunwayList = GetRunwayList(vResult);
// 	
// 	int nRunwayCount = (int)vRunwayList.size();
// 
	// 	for (int nRunway = 0; nRunway < nRunwayCount; ++ nRunway)
	// 	{
	CAirsideRunwayCrossingsBaseSummaryResult::StatisticsSummaryRunwayCrossingsItem statisticItem;
	CStatisticalTools<double> statisticalTool;

	int nCount = vResult.size();
	if(nCount == 0)
		return;


	ElapsedTime eStartTime = pPara->getStartTime();
	ElapsedTime eEndTime = pPara->getEndTime();
	ElapsedTime eInterval(pPara->getInterval());
	if (eInterval == ElapsedTime(0L))
	{
		eInterval = ElapsedTime(60 * 60L);
	}
	long lWaiTime;
	long lCount;
	for (;eStartTime < eEndTime; eStartTime += eInterval)
	{
		if(emType == SummaryRunwayCrossingsItem::SM_CROSSSINGS)
		{
			//statistical tool
			lCount = GetCrossingsCount(eStartTime,eEndTime,eInterval,item,vResult);
			statisticalTool.AddNewData(lCount);
		}
		else if(emType == SummaryRunwayCrossingsItem::SM_WAITTIME)
		{
			lWaiTime = GetWaitTime(eStartTime,eEndTime,eInterval,item,vResult);
			statisticalTool.AddNewData(lWaiTime);
		}
		else 
		{
			continue;
		}
	}
	statisticalTool.SortData();

	statisticItem.m_nQ1 = static_cast<int>(statisticalTool.GetPercentile(25));
	statisticItem.m_nQ2 = static_cast<int>(statisticalTool.GetPercentile(50));
	statisticItem.m_nQ3 = static_cast<int>(statisticalTool.GetPercentile(75));
	statisticItem.m_nP1 = static_cast<int>(statisticalTool.GetPercentile(1));
	statisticItem.m_nP5 = static_cast<int>(statisticalTool.GetPercentile(5));
	statisticItem.m_nP10 = static_cast<int>(statisticalTool.GetPercentile(10));
	statisticItem.m_nP90 = static_cast<int>(statisticalTool.GetPercentile(90));
	statisticItem.m_nP95 = static_cast<int>(statisticalTool.GetPercentile(95));
	statisticItem.m_nP99 = static_cast<int>(statisticalTool.GetPercentile(99));
	statisticItem.m_nStdDev = static_cast<int>(statisticalTool.GetSigma());
	/*	}*/
	if(emType == SummaryRunwayCrossingsItem::SM_CROSSSINGS)
	{
		item.m_staCrossingCount = statisticItem;
	}
	else if(emType == SummaryRunwayCrossingsItem::SM_WAITTIME)
	{
		item.m_staWaitTime = statisticItem;
	}
}

std::vector<std::pair<CString,ALTObjectID> > CAirsideRunwayCrossingsBaseSummaryResult::GetRunwayList(std::vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayItem>& vResult)
{
	std::vector<std::pair<CString,ALTObjectID> > vRunwayList;
	vRunwayList.clear();
	for (int i = 0; i < (int)vResult.size(); i++)
	{
		ALTObjectID runwayID(vResult[i].m_sRunway);
		std::pair<CString,ALTObjectID> itemPair;
		itemPair.first = vResult[i].m_sNodeName;
		itemPair.second = runwayID;
		if (std::find(vRunwayList.begin(),vRunwayList.end(),itemPair) == vRunwayList.end())
		{
			vRunwayList.push_back(std::make_pair(vResult[i].m_sNodeName,runwayID));
		}
	}
	return vRunwayList;
}
long CAirsideRunwayCrossingsBaseSummaryResult::GetCrossingsCount(const ElapsedTime& startTime,const ElapsedTime& endTime,const ElapsedTime& eIntervalTime,const SummaryRunwayCrossingsItem& item,std::vector<SummaryRunwayItem>&vResult)
{
	long lCrossingCount = 0;
	for (int i = 0; i < (int)vResult.size(); i++)
	{
		SummaryRunwayItem& runwayItem = vResult[i];
		if (!runwayItem.m_sNodeName.CompareNoCase(item.m_sNodeName) \
			&& startTime <= (runwayItem.m_lEnterRunway)  \
			&& (startTime + eIntervalTime) > (runwayItem.m_lEnterRunway))
		{
			lCrossingCount++;
		}
	}
	return lCrossingCount;
}

void CAirsideRunwayCrossingsBaseSummaryResult::GetAverageCrossingsCountAndTotal(CParameters* pPara,SummaryRunwayCrossingsItem& item,std::vector<SummaryRunwayItem>&vResult)
{
	ElapsedTime eStartTime = pPara->getStartTime();
	ElapsedTime eEndTime = pPara->getEndTime();
	ElapsedTime eInterval(pPara->getInterval());
	if (eInterval == ElapsedTime(0L))
	{
		eInterval = ElapsedTime(60 * 60L);
	}

	long lCrossingCount = 0;
	long nSize = 0;
	for (;eStartTime < eEndTime; eStartTime += eInterval)
	{
		for (int i = 0; i < (int)vResult.size(); i++)
		{
			SummaryRunwayItem& runwayItem = vResult[i];
			if (!runwayItem.m_sNodeName.CompareNoCase(item.m_sNodeName) \
				&& eStartTime <= (runwayItem.m_lEnterRunway/100)  \
				&& (eStartTime + eInterval) > (runwayItem.m_lEnterRunway/100) )
			{
				lCrossingCount++;
			}
		}
		nSize++;
	}
	item.m_dAverageCrosings = (lCrossingCount*1.0)/nSize;
	item.m_lTotalCrossings = lCrossingCount;
}

void CAirsideRunwayCrossingsBaseSummaryResult::GetMinCrossingsCountAndIntervalIndex(CParameters* pPara,SummaryRunwayCrossingsItem& item,std::vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayItem>&vResult)
{
	ElapsedTime eStartTime = pPara->getStartTime();
	ElapsedTime eEndTime = pPara->getEndTime();
	ElapsedTime eInterval(pPara->getInterval());
	if (eInterval == ElapsedTime(0L))
	{
		eInterval = ElapsedTime(60 * 60L);
	}

	long lMinCount = LONG_MAX;
	long nIndex = 0;
	std::vector<long> vMinCrossingIntervals;
	vMinCrossingIntervals.clear();
	for (;eStartTime < eEndTime; eStartTime += eInterval)
	{
		int nCrossingCount = GetCrossingsCount(eStartTime,eEndTime,eInterval,item,vResult);
		if(nCrossingCount < lMinCount)
		{
			lMinCount = nCrossingCount;
			vMinCrossingIntervals.clear();
			vMinCrossingIntervals.push_back(nIndex);
		}
		else if(nCrossingCount == lMinCount)
		{
			vMinCrossingIntervals.push_back(nIndex);
		}
		nIndex++;
	}

	item.m_lMinCrossings = lMinCount;
	item.m_vMinCrossingIntervals = vMinCrossingIntervals; 
}

void CAirsideRunwayCrossingsBaseSummaryResult::GetMaxCrossingsCountAndIntervalIndex(CParameters* pPara,SummaryRunwayCrossingsItem& item,std::vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayItem>&vResult)
{
	ElapsedTime eStartTime = pPara->getStartTime();
	ElapsedTime eEndTime = pPara->getEndTime();
	ElapsedTime eInterval(pPara->getInterval());
	if (eInterval == ElapsedTime(0L))
	{
		eInterval = ElapsedTime(60 * 60L);
	}

	long lMaxCount = 0;
	long nIndex = 0;
	std::vector<long> vMaxCrossingIntervals;
	vMaxCrossingIntervals.clear();
	for (;eStartTime < eEndTime; eStartTime += eInterval)
	{
		int nCrossingCount = GetCrossingsCount(eStartTime,eEndTime,eInterval,item,vResult);
		if(nCrossingCount > lMaxCount)
		{
			lMaxCount = nCrossingCount;
			vMaxCrossingIntervals.clear();
			vMaxCrossingIntervals.push_back(nIndex);
		}
		else if(nCrossingCount == lMaxCount)
		{
			vMaxCrossingIntervals.push_back(nIndex);
		}
		nIndex++;
	}

	item.m_lMaxCrossings = lMaxCount;
	item.m_vMaxCrossingIntervals = vMaxCrossingIntervals; 
}

long CAirsideRunwayCrossingsBaseSummaryResult::GetItemMinWaitTime(const ElapsedTime& startTime,const ElapsedTime& endTime,const ElapsedTime& eIntervalTime,const SummaryRunwayCrossingsItem& item,std::vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayItem>&vResult)
{
	long lMinWaitTime = LONG_MAX;
	for (int i = 0; i < (int)vResult.size(); i++)
	{
		SummaryRunwayItem& runwayItem = vResult[i];
		if (!runwayItem.m_sNodeName.CompareNoCase(item.m_sNodeName) \
			&& startTime <= (runwayItem.m_lEnterRunway)  \
			&& (startTime + eIntervalTime) > (runwayItem.m_lEnterRunway) )
		{
			if (runwayItem.m_lWaitTime < lMinWaitTime)
			{
				lMinWaitTime = runwayItem.m_lWaitTime;
			}
		}
	}
	if (lMinWaitTime == LONG_MAX)
	{
		lMinWaitTime = 0;
	}
	return lMinWaitTime;
}

void CAirsideRunwayCrossingsBaseSummaryResult::GetAverageWaitTimeAndTotal(CParameters* pPara,SummaryRunwayCrossingsItem& item,std::vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayItem>&vResult)
{
	ElapsedTime eStartTime = pPara->getStartTime();
	ElapsedTime eEndTime = pPara->getEndTime();
	ElapsedTime eInterval(pPara->getInterval());
	if (eInterval == ElapsedTime(0L))
	{
		eInterval = ElapsedTime(60 * 60L);
	}

	long lTotalWaitTime = 0;
	long nSize = 0;
	for (;eStartTime < eEndTime; eStartTime += eInterval)
	{
		for (int i = 0; i < (int)vResult.size(); i++)
		{
			SummaryRunwayItem& runwayItem = vResult[i];
			if (!runwayItem.m_sNodeName.CompareNoCase(item.m_sNodeName) \
				&& eStartTime <= (runwayItem.m_lEnterRunway/100)  \
				&& (eStartTime + eInterval) > (runwayItem.m_lEnterRunway/100) )
			{
				lTotalWaitTime += runwayItem.m_lWaitTime;
			}
		}
		nSize++;
	}
	item.m_lAverageWaitTime = lTotalWaitTime/nSize;
	item.m_lTotalWaitTime = lTotalWaitTime;
}

long CAirsideRunwayCrossingsBaseSummaryResult::GetItemMaxWaitTime(const ElapsedTime& startTime,const ElapsedTime& endTime,const ElapsedTime& eIntervalTime,const SummaryRunwayCrossingsItem& item,std::vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayItem>&vResult)
{
	long lMaxWaitTime = 0;
	for (int i = 0; i < (int)vResult.size(); i++)
	{
		SummaryRunwayItem& runwayItem = vResult[i];
		if (!runwayItem.m_sNodeName.CompareNoCase(item.m_sNodeName) \
			&& startTime <= (runwayItem.m_lEnterRunway/100) \
			&& (startTime + eIntervalTime) > (runwayItem.m_lEnterRunway/100))
		{
			if (runwayItem.m_lWaitTime > lMaxWaitTime)
			{
				lMaxWaitTime = runwayItem.m_lWaitTime;
			}
		}
	}
	return lMaxWaitTime;
}

long CAirsideRunwayCrossingsBaseSummaryResult::GetWaitTime(const ElapsedTime& startTime,const ElapsedTime& endTime,const ElapsedTime& eIntervalTime,const SummaryRunwayCrossingsItem& item,std::vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayItem>&vResult)
{
	long lWaitTime = 0;
	for (int i = 0; i < (int)vResult.size(); i++)
	{
		SummaryRunwayItem& runwayItem = vResult[i];
		if (!runwayItem.m_sNodeName.CompareNoCase(item.m_sNodeName) \
			&& startTime <= (runwayItem.m_lEnterRunway)  \
			&& (startTime + eIntervalTime) > (runwayItem.m_lEnterRunway) )
		{
			lWaitTime += runwayItem.m_lWaitTime;
		}
	}
	return lWaitTime;
}

void CAirsideRunwayCrossingsBaseSummaryResult::GetMinWaitTimeAndIntervalMinIndex(CParameters* pPara,SummaryRunwayCrossingsItem& item,std::vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayItem>&vResult)
{
	ElapsedTime eStartTime = pPara->getStartTime();
	ElapsedTime eEndTime = pPara->getEndTime();
	ElapsedTime eInterval(pPara->getInterval());
	if (eInterval == ElapsedTime(0L))
	{
		eInterval = ElapsedTime(60 * 60L);
	}

	long lMinWaiTime = LONG_MAX;
	long nMinIndex = 0;
	long nIndex = 0;
	for (;eStartTime < eEndTime + eInterval; eStartTime += eInterval)
	{
		long lWaitTime = GetWaitTime(eStartTime,eEndTime,eInterval,item,vResult);
		if (lMinWaiTime > lWaitTime)
		{
			lMinWaiTime = lWaitTime;
			nMinIndex = nIndex;
		}
		nIndex++;
	}

	item.m_lMinWaitTime = lMinWaiTime;
	item.m_lIntervalMinWaitTime = nMinIndex;
}

void CAirsideRunwayCrossingsBaseSummaryResult::GetMaxWaitTimeAndItervalMaxIndex(CParameters* pPara,SummaryRunwayCrossingsItem& item,std::vector<CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayItem>&vResult)
{
	ElapsedTime eStartTime = pPara->getStartTime();
	ElapsedTime eEndTime = pPara->getEndTime();
	ElapsedTime eInterval(pPara->getInterval());
	if (eInterval == ElapsedTime(0L))
	{
		eInterval = ElapsedTime(60 * 60L);
	}

	long lMaxWaitTime = 0;
	long nMaxIndex = 0;
	long nIndex = 0;
	for (;eStartTime < eEndTime; eStartTime += eInterval)
	{
		long lWaitTime = GetWaitTime(eStartTime,eEndTime,eInterval,item,vResult);
		if (lMaxWaitTime < lWaitTime)
		{
			lMaxWaitTime = lWaitTime;
			nMaxIndex = nIndex;
		}
		nIndex++;
	}

	item.m_lMaxWaitTime = lMaxWaitTime;
	item.m_lIntervalMaxWaitTime = nMaxIndex;
}

void CAirsideRunwayCrossingsBaseSummaryResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if (m_pChartResult)
	{
		m_pChartResult->Draw3DChart(chartWnd,pParameter);
	}
}

void CAirsideRunwayCrossingsBaseSummaryResult::RefreshReport(CParameters * parameter)
{
	AirsideRunwayCrossingsParameter* pParam = (AirsideRunwayCrossingsParameter*)parameter;
	ASSERT(pParam != NULL);

	if (NULL != m_pChartResult)
	{
		delete m_pChartResult;
		m_pChartResult = NULL;
	}

	switch(pParam->getSubType())
	{
	case SM_COUNT:
		{
			m_pChartResult = new CRunwayCrossingSummaryCountResult;
			CRunwayCrossingSummaryCountResult* pResult = (CRunwayCrossingSummaryCountResult*)m_pChartResult;
			pResult->GenerateResult(m_vResult, parameter);
		}
		break;
	case SM_WAITTIME:
		{
			m_pChartResult = new CRunwayCrossingSummaryWaitTimeResult;
			CRunwayCrossingSummaryWaitTimeResult* pResult = (CRunwayCrossingSummaryWaitTimeResult*)m_pChartResult;
			pResult->GenerateResult(m_vResult, parameter);
		}
		break;
	default:
		break;
	}
}

void CAirsideRunwayCrossingsBaseSummaryResult::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("Runway"), LVCFMT_LEFT, 100);	
	cxListCtrl.InsertColumn(1, _T("Taxiway@entry"), LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(2, _T("Min crossings"), LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(3, _T("Interval@Min crossings"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(4, _T("Average crossing"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(5, _T("Max crossings"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(6, _T("Interval@max crossings"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(7, _T("Total crossings"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(8, _T("Q1 crossing"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(9, _T("Q2 crossing"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(10, _T("Q3 crossing"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(11, _T("P1 crossing"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(12, _T("P5 crossing"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(13, _T("P10 crossing"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(14, _T("P90 crossing"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(15, _T("P95 crossing"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(16, _T("P99 crossing"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(17, _T("Sigma crossing"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(18, _T("Min waiting time"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(19, _T("Interval @ min waiting time"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(20, _T("Average waiting time"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(21, _T("Max waiting time"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(22, _T("Interval@max waiting time"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(23, _T("Total waiting time"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(24, _T("Q1 waiting time"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(25, _T("Q2 waiting time"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(26, _T("Q3 waiting time"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(27, _T("P1 waiting time"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(28, _T("P5 waiting time"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(29, _T("P10 waiting time"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(30, _T("P90 waiting time"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(31, _T("P95 waiting time"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(32, _T("P99 waiting time"),LVCFMT_LEFT,120);
	cxListCtrl.InsertColumn(33, _T("Sigma waiting time"),LVCFMT_LEFT,120);

}

void CAirsideRunwayCrossingsBaseSummaryResult::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	for (int i=0; i<(int)m_vResult.size(); i++)
	{
		SummaryRunwayCrossingsItem runwayCrossingsItem = m_vResult[i];
		cxListCtrl.InsertItem(i, runwayCrossingsItem.m_sRunway);

		cxListCtrl.SetItemText(i, 1,runwayCrossingsItem.m_sEnterTaxiway);

		CString strMinCrossings(_T(""));
		strMinCrossings.Format(_T("%d"),runwayCrossingsItem.m_lMinCrossings);
		cxListCtrl.SetItemText(i, 2,strMinCrossings);

		CString strInterMinCrossings(_T(""));
		CString strTemp(_T(""));
		int nCount = runwayCrossingsItem.m_vMinCrossingIntervals.size();
		for(int j=0; j<nCount; j++)
		{
			strTemp.Format(_T("%dth"), runwayCrossingsItem.m_vMinCrossingIntervals.at(j)+1);
			strInterMinCrossings += strTemp;
			if(j != (nCount - 1))
			{
				strInterMinCrossings += CString(", ");
			}
		}
		cxListCtrl.SetItemText(i, 3,strInterMinCrossings);

		CString strAverageCrossings(_T(""));
		strAverageCrossings.Format(_T("%.2f"),runwayCrossingsItem.m_dAverageCrosings);
		cxListCtrl.SetItemText(i, 4,strAverageCrossings);


		CString strMaxCrossigns(_T(""));
		strMaxCrossigns.Format(_T("%d"),runwayCrossingsItem.m_lMaxCrossings);
		cxListCtrl.SetItemText(i, 5,strMaxCrossigns);

		CString strInterMaxCrossings(_T(""));
		nCount = runwayCrossingsItem.m_vMaxCrossingIntervals.size();
		for(int j=0; j<nCount; j++)
		{
			strTemp.Format(_T("%dth"), runwayCrossingsItem.m_vMaxCrossingIntervals.at(j)+1);
			strInterMaxCrossings += strTemp;
			if(j != (nCount - 1))
			{
				strInterMaxCrossings += CString(", ");
			}
		}
		cxListCtrl.SetItemText(i, 6,strInterMaxCrossings);

		CString strTotal(_T(""));
		strTotal.Format(_T("%d"),runwayCrossingsItem.m_lTotalCrossings);
		cxListCtrl.SetItemText(i, 7,strTotal);

		CString strQ1Crossings(_T(""));
		strQ1Crossings.Format(_T("%d"),runwayCrossingsItem.m_staCrossingCount.m_nQ1);
		cxListCtrl.SetItemText(i, 8,strQ1Crossings);

		CString strQ2Crossings(_T(""));
		strQ2Crossings.Format(_T("%d"),runwayCrossingsItem.m_staCrossingCount.m_nQ2);
		cxListCtrl.SetItemText(i, 9,strQ2Crossings);

		CString strQ3Crossings(_T(""));
		strQ3Crossings.Format(_T("%d"),runwayCrossingsItem.m_staCrossingCount.m_nQ3);
		cxListCtrl.SetItemText(i, 10,strQ3Crossings);

		CString strP1Crossings(_T(""));
		strP1Crossings.Format(_T("%d"),runwayCrossingsItem.m_staCrossingCount.m_nP1);
		cxListCtrl.SetItemText(i, 11,strP1Crossings);

		CString strP5Crossings(_T(""));
		strP5Crossings.Format(_T("%d"),runwayCrossingsItem.m_staCrossingCount.m_nP5);
		cxListCtrl.SetItemText(i, 12,strP5Crossings);

		CString strP10Crossings(_T(""));
		strP10Crossings.Format(_T("%d"),runwayCrossingsItem.m_staCrossingCount.m_nP10);
		cxListCtrl.SetItemText(i, 13,strP10Crossings);

		CString strP90Crossings(_T(""));
		strP90Crossings.Format(_T("%d"),runwayCrossingsItem.m_staCrossingCount.m_nP90);
		cxListCtrl.SetItemText(i, 14,strP90Crossings);

		CString strP95Crossings(_T(""));
		strP95Crossings.Format(_T("%d"),runwayCrossingsItem.m_staCrossingCount.m_nP95);
		cxListCtrl.SetItemText(i, 15,strP95Crossings);

		CString strP99Crossings(_T(""));
		strP99Crossings.Format(_T("%d"),runwayCrossingsItem.m_staCrossingCount.m_nP99);
		cxListCtrl.SetItemText(i, 16,strP99Crossings);

		CString strSigmaCrossings(_T(""));
		strSigmaCrossings.Format(_T("%d"),runwayCrossingsItem.m_staCrossingCount.m_nStdDev);
		cxListCtrl.SetItemText(i, 17,strSigmaCrossings);

		CString strMinWaitTime(_T(""));
		strMinWaitTime.Format(_T("%d"),runwayCrossingsItem.m_lMinWaitTime);
		cxListCtrl.SetItemText(i, 18,strMinWaitTime);

		CString strIntervalMinWaitTime(_T(""));
		strIntervalMinWaitTime.Format(_T("%dth"),runwayCrossingsItem.m_lIntervalMinWaitTime);
		cxListCtrl.SetItemText(i, 19,strIntervalMinWaitTime);

		CString strAverageWaitTime(_T(""));
		strAverageWaitTime.Format(_T("%d"),runwayCrossingsItem.m_lAverageWaitTime);
		cxListCtrl.SetItemText(i, 20,strAverageWaitTime);

		CString strMaxWaitTime(_T(""));
		strMaxWaitTime.Format(_T("%d"),runwayCrossingsItem.m_lMaxWaitTime);
		cxListCtrl.SetItemText(i, 21,strMaxWaitTime);

		CString strIntervalMaxWaitTime(_T(""));
		strIntervalMaxWaitTime.Format(_T("%dth"),runwayCrossingsItem.m_lIntervalMaxWaitTime);
		cxListCtrl.SetItemText(i, 22,strIntervalMaxWaitTime);

		CString strTotalWaitTime(_T(""));
		strTotalWaitTime.Format(_T("%d"),runwayCrossingsItem.m_lTotalWaitTime);
		cxListCtrl.SetItemText(i, 23,strTotalWaitTime);

		CString strQ1WaitTime(_T(""));
		strQ1WaitTime.Format(_T("%d"),runwayCrossingsItem.m_staWaitTime.m_nQ1);
		cxListCtrl.SetItemText(i, 24,strQ1WaitTime);

		CString strQ2WaitTime(_T(""));
		strQ2WaitTime.Format(_T("%d"),runwayCrossingsItem.m_staWaitTime.m_nQ2);
		cxListCtrl.SetItemText(i, 25,strQ2WaitTime);

		CString strQ3WaitTime(_T(""));
		strQ3WaitTime.Format(_T("%d"),runwayCrossingsItem.m_staWaitTime.m_nQ3);
		cxListCtrl.SetItemText(i, 26,strQ3WaitTime);

		CString strP1WaitTime(_T(""));
		strP1WaitTime.Format(_T("%d"),runwayCrossingsItem.m_staWaitTime.m_nP1);
		cxListCtrl.SetItemText(i, 27,strP1WaitTime);

		CString strP5WaitTime(_T(""));
		strP5WaitTime.Format(_T("%d"),runwayCrossingsItem.m_staWaitTime.m_nP5);
		cxListCtrl.SetItemText(i, 28,strP5WaitTime);

		CString strP10WaitTime(_T(""));
		strP10WaitTime.Format(_T("%d"),runwayCrossingsItem.m_staWaitTime.m_nP10);
		cxListCtrl.SetItemText(i, 29,strP10WaitTime);

		CString strP90WaitTime(_T(""));
		strP90WaitTime.Format(_T("%d"),runwayCrossingsItem.m_staWaitTime.m_nP90);
		cxListCtrl.SetItemText(i, 30,strP90WaitTime);

		CString strP95WaitTime(_T(""));
		strP95WaitTime.Format(_T("%d"),runwayCrossingsItem.m_staWaitTime.m_nP95);
		cxListCtrl.SetItemText(i, 31,strP95WaitTime);

		CString strP99WaitTime(_T(""));
		strP99WaitTime.Format(_T("%d"),runwayCrossingsItem.m_staWaitTime.m_nP99);
		cxListCtrl.SetItemText(i, 32,strP99WaitTime);

		CString strSigmaWaitTime(_T(""));
		strSigmaWaitTime.Format(_T("%d"),runwayCrossingsItem.m_staWaitTime.m_nStdDev);
		cxListCtrl.SetItemText(i, 33,strSigmaWaitTime);
	}
}

BOOL CAirsideRunwayCrossingsBaseSummaryResult::ExportListData(ArctermFile& _file,CParameters * parameter)
{
	return TRUE;
}

BOOL CAirsideRunwayCrossingsBaseSummaryResult::ImportReportData(ArctermFile& _file,CString& Errmsg)
{
	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nIndex = 0; nIndex < nCount; ++nIndex)
	{
		CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayCrossingsItem item;

		//runway information
		_file.getField(item.m_sRunway.GetBuffer(1024),1024);
		_file.getField(item.m_sEnterTaxiway.GetBuffer(1024),1024);
		_file.getField(item.m_sNodeName.GetBuffer(1024),1024);
		_file.getInteger(item.m_lMinCrossings);
		char buf[16] = {0};
		_file.getSubField(buf, ';');
		int nCount1 = atoi(buf);
		for(int i=0; i<nCount1; i++)
		{
			_file.getSubField(buf, ';');
			item.m_vMinCrossingIntervals.push_back(atoi(buf));
		}
		_file.getInteger(item.m_lMaxCrossings);
		_file.getFloat(item.m_dAverageCrosings);
		_file.getSubField(buf, ';');
		nCount1 = atoi(buf);
		for(int i=0; i<nCount1; i++)
		{
			_file.getSubField(buf, ';');
			item.m_vMaxCrossingIntervals.push_back(atoi(buf));
		}
		_file.getInteger(item.m_lTotalCrossings);
		_file.getInteger(item.m_lMinWaitTime);
		_file.getInteger(item.m_lIntervalMinWaitTime);
		_file.getInteger(item.m_lMaxWaitTime);
		_file.getInteger(item.m_lAverageWaitTime);
		_file.getInteger(item.m_lIntervalMaxWaitTime);
		_file.getInteger(item.m_lTotalWaitTime);

		//runway crossings count
		{
			CAirsideRunwayCrossingsBaseSummaryResult::StatisticsSummaryRunwayCrossingsItem& crossingsCountItem = item.m_staCrossingCount;
			_file.getInteger(crossingsCountItem.m_nQ1);
			_file.getInteger(crossingsCountItem.m_nQ2);

			_file.getInteger(crossingsCountItem.m_nQ3);
			_file.getInteger(crossingsCountItem.m_nP1);

			_file.getInteger(crossingsCountItem.m_nP5);
			_file.getInteger(crossingsCountItem.m_nP10);

			_file.getInteger(crossingsCountItem.m_nP90);
			_file.getInteger(crossingsCountItem.m_nP95);
			_file.getInteger(crossingsCountItem.m_nP99);
			_file.getInteger(crossingsCountItem.m_nStdDev);
		}



		//runway crossings wait times
		{
			CAirsideRunwayCrossingsBaseSummaryResult::StatisticsSummaryRunwayCrossingsItem& waitTimeItem = item.m_staWaitTime;
			_file.getInteger(waitTimeItem.m_nQ1);
			_file.getInteger(waitTimeItem.m_nQ2);

			_file.getInteger(waitTimeItem.m_nQ3);
			_file.getInteger(waitTimeItem.m_nP1);

			_file.getInteger(waitTimeItem.m_nP5);
			_file.getInteger(waitTimeItem.m_nP10);

			_file.getInteger(waitTimeItem.m_nP90);
			_file.getInteger(waitTimeItem.m_nP95);
			_file.getInteger(waitTimeItem.m_nP99);
			_file.getInteger(waitTimeItem.m_nStdDev);
		}


		m_vResult.push_back(item);
		_file.getLine();

	}
	return TRUE;
}

BOOL CAirsideRunwayCrossingsBaseSummaryResult::ExportReportData(ArctermFile& _file,CString& Errmsg)
{
	int nCount = (int)m_vResult.size();
	_file.writeInt(nCount);
	_file.writeLine();

	for (int nIndex = 0; nIndex < nCount; ++ nIndex)
	{
		CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayCrossingsItem& item = m_vResult[nIndex];

		_file.writeField(item.m_sRunway);
		_file.writeField(item.m_sEnterTaxiway);
		_file.writeField(item.m_sNodeName);
		_file.writeInt(item.m_lMinCrossings);
		int nCount1 = (int)item.m_vMinCrossingIntervals.size();
		char buf[16] = {0};
		_file.appendValue(",");
		_file.appendValue(_itoa(nCount1, buf, 10));
		for(int i=0; i<nCount1; i++)
		{
			_file.appendValue(";");
			memset(buf, 0, 16);
			_file.appendValue(_itoa(item.m_vMinCrossingIntervals.at(i), buf, 10));
		}
		_file.writeInt(item.m_lMaxCrossings);
		_file.writeFloat((float)item.m_dAverageCrosings);
		nCount1 = (int)item.m_vMaxCrossingIntervals.size();
		_file.appendValue(",");
		_file.appendValue(_itoa(nCount1, buf, 10));
		for(int i=0; i<nCount1; i++)
		{
			_file.appendValue(";");
			memset(buf, 0, 16);
			_file.appendValue(_itoa(item.m_vMaxCrossingIntervals.at(i), buf, 10));
		}
		_file.writeInt(item.m_lTotalCrossings);
		_file.writeInt(item.m_lMinWaitTime);
		_file.writeInt(item.m_lIntervalMinWaitTime);
		_file.writeInt(item.m_lMaxWaitTime);
		_file.writeInt(item.m_lAverageWaitTime);
		_file.writeInt(item.m_lIntervalMaxWaitTime);
		_file.writeInt(item.m_lTotalWaitTime);

		//runway crossings count
		{
			StatisticsSummaryRunwayCrossingsItem& CrossingCountItem = item.m_staCrossingCount;

			_file.writeInt(CrossingCountItem.m_nQ3);
			_file.writeInt(CrossingCountItem.m_nP1);

			_file.writeInt(CrossingCountItem.m_nP5);
			_file.writeInt(CrossingCountItem.m_nP10);


			_file.writeInt(CrossingCountItem.m_nP90);
			_file.writeInt(CrossingCountItem.m_nP95);
			_file.writeInt(CrossingCountItem.m_nP99);
			_file.writeInt(CrossingCountItem.m_nStdDev);
		}


		//runway crossings wait times 
		{
			StatisticsSummaryRunwayCrossingsItem& WaitTimeItem = item.m_staWaitTime;
			_file.writeInt(WaitTimeItem.m_nQ1);
			_file.writeInt(WaitTimeItem.m_nQ2);

			_file.writeInt(WaitTimeItem.m_nQ3);
			_file.writeInt(WaitTimeItem.m_nP1);

			_file.writeInt(WaitTimeItem.m_nP5);
			_file.writeInt(WaitTimeItem.m_nP10);


			_file.writeInt(WaitTimeItem.m_nP90);
			_file.writeInt(WaitTimeItem.m_nP95);
			_file.writeInt(WaitTimeItem.m_nP99);
			_file.writeInt(WaitTimeItem.m_nStdDev);
		}
		_file.writeLine();
	}

	return TRUE;
}

BOOL CAirsideRunwayCrossingsBaseSummaryResult::WriteReportData( ArctermFile& _file )
{
	_file.writeField("Runway Crossings Report");
	_file.writeLine();

	_file.writeInt(ASReportType_Summary);
	_file.writeLine();

	int nCount = (int)m_vResult.size();
	_file.writeInt(nCount);
	_file.writeLine();

	for (int nIndex = 0; nIndex < nCount; ++ nIndex)
	{
		CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayCrossingsItem& item = m_vResult[nIndex];

		_file.writeField(item.m_sRunway);
		_file.writeField(item.m_sEnterTaxiway);
		_file.writeField(item.m_sNodeName);
		_file.writeInt(item.m_lMinCrossings);
		int nCount1 = (int)item.m_vMinCrossingIntervals.size();
		char buf[16] = {0};
		_file.appendValue(",");
		_file.appendValue(_itoa(nCount1, buf, 10));
		for(int i=0; i<nCount1; i++)
		{
			_file.appendValue(";");
			memset(buf, 0, 16);
			_file.appendValue(_itoa(item.m_vMinCrossingIntervals.at(i), buf, 10));
		}
		_file.writeInt(item.m_lMaxCrossings);
		_file.writeFloat((float)item.m_dAverageCrosings);
		nCount1 = (int)item.m_vMaxCrossingIntervals.size();
		_file.appendValue(",");
		_file.appendValue(_itoa(nCount, buf, 10));
		for(int i=0; i<nCount1; i++)
		{
			_file.appendValue(";");
			memset(buf, 0, 16);
			_file.appendValue(_itoa(item.m_vMaxCrossingIntervals.at(i), buf, 10));
		}
		_file.writeInt(item.m_lTotalCrossings);
		_file.writeInt(item.m_lMinWaitTime);
		_file.writeInt(item.m_lIntervalMinWaitTime);
		_file.writeInt(item.m_lMaxWaitTime);
		_file.writeInt(item.m_lAverageWaitTime);
		_file.writeInt(item.m_lIntervalMaxWaitTime);
		_file.writeInt(item.m_lTotalWaitTime);

		//runway crossings count
		{
			StatisticsSummaryRunwayCrossingsItem& CrossingCountItem = item.m_staCrossingCount;

			_file.writeInt(CrossingCountItem.m_nQ3);
			_file.writeInt(CrossingCountItem.m_nP1);

			_file.writeInt(CrossingCountItem.m_nP5);
			_file.writeInt(CrossingCountItem.m_nP10);


			_file.writeInt(CrossingCountItem.m_nP90);
			_file.writeInt(CrossingCountItem.m_nP95);
			_file.writeInt(CrossingCountItem.m_nP99);
			_file.writeInt(CrossingCountItem.m_nStdDev);
		}


		//runway crossings wait times 
		{
			StatisticsSummaryRunwayCrossingsItem& WaitTimeItem = item.m_staWaitTime;
			_file.writeInt(WaitTimeItem.m_nQ1);
			_file.writeInt(WaitTimeItem.m_nQ2);

			_file.writeInt(WaitTimeItem.m_nQ3);
			_file.writeInt(WaitTimeItem.m_nP1);

			_file.writeInt(WaitTimeItem.m_nP5);
			_file.writeInt(WaitTimeItem.m_nP10);


			_file.writeInt(WaitTimeItem.m_nP90);
			_file.writeInt(WaitTimeItem.m_nP95);
			_file.writeInt(WaitTimeItem.m_nP99);
			_file.writeInt(WaitTimeItem.m_nStdDev);
		}
		_file.writeLine();
	}

	return TRUE;
}

BOOL CAirsideRunwayCrossingsBaseSummaryResult::ReadReportData( ArctermFile& _file )
{

	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nIndex = 0; nIndex < nCount; ++nIndex)
	{
		CAirsideRunwayCrossingsBaseSummaryResult::SummaryRunwayCrossingsItem item;

		//runway information
		_file.getField(item.m_sRunway.GetBuffer(1024),1024);
		_file.getField(item.m_sEnterTaxiway.GetBuffer(1024),1024);
		_file.getField(item.m_sNodeName.GetBuffer(1024),1024);
		_file.getInteger(item.m_lMinCrossings);
		char buf[16] = {0};
		_file.getSubField(buf, ';');
		int nCount = atoi(buf);
		for(int i=0; i<nCount; i++)
		{
			_file.getSubField(buf, ';');
			item.m_vMinCrossingIntervals.push_back(atoi(buf));
		}
		_file.getInteger(item.m_lMaxCrossings);
		_file.getFloat(item.m_dAverageCrosings);
		_file.getSubField(buf, ';');
		nCount = atoi(buf);
		for(int i=0; i<nCount; i++)
		{
			_file.getSubField(buf, ';');
			item.m_vMaxCrossingIntervals.push_back(atoi(buf));
		}
		_file.getInteger(item.m_lTotalCrossings);
		_file.getInteger(item.m_lMinWaitTime);
		_file.getInteger(item.m_lIntervalMinWaitTime);
		_file.getInteger(item.m_lMaxWaitTime);
		_file.getInteger(item.m_lAverageWaitTime);
		_file.getInteger(item.m_lIntervalMaxWaitTime);
		_file.getInteger(item.m_lTotalWaitTime);

		//runway crossings count
		{
			CAirsideRunwayCrossingsBaseSummaryResult::StatisticsSummaryRunwayCrossingsItem& crossingsCountItem = item.m_staCrossingCount;
			_file.getInteger(crossingsCountItem.m_nQ1);
			_file.getInteger(crossingsCountItem.m_nQ2);

			_file.getInteger(crossingsCountItem.m_nQ3);
			_file.getInteger(crossingsCountItem.m_nP1);

			_file.getInteger(crossingsCountItem.m_nP5);
			_file.getInteger(crossingsCountItem.m_nP10);

			_file.getInteger(crossingsCountItem.m_nP90);
			_file.getInteger(crossingsCountItem.m_nP95);
			_file.getInteger(crossingsCountItem.m_nP99);
			_file.getInteger(crossingsCountItem.m_nStdDev);
		}



		//runway crossings wait times
		{
			CAirsideRunwayCrossingsBaseSummaryResult::StatisticsSummaryRunwayCrossingsItem& waitTimeItem = item.m_staWaitTime;
			_file.getInteger(waitTimeItem.m_nQ1);
			_file.getInteger(waitTimeItem.m_nQ2);

			_file.getInteger(waitTimeItem.m_nQ3);
			_file.getInteger(waitTimeItem.m_nP1);

			_file.getInteger(waitTimeItem.m_nP5);
			_file.getInteger(waitTimeItem.m_nP10);

			_file.getInteger(waitTimeItem.m_nP90);
			_file.getInteger(waitTimeItem.m_nP95);
			_file.getInteger(waitTimeItem.m_nP99);
			_file.getInteger(waitTimeItem.m_nStdDev);
		}


		m_vResult.push_back(item);
		_file.getLine();

	}
	return TRUE;
}