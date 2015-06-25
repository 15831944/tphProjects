#include "stdafx.h"
#include "CARC3DChart.h"
#include <comdef.h>

// CARC3DChart.cpp : implementation file

// CARC3DChart
CARC3DChart::CARC3DChart(void)
{
	m_p3DChart = 0;
}

CARC3DChart::~CARC3DChart(void)
{
	if(m_p3DChart)delete m_p3DChart;
}

bool CARC3DChart::Print3DChart(void)
{//print 3DChart	
	bool bReturn = false;

#ifdef N3DCHART
	if(m_p3DChart) 
		bReturn = m_p3DChart->PrintN3DChart(); 
	else
		bReturn = false;
#elif defined MSCHART
	if(m_p3DChart) 
		bReturn = m_p3DChart->PrintMSChart();
	else
		bReturn = false;
#else
	bReturn = false;
#endif

	return (bReturn);
}

bool CARC3DChart::ExportToBMPFile(void)
{//export chart to bitmap file
	bool bReturn = false;

#ifdef N3DCHART
	if(m_p3DChart) 
		bReturn = m_p3DChart->ExportToBMPFile(); 
	else
		bReturn = false;
#elif defined MSCHART
	if(m_p3DChart) 
		bReturn = m_p3DChart->ExportToBMPFile(); 
	else
		bReturn = false;
#else
	bReturn = false;
#endif

	return (bReturn);
}

bool CARC3DChart::Set3DChartType(Arc3DChartType actype3DChart/* = Arc3DChartType_2D_Bar*/)
{
	bool bReturn = false;
	
#ifdef N3DCHART
	if(m_p3DChart) 
		bReturn = m_p3DChart->ShowN3DChartType(actype3DChart); 
	else
		bReturn = false;
#elif defined MSCHART
	if(m_p3DChart) 
		bReturn = m_p3DChart->SetMSChartType(actype3DChart);
	else
		bReturn = false;
#else
	bReturn = false;
#endif

	return (bReturn);
}

void CARC3DChart::SetAxiLabel(int nInterval)
{
#ifdef MSCHART
	if(m_p3DChart) 
	{
		m_p3DChart->SetAxiLabe(nInterval);
	}
#endif
}

bool CARC3DChart::DrawChart(C2DChartData & c2dChartData)
{//draw chart
	bool bReturn = false; 
	try
	{
#ifdef N3DCHART
			if(m_p3DChart) 
			{
				if(c2dChartData.m_vr2DChartData.size() > 0)
				{
					bReturn = m_p3DChart->DrawChart(c2dChartData); 
					m_p3DChart->ShowWindow(SW_SHOW);
				}
				else
					m_p3DChart->ShowWindow(SW_HIDE);

			}
			else
				bReturn = false;
#elif defined MSCHART
			if(m_p3DChart) 
			{			
				if(c2dChartData.m_vr2DChartData.size() > 0)
				{
					bReturn = m_p3DChart->DrawChart(c2dChartData); 
					m_p3DChart->ShowWindow(SW_SHOW);
				}
				else
					m_p3DChart->ShowWindow(SW_HIDE);
			}
			else
				bReturn = false;
#else
			bReturn = false;
#endif
	}
	catch (_com_error& e)
	{
		CString strText = _T("An error occurs when drawing chart: ");
		strText += e.ErrorMessage();
		AfxMessageBox(strText);
		return false;
	}
	catch (...)
	{
		AfxMessageBox(_T("An error occurs when drawing chart!"));
		return false;
	}

	return (bReturn);
}

BOOL CARC3DChart::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	BOOL bReturn = FALSE;

#ifdef N3DCHART
	if(!m_p3DChart)m_p3DChart = new CN3DChart;
	if(m_p3DChart) 
	{
		bReturn = m_p3DChart->Create(lpszClassName,lpszWindowName,dwStyle,rect,pParentWnd,nID,pContext); 
	}
	else
		bReturn = false;
#elif defined MSCHART
	if(!m_p3DChart)m_p3DChart = new CMSChart;
	if(m_p3DChart) 
	{
		bReturn = m_p3DChart->Create(lpszClassName,lpszWindowName,dwStyle,rect,pParentWnd,nID,pContext); 
		if(bReturn)
			m_p3DChart->ShowWindow(SW_HIDE);
	}
	else
		bReturn = false;
#else
	bReturn = false;
#endif

	return (bReturn);
}

void CARC3DChart::PropertyEdit(void)
{
#ifdef N3DCHART	
#elif defined MSCHART
	if(m_p3DChart) 
		m_p3DChart->PropertyEdit();
#else	
#endif
}