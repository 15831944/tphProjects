#include "StdAfx.h"
#include ".\flightoperation.h"

CFlightOperation::CFlightOperation(void)
{
	m_Time = 0l;
    m_nStandIdx = 0;
}

CFlightOperation::~CFlightOperation(void)
{
}

CFlightOperation& CFlightOperation::operator = (const CFlightOperation& flight)
{
	m_ID = flight.m_ID;
	m_Airport = flight.m_Airport;
	m_Stand = flight.m_Stand;
	m_Time = flight.m_Time;
	m_nStandIdx = flight.m_nStandIdx;
	
	return *this;
}

bool CFlightOperation::operator == (const CFlightOperation& flight) const 
{ 
	return this == &flight; 
}

bool CFlightOperation::operator < (const CFlightOperation& flight) const
{
	return m_Time < flight.m_Time;
}

void CFlightOperation::ClearData()
{
	m_ID = 0;
	m_Airport = "";
	ALTObjectID emptyID;
	m_Stand = emptyID;
	m_Time = -1L;
	m_nStandIdx = -1;
}