#include "StdAfx.h"
#include ".\landsidebaseresult.h"

LandsideBaseResult::LandsideBaseResult(void)
{
}

LandsideBaseResult::~LandsideBaseResult(void)
{
}
LSGraphChartTypeList LandsideBaseResult::GetChartList() const
{
	ASSERT(0);
	return LSGraphChartTypeList();
}

LandsideSummaryTime::LandsideSummaryTime(void)
: m_estTotal(0l)
, m_estMin(0l)
, m_estAverage(0l)
, m_estMax(0l)
, m_estQ1(0l)
, m_estQ2(0l)
, m_estQ3(0l)
, m_estP1(0l)
, m_estP5(0l)
, m_estP10(0l)
, m_estP90(0l)
, m_estP95(0l)
, m_estP99(0l)
, m_estSigma(0l)
{
}

LandsideSummaryTime::~LandsideSummaryTime(void)
{
}