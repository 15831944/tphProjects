#pragma once
#include "event.h"

class MobileElementExitSystemEvent :
	public Event
{
public:
	MobileElementExitSystemEvent(MobileElement *pMobElement, ElapsedTime eTime);
	~MobileElementExitSystemEvent(void);

	// Main event processing routine for each Event type.
	//Modified by Xie Bo 2002.3.22
	virtual int process (CARCportEngine* pEngine );

	//For shut down event
	virtual int kill (void);

	//It returns event's name
	virtual const char *getTypeName (void) const;

	//It returns event type
	virtual int getEventType (void) const;

protected:
	MobileElement	*m_pMobileElement;
	int				m_nMobID;
};

