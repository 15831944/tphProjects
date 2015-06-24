// ChartWnd.cpp : implementation file
//

#include "stdafx.h"
#include "GannChartWnd.h"
#include "inputs\flight.h"
#include "inputs\assigndb.h"
#include "inputs\in_term.h"
#include "common\WinMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGannChartWnd

CGannChartWnd::CGannChartWnd(InputTerminal* _pInTerm)
{
	m_nZoomFactor = 8;	// 24 / 8 = 3 hours
	m_bGateAssignment = true;
	m_pInTerm = _pInTerm;
}

CGannChartWnd::~CGannChartWnd()
{
	delete m_pTimeLine;	
	for( int i=0; i<MAX_NUM_ROWS_ONSCREEN; i++ )
	{
		delete m_pGannRowWnd[i];
	}
}


BEGIN_MESSAGE_MAP(CGannChartWnd, CWnd)
	//{{AFX_MSG_MAP(CGannChartWnd)
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
// CGannChartWnd message handlers

BOOL CGannChartWnd::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect clientRect;
	GetClientRect( &clientRect );
	pDC->FillSolidRect( clientRect,  GANN_BK_COLOR );
	return CWnd::OnEraseBkgnd(pDC);
}

void CGannChartWnd::Clear()
{

}

int CGannChartWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CreateRowWindows();
	return 0;
}


void CGannChartWnd::CreateRowWindows()
{
	CRect rectClient;
	GetClientRect( rectClient );
	rectClient.bottom = rectClient.top + TIMELINE_HEIGHT;

	rectClient.right = rectClient.Width() * m_nZoomFactor;

	// create time line
	m_pTimeLine = new CGannRowWnd( m_pInTerm );
	m_pTimeLine->Create( NULL, NULL, WS_CHILD|WS_VISIBLE, rectClient, this, 101 );
	m_pTimeLine->SetTimeLineFlag();
	if( m_bGateAssignment )
		m_pTimeLine->SetTitle( "Gate Assignment" );
	else
	{
		m_nZoomFactor = 1;
		m_pTimeLine->SetTitle( "Processor Assignment" );
	}

	SCROLLINFO	scrInfo;

	scrInfo.fMask = SIF_PAGE|SIF_RANGE|SIF_POS;
	scrInfo.nMin = 0;
	scrInfo.nMax = 23;
	scrInfo.nPage = 24 / m_nZoomFactor;
	scrInfo.nPos = 0;
	SetScrollInfo( SB_HORZ, &scrInfo, true );

	rectClient.bottom += GAP_TIMELINE_ROW;
	for( int i=0; i<MAX_NUM_ROWS_ONSCREEN; i++ )
	{
		rectClient.top = rectClient.bottom;
		rectClient.bottom = rectClient.top + GANNROW_HEIGHT;
		m_pGannRowWnd[i] = new CGannRowWnd(m_pInTerm);
		m_pGannRowWnd[i]->Create( NULL, NULL, WS_CHILD|WS_VISIBLE, rectClient, this, 101 );
		m_pGannRowWnd[i]->SetAssignType( m_bGateAssignment );
	}
}

void CGannChartWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

void CGannChartWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	ModifyStyleEx( 0, WS_EX_CLIENTEDGE );		
}


// refresh the gate assign information and start postion of the GUI.
void CGannChartWnd::RefreshAssignInfo( const std::vector<CAssignGate>& _vectGate, int _nPosHour )
{
	if( !m_bGateAssignment )
		return;

	m_vectGate = _vectGate;
	int nCount = m_vectGate.size();
	nCount = nCount > MAX_NUM_ROWS_ONSCREEN ? MAX_NUM_ROWS_ONSCREEN : nCount;
	for( int i=0; i<nCount; i++ )
	{
		m_pGannRowWnd[i]->SetSelected( false );
		m_pGannRowWnd[i]->SetAssignedGate( m_vectGate[i] );
		m_pGannRowWnd[i]->ShowWindow( true );
	}
	for(int i=nCount; i<MAX_NUM_ROWS_ONSCREEN; i++ )
	{
		m_pGannRowWnd[i]->ShowWindow( false );
	}
	if( _nPosHour != -1 )
	{
		SCROLLINFO	scrInfo;
		GetScrollInfo( SB_HORZ, &scrInfo, SIF_PAGE );
		scrInfo.fMask = SIF_POS;
		int nPos = _nPosHour - scrInfo.nPage / 2;
		scrInfo.nPos = nPos > 0 ? nPos : 0 ;
		SetScrollInfo( SB_HORZ, &scrInfo, true );
	}
	Invalidate();
}


LRESULT CGannChartWnd::OnGCLClick( WPARAM wParam, LPARAM lParam )
{
	if( lParam < 0 )
	{
		GetParent()->SendMessage( WM_GC_LCLICK, (WPARAM)-1, lParam );
		return 0;
	}
	int nCount = m_vectGate.size();
	int i=0;
	for(; i<nCount; i++ )
	{
		if( m_pGannRowWnd[i]->m_hWnd == (HWND)wParam )
		{
			break;
		}
	}
	if( i < nCount )
	{															
		GetParent()->SendMessage( WM_GC_LCLICK, (WPARAM)i, lParam );
	}
	return 0;
}

LRESULT CGannChartWnd::OnGCLDBClick( WPARAM wParam, LPARAM lParam )
{
	if( lParam < 0 )
	{
		GetParent()->SendMessage( WM_GC_LDBCLICK, (WPARAM)-1, lParam );
		return 0;
	}

	int nCount = m_vectGate.size();
	int i=0; 
	for(; i<nCount; i++ )
	{
		if( m_pGannRowWnd[i]->m_hWnd == (HWND)wParam )
		{
			break;
		}
	}
	if( i < nCount )
	{															
		GetParent()->SendMessage( WM_GC_LDBCLICK, (WPARAM)i, lParam );
	}
	return 0;
}

void CGannChartWnd::ZoomIn()
{
	m_nZoomFactor *= 2;
	m_nZoomFactor = m_nZoomFactor > 24 ? 24 : m_nZoomFactor;
	SCROLLINFO	scrInfo;
	scrInfo.fMask = SIF_PAGE;
	scrInfo.nPage = 24 / m_nZoomFactor;
	SetScrollInfo( SB_HORZ, &scrInfo, true );
	Invalidate();
}

void CGannChartWnd::ZoomOut()
{
	m_nZoomFactor /= 2;
	m_nZoomFactor = m_nZoomFactor < 1 ? 1 : m_nZoomFactor;
	SCROLLINFO	scrInfo;
	scrInfo.fMask = SIF_PAGE;
	scrInfo.nPage = 24 / m_nZoomFactor;
	SetScrollInfo( SB_HORZ, &scrInfo, true );
	Invalidate();
}

void CGannChartWnd::OnPaint() 
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
	int nTotalSize = scrollInfo.nMax + 1;

	nLeft = -( rectClient.Width() * m_nZoomFactor * nCurPos / nTotalSize );
	nRight = rectClient.Width() * m_nZoomFactor * (nTotalSize-nCurPos) / nTotalSize;

	rectClient.left = nLeft;
	rectClient.right = nRight;
	rectClient.bottom = rectClient.top + TIMELINE_HEIGHT;
	m_pTimeLine->MoveWindow( rectClient );

	rectClient.bottom += GAP_TIMELINE_ROW;
	for( int i=0; i<MAX_NUM_ROWS_ONSCREEN; i++ )
	{
		if( m_pGannRowWnd[i]->IsWindowVisible() )
		{
			rectClient.top = rectClient.bottom;
			rectClient.bottom = rectClient.top + GANNROW_HEIGHT;
			m_pGannRowWnd[i]->MoveWindow( rectClient );
		}
	}	
	// Do not call CWnd::OnPaint() for painting messages
}


int CGannChartWnd::GetIndex(HWND _hWnd)
{
	for( int i=0; i<MAX_NUM_ROWS_ONSCREEN; i++ )
	{
		if( m_pGannRowWnd[i]->IsWindowVisible() && ( m_pGannRowWnd[i]->m_hWnd == _hWnd ) )
			return i;
	}
	return -1;
}

void CGannChartWnd::SetAssignType(bool _bGateAssignment)
{
	m_bGateAssignment = _bGateAssignment;

}

int CGannChartWnd::GetNumLinesOnScreen()
{
	CRect rectClient;
	GetClientRect( rectClient );
	return (int)((rectClient.bottom - rectClient.top - 10 ) / GANNROW_HEIGHT ) - 1;
}


void CGannChartWnd::SetSelected(int _nIdx)
{
	for( int i=0; i<MAX_NUM_ROWS_ONSCREEN; i++ )
	{
		if( i == _nIdx )
			m_pGannRowWnd[i]->SetSelected( true );
		else
			m_pGannRowWnd[i]->SetSelected( false );
	}
}


