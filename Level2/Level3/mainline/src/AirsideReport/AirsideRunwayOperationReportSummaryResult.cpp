#include "StdAfx.h"
#include ".\airsiderunwayoperationreportsummaryresult.h"
#include "AirsideRunwayOperationReportParam.h"
#include "../Common\AirportDatabase.h"
#include "../Reports/StatisticalTools.h"


CAirsideRunwayOperationReportSummaryResult::CAirsideRunwayOperationReportSummaryResult(void)
{
	m_pParameter = NULL;
	m_pChartResult = NULL;
}

CAirsideRunwayOperationReportSummaryResult::~CAirsideRunwayOperationReportSummaryResult(void)
{
	if(m_pChartResult != NULL)
		delete m_pChartResult;
	m_pChartResult = NULL;
}

void CAirsideRunwayOperationReportSummaryResult::GenerateResult( CParameters *pParameter )
{
	AirsideRunwayOperationReportParam *pParam = (AirsideRunwayOperationReportParam *)pParameter;
	if(pParam  == NULL)
		return;
	if(m_AirportDB == NULL)
		return;

	m_pParameter = pParameter;
	//m_pParameter = pParam;
	AircraftClassificationManager m_airCraftClassicfication;
	m_airCraftClassicfication.Init(m_AirportDB->getAcMan(),pParam->GetProjID());

	//read log
	CString strDescFilepath = (*m_pCBGetLogFilePath)(AirsideDescFile);
	CString strEventFilePath = (*m_pCBGetLogFilePath)(AirsideEventFile);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	EventLog<AirsideFlightEventStruct> airsideFlightEventLog;
	AirsideFlightLog fltLog;
	fltLog.SetEventLog(&airsideFlightEventLog);
	fltLog.LoadDataIfNecessary(strDescFilepath);

	CString strRunwayOpDescPath = (*m_pCBGetLogFilePath)(AirsideRunwayOperationFile);
	AirsideRunwayOperationLog m_airsideOperationLog;
	m_airsideOperationLog.ClearData();
	m_airsideOperationLog.LoadData(strRunwayOpDescPath);


	InitRunway(pParam);
	for (int nOperation = 0; nOperation < m_airsideOperationLog.GetItemCount(); ++ nOperation)
	{
		AirsideRunwayOperationLog::RunwayLogItem *pOperationItem = m_airsideOperationLog.GetItem(nOperation);
		if(pOperationItem == NULL)
			continue;

		if(pOperationItem->m_enumEntryOrExit != AirsideRunwayOperationLog::enumEntryOrExit_Entry)
			continue;

		//time range
		if( pOperationItem->m_eTime < pParameter->getStartTime() ||
			pOperationItem->m_eTime > pParameter->getEndTime())
			continue;


		//check runway 
		CAirsideReportRunwayMark reportRunwayMark;
		reportRunwayMark.SetRunway(pOperationItem->m_nRunwayID,pOperationItem->m_enumRunwayMark,pOperationItem->m_strMarkName);

		if(!pParam->IsRunwayMarkFit(reportRunwayMark))//runway mark not selected
			continue;


		//check constraint
		bool bFitFltConstraint = false;
		int nFltLogCount = fltLog.getCount();
		for (int i =0; i < nFltLogCount; ++i)
		{
			AirsideFlightLogEntry logEntry;
			logEntry.SetEventLog(&airsideFlightEventLog);
			fltLog.getItem(logEntry,i);
			AirsideFlightDescStruct	fltDesc = logEntry.GetAirsideDescStruct();
			if(fltDesc.id == pOperationItem->m_nFlightIndex )
			{
				if(fits(pParameter,fltDesc))
				{
					bFitFltConstraint = true;
					break;
				}

			}
		}
		if(bFitFltConstraint == false)//the flight constraint not fit move to next 
			continue;

		
		//get the runway
		RunwayOperationStatisticItem*  pRunwayStastic = GetRunwayStastic( reportRunwayMark );
		ASSERT(pRunwayStastic != NULL);
		if(pRunwayStastic == NULL)
			continue;

		//put the operation to result
		int nRunwayStasticCount = pRunwayStastic->m_vRunwayStatistic.size();
		for (int nStastic = 0; nStastic < nRunwayStasticCount-1; ++ nStastic)
		{
			if (pOperationItem->m_eTime >= pRunwayStastic->m_vRunwayStatistic.at(nStastic).m_eTime &&
				pOperationItem->m_eTime < pRunwayStastic->m_vRunwayStatistic.at(nStastic + 1).m_eTime)
			{
				//total count
				pRunwayStastic->m_vRunwayStatistic[nStastic].m_nOperationCount += 1;
				
				//landing
				if(pOperationItem->m_enumOperation == AirsideRunwayOperationLog::enumOperation_Landing)
				{
					pRunwayStastic->m_vRunwayStatistic[nStastic].m_nLandingCount += 1;
				}//take off
				else if(pOperationItem->m_enumOperation == AirsideRunwayOperationLog::enumOperation_Takeoff)
				{
					pRunwayStastic->m_vRunwayStatistic[nStastic].m_nTakeOffCount += 1;
				}
			}
		}
	}

	//Statistic data
	StatisticData(AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Landing);
	StatisticData(AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_TakeOff);
	StatisticData(AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Operations);
	StatisticAllRunwayData();

}
void CAirsideRunwayOperationReportSummaryResult::InitRunway( CParameters * parameter )
{
	AirsideRunwayOperationReportParam* pParam = (AirsideRunwayOperationReportParam*)parameter;
	if(pParam == NULL)
		return;

	int nRunwayCount = pParam->m_vRunway.size();
	for (int nRunway = 0; nRunway < nRunwayCount; ++ nRunway)
	{
		RunwayOperationStatisticItem *pRunwayDetail = new RunwayOperationStatisticItem;
		pRunwayDetail->m_runWaymark = pParam->m_vRunway[nRunway];
		pRunwayDetail->Init(pParam->getStartTime(),pParam->getEndTime(),pParam->getInterval());

		//pRunwayDetail
		m_vOperationStatistic.push_back(pRunwayDetail);
	}
}
void CAirsideRunwayOperationReportSummaryResult::RunwayOperationStatisticItem::Init( ElapsedTime eStartTime, ElapsedTime eEndTime, ElapsedTime eInterval )
{
	ASSERT(eInterval > ElapsedTime(0L));
	if(eInterval == ElapsedTime(0L) )
	{
		eInterval = ElapsedTime(60 * 60L);
	}

	for (;eStartTime < eEndTime + eInterval; eStartTime += eInterval)
	{
		RunwayOperationTimeCount timevalue;
		timevalue.m_eTime = eStartTime;
		m_vRunwayStatistic.push_back(timevalue);
	}
}

CAirsideRunwayOperationReportSummaryResult::RunwayOperationStatisticItem* CAirsideRunwayOperationReportSummaryResult::GetRunwayStastic( CAirsideReportRunwayMark& runwayMark )
{
	for (int nDetail = 0; nDetail < static_cast<int>(m_vOperationStatistic.size()); ++ nDetail)
	{
		if(m_vOperationStatistic.at(nDetail)->m_runWaymark.m_nRunwayID == runwayMark.m_nRunwayID &&
			m_vOperationStatistic.at(nDetail)->m_runWaymark.m_enumRunwayMark == runwayMark.m_enumRunwayMark )
			return m_vOperationStatistic[nDetail];
	}

	return NULL;
}

bool CAirsideRunwayOperationReportSummaryResult::fits(CParameters * parameter,const AirsideFlightDescStruct& fltDesc)
{
	size_t nFltConsCount = parameter->getFlightConstraintCount();
	for (size_t nfltCons =0; nfltCons < nFltConsCount; ++ nfltCons)
	{
		if(parameter->getFlightConstraint(nfltCons).fits(fltDesc))
		{
			return true;
		}
	}

	return false;
}



void CAirsideRunwayOperationReportSummaryResult::Draw3DChart()
{


}

void CAirsideRunwayOperationReportSummaryResult::InitListHead( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	//if(m_pParameter == NULL)
	//	return;
	//AirsideRunwayOperationReportParam *pParam = (AirsideRunwayOperationReportParam *)m_pParameter;
	//if(pParam  == NULL)
	//	return;
	
	if(!IsLoadFromFile())
	{
		GetListColumns(m_lstColumns);
	}
	for (int nCol = 0 ; nCol < (int)m_lstColumns.size(); ++nCol)
	{
		cxListCtrl.InsertColumn(nCol,m_lstColumns[nCol],LVCFMT_LEFT, 100);
		if (piSHC)
		{
			switch (nCol)
			{
			case 0: case 2: case 5: case 17: 
			case 20: case 32: case 35: 
					piSHC->SetDataType(nCol,dtSTRING);
				break;
			default:
					piSHC->SetDataType(nCol,dtINT);
				break;
			}
		}
	}

}

void CAirsideRunwayOperationReportSummaryResult::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter )
{	
	
	cxListCtrl.DeleteAllItems();
	int nRunwayCount = m_vOperationStatistic.size();
	for (int nRunway = 0; nRunway < nRunwayCount; ++ nRunway)
	{
		CAirsideRunwayOperationReportSummaryResult::RunwayOperationStatisticItem* pRunwayItem = m_vOperationStatistic[nRunway];
		if(pRunwayItem == NULL)
			continue;

		int nItem = cxListCtrl.InsertItem(nRunway,pRunwayItem->m_runWaymark.m_strMarkName);
		//take off

		cxListCtrl.SetItemText(nItem,1,FormatIntToString(pRunwayItem->m_TakeOffStastic.m_minCount));
		cxListCtrl.SetItemText(nItem,2,pRunwayItem->m_TakeOffStastic.m_strMinInterval);
		cxListCtrl.SetItemText(nItem,3,FormatIntToString(pRunwayItem->m_TakeOffStastic.m_nAverageCount));
		cxListCtrl.SetItemText(nItem,4,FormatIntToString(pRunwayItem->m_TakeOffStastic.m_maxCount));
		cxListCtrl.SetItemText(nItem,5,pRunwayItem->m_TakeOffStastic.m_strMaxInterval);

		cxListCtrl.SetItemText(nItem,6,FormatIntToString(pRunwayItem->m_TakeOffStastic.m_nQ1));
		cxListCtrl.SetItemText(nItem,7,FormatIntToString(pRunwayItem->m_TakeOffStastic.m_nQ2));
		cxListCtrl.SetItemText(nItem,8,FormatIntToString(pRunwayItem->m_TakeOffStastic.m_nQ3));

		cxListCtrl.SetItemText(nItem,9,FormatIntToString(pRunwayItem->m_TakeOffStastic.m_nP1));
		cxListCtrl.SetItemText(nItem,10,FormatIntToString(pRunwayItem->m_TakeOffStastic.m_nP5));
		cxListCtrl.SetItemText(nItem,11,FormatIntToString(pRunwayItem->m_TakeOffStastic.m_nP10));
		cxListCtrl.SetItemText(nItem,12,FormatIntToString(pRunwayItem->m_TakeOffStastic.m_nP90));
		cxListCtrl.SetItemText(nItem,13,FormatIntToString(pRunwayItem->m_TakeOffStastic.m_nP95));
		cxListCtrl.SetItemText(nItem,14,FormatIntToString(pRunwayItem->m_TakeOffStastic.m_nP99));
		cxListCtrl.SetItemText(nItem,15,FormatIntToString(pRunwayItem->m_TakeOffStastic.m_nStdDev));

		//landing
		int nLandingStartIndex = 15;
		cxListCtrl.SetItemText(nItem,nLandingStartIndex + 1,FormatIntToString(pRunwayItem->m_landingStastic.m_minCount));
		cxListCtrl.SetItemText(nItem,nLandingStartIndex + 2,pRunwayItem->m_landingStastic.m_strMinInterval);
		cxListCtrl.SetItemText(nItem,nLandingStartIndex + 3,FormatIntToString(pRunwayItem->m_landingStastic.m_nAverageCount));
		cxListCtrl.SetItemText(nItem,nLandingStartIndex + 4,FormatIntToString(pRunwayItem->m_landingStastic.m_maxCount));
		cxListCtrl.SetItemText(nItem,nLandingStartIndex + 5,pRunwayItem->m_landingStastic.m_strMaxInterval);

		cxListCtrl.SetItemText(nItem,nLandingStartIndex + 6,FormatIntToString(pRunwayItem->m_landingStastic.m_nQ1));
		cxListCtrl.SetItemText(nItem,nLandingStartIndex + 7,FormatIntToString(pRunwayItem->m_landingStastic.m_nQ2));
		cxListCtrl.SetItemText(nItem,nLandingStartIndex + 8,FormatIntToString(pRunwayItem->m_landingStastic.m_nQ3));

		cxListCtrl.SetItemText(nItem,nLandingStartIndex + 9,FormatIntToString(pRunwayItem->m_landingStastic.m_nP1));
		cxListCtrl.SetItemText(nItem,nLandingStartIndex + 10,FormatIntToString(pRunwayItem->m_landingStastic.m_nP5));
		cxListCtrl.SetItemText(nItem,nLandingStartIndex + 11,FormatIntToString(pRunwayItem->m_landingStastic.m_nP10));
		cxListCtrl.SetItemText(nItem,nLandingStartIndex + 12,FormatIntToString(pRunwayItem->m_landingStastic.m_nP90));
		cxListCtrl.SetItemText(nItem,nLandingStartIndex + 13,FormatIntToString(pRunwayItem->m_landingStastic.m_nP95));
		cxListCtrl.SetItemText(nItem,nLandingStartIndex + 14,FormatIntToString(pRunwayItem->m_landingStastic.m_nP99));
		cxListCtrl.SetItemText(nItem,nLandingStartIndex + 15,FormatIntToString(pRunwayItem->m_landingStastic.m_nStdDev));


		//movements
		int nMovementStartIndex = 15 + 15;
		cxListCtrl.SetItemText(nItem,nMovementStartIndex + 1,FormatIntToString(pRunwayItem->m_MovementStastic.m_minCount));
		cxListCtrl.SetItemText(nItem,nMovementStartIndex + 2,pRunwayItem->m_MovementStastic.m_strMinInterval);
		cxListCtrl.SetItemText(nItem,nMovementStartIndex + 3,FormatIntToString(pRunwayItem->m_MovementStastic.m_nAverageCount));
		cxListCtrl.SetItemText(nItem,nMovementStartIndex + 4,FormatIntToString(pRunwayItem->m_MovementStastic.m_maxCount));
		cxListCtrl.SetItemText(nItem,nMovementStartIndex + 5,pRunwayItem->m_MovementStastic.m_strMaxInterval);

		cxListCtrl.SetItemText(nItem,nMovementStartIndex + 6,FormatIntToString(pRunwayItem->m_MovementStastic.m_nQ1));
		cxListCtrl.SetItemText(nItem,nMovementStartIndex + 7,FormatIntToString(pRunwayItem->m_MovementStastic.m_nQ2));
		cxListCtrl.SetItemText(nItem,nMovementStartIndex + 8,FormatIntToString(pRunwayItem->m_MovementStastic.m_nQ3));

		cxListCtrl.SetItemText(nItem,nMovementStartIndex + 9,FormatIntToString(pRunwayItem->m_MovementStastic.m_nP1));
		cxListCtrl.SetItemText(nItem,nMovementStartIndex + 10,FormatIntToString(pRunwayItem->m_MovementStastic.m_nP5));
		cxListCtrl.SetItemText(nItem,nMovementStartIndex + 11,FormatIntToString(pRunwayItem->m_MovementStastic.m_nP10));
		cxListCtrl.SetItemText(nItem,nMovementStartIndex + 12,FormatIntToString(pRunwayItem->m_MovementStastic.m_nP90));
		cxListCtrl.SetItemText(nItem,nMovementStartIndex + 13,FormatIntToString(pRunwayItem->m_MovementStastic.m_nP95));
		cxListCtrl.SetItemText(nItem,nMovementStartIndex + 14,FormatIntToString(pRunwayItem->m_MovementStastic.m_nP99));
		cxListCtrl.SetItemText(nItem,nMovementStartIndex + 15,FormatIntToString(pRunwayItem->m_MovementStastic.m_nStdDev));


	}
}


void CAirsideRunwayOperationReportSummaryResult::RefreshReport( CParameters * parameter )
{
	AirsideRunwayOperationReportParam *pParam = (AirsideRunwayOperationReportParam *)parameter;
	if(pParam  == NULL)
		return;

	if(m_pChartResult != NULL)
		delete m_pChartResult;
	m_pChartResult = NULL;
	if(pParam->getSubType() == -1 )
		pParam->setSubType(AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Operations);
	if(pParam->getSubType() == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Operations)
	{
		m_pChartResult = new AirsideRunwayOperationReportSummary::RunwayOperationalStatisticsChartResult(AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Operations);
		AirsideRunwayOperationReportSummary::RunwayOperationalStatisticsChartResult *pChartResult = 
			(AirsideRunwayOperationReportSummary::RunwayOperationalStatisticsChartResult *)m_pChartResult;

		pChartResult->GenerateResult(&m_vOperationStatistic);
	}
	else if(pParam->getSubType() == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_TakeOff)
	{
		m_pChartResult = new AirsideRunwayOperationReportSummary::RunwayOperationalStatisticsChartResult(AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_TakeOff);
		AirsideRunwayOperationReportSummary::RunwayOperationalStatisticsChartResult *pChartResult = 
			(AirsideRunwayOperationReportSummary::RunwayOperationalStatisticsChartResult *)m_pChartResult;

		pChartResult->GenerateResult(&m_vOperationStatistic);

	}
	else if(pParam->getSubType() == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Landing)
	{
		m_pChartResult = new AirsideRunwayOperationReportSummary::RunwayOperationalStatisticsChartResult(AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Landing);
		AirsideRunwayOperationReportSummary::RunwayOperationalStatisticsChartResult *pChartResult = 
			(AirsideRunwayOperationReportSummary::RunwayOperationalStatisticsChartResult *)m_pChartResult;

		pChartResult->GenerateResult(&m_vOperationStatistic);
	}
	else
	{
		ASSERT(0);
		//should not get here
	}
}

void CAirsideRunwayOperationReportSummaryResult::StatisticData(AirsideRunwayOperationsReport::ChartType enumChartType)
{
	int nRunwayCount = m_vOperationStatistic.size();

	for (int nRunway = 0; nRunway < nRunwayCount; ++ nRunway)
	{
		//new runway statistic 
		
		CAirsideRunwayOperationReportSummaryResult::StatisticsSummaryItem statisticItem;


		std::vector<CAirsideRunwayOperationReportSummaryResult::RunwayOperationTimeCount>& vRunwayStatistic = m_vOperationStatistic[nRunway]->m_vRunwayStatistic;

		//get min, max, average
		int nMinCount = 100000000;
		CString strMinTimeInterval;

		int nMaxCount = -1;
		CString strMaxTimeInterval;

		int nTotalCount = 0;

		CStatisticalTools<double> statisticalTool;


		//time segment count
		int nTimeCount = vRunwayStatistic.size();
		if(nTimeCount == 0)
			continue;


		for (int nTime =0; nTime < nTimeCount; ++nTime)
		{
			//ChartType_Summary_RunwayOperationalStatistic_Operations,
			//	ChartType_Summary_RunwayOperationalStatistic_TakeOff,
			//	ChartType_Summary_RunwayOperationalStatistic_Landing,

			//ElapsedTime m_eTime;
			//int			m_nOperationCount;
			//int			m_nLandingCount;
			//int			m_nTakeOffCount;
			CString strTimeInterval;
			if(nTime <= nTimeCount- 1)
			{
				//ElapsedTime eFromtime = vRunwayStatistic[nTime].m_eTime;

				//ElapsedTime eToTime = vRunwayStatistic[nTime + 1].m_eTime;

				//strTimeInterval.Format(_T("%s-%s"),eFromtime.PrintDateTime(),eToTime.PrintDateTime());

				strTimeInterval.Format(_T("%dth"),nTime);
			}


			if(enumChartType == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Operations)
			{
				if(vRunwayStatistic[nTime].m_nOperationCount < nMinCount)
				{
					nMinCount = vRunwayStatistic[nTime].m_nOperationCount;
					strMinTimeInterval = strTimeInterval;
				}

				if(vRunwayStatistic[nTime].m_nOperationCount > nMaxCount)
				{
					nMaxCount = vRunwayStatistic[nTime].m_nOperationCount;
					strMaxTimeInterval = strTimeInterval;
				}
				//total count
				nTotalCount += vRunwayStatistic[nTime].m_nOperationCount;
				//statistical tool
				statisticalTool.AddNewData(vRunwayStatistic[nTime].m_nOperationCount);
			}
			else if(enumChartType == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Landing)
			{
				//min landing
				if(vRunwayStatistic[nTime].m_nLandingCount < nMinCount)
				{
					nMinCount = vRunwayStatistic[nTime].m_nLandingCount;
					strMinTimeInterval = strTimeInterval;
				}
				//max landing
				if(vRunwayStatistic[nTime].m_nLandingCount > nMaxCount)
				{
					nMaxCount = vRunwayStatistic[nTime].m_nLandingCount;
					strMaxTimeInterval = strTimeInterval;
				}
				//total take off
				nTotalCount += vRunwayStatistic[nTime].m_nLandingCount;

				statisticalTool.AddNewData(vRunwayStatistic[nTime].m_nLandingCount);

			}
			else if(enumChartType == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_TakeOff)
			{
				//min take off
				if(vRunwayStatistic[nTime].m_nTakeOffCount < nMinCount)
				{
					nMinCount = vRunwayStatistic[nTime].m_nTakeOffCount;
					strMinTimeInterval = strTimeInterval;
				}
				//max take off
				if(vRunwayStatistic[nTime].m_nTakeOffCount > nMaxCount)
				{
					nMaxCount = vRunwayStatistic[nTime].m_nTakeOffCount;
					strMaxTimeInterval = strTimeInterval;
				}
				//total take off
				nTotalCount += vRunwayStatistic[nTime].m_nTakeOffCount;
				//statistical tool
				statisticalTool.AddNewData(vRunwayStatistic[nTime].m_nTakeOffCount);
			}
			else 
			{
				continue;
				//should not get here
			}
		}
		statisticalTool.SortData();
		//statistical value
		statisticItem.m_minCount = nMinCount;
		statisticItem.m_strMinInterval = strMinTimeInterval;

		statisticItem.m_maxCount = nMaxCount;
		statisticItem.m_strMaxInterval = strMaxTimeInterval;

		if(nTimeCount > 1)
			statisticItem.m_nAverageCount = static_cast<int>(nTotalCount/(nTimeCount - 1));

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


		if(enumChartType == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Operations)
		{
			m_vOperationStatistic[nRunway]->m_MovementStastic = statisticItem;
		}
		else if(enumChartType == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Landing)
		{
			m_vOperationStatistic[nRunway]->m_landingStastic = statisticItem;
		}
		else if(enumChartType == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_TakeOff)
		{
			m_vOperationStatistic[nRunway]->m_TakeOffStastic = statisticItem;
		}
	}
}


CString CAirsideRunwayOperationReportSummaryResult::FormatIntToString( int nValue )
{
	CString str;
	str.Format(_T("%d"),nValue);
	return str;
}

BOOL CAirsideRunwayOperationReportSummaryResult::WriteReportData( ArctermFile& _file )
{

	//title
	_file.writeField("Runway Operation Summary Report");
	_file.writeLine();
	//columns
	CString strColumns= _T("RunwayID, Mark, Name, total landing count, classificatin count");
	_file.writeField(_T("Columns"));
	_file.writeLine();

	_file.writeInt((int)ASReportType_Summary);
	_file.writeLine();

	std::vector<CString> vColumns = m_lstColumns;
	if(!IsLoadFromFile())
		GetListColumns(vColumns);
	_file.writeInt((int)vColumns.size());
	for (int nCol = 0; nCol < (int)vColumns.size(); ++nCol)
	{
		_file.writeField(vColumns[nCol]);
	}
	_file.writeLine();
		

	int nRunwwayCount = m_vOperationStatistic.size();
	_file.writeField(_T("Runway Count:"));
	_file.writeInt(nRunwwayCount);
	_file.writeLine();

	for (int nRunway = 0; nRunway < nRunwwayCount; ++ nRunway)
	{
		RunwayOperationStatisticItem* pRunwayStatistic = m_vOperationStatistic[nRunway];

		if(pRunwayStatistic == NULL)
			return FALSE;
		//runway information
		_file.writeInt(pRunwayStatistic->m_runWaymark.m_nRunwayID);
		_file.writeInt((int)pRunwayStatistic->m_runWaymark.m_enumRunwayMark);
		_file.writeField(pRunwayStatistic->m_runWaymark.m_strMarkName);

		//landing
		{
			StatisticsSummaryItem& landingStastic = pRunwayStatistic->m_landingStastic;
			_file.writeInt(landingStastic.m_minCount);
			_file.writeField(landingStastic.m_strMinInterval);
			_file.writeInt(landingStastic.m_nAverageCount);
			_file.writeInt(landingStastic.m_maxCount);
			_file.writeField(landingStastic.m_strMaxInterval);
			_file.writeInt(landingStastic.m_nQ1);
			_file.writeInt(landingStastic.m_nQ2);

			_file.writeInt(landingStastic.m_nQ3);
			_file.writeInt(landingStastic.m_nP1);

			_file.writeInt(landingStastic.m_nP5);
			_file.writeInt(landingStastic.m_nP10);


			_file.writeInt(landingStastic.m_nP90);
			_file.writeInt(landingStastic.m_nP95);
			_file.writeInt(landingStastic.m_nP99);
			_file.writeInt(landingStastic.m_nStdDev);
		}



		//take off 
		{
			StatisticsSummaryItem& takeoffStastic = pRunwayStatistic->m_TakeOffStastic;
			_file.writeInt(takeoffStastic.m_minCount);
			_file.writeField(takeoffStastic.m_strMinInterval);
			_file.writeInt(takeoffStastic.m_nAverageCount);
			_file.writeInt(takeoffStastic.m_maxCount);
			_file.writeField(takeoffStastic.m_strMaxInterval);
			_file.writeInt(takeoffStastic.m_nQ1);
			_file.writeInt(takeoffStastic.m_nQ2);

			_file.writeInt(takeoffStastic.m_nQ3);
			_file.writeInt(takeoffStastic.m_nP1);

			_file.writeInt(takeoffStastic.m_nP5);
			_file.writeInt(takeoffStastic.m_nP10);


			_file.writeInt(takeoffStastic.m_nP90);
			_file.writeInt(takeoffStastic.m_nP95);
			_file.writeInt(takeoffStastic.m_nP99);
			_file.writeInt(takeoffStastic.m_nStdDev);
		}


		//movements
		{
			StatisticsSummaryItem& movementStastic = pRunwayStatistic->m_MovementStastic;
			_file.writeInt(movementStastic.m_minCount);
			_file.writeField(movementStastic.m_strMinInterval);
			_file.writeInt(movementStastic.m_nAverageCount);
			_file.writeInt(movementStastic.m_maxCount);
			_file.writeField(movementStastic.m_strMaxInterval);
			_file.writeInt(movementStastic.m_nQ1);
			_file.writeInt(movementStastic.m_nQ2);

			_file.writeInt(movementStastic.m_nQ3);
			_file.writeInt(movementStastic.m_nP1);

			_file.writeInt(movementStastic.m_nP5);
			_file.writeInt(movementStastic.m_nP10);


			_file.writeInt(movementStastic.m_nP90);
			_file.writeInt(movementStastic.m_nP95);
			_file.writeInt(movementStastic.m_nP99);
			_file.writeInt(movementStastic.m_nStdDev);
		}

		_file.writeLine();

	}

	return FALSE;
}

BOOL CAirsideRunwayOperationReportSummaryResult::ReadReportData( ArctermFile& _file )
{

	_file.getLine();
	//columns
	int nColCount = 0;
	_file.getInteger(nColCount);
	for (int nCol = 0; nCol < nColCount; ++nCol)
	{
		CString strCol;
		_file.getField(strCol.GetBuffer(1024),1024);
		strCol.ReleaseBuffer();
		m_lstColumns.push_back(strCol);

	}
	_file.getLine();


	int nRunwwayCount = 0;
	_file.skipField(1);//skip runway count
	_file.getInteger(nRunwwayCount);
	_file.getLine();

	for (int nRunway = 0; nRunway < nRunwwayCount; ++ nRunway)
	{
		RunwayOperationStatisticItem* pRunwayStatistic = new RunwayOperationStatisticItem;

		CAirsideReportRunwayMark runWaymark;

		//runway information
		_file.getInteger(runWaymark.m_nRunwayID);

		_file.getInteger((int&)runWaymark.m_enumRunwayMark);
		_file.getField(runWaymark.m_strMarkName.GetBuffer(1024),1024);
		runWaymark.m_strMarkName.ReleaseBuffer();
		pRunwayStatistic->m_runWaymark = runWaymark;



		//landing
		{
			StatisticsSummaryItem& landingStastic = pRunwayStatistic->m_landingStastic;
			_file.getInteger(landingStastic.m_minCount);
			_file.getField(landingStastic.m_strMinInterval.GetBuffer(1024),1024);
			landingStastic.m_strMinInterval.ReleaseBuffer();

			_file.getInteger(landingStastic.m_nAverageCount);
			_file.getInteger(landingStastic.m_maxCount);
			_file.getField(landingStastic.m_strMaxInterval.GetBuffer(1024),1024);
			landingStastic.m_strMaxInterval.ReleaseBuffer();
			_file.getInteger(landingStastic.m_nQ1);
			_file.getInteger(landingStastic.m_nQ2);

			_file.getInteger(landingStastic.m_nQ3);
			_file.getInteger(landingStastic.m_nP1);

			_file.getInteger(landingStastic.m_nP5);
			_file.getInteger(landingStastic.m_nP10);


			_file.getInteger(landingStastic.m_nP90);
			_file.getInteger(landingStastic.m_nP95);
			_file.getInteger(landingStastic.m_nP99);
			_file.getInteger(landingStastic.m_nStdDev);
		}



		//take off 
		{
			StatisticsSummaryItem& takeoffStastic = pRunwayStatistic->m_TakeOffStastic;
			_file.getInteger(takeoffStastic.m_minCount);
			_file.getField(takeoffStastic.m_strMinInterval.GetBuffer(1024),1024);
			takeoffStastic.m_strMinInterval.ReleaseBuffer();
			_file.getInteger(takeoffStastic.m_nAverageCount);
			_file.getInteger(takeoffStastic.m_maxCount);
			_file.getField(takeoffStastic.m_strMaxInterval.GetBuffer(1024),1024);
			takeoffStastic.m_strMaxInterval.ReleaseBuffer();

			_file.getInteger(takeoffStastic.m_nQ1);
			_file.getInteger(takeoffStastic.m_nQ2);

			_file.getInteger(takeoffStastic.m_nQ3);
			_file.getInteger(takeoffStastic.m_nP1);

			_file.getInteger(takeoffStastic.m_nP5);
			_file.getInteger(takeoffStastic.m_nP10);


			_file.getInteger(takeoffStastic.m_nP90);
			_file.getInteger(takeoffStastic.m_nP95);
			_file.getInteger(takeoffStastic.m_nP99);
			_file.getInteger(takeoffStastic.m_nStdDev);
		}


		//movements
		{
			StatisticsSummaryItem& movementStastic = pRunwayStatistic->m_MovementStastic;
			_file.getInteger(movementStastic.m_minCount);
			_file.getField(movementStastic.m_strMinInterval.GetBuffer(1024),1024);
			movementStastic.m_strMinInterval.ReleaseBuffer();
			_file.getInteger(movementStastic.m_nAverageCount);
			_file.getInteger(movementStastic.m_maxCount);

			_file.getField(movementStastic.m_strMaxInterval.GetBuffer(1024),1024);
			movementStastic.m_strMaxInterval.ReleaseBuffer();


			_file.getInteger(movementStastic.m_nQ1);
			_file.getInteger(movementStastic.m_nQ2);

			_file.getInteger(movementStastic.m_nQ3);
			_file.getInteger(movementStastic.m_nP1);

			_file.getInteger(movementStastic.m_nP5);
			_file.getInteger(movementStastic.m_nP10);


			_file.getInteger(movementStastic.m_nP90);
			_file.getInteger(movementStastic.m_nP95);
			_file.getInteger(movementStastic.m_nP99);
			_file.getInteger(movementStastic.m_nStdDev);
		}

		m_vOperationStatistic.push_back(pRunwayStatistic);
		_file.getLine();







	}
	return FALSE;
}

void CAirsideRunwayOperationReportSummaryResult::StatisticAllRunwayData()
{	
	int nRunwayCount = m_vOperationStatistic.size();
	ASSERT(nRunwayCount > 0);
	if(nRunwayCount <=0 )
		return;

	RunwayOperationStatisticItem* allRunwayStastic = new RunwayOperationStatisticItem;
	allRunwayStastic->m_runWaymark.m_strMarkName = _T("All");
	allRunwayStastic->m_vRunwayStatistic.resize(m_vOperationStatistic[0]->m_vRunwayStatistic.size());

	for (int nRunway = 0; nRunway < nRunwayCount; ++ nRunway)
	{
		//new runway statistic 
		std::vector<CAirsideRunwayOperationReportSummaryResult::RunwayOperationTimeCount>& vRunwayStatistic = m_vOperationStatistic[nRunway]->m_vRunwayStatistic;
		for (int i = 0; i < (int)vRunwayStatistic.size(); ++ i)
		{
			allRunwayStastic->m_vRunwayStatistic[i].m_eTime = vRunwayStatistic[i].m_eTime;
			allRunwayStastic->m_vRunwayStatistic[i].m_nOperationCount += vRunwayStatistic[i].m_nOperationCount;
			allRunwayStastic->m_vRunwayStatistic[i].m_nLandingCount += vRunwayStatistic[i].m_nLandingCount;
			allRunwayStastic->m_vRunwayStatistic[i].m_nTakeOffCount += vRunwayStatistic[i].m_nTakeOffCount;
		}
	}


	StatisticData(allRunwayStastic,AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Landing,allRunwayStastic->m_landingStastic);
	StatisticData(allRunwayStastic,AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_TakeOff,allRunwayStastic->m_TakeOffStastic);
	StatisticData(allRunwayStastic,AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Operations,allRunwayStastic->m_MovementStastic);
	
	m_vOperationStatistic.insert(m_vOperationStatistic.begin(),allRunwayStastic);
	
}
void CAirsideRunwayOperationReportSummaryResult::StatisticData( CAirsideRunwayOperationReportSummaryResult::RunwayOperationStatisticItem *pAllRunwayStastic,
															   AirsideRunwayOperationsReport::ChartType enumChartType,
															   CAirsideRunwayOperationReportSummaryResult::StatisticsSummaryItem& statisticItem)
{
	if(pAllRunwayStastic == NULL)
		return;

	std::vector<CAirsideRunwayOperationReportSummaryResult::RunwayOperationTimeCount>& vRunwayStatistic = pAllRunwayStastic->m_vRunwayStatistic;

	//get min, max, average
	int nMinCount = 100000000;
	CString strMinTimeInterval;

	int nMaxCount = -1;
	CString strMaxTimeInterval;

	int nTotalCount = 0;

	CStatisticalTools<double> statisticalTool;


	//time segment count
	int nTimeCount = vRunwayStatistic.size();
	if(nTimeCount == 0)
		return;


	for (int nTime =0; nTime < nTimeCount; ++nTime)
	{
		//ChartType_Summary_RunwayOperationalStatistic_Operations,
		//	ChartType_Summary_RunwayOperationalStatistic_TakeOff,
		//	ChartType_Summary_RunwayOperationalStatistic_Landing,

		//ElapsedTime m_eTime;
		//int			m_nOperationCount;
		//int			m_nLandingCount;
		//int			m_nTakeOffCount;
		CString strTimeInterval;
		if(nTime <= nTimeCount- 1)
		{
			//ElapsedTime eFromtime = vRunwayStatistic[nTime].m_eTime;

			//ElapsedTime eToTime = vRunwayStatistic[nTime + 1].m_eTime;

			//strTimeInterval.Format(_T("%s-%s"),eFromtime.PrintDateTime(),eToTime.PrintDateTime());

			strTimeInterval.Format(_T("%dth"),nTime);
		}


		if(enumChartType == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Operations)
		{
			if(vRunwayStatistic[nTime].m_nOperationCount < nMinCount)
			{
				nMinCount = vRunwayStatistic[nTime].m_nOperationCount;
				strMinTimeInterval = strTimeInterval;
			}

			if(vRunwayStatistic[nTime].m_nOperationCount > nMaxCount)
			{
				nMaxCount = vRunwayStatistic[nTime].m_nOperationCount;
				strMaxTimeInterval = strTimeInterval;
			}
			//total count
			nTotalCount += vRunwayStatistic[nTime].m_nOperationCount;
			//statistical tool
			statisticalTool.AddNewData(vRunwayStatistic[nTime].m_nOperationCount);
		}
		else if(enumChartType == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Landing)
		{
			//min landing
			if(vRunwayStatistic[nTime].m_nLandingCount < nMinCount)
			{
				nMinCount = vRunwayStatistic[nTime].m_nLandingCount;
				strMinTimeInterval = strTimeInterval;
			}
			//max landing
			if(vRunwayStatistic[nTime].m_nLandingCount > nMaxCount)
			{
				nMaxCount = vRunwayStatistic[nTime].m_nLandingCount;
				strMaxTimeInterval = strTimeInterval;
			}
			//total take off
			nTotalCount += vRunwayStatistic[nTime].m_nLandingCount;

			statisticalTool.AddNewData(vRunwayStatistic[nTime].m_nLandingCount);

		}
		else if(enumChartType == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_TakeOff)
		{
			//min take off
			if(vRunwayStatistic[nTime].m_nTakeOffCount < nMinCount)
			{
				nMinCount = vRunwayStatistic[nTime].m_nTakeOffCount;
				strMinTimeInterval = strTimeInterval;
			}
			//max take off
			if(vRunwayStatistic[nTime].m_nTakeOffCount > nMaxCount)
			{
				nMaxCount = vRunwayStatistic[nTime].m_nTakeOffCount;
				strMaxTimeInterval = strTimeInterval;
			}
			//total take off
			nTotalCount += vRunwayStatistic[nTime].m_nTakeOffCount;
			//statistical tool
			statisticalTool.AddNewData(vRunwayStatistic[nTime].m_nTakeOffCount);
		}
		else 
		{
			continue;
			//should not get here
		}
	}
	statisticalTool.SortData();
	//statistical value
	statisticItem.m_minCount = nMinCount;
	statisticItem.m_strMinInterval = strMinTimeInterval;

	statisticItem.m_maxCount = nMaxCount;
	statisticItem.m_strMaxInterval = strMaxTimeInterval;

	if(nTimeCount > 1)
		statisticItem.m_nAverageCount = static_cast<int>(nTotalCount/(nTimeCount - 1));

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
}

void CAirsideRunwayOperationReportSummaryResult::GetListColumns( std::vector<CString>& lstColumns )
{
	lstColumns.clear();

	lstColumns.push_back(_T("Runway"));
	////landing, take off
	std::vector<CString > vStatisticLabel;
	vStatisticLabel.push_back(_T("Min"));
	vStatisticLabel.push_back(_T("Min Interval"));
	vStatisticLabel.push_back(_T("Average"));
	vStatisticLabel.push_back(_T("Max"));
	vStatisticLabel.push_back(_T("Max Interval"));
	vStatisticLabel.push_back(_T("Q1"));
	vStatisticLabel.push_back(_T("Q2"));
	vStatisticLabel.push_back(_T("Q3"));
	vStatisticLabel.push_back(_T("P1"));
	vStatisticLabel.push_back(_T("P5"));
	vStatisticLabel.push_back(_T("P10"));
	vStatisticLabel.push_back(_T("P90"));
	vStatisticLabel.push_back(_T("P95"));
	vStatisticLabel.push_back(_T("P99"));
	vStatisticLabel.push_back(_T("Std Dev"));

	//Take off
	int nStatisticCount = vStatisticLabel.size();

	for (int i =0; i < nStatisticCount; ++ i)
	{
		lstColumns.push_back(vStatisticLabel[i] + _T("(TO)"));
	}
	//landing
	for (int i =0; i < nStatisticCount; ++ i)
	{
		lstColumns.push_back(vStatisticLabel[i] + _T("(Landing)"));
	}

	//movement
	for (int i =0; i < nStatisticCount; ++ i)
	{
		lstColumns.push_back(vStatisticLabel[i] + _T("(Movements)"));
	}
}

////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
AirsideRunwayOperationReportSummary::RunwayOperationalStatisticsChartResult::RunwayOperationalStatisticsChartResult(AirsideRunwayOperationsReport::ChartType chartType)
:m_enumChartType(chartType)
{
	m_vRunwayOperationStastic = NULL;
}

AirsideRunwayOperationReportSummary::RunwayOperationalStatisticsChartResult::~RunwayOperationalStatisticsChartResult()
{

}

void AirsideRunwayOperationReportSummary::RunwayOperationalStatisticsChartResult::GenerateResult( std::vector<CAirsideRunwayOperationReportSummaryResult::RunwayOperationStatisticItem *> * vRunwayOperationStastic )
{
	
	m_vRunwayOperationStastic = vRunwayOperationStastic;
}

void AirsideRunwayOperationReportSummary::RunwayOperationalStatisticsChartResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	if(m_vRunwayOperationStastic == NULL || pParameter == NULL)
		return;

	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	CString strReportType = _T("Movements");

	if(m_enumChartType == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Operations)
	{
		strReportType = _T("Movements");
	}
	else if(m_enumChartType == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Landing)
	{
		strReportType = _T("Landing");
	}
	else if(m_enumChartType == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_TakeOff)
	{
		strReportType = _T("Take Off");
	}
	else
	{
		return;
	}


	c2dGraphData.m_strChartTitle = _T(" Runway Operational Statistic - ") + strReportType;


	c2dGraphData.m_strYtitle = _T("Number of Operations");
	c2dGraphData.m_strXtitle = _T("Runway");

	int nRunwayCount = m_vRunwayOperationStastic->size();
	if(nRunwayCount == 0)
		return;


	//x tick, runway
	for (int nRunway = 0; nRunway < nRunwayCount; ++nRunway)
	{
		c2dGraphData.m_vrXTickTitle.push_back((*m_vRunwayOperationStastic)[nRunway]->m_runWaymark.m_strMarkName);
	}

	//c2dGraphData.m_vrXTickTitle = m_vTimeRange;


	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());


	bool bCommaFlag = true;
	AirsideRunwayOperationReportParam* pParam = (AirsideRunwayOperationReportParam*)pParameter;
	for (int i=0; i<(int)pParam->getFlightConstraintCount(); i++)
	{
		FlightConstraint fltCons = pParam->getFlightConstraint(i);

		CString strFlight(_T(""));
		fltCons.screenPrint(strFlight.GetBuffer(1024));

		if (bCommaFlag)
		{
			bCommaFlag = false;
			strFooter.AppendFormat("%s", strFlight);
		}
		else
		{
			strFooter.AppendFormat(",%s", strFlight);
		}
	}
	c2dGraphData.m_strFooter = strFooter;


	std::vector<CString> vLegends;
	//get the legend
	vLegends.push_back(_T("Min"));
	vLegends.push_back(_T("Average"));
	vLegends.push_back(_T("Max"));
	vLegends.push_back(_T("Q1"));
	vLegends.push_back(_T("Q2"));
	vLegends.push_back(_T("Q3"));
	vLegends.push_back(_T("P1"));
	vLegends.push_back(_T("P5"));
	vLegends.push_back(_T("P10"));
	vLegends.push_back(_T("P90"));
	vLegends.push_back(_T("P95"));
	vLegends.push_back(_T("P99"));
	vLegends.push_back(_T("Std Dev"));


	std::vector< std::vector<double> > vLegendData;
	vLegendData.resize(vLegends.size());


	for (int nRunwayStastic = 0; nRunwayStastic < nRunwayCount; ++ nRunwayStastic)
	{
		CAirsideRunwayOperationReportSummaryResult::StatisticsSummaryItem* statisticItem = NULL;
		

		if(m_enumChartType == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Operations)
		{
			statisticItem = &((*m_vRunwayOperationStastic)[nRunwayStastic]->m_MovementStastic);
		}
		else if(m_enumChartType == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Landing)
		{
			statisticItem = &((*m_vRunwayOperationStastic)[nRunwayStastic]->m_landingStastic);
		}
		else if(m_enumChartType == AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_TakeOff)
		{
			statisticItem = &((*m_vRunwayOperationStastic)[nRunwayStastic]->m_TakeOffStastic);
		}

		if(statisticItem == NULL)
			continue;
		
		//min
		vLegendData[0].push_back((*statisticItem).m_minCount);
		vLegendData[1].push_back((*statisticItem).m_nAverageCount);
		vLegendData[2].push_back((*statisticItem).m_maxCount);
		vLegendData[3].push_back((*statisticItem).m_nQ1);
		vLegendData[4].push_back((*statisticItem).m_nQ2);
		vLegendData[5].push_back((*statisticItem).m_nQ3);
		vLegendData[6].push_back((*statisticItem).m_nP1);
		vLegendData[7].push_back((*statisticItem).m_nP5);
		vLegendData[8].push_back((*statisticItem).m_nP10);
		vLegendData[9].push_back((*statisticItem).m_nP90);
		vLegendData[10].push_back((*statisticItem).m_nP95);
		vLegendData[11].push_back((*statisticItem).m_nP99);
		vLegendData[12].push_back((*statisticItem).m_nStdDev);
	}

	int nLegendCount = vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	chartWnd.DrawChart(c2dGraphData);
}
























