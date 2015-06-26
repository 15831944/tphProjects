#include "stdafx.h"
#include "Common\TERM_BIN.H"
#include "CheckEvent.h"
#include "Common\UTIL.H"
#include "Common\STATES.H"




void CheckEvent::check(int mobid, const MobEventStruct& track)
{
	if(track.state > ArriveAtServer)
		return;
	
	if(track.m_IsRealZ)
	{
		ASSERT(track.m_RealZ > 100);
		ASSERT(track.z > 100);
	}
	else
	{
	
		int nCount = 2;
		//ASSERT(track.z/SCALE_FACTOR < nCount);
	}
}

