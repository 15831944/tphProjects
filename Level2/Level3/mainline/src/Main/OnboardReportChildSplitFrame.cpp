// OnboardReportChildSplitFrame.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "OnboardReportChildSplitFrame.h"


#include "OnboardReportControlView.h"
#include "OnboardReportListView.h"
#include "OnboardReportGraphView.h"

#include "TermPlanDoc.h"



// OnboardReportChildSplitFrame

IMPLEMENT_DYNCREATE(OnboardReportChildSplitFrame, CMDIChildWnd)

OnboardReportChildSplitFrame::OnboardReportChildSplitFrame()
{
}

OnboardReportChildSplitFrame::~OnboardReportChildSplitFrame()
{
}

BOOL OnboardReportChildSplitFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	////return m_wndSplitter.Create(this,
	////	2, 2,       // TODO: adjust the number of rows, columns
	////	CSize(10, 10),  // TODO: adjust the minimum pane size
	////	pContext);

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
		RUNTIME_CLASS( OnboardReportControlView ), 
		size, pContext ) )
	{
		TRACE0( "Failed to create top-right view\n" );
		return FALSE;
	}

	size = rect.Size();
	// 4 - Create bottom-left view
	if( !m_wndSplitter2.CreateView( 1, 0,			// row 1, col 0
		RUNTIME_CLASS( OnboardReportListView ), 
		size, pContext ) )
	{
		TRACE0( "Failed to create bottom-right view\n" );
		return FALSE;
	}

	// 3 - Create right column view
	if( !m_wndSplitter1.CreateView( 0, 1,				// row 0, col 1
		RUNTIME_CLASS( OnboardReportGraphView ), 
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

BEGIN_MESSAGE_MAP(OnboardReportChildSplitFrame, CMDIChildWnd)
END_MESSAGE_MAP()


// OnboardReportChildSplitFrame message handlers
void OnboardReportChildSplitFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{

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
			pDocument->GetTitle() + " - " + ((CTermPlanDoc*)pDocument)->GetARCReportManager().GetOnboardReportTitle() );

		//if (m_nWindow > 0)
		//	wsprintf(szText + lstrlen(szText), _T(":%d"), m_nWindow);

		// set title if changed, but don't remove completely
		//AfxSetWindowText(m_hWnd, szText);
		this->SetWindowText(szText);
	}


	//CMDIChildWnd::OnUpdateFrameTitle(bAddToTitle);
}

void OnboardReportChildSplitFrame::OnUpdateFrameMenu(BOOL bActive, CWnd* pActivateWnd, HMENU hMenuAlt)
{

}