#include "StdAfx.h"
#include ".\vehicleservicedelay.h"
#include "../EngineDll.h"

#include "../../Common/termfile.h"


const int MAX_REASONLENGTH = 255;
VehicleServiceDelay::VehicleServiceDelay(void)
{
}

VehicleServiceDelay::~VehicleServiceDelay(void)
{
}

AirsideFlightDelay::DelayType VehicleServiceDelay::GetType() const
{
	return AirsideFlightDelay::enumVehicleServiceDelay;
}

bool VehicleServiceDelay::IsIdentical(const AirsideFlightDelay* otherDelay) const
{
	if (GetType() != otherDelay->GetType())
		return false;

	//if (GetAtObjectID() > otherDelay->GetAtObjectID() || GetAtObjectID() < otherDelay->GetAtObjectID())
	//	return false;

	if (m_strReasonDescrip != ((VehicleServiceDelay*)otherDelay)->GetReasonDescrip())
		return false;

	return true;
}

void VehicleServiceDelay::ReadDelayReason(ArctermFile& inFile)
{
	char strBuffer[MAX_REASONLENGTH];
	inFile.getField(strBuffer,MAX_REASONLENGTH);
	m_strReasonDescrip = strBuffer;

}

void VehicleServiceDelay::WriteDelayReason(ArctermFile& outFile)
{
	outFile.writeField(m_strReasonDescrip.GetBuffer());
}

void VehicleServiceDelay::SetReasonDescription(const CString& strReason)
{
	m_strReasonDescrip = strReason;
}

const CString& VehicleServiceDelay::GetReasonDescrip()
{
	return m_strReasonDescrip;
}

CString VehicleServiceDelay::GetStringDelayReason()
{
	return m_strReasonDescrip;
}