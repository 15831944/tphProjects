ConveyorStepItEvent.cpp: implementation of the CConveyorStepItEvent class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ConveyorStepItEvent.h"
#include "simpleconveyor.h"
#include "common\CodeTimeTest.h"
#include "engine\terminal.h"
#include "../Common/ARCTracker.h"
#include "Engine/ARCportEngine.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConveyorStepItEvent::CConveyorStepItEvent( CSimpleConveyor* _pConveyor ) : m_pConveyor( _pConveyor )
{

}

CConveyorStepItEvent::~CConveyorStepItEvent()
{

}

int CConveyorStepItEvent::process( CARCportEngine *pEngine )
{
	PLACE_METHOD_TRACK_STRING();
	try
	{
		m_pConveyor->processorStepItEvent( pEngine->getTerminal(), time );		
	}
	// all recoverable errors that affect 1 passenger are trapped here
    catch (ARCSimEngineException* sim_exception)
    {
		pEngine->SendSimFormatMessage( sim_exception->getFormatErrorMsg() );
        delete sim_exception;
    }
	catch(...)
	{
		throw;
	}

	
	return 1;
}