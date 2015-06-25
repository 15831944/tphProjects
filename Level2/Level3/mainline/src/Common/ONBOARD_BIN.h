#pragma once

#include "template.h"
#include "elaptime.h"
#include "StartDate.h"
#include "IDGather.h"

#pragma pack(push, 1)

// Mobile Agent information
typedef struct
{
    long    id;                 // Mobile Element identification
    long    startTime;          // First hit time
    long    endTime;            // Last hit time
    long    startPos;           // pointer to start of track in binary file
    long    endPos;             // pointer to end of track in binary file
	char	mobileType;			// the index to the mobiletype list.
    char    intrinsic;          // basic type
    short   paxType[MAX_PAX_TYPES];  // type description
    short   arrFlight;          // index of element's arriving flight in list
    short   depFlight;          // index of element's departing flight in list
    char    groupSize;          // size of group (1+)
    char    groupMember;        // element's group member number (0+)
    char    bagCount;           // number of bags (group), -1 if none
    char    cartCount;          // number of carts (group), -1 if none
    long    ownerID;	         // pax's visitor or visitor's pax, -1 if none
    long    indexNum;           // index of pax in log
	float   speed;				// speed of the person moves.
	char	ownStart;			// has its own start.
	char	eachPaxOwnNoPax;	// is not group own no_pax.
	bool	bMissFlightDeath;	// the flag of pax miss flight.
} MobAgentDescStruct;

typedef struct _MobAgentDescStructEx {
	COLORREF		color;
	BOOL			visible;
	BOOL			leavetrail;
	int				shape;
	int				linetype;
	BOOL			showtags;
	DWORD			taginfo;
	int				displaylistid; //index in PaxDispProps -1 if default, -2 if overlap
} MobAgentDescStructEx;

// Tracking information
typedef struct _MobAgentEventStruct
{
    long    time;               // current hit time (1/18.2 of second)
    float   x;                  // x position of hit in nm
    float   y;                  // x position of hit in nm
    short   z;                  // z position of hit (floor)
    char    state;              // state of element at this hit (states.h)
    short   procNumber;         // index of current processor
	bool	bDynamicCreatedProc;// ( currently ,only used in TLOS Created new processor dynamicly ) it means whether or not processor( procNumber ) is dynamicly created
	bool	followowner;		// flag visitor is moveing with its owner or not; to pax,the flag is invalidate
	short   reason;				// ( currently, only used in conveyor system ) which processor cause me stop
	bool    backup;				// point the direction of out constrain 
	double  speed ;             //the speed  
	_MobAgentEventStruct()
	{
       speed = 0 ;
	};
} MobAgentEventStruct;               // 20 bytes



#pragma pack(pop)

