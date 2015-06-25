#include "stdafx.h"
#include "ChartDataDefine.h"

C2DChartData::C2DChartData(void)
:m_strChartTitle(_T(""))
,m_strFooter(_T(""))
,m_strXtitle(_T(""))
,m_strYtitle(_T(""))
{	
	m_vr2DChartData.clear();
	m_vrLegend.clear();
	m_vrXTickTitle.clear();
}

C2DChartData::C2DChartData(const C2DChartData & srC2DChartData)
{
	m_strChartTitle = srC2DChartData.m_strChartTitle;
	m_strFooter     = srC2DChartData.m_strFooter;
	m_strXtitle     = srC2DChartData.m_strXtitle;
	m_strYtitle     = srC2DChartData.m_strYtitle;
	m_vr2DChartData = srC2DChartData.m_vr2DChartData;
	m_vrLegend      = srC2DChartData.m_vrLegend;
	m_vrXTickTitle  = srC2DChartData.m_vrXTickTitle;
}

C2DChartData & C2DChartData::operator = (const C2DChartData & srC2DChartData)
{
	m_strChartTitle = srC2DChartData.m_strChartTitle;
	m_strFooter     = srC2DChartData.m_strFooter;
	m_strXtitle     = srC2DChartData.m_strXtitle;
	m_strYtitle     = srC2DChartData.m_strYtitle;
	m_vr2DChartData = srC2DChartData.m_vr2DChartData;
	m_vrLegend      = srC2DChartData.m_vrLegend;
	m_vrXTickTitle  = srC2DChartData.m_vrXTickTitle;
	return (*this);
}

C2DChartData::~C2DChartData(void)
{
	m_vr2DChartData.clear();
	m_vrLegend.clear();
	m_vrXTickTitle.clear();
}