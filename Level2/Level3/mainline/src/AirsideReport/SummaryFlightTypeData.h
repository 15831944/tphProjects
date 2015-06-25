#pragma once
#include "SummaryData.h"
#include "../common/FLT_CNST.H"

class CSummaryFlightTypeData
{
public:
	CSummaryFlightTypeData(void);
	~CSummaryFlightTypeData(void);

public:
	FlightConstraint      m_fltConstraint;
	CSummaryData          m_summaryData;
};
