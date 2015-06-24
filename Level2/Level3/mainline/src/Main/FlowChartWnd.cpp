// FlowChartWnd.cpp : implementation file
//

#include "stdafx.h"
#include "FlowChartWnd.h"

using namespace FLOWCHARTLib;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlowChartWnd

IMPLEMENT_DYNCREATE(CFlowChartWnd, CWnd)

CFlowChartWnd::CFlowChartWnd()
{
}

CFlowChartWnd::~CFlowChartWnd()
{

}


BEGIN_MESSAGE_MAP(CFlowChartWnd, CWnd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlowChartWnd diagnostics

#ifdef _DEBUG
void CFlowChartWnd::AssertValid() const
{
	CWnd::AssertValid();
}

void CFlowChartWnd::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFlowChartWnd message handlers
bool CFlowChartWnd::Create(LPCTSTR lpszWindowName, DWORD dwStyle,
						   const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (CreateControl(__uuidof(FlowChart), lpszWindowName, dwStyle, rect, pParentWnd, nID))
	{
		IFlowChart* pFlowChart;
		if (FAILED(GetControlUnknown()->QueryInterface(__uuidof(pFlowChart), (void**)&pFlowChart)))
		{
			AfxMessageBox(_T("Failed to query interface!"));
			return false;
		}
		else
		{
			m_pFlowChart.Attach(pFlowChart);
		}
	}
	else
	{
		AfxMessageBox(_T("Failed to create FlowChart!"));
		return false;
	}

	return true;
}
