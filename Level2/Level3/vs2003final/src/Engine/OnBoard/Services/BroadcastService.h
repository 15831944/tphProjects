#pragma once
/**
* Class to provide crew broadcast.Both Enplane and deplane process.
*/

#include "Engine/ServiceObject.h"

class CBroadcastService : public ServiceObject < CBroadcastService >
{
public:
	CBroadcastService();
	~CBroadcastService();

	void Initialize(){};

};

CBroadcastService* broadcastService();
AFX_INLINE CBroadcastService* broadcastService()
{
	return CBroadcastService::GetInstance();

}
