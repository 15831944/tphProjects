#include "StdAfx.h"
#include ".\flightconflictsummarydata.h"
#include "SummaryParaFilter.h"

FlightConflictSummaryData::FlightConflictSummaryData(void)
{
	m_pParaFilter = NULL;
}

FlightConflictSummaryData::~FlightConflictSummaryData(void)
{
	if (m_pParaFilter)
	{
		delete m_pParaFilter;
		m_pParaFilter = NULL;
	}
}
