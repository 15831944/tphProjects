// FetchPersonInQueueEvent.h: interface for the FetchPersonInQueueEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FETCHPERSONINQUEUEEVENT_H__03AEAD94_0768_4B73_BE03_B6783A46BD4E__INCLUDED_)
#define AFX_FETCHPERSONINQUEUEEVENT_H__03AEAD94_0768_4B73_BE03_B6783A46BD4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TERMINALEVENT.H"
class Processor;
class FetchPersonInQueueEvent : public TerminalEvent  
{
	Processor* m_pProc;
public:
	FetchPersonInQueueEvent( Processor* _pProc );
	virtual ~FetchPersonInQueueEvent();
public:
	virtual int process ( InputTerminal* _pInTerm );
	

    //For shut down event
    virtual int kill (void){ return 1;};

    //It returns event's name
    virtual const char *getTypeName (void) const{ return "FetchPersonInQueue Event"; };

    //It returns event type
    virtual int getEventType (void) const { return FetchPersonInQueue;}

};

#endif // !defined(AFX_FETCHPERSONINQUEUEEVENT_H__03AEAD94_0768_4B73_BE03_B6783A46BD4E__INCLUDED_)
