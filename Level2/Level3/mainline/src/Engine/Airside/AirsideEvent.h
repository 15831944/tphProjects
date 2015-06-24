#pragma once
#include "common\elaptime.h"
#include "common\util.h"
#include "../enginedll.h"
#include "inputs\in_term.h"
#include "../event.h"


class FlightInAirsideSimulation;
class CARCportEngine;
class ENGINE_TRANSFER AirsideEvent : public Event
{
public:
	AirsideEvent(void);
	~AirsideEvent(void);

	virtual int process(CARCportEngine* _pEngine);

	virtual int kill();
	virtual int Process() = 0;	//Event process
	
protected:
	//FlightInAirsideSimulation* m_pFlight;
};
