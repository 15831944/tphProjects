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

CAirsideFlightMutiRunDelayResult::CAirsideFlightMutiRunDelayResult(void)
{
}


CAirsideFlightMutiRunDelayResult::~CAirsideFlightMutiRunDelayResult(void)
{
}

void CAirsideFlightMutiRunDelayResult::LoadMultipleRunReport(long iInterval)
{
	ClearData();

	mapLoadResult mapTotalDelay;
	mapLoadResult mapAirDelay;
	mapLoadResult mapTaxiDelay;
	mapLoadResult mapStandDelay;
	mapLoadResult mapServiceDelay;
	mapLoadResult mapTakeoffDelay;
	mapLoadResult mapScheduleDelay;

	mapLoadComponentAndSegment mapSegmentDelay;
	mapLoadComponentAndSegment mapComponentDelay;
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

						int iAirDelay = 0;
						file.getInteger(iAirDelay);
						mapAirDelay[strSimResult].push_back(iAirDelay);

						int iTaxiDelay = 0;
						file.getInteger(iTaxiDelay);
						mapTaxiDelay[strSimResult].push_back(iTaxiDelay);

						int iStandDelay = 0;
						file.getInteger(iStandDelay);
						mapStandDelay[strSimResult].push_back(iStandDelay);

						int iServiceDelay = 0;
						file.getInteger(iServiceDelay);
						mapServiceDelay[strSimResult].push_back(iServiceDelay);

						int iTakeoffDelay = 0;
						file.getInteger(iTakeoffDelay);
						mapTakeoffDelay[strSimResult].push_back(iTakeoffDelay);

						long actStartTime = 0;
						file.getInteger(actStartTime);
						long actEndTime = 0;
						file.getInteger(actEndTime);
						long planSt = 0;
						file.getInteger(planSt);

						file.skipField(2);
						int iArrOrDeplDelay = 0;
						file.getInteger(iArrOrDeplDelay);

						int iScheduleDelay = 0;
						if (iArrOrDeplDelay)
						{
							iScheduleDelay = max(actEndTime - planSt,0l);
						}
						else
						{
							iScheduleDelay = max(actStartTime - planSt,0l);
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
						if(mapSegmentDelay[strSimResult].empty())
						{
							mapSegmentDelay[strSimResult][FltDelayReason_Slowed].clear();
							mapSegmentDelay[strSimResult][FltDelayReason_Vectored].clear();
							mapSegmentDelay[strSimResult][FltDelayReason_AirHold].clear();
							mapSegmentDelay[strSimResult][FltDelayReason_AltitudeChanged].clear();
							mapSegmentDelay[strSimResult][FltDelayReason_SideStep].clear();
							mapSegmentDelay[strSimResult][FltDelayReason_Stop].clear();
							mapSegmentDelay[strSimResult][FltDelayReason_Service].clear();
						}

						if (mapComponentDelay[strSimResult].empty())
						{
							mapComponentDelay[strSimResult][CAirsideFlightDelayReport::FltDelaySegment_Air].clear();
							mapComponentDelay[strSimResult][CAirsideFlightDelayReport::FltDelaySegment_Taxi].clear();
							mapComponentDelay[strSimResult][CAirsideFlightDelayReport::FltDelaySegment_Stand].clear();
							mapComponentDelay[strSimResult][CAirsideFlightDelayReport::FltDelaySegment_Service].clear();
							mapComponentDelay[strSimResult][CAirsideFlightDelayReport::FltDelaySegment_TakeOff].clear();
						}
						for (int iSkip = 0; iSkip < nNodeCount; iSkip++)
						{
							long lDelay = 0;
							file.getInteger(lDelay);
							file.skipField(1);
							int iSegment = 0;
							file.getInteger(iSegment);
							mapSegmentDelay[strSimResult][iSegment].push_back(lDelay);

							file.skipField(2);
							int iReason = 0;
							file.getInteger(iReason);
							mapComponentDelay[strSimResult][iReason].push_back(lDelay);
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
	BulidDetailMultiRunDelayCount(m_totalDelayData,mapTotalDelay,iInterval);
	//generate detail of air delay
	BulidDetailMultiRunDelayTime(m_airDelayData,mapAirDelay,iInterval);
	//generate detail of stand delay
	BulidDetailMultiRunDelayTime(m_standDelayData,mapStandDelay,iInterval);
	//generate detail of  taxi delay
	BulidDetailMultiRunDelayTime(m_taxiDelayData,mapTaxiDelay,iInterval);
	//generate detail of service delay
	BulidDetailMultiRunDelayTime(m_serviceDelayData,mapServiceDelay,iInterval);
	//generate detail of takeoff delay
	BulidDetailMultiRunDelayTime(m_takeoffDelayData,mapTakeoffDelay,iInterval);
	//generate detail of schedule delay
	BulidDetailMultiRunDelayCount(m_scheduleDelayData,mapScheduleDelay,iInterval);

	//generate detail of segment delay
	BuildDetailComponentSegmentData(m_segmentDelayData,mapSegmentDelay,iInterval);
	//generate detail of component delay
	BuildDetailComponentSegmentData(m_componentDelayData,mapComponentDelay,iInterval);
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

long CAirsideFlightMutiRunDelayResult::ClacTimeRange(ElapsedTime& eMaxTime,ElapsedTime& eMinValue,ElapsedTime& eInterval)
{
	if (eMinValue == eMaxTime)
	{
		eMinValue = ElapsedTime(0L);
	}
	bool bInSecond = false;
	int _nMinNumCol = 3;
	ElapsedTime timeDiff = eMaxTime - eMinValue;

	ElapsedTime eUnitTimeOnX;

	int nTimeDiffAsHours = timeDiff.asHours();
	int nTimeDiffAsMin = timeDiff.asMinutes();
	int nTimeDiffAsSec = timeDiff.asSeconds();

	if( nTimeDiffAsHours > _nMinNumCol )
	{
		// if hh  > _nMinNumCol, display column up to 2*_nMinNumCol
		int nMinHours = nTimeDiffAsHours / (2*_nMinNumCol) + 1;
		eUnitTimeOnX.set( nMinHours, 0, 0 );
		eMinValue.SetMinute( 0 );
		eMinValue.SetSecond( 0 );
		if( nTimeDiffAsHours > (2*_nMinNumCol) )
			_nMinNumCol = (2*_nMinNumCol);
		else
			_nMinNumCol = nTimeDiffAsHours;
	}
	else if( nTimeDiffAsMin > _nMinNumCol )
	{
		// else if hhmm > nNumCol min, so the column unit should be hhmm % 2 nNumCol min
		int nMinMinutes = nTimeDiffAsMin / ( 2*_nMinNumCol ) + 1;
		eUnitTimeOnX.set( nMinMinutes / 60, nMinMinutes % 60, 0 );
		eMinValue.SetSecond( 0 );
		if( nTimeDiffAsMin > 2 * _nMinNumCol )
			_nMinNumCol = 2 * _nMinNumCol;
		else
			_nMinNumCol = nTimeDiffAsMin;
	}
	else if( nTimeDiffAsSec > _nMinNumCol )
	{
		// be hhmmss % 2 nNumCol sec.
		int mMinSeconds = nTimeDiffAsSec / ( 2*_nMinNumCol ) + 1;
		eUnitTimeOnX.set( mMinSeconds / 3600, mMinSeconds / 60, mMinSeconds % 60 );
		if( nTimeDiffAsSec > 2 * _nMinNumCol )
			_nMinNumCol = 2 * _nMinNumCol;
		else
			_nMinNumCol = nTimeDiffAsSec;
		bInSecond = true;
	}

	eInterval = eUnitTimeOnX;
	if (eInterval == ElapsedTime(0L))
	{
		eInterval = ElapsedTime(60L);
	}

	return _nMinNumCol;
}

void CAirsideFlightMutiRunDelayResult::BuildDetailComponentSegmentData(DelayComponentAndSegmentMap mapDetailData,mapLoadComponentAndSegment& componentSegmentMapData,long iInterval)
{
	std::map<long, GroupMinMax> mapMinMaxData;
	mapLoadComponentAndSegment::iterator iter = componentSegmentMapData.begin();
	for (; iter != componentSegmentMapData.begin(); ++iter)
	{
		mapComponentAndSegmentResult::iterator mapIter = iter->second.begin();
		for (; mapIter != iter->second.end(); ++mapIter)
		{
			if (mapMinMaxData.find(mapIter->first) == mapMinMaxData.end())
			{
				GroupMinMax groupData;
				groupData.m_iMin = GetComponentSegmentMinValue(componentSegmentMapData,mapIter->first);
				groupData.m_iMax = GetComponentSegmentMaxValue(componentSegmentMapData,mapIter->first);
				mapMinMaxData[mapIter->first] = groupData;
			}
			
			long lMinDelayTime = mapMinMaxData[mapIter->first].m_iMin;
			long lMaxDelayTime = mapMinMaxData[mapIter->first].m_iMax;

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

			for (long i = 0; i < lDelayTimeSegmentCount; i++)
			{
				ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*i);
				ElapsedTime estTempMaxDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*(i + 1));

				MultipleRunFlightDelayData delayData;
				delayData.m_iStart = estTempMinDelayTime.getPrecisely();
				delayData.m_iEnd = estTempMaxDelayTime.getPrecisely();

				long iDelayTime = GetIntervalDelayTime(delayData.m_iStart,delayData.m_iEnd,mapIter->second);
				delayData.m_iData = iDelayTime;
				
				mapDetailData[iter->first][mapIter->first].push_back(delayData);
			}
		}
	}
}

void CAirsideFlightMutiRunDelayResult::BulidDetailMultiRunDelayCount( DelayDetailMap& mapDetailData,mapLoadResult mapData,long iInterval )
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

			MultipleRunFlightDelayData delayData;
			delayData.m_iStart = estTempMinDelayTime.getPrecisely();
			delayData.m_iEnd = estTempMaxDelayTime.getPrecisely();

			long iCount = GetIntervalCount(delayData.m_iStart,delayData.m_iEnd,iter->second);
			delayData.m_iData = iCount;
			mapDetailData[iter->first].push_back(delayData);
		}
	}
}

void CAirsideFlightMutiRunDelayResult::BulidDetailMultiRunDelayTime( DelayDetailMap& mapDetailData,mapLoadResult mapData,long iInterval )
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

			MultipleRunFlightDelayData delayData;
			delayData.m_iStart = estTempMinDelayTime.getPrecisely();
			delayData.m_iEnd = estTempMaxDelayTime.getPrecisely();

			long iDelayTime = GetIntervalDelayTime(delayData.m_iStart,delayData.m_iEnd,iter->second);
			delayData.m_iData = iDelayTime;
			mapDetailData[iter->first].push_back(delayData);
		}
	}
}

long CAirsideFlightMutiRunDelayResult::GetComponentSegmentMaxValue( mapLoadComponentAndSegment& componentSegmentMapData,int iType )
{
	bool bInitFlag = true;
	long iMax = 0;
	mapLoadComponentAndSegment::iterator iter = componentSegmentMapData.begin();
	for (; iter != componentSegmentMapData.begin(); ++iter)
	{
		int iValue = GetMaxValue(iter->second[iType]);
		if (bInitFlag)
		{
			iMax = iValue;
			bInitFlag = false;
		}
		else if (iMax < iValue)
		{
			iMax = iValue;
		}
	}
	return iMax;
}

long CAirsideFlightMutiRunDelayResult::GetComponentSegmentMinValue( mapLoadComponentAndSegment& componentSegmentMapData,int iType )
{
	bool bInitFlag = true;
	long iMin = 0;
	mapLoadComponentAndSegment::iterator iter = componentSegmentMapData.begin();
	for (; iter != componentSegmentMapData.begin(); ++iter)
	{
		int iValue = GetMinValue(iter->second[iType]);
		if (bInitFlag)
		{
			iMin = iValue;
		}
		else if (iMin > iValue)
		{
			iMin = iValue;
		}
	}
	return iMin;
}


long CAirsideFlightMutiRunDelayResult::GetMapMinValue( mapLoadResult mapData )
{
	bool bInitFlag = true;
	long iMin = 0;
	mapLoadResult::iterator iter = mapData.begin();
	for (; iter != mapData.end(); ++iter)
	{
		 int iValue = GetMinValue(iter->second);
		if (bInitFlag)
		{
			iMin = iValue;
			bInitFlag = false;
		}
		else if (iMin > iValue)
		{
			iMin = iValue;
		}
	}
	return iMin;
}

long CAirsideFlightMutiRunDelayResult::GetMapMaxValue( mapLoadResult mapData )
{
	bool bInitFlag = true;
	long iMax = 0;
	mapLoadResult::iterator iter = mapData.begin();
	for (; iter != mapData.end(); ++iter)
	{
		int iValue = GetMaxValue(iter->second);
		if (bInitFlag)
		{
			iMax = iValue;
			bInitFlag = false;
		}
		else if (iMax < iValue)
		{
			iMax = iValue;
		}
	}
	return iMax;
}

long CAirsideFlightMutiRunDelayResult::GetMinValue( std::vector<long> vData ) const
{
	bool bInitFlag = true;
	long iMin = 0;
	for (size_t i = 0; i < vData.size(); i++)
	{
		int iValue = vData.at(i);
		if (bInitFlag)
		{
			iMin = iValue;
			bInitFlag = false;
		}
		else if (iMin > iValue)
		{
			iMin = iValue;
		}
	}
	return iMin;
}

long CAirsideFlightMutiRunDelayResult::GetMaxValue( std::vector<long> vData ) const
{
	bool bInitFlag = true;
	long iMax = 0;
	for (size_t i = 0; i < vData.size(); i++)
	{
		int iValue = vData.at(i);
		if (bInitFlag)
		{
			iMax = iValue;
			bInitFlag = false;
		}
		else if (iMax < iValue)
		{
			iMax = iValue;
		}
	}
	return iMax;
}

int CAirsideFlightMutiRunDelayResult::GetIntervalCount( long iStart, long iEnd, std::vector<long> vData ) const
{
	int iCount = 0;
	for (unsigned i = 0; i < vData.size(); i++)
	{
		long iData = vData.at(i);
		if (iData >= iStart && iData < iEnd)
		{
			iCount++;
		}
	}
	return iCount;
}

long CAirsideFlightMutiRunDelayResult::GetIntervalDelayTime( long iStart,long iEnd,std::vector<long> vData ) const
{
	long iDelayTime = 0;
	for (unsigned i = 0; i < vData.size(); i++)
	{
		long iData = vData.at(i);
		if (iData >= iStart && iData < iEnd)
		{
			iDelayTime += iData;
		}
	}
	return iDelayTime;
}

void CAirsideFlightMutiRunDelayResult::InitDetailListHead( CXListCtrl& cxListCtrl,DelayDetailMap mapDetailData,CSortableHeaderCtrl* piSHC/*=NULL*/ )
{
	if (mapDetailData.empty())
		return;

	DelayDetailMap::iterator iter = mapDetailData.begin();
	size_t rangeCount = iter->second.size();
	for (size_t i = 0; i < rangeCount; ++i)
	{
		MultipleRunFlightDelayData delayData = iter->second.at(i);
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

void CAirsideFlightMutiRunDelayResult::InitSummaryListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC)
{
    cxListCtrl.DeleteAllItems();

    while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
    {
        cxListCtrl.DeleteColumn(0);
    }
    cxListCtrl.InsertColumn(2, _T("Total Delay(mins)"), LVCFMT_LEFT, 100);
    cxListCtrl.InsertColumn(3, _T("Min Delay(mins)"), LVCFMT_LEFT, 100);
    cxListCtrl.InsertColumn(4, _T("Max Delay(mins)"), LVCFMT_LEFT, 100);
    cxListCtrl.InsertColumn(5, _T("Mean Delay(mins)"), LVCFMT_LEFT, 100);
    cxListCtrl.InsertColumn(6, _T("Q1(mins)"), LVCFMT_LEFT, 100);
    cxListCtrl.InsertColumn(7, _T("Q2(mins)"), LVCFMT_LEFT, 100);
    cxListCtrl.InsertColumn(8, _T("Q3(mins)"), LVCFMT_LEFT, 100);
    cxListCtrl.InsertColumn(9, _T("P1(mins)"), LVCFMT_LEFT, 100);
    cxListCtrl.InsertColumn(10, _T("P5(mins)"), LVCFMT_LEFT, 100);
    cxListCtrl.InsertColumn(11, _T("P10(mins)"), LVCFMT_LEFT, 100);
    cxListCtrl.InsertColumn(12, _T("P90(mins)"), LVCFMT_LEFT, 100);
    cxListCtrl.InsertColumn(13, _T("P95(mins)"), LVCFMT_LEFT, 100);
    cxListCtrl.InsertColumn(14, _T("P99(mins)"), LVCFMT_LEFT, 100);
    cxListCtrl.InsertColumn(15, _T("Std dev(mins)"), LVCFMT_LEFT, 100);

    if (piSHC)
    {
        piSHC->ResetAll();
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
		MultipleRunFlightDelayData delayData = iter->second[iType].at(i);
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
        InitSummaryListHead(cxListCtrl, piSHC);
    }
}
void CAirsideFlightMutiRunDelayResult::FillDetailListCountContent( CXListCtrl& cxListCtrl,DelayDetailMap mapDetailData )
{
	DelayDetailMap::iterator iter = mapDetailData.begin();
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
			MultipleRunFlightDelayData delayData = iter->second.at(n);
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

void CAirsideFlightMutiRunDelayResult::FillSummaryListContent(CXListCtrl& cxListCtrl, DelayDetailMap mapDetailData)
{
    DelayDetailMap::iterator iter = mapDetailData.begin();

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

        cxListCtrl.SetItemText(idx, 1, strRun);
        cxListCtrl.SetItemData(idx, idx);

        CStatisticalTools<double> tempTools;
        for (size_t n = 0; n < iter->second.size(); ++n)
        {
            MultipleRunFlightDelayData delayData = iter->second.at(n);
            tempTools.AddNewData((double)delayData.m_iData);
        }
        tempTools.SortData();

        int nCurCol = 2;
        CString strTemp;
        strTemp.Format("%d", (long)tempTools.GetSum());
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%d", (long)tempTools.GetMin());
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%d", (long)tempTools.GetMax());
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", tempTools.GetAvarage());
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", tempTools.GetMSExcelPercentile(25));
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", tempTools.GetMSExcelPercentile(50));
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", tempTools.GetMSExcelPercentile(75));
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", tempTools.GetMSExcelPercentile(1));
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", tempTools.GetMSExcelPercentile(5));
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", tempTools.GetMSExcelPercentile(10));
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", tempTools.GetMSExcelPercentile(90));
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", tempTools.GetMSExcelPercentile(95));
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", tempTools.GetMSExcelPercentile(99));
        cxListCtrl.SetItemText(idx, nCurCol, strTemp);
        nCurCol++;

        strTemp.Format("%.2f", tempTools.GetSigma());
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

		for (size_t n = 0; n < iter->second[iType].size(); ++n)
		{
			MultipleRunFlightDelayData delayData = iter->second[iType].at(n);
			ElapsedTime eTime;
			if(n <  iter->second.size())
				eTime.setPrecisely(delayData.m_iData);

			cxListCtrl.SetItemText(idx, n + 2, eTime.printTime());
		}
		idx++;
	}
}

void CAirsideFlightMutiRunDelayResult::FillDetailListTimeContent( CXListCtrl& cxListCtrl,DelayDetailMap mapDetailData )
{
	DelayDetailMap::iterator iter = mapDetailData.begin();
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
			MultipleRunFlightDelayData delayData = iter->second.at(n);
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
            FillSummaryListContent(cxListCtrl,m_totalDelayData);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_AIRDELAY:
            FillSummaryListContent(cxListCtrl,m_airDelayData);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_TAXIDELAY:
            FillSummaryListContent(cxListCtrl,m_taxiDelayData);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_TAKOFFDELAY:
            FillSummaryListContent(cxListCtrl,m_takeoffDelayData);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_STANDDELAY:
            FillSummaryListContent(cxListCtrl,m_standDelayData);
            break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_SCHEDULEDELAY:
        case CAirsideFlightDelayReport::SRT_DETAIL_SCHEDULEDELAY:// default choose 
            FillSummaryListContent(cxListCtrl,m_scheduleDelayData);
            break;
//         case CAirsideFlightDelayReport::SRT_SUMMARY_SEGMENTDELAY:
//             FillSummaryListContent(cxListCtrl,m_segmentDelayData);
//             break;
//         case CAirsideFlightDelayReport::SRT_SUMMARY_COMPONENTDELAY:
//             FillSummaryListContent(cxListCtrl,m_componentDelayData);
//             break;
        case CAirsideFlightDelayReport::SRT_SUMMARY_SERVICEDELAY:
            FillSummaryListContent(cxListCtrl,m_serviceDelayData);
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
}

void CAirsideFlightMutiRunDelayResult::GenerateComponentSegmentTimeData(DelayComponentAndSegmentMap mapDetailData,int iType,CARC3DChart& chartWnd, CParameters *pParameter)
{
	C2DChartData c2dGraphData;

	SetDetail3DChartString(c2dGraphData,pParameter);

	if (mapDetailData.empty() == true)
		return;

	DelayComponentAndSegmentMap::iterator iter = mapDetailData.begin();
	for (unsigned iTitle = 0; iTitle < iter->second.size(); iTitle++)
	{
		MultipleRunFlightDelayData delayData = iter->second[iType].at(iTitle);
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
			MultipleRunFlightDelayData delayData = iter->second[iType].at(i);
			vData.push_back(delayData.m_iData);
		}
		c2dGraphData.m_vr2DChartData.push_back(vData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CAirsideFlightMutiRunDelayResult::Generate3DChartCountData( DelayDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter )
{
	C2DChartData c2dGraphData;
	
	SetDetail3DChartString(c2dGraphData,pParameter);

	if (mapDetailData.empty() == true)
		return;

	DelayDetailMap::iterator iter = mapDetailData.begin();
	for (unsigned iTitle = 0; iTitle < iter->second.size(); iTitle++)
	{
		MultipleRunFlightDelayData delayData = iter->second.at(iTitle);
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
			MultipleRunFlightDelayData delayData = iter->second.at(i);
			vData.push_back(delayData.m_iData);
		}
		c2dGraphData.m_vr2DChartData.push_back(vData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CAirsideFlightMutiRunDelayResult::Generate3DChartTimeData( DelayDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter )
{
	C2DChartData c2dGraphData;

	SetDetail3DChartString(c2dGraphData,pParameter);

	if (mapDetailData.empty() == true)
		return;

	DelayDetailMap::iterator iter = mapDetailData.begin();
	for (unsigned iTitle = 0; iTitle < iter->second.size(); iTitle++)
	{
		MultipleRunFlightDelayData delayData = iter->second.at(iTitle);
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
			MultipleRunFlightDelayData delayData = iter->second.at(i);
			ElapsedTime tTime;
			tTime.setPrecisely(delayData.m_iData);
			vData.push_back(tTime.asMinutes());
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

void CAirsideFlightMutiRunDelayResult::SetDetail3DChartString( C2DChartData& c2dGraphData,CParameters *pParameter )
{
	ASSERT(pParameter);
	if (pParameter->getReportType() == ASReportType_Detail)
	{
		switch (pParameter->getSubType())
		{
		case CAirsideFlightDelayReport::SRT_DETAIL_FLIGHTTOTALDELAY:
			SetDetailTakeoffDelay3DChartString(c2dGraphData,pParameter);
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
			SetDetailSegmentDelay3DChartString(c2dGraphData,pParameter);
			break;
		case CAirsideFlightDelayReport::SRT_DETAIL_COMPONENTDELAY:
			SetDetailCompointDelay3DChartString(c2dGraphData,pParameter);
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

void CAirsideFlightMutiRunDelayResult::SetDetailSegmentDelay3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T(" Delay vs. time of day ");
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

void CAirsideFlightMutiRunDelayResult::SetDetailCompointDelay3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T(" Delay component vs. time of day ");
	c2dGraphData.m_strYtitle = _T("Delays (mins)");
	c2dGraphData.m_strXtitle.Format(_T("Time of day"));

	CString strFooter(_T(""));
	strFooter.Format(_T("COMPONENT DELAY REPORT %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	c2dGraphData.m_strFooter = strFooter;
}





