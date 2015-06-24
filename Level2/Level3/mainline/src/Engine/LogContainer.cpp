#include "StdAfx.h"
#include ".\logcontainer.h"
#include "../Common/ARCTracker.h"
#include "SimEngineConfig.h"

 CFlightPaxLogContainer* CFlightPaxLogContainer::p_containter = NULL ;
CFlightPaxLogContainer::CFlightPaxLogContainer()
{
	m_NodelayPaxlog = new PaxLog(new EventLog<MobEventStruct>()) ;
}
CFlightPaxLogContainer::~CFlightPaxLogContainer()
{
	std::vector<CFlightLogPair*>::iterator iter  = m_ArrDelayContainer.begin() ;
	for ( ; iter != m_ArrDelayContainer.end() ; ++iter)
	{
		if((*iter) != NULL)
			delete *iter ;
	}
}
void CFlightPaxLogContainer::Reset()
{
	PLACE_METHOD_TRACK_STRING();
	std::vector<CFlightLogPair*>::iterator iter  = m_ArrDelayContainer.begin() ;
	for ( ; iter != m_ArrDelayContainer.end() ; ++iter)
	{
		if((*iter) != NULL)
		{
			delete *iter ;
		}
	}
	m_NodelayPaxlog->clearLogs() ;
	m_ArrDelayContainer.clear() ;
}
CFlightPaxLogContainer* CFlightPaxLogContainer::GetInstance()
{
	static CFlightPaxLogContainer _FlightPaxLogContainer ;
		CFlightPaxLogContainer::p_containter = &_FlightPaxLogContainer;
	return CFlightPaxLogContainer::p_containter ;
}
void CFlightPaxLogContainer::SetFlightSchedule(const FlightSchedule* pschedule)
{
	p_schedule = pschedule ;
}
void CFlightPaxLogContainer::SetPaxLog(PaxLog* p_log)
{
	p_AllPaxLog = p_log ;
}
void CFlightPaxLogContainer::InitContainer(CSimEngineConfig* pSimConf)
{
	PLACE_METHOD_TRACK_STRING();
	long count = p_AllPaxLog->getCount();
	if(!count)
		return;

	for(long i=0; i<count; i++)
	{
		MobLogEntry nextEntry ;
		p_AllPaxLog->getItem( nextEntry, i );
		if(nextEntry.isArriving())
		{ 
			if(pSimConf->isSimAirsideMode())
			{
				AddArrDelayMobLogEntry(nextEntry);
				continue ;
			}
			if(!pSimConf->IsTerminalSel()){
				continue;
			}
		}
		if(nextEntry.isDeparture() && pSimConf->isSimLandsideMode())
		{
			//AddDepDelayMobLogEntry(nextEntry) ;
			continue;
		}			
		

		m_NodelayPaxlog->addItem(nextEntry);
	}

}
CFlightLogPair* CFlightPaxLogContainer::GetDelayPaxLogByFlightID(int flight_id) 
{
	return FindArrPaxLog(flight_id) ;
}
PaxLog* CFlightPaxLogContainer::GetNoDelayPaxLog()
{
	return m_NodelayPaxlog;
}
void CFlightPaxLogContainer::AddArrDelayMobLogEntry(MobLogEntry& p_entry)
{

	int flight_id = p_entry.getArrFlight() ;
	CFlightLogPair* p_pair = FindArrPaxLog(flight_id) ;
	PaxLog* p_paxlog = NULL ;
	if(p_pair == NULL)
	{
		p_paxlog = new PaxLog(new EventLog<MobEventStruct>()) ;
		p_pair = new CFlightLogPair(p_paxlog,flight_id) ;
		m_ArrDelayContainer.push_back(p_pair) ;
	}else
		p_paxlog = p_pair->p_Log ;
	 p_paxlog->addItem(p_entry) ;
}


CFlightLogPair* CFlightPaxLogContainer::FindArrPaxLog( int fli_ID)
{
	std::vector<CFlightLogPair*>::iterator iter = m_ArrDelayContainer.begin() ;
	for ( ;iter != m_ArrDelayContainer.end() ;++iter)
	{
		if((*iter)->Flight_ID == fli_ID)
			return (*iter) ;
	}
	return NULL ;
}
int CFlightPaxLogContainer::CurrentIndexOfPaxlogByFlightID(int Flight_id) 
{
	CFlightLogPair* pair = NULL ;
	pair = FindArrPaxLog(Flight_id) ;
	 if(pair != NULL)
		 return pair->m_index_NoActive ;
	 else
		 return 0 ;
}
void CFlightPaxLogContainer::SetCurrentIndexOfPaxlogByFlightID(int _index ,int Flight_id)
{
	CFlightLogPair* pair = NULL ;
	pair = FindArrPaxLog(Flight_id) ;
	if(pair != NULL)
		 pair->m_index_NoActive = _index;
}

void CFlightPaxLogContainer::AddDepDelayMobLogEntry( MobLogEntry& p_entry )
{
	int flight_id = p_entry.getDepFlight() ;
	CFlightLogPair* p_pair = FindDepPaxLog(flight_id) ;
	PaxLog* p_paxlog = NULL ;
	if(p_pair == NULL)
	{
		p_paxlog = new PaxLog(new EventLog<MobEventStruct>()) ;
		p_pair = new CFlightLogPair(p_paxlog,flight_id) ;
		m_DepDelayContainer.push_back(p_pair);
	}else
		p_paxlog = p_pair->p_Log ;
	p_paxlog->addItem(p_entry) ;
}

CFlightLogPair* CFlightPaxLogContainer::GetDepDelayPaxLogByFlightID( int flight_id )
{
	return FindDepPaxLog(flight_id);
}

CFlightLogPair* CFlightPaxLogContainer::FindDepPaxLog( int fli_ID )
{
	std::vector<CFlightLogPair*>::iterator iter = m_DepDelayContainer.begin();
	for ( ;iter != m_DepDelayContainer.end() ;++iter)
	{
		if((*iter)->Flight_ID == fli_ID)
			return (*iter) ;
	}
	return NULL ;
}
void CFlightLogPair::GetNoActivePaxlog(PaxLog& _log , int count)
{
	MobLogEntry entry ;

	int nStartPos = m_index_NoActive;
	for (int i = nStartPos; i < count && i < p_Log->getCount(); i++)
	{
		p_Log->getItem(entry,i);
		_log.addItem(entry);
		m_index_NoActive++;
	}
// 	int m_active = m_index_NoActive - 1;
// 	int oldposition = m_index_NoActive ;
// 	int number  = 0 ;
// 	int Groupsize = 0 ;
// 	for(;m_index_NoActive < p_Log->getCount()&&number<=count;)
// 	{
//           p_Log->getItem(entry,m_index_NoActive) ;
// 		Groupsize = entry.getGroupSize();
// 		if(entry.GetMobileType() == 2)
// 			Groupsize = 1 ;
// 		if(entry.GetMobileType() == 0 && entry.getGroupMember() == 0)
// 		{
// 			number += Groupsize;
// 			if(number > count)
// 				break ;
// 		}
// 		m_active = m_active+Groupsize;
// 		_log.addItem(entry) ;
// 		m_index_NoActive +=  Groupsize ;
// 	}
}

CFlightLogPair::CFlightLogPair( PaxLog* _log , int Fli_ID ) :p_Log(_log),Flight_ID(Fli_ID),m_index_NoActive(0)
{
	m_BagLog = new PaxLog(new EventLog<MobEventStruct>()) ;
}
