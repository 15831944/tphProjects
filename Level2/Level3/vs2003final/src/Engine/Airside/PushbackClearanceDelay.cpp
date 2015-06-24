#include "StdAfx.h"
#include ".\pushbackclearancedelay.h"
#include "../EngineDll.h"

#include "../../Common/termfile.h"


const int MAX_REASONLENGTH = 255;
PushbackClearanceDelay::PushbackClearanceDelay(void)
{
}

PushbackClearanceDelay::~PushbackClearanceDelay(void)
{
}

AirsideFlightDelay::DelayType PushbackClearanceDelay::GetType() const
{
	return AirsideFlightDelay::enumPushbackDelay;
}

bool PushbackClearanceDelay::IsIdentical(const AirsideFlightDelay* otherDelay) const
{
	if (GetType() != otherDelay->GetType())
		return false;

	//if (GetAtObjectID() > otherDelay->GetAtObjectID() || GetAtObjectID() < otherDelay->GetAtObjectID())
	//	return false;

	if (m_strReasonDescrip != ((PushbackClearanceDelay*)otherDelay)->GetReasonDescrip())
		return false;

	return true;
}

void PushbackClearanceDelay::ReadDelayReason(ArctermFile& inFile)
{
	//inFile.getField(m_strReasonDescrip.GetBuffer(),MAX_REASONLENGTH);
	char strBuffer[MAX_REASONLENGTH];
	inFile.getField(strBuffer,MAX_REASONLENGTH);
	m_strReasonDescrip = strBuffer;
}

void PushbackClearanceDelay::WriteDelayReason(ArctermFile& outFile)
{
	outFile.writeField(m_strReasonDescrip.GetBuffer());
}

//AirsideFlightDelay* PushbackClearanceDelay::NewFlightDelay(const DelayType& type)
//{
//	AirsideFlightDelay* pDelay = NULL;
//
//	if (type == AirsideFlightDelay::enumPushbackDelay)
//	{
//		pDelay = new PushbackClearanceDelay;
//	}
//
//	return pDelay;
//}

void PushbackClearanceDelay::SetReasonDescription(const CString& strReason)
{
	m_strReasonDescrip = strReason;
}

const CString& PushbackClearanceDelay::GetReasonDescrip()
{
	return m_strReasonDescrip;
}

CString PushbackClearanceDelay::GetStringDelayReason()
{
	return m_strReasonDescrip;
}
