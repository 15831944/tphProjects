#include "StdAfx.h"
#include ".\airsideflightrunwaydelaydata.h"
#include "../Results/ARCBaseLog.h"
#include "../Results/AirsideFlightLogItem.h"
#include "../Results/ARCBaseLog.hpp"
#include "../Results/AirsideReportLog.h"
#include "../Results/AirsideFllightRunwayDelayLog.h"
#include "../Results/AirsideFlightEventLog.h"
#include "Parameters.h"
#include "AirsideFlightRunwayDelayReportPara.h"
#include "LogFilterFun.h"


AirsideFlightRunwayDelayData::AirsideFlightRunwayDelayData(AirsideFlightRunwayDelayReportPara * parameter)
:m_pParameter(parameter)
{
	m_filterFun = new CDefaultFilterFun(parameter);
}

AirsideFlightRunwayDelayData::~AirsideFlightRunwayDelayData(void)
{
	delete m_filterFun;
	m_filterFun = NULL;

	ClearData();
}

AirsideFlightRunwayDelayData::RunwayDelayItem::RunwayDelayItem()
{

}

AirsideFlightRunwayDelayData::RunwayDelayItem::~RunwayDelayItem()
{

}

int AirsideFlightRunwayDelayData::RunwayDelayItem::GetDelayInIntervalIndex(long lInterval)
{
	int nIdx = m_lTotalDelay/lInterval;
	if (m_lTotalDelay % lInterval > 0)
		nIdx +=1;

	return max(nIdx-1,0) ;
}

int AirsideFlightRunwayDelayData::RunwayDelayItem::GetTimeInIntervalIndex(CParameters* pParameter)
{
	ElapsedTime tTime = m_tStartTime - pParameter->getStartTime();
	long lTime = (long)tTime.asSeconds();
	int nIdx = lTime/pParameter->getInterval();

	if (lTime%pParameter->getInterval() > 0)
		nIdx +=1;

	return max(nIdx-1,0) ;
}

void AirsideFlightRunwayDelayData::LoadData(const CString& _csDescFileName,const CString& _csEventFileName)
{
	ARCBaseLog<AirsideFlightLogItem> mFlightLogData;
	mFlightLogData.OpenInput(_csDescFileName.GetString(), _csEventFileName.GetString());
	int nCount = mFlightLogData.getItemCount();
	for (int i = 0; i < nCount; i++)
	{
		mFlightLogData.LoadItem(mFlightLogData.ItemAt(i));
		AirsideFlightLogItem item = mFlightLogData.ItemAt(i);
		AddRunwayDelay(item);
	}
}

bool CompareRwyDelayLogData(AirsideFlightRunwayDelayLog* fLog, AirsideFlightRunwayDelayLog* sLog)
{
	if ((int)fLog->mPosition < (int)sLog->mPosition)
		return true;

	if ((int)fLog->mPosition == (int)sLog->mPosition && fLog->mAvailableTime < sLog->mAvailableTime )
		return true;

	return false;
}	

bool CompareDelayInfoData(const AirsideFlightRunwayDelayData::DETAILINFO& fInfo,const AirsideFlightRunwayDelayData::DETAILINFO& sInfo)
{
	if (fInfo.first > sInfo.first)
		return true;

	return false;
}

void AirsideFlightRunwayDelayData::AddRunwayDelay(const AirsideFlightLogItem& item)
{		
	AirsideFlightLogDesc fltdesc = item.mFltDesc;

	AirsideFlightDescStruct descStruct;
	fltdesc.getFlightDescStruct(descStruct);

	std::vector<AirsideFlightRunwayDelayLog*> vFitDelays;
	vFitDelays.clear();

	//to get data which fit parameter setting
	int nCount = item.mpEventList->mEventList.size();
	for (int i = 0; i < nCount; i++)
	{
		ARCEventLog* pLog = item.mpEventList->mEventList.at(i);

		if(!m_filterFun->bFit(typeof(AirsideFlightRunwayDelayLog), pLog))
			continue ;

		AirsideFlightRunwayDelayLog* pDelayLog = (AirsideFlightRunwayDelayLog*)pLog;

		if (!m_pParameter->IsDelayLogFitPara(pDelayLog,descStruct))
			continue;

		vFitDelays.push_back(pDelayLog);
	}

	std::sort(vFitDelays.begin(), vFitDelays.end(), CompareRwyDelayLogData);		//sort data

	int nPosition = -1; 
	long lTotalDelay = 0l;
	long lAvailTime = 0l;
	long lEnterRwy =0l;

	int nLogCount = vFitDelays.size();
	RunwayDelayItem* pLogItem = NULL;
	for (int i = 0; i < nLogCount; i++)
	{
		AirsideFlightRunwayDelayLog* pDelayLog = vFitDelays.at(i);

		if (pDelayLog->mPosition != nPosition)
		{

			if ((pDelayLog->mPosition == FlightRunwayDelay::AtHoldShort || pDelayLog->mPosition == FlightRunwayDelay::UNKNOWNPOS)
				&& nPosition == (int)FlightRunwayDelay::InQueue)		//to calculate InQueue available time and delay time
			{
				lAvailTime = pDelayLog->time;
				lTotalDelay = lAvailTime - pLogItem->m_tStartTime.asSeconds();
			}

			if (pLogItem && pLogItem->m_PositionType == (int)FlightRunwayDelay::AtHoldShort && pDelayLog->mPosition == FlightRunwayDelay::UNKNOWNPOS)
			{
				lEnterRwy = lAvailTime;
				//the delay occurred at hold short
				if (pDelayLog->mAvailableTime <= lEnterRwy)		
					continue;
			}

			//the delay occurred in position
			if (pLogItem && pLogItem->m_PositionType == (int)FlightRunwayDelay::InPosition && pDelayLog->mPosition == FlightRunwayDelay::UNKNOWNPOS)	
			{
				lAvailTime = pDelayLog->mAvailableTime > lAvailTime? pDelayLog->mAvailableTime : lAvailTime;

				if (pDelayLog->time <= lEnterRwy)
					pDelayLog->mDelayTime = pDelayLog->mDelayTime - (lEnterRwy - pDelayLog->time);

				if (pDelayLog->mDelayTime > lAvailTime - pLogItem->m_tStartTime.asSeconds())
					pDelayLog->mDelayTime = lAvailTime - pLogItem->m_tStartTime.asSeconds();

				if (pDelayLog->mDelayTime >0l)
				{
					lTotalDelay += pDelayLog->mDelayTime;
					DETAILINFO info;
					info.first = pDelayLog->mDelayTime;
					info.second = pDelayLog->sReasonDetail.c_str();
					pLogItem->m_vReasonDetail.push_back(info);
				}
				continue;
			}

			//calculate the previous delay's operation time and delay time 
			if (pLogItem)		
			{
				long lStart = pLogItem->m_tStartTime.asSeconds();
				pLogItem->m_lTotalDelay = min(lTotalDelay, lAvailTime - lStart);
				pLogItem->m_lExpectOpTime = max(lAvailTime - lStart - lTotalDelay, 0l);
				pLogItem->m_lRealityOpTime = lAvailTime - lStart;
				std::sort(pLogItem->m_vReasonDetail.begin(),pLogItem->m_vReasonDetail.end(),CompareDelayInfoData);
				m_vDataItemList.push_back(pLogItem);
			}

			//update current position type
			if (pDelayLog->mPosition != FlightRunwayDelay::UNKNOWNPOS)
				nPosition = (int)pDelayLog->mPosition;
			else
				nPosition = FlightRunwayDelay::InPosition;

			//new delay type, and init the delay data
			pLogItem = new RunwayDelayItem;
			pLogItem->m_tStartTime= ElapsedTime(pDelayLog->time);
			if (pDelayLog->mMode == OnLanding)
			{
				pLogItem->m_strACName.Format("%s%s",fltdesc.sAirline.c_str(),fltdesc.sArrID.c_str());
				pLogItem->m_OperationType = 0;
			}
			else
			{
				pLogItem->m_strACName.Format("%s%s",fltdesc.sAirline.c_str(),fltdesc.sDepID.c_str());
				pLogItem->m_OperationType = 1;
			}
			pLogItem->m_nACID = fltdesc.id;
			pLogItem->m_nRwyID = pDelayLog->mAtRunway.resid;
			pLogItem->m_nRwyMark = (int)pDelayLog->mAtRunway.resdir;
			pLogItem->m_strRunwayMark = pDelayLog->mAtRunway.strRes.c_str();
			pLogItem->m_sACType =fltdesc.sAcType.c_str();
			pLogItem->mMode = pDelayLog->mMode;
			lAvailTime = pDelayLog->mAvailableTime;
			CString strTime;
			strTime = ElapsedTime(lAvailTime).printTime();
			lTotalDelay = pDelayLog->mDelayTime;
			pLogItem->m_PositionType = nPosition;

			if (pDelayLog->mDelayTime >0l)
			{
				DETAILINFO info;
				info.first = pDelayLog->mDelayTime;
				info.second = pDelayLog->sReasonDetail.c_str();
				pLogItem->m_vReasonDetail.push_back(info);	
			}
		}
		else			//same delay position
		{
	//		lAvailTime = pDelayLog->mAvailableTime > lAvailTime? pDelayLog->mAvailableTime : lAvailTime;
			
			long lDelayTime = 0;
			if (pDelayLog->mAvailableTime > lAvailTime)
			{
				DETAILINFO backInfo = pLogItem->m_vReasonDetail.back();
				pLogItem->m_vReasonDetail.pop_back();

				lDelayTime = pDelayLog->mAvailableTime - lAvailTime;
				lAvailTime = pDelayLog->mAvailableTime;

				long lOffsetTime = pDelayLog->mDelayTime - lDelayTime;

				backInfo.first -= lOffsetTime;
				pLogItem->m_vReasonDetail.push_back(backInfo);
			}

			if (pDelayLog->mDelayTime >0l)
			{
				//lTotalDelay += pDelayLog->mDelayTime;
				lDelayTime = pDelayLog->mDelayTime > lDelayTime ? lDelayTime : pDelayLog->mDelayTime;
				lTotalDelay += lDelayTime;
				DETAILINFO info;
				info.first = pDelayLog->mDelayTime;
				info.second = pDelayLog->sReasonDetail.c_str();
				pLogItem->m_vReasonDetail.push_back(info);
			}
		}
	}

	if (pLogItem)		//add the last data
	{
		long lStart = pLogItem->m_tStartTime.asSeconds();
		pLogItem->m_lTotalDelay = min(lTotalDelay, lAvailTime - lStart);
		pLogItem->m_lExpectOpTime = max(lAvailTime - lStart - lTotalDelay, 0l);
		pLogItem->m_lRealityOpTime = lAvailTime - lStart;
		std::sort(pLogItem->m_vReasonDetail.begin(),pLogItem->m_vReasonDetail.end(),CompareDelayInfoData);
		m_vDataItemList.push_back(pLogItem);
	}
}

void AirsideFlightRunwayDelayData::ClearData()
{
	int nCount = m_vDataItemList.size();
	for (int i = 0; i < nCount; i++)
	{
		RunwayDelayItem* pData = m_vDataItemList.at(i);

		delete pData;
		pData = NULL;
	}
	m_vDataItemList.clear();
}