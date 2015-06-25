#pragma once
#include "commondll.h"

enum OnBoardEvents
{
	 ONBOARD_EVENT_ENUM_MIN = 100,
	 EnterOnBoardMode,
     ArriveAtEntryDoor,
	 ArriveAtExitDoor,
	 FreeMovingOnDeck,
	 WayFindingOnDeck,
//	 ArriveAtSeat,
	 SeatingOnChair,
	 WaitingForEnplane,
	 WaitingForDepalne,
	 WaitingForCarrierTakeOff,
	 WaitingForLeaveCarrier,
	 ONBOARD_EVENT_ENUM_MAX
};

COMMON_TRANSFER extern char *ONBOARD_STATE_NAMES[];

