#include "stdafx.h"
#include "common\elaptime.h"
#include "engine\greeter.h"
#include "engine\greet.h"
#include "engine\entry.h"
#include "results\paxlog.h"
#include "results\out_term.h"
#include "engine\pax.h"
#include "common\exeption.h"
#include "common\states.h"

Greeter::Greeter (MobLogEntry& p_entry, Terminal* _pTerm) : Visitor (p_entry, _pTerm)
{
	m_pTerm = _pTerm;
    freeMoving = 1;
    paxIndex = p_entry.getVisitor();
}

//The attached passenger is as input parameter.
void Greeter::initPassenger (Passenger *p_pax)
{
    pax = p_pax;
    paxGroupSize = pax->getGroupSize();
    freeMovingIndex = m_logEntry.getCurrentCount();
}

//It serves greeter who just met his passenger arrival.
//The greeter's state changes to LeaveServer.
void Greeter::processServerArrival (ElapsedTime p_time)
{
    if (!pax && m_pProcessor->getProcessorType() == HoldAreaProc)
    {
        Passenger *aPax = (Passenger *)m_pProcessor->findElement (paxIndex);
        if (aPax && aPax->getState() == WaitInHoldingArea)
        {
            destination = aPax->getPoint();
            GreetEvent *event = new GreetEvent (aPax, this, p_time + moveTime());
            event->addEvent();
            return;
        }
		regetAttatchedNopax();
        m_nStranded = !m_pProcessor->getServiceTime (m_type, p_time);
    }

	m_iPreState = state;
    state = LeaveServer;
    m_pProcessor->beginService (this, p_time);
}

 
int Greeter::update (ElapsedTime p_time)
{
	m_iPreState = state;
    state = pax->getState();
    m_nPathIndex = pax->getIndex();
    m_pProcessor = pax->getProcessor();

    // write log entry
    writeLog (p_time);
    return 1;
}

 
void Greeter::flushLog (ElapsedTime p_time, bool bmissflight)
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
    // adjust around passenger, last part of lifetime
    adjustCoordinates (log + freeMovingIndex, trackCount -
        freeMovingIndex, paxRotation, m_spacing);

    m_logEntry.setEventList (log, trackCount);
   	assert( m_pTerm );
	m_pTerm->paxLog->updateItem (m_logEntry, m_logEntry.getIndex());
	
	delete [] log;
	log = NULL;

    for (int i = 1; i < m_logEntry.getGroupSize(); i++)
    {
        paxRotation = (i+paxGroupSize>=MAX_GROUP)?0.0:rotation[i+paxGroupSize];
        m_logEntry.getLog (log);
        // adjust around first visitor, while free moving
        adjustCoordinates (log, freeMovingIndex+2,
            (i>=MAX_GROUP)?0.0:rotation[i], m_spacing);

        // adjust around passenger, last part of lifetime
        adjustCoordinates (log + freeMovingIndex, trackCount -
            freeMovingIndex, paxRotation, m_spacing);

        m_logEntry.setEventList (log, trackCount);

		delete [] log;
		log = NULL;

        m_logEntry.incPax();
        m_pTerm->paxLog->updateItem (m_logEntry, m_logEntry.getIndex());
    }
    m_logEntry.clearLog();
}

