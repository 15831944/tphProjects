#include "stdafx.h"
#include "AirsideStandMultiRunOperatinResult.h"
#include "Common/TERMFILE.H"
#include "Common/elaptime.h"
#include "MFCExControl/XListCtrl.h"
#include "CARC3DChart.h"
#include "FlightStandOperationParameter.h"
#include "AirsideFlightStandOperationReport.h"
#include <algorithm>
#include "Parameters.h"
#include "StandOperationDataCalculation.h"

static const char* strSummaryListTitle[] = 
{
    "Min",
    "Avg",
    "Max",
    "Q1",
    "Q2",
    "Q3",
    "P1",
    "P5",
    "P10",
    "P90",
    "P95",
    "P99",
    "Std dev"
};

CAirsideStandMultiRunOperatinResult::CAirsideStandMultiRunOperatinResult(void)
{
}


CAirsideStandMultiRunOperatinResult::~CAirsideStandMultiRunOperatinResult(void)
{
	ClearData();
}

void CAirsideStandMultiRunOperatinResult::LoadMultipleRunReport( CParameters* pParameter )
{
	ClearData();
	ArctermFile file;
	MapMultiRunStandOperationData mapStandLoadData;
	DelayResultPath::iterator iter = m_mapResultPath.begin();
	for (; iter != m_mapResultPath.end(); ++iter)
	{
		CString strResultPath = iter->second;
		CString strSimResult = iter->first;

		try
		{
			if (file.openFile(strResultPath.GetString(),READ))
			{
				int nCount = 0;
				file.getLine();
				file.getInteger(nCount);
				file.getLine();

				for (int nIndex = 0; nIndex < nCount; nIndex++)
				{
					file.skipField(2);
					StandMultipleOperationData standOperationData;
					file.getChar(standOperationData.m_fltmode);
					file.getField(standOperationData.m_sSchedName.GetBuffer(1024),1024);
					standOperationData.m_sSchedName.ReleaseBuffer();

					file.skipField(2);
                    file.getInteger(standOperationData.m_lSchedOccupancy);

					file.getField(standOperationData.m_sActualName.GetBuffer(1024),1024);
					standOperationData.m_sActualName.ReleaseBuffer();

					file.skipField(2);
					file.getInteger(standOperationData.m_lOccupiedTime);

					long lDelayEnter = 0;
					file.getInteger(lDelayEnter);

					file.skipField(1);
					long lDelayLeaving = 0;
					file.getInteger(lDelayLeaving);
					standOperationData.m_lDelayEnter = lDelayEnter;
					standOperationData.m_lDelayLeaving = lDelayLeaving;
					mapStandLoadData[strSimResult].push_back(standOperationData);

					file.getLine();
				}

				file.closeIn();
			}
		}
		catch(...)
		{
			ClearData();
		}
	}

	//generate detail stand occupancy
	BuildDetailStandOperationOccupancy(pParameter,mapStandLoadData);
	//generate detail stand percentage
	BuildDetailStandOperationPencentage(pParameter,mapStandLoadData);
	//generate detail stand idle
	BuildDetailStandOperationIdel(pParameter,mapStandLoadData);
	//generate stand arr delay
	BuildDetailStandOperationDelay(pParameter,m_standArrDelayMap,mapStandLoadData,'A');
	//generate stand dep delay
	BuildDetailStandOperationDelay(pParameter,m_standDepDelayMap,mapStandLoadData,'D');
	//generate stand arr conflicts
	BuildDetailStandOperationConflict(pParameter,m_standArrConflictsMap,mapStandLoadData,'A');
	//generate stand dep conflicts
	BuildDetailStandOperationConflict(pParameter,m_standDepConfictsMap,mapStandLoadData,'D');

    // generate summary scheduled stand utilization
    BuildSummaryScheduleUtilizationData(mapStandLoadData);
    // generate summary scheduled stand idle
    BuildSummaryScheduleIdleData(mapStandLoadData, pParameter);
    // generate summary actual stand utilization
    BuildSummaryActualUtilizationData(mapStandLoadData);
    // generate summary actual stand idle
    BuildSummaryActualIdleData(mapStandLoadData, pParameter);
    // generate summary stand delay data
    BuildSummaryDelayData(mapStandLoadData);
    // generate summary stand conflict data
    BuildSummaryConflictData(mapStandLoadData);
}

void CAirsideStandMultiRunOperatinResult::InitDetailListPercentagetHead(CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData,CSortableHeaderCtrl* piSHC/* =NULL */)
{
	if (mapDetailData.empty())
		return;
	MultiRunDetailMap::iterator iter = mapDetailData.begin();
	size_t rangeCount = iter->second.size();
	for (size_t i = 0; i < rangeCount; ++i)
	{
		MultipleRunReportData delayData = iter->second.at(i);
		CString strRange;
		strRange.Format(_T("%d - %d(%%)"),delayData.m_iStart, delayData.m_iEnd);

		DWORD dwStyle = LVCFMT_LEFT;
		dwStyle &= ~HDF_OWNERDRAW;

		cxListCtrl.InsertColumn(2+i, strRange, dwStyle, 100);
	}
}

void CAirsideStandMultiRunOperatinResult::InitDetailListConfictHead(CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData,CSortableHeaderCtrl* piSHC/* =NULL */)
{
	if (mapDetailData.empty())
		return;
	MultiRunDetailMap::iterator iter = mapDetailData.begin();
	size_t rangeCount = iter->second.size();
	for (size_t i = 0; i < rangeCount; ++i)
	{
		MultipleRunReportData delayData = iter->second.at(i);
		CString strRange;
		strRange.Format(_T("%d - %d(Number of Conflicts)"),delayData.m_iStart, delayData.m_iEnd);

		DWORD dwStyle = LVCFMT_LEFT;
		dwStyle &= ~HDF_OWNERDRAW;

		cxListCtrl.InsertColumn(2+i, strRange, /*LVCFMT_LEFT*/dwStyle, 100);
	}
}

void CAirsideStandMultiRunOperatinResult::InitListHead( CXListCtrl& cxListCtrl,CParameters * parameter, int iType /*= 0*/,CSortableHeaderCtrl* piSHC/*=NULL*/ )
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	if (parameter->getReportType() == ASReportType_Detail)
	{
		DWORD headStyle = LVCFMT_CENTER;
		headStyle &= ~HDF_OWNERDRAW;
		cxListCtrl.InsertColumn(0,"",headStyle,20);

		headStyle = LVCFMT_LEFT;
		headStyle &= ~HDF_OWNERDRAW;
		cxListCtrl.InsertColumn(1, _T("SimResult"), headStyle, 80);

		CFlightStandOperationParameter* pStandPara = (CFlightStandOperationParameter*)parameter;
		switch (pStandPara->getSubType())
		{
		case CAirsideFlightStandOperationReport::ChartType_Detail_Delay:
			if (iType == 0)
			{
				InitDetailListHead(cxListCtrl,m_standArrDelayMap,piSHC);
			}
			else
			{
				InitDetailListHead(cxListCtrl,m_standDepDelayMap,piSHC);
			}
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Conflict:
			if (iType == 0)
			{
				InitDetailListConfictHead(cxListCtrl,m_standArrConflictsMap,piSHC);
			}
			else
			{
				InitDetailListConfictHead(cxListCtrl,m_standDepConfictsMap,piSHC);
			}
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_IdleTime:
			InitDetailListHead(cxListCtrl,m_standidlemap,piSHC);
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Percentage:
			InitDetailListPercentagetHead(cxListCtrl,m_standRatiomap,piSHC);
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Occupancy:
			InitDetailListHead(cxListCtrl,m_standOccupMap,piSHC);
			break;
		default:
			break;
		}
	}
    else if (parameter->getReportType() == ASReportType_Summary)
    {
        InitSummaryListHead(cxListCtrl, piSHC);
    }
}


void CAirsideStandMultiRunOperatinResult::InitDetailListHead( CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData,CSortableHeaderCtrl* piSHC/*=NULL*/ )
{
	if (mapDetailData.empty())
		return;
	MultiRunDetailMap::iterator iter = mapDetailData.begin();
	size_t rangeCount = iter->second.size();
	for (size_t i = 0; i < rangeCount; ++i)
	{
		MultipleRunReportData delayData = iter->second.at(i);
		CString strRange;
		ElapsedTime eStartTime;
		ElapsedTime eEndTime;
		eStartTime.setPrecisely(delayData.m_iStart);
		eEndTime.setPrecisely(delayData.m_iEnd);
		strRange.Format(_T("%d - %d(mins)"),eStartTime.asMinutes(), eEndTime.asMinutes());

		DWORD dwStyle = LVCFMT_LEFT;
		dwStyle &= ~HDF_OWNERDRAW;

		cxListCtrl.InsertColumn(2+i, strRange, /*LVCFMT_LEFT*/dwStyle, 100);
	}
}

void CAirsideStandMultiRunOperatinResult::SetDetail3DChartString( C2DChartData& c2dGraphData,CParameters *pParameter,int iType /*= 0*/ )
{
	ASSERT(pParameter);
	if (pParameter->getReportType() == ASReportType_Detail)
	{
		CFlightStandOperationParameter* pStandPara = (CFlightStandOperationParameter*)pParameter;
		switch (pStandPara->getSubType())
		{
		case CAirsideFlightStandOperationReport::ChartType_Detail_Delay:
			SetDetail3DDelayChartString(c2dGraphData,pParameter,iType);
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Conflict:
			SetDetail3DConflictChartString(c2dGraphData,pParameter,iType);
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_IdleTime:
			SetDetail3DIdleChartString(c2dGraphData,pParameter);
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Percentage:
			SetDetail3DRatioChartString(c2dGraphData,pParameter);
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Occupancy:
			SetDetail3DOccupancyChartString(c2dGraphData,pParameter);
			break;
		default:
			break;
		}
	}
}

void CAirsideStandMultiRunOperatinResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter, int iType /*= 0*/ )
{
	if (pParameter->getReportType() == ASReportType_Detail)
	{
		CFlightStandOperationParameter* pStandPara = (CFlightStandOperationParameter*)pParameter;
		switch (pStandPara->getSubType())
		{
		case CAirsideFlightStandOperationReport::ChartType_Detail_Delay:
			if (iType == 0)
			{
				Generate3DChartDelayData(m_standArrDelayMap,chartWnd,pStandPara,0);
			}
			else
			{
				Generate3DChartDelayData(m_standDepDelayMap,chartWnd,pStandPara,1);
			}
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Conflict:
			if (iType == 0)
			{
				Generate3DChartConflictData(m_standArrConflictsMap,chartWnd,pStandPara,0);
			}
			else
			{
				Generate3DChartConflictData(m_standDepConfictsMap,chartWnd,pStandPara,1);
			}
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_IdleTime:
			Generate3DChartCountData(m_standidlemap,chartWnd,pStandPara);
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Percentage:
			Generate3DChartPercentageData(m_standRatiomap,chartWnd,pStandPara);
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Occupancy:
			Generate3DChartCountData(m_standOccupMap,chartWnd,pStandPara);
			break;
		default:
			break;
		}
	}
    else if (pParameter->getReportType() == ASReportType_Summary)
    {
        DrawSummary3DChart(chartWnd, pParameter);
    }
}
void CAirsideStandMultiRunOperatinResult::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter, int iType /*= 0*/ )
{
	ASSERT(parameter);
	if (parameter->getReportType() == ASReportType_Detail)
	{
		CFlightStandOperationParameter* pStandPara = (CFlightStandOperationParameter*)parameter;
		switch (pStandPara->getSubType())
		{
		case CAirsideFlightStandOperationReport::ChartType_Detail_Delay:
			if (iType == 0)
			{
				FillDetailListCountContent(cxListCtrl,m_standArrDelayMap);
			}
			else
			{
				FillDetailListCountContent(cxListCtrl,m_standDepDelayMap);
			}
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Conflict:
			if (iType == 0)
			{
				FillDetailListCountContent(cxListCtrl,m_standArrConflictsMap);
			}
			else
			{
				FillDetailListCountContent(cxListCtrl,m_standDepConfictsMap);
			}
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_IdleTime:
			FillDetailListCountContent(cxListCtrl,m_standidlemap);
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Percentage:
			FillDetailListCountContent(cxListCtrl,m_standRatiomap);
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Occupancy:
			FillDetailListCountContent(cxListCtrl,m_standOccupMap);
			break;
		default:
			break;
		}
	}
    else if (parameter->getReportType() == ASReportType_Summary)
    {
        CFlightStandOperationParameter* pStandPara = (CFlightStandOperationParameter*)parameter;
        switch (pStandPara->getSubType())
        {
        case CAirsideFlightStandOperationReport::ChartType_Summary_SchedUtilization:
            FillSummaryListContent(cxListCtrl, m_summarySchedUtilizeMap);
            break;
        case CAirsideFlightStandOperationReport::ChartType_Summary_ActualUtilization:
            FillSummaryListContent(cxListCtrl, m_summaryActualUtilizeMap);
            break;
        case CAirsideFlightStandOperationReport::ChartType_Summary_SchedIdle:
            FillSummaryListContent(cxListCtrl, m_summarySchedIdleMap);
            break;
        case CAirsideFlightStandOperationReport::ChartType_Summary_ActualIdle:
            FillSummaryListContent(cxListCtrl, m_summaryActualIdleMap);
            break;
        case CAirsideFlightStandOperationReport::ChartType_Summary_Delay:
            FillSummaryListContent(cxListCtrl, m_summaryDelayMap);
            break;
        case CAirsideFlightStandOperationReport::ChartType_Summary_Conflict:
            FillSummaryConflictListContent(cxListCtrl, m_summaryConflictMap);
            break;
        default:
            break;
        }
    }
}

void CAirsideStandMultiRunOperatinResult::FillDetailListCountContent( CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData )
{
	MultiRunDetailMap::iterator iter = mapDetailData.begin();
	int idx = 0;
	for (; iter != mapDetailData.end(); ++iter)
	{
		CString strIndex;
		strIndex.Format(_T("%d"),idx+1);
		cxListCtrl.InsertItem(idx,strIndex);

		CString strSimName = iter->first;
		int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
		CString strRun = _T("");
		strRun.Format(_T("Run%d"),nCurSimResult+1);

		cxListCtrl.SetItemText(idx,1,strRun);
		cxListCtrl.SetItemData(idx,idx);
		for (size_t n = 0; n < iter->second.size(); ++n)
		{
			MultipleRunReportData delayData = iter->second.at(n);
			CString strCount;
			if(n <  iter->second.size())
				strCount.Format(_T("%d"), delayData.m_iData);
			else
				strCount.Format(_T("%d"),0) ;

			cxListCtrl.SetItemText(idx, n + 2, strCount);
		}
		idx++;
	}
}


void CAirsideStandMultiRunOperatinResult::ClearData()
{
	m_standOccupMap.clear();
	m_standidlemap.clear();
	m_standRatiomap.clear();
	m_standArrDelayMap.clear();
	m_standDepDelayMap.clear();
	m_standArrConflictsMap.clear();
	m_standDepConfictsMap.clear();
}

void CAirsideStandMultiRunOperatinResult::BuildDetailStandOperationOccupancy( CParameters* pParameter,MapMultiRunStandOperationData standOperationData )
{
	mapStandOpResult  mapLoadData;
	MapMultiRunStandOperationData::iterator iter = standOperationData.begin();
	long lMaxOccupancyTime = 0;
	for (; iter != standOperationData.end(); ++iter)
	{
		int iCount = iter->second.size();
		for (int i = 0; i < iCount; i++)
		{
			StandMultipleOperationData operationData = iter->second[i];
			mapLoadData[iter->first][operationData.m_sActualName] += operationData.m_lOccupiedTime;

			if (mapLoadData[iter->first][operationData.m_sActualName] > lMaxOccupancyTime)
			{
				lMaxOccupancyTime = mapLoadData[iter->first][operationData.m_sActualName];
			}
		}
	}

	int nIntervalSize = 0;
	ElapsedTime eMaxTime;
	eMaxTime.setPrecisely(lMaxOccupancyTime);
	nIntervalSize = eMaxTime.asSeconds() / pParameter->getInterval() ;

	if (nIntervalSize ==0)
		nIntervalSize = 1;

	mapStandOpResult::iterator mapIter = mapLoadData.begin();
	for (; mapIter != mapLoadData.end(); ++mapIter)
	{
		for (int i=0; i< nIntervalSize; i++)
		{
			MultipleRunReportData reportData;
			reportData.m_iStart = pParameter->getInterval()*i*100;
			reportData.m_iEnd = pParameter->getInterval()*(i+1)*100;

			reportData.m_iData = GetIntervalCount(reportData.m_iStart,reportData.m_iEnd,mapIter->second);

			m_standOccupMap[mapIter->first].push_back(reportData);
		}
	}

}

void CAirsideStandMultiRunOperatinResult::BuildDetailStandOperationIdel( CParameters* pParameter,MapMultiRunStandOperationData standOperationData )
{
	mapStandOpResult  mapLoadData;
	MapMultiRunStandOperationData::iterator iter = standOperationData.begin();
	long lMinOccupancyTime = (std::numeric_limits<long>::max)();
	for (; iter != standOperationData.end(); ++iter)
	{
		int iCount = iter->second.size();
		for (int i = 0; i < iCount; i++)
		{
			StandMultipleOperationData operationData = iter->second[i];
			mapLoadData[iter->first][operationData.m_sActualName] += operationData.m_lOccupiedTime;
			if(mapLoadData[iter->first][operationData.m_sActualName]  < lMinOccupancyTime)
			{
				lMinOccupancyTime = mapLoadData[iter->first][operationData.m_sActualName];
			}
		}
	}

	ElapsedTime eMinTime;
	eMinTime.setPrecisely(lMinOccupancyTime);
	int nDuration = pParameter->getEndTime().asSeconds() - pParameter->getStartTime().asSeconds();
	int nIntervalSize = (nDuration - eMinTime.asSeconds()) / (pParameter->getInterval()) ;

	if (nIntervalSize ==0)
		nIntervalSize = 1;

	mapStandOpResult::iterator mapIter = mapLoadData.begin();
	for (; mapIter != mapLoadData.end(); ++mapIter)
	{
		for (int i=0; i< nIntervalSize; i++)
		{
			MultipleRunReportData reportData;
			reportData.m_iStart = pParameter->getInterval()*i*100;
			reportData.m_iEnd = pParameter->getInterval()*(i+1)*100;

			mapStandResult::iterator standIter = mapIter->second.begin();
			for (; standIter != mapIter->second.end(); ++standIter)
			{
				long lTime = nDuration*100 -  standIter->second;

				if( reportData.m_iStart <= lTime && lTime < reportData.m_iEnd)
					reportData.m_iData++;
			}
			m_standidlemap[mapIter->first].push_back(reportData);
		}
	}
}

void CAirsideStandMultiRunOperatinResult::BuildDetailStandOperationPencentage( CParameters* pParameter,MapMultiRunStandOperationData standOperationData )
{
	mapStandOpResult  mapLoadData;
	MapMultiRunStandOperationData::iterator iter = standOperationData.begin();
	long lMaxOccupancyTime = 0;
	for (; iter != standOperationData.end(); ++iter)
	{
		int iCount = iter->second.size();
		for (int i = 0; i < iCount; i++)
		{
			StandMultipleOperationData operationData = iter->second[i];
			mapLoadData[iter->first][operationData.m_sActualName] += operationData.m_lOccupiedTime;
			if (mapLoadData[iter->first][operationData.m_sActualName] > lMaxOccupancyTime)
			{
				lMaxOccupancyTime = mapLoadData[iter->first][operationData.m_sActualName];
			}
		}
	}

	int nIntervalSize = 0;
	ElapsedTime eMaxTime;
	eMaxTime.setPrecisely(lMaxOccupancyTime);
	int nDuration = pParameter->getEndTime().asSeconds() - pParameter->getStartTime().asSeconds();
	nIntervalSize = int((double)eMaxTime.asSeconds()/ (double)(nDuration)*100)/10 ;

	mapStandOpResult::iterator mapIter = mapLoadData.begin();
	for (; mapIter != mapLoadData.end(); ++mapIter)
	{
		for (int i=0; i< nIntervalSize; i++)
		{
			MultipleRunReportData reportData;
			reportData.m_iStart = i *10;
			reportData.m_iEnd = (i+1) *10;

			mapStandResult::iterator standIter = mapIter->second.begin();
			for (; standIter != mapIter->second.end(); ++standIter)
			{
				long lTime = standIter->second;
				ElapsedTime eTime;
				eTime.setPrecisely(lTime);
				int nPercent = static_cast<int>(((double)eTime.asSeconds())/nDuration * 100);

				if( reportData.m_iStart <= nPercent && nPercent < reportData.m_iEnd)
					reportData.m_iData++;
			}
			m_standRatiomap[mapIter->first].push_back(reportData);
		}

		for (int idx = nIntervalSize; idx < 10; idx++)
		{
			MultipleRunReportData reportData;
			reportData.m_iStart = idx *10;
			reportData.m_iEnd = (idx+1) *10;
			reportData.m_iData = 0;
			m_standRatiomap[mapIter->first].push_back(reportData);
		}
	}
}

static bool DataCompareByStand(StandMultipleOperationData fistOperation, StandMultipleOperationData secondOperation)
{
	if (fistOperation.m_sActualName < secondOperation.m_sActualName)
		return true;

	return false;
}

void CAirsideStandMultiRunOperatinResult::BuildDetailStandOperationDelay( CParameters* pParameter,MultiRunDetailMap& mapDetailData,MapMultiRunStandOperationData standOperationData,char fltMode )
{
	mapLoadResult  mapLoadData;
	MapMultiRunStandOperationData::iterator iter = standOperationData.begin();
	long lMaxDelayTime = 0;
	for (; iter != standOperationData.end(); ++iter)
	{
		std::sort(standOperationData[iter->first].begin(),standOperationData[iter->first].end(),DataCompareByStand);
		if(standOperationData[iter->first].empty())
			continue;

		long lTotal = 0; 
		mapLoadData[iter->first].clear();
		CString sStandName =standOperationData[iter->first].front().m_sActualName;
		for (unsigned i = 0; i < standOperationData[iter->first].size(); i++)
		{
			StandMultipleOperationData data = standOperationData[iter->first].at(i);
			if (!data.IsDelay() || data.m_fltmode != fltMode)
				continue;

			if (data.m_sActualName == sStandName)
			{
				lTotal += data.m_lDelayEnter;
				lTotal += data.m_lDelayLeaving;
			}
			else
			{
				if (lTotal > lMaxDelayTime)
				{
					lMaxDelayTime = lTotal;
				}
				mapLoadData[iter->first].push_back(lTotal);
				sStandName = data.m_sActualName;
				lTotal = data.m_lDelayEnter + data.m_lDelayLeaving;
			}
		}
	}

	int nIntervalSize = 0;
	ElapsedTime eMaxTime;
	eMaxTime.setPrecisely(lMaxDelayTime);
	nIntervalSize = eMaxTime.asSeconds() / pParameter->getInterval();

	if (nIntervalSize ==0)
		nIntervalSize = 1;

	mapLoadResult::iterator mapIter = mapLoadData.begin();
	for (; mapIter != mapLoadData.end(); ++mapIter)
	{
		for (int i=0; i< nIntervalSize; i++)
		{
			MultipleRunReportData reportData;
			reportData.m_iStart = pParameter->getInterval()*i*100;
			reportData.m_iEnd = pParameter->getInterval()*(i+1)*100;;

			for (size_t j = 0; j < mapIter->second.size(); j++)
			{
				long lDelay= mapIter->second.at(j);

				if( reportData.m_iStart <= lDelay && lDelay < reportData.m_iEnd)
					reportData.m_iData++;						
			}
			mapDetailData[mapIter->first].push_back(reportData);
		}
	}
}

void CAirsideStandMultiRunOperatinResult::BuildDetailStandOperationConflict( CParameters* pParameter,MultiRunDetailMap& mapDetailData,MapMultiRunStandOperationData standOperationData,char fltMode)
{
	mapLoadResult  mapLoadData;
	MapMultiRunStandOperationData::iterator iter = standOperationData.begin();
	long nMaxConflicts = 0;
	for (; iter != standOperationData.end(); ++iter)
	{
		std::sort(standOperationData[iter->first].begin(),standOperationData[iter->first].end(),DataCompareByStand);
		if(standOperationData[iter->first].empty())
			continue;

		int nTotal = 0; 
		mapLoadData[iter->first].clear();
		CString sStandName =standOperationData[iter->first].front().m_sActualName;
		for (unsigned i = 0; i < standOperationData[iter->first].size(); i++)
		{
			StandMultipleOperationData data = standOperationData[iter->first].at(i);
			if (!data.IsConflict() ||  data.m_fltmode != fltMode)
				continue;

			if (data.m_sActualName == sStandName)
				nTotal ++;
			else
			{
				if (nTotal > nMaxConflicts)
				{
					nMaxConflicts = nTotal;
				}
				mapLoadData[iter->first].push_back(nTotal);
				sStandName = data.m_sActualName;
				nTotal = 1;
			}
		}
	}

	int nIntervalSize = 0;
	nIntervalSize = nMaxConflicts / 5 ;

	if (nIntervalSize ==0)
		nIntervalSize = 1;

	mapLoadResult::iterator mapIter = mapLoadData.begin();
	for (; mapIter != mapLoadData.end(); ++mapIter)
	{
		for (int i=0; i< nIntervalSize; i++)
		{
			MultipleRunReportData reportData;
			reportData.m_iStart = i*5;
			reportData.m_iEnd = (i+1)*5;

			for (size_t j = 0; j < mapIter->second.size(); j++)
			{
				int nConflict = mapIter->second.at(j);

				if( reportData.m_iStart <= nConflict && nConflict < reportData.m_iEnd)
					reportData.m_iData++;						
			}
			mapDetailData[mapIter->first].push_back(reportData);
		}
	}
}

int CAirsideStandMultiRunOperatinResult::GetIntervalCount( long iStart, long iEnd, mapStandResult mapData,long iIgnore /*= 0*/ ) const
{
	int iCount = 0;
	mapStandResult::iterator iter = mapData.begin();
	for ( ;iter != mapData.end(); ++iter)
	{
		long iData = iter->second;
		if (iData < iIgnore)//ignore necessary secs
			continue;

		if (iData >= iStart && iData < iEnd)
		{
			iCount++;
		}
	}
	return iCount;
}

void CAirsideStandMultiRunOperatinResult::Generate3DChartPercentageData(MultiRunDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter)
{
	C2DChartData c2dGraphData;

	SetDetail3DChartString(c2dGraphData,pParameter);

	if (mapDetailData.empty() == true)
		return;

	MultiRunDetailMap::iterator iter = mapDetailData.begin();
	for (unsigned iTitle = 0; iTitle < iter->second.size(); iTitle++)
	{
		MultipleRunReportData delayData = iter->second.at(iTitle);
		long iInterval = (delayData.m_iEnd - delayData.m_iStart) * (iTitle + 1);
		CString strTimeRange;
		strTimeRange.Format(_T("%d"),iInterval);
		c2dGraphData.m_vrXTickTitle.push_back(strTimeRange);
	}

	for (; iter != mapDetailData.end(); ++iter)
	{
		CString strSimName = iter->first;
		int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
		CString strLegend;
		strLegend.Format(_T("Run%d"),nCurSimResult+1);
		c2dGraphData.m_vrLegend.push_back(strLegend);

		std::vector<double>  vData;
		for (unsigned i = 0; i < iter->second.size(); i++)
		{
			MultipleRunReportData delayData = iter->second.at(i);
			vData.push_back(delayData.m_iData);
		}
		c2dGraphData.m_vr2DChartData.push_back(vData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CAirsideStandMultiRunOperatinResult::Generate3DChartCountData( MultiRunDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter )
{
	C2DChartData c2dGraphData;

	SetDetail3DChartString(c2dGraphData,pParameter);

	if (mapDetailData.empty() == true)
		return;

	MultiRunDetailMap::iterator iter = mapDetailData.begin();
	for (unsigned iTitle = 0; iTitle < iter->second.size(); iTitle++)
	{
		MultipleRunReportData delayData = iter->second.at(iTitle);
		ElapsedTime eStartTime;
		ElapsedTime eEndTime;
		eStartTime.setPrecisely(delayData.m_iStart);
		eEndTime.setPrecisely(delayData.m_iEnd);

		long iInterval = delayData.m_iEnd - delayData.m_iStart;
		long lTime = iInterval*(iTitle+1);
		ElapsedTime estTempTime;
		estTempTime.setPrecisely(lTime);
		CString strTimeRange;
		strTimeRange.Format(_T("%d"),estTempTime.asMinutes());
		c2dGraphData.m_vrXTickTitle.push_back(strTimeRange);
	}

	for (iter = mapDetailData.begin(); iter != mapDetailData.end(); ++iter)
	{
		CString strSimName = iter->first;
		int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
		CString strLegend;
		strLegend.Format(_T("Run%d"),nCurSimResult+1);
		c2dGraphData.m_vrLegend.push_back(strLegend);

		std::vector<double>  vData;
		for (unsigned i = 0; i < iter->second.size(); i++)
		{
			MultipleRunReportData delayData = iter->second.at(i);
			vData.push_back(delayData.m_iData);
		}
		c2dGraphData.m_vr2DChartData.push_back(vData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CAirsideStandMultiRunOperatinResult::Generate3DChartConflictData( MultiRunDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter,int iType )
{
	C2DChartData c2dGraphData;

	SetDetail3DChartString(c2dGraphData,pParameter,iType);

	if (mapDetailData.empty() == true)
		return;

	MultiRunDetailMap::iterator iter = mapDetailData.begin();
	for (unsigned iTitle = 0; iTitle < iter->second.size(); iTitle++)
	{
		MultipleRunReportData delayData = iter->second.at(iTitle);
		ElapsedTime eStartTime;
		ElapsedTime eEndTime;
		eStartTime.setPrecisely(delayData.m_iStart);
		eEndTime.setPrecisely(delayData.m_iEnd);

		CString strTimeRange;
		strTimeRange.Format(_T("%d"),(iTitle+1)*5);
		c2dGraphData.m_vrXTickTitle.push_back(strTimeRange);
	}

	for (iter = mapDetailData.begin(); iter != mapDetailData.end(); ++iter)
	{
		CString strSimName = iter->first;
		int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
		CString strLegend;
		strLegend.Format(_T("Run%d"),nCurSimResult+1);
		c2dGraphData.m_vrLegend.push_back(strLegend);

		std::vector<double>  vData;
		for (unsigned i = 0; i < iter->second.size(); i++)
		{
			MultipleRunReportData delayData = iter->second.at(i);
			vData.push_back(delayData.m_iData);
		}
		c2dGraphData.m_vr2DChartData.push_back(vData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CAirsideStandMultiRunOperatinResult::Generate3DChartDelayData( MultiRunDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter,int iType )
{
	C2DChartData c2dGraphData;

	SetDetail3DChartString(c2dGraphData,pParameter,iType);

	if (mapDetailData.empty() == true)
		return;

	MultiRunDetailMap::iterator iter = mapDetailData.begin();
	for (unsigned iTitle = 0; iTitle < iter->second.size(); iTitle++)
	{
		MultipleRunReportData delayData = iter->second.at(iTitle);
		ElapsedTime eStartTime;
		ElapsedTime eEndTime;
		eStartTime.setPrecisely(delayData.m_iStart);
		eEndTime.setPrecisely(delayData.m_iEnd);

		long iInterval = delayData.m_iEnd - delayData.m_iStart;
		long lTime = iInterval*(iTitle+1);
		ElapsedTime estTempTime;
		estTempTime.setPrecisely(lTime);
		CString strTimeRange;
		strTimeRange.Format(_T("%d"),estTempTime.asMinutes());
		c2dGraphData.m_vrXTickTitle.push_back(strTimeRange);
	}

	for (iter = mapDetailData.begin(); iter != mapDetailData.end(); ++iter)
	{
		CString strSimName = iter->first;
		int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
		CString strLegend;
		strLegend.Format(_T("Run%d"),nCurSimResult+1);
		c2dGraphData.m_vrLegend.push_back(strLegend);

		std::vector<double>  vData;
		for (unsigned i = 0; i < iter->second.size(); i++)
		{
			MultipleRunReportData delayData = iter->second.at(i);
			vData.push_back(delayData.m_iData);
		}
		c2dGraphData.m_vr2DChartData.push_back(vData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CAirsideStandMultiRunOperatinResult::SetDetail3DConflictChartString( C2DChartData& c2dGraphData,CParameters *pParameter,int iType /*= 0*/ )
{
	c2dGraphData.m_strChartTitle = _T("Stand conflicts");
	c2dGraphData.m_strYtitle = _T("Number of Stands");
	c2dGraphData.m_strXtitle = _T("Number of conflicts");	

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Detailed Stand Utilization %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;
}

void CAirsideStandMultiRunOperatinResult::SetDetail3DDelayChartString( C2DChartData& c2dGraphData,CParameters *pParameter,int iType /*= 0*/ )
{
	c2dGraphData.m_strChartTitle = _T("Stand Delays");
	c2dGraphData.m_strYtitle = _T("Number of Stands");
	c2dGraphData.m_strXtitle = _T("Time (mins)");	

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Detailed Stand Utilization %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;
}


void CAirsideStandMultiRunOperatinResult::SetDetail3DIdleChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T("Detailed Stand Utilization Report(Unused time)");
	c2dGraphData.m_strYtitle = _T("Number of Stands");
	c2dGraphData.m_strXtitle = _T("Unused time (mins at stand)");	

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Detailed Stand Utilization %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;
}

void CAirsideStandMultiRunOperatinResult::SetDetail3DRatioChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T("Detailed Stand Utilization Report(Occupancy ratio)");
	c2dGraphData.m_strYtitle = _T("Number of Stands");
	c2dGraphData.m_strXtitle = _T("Occupancy (% of report time)");	

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Detailed Stand Utilization %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;
}

void CAirsideStandMultiRunOperatinResult::SetDetail3DOccupancyChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T("Detailed Stand Utilization Report(Occupancy)");
	c2dGraphData.m_strYtitle = _T("Number of Stands");
	c2dGraphData.m_strXtitle = _T("Occupancy (mins at stand)");	

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Detailed Stand Utilization %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;
}

void CAirsideStandMultiRunOperatinResult::BuildSummaryScheduleUtilizationData(MapMultiRunStandOperationData& standOperationData)
{
    mapStandOpResult  mapLoadData;
    MapMultiRunStandOperationData::iterator iter = standOperationData.begin();

    for (; iter != standOperationData.end(); ++iter)
    {
		mapLoadData[iter->first].clear();
        int iCount = iter->second.size();
        for (int i = 0; i < iCount; ++i)
        {
            StandMultipleOperationData operationData = iter->second[i];
            mapLoadData[iter->first][operationData.m_sSchedName] += operationData.m_lSchedOccupancy;
        }
    }

    CStatisticalTools<double> tempTool;
    mapStandOpResult::iterator simIter = mapLoadData.begin();
    for(; simIter!=mapLoadData.end(); ++simIter)
    {
        tempTool.Clear();
        mapStandResult::iterator standIter = simIter->second.begin();
        for(; standIter!=simIter->second.end(); standIter++)
        {
            tempTool.AddNewData(standIter->second/100);
        }
        tempTool.SortData();

        m_summarySchedUtilizeMap[simIter->first].m_estTotal = (long)(tempTool.GetSum());
        m_summarySchedUtilizeMap[simIter->first].m_estMin = (long)(tempTool.GetMin());
        m_summarySchedUtilizeMap[simIter->first].m_estAverage = (long)(tempTool.GetAvarage());
        m_summarySchedUtilizeMap[simIter->first].m_estMax = (long)(tempTool.GetMax());
        m_summarySchedUtilizeMap[simIter->first].m_estQ1 = (long)(tempTool.GetPercentile(25));
        m_summarySchedUtilizeMap[simIter->first].m_estQ2 = (long)(tempTool.GetPercentile(50));
        m_summarySchedUtilizeMap[simIter->first].m_estQ3 = (long)(tempTool.GetPercentile(75));
        m_summarySchedUtilizeMap[simIter->first].m_estP1 = (long)(tempTool.GetPercentile(1));
        m_summarySchedUtilizeMap[simIter->first].m_estP5 = (long)(tempTool.GetPercentile(5));
        m_summarySchedUtilizeMap[simIter->first].m_estP10 = (long)(tempTool.GetPercentile(10));
        m_summarySchedUtilizeMap[simIter->first].m_estP90 = (long)(tempTool.GetPercentile(90));
        m_summarySchedUtilizeMap[simIter->first].m_estP95 = (long)(tempTool.GetPercentile(95));
        m_summarySchedUtilizeMap[simIter->first].m_estP99 = (long)(tempTool.GetPercentile(99));
        m_summarySchedUtilizeMap[simIter->first].m_estSigma = (long)(tempTool.GetSigma());
    }
}

void CAirsideStandMultiRunOperatinResult::BuildSummaryScheduleIdleData(MapMultiRunStandOperationData& standOperationData, CParameters *pParameter)
{
    mapStandOpResult  mapLoadData;
    MapMultiRunStandOperationData::iterator iter = standOperationData.begin();

    for (; iter != standOperationData.end(); ++iter)
    {
		mapLoadData[iter->first].clear();
        int iCount = iter->second.size();
        for (int i = 0; i < iCount; i++)
        {
            StandMultipleOperationData operationData = iter->second[i];
            mapLoadData[iter->first][operationData.m_sSchedName] += operationData.m_lSchedOccupancy;
        }
    }

    CStatisticalTools<double> tempTool;
    mapStandOpResult::iterator simIter = mapLoadData.begin();
    long lDuration = pParameter->getEndTime().asSeconds() - pParameter->getStartTime().asSeconds();
    for(; simIter!=mapLoadData.end(); ++simIter)
    {
        tempTool.Clear();
        mapStandResult::iterator standIter = simIter->second.begin();
        for(; standIter!=simIter->second.end(); ++standIter)
        {
            long lData = lDuration - standIter->second/100;
            if(lData<0)
                lData = 0;
            tempTool.AddNewData(lData);
        }
        tempTool.SortData();

        m_summarySchedIdleMap[simIter->first].m_estTotal = (long)(tempTool.GetSum());
        m_summarySchedIdleMap[simIter->first].m_estMin = (long)(tempTool.GetMin());
        m_summarySchedIdleMap[simIter->first].m_estAverage = (long)(tempTool.GetAvarage());
        m_summarySchedIdleMap[simIter->first].m_estMax = (long)(tempTool.GetMax());
        m_summarySchedIdleMap[simIter->first].m_estQ1 = (long)(tempTool.GetPercentile(25));
        m_summarySchedIdleMap[simIter->first].m_estQ2 = (long)(tempTool.GetPercentile(50));
        m_summarySchedIdleMap[simIter->first].m_estQ3 = (long)(tempTool.GetPercentile(75));
        m_summarySchedIdleMap[simIter->first].m_estP1 = (long)(tempTool.GetPercentile(1));
        m_summarySchedIdleMap[simIter->first].m_estP5 = (long)(tempTool.GetPercentile(5));
        m_summarySchedIdleMap[simIter->first].m_estP10 = (long)(tempTool.GetPercentile(10));
        m_summarySchedIdleMap[simIter->first].m_estP90 = (long)(tempTool.GetPercentile(90));
        m_summarySchedIdleMap[simIter->first].m_estP95 = (long)(tempTool.GetPercentile(95));
        m_summarySchedIdleMap[simIter->first].m_estP99 = (long)(tempTool.GetPercentile(99));
        m_summarySchedIdleMap[simIter->first].m_estSigma = (long)(tempTool.GetSigma());
    }
}

void CAirsideStandMultiRunOperatinResult::BuildSummaryActualUtilizationData(MapMultiRunStandOperationData& standOperationData)
{
    mapStandOpResult  mapLoadData;
    MapMultiRunStandOperationData::iterator iter = standOperationData.begin();

    for (; iter != standOperationData.end(); ++iter)
    {
		mapLoadData[iter->first].clear();
        int iCount = iter->second.size();
        for (int i = 0; i < iCount; i++)
        {
            StandMultipleOperationData operationData = iter->second[i];
            mapLoadData[iter->first][operationData.m_sActualName] += operationData.m_lOccupiedTime;
        }
    }

    CStatisticalTools<double> tempTool;
    mapStandOpResult::iterator simIter = mapLoadData.begin();
    for(; simIter!=mapLoadData.end(); ++simIter)
    {
        tempTool.Clear();
        mapStandResult::iterator standIter = simIter->second.begin();
        for(; standIter!=simIter->second.end(); ++standIter)
        {
            tempTool.AddNewData(standIter->second/100);
        }
        tempTool.SortData();

        m_summaryActualUtilizeMap[simIter->first].m_estTotal = (long)(tempTool.GetSum());
        m_summaryActualUtilizeMap[simIter->first].m_estMin = (long)(tempTool.GetMin());
        m_summaryActualUtilizeMap[simIter->first].m_estAverage = (long)(tempTool.GetAvarage());
        m_summaryActualUtilizeMap[simIter->first].m_estMax = (long)(tempTool.GetMax());
        m_summaryActualUtilizeMap[simIter->first].m_estQ1 = (long)(tempTool.GetPercentile(25));
        m_summaryActualUtilizeMap[simIter->first].m_estQ2 = (long)(tempTool.GetPercentile(50));
        m_summaryActualUtilizeMap[simIter->first].m_estQ3 = (long)(tempTool.GetPercentile(75));
        m_summaryActualUtilizeMap[simIter->first].m_estP1 = (long)(tempTool.GetPercentile(1));
        m_summaryActualUtilizeMap[simIter->first].m_estP5 = (long)(tempTool.GetPercentile(5));
        m_summaryActualUtilizeMap[simIter->first].m_estP10 = (long)(tempTool.GetPercentile(10));
        m_summaryActualUtilizeMap[simIter->first].m_estP90 = (long)(tempTool.GetPercentile(90));
        m_summaryActualUtilizeMap[simIter->first].m_estP95 = (long)(tempTool.GetPercentile(95));
        m_summaryActualUtilizeMap[simIter->first].m_estP99 = (long)(tempTool.GetPercentile(99));
        m_summaryActualUtilizeMap[simIter->first].m_estSigma = (long)(tempTool.GetSigma());
    }
}

void CAirsideStandMultiRunOperatinResult::BuildSummaryActualIdleData(MapMultiRunStandOperationData& standOperationData, CParameters *pParameter)
{
    mapStandOpResult  mapLoadData;
    MapMultiRunStandOperationData::iterator iter = standOperationData.begin();

    for (; iter != standOperationData.end(); ++iter)
    {
		mapLoadData[iter->first].clear();
        int iCount = iter->second.size();
        for (int i = 0; i < iCount; i++)
        {
            StandMultipleOperationData operationData = iter->second[i];
            mapLoadData[iter->first][operationData.m_sActualName] += operationData.m_lOccupiedTime;
        }
    }

    CStatisticalTools<double> tempTool;
    mapStandOpResult::iterator simIter = mapLoadData.begin();
    long lDuration = pParameter->getEndTime().asSeconds() - pParameter->getStartTime().asSeconds();
    for(; simIter!=mapLoadData.end(); ++simIter)
    {
        tempTool.Clear();
        mapStandResult::iterator standIter = simIter->second.begin();
        for(; standIter!=simIter->second.end(); ++standIter)
        {
            long lData = lDuration - standIter->second/100;
            if(lData<0)
                lData = 0;
            tempTool.AddNewData(lData);
        }
        tempTool.SortData();

        m_summaryActualIdleMap[simIter->first].m_estTotal = (long)(tempTool.GetSum());
        m_summaryActualIdleMap[simIter->first].m_estMin = (long)(tempTool.GetMin());
        m_summaryActualIdleMap[simIter->first].m_estAverage = (long)(tempTool.GetAvarage());
        m_summaryActualIdleMap[simIter->first].m_estMax = (long)(tempTool.GetMax());
        m_summaryActualIdleMap[simIter->first].m_estQ1 = (long)(tempTool.GetPercentile(25));
        m_summaryActualIdleMap[simIter->first].m_estQ2 = (long)(tempTool.GetPercentile(50));
        m_summaryActualIdleMap[simIter->first].m_estQ3 = (long)(tempTool.GetPercentile(75));
        m_summaryActualIdleMap[simIter->first].m_estP1 = (long)(tempTool.GetPercentile(1));
        m_summaryActualIdleMap[simIter->first].m_estP5 = (long)(tempTool.GetPercentile(5));
        m_summaryActualIdleMap[simIter->first].m_estP10 = (long)(tempTool.GetPercentile(10));
        m_summaryActualIdleMap[simIter->first].m_estP90 = (long)(tempTool.GetPercentile(90));
        m_summaryActualIdleMap[simIter->first].m_estP95 = (long)(tempTool.GetPercentile(95));
        m_summaryActualIdleMap[simIter->first].m_estP99 = (long)(tempTool.GetPercentile(99));
        m_summaryActualIdleMap[simIter->first].m_estSigma = (long)(tempTool.GetSigma());
    }
}

void CAirsideStandMultiRunOperatinResult::BuildSummaryDelayData(MapMultiRunStandOperationData& standOperationData)
{
    mapStandOpResult  mapLoadData;
    MapMultiRunStandOperationData::iterator iter = standOperationData.begin();

    for (; iter != standOperationData.end(); ++iter)
    {
		mapLoadData[iter->first].clear();
        int iCount = iter->second.size();
        for (int i = 0; i < iCount; i++)
        {
            StandMultipleOperationData operationData = iter->second[i];
            mapLoadData[iter->first][operationData.m_sActualName] += operationData.m_lDelayEnter;
            mapLoadData[iter->first][operationData.m_sActualName] += operationData.m_lDelayLeaving;
        }
    }

    CStatisticalTools<double> tempTool;
    mapStandOpResult::iterator simIter = mapLoadData.begin();
    for(; simIter!=mapLoadData.end(); ++simIter)
    {
        tempTool.Clear();

        // Remove the zero item.
        mapStandResult::iterator standIter2 = simIter->second.begin();
        while(standIter2!=simIter->second.end())
        {
            mapStandResult::iterator tempIter = standIter2;
            standIter2++;

            if(tempIter->second == 0)
            {
                simIter->second.erase(tempIter);
            }
        }

        mapStandResult::iterator standIter = simIter->second.begin();
        unsigned idx = 0; // single report calculation of stand conflict: StandOperationDataCalculation.h: 246
        // the last one data is discarded, need to be fixed.
        unsigned iSize = simIter->second.size();
        for(; standIter!=simIter->second.end()&& idx < iSize - 1; ++standIter, ++idx)
        {
            if(standIter->second != 0)
                tempTool.AddNewData(standIter->second/100);
        }
        tempTool.SortData();

        m_summaryDelayMap[simIter->first].m_estTotal = (long)(tempTool.GetSum());
        m_summaryDelayMap[simIter->first].m_estMin = (long)(tempTool.GetMin());
        m_summaryDelayMap[simIter->first].m_estAverage = (long)(tempTool.GetAvarage());
        m_summaryDelayMap[simIter->first].m_estMax = (long)(tempTool.GetMax());
        m_summaryDelayMap[simIter->first].m_estQ1 = (long)(tempTool.GetPercentile(25));
        m_summaryDelayMap[simIter->first].m_estQ2 = (long)(tempTool.GetPercentile(50));
        m_summaryDelayMap[simIter->first].m_estQ3 = (long)(tempTool.GetPercentile(75));
        m_summaryDelayMap[simIter->first].m_estP1 = (long)(tempTool.GetPercentile(1));
        m_summaryDelayMap[simIter->first].m_estP5 = (long)(tempTool.GetPercentile(5));
        m_summaryDelayMap[simIter->first].m_estP10 = (long)(tempTool.GetPercentile(10));
        m_summaryDelayMap[simIter->first].m_estP90 = (long)(tempTool.GetPercentile(90));
        m_summaryDelayMap[simIter->first].m_estP95 = (long)(tempTool.GetPercentile(95));
        m_summaryDelayMap[simIter->first].m_estP99 = (long)(tempTool.GetPercentile(99));
        m_summaryDelayMap[simIter->first].m_estSigma = (long)(tempTool.GetSigma());
    }
}

void CAirsideStandMultiRunOperatinResult::BuildSummaryConflictData(MapMultiRunStandOperationData& standOperationData)
{
    mapStandOpResult  mapLoadData;
    MapMultiRunStandOperationData::iterator iter = standOperationData.begin();

    for (; iter != standOperationData.end(); ++iter)
    {
        int iCount = iter->second.size();
		mapLoadData[iter->first].clear();
        for (int i = 0; i < iCount; i++)
        {
            StandMultipleOperationData operationData = iter->second[i];
            if(operationData.m_sActualName != operationData.m_sSchedName)
            {
                mapLoadData[iter->first][operationData.m_sActualName]++;
            }
        }
    }

    CStatisticalTools<double> tempTool;
    mapStandOpResult::iterator simIter = mapLoadData.begin();
    for(; simIter!=mapLoadData.end(); ++simIter)
    {
        tempTool.Clear();
        mapStandResult::iterator standIter = simIter->second.begin();
        unsigned idx = 0; // single report calculation of stand conflict: StandOperationDataCalculation.h: 182
                          // the last one data is discarded.
        unsigned iSize = simIter->second.size();
        for(; standIter!=simIter->second.end()&& idx < iSize - 1; ++standIter, ++idx)
        {
            tempTool.AddNewData(standIter->second);
        }
        tempTool.SortData();

        m_summaryConflictMap[simIter->first].m_estTotal.setPrecisely((long)(tempTool.GetSum()*100 + 0.5));
        m_summaryConflictMap[simIter->first].m_estMin.setPrecisely((long)(tempTool.GetMin()*100 + 0.5));
        m_summaryConflictMap[simIter->first].m_estAverage.setPrecisely((long)(tempTool.GetAvarage()*100 + 0.5));
        m_summaryConflictMap[simIter->first].m_estMax.setPrecisely((long)(tempTool.GetMax()*100 + 0.5));
        m_summaryConflictMap[simIter->first].m_estQ1.setPrecisely((long)(tempTool.GetPercentile(25)*100 + 0.5));
        m_summaryConflictMap[simIter->first].m_estQ2.setPrecisely((long)(tempTool.GetPercentile(50)*100 + 0.5));
        m_summaryConflictMap[simIter->first].m_estQ3.setPrecisely((long)(tempTool.GetPercentile(75)*100 + 0.5));
        m_summaryConflictMap[simIter->first].m_estP1.setPrecisely((long)(tempTool.GetPercentile(1)*100 + 0.5));
        m_summaryConflictMap[simIter->first].m_estP5.setPrecisely((long)(tempTool.GetPercentile(5)*100 + 0.5));
        m_summaryConflictMap[simIter->first].m_estP10.setPrecisely((long)(tempTool.GetPercentile(10)*100 + 0.5));
        m_summaryConflictMap[simIter->first].m_estP90.setPrecisely((long)(tempTool.GetPercentile(90)*100 + 0.5));
        m_summaryConflictMap[simIter->first].m_estP95.setPrecisely((long)(tempTool.GetPercentile(95)*100 + 0.5));
        m_summaryConflictMap[simIter->first].m_estP99.setPrecisely((long)(tempTool.GetPercentile(99)*100 + 0.5));
        m_summaryConflictMap[simIter->first].m_estSigma.setPrecisely((long)(tempTool.GetSigma()*100 + 0.5));
    }
}

void CAirsideStandMultiRunOperatinResult::InitSummaryListHead( CXListCtrl &cxListCtrl, CSortableHeaderCtrl* piSHC )
{
    int nCurCol = 0;
    DWORD headStyle = LVCFMT_CENTER;
    headStyle &= ~HDF_OWNERDRAW;
    cxListCtrl.InsertColumn(nCurCol,"",headStyle,20);
    nCurCol++;

    headStyle = LVCFMT_LEFT;
    headStyle &= ~HDF_OWNERDRAW;
    cxListCtrl.InsertColumn(nCurCol, _T("SimResult"), headStyle, 80);
    nCurCol++;

    cxListCtrl.InsertColumn(nCurCol, _T("Total"), headStyle, 100);
    nCurCol++;

    int nCount = sizeof(strSummaryListTitle)/sizeof(strSummaryListTitle[0]);
    for(int i=0; i<nCount; i++)
    {
        cxListCtrl.InsertColumn(nCurCol, strSummaryListTitle[i], LVCFMT_LEFT, 100);
        if(piSHC)
        {
            piSHC->SetDataType(nCurCol,dtTIME);
        }
        nCurCol++;
    }
}

void CAirsideStandMultiRunOperatinResult::FillSummaryListContent(CXListCtrl &cxListCtrl, MultiRunSummaryMap &multiRunSummaryMap)
{
    MultiRunSummaryMap::iterator iter = multiRunSummaryMap.begin();
    int idx = 0;
    for (; iter != multiRunSummaryMap.end(); iter++)
    {
        CString strIndex;
        strIndex.Format(_T("%d"),idx+1);
        cxListCtrl.InsertItem(idx,strIndex);

        CString strSimName = iter->first;
        int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
        CString strRun = _T("");
        strRun.Format(_T("Run%d"),nCurSimResult+1);
        int nCurCol = 1;
        cxListCtrl.SetItemText(idx, nCurCol, strRun);
        cxListCtrl.SetItemData(idx, idx);
        nCurCol++;

        CString strTemp;
        strTemp = iter->second.m_estTotal.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second.m_estMin.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second.m_estAverage.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second.m_estMax.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second.m_estQ1.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second.m_estQ2.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second.m_estQ3.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second.m_estP1.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second.m_estP5.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second.m_estP10.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second.m_estP90.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second.m_estP95.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second.m_estP99.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second.m_estSigma.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        idx++;
    }
}

void CAirsideStandMultiRunOperatinResult::DrawSummary3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
    C2DChartData c2dGraphData;

    CFlightStandOperationParameter* pStandPara = (CFlightStandOperationParameter*)pParameter;
    switch (pStandPara->getSubType())
    {
    case CAirsideFlightStandOperationReport::ChartType_Summary_SchedUtilization:
        SetSummarySchedUtilize3DChartString(c2dGraphData, pParameter);
        GenerateSummary2DChartData(c2dGraphData, m_summarySchedUtilizeMap);
        break;
    case CAirsideFlightStandOperationReport::ChartType_Summary_ActualUtilization:
        SetSummaryActualUtilize3DChartString(c2dGraphData, pParameter);
        GenerateSummary2DChartData(c2dGraphData, m_summaryActualUtilizeMap);
        break;
    case CAirsideFlightStandOperationReport::ChartType_Summary_SchedIdle:
        SetSummarySchedIdle3DChartString(c2dGraphData, pParameter);
        GenerateSummary2DChartData(c2dGraphData, m_summarySchedIdleMap);
        break;
    case CAirsideFlightStandOperationReport::ChartType_Summary_ActualIdle:
        SetSummaryActualIdle3DChartString(c2dGraphData, pParameter);
        GenerateSummary2DChartData(c2dGraphData, m_summaryActualIdleMap);
        break;
    case CAirsideFlightStandOperationReport::ChartType_Summary_Delay:
        SetSummaryDelay3DChartString(c2dGraphData, pParameter);
        GenerateSummary2DChartData(c2dGraphData, m_summaryDelayMap);
        break;
    case CAirsideFlightStandOperationReport::ChartType_Summary_Conflict:
        SetSummaryConflict3DChartString(c2dGraphData, pParameter);
        GenerateSummary2DChartData(c2dGraphData, m_summaryConflictMap);
        break;
    default:
        break;
    }

    chartWnd.DrawChart(c2dGraphData);
}

void CAirsideStandMultiRunOperatinResult::SetSummarySchedUtilize3DChartString(C2DChartData& c2dGraphData,CParameters *pParameter)
{
    c2dGraphData.m_strChartTitle = _T(" Summary Schedule Stand Utilization ");
    c2dGraphData.m_strYtitle = _T("Time(second)");
    c2dGraphData.m_strXtitle.Format(_T("Runs"));

    CString strFooter(_T(""));
    strFooter.Format(_T("Summary Stand Operations %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideStandMultiRunOperatinResult::SetSummarySchedIdle3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter)
{
    c2dGraphData.m_strChartTitle = _T(" Summary Schedule Stand Idle ");
    c2dGraphData.m_strYtitle = _T("Time(second)");
    c2dGraphData.m_strXtitle.Format(_T("Runs"));

    CString strFooter(_T(""));
    strFooter.Format(_T("Summary Stand Operations %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideStandMultiRunOperatinResult::SetSummaryActualUtilize3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter)
{
    c2dGraphData.m_strChartTitle = _T(" Summary Actual Stand Utilization ");
    c2dGraphData.m_strYtitle = _T("Time(second)");
    c2dGraphData.m_strXtitle.Format(_T("Runs"));

    CString strFooter(_T(""));
    strFooter.Format(_T("Summary Stand Operations %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideStandMultiRunOperatinResult::SetSummaryActualIdle3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter)
{
    c2dGraphData.m_strChartTitle = _T(" Summary Actual Stand Idle ");
    c2dGraphData.m_strYtitle = _T("Time(second)");
    c2dGraphData.m_strXtitle.Format(_T("Runs"));

    CString strFooter(_T(""));
    strFooter.Format(_T("Summary Stand Operations %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideStandMultiRunOperatinResult::SetSummaryDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter)
{
    c2dGraphData.m_strChartTitle = _T(" Summary Stand Delays ");
    c2dGraphData.m_strYtitle = _T("Time(second)");
    c2dGraphData.m_strXtitle.Format(_T("Runs"));

    CString strFooter(_T(""));
    strFooter.Format(_T("Summary Stand Operations %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideStandMultiRunOperatinResult::SetSummaryConflict3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter)
{
    c2dGraphData.m_strChartTitle = _T(" Summary Stand Conflict ");
    c2dGraphData.m_strYtitle = _T("Count");
    c2dGraphData.m_strXtitle.Format(_T("Runs"));

    CString strFooter(_T(""));
    strFooter.Format(_T("Summary Stand Operations %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideStandMultiRunOperatinResult::GenerateSummary2DChartData(C2DChartData& c2dGraphData, MultiRunSummaryMap& multiRunSummaryMap)
{
    int nCount = sizeof(strSummaryListTitle) / sizeof(strSummaryListTitle[0]);
    for(int i=0; i<nCount; i++)
        c2dGraphData.m_vrLegend.push_back(strSummaryListTitle[i]);

    c2dGraphData.m_vr2DChartData.resize(13);
    MultiRunSummaryMap::iterator iter = multiRunSummaryMap.begin();
    for(; iter != multiRunSummaryMap.end(); iter++)
    {
        CString strSimName = iter->first;
        int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
        CString strXTickTitle;
        strXTickTitle.Format(_T("Run%d"), nCurSimResult);
        c2dGraphData.m_vrXTickTitle.push_back(strXTickTitle);

        c2dGraphData.m_vr2DChartData[0].push_back((double)iter->second.m_estMin);
        c2dGraphData.m_vr2DChartData[1].push_back((double)iter->second.m_estAverage);
        c2dGraphData.m_vr2DChartData[2].push_back((double)iter->second.m_estMax);
        c2dGraphData.m_vr2DChartData[3].push_back((double)iter->second.m_estQ1);
        c2dGraphData.m_vr2DChartData[4].push_back((double)iter->second.m_estQ2);
        c2dGraphData.m_vr2DChartData[5].push_back((double)iter->second.m_estQ3);
        c2dGraphData.m_vr2DChartData[6].push_back((double)iter->second.m_estP1);
        c2dGraphData.m_vr2DChartData[7].push_back((double)iter->second.m_estP5);
        c2dGraphData.m_vr2DChartData[8].push_back((double)iter->second.m_estP10);
        c2dGraphData.m_vr2DChartData[9].push_back((double)iter->second.m_estP90);
        c2dGraphData.m_vr2DChartData[10].push_back((double)iter->second.m_estP95);
        c2dGraphData.m_vr2DChartData[11].push_back((double)iter->second.m_estP99);
        c2dGraphData.m_vr2DChartData[12].push_back((double)iter->second.m_estSigma);
    }
}

void CAirsideStandMultiRunOperatinResult::FillSummaryConflictListContent(CXListCtrl& cxListCtrl, MultiRunSummaryMap& multiRunSummaryMap)
{
    MultiRunSummaryMap::iterator iter = multiRunSummaryMap.begin();
    int idx = 0;
    for (; iter != multiRunSummaryMap.end(); ++iter)
    {
        CString strIndex;
        strIndex.Format(_T("%d"),idx+1);
        cxListCtrl.InsertItem(idx,strIndex);

        CString strSimName = iter->first;
        int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
        CString strRun = _T("");
        strRun.Format(_T("Run%d"),nCurSimResult+1);
        int nCurCol = 1;
        cxListCtrl.SetItemText(idx, nCurCol, strRun);
        cxListCtrl.SetItemData(idx, idx);
        nCurCol++;

        CString strTemp;
        strTemp.Format("%.2f", (float)iter->second.m_estTotal.getPrecisely()/100);
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", (float)iter->second.m_estMin.getPrecisely()/100);
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", (float)iter->second.m_estAverage.getPrecisely()/100);
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", (float)iter->second.m_estMax.getPrecisely()/100);
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", (float)iter->second.m_estQ1.getPrecisely()/100);
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", (float)iter->second.m_estQ2.getPrecisely()/100);
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", (float)iter->second.m_estQ3.getPrecisely()/100);
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", (float)iter->second.m_estP1.getPrecisely()/100);
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", (float)iter->second.m_estP5.getPrecisely()/100);
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", (float)iter->second.m_estP10.getPrecisely()/100);
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", (float)iter->second.m_estP90.getPrecisely()/100);
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", (float)iter->second.m_estP95.getPrecisely()/100);
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", (float)iter->second.m_estP99.getPrecisely()/100);
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", (float)iter->second.m_estSigma.getPrecisely()/100);
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        idx++;
    }
}
