#include "stdafx.h"
#include "engine\sender.h"
#include "common\elaptime.h"
#include "results\paxlog.h"
#include "results\out_term.h"
#include "engine\process.h"
#include "inputs\probab.h"
#include "inputs\pax_db.h"
#include "inputs\paxdata.h"
#include "inputs\in_term.h"
#include "engine\pax.h"
#include "common\exeption.h"

Sender::Sender (MobLogEntry& p_entry, Terminal* _pTerm) : Visitor (p_entry, _pTerm)
{
    static long xxxx = -1;
    if (id == xxxx)
        cout << '\r';

    freeMoving = 0;
    const ProbabilityDistribution* visitTimeDist;

	assert( m_pTerm );
    visitTimeDist = (m_pTerm->paxDataList->getVisitTime())->FindFit(m_type);
    if (visitTimeDist == NULL)
        throw new StringError ("Missing visit time distribution");
    visitTime = ((ElapsedTime)(60.0 * visitTimeDist->getRandomValue()))
        + p_entry.getEntryTime();
}

void Sender::initPassenger (Passenger *p_pax)
{
    pax = p_pax;
    paxGroupSize = pax->getGroupSize();
}

 
int Sender::update (ElapsedTime p_time)
{
	int nextState = pax->getState();
	Processor *nextProc = pax->getProcessor();
	if( nextState == FreeMoving && m_pProcessor->HasDestination( m_type, m_nInGateDetailMode ) )
	{
		m_iPreState = state;
		state = FreeMoving;
		freeMoving = 1;
		freeMovingIndex = m_logEntry.getCurrentCount();

		if( getNextProcessor( p_time ) )
			generateEvent (p_time);

		return 0;
	}

/*

    // test visitor for limit of visit with passenger (senders)
    if ((nextProc->getProcessorType() != DepSinkProc && visitTime < p_time
        && nextState == FreeMoving) || !nextProc->acceptsVisitors())
    { 
        state = FreeMoving;
        freeMoving = 1;
        freeMovingIndex = m_logEntry.getCurrentCount();
 
//        int flag = m_pProcessor->getAvail();
//        m_pProcessor->addPerson (this);
//        Processor *proc = m_pProcessor;

        if (getNextProcessor (p_time))
            generateEvent (p_time);
//      if (proc->getProcessorType() == DepSinkProc)
//          proc->setAvail (flag);
        return 0;
    }
*/
	m_iPreState = state;
    state = nextState;
    m_nPathIndex = pax->getIndex();
    m_pProcessor = nextProc;

    // write log entry
    writeLog (p_time);
    return 1;
}

 
void Sender::flushLog (ElapsedTime p_time, bool bmissflight)
{
	m_iPreState = state;
    state = Death;
    writeLogEntry (p_time);
    m_logEntry.setExitTime (p_time);
    long trackCount = m_logEntry.getCurrentCount();

    MobEventStruct *log = NULL;
    m_logEntry.getLog (log);
    freeMovingIndex = (freeMovingIndex != -1)? freeMovingIndex: trackCount-2;
    float paxRotation = (paxGroupSize>=MAX_GROUP)?0.0:rotation[paxGroupSize];

    // write first member of group
        // adjust around passenger, first part of lifetime
    adjustCoordinates (log, freeMovingIndex+2, paxRotation, m_spacing);
    m_logEntry.setEventList (log, trackCount);

	delete [] log;
	log = NULL;

	assert( m_pTerm );
    m_pTerm->paxLog->updateItem (m_logEntry, m_logEntry.getIndex());
	
    for (int i = 1; i < m_logEntry.getGroupSize(); i++)
    {
        paxRotation = (i+paxGroupSize>=MAX_GROUP)?0.0:rotation[i + paxGroupSize];
        m_logEntry.getLog (log);
        // adjust around passenger, first part of lifetime
        adjustCoordinates (log, freeMovingIndex+2, paxRotation, m_spacing);

        // adjust around first visitor, while free moving
        adjustCoordinates (log + freeMovingIndex, trackCount -
            freeMovingIndex, (i>=MAX_GROUP)?0.0:rotation[i], m_spacing);

        m_logEntry.setEventList (log, trackCount);

		delete [] log;

		log = NULL;

        m_logEntry.incPax();
        m_pTerm->paxLog->updateItem (m_logEntry, m_logEntry.getIndex());

    }
    m_logEntry.clearLog();
}

