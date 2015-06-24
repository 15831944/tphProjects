// PusherCheckMaxWaitTimeEvent.cpp: implementation of the PusherCheckMaxWaitTimeEvent class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PusherCheckMaxWaitTimeEvent.h"
#include "Pusher.h"
#include "common\CodeTimeTest.h"
#include "../Common/ARCTracker.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PusherCheckMaxWaitTimeEvent::PusherCheckMaxWaitTimeEvent( Pusher* _pPusher, long _lBaggageToBeChecked )
:m_pWhichPusher( _pPusher ),m_lBaggageToBeChecked( _lBaggageToBeChecked )
{

}

PusherCheckMaxWaitTimeEvent::~PusherCheckMaxWaitTimeEvent()
{

}
int PusherCheckMaxWaitTimeEvent::process ( CARCportEngine *_pEngine )
{
	PLACE_METHOD_TRACK_STRING();
	m_pWhichPusher->ReleaseBaggageIfStillOnPusher( m_lBaggageToBeChecked , time );
	return TRUE;
}