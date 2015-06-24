#pragma once
#include "airportstatus.h"

class AirportStatusManager;
class ClearanceItem;
class AirsideResource;

class ENGINE_TRANSFER OutboundStatus :public AirportStatus
{
public:
	OutboundStatus(AirportStatusManager* pStatusManager);
	~OutboundStatus(void);
	void UpdateStatus(AirsideResource* pOriResource,const ClearanceItem& item);

private:
	AirportStatusManager* m_pStatusManager;
};
