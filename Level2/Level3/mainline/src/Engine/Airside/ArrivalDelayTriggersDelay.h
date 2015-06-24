#pragma once
#include "airsideflightdelay.h"
#include "../EngineDll.h"

class ENGINE_TRANSFER ArrivalDelayTriggersDelay :public AirsideFlightDelay
{
public:
	ArrivalDelayTriggersDelay(void);
	~ArrivalDelayTriggersDelay(void);

	DelayType GetType()const;
	bool IsIdentical(const AirsideFlightDelay* otherDelay)const;
	void WriteDelayReason(ArctermFile& outFile);
	void ReadDelayReason(ArctermFile& inFile);
	//static AirsideFlightDelay* NewFlightDelay(const DelayType& type);

	void SetReasonDescription(const CString& strReason);

	virtual CString GetStringDelayReason();

private:
	CString m_strReasonDescrip;
};
