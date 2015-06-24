#pragma once
#include "./Results/TrainLog.h"
#include "terminalevent.h"

class TerminalTrainEntryEvent :public TerminalEvent
{
public:
	TerminalTrainEntryEvent(void);
	~TerminalTrainEntryEvent(void);
protected:
	//The list of Train elements waiting to enter the terminal.
	CTrainLog* m_pEntryList;

	//Next Train scheduled to enter the terminal.
	CTrainLogEntry m_nextEntry;

	//Total number of mobile elements being simulated.
	long m_nTrainCount;

	//Number of elements processed so far
	long m_nCurrentCount;
public:
	virtual int process (CARCportEngine* pEngine);
	virtual int kill (void);
	//It returns event's name
	virtual const char *getTypeName (void) const;

	//It returns event type
	virtual int getEventType (void) const;
	//make first event
	void initList (CTrainLog *p_list, long p_max);
	// make next Entry event
	int scheduleNextEntry ();
};
