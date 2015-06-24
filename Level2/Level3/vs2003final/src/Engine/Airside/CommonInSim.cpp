#include "StdAfx.h"
#include "CommonInSim.h"

char *AIRSIDEMOBILE_MODE_NAME[MODE_COUNT] =
{
//Flights
	"OnBirth" ,
	"OnCruiseThrough",
	"OnCruiseArr",
	"OnTerminal",
	"OnApproach",
	"OnFinalApproach",
	"OnMissApproach",
	"OnLanding",
	"OnLASHold",
	"OnExitRunway",
	"OnTaxiToMeetingPoint",

	"OnTaxiToTempParking",
	"OnEnterTempStand",
	"OnEnterTempParking",
	"OnHeldAtTempParking",
	
	"OnExitTempStand",
	"OnTaxiToStand",
	"OnEnterStand",
	"OnHeldAtStand",
	"OnExitStand",

	"OnTowToDestination",		//	

	"OnTaxiToRunway",      //	
	"OnQueueToRunway",
	"OnTakeOffWaitEnterRunway",//
	"OnTakeOffEnterRunway",
	"OnPreTakeoff",
	"OnTakeoff",
	"OnClimbout",
	"OnCruiseDep",
	"OnTerminate",

	"OnTaxiToDeice",	
	"OnHoldArea",
	"OnTaxiToHoldArea",
	"OnWaitInHold",//wait in hold

	//vehicle
	"OnVehicleBirth",
	"OnParkingPool",
	"OnMoveToService",
	//////////////////////////////////////////////////////////////////////////
	"OnBeginToService" , //for log 
	"OnBeginToServiceToGate" ,
	//////////////////////////////////////////////////////////////////////////
	"OnMoveToDeiceService",
	//service
	"OnMoveToRingRoute",
	"OnServiceMovement",
	"OnService",
	"OnWaitingFlightArrive",
	"OnVehicleMoveToTempParking",
	"OnVehicleMoveOutTempParking",
	"OnWaitingPreVehicle",
	"OnLeaveServicePoint",
	"OnLeaveServiceMovement",
	"OnLeaveRingRoute",
	//service

	//paxbus
	"OnMoveToGate",
	"OnMoveInGate",
	"OnMoveOutGate",
	"OnArriveAtGate",  
	"OnLeaveGate",
	//

	"OnWaitForService",
	"OnBackPool",
	"OnVehicleTerminate",
};

namespace FlightConflict
{
	char * far OPERATIONTYPE[]=
	{
		"STAR",
			"SID",
			"En-route",
			"Exit Runway",
			"Taxi Inbound",
			"Pushback/Power out",
			"Taxi Outbound",
			"Under Towing",
			"Unknown"
	};

	char * far LOCATIONTYPE[]=
	{
		"Between waypoints",
			"Runway",
			"Exit",
			"Approach intersections",
			"Between intersections",
			"Entering stand",
			"Leaving stand",
			"Enter deice pad",
			"Leaving deice pad",
			"Deice",
			"Vehicle",
			"Unknown"
	};

	char * far ACTIONTYPE[]=
	{
		"Slowed",
			"Accelerate",
			"Vector(Air)",
			"Hold(Air)",
			"Altitude change",
			"Miss approach",
			"Stopped",
			"Reroute",
			"Reassign",
			"Temp parking",
			"No action"
	};
}

namespace FlightRunwayDelay
{
	char * far POSITIONTYPE[]=
	{
		"Landing Roll",
		"Exiting",	
		"In Takeoff Queue",
		"At Hold Short",
		"In Position"
	};
}