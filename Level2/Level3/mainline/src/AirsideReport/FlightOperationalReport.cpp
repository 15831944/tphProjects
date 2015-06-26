#include "StdAfx.h"
#include ".\flightoperationalreport.h"
#include "FlightOperationalParam.h"
#include "FlightOperationalAirDistanceResult.h"
#include "FlightOperationalGroundDistanceResult.h"
#include "../AirsideReport/Parameters.h"
#include "../Common/ARCUnit.h"
#include "../InputAirside/Runway.h"
#include "../InputAirside/Stand.h"
#include "../InputAirside/RunwayExit.h"
#include "../Inputs/flight.h"
#include <limits>


CFlightOperationalReport::CFlightOperationalReport(CBGetLogFilePath pFunc)
: CAirsideBaseReport(pFunc)
, m_pResult(NULL)
{
}

CFlightOperationalReport::~CFlightOperationalReport(void)
{
}

void CFlightOperationalReport::CalculateOperationResult(AirsideFlightLogEntry& logEntry,CFlightOperationalParam * pParam,AirsideFlightDescStruct& fltDesc,int nStartPos,int nEndPos,bool bArrival)
{
	//new report item
	FltOperationalItem fltOperationalItem(m_AirportDB);
	fltOperationalItem.m_ID = (int)m_vResult.size();
	fltOperationalItem.m_fltDesc = fltDesc;
	fltOperationalItem.m_bArrival = bArrival;

	CalcAirDistance(logEntry, nStartPos, nEndPos, fltOperationalItem);
	CalcGroundDistance(logEntry, nStartPos, nEndPos, fltOperationalItem);
	CalcAirTime(logEntry, nStartPos, nEndPos, fltOperationalItem);
	CalStayTime(logEntry, nStartPos, nEndPos,fltOperationalItem);
	CalcGourndTime(logEntry, nStartPos, nEndPos, fltOperationalItem);
	CalcAirFuel(logEntry, nStartPos, nEndPos, fltOperationalItem);
	CalcGroundFuel(logEntry, nStartPos, nEndPos, fltOperationalItem);
	CalcOperatingCost(logEntry, nStartPos, nEndPos, fltOperationalItem);
	if (bArrival)
	{
		CalcLandRunway(logEntry, nStartPos, nEndPos, fltOperationalItem);
		CalcLandDist(logEntry, nStartPos, nEndPos, fltOperationalItem);
		CalcRunwayExit(logEntry, nStartPos, nEndPos, fltOperationalItem);
		CalcArrStand(logEntry, nStartPos, nEndPos, fltOperationalItem);
	}
	else
	{
		CalcIntStand(logEntry, nStartPos, nEndPos, fltOperationalItem);
		CalcDepStand(logEntry, nStartPos, nEndPos, fltDesc, fltOperationalItem);
		CalcTakeoffRunway(logEntry, nStartPos, nEndPos, fltDesc, fltOperationalItem);
		CalcTakeoffIntersection(logEntry, nStartPos, nEndPos, fltDesc, fltOperationalItem);
		CalcTakeoffDistance(logEntry, nStartPos, nEndPos, fltDesc, fltOperationalItem);
	}
	
	
	CalcMaxAirSpeed(logEntry, nStartPos, nEndPos, fltOperationalItem);
	CalcMaxTaxiSpeed(logEntry, nStartPos, nEndPos, fltOperationalItem);
	CalcAvgTaxiSpeed(logEntry, nStartPos, nEndPos, fltOperationalItem);

	//m_vResult.push_back(fltOperationalItem);
	AddToResult(pParam, fltOperationalItem);
}

void CFlightOperationalReport::GenerateReport(CParameters * parameter)
{
	m_vResult.clear();

	CFlightOperationalParam *pParam = (CFlightOperationalParam*)parameter;
	ASSERT(pParam != NULL);

	long lStartTime = pParam->getStartTime().asSeconds()*100;
	long lEndTime = pParam->getEndTime().asSeconds()*100;

	//read log
	CString strDescFilepath = (*m_pCBGetLogFilePath)(AirsideDescFile);
	CString strEventFilePath = (*m_pCBGetLogFilePath)(AirsideEventFile);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	EventLog<AirsideFlightEventStruct> airsideFlightEventLog;
	AirsideFlightLog fltLog;
	fltLog.SetEventLog(&airsideFlightEventLog);
	fltLog.LoadDataIfNecessary(strDescFilepath);
	fltLog.LoadEventDataIfNecessary(strEventFilePath);

	int nFltLogCount = fltLog.getCount();
	CProgressBar bar(_T("Generate report..."),100,nFltLogCount,TRUE);
	for (int i =0; i < nFltLogCount; ++i)
	{
		bar.StepIt();
		long lDelayTime = 0L;

		AirsideFlightLogEntry logEntry;
		logEntry.SetEventLog(&airsideFlightEventLog);
		fltLog.getItem(logEntry,i);
		AirsideFlightDescStruct	fltDesc = logEntry.GetAirsideDescStruct();

		FlightConstraint fltCons;
		FLTCNSTR_MODE fltConMode;
		if (!fits(parameter,fltDesc,fltCons,fltConMode))
			continue;

		if (fltDesc.startTime == 0 && fltDesc.endTime == 0)
			continue;

		//check time
		if (fltDesc.endTime < lStartTime || fltDesc.startTime>lEndTime)//not in the time range
			continue;

		//arrival part
		if (fltConMode != ENUM_FLTCNSTR_MODE_DEP)
		{
			int nArrStartPos = 0;
			int nArrEndPos = 0;
			//return start log item and end item
			if (IsPassObject(logEntry, pParam, nArrStartPos, nArrEndPos,true))
			{
				//add last item
				if (strlen(fltDesc.depID) > 0 && nArrEndPos&& logEntry.getCount() > nArrEndPos + 1)
					nArrEndPos++;
				CalculateOperationResult(logEntry,pParam,fltDesc,nArrStartPos,nArrEndPos,true);
			}
		}
		
		//departure part
		if (fltConMode != ENUM_FLTCNSTR_MODE_ARR)
		{
			int nDepStartPos = 0;
			int nDepEndPos = 0;
			//return start log item and end item
			if (IsPassObject(logEntry, pParam, nDepStartPos, nDepEndPos,false))
			{
				CalculateOperationResult(logEntry,pParam,fltDesc,nDepStartPos,nDepEndPos,false);
			}
		}
	}
}

void CFlightOperationalReport::AddToResult(CParameters* param, FltOperationalItem& fltOperationalItem)
{
	ASSERT(param != NULL);

	int nFltConsCount = (int)param->getFlightConstraintCount();
	for(int i=0; i<nFltConsCount; i++)
	{
		if (param->getFlightConstraint(i).fits(fltOperationalItem.m_fltDesc))
		{
			fltOperationalItem.m_fltCons = param->getFlightConstraint(i);
			fltOperationalItem.m_fltCons.screenPrint(fltOperationalItem.m_strFltType.GetBuffer(1024));	
			fltOperationalItem.m_strFltType.ReleaseBuffer();

			m_vResult.push_back(fltOperationalItem);
		}
	}
}

//retrieve runway exit intersection node id
int CFlightOperationalReport::GetIntersectionNodeID(const AirsideFlightEventStruct& event)const
{
	//filter runway exit get invalid id
	if (event.intNodeID == -1)
		return -1;
	
	if (event.mode == OnExitRunway)
	{
		RunwayExit runwayExit;
		runwayExit.ReadData(event.intNodeID);

		return runwayExit.GetIntersectNodeID(); 
	}
	return event.intNodeID;
}

//return start log item and end item
bool CFlightOperationalReport::IsPassObject(AirsideFlightLogEntry& logEntry,CFlightOperationalParam * pParam,int& startpos,int& endpos,bool bArrival)
{
	int nEventCount = logEntry.getCount();

	CAirsideReportNode startNode = pParam->GetStartObject();
	CAirsideReportNode endNode = pParam->GetEndObject();

	char state;
	if (bArrival)
	{
		state = 'A';
	}
	else
	{
		state = 'D';
	}

	//default
	if (startNode.IsDefault() && endNode.IsDefault())
	{
		bool bFind = false;
		if (bArrival)
		{
			startpos = 0;
			for (int nEvent = nEventCount - 1; nEvent >= 0; --nEvent)
			{
				AirsideFlightEventStruct event = logEntry.getEvent(nEvent);
				if (event.state == state)
				{
					endpos = nEvent;
					bFind = true;
					break;
				}
			}
		}
		else
		{
			endpos = nEventCount-1;
			for (int nEvent = 0; nEvent < nEventCount; ++nEvent)
			{
				AirsideFlightEventStruct event = logEntry.getEvent(nEvent);
				if (event.state == state)
				{
					startpos = nEvent;
					bFind = true;
					break;
				}
			}
		}
		return bFind;
	}
	else if (startNode.IsDefault())
	{
		bool bFind = false;
		if (bArrival)
		{
			startpos = 0;
			bFind = true;
		}
		else
		{
			for (int nEvent = 0; nEvent < nEventCount; ++nEvent)
			{
				AirsideFlightEventStruct event = logEntry.getEvent(nEvent);
				if (event.state == state)
				{
					startpos = nEvent;
					bFind = true;
					break;
				}
			}
		}

		for (int nEvent = 0; nEvent < nEventCount; ++nEvent)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(nEvent);
			if (event.state == state)
			{
				if ((pParam->GetEndObject().GetNodeType() == CAirsideReportNode::ReportNodeType_WayPointStand && pParam->GetEndObject().IncludeObj(event.nodeNO))||
					(pParam->GetEndObject().GetNodeType() == CAirsideReportNode::ReportNodeType_Intersection&&pParam->GetEndObject().IncludeObj(GetIntersectionNodeID(event))))
				{
					endpos = nEvent;
					return bFind;
				}
			}
		}
	}
	else if ( endNode.IsDefault())
	{
		bool bFind = false;
		for (int nEvent = 0; nEvent < nEventCount; ++nEvent)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(nEvent);

			if (event.state == state)
			{
				if ((pParam->GetStartObject().GetNodeType() == CAirsideReportNode::ReportNodeType_WayPointStand &&pParam->GetStartObject().IncludeObj(event.nodeNO))||
					(pParam->GetStartObject().GetNodeType() == CAirsideReportNode::ReportNodeType_Intersection&&pParam->GetStartObject().IncludeObj(GetIntersectionNodeID(event))))
				{
					startpos = nEvent;
					bFind = true;
					break;
				}
			}
		}

		if (!bArrival)
		{
			endpos = nEventCount-1;
			return bFind;
		}
		else
		{
			for (int nEvent = nEventCount - 1; nEvent >= 0; --nEvent)
			{
				AirsideFlightEventStruct event = logEntry.getEvent(nEvent);
				if (event.state == state)
				{
					endpos = nEvent;
					return bFind;
				}
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
			if (event.state == state)
			{
				if ((pParam->GetStartObject().GetNodeType() == CAirsideReportNode::ReportNodeType_WayPointStand &&pParam->GetStartObject().IncludeObj(event.nodeNO))||
					(pParam->GetStartObject().GetNodeType() == CAirsideReportNode::ReportNodeType_Intersection&&pParam->GetStartObject().IncludeObj(GetIntersectionNodeID(event))))
				{
					startpos = nStartEvent;
					bFindStartPos = true;
					break;
				}
			}
			
		}

		if (bFindStartPos == false)
			return false;

		//find endpos
		bool  bFindEndPos = false;
		for (int nEndEvent = 0; nEndEvent < nEventCount; ++nEndEvent)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(nEndEvent);
			if (event.state == state)
			{
				if ((pParam->GetEndObject().GetNodeType() == CAirsideReportNode::ReportNodeType_WayPointStand &&pParam->GetEndObject().IncludeObj(event.nodeNO))||
					(pParam->GetEndObject().GetNodeType() == CAirsideReportNode::ReportNodeType_Intersection&&pParam->GetEndObject().IncludeObj(GetIntersectionNodeID(event))))
				{
					endpos = nEndEvent;
					bFindEndPos = true;
					break;
				}
			}
		}

		if (bFindEndPos == false)
			return false;

		return true;
	}

	return false;
}

long CFlightOperationalReport::CalTowingTime(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{
	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		return 0l;
	}

	//calculate tow event
	long lTime = 0l;

	AirsideFlightEventStruct firstEvent = LogEntry.getEvent(nStartPos);
	for (int nPos = nStartPos+1; nPos <= nEndPos; ++nPos)
	{
		AirsideFlightEventStruct secondEvent = LogEntry.getEvent(nPos);
		if (firstEvent.towOperation == true
			&& secondEvent.towOperation == true)
		{
			long detaTime = secondEvent.time - firstEvent.time;
			lTime += detaTime;
		}

		firstEvent = secondEvent;
	}
	return lTime;
}

void CFlightOperationalReport::CalStayTime(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{
	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		fltOperationalItem.m_lStayTime = 0l;
		return;
	}

	AirsideFlightEventStruct firstEvent = LogEntry.getEvent(nStartPos);
	for (int nPos = nStartPos+1; nPos <= nEndPos; ++nPos)
	{
		AirsideFlightEventStruct secondEvent = LogEntry.getEvent(nPos);
		long detaTime = secondEvent.time - firstEvent.time;

		if (IsPointOnGround(firstEvent) && IsPointOnGround(secondEvent))
		{
			//calculate stand parking time
			if (firstEvent.mode == OnHeldAtStand
				&& secondEvent.mode == OnHeldAtStand)
			{
				fltOperationalItem.m_lStayTime += detaTime;
				//next loop
				firstEvent = secondEvent;
				continue;
			}

			//calculate push back time and unhook time
			if (firstEvent.IsBackUp == true)
			{
				fltOperationalItem.m_lStayTime += detaTime;
				//next loop
				firstEvent = secondEvent;
				continue;
			}

			//deice parking time
			if (firstEvent.deiceParking == true)
			{
				fltOperationalItem.m_lStayTime += detaTime;
				//next loop
				firstEvent = secondEvent;
				continue;
			}

			//calculate temporary parking time
			if (firstEvent.mode == OnHeldAtTempParking)
			{
				fltOperationalItem.m_lStayTime += detaTime;
				//next loop
				firstEvent = secondEvent;
				continue;
			}

			//calculate tow parking time
			if (firstEvent.towOperation == true
				&& secondEvent.towOperation != true)
			{
				fltOperationalItem.m_lStayTime += detaTime;
				//next loop
				firstEvent = secondEvent;
				continue;
			}
		}

		//next loop
		firstEvent = secondEvent;
	}
}

void CFlightOperationalReport::CalcAirDistance(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{
	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		fltOperationalItem.m_fAirDis = 0.0;
		return;
	}

	AirsideFlightEventStruct firstEvent = LogEntry.getEvent(nStartPos);
	for (int nPos = nStartPos+1; nPos <= nEndPos; ++nPos)
	{
		AirsideFlightEventStruct secondEvent = LogEntry.getEvent(nPos);

		float fDisX = secondEvent.x - firstEvent.x;
		float fDisY = secondEvent.y - firstEvent.y;
		float fDisZ = secondEvent.z - firstEvent.z;

		//point one and point two is on air
		if (IsPointOnAir(firstEvent)
			&& IsPointOnAir(secondEvent))
		{
			fltOperationalItem.m_fAirDis += sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);
		}

		//point one on air,point two isn't on air
		if (IsPointOnAir(firstEvent)
			&& !IsPointOnAir(secondEvent))
		{
			fltOperationalItem.m_fAirDis += sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);
		}

		//point one isn't on air, point two on air
		if (!IsPointOnAir(firstEvent)
			&& IsPointOnAir(secondEvent))
		{
			fltOperationalItem.m_fAirDis += sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);
		}

		firstEvent = secondEvent;
	}
}

void CFlightOperationalReport::CalcGroundDistance(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{
	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		fltOperationalItem.m_fGroundDis = 0.0;
		return;
	}

	AirsideFlightEventStruct firstEvent = LogEntry.getEvent(nStartPos);
	for (int nPos = nStartPos+1; nPos <= nEndPos; ++nPos)
	{
		AirsideFlightEventStruct secondEvent = LogEntry.getEvent(nPos);

		float fDisX = secondEvent.x - firstEvent.x;
		float fDisY = secondEvent.y - firstEvent.y;
		float fDisZ = secondEvent.z - firstEvent.z;

		//point one and point two is on ground
		if (IsPointOnGround(firstEvent)
			&& IsPointOnGround(secondEvent))
		{
			fltOperationalItem.m_fGroundDis += sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);

		}

		firstEvent = secondEvent;
	}

	fltOperationalItem.m_fGroundDis -= CalTowAndPushbackDistance(LogEntry,nStartPos,nEndPos,fltOperationalItem);
}

void CFlightOperationalReport::CalcAirTime(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{
	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		fltOperationalItem.m_lAirTime = 0L;
		return;
	}

	AirsideFlightEventStruct firstEvent = LogEntry.getEvent(nStartPos);
	for (int nPos = nStartPos+1; nPos <= nEndPos; ++nPos)
	{
		AirsideFlightEventStruct secondEvent = LogEntry.getEvent(nPos);

		long lTime = secondEvent.time - firstEvent.time;

		//point one and point two is on air
		if (IsPointOnAir(firstEvent)
			&& IsPointOnAir(secondEvent))
		{
			fltOperationalItem.m_lAirTime += lTime;
		}

		//point one on air,point two isn't on air
		if (IsPointOnAir(firstEvent)
			&& !IsPointOnAir(secondEvent))
		{
			fltOperationalItem.m_lAirTime += lTime;
		}

		//point one isn't on air, point two on air
		if (!IsPointOnAir(firstEvent)
			&& IsPointOnAir(secondEvent))
		{
			fltOperationalItem.m_lAirTime += lTime;
		}

		firstEvent = secondEvent;
	}
}

//calculate tow by vehicle and push back operation
float CFlightOperationalReport::CalTowAndPushbackDistance(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{
	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		return 0.0;
	}

	//calculate tow event
	float fDist = 0.0;

	AirsideFlightEventStruct firstEvent = LogEntry.getEvent(nStartPos);
	for (int nPos = nStartPos+1; nPos <= nEndPos; ++nPos)
	{
		AirsideFlightEventStruct secondEvent = LogEntry.getEvent(nPos);

		float fDisX = secondEvent.x - firstEvent.x;
		float fDisY = secondEvent.y - firstEvent.y;
		float fDisZ = secondEvent.z - firstEvent.z;

		if (IsPointOnGround(firstEvent) && IsPointOnGround(secondEvent))
		{
			//tow distance
			if (firstEvent.towOperation == true
				&& secondEvent.towOperation == true)
			{
				fDist += sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);
			}
			else if (firstEvent.IsBackUp == true
				&& secondEvent.IsBackUp == true)//push back distance
			{
				fDist += sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);
			}
		}
		
		firstEvent = secondEvent;
	}
	return fDist;
}

//calculate taxi time
void CFlightOperationalReport::CalcGourndTime(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{
	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		fltOperationalItem.m_lGroundTime = 0L;
		return;
	}

	long lTaxiTime = 0l;
	AirsideFlightEventStruct firstEvent = LogEntry.getEvent(nStartPos);
	for (int nPos = nStartPos+1; nPos <= nEndPos; ++nPos)
	{
		AirsideFlightEventStruct secondEvent = LogEntry.getEvent(nPos);

		long lTime = secondEvent.time - firstEvent.time;

		//point one and point two is on ground
		if (IsPointOnGround(firstEvent)
			&& IsPointOnGround(secondEvent))
		{
			lTaxiTime += lTime;
		}

		firstEvent = secondEvent;
	}

	//calculate tow time
	long lTowTime = CalTowingTime(LogEntry,nStartPos,nEndPos,fltOperationalItem);
	fltOperationalItem.m_lGroundTime = lTaxiTime - lTowTime - fltOperationalItem.m_lStayTime;
}

void CFlightOperationalReport::CalcAirFuel(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{

}

void CFlightOperationalReport::CalcGroundFuel(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{
}

void CFlightOperationalReport::CalcOperatingCost(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{

}

void CFlightOperationalReport::CalcLandRunway(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{
	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		fltOperationalItem.m_strLandRunway = _T("");
		return;
	}

	for (int nPos = 0; nPos < nCount; ++nPos)
	{
		AirsideFlightEventStruct eventNode = LogEntry.getEvent(nPos);

		CString strEventCode(_T(""));
		strEventCode.Format(_T("%c"), eventNode.eventCode);

		if (eventNode.mode == OnLanding)
		{
			int nRunwayID = eventNode.nodeNO;
			Runway runway;
			runway.ReadObject(nRunwayID);

			fltOperationalItem.m_strLandRunway.Format(_T("%s"), runway.GetObjNameString());

			break;
		}
	}
}

void CFlightOperationalReport::CalcLandDist(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{
	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		fltOperationalItem.m_fLandDist = 0.0;
		return;
	}

	AirsideFlightEventStruct firstEvent = LogEntry.getEvent(nStartPos);
	for (int nPos = 0; nPos < nCount; ++nPos)
	{
		AirsideFlightEventStruct secondEvent = LogEntry.getEvent(nPos);

		float fDisX = secondEvent.x - firstEvent.x;
		float fDisY = secondEvent.y - firstEvent.y;
		float fDisZ = secondEvent.z - firstEvent.z;

		//point one and point two is in land dist
		if (IsPointBetweenOnLandingAndOnExitRunway(firstEvent)
			&& IsPointBetweenOnLandingAndOnExitRunway(secondEvent)
			&& firstEvent.mode <= secondEvent.mode)
		{
			fltOperationalItem.m_fLandDist += sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);
		}

		firstEvent = secondEvent;
	}
}

void CFlightOperationalReport::CalcRunwayExit(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{
	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		fltOperationalItem.m_strRunwayExit = _T("");
		return;
	}

	for (int nPos = 0; nPos < nCount; ++nPos)
	{
		AirsideFlightEventStruct eventNode = LogEntry.getEvent(nPos);

		CString strEventCode(_T(""));
		strEventCode.Format(_T("%c"), eventNode.eventCode);

		if (eventNode.mode == OnExitRunway)
		{
			int nRunwayExitID = eventNode.intNodeID;
			RunwayExit runwayExit;
			runwayExit.ReadData(nRunwayExitID);

			fltOperationalItem.m_strRunwayExit.Format(_T("%s"), runwayExit.GetName());

			break;
		}
	}
}

void CFlightOperationalReport::CalcArrStand(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{
	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		fltOperationalItem.m_strArrStand = _T("");
		return;
	}

	for (int nPos = 0; nPos < nCount; ++nPos)
	{
		AirsideFlightEventStruct eventNode = LogEntry.getEvent(nPos);

		if (eventNode.mode == OnEnterStand)
		{
			int nArrStandID = eventNode.nodeNO;
			Stand arrStand;
			arrStand.ReadObject(nArrStandID);

			fltOperationalItem.m_strArrStand.Format(_T("%s"), arrStand.GetObjNameString());

			break;
		}
	}
}

void CFlightOperationalReport::CalcIntStand(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{

}

void CFlightOperationalReport::CalcDepStand(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, AirsideFlightDescStruct& fltDesc, FltOperationalItem& fltOperationalItem)
{
	//arrival only
	if (strlen(fltDesc.depID) <= 0)
	{
		return;
	}

	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		fltOperationalItem.m_strDepStand = _T("");
		return;
	}

	for (int nPos = 0; nPos < nCount; ++nPos)
	{
		AirsideFlightEventStruct eventNode = LogEntry.getEvent(nPos);

		//eventNode.
		if (eventNode.mode == OnHeldAtStand
			&& 0 < eventNode.nodeNO)
		{
			int nDepStandID = eventNode.nodeNO;
			Stand depStand;
			depStand.ReadObject(nDepStandID);

			fltOperationalItem.m_strDepStand.Format(_T("%s"), depStand.GetObjNameString());

			break;
		}
	}
}

void CFlightOperationalReport::CalcTakeoffRunway(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, AirsideFlightDescStruct& fltDesc, FltOperationalItem& fltOperationalItem)
{
	//arrival only
	if (strlen(fltDesc.depID) <= 0)
	{
		return;
	}

	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		fltOperationalItem.m_strTakeoffRunway = _T("");
		return;
	}

	for (int nPos = 0; nPos < nCount; ++nPos)
	{
		AirsideFlightEventStruct eventNode = LogEntry.getEvent(nPos);

		if (eventNode.mode == OnTakeoff
			&& 0 < eventNode.nodeNO)
		{
			int nRunwayID = eventNode.nodeNO;
			Runway runway;
			runway.ReadObject(nRunwayID);

			fltOperationalItem.m_strTakeoffRunway.Format(_T("%s"), runway.GetObjNameString());

			break;
		}
	}
}

void CFlightOperationalReport::CalcTakeoffIntersection(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, AirsideFlightDescStruct& fltDesc, FltOperationalItem& fltOperationalItem)
{
	//arrival only
	if (strlen(fltDesc.depID) <= 0)
	{
		return;
	}

	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		fltOperationalItem.m_strTakeoffIntersection = _T("");
		return;
	}

	for (int nPos = 0; nPos < nCount; ++nPos)
	{
		AirsideFlightEventStruct eventNode = LogEntry.getEvent(nPos);

		CString strEventCode(_T(""));
		strEventCode.Format(_T("%c"), eventNode.eventCode);

		if (eventNode.mode == OnPreTakeoff)
		{
			int nRunwayExitID = eventNode.nodeNO;
			RunwayExit runwayExit;
			runwayExit.ReadData(nRunwayExitID);

			fltOperationalItem.m_strTakeoffIntersection.Format(_T("%s"), runwayExit.GetName());

			break;
		}
	}
}

void CFlightOperationalReport::CalcTakeoffDistance(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, AirsideFlightDescStruct& fltDesc, FltOperationalItem& fltOperationalItem)
{
	//arrival only
	if (strlen(fltDesc.depID) <= 0)
	{
		return;
	}

	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		fltOperationalItem.m_fTakeoffDistance = 0.0;
		return;
	}

	AirsideFlightEventStruct firstEvent = LogEntry.getEvent(nStartPos);
	for (int nPos = 0; nPos < nCount; ++nPos)
	{
		AirsideFlightEventStruct secondEvent = LogEntry.getEvent(nPos);

		float fDisX = secondEvent.x - firstEvent.x;
		float fDisY = secondEvent.y - firstEvent.y;
		float fDisZ = secondEvent.z - firstEvent.z;

		//point one and point two is in takeoff dist
		if (IsPointBetweenOnTakeoffAndOnClimbOut(firstEvent)
			&& IsPointBetweenOnTakeoffAndOnClimbOut(secondEvent)
			&& firstEvent.mode <= secondEvent.mode)
		{
			fltOperationalItem.m_fTakeoffDistance += sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);
		}

		firstEvent = secondEvent;
	}
}

void CFlightOperationalReport::CalcMaxAirSpeed(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{
	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		fltOperationalItem.m_fMaxAirSpeed = 0.0;
		return;
	}

	AirsideFlightEventStruct firstEvent = LogEntry.getEvent(nStartPos);
	for (int nPos = nStartPos+1; nPos <= nEndPos; ++nPos)
	{
		AirsideFlightEventStruct secondEvent = LogEntry.getEvent(nPos);

		float fDisX = secondEvent.x - firstEvent.x;
		float fDisY = secondEvent.y - firstEvent.y;
		float fDisZ = secondEvent.z - firstEvent.z;

		float fDistance = 0.0;
		//point one and point two is on air
		if (IsPointOnAir(firstEvent)
			&& IsPointOnAir(secondEvent))
		{
			fDistance = sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);
			long lTimeBetween = secondEvent.time - firstEvent.time;
			double dTimeBetween = lTimeBetween/100.0;
			if (dTimeBetween < 0.000001)
			{
				firstEvent = secondEvent;
				continue;
			}
			double dDistance = ARCUnit::ConvertLength(fDistance, ARCUnit::CM, ARCUnit::M);
			double dSpeed = dDistance / dTimeBetween;
			double dKnotSpeed = ARCUnit::ConvertVelocity(dSpeed, ARCUnit::MpS, ARCUnit::KNOT);

			if (fltOperationalItem.m_fMaxAirSpeed < dKnotSpeed)
			{
				fltOperationalItem.m_fMaxAirSpeed = (float)dKnotSpeed;
			}
		}

		//point one on air,point two isn't on air
		if (IsPointOnAir(firstEvent)
			&& !IsPointOnAir(secondEvent))
		{
			fDistance = sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);
			long lTimeBetween = secondEvent.time - firstEvent.time;
			double dTimeBetween = lTimeBetween/100.0;
			if (dTimeBetween < 0.000001)
			{
				firstEvent = secondEvent;
				continue;
			}
			double dDistance = ARCUnit::ConvertLength(fDistance, ARCUnit::CM, ARCUnit::M);
			double dSpeed = dDistance / dTimeBetween;
			double dKnotSpeed = ARCUnit::ConvertVelocity(dSpeed, ARCUnit::MpS, ARCUnit::KNOT);

			if (fltOperationalItem.m_fMaxAirSpeed < dKnotSpeed)
			{
				fltOperationalItem.m_fMaxAirSpeed = (float)dKnotSpeed;
			}
		}

		//point one isn't on air, point two on air
		if (!IsPointOnAir(firstEvent)
			&& IsPointOnAir(secondEvent))
		{
			fDistance = sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);
			long lTimeBetween = secondEvent.time - firstEvent.time;
			double dTimeBetween = lTimeBetween/100.0;
			if (dTimeBetween < 0.000001)
			{
				firstEvent = secondEvent;
				continue;
			}
			double dDistance = ARCUnit::ConvertLength(fDistance, ARCUnit::CM, ARCUnit::M);
			double dSpeed = dDistance / dTimeBetween;
			double dKnotSpeed = ARCUnit::ConvertVelocity(dSpeed, ARCUnit::MpS, ARCUnit::KNOT);

			if (fltOperationalItem.m_fMaxAirSpeed < dKnotSpeed)
			{
				fltOperationalItem.m_fMaxAirSpeed = (float)dKnotSpeed;
			}
		}

		firstEvent = secondEvent;
	}
}

void CFlightOperationalReport::CalcMaxTaxiSpeed(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{
	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		fltOperationalItem.m_fMaxTaxiSpeed = 0.0;
		return;
	}

	for (int nPos = nStartPos; nPos <= nEndPos; ++nPos)
	{
		AirsideFlightEventStruct flightEvent = LogEntry.getEvent(nPos);
		if (IsPointOnTaxiway(flightEvent))
		{
			double dSpeed = flightEvent.speed/100;
			if (fltOperationalItem.m_fMaxTaxiSpeed < dSpeed)
			{
				fltOperationalItem.m_fMaxTaxiSpeed = (float)dSpeed;
			}
		}
	}
}

void CFlightOperationalReport::CalcAvgTaxiSpeed(AirsideFlightLogEntry& LogEntry, int nStartPos, int nEndPos, FltOperationalItem& fltOperationalItem)
{
	int nCount = LogEntry.getCount();
	if (nCount == 0
		|| nCount== 1)
	{
		fltOperationalItem.m_fAvgTaxiSpeed = 0.0;
		return;
	}

	vector<double> vTaxiSpeed;
	vector<double> vDistance;
	vector<double> vTime;
	AirsideFlightEventStruct firstEvent = LogEntry.getEvent(nStartPos);
	for (int nPos = nStartPos+1; nPos <= nEndPos; ++nPos)
	{
		AirsideFlightEventStruct secondEvent = LogEntry.getEvent(nPos);

		float fDisX = secondEvent.x - firstEvent.x;
		float fDisY = secondEvent.y - firstEvent.y;
		float fDisZ = secondEvent.z - firstEvent.z;

		float fDistance = 0.0;
		//point one and point two is on taxi
		if (IsPointOnTaxiway(firstEvent)
			&& IsPointOnTaxiway(secondEvent))
		{
			if (firstEvent.towOperation == true || firstEvent.IsBackUp == true)
			{
				firstEvent = secondEvent;
				continue;
			}

			fDistance = sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);
			long lTimeBetween = secondEvent.time - firstEvent.time;
			double dTimeBetween = lTimeBetween/100.0;  //second

			double dDistance = ARCUnit::ConvertLength(fDistance, ARCUnit::CM, ARCUnit::M);

			vDistance.push_back(dDistance);
			vTime.push_back(dTimeBetween);
		}

		//point one on taxi,point two isn't on taxi
		if (IsPointOnTaxiway(firstEvent)
			&& !IsPointOnTaxiway(secondEvent))
		{
			fDistance = sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);
			long lTimeBetween = secondEvent.time - firstEvent.time;
			double dTimeBetween = lTimeBetween/100.0;

			double dDistance = ARCUnit::ConvertLength(fDistance, ARCUnit::CM, ARCUnit::M);

			vDistance.push_back(dDistance);
			vTime.push_back(dTimeBetween);
		}

		//point one isn't on taxi, point two on taxi
		if (!IsPointOnTaxiway(firstEvent)
			&& IsPointOnTaxiway(secondEvent))
		{
			fDistance = sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);
			long lTimeBetween = secondEvent.time - firstEvent.time;
			double dTimeBetween = lTimeBetween/100.0;

			double dDistance = ARCUnit::ConvertLength(fDistance, ARCUnit::CM, ARCUnit::M);

			vDistance.push_back(dDistance);
			vTime.push_back(dTimeBetween);
		}

		firstEvent = secondEvent;
	}	

	ASSERT(vDistance.size() == vTime.size());

	double dTotalDistance = 0.0;
	double dTotalTime     = 0.0;
	for (int i=0; i<(int)vDistance.size(); i++)
	{
		dTotalDistance += vDistance[i];
	}

	for (int i=0; i<(int)vTime.size(); i++)
	{
		dTotalTime += vTime[i];
	}

	double dAvgSpeed = 0.0;
	if (dTotalTime < 0.000001)
	{
		dAvgSpeed = 0.0;
	}
	else
	{
		dAvgSpeed = dTotalDistance / dTotalTime;	
	}

	fltOperationalItem.m_fAvgTaxiSpeed = (float)dAvgSpeed;
}

//return true if point on air, return false if point not on air
bool CFlightOperationalReport::IsPointOnAir(AirsideFlightEventStruct& eventStruct)
{
	if (eventStruct.mode < OnLanding 
		|| ((eventStruct.mode < OnTerminate) && (OnClimbout <= eventStruct.mode)))
	{
		return true;
	}

	return false;
}

//return true if point on ground, return false if point not on ground
bool CFlightOperationalReport::IsPointOnGround(AirsideFlightEventStruct& eventStruct)
{
	if (OnExitRunway <= eventStruct.mode
		&& eventStruct.mode <= OnQueueToRunway)
	{
		return true;
	}

	return false;
}

//return true if point between OnLanding and OnExitRunway
bool CFlightOperationalReport::IsPointBetweenOnLandingAndOnExitRunway(AirsideFlightEventStruct& eventStruct)
{
	if (OnLanding <= eventStruct.mode
		&& eventStruct.mode <= OnExitRunway)
	{
		return true;
	}

	return false;
}

//return true if point between OnTakeoff and OnClimbOut
bool CFlightOperationalReport::IsPointBetweenOnTakeoffAndOnClimbOut(AirsideFlightEventStruct& eventStruct)
{
	if (OnTakeoff <= eventStruct.mode
		&& eventStruct.mode < OnClimbout)
	{
		return true;
	}

	return false;
}

//return true if point on taxiway , return false if point isn't on taxiway
bool CFlightOperationalReport::IsPointOnTaxiway(AirsideFlightEventStruct& eventStruct)
{
	if (eventStruct.mode == OnTaxiToTempParking
		|| eventStruct.mode == OnTaxiToStand
		|| eventStruct.mode == OnTaxiToRunway
		|| eventStruct.mode == OnQueueToRunway
		|| eventStruct.mode == OnTaxiToDeice
		|| eventStruct.mode == OnTaxiToHoldArea)
	{
		return true;
	}

	return false;
}

int CFlightOperationalReport::GetReportType()
{
	return Airside_AircraftOperational;
}

void CFlightOperationalReport::RefreshReport(CParameters * parameter)
{
	CFlightOperationalParam* pParam = (CFlightOperationalParam*)parameter;
	ASSERT(pParam != NULL);

	if (NULL != m_pResult)
	{
		delete m_pResult;
		m_pResult = NULL;
	}

	switch(pParam->getSubType())
	{
	case OPERATIONAL_FLIGHTTYPEVSAIRDISTANCE:
		{
			m_pResult = new CFlightOperationalAirDistanceResult;
			CFlightOperationalAirDistanceResult* pResult = (CFlightOperationalAirDistanceResult*)m_pResult;
			pResult->GenerateResult(m_vResult, parameter);
		}
		break;

	case OPERATIONAL_FLIGHTTYPEVSGROUNDDISTANCE:
		{
			m_pResult = new CFlightOperationalGroundDistanceResult;
			CFlightOperationalGroundDistanceResult* pResult = (CFlightOperationalGroundDistanceResult*)m_pResult;
			pResult->GenerateResult(m_vResult, parameter);
		}
		break;

	default:
		break;
	}
}

//CAirsideReportBaseResult *GetReportResult(){ return m_pResult;}
void CFlightOperationalReport::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("Flight ID"), LVCFMT_LEFT, 100);	
	cxListCtrl.InsertColumn(1, _T("Air distance(km)"), LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(2, _T("Taxi dis(m)"), LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(3, _T("Air time"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(4, _T("Taxi Time"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(5, _T("Stay Time"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(6, _T("Air Fuel(kg)"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(7, _T("Ground Fuel(kg)"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(8, _T("Operating cost($)"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(9, _T("Land runway"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(10, _T("Land dist(m)"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(11, _T("Runway exit"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(12, _T("Arr stand"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(13, _T("Arr stand planed"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(14, _T("Int stand"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(15, _T("Int stand planed"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(16, _T("Dep stand"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(17, _T("Dep stand planed"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(18, _T("Takeoff runway"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(19, _T("Takeoff intersection"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(20, _T("Takeoff Distance(m)"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(21, _T("Max airspeed(kts)"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(22, _T("Max taxispeed(m/s)"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(23, _T("Avg taxispeed(m/s)"),LVCFMT_LEFT,100);

	if (piSHC)
	{
		piSHC->ResetAll();
		piSHC->SetDataType(0,dtSTRING);
		piSHC->SetDataType(1,dtDEC);
		piSHC->SetDataType(2,dtDEC);
		piSHC->SetDataType(3,dtTIME);
		piSHC->SetDataType(4,dtTIME);
		piSHC->SetDataType(5,dtTIME);
		piSHC->SetDataType(6,dtDEC);
		piSHC->SetDataType(7,dtDEC);
		piSHC->SetDataType(8,dtDEC);
		piSHC->SetDataType(9,dtSTRING);
		piSHC->SetDataType(10,dtDEC);
		piSHC->SetDataType(11,dtSTRING);
		piSHC->SetDataType(12,dtSTRING);
		piSHC->SetDataType(13,dtSTRING);
		piSHC->SetDataType(14,dtSTRING);
		piSHC->SetDataType(15,dtSTRING);
		piSHC->SetDataType(16,dtSTRING);
		piSHC->SetDataType(17,dtSTRING);
		piSHC->SetDataType(18,dtSTRING);
		piSHC->SetDataType(19,dtSTRING);
		piSHC->SetDataType(20,dtDEC);
		piSHC->SetDataType(21,dtDEC);
		piSHC->SetDataType(22,dtDEC);
		piSHC->SetDataType(23,dtDEC);
	}
}

BOOL CFlightOperationalReport::ExportListData(ArctermFile& _file,CParameters * parameter) 
{
	for (int i=0; i<(int)m_vResult.size(); i++)
	{
		FltOperationalItem fltOperationalItem = m_vResult[i];

		//Flight ID
		CString strID = _T("");
		if (fltOperationalItem.m_bArrival)
		{
			if (fltOperationalItem.m_fltDesc._arrID.HasValue())
			{
				strID = fltOperationalItem.m_fltDesc._airline.GetValue();
				strID += fltOperationalItem.m_fltDesc._arrID.GetValue();			
			}
		}
		else if (fltOperationalItem.m_fltDesc._depID.HasValue())
		{
			strID = fltOperationalItem.m_fltDesc._airline.GetValue();
			strID += fltOperationalItem.m_fltDesc._depID.GetValue();			
		}
		_file.writeField(strID);

		//Air distance(km)
		CString strText;
		double dAirDistance = ARCUnit::ConvertLength(fltOperationalItem.m_fAirDis, ARCUnit::CM, ARCUnit::KM);
		strText.Format(_T("%.2f"), dAirDistance);
		_file.writeField( strText);
		//Ground dis(m)
		double dGroundDistance = ARCUnit::ConvertLength(fltOperationalItem.m_fGroundDis, ARCUnit::CM, ARCUnit::M);
		strText.Format(_T("%.2f"), dGroundDistance);
		_file.writeField(strText);
		//Air time(mins)
		ElapsedTime estAirTime(long(fltOperationalItem.m_lAirTime / 100.0+0.5));
		//strText.Format(_T("%d"), estAirTime.asMinutes());
		strText.Format(_T("%s"), estAirTime.printTime(1));
		_file.writeField(strText);
		//Ground Time(mins)
		ElapsedTime estGroundTime(long(fltOperationalItem.m_lGroundTime / 100.0+0.5));
		//strText.Format(_T("%d"), estGroundTime.asMinutes());
		strText.Format(_T("%s"), estGroundTime.printTime(1));
		_file.writeField( strText);

		//Stay Time(mins)
		ElapsedTime estStayTime(long(fltOperationalItem.m_lStayTime / 100.0+0.5));
		strText.Format(_T("%s"),estStayTime.printTime(1));
		_file.writeField(strText);

		//Air Fuel(kg)
		strText.Format(_T("%.2f"), fltOperationalItem.m_fAirFuel);
		_file.writeField(strText);
		//Ground Fuel(kg)
		strText.Format(_T("%.2f"), fltOperationalItem.m_fGroundFuel);
		_file.writeField( strText);
		//Operating cost($)
		strText.Format(_T("%.2f"), fltOperationalItem.m_fOperatingCost);
		_file.writeField(strText);
		//Land runway
		_file.writeField( fltOperationalItem.m_strLandRunway);
		//Land dist(m)
		double dLandDist = ARCUnit::ConvertLength(fltOperationalItem.m_fLandDist, ARCUnit::CM, ARCUnit::M);
		strText.Format(_T("%.2f"), dLandDist);
		_file.writeField(strText);
		//Runway exit
		_file.writeField(fltOperationalItem.m_strRunwayExit);
		//Arr stand
		_file.writeField(fltOperationalItem.m_strArrStand);
		//planed arr stand
		_file.writeField(fltOperationalItem.m_fltDesc.arrStandPlaned);
		//Int stand
		_file.writeField(fltOperationalItem.m_strIntStand);
		//planed int stand
		_file.writeField(fltOperationalItem.m_fltDesc.intStandPlaned);
		//Dep stand
		_file.writeField(fltOperationalItem.m_strDepStand);
		//planed dep stand
		_file.writeField(fltOperationalItem.m_fltDesc.depStandPlaned);
		//Takeoff runway
		_file.writeField(fltOperationalItem.m_strTakeoffRunway);
		//Takeoff intersection
		_file.writeField( fltOperationalItem.m_strTakeoffIntersection);
		//Takeoff Distance(m)
		double dTakeoffDistance = ARCUnit::ConvertLength(fltOperationalItem.m_fTakeoffDistance, ARCUnit::CM, ARCUnit::M);
		strText.Format(_T("%.2f"), dTakeoffDistance);
		_file.writeField(strText);
		//Max airspeed(kts)
		//double dAirSpeed = ARCUnit::ConvertVelocity(fltOperationalItem.m_fMaxAirSpeed, ARCUnit::MpS, ARCUnit::KNOT);
		strText.Format(_T("%.2f"), fltOperationalItem.m_fMaxAirSpeed);
		_file.writeField( strText);
		//Max taxispeed(kts)
		//double dTaxiSpeed = ARCUnit::ConvertVelocity(fltOperationalItem.m_fMaxTaxiSpeed, ARCUnit::MpS, ARCUnit::KNOT);
		strText.Format(_T("%.2f"), fltOperationalItem.m_fMaxTaxiSpeed);
		_file.writeField(strText);
		//Avg taxispeed(kts)
		//double dAvgTaxiSpeed = ARCUnit::ConvertVelocity(fltOperationalItem.m_fAvgTaxiSpeed, ARCUnit::MpS, ARCUnit::KNOT);
		strText.Format(_T("%.2f"), fltOperationalItem.m_fAvgTaxiSpeed);
		_file.writeField(strText);
		_file.writeLine() ;
	}
	return TRUE ;
}
void CFlightOperationalReport::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	for (int i=0; i<(int)m_vResult.size(); i++)
	{
		FltOperationalItem fltOperationalItem = m_vResult[i];

		//Flight ID
		CString strID = _T("");
		if (fltOperationalItem.m_bArrival)
		{
			if (fltOperationalItem.m_fltDesc._arrID.HasValue())
			{
				strID = fltOperationalItem.m_fltDesc._airline.GetValue();
				strID += fltOperationalItem.m_fltDesc._arrID.GetValue();			
			}
		}
		else if (fltOperationalItem.m_fltDesc._depID.HasValue())
		{
			strID = fltOperationalItem.m_fltDesc._airline.GetValue();
			strID += fltOperationalItem.m_fltDesc._depID.GetValue();			
		}
		cxListCtrl.InsertItem(i, strID);
		cxListCtrl.SetItemData(i, fltOperationalItem.m_ID);

		//Air distance(km)
		CString strText;
		double dAirDistance = ARCUnit::ConvertLength(fltOperationalItem.m_fAirDis, ARCUnit::CM, ARCUnit::KM);
		strText.Format(_T("%.2f"), dAirDistance);
		cxListCtrl.SetItemText(i, 1, strText);
		//Ground dis(m)
		double dGroundDistance = ARCUnit::ConvertLength(fltOperationalItem.m_fGroundDis, ARCUnit::CM, ARCUnit::M);
		strText.Format(_T("%.2f"), dGroundDistance);
		cxListCtrl.SetItemText(i, 2, strText);
		//Air time(mins)
		ElapsedTime estAirTime(long(fltOperationalItem.m_lAirTime / 100.0+0.5));
		//strText.Format(_T("%d"), estAirTime.asMinutes());
		strText.Format(_T("%s"), estAirTime.printTime(1));
		cxListCtrl.SetItemText(i, 3, strText);
		//taxi Time(mins)
		ElapsedTime estGroundTime(long(fltOperationalItem.m_lGroundTime / 100.0+0.5));
		//strText.Format(_T("%d"), estGroundTime.asMinutes());
		strText.Format(_T("%s"), estGroundTime.printTime(1));
		cxListCtrl.SetItemText(i, 4, strText);

		ElapsedTime estStayTime(long(fltOperationalItem.m_lStayTime / 100.0+0.5));
		strText.Format(_T("%s"),estStayTime.printTime(1));
		cxListCtrl.SetItemText(i, 5, strText);

		//Air Fuel(kg)
		strText.Format(_T("%.2f"), fltOperationalItem.m_fAirFuel);
		cxListCtrl.SetItemText(i, 6, strText);
		//Ground Fuel(kg)
		strText.Format(_T("%.2f"), fltOperationalItem.m_fGroundFuel);
		cxListCtrl.SetItemText(i, 7, strText);
		//Operating cost($)
		strText.Format(_T("%.2f"), fltOperationalItem.m_fOperatingCost);
		cxListCtrl.SetItemText(i, 8, strText);
		//Land runway
		cxListCtrl.SetItemText(i, 9, fltOperationalItem.m_strLandRunway);
		//Land dist(m)
		if (fltOperationalItem.m_bArrival)
		{
			double dLandDist = ARCUnit::ConvertLength(fltOperationalItem.m_fLandDist, ARCUnit::CM, ARCUnit::M);
			strText.Format(_T("%.2f"), dLandDist);
			cxListCtrl.SetItemText(i, 10, strText);
		}
		
		//Runway exit
		cxListCtrl.SetItemText(i, 11, fltOperationalItem.m_strRunwayExit);
		//Arr stand
		cxListCtrl.SetItemText(i, 12, fltOperationalItem.m_strArrStand);
		
		if (fltOperationalItem.m_bArrival)
		{
			//planed arr stand
			cxListCtrl.SetItemText(i, 13, fltOperationalItem.m_fltDesc.arrStandPlaned);
		}
		else
		{
			//Int stand
			cxListCtrl.SetItemText(i, 14, fltOperationalItem.m_strIntStand);
			//planed int stand
			cxListCtrl.SetItemText(i, 15, fltOperationalItem.m_fltDesc.intStandPlaned);
			//Dep stand
			cxListCtrl.SetItemText(i, 16, fltOperationalItem.m_strDepStand);
			//planed dep stand
			cxListCtrl.SetItemText(i, 17, fltOperationalItem.m_fltDesc.depStandPlaned);

			//Takeoff Distance(m)
			double dTakeoffDistance = ARCUnit::ConvertLength(fltOperationalItem.m_fTakeoffDistance, ARCUnit::CM, ARCUnit::M);
			strText.Format(_T("%.2f"), dTakeoffDistance);
			cxListCtrl.SetItemText(i, 19, strText);
		}
		
		//Takeoff runway
		cxListCtrl.SetItemText(i, 18, fltOperationalItem.m_strTakeoffRunway);
		//Takeoff intersection
		cxListCtrl.SetItemText(i, 20, fltOperationalItem.m_strTakeoffIntersection);
		
		//Max airspeed(kts)
		//double dAirSpeed = ARCUnit::ConvertVelocity(fltOperationalItem.m_fMaxAirSpeed, ARCUnit::MpS, ARCUnit::KNOT);
		strText.Format(_T("%.2f"), fltOperationalItem.m_fMaxAirSpeed);
		cxListCtrl.SetItemText(i, 21, strText);
		//Max taxispeed(kts)
		//double dTaxiSpeed = ARCUnit::ConvertVelocity(fltOperationalItem.m_fMaxTaxiSpeed, ARCUnit::MpS, ARCUnit::KNOT);
		strText.Format(_T("%.2f"), fltOperationalItem.m_fMaxTaxiSpeed);
		cxListCtrl.SetItemText(i, 22, strText);
		//Avg taxispeed(kts)
		//double dAvgTaxiSpeed = ARCUnit::ConvertVelocity(fltOperationalItem.m_fAvgTaxiSpeed, ARCUnit::MpS, ARCUnit::KNOT);
		strText.Format(_T("%.2f"), fltOperationalItem.m_fAvgTaxiSpeed);
		cxListCtrl.SetItemText(i, 23, strText);
	}
}

bool CFlightOperationalReport::fits(CParameters *parameter, const AirsideFlightDescStruct& fltDesc,FlightConstraint& fltCons,FLTCNSTR_MODE& fltConMode)
{
	bool bArrFit = false;
	bool bDepFit = false;
	size_t nFltConsCount = parameter->getFlightConstraintCount();
	for (size_t nfltCons =0; nfltCons < nFltConsCount; ++ nfltCons)
	{
		if(parameter->getFlightConstraint(nfltCons).fits(fltDesc))
		{
			fltCons = parameter->getFlightConstraint(nfltCons);
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

BOOL CFlightOperationalReport::ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType )
{
	int size = 0 ;
	size = (int)m_vResult.size() ;
	_file.writeInt(size) ;
	_file.writeLine() ;

	for (int i = 0 ; i < size ;i++)
	{
		if(!m_vResult[i].ExportFile(_file))
			return FALSE ;
	}
	return TRUE ;
}
BOOL CFlightOperationalReport::ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType )
{
	int size = 0 ;
	if(!_file.getInteger(size))
		return FALSE ;
	for (int i = 0 ; i < size ;i++)
	{
		_file.getLine() ;
		FltOperationalItem operItem(m_AirportDB);
		if(!operItem.ImportFile(_file))
			return FALSE ;
		m_vResult.push_back(operItem) ;
	}
	return TRUE ;
}
BOOL FltOperationalItem::ExportFile(ArctermFile& _file)
{
	_file.writeField(m_strFltType);
	_file.writeInt(m_bArrival?1:0) ;
	_file.writeFloat(m_fAirDis);
	_file.writeFloat(m_fGroundDis);
	_file.writeInt(m_lAirTime);
	_file.writeInt(m_lGroundTime);
	_file.writeInt(m_lStayTime);
	_file.writeFloat(m_fAirFuel);
	_file.writeFloat(m_fGroundFuel);
	_file.writeFloat(m_fOperatingCost);
	_file.writeField(m_strLandRunway);
	_file.writeFloat(m_fLandDist);
	_file.writeField(m_strRunwayExit);
	_file.writeField(m_strArrStand);
	_file.writeField(m_strIntStand);
	_file.writeField(m_strDepStand);
	_file.writeField(m_strTakeoffRunway);
	_file.writeField(m_strTakeoffIntersection);
	_file.writeFloat(m_fTakeoffDistance);
	_file.writeFloat(m_fMaxAirSpeed);
	_file.writeFloat(m_fMaxTaxiSpeed);
	_file.writeFloat(m_fAvgTaxiSpeed);

	TCHAR th[1024] = {0} ;
	m_fltCons.WriteSyntaxStringWithVersion(th) ;
	_file.writeField(th) ;
	_file.writeLine() ;

	m_fltDesc.ExportFile(_file) ;
	return TRUE ;
}
BOOL FltOperationalItem::ImportFile(ArctermFile& _file)
{
	TCHAR th[1024] = {0} ;
	_file.getField(th,1024) ;
	m_strFltType.Format(_T("%s"),th) ;
	int res = 0 ;
	_file.getInteger(res) ;
	m_bArrival = res>0?true:false;
	_file.getFloat(m_fAirDis);

	_file.getFloat(m_fGroundDis);
	_file.getInteger(m_lAirTime);
	_file.getInteger(m_lGroundTime);
	_file.getInteger(m_lStayTime);
	_file.getFloat(m_fAirFuel);
	_file.getFloat(m_fGroundFuel);
	_file.getFloat(m_fOperatingCost);

	_file.getField(th,1024) ;
	m_strLandRunway.Format(_T("%s"),th);

	_file.getFloat(m_fLandDist);
	_file.getField(th,1024);
	m_strRunwayExit.Format(_T("%s"),th);

	_file.getField(th,1024) ;
	m_strArrStand.Format(_T("%s"),th);

	_file.getField(th,1024) ;
	m_strIntStand.Format(_T("%s"),th);

	_file.getField(th,1024) ;
	m_strDepStand.Format(_T("%s"),th);

	_file.getField(th,1024) ;
	m_strTakeoffRunway.Format(_T("%s"),th);

	_file.getField(th,1024) ;
	m_strTakeoffIntersection.Format(_T("%s"),th) ;

	_file.getFloat(m_fTakeoffDistance) ;
	_file.getFloat(m_fMaxAirSpeed) ;
	_file.getFloat(m_fMaxTaxiSpeed) ;
	_file.getFloat(m_fAvgTaxiSpeed) ;

	_file.getField(th,1024) ;
	m_fltCons.setConstraintWithVersion(th) ;

	_file.getLine() ;
	m_fltDesc.ImportFile(_file) ;
	return TRUE ;
}