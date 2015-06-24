#pragma once
#include "event.h"
class CFreeMovingLogic;
class CARCportEngine;
class CCrossWalkInSim;

class LandsideConflictMoveEvent : public Event
{
public:
	LandsideConflictMoveEvent(CFreeMovingLogic* pPaxLogic);
	virtual ~LandsideConflictMoveEvent(void);

	virtual int process (CARCportEngine* pEngine );

	//For shut down event
	virtual int kill (void);

	//It returns event's name
	virtual const char *getTypeName (void) const;

	//It returns event type
	virtual int getEventType (void) const;

protected:
	CFreeMovingLogic *m_pPaxLogic;
};

class LandsideLeaveCrossWalkEvent : public Event
{
public:
	LandsideLeaveCrossWalkEvent(CFreeMovingLogic* pPaxLogic,CCrossWalkInSim* pCrossWalkInSim,const ElapsedTime& time);
	virtual ~LandsideLeaveCrossWalkEvent(void);

	virtual int process (CARCportEngine* pEngine );

	//For shut down event
	virtual int kill (void);

	//It returns event's name
	virtual const char *getTypeName (void) const;

	//It returns event type
	virtual int getEventType (void) const;

protected:
	CFreeMovingLogic *m_pPaxLogic;
	CCrossWalkInSim* m_pCrossWalkInSim;
};
