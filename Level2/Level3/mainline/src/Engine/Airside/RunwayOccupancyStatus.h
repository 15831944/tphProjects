#pragma once
#include "airportstatus.h"
#include "AirsideObserver.h"

class AirportStatusManager;
class ClearanceItem;
class AirsideResource;


class ENGINE_TRANSFER RunwayOccupancyStatus :public AirportStatus
{
public:
	RunwayOccupancyStatus(AirportStatusManager* pStatusManager);
	~RunwayOccupancyStatus(void);

	void UpdateStatus(AirsideResource* pOriResource,const ClearanceItem& item);

private:
	AirportStatusManager* m_pStatusManager;
};
