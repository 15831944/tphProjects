#pragma once


enum AirsideMobileElementMode
{
	//Flights
	OnBirth = 0,
	OnCruiseThrough,
	OnCruiseArr,
	OnTerminal,
	OnApproach,
	OnFinalApproach,
	OnMissApproach,
	OnLanding,
	OnLASHold,
	OnExitRunway,
	OnTaxiToMeetingPoint,

	OnTaxiToTempParking,
	OnEnterTempStand,
	OnEnterTempParking,
	OnHeldAtTempParking,

	OnExitTempStand,
	OnTaxiToStand,
	OnEnterStand,
	OnHeldAtStand,
	OnExitStand,

	OnTowToDestination,		//towing	
	
	OnTaxiToRunway,      //	
	OnQueueToRunway,
	OnTakeOffWaitEnterRunway,//
	OnTakeOffEnterRunway,
	OnPreTakeoff,
	OnTakeoff,
	OnClimbout,
	OnCruiseDep,
	OnTerminate,

	OnTaxiToDeice,	
	OnHoldArea,
	OnTaxiToHoldArea,

	OnWaitInHold,		//airspace hold


	//Vehicles
	OnVehicleBirth,
	OnParkingPool,
	OnMoveToService,
	//////////////////////////////////////////////////////////////////////////
	OnBeginToService , //for log 
	OnBeginToServiceToGate ,
	//////////////////////////////////////////////////////////////////////////
	OnMoveToDeiceService,
	//service
	OnMoveToRingRoute,
	OnServiceMovement,
	OnService,
	OnWaitingFlightArrive,
	OnVehicleMoveToTempParking,
	OnVehicleMoveOutTempParking,
	OnWaitingPreVehicle,
	OnLeaveServicePoint,
	OnLeaveServiceMovement,
	OnLeaveRingRoute,
	//service

	//paxbus
	OnMoveToGate,
	OnMoveInGate,
	OnMoveOutGate,
	OnArriveAtGate,  
	OnLeaveGate,
	//paxbus

	OnWaitForService,
	OnBackPool,
	OnVehicleTerminate,

	//baggage train
	OnMoveToBagTrainSpot,
	OnMoveInBagTrainSpot,
	OnMoveOutBagTrainSpot,
	OnArriveAtBagTrainSpot,  
	OnLeaveBagTrainSpot,


	MODE_COUNT,
};

enum FltDelayReason
{
	FltDelayReason_Unknown = 0,
	FltDelayReason_Slowed,
	FltDelayReason_Vectored,
	FltDelayReason_AirHold,
	FltDelayReason_RunwayHold,
	FltDelayReason_AltitudeChanged,
	FltDelayReason_SideStep,
	FltDelayReason_PushbackClearance,
	FltDelayReason_Stop,
	FltDelayReason_Service
};

extern char *AIRSIDEMOBILE_MODE_NAME[MODE_COUNT];


namespace FlightConflict
{
	enum ConflictType
	{
		AC_AC = 0,
		AC_GSE
	};

	enum OperationType
	{
		STAR =0,
		SID,
		ENROUTE,
		EXITRUNWAY,
		TAXIINBOUND,
		PUSHBACK_POWEROUT,
		TAXIOUTBOUND,
		UNDERTOWING,
		//exist order, if need add new, add it after here 

		UNKNOWNOP		//default value
	};

	enum LocationType
	{
		BETWEENWAYPOINTS =0,
		RUNWAY,
		EXIT,
		APPROACHINTERSECTION,
		BETWEENINTERSECTIONS,
		ENTERINGSTAND,
		LEAVINGSTAND,
		ENTERINGDEICEPAD,
		LEAVINGDEICEPAD,
		//exist order, if need add new, add it after here 
		DEICE,
		VEHICLE,

		UNKNOWNLOC		//default value
	};

	enum ActionType
	{
		SLOWED =0,
		ACCELERATED,
		VECTORED_AIR,
		HOLD_AIR,
		ALTITUDECHANGE,
		MISSAPPROACH,
		STOPPED,
		REROUTE,
		REASSIGN,
		TEMPPARKING,
		NOACTION
		//exist order, if need add new, add it after here 

	};

	const int ACTION_FIELD = 11;
	const int LOCATION_FIELD = 11;
	const int OPERATION_FIELD = 8;

	extern char * far ACTIONTYPE[];
	extern char * far LOCATIONTYPE[];
	extern char * far OPERATIONTYPE[];

}

namespace FlightRunwayDelay
{
	enum PositionType
	{
		LandingRoll =0,
		Existing,
		InQueue,
		AtHoldShort,
		InPosition,
		UNKNOWNPOS
	};

	const int POSITION_FIELD = 5;
	extern char * far POSITIONTYPE[];
}
