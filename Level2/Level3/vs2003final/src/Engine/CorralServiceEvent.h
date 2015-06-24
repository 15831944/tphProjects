#pragma once
#include "terminalevent.h"

class CorralServiceEvent :public TerminalEvent
{
public:
	CorralServiceEvent(HoldingArea *pHold, ElapsedTime eTime);
	~CorralServiceEvent(void);


	virtual int process (CARCportEngine* pEngine );

	//For shut down event
	virtual int kill (void);

	//It returns event's name
	virtual const char *getTypeName (void) const;

	//It returns event type
	virtual int getEventType (void) const;
	


	void SetAvailable(bool bAvailble);


protected:
	HoldingArea *m_pHold;

	bool		m_bAvailable;




};
