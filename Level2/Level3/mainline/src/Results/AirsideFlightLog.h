#pragma once

#include "results\baselog.h"
#include "results\AirsideFlightLogEntry.h"

class AirsideFlightLog : public BaseLog<AirsideFlightDescStruct, AirsideFlightEventStruct, AirsideFlightLogEntry>
{
public:
	AirsideFlightLog(void);
	AirsideFlightLog(EventLog<AirsideFlightEventStruct>* pAirsideEventLog);
	virtual ~AirsideFlightLog(void);
public:
	void SetEventLog(EventLog<AirsideFlightEventStruct>* pAirsideEventLog)
	{ eventLog = pAirsideEventLog; }
	long getRangeCount (ElapsedTime pstart, ElapsedTime pend);
	ElapsedTime getMinTime (void);
	ElapsedTime getMaxTime (void);
	bool GetItemByID( AirsideFlightLogEntry& _entry , long _lID );
	void SaveEventToFile(const char *pfilename);
};
