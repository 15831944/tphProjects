#include "StdAfx.h"
#include ".\airsideflightdelayreport.h"
#include <limits>
#include "AirsideFlightDelayParam.h"
#include "DetailTotalDelayResult.h"
#include "DetailAirDelayResult.h"
#include "DetailTaxiDelayResult.h"
#include "DetailStandDelayResult.h"
#include "DetailServiceDelayResult.h"
#include "DetailTakeOffDelayResult.h"
#include "DetailSegmentDelayResult.h"
#include "DetailComponentDelayResult.h"
#include "CDetailFlightScheduleDelay.h"
#include "SummaryAirDelayResult.h"
#include "SummaryComponentDelayResult.h"
#include "SummarySegmentDelayResult.h"
#include "SummaryServiceDelayResult.h"
#include "SummaryStandDelayResult.h"
#include "SummaryTakeOffDelayResult.h"
#include "SummaryTaxiDelayResult.h"
#include "SummaryTotalDelayResult.h"
#include "CSummaryFlightScheduleDelay.h"
#include "LogFilterFun.h"
#include "../Results/AirsideFlightLogItem.h"


#include "../Engine/Airside/AirsideFlightDelay.h"

const char* CAirsideFlightDelayReport::subReportName[] = 
{
		_T("Schedule delay"),
		_T("Flight Total Delay"),
		_T("Segment Delay"),
		_T("Component Delay"),
		_T("Air Delay"),
		_T("Taxi Delay"),
		_T("Stand Delay"),
		_T("Service Delay"),
		_T("Take Off Delay"),
		_T("Detail Delay Count"),

		_T("Schedule delay"),
		_T("Flight Total Delay"),
		_T("Segment Delay"),
		_T("Component Delay"),
		_T("Air Delay"),
		_T("Taxi Delay"),
		_T("Stand Delay"),
		_T("Service Delay"),
		_T("Take Off Delay"),

};
CAirsideFlightDelayReport::CAirsideFlightDelayReport(CBGetLogFilePath pFunc)
:CAirsideBaseReport(pFunc)
,m_pResult(NULL)
{

}

CAirsideFlightDelayReport::~CAirsideFlightDelayReport(void)
{
	delete m_pResult;
}

bool CAirsideFlightDelayReport::GetAirsideFlightLogItem(AirsideFlightLogItem& item,AirsideFlightLogEntry logEntry,ARCBaseLog<AirsideFlightLogItem>& mFlightLogData)const
{
	int nFltLogCount = mFlightLogData.getItemCount();
	for (int i = 0; i < nFltLogCount; i++)
	{
		mFlightLogData.LoadItem(mFlightLogData.ItemAt(i));
		AirsideFlightLogItem curitem = mFlightLogData.ItemAt(i);
		if (curitem.mFltDesc.id == logEntry.GetAirsideDescStruct().id)
		{
			item = curitem;
			return true;
		}
	}
	return false;
}

void CAirsideFlightDelayReport::GenerateReport(CParameters * parameter)
{
	m_vResult.clear();
	m_vTotalResult.clear();
	//init result; using flight constraint
	size_t nFltConsCount = parameter->getFlightConstraintCount();
	for (size_t nfltCons =0; nfltCons < nFltConsCount; ++ nfltCons)
	{
		m_vResult.push_back(FltTypeDelayItem(parameter->getFlightConstraint(nfltCons)));
	}


	CAirsideFlightDelayParam *pParam = (CAirsideFlightDelayParam *)parameter;
	ASSERT(pParam != NULL);

	//read log
	CString strDescFilepath = (*m_pCBGetLogFilePath)(AirsideDescFile);
	CString strEventFilePath = (*m_pCBGetLogFilePath)(AirsideEventFile);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	CString strDelayDescPath = (*m_pCBGetLogFilePath)(AirsideFlightDelayFile);
	m_delayDescLog.LoadData(strDelayDescPath);

	EventLog<AirsideFlightEventStruct> airsideFlightEventLog;
	AirsideFlightLog fltLog;
	fltLog.SetEventLog(&airsideFlightEventLog);
	fltLog.LoadDataIfNecessary(strDescFilepath);
	fltLog.LoadEventDataIfNecessary(strEventFilePath);

	CString strDescConflictFilepath = (*m_pCBGetLogFilePath)(AirsideFlightDescFileReport);
	CString strEventConflictFilePath = (*m_pCBGetLogFilePath)(AirsideFlightEventFileReport);
	if (strDescConflictFilepath.IsEmpty() || strEventConflictFilePath.IsEmpty())
		return;

	CBaseFilterFun* pFilterFun = new CBaseFilterFun;

	FlightConflictReportData ReportConflictData(pParam,pFilterFun);
	ReportConflictData.LoadData(strDescConflictFilepath,strEventConflictFilePath);

	ARCBaseLog<AirsideFlightLogItem> mFlightLogData;
	mFlightLogData.OpenInput(strDescConflictFilepath, strEventConflictFilePath);

	int nFltLogCount = fltLog.getCount();
	CProgressBar bar(_T("Generate flight delay report..."),100,nFltLogCount,TRUE);
	for (int i =0; i < nFltLogCount; ++i)
	{

		long lDelayTime = 0L;

		AirsideFlightLogEntry logEntry;
		logEntry.SetEventLog(&airsideFlightEventLog);
		fltLog.getItem(logEntry,i);	
		AirsideFlightLogItem item;
		if (!GetAirsideFlightLogItem(item,logEntry,mFlightLogData))
			continue;
		
		HandleFlightLog(logEntry,item,ReportConflictData,pParam);
		bar.StepIt();
	}

}

long CAirsideFlightDelayReport::getHeldAtStandTime(AirsideFlightLogEntry& logEntry)const
{
	int nCount = logEntry.getCount();
	for (int nPos = 0; nPos < nCount; nPos++)
	{
		AirsideFlightEventStruct event = logEntry.getEvent(nPos);
		if (event.mode == OnHeldAtStand)
		{
			return event.time;
		}
	}
	return 0L;
}

bool CAirsideFlightDelayReport::setArrAirModeContent(FltDelayItem& fltArrivalDelayItem,FltNodeDelayItem& nodeDelayItem,long lDelayTime,long lAirTime,const FlightConflictReportData::ConflictDataItem* pData)const
{
	if ((pData->m_nMode >= OnBirth && pData->m_nMode < OnExitRunway) || (pData->m_nMode == OnWaitInHold && (long)pData->m_tTime <= lAirTime ))
	{
		fltArrivalDelayItem.totalDelayTime += lDelayTime;
		nodeDelayItem.eArriveTime = (long)pData->m_tTime - fltArrivalDelayItem.totalDelayTime;

		nodeDelayItem.nSegment = FltDelaySegment_Air;
		fltArrivalDelayItem.airDelayTime += lDelayTime;


		fltArrivalDelayItem.vNodeDelay.push_back(nodeDelayItem);

		return true;
	}
	return false;
}

bool CAirsideFlightDelayReport::setDepAirModeContent(FltDelayItem& fltDepartureDelayItem,FltNodeDelayItem& nodeDelayItem,long lDelayTime,long lAirTime,const FlightConflictReportData::ConflictDataItem* pData)const
{
	if ((pData->m_nMode > OnClimbout && pData->m_nMode < OnTerminate) || (pData->m_nMode == OnWaitInHold && (long)pData->m_tTime > lAirTime))
	{
		fltDepartureDelayItem.totalDelayTime += (long)lDelayTime;
		nodeDelayItem.eArriveTime = (long)pData->m_tTime - fltDepartureDelayItem.totalDelayTime;

		nodeDelayItem.nSegment = FltDelaySegment_Air;
		fltDepartureDelayItem.airDelayTime += (long)lDelayTime;

		fltDepartureDelayItem.vNodeDelay.push_back(nodeDelayItem);

		return true;
	}

	return false;
}

bool CAirsideFlightDelayReport::setArrTaxiwayModeContent(FltDelayItem& fltArrivalDelayItem,FltNodeDelayItem& nodeDelayItem,long lDelayTime,const FlightConflictReportData::ConflictDataItem* pData)const
{
	if (pData->m_nMode == OnTaxiToStand || pData->m_nMode == OnTaxiToHoldArea)
	{
		fltArrivalDelayItem.totalDelayTime += lDelayTime;
		nodeDelayItem.eArriveTime = (long)pData->m_tTime - fltArrivalDelayItem.totalDelayTime;

		nodeDelayItem.nSegment = FltDelaySegment_Taxi;

		fltArrivalDelayItem.taxiDelayTime += lDelayTime;


		fltArrivalDelayItem.vNodeDelay.push_back(nodeDelayItem);

		return true;
	}
	return false;
}

bool CAirsideFlightDelayReport::setDepTaxiwayModeContent(FltDelayItem& fltDepartureDelayItem,FltNodeDelayItem& nodeDelayItem,long lDelayTime,const FlightConflictReportData::ConflictDataItem* pData)const
{
	if (pData->m_nMode == OnTaxiToRunway || pData->m_nMode == OnQueueToRunway || pData->m_nMode == OnTowToDestination \
		|| pData->m_nMode == OnTakeOffWaitEnterRunway)
	{
		fltDepartureDelayItem.totalDelayTime += (long)lDelayTime;
		nodeDelayItem.eArriveTime = (long)pData->m_tTime - fltDepartureDelayItem.totalDelayTime;

		nodeDelayItem.nSegment = FltDelaySegment_Taxi;
		fltDepartureDelayItem.taxiDelayTime += (long)lDelayTime;

		fltDepartureDelayItem.vNodeDelay.push_back(nodeDelayItem);
		return true;
	}
	return false;
}

bool CAirsideFlightDelayReport::setArrStandModeContent(FltDelayItem& fltArrivalDelayItem,FltNodeDelayItem& nodeDelayItem,long lDelayTime,const FlightConflictReportData::ConflictDataItem* pData)const
{
	if (pData->m_nMode == OnEnterStand)
	{
		fltArrivalDelayItem.totalDelayTime += lDelayTime;
		nodeDelayItem.eArriveTime = (long)pData->m_tTime - fltArrivalDelayItem.totalDelayTime;

		nodeDelayItem.nSegment = FltDelaySegment_Stand;
		fltArrivalDelayItem.standDelayTime += lDelayTime;

		fltArrivalDelayItem.vNodeDelay.push_back(nodeDelayItem);
		return true;
	}
	return false;
}

bool CAirsideFlightDelayReport::setDepStandModeContent(FltDelayItem& fltDepartureDelayItem,FltNodeDelayItem& nodeDelayItem,long lDelayTime,const FlightConflictReportData::ConflictDataItem* pData)const
{
	if (pData->m_nMode == OnExitStand)
	{
		fltDepartureDelayItem.totalDelayTime += lDelayTime;
		nodeDelayItem.eArriveTime = (long)pData->m_tTime - fltDepartureDelayItem.totalDelayTime;

		nodeDelayItem.nSegment = FltDelaySegment_Stand;
		fltDepartureDelayItem.standDelayTime += lDelayTime;

		fltDepartureDelayItem.vNodeDelay.push_back(nodeDelayItem);

		return true;
	}

	return false;
}


bool CAirsideFlightDelayReport::setArrServiceModeContent(FltDelayItem& fltArrivalDelayItem,FltNodeDelayItem& nodeDelayItem,long lDelayTime,const FlightConflictReportData::ConflictDataItem* pData)const
{
	if (pData->m_nMode == OnHeldAtStand || pData->m_nMode == OnTaxiToDeice)
	{
		fltArrivalDelayItem.totalDelayTime += (long)lDelayTime;
		nodeDelayItem.eArriveTime = (long)pData->m_tTime - fltArrivalDelayItem.totalDelayTime;

		nodeDelayItem.nSegment = FltDelaySegment_Service;
		fltArrivalDelayItem.serviceDelayTime += (long)lDelayTime;

		fltArrivalDelayItem.vNodeDelay.push_back(nodeDelayItem);
		return true;
	}
	return false;
}


bool CAirsideFlightDelayReport::setDepTakeoffModeContent(FltDelayItem& fltDepartureDelayItem,FltNodeDelayItem& nodeDelayItem,long lDelayTime,const FlightConflictReportData::ConflictDataItem* pData)const
{
	if (pData->m_nMode >= OnTakeOffEnterRunway && pData->m_nMode <= OnClimbout)
	{
		fltDepartureDelayItem.totalDelayTime += (long)lDelayTime;
		nodeDelayItem.eArriveTime = (long)pData->m_tTime - fltDepartureDelayItem.totalDelayTime;

		nodeDelayItem.nSegment = FltDelaySegment_TakeOff;
		fltDepartureDelayItem.takeoffDelayTime += (long)lDelayTime;

		fltDepartureDelayItem.vNodeDelay.push_back(nodeDelayItem);
		return true;
	}
	return false;
}


class EqualValue
{
public:
	EqualValue(int nValue)
		:m_nID(nValue)
	{
		
	}

	bool operator() (const FlightConflictReportData::ConflictDataItem* pData)
	{
		return (pData->fltdesc.id != m_nID);
	}

private:
	int m_nID;
};


void CAirsideFlightDelayReport::HandleFlightLog(AirsideFlightLogEntry& logEntry,AirsideFlightLogItem& item,const FlightConflictReportData& ReportConflictData,CAirsideFlightDelayParam * parameter)
{		
	AirsideFlightDescStruct	fltDesc = logEntry.GetAirsideDescStruct();

	if (IsValidTime(fltDesc,parameter) == false)
		return;

	FLTCNSTR_MODE fltConMode;
	if (IsFitConstraint(fltDesc,parameter,fltConMode) == false)
		return;

	long lDelayTime = 0;
	int nStartPos = 0;
	int nEndPos = 0;

	std::vector<FlightConflictReportData::ConflictDataItem*>vConflictData;
	vConflictData.clear();
	vConflictData.assign(ReportConflictData.m_vDataItemList.size(),NULL);

	std::remove_copy_if(ReportConflictData.m_vDataItemList.begin(),ReportConflictData.m_vDataItemList.end(),vConflictData.begin(),EqualValue(fltDesc.id));
	std::vector<FlightConflictReportData::ConflictDataItem*>::iterator iter = std::remove(vConflictData.begin(),vConflictData.end(),(FlightConflictReportData::ConflictDataItem*)NULL);
	vConflictData.erase(iter,vConflictData.end());

	if (IsPassObject(logEntry,parameter,nStartPos,nEndPos,lDelayTime) == false )
	 	return;

	setConflightBelongPosition(logEntry,nStartPos,nEndPos,vConflictData);
	AirsideFlightDescStruct airsideFlightDescStruct = logEntry.GetAirsideDesc();

	//arrival item
	FltDelayItem fltArrivalDelayItem(m_AirportDB);
	fltArrivalDelayItem.fltDesc = logEntry.GetAirsideDesc();
	ZeroMemory(fltArrivalDelayItem.fltDesc.depID, sizeof(fltArrivalDelayItem.fltDesc.depID));

	//departure item
	FltDelayItem fltDepartureDelayItem(m_AirportDB);
	fltDepartureDelayItem.fltDesc = logEntry.GetAirsideDesc();
	ZeroMemory(fltDepartureDelayItem.fltDesc.arrID, sizeof(fltDepartureDelayItem.fltDesc.arrID));
	std::vector<int> DelayLogIndexDB ;
	int nCount = (int)vConflictData.size();
	long tHeldAtStand = getHeldAtStandTime(logEntry);

	//arrival item
	fltArrivalDelayItem.planSt = item.mFltDesc.schArrTime;
	fltArrivalDelayItem.ataTime = item.mFltDesc.actAtaTime;

	//departure item
	fltDepartureDelayItem.planSt = item.mFltDesc.schDepTime;
	fltDepartureDelayItem.atdTime = item.mFltDesc.actAtdTime;
	for (int nPos = 0; nPos < nCount; ++nPos)
	{
		FlightConflictReportData::ConflictDataItem* pData = vConflictData.at(nPos);

		if (pData->fltdesc.id != airsideFlightDescStruct.id)
			continue;

		if (pData->m_tTime < parameter->getStartTime() || pData->m_tTime> parameter->getEndTime())//not in the time range
			continue;

		FltNodeDelayItem nodeDelayItem;

		long dDelayTime = 0L;
		dDelayTime += (long)pData->m_tDelay;
		nodeDelayItem.eActArriveTime = pData->m_tTime;
		nodeDelayItem.delayTime = (long)pData->m_tDelay;
		nodeDelayItem.m_strResName = pData->m_ResourceDes.strRes.c_str();
		nodeDelayItem.m_strDetailReason = pData->m_sDelayReason;
		nodeDelayItem.nReason = pData->m_emDelayReson;

		//arrival 
		//arrival air
		setArrAirModeContent(fltArrivalDelayItem,nodeDelayItem,dDelayTime,tHeldAtStand,pData);
		//arrival taxi
		setArrTaxiwayModeContent(fltArrivalDelayItem,nodeDelayItem,dDelayTime,pData);
		//arrival stand
		setArrStandModeContent(fltArrivalDelayItem,nodeDelayItem,dDelayTime,pData);
		//service
		setArrServiceModeContent(fltArrivalDelayItem,nodeDelayItem,dDelayTime,pData);

		//departure
		//departure stand
		setDepStandModeContent(fltDepartureDelayItem,nodeDelayItem,dDelayTime,pData);
		//departure taxi
		setDepTaxiwayModeContent(fltDepartureDelayItem,nodeDelayItem,dDelayTime,pData);
		//departure takeoff
		setDepTakeoffModeContent(fltDepartureDelayItem,nodeDelayItem,dDelayTime,pData);
		//departure air
		setDepAirModeContent(fltDepartureDelayItem,nodeDelayItem,dDelayTime,tHeldAtStand,pData);
		
	}

	
	//arrival
	SetStartAndEndNode(logEntry,parameter,&fltArrivalDelayItem);
	fltArrivalDelayItem.bValidData = calculateStartAndEndTime(true,fltArrivalDelayItem,logEntry);
	if (strlen(fltArrivalDelayItem.fltDesc.arrID) > 0 && fltConMode != ENUM_FLTCNSTR_MODE_DEP)
	{
	//	if(fltArrivalDelayItem.vNodeDelay.size() > 0) //only add the item which has delay
		
		//check the arrival part is in the report time range or not
		if(fltArrivalDelayItem.actStartTime > parameter->getEndTime().getPrecisely() ||
            fltArrivalDelayItem.actEndTime < parameter->getStartTime().getPrecisely())
		{
			//this item is not in the time range
		}
		else
		{
			fltArrivalDelayItem.bArrOrDeplDelay = true;
			AddToResult(fltArrivalDelayItem);
		}
	}
	//departure
	SetStartAndEndNode(logEntry,parameter,&fltDepartureDelayItem);
	fltDepartureDelayItem.bValidData = calculateStartAndEndTime(false,fltDepartureDelayItem,logEntry);
	if (strlen(fltDepartureDelayItem.fltDesc.depID) > 0 && fltConMode != ENUM_FLTCNSTR_MODE_ARR)
	{
	//	if(fltDepartureDelayItem.vNodeDelay.size() >0)//only add the item to result list which has delay
		if(fltDepartureDelayItem.actStartTime > parameter->getEndTime().getPrecisely() ||
			fltDepartureDelayItem.actEndTime < parameter->getStartTime().getPrecisely())
		{
			//this item is not in the time range
		}
		else
		{
			fltDepartureDelayItem.bArrOrDeplDelay = false;
			AddToResult(fltDepartureDelayItem);
		}
	}
}

bool CAirsideFlightDelayReport::IsValidTime(AirsideFlightDescStruct& fltDesc,CAirsideFlightDelayParam * parameter)
{

	//time valid
	if (fltDesc.startTime == 0 && fltDesc.endTime == 0)
		return false;

	if(parameter->getStartTime() >= parameter->getEndTime())
		return false;

	if(fltDesc.startTime >= fltDesc.endTime)
		return false;

	if (fltDesc.endTime < parameter->getStartTime().asSeconds()*100 || fltDesc.startTime> parameter->getEndTime().asSeconds()*100)//not in the time range
		return false;

	return true;
}
bool CAirsideFlightDelayReport::IsFitConstraint(AirsideFlightDescStruct& fltDesc,CAirsideFlightDelayParam * parameter,FLTCNSTR_MODE& fltConMode)
{	
	bool bArrFit = false;
	bool bDepFit = false;

	std::vector<FltTypeDelayItem>::iterator iter = m_vResult.begin();
	for (;iter!=m_vResult.end(); ++iter)
	{
		FlightConstraint& fltCons = (*iter).m_fltCons;
		if(fltCons.fits(fltDesc))
		{
			switch (fltCons.GetFltConstraintMode())
			{
			case ENUM_FLTCNSTR_MODE_ALL:
				{
					fltConMode = ENUM_FLTCNSTR_MODE_ALL;
					return true;
				}
				break;
			case ENUM_FLTCNSTR_MODE_ARR:
				{
					bArrFit = true;
				}
				break;
			case ENUM_FLTCNSTR_MODE_DEP:
				{
					bDepFit = true;
				}
				break;
			default:
				{
					// 					ASSERT(FALSE);
				}
				break;
			}
			if (bArrFit && bDepFit)
			{
				fltConMode = ENUM_FLTCNSTR_MODE_ALL;
				return true;
			}
		
		}
	}

	if (bArrFit)
	{
		fltConMode = ENUM_FLTCNSTR_MODE_ARR;
		return true;
	}
	if (bDepFit)
	{
		fltConMode = ENUM_FLTCNSTR_MODE_DEP;
		return true;
	}

	return bArrFit || bDepFit;
}

bool CAirsideFlightDelayReport::SetStartAndEndNode(AirsideFlightLogEntry& logEntry,CAirsideFlightDelayParam* pParam,FltDelayItem* pFltItem)
{
	int nEventCount = logEntry.getCount();

	CAirsideReportNode startNode = pParam->GetStartObject();
	CAirsideReportNode endNode = pParam->GetEndObject();

	if (startNode.IsDefault())
	{
		for (int nEvent = 0; nEvent < nEventCount; ++nEvent)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(nEvent);
			if (event.nodeNO != -1)
			{
				pFltItem->m_startNode.SetNodeID(event.nodeNO);
				pFltItem->m_startNode.SetNodeType(CAirsideReportNode::ReportNodeType_WayPointStand);
				break;
			}
			if (event.intNodeID != -1)
			{
				pFltItem->m_startNode.SetNodeID(event.intNodeID);
				pFltItem->m_startNode.SetNodeType(CAirsideReportNode::ReportNodeType_Intersection);
				break;
			}
		}
	}

	if (endNode.IsDefault())
	{
		for (int nEvent = nEventCount-1; nEvent >= 0; --nEvent)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(nEvent);
			if (event.nodeNO != -1)
			{
				pFltItem->m_endNode.SetNodeID(event.nodeNO);
				pFltItem->m_endNode.SetNodeType(CAirsideReportNode::ReportNodeType_WayPointStand);
				break;
			}
			if (event.intNodeID != -1)
			{
				pFltItem->m_endNode.SetNodeID(event.intNodeID);
				pFltItem->m_endNode.SetNodeType(CAirsideReportNode::ReportNodeType_Intersection);
				break;
			}
		}
	}

	for (int nEvent = 0; nEvent < nEventCount; ++nEvent)
	{
		AirsideFlightEventStruct event = logEntry.getEvent(nEvent);
		if (event.mode == OnHeldAtStand)
		{
			pFltItem->m_midNode.SetNodeID(event.nodeNO);
			pFltItem->m_midNode.SetNodeType(CAirsideReportNode::ReportNodeType_WayPointStand);
		}
	}
	return true;
}

bool CAirsideFlightDelayReport::calculateStartAndEndTime(bool Arrival,FltDelayItem& fltDelayItem,AirsideFlightLogEntry& logEntry)
{
	int nEventCount = logEntry.getCount();
	if (nEventCount == 0)
		return false;
	
	std::vector<AirsideFlightEventStruct> vEventList;
	for (int nEvent = 0; nEvent < nEventCount; ++nEvent)
	{
		AirsideFlightEventStruct& event = logEntry.getEvent(nEvent);
		if (event.mode == OnHeldAtStand)
		{
			vEventList.push_back(event);
		}
	}

	if (vEventList.empty())
		return false;
	
	//calculate flight schedule time delay
	AirsideFlightEventStruct& frontEvent = logEntry.getEvent(0);
	AirsideFlightEventStruct& backEvent = logEntry.getEvent(nEventCount-1);
	if (Arrival)
	{
		fltDelayItem.actStartTime = frontEvent.time;
	}
	else
	{
		fltDelayItem.actEndTime = backEvent.time;
	}

	if (Arrival)
	{
		fltDelayItem.actEndTime = vEventList.front().time;
	}
	else
	{
		fltDelayItem.actStartTime = vEventList.back().time;
	}
	return true;
}

bool CAirsideFlightDelayReport::setConflightBelongPosition(AirsideFlightLogEntry& logEntry,int startpos,int endpos,std::vector<FlightConflictReportData::ConflictDataItem*>& vConflictData)
{
	int nEventCount = logEntry.getCount();
	int nConflictCount = (int)vConflictData.size();
	std::vector<FlightConflictReportData::ConflictDataItem*>vTempData;
	vTempData.clear();
	for (int nPos = 0; nPos < nConflictCount; ++nPos)
	{
		FlightConflictReportData::ConflictDataItem* pData = vConflictData.at(nPos);
		for (int nEvent = startpos; nEvent < endpos; ++nEvent)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(nEvent);
			if (pData->m_nMode == event.mode)
			{
				vTempData.push_back(pData);
				break;
			}
		}
	}
	vConflictData.clear();
	vConflictData.insert(vConflictData.begin(),vTempData.begin(),vTempData.end());
	return true;
}

//return start log item and end item
bool CAirsideFlightDelayReport::IsPassObject(AirsideFlightLogEntry& logEntry,CAirsideFlightDelayParam * pParam,int& startpos,int& endpos,long& delayTime)
{
	int nEventCount = logEntry.getCount();

	CAirsideReportNode startNode = pParam->GetStartObject();
	CAirsideReportNode endNode = pParam->GetEndObject();

	//default
	if (startNode.IsDefault() && endNode.IsDefault())
	{
		delayTime = 0;
		startpos = 0;
		endpos = nEventCount;
		return true;
	}
	else if (startNode.IsDefault())
	{
		for (int nEvent = 0; nEvent < nEventCount; ++nEvent)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(nEvent);
			if ((pParam->GetEndObject().GetNodeType() == CAirsideReportNode::ReportNodeType_WayPointStand && pParam->GetEndObject().IncludeObj(event.nodeNO))||
				(pParam->GetEndObject().GetNodeType() == CAirsideReportNode::ReportNodeType_Intersection&&pParam->GetEndObject().IncludeObj(event.intNodeID))||
				(pParam->GetEndObject().GetNodeType() == CAirsideReportNode::ReportNodeType_Taxiway&&pParam->GetEndObject().IncludeObj(event.intNodeID)))
			{
				startpos = 0;
				endpos = nEvent;
				return true;
			}
		}
	}
	else if ( endNode.IsDefault())
	{
		for (int nEvent = 0; nEvent < nEventCount; ++nEvent)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(nEvent);
			if ((pParam->GetStartObject().GetNodeType() == CAirsideReportNode::ReportNodeType_WayPointStand && pParam->GetStartObject().IncludeObj(event.nodeNO))||
				(pParam->GetStartObject().GetNodeType() == CAirsideReportNode::ReportNodeType_WayPointStand && event.mode == OnBirth)||
				(pParam->GetStartObject().GetNodeType() == CAirsideReportNode::ReportNodeType_Intersection&&pParam->GetStartObject().IncludeObj(event.intNodeID))||
				(pParam->GetStartObject().GetNodeType() == CAirsideReportNode::ReportNodeType_Taxiway&&pParam->GetStartObject().IncludeObj(event.intNodeID)))

			{
				delayTime += event.delayTime;
				startpos = nEvent;
				endpos = nEventCount;
				return true;
			}
		}

	}
	else
	{
		//find start pos
		bool  bFindStartPos = false;
		for (int nStartEvent = 0; nStartEvent < nEventCount; ++nStartEvent)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(nStartEvent);
			if ((pParam->GetStartObject().GetNodeType() == CAirsideReportNode::ReportNodeType_WayPointStand && pParam->GetStartObject().IncludeObj(event.nodeNO))||
				(pParam->GetStartObject().GetNodeType() == CAirsideReportNode::ReportNodeType_WayPointStand && event.mode == OnBirth)||
				(pParam->GetStartObject().GetNodeType() == CAirsideReportNode::ReportNodeType_Intersection&&pParam->GetStartObject().IncludeObj(event.intNodeID))||
				(pParam->GetStartObject().GetNodeType() == CAirsideReportNode::ReportNodeType_Taxiway&&pParam->GetStartObject().IncludeObj(event.intNodeID)))
			{
				delayTime += event.delayTime;
				startpos = nStartEvent;
				bFindStartPos = true;
				break;
			}
		}

		if (bFindStartPos == false)
			return false;

		//find endpos
		bool  bFindEndPos = false;
		for (int nEndEvent = 0; nEndEvent < nEventCount; ++nEndEvent)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(nEndEvent);
			if ((pParam->GetEndObject().GetNodeType() == CAirsideReportNode::ReportNodeType_WayPointStand && pParam->GetEndObject().IncludeObj(event.nodeNO))||
				(pParam->GetEndObject().GetNodeType() == CAirsideReportNode::ReportNodeType_Intersection&&pParam->GetEndObject().IncludeObj(event.intNodeID))||
				(pParam->GetEndObject().GetNodeType() == CAirsideReportNode::ReportNodeType_Taxiway&&pParam->GetEndObject().IncludeObj(event.intNodeID)))
			{
				endpos = nEndEvent;
				bFindEndPos = true;
				break;
			}
		}

		if (bFindEndPos == false)
			return false;


		return true;

	}

	return false;

}


void CAirsideFlightDelayReport::InitResult()
{

}
void CAirsideFlightDelayReport::AddToResult(FltDelayItem& item)
{
	bool bFits = false;
	std::vector<FltTypeDelayItem>::iterator iter = m_vResult.begin();
	for (;iter!=m_vResult.end(); ++iter)
	{
		if((*iter).m_fltCons.fits(item.fltDesc))
		{
			item.m_fltCons = (*iter).m_fltCons;

			(*iter).m_vDelayData.push_back(item);
			bFits = true;
		}
	}

	if (bFits)
		m_vTotalResult.push_back(item);
}

void CAirsideFlightDelayReport::InitResultListHead(CXListCtrl& cxListCtrl, CParameters * parameter,CSortableHeaderCtrl* piSHC)
{
	CAirsideFlightDelayParam *pParam = (CAirsideFlightDelayParam *)parameter;
	ASSERT(pParam != NULL);

	switch(pParam->getSubType())
	{
	case SRT_DETAIL_FLIGHTTOTALDELAY:
	case SRT_DETAIL_SCHEDULEDELAY:
		InitListHead(cxListCtrl,ASReportType_Detail,piSHC);
		break;

	case SRT_DETAIL_AIRDELAY:
	case SRT_DETAIL_TAXIDELAY:
	case SRT_DETAIL_STANDDELAY:
	case SRT_DETAIL_SERVICEDELAY:
	case SRT_DETAIL_TAKOFFDELAY:
	case SRT_DETAIL_SEGMENTDELAY:
	case SRT_DETAIL_COMPONENTDELAY:
		InitDetailListHead(cxListCtrl,pParam,piSHC);
		break;

	case SRT_SUMMARY_FLIGHTTOTALDELAY:
	case SRT_SUMMARY_SEGMENTDELAY:
	case SRT_SUMMARY_COMPONENTDELAY:
		{
			cxListCtrl.DeleteAllItems();

			while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
				cxListCtrl.DeleteColumn(0);
			cxListCtrl.InsertColumn(0, _T("Flight Type"), LVCFMT_LEFT, 100);

			//total delay
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
				//total delay
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
	case SRT_SUMMARY_AIRDELAY:
	case SRT_SUMMARY_TAXIDELAY:
	case SRT_SUMMARY_STANDDELAY:
	case SRT_SUMMARY_SERVICEDELAY:
	case SRT_SUMMARY_TAKOFFDELAY:
	case SRT_SUMMARY_SCHEDULEDELAY:
		InitSummaryListHead(cxListCtrl,pParam,piSHC);
		break;

	default:
		break;
	}
}

void CAirsideFlightDelayReport::InitDetailListHead(CXListCtrl& cxListCtrl, CParameters * parameter,CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0,_T("Flight"),LVCFMT_LEFT,140,-1);
	cxListCtrl.InsertColumn(1,_T("Expected time"),LVCFMT_LEFT,100,-1);
	cxListCtrl.InsertColumn(2,_T("Actual time"),LVCFMT_LEFT,100,-1);
	cxListCtrl.InsertColumn(3,_T("Node"),LVCFMT_LEFT,140,-1);
	cxListCtrl.InsertColumn(4,_T("Delay"),LVCFMT_LEFT,60,-1);
	cxListCtrl.InsertColumn(5,_T("Delay segment"),LVCFMT_LEFT,60,-1);
	cxListCtrl.InsertColumn(6,_T("Reason"),LVCFMT_LEFT,80,-1);
	cxListCtrl.InsertColumn(7,_T("Detail"),LVCFMT_LEFT,80,-1);

	if (piSHC)
	{
		piSHC->ResetAll();
		piSHC->SetDataType(0,dtSTRING);
		piSHC->SetDataType(1,dtTIME);
		piSHC->SetDataType(2,dtTIME);
		piSHC->SetDataType(3,dtSTRING);
		piSHC->SetDataType(4,dtSTRING);
		piSHC->SetDataType(5,dtSTRING);
		piSHC->SetDataType(6,dtSTRING);
		piSHC->SetDataType(7,dtSTRING);
		piSHC->SetDataType(8,dtSTRING);
	}
}

void CAirsideFlightDelayReport::InitSummaryListHead(CXListCtrl& cxListCtrl, CParameters * parameter,CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	bool bFit = true;
	CAirsideFlightDelayParam *pParam = (CAirsideFlightDelayParam *)parameter;
	switch (pParam->getSubType())
	{
	case SRT_SUMMARY_AIRDELAY:
		{
			cxListCtrl.InsertColumn(0, _T("Flight"), LVCFMT_LEFT, 100);
			//air delay
			cxListCtrl.InsertColumn(1, _T("Air Delay Min(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(2, _T("Air Delay Max(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(3, _T("Air Delay Mean(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(4, _T("Air Delay Q1(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(5, _T("Air Delay Q2(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(6, _T("Air Delay Q3(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(7, _T("Air Delay P1(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(8, _T("Air Delay P5(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(9, _T("Air Delay P10(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(10, _T("Air Delay P90(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(11, _T("Air Delay P95(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(12, _T("Air Delay P99(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(13, _T("Air Delay Std dev(mins)"), LVCFMT_LEFT, 100);
		}
		break;
	case SRT_SUMMARY_TAXIDELAY:
		{
			cxListCtrl.InsertColumn(0, _T("Flight"), LVCFMT_LEFT, 100);
			//Taxi delay
			cxListCtrl.InsertColumn(1, _T("Taxi Delay Min(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(2, _T("Taxi Delay Max(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(3, _T("Taxi Delay Mean(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(4, _T("Taxi Delay Q1(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(5, _T("Taxi Delay Q2(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(6, _T("Taxi Delay Q3(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(7, _T("Taxi Delay P1(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(8, _T("Taxi Delay P5(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(9, _T("Taxi Delay P10(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(10, _T("Taxi Delay P90(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(11, _T("Taxi Delay P95(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(12, _T("Taxi Delay P99(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(13, _T("Taxi Delay Std dev(mins)"), LVCFMT_LEFT, 100);
		}
		break;
	case SRT_SUMMARY_STANDDELAY:
		{
			cxListCtrl.InsertColumn(0, _T("Flight"), LVCFMT_LEFT, 100);
			//Stand delay
			cxListCtrl.InsertColumn(1, _T("Stand Delay Min(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(2, _T("Stand Delay Max(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(3, _T("Stand Delay Mean(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(4, _T("Stand Delay Q1(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(5, _T("Stand Delay Q2(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(6, _T("Stand Delay Q3(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(7, _T("Stand Delay P1(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(8, _T("Stand Delay P5(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(9, _T("Stand Delay P10(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(10, _T("Stand Delay P90(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(11, _T("Stand Delay P95(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(12, _T("Stand Delay P99(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(13, _T("Stand Delay Std dev(mins)"), LVCFMT_LEFT, 100);
		}
		break;
	case SRT_SUMMARY_SERVICEDELAY:
		{
			cxListCtrl.InsertColumn(0, _T("Flight"), LVCFMT_LEFT, 100);
			//Service delay
			cxListCtrl.InsertColumn(1, _T("Service Delay Min(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(2, _T("Service Delay Max(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(3, _T("Service Delay Mean(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(4, _T("Service Delay Q1(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(5, _T("Service Delay Q2(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(6, _T("Service Delay Q3(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(7, _T("Service Delay P1(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(8, _T("Service Delay P5(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(9, _T("Service Delay P10(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(10, _T("Service Delay P90(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(11, _T("Service Delay P95(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(12, _T("Service Delay P99(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(13, _T("Service Delay Std dev(mins)"), LVCFMT_LEFT, 100);
		}
		break;
	case SRT_SUMMARY_TAKOFFDELAY:
		{
			cxListCtrl.InsertColumn(0, _T("Flight"), LVCFMT_LEFT, 100);
			//schedule delay
			cxListCtrl.InsertColumn(1, _T("Takeoff Delay Min(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(2, _T("Takeoff Delay Max(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(3, _T("Takeoff Delay Mean(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(4, _T("Takeoff Delay Q1(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(5, _T("Takeoff Delay Q2(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(6, _T("Takeoff Delay Q3(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(7, _T("Takeoff Delay P1(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(8, _T("Takeoff Delay P5(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(9, _T("Takeoff Delay P10(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(10, _T("Takeoff Delay P90(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(11, _T("Takeoff Delay P95(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(12, _T("Takeoff Delay P99(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(13, _T("Takeoff Delay Std dev(mins)"), LVCFMT_LEFT, 100);
		}
		break;
	case SRT_SUMMARY_SCHEDULEDELAY:
		{
			cxListCtrl.InsertColumn(0, _T("Flight"), LVCFMT_LEFT, 100);
			//Takeoff delay
			cxListCtrl.InsertColumn(1, _T("Schedule Delay Min(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(2, _T("Schedule Delay Max(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(3, _T("Schedule Delay Mean(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(4, _T("Schedule Delay Q1(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(5, _T("Schedule Delay Q2(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(6, _T("Schedule Delay Q3(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(7, _T("Schedule Delay P1(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(8, _T("Schedule Delay P5(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(9, _T("Schedule Delay P10(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(10, _T("Schedule Delay P90(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(11, _T("Schedule Delay P95(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(12, _T("Schedule Delay P99(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(13, _T("Schedule Delay Std dev(mins)"), LVCFMT_LEFT, 100);
		}
		break;
	default:
		{
			bFit = false;
		}
		break;
	}

	if (true == bFit)
	{
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
	}
}

bool CAirsideFlightDelayReport::fit(int nType,CParameters* parameter)const
{
	CAirsideFlightDelayParam *pParam = (CAirsideFlightDelayParam *)parameter;
	ASSERT(pParam != NULL);

	switch (pParam->getSubType())
	{
	case SRT_DETAIL_COMPONENTDELAY:
	case SRT_DETAIL_SEGMENTDELAY:
		return true;

	case SRT_DETAIL_AIRDELAY:
		if (nType == CAirsideFlightDelayReport::FltDelaySegment_Air)
		{
			return true;
		}
		break;

	case SRT_DETAIL_TAXIDELAY:
		if (nType == CAirsideFlightDelayReport::FltDelaySegment_Taxi)
		{
			return true;
		}
		break;
	case SRT_DETAIL_STANDDELAY:
		if (nType == CAirsideFlightDelayReport::FltDelaySegment_Stand)
		{
			return true;
		}
		break;
	case SRT_DETAIL_SERVICEDELAY:
		if (nType == CAirsideFlightDelayReport::FltDelaySegment_Service)
		{
			return true;
		}
		break;
	case SRT_DETAIL_TAKOFFDELAY:
		if (nType == CAirsideFlightDelayReport::FltDelaySegment_TakeOff)
		{
			return true;
		}
		break;
	}

	return false;
}

//fill component and segment list ctrl 
void CAirsideFlightDelayReport::FillSummaryTypeListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	CAirsideFlightDelayParam *pParam = (CAirsideFlightDelayParam *)parameter;
	ASSERT(pParam != NULL);
	switch (pParam->getSubType())
	{
	case SRT_SUMMARY_AIRDELAY:
		FillAirDelaySummary(cxListCtrl,parameter);
		break;
	case SRT_SUMMARY_TAXIDELAY:
		FillTaxiDelaySummary(cxListCtrl,parameter);
		break;
	case SRT_SUMMARY_STANDDELAY:
		FillStandDelaySummary(cxListCtrl,parameter);
		break;
	case SRT_SUMMARY_SERVICEDELAY:
		FillServiceDelaySummary(cxListCtrl,parameter);
		break;
	case SRT_SUMMARY_TAKOFFDELAY:
		FillTakeoffDelaySummary(cxListCtrl,parameter);
		break;
	default:
		break;
	}
}


//fill summary air delay list ctrl
void CAirsideFlightDelayReport::FillAirDelaySummary(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	ASSERT(parameter);
	CAirsideFlightDelayParam* pAirsideFlightDelayParam = (CAirsideFlightDelayParam*)parameter;
	ASSERT(pAirsideFlightDelayParam);

	CSummaryAirDelayResult         summaryAirDelayResult;
	summaryAirDelayResult.GenerateResult(m_vResult, parameter);
	vector<CSummaryFlightTypeData>  airSummaryData;
	summaryAirDelayResult.GetSummaryResult(airSummaryData);

	for (int i=0; i<(int)airSummaryData.size(); i++)
	{
		//flight type
		CString strFlightType;
		airSummaryData[i].m_fltConstraint.screenPrint(strFlightType.GetBuffer(1024));
		cxListCtrl.InsertItem(i, strFlightType);

		//air delay
		//Min Delay(mins)
		cxListCtrl.SetItemText(i, 1,airSummaryData[i].m_summaryData.m_estMin.printTime());
		//Max Delay(mins)
		cxListCtrl.SetItemText(i, 2,airSummaryData[i].m_summaryData.m_estMax.printTime());
		//Mean Delay(mins)
		cxListCtrl.SetItemText(i, 3,airSummaryData[i].m_summaryData.m_estAverage.printTime());
		//Q1(mins)
		cxListCtrl.SetItemText(i, 4,airSummaryData[i].m_summaryData.m_estQ1.printTime());
		//Q2(mins)
		cxListCtrl.SetItemText(i, 5,airSummaryData[i].m_summaryData.m_estQ2.printTime());
		//Q3(mins)
		cxListCtrl.SetItemText(i, 6,airSummaryData[i].m_summaryData.m_estQ3.printTime());
		//P1(mins)
		cxListCtrl.SetItemText(i, 7,airSummaryData[i].m_summaryData.m_estP1.printTime());
		//P5(mins)
		cxListCtrl.SetItemText(i, 8,airSummaryData[i].m_summaryData.m_estP5.printTime());
		//P10(mins)
		cxListCtrl.SetItemText(i, 9,airSummaryData[i].m_summaryData.m_estP10.printTime());
		//P90(mins)
		cxListCtrl.SetItemText(i, 10,airSummaryData[i].m_summaryData.m_estP90.printTime());
		//P95(mins)
		cxListCtrl.SetItemText(i, 11,airSummaryData[i].m_summaryData.m_estP95.printTime());
		//P99(mins)
		cxListCtrl.SetItemText(i, 12,airSummaryData[i].m_summaryData.m_estP99.printTime());
		//Std dev(mins)
		cxListCtrl.SetItemText(i, 13,airSummaryData[i].m_summaryData.m_estSigma.printTime());
	}
}

//fill summary taxi delay list ctrl
void CAirsideFlightDelayReport::FillTaxiDelaySummary(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	CSummaryTaxiDelayResult  summaryTaxiDelayResult;
	summaryTaxiDelayResult.GenerateResult(m_vResult, parameter);
	vector<CSummaryFlightTypeData>  taxiSummaryData;
	summaryTaxiDelayResult.GetSummaryResult(taxiSummaryData);

	for (int i=0; i<(int)taxiSummaryData.size(); i++)
	{
		//flight type
		CString strFlightType;
		taxiSummaryData[i].m_fltConstraint.screenPrint(strFlightType.GetBuffer(1024));
		cxListCtrl.InsertItem(i, strFlightType);

		//Taxi delay
		//Min Delay(mins)
		cxListCtrl.SetItemText(i, 1,taxiSummaryData[i].m_summaryData.m_estMin.printTime());
		//Max Delay(mins)
		cxListCtrl.SetItemText(i, 2,taxiSummaryData[i].m_summaryData.m_estMax.printTime());
		//Mean Delay(mins)
		cxListCtrl.SetItemText(i, 3,taxiSummaryData[i].m_summaryData.m_estAverage.printTime());
		//Q1(mins)
		cxListCtrl.SetItemText(i, 4,taxiSummaryData[i].m_summaryData.m_estQ1.printTime());
		//Q2(mins)
		cxListCtrl.SetItemText(i, 5,taxiSummaryData[i].m_summaryData.m_estQ2.printTime());
		//Q3(mins)
		cxListCtrl.SetItemText(i, 6,taxiSummaryData[i].m_summaryData.m_estQ3.printTime());
		//P1(mins)
		cxListCtrl.SetItemText(i, 7,taxiSummaryData[i].m_summaryData.m_estP1.printTime());
		//P5(mins)
		cxListCtrl.SetItemText(i, 8,taxiSummaryData[i].m_summaryData.m_estP5.printTime());
		//P10(mins)
		cxListCtrl.SetItemText(i, 9,taxiSummaryData[i].m_summaryData.m_estP10.printTime());
		//P90(mins)
		cxListCtrl.SetItemText(i, 10,taxiSummaryData[i].m_summaryData.m_estP90.printTime());
		//P95(mins)
		cxListCtrl.SetItemText(i, 11,taxiSummaryData[i].m_summaryData.m_estP95.printTime());
		//P99(mins)
		cxListCtrl.SetItemText(i, 12,taxiSummaryData[i].m_summaryData.m_estP99.printTime());
		//Std dev(mins)
		cxListCtrl.SetItemText(i, 13,taxiSummaryData[i].m_summaryData.m_estSigma.printTime());
	}
}

//fill summary stand delay list ctrl
void CAirsideFlightDelayReport::FillStandDelaySummary(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	CSummaryStandDelayResult  summaryStandDelayResult;
	summaryStandDelayResult.GenerateResult(m_vResult, parameter);
	vector<CSummaryFlightTypeData>  standSummaryData;
	summaryStandDelayResult.GetSummaryResult(standSummaryData);
	for (int i=0; i<(int)standSummaryData.size(); i++)
	{
		//flight type
		CString strFlightType;
		standSummaryData[i].m_fltConstraint.screenPrint(strFlightType.GetBuffer(1024));
		cxListCtrl.InsertItem(i, strFlightType);

		//Stand delay
		//Min Delay(mins)
		cxListCtrl.SetItemText(i, 1,standSummaryData[i].m_summaryData.m_estMin.printTime());
		//Max Delay(mins)
		cxListCtrl.SetItemText(i, 2,standSummaryData[i].m_summaryData.m_estMax.printTime());
		//Mean Delay(mins)
		cxListCtrl.SetItemText(i, 3,standSummaryData[i].m_summaryData.m_estAverage.printTime());
		//Q1(mins)
		cxListCtrl.SetItemText(i, 4,standSummaryData[i].m_summaryData.m_estQ1.printTime());
		//Q2(mins)
		cxListCtrl.SetItemText(i, 5,standSummaryData[i].m_summaryData.m_estQ2.printTime());
		//Q3(mins)
		cxListCtrl.SetItemText(i, 6,standSummaryData[i].m_summaryData.m_estQ3.printTime());
		//P1(mins)
		cxListCtrl.SetItemText(i, 7,standSummaryData[i].m_summaryData.m_estP1.printTime());
		//P5(mins)
		cxListCtrl.SetItemText(i, 8,standSummaryData[i].m_summaryData.m_estP5.printTime());
		//P10(mins)
		cxListCtrl.SetItemText(i, 9,standSummaryData[i].m_summaryData.m_estP10.printTime());
		//P90(mins)
		cxListCtrl.SetItemText(i, 10,standSummaryData[i].m_summaryData.m_estP90.printTime());
		//P95(mins)
		cxListCtrl.SetItemText(i, 11,standSummaryData[i].m_summaryData.m_estP95.printTime());
		//P99(mins)
		cxListCtrl.SetItemText(i, 12,standSummaryData[i].m_summaryData.m_estP99.printTime());
		//Std dev(mins)
		cxListCtrl.SetItemText(i, 13,standSummaryData[i].m_summaryData.m_estSigma.printTime());
	}
}

//fill summary service delay list ctrl
void CAirsideFlightDelayReport::FillServiceDelaySummary(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	CSummaryServiceDelayResult  summaryServiceDelayResult;
	summaryServiceDelayResult.GenerateResult(m_vResult, parameter);
	vector<CSummaryFlightTypeData>  serviceSummaryData;
	summaryServiceDelayResult.GetSummaryResult(serviceSummaryData);
	for (int i=0; i<(int)serviceSummaryData.size(); i++)
	{
		//flight type
		CString strFlightType;
		serviceSummaryData[i].m_fltConstraint.screenPrint(strFlightType.GetBuffer(1024));
		cxListCtrl.InsertItem(i, strFlightType);

		//Service delay
		//Min Delay(mins)
		cxListCtrl.SetItemText(i, 1,serviceSummaryData[i].m_summaryData.m_estMin.printTime());
		//Max Delay(mins)
		cxListCtrl.SetItemText(i, 2,serviceSummaryData[i].m_summaryData.m_estMax.printTime());
		//Mean Delay(mins)
		cxListCtrl.SetItemText(i, 3,serviceSummaryData[i].m_summaryData.m_estAverage.printTime());
		//Q1(mins)
		cxListCtrl.SetItemText(i, 4,serviceSummaryData[i].m_summaryData.m_estQ1.printTime());
		//Q2(mins)
		cxListCtrl.SetItemText(i, 5,serviceSummaryData[i].m_summaryData.m_estQ2.printTime());
		//Q3(mins)
		cxListCtrl.SetItemText(i, 6,serviceSummaryData[i].m_summaryData.m_estQ3.printTime());
		//P1(mins)
		cxListCtrl.SetItemText(i, 7,serviceSummaryData[i].m_summaryData.m_estP1.printTime());
		//P5(mins)
		cxListCtrl.SetItemText(i, 8,serviceSummaryData[i].m_summaryData.m_estP5.printTime());
		//P10(mins)
		cxListCtrl.SetItemText(i, 9,serviceSummaryData[i].m_summaryData.m_estP10.printTime());
		//P90(mins)
		cxListCtrl.SetItemText(i, 10,serviceSummaryData[i].m_summaryData.m_estP90.printTime());
		//P95(mins)
		cxListCtrl.SetItemText(i, 11,serviceSummaryData[i].m_summaryData.m_estP95.printTime());
		//P99(mins)
		cxListCtrl.SetItemText(i, 12,serviceSummaryData[i].m_summaryData.m_estP99.printTime());
		//Std dev(mins)
		cxListCtrl.SetItemText(i, 13,serviceSummaryData[i].m_summaryData.m_estSigma.printTime());
	}
}

//fill summary take off delay list ctrl
void CAirsideFlightDelayReport::FillTakeoffDelaySummary(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	CSummaryTakeOffDelayResult  summaryTakeoffDelayResult;
	summaryTakeoffDelayResult.GenerateResult(m_vResult, parameter);
	vector<CSummaryFlightTypeData>  takeoffSummaryData;
	summaryTakeoffDelayResult.GetSummaryResult(takeoffSummaryData);
	for (int i=0; i<(int)takeoffSummaryData.size(); i++)
	{
		//flight type
		CString strFlightType;
		takeoffSummaryData[i].m_fltConstraint.screenPrint(strFlightType.GetBuffer(1024));
		cxListCtrl.InsertItem(i, strFlightType);
		//Takeoff delay
		//Min Delay(mins)
		cxListCtrl.SetItemText(i, 1,takeoffSummaryData[i].m_summaryData.m_estMin.printTime());
		//Max Delay(mins)
		cxListCtrl.SetItemText(i, 2,takeoffSummaryData[i].m_summaryData.m_estMax.printTime());
		//Mean Delay(mins)
		cxListCtrl.SetItemText(i, 3,takeoffSummaryData[i].m_summaryData.m_estAverage.printTime());
		//Q1(mins)
		cxListCtrl.SetItemText(i, 4,takeoffSummaryData[i].m_summaryData.m_estQ1.printTime());
		//Q2(mins)
		cxListCtrl.SetItemText(i, 5,takeoffSummaryData[i].m_summaryData.m_estQ2.printTime());
		//Q3(mins)
		cxListCtrl.SetItemText(i, 6,takeoffSummaryData[i].m_summaryData.m_estQ3.printTime());
		//P1(mins)
		cxListCtrl.SetItemText(i, 7,takeoffSummaryData[i].m_summaryData.m_estP1.printTime());
		//P5(mins)
		cxListCtrl.SetItemText(i, 8,takeoffSummaryData[i].m_summaryData.m_estP5.printTime());
		//P10(mins)
		cxListCtrl.SetItemText(i, 9,takeoffSummaryData[i].m_summaryData.m_estP10.printTime());
		//P90(mins)
		cxListCtrl.SetItemText(i, 10,takeoffSummaryData[i].m_summaryData.m_estP90.printTime());
		//P95(mins)
		cxListCtrl.SetItemText(i, 11,takeoffSummaryData[i].m_summaryData.m_estP95.printTime());
		//P99(mins)
		cxListCtrl.SetItemText(i, 12,takeoffSummaryData[i].m_summaryData.m_estP99.printTime());
		//Std dev(mins)
		cxListCtrl.SetItemText(i, 13,takeoffSummaryData[i].m_summaryData.m_estSigma.printTime());
	}
}

void CAirsideFlightDelayReport::FillDetailTypeListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	int nShowRows = 0;
	for (size_t i =0; i < m_vTotalResult.size(); ++i )
	{
		FltDelayItem& fltDelayItem = m_vTotalResult[i];

		for(int j=0; j<(int)fltDelayItem.vNodeDelay.size(); j++)
		{
			CAirsideFlightDelayReport::FltNodeDelayItem nodeDelayItem = fltDelayItem.vNodeDelay[j];

			if(nodeDelayItem.delayTime == 0L)
				continue;

			if (fit(nodeDelayItem.nSegment,parameter) == false)
				continue;

			//flight
			CString strFlight(_T(""));
			if (fltDelayItem.bArrOrDeplDelay)
			{
				strFlight.Format("%s%s", fltDelayItem.fltDesc._airline.GetValue(), fltDelayItem.fltDesc.arrID);
			}
			else
			{
				strFlight.Format("%s%s", fltDelayItem.fltDesc._airline.GetValue(), fltDelayItem.fltDesc.depID);
			}
			cxListCtrl.InsertItem(nShowRows,strFlight);
			
			CString strText;
			ElapsedTime estTime(long(nodeDelayItem.eArriveTime/100.0+0.5));
			//expect time
			strText.Format(_T("Day%d %02d:%02d:%02d"), estTime.GetDay(), estTime.GetHour(), estTime.GetMinute(), estTime.GetSecond());
			cxListCtrl.SetItemText(nShowRows,1,strText);

			//actual time
			ElapsedTime estActualTime(long(nodeDelayItem.eActArriveTime/100.0+0.5));
			strText.Format(_T("Day%d %02d:%02d:%02d"), estActualTime.GetDay(), estActualTime.GetHour(), estActualTime.GetMinute(), estActualTime.GetSecond());
			cxListCtrl.SetItemText(nShowRows,2,strText);

			//node
			cxListCtrl.SetItemText(nShowRows,3,nodeDelayItem.m_strResName);

			//delay
			ElapsedTime estDelayTime(long(nodeDelayItem.delayTime/100.0+0.5));
			cxListCtrl.SetItemText(nShowRows,4,estDelayTime.printTime());

			//delay segment
			switch(nodeDelayItem.nSegment)
			{
			case FltDelaySegment_Air:
				strText.Format(_T("Air"));
				break;
			case FltDelaySegment_Taxi:
				strText.Format(_T("Taxi"));
				break;
			case FltDelaySegment_Stand:
				strText.Format(_T("Stand"));
				break;
			case FltDelaySegment_Service:
				strText.Format(_T("Service"));
				break;
			case FltDelaySegment_TakeOff:
				strText.Format(_T("Take off"));
				break;

			default:
				strText.Format(_T("Unknow"));
				break;
			}
			cxListCtrl.SetItemText(nShowRows,5,strText);

			//reason
			switch(nodeDelayItem.nReason)
			{
			case FltDelayReason_Slowed:
				strText.Format(_T("Slowed"));
				break;

			case FltDelayReason_Vectored:
				strText.Format(_T("Vectored"));
				break;

			case FltDelayReason_AirHold:
				strText.Format(_T("Hold"));
				break;

			case FltDelayReason_AltitudeChanged:
				strText.Format(_T("Altitude changed"));
				break;

			case FltDelayReason_SideStep:
				strText.Format(_T("Side step"));
				break;

			case FltDelayReason_Stop:
				strText.Format(_T("Stop"));
				break;

			case FltDelayReason_Service:
				strText.Format(_T("Service"));
				break;
			
			case FltDelayReason_RunwayHold:
				strText.Format(_T("Runway hold"));
				break;
			default:
				strText.Format(_T("Unknow"));
				break;
			}
			cxListCtrl.SetItemText(nShowRows,6,strText);

			//detail

			cxListCtrl.SetItemText(nShowRows,7,nodeDelayItem.m_strDetailReason);

			nShowRows += 1;
		}
	}
	
}

void CAirsideFlightDelayReport::FillResultListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	CAirsideFlightDelayParam *pParam = (CAirsideFlightDelayParam *)parameter;
	ASSERT(pParam != NULL);

	switch(pParam->getSubType())
	{
	case SRT_DETAIL_FLIGHTTOTALDELAY:
	case SRT_DETAIL_SCHEDULEDELAY:
		FillDetailListContent(cxListCtrl,pParam);
		break;

	case SRT_DETAIL_AIRDELAY:
	case SRT_DETAIL_TAXIDELAY:
	case SRT_DETAIL_STANDDELAY:
	case SRT_DETAIL_SERVICEDELAY:
	case SRT_DETAIL_TAKOFFDELAY:
	case SRT_DETAIL_SEGMENTDELAY:
	case SRT_DETAIL_COMPONENTDELAY:
		FillDetailTypeListContent(cxListCtrl,pParam);
		break;

	case SRT_SUMMARY_FLIGHTTOTALDELAY:
		case SRT_SUMMARY_SEGMENTDELAY:
	case SRT_SUMMARY_COMPONENTDELAY:
		FillSummaryListContent(cxListCtrl,pParam);
		break;
	case SRT_SUMMARY_SCHEDULEDELAY:
		FillSummaryScheduleListContent(cxListCtrl,pParam);
		break;
	case SRT_SUMMARY_AIRDELAY:
	case SRT_SUMMARY_TAXIDELAY:
	case SRT_SUMMARY_STANDDELAY:
	case SRT_SUMMARY_SERVICEDELAY:
	case SRT_SUMMARY_TAKOFFDELAY:
		FillSummaryTypeListContent(cxListCtrl,pParam);
		break;

	default:
		break;
	}
}

void CAirsideFlightDelayReport::RefreshReport(CParameters * parameter)
{
	CAirsideFlightDelayParam *pParam = (CAirsideFlightDelayParam *)parameter;
	ASSERT(pParam != NULL);

	//if (m_pResult != NULL && m_pResult->getType() ==pParam->getSubType() )
	delete m_pResult;


	switch(pParam->getSubType())
	{
	case SRT_DETAIL_FLIGHTTOTALDELAY:
		m_pResult = new CDetailTotalDelayResult;
		break;

	case SRT_DETAIL_AIRDELAY:
		m_pResult   = new CDetailAirDelayResult;
		break;

	case SRT_DETAIL_TAXIDELAY:
		m_pResult  = new CDetailTaxiDelayResult;
		break;

	case SRT_DETAIL_STANDDELAY:
		m_pResult = new CDetailStandDelayResult;
		break;

	case SRT_DETAIL_SERVICEDELAY:
		m_pResult = new CDetailServiceDelayResult;
		break;

	case SRT_DETAIL_TAKOFFDELAY:
		m_pResult = new CDetailTakeOffDelayResult;
		break;

	case SRT_DETAIL_SEGMENTDELAY:
		m_pResult = new CDetailSegmentDelayResult;
		break;

	case SRT_DETAIL_COMPONENTDELAY:
		m_pResult = new CDetailComponentDelayResult;
		break;

	case SRT_DETAIL_SCHEDULEDELAY:
		m_pResult = new CDetailFlightScheduleDelayResult;
		break;

	case SRT_SUMMARY_FLIGHTTOTALDELAY:
		m_pResult = new CSummaryTotalDelayResult;
		break;

	case SRT_SUMMARY_SEGMENTDELAY:
		m_pResult = new CSummarySegmentDelayResult;
		break;

	case SRT_SUMMARY_COMPONENTDELAY:
		m_pResult = new CSummaryComponentDelayResult;
		break;

	case SRT_SUMMARY_AIRDELAY:
		m_pResult = new CSummaryAirDelayResult;
		break;

	case SRT_SUMMARY_TAXIDELAY:
		m_pResult = new CSummaryTaxiDelayResult;
		break;

	case SRT_SUMMARY_STANDDELAY:
		m_pResult = new CSummaryStandDelayResult;
		break;

	case SRT_SUMMARY_SERVICEDELAY:
		m_pResult = new CSummaryServiceDelayResult;
		break;

	case SRT_SUMMARY_TAKOFFDELAY:
		m_pResult = new CSummaryTakeOffDelayResult;
		break;

	case SRT_SUMMARY_SCHEDULEDELAY:
		m_pResult = new CSummaryFlightScheduleDelayResult;
		break;

	default:
		break;
	}


	if (m_pResult != NULL)
		m_pResult->GenerateResult(m_vResult,parameter);
}

//init default list ctrl header
void CAirsideFlightDelayReport::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC)
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
			cxListCtrl.InsertColumn(3, _T("Entry pos"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(4, _T("Exit pos"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(5, _T("STD/STA"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(6, _T("SMTD/SMTA"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(7, _T("SIM SCH DELAY(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(8, _T("ATD/ATA"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(9, _T("ACT SCH DELAY(mins)"), LVCFMT_LEFT, 100);

			cxListCtrl.InsertColumn(10, _T("T start(plan)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(11, _T("T end(plan)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(12, _T("T start(act)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(13, _T("T end(act)"), LVCFMT_LEFT, 100);
			
			cxListCtrl.InsertColumn(14, _T("Total Delay(mins)"), LVCFMT_LEFT, 100);

			if (piSHC)
			{
				piSHC->ResetAll();
				piSHC->SetDataType(0,dtSTRING);
				piSHC->SetDataType(1,dtSTRING);
				piSHC->SetDataType(2,dtSTRING);
				piSHC->SetDataType(3,dtSTRING);
				piSHC->SetDataType(4,dtSTRING);
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

	case ASReportType_Summary:
		{
			cxListCtrl.InsertColumn(0, _T("Flight"), LVCFMT_LEFT, 100);
			//Takeoff delay
			cxListCtrl.InsertColumn(1, _T("Schedule Delay Min(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(2, _T("Schedule Delay Max(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(3, _T("Schedule Delay Mean(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(4, _T("Schedule Delay Q1(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(5, _T("Schedule Delay Q2(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(6, _T("Schedule Delay Q3(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(7, _T("Schedule Delay P1(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(8, _T("Schedule Delay P5(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(9, _T("Schedule Delay P10(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(10, _T("Schedule Delay P90(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(11, _T("Schedule Delay P95(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(12, _T("Schedule Delay P99(mins)"), LVCFMT_LEFT, 100);
			cxListCtrl.InsertColumn(13, _T("Schedule Delay Std dev(mins)"), LVCFMT_LEFT, 100);

			if (piSHC)
			{
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
			}
			
		}
		break;

	default:
		ASSERT(false);
		break;
	}
}

void CAirsideFlightDelayReport::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	ASSERT(parameter);

	switch(parameter->getReportType())
	{
	case ASReportType_Detail:
		FillDetailListContent(cxListCtrl, parameter);
		break;
	case ASReportType_Summary:
		FillSummaryScheduleListContent(cxListCtrl, parameter);
		break;
	default:
		ASSERT(false);
		break;
	}
}
BOOL CAirsideFlightDelayReport::ExportListData(ArctermFile& _file,CParameters * parameter)
{
	return CAirsideBaseReport::ExportListData(_file, parameter);
	//if(parameter == NULL)
	//	return FALSE ;

	//switch(parameter->getReportType())
	//{
	//case ASReportType_Detail:
	//	ExportListDetail(_file, parameter);
	//	break;
	//case ASReportType_Summary:
	//	ExportListSummary(_file, parameter);
	//	break;
	//default:
	//	ASSERT(false);
	//	break;
	//}
	//return TRUE ;
}
void CAirsideFlightDelayReport::ExportListDetail(ArctermFile& _file,CParameters * parameter)
{
	ASSERT(parameter);
	CAirsideFlightDelayParam* pAirsideFlightDelayParam = (CAirsideFlightDelayParam*)parameter;
	ASSERT(pAirsideFlightDelayParam);

	int nRowCount = 0;
	for (size_t i =0; i < m_vTotalResult.size(); ++i )
	{
		FltDelayItem& delayItem = m_vTotalResult[i];

		//arrival
		if (delayItem.bArrOrDeplDelay)
		{
			//flight
			CString strFlight(_T(""));
			strFlight.Format("%s%s", delayItem.fltDesc._airline.GetValue(), delayItem.fltDesc.arrID);
			_file.writeField(strFlight);

			//flight type
			CString strFlightType;
			delayItem.m_fltCons.screenPrint(strFlightType.GetBuffer(1024));
			_file.writeField(strFlightType);

			//Operation
			_file.writeField( _T("A"));

			//Start
			CString strStart(_T("Start"));
			CAirsideReportNode airsideReportStartNode = pAirsideFlightDelayParam->GetStartObject();
			strStart.Format(_T("%s"), airsideReportStartNode.GetNodeName());
			_file.writeField(strStart);

			//end
			CString strEnd(_T("End"));
			CAirsideReportNode airsideReportEndNode = pAirsideFlightDelayParam->GetEndObject();
			strEnd.Format(_T("%s"), airsideReportEndNode.GetNodeName());
			_file.writeField( strEnd);

			//STD and STA
			CString strStand;
			long lplanSta = 0;
			lplanSta = delayItem.planSt;
			ElapsedTime estPlanSta(long(lplanSta/100.0+0.5));
			strStand.Format(_T("Day%d %02d:%02d:%02d"), estPlanSta.GetDay(), estPlanSta.GetHour(), estPlanSta.GetMinute(), estPlanSta.GetSecond());
			_file.writeField( strStand);

			{
				//SMTD and SMTA
				CString strSmta;
				long lSmta = delayItem.actEndTime;
				ElapsedTime estSmta(long(lSmta/100.0+0.5));
				strSmta.Format(_T("Day%d %02d:%02d:%02d"), estSmta.GetDay(), estSmta.GetHour(), estSmta.GetMinute(), estSmta.GetSecond());
				_file.writeField( strSmta);

				//Sim sch delay
				CString strSimSchDelay;

				long lSimDelay = 0l;
				if (delayItem.actEndTime)
				{
					lSimDelay = (delayItem.actEndTime - delayItem.planSt);
				}
				
				ElapsedTime estSimDelay(long(lSimDelay/100.0+0.5));
				strSimSchDelay.Format(_T("%d"), estSimDelay.asMinutes());
				_file.writeField(strSimSchDelay);
				
			}
			
			{
				
				//ATD and ATA
				CString strActualStand;
				long lActualTime = 0;
				lActualTime = delayItem.ataTime;
				if (lActualTime >= 0)
				{
					ElapsedTime estStartActual(long(lActualTime/100.0+0.5));
					strActualStand.Format(_T("Day%d %02d:%02d:%02d"), estStartActual.GetDay(), estStartActual.GetHour(), estStartActual.GetMinute(), estStartActual.GetSecond());
				}
				_file.writeField( strActualStand);
				
				//schedule delay
				CString strScheduleDelay;
				if (delayItem.ataTime >= 0)
				{
					long lDelay = (delayItem.ataTime - delayItem.planSt);
					ElapsedTime estSchDelay(long(lDelay/100.0+0.5));
					strScheduleDelay.Format(_T("%d"), estSchDelay.asMinutes());
				}
				_file.writeField( strScheduleDelay);
			}
			


			//T Start(plan)
			long lStartTime = 0;
			lStartTime = delayItem.actStartTime;
			ElapsedTime estStartPlan(long(lStartTime/100.0+0.5));
			CString strStartPlan(_T(""));
			strStartPlan.Format(_T("Day%d %02d:%02d:%02d"), estStartPlan.GetDay(), estStartPlan.GetHour(), estStartPlan.GetMinute(), estStartPlan.GetSecond());

			_file.writeField(strStartPlan);

			//T end (plan)
			long lEndTime = 0;
			if (delayItem.actEndTime)
			{
				lEndTime = delayItem.actEndTime - delayItem.totalDelayTime;
			}
			ElapsedTime estEndPlan(long(lEndTime/100.0+0.5));
			CString strEndPlan(_T(""));
			strEndPlan.Format(_T("Day%d %02d:%02d:%02d"), estEndPlan.GetDay(), estEndPlan.GetHour(), estEndPlan.GetMinute(), estEndPlan.GetSecond());

			_file.writeField(strEndPlan);

			//T Start(act)
			long lStartActTime = 0;
			lStartActTime = delayItem.actStartTime;
			ElapsedTime estStartActPlan(long(lStartActTime/100.0+0.5));
			CString strStartActPlan(_T(""));
			strStartActPlan.Format(_T("Day%d %02d:%02d:%02d"), estStartActPlan.GetDay(), estStartActPlan.GetHour(), estStartActPlan.GetMinute(), estStartActPlan.GetSecond());

			_file.writeField(strStartActPlan);

			//T end(act)
			long lEndActTime = 0;
			lEndActTime = delayItem.actEndTime;
			ElapsedTime estEndActPlan(long(lEndActTime/100.0+0.5));
			CString strEndActPlan(_T(""));
			strEndActPlan.Format(_T("Day%d %02d:%02d:%02d"), estEndActPlan.GetDay(), estEndActPlan.GetHour(), estEndActPlan.GetMinute(), estEndActPlan.GetSecond());

			_file.writeField( strEndActPlan);

			//Total delay(mins)
			ElapsedTime estTotalDelayTime(long(m_vTotalResult[i].totalDelayTime/100.0+0.5));
			CString strTotalDelayTime;
			strTotalDelayTime.Format("%s", estTotalDelayTime.printTime());
			_file.writeField(strTotalDelayTime);

			//add one row
			nRowCount++;
		}

		//depature
		else
		{
			//flight
			CString strFlight(_T(""));
			strFlight.Format("%s%s", m_vTotalResult[i].fltDesc._airline.GetValue(), m_vTotalResult[i].fltDesc.depID);
			_file.writeField( strFlight);

			//flight type
			CString strFlightType;
			m_vTotalResult[i].m_fltCons.screenPrint(strFlightType.GetBuffer(1024));
			_file.writeField(strFlightType);

			//Operation
			_file.writeField( _T("D"));

			//Start
			CString strStart(_T("Start"));
			CAirsideReportNode airsideReportStartNode = pAirsideFlightDelayParam->GetStartObject();
			strStart.Format(_T("%s"),airsideReportStartNode.GetNodeName());
			_file.writeField(strStart);

			//end
			CString strEnd(_T("End"));
			CAirsideReportNode airsideReportEndNode = pAirsideFlightDelayParam->GetEndObject();
			strEnd.Format(_T("%s"),airsideReportEndNode.GetNodeName());
			_file.writeField( strEnd);

			//STD and STA
			CString strStand;
			long lplanStd = 0;
			lplanStd = delayItem.planSt;
			ElapsedTime estPlanStd(long(lplanStd/100.0+0.5));
			strStand.Format(_T("Day%d %02d:%02d:%02d"), estPlanStd.GetDay(), estPlanStd.GetHour(), estPlanStd.GetMinute(), estPlanStd.GetSecond());
			_file.writeField( strStand);

			{
				//SMTD and SMTA
				CString strSmtd;
				long lSmtd = delayItem.actStartTime;
				ElapsedTime estSmtd(LONG(lSmtd/100.0+0.5));
				strSmtd.Format(_T("Day%d %02d:%02d:%02d"), estSmtd.GetDay(), estSmtd.GetHour(), estSmtd.GetMinute(), estSmtd.GetSecond());
				_file.writeField( strSmtd);

				//Sim sch delay
				//Total delay(mins)
				CString strSimSchDelay;
				long lSimDelay = 0;
				if (delayItem.actStartTime)
				{
					lSimDelay = (delayItem.actStartTime - delayItem.planSt);
				}
				
				ElapsedTime estSimDelay(long(lSimDelay/100.0+0.5));
				strSimSchDelay.Format(_T("%d"), estSimDelay.asMinutes());
				_file.writeField(strSimSchDelay);
			}
			
			{
				//ATD and ATA
				CString strActualStand;
				if (delayItem.atdTime >= 0)
				{
					long lActualTime = 0;
					lActualTime = delayItem.atdTime;
					ElapsedTime estStartActual(long(lActualTime/100.0+0.5));
					strActualStand.Format(_T("Day%d %02d:%02d:%02d"), estStartActual.GetDay(), estStartActual.GetHour(), estStartActual.GetMinute(), estStartActual.GetSecond());
				}
				_file.writeField( strActualStand);

				//schedule delay
				CString strScheduleDelay;
				if (delayItem.atdTime >= 0)
				{
					long lDelay = (delayItem.atdTime - delayItem.planSt);
					ElapsedTime estSchDelay(long(lDelay/100.0+0.5));
					strScheduleDelay.Format(_T("%d"), estSchDelay.asMinutes());
				}
				_file.writeField( strScheduleDelay);
			}
			

			//T Start(plan)
			long lStartTime = 0;
			lStartTime = delayItem.actStartTime;
			ElapsedTime estStartPlan(long(lStartTime/100.0+0.5));
			CString strStartPlan(_T(""));
			strStartPlan.Format(_T("Day%d %02d:%02d:%02d"), estStartPlan.GetDay(), estStartPlan.GetHour(), estStartPlan.GetMinute(), estStartPlan.GetSecond());

			_file.writeField( strStartPlan);

			//T end (plan)
			long lEndTime = 0;
			if (delayItem.actEndTime)
			{
				lEndTime = delayItem.actEndTime - delayItem.totalDelayTime;
			}
			ElapsedTime estEndPlan(long(lEndTime/100.0+0.5));
			CString strEndPlan(_T(""));
			strEndPlan.Format(_T("Day%d %02d:%02d:%02d"), estEndPlan.GetDay(), estEndPlan.GetHour(), estEndPlan.GetMinute(), estEndPlan.GetSecond());

			_file.writeField( strEndPlan);

			//T Start(act)
			long lStartActTime = 0;
			lStartActTime = delayItem.actStartTime;
			ElapsedTime estStartActPlan(long(lStartActTime/100.0+0.5));
			CString strStartActPlan(_T(""));
			strStartActPlan.Format(_T("Day%d %02d:%02d:%02d"), estStartActPlan.GetDay(), estStartActPlan.GetHour(), estStartActPlan.GetMinute(), estStartActPlan.GetSecond());

			_file.writeField(strStartActPlan);

			//T end(act)
			long lEndActTime = 0;
			lEndActTime = delayItem.actEndTime;
			ElapsedTime estEndActPlan(long(lEndActTime/100.0+0.5));
			CString strEndActPlan(_T(""));
			strEndActPlan.Format(_T("Day%d %02d:%02d:%02d"), estEndActPlan.GetDay(), estEndActPlan.GetHour(), estEndActPlan.GetMinute(), estEndActPlan.GetSecond());

			_file.writeField(strEndActPlan);

			//Total delay(mins)
			ElapsedTime estTotalDelayTime(long(m_vTotalResult[i].totalDelayTime/100.0+0.5));
			CString strTotalDelayTime;
			strTotalDelayTime.Format("%s", estTotalDelayTime.printTime());
			_file.writeField( strTotalDelayTime);

			//add one row
			nRowCount++;
		}
		_file.writeLine() ;
	}

}
void CAirsideFlightDelayReport::ExportListSummary(ArctermFile& _file,CParameters * parameter)
{
	ASSERT(parameter);
	CAirsideFlightDelayParam* pAirsideFlightDelayParam = (CAirsideFlightDelayParam*)parameter;
	ASSERT(pAirsideFlightDelayParam);

	CSummaryTotalDelayResult       summaryTotalDelayResult;
	CSummaryAirDelayResult         summaryAirDelayResult;
	CSummaryTaxiDelayResult        summaryTaxiDelayResult;
	CSummaryStandDelayResult       summaryStandDelayResult;
	CSummaryServiceDelayResult     summaryServiceDelayResult;
	CSummaryTakeOffDelayResult     summaryTakeoffDelayResult;

	summaryTotalDelayResult.GenerateResult(m_vResult, parameter);
	vector<CSummaryFlightTypeData>  totalSummaryData;
	summaryTotalDelayResult.GetSummaryResult(totalSummaryData);

	summaryAirDelayResult.GenerateResult(m_vResult, parameter);
	vector<CSummaryFlightTypeData>  airSummaryData;
	summaryAirDelayResult.GetSummaryResult(airSummaryData);

	summaryTaxiDelayResult.GenerateResult(m_vResult, parameter);
	vector<CSummaryFlightTypeData>  taxiSummaryData;
	summaryTaxiDelayResult.GetSummaryResult(taxiSummaryData);

	summaryStandDelayResult.GenerateResult(m_vResult, parameter);
	vector<CSummaryFlightTypeData>  standSummaryData;
	summaryStandDelayResult.GetSummaryResult(standSummaryData);

	summaryServiceDelayResult.GenerateResult(m_vResult, parameter);
	vector<CSummaryFlightTypeData>  serviceSummaryData;
	summaryServiceDelayResult.GetSummaryResult(serviceSummaryData);

	summaryTakeoffDelayResult.GenerateResult(m_vResult, parameter);
	vector<CSummaryFlightTypeData>  takeoffSummaryData;
	summaryTakeoffDelayResult.GetSummaryResult(takeoffSummaryData);

	//assert
	ASSERT(totalSummaryData.size() == airSummaryData.size());
	ASSERT(airSummaryData.size() == taxiSummaryData.size());
	ASSERT(taxiSummaryData.size() == standSummaryData.size());
	ASSERT(standSummaryData.size() == serviceSummaryData.size());
	ASSERT(serviceSummaryData.size() == takeoffSummaryData.size());

	for (int i=0; i<(int)totalSummaryData.size(); i++)
	{
		//flight type
		CString strFlightType;
		totalSummaryData[i].m_fltConstraint.screenPrint(strFlightType.GetBuffer(1024));
		_file.writeField( strFlightType);

		//total delay
		_file.writeField(totalSummaryData[i].m_summaryData.m_estTotal.printTime());
		//Min Delay(mins)
		_file.writeField(totalSummaryData[i].m_summaryData.m_estMin.printTime());
		//Max Delay(mins)
		_file.writeField(totalSummaryData[i].m_summaryData.m_estMax.printTime());
		//Mean Delay(mins)
		_file.writeField(totalSummaryData[i].m_summaryData.m_estAverage.printTime());
		//Q1(mins)
		_file.writeField(totalSummaryData[i].m_summaryData.m_estQ1.printTime());
		//Q2(mins)
		_file.writeField(totalSummaryData[i].m_summaryData.m_estQ2.printTime());
		//Q3(mins)
		_file.writeField(totalSummaryData[i].m_summaryData.m_estQ3.printTime());
		//P1(mins)
		_file.writeField(totalSummaryData[i].m_summaryData.m_estP1.printTime());
		//P5(mins)
		_file.writeField(totalSummaryData[i].m_summaryData.m_estP5.printTime());
		//P10(mins)
		_file.writeField(totalSummaryData[i].m_summaryData.m_estP10.printTime());
		//P90(mins)
		_file.writeField(totalSummaryData[i].m_summaryData.m_estP90.printTime());
		//P95(mins)
		_file.writeField(totalSummaryData[i].m_summaryData.m_estP95.printTime());
		//P99(mins)
		_file.writeField(totalSummaryData[i].m_summaryData.m_estP99.printTime());
		//Std dev(mins)
		_file.writeField(totalSummaryData[i].m_summaryData.m_estSigma.printTime());

		//air delay
		//Min Delay(mins)
		_file.writeField(airSummaryData[i].m_summaryData.m_estMin.printTime());
		//Max Delay(mins)
		_file.writeField(airSummaryData[i].m_summaryData.m_estMax.printTime());
		//Mean Delay(mins)
		_file.writeField(airSummaryData[i].m_summaryData.m_estAverage.printTime());
		//Q1(mins)
		_file.writeField(airSummaryData[i].m_summaryData.m_estQ1.printTime());
		//Q2(mins)
		_file.writeField(airSummaryData[i].m_summaryData.m_estQ2.printTime());
		//Q3(mins)
		_file.writeField(airSummaryData[i].m_summaryData.m_estQ3.printTime());
		//P1(mins)
		_file.writeField(airSummaryData[i].m_summaryData.m_estP1.printTime());
		//P5(mins)
		_file.writeField(airSummaryData[i].m_summaryData.m_estP5.printTime());
		//P10(mins)
		_file.writeField(airSummaryData[i].m_summaryData.m_estP10.printTime());
		//P90(mins)
		_file.writeField(airSummaryData[i].m_summaryData.m_estP90.printTime());
		//P95(mins)
		_file.writeField(airSummaryData[i].m_summaryData.m_estP95.printTime());
		//P99(mins)
		_file.writeField(airSummaryData[i].m_summaryData.m_estP99.printTime());
		//Std dev(mins)
		_file.writeField(airSummaryData[i].m_summaryData.m_estSigma.printTime());

		//Taxi delay
		//Min Delay(mins)
		_file.writeField(taxiSummaryData[i].m_summaryData.m_estMin.printTime());
		//Max Delay(mins)
		_file.writeField(taxiSummaryData[i].m_summaryData.m_estMax.printTime());
		//Mean Delay(mins)
		_file.writeField(taxiSummaryData[i].m_summaryData.m_estAverage.printTime());
		//Q1(mins)
		_file.writeField(taxiSummaryData[i].m_summaryData.m_estQ1.printTime());
		//Q2(mins)
		_file.writeField(taxiSummaryData[i].m_summaryData.m_estQ2.printTime());
		//Q3(mins)
		_file.writeField(taxiSummaryData[i].m_summaryData.m_estQ3.printTime());
		//P1(mins)
		_file.writeField(taxiSummaryData[i].m_summaryData.m_estP1.printTime());
		//P5(mins)
		_file.writeField(taxiSummaryData[i].m_summaryData.m_estP5.printTime());
		//P10(mins)
		_file.writeField(taxiSummaryData[i].m_summaryData.m_estP10.printTime());
		//P90(mins)
		_file.writeField(taxiSummaryData[i].m_summaryData.m_estP90.printTime());
		//P95(mins)
		_file.writeField(taxiSummaryData[i].m_summaryData.m_estP95.printTime());
		//P99(mins)
		_file.writeField(taxiSummaryData[i].m_summaryData.m_estP99.printTime());
		//Std dev(mins)
		_file.writeField(taxiSummaryData[i].m_summaryData.m_estSigma.printTime());

		//Stand delay
		//Min Delay(mins)
		_file.writeField(standSummaryData[i].m_summaryData.m_estMin.printTime());
		//Max Delay(mins)
		_file.writeField(standSummaryData[i].m_summaryData.m_estMax.printTime());
		//Mean Delay(mins)
		_file.writeField(standSummaryData[i].m_summaryData.m_estAverage.printTime());
		//Q1(mins)
		_file.writeField(standSummaryData[i].m_summaryData.m_estQ1.printTime());
		//Q2(mins)
		_file.writeField(standSummaryData[i].m_summaryData.m_estQ2.printTime());
		//Q3(mins)
		_file.writeField(standSummaryData[i].m_summaryData.m_estQ3.printTime());
		//P1(mins)
		_file.writeField(standSummaryData[i].m_summaryData.m_estP1.printTime());
		//P5(mins)
		_file.writeField(standSummaryData[i].m_summaryData.m_estP5.printTime());
		//P10(mins)
		_file.writeField(standSummaryData[i].m_summaryData.m_estP10.printTime());
		//P90(mins)
		_file.writeField(standSummaryData[i].m_summaryData.m_estP90.printTime());
		//P95(mins)
		_file.writeField(standSummaryData[i].m_summaryData.m_estP95.printTime());
		//P99(mins)
		_file.writeField(standSummaryData[i].m_summaryData.m_estP99.printTime());
		//Std dev(mins)
		_file.writeField(standSummaryData[i].m_summaryData.m_estSigma.printTime());

		//Service delay
		//Min Delay(mins)
		_file.writeField(serviceSummaryData[i].m_summaryData.m_estMin.printTime());
		//Max Delay(mins)
		_file.writeField(serviceSummaryData[i].m_summaryData.m_estMax.printTime());
		//Mean Delay(mins)
		_file.writeField(serviceSummaryData[i].m_summaryData.m_estAverage.printTime());
		//Q1(mins)
		_file.writeField(serviceSummaryData[i].m_summaryData.m_estQ1.printTime());
		//Q2(mins)
		_file.writeField(serviceSummaryData[i].m_summaryData.m_estQ2.printTime());
		//Q3(mins)
		_file.writeField(serviceSummaryData[i].m_summaryData.m_estQ3.printTime());
		//P1(mins)
		_file.writeField(serviceSummaryData[i].m_summaryData.m_estP1.printTime());
		//P5(mins)
		_file.writeField(serviceSummaryData[i].m_summaryData.m_estP5.printTime());
		//P10(mins)
		_file.writeField(serviceSummaryData[i].m_summaryData.m_estP10.printTime());
		//P90(mins)
		_file.writeField(serviceSummaryData[i].m_summaryData.m_estP90.printTime());
		//P95(mins)
		_file.writeField(serviceSummaryData[i].m_summaryData.m_estP95.printTime());
		//P99(mins)
		_file.writeField(serviceSummaryData[i].m_summaryData.m_estP99.printTime());
		//Std dev(mins)
		_file.writeField(serviceSummaryData[i].m_summaryData.m_estSigma.printTime());

		//Takeoff delay
		//Min Delay(mins)
		_file.writeField(takeoffSummaryData[i].m_summaryData.m_estMin.printTime());
		//Max Delay(mins)
		_file.writeField(takeoffSummaryData[i].m_summaryData.m_estMax.printTime());
		//Mean Delay(mins)
		_file.writeField(takeoffSummaryData[i].m_summaryData.m_estAverage.printTime());
		//Q1(mins)
		_file.writeField(takeoffSummaryData[i].m_summaryData.m_estQ1.printTime());
		//Q2(mins)
		_file.writeField(takeoffSummaryData[i].m_summaryData.m_estQ2.printTime());
		//Q3(mins)
		_file.writeField(takeoffSummaryData[i].m_summaryData.m_estQ3.printTime());
		//P1(mins)
		_file.writeField(takeoffSummaryData[i].m_summaryData.m_estP1.printTime());
		//P5(mins)
		_file.writeField(takeoffSummaryData[i].m_summaryData.m_estP5.printTime());
		//P10(mins)
		_file.writeField(takeoffSummaryData[i].m_summaryData.m_estP10.printTime());
		//P90(mins)
		_file.writeField(takeoffSummaryData[i].m_summaryData.m_estP90.printTime());
		//P95(mins)
		_file.writeField(takeoffSummaryData[i].m_summaryData.m_estP95.printTime());
		//P99(mins)
		_file.writeField(takeoffSummaryData[i].m_summaryData.m_estP99.printTime());
		//Std dev(mins)
		_file.writeField(takeoffSummaryData[i].m_summaryData.m_estSigma.printTime());

		_file.writeLine() ;
	}
}
void CAirsideFlightDelayReport::FillDetailListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	ASSERT(parameter);
	CAirsideFlightDelayParam* pAirsideFlightDelayParam = (CAirsideFlightDelayParam*)parameter;
	ASSERT(pAirsideFlightDelayParam);

	int nRowCount = 0;
	for (size_t i =0; i < m_vTotalResult.size(); ++i )
	{
		FltDelayItem& delayItem = m_vTotalResult[i];

		//arrival
		if (delayItem.bArrOrDeplDelay)
		{
			//flight
			CString strFlight(_T(""));
			strFlight.Format("%s%s", delayItem.fltDesc._airline.GetValue(), delayItem.fltDesc.arrID);
			cxListCtrl.InsertItem(nRowCount, strFlight);
			cxListCtrl.SetItemData(nRowCount,i);

			//flight type
			CString strFlightType;
			delayItem.m_fltCons.screenPrint(strFlightType.GetBuffer(1024));
			cxListCtrl.SetItemText(nRowCount, 1,strFlightType);

			//Operation
			cxListCtrl.SetItemText(nRowCount, 2, _T("A"));

			//Start
			CString strStart(_T("Start"));
			CAirsideReportNode airsideReportStartNode = pAirsideFlightDelayParam->GetStartObject();
			strStart.Format(_T("%s"), airsideReportStartNode.GetNodeName());
			cxListCtrl.SetItemText(nRowCount, 3, strStart);

			//end
			CString strEnd(_T("End"));
			CAirsideReportNode airsideReportEndNode = pAirsideFlightDelayParam->GetEndObject();
			strEnd.Format(_T("%s"), airsideReportEndNode.GetNodeName());
			cxListCtrl.SetItemText(nRowCount, 4, strEnd);

			//STD and STA
			CString strStand;
			long lplanSta = 0;
			lplanSta = delayItem.planSt;
			ElapsedTime estPlanSta(long(lplanSta/100.0+0.5));
			strStand.Format(_T("Day%d %02d:%02d:%02d"), estPlanSta.GetDay(), estPlanSta.GetHour(), estPlanSta.GetMinute(), estPlanSta.GetSecond());
			cxListCtrl.SetItemText(nRowCount, 5, strStand);

			
			{
				//SMTD and SMTA
				CString strSmta;
				long lSmta = delayItem.actEndTime;
				ElapsedTime estSmta(long(lSmta/100.0+0.5));
				strSmta.Format(_T("Day%d %02d:%02d:%02d"), estSmta.GetDay(), estSmta.GetHour(), estSmta.GetMinute(), estSmta.GetSecond());
				cxListCtrl.SetItemText(nRowCount, 6, strSmta);

				//Sim sch delay
				CString strSimSchDelay;
				long lSimDelay = 0l;
				if (delayItem.actEndTime)
				{
					lSimDelay = (delayItem.actEndTime - delayItem.planSt);
				}
				if (delayItem.bValidData)
				{
					ElapsedTime estSimDelay(long(lSimDelay/100.0+0.5));
					strSimSchDelay.Format(_T("%d"), estSimDelay.asMinutes());
					cxListCtrl.SetItemText(nRowCount, 7, strSimSchDelay);
				}
			}

			{
				//ATD and ATA
				CString strActualStand;
				long lActualTime = 0;
				lActualTime = delayItem.ataTime;
				if (lActualTime >= 0)
				{
					ElapsedTime estStartActual(long(lActualTime/100.0+0.5));
					strActualStand.Format(_T("Day%d %02d:%02d:%02d"), estStartActual.GetDay(), estStartActual.GetHour(), estStartActual.GetMinute(), estStartActual.GetSecond());
				}

				cxListCtrl.SetItemText(nRowCount, 8, strActualStand);

				//schedule delay
				CString strScheduleDelay;
				if (delayItem.ataTime >= 0)
				{
					long lDelay = (delayItem.ataTime - delayItem.planSt);
					ElapsedTime estSchDelay(long(lDelay/100.0+0.5));
					strScheduleDelay.Format(_T("%d"), estSchDelay.asMinutes());
				}
				cxListCtrl.SetItemText(nRowCount, 9, strScheduleDelay);
			}
		


			//T Start(plan)
			long lStartTime = 0;
			lStartTime = delayItem.actStartTime;
			ElapsedTime estStartPlan(long(lStartTime/100.0+0.5));
			CString strStartPlan(_T(""));
			strStartPlan.Format(_T("Day%d %02d:%02d:%02d"), estStartPlan.GetDay(), estStartPlan.GetHour(), estStartPlan.GetMinute(), estStartPlan.GetSecond());
			cxListCtrl.SetItemText(nRowCount, 10, strStartPlan);

			//T end (plan)
			long lEndTime = 0;
			if (delayItem.actEndTime)
			{
				lEndTime = delayItem.actEndTime - delayItem.totalDelayTime;
			}
			ElapsedTime estEndPlan(long(lEndTime/100.0+0.5));
			CString strEndPlan(_T(""));
			strEndPlan.Format(_T("Day%d %02d:%02d:%02d"), estEndPlan.GetDay(), estEndPlan.GetHour(), estEndPlan.GetMinute(), estEndPlan.GetSecond());

			cxListCtrl.SetItemText(nRowCount, 11, strEndPlan);

			//T Start(act)
			long lStartActTime = 0;
			lStartActTime = delayItem.actStartTime;
			ElapsedTime estStartActPlan(long(lStartActTime/100.0+0.5));
			CString strStartActPlan(_T(""));
			strStartActPlan.Format(_T("Day%d %02d:%02d:%02d"), estStartActPlan.GetDay(), estStartActPlan.GetHour(), estStartActPlan.GetMinute(), estStartActPlan.GetSecond());

			cxListCtrl.SetItemText(nRowCount, 12, strStartActPlan);

			//T end(act)
			long lEndActTime = 0;
			lEndActTime = delayItem.actEndTime;
			ElapsedTime estEndActPlan(long(lEndActTime/100.0+0.5));
			CString strEndActPlan(_T(""));
			strEndActPlan.Format(_T("Day%d %02d:%02d:%02d"), estEndActPlan.GetDay(), estEndActPlan.GetHour(), estEndActPlan.GetMinute(), estEndActPlan.GetSecond());

			cxListCtrl.SetItemText(nRowCount, 13, strEndActPlan);

			//Total delay(mins)
			ElapsedTime estTotalDelayTime(long(m_vTotalResult[i].totalDelayTime/100.0+0.5));
			CString strTotalDelayTime;
			strTotalDelayTime.Format("%s", estTotalDelayTime.printTime());
			cxListCtrl.SetItemText(nRowCount, 14, strTotalDelayTime);

			//add one row
			nRowCount++;
		}

		//departure
		else
		{
			//flight
			CString strFlight(_T(""));
			strFlight.Format("%s%s", m_vTotalResult[i].fltDesc._airline.GetValue(), m_vTotalResult[i].fltDesc.depID);
			cxListCtrl.InsertItem(nRowCount, strFlight);
			cxListCtrl.SetItemData(nRowCount,i);

			//flight type
			CString strFlightType;
			m_vTotalResult[i].m_fltCons.screenPrint(strFlightType.GetBuffer(1024));
			cxListCtrl.SetItemText(nRowCount, 1,strFlightType);

			//Operation
			cxListCtrl.SetItemText(nRowCount, 2, _T("D"));

			//Start
			CString strStart(_T("Start"));
			CAirsideReportNode airsideReportStartNode = pAirsideFlightDelayParam->GetStartObject();
			strStart.Format(_T("%s"),airsideReportStartNode.GetNodeName());
			cxListCtrl.SetItemText(nRowCount, 3, strStart);

			//end
			CString strEnd(_T("End"));
			CAirsideReportNode airsideReportEndNode = pAirsideFlightDelayParam->GetEndObject();
			strEnd.Format(_T("%s"),airsideReportEndNode.GetNodeName());
			cxListCtrl.SetItemText(nRowCount, 4, strEnd);

			//STD and STA
			CString strStand;
			long lplanStd = 0;
			lplanStd = delayItem.planSt;
			ElapsedTime estPlanStd(long(lplanStd/100.0+0.5));
			strStand.Format(_T("Day%d %02d:%02d:%02d"), estPlanStd.GetDay(), estPlanStd.GetHour(), estPlanStd.GetMinute(), estPlanStd.GetSecond());
			cxListCtrl.SetItemText(nRowCount, 5, strStand);

			{	
				//SMTD and SMTA
				CString strSmtd;
				long lSmtd = delayItem.actStartTime;
				ElapsedTime estSmtd(long(lSmtd/100.0+0.5));
				strSmtd.Format(_T("Day%d %02d:%02d:%02d"), estSmtd.GetDay(), estSmtd.GetHour(), estSmtd.GetMinute(), estSmtd.GetSecond());
				cxListCtrl.SetItemText(nRowCount, 6, strSmtd);

				//Sim sch delay
				//Total delay(mins)
				CString strSimSchDelay;
				long lSimDelay = 0l;
				if (delayItem.actStartTime)
				{
					lSimDelay = (delayItem.actStartTime - delayItem.planSt);
				}
				
				if (delayItem.bValidData)
				{
					ElapsedTime estSimDelay(long(lSimDelay/100.0+0.5));
					strSimSchDelay.Format(_T("%d"), estSimDelay.asMinutes());
					cxListCtrl.SetItemText(nRowCount, 7,strSimSchDelay);
				}
				
			}

			{
				//ATD and ATA
				CString strActualStand;
				long lActualTime = 0;
				lActualTime = delayItem.atdTime;
				if (lActualTime >= 0)
				{
					ElapsedTime estStartActual(long(lActualTime/100.0+0.5));
					strActualStand.Format(_T("Day%d %02d:%02d:%02d"), estStartActual.GetDay(), estStartActual.GetHour(), estStartActual.GetMinute(), estStartActual.GetSecond());
				}
				cxListCtrl.SetItemText(nRowCount, 8, strActualStand);

				//schedule delay
				CString strScheduleDelay;
				if (delayItem.atdTime >= 0)
				{
					long lDelay = (delayItem.atdTime - delayItem.planSt);
					ElapsedTime estSchDelay(long(lDelay/100.0+0.5));
					strScheduleDelay.Format(_T("%d"), estSchDelay.asMinutes());
				}
				cxListCtrl.SetItemText(nRowCount, 9, strScheduleDelay);

			}
			
			//T Start(plan)
			long lStartTime = 0;
			lStartTime = delayItem.actStartTime;
			ElapsedTime estStartPlan(long(lStartTime/100.0+0.5));
			CString strStartPlan(_T(""));
			strStartPlan.Format(_T("Day%d %02d:%02d:%02d"), estStartPlan.GetDay(), estStartPlan.GetHour(), estStartPlan.GetMinute(), estStartPlan.GetSecond());

			cxListCtrl.SetItemText(nRowCount, 10, strStartPlan);

			//T end (plan)
			long lEndTime = 0;
			if (delayItem.actEndTime)
			{
				lEndTime = delayItem.actEndTime - delayItem.totalDelayTime;
			}
			ElapsedTime estEndPlan(long(lEndTime/100.0+0.5));
			CString strEndPlan(_T(""));
			strEndPlan.Format(_T("Day%d %02d:%02d:%02d"), estEndPlan.GetDay(), estEndPlan.GetHour(), estEndPlan.GetMinute(), estEndPlan.GetSecond());

			cxListCtrl.SetItemText(nRowCount, 11, strEndPlan);

			//T Start(act)
			long lStartActTime = 0;
			lStartActTime = delayItem.actStartTime;
			ElapsedTime estStartActPlan(long(lStartActTime/100.0+0.5));
			CString strStartActPlan(_T(""));
			strStartActPlan.Format(_T("Day%d %02d:%02d:%02d"), estStartActPlan.GetDay(), estStartActPlan.GetHour(), estStartActPlan.GetMinute(), estStartActPlan.GetSecond());

			cxListCtrl.SetItemText(nRowCount, 12, strStartActPlan);

			//T end(act)
			long lEndActTime = 0;
			lEndActTime = delayItem.actEndTime;
			ElapsedTime estEndActPlan(long(lEndActTime/100.0+0.5));
			CString strEndActPlan(_T(""));
			strEndActPlan.Format(_T("Day%d %02d:%02d:%02d"), estEndActPlan.GetDay(), estEndActPlan.GetHour(), estEndActPlan.GetMinute(), estEndActPlan.GetSecond());

			cxListCtrl.SetItemText(nRowCount, 13, strEndActPlan);

			//Total delay(mins)
			ElapsedTime estTotalDelayTime(long(m_vTotalResult[i].totalDelayTime/100.0+0.5));
			CString strTotalDelayTime;
			strTotalDelayTime.Format("%s", estTotalDelayTime.printTime());
			cxListCtrl.SetItemText(nRowCount, 14, strTotalDelayTime);

			//add one row
			nRowCount++;
		}
	}
}

//fill summary flight schedule list ctrl
void CAirsideFlightDelayReport::FillSummaryScheduleListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	ASSERT(parameter);
	CAirsideFlightDelayParam* pAirsideFlightDelayParam = (CAirsideFlightDelayParam*)parameter;
	ASSERT(pAirsideFlightDelayParam);

	CSummaryFlightScheduleDelayResult       summaryScheduleDelayResult;

	summaryScheduleDelayResult.GenerateResult(m_vResult, parameter);
	vector<CSummaryFlightTypeData>  scheduleSummaryData;
	summaryScheduleDelayResult.GetSummaryResult(scheduleSummaryData);

	for (int i=0; i<(int)scheduleSummaryData.size(); i++)
	{
		//flight type
		CString strFlightType;
		scheduleSummaryData[i].m_fltConstraint.screenPrint(strFlightType.GetBuffer(1024));
		cxListCtrl.InsertItem(i, strFlightType);

		//Min Delay(mins)
		cxListCtrl.SetItemText(i, 1,scheduleSummaryData[i].m_summaryData.m_estMin.printTime());
		//Max Delay(mins)
		cxListCtrl.SetItemText(i, 2,scheduleSummaryData[i].m_summaryData.m_estMax.printTime());
		//Mean Delay(mins)
		cxListCtrl.SetItemText(i, 3,scheduleSummaryData[i].m_summaryData.m_estAverage.printTime());
		//Q1(mins)
		cxListCtrl.SetItemText(i, 4,scheduleSummaryData[i].m_summaryData.m_estQ1.printTime());
		//Q2(mins)
		cxListCtrl.SetItemText(i, 5,scheduleSummaryData[i].m_summaryData.m_estQ2.printTime());
		//Q3(mins)
		cxListCtrl.SetItemText(i, 6,scheduleSummaryData[i].m_summaryData.m_estQ3.printTime());
		//P1(mins)
		cxListCtrl.SetItemText(i, 7,scheduleSummaryData[i].m_summaryData.m_estP1.printTime());
		//P5(mins)
		cxListCtrl.SetItemText(i, 8,scheduleSummaryData[i].m_summaryData.m_estP5.printTime());
		//P10(mins)
		cxListCtrl.SetItemText(i, 9,scheduleSummaryData[i].m_summaryData.m_estP10.printTime());
		//P90(mins)
		cxListCtrl.SetItemText(i, 10,scheduleSummaryData[i].m_summaryData.m_estP90.printTime());
		//P95(mins)
		cxListCtrl.SetItemText(i, 11,scheduleSummaryData[i].m_summaryData.m_estP95.printTime());
		//P99(mins)
		cxListCtrl.SetItemText(i, 12,scheduleSummaryData[i].m_summaryData.m_estP99.printTime());
		//Std dev(mins)
		cxListCtrl.SetItemText(i, 13,scheduleSummaryData[i].m_summaryData.m_estSigma.printTime());
	}
}

void CAirsideFlightDelayReport::FillSummaryListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	ASSERT(parameter);
	CAirsideFlightDelayParam* pAirsideFlightDelayParam = (CAirsideFlightDelayParam*)parameter;
	ASSERT(pAirsideFlightDelayParam);

	CSummaryTotalDelayResult       summaryTotalDelayResult;

	summaryTotalDelayResult.GenerateResult(m_vResult, parameter);
	vector<CSummaryFlightTypeData>  totalSummaryData;
	summaryTotalDelayResult.GetSummaryResult(totalSummaryData);

	for (int i=0; i<(int)totalSummaryData.size(); i++)
	{
		//flight type
		CString strFlightType;
		totalSummaryData[i].m_fltConstraint.screenPrint(strFlightType.GetBuffer(1024));
		cxListCtrl.InsertItem(i, strFlightType);

		//total delay
		cxListCtrl.SetItemText(i, 1,totalSummaryData[i].m_summaryData.m_estTotal.printTime());
		//Min Delay(mins)
		cxListCtrl.SetItemText(i, 2,totalSummaryData[i].m_summaryData.m_estMin.printTime());
		//Max Delay(mins)
		cxListCtrl.SetItemText(i, 3,totalSummaryData[i].m_summaryData.m_estMax.printTime());
		//Mean Delay(mins)
		cxListCtrl.SetItemText(i, 4,totalSummaryData[i].m_summaryData.m_estAverage.printTime());
		//Q1(mins)
		cxListCtrl.SetItemText(i, 5,totalSummaryData[i].m_summaryData.m_estQ1.printTime());
		//Q2(mins)
		cxListCtrl.SetItemText(i, 6,totalSummaryData[i].m_summaryData.m_estQ2.printTime());
		//Q3(mins)
		cxListCtrl.SetItemText(i, 7,totalSummaryData[i].m_summaryData.m_estQ3.printTime());
		//P1(mins)
		cxListCtrl.SetItemText(i, 8,totalSummaryData[i].m_summaryData.m_estP1.printTime());
		//P5(mins)
		cxListCtrl.SetItemText(i, 9,totalSummaryData[i].m_summaryData.m_estP5.printTime());
		//P10(mins)
		cxListCtrl.SetItemText(i, 10,totalSummaryData[i].m_summaryData.m_estP10.printTime());
		//P90(mins)
		cxListCtrl.SetItemText(i, 11,totalSummaryData[i].m_summaryData.m_estP90.printTime());
		//P95(mins)
		cxListCtrl.SetItemText(i, 12,totalSummaryData[i].m_summaryData.m_estP95.printTime());
		//P99(mins)
		cxListCtrl.SetItemText(i, 13,totalSummaryData[i].m_summaryData.m_estP99.printTime());
		//Std dev(mins)
		cxListCtrl.SetItemText(i, 14,totalSummaryData[i].m_summaryData.m_estSigma.printTime());
	}
}
BOOL CAirsideFlightDelayReport::ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType )
{
	_file.writeInt((int)m_vResult.size()) ;
	_file.writeLine() ;
	for (int i = 0 ;i < (int)m_vResult.size() ;i++)
	{
		if(!m_vResult[i].ExportFile(_file))
			return FALSE ;
	}
	return TRUE ;
}
BOOL CAirsideFlightDelayReport::ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType )
{
	m_vResult.clear() ;
	int size = 0 ;
	if(!_file.getInteger(size))
		return FALSE ;
	for(int i = 0 ; i < size ;i++)
	{
		_file.getLine() ;
		FltTypeDelayItem delayitem(m_AirportDB) ;
		delayitem.ImportFile(_file) ;
		for (int j = 0 ; j < (int)delayitem.m_vDelayData.size() ;j++)
		{
			m_vTotalResult.push_back(delayitem.m_vDelayData[j]) ;
		}
		m_vResult.push_back(delayitem) ;

	}
	return TRUE ;
}
BOOL CAirsideFlightDelayReport::FltTypeDelayItem::ExportFile(ArctermFile& _file)
{
	TCHAR th[1024] = {0} ;
	m_fltCons.WriteSyntaxStringWithVersion(th) ;
	_file.writeField(th) ;
	_file.writeLine() ;

	_file.writeInt((int)m_vDelayData.size()) ;
	_file.writeLine() ;
	for (int i = 0 ; i < (int)m_vDelayData.size() ;i++)
	{
		if(!m_vDelayData[i].ExportFile(_file))
			return FALSE ;
	}
	return TRUE ;
}
BOOL CAirsideFlightDelayReport::FltTypeDelayItem::ImportFile(ArctermFile& _file)
{
	TCHAR th[1024] = {0} ;
	_file.getField(th,1024) ;
	m_fltCons.setConstraintWithVersion(th) ;

	_file.getLine() ;
	int size = 0 ;
	if(!_file.getInteger(size) )
		return FALSE ;

	for(int i = 0 ; i < size ;i++)
	{
		_file.getLine() ;
		FltDelayItem fltdelayItem(m_fltCons.GetAirportDB()) ;
		fltdelayItem.ImportFile(_file) ;
		m_vDelayData.push_back(fltdelayItem) ;
	}
	return TRUE ;
}
BOOL CAirsideFlightDelayReport::FltDelayItem::ExportFile(ArctermFile& _file)
{
	TCHAR th[1024] = { 0} ;
	m_fltCons.WriteSyntaxStringWithVersion(th) ;
	_file.writeField(th) ;
	_file.writeInt(totalDelayTime) ;
	_file.writeInt(airDelayTime) ;
	_file.writeInt(taxiDelayTime) ;
	_file.writeInt(standDelayTime) ;
	_file.writeInt(serviceDelayTime) ;
	_file.writeInt(takeoffDelayTime) ;
	_file.writeInt(actStartTime);
	_file.writeInt(actEndTime);
	_file.writeInt(planSt);
	_file.writeInt(ataTime);
	_file.writeInt(atdTime);
	_file.writeInt(bArrOrDeplDelay?1:0) ;
	_file.writeInt(bValidData?1:0);
	_file.writeLine() ;
	fltDesc.ExportFile(_file) ;

	m_startNode.ExportFile(_file) ;
	m_endNode.ExportFile(_file) ;
	m_midNode.ExportFile(_file) ;

	_file.writeInt((int)vNodeDelay.size()) ;
	_file.writeLine() ;
	for (int i = 0 ; i < (int)vNodeDelay.size() ;i++)
	{
		vNodeDelay[i].ExportFile(_file) ;
	}
	return TRUE ;
}
BOOL CAirsideFlightDelayReport::FltDelayItem::ImportFile(ArctermFile& _file)
{
	TCHAR th[1024] = { 0} ;
	_file.getField(th,1024) ;
	m_fltCons.setConstraintWithVersion(th) ;
	_file.getInteger(totalDelayTime) ;
	_file.getInteger(airDelayTime) ;
	_file.getInteger(taxiDelayTime) ;
	_file.getInteger(standDelayTime) ;
	_file.getInteger(serviceDelayTime) ;
	_file.getInteger(takeoffDelayTime) ;
	_file.getInteger(actStartTime);
	_file.getInteger(actEndTime);
	_file.getInteger(planSt);
	_file.getInteger(ataTime);
	_file.getInteger(atdTime);
	int res = 0 ;
	_file.getInteger(res) ;
	bArrOrDeplDelay = res>0?true:false;
	_file.getInteger(res);
	bValidData = res?true:false;
	_file.getLine() ;
	fltDesc.ImportFile(_file) ;
	_file.getLine() ;
	m_startNode.ImportFile(_file) ;
	_file.getLine() ;
	m_endNode.ImportFile(_file) ;
	_file.getLine() ;
	m_midNode.ImportFile(_file) ;
	_file.getLine() ;
	int size = 0 ;
	if(!_file.getInteger(size))  
		return FALSE ;
	for (int i = 0 ; i < size ;i++)
	{
		_file.getLine() ;
		FltNodeDelayItem  nodedelayItem ;
		nodedelayItem.ImportFile(_file) ;
		vNodeDelay.push_back(nodedelayItem) ;
	}
	return TRUE ;
}
BOOL CAirsideFlightDelayReport::FltNodeDelayItem::ExportFile(ArctermFile& _file)
{
	_file.writeInt(delayTime) ;
	_file.writeField(m_strResName) ;
	//_file.writeInt((int)nNodeType) ;
	//_file.writeInt(nNodeID) ;
	_file.writeInt((int)nSegment) ;
	_file.writeInt(eArriveTime) ;
	_file.writeInt(eActArriveTime) ;
	_file.writeInt((int)nReason) ;
	_file.writeField(m_strDetailReason) ;
	_file.writeLine() ;
	return TRUE ;
}
BOOL CAirsideFlightDelayReport::FltNodeDelayItem::ImportFile(ArctermFile& _file)
{
	_file.getInteger(delayTime) ;
	int nodetye = 0 ;
	_file.getField(m_strResName.GetBuffer(1024),1024);
	// 	_file.getInteger(nodetye) ;
	// 	 nNodeType = (CAirsideReportNode::ReportNodeType)nodetye ;
	// 	_file.getInteger(nNodeID) ;
	// 	_file.getInteger(nodetye) ;
	nSegment = (FltDelaySegment)nodetye ;
	_file.getInteger(eArriveTime) ;
	_file.getInteger(eActArriveTime) ;
	_file.getInteger(nodetye) ;
	nReason = (FltDelayReason)nodetye ;
	TCHAR th[1024] = {0} ;
	_file.getField(th,1024 ) ;
	m_strDetailReason.Format(_T("%s"),th) ;
	return TRUE ;
}
