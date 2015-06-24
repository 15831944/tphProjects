#pragma once
#include "airportstatus.h"
#include "AirsideObserver.h"

class AirportStatusManager;
class ClearanceItem;
class AirsideResource;


class ENGINE_TRANSFER TakeoffQueueStatus :public AirportStatus
{
public:
	TakeoffQueueStatus(AirportStatusManager* pStatusManager);
	~TakeoffQueueStatus(void);

	void UpdateStatus(AirsideResource* pOriResource,const ClearanceItem& item);

private:
	AirportStatusManager* m_pStatusManager;
};
