#include "StdAfx.h"
#include ".\CrossWalkInSim.h"
#include <algorithm>
#include "..\Common\Path2008.h"
#include "PaxLandsideBehavior.h"
#include "Engine/LandsideTrafficSystem.h"
#include "LandsideVehicleInSim.h"
#include "..\Landside\LandsideCrosswalk.h"
#include "LandsideConflictMoveEvent.h"
CCrossWalkInSim::CCrossWalkInSim(LandsideCrosswalk* pCrossWalk)
:m_pCrossWalk(pCrossWalk)
{
	m_ePaxLightState=CROSSLIGHT_RED;
	m_lBufferTime=80;
	m_lGreenTime=200;
	m_bKilled = false;

}

CCrossWalkInSim::~CCrossWalkInSim(void)
{
}

//----------------------------------------------------------------------
//Summary:
//		check cross walk whether valid for pax
//----------------------------------------------------------------------
bool CCrossWalkInSim::PaxValid() const
{
	//occupied by vehicle
	if (!m_VehicleOccupiedList.empty())
		return false;
	
	//approach vehicle will occupied
	//if (!m_VehicleapproachList.empty())
	//	return false;
	
	return true;
}
//----------------------------------------------------------------------
//Summary:
//		check crosswalk whether valid for vehicle
//----------------------------------------------------------------------
bool CCrossWalkInSim::VehicleValid() const
{
	//occupied by passenger
	if (!m_PaxOccupiedList.empty())
		return false;

	//approach passengers will occupied
	if (!m_PaxApproachList.empty())
		return false;
	
	return true;
}
//----------------------------------------------------------------------
//Summary:
//		step on crosswalk
//Parameter:
//		time[out]: passed time
//		routePath: move on path
//		pFreeMoveLogic[in]: person free moving logic
//---------------------------------------------------------------------
void CCrossWalkInSim::AddVehicleOccupied(LandsideVehicleInSim* pVehicleInSim)
{
	ASSERT(pVehicleInSim);
	if (pVehicleInSim)
	{
		m_VehicleOccupiedList.push_back(pVehicleInSim);
	}
}

void CCrossWalkInSim::ReleaseVehilceOccupied(LandsideVehicleInSim* pVehicleInSim)
{
	ASSERT(pVehicleInSim);
	std::vector<LandsideVehicleInSim*>::iterator iter = m_VehicleOccupiedList.begin();
	iter = std::find(m_VehicleOccupiedList.begin(),m_VehicleOccupiedList.end(),pVehicleInSim);
	if (iter != m_VehicleOccupiedList.end())
	{
		m_VehicleOccupiedList.erase(iter);
	}
}

//void CCrossWalkInSim::AddVehicleApproach(LandsideVehicleInSim* pVehicleInSim)
//{
//	ASSERT(pVehicleInSim);
//	if (pVehicleInSim)
//	{
//		m_VehicleapproachList.push_back(pVehicleInSim);
//	}
//}
//
//void CCrossWalkInSim::ReleaseVehicleApproach(LandsideVehicleInSim* pVehicleInSim)
//{
//	ASSERT(pVehicleInSim);
//	std::vector<LandsideVehicleInSim*>::iterator iter = m_VehicleapproachList.begin();
//	iter = std::find(m_VehicleapproachList.begin(),m_VehicleapproachList.end(),pVehicleInSim);
//	if (iter != m_VehicleapproachList.end())
//	{
//		m_VehicleapproachList.erase(iter);
//	}
//}

void CCrossWalkInSim::AddPaxOccupied( CFreeMovingLogic* pLandsideBehavior )
{
	ASSERT(pLandsideBehavior);
	if (pLandsideBehavior)
	{
		m_PaxOccupiedList.push_back(pLandsideBehavior);
	}
}

void CCrossWalkInSim::ReleasePaxOccupied( CFreeMovingLogic* pLandsideBehavior )
{
	ASSERT(pLandsideBehavior);
	std::vector<CFreeMovingLogic*>::iterator iter = m_PaxOccupiedList.begin();
	iter = std::find(m_PaxOccupiedList.begin(),m_PaxOccupiedList.end(),pLandsideBehavior);
	if (iter != m_PaxOccupiedList.end())
	{
		m_PaxOccupiedList.erase(iter);
	}
}

void CCrossWalkInSim::AddPaxApproach( CFreeMovingLogic* pLandsideBehavior )
{
	ASSERT(pLandsideBehavior);
	std::vector<CFreeMovingLogic*>::iterator iter = m_PaxApproachList.begin();
	iter = std::find(m_PaxApproachList.begin(),m_PaxApproachList.end(),pLandsideBehavior);
	if (iter != m_PaxApproachList.end())
	{
		return;
	}

	if (pLandsideBehavior)
	{
		m_PaxApproachList.push_back(pLandsideBehavior);
	}
}

void CCrossWalkInSim::ReleasePaxApproach( CFreeMovingLogic* pLandsideBehavior )
{
	ASSERT(pLandsideBehavior);
	std::vector<CFreeMovingLogic*>::iterator iter = m_PaxApproachList.begin();
	iter = std::find(m_PaxApproachList.begin(),m_PaxApproachList.end(),pLandsideBehavior);
	if (iter != m_PaxApproachList.end())
	{
		m_PaxApproachList.erase(iter);
	}
}
//----------------------------------------------------------------------
//Summary:
//		step on crosswalk
//Parameter:
//		time[out]: passed time
//		routePath: move on path
//		pFreeMoveLogic[in]: person free moving logic
//---------------------------------------------------------------------
void CCrossWalkInSim::StepOnCrossWalk(ElapsedTime& time,LandsideTrafficGraphVertexList& routePath,CFreeMovingLogic* pFreeMoveLogic)
{
	ReleasePaxApproach(pFreeMoveLogic);
	AddPaxOccupied(pFreeMoveLogic);
 	ARCVector3 pos=pFreeMoveLogic->GetBehavior()->getPoint();
 	pFreeMoveLogic->GetBehavior()->setDestination(pos);
 	pFreeMoveLogic->GetBehavior()->writeLog(time);
	while(!routePath.empty())
	{
		LandsideTrafficGraphVertex& vertex = routePath.back();
		pFreeMoveLogic->Step(time,vertex);
		pFreeMoveLogic->GetBehavior()->setCurrentVertex(vertex);
		if (vertex.GetTrafficInSim() != this)
		{
			LandsideLeaveCrossWalkEvent *pEvent =  new LandsideLeaveCrossWalkEvent(pFreeMoveLogic,this,time);
			pEvent->addEvent();
			return;
		}
	}
}
//-----------------------------------------------------------------------
//Summary:
//		retrieve crosswalk center path
//-----------------------------------------------------------------------
 CPath2008 CCrossWalkInSim::GetCenterPath() const
{
	ASSERT(m_pCrossWalk);
	return m_pCrossWalk->getCenterPath();
}
//----------------------------------------------------------------------
//Summary:
//		retrieve crosswalk right path
//----------------------------------------------------------------------
CPath2008 CCrossWalkInSim::GetRightPath() const
{
	ASSERT(m_pCrossWalk);
	return m_pCrossWalk->getRightPath();
}
//----------------------------------------------------------------------
//Summary:
//		retrieve crosswalk left path
//----------------------------------------------------------------------
CPath2008 CCrossWalkInSim::GetLeftPath() const
{
	ASSERT(m_pCrossWalk);
	return m_pCrossWalk->getLeftPath();
}
//-------------------------------------------------------------------
//Summary:
//		wake up vehicle and need implement
//-------------------------------------------------------------------
void CCrossWalkInSim::NoticefyVehicle(const ElapsedTime& t)
{
	//SendSignal(new SignalCrossStateChange(), t);
	//
	NotifyObservers();
}
//-------------------------------------------------------------------
//Summary:
//		wake up person and need implement
//--------------------------------------------------------------------
void CCrossWalkInSim::NoticefyPax( const ElapsedTime& t )
{
	for(size_t i=0;i<m_PaxApproachList.size();i++)
	{
		CFreeMovingLogic* pLogic= m_PaxApproachList[i];
		pLogic->ProcessMove(t);
	}
}
//------------------------------------------------------------------------
//Summary:
//		retrieve traffic type
//------------------------------------------------------------------------
CLandsideWalkTrafficObjectInSim::TrafficObjectType CCrossWalkInSim::GetTrafficType() const
{
	return CLandsideWalkTrafficObjectInSim::CrossWalk_Type;
}

int CCrossWalkInSim::getAtStretchID() const
{
	if(m_pCrossWalk && m_pCrossWalk->getStretch())
		return m_pCrossWalk->getStretch()->getID();
	return -1;
}

double CCrossWalkInSim::getAtStretchPos() const
{
	if(m_pCrossWalk )
		return m_pCrossWalk->getPosOnStretch();
	return -1;

}

bool CCrossWalkInSim::IsVehicleOccupied( LandsideVehicleInSim* pVehicleInSim ) const
{
	if( std::find(m_VehicleOccupiedList.begin(),m_VehicleOccupiedList.end(),pVehicleInSim)!=m_VehicleOccupiedList.end() )
		return true;
	return false;
}

double CCrossWalkInSim::getHalfWidth() const
{
	if(m_pCrossWalk)
		return m_pCrossWalk->getWidth()*0.5;
	return 0;
}

bool CCrossWalkInSim::CanVehicleCross(const ElapsedTime& eTime) const
{
	if(!m_PaxOccupiedList.empty())
	{
		return false;
	}
	if(!CorsswalkIdle(eTime))
		return false;


	switch(m_pCrossWalk->getCrossType())
	{
	case Cross_Intersection:
		{
			if (m_ePaxLightState==CROSSLIGHT_GREEN)
			{
				return false;
			}
			return true;
			break;
		}
	case Cross_Pelican:
		{
			if (m_ePaxLightState==CROSSLIGHT_GREEN)
			{
				return false;
			}
			return true;
			break;
		}
	case Cross_Zebra:
		{
			if(!m_PaxOccupiedList.empty() || !m_PaxApproachList.empty())
			{
				return false;
			}
			return true;
			break;
		}
	default:
		{
			return false;
			break;
		}
	}
}
void CCrossWalkInSim::setPaxLightState(Cross_LightState lightState)
{
	m_ePaxLightState=lightState;
	switch(m_ePaxLightState)
	{
	case CROSSLIGHT_GREEN:
		{
			NoticefyPax(curTime());
			break;
		}
	case CROSSLIGHT_RED:
		{
			NoticefyVehicle(curTime());
			break;
		}
	}
	WriteLog(curTime());
}
void CCrossWalkInSim::pressCtrlButton()
{
	if (getCrossType()!=Cross_Pelican || getPaxLightState()!=CROSSLIGHT_RED)
	{
		return;
	}
	setPaxLightState(CROSSLIGHT_BUFFER);
	ElapsedTime nextTime=curTime()+m_lBufferTime;
	Activate(nextTime);	
}
void CCrossWalkInSim::OnActive(CARCportEngine*pEngine)
{
	if (getPaxLightState()==CROSSLIGHT_BUFFER)
	{
		setPaxLightState(CROSSLIGHT_GREEN);
		ElapsedTime nextTime=curTime()+m_lGreenTime;
		Activate(nextTime);
		
	}else if (getPaxLightState()==CROSSLIGHT_GREEN)
	{	
		setPaxLightState(CROSSLIGHT_RED);			
	}
// 	WriteLog();
}
void CCrossWalkInSim::OnTerminate(CARCportEngine*pEngine)
{
	//CrossWalkDescStruct &desc = m_logEntry.GetCrossWalkDesc();	
	WriteLog(curTime());
	m_logEntry.saveEvents();
}
void CCrossWalkInSim::InitLogEntry(OutputLandside* pOutput )
{
	CrossWalkDescStruct &desc = m_logEntry.GetCrossWalkDesc();
	desc.crossID=m_pCrossWalk->getID();
	desc.crossType=m_pCrossWalk->getCrossType();
	m_logEntry.SetEventLog(pOutput->m_SimLogs.getCrossWalkEventLog());
}
void CCrossWalkInSim::WriteLog(const ElapsedTime& t)
{
	CrossWalkEventStruct curEvent;
	curEvent.lightState=m_ePaxLightState;
	curEvent.time=t.getPrecisely();
	m_logEntry.addEvent(curEvent);
}
int CCrossWalkInSim::GetLinkIntersectionID()
{
	if (m_pCrossWalk && m_pCrossWalk->getLinkIntersection())
	{
		return m_pCrossWalk->getLinkIntersection()->getID();
	}
	return -1;
}
int CCrossWalkInSim::GetLinkGroup()
{
	if (m_pCrossWalk && m_pCrossWalk->getLinkIntersection())
	{
		return m_pCrossWalk->getLinkGroup();
	}
	return -1;
}



void CCrossWalkInSim::Kill( OutputLandside* pOutput , const ElapsedTime& t )
{
	if(!m_logEntry.isInited())
		return ;
	if(m_bKilled)
		return ;
	
	WriteLog(t);
	m_logEntry.saveEvents();
	pOutput->m_SimLogs.GetCrossWalkLog().addItem(m_logEntry);

	m_bKilled = true;
}

bool CCrossWalkInSim::CorsswalkIdle( const ElapsedTime& eTime ) const
{
	for(UINT i = 0; i < m_vCorssWalkPax.size(); i++)
	{
		const CrossWalkPaxLog& logItem = m_vCorssWalkPax.at(i);
		if (logItem.Alive(eTime))
		{
			return false;
		}
	}
	return true;
}

void CCrossWalkInSim::WritePassengerOnCrosswalk( int nPaxID,const ElapsedTime& tEnterTime,const ElapsedTime& tExitTime,bool bCreate )
{
	if (!bCreate)
	{
		for (UINT i = 0; i < m_vCorssWalkPax.size(); i++)
		{
			CrossWalkPaxLog& logItem = m_vCorssWalkPax[i];
			if (logItem.m_nPaxID == nPaxID)
			{
				logItem.m_startTime = MIN(logItem.m_startTime,tEnterTime);
				logItem.m_endTime = MAX(logItem.m_endTime,tExitTime);

				/*ofsstream echoFile ("d:\\Crosswalk.log", stdios::app);
				echoFile<<"Pax ID:  "<<nPaxID<<"\n";

				echoFile<<"Start time:  "<<logItem.m_startTime.printTime()<<"\n";
				echoFile<<"End time:  "<<logItem.m_endTime.printTime()<<"\n";

				echoFile.close();*/

				return;
			}
		}
	}
	
	//current doesn't exsit the passenger move information, create one
	CrossWalkPaxLog newItem;
	newItem.m_nPaxID = nPaxID;
	newItem.m_startTime = tEnterTime;
	newItem.m_endTime = tExitTime;
	m_vCorssWalkPax.push_back(newItem);
}
