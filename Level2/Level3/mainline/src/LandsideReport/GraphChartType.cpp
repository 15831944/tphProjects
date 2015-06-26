#include "StdAfx.h"
#include ".\graphcharttype.h"


//////////////////////////////////////////////////////////////////////////
//GraphChartType
LSGraphChartType::LSGraphChartType(void)
{
	m_nEnum		= -1;
	m_strTitle	= _T("INVALID");
}

LSGraphChartType::~LSGraphChartType(void)
{
}








//////////////////////////////////////////////////////////////////////////
//GraphChartTypeList
LSGraphChartTypeList::LSGraphChartTypeList()
{

}

LSGraphChartTypeList::~LSGraphChartTypeList()
{

}

void LSGraphChartTypeList::AddItem( int nEnum, const CString& strName )
{
	LSGraphChartType chart;
	chart.m_nEnum	 = nEnum;
	chart.m_strTitle = strName;
	m_vChartType.push_back(chart);
}

int LSGraphChartTypeList::GetCount() const
{
	return static_cast<int>(m_vChartType.size());
}

LSGraphChartType LSGraphChartTypeList::GetItem( int nIndex ) const
{
	if(nIndex <0 && nIndex >= GetCount())
		return LSGraphChartType();

	return m_vChartType.at(nIndex);
}

void LSGraphChartTypeList::Clean()
{
	m_vChartType.clear();
}
