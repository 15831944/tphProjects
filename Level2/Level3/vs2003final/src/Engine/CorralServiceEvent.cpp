#include "StdAfx.h"
#include ".\Corralserviceevent.h"
#include "hold.h"

CorralServiceEvent::CorralServiceEvent( HoldingArea *pHold, ElapsedTime eTime )
{
	m_pHold = pHold;
	setTime(eTime);
	m_bAvailable = true;

	TRACE("\r\n Creat Event \r\n");
}

CorralServiceEvent::~CorralServiceEvent(void)
{
}

int CorralServiceEvent::process( CARCportEngine* pEngine )
{
	TRACE("Process Event ?");
	if(m_bAvailable)
	{
		TRACE("YES \r\n");
		m_pHold->BeginCorralService(getTime());
	}
	else
	{
		TRACE("NO \r\n");
	}

	m_pHold->ClearCorralEvent(this);

	return 1;
}

int CorralServiceEvent::kill( void )
{
	return 1;
}

const char * CorralServiceEvent::getTypeName( void ) const
{
	return _T("Corral Service");
}

int CorralServiceEvent::getEventType( void ) const
{
	return CorralService;
}

void CorralServiceEvent::SetAvailable( bool bAvailble )
{
	m_bAvailable = bAvailble;
}
