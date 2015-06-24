#pragma once
#include "OnBoardEntryEvent.h"

#include "../entry.h"

class PaxLog;

class OnBoardEntryEvent : public Event
{
protected:
	//The list of mobile elements waiting to enter the terminal.
	PaxLog* m_pEntryList;

	//Next element scheduled to enter the terminal.
	MobLogEntry m_nextEntry;

	//Total number of mobile elements being simulated.
	long m_nPaxCount;

	//Number of elements processed so far
	long m_nCurrentCount;

public:
	OnBoardEntryEvent() {};
	virtual ~OnBoardEntryEvent () {};

	//It initializes list and schedules first entry event.
	void initList (PaxLog *p_list, long p_max);

	// It is the main event processing routine. A new MobileElement is created and scheduled its BIRTH event.
	// Returns FALSE, only one event in existence.
	virtual int process (CARCportEngine* pEngine);

	//It shuts down event
	virtual int kill (void) { return 0; }

	//It reads and schedules the next arrival event
	int scheduleNextEntry ();

	//Returns event's name
	virtual const char *getTypeName (void) const { return "Entry"; };

	//It returns event type
	int getEventType (void) const { return EntryEvent; };

	int GetCurCount(){ return m_nCurrentCount; }

	int GetTotalCount(){ return m_nPaxCount; }
};








