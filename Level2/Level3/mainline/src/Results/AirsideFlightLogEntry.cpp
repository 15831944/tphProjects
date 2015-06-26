#include "StdAfx.h"

#include "AirsideFlightLogEntry.h"

AirsideFlightLogEntry::AirsideFlightLogEntry(void)
{
}

AirsideFlightLogEntry::~AirsideFlightLogEntry(void)
{
}

void AirsideFlightLogEntry::echo (ofsstream& p_stream, const CString& _csProjPath) const
{

}

ElapsedTime AirsideFlightLogEntry::getEntryTime (void) const
{
	ElapsedTime t;
	t.setPrecisely (item.startTime);
	return t;
}

ElapsedTime AirsideFlightLogEntry::getExitTime (void) const
{
	ElapsedTime t;
	t.setPrecisely (item.endTime);
	return t;
}

void AirsideFlightLogEntry::SetStartTime( long t )
{
	item.startTime = t;
}

void AirsideFlightLogEntry::SetEndTime( long t )
{
	item.endTime = t;
}