#pragma once

#include "Movevent.h"

class CARCportEngine;
class CongestionAreaEvent : public MobileElementMovementEvent
{
public:
	CongestionAreaEvent(void);
	~CongestionAreaEvent(void);
public:
	virtual int process (CARCportEngine *_pEngine );
};
