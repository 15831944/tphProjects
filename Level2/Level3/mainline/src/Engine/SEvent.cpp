#include "stdafx.h"
#include "SEvent.H"


void SEvent::addEvent()
{
	if(bInList)
	{
		removeFromEventList();
	}
	Event::addEvent();
	bInList = true;
}

ElapsedTime SEvent::curTime()
{
	if(m_pEventList)
		return m_pEventList->GetCurTime(); 
	return ElapsedTime(0L);
}

int SEvent::process( CARCportEngine* pEngine )
{
	bInList = false;
	doProcess(pEngine);
	return FALSE;
}

SEvent::~SEvent()
{
	if(bInList)
		removeFromEventList();
}

SEvent::SEvent()
{
	bInList = false;
}
