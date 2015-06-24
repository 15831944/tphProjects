// ElevatorMoveEvent.h: interface for the ElevatorMoveEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEVATORMOVEEVENT_H__075ABBAA_00A7_46C6_AE81_38F61FBD9797__INCLUDED_)
#define AFX_ELEVATORMOVEEVENT_H__075ABBAA_00A7_46C6_AE81_38F61FBD9797__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TERMINALEVENT.H"
class ElevatorProc;
class CARCportEngine;

class ElevatorMoveEvent : public TerminalEvent  
{
	ElevatorProc* m_pElevator;
	int m_iLift;
public:
	ElevatorMoveEvent( ElevatorProc* _pElevator, int _iLift );
	virtual ~ElevatorMoveEvent();

	virtual int process (CARCportEngine *_pEngine );
    //For shut down event
    virtual int kill (void){ return 1;};

    //It returns event's name
    virtual const char *getTypeName (void) const{ return "Elevator moving Event"; };

    //It returns event type
    virtual int getEventType (void) const { return ElevatorMovingEvent;}


};

#endif // !defined(AFX_ELEVATORMOVEEVENT_H__075ABBAA_00A7_46C6_AE81_38F61FBD9797__INCLUDED_)
