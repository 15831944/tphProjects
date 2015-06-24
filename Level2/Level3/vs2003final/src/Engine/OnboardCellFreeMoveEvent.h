#pragma once
#include "onboard\onboardevent.h"

class OnboardCellFreeMoveLogic;
class OnboardCellFreeMoveEvent :
	public OnBoardEvent
{
public:
	OnboardCellFreeMoveEvent(OnboardCellFreeMoveLogic *pFreeMoveLogic);
	~OnboardCellFreeMoveEvent(void);


public:
	virtual int process (CARCportEngine* pEngine );

	//For shut down event
	virtual int kill (void);

	//It returns event's name
	virtual const char *getTypeName (void) const;

	//It returns event type
	virtual int getEventType (void) const ;
protected:
	OnboardCellFreeMoveLogic *m_pFreeMoveLogic;

};
