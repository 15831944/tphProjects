#include "StdAfx.h"
#include ".\airsideflightrunwaydelay.h"

CAirsideFlightRunwayDelay::CAirsideFlightRunwayDelay(void)
{
}

CAirsideFlightRunwayDelay::~CAirsideFlightRunwayDelay(void)
{
}

AirsideFlightDelay::DelayType CAirsideFlightRunwayDelay::GetType() const
{
	return enumDelayType_Runway;
}

bool CAirsideFlightRunwayDelay::IsIdentical( const AirsideFlightDelay* otherDelay ) const
{
	return false;
}

void CAirsideFlightRunwayDelay::WriteDelayReason( ArctermFile& outFile )
{

}

void CAirsideFlightRunwayDelay::ReadDelayReason( ArctermFile& inFile )
{

}