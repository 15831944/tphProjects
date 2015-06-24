// FetchPersonInQueueEvent.cpp: implementation of the FetchPersonInQueueEvent class.
//
//////////////////////////////////////////////////////////////////////
#include"stdafx.h"
#include "FetchPersonInQueueEvent.h"
#include "engine\process.h"
#include "common\CodeTimeTest.h"
#include "../Common/ARCTracker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FetchPersonInQueueEvent::FetchPersonInQueueEvent(Processor* _pProc )
:m_pProc( _pProc )
{

}

FetchPersonInQueueEvent::~FetchPersonInQueueEvent()
{

}
int FetchPersonInQueueEvent::process ( InputTerminal* _pInTerm )
{
	PLACE_METHOD_TRACK_STRING();
	if( m_pProc->isAvailable() )
	{
	//	m_pProc->FetchPersonFromQueue( m_pProc, time );
	}
	return TRUE;
}