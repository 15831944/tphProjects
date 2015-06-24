// TLOSEvent.cpp: implementation of the TLOSEvent class.
//
//////////////////////////////////////////////////////////////////////
#include"stdafx.h"
#include "TLOSEvent.h"
#include "Monitor.h"
#include "common\CodeTimeTest.h"
#include "../Common/ARCTracker.h"
#include "Engine/ARCportEngine.h"
#include "terminal.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TLOSEvent::TLOSEvent()
{

}

TLOSEvent::~TLOSEvent()
{

}
int TLOSEvent::process ( CARCportEngine *_pEngine)
{
	PLACE_METHOD_TRACK_STRING();
	_pEngine->getTerminal()->m_pTLOSMonitor->DoTLOSCheck( m_pProcID , time , m_sPaxType );
	return 1;
}