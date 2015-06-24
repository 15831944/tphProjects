// 3DChartWnd.cpp : implementation file
//

#include "stdafx.h"
#include <afxocc.h>
#include "Cmp3DChartWnd.h"

using namespace N3DCHARTLib;

// C3DChartWnd

IMPLEMENT_DYNAMIC(CCmp3DChartWnd, CWnd)
CCmp3DChartWnd::CCmp3DChartWnd()
{
}

CCmp3DChartWnd::~CCmp3DChartWnd()
{
}


BEGIN_MESSAGE_MAP(CCmp3DChartWnd, CWnd)
END_MESSAGE_MAP()



// C3DChartWnd message handlers
bool CCmp3DChartWnd::Create(LPCTSTR lpszWindowName, DWORD dwStyle,
					   const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (CreateControl(__uuidof(NChart), lpszWindowName, dwStyle, rect, pParentWnd, nID))
	{
		IDualChart *pDualChart;
		if (FAILED(GetControlUnknown()->QueryInterface(__uuidof(IDualChart), (void**)&pDualChart)))
		{
			AfxMessageBox(_T("Query interace failed"));
			return false;
		}
		else
		{
			m_pChart.Attach(pDualChart);
		}
	}
	else
	{
		AfxMessageBox(_T("Can not create 3DChart!"));
		return false;
	}

	ASSERT(m_pCtrlSite != NULL);
	m_h3DWnd = m_pCtrlSite->m_hWnd;

	return true;
}
