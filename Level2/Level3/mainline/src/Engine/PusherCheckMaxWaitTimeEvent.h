// PusherCheckMaxWaitTimeEvent.h: interface for the PusherCheckMaxWaitTimeEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUSHERCHECKMAXWAITTIMEEVENT_H__4504B328_F6B4_46F9_B904_B79BD3B14228__INCLUDED_)
#define AFX_PUSHERCHECKMAXWAITTIMEEVENT_H__4504B328_F6B4_46F9_B904_B79BD3B14228__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Pusher;
#include "TERMINALEVENT.H"

class CARCportEngine;
class PusherCheckMaxWaitTimeEvent : public TerminalEvent  
{
	Pusher* m_pWhichPusher;
	long m_lBaggageToBeChecked;
public:
	PusherCheckMaxWaitTimeEvent( Pusher* _pPusher, long _lBaggageToBeChecked );
	virtual ~PusherCheckMaxWaitTimeEvent();

	
	virtual int process ( CARCportEngine *_pEngine );
    //For shut down event
    virtual int kill (void){ return 1;};

    //It returns event's name
    virtual const char *getTypeName (void) const{ return "Pusher Check Max WaitingTime Event"; };

    //It returns event type
    virtual int getEventType (void) const { return PusherMaxWaitTimeEvent;}

};

#endif // !defined(AFX_PUSHERCHECKMAXWAITTIMEEVENT_H__4504B328_F6B4_46F9_B904_B79BD3B14228__INCLUDED_)
