#pragma once
#include "commondll.h"

enum eLandsideVehicleState
{
	_onBirth, //wait for birth
	_moveToDest, //move in stretch to next resource 
	_approachDest, //move and try parking
	_beginProcessAtDest,
	_processAtDest, //parking at the dest
	_leaveCurrent,   //leave the dest
	_waitforleave,  //wait for leave current route item, for taxi pool

	_waitOnHomeBase,
	_terminate,

	//_shuttlebus_arrivebusstation,
	//_shuttlebus_paxservice,
	//_shuttlebus_leavebussation
	LANDSIDE_STATE_NUM,

};	
COMMON_TRANSFER extern char *LANDSIDEVEHICLE_STATE_NAMES[];
