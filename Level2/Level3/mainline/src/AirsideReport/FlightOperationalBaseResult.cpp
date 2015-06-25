#include "StdAfx.h"
#include ".\flightoperationalbaseresult.h"

CFlightOperationalBaseResult::CFlightOperationalBaseResult(void)
{
}

CFlightOperationalBaseResult::~CFlightOperationalBaseResult(void)
{
}

void CFlightOperationalBaseResult::ClearAllData()
{
	for (int i=0; i<(int)m_pResultData.size(); i++)
	{
		delete m_pResultData[i];
	}

	m_pResultData.clear();
}

