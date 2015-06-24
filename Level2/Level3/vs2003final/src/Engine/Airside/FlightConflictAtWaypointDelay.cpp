#include "StdAfx.h"
#include ".\flightconflictatwaypointdelay.h"


#include "../../Common/termfile.h"


const int MAX_REASONLENGTH = 255;
FlightConflictAtWaypointDelay::FlightConflictAtWaypointDelay(void)
:m_strDelayReason(_T("Waypoint or Runway be occupied"))
{
}

FlightConflictAtWaypointDelay::~FlightConflictAtWaypointDelay(void)
{
}

AirsideFlightDelay::DelayType FlightConflictAtWaypointDelay::GetType() const
{
	return AirsideFlightDelay::enumFlightConflictAtWayPoint;
}

bool FlightConflictAtWaypointDelay::IsIdentical(const AirsideFlightDelay* otherDelay) const
{
	if (GetType() != otherDelay->GetType())
		return false;

	/*if (GetAtObjectID() > otherDelay->GetAtObjectID() || GetAtObjectID() < otherDelay->GetAtObjectID())
		return false;*/

	return true;
}

void FlightConflictAtWaypointDelay::ReadDelayReason(ArctermFile& inFile)
{
	//inFile.getField(m_strDelayReason.GetBuffer(),MAX_REASONLENGTH);
	char strBuffer[MAX_REASONLENGTH];
	inFile.getField(strBuffer,MAX_REASONLENGTH);
	m_strDelayReason = strBuffer;
}

void FlightConflictAtWaypointDelay::WriteDelayReason(ArctermFile& outFile)
{
	outFile.writeField(m_strDelayReason.GetBuffer());
}

//AirsideFlightDelay* FlightConflictAtWaypointDelay::NewFlightDelay(const DelayType& type)
//{
//	AirsideFlightDelay* pDelay = NULL;
//	if (type == AirsideFlightDelay::enumFlightConflictAtWayPoint)
//	{
//		pDelay = new FlightConflictAtWaypointDelay;
//	}
//	return pDelay;
//}

CString FlightConflictAtWaypointDelay::GetStringDelayReason()
{

	return m_strDelayReason;
}