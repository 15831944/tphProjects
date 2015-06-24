#include "StdAfx.h"
#include ".\arrivaldelaytriggersdelay.h"
#include "../EngineDll.h"

#include "../../Common/termfile.h"


const int MAX_REASONLENGTH = 255;
ArrivalDelayTriggersDelay::ArrivalDelayTriggersDelay(void)
{
}

ArrivalDelayTriggersDelay::~ArrivalDelayTriggersDelay(void)
{
}

AirsideFlightDelay::DelayType ArrivalDelayTriggersDelay::GetType() const
{
	return AirsideFlightDelay::enumArrivalDelayTriggers;
}

bool ArrivalDelayTriggersDelay::IsIdentical(const AirsideFlightDelay* otherDelay) const
{
	if (GetType() != otherDelay->GetType())
		return false;

	/*if (GetAtObjectID() > otherDelay->GetAtObjectID() || GetAtObjectID() < otherDelay->GetAtObjectID())
		return false;*/

	return true;
}

void ArrivalDelayTriggersDelay::ReadDelayReason(ArctermFile& inFile)
{
	//inFile.getField(m_strReasonDescrip.GetBuffer(),MAX_REASONLENGTH);
	char strBuffer[MAX_REASONLENGTH];
	inFile.getField(strBuffer,MAX_REASONLENGTH);
	m_strReasonDescrip = strBuffer;
}

void ArrivalDelayTriggersDelay::WriteDelayReason(ArctermFile& outFile)
{
	outFile.writeField(m_strReasonDescrip.GetBuffer());
}

//AirsideFlightDelay* ArrivalDelayTriggersDelay::NewFlightDelay(const DelayType& type)
//{
//	AirsideFlightDelay* pDelay = NULL;
//
//	if (type == AirsideFlightDelay::enumArrivalDelayTriggers)
//	{
//		pDelay = new ArrivalDelayTriggersDelay;
//	}
//
//	return pDelay;
//}

void ArrivalDelayTriggersDelay::SetReasonDescription(const CString& strReason)
{
	m_strReasonDescrip = strReason;
}

CString ArrivalDelayTriggersDelay::GetStringDelayReason()
{
	return m_strReasonDescrip;
}