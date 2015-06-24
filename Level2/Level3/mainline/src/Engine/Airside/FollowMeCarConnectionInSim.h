#pragma once

class CAirportDatabase;
class AirsideFlightInSim;
class StandInSim;
class CFollowMeConnectionStand;
class CFollowMeConnectionData;
class FollowMeCarConnectionInSim
{
public:
	FollowMeCarConnectionInSim(CAirportDatabase* pAirportDB);
	~FollowMeCarConnectionInSim(void);

	CFollowMeConnectionStand* GetAvailableFollowMeCarConnectionData(AirsideFlightInSim* pFlight,StandInSim* pDest);
private:
	CFollowMeConnectionData* m_pFollowMeCarConnection;
};
