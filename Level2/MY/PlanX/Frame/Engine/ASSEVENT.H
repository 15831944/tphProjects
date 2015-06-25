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

#ifndef PROC_ASSIGNMENT_EVENT_H
#define PROC_ASSIGNMENT_EVENT_H
              
#include "engine\terminalevent.h"
#include "common\template.h"
#include "common\containr.h"
#include "enginedll.h"

/*
The purpose of this class is to handle timing of changes to processor 
assignments and to instructs one group's assignment list to increment the
current assignment and notify its processors of the change
*/
class ENGINE_TRANSFER ProcessorAssignmentEvent : public TerminalEvent
{
protected:

    //an assignment list of processors that the event will be updating
    ProcAssignEntry *processorAssignment;

public:

    ProcessorAssignmentEvent() {};

    //copy constructor.
    ProcessorAssignmentEvent (ProcessorAssignmentEvent& anEvent)
        { processorAssignment = anEvent.processorAssignment; };

    virtual ~ProcessorAssignmentEvent () {};

    //It initializes the assignment group and schedules the first event.
    void init (ProcAssignEntry *entry);

    // The function updates the assignment entries for one assignment group.
    // It returns FALSE until last assignment of the group is reached.
    virtual int process (CARCportEngine *pEngine);

    //It is for shut down event.
    virtual int kill (void) { return 0; };

    // The function determines time of next assignment and schedules an event.
    // It returns FALSE if no more assignment changes.
    int scheduleEvent ();

    //It returns an event's name
    virtual const char *getTypeName (void) const { return "Assignment"; };

    //It returns an event type
    int getEventType (void) const { return Assignment; };
};

#endif
