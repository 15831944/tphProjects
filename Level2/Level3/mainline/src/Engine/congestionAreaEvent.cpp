#include "StdAfx.h"
#include ".\congestionareaevent.h"
#include "..\engine\TERMINAL.H"
#include "../Common/ARCTracker.h"
#include "ARCportEngine.h"
CongestionAreaEvent::CongestionAreaEvent(void)
{
}

CongestionAreaEvent::~CongestionAreaEvent(void)
{
}

int CongestionAreaEvent::process (CARCportEngine *_pEngine)
{
	PLACE_METHOD_TRACK_STRING();
	if(_pEngine)
	{
		_pEngine->getCongestionAreaManager().ProcessCongestionEvent(this, time);
		return 1;
	}

	return 0;
}