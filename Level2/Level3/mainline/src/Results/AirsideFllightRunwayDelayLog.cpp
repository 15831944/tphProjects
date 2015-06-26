#include "StdAfx.h"
#include ".\airsidefllightrunwaydelaylog.h"
#include "../Common/Types.hpp"

DynamicClassImpl(AirsideFlightRunwayDelayLog)
AirsideFlightRunwayDelayLog::AirsideFlightRunwayDelayLog(const ResourceDesc& _runway, long lTime, AirsideMobileElementMode mode, long lAvailableTime, long lDelayTime, FlightRunwayDelay::PositionType _position)
:ARCEventLog()
,mAtRunway(_runway)
,mMode(mode)
,mAvailableTime(lAvailableTime)
,mDelayTime(lDelayTime)
,mPosition(_position)
,sReasonDetail(_T(""))
{
	time = lTime;
}

AirsideFlightRunwayDelayLog::AirsideFlightRunwayDelayLog()
{

}

AirsideFlightRunwayDelayLog::~AirsideFlightRunwayDelayLog(void)
{

}

bool AirsideFlightRunwayDelayLog::bTheSameDelay(const ResourceDesc& _resource,const FlightRunwayDelay::PositionType& _position) const
{
	if(mAtRunway.resid == _resource.resid && mAtRunway.resType == _resource.resType && mPosition == _position )
		return true;

	return false;
}