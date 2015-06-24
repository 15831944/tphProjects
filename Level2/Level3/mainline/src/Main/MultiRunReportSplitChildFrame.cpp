// MutiRunReportSplitChildFrame.cpp : implementation file
//
#include "stdafx.h"
#include "MultiRunReportSplitChildFrame.h"
#include "MultiRunRepControlView.h"
#include "MultiRunRepListView.h"
#include "MultiRunRepGraphView.h"

IMPLEMENT_DYNCREATE(CMultiRunReportSplitChildFrame, CMDIChildWnd)

CMultiRunReportSplitChildFrame::CMultiRunReportSplitChildFrame()
{
}

CMultiRunReportSplitChildFrame::~CMultiRunReportSplitChildFrame()
{
}

BOOL CMultiRunReportSplitChildFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	CRect rect;
	GetClientRect(&rect);
	CSize size = rect.Size();
	size.cx /= 2;		// Initial column size
	size.cy /= 2;		// Initial row size

	// 1 - Create first static splitter
	if (!m_wndSplitter1.CreateStatic(this, 1, 2))	// 1 row, 2 cols
	{
		TRACE0("Failed to create first static splitter\n");
		return FALSE;
	}

	// 2 - Create nested static splitter 
	if (!m_wndSplitter2.CreateStatic(&m_wndSplitter1, 2, 1,	WS_CHILD|WS_VISIBLE, m_wndSplitter1.IdFromRowCol(0, 0)))
	{
		TRACE0("Failed to create nested static splitter\n");
		return FALSE;
	}

	// 3 - Create top-left view
	size.cx = 0;
	size.cy = 0;
	if (!m_wndSplitter2.CreateView(0, 0, RUNTIME_CLASS(CMultiRunRepControlView), size, pContext))
	{
		TRACE0("Failed to create top-right view\n");
		return FALSE;
	}

	size = rect.Size();
	// 4 - Create bottom-left view
	if (!m_wndSplitter2.CreateView(1, 0, RUNTIME_CLASS(CMultiRunRepListView), size, pContext))
	{
		TRACE0("Failed to create bottom-right view\n");
		return FALSE;
	}

	// 3 - Create right column view
	if (!m_wndSplitter1.CreateView(0, 1, RUNTIME_CLASS(CMultiRunRepGraphView), CSize(size.cx, size.cy * 2), pContext))
	{
		TRACE0("Failed to create left view\n");
		return FALSE;
	}

	GetWindowRect(&rect);
	m_wndSplitter1.SetColumnInfo(0, 350, 260);
	m_wndSplitter2.SetRowInfo(0, 300, 300);
	m_wndSplitter1.SetActivePane(0, 0);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CMultiRunReportSplitChildFrame, CMDIChildWnd)
END_MESSAGE_MAP()


// CMultiRunReportSplitChildFrame message handlers
