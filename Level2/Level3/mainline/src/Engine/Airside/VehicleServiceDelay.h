#pragma once
#include "airsideflightdelay.h"
#include "../EngineDll.h"

class ENGINE_TRANSFER VehicleServiceDelay :public AirsideFlightDelay
{
public:
	VehicleServiceDelay(void);
	~VehicleServiceDelay(void);

	DelayType GetType()const;
	bool IsIdentical(const AirsideFlightDelay* otherDelay)const;
	void WriteDelayReason(ArctermFile& outFile);
	void ReadDelayReason(ArctermFile& inFile);

	void SetReasonDescription(const CString& strReason);

	const CString& GetReasonDescrip(); 

	CString GetStringDelayReason();

private:
	CString m_strReasonDescrip;
};
