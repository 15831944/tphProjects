#include "StdAfx.h"
#include ".\paxbusservicerequest.h"

CPaxBusServiceRequest::CPaxBusServiceRequest(void)
{
	m_bArrival = false;
	m_nPaxCount = 0;
	m_nLeftPaxCount = 0;
}

CPaxBusServiceRequest::~CPaxBusServiceRequest(void)
{
}

bool CPaxBusServiceRequest::IsCompleteService()
{
	if (m_nLeftPaxCount >0 )
		return false;


	return VehicleServiceRequest::IsCompleteService();
}

void CPaxBusServiceRequest::SetArrival(bool bArrival)
{
	m_bArrival = bArrival;
}
bool CPaxBusServiceRequest::IsArrival()
{
	return m_bArrival;
}

void CPaxBusServiceRequest::SetPaxCount(int nCount)
{
	m_nPaxCount = nCount;
	m_nLeftPaxCount = nCount;
}

int CPaxBusServiceRequest::GetLeftPaxCount()
{
	return m_nLeftPaxCount;
}

void CPaxBusServiceRequest::ServicedPaxCount(int nServicePaxCount)
{
	m_nLeftPaxCount =m_nLeftPaxCount - nServicePaxCount;
}

int CPaxBusServiceRequest::GetAllPaxCount()
{
	return m_nPaxCount;
}

