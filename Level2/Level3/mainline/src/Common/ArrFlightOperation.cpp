#include "StdAfx.h"
#include ".\arrflightoperation.h"
//#include "Common\termfile.h"
//#include "Common\strdict.h"
#include "common\IDGather.h"
#include "common\ALTObjectID.h"

CArrFlightOperation::CArrFlightOperation(void)
{
}

CArrFlightOperation::~CArrFlightOperation(void)
{
}


void CArrFlightOperation::SetArrFlightID(CFlightID arrFlightID)
{
	m_ID = arrFlightID;
}

void CArrFlightOperation::SetArrFlightID(const char *p_str)
{
	m_ID = p_str;
}


void CArrFlightOperation::SetArrStand(ALTObjectID arrStand)
{
	m_Stand = arrStand;
}

void CArrFlightOperation::SetArrTime(long tTime)
{
	m_Time = tTime;
}


CFlightID CArrFlightOperation::GetArrFlightID()
{
	return m_ID;
}

ALTObjectID CArrFlightOperation::GetArrStand()
{
	return m_Stand;
}

long CArrFlightOperation::GetArrTime()
{
	return m_Time;
}

void CArrFlightOperation::SetOriginAirport(CAirportCode origin)
{
	m_Airport = origin;
}
void CArrFlightOperation::SetOriginAirport(const char *p_str)
{
	m_Airport = p_str;
}

CAirportCode CArrFlightOperation::GetOriginAirport()
{
	return m_Airport;
}


void CArrFlightOperation::SetArrStandIdx(short nIdx)
{
	m_nStandIdx = nIdx;
}

short CArrFlightOperation::GetArrStandIdx()
{
	return m_nStandIdx;
}