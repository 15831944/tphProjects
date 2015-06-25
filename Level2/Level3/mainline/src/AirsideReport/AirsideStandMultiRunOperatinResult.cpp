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

CAirsideStandMultiRunOperatinResult::CAirsideStandMultiRunOperatinResult(void)
{
}


CAirsideStandMultiRunOperatinResult::~CAirsideStandMultiRunOperatinResult(void)
{
	ClearData();
}

void CAirsideStandMultiRunOperatinResult::LoadMultipleRunReport( CParameters* pParameter )
{
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
				InitDetailListHead(cxListCtrl,m_standArrConflictsMap,piSHC);
			}
			else
			{
				InitDetailListHead(cxListCtrl,m_standDepConfictsMap,piSHC);
			}
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_IdleTime:
			InitDetailListHead(cxListCtrl,m_standidlemap,piSHC);
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Percentage:
			InitDetailListHead(cxListCtrl,m_standRatiomap,piSHC);
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Occupancy:
			InitDetailListHead(cxListCtrl,m_standOccupMap,piSHC);
			break;
		default:
			break;
		}
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
			SetDetail3DRatioChartString(c2dGraphData,pParameter);
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Percentage:
			SetDetail3DIdleChartString(c2dGraphData,pParameter);
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
			Generate3DChartCountData(m_standRatiomap,chartWnd,pStandPara);
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Occupancy:
			Generate3DChartCountData(m_standOccupMap,chartWnd,pStandPara);
			break;
			break;
		default:
			break;
		}
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
				FilllDetailDelayConflictContent(cxListCtrl,m_standArrDelayMap,0);
			}
			else
			{
				FilllDetailDelayConflictContent(cxListCtrl,m_standDepDelayMap,1);
			}
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Conflict:
			if (iType == 0)
			{
				FilllDetailDelayConflictContent(cxListCtrl,m_standArrConflictsMap,0);
			}
			else
			{
				FilllDetailDelayConflictContent(cxListCtrl,m_standDepConfictsMap,1);
			}
			break;
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_IdleTime:
			FillDetailListTimeContent(cxListCtrl,m_standidlemap);
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Percentage:
			FillDetailListTimeContent(cxListCtrl,m_standRatiomap);
			break;
		case CAirsideFlightStandOperationReport::ChartType_Detail_Occupancy:
			FillDetailListTimeContent(cxListCtrl,m_standOccupMap);
			break;
		default:
			break;
		}
	}
}

void CAirsideStandMultiRunOperatinResult::FillDetailListTimeContent( CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData )
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
			ElapsedTime eTime;
			if(n <  iter->second.size())
				eTime.setPrecisely(delayData.m_iData);

			cxListCtrl.SetItemText(idx, n + 2, eTime.printTime());
		}
		idx++;
	}
}

void CAirsideStandMultiRunOperatinResult::FilllDetailDelayConflictContent( CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData,int iType )
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

		//	wndListCtrl.InsertItem(i, strRun);
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
	mapLoadResult  mapLoadData;
	MapMultiRunStandOperationData::iterator iter = standOperationData.begin();
	long lMaxOccupancyTime = 0;
	for (; iter != standOperationData.end(); ++iter)
	{
		int iCount = iter->second.size();
		for (int i = 0; i < iCount; i++)
		{
			StandMultipleOperationData operationData = iter->second[i];
			mapLoadData[iter->first].push_back(operationData.m_lOccupiedTime);
			if (operationData.m_lOccupiedTime > lMaxOccupancyTime)
			{
				lMaxOccupancyTime = operationData.m_lOccupiedTime;
			}
		}

		std::sort(mapLoadData[iter->first].begin(),mapLoadData[iter->first].end());
	}

	int nIntervalSize = 0;
	nIntervalSize = lMaxOccupancyTime / pParameter->getInterval() ;

	if (nIntervalSize ==0)
		nIntervalSize = 1;

	mapLoadResult::iterator mapIter = mapLoadData.begin();
	for (; mapIter != mapLoadData.end(); ++iter)
	{
		for (int i=0; i< nIntervalSize; i++)
		{
			MultipleRunReportData reportData;
			reportData.m_iStart = pParameter->getInterval()*i;
			reportData.m_iEnd = pParameter->getInterval()*(i+1);

			reportData.m_iData = GetIntervalCount(reportData.m_iStart,reportData.m_iEnd,mapIter->second);

			m_standOccupMap[mapIter->first].push_back(reportData);
		}
	}

}

void CAirsideStandMultiRunOperatinResult::BuildDetailStandOperationIdel( CParameters* pParameter,MapMultiRunStandOperationData standOperationData )
{
	mapLoadResult  mapLoadData;
	MapMultiRunStandOperationData::iterator iter = standOperationData.begin();
	long lMinOccupancyTime = (std::numeric_limits<long>::max)();
	for (; iter != standOperationData.end(); ++iter)
	{
		int iCount = iter->second.size();
		for (int i = 0; i < iCount; i++)
		{
			StandMultipleOperationData operationData = iter->second[i];
			mapLoadData[iter->first].push_back(operationData.m_lOccupiedTime);
			if (operationData.m_lOccupiedTime < lMinOccupancyTime)
			{
				lMinOccupancyTime = operationData.m_lOccupiedTime;
			}
		}

		std::sort(mapLoadData[iter->first].begin(),mapLoadData[iter->first].end());
	}

	int nDuration = pParameter->getEndTime().asSeconds() - pParameter->getStartTime().asSeconds();
	int nIntervalSize = (nDuration - lMinOccupancyTime) / pParameter->getInterval() ;

	if (nIntervalSize ==0)
		nIntervalSize = 1;

	mapLoadResult::iterator mapIter = mapLoadData.begin();
	for (; mapIter != mapLoadData.end(); ++iter)
	{
		for (int i=0; i< nIntervalSize; i++)
		{
			MultipleRunReportData reportData;
			reportData.m_iStart = pParameter->getInterval()*i;
			reportData.m_iEnd = pParameter->getInterval()*(i+1);

			for (size_t j = 0; j < mapIter->second.size(); j++)
			{
				long lTime = nDuration - mapIter->second.at(j);

				if( reportData.m_iStart <= lTime && lTime < reportData.m_iEnd)
					reportData.m_iData++;
			}
			m_standidlemap[mapIter->first].push_back(reportData);
		}
	}
}

void CAirsideStandMultiRunOperatinResult::BuildDetailStandOperationPencentage( CParameters* pParameter,MapMultiRunStandOperationData standOperationData )
{
	mapLoadResult  mapLoadData;
	MapMultiRunStandOperationData::iterator iter = standOperationData.begin();
	long lMaxOccupancyTime = 0;
	for (; iter != standOperationData.end(); ++iter)
	{
		int iCount = iter->second.size();
		for (int i = 0; i < iCount; i++)
		{
			StandMultipleOperationData operationData = iter->second[i];
			mapLoadData[iter->first].push_back(operationData.m_lOccupiedTime);
			if (operationData.m_lOccupiedTime > lMaxOccupancyTime)
			{
				lMaxOccupancyTime = operationData.m_lOccupiedTime;
			}
		}

		std::sort(mapLoadData[iter->first].begin(),mapLoadData[iter->first].end());
	}

	int nIntervalSize = 0;
	int nDuration = pParameter->getEndTime() - pParameter->getStartTime();
	nIntervalSize = int((double)lMaxOccupancyTime/ (double)(nDuration)*100)/10 ;

	mapLoadResult::iterator mapIter = mapLoadData.begin();
	for (; mapIter != mapLoadData.end(); ++iter)
	{
		for (int i=0; i< nIntervalSize; i++)
		{
			MultipleRunReportData reportData;
			reportData.m_iStart = i *10;
			reportData.m_iEnd = (i+1) *10;

			for (size_t j = 0; j < mapIter->second.size(); j++)
			{
				long lTime = nDuration - mapIter->second.at(j);
				int nPercent = static_cast<int>(((double)lTime)/nDuration*100);

				if( reportData.m_iStart <= lTime && lTime < reportData.m_iEnd)
					reportData.m_iData++;
			}
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
	nIntervalSize = lMaxDelayTime / pParameter->getInterval() ;;

	if (nIntervalSize ==0)
		nIntervalSize = 1;

	mapLoadResult::iterator mapIter = mapLoadData.begin();
	for (; mapIter != mapLoadData.end(); ++iter)
	{
		for (int i=0; i< nIntervalSize; i++)
		{
			MultipleRunReportData reportData;
			reportData.m_iStart = pParameter->getInterval()*i;
			reportData.m_iEnd = pParameter->getInterval()*(i+1);;

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
	for (; mapIter != mapLoadData.end(); ++iter)
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

int CAirsideStandMultiRunOperatinResult::GetIntervalCount( long iStart, long iEnd, std::vector<long> vData,long iIgnore /*= 0*/ ) const
{
	int iCount = 0;
	for (unsigned i = 0; i < vData.size(); i++)
	{
		long iData = vData.at(i);
		if (iData < iIgnore)//ignore necessary secs
			continue;

		if (iData >= iStart && iData < iEnd)
		{
			iCount++;
		}
	}
	return iCount;
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
		ElapsedTime estTempTime(lTime);
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
		ElapsedTime estTempTime(lTime);
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
	c2dGraphData.m_strXtitle = _T("Time (hh:mm:ss)");	

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




