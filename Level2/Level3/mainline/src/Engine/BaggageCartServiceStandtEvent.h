#pragma once
#include "Airside\AirsideMobileElementEvent.h"

class AirsideBaggageTrainInSim;

class CBaggageCartServiceStandtEvent :
	public CAirsideMobileElementEvent
{
public:
	CBaggageCartServiceStandtEvent(AirsideBaggageTrainInSim *pBaggageTrain, ElapsedTime time);
	~CBaggageCartServiceStandtEvent(void);

public:
	int Process() ;
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "BagCartServiceStandEvent";}

	virtual int getEventType (void) const {return BagCartServiceStandEvent;}

private:
	AirsideBaggageTrainInSim *m_pBaggageTrain;
	//Terminal* p_terminal ;
	CARCportEngine *m_pEngine;

protected:

};
