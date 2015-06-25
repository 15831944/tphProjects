#pragma once
#include "airside\airsidemobileelementevent.h"


class AirsideBaggageTrainInSim;
class CARCportEngine;

class CBagCartsBeginServiceParkingSpotEvent :
	public CAirsideMobileElementEvent
{
public:
	CBagCartsBeginServiceParkingSpotEvent(AirsideBaggageTrainInSim* pBagTrain,ElapsedTime _time,CARCportEngine *_pEngine);
	~CBagCartsBeginServiceParkingSpotEvent(void);

	int Process() ;
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "BagCartBeginServiceParkingSpot";}

	virtual int getEventType (void) const {return BagCartBeginServiceParkingSpot;}

private:
	AirsideBaggageTrainInSim *m_pBaggageTrain;
	//Terminal* p_terminal ;
	CARCportEngine *m_pEngine;

protected:

	void NotifyBagTakeOffBusToLoaderProcessor() ;
	//notice baggage move to baggage carts from the pusher processor
	void NotifyBagTakeOnBaggageCarts() ;
};
