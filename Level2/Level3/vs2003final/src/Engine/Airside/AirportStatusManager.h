#pragma once
#include "set"
#include "../EngineDll.h"

class AirportStatus;
class AirsideResource;
class ClearanceItem;

class ENGINE_TRANSFER AirportStatusManager
{
public:
	AirportStatusManager(void);
	~AirportStatusManager(void);

	void RegisterAirportStatus(AirportStatus* pStatus);
	void RemoveAirportStatus(AirportStatus* pStatus);
	void NotifyAirportStatuses(AirsideResource* pOriResource,const ClearanceItem& item);

private:
	std::set<AirportStatus*> m_vStatuslist;
};
