/*****
*
*   Class       GreetingEvent
*   Author      Lincoln Fellingham
*   Date        May 1996
*   Purpose     Processes "greeting" between a visitor and an arrival
*
*****/
#include "stdafx.h"
#include "engine\greet.h"
#include "engine\pax.h"
#include "common\CodeTimeTest.h"
#include "../Common/ARCTracker.h"
#include "PaxTerminalBehavior.h"


// It joins passenger and greeter.
// It always returns true.  
int GreetEvent::process(CARCportEngine *_pEngine)
{
	PLACE_METHOD_TRACK_STRING();
	PaxTerminalBehavior* spTerminalBehavior = (PaxTerminalBehavior*)pax->getBehavior(MobElementBehavior::TerminalBehavior);
	if (spTerminalBehavior)
	{
		spTerminalBehavior->greet (greeter, time);
	}
	
    return TRUE;
}

