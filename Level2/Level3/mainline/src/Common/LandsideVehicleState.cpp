#include "stdafx.h"
#include "LandsideVehicleState.h"


char *LANDSIDEVEHICLE_STATE_NAMES[]=
{
	"onBirth", //wait for birth
	"moveToDest", //move in stretch to next resource 
	"approachDest", //move and try parking
	"beginProcessAtDest",
	"processAtDest", //parking at the dest
	"leaveCurrent",   //leave the dest
	"waitforleave",
	"waitOnHomeBase",
	"terminate",

};