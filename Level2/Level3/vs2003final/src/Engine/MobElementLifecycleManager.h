#pragma once
#include "ServiceObject.h"

#include "entry.h"
#include "OnBoard/OnBoardEntryEvent.h"
#include "MobElementEntryEvent.h"

#include "Results/eventlog.h"
#include "Results/paxentry.h"
#include "MobileElementGroup.h"

class PaxLog;
class FlightSchedule;
class Flight;
class Person;

/**
	Purpose:Manager mobile element lifecycle in ALTO system:
	1.	Active resting elements when arrival flight (which elements belongs to) arrive at stand.
	2.	Delivery mobile elements from one mode to other(depend on simulation mode's config). 
	3.	Ensure all generated mobile elements have been actived when simulation done.
**/

class MobElementsLifeCycleManager : public ServiceObject < MobElementsLifeCycleManager >
{
public:
	typedef std::vector< MobileElementGroup* > MobileElementGroupVector;
	typedef MobileElementGroupVector::iterator MobileElementGroupIter;

public:
	MobElementsLifeCycleManager(void);
	~MobElementsLifeCycleManager(void); 

	void initialize(PaxLog* pAllPaxLog, const FlightSchedule* pSchedule);


public:

	void activeMobElementGroup(const Flight* pFlight, const ElapsedTime& actualTime, int nActivePaxMaxCount = -1);

//	next thing detecting, sim mode transfer.(qiankun danuoyi)
	void simModeTransfer(Person* pPerson){};

	
//	diagnosis
	//void log();

private:
	void cleanup();
	void cleanupActivedMobileElementGroup(MobileElementGroup* pActivedMobGroup);
	void PartitionPaxLog();
	void ScheduleGroupEntryEvent();
	MobileElementGroup* GetMobileElementGroup(const int nFltIndex);
	int GetFlightIndex(const Flight* pFlight);
	void AddMobLogEntry(MobLogEntry& nextEntry, long nOldIndex);

	//TerminalEntryEvent						terminalEntryEvent;
	OnBoardEntryEvent						onBoardEntryEvent;
	//MobElementEntryEvent					mobElementEntryEvent;

	MobileElementGroupVector				m_vectNonActiveGroup;
	MobileElementActiveGroup				m_activeGroup;
	const FlightSchedule					*m_pFlightSchedule;
	TerminalEntryEvent						m_eventNonArrivalPaxEvent;
	
	EventLog<MobEventStruct>				*m_pMobEventLog;
	PaxLog									*m_pNonArrivalPaxLog;
	PaxLog									*m_pAllPaxLog;

};



MobElementsLifeCycleManager* MobElementsLifeCycleMgr();
AFX_INLINE MobElementsLifeCycleManager* MobElementsLifeCycleMgr()
{
	return MobElementsLifeCycleManager::GetInstance();
}


