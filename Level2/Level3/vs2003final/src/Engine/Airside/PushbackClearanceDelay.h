#pragma once
#include "airsideflightdelay.h"
#include "../EngineDll.h"

class ENGINE_TRANSFER PushbackClearanceDelay :public AirsideFlightDelay
{
public:
	PushbackClearanceDelay(void);
	~PushbackClearanceDelay(void);

	DelayType GetType()const;
	bool IsIdentical(const AirsideFlightDelay* otherDelay)const;
	void WriteDelayReason(ArctermFile& outFile);
	void ReadDelayReason(ArctermFile& inFile);
	//static AirsideFlightDelay* NewFlightDelay(const DelayType& type);

	void SetReasonDescription(const CString& strReason);

	const CString& GetReasonDescrip(); 

	CString GetStringDelayReason();

private:
	CString m_strReasonDescrip;
};
