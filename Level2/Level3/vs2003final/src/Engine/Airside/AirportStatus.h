#pragma once
#include "../EngineDll.h"
#include "AirsideObserver.h"

class AirsideResource;
class ClearanceItem;

class ENGINE_TRANSFER AirportStatus: public CAirsideSubject
{
public:
	AirportStatus();
	virtual ~AirportStatus();

	virtual void UpdateStatus(AirsideResource* pOriResource,const ClearanceItem& item) = 0;

};
