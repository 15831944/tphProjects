// ExceedMaxTimeCheckEvent.h: interface for the ExceedMaxTimeCheckEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXCEEDMAXTIMECHECKEVENT_H__D4ACC888_4E20_468E_9B4F_B44687EE86D2__INCLUDED_)
#define AFX_EXCEEDMAXTIMECHECKEVENT_H__D4ACC888_4E20_468E_9B4F_B44687EE86D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TERMINALEVENT.H"
class BaggageProcessor;
class Person;
class CARCportEngine;
class ExceedMaxTimeCheckEvent : public TerminalEvent  
{
	BaggageProcessor* m_pBaggageProc;
	//Person* m_pPersonToBeChecked;//
	long m_lPersonToBeChecked;
	CString m_strMobType;
public:
	ExceedMaxTimeCheckEvent( BaggageProcessor* _pBaggageProc, long _lPersonID ,CString strMobType);
	virtual ~ExceedMaxTimeCheckEvent();



	virtual int process ( CARCportEngine *_pEngine );
    //For shut down event
    virtual int kill (void){ return 1;};

    //It returns event's name
    virtual const char *getTypeName (void) const{ return "Exceed Max WaitingTime Check Event"; };

    //It returns event type
    virtual int getEventType (void) const { return ExceedMaxTimeEvent;}

};

#endif // !defined(AFX_EXCEEDMAXTIMECHECKEVENT_H__D4ACC888_4E20_468E_9B4F_B44687EE86D2__INCLUDED_)
