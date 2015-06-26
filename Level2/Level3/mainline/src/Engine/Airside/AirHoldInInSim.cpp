#include "StdAfx.h"
#include ".\airholdininsim.h"
#include ".\AirsideFlightInSim.h"
#include "AirsideResourceManager.h"
//#include "AirspaceResourceManager.h"
#include "AirTrafficController.h"

AirEntrySystemHoldInInSim::~AirEntrySystemHoldInInSim(void)
{
}

AirEntrySystemHoldInInSim::AirEntrySystemHoldInInSim( const ARCVector3& dir, const CPoint2008& pos )
{
	m_level0pos = pos;
	m_dir = dir;
}

void AirEntrySystemHoldInInSim::RemoveFlightInQueue( AirsideFlightInSim* pFlight )
{
// 	FlightListType oldflts = m_flightQueue;
// 	m_flightQueue.clear();
// 	std::remove_copy(oldflts.begin(),oldflts.end(),m_flightQueue.begin(),pFlight);
	FlightListType::iterator iter = std::find(m_flightQueue.begin(),m_flightQueue.end(),pFlight);
	if (iter != m_flightQueue.end())
	{
		m_flightQueue.erase(iter);
	}
}

void AirEntrySystemHoldInInSim::AddFlightToQueue( AirsideFlightInSim* pFlight )
{
	if( std::find(m_flightQueue.begin(),m_flightQueue.end(),pFlight)==m_flightQueue.end() )
	{
		m_flightQueue.push_back(pFlight);
	}
}

AirsideFlightInSim* AirEntrySystemHoldInInSim::GetFrontFlight() const
{
	if (m_flightQueue.empty())
		return NULL;
	
	return m_flightQueue.front();
}

bool AirEntrySystemHoldInInSim::isFlightInQ( AirsideFlightInSim* pFlight ) const
{
	for(FlightListType::const_iterator itr = m_flightQueue.begin(); itr!=m_flightQueue.end(); ++itr)
	{
		if(pFlight == *itr)
			return true;
	}
	return false;
}

CString AirEntrySystemHoldInInSim::PrintResource() const
{
	CString strPrint;
	strPrint = GetTypeName();
	return strPrint;
}

CPoint2008 AirEntrySystemHoldInInSim::GetDistancePoint( double dist ) const
{
	return m_level0pos;
}

AirsideResource::ResourceType AirEntrySystemHoldInInSim::GetType() const
{
	return ResType_AirEntrySysHold;
}

CString AirEntrySystemHoldInInSim::GetTypeName() const
{
	return _T("AirEntrySysHold");
}

void AirEntrySystemHoldInInSim::writeFlightLog( AirsideFlightInSim* pFlight, const ElapsedTime& exitHoldTime )
{
	OccupancyInstance flightinstance  = GetOccupyInstance(pFlight);
	if(!flightinstance.IsValid() || !flightinstance.IsEnterTimeValid() )return;
	ElapsedTime entryTime = flightinstance.GetEnterTime();

	std::vector<ElapsedTime> otherFlightExitTime;   //the time other flight exit hold when the flight enter the hold 
	for(OccupancyTable::iterator itr= m_vOccupancyTable.begin();itr!=m_vOccupancyTable.end();++itr)
	{
		OccupancyInstance& ocyInst = *itr;
		if( ocyInst.GetExitTime() >= entryTime && ocyInst.GetFlight()!=pFlight)
		{ 
			otherFlightExitTime.push_back(ocyInst.GetExitTime());
		}
	}
	
	std::sort(otherFlightExitTime.begin(),otherFlightExitTime.end());
	AirsideFlightState& fltstate = pFlight->GetCurState();	
	fltstate.m_pPosition = GetLevelPos(otherFlightExitTime.size());
	fltstate.m_dAlt = fltstate.m_pPosition.getZ();
	fltstate.m_tTime = entryTime;
	pFlight->WriteLog();

	for(int i=0;i<(int)otherFlightExitTime.size();++i)
	{
		fltstate.m_tTime = otherFlightExitTime[i] + ElapsedTime(10L);
		fltstate.m_pPosition += CPoint2008(m_dir[VX],m_dir[VY],m_dir[VZ]);
		pFlight->WriteLog();

		fltstate.m_pPosition = GetLevelPos(otherFlightExitTime.size()-i-1);

		fltstate.m_dAlt = fltstate.m_pPosition.getZ();
		pFlight->WriteLog();

	}
	fltstate.m_pPosition += CPoint2008(m_dir[VX],m_dir[VY],m_dir[VZ]);

	fltstate.m_tTime = max(exitHoldTime,fltstate.m_tTime);
	pFlight->WriteLog();
}

const static double dLevelAlt = 10000.0;
CPoint2008 AirEntrySystemHoldInInSim::GetLevelPos( int idx ) const
{
	return m_level0pos + CPoint2008(0,0,idx * dLevelAlt );
}

void AirEntrySystemHoldInInSim::OnFlightEnter( AirsideFlightInSim* pFlight, const ElapsedTime& enterTime )
{
	
	AirsideFlightState& fltstate = pFlight->GetCurState();
	//get level
	int iInHoldFlightCount = 0;  //the time other flight exit hold when the flight enter the hold 
	for(OccupancyTable::iterator itr= m_vOccupancyTable.begin();itr!=m_vOccupancyTable.end();++itr)
	{
		OccupancyInstance& ocyInst = *itr;
		if( ocyInst.GetExitTime() >= enterTime && ocyInst.GetFlight()!=pFlight)
		{ 
			iInHoldFlightCount++;
		}
		else if(!ocyInst.IsExitTimeValid() && ocyInst.GetFlight()!=pFlight)
		{
			iInHoldFlightCount++;
		}
	}

	fltstate.m_pPosition = GetLevelPos(iInHoldFlightCount) -  m_dir.SetLength(20); //(m_dir[VX],m_dir[VY],m_dir[VZ]);
	fltstate.m_dAlt = fltstate.m_pPosition.getZ();
	fltstate.m_tTime = enterTime;
	pFlight->WriteLog();

	fltstate.m_pPosition = GetLevelPos(iInHoldFlightCount) -  m_dir; //(m_dir[VX],m_dir[VY],m_dir[VZ]);
	fltstate.m_dAlt = fltstate.m_pPosition.getZ();
	fltstate.m_tTime = enterTime;
	pFlight->WriteLog();

	fltstate.m_pPosition = GetLevelPos(iInHoldFlightCount);
	fltstate.m_tTime = enterTime;
	pFlight->WriteLog();

	SetEnterTime(pFlight, enterTime, OnBirth, 0);

	ClearanceItem enterItem(this,OnBirth,0);
	enterItem.SetTime(enterTime);
	pFlight->StartDelay(enterItem, NULL, FlightConflict::HOLD_AIR, FlightConflict::BETWEENINTERSECTIONS,FltDelayReason_AirHold, _T("Holding delay") );
}

ARCVector3 AirEntrySystemHoldInInSim::GetDir() const
{
	return m_dir;
}

#include "..\..\Results\AirsideFlightEventLog.h"
void AirEntrySystemHoldInInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = 0;
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource();
}

#include ".\FlightGetClearanceEvent.h"
void AirEntrySystemHoldInInSim::OnFlightExit( AirsideFlightInSim* pFlight,const ElapsedTime& exitTime )
{
	SetExitTime(pFlight, exitTime);
	RemoveFlightInQueue(pFlight);
	//write this Flight logs
//	writeFlightLog(pFlight,exitTime);
	//
	AirsideFlightInSim* pNextFlight = GetFrontFlight();
	if(pNextFlight)//wake up the next flight in the hold
	{
		FlightGetClearanceEvent* newEvent = new FlightGetClearanceEvent(pNextFlight);
		newEvent->setTime(exitTime);
		newEvent->addEvent();

		ClearanceItem endItem(this, OnBirth, 0);
		endItem.SetTime(exitTime);
		pNextFlight->EndDelay(endItem);
	}

	/*OccupancyInstance flightinstance  = GetOccupyInstance(pFlight);
	if(!flightinstance.IsValid() || !flightinstance.IsEnterTimeValid() )return;
	ElapsedTime entryTime = flightinstance.GetEnterTime();*/

	/*if (exitTime - entryTime > 0L)
	{
	AirsideConflictionDelayLog * pEnterSysDelay = new AirsideConflictionDelayLog();
	pEnterSysDelay->mMode = OnBirth;
	pEnterSysDelay->mAction = FlightConflict::HOLD_AIR;
	pEnterSysDelay->mDelayTime = exitTime - entryTime;
	pEnterSysDelay->time = exitTime;
	pEnterSysDelay->distInRes = 0;
	pEnterSysDelay->m_emFlightDelayReason = FltDelayReason_AirHold;
	pEnterSysDelay->m_sDetailReason = _T("Holding delay");
	getDesc(pEnterSysDelay->mAtResource);
	pEnterSysDelay->mdSpd=0;
	pEnterSysDelay->mConflictLocation = FlightConflict::BETWEENWAYPOINTS;
	CString strName = pFlight->GetAirTrafficController()->GetAirsideResourceManager()->GetInSectorNameAndID(pFlight->GetPosition(),pFlight->GetCurState().m_dAlt, pEnterSysDelay->mAreaID);
	pEnterSysDelay->sAreaName = strName.GetString();
	pFlight->LogEventItem(pEnterSysDelay);
	}*/
}
