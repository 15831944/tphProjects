/*****
*
*   Class       TerminalEntryEvent
*   Author      Lincoln Fellingham
*   Date        January 1994
*   Purpose     Handles timing of mobile elements entering the terminal
*
*               Designed to have only one instance in memory at a time
*               To load entire event list some members must become static
*
*****/

#ifndef TERMINAL_ENTRY_EVENT_H
#define TERMINAL_ENTRY_EVENT_H

#include "results\paxlog.h"
#include "terminalevent.h"

/*
This class will handle timing of mobile elements entering the terminal.
It is designed to have only one instance in memory at a time.
*/
class AirsideFlightInSim ;
class CAirsidePaxBusInSim ;
class ENGINE_TRANSFER TerminalEntryEvent : public TerminalEvent
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
    TerminalEntryEvent() {};
    virtual ~TerminalEntryEvent () {};

    //It initializes list and schedules first entry event.
    virtual void initList (PaxLog *p_list, long p_max);

    // It is the main event processing routine. A new MobileElement is created and scheduled its BIRTH event.
    // Returns FALSE, only one event in existence.
    virtual int process (CARCportEngine* pEngine);

    //It shuts down event
    virtual int kill (void) { return 0; }

    //It reads and schedules the next arrival event
    virtual int scheduleNextEntry ();

    //Returns event's name
    virtual const char *getTypeName (void) const { return "Entry"; };

    //It returns event type
    int getEventType (void) const { return EntryEvent; };
	
	int GetCurCount(){ return m_nCurrentCount; }

	int GetTotalCount(){ return m_nPaxCount; }

	void addEvent();

};

//class ENGINE_TRANSFER CAirsideEntryEvent : public TerminalEntryEvent
//{
//public:
//	int process (InputTerminal* _pInTerm); 
//    int scheduleNextEntry (); 
//	void SetNextMoblogEntry(MobLogEntry _logentry) {m_nextEntry = _logentry ;};
//	void SetCurrentCount(int _currnt) { m_nCurrentCount = _currnt ;};
//	CAirsideEntryEvent():m_Flight(NULL),m_Bus(NULL),m_hasBus(TRUE),TerminalEntryEvent(){};
//	CAirsideEntryEvent(const CAirsideEntryEvent& _event):TerminalEntryEvent()
//	{
//		this->m_pEntryList = _event.m_pEntryList ;
//		this->m_nCurrentCount = _event.m_nCurrentCount ;
//		this->m_nextEntry = _event.m_nextEntry ;
//		this->m_nPaxCount = _event.m_nPaxCount ;
//	    this->m_deyTime = _event.m_deyTime ;
//		this->time = _event.time ;
//		this->m_hasBus = _event.m_hasBus ;
//		this->m_Bus = _event.m_Bus ;
//
//	}
//	virtual ~CAirsideEntryEvent () { };
//	AirsideFlightInSim* m_Flight ;
//
//	void initList (PaxLog *p_list, long p_max); 
//protected:
//	ElapsedTime m_deyTime ;
//	BOOL m_hasBus ;
//	CAirsidePaxBusInSim* m_Bus ;
//public:
//	ElapsedTime GetDeyTime() {return m_deyTime ;} ;
//	void SetDeyTime(ElapsedTime _time) { m_deyTime = _time ;} ;
//	void HasBusServer(BOOL _res) { m_hasBus = _res ;} ;
//	void SetBus(CAirsidePaxBusInSim* _bus ) { m_Bus = _bus ;} ;
//};
#endif








