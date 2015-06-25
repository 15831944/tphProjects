#ifndef STATES_H
#define STATES_H
#include "commondll.h"

enum EntityEvents
{
	//terminal behavior state
	 TerminalBegin,
	 TerminalEnd,
	 Birth,
     Death,
     FreeMoving,
     StartMovement,
     MoveAlongInConstraint,
     MoveToQueue,
	 WaitInQueue,
	 JumpQueue,
     LeaveQueue,
     MoveToServer,
     ArriveAtServer,
     LeaveServer,
     MoveAlongOutConstraint,
	 WaitInHoldingArea,
     LeaveHoldingArea,
	 WaitForBaggage,
	 WaitForVehicle,
     BoardVehicle,
	 OnVehicle,
     LeaveVehicle,
     PickupPassenger,
     DropoffPassenger,
     PickupBag,
     DropoffBag,
	 OpenForService,
     StartToClose,
     CloseForService,
	 IncreaseQueue,
	 DecreaseQueue,
     BeginService,
     CompleteService,
	 TrainAtStation,
	 TrainAccelerate,
	 TrainDecelerate,
	 TrainOnNormalSpeed,
	 TrainFreeMoving,
	 TrainArriveAtStation,
	 TrainWaitForLeave,
	 TrainLeaveStation,
	 OnMovingSideWalk,
	 WalkOnPipe,
	 WaitingForOwner,
	 WaitingForOwnerPickUpOtherBag,
	 WaitingForBags,
	 Greeting,
	 BagArriveAtBaggageProc,
	 BagLeaveBaggageProc,
	 FlightArrivalAtAirport,
	 FlightDepartureAirport,
	 LeaveInConstraint,
	 PaxArriveAtBaggageProc,
	 PaxLeaveBaggageProc,
	 EvacuationFire,
	 WaitForEmptySlot,
	 MovingInConveyor,
	 MovingOnPusher,
	 Resource_Stay_In_Base,
	 Resource_Arrival_Processor,
	 Resource_Leave_Processor,
	 Resource_Back_To_Base,
	 ProcessorStateStop,
	 ProcessorStateRun,
	 AvoidDensitying,
	 StopByBillboard,
	 WalkThroughBillboard,
	 CompleteServiceBillboardLinkedProc,


	//Entry Airside----------------------
	EntryAirside,
	WalkOnBridge,

	TAKE_OFF_FLIGHT ,
	TAKE_OFF_WAITFOR_BUS,
	HOLDAREA_FOR_BUS,
	WAIT_FOR_BUS ,
	WAIT_FOR_FLIGHT ,
	TAKE_ON_BUS ,
	TAKE_OFF_BUS_TOFLIGHT ,
	TAKE_OFF_BUS_TOGATE,
	WAITE_FOR_GATEOPEN ,
	ARRIVE_AT_GATE ,
	//Flight Departure
	WALK_OUT_GATE ,
	TAKE_ON_FLIGHT ,
	_DEATH ,
	TAKE_OFF_FLGHT_WITH_NOBUS,

	//bag
	MOVETOBAGCART,
	ARRIVEATBAGCART,
	STAYATCART,
	LEAVEBAGCART,
	MOVETOFLIGHTCARGO,
	ARRIVEATFLIGHTCARGO,
	LEAVEFLIGHTCARGO,
	//Exit Airside----------------------





	//---------------entry on board----------------------------
	EntryOnboard,// entry onboard system, if no onboard selected, flush log
	OnboardFreeMoving,//move to door
	OnboardFreeMoveEnd,
	ArriveAtDoor,
	ArriveAtStorage,
	PutHandsUp,
	StorageCarryOn,
	PutHandsDown,
	ArriveAtCheckingPoint,
	MoveToStorage,//for non passenger move to storage
	ReturnToCheckingPoint,
	WaittingAtCheckingPoint,
	StrideOverSeat,
	ArriveAtTempPlace,
	WaitAtTempPlace,
	ArriveAtSeatEntry,//arrive at seat entry point
	ArriveAtSeat,
	SitOnSeat,//pax seat on seat
	StowCarryonToSeat,//passenger arrive at seat, and put his carryon to seat staging
	MoveUnderSeatGround,//non pax under seat
	MoveToDeckGround,//non pax state
	WaitTakeOff,
	StandUp,//pax stand up when flight stop at stand
	ArriveAtDeviceEntry,
	ServiceOnDevice,
	LeavesDevice,
	MoveToDest,


	//-------------exit on board---------------------------
	//
	EntryLandside,
	On_Vehicle,
	GetOn_Vehicle,
	GetOff_Vehice,

	WaitFor_Vehicle,
	GoTo_VehicleParkingPlace,
	GoTo_Curbside,
// 	GoTo_BusStation,
	Put_Bag,
		
	Pick_Bag,
	WaitFor_Visitor,
	ArrivalAtCubside,
	ArrivalAtTerminalPosition,

	Landside_PaxLeaveQueue,

	//Facility Operation
	MoveToFacility,
	ArriveAtFacility,
	LeaveFacility,
	LandsideLeaveQueue,

	//Vehicle Operation
	WaitingForVehicle,
	MoveToVehicle,
	PutCarryonToVehicle,
	GetOnVehicle,
	StayOnVehicle,
	GetOffVehicle,
	PickCarryOnFromVehicle,

	//--------state on landside
	//
	MoveToInterestedEntryPoint,
	 EVENT_ENUM_MAX
};

COMMON_TRANSFER extern char *STATE_NAMES[];

#endif
