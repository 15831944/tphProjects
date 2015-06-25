#include "stdafx.h"
#include "MoveToInterestedEntryEvent.h"

MoveToInterestedEntryEvent::MoveToInterestedEntryEvent()
{

}

MoveToInterestedEntryEvent::~MoveToInterestedEntryEvent()
{

}

void MoveToInterestedEntryEvent::init(MobileElement *aMover, ElapsedTime eventTime, bool bMoLog)
{
	MobileElementMovementEvent::init(aMover, eventTime , bMoLog);
}

// processor event
int MoveToInterestedEntryEvent::process (CARCportEngine *_pEngine)
{

	return 0;
}
