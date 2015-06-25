// ReportFrameWnd.cpp : implementation file
//

#include "stdafx.h"
#include "CmpReportFrameWnd.h"

using namespace ReportFrameLib;

CCmpReportFrameWnd::CCmpReportFrameWnd()
{
}

CCmpReportFrameWnd::~CCmpReportFrameWnd()
{
}

BEGIN_MESSAGE_MAP(CCmpReportFrameWnd, CWnd)
END_MESSAGE_MAP()

// CReportFrameWnd message handlers
bool CCmpReportFrameWnd::Create(LPCTSTR lpszWindowName, DWORD dwStyle,
						   const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (CreateControl(__uuidof(ReportFrameCtrl), lpszWindowName, dwStyle, rect, pParentWnd, nID))
	{
		IReportFrameCtrl* pReportFrame;
		if (FAILED(GetControlUnknown()->QueryInterface(__uuidof(IReportFrameCtrl), (void**)&pReportFrame)))
		{
			AfxMessageBox(_T("Failed to query interface!"));
			return false;
		}
		else
		{
			m_ReportFramePtr.Attach(pReportFrame);
		}
	}
	else
	{
		AfxMessageBox(_T("Failed to create ReportFrame window!"));
		return false;
	}

	return true;
}
