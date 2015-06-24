// GanntCharOneLineWnd.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "GanntChartOneLineWnd.h"
#include "common\WinMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGanntChartOneLineWnd

CGanntChartOneLineWnd::CGanntChartOneLineWnd(InputTerminal* _pInTerm)
{
	m_nZoomFactor = 1;	// 24 / 8 = 3 hours
	m_pInTerm = _pInTerm;
}

CGanntChartOneLineWnd::~CGanntChartOneLineWnd()
{
	delete m_pTimeLine;
	delete m_pGannRowWnd;	
}


BEGIN_MESSAGE_MAP(CGanntChartOneLineWnd, CWnd)
	//{{AFX_MSG_MAP(CGanntChartOneLineWnd)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_MESSAGE( WM_GC_LCLICK, OnGCLClick )
	ON_MESSAGE( WM_GC_LDBCLICK, OnGCLDBClick )
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGanntChartOneLineWnd message handlers

int CGanntChartOneLineWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CreateRowWindows();	
	return 0;
}


void CGanntChartOneLineWnd::CreateRowWindows()
{
	CRect rectClient;
	GetClientRect( rectClient );
	rectClient.bottom = rectClient.top + TIMELINE_HEIGHT;

	rectClient.right = rectClient.Width() * m_nZoomFactor;

	// create time line
	m_pTimeLine = new CGannRowWnd( m_pInTerm );
	m_pTimeLine->Create( NULL, NULL, WS_CHILD|WS_VISIBLE, rectClient, this, 101 );
	m_pTimeLine->SetTimeLineFlag();
	m_pTimeLine->SetTitle( "Processor Assignment" );

	SCROLLINFO	scrInfo;

	scrInfo.fMask = SIF_PAGE|SIF_RANGE|SIF_POS;
	scrInfo.nMin = 0;
	scrInfo.nMax = 23;
	scrInfo.nPage = 24 / m_nZoomFactor;
	scrInfo.nPos = 0;
	SetScrollInfo( SB_HORZ, &scrInfo, true );

	rectClient.bottom += GAP_TIMELINE_ROW;
	rectClient.top = rectClient.bottom;
	rectClient.bottom = rectClient.top + GANNROW_HEIGHT;
	m_pGannRowWnd = new CGannRowWnd(m_pInTerm);
	m_pGannRowWnd->Create( NULL, NULL, WS_CHILD|WS_VISIBLE, rectClient, this, 101 );
	m_pGannRowWnd->SetAssignType( false );
}

void CGanntChartOneLineWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	int pos, minNum, maxNum, limitPos, pageNum, prevPos;

	limitPos = GetScrollLimit( SB_HORZ );
	GetScrollRange( SB_HORZ, &minNum, &maxNum );
	pageNum = maxNum - limitPos + 1;
	prevPos = GetScrollPos( SB_HORZ ); 
	pos = -1;

	switch( nSBCode )
	{
	case SB_LEFT:
		pos = 0;
		break;
	case SB_ENDSCROLL:
		return;
	case SB_LINELEFT:
		pos = prevPos - 1;
		pos = ( pos<0 ? 0 : pos );
		break;
	case SB_LINERIGHT:
		pos = prevPos + 1;
		pos = ( pos>limitPos ? limitPos : pos );
		break;
	case SB_PAGELEFT:
		pos = prevPos - pageNum;
		pos = ( pos<0 ? 0 : pos );
		break;
	case SB_PAGERIGHT:
		pos = prevPos + pageNum;
		pos = ( pos>limitPos ? limitPos : pos );
		break;
	case SB_RIGHT:
		pos = limitPos;
		break;
	case SB_THUMBPOSITION:
		pos = nPos;
		break;
	case SB_THUMBTRACK:
		pos = nPos;
		break;
	}

	if( pos >= 0 )
	{
		SetScrollPos( SB_HORZ, pos );
//		SetRosterRowRect( pos );
		Invalidate();
	}
	
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CGanntChartOneLineWnd::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect clientRect;
	GetClientRect( &clientRect );
	pDC->FillSolidRect( clientRect,  GANN_BK_COLOR );
	return CWnd::OnEraseBkgnd(pDC);
}

void CGanntChartOneLineWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	ModifyStyleEx( 0, WS_EX_CLIENTEDGE );		
	
}

void CGanntChartOneLineWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect rectClient;
	int	nLeft;
	int	nRight;

	SCROLLINFO	scrollInfo;
	GetClientRect( rectClient );
	GetScrollInfo( SB_HORZ, &scrollInfo, SIF_ALL );
	int nCurPos = scrollInfo.nPos;
//	int nTotalSize = scrollInfo.nMax + 1;
	int nTotalSize = scrollInfo.nMax;

	nLeft = -( rectClient.Width() * m_nZoomFactor * nCurPos / nTotalSize );
	nRight = rectClient.Width() * m_nZoomFactor * (nTotalSize-nCurPos) / nTotalSize;

	rectClient.left = nLeft;
	rectClient.right = nRight;
	rectClient.bottom = rectClient.top + TIMELINE_HEIGHT;
	m_pTimeLine->MoveWindow( rectClient );

	rectClient.bottom += GAP_TIMELINE_ROW;
	rectClient.top = rectClient.bottom;
	rectClient.bottom = rectClient.top + GANNROW_HEIGHT;
	m_pGannRowWnd->MoveWindow( rectClient );
	
	// Do not call CWnd::OnPaint() for painting messages
}


LRESULT CGanntChartOneLineWnd::OnGCLClick( WPARAM wParam, LPARAM lParam )
{
	GetParent()->SendMessage( WM_GC_LCLICK, wParam, lParam );
	return 0;
}

LRESULT CGanntChartOneLineWnd::OnGCLDBClick( WPARAM wParam, LPARAM lParam )
{
	GetParent()->SendMessage( WM_GC_LDBCLICK, wParam, lParam );
	return 0;
}

void CGanntChartOneLineWnd::ZoomIn()
{
	m_nZoomFactor *= 2;
	m_nZoomFactor = m_nZoomFactor > 24 ? 24 : m_nZoomFactor;
	SCROLLINFO	scrInfo;
	scrInfo.fMask = SIF_PAGE;
	scrInfo.nPage = 24 / m_nZoomFactor;
	SetScrollInfo( SB_HORZ, &scrInfo, true );
	Invalidate();
}

void CGanntChartOneLineWnd::ZoomOut()
{
	m_nZoomFactor /= 2;
	m_nZoomFactor = m_nZoomFactor < 1 ? 1 : m_nZoomFactor;
	SCROLLINFO	scrInfo;
	scrInfo.fMask = SIF_PAGE;
	scrInfo.nPage = 24 / m_nZoomFactor;
	SetScrollInfo( SB_HORZ, &scrInfo, true );
	Invalidate();
}


void CGanntChartOneLineWnd::SetProcAssnSchd(ProcessorRosterSchedule *_pProcSchd)
{
	m_pGannRowWnd->SetProcAssnSchd( _pProcSchd );
}
