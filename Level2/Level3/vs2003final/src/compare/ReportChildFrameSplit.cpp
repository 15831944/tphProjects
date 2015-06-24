// ReportChildFrameSplit.cpp : implementation file
//

#include "stdafx.h"
#include "ReportChildFrameSplit.h"
#include "common\dataset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__; 
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportChildFrameSplit

IMPLEMENT_DYNCREATE(CReportChildFrameSplit, CMDIChildWnd)

CReportChildFrameSplit::CReportChildFrameSplit()
{
}

CReportChildFrameSplit::~CReportChildFrameSplit()
{
}


BEGIN_MESSAGE_MAP(CReportChildFrameSplit, CMDIChildWnd)
	//{{AFX_MSG_MAP(CReportChildFrameSplit)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportChildFrameSplit message handlers

BOOL CReportChildFrameSplit::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
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
							RUNTIME_CLASS( CRepControlView ), 
							size, pContext ) )
	{
		TRACE0( "Failed to create top-right view\n" );
		return FALSE;
	}

	size = rect.Size();
	// 4 - Create bottom-left view
	if( !m_wndSplitter2.CreateView( 1, 0,			// row 1, col 0
							RUNTIME_CLASS( CRepListView ), 
							size, pContext ) )
	{
		TRACE0( "Failed to create bottom-right view\n" );
		return FALSE;
	}

	// 3 - Create right column view
	if( !m_wndSplitter1.CreateView( 0, 1,				// row 0, col 1
							RUNTIME_CLASS( CRepGraphView ), 
							CSize(size.cx,size.cy*2), pContext ) )
	{
		TRACE0( "Failed to create left view\n" );
		return FALSE;
	}

	GetWindowRect(&rect);
	m_wndSplitter1.SetColumnInfo(0,350,260);
	m_wndSplitter2.SetRowInfo(0,rect.Height()/2,/*rect.Width()*/0);
	m_wndSplitter2.SetRowInfo(1,rect.Height()/2,0);
	m_wndSplitter1.SetActivePane(0,0);
	return TRUE;
}

void CReportChildFrameSplit::OnSize(UINT nType, int cx, int cy)
{
	CMDIChildWnd::OnSize(nType,cx,cy);
}
void CReportChildFrameSplit::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// update our parent window first
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
			lstrcpy(szText, pDocument->GetTitle() + " - " + ((CTermPlanDoc*)pDocument)->GetARCReportManager().GetRepTitle() );
			
		//if (m_nWindow > 0)
		//	wsprintf(szText + lstrlen(szText), _T(":%d"), m_nWindow);

		// set title if changed, but don't remove completely
		//AfxSetWindowText(m_hWnd, szText);
		this->SetWindowText(szText);
	}
}

int CReportChildFrameSplit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO: Add your specialized creation code here

	return 0;
}

void CReportChildFrameSplit::OnUpdateFrameMenu (BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt)
{
    CMDIChildWnd::OnUpdateFrameMenu (bActivate, pActivateWnd, hMenuAlt);
	if(bActivate)
		GetDocument()->UpdateTrackersMenu();
}
