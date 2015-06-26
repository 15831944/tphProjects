#include "stdafx.h"
#include "AirsideFlightMutiRunDelayReport.h"
#include "Common/TERMFILE.H"
#include "Common/elaptime.h"
#include "MFCExControl/XListCtrl.h"
#include "Parameters.h"
#include "CARC3DChart.h"
#include "AirsideFlightDelayReport.h"
#include "ParameterCaptionDefine.h"
#include "AirsideFlightDelayParam.h"
#include "Reports/StatisticalTools.h"

const char* strSegmentString[] = {"Unknown","Air","Taxi","Stand","Service","Takeoff"};
const char* strComponentString[] = {"Unknown","Slowed","Vectored","Hold","RunWay Hold","Alt change","Side step","Stop","Service"};
const char* strSummaryContent[] = 
{
    "Min Delay(hh:mm:ss)",
    "Mean Delay(hh:mm:ss)",
    "Max Delay(hh:mm:ss)",
    "Q1(hh:mm:ss)",
    "Q2(hh:mm:ss)",
    "Q3(hh:mm:ss)",
    "P1(hh:mm:ss)",
    "P5(hh:mm:ss)",
    "P10(hh:mm:ss)",
    "P90(hh:mm:ss)",
    "P95(hh:mm:ss)",
    "P99(hh:mm:ss)",
    "Std dev(hh:mm:ss)"
};
CAirsideFlightMutiRunDelayResult::CAirsideFlightMutiRunDelayResult(void)
{
}


CAirsideFlightMutiRunDelayResult::~CAirsideFlightMutiRunDelayResult(void)
{
}

void CAirsideFlightMutiRunDelayResult::LoadMultipleRunReport(CParameters* pParameter)
{
	ClearData();

	mapLoadResult mapTotalDelay;
	mapLoadResult mapScheduleDelay;

	mapDelayResultData mapSegmentDelay;
	mapDelayResultData mapComponentDelay;

    mapSummarySegmentData mapSummarySegmentDelay;
	ArctermFile file;
	DelayResultPath::iterator iter = m_mapResultPath.begin();
	for (; iter != m_mapResultPath.end(); ++iter)
	{
		CString strResultPath = iter->second;
		CString strSimResult = iter->first;

		try
		{
			if (file.openFile(strResultPath.GetString(),READ))
			{
				int iCount = 0; 
				if (file.getInteger(iCount) == 0)
					return;
		
				file.getLine();
				file.getLine();
				for (int i = 0; i < iCount; i++)
				{
					int iSize = 0;
					if(file.getInteger(iSize) == 0)
						return;

					file.getLine();
					for (int j = 0; j < iSize; j++)
					{
						file.skipField(1);
						int iTotalDelay = 0;
						file.getInteger(iTotalDelay);
						mapTotalDelay[strSimResult].push_back(iTotalDelay);

                        long lData = 0l;
                        file.getInteger(lData);
                        mapSummarySegmentDelay[strSimResult][CAirsideFlightDelayReport::FltDelaySegment_Air].push_back(lData);

                        file.getInteger(lData);
                        mapSummarySegmentDelay[strSimResult][CAirsideFlightDelayReport::FltDelaySegment_Taxi].push_back(lData);

                        file.getInteger(lData);
                        mapSummarySegmentDelay[strSimResult][CAirsideFlightDelayReport::FltDelaySegment_Stand].push_back(lData);

                        file.getInteger(lData);
                        mapSummarySegmentDelay[strSimResult][CAirsideFlightDelayReport::FltDelaySegment_Service].push_back(lData);

                        file.getInteger(lData);
                        mapSummarySegmentDelay[strSimResult][CAirsideFlightDelayReport::FltDelaySegment_TakeOff].push_back(lData);

                        //file.skipField(5);

						//long actStartTime = 0;
						//file.getInteger(actStartTime);
						//long actEndTime = 0;
						//file.getInteger(actEndTime);
			
						file.skipField(2);
						long smtaTime = 0;
						file.getInteger(smtaTime);
						long smtdTime = 0;
						file.getInteger(smtdTime);
						long planSt = 0;
						file.getInteger(planSt);

						file.skipField(2);
						int iArrOrDeplDelay = 0;
						file.getInteger(iArrOrDeplDelay);

						int iScheduleDelay = 0;
						if (iArrOrDeplDelay)
						{
							if (smtaTime >= 0)
							{
								iScheduleDelay = max(smtaTime - planSt,0l);
							}
						}
						else
						{
							if (smtdTime >= 0)
							{
								iScheduleDelay = max(smtdTime - planSt,0l);
							}
							
						}
						mapScheduleDelay[strSimResult].push_back(iScheduleDelay);

						file.getLine();
						file.getLine();
						file.getLine();
						file.getLine();
						file.getLine();
						
						int nNodeCount = 0;
						file.getInteger(nNodeCount);
						file.getLine();
						//node delay start, will implement
						if(mapComponentDelay[strSimResult].empty())
						{
							mapComponentDelay[strSimResult][FltDelayReason_Slowed].clear();
							mapComponentDelay[strSimResult][FltDelayReason_Vectored].clear();
							mapComponentDelay[strSimResult][FltDelayReason_AirHold].clear();
							mapComponentDelay[strSimResult][FltDelayReason_AltitudeChanged].clear();
							mapComponentDelay[strSimResult][FltDelayReason_SideStep].clear();
							mapComponentDelay[strSimResult][FltDelayReason_Stop].clear();
							mapComponentDelay[strSimResult][FltDelayReason_Service].clear();
						}

						if (mapSegmentDelay[strSimResult].empty())
						{
							mapSegmentDelay[strSimResult][CAirsideFlightDelayReport::FltDelaySegment_Air].clear();
							mapSegmentDelay[strSimResult][CAirsideFlightDelayReport::FltDelaySegment_Taxi].clear();
							mapSegmentDelay[strSimResult][CAirsideFlightDelayReport::FltDelaySegment_Stand].clear();
							mapSegmentDelay[strSimResult][CAirsideFlightDelayReport::FltDelaySegment_Service].clear();
							mapSegmentDelay[strSimResult][CAirsideFlightDelayReport::FltDelaySegment_TakeOff].clear();
						}
						for (int iSkip = 0; iSkip < nNodeCount; iSkip++)
						{
							long lDelay = 0;
							file.getInteger(lDelay);
							file.skipField(1);
							int iSegment = 0;
							file.getInteger(iSegment);

							long lArrTime = 0;
							file.getInteger(lArrTime);

							long lActArrTime = 0;
							file.getInteger(lActArrTime);
							FlightDelayData delaySegmentData;
							delaySegmentData.m_iArrTime = lActArrTime;
							delaySegmentData.m_lDelayTime = lDelay;
							if (mapSegmentDelay[strSimResult].find(iSegment) != mapSegmentDelay[strSimResult].end())
							{
								mapSegmentDelay[strSimResult][iSegment].push_back(delaySegmentData);
							}
							
							int iReason = 0;
							file.getInteger(iReason);
							FlightDelayData delayComponentData;
							delayComponentData.m_iArrTime = lArrTime;
							delayComponentData.m_lDelayTime = lDelay;
							if (mapComponentDelay[strSimResult].find(iReason) != mapComponentDelay[strSimResult].end())
							{
								mapComponentDelay[strSimResult][iReason].push_back(delayComponentData);
							}
							file.getLine();
						}
					}
				}
				file.closeIn();
			}
		}
		catch (...)
		{
			ClearData();
		}
	}

	//generate detail of total delay
	BulidDetailMultiRunDelayCount(m_totalDelayData,mapTotalDelay,pParameter->getInterval());
	//generate detail of air delay
	BulidDetailMultiRunDelayTime(m_airDelayData,CAirsideFlightDelayReport::FltDelaySegment_Air,mapSegmentDelay,pParameter);
	//generate detail of stand delay
	BulidDetailMultiRunDelayTime(m_standDelayData,CAirsideFlightDelayReport::FltDelaySegment_Stand,mapSegmentDelay,pParameter);
	//generate detail of  taxi delay
	BulidDetailMultiRunDelayTime(m_taxiDelayData,CAirsideFlightDelayReport::FltDelaySegment_Taxi,mapSegmentDelay,pParameter);
	//generate detail of service delay
	BulidDetailMultiRunDelayTime(m_serviceDelayData,CAirsideFlightDelayReport::FltDelaySegment_Service,mapSegmentDelay,pParameter);
	//generate detail of takeoff delay
	BulidDetailMultiRunDelayTime(m_takeoffDelayData,CAirsideFlightDelayReport::FltDelaySegment_TakeOff,mapSegmentDelay,pParameter);
	//generate detail of schedule delay
	BulidDetailMultiRunDelayCount(m_scheduleDelayData,mapScheduleDelay,pParameter->getInterval(),100);

	//generate detail of segment delay
	BuildDetailComponentSegmentData(m_segmentDelayData,mapSegmentDelay,pParameter);
	//generate detail of component delay
	BuildDetailComponentSegmentData(m_componentDelayData,mapComponentDelay,pParameter);

// build summary data begin
    //generate summary of total delay
    BuildSummaryTotalDelayData(m_totalSummaryDelayData, mapTotalDelay);
    //generate summary of air delay
    BuildSummarySegmentData(m_airSummaryDelayData,CAirsideFlightDelayReport::FltDelaySegment_Air, mapSummarySegmentDelay,pParameter);
    //generate summary of stand delay
    BuildSummarySegmentData(m_standSummaryDelayData,CAirsideFlightDelayReport::FltDelaySegment_Stand,mapSummarySegmentDelay,pParameter);
    //generate summary of  taxi delay
    BuildSummarySegmentData(m_taxiSummaryDelayData,CAirsideFlightDelayReport::FltDelaySegment_Taxi,mapSummarySegmentDelay,pParameter);
    //generate summary of service delay
    BuildSummarySegmentData(m_serviceSummaryDelayData,CAirsideFlightDelayReport::FltDelaySegment_Service,mapSummarySegmentDelay,pParameter);
    //generate summary of takeoff delay
    BuildSummarySegmentData(m_takeoffSummaryDelayData,CAirsideFlightDelayReport::FltDelaySegment_TakeOff,mapSummarySegmentDelay,pParameter);
    //generate summary of schedule delay
    BuildSummaryTotalDelayData(m_scheduleSummaryDelayData,mapScheduleDelay);
    //generate summary of segment delay
    BuildSummarySegmentData2(m_segmentSummaryDelayData, mapSummarySegmentDelay);
    //generate summary of component delay 
    BuildSummaryComponentSegmentData(m_componentSummaryDelayData, mapComponentDelay);
// build summary data end
}

void CAirsideFlightMutiRunDelayResult::ClearData()
{
	m_totalDelayData.clear();
	m_airDelayData.clear();
	m_taxiDelayData.clear();
	m_standDelayData.clear();
	m_serviceDelayData.clear();
	m_takeoffDelayData.clear();
	m_segmentDelayData.clear();
	m_componentDelayData.clear();
	m_scheduleDelayData.clear();
}

void CAirsideFlightMutiRunDelayResult::BuildDetailComponentSegmentData(DelayComponentAndSegmentMap& mapDetailData,mapDelayResultData& componentSegmentMapData,CParameters* pParameter)
{
	ElapsedTime estMinDelayTime = pParameter->getStartTime();
	ElapsedTime estMaxDelayTime = pParameter->getEndTime();

	long lMinDelayTime = estMinDelayTime.asSeconds();
	long lMaxDelayTime = estMaxDelayTime.asSeconds();

	if (lMinDelayTime > lMaxDelayTime)
		return;

	long iInterval = pParameter->getInterval();
	ElapsedTime estUserIntervalTime = ElapsedTime(iInterval);

	long lDelayTimeSegmentCount = 0;             //the count of the delayTime segment
	if (0 < pParameter->getInterval())
	{
		lDelayTimeSegmentCount = (lMaxDelayTime - lMinDelayTime) / (iInterval);
		estMinDelayTime = ElapsedTime((lMinDelayTime - lMinDelayTime%(iInterval)) /100);

//		lDelayTimeSegmentCount++;
	}
	else
	{
		lDelayTimeSegmentCount= ClacTimeRange(estMaxDelayTime, estMinDelayTime, estUserIntervalTime);
	}

	std::vector<long> vTypeData;
	mapDelayResultData::iterator iter = componentSegmentMapData.begin();
	for (; iter != componentSegmentMapData.end(); ++iter)
	{
		mapDelayData::iterator mapIter = iter->second.begin();
		for (; mapIter != iter->second.end(); ++mapIter)
		{
			if (std::find(vTypeData.begin(),vTypeData.end(),mapIter->first) == vTypeData.end())
			{
				vTypeData.push_back(mapIter->first);
			}
			
			for (long i = 0; i < lDelayTimeSegmentCount; i++)
			{
				ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*i);
				ElapsedTime estTempMaxDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*(i + 1));

				MultipleRunReportData delayData;
				delayData.m_iStart = estTempMinDelayTime.getPrecisely();
				delayData.m_iEnd = estTempMaxDelayTime.getPrecisely();

				long iDelayTime = GetIntervalDelayTime(delayData.m_iStart,delayData.m_iEnd,mapIter->second);
				delayData.m_iData = iDelayTime;
				
				mapDetailData[iter->first][mapIter->first].push_back(delayData);
			}
		}
	}
}

void CAirsideFlightMutiRunDelayResult::BulidDetailMultiRunDelayCount( MultiRunDetailMap& mapDetailData,mapLoadResult mapData,long iInterval,long iIgnore/*=0*/  )
{
	long lMinDelayTime = GetMapMinValue(mapData);
	long lMaxDelayTime = GetMapMaxValue(mapData);

	if (lMinDelayTime > lMaxDelayTime)
		return;

	ElapsedTime estMinDelayTime = ElapsedTime(long(lMinDelayTime/100.0+0.5));
	ElapsedTime estMaxDelayTime = ElapsedTime(long(lMaxDelayTime/100.0+0.5));
	ElapsedTime estUserIntervalTime = ElapsedTime(iInterval);

	long lDelayTimeSegmentCount = 0;             //the count of the delayTime segment
	if (0 < iInterval)
	{
		lDelayTimeSegmentCount = (lMaxDelayTime - lMinDelayTime) / (iInterval * 100);
		estMinDelayTime = ElapsedTime((lMinDelayTime - lMinDelayTime%(iInterval*100)) /100);

		lDelayTimeSegmentCount++;
	}
	else
	{
		lDelayTimeSegmentCount= ClacTimeRange(estMaxDelayTime, estMinDelayTime, estUserIntervalTime);
	}

	mapLoadResult::iterator iter = mapData.begin();
	for (; iter != mapData.end(); ++iter)
	{
		for (long i = 0; i < lDelayTimeSegmentCount; i++)
		{
			ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*i);
			ElapsedTime estTempMaxDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*(i + 1));

			MultipleRunReportData delayData;
			delayData.m_iStart = estTempMinDelayTime.getPrecisely();
			delayData.m_iEnd = estTempMaxDelayTime.getPrecisely();

			long iCount = GetIntervalCount(delayData.m_iStart,delayData.m_iEnd,iter->second,iIgnore);
			delayData.m_iData = iCount;
			mapDetailData[iter->first].push_back(delayData);
		}
	}
}

void CAirsideFlightMutiRunDelayResult::BulidDetailMultiRunDelayTime( MultiRunDetailMap& mapDetailData,int iType,mapDelayResultData mapData,CParameters* pParameter )
{
	ElapsedTime estMinDelayTime = pParameter->getStartTime();
	ElapsedTime estMaxDelayTime = pParameter->getEndTime();

	long lMinDelayTime = estMinDelayTime.asSeconds();
	long lMaxDelayTime = estMaxDelayTime.asSeconds();

	if (lMinDelayTime > lMaxDelayTime)
		return;

	long iInterval = pParameter->getInterval();
	ElapsedTime estUserIntervalTime = ElapsedTime(iInterval);

	long lDelayTimeSegmentCount = 0;             //the count of the delayTime segment
	if (0 < iInterval)
	{
		lDelayTimeSegmentCount = (lMaxDelayTime - lMinDelayTime) / (iInterval);
		estMinDelayTime = ElapsedTime((lMinDelayTime - lMinDelayTime%(iInterval)) /100);

	//	lDelayTimeSegmentCount++;
	}
	else
	{
		lDelayTimeSegmentCount= ClacTimeRange(estMaxDelayTime, estMinDelayTime, estUserIntervalTime);
	}

	mapDelayResultData::iterator iter = mapData.begin();
	for (; iter != mapData.end(); ++iter)
	{
		for (long i = 0; i < lDelayTimeSegmentCount; i++)
		{
			ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*i);
			ElapsedTime estTempMaxDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*(i + 1));

			MultipleRunReportData delayData;
			delayData.m_iStart = estTempMinDelayTime.getPrecisely();
			delayData.m_iEnd = estTempMaxDelayTime.getPrecisely();

			long iDelayTime = GetIntervalDelayTime(delayData.m_iStart,delayData.m_iEnd,iter->second[iType]);
			delayData.m_iData = iDelayTime;
			mapDetailData[iter->first].push_back(delayData);
		}
	}
}


long CAirsideFlightMutiRunDelayResult::GetIntervalDelayTime( long iStart,long iEnd,std::vector<FlightDelayData> vData) const
{
	long iDelayTime = 0;
	for (unsigned i = 0; i < vData.size(); i++)
	{
		FlightDelayData delayData = vData.at(i);
		if (delayData.m_iArrTime >= iStart && delayData.m_iArrTime < iEnd)
		{
			iDelayTime += delayData.m_lDelayTime;
		}
	}
	return iDelayTime;
}

void CAirsideFlightMutiRunDelayResult::InitDetailListHead( CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData,CSortableHeaderCtrl* piSHC/*=NULL*/ )
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
		strRange.Format(_T("%s - %s"),eStartTime.printTime(), eEndTime.printTime());

		DWORD dwStyle = LVCFMT_LEFT;
		dwStyle &= ~HDF_OWNERDRAW;

		cxListCtrl.InsertColumn(2+i, strRange, /*LVCFMT_LEFT*/dwStyle, 100);
	}
}

void CAirsideFlightMutiRunDelayResult::InitSummaryListHead(CXListCtrl& cxListCtrl, CParameters* pParameter, CSortableHeaderCtrl* piSHC)
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

    CAirsideFlightDelayParam* pDelayPara = (CAirsideFlightDelayParam*)pParameter;
    if(pDelayPara->getSubType() == CAirsideFlightDelayReport::SRT_SUMMARY_FLIGHTTOTALDELAY ||
        pDelayPara->getSubType() == CAirsideFlightDelayReport::SRT_SUMMARY_SEGMENTDELAY ||
        pDelayPara->getSubType() == CAirsideFlightDelayReport::SRT_SUMMARY_COMPONENTDELAY)
    {
        cxListCtrl.InsertColumn(nCurCol, _T("Total Delay(hh:mm:ss)"), headStyle, 80);
        nCurCol++;
    }

    int nCount = sizeof(strSummaryContent)/sizeof(strSummaryContent[0]);
    for(int i=0; i<nCount; i++)
    {
        cxListCtrl.InsertColumn(nCurCol, strSummaryContent[i], LVCFMT_LEFT, 100);
        if(piSHC)
        {
            piSHC->SetDataType(nCurCol,dtTIME);
        }
        nCurCol++;
    }
}

void CAirsideFlightMutiRunDelayResult::InitDetailComponentSegmentListHead(CXListCtrl& cxListCtrl,DelayComponentAndSegmentMap mapDetailData,int iType/*=0*/, CSortableHeaderCtrl* piSHC/*=NULL*/)
{
	if (mapDetailData.empty())
		return;
	DelayComponentAndSegmentMap::iterator iter = mapDetailData.begin();
	size_t rangeCount = iter->second[iType].size();
	for (size_t i = 0; i < rangeCount; ++i)
	{
		MultipleRunReportData delayData = iter->second[iType].at(i);
		CString strRange;
		ElapsedTime eStartTime;
		ElapsedTime eEndTime;
		eStartTime.setPrecisely(delayData.m_iStart);
		eEndTime.setPrecisely(delayData.m_iEnd);
		strRange.Format(_T("%s - %s"),eStartTime.printTime(), eEndTime.printTime());

		DWORD dwStyle = LVCFMT_LEFT;
		dwStyle &= ~HDF_OWNERDRAW;

		cxListCtrl.InsertColumn(2+i, strRange, /*LVCFMT_LEFT*/dwStyle, 100);
	}
}

void CAirsideFlightMutiRunDelayResult::InitListHead( CXListCtrl& cxListCtrl, CParameters * parameter, int iType/*=0*/,CSortableHeaderCtrl* piSHC/*=NULL*/ )
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

		CAirsideFlightDelayParam* pDelayPara = (CAirsideFlightDelayParam*)parameter;
		switch (pDelayPara->getSubType())
		{
		case CAirsideFlightDelayReport::SRT_DETAIL_FLIGHTTOTALDELAY:
			InitDetailListHead(cxListCtrl,m_totalDelayData,piSHC);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_AIRDELAY:
			InitDetailListHead(cxListCtrl,m_airDelayData,piSHC);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_TAXIDELAY:
			InitDetailListHead(cxListCtrl,m_taxiDelayData,piSHC);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_TAKOFFDELAY:
			InitDetailListHead(cxListCtrl,m_takeoffDelayData,piSHC);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_STANDDELAY:
			InitDetailListHead(cxListCtrl,m_standDelayData,piSHC);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_SCHEDULEDELAY:
			InitDetailListHead(cxListCtrl,m_scheduleDelayData,piSHC);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_SEGMENTDELAY:
			InitDetailComponentSegmentListHead(cxListCtrl,m_segmentDelayData,iType,piSHC);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_COMPONENTDELAY:
			InitDetailComponentSegmentListHead(cxListCtrl,m_componentDelayData,iType,piSHC);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_SERVICEDELAY:
			InitDetailListHead(cxListCtrl,m_serviceDelayData,piSHC);
			break;
		default:
			break;
		}
	}
    else if(parameter->getReportType() == ASReportType_Summary)
    {
        InitSummaryListHead(cxListCtrl, parameter, piSHC);
    }
}
void CAirsideFlightMutiRunDelayResult::FillDetailListCountContent( CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData )
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

void CAirsideFlightMutiRunDelayResult::FillSummaryComponentSegmentContent(CXListCtrl& cxListCtrl,SummaryCompomentAndSegmentResultMap mapSummaryData,int iType)
{
    SummaryCompomentAndSegmentResultMap::iterator iter = mapSummaryData.begin();

    int idx = 0;
    for (; iter != mapSummaryData.end(); ++iter)
    {
        CString strIndex;
        strIndex.Format(_T("%d"),idx+1);
        cxListCtrl.InsertItem(idx,strIndex);

        CString strSimName = iter->first;
        int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
        CString strRun = _T("");
        strRun.Format(_T("Run%d"),nCurSimResult+1);

        cxListCtrl.SetItemText(idx, 1, strRun);
        cxListCtrl.SetItemData(idx, idx);

        int nCurCol = 2;
        CString strTemp = iter->second[iType].m_estTotal.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second[iType].m_estMin.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second[iType].m_estAverage.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second[iType].m_estMax.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second[iType].m_estQ1.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second[iType].m_estQ2.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second[iType].m_estQ3.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second[iType].m_estP1.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second[iType].m_estP5.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second[iType].m_estP10.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second[iType].m_estP90.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second[iType].m_estP95.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second[iType].m_estP99.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp = iter->second[iType].m_estSigma.printTime();
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        idx++;
    }
}

void CAirsideFlightMutiRunDelayResult::FillSummaryListContent(CXListCtrl& cxListCtrl, CParameters* pParameter, MultiRunSummaryMap& mapSummaryData)
{
    MultiRunSummaryMap::iterator iter = mapSummaryData.begin();

    int idx = 0;
    for (; iter != mapSummaryData.end(); ++iter)
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
        CAirsideFlightDelayParam* pDelayPara = (CAirsideFlightDelayParam*)pParameter;
        if(pDelayPara->getSubType() == CAirsideFlightDelayReport::SRT_SUMMARY_FLIGHTTOTALDELAY)
        {
            strTemp = iter->second.m_estTotal.printTime();
            cxListCtrl.SetItemText(idx, nCurCol, strTemp);
            nCurCol++;
        }

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

void CAirsideFlightMutiRunDelayResult::FilllDetailComponentSegmentContent(CXListCtrl& cxListCtrl,DelayComponentAndSegmentMap mapDetailData,int iType)
{
	DelayComponentAndSegmentMap::iterator iter = mapDetailData.begin();
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
		for (size_t n = 0; n < iter->second[iType].size(); ++n)
		{
			MultipleRunReportData delayData = iter->second[iType].at(n);
			ElapsedTime eTime;
			if(n <  iter->second[iType].size())
				eTime.setPrecisely(delayData.m_iData);

			cxListCtrl.SetItemText(idx, n + 2, eTime.printTime());
		}
		idx++;
	}
}

void CAirsideFlightMutiRunDelayResult::FillDetailListTimeContent( CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData )
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
			ElapsedTime eTime;
			if(n <  iter->second.size())
				eTime.setPrecisely(delayData.m_iData);

			cxListCtrl.SetItemText(idx, n + 2, eTime.printTime());
		}
		idx++;
	}
}

void CAirsideFlightMutiRunDelayResult::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter,int iType/*=0*/ )
{
	ASSERT(parameter);
	if (parameter->getReportType() == ASReportType_Detail)
	{
		CAirsideFlightDelayParam* pDelayPara = (CAirsideFlightDelayParam*)parameter;
		switch (pDelayPara->getSubType())
		{
		case CAirsideFlightDelayReport::SRT_DETAIL_FLIGHTTOTALDELAY:
			FillDetailListCountContent(cxListCtrl,m_totalDelayData);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_AIRDELAY:
			FillDetailListTimeContent(cxListCtrl,m_airDelayData);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_TAXIDELAY:
			FillDetailListTimeContent(cxListCtrl,m_taxiDelayData);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_TAKOFFDELAY:
			FillDetailListTimeContent(cxListCtrl,m_takeoffDelayData);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_STANDDELAY:
			FillDetailListTimeContent(cxListCtrl,m_standDelayData);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_SCHEDULEDELAY:
			FillDetailListCountContent(cxListCtrl,m_scheduleDelayData);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_SEGMENTDELAY:
			FilllDetailComponentSegmentContent(cxListCtrl,m_segmentDelayData,iType);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_COMPONENTDELAY:
			FilllDetailComponentSegmentContent(cxListCtrl,m_componentDelayData,iType);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_SERVICEDELAY:
			FillDetailListTimeContent(cxListCtrl,m_serviceDelayData);
			break;
		default:
			break;
		}
	}
    else if(parameter->getReportType() == ASReportType_Summary)
    {
        CAirsideFlightDelayParam* pDelayPara = (CAirsideFlightDelayParam*)parameter;
        switch (pDelayPara->getSubType())
        {
        case CAirsideFlightDelayReport::SRT_SUMMARY_FLIGHTTOTALDELAY:
            FillSummaryListContent(cxListCtrl, parameter, m_totalSummaryDelayData);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_AIRDELAY:
            FillSummaryListContent(cxListCtrl, parameter, m_airSummaryDelayData);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_TAXIDELAY:
            FillSummaryListContent(cxListCtrl, parameter, m_taxiSummaryDelayData);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_TAKOFFDELAY:
            FillSummaryListContent(cxListCtrl, parameter, m_takeoffSummaryDelayData);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_STANDDELAY:
            FillSummaryListContent(cxListCtrl, parameter, m_standSummaryDelayData);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_SCHEDULEDELAY:
        case CAirsideFlightDelayReport::SRT_DETAIL_SCHEDULEDELAY:// default choose 
            FillSummaryListContent(cxListCtrl, parameter, m_scheduleSummaryDelayData);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_SEGMENTDELAY:
            FillSummaryComponentSegmentContent(cxListCtrl,m_segmentSummaryDelayData, iType);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_COMPONENTDELAY:
            FillSummaryComponentSegmentContent(cxListCtrl,m_componentSummaryDelayData, iType);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_SERVICEDELAY:
            FillSummaryListContent(cxListCtrl, parameter, m_serviceSummaryDelayData);
            break;
        default:
            break;
        }
    }
}

void CAirsideFlightMutiRunDelayResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter,int iType/*=0*/ )
{
	if (pParameter->getReportType() == ASReportType_Detail)
	{
		CAirsideFlightDelayParam* pDelayPara = (CAirsideFlightDelayParam*)pParameter;
		switch (pDelayPara->getSubType())
		{
		case CAirsideFlightDelayReport::SRT_DETAIL_FLIGHTTOTALDELAY:
			Generate3DChartCountData(m_totalDelayData,chartWnd,pDelayPara);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_AIRDELAY:
			Generate3DChartTimeData(m_airDelayData,chartWnd,pDelayPara);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_TAXIDELAY:
			Generate3DChartTimeData(m_taxiDelayData,chartWnd,pDelayPara);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_TAKOFFDELAY:
			Generate3DChartTimeData(m_takeoffDelayData,chartWnd,pDelayPara);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_STANDDELAY:
			Generate3DChartTimeData(m_standDelayData,chartWnd,pDelayPara);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_SCHEDULEDELAY:
			Generate3DChartCountData(m_scheduleDelayData,chartWnd,pDelayPara);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_SEGMENTDELAY:
			GenerateComponentSegmentTimeData(m_segmentDelayData,iType,chartWnd,pDelayPara);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_COMPONENTDELAY:
			GenerateComponentSegmentTimeData(m_componentDelayData,iType,chartWnd,pDelayPara);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_SERVICEDELAY:
			Generate3DChartTimeData(m_serviceDelayData,chartWnd,pDelayPara);
			break;
		default:
			break;
		}
	}
    else if (pParameter->getReportType() == ASReportType_Summary)
    {
        CAirsideFlightDelayParam* pDelayPara = (CAirsideFlightDelayParam*)pParameter;
        switch (pDelayPara->getSubType())
        {
        case CAirsideFlightDelayReport::SRT_SUMMARY_FLIGHTTOTALDELAY:
            GenerateSummary3DChartTimeData(m_totalSummaryDelayData, chartWnd, pDelayPara);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_AIRDELAY:
            GenerateSummary3DChartTimeData(m_airSummaryDelayData, chartWnd, pDelayPara);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_TAXIDELAY:
            GenerateSummary3DChartTimeData(m_taxiSummaryDelayData, chartWnd, pDelayPara);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_TAKOFFDELAY:
            GenerateSummary3DChartTimeData(m_takeoffSummaryDelayData, chartWnd, pDelayPara);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_STANDDELAY:
            GenerateSummary3DChartTimeData(m_standSummaryDelayData, chartWnd, pDelayPara);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_SCHEDULEDELAY:
            GenerateSummary3DChartTimeData(m_scheduleSummaryDelayData, chartWnd, pDelayPara);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_SEGMENTDELAY:
            GenerateSummaryComponentSegmentTimeData(m_segmentSummaryDelayData, iType, chartWnd, pDelayPara);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_COMPONENTDELAY:
            GenerateSummaryComponentSegmentTimeData(m_componentSummaryDelayData, iType, chartWnd, pDelayPara);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_SERVICEDELAY:
            GenerateSummary3DChartTimeData(m_serviceSummaryDelayData, chartWnd, pDelayPara);
            break;
        default:
            break;
        }
    }
}

void CAirsideFlightMutiRunDelayResult::GenerateComponentSegmentTimeData(DelayComponentAndSegmentMap mapDetailData,int iType,CARC3DChart& chartWnd, CParameters *pParameter)
{
	C2DChartData c2dGraphData;

	SetDetail3DChartString(c2dGraphData,pParameter,iType);

	if (mapDetailData.empty() == true)
		return;

	DelayComponentAndSegmentMap::iterator iter = mapDetailData.begin();
	for (unsigned iTitle = 0; iTitle < iter->second[iType].size(); iTitle++)
	{
		MultipleRunReportData delayData = iter->second[iType].at(iTitle);
		ElapsedTime eStartTime;
		ElapsedTime eEndTime;
		eStartTime.setPrecisely(delayData.m_iStart);
		eEndTime.setPrecisely(delayData.m_iEnd);

		CString strTimeRange;
		strTimeRange.Format(_T("%02d:%02d-%02d:%02d"), eStartTime.asHours(), eStartTime.GetMinute(), eEndTime.asHours(), eEndTime.GetMinute());
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
		for (unsigned i = 0; i < iter->second[iType].size(); i++)
		{
			MultipleRunReportData delayData = iter->second[iType].at(i);
			ElapsedTime tTime;
			tTime.setPrecisely(delayData.m_iData);
			vData.push_back(tTime.asSeconds()/60.0);
		}
		c2dGraphData.m_vr2DChartData.push_back(vData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CAirsideFlightMutiRunDelayResult::Generate3DChartCountData( MultiRunDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter )
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

		CString strTimeRange;
		strTimeRange.Format(_T("%02d:%02d-%02d:%02d"), eStartTime.asHours(), eStartTime.GetMinute(), eEndTime.asHours(), eEndTime.GetMinute());
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

void CAirsideFlightMutiRunDelayResult::Generate3DChartTimeData( MultiRunDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter )
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

		CString strTimeRange;
		strTimeRange.Format(_T("%02d:%02d-%02d:%02d"), eStartTime.asHours(), eStartTime.GetMinute(), eEndTime.asHours(), eEndTime.GetMinute());
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
			ElapsedTime tTime;
			tTime.setPrecisely(delayData.m_iData);
			vData.push_back(tTime.asSeconds()/60.0);
		}
		c2dGraphData.m_vr2DChartData.push_back(vData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CAirsideFlightMutiRunDelayResult::SetDetailTotalDelay3DChartString(  C2DChartData& c2dGraphData, CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T(" Number of flights vs. total delay ");
	c2dGraphData.m_strYtitle = _T("Number of flights");
	c2dGraphData.m_strXtitle = _T("Delay (hh:mm )");	

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("TOTAL DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;
}

void CAirsideFlightMutiRunDelayResult::SetDetailAirDelay3DChartString(  C2DChartData& c2dGraphData, CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T(" Air delay vs. time of day ");
	c2dGraphData.m_strYtitle = _T("Delays(mins)");
	c2dGraphData.m_strXtitle.Format(_T("Time of day"));	

	CString strFooter(_T(""));
	strFooter.Format(_T("AIR DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;
}

void CAirsideFlightMutiRunDelayResult::SetDetailTaxiDelay3DChartString(  C2DChartData& c2dGraphData, CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T(" Taxi delay vs. time of day ");
	c2dGraphData.m_strYtitle = _T("Delays(mins)");
	c2dGraphData.m_strXtitle.Format(_T("Time of day"));	

	CString strFooter(_T(""));
	strFooter.Format(_T("TAXI DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;
}

void CAirsideFlightMutiRunDelayResult::SetDetailStandDelay3DChartString(  C2DChartData& c2dGraphData, CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T(" Stand delay vs. time of day ");
	c2dGraphData.m_strYtitle = _T("Delays(mins)");
	c2dGraphData.m_strXtitle.Format(_T("Time of day"));

	CString strFooter(_T(""));
	strFooter.Format(_T("STAND DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;
}

void CAirsideFlightMutiRunDelayResult::SetDetailTakeoffDelay3DChartString(  C2DChartData& c2dGraphData, CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T(" Take off delay vs. time of day ");
	c2dGraphData.m_strYtitle = _T("Delays(mins)");
	c2dGraphData.m_strXtitle.Format(_T("Time of day"));	

	CString strFooter(_T(""));
	strFooter.Format(_T("TAKEOFF DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;
}

void CAirsideFlightMutiRunDelayResult::SetDetail3DChartString( C2DChartData& c2dGraphData,CParameters *pParameter,int iType/*=0*/ )
{
	ASSERT(pParameter);
	if (pParameter->getReportType() == ASReportType_Detail)
	{
		CAirsideFlightDelayParam* pDelayPara = (CAirsideFlightDelayParam*)pParameter;
		switch (pDelayPara->getSubType())
		{
		case CAirsideFlightDelayReport::SRT_DETAIL_FLIGHTTOTALDELAY:
			SetDetailTotalDelay3DChartString(c2dGraphData,pParameter);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_AIRDELAY:
			SetDetailAirDelay3DChartString(c2dGraphData,pParameter);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_TAXIDELAY:
			SetDetailTaxiDelay3DChartString(c2dGraphData,pParameter);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_TAKOFFDELAY:
			SetDetailTakeoffDelay3DChartString(c2dGraphData,pParameter);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_STANDDELAY:
			SetDetailStandDelay3DChartString(c2dGraphData,pParameter);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_SCHEDULEDELAY:
			SetDetailScheduleDelay3DChartString(c2dGraphData,pParameter);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_SEGMENTDELAY:
			SetDetailSegmentDelay3DChartString(c2dGraphData,pParameter,iType);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_COMPONENTDELAY:
			SetDetailComponentDelay3DChartString(c2dGraphData,pParameter,iType);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_SERVICEDELAY:
			SetDetailServiceDelay3DChartString(c2dGraphData,pParameter);
			break;
		default:
			break;
		}
	}
}

void CAirsideFlightMutiRunDelayResult::SetDetailServiceDelay3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T(" GSE Induced Delay vs. time of day ");
	c2dGraphData.m_strYtitle = _T("Delays(mins)");
	c2dGraphData.m_strXtitle.Format(_T("Time of day"));	

	CString strFooter(_T(""));
	strFooter.Format(_T("GSE INDUCED DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;
}

void CAirsideFlightMutiRunDelayResult::SetDetailSegmentDelay3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter,int iType )
{
	CString strChartTitle;
	strChartTitle.Format(_T("Segment Delay %s vs. time of day"),strSegmentString[iType]);
	c2dGraphData.m_strChartTitle = strChartTitle;
	c2dGraphData.m_strYtitle = _T("Delays (mins)");
	c2dGraphData.m_strXtitle.Format(_T("Time of day"));

	CString strFooter(_T(""));
	strFooter.Format(_T("SEGMENT DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;
}

void CAirsideFlightMutiRunDelayResult::SetDetailScheduleDelay3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T(" Number of flights vs. schedule delay ");
	c2dGraphData.m_strYtitle = _T("Number of flights");
	c2dGraphData.m_strXtitle = _T("Delay (hh:mm )");	

	CString strFooter(_T(""));
	strFooter.Format(_T("SCHEDULE DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;
}

void CAirsideFlightMutiRunDelayResult::SetDetailComponentDelay3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter,int iType  )
{
	CString strChartTitle;
	strChartTitle.Format(_T("Component Delay %s vs. time of day"),strComponentString[iType]);
	c2dGraphData.m_strChartTitle = strChartTitle;
	c2dGraphData.m_strYtitle = _T("Delays (mins)");
	c2dGraphData.m_strXtitle.Format(_T("Time of day"));

	CString strFooter(_T(""));
	strFooter.Format(_T("COMPONENT DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;
}


void CAirsideFlightMutiRunDelayResult::BuildSummaryTotalDelayData(MultiRunSummaryMap& mapSummaryData, mapLoadResult mapTotalDelay)
{
    mapLoadResult::iterator iter = mapTotalDelay.begin();
    for (; iter != mapTotalDelay.end(); ++iter)
    {
        CStatisticalTools<double> tempTools;
        for (size_t n = 0; n < iter->second.size(); ++n)
        {
            tempTools.AddNewData((double)iter->second[n]);
        }
        tempTools.SortData();

        mapSummaryData[iter->first].m_estTotal = (long)(tempTools.GetSum()/100.0+0.5);
        mapSummaryData[iter->first].m_estMin = (long)(tempTools.GetMin()/100.0+0.5);
        mapSummaryData[iter->first].m_estAverage = (long)(tempTools.GetAvarage()/100.0+0.5);
        mapSummaryData[iter->first].m_estMax = (long)(tempTools.GetMax()/100.0+0.5);
        mapSummaryData[iter->first].m_estQ1 = (long)(tempTools.GetPercentile(25)/100.0+0.5);
        mapSummaryData[iter->first].m_estQ2 = (long)(tempTools.GetPercentile(50)/100.0+0.5);
        mapSummaryData[iter->first].m_estQ3 = (long)(tempTools.GetPercentile(75)/100.0+0.5);
        mapSummaryData[iter->first].m_estP1 = (long)(tempTools.GetPercentile(1)/100.0+0.5);
        mapSummaryData[iter->first].m_estP5 = (long)(tempTools.GetPercentile(5)/100.0+0.5);
        mapSummaryData[iter->first].m_estP10 = (long)(tempTools.GetPercentile(10)/100.0+0.5);
        mapSummaryData[iter->first].m_estP90 = (long)(tempTools.GetPercentile(90)/100.0+0.5);
        mapSummaryData[iter->first].m_estP95 = (long)(tempTools.GetPercentile(95)/100.0+0.5);
        mapSummaryData[iter->first].m_estP99 = (long)(tempTools.GetPercentile(99)/100.0+0.5);
        mapSummaryData[iter->first].m_estSigma = (long)(tempTools.GetSigma()/100.0+0.5);
    }
}

void CAirsideFlightMutiRunDelayResult::BuildSummarySegmentData(MultiRunSummaryMap& mapSummaryData, 
    CAirsideFlightDelayReport::FltDelaySegment iType, mapSummarySegmentData& oriData, CParameters* pParameter)
{
    mapSummarySegmentData::iterator iter = oriData.begin();
    for (; iter != oriData.end(); ++iter)
    {
        CStatisticalTools<double> tempTools;
        size_t nCount = iter->second[iType].size();
        for (size_t n = 0; n < nCount; ++n)
        {
            tempTools.AddNewData((double)(iter->second[iType][n]));
        }
        tempTools.SortData();

        mapSummaryData[iter->first].m_estTotal = (long)(tempTools.GetSum()/100.0+0.5);
        mapSummaryData[iter->first].m_estMin = (long)(tempTools.GetMin()/100.0+0.5);
        mapSummaryData[iter->first].m_estAverage = (long)(tempTools.GetAvarage()/100.0+0.5);
        mapSummaryData[iter->first].m_estMax = (long)(tempTools.GetMax()/100.0+0.5);
        mapSummaryData[iter->first].m_estQ1 = (long)(tempTools.GetPercentile(25)/100.0+0.5);
        mapSummaryData[iter->first].m_estQ2 = (long)(tempTools.GetPercentile(50)/100.0+0.5);
        mapSummaryData[iter->first].m_estQ3 = (long)(tempTools.GetPercentile(75)/100.0+0.5);
        mapSummaryData[iter->first].m_estP1 = (long)(tempTools.GetPercentile(1)/100.0+0.5);
        mapSummaryData[iter->first].m_estP5 = (long)(tempTools.GetPercentile(5)/100.0+0.5);
        mapSummaryData[iter->first].m_estP10 = (long)(tempTools.GetPercentile(10)/100.0+0.5);
        mapSummaryData[iter->first].m_estP90 = (long)(tempTools.GetPercentile(90)/100.0+0.5);
        mapSummaryData[iter->first].m_estP95 = (long)(tempTools.GetPercentile(95)/100.0+0.5);
        mapSummaryData[iter->first].m_estP99 = (long)(tempTools.GetPercentile(99)/100.0+0.5);
        mapSummaryData[iter->first].m_estSigma = (long)(tempTools.GetSigma()/100.0+0.5);
    }
}

void CAirsideFlightMutiRunDelayResult::BuildSummarySegmentData2(SummaryCompomentAndSegmentResultMap& mapSummaryData, 
    mapSummarySegmentData& oriData)
{
    mapSummarySegmentData::iterator iter = oriData.begin();
    for (; iter != oriData.end(); ++iter)
    {
        summarySegmentData::iterator mapIter = iter->second.begin();
        for (; mapIter != iter->second.end(); ++mapIter)
        {
            CStatisticalTools<double> tempTools;
            for (size_t n = 0; n < mapIter->second.size(); ++n)
            {
                tempTools.AddNewData((double)mapIter->second[n]);
            }
            tempTools.SortData();

            mapSummaryData[iter->first][mapIter->first].m_estTotal = (long)(tempTools.GetSum()/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estMin = (long)(tempTools.GetMin()/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estAverage = (long)(tempTools.GetAvarage()/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estMax = (long)(tempTools.GetMax()/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estQ1 = (long)(tempTools.GetPercentile(25)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estQ2 = (long)(tempTools.GetPercentile(50)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estQ3 = (long)(tempTools.GetPercentile(75)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estP1 = (long)(tempTools.GetPercentile(1)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estP5 = (long)(tempTools.GetPercentile(5)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estP10 = (long)(tempTools.GetPercentile(10)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estP90 = (long)(tempTools.GetPercentile(90)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estP95 = (long)(tempTools.GetPercentile(95)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estP99 = (long)(tempTools.GetPercentile(99)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estSigma = (long)(tempTools.GetSigma()/100.0+0.5);
        }
    }
}

// void CAirsideFlightMutiRunDelayResult::BulidSummaryMultiRunDelayTimeData(SummaryDelayResultMap& mapSummaryData, 
//     CAirsideFlightDelayReport::FltDelaySegment iType,
//     mapDelayResultData mapData,
//     CParameters* pParameter)
// {
//     mapDelayResultData::iterator iter = mapData.begin();
//     for (; iter != mapData.end(); ++iter)
//     {
//         CStatisticalTools<double> tempTools;
//         int nCount = iter->second[iType].size();
//         for (size_t n = 0; n < nCount; ++n)
//         {
//             tempTools.AddNewData((double)(iter->second[iType][n].m_lDelayTime));
//         }
//         tempTools.SortData();
// 
//         mapSummaryData[iter->first].m_estTotal = (long)(tempTools.GetSum()/100.0+0.5);
//         mapSummaryData[iter->first].m_estMin = (long)(tempTools.GetMin()/100.0+0.5);
//         mapSummaryData[iter->first].m_estAverage = (long)(tempTools.GetAvarage()/100.0+0.5);
//         mapSummaryData[iter->first].m_estMax = (long)(tempTools.GetMax()/100.0+0.5);
//         mapSummaryData[iter->first].m_estQ1 = (long)(tempTools.GetPercentile(25)/100.0+0.5);
//         mapSummaryData[iter->first].m_estQ2 = (long)(tempTools.GetPercentile(50)/100.0+0.5);
//         mapSummaryData[iter->first].m_estQ3 = (long)(tempTools.GetPercentile(75)/100.0+0.5);
//         mapSummaryData[iter->first].m_estP1 = (long)(tempTools.GetPercentile(1)/100.0+0.5);
//         mapSummaryData[iter->first].m_estP5 = (long)(tempTools.GetPercentile(5)/100.0+0.5);
//         mapSummaryData[iter->first].m_estP10 = (long)(tempTools.GetPercentile(10)/100.0+0.5);
//         mapSummaryData[iter->first].m_estP90 = (long)(tempTools.GetPercentile(90)/100.0+0.5);
//         mapSummaryData[iter->first].m_estP95 = (long)(tempTools.GetPercentile(95)/100.0+0.5);
//         mapSummaryData[iter->first].m_estP99 = (long)(tempTools.GetPercentile(99)/100.0+0.5);
//         mapSummaryData[iter->first].m_estSigma = (long)(tempTools.GetSigma()/100.0+0.5);
//     }
// }

void CAirsideFlightMutiRunDelayResult::GenerateSummary3DChartTimeData(MultiRunSummaryMap& mapSummaryData, CARC3DChart& chartWnd, CParameters *pParameter )
{
    C2DChartData c2dGraphData;
    SetSummary3DChartString(c2dGraphData,pParameter);

    if (mapSummaryData.empty() == true)
        return;

    int nCount = sizeof(strSummaryContent)/sizeof(strSummaryContent[0]);
    for(int i=0; i<nCount; i++)
    {
        c2dGraphData.m_vrLegend.push_back(strSummaryContent[i]);
    }

    MultiRunSummaryMap::iterator iter = mapSummaryData.begin();
    for(; iter != mapSummaryData.end(); ++iter)
    {
        CString strSimName = iter->first;
        int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
        CString strXTickTitle;
        strXTickTitle.Format(_T("Run%d"), nCurSimResult);
        c2dGraphData.m_vrXTickTitle.push_back(strXTickTitle);


        c2dGraphData.m_vr2DChartData.resize(13);
        c2dGraphData.m_vr2DChartData[0].push_back((double)iter->second.m_estMin/60.0f);
        c2dGraphData.m_vr2DChartData[1].push_back((double)iter->second.m_estAverage/60.0f);
        c2dGraphData.m_vr2DChartData[2].push_back((double)iter->second.m_estMax/60.0f);
        c2dGraphData.m_vr2DChartData[3].push_back((double)iter->second.m_estQ1/60.0f);
        c2dGraphData.m_vr2DChartData[4].push_back((double)iter->second.m_estQ2/60.0f);
        c2dGraphData.m_vr2DChartData[5].push_back((double)iter->second.m_estQ3/60.0f);
        c2dGraphData.m_vr2DChartData[6].push_back((double)iter->second.m_estP1/60.0f);
        c2dGraphData.m_vr2DChartData[7].push_back((double)iter->second.m_estP5/60.0f);
        c2dGraphData.m_vr2DChartData[8].push_back((double)iter->second.m_estP10/60.0f);
        c2dGraphData.m_vr2DChartData[9].push_back((double)iter->second.m_estP90/60.0f);
        c2dGraphData.m_vr2DChartData[10].push_back((double)iter->second.m_estP95/60.0f);
        c2dGraphData.m_vr2DChartData[11].push_back((double)iter->second.m_estP99/60.0f);
        c2dGraphData.m_vr2DChartData[12].push_back((double)iter->second.m_estSigma/60.0f);
        
    }
    chartWnd.DrawChart(c2dGraphData);
}

void CAirsideFlightMutiRunDelayResult::BuildSummaryComponentSegmentData(SummaryCompomentAndSegmentResultMap& mapSummaryData, 
    mapDelayResultData& componentSegmentMapData)
{
    mapDelayResultData::iterator iter = componentSegmentMapData.begin();
    for (; iter != componentSegmentMapData.end(); ++iter)
    {
        mapDelayData::iterator mapIter = iter->second.begin();
        for (; mapIter != iter->second.end(); ++mapIter)
        {
            CStatisticalTools<double> tempTools;
            for (size_t n = 0; n < mapIter->second.size(); ++n)
            {
                tempTools.AddNewData((double)mapIter->second[n].m_lDelayTime);
            }
            tempTools.SortData();

            mapSummaryData[iter->first][mapIter->first].m_estTotal = (long)(tempTools.GetSum()/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estMin = (long)(tempTools.GetMin()/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estAverage = (long)(tempTools.GetAvarage()/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estMax = (long)(tempTools.GetMax()/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estQ1 = (long)(tempTools.GetPercentile(25)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estQ2 = (long)(tempTools.GetPercentile(50)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estQ3 = (long)(tempTools.GetPercentile(75)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estP1 = (long)(tempTools.GetPercentile(1)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estP5 = (long)(tempTools.GetPercentile(5)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estP10 = (long)(tempTools.GetPercentile(10)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estP90 = (long)(tempTools.GetPercentile(90)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estP95 = (long)(tempTools.GetPercentile(95)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estP99 = (long)(tempTools.GetPercentile(99)/100.0+0.5);
            mapSummaryData[iter->first][mapIter->first].m_estSigma = (long)(tempTools.GetSigma()/100.0+0.5);
        }
    }
}

void CAirsideFlightMutiRunDelayResult::GenerateSummaryComponentSegmentTimeData(SummaryCompomentAndSegmentResultMap& mapSummaryData,
    int iType, CARC3DChart& chartWnd, CParameters *pParameter)
{
    C2DChartData c2dGraphData;
    SetSummary3DChartString(c2dGraphData, pParameter, iType);

    if (mapSummaryData.empty() == true)
        return;

    int nCount = sizeof(strSummaryContent)/sizeof(strSummaryContent[0]);
    for(int i=0; i<nCount; i++)
    {
        c2dGraphData.m_vrLegend.push_back(strSummaryContent[i]);
    }

    c2dGraphData.m_vr2DChartData.resize(13);
    SummaryCompomentAndSegmentResultMap::iterator iter = mapSummaryData.begin();
    for(; iter != mapSummaryData.end(); ++iter)
    {
        CString strSimName = iter->first;
        int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
        CString strXTickTitle;
        strXTickTitle.Format(_T("Run%d"), nCurSimResult);
        c2dGraphData.m_vrXTickTitle.push_back(strXTickTitle);

        c2dGraphData.m_vr2DChartData[0].push_back((double)iter->second[iType].m_estMin/60.0f);
        c2dGraphData.m_vr2DChartData[1].push_back((double)iter->second[iType].m_estAverage/60.0f);
        c2dGraphData.m_vr2DChartData[2].push_back((double)iter->second[iType].m_estMax/60.0f);
        c2dGraphData.m_vr2DChartData[3].push_back((double)iter->second[iType].m_estQ1/60.0f);
        c2dGraphData.m_vr2DChartData[4].push_back((double)iter->second[iType].m_estQ2/60.0f);
        c2dGraphData.m_vr2DChartData[5].push_back((double)iter->second[iType].m_estQ3/60.0f);
        c2dGraphData.m_vr2DChartData[6].push_back((double)iter->second[iType].m_estP1/60.0f);
        c2dGraphData.m_vr2DChartData[7].push_back((double)iter->second[iType].m_estP5/60.0f);
        c2dGraphData.m_vr2DChartData[8].push_back((double)iter->second[iType].m_estP10/60.0f);
        c2dGraphData.m_vr2DChartData[9].push_back((double)iter->second[iType].m_estP90/60.0f);
        c2dGraphData.m_vr2DChartData[10].push_back((double)iter->second[iType].m_estP95/60.0f);
        c2dGraphData.m_vr2DChartData[11].push_back((double)iter->second[iType].m_estP99/60.0f);
        c2dGraphData.m_vr2DChartData[12].push_back((double)iter->second[iType].m_estSigma/60.0f);

    }
    chartWnd.DrawChart(c2dGraphData);
}

void CAirsideFlightMutiRunDelayResult::SetSummary3DChartString( C2DChartData& c2dGraphData,CParameters *pParameter,int iType /*= 0*/ )
{
    ASSERT(pParameter);
    if (pParameter->getReportType() == ASReportType_Summary)
    {
        CAirsideFlightDelayParam* pDelayPara = (CAirsideFlightDelayParam*)pParameter;
        switch (pDelayPara->getSubType())
        {
        case CAirsideFlightDelayReport::SRT_SUMMARY_FLIGHTTOTALDELAY:
            SetSummaryTotalDelay3DChartString(c2dGraphData,pParameter);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_AIRDELAY:
            SetSummaryAirDelay3DChartString(c2dGraphData,pParameter);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_TAXIDELAY:
            SetSummaryTaxiDelay3DChartString(c2dGraphData,pParameter);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_TAKOFFDELAY:
            SetSummaryTakeoffDelay3DChartString(c2dGraphData,pParameter);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_STANDDELAY:
            SetSummaryStandDelay3DChartString(c2dGraphData,pParameter);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_SCHEDULEDELAY:
            SetSummaryScheduleDelay3DChartString(c2dGraphData,pParameter);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_SEGMENTDELAY:
            SetSummarySegmentDelay3DChartString(c2dGraphData,pParameter,iType);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_COMPONENTDELAY:
            SetSummaryComponentDelay3DChartString(c2dGraphData,pParameter,iType);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_SERVICEDELAY:
            SetSummaryServiceDelay3DChartString(c2dGraphData,pParameter);
            break;
        default:
            break;
        }
    }
}

void CAirsideFlightMutiRunDelayResult::SetSummaryTotalDelay3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T("Total delay Report(Summary)");
    c2dGraphData.m_strYtitle = _T("Delays (mins)");
    c2dGraphData.m_strXtitle = _T("Runs");	

    //set footer
    CString strFooter(_T(""));
    strFooter.Format(_T("TOTAL DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideFlightMutiRunDelayResult::SetSummaryAirDelay3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T("Air delay Report(Summary)");
    c2dGraphData.m_strYtitle = _T("Delays (mins)");
    c2dGraphData.m_strXtitle.Format(_T("Runs"));

    CString strFooter(_T(""));
    strFooter.Format(_T("AIR DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideFlightMutiRunDelayResult::SetSummaryTaxiDelay3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T("Taxi delay Report(Summary)");
    c2dGraphData.m_strYtitle = _T("Delays (mins)");
    c2dGraphData.m_strXtitle.Format(_T("Runs"));

    CString strFooter(_T(""));
    strFooter.Format(_T("TAXI DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideFlightMutiRunDelayResult::SetSummaryStandDelay3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T("Stand delay Report(Summary)");
    c2dGraphData.m_strYtitle = _T("Delays (mins)");
    c2dGraphData.m_strXtitle.Format(_T("Runs"));

    CString strFooter(_T(""));
    strFooter.Format(_T("STAND DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideFlightMutiRunDelayResult::SetSummaryTakeoffDelay3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T("Takeoff delay Report(Summary)");
    c2dGraphData.m_strYtitle = _T("Delays (mins)");
    c2dGraphData.m_strXtitle.Format(_T("Runs"));

    CString strFooter(_T(""));
    strFooter.Format(_T("TAKEOFF DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideFlightMutiRunDelayResult::SetSummaryServiceDelay3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T("Service delay Report(Summary)");
    c2dGraphData.m_strYtitle = _T("Delays (mins)");
    c2dGraphData.m_strXtitle.Format(_T("Simulation Runs"));

    CString strFooter(_T(""));
    strFooter.Format(_T("SERVICE DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideFlightMutiRunDelayResult::SetSummarySegmentDelay3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter,int iType )
{
    CString strChartTitle;
    strChartTitle.Format(_T(" Segment delay Report %s(Summary) "),strSegmentString[iType]);
    c2dGraphData.m_strChartTitle = strChartTitle;
    c2dGraphData.m_strYtitle = _T("Delays (mins)");
    c2dGraphData.m_strXtitle.Format(_T("Runs"));

    CString strFooter(_T(""));
    strFooter.Format(_T("SEGMENT DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideFlightMutiRunDelayResult::SetSummaryScheduleDelay3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
    c2dGraphData.m_strChartTitle = _T("Schedule delay Report(Summary)");
    c2dGraphData.m_strYtitle = _T("Delays (mins)");
    c2dGraphData.m_strXtitle.Format(_T("Runs"));

    CString strFooter(_T(""));
    strFooter.Format(_T("SCHEDULE DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}

void CAirsideFlightMutiRunDelayResult::SetSummaryComponentDelay3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter,int iType )
{
    CString strChartTitle;
    strChartTitle.Format(_T(" Component delay Report %s(Summary) "),strComponentString[iType]);
    c2dGraphData.m_strChartTitle = strChartTitle;
    c2dGraphData.m_strYtitle = _T("Delays (mins)");
    c2dGraphData.m_strXtitle.Format(_T("Runs"));

    CString strFooter(_T(""));
    strFooter.Format(_T("COMPONENT DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
    c2dGraphData.m_strFooter = strFooter;
}





