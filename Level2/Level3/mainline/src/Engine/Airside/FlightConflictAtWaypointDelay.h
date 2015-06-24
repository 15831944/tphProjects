#pragma once
#include "airsideflightdelay.h"
#include "../EngineDll.h"

class ENGINE_TRANSFER FlightConflictAtWaypointDelay :public AirsideFlightDelay
{
public:
	FlightConflictAtWaypointDelay(void);
	~FlightConflictAtWaypointDelay(void);

	DelayType GetType()const;
	bool IsIdentical(const AirsideFlightDelay* otherDelay)const;
	void WriteDelayReason(ArctermFile& outFile );
	void ReadDelayReason(ArctermFile& inFile);
	//static AirsideFlightDelay* NewFlightDelay(const DelayType& type);
	virtual CString GetStringDelayReason();
private:
	CString m_strDelayReason;

};
