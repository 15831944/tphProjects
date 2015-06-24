// FireEvacuationEvent.h: interface for the FireEvacuationEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIREEVACUATIONEVENT_H__2565815F_542A_43E3_9215_EAABA0E20C29__INCLUDED_)
#define AFX_FIREEVACUATIONEVENT_H__2565815F_542A_43E3_9215_EAABA0E20C29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TERMINALEVENT.H"

class FireEvacuationEvent : public TerminalEvent  
{
public:
	FireEvacuationEvent( );
	virtual ~FireEvacuationEvent();
public:
	virtual int process ( CARCportEngine* pEngine  );
    //For shut down event
    virtual int kill (void){ return 1;};

    //It returns event's name
    virtual const char *getTypeName (void) const{ return "Fire Evacuation Event"; };

    //It returns event type
    virtual int getEventType (void) const { return FireEvacuation;}
private:
	//process all person who still generate event
	void ProcessActivePerson( InputTerminal* _pInTerm );

	// process all person who are inactive ( such as ,visitor wait in holding area for his owner, person wait in queue...)
	void ProcessInActivePerson( InputTerminal* _pInTerm );

	//caculate the mid point of two log point
	Point CaculateCurPositionOfPerson( const ElapsedTime& _timeEvent , MobileElement* _pElement);
};

#endif // !defined(AFX_FIREEVACUATIONEVENT_H__2565815F_542A_43E3_9215_EAABA0E20C29__INCLUDED_)
