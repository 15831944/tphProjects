#ifndef EVENT_H
#define EVENT_H

#include "common\containr.h"
#include "common\elaptime.h"
#include "common\util.h"
#include "enginedll.h"
#include "inputs\in_term.h"


 //Subclass types;
enum EventType
{
	//Terminal
    Assignment,
    EntryEvent,
    BagEntry,
    BoardingCall,
    MobileMovement,
    GreetingEvent,
    PeopleMoverEvent,
	FlightEvent,
	TLOS,
	ExceedMaxTimeEvent,
	ElevatorMovingEvent,
	FireEvacuation,
	ConveyorStepItEvent,
	PusherMaxWaitTimeEvent,
	PusherScheduleEventType,
	ResourceMovementEvent,
	FetchPersonInQueue,
	MobileElementAvoidDensityEvent,
	CorralService,
	//Airside
	FlightBirth,
	FlightMovement,
	FlightGetClearnce,
	FlightExitResource,
	FlightEnterResource,
	FlightArrivalStand,
	FlightExitStand,
	FlightDelayTakeoffOnRwy,
	FlightTakeOffOnRwy,
	FlightDelayOnHold,

	FlightInHold,
	ApplyForTowServiceEvent,
	ApplyForDeiceServiceEvent,

	AirsideMobElementWriteLogEvent,
	
	FlightMoveToMeetingPoint,
	DepatureFlightStandEvent,

	FlightTerminateEvent,
	//Vehicle
	VehicleBirth,
	VehicleMove,
	VehicleService,
	TowTruckService,
	VehicleServiceGate,
	FollowMeCarService,
	AirsideMobileElementEvent,
	//Airside MobileEvent
	InAirsideMobElementMovementEvent,

	//wave cross
	CheckRunwayWaveCrossEvent,
	NotifyRunwayWaveCrossEvent,
	WakeUpFlightWaitingLandingEvent,
	WakeUpFlightWaitingTakeOffEvent,
	RemoveAirsideMobileElementEventEvent,
	WakeUpRunwayEven,
	FlightEnterLane,
	FlightExitLane,
	RemoveFlightFromTakeOffQueue,

	//step c
	ScheduleStep,
	OnboardPaxFreeMoveEvent,
	OnboardPaxStepEvent,
	OnboardPaxMoveToSeatEvent,
	CloseDoorEvent,

	//landside walk on graph
	LandsideConflictEvent,
	LandsideFreeMoveEvent,

	//sim core
	SendOutSignalEvent,
	AgentActiveEvent,
	AgentTerminatEvent,
	NormalEvent, //no particular type info
	MobStateEvent,

	MobElementExitEvent,
	Landside_VehicleServiceEndEvent,
	Landside_PaxGetoffEndEvent,
	EnumLandsideSimTimeStepEvent
};

/*****
*
*   Abstract event class
*   Contains event time and methods to execute event
*
*****/
class EventList;
class CARCportEngine;


typedef struct
{
	int nEventNum;
	CString csEventTime;
	CString csEventType;
}EventMsg;

// This is an abstract event class which contains event time and methods to execute event.
class ENGINE_TRANSFER Event
{
protected:

    //A main event list used for scheduling events
	static EventList* m_pEventList;

    // Event's scheduled firing time
    ElapsedTime time;

public:

    //Initializes eventList.
	static void initEvents (EventList *list) { m_pEventList = list; };
	static int getCurEventNum();

    Event () { time = 0l; };

    //copy constructor.
    Event (const Event& anEvent) { time = anEvent.time;  };

    virtual ~Event();

    //It sets the firing time of the event
    void setTime (ElapsedTime t) { time = t; };

    //It returns the firing time of the event
    ElapsedTime getTime (void) const { return time; };

    //It adds receiver to the event list
    virtual void addEvent ();

    // Main event processing routine for each Event type.
	//Modified by Xie Bo 2002.3.22  if return TRUE will delete this event
    virtual int process (CARCportEngine* pEngine ) = 0;
	
	//For shut down event
	virtual int kill (void) = 0;

    //It returns event's name
    virtual const char *getTypeName (void) const = 0;

    //It returns event type
    virtual int getEventType (void) const = 0;

    // ordinal operators (reqired for use in SortedArray)
    int operator < (const Event& anEvent) const { return time < anEvent.time; };
    int operator == (const Event& anEvent) const { return time == anEvent.time; };

	virtual bool IsAirsideMobileEllementEvent(){ return false;}
};


/*
Main Event Loop.
It stores, sorts, and executes all events.
*/
class ENGINE_TRANSFER EventList : public SortedContainer<Event>
{
protected:
    
	ElapsedTime startTime;
	bool m_bSetStartTime;
	
	ElapsedTime endTime;
	//add by adam 2007-09-26 fix bug arcport2007 NO.31
	ElapsedTime m_estSimulationStartTime;
	ElapsedTime m_estSimulationEndTime;
	//Endadd by adam 2007-09-26 fix bug arcport2007 NO.31
	
	ElapsedTime currentTime;

public:
	// current time of the engine
	ElapsedTime GetStartTime()const{ return startTime;	}
	ElapsedTime GetEndTime()const{ return endTime;	}
	ElapsedTime GetCurTime()const{ return currentTime; };
	//add by adam 2007-09-26 fix bug arcport2007 NO.31
	void SetSimulationStartTime(ElapsedTime estStartTime);
	void SetSimulationEndTime(ElapsedTime estEndTime);
	//Endadd by adam 2007-09-26 fix bug arcport2007 NO.31

    EventList () : SortedContainer<Event> (16, 0, 16){ m_bSetStartTime = true;ownsElements(1); };
    virtual ~EventList () {};

    // Simulation engine's main processing loop
    ElapsedTime start ( CARCportEngine *_pEngine, bool* _pBCancel, bool _bShowMsg );

    void kill (void);

	// in the sorted list find the first match.
	// return -1 for can not found.
	int FindFirstMatch( ElapsedTime _eTime, int _nStart = 0, int _nEnd = -1 );
	void deleteItem (int p_ndx);
	void removeItem (int p_ndx);
	void clear();
	Event* getItem(int p_ndx)const{ return (*this)[p_ndx]; }
};

#endif






















