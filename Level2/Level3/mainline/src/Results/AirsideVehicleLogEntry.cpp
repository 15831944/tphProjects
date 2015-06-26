#include "StdAfx.h"
#include ".\airsidevehiclelogentry.h"

AirsideVehicleLogEntry::AirsideVehicleLogEntry(void)
{
}

AirsideVehicleLogEntry::~AirsideVehicleLogEntry(void)
{
}

void AirsideVehicleLogEntry::echo (ofsstream& p_stream, const CString& _csProjPath) const
{

}

ElapsedTime AirsideVehicleLogEntry::getEntryTime (void) const
{
	ElapsedTime t;
	t.setPrecisely (item.startTime);
	return t;
}

ElapsedTime AirsideVehicleLogEntry::getExitTime (void) const
{
	ElapsedTime t;
	t.setPrecisely (item.endTime);
	return t;
}