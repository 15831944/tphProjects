#pragma once
#include "../Reports/ReportType.h"

// CMultiRunReportSplitChildFrame frame with splitter

class CMultiRunReportSplitChildFrame : public CMDIChildWnd
{
protected:
	DECLARE_DYNCREATE(CMultiRunReportSplitChildFrame)
	DECLARE_MESSAGE_MAP()

	CMultiRunReportSplitChildFrame();
	virtual ~CMultiRunReportSplitChildFrame();
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	CSplitterWnd m_wndSplitter1;
	CSplitterWnd m_wndSplitter2;
};


