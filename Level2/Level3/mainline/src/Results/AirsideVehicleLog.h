#pragma once

#include "results\baselog.h"
#include "results\AirsideVehicleLogEntry.h"

class AirsideVehicleLog : public BaseLog<AirsideVehicleDescStruct, AirsideVehicleEventStruct, AirsideVehicleLogEntry>
{
public:
	AirsideVehicleLog(void);
	AirsideVehicleLog(EventLog<AirsideVehicleEventStruct>* pAirsideEventLog)
		: BaseLog<AirsideVehicleDescStruct, AirsideVehicleEventStruct, AirsideVehicleLogEntry>()
	{ eventLog = pAirsideEventLog; }
	virtual ~AirsideVehicleLog(void);
public:
	void SetEventLog(EventLog<AirsideVehicleEventStruct>* pAirsideEventLog)
	{ eventLog = pAirsideEventLog; }
	long getRangeCount (ElapsedTime pstart, ElapsedTime pend);
	ElapsedTime getMinTime (void);
	ElapsedTime getMaxTime (void);
	bool GetItemByID( AirsideVehicleLogEntry& _entry , long _lID );
	void SaveEventToFile(const char *pfilename);
};
