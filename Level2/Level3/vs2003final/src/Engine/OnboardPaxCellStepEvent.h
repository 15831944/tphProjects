#pragma once

#include "event.h"


class OnboardCellFreeMoveLogic;
class ElapsedTime;

class ENGINE_TRANSFER OnboardPaxCellStepEvent: public Event
{
public:
	OnboardPaxCellStepEvent(OnboardCellFreeMoveLogic  *pPaxLogic, const ElapsedTime& eTime);
	~OnboardPaxCellStepEvent(void);



public:

	virtual int process (CARCportEngine* pEngine );

	//For shut down event
	virtual int kill (void);

	//It returns event's name
	virtual const char *getTypeName (void) const;

	//It returns event type
	virtual int getEventType (void) const;



protected:
	OnboardCellFreeMoveLogic *m_pPaxLogic;


};
