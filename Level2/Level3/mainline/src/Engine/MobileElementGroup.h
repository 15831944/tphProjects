#pragma once

#include "entry.h"
#include "../Common/elaptime.h"
#include "../Results/eventlog.h"
#include "../Results/paxentry.h"
#include <map>

//----------------------------------------------------------------------------------
//MobileElementGroup
class MobileElementGroup
{
public:
	MobileElementGroup();
	virtual ~MobileElementGroup(void);

public:
	int	 GetFlightIndex(){	return m_nFltIndex; }
	void SetFlightIndex(int nFltIndex){ m_nFltIndex = nFltIndex; }

	void SetTerminalPaxLogPtr(PaxLog* pAllPaxLog);

	virtual bool IsActive() = 0;
	virtual bool IsStarted(){	return m_bIsStarted; }

	virtual void AddMobLogEntry(MobLogEntry& mobLogEntry, long nOldIndex){	ASSERT(FALSE);	}
	virtual void AdjustMobileElementEntryTime(const ElapsedTime& offsetTime){	ASSERT(FALSE);	}
	virtual void ScheduleEntryEvent(const ElapsedTime& offsetTime, int nActivePaxMaxCount){	ASSERT(FALSE);	}
	virtual void ScheduleEntryEventDirectly();

protected:
	int							m_nFltIndex;
	TerminalEntryEvent			m_entryEvent;
	PaxLog						*m_pPaxsLog;
	EventLog<MobEventStruct>*	m_pMobEventLog;
	bool						m_bIsStarted;
	PaxLog						*m_pTerminalPaxLog;

	std::map<long, long >       m_mapNewIdxToOldIdx;
};

//----------------------------------------------------------------------------------
//MobileElementActiveGroup
//TODO:
class MobileElementActiveGroup : public MobileElementGroup
{
public:
	MobileElementActiveGroup();
	virtual ~MobileElementActiveGroup(void);

public:
	virtual bool IsActive(){	return true; }

	virtual void AddMobLogEntry(MobLogEntry& mobLogEntry, long nOldIndex){	ASSERT(FALSE); }
	virtual void AdjustMobileElementEntryTime(const ElapsedTime& offsetTime){	ASSERT(FALSE); }
	virtual void ScheduleEntryEvent(const ElapsedTime& offsetTime,int nActivePaxMaxCount){	ASSERT(FALSE); }


};

//----------------------------------------------------------------------------------
//MobileElementNonActiveGroup
class MobileElementNonActiveGroup : public MobileElementGroup
{
public:
	MobileElementNonActiveGroup();
	virtual ~MobileElementNonActiveGroup(void);

public:
	virtual bool IsActive(){	return false; }

	virtual void AddMobLogEntry(MobLogEntry& mobLogEntry, long nOldIndex);
	virtual void AdjustMobileElementEntryTime(const ElapsedTime& offsetTime);
	virtual void ScheduleEntryEvent(const ElapsedTime& offsetTime, int nActivePaxMaxCount);
};












