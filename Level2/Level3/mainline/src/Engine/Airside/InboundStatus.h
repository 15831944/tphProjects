#pragma once
#include "airportstatus.h"

class AirportStatusManager;
class ClearanceItem;
class AirsideResource;

class ENGINE_TRANSFER InboundStatus :public AirportStatus
{
public:
	InboundStatus(AirportStatusManager* pStatusManager);
	~InboundStatus(void);

	void UpdateStatus(AirsideResource* pOriResource,const ClearanceItem& item);

private:
	AirportStatusManager* m_pStatusManager;
};
