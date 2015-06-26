#pragma once


#include <afx.h>
#include "results\logitem.h"
#include "common\AIRSIDE_BIN.h"
#include "common\fsstream.h"


class AirsideFlightLogEntry : public LogItem<AirsideFlightDescStruct, AirsideFlightEventStruct>
{
public:
	AirsideFlightLogEntry(void);
	virtual ~AirsideFlightLogEntry(void);
public:
	virtual void echo (ofsstream& p_stream, const CString& _csProjPath) const;

	ElapsedTime getEntryTime (void) const;
	ElapsedTime getExitTime (void) const;

	AirsideFlightDescStruct GetAirsideDescStruct() const{ return item; }
	AirsideFlightDescStruct& GetAirsideDesc()  { return item; }
	const AirsideFlightDescStruct& GetAirsideDesc() const { return item; }

	void SetStartTime(long t);
	void SetEndTime(long t); 
};
