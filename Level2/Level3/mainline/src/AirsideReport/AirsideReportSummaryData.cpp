#include "StdAfx.h"
#include ".\airsidereportsummarydata.h"

CAirsideReportSummaryData::CAirsideReportSummaryData(void)
{
	m_dTotal = 0.0;
	m_dMin = 0.0;
	m_dAverage = 0.0;
	m_dMax = 0.0;
	m_dQ1 = 0.0;
	m_dQ2 = 0.0;
	m_dQ3 = 0.0;
	m_dP1 = 0.0;
	m_dP5 = 0.0;
	m_dP10 = 0.0;
	m_dP90 = 0.0;
	m_dP95 = 0.0;
	m_dP99 = 0.0;
	m_dSigma = 0.0;
	m_dCount = 0.0;
}

CAirsideReportSummaryData::~CAirsideReportSummaryData(void)
{
}
