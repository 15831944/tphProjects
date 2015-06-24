// PusherScheduleEvent.h: interface for the PusherScheduleEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUSHERSCHEDULEEVENT_H__6E24DE23_A668_4958_A986_B8A3CB564956__INCLUDED_)
#define AFX_PUSHERSCHEDULEEVENT_H__6E24DE23_A668_4958_A986_B8A3CB564956__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TERMINALEVENT.H"
class Pusher;
class CARCportEngine;
class PusherScheduleEvent : public TerminalEvent  
{
	Pusher* m_pWhichPusher;
public:
	PusherScheduleEvent( Pusher* _pPusher );
	virtual ~PusherScheduleEvent();

	virtual int process ( CARCportEngine *_pEngine );
    //For shut down event
    virtual int kill (void){ return 1;};

    //It returns event's name
    virtual const char *getTypeName (void) const{ return "Pusher Schedule Event"; };

    //It returns event type
    virtual int getEventType (void) const { return PusherScheduleEventType;}

};

#endif // !defined(AFX_PUSHERSCHEDULEEVENT_H__6E24DE23_A668_4958_A986_B8A3CB564956__INCLUDED_)
