#pragma once

#include "airsideflightdelay.h"
#include "../EngineDll.h"
class ENGINE_TRANSFER  CAirsideFlightRunwayDelay:public AirsideFlightDelay
{
public:
	CAirsideFlightRunwayDelay(void);
	~CAirsideFlightRunwayDelay(void);

public:
	virtual DelayType GetType()const;
	virtual bool IsIdentical(const AirsideFlightDelay* otherDelay)const;

	virtual void WriteDelayReason(ArctermFile& outFile );
	virtual void ReadDelayReason(ArctermFile& inFile);

};
