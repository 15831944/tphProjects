#include "StdAfx.h"
#include ".\depflightoperation.h"
//#include "Common\termfile.h"
//#include "Common\strdict.h"
#include "common\IDGather.h"
#include "common\ALTObjectID.h"

CDepFlightOperation::CDepFlightOperation(void)
{
}

CDepFlightOperation::~CDepFlightOperation(void)
{
}


void CDepFlightOperation::SetDepFlightID(CFlightID depFlightID)
{
	m_ID = depFlightID;
}

void CDepFlightOperation::SetDepFlightID(const char *p_str)
{
	m_ID = p_str;
}


void CDepFlightOperation::SetDepStand(ALTObjectID depStand)
{
	m_Stand = depStand;
}

void CDepFlightOperation::SetDepTime(long tTime)
{
	m_Time = tTime;
}


const CFlightID CDepFlightOperation::GetDepFlightID()
{
	return m_ID;
}


const ALTObjectID CDepFlightOperation::GetDepStand()
{
	return m_Stand;
}

const long CDepFlightOperation::GetDepTime()
{
	return m_Time;
}

void CDepFlightOperation::SetDestinationAirport(CAirportCode destination)
{
	m_Airport = destination;
}

void CDepFlightOperation::SetDestinationAirport(const char *p_str)
{
	m_Airport = p_str;
}

const CAirportCode CDepFlightOperation::GetDestinationAirport()
{
	return m_Airport;
}


void CDepFlightOperation::SetDepStandIdx(short nIdx)
{
	m_nStandIdx = nIdx;
}


const short CDepFlightOperation::GetDepStandIdx()
{
	return m_nStandIdx;
}
