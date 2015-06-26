#pragma once

#include <afx.h>
#include "results\logitem.h"
#include "common\AIRSIDE_BIN.h"
#include "common\fsstream.h"

class AirsideVehicleLogEntry : public LogItem<AirsideVehicleDescStruct, AirsideVehicleEventStruct>
{
public:
	AirsideVehicleLogEntry(void);
	virtual ~AirsideVehicleLogEntry(void);

public:
	virtual void echo (ofsstream& p_stream, const CString& _csProjPath) const;

	ElapsedTime getEntryTime (void) const;
	ElapsedTime getExitTime (void) const;

	const AirsideVehicleDescStruct& GetAirsideDesc() const{ return item; }
	AirsideVehicleDescStruct& GetAirsideDesc()  { return item; }

	void SetAirsideDescStruct( const AirsideVehicleDescStruct desc ) { item = desc; }

	void SetStartTime(long t){ item.startTime = t; }
	void SetEndTime(long t) { item.endTime = t; } 
};
