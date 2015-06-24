/*****
*
*   Class       BaggageEntryEvent
*   Author      Lincoln Fellingham
*   Date        May 1994
*   Purpose     Handles timing of arriving bags arriving at baggage
*               carosels in the terminal
*
*               Designed to have only one instance in memory at a time
*               To load entire event list some members must become static
*
*****/
#include "stdafx.h"
#include "engine\bagenter.h"
#include "results\baglog.h"
#include "engine\proclist.h"
#include "engine\baggage.h"
#include "inputs\in_term.h"
#include "common\exeption.h"
#include "common\CodeTimeTest.h"
#include "../Common/ARCTracker.h"


//Since there is no bag currently processed, the currnetCount is set zero; 
//the total bag count is equal to the number of bags of input bag list.
void BaggageEntryEvent::initBagList (BagLog *p_list)
{
    entryList = p_list;
    currentCount = 0;

    bagCount = p_list->getCount();
    scheduleNextEntry();
}

//It return FALSE if no more bag. Otherwise, it gets the event of next bag,
//sets event time as the fired time and adds the next bag event to the static 
//event list.
int BaggageEntryEvent::scheduleNextEntry ()
{
    if (currentCount >= bagCount)
        return FALSE;

    entryList->getItem (nextEntry, currentCount++);
    time = nextEntry.getEntryTime();
    addEvent();
    return TRUE;
}


int BaggageEntryEvent::process (InputTerminal* _pInTerm)
{
	PLACE_METHOD_TRACK_STRING();
	BaggageProcessor *carosel;
    carosel = (BaggageProcessor *)_pInTerm->procList->getProcessor (nextEntry.getProc());

	ProcessorID* pId = (ProcessorID*)carosel->getID();
	pId->SetStrDict( _pInTerm->inStrDict );
	char buf[256];
	pId->printID( buf );
//	// TRACE( "baggage carosel: %s\n", buf );
    ///carosel->addBag (new BagLogEntry (nextEntry), time);

    scheduleNextEntry();
    return FALSE;
}

