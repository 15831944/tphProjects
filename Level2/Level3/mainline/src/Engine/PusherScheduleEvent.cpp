// PusherScheduleEvent.cpp: implementation of the PusherScheduleEvent class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Pusher.h"
#include "PusherScheduleEvent.h"
#include "../Common/ARCTracker.h"
#include "common\CodeTimeTest.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PusherScheduleEvent::PusherScheduleEvent( Pusher* _pPusher )
:m_pWhichPusher( _pPusher )
{

}

PusherScheduleEvent::~PusherScheduleEvent()
{

}
int PusherScheduleEvent::process ( CARCportEngine *_pEngine)
{
	PLACE_METHOD_TRACK_STRING();
	m_pWhichPusher->ReleaseAllBaggageOnPusher( this->time );
	m_pWhichPusher->NeedGenerateScheduleEvent();
	return TRUE;
}