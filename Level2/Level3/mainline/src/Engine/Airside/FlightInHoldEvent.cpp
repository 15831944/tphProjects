#include "StdAfx.h"
#include ".\flightinholdevent.h"
#include "Clearance.h"
#include "AirsideFlightInSim.h"
#include "AirspaceHoldInSim.h"
#include "../../Common/Path2008.h"
#include "../../Common/ARCMathCommon.h"
#include "SimulationErrorShow.h"
#include "AirRoutePointInSim.h"
#include "AirspaceResourceManager.h"
#include "../../Results/AirsideFllightRunwayDelayLog.h"

const ElapsedTime MAXHOLDINGTIME = ElapsedTime(6*60*60L);
FlightInHoldEvent::FlightInHoldEvent(AirsideFlightInSim* flt, AirspaceHoldInSim* pHold, ElapsedTime tHoldTime)
:CAirsideMobileElementEvent(flt)
{
	m_pCFlight = flt;
	m_pHold = pHold;
	m_tHoldTime = tHoldTime;

}

FlightInHoldEvent::~FlightInHoldEvent(void)
{
}

void FlightInHoldEvent::stepIt(const ElapsedTime& eTime,CPoint2008& pt)
{
	//ASSERT(m_pCFlight->GetUID() != 34);
	double totalLength = m_pHold->GetPath().GetTotalLength();
	double dist = 0.0;
	if (m_pCFlight->GetResource() == m_pHold)	//already enter hold
		dist = m_pCFlight->GetDistInResource() + double(eTime - m_pCFlight->GetTime())* m_pCFlight->GetSpeed();

	pt = m_pHold->GetDistancePoint(((int)dist)% ((int)totalLength));

	if (pt == m_pCFlight->GetPosition())
		pt.setZ(m_pCFlight->GetCurState().m_dAlt);
	else
	{

		//flight down to next height level, the descend angle must no more than 3 degree
		double dAlt =  m_pHold->getAvailableHeight(m_pCFlight);
		if (dAlt != m_pCFlight->GetCurState().m_dAlt)
		{
			double dStep = m_pHold->getAvailableHeight(m_pCFlight) - m_pCFlight->GetCurState().m_dAlt;
			double dStepHeight = (dist-m_pCFlight->GetDistInResource())*tan(ARCMath::DegreesToRadians(3.0));
			if (dStep >0)
				dAlt = m_pCFlight->GetCurState().m_dAlt + (dStepHeight<dStep?dStepHeight:dStep);
			else 
				dAlt = m_pCFlight->GetCurState().m_dAlt - (dStepHeight<-dStep?dStepHeight:-dStep);				
		}
		pt.setZ(dAlt);
	}

	ClearanceItem nextItem(m_pHold,OnWaitInHold,((int)dist)% ((int)totalLength));
	nextItem.SetPosition(pt);
	nextItem.SetTime(eTime);
	nextItem.SetSpeed(m_pCFlight->GetSpeed());
	nextItem.SetAltitude(pt.getZ());

	m_pCFlight->PerformClearanceItem(nextItem);
}

int FlightInHoldEvent::Process()
{
	if (m_pCFlight->GetMode() == OnTerminate)
		return 0;


	//ASSERT(m_pCFlight->GetUID() != 34);
	double totalLength = m_pHold->GetPath().GetTotalLength();
	double dist = 0.0;
	if (m_pCFlight->GetResource() == m_pHold)	//already enter hold
		dist = m_pCFlight->GetDistInResource() + double(time - m_pCFlight->GetTime())* m_pCFlight->GetSpeed();

	CPoint2008 pos = m_pHold->GetDistancePoint(((int)dist)% ((int)totalLength));

	if (pos == m_pCFlight->GetPosition())
		pos.setZ(m_pCFlight->GetCurState().m_dAlt);
	else
	{

		//flight down to next height level, the descend angle must no more than 3 degree
		double dAlt =  m_pHold->getAvailableHeight(m_pCFlight);
		if (dAlt != m_pCFlight->GetCurState().m_dAlt)
		{
			double dStep = m_pHold->getAvailableHeight(m_pCFlight) - m_pCFlight->GetCurState().m_dAlt;
			double dStepHeight = (dist-m_pCFlight->GetDistInResource())*tan(ARCMath::DegreesToRadians(3.0));
			if (dStep >0)
				dAlt = m_pCFlight->GetCurState().m_dAlt + (dStepHeight<dStep?dStepHeight:dStep);
			else 
				dAlt = m_pCFlight->GetCurState().m_dAlt - (dStepHeight<-dStep?dStepHeight:-dStep);				
		}
		pos.setZ(dAlt);
	}

	ClearanceItem nextItem(m_pHold,OnWaitInHold,((int)dist)% ((int)totalLength));
	nextItem.SetPosition(pos);
	nextItem.SetTime(time);
	nextItem.SetSpeed(m_pCFlight->GetSpeed());
	nextItem.SetAltitude(pos.getZ());

	m_pCFlight->PerformClearanceItem(nextItem);

	if (m_pHold != NULL)		//the flight max holding time cannot exceed 24h
	{
		AirWayPointInSim* pWaypoint = m_pHold->GetWaypointInSim();
		ElapsedTime tEnter = m_pHold->GetOccupyInstance(m_pCFlight).GetEnterTime();
		if (tEnter + MAXHOLDINGTIME <= time)
		{

			CString strWarn;
			strWarn.Format("The Flight's holding time in %s too long",m_pHold->PrintResource()) ;
			CString strError = _T("AIRCRAFT TERMINATE");
			AirsideSimErrorShown::SimWarning(m_pCFlight,strWarn,strError);

			ClearanceItem newItem(NULL, OnTerminate,0);
			nextItem.SetTime(time);
			nextItem.SetSpeed(0);

			m_pCFlight->PerformClearanceItem(newItem);
			return 0;
		}

		ElapsedTime tMinLap = ElapsedTime((0.33*totalLength)/m_pCFlight->GetSpeed());
		if (m_tHoldTime >= 0L && (m_tHoldTime - tEnter) < tMinLap)
		{
			m_tHoldTime = tEnter + tMinLap;
		}
	}


	if(m_pHold->GetHeadFlightCount(m_pCFlight)>0)	//the flight is not at the lowest level in hold
	{
		double stepLength = totalLength/8.0;
		ElapsedTime nextTime = time + ElapsedTime(stepLength/m_pCFlight->GetSpeed());
		if (m_tHoldTime > 0L && nextTime > m_tHoldTime )
			m_tHoldTime = (nextTime + 10L);

		//ASSERT(m_pCFlight->GetUID() != 31);
		FlightInHoldEvent* pNextEvent = new FlightInHoldEvent(m_pCFlight,m_pHold, m_tHoldTime);
		pNextEvent->setTime(nextTime);
		pNextEvent->addEvent();

		return 0;
	}

	CPoint2008 WPpos = m_pHold->GetDistancePoint(0.0);
	ElapsedTime dTime = ElapsedTime(pos.distance(WPpos)/m_pCFlight->GetSpeed());

	if ((m_tHoldTime >0L && (time+dTime) >= m_tHoldTime))
	{
		ClearanceItem newItem((AirsideResource*)m_pHold->GetWaypointInSim() ,OnWaitInHold, 0.0);
		WPpos.setZ(pos.getZ());
		newItem.SetPosition(WPpos);
		newItem.SetTime(time+dTime);
		newItem.SetSpeed(m_pCFlight->GetSpeed());
		newItem.SetAltitude(pos.getZ());
		Clearance nextClearance;
		ClearanceItem newItemCopy = newItem;
		bool bExit = m_pHold->GetAirRouteNetwork()->IsFlightNeedHolding(m_pCFlight,m_pHold,newItemCopy,nextClearance);
		if (bExit)
		{
			if (m_pCFlight->GetCurDelayLog()->mConflictLocation == FlightConflict::RUNWAY )
			{
				ElapsedTime tAvailableTime = newItem.GetTime();
				ElapsedTime tDelay = m_pCFlight->GetCurDelayLog()->mDelayTime;
				ElapsedTime tStartTime = tAvailableTime - tDelay;
				ResourceDesc resDesc;
				m_pCFlight->GetLandingRunway()->getDesc(resDesc);
				AirsideFlightRunwayDelayLog* pLog = new AirsideFlightRunwayDelayLog(resDesc, tStartTime.asSeconds(), OnLanding, tAvailableTime.asSeconds(), tDelay.asSeconds(), FlightRunwayDelay::LandingRoll);
				pLog->sReasonDetail = "Wave crossings";
				m_pCFlight->LogEventItem(pLog);
			}

			m_pCFlight->EndDelay(newItem);			//end holding delay
			if (nextClearance.GetItemCount())
			{
				ClearanceItem& firstItem = nextClearance.ItemAt(0);
				ElapsedTime startTime = time;
				ElapsedTime endTime = firstItem.GetTime();
				ElapsedTime detaTime = endTime - startTime;
				if (detaTime > 0l && firstItem.GetPosition() == newItem.GetPosition())
				{
					double stepLength = totalLength/8.0;
					ElapsedTime nextTime = ElapsedTime(stepLength/m_pCFlight->GetSpeed());
					startTime += nextTime;
					CPoint2008 pt;
					for (; startTime < endTime; startTime += nextTime)
					{
						stepIt(startTime,pt);
					}
					stepIt(endTime,pt);
					////step end item
					//{
					//	CPoint2008 WPpos = m_pHold->GetDistancePoint(0.0);
					//	ClearanceItem newItem((AirsideResource*)m_pHold->GetWaypointInSim() ,OnWaitInHold, 0.0);
					//	WPpos.setZ(pt.getZ());
					//	newItem.SetPosition(WPpos);
					//	newItem.SetTime(endTime);
					//	newItem.SetSpeed(m_pCFlight->GetSpeed());
					//	newItem.SetAltitude(pt.getZ());
					//	m_pCFlight->PerformClearanceItem(newItem);
					//}
					firstItem.SetPosition(WPpos);
					firstItem.SetAltitude(pt.getZ());
				}
				else
				{
					m_pCFlight->PerformClearanceItem(newItem);
				}
			}
			else
			{
				m_pCFlight->PerformClearanceItem(newItem);
			}
			m_pCFlight->PerformClearance(nextClearance);
			FlightWakeupEvent* pEvent = new FlightWakeupEvent(m_pCFlight);
			pEvent->setTime(m_pCFlight->GetTime());
			pEvent->addEvent();

		}

		return 0;
	}

	double stepLength = totalLength/8.0;
	ElapsedTime nextTime = time + ElapsedTime(stepLength/m_pCFlight->GetSpeed());
	if ( m_tHoldTime > 0L && nextTime > m_tHoldTime)
		nextTime = m_tHoldTime;

	//ASSERT(m_pCFlight->GetUID() != 31);
	FlightInHoldEvent* pNextEvent = new FlightInHoldEvent(m_pCFlight,m_pHold, m_tHoldTime);
	pNextEvent->setTime(nextTime);
	pNextEvent->addEvent();

	return 0;
	
}

///////////////////////////////////////////////////////////////////
FlightWakeupEvent::FlightWakeupEvent(AirsideFlightInSim* flt)
:CAirsideMobileElementEvent(flt)
{
	m_pCFlight = flt;
}

FlightWakeupEvent::~FlightWakeupEvent(void)
{
}

int FlightWakeupEvent::Process()
{
	m_pCFlight->WakeUp(time);
	return 0;
}