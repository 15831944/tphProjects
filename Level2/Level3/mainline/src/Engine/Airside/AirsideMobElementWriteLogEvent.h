#pragma once
#include "airsideevent.h"

class AirsideMobElementWriteLogItem;


//airside flight or vehicle write log event
//base on Airside Event
class CAirsideMobElementWriteLogEvent :
	public AirsideEvent
{
public:
	CAirsideMobElementWriteLogEvent(AirsideMobElementWriteLogItem* logItem);
	~CAirsideMobElementWriteLogEvent(void);

public:
	virtual const char *getTypeName (void) const;

	//It returns event type
	virtual int getEventType (void) const;
	//handle the event
	int Process();

protected:
	AirsideMobElementWriteLogItem* m_pLogItem;
};
