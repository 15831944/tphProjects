// ElevatorMoveEvent.cpp: implementation of the ElevatorMoveEvent class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ElevatorMoveEvent.h"
#include "ElevatorProc.h"
#include "common\CodeTimeTest.h"
#include "../Common/ARCTracker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ElevatorMoveEvent::ElevatorMoveEvent( ElevatorProc* _pElevator, int _iLift )
: m_pElevator( _pElevator ), m_iLift ( _iLift )
{
	
}

ElevatorMoveEvent::~ElevatorMoveEvent()
{

}
int ElevatorMoveEvent::process ( CARCportEngine *_pEngine )
{
	PLACE_METHOD_TRACK_STRING();
	m_pElevator->ElevatorMoveEventComes( m_iLift, time );
	return TRUE;
}