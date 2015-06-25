#pragma once
class CAirsideEnrouteQueueCapacity;
class AirsideFlightInSim;
class AirportResourceManager;
class ElapsedTime;

class EnrouteQueueCapacityInSim
{
public:
	EnrouteQueueCapacityInSim(void);
	~EnrouteQueueCapacityInSim(void);

	void Init( AirportResourceManager * pAirportRes);

	bool PushBackExitEnrouteQCapacity(const ElapsedTime& eTime, AirsideFlightInSim* pFlight);

protected:
	AirportResourceManager * m_pAirportres;
	CAirsideEnrouteQueueCapacity* m_pEnrouteQueueCapacity;
};
