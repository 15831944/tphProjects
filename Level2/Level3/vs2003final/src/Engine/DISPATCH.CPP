#include "stdafx.h"
#include "engine\dispatch.h"
#include "engine\proclist.h"
#include "inputs\in_term.h"
#include <stdlib.h>

//Initializes a dispatch event.
void DispatcherEvent::init (int p_op, Dispatcher *p_disp, Station *p_stn, PeopleMover *p_mover)
{
    operation = p_op;
    dispatcher = p_disp;
    station = p_stn;
    mover = p_mover;
}

//It adds receiver to the event list
void DispatcherEvent::addEvent ()
{
    char *states[] =
    {
        "UPDATE_DATA",
        "ARRIVE_AT_STATION",
        "START_LOADING",
        "CLOSE_STATION",
        "DEPART_STATION"
    };

    ofsstream testFile ("dispatch.log", stdios::app);
    testFile << '\n' << "Schedule: " << time << " State: " << states[operation];
    if (station)
	{
		char str[32];
		station->getID()->printID (str);
        testFile << " Station: " << str;
	}
    if (mover)
        testFile << " Mover: " << mover->getID();
    testFile.close();

    m_pEventList->addItem (this);
}

//It removes a dispatched event from eventList.
void DispatcherEvent::removeEvent (Station *p_stn, int p_op, EventList* _pEventList )
{
    char *states[] =
    {
        "UPDATE_DATA",
        "ARRIVE_AT_STATION",
        "START_LOADING",
        "CLOSE_STATION",
        "DEPART_STATION"
    };

    Event *anEvent;
    DispatcherEvent *dispatch;

    int eventCount = _pEventList->getCount();
    for (int i = 0; i < eventCount; i++)
    {
        anEvent = _pEventList->getItem (i);
        if (anEvent->getEventType() != PeopleMoverEvent)
            continue;

        dispatch = (DispatcherEvent *) anEvent;
        if (dispatch->station == p_stn && dispatch->operation == p_op)
        {
           ofsstream testFile ("dispatch.log", stdios::app);
           testFile << '\n' << "Remove:   " << dispatch->time << " State: " << states[dispatch->operation];
           if (dispatch->station)
		   {
				char str[32];

				dispatch->station->getID()->printID (str);

				testFile << " Station: " << str;
		   }
           
           testFile.close();
           _pEventList->deleteItem (i);
           break;
        }
    }
}


int DispatcherEvent::process (InputTerminal* _pInTerm)
{
    char *states[] =
    {
        "UPDATE_DATA",
        "ARRIVE_AT_STATION",
        "START_LOADING",
        "CLOSE_STATION",
        "DEPART_STATION"
    };

    ofsstream testFile ("dispatch.log", stdios::app);
    testFile << '\n' << "Trigger:  " << time << " State: " << states[operation];
	char str[32];
    if (station)
	{
		station->getID()->printID (str);
        testFile << " Station: " << str;
	}
    if (mover)
	{
        testFile << " Mover: " << mover->getID();
	}
    testFile.close();


    if (operation == UPDATE_DATA)
        dispatcher->updateData (time);
    else if (operation == ARRIVE_AT_STATION)
        station->arriveAtStation (time);
    else if (operation == START_LOADING)
        station->openLoaders (time);
    else if (operation == CLOSE_STATION)
        station->closeStation (time);
    else if (operation == DEPART_STATION)
        station->departStation (time);
    return 1;
}

Dispatcher::~Dispatcher ()
{
    stationList.clear();
    PeopleMover *mover;
    while (peopleMovers.getCount())
    {
        mover = peopleMovers.getItem (0);
        delete mover;
    }
}

// The funcition loads and closes all stations and schedules initial events.
// it adds new movers to station's arrival list
void Dispatcher::init (PeopleMoverSystem *p_system,InputTerminal* _pInTerm)
{
    system = p_system;
    ProcessorIDList *stationIDs = system->getStations();
    GroupIndex group;
    Station *station;

    for (int i = 0; i < stationIDs->getCount(); i++)
    {
        group = _pInTerm->procList->getGroupIndex (*(stationIDs->getItem (i)));
        for (int j = group.start; j <= group.end; j++)
        {
            station = (Station *)_pInTerm->procList->getProcessor(j);
            station->initDispatcher (this);
            stationList.addItem (station);
        }
    }

    if (!stationList.getCount())
        return;

    // close all stations
    for (i = 0; i < stationList.getCount(); i++)
        (stationList.getItem (i))->closeStation ((ElapsedTime)0l);

    scheduleUpdate();
}

// schedules event that triggers change of system operations
void Dispatcher::scheduleUpdate ()
{
    if (curData+1 < system->getCount())
    {
        const PeopleMoverData *data = system->getItem (curData+1);
        DispatcherEvent *event = new DispatcherEvent;
        event->setTime (data->getTime());
        event->init (DispatcherEvent::UPDATE_DATA, this, NULL, NULL);
        event->addEvent();
    }
}

//It adds new vehicles, removes retiring vehicles and updates all vehicles' data
// check for retiring vehicles
void Dispatcher::updateData (ElapsedTime p_time)
{
    curData++;
    scheduleUpdate();

    PeopleMoverData *data = system->getItem (curData);

    // add new vehicles
    int newCount = data->getValue (PeopleMoverData::TRAINS);
    newCount = (newCount < 0)? 0: newCount;

    Station *firstStation = (Station *)stationList.getItem (0);
    PeopleMover *mover;
    while (newCount > peopleMovers.getCount())
    {
        mover = new PeopleMover (this);
        peopleMovers.addItem (mover);
        firstStation->addPeopleMover (mover, p_time);
    }

    // remove vehicles
    while (newCount < peopleMovers.getCount())
    {
        int ndx = random (peopleMovers.getCount());
        mover = peopleMovers.getItem (ndx);
        mover->shutDown();
        peopleMovers.removeItem (ndx);
    }

    // update vehicle data
    for (int i = 0; i < peopleMovers.getCount(); i++)
    {
        mover = peopleMovers.getItem (i);
        mover->initialize (data);
    }
}

//It gets next station id by current station
Station *Dispatcher::getNextStation (Station *p_current)
{
    int ndx = stationList.findItem (p_current) + 1;
    if (ndx == stationList.getCount())
        ndx = 0;
    return (Station *)stationList.getItem (ndx);
}

// It removes all passengers from a dispatch.
void Dispatcher::clearPassengers (ElapsedTime p_time)
{
    PeopleMover *mover;
    for (int i = peopleMovers.getCount() - 1; i >= 0; i--)
    {
        mover = peopleMovers.getItem (i);
        mover->clearPassengers (p_time);
    }
}

// allocate all dispatchers
void DispatcherList::initialize (PeopleMoverDataSet *p_dataSet, InputTerminal* _pInTerm )
{
    unlink ("dispatch.log");
    PeopleMoverSystem *system;
    Dispatcher *dispatcher;

    for (int i = 0; i < p_dataSet->getCount(); i++)
    {
        system = p_dataSet->getItem (i);

        // ignore systems with no data
        if (system->getCount())
        {
            dispatcher = new Dispatcher;
            dispatcher->init (system, _pInTerm);
            addItem (dispatcher);
        }
    }
}

//Removes passengers from a list of dispatches.
void DispatcherList::clearPassengers (ElapsedTime p_time)
{
    Dispatcher *dispatcher;
    for (int i = getCount() - 1; i >= 0; i--)
    {
        dispatcher = getItem (i);
        dispatcher->clearPassengers (p_time);
    }
}

