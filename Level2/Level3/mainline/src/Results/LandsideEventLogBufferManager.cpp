#include "StdAfx.h"
#include ".\LandsideEventLogBufferManager.h"
#include "landside/LandsideVehicleLog.h"
#include "LandsideEventLogBuffer.h"
#include "../main/TermPlanDoc.h"


//void CLandsideEventLogBufferManager::LoadDataIfNecessary(const LandsideVehicleLog& logIndex, long _lCurTime )
//{	
//	m_vehicleLogBuf.load
//
//	if(bFirstTimeLoad)
//	{//init data
//		m_vehicleLogBuf.resize(logIndex.getCount());
//		//load vehicle events	
//		for(int i=0;i<logIndex.getCount();i++)
//		{
//			LandsideVehicleLogEntry& entry = m_vehicleLogBuf[i];
//			logIndex.getItem(entry,i);
//			entry.SetEventLog(logIndex.getEventLog());
//		}
//
//		bFirstTimeLoad = false;
//	}
//	//load event
//	for(size_t i=0;i<m_vehicleLogBuf.size();i++)
//	{
//		LandsideVehicleLogEntry& entry = m_vehicleLogBuf[i];
//		if( entry.CheckAlive(_lCurTime) )
//		{
//			entry.loadEvents();
//		}
//	}
//}
void CLandsideEventLogBufferManager::LoadLinkGroupDataIfNecessary(const LandsideLinkGroupLog &logIndex,long _lCurTime)
{
	if(bFirstLoadGroupData)
	{//init data
		m_vLinkGroupLogBuff.resize(logIndex.getCount());
		//load vehicle events	
		for(int i=0;i<logIndex.getCount();i++)
		{
			IntersectionLinkGroupLogEntry& entry = m_vLinkGroupLogBuff[i];
			logIndex.getItem(entry,i);
			entry.SetEventLog(logIndex.getEventLog());
		}

		bFirstLoadGroupData = false;
	}
	//load event
	for(size_t i=0;i<m_vLinkGroupLogBuff.size();i++)
	{
		IntersectionLinkGroupLogEntry& entry = m_vLinkGroupLogBuff[i];
		if( entry.CheckAlive(_lCurTime) )
		{
			entry.loadEvents();
		}
	}
}
void CLandsideEventLogBufferManager::LoadCrossWalkDataIfNecessary(const LandsideCrossWalkLog &logIndex,long _lCurTime)
{
	if(bFirstLoadCrossData)
	{//init data
		m_vCrossWalkLogBuff.resize(logIndex.getCount());
		//load vehicle events	
		for(int i=0;i<logIndex.getCount();i++)
		{
			CCrossWalkLogEntry& entry = m_vCrossWalkLogBuff[i];
			logIndex.getItem(entry,i);
			entry.SetEventLog(logIndex.getEventLog());
		}

		bFirstLoadCrossData = false;
	}
	//load event
	for(size_t i=0;i<m_vCrossWalkLogBuff.size();i++)
	{
		CCrossWalkLogEntry& entry = m_vCrossWalkLogBuff[i];
		if( entry.CheckAlive(_lCurTime) )
		{
			entry.loadEvents();
		}
	}
}
CLandsideEventLogBufferManager::CLandsideEventLogBufferManager()
{
	bFirstTimeLoad  =true;
	bFirstLoadGroupData=true;
	bFirstLoadCrossData =true;
	m_pCurrentLogBuffer = NULL;
	m_detaTime= 15*60*100L;
}

CLandsideEventLogBufferManager::~CLandsideEventLogBufferManager()
{

}

CLandsideEventLogBuffer* CLandsideEventLogBufferManager::loadVehicleDataIfNecessary( long _lCurTime, bool bAnimForward,CTermPlanDoc* pTermDoc )
{
	

	long startTime	= m_pCurrentLogBuffer->getStartTime();
	long endTime	= m_pCurrentLogBuffer->getEndTime();

	if( _lCurTime >= startTime && _lCurTime <= endTime )	// need not to read data
		return m_pCurrentLogBuffer;

	if (bAnimForward)
	{
		startTime = max( _lCurTime , 0l );
		endTime = _lCurTime + m_detaTime;
	}
	else
	{
		startTime = max( _lCurTime - m_detaTime, 0l );
		endTime = _lCurTime;
	}

	CTermPlanDoc::ANIMATIONSTATE eState = pTermDoc->m_eAnimState;
	pTermDoc->m_eAnimState = CTermPlanDoc::anim_pause;
	pTermDoc->AnimTimerCallback();
	m_pCurrentLogBuffer->readDataFromFile(  startTime, endTime,pTermDoc );
	pTermDoc->AnimTimerCallback();
	pTermDoc->m_eAnimState = eState;
	return m_pCurrentLogBuffer;
}

void CLandsideEventLogBufferManager::InitVehicleLogBuf( LandsideVehicleLog* plog )
{
	if(!m_pCurrentLogBuffer){
		m_pCurrentLogBuffer = new CLandsideEventLogBuffer(plog);
	}
}
