#pragma once
#include "event.h"



class OnboardCellSeatGivePlaceLogic;
class PaxOnboardBaseBehavior;
class ElapsedTime;

class OnboardPaxCellMoveToSeatEvent :
	public Event
{
public:
	OnboardPaxCellMoveToSeatEvent(OnboardCellSeatGivePlaceLogic *pLogic,
									PaxOnboardBaseBehavior* pPax,
									const ElapsedTime& eTime);
	~OnboardPaxCellMoveToSeatEvent(void);



public:

	virtual int process (CARCportEngine* pEngine );

	//For shut down event
	virtual int kill (void);

	//It returns event's name
	virtual const char *getTypeName (void) const;

	//It returns event type
	virtual int getEventType (void) const;

	ElapsedTime removeMoveEvent(PaxOnboardBaseBehavior *anElement);

protected:
	OnboardCellSeatGivePlaceLogic *m_pPaxLogic;
	PaxOnboardBaseBehavior *m_pPaxBehavior;

	bool m_bCancel;

};
