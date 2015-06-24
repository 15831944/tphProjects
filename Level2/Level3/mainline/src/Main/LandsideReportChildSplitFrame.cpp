// LandsideReportChildSplitFrame.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "LandsideReportChildSplitFrame.h"
#include ".\landsidereportchildsplitframe.h"

#include "LandsideReportControlView.h"
#include "LandsideReportListView.h"
#include "LandsideReportGraphView.h"
// LandsideReportChildSplitFrame

#include "TermPlanDoc.h"

IMPLEMENT_DYNCREATE(LandsideReportChildSplitFrame, CMDIChildWnd)

LandsideReportChildSplitFrame::LandsideReportChildSplitFrame()
{
}

LandsideReportChildSplitFrame::~LandsideReportChildSplitFrame()
{
}

BOOL LandsideReportChildSplitFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	//return m_wndSplitter.Create(this,
	//	2, 2,       // TODO: adjust the number of rows, columns
	//	CSize(10, 10),  // TODO: adjust the minimum pane size
	//	pContext);

	CRect rect;
	GetClientRect( &rect );
	CSize size = rect.Size();
	size.cx /= 2;		// Initial column size
	size.cy /= 2;		// Initial row size

	// 1 - Create first static splitter
	if( !m_wndSplitter1.CreateStatic( this, 1, 2 ) )	// 1 row, 2 cols
	{
		TRACE0( "Failed to create first static splitter\n" );
		return FALSE;
	}

	// 2 - Create nested static splitter 
	if( !m_wndSplitter2.CreateStatic( &m_wndSplitter1, 2, 1,	// 2 rows, 1 col
		WS_CHILD | WS_VISIBLE,
		m_wndSplitter1.IdFromRowCol( 0, 0 ) ) )
	{
		TRACE0( "Failed to create nested static splitter\n" );
		return FALSE;
	}

	// 3 - Create top-left view
	size.cx = 0;
	size.cy = 0;
	if( !m_wndSplitter2.CreateView( 0, 0,			// row 0, col 0
		RUNTIME_CLASS( LandsideReportControlView ), 
		size, pContext ) )
	{
		TRACE0( "Failed to create top-right view\n" );
		return FALSE;
	}

	size = rect.Size();
	// 4 - Create bottom-left view
	if( !m_wndSplitter2.CreateView( 1, 0,			// row 1, col 0
		RUNTIME_CLASS( LandsideReportListView ), 
		size, pContext ) )
	{
		TRACE0( "Failed to create bottom-right view\n" );
		return FALSE;
	}

	// 3 - Create right column view
	if( !m_wndSplitter1.CreateView( 0, 1,				// row 0, col 1
		RUNTIME_CLASS( LandsideReportGraphView ), 
		CSize(size.cx,size.cy*2), pContext ) )
	{
		TRACE0( "Failed to create left view\n" );
		return FALSE;
	}

	GetWindowRect(&rect);
	m_wndSplitter1.SetColumnInfo(0,350,260);
	m_wndSplitter2.SetRowInfo(0,350,236);
	m_wndSplitter1.SetActivePane(0,0);

	return TRUE;
}

BEGIN_MESSAGE_MAP(LandsideReportChildSplitFrame, CMDIChildWnd)
END_MESSAGE_MAP()


// LandsideReportChildSplitFrame message handlers

void LandsideReportChildSplitFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// TODO: Add your specialized code here and/or call the base class

//	CMDIChildWnd::OnUpdateFrameTitle(bAddToTitle);

	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);

	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave child window alone!

	CDocument * pDocument = GetActiveDocument();
	if(bAddToTitle)
	{
		TCHAR szText[256+_MAX_PATH];
		if (pDocument == NULL)
			lstrcpy(szText, m_strTitle);
		else
			lstrcpy(szText, 
			pDocument->GetTitle() + " - " + ((CTermPlanDoc*)pDocument)->GetARCReportManager().GetLandsideReportTitle() );

		//if (m_nWindow > 0)
		//	wsprintf(szText + lstrlen(szText), _T(":%d"), m_nWindow);

		// set title if changed, but don't remove completely
		//AfxSetWindowText(m_hWnd, szText);
		this->SetWindowText(szText);
	}

}
