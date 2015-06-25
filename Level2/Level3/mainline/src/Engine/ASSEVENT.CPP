
/*****
*
*   Class       ProcessorAssignmentEvent
*   Author      Lincoln Fellingham
*   Date        March 1994
*   Purpose     Handles timing of changes to processor assignments
*               Instructs one group's assignment list to increment the
*               current assignment and notify its processors of the change
*
*****/
#include "stdafx.h"
#include "engine\assevent.h"
#include "inputs\assign.h"
#include "inputs\assigndb.h"
#include "common\CodeTimeTest.h"
#include <CommonData/procid.h>
#include "../Common/ARCTracker.h"
#include "Engine/ARCportEngine.h"
#include "Engine/terminal.h"

//It assigns an entry to its attribute, the processorAssignment, and 
//schedules the next event.
void ProcessorAssignmentEvent::init (ProcAssignEntry *entry)
{
    processorAssignment = entry;
    scheduleEvent();
}
 
int ProcessorAssignmentEvent::scheduleEvent ()
{
    ProcessorRosterSchedule *schedule = processorAssignment->getSchedule();
    ProcessorRoster *nextAssignment = schedule->getNextProcRoster();
    // nextAssignment will equal NULL when the last assignment is current
    if (nextAssignment)
    {
		
        time = nextAssignment->getAbsOpenTime();
		// TRACE("=======================event = %s , %s, %d\n", this->processorAssignment->getID()->GetIDString(), time.printTime(), nextAssignment->isOpen() );
		addEvent();
        return FALSE;
    }
    else
        return TRUE;
}

 
int ProcessorAssignmentEvent::process (CARCportEngine *pEngine)
{
	PLACE_METHOD_TRACK_STRING();

	TRACE("=======================Process event = %s , %s, %d\n", this->processorAssignment->getID()->GetIDString(), time.printTime(), 0 );
	processorAssignment->updateProcessors(pEngine->getTerminal()->procList);
    return scheduleEvent();
}

