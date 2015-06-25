#include "StdAfx.h"
#include ".\flightconflictreportdata.h"
#include "../Results/ARCBaseLog.h"
#include "../Results/AirsideFlightLogItem.h"
#include "../Results/ARCBaseLog.hpp"
#include "../Results/AirsideReportLog.h"
#include "Parameters.h"
#include "AirsideFlightConflictPara.h"
#include "LogFilterFun.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FlightConflictReportData::FlightConflictReportData(CParameters * parameter ,CBaseFilterFun* _filterFun)
{
	m_pParameter = parameter;
	if(_filterFun == NULL)
		m_filterFun = new CDefaultFilterFun(m_pParameter) ;
	else
		m_filterFun = _filterFun ;
}

FlightConflictReportData::~FlightConflictReportData(void)
{
	ClearData();
	if (m_filterFun)
	{
		delete m_filterFun;
		m_filterFun = NULL;
	}
}

FlightConflictReportData::ConflictDataItem::ConflictDataItem()
{

}

FlightConflictReportData::ConflictDataItem::~ConflictDataItem()
{

}

int FlightConflictReportData::ConflictDataItem::GetInIntervalIndex(CParameters* pParameter)
{
	ElapsedTime tTime = m_tTime - pParameter->getStartTime();
	long lTime = (long)tTime.asSeconds();
	int nIdx = lTime/pParameter->getInterval();

	if (lTime%pParameter->getInterval() > 0)
		nIdx +=1;

	return nIdx-1;
}

void FlightConflictReportData::LoadData(const CString& _csDescFileName,const CString& _csEventFileName)
{
	ARCBaseLog<AirsideFlightLogItem> mFlightLogData;
	mFlightLogData.OpenInput(_csDescFileName.GetString(), _csEventFileName.GetString());
	int nCount = mFlightLogData.getItemCount();
	for (int i = 0; i < nCount; i++)
	{
		//try
		//{
		mFlightLogData.LoadItem(mFlightLogData.ItemAt(i));
		AirsideFlightLogItem item = mFlightLogData.ItemAt(i);
		AddFlightConflict(item);
		//}
		//catch(StringError* pErr)
		//{
		//	AfxMessageBox("The airside log file is old, run simulation please!");
		//	delete pErr;		
		//	return;
		//}

	}

}

void FlightConflictReportData::AddFlightConflict(const AirsideFlightLogItem& item)
{		
	AirsideFlightLogDesc fltdesc = item.mFltDesc;

	int nCount = item.mpEventList->mEventList.size();
	for (int i = 0; i < nCount; i++)
	{
		ARCEventLog* pLog = item.mpEventList->mEventList.at(i);
		if(!m_filterFun->bFit(typeof(AirsideConflictionDelayLog), pLog))
			continue ;

		AirsideConflictionDelayLog* pConflictLog = (AirsideConflictionDelayLog*)pLog;

		if (pConflictLog->mDelayTime < 100L && pConflictLog->mAction != FlightConflict::REASSIGN)		//less than 1s, neglect
			continue;

		ConflictDataItem* pLogItem = new ConflictDataItem;

		pLogItem->fltdesc = item.mFltDesc;
		pLogItem->m_FlightID = fltdesc.id ;
		pLogItem->m_nMode = pConflictLog->mMode;

		pLogItem->m_tTime.setPrecisely(pConflictLog->time);
		if (pConflictLog->state ==  AirsideFlightEventLog::ARR)
		{
			pLogItem->m_strACName.Format("%s%s",fltdesc.sAirline.c_str(),fltdesc.sArrID.c_str());
			pLogItem->m_IsDep = FALSE ;
		}
		else
		{
			pLogItem->m_IsDep = TRUE ;
			pLogItem->m_strACName.Format("%s%s",fltdesc.sAirline.c_str(),fltdesc.sDepID.c_str());
		}
		pLogItem->m_nACID = fltdesc.id;
		pLogItem->m_n2ndPartyID = pConflictLog->motherMobileDesc.nMobileID;
		pLogItem->m_emDelayReson = pConflictLog->m_emFlightDelayReason;
		pLogItem->m_sDelayReason = pConflictLog->m_sDetailReason.c_str();
		pLogItem->m_str2ndPartyName.Format("%s",pConflictLog->motherMobileDesc.strName.c_str());
		if (pLogItem->m_str2ndPartyName == "")
		{
			pLogItem->m_str2ndPartyName = "Unknown";
		}

		pLogItem->m_nConflictType = pConflictLog->motherMobileDesc.mType == ARCMobileDesc::FLIGHT ? 0:1;
		pLogItem->m_tDelay.setPrecisely(pConflictLog->mDelayTime);
		pLogItem->m_nAreaID = pConflictLog->mAreaID;
		pLogItem->m_strAreaName.Format("%s",pConflictLog->sAreaName.c_str());
		pLogItem->m_nActionType = (int)pConflictLog->mAction;
		pLogItem->m_nLocationType = (int)pConflictLog->mConflictLocation;
		pLogItem->m_ResDis = pConflictLog->distInRes ;
		if (pConflictLog->mMode >= OnBirth && pConflictLog->mMode < OnExitRunway)
			pLogItem->m_nOperationType = (int)FlightConflict::STAR;
		else if (pConflictLog->mMode == OnExitRunway)
			pLogItem->m_nOperationType = (int)FlightConflict::EXITRUNWAY;
		else if (pConflictLog->mMode > OnExitRunway && pConflictLog->mMode <= OnTaxiToStand)
			pLogItem->m_nOperationType = (int)FlightConflict::TAXIINBOUND;
		else if (pConflictLog->mMode == OnEnterStand)
			pLogItem->m_nOperationType = (int)FlightConflict::ENTERINGSTAND;
		else if (pConflictLog->mMode == OnExitStand)
			pLogItem->m_nOperationType = (int)FlightConflict::PUSHBACK_POWEROUT;
		else if (pConflictLog->mMode == OnTaxiToRunway || pConflictLog->mMode == OnQueueToRunway || pConflictLog->mMode == OnTaxiToDeice)
			pLogItem->m_nOperationType = (int)FlightConflict::TAXIOUTBOUND;
		else if (pConflictLog->mMode == OnTowToDestination)
			pLogItem->m_nOperationType = (int)FlightConflict::UNDERTOWING;	
		else if (pConflictLog->mMode >= OnTakeOffWaitEnterRunway && pConflictLog->mMode <= OnCruiseDep)
			pLogItem->m_nOperationType = (int)FlightConflict::SID;
		else if (pConflictLog->mMode == OnCruiseThrough)
			pLogItem->m_nOperationType = (int)FlightConflict::ENROUTE;
		else
		{
//			ASSERT(0);
			pLogItem->m_nOperationType = (int)FlightConflict::UNKNOWNOP;	
		}

		pLogItem->m_ResourceDes = pConflictLog->mAtResource ;
		m_vDataItemList.push_back(pLogItem) ;
	}
}

bool ACCompare(FlightConflictReportData::ConflictDataItem* fItem,FlightConflictReportData::ConflictDataItem* sItem)
{
	if (fItem->m_nACID <= sItem->m_nACID)
		return true;

	return false;
}

bool AreaCompare(FlightConflictReportData::ConflictDataItem* fItem,FlightConflictReportData::ConflictDataItem* sItem)
{
	if (fItem->m_nAreaID <= sItem->m_nAreaID)
		return true;

	return false;
}

bool ActionTypeCompare(FlightConflictReportData::ConflictDataItem* fItem,FlightConflictReportData::ConflictDataItem* sItem)
{
	if (fItem->m_nActionType <= sItem->m_nActionType)
		return true;

	return false;
}

bool LocationTypeCompare(FlightConflictReportData::ConflictDataItem* fItem,FlightConflictReportData::ConflictDataItem* sItem)
{
	if (fItem->m_nLocationType <= sItem->m_nLocationType)
		return true;

	return false;
}

bool OperationTypeCompare(FlightConflictReportData::ConflictDataItem* fItem,FlightConflictReportData::ConflictDataItem* sItem)
{
	if (fItem->m_nOperationType <= sItem->m_nOperationType)
		return true;

	return false;
}

void FlightConflictReportData::SortItemsByAC()
{
	std::sort(m_vDataItemList.begin(), m_vDataItemList.end(),ACCompare);
}

void FlightConflictReportData::SortItemsByArea()
{
	std::sort(m_vDataItemList.begin(), m_vDataItemList.end(),AreaCompare);
}

void FlightConflictReportData::SortItemsByActionType()
{
	std::sort(m_vDataItemList.begin(), m_vDataItemList.end(),ActionTypeCompare);
}

void FlightConflictReportData::SortItemsByLocationType()
{
	std::sort(m_vDataItemList.begin(), m_vDataItemList.end(),LocationTypeCompare);
}

void FlightConflictReportData::SortItemsByOperationType()
{
	std::sort(m_vDataItemList.begin(), m_vDataItemList.end(),OperationTypeCompare);
}

void FlightConflictReportData::AddConflictItem( ConflictDataItem* pItem )
{
	m_vDataItemList.push_back(pItem);
}

void FlightConflictReportData::ClearData()
{
	int nCount = m_vDataItemList.size();

	for (int i = 0; i < nCount; i++)
	{
		ConflictDataItem* pData = m_vDataItemList.at(i);

		delete pData;
		pData = NULL;
	}
	m_vDataItemList.clear();

}
