#pragma once
#include "../../Common/elaptime.h"



class AirportResourceManager;
class AirsideFlightInSim;
class CAirsideArrivalDelayTrigger;
class CAirportDatabase;

class ArrivalDelayTriggerInSim
{
public:
	ArrivalDelayTriggerInSim();


	void Init(int nPrjId, AirportResourceManager * pAirportRes,CAirportDatabase* pAirportDB);
	~ArrivalDelayTriggerInSim(void);

	ElapsedTime GetDelayTime(AirsideFlightInSim* pFlight,CString& strReason);

protected:
	AirportResourceManager * m_pAirportres;
	CAirsideArrivalDelayTrigger* m_pdelayTriggerInput;
};
