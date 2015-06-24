// ConveyorStepItEvent.h: interface for the CConveyorStepItEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONVEYORSTEPITEVENT_H__92CE3AB5_06BA_46CB_9FA0_AD56205F55EF__INCLUDED_)
#define AFX_CONVEYORSTEPITEVENT_H__92CE3AB5_06BA_46CB_9FA0_AD56205F55EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TERMINALEVENT.H"
class CSimpleConveyor;
class CARCportEngine;


class CConveyorStepItEvent : public TerminalEvent  
{
public:
	CConveyorStepItEvent( CSimpleConveyor* _pConveyor );
	virtual ~CConveyorStepItEvent();

private:
	CSimpleConveyor* m_pConveyor;

public:
	virtual int process( CARCportEngine *pEngine );
	virtual int kill( void ) { return 1;}
	virtual const char* getTypeName( void ) const 
	{
		return "Conveyor Step It Event";
	}
	virtual int getEventType( void ) const 
	{
		return ConveyorStepItEvent;
	}
	
};

#endif // !defined(AFX_CONVEYORSTEPITEVENT_H__92CE3AB5_06BA_46CB_9FA0_AD56205F55EF__INCLUDED_)

