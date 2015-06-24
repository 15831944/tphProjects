// GannChartFrameWnd.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "GannChartFrameWnd.h"
#include "ListBoxEx.h"
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

const int constLeftLabelBarWidth = 80;
const int constScrollbarWidth = 16;

/////////////////////////////////////////////////////////////////////////////
// CGannChartFrameWnd

CGannChartFrameWnd::CGannChartFrameWnd(InputTerminal* _pInTerm)
{
	m_pInTerm = _pInTerm;
	m_nCurrentPos = 0;
}

CGannChartFrameWnd::~CGannChartFrameWnd()
{
	delete m_pScrollBar;
	delete m_pWndGannChart;
}


BEGIN_MESSAGE_MAP(CGannChartFrameWnd, CWnd)
//{{AFX_MSG_MAP(CGannChartFrameWnd)
ON_WM_CREATE()
ON_WM_PAINT()
ON_MESSAGE( WM_GC_LCLICK, OnGCLClick )
ON_MESSAGE( WM_GC_LDBCLICK, OnGCLDBClick )
ON_WM_VSCROLL()
ON_WM_LBUTTONDOWN()
ON_WM_ERASEBKGND()
ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGannChartFrameWnd message handlers


// just create all the windows.
int CGannChartFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	// create CGannChartWnd
	CRect rectClient;
	GetClientRect( &rectClient );
	CRect rectChild = rectClient;
	rectChild.DeflateRect( constLeftLabelBarWidth, 0, constScrollbarWidth, 0 );
	m_pWndGannChart = new CGannChartWnd( m_pInTerm );
	m_pWndGannChart->CreateEx( WS_EX_CLIENTEDGE, NULL, NULL, WS_HSCROLL|WS_CHILD|WS_VISIBLE, rectChild, this, IDD_FORMVIEW_GANNCHARTFRAME );
	
	// create CScrollBar
	rectChild = rectClient;
	rectChild.DeflateRect( rectChild.Width() - constScrollbarWidth, LABEL_WIDTH, 0, constScrollbarWidth );
	m_pScrollBar = new CScrollBar;
	m_pScrollBar->Create( WS_VISIBLE|WS_CHILD|SBS_VERT,rectChild, this, IDC_SCROLLBAR_GANNCHART );
    
	// create ListBox Window
	rectChild = rectClient;
	m_pWndGannChart->GetWindowRect( &rectClient );
	rectChild.DeflateRect( 0, LABEL_WIDTH+10, rectClient.Width()+constScrollbarWidth, 0 );
	m_pListBoxWnd = new CListBoxEx;
	m_pListBoxWnd->Create( WS_VISIBLE|WS_CHILD | WS_HSCROLL | LBS_NOINTEGRALHEIGHT ,rectChild, this,IDC_LISTBOX_GANNCHART );
	
	return 0;
}



void CGannChartFrameWnd::RefreshAssignInfo(  std::vector<CAssignGate>& _vectGate, int _nPosHour  )
{	
	m_vectGate = _vectGate;	
	RefreshAssignInfo( _nPosHour );
}



// Set the gate for the first time.
void CGannChartFrameWnd::SetGate( const std::vector<CAssignGate>& _vectGate )
{
	m_vectGate = _vectGate;	
	int nCount = m_vectGate.size();
	m_pListBoxWnd->ResetContent();
	for( int i=0;i<nCount; i++)
	{
		CString csName;
		CAssignGate gate = m_vectGate[i];
		csName = gate.GetName();
		m_pListBoxWnd->AddString( csName );
		m_pListBoxWnd->SetItemHeight( i,GANNROW_HEIGHT );
	}
	m_pListBoxWnd->SetColumnWidth( 6*GANNROW_HEIGHT );
	m_pListBoxWnd->SetHorizontalExtent( 5*GANNROW_HEIGHT );
	m_pListBoxWnd->ShowWindow( WM_SHOWWINDOW );
}

// force the v scroll bar to a pos
void CGannChartFrameWnd::SetVScrollBar( int _nCurPos )
{
	
	int nCount;
	nCount = m_vectGate.size();
	
	SCROLLINFO				ScrInfo;
	ScrInfo.cbSize = sizeof(SCROLLINFO);
	ScrInfo.fMask = SIF_PAGE|SIF_RANGE|SIF_POS;
	ScrInfo.nMin = 0;
	ScrInfo.nMax = nCount;
	ScrInfo.nPage = m_pWndGannChart->GetNumLinesOnScreen();
	ScrInfo.nPos = _nCurPos;
	m_pScrollBar->SetScrollInfo(&ScrInfo, true);
	if( m_pScrollBar->GetScrollLimit() > 1 )
		m_pScrollBar->ShowWindow(SW_SHOW);
	else
		m_pScrollBar->ShowWindow(SW_HIDE);
	
	OnVScroll( SB_THUMBPOSITION, _nCurPos, m_pScrollBar );
}







//*****************************************
// protected:
//*****************************************


// refresh to the latest gate assign info from the m_vgate.
void CGannChartFrameWnd::RefreshAssignInfo( int _nPosHour )
{
	int nStartIndex = m_pListBoxWnd->GetTopIndex();		// settopindex might controled by the max scroll.
	std::vector<CAssignGate> vectGate;
	int nInPage = m_pWndGannChart->GetNumLinesOnScreen();
	
	int nGateCount = m_vectGate.size();
		
	for( int i=0; i<nInPage; i++ )
	{
		if( i + nStartIndex >= nGateCount )
			break;
		vectGate.push_back( m_vectGate[i+nStartIndex] );
	}

	m_pWndGannChart->RefreshAssignInfo( vectGate, _nPosHour );
}


// called by framework to manage the scroll change.
// need to set the position of:
// 1. pScrollBar.
// 2. listBox
// 3. invalidate the client window will repostion the list of gannchart.

void CGannChartFrameWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if( pScrollBar != m_pScrollBar )
		return;
	
	int nNewPos = 0;
	
	switch( nSBCode )
	{
	case SB_TOP:
		m_pScrollBar->SetScrollPos( 0, false );
		break;
		
	case SB_LINEDOWN:
		m_nSBCODE = nSBCode;
		nNewPos = m_pScrollBar->GetScrollPos() + 1;
		m_nCurrentPos = nNewPos;
		if( nNewPos <= m_pScrollBar->GetScrollLimit() )
		{
//			// TRACE( "limit = %d\n", m_pScrollBar->GetScrollLimit() );
			m_pScrollBar->SetScrollPos( nNewPos, false );
		}
		break;
		
	case SB_PAGEDOWN:
		m_nSBCODE = nSBCode;
		nNewPos = m_pScrollBar->GetScrollPos() + m_pWndGannChart->GetNumLinesOnScreen();
		if( nNewPos <= m_pScrollBar->GetScrollLimit() )
		{
			m_pScrollBar->SetScrollPos( nNewPos, false );
			m_nCurrentPos = nNewPos;
		}
		else
		{
			m_pScrollBar->SetScrollPos( m_pScrollBar->GetScrollLimit(), false );
			m_nCurrentPos = m_pScrollBar->GetScrollLimit();
		}
		break;
		
	case SB_BOTTOM:
		m_pScrollBar->SetScrollPos( m_pScrollBar->GetScrollLimit(), false );
		m_nCurrentPos = m_pScrollBar->GetScrollLimit();
		break;
		
	case SB_LINEUP:
		m_nSBCODE = nSBCode;
		nNewPos = m_pScrollBar->GetScrollPos() - 1;
		if( nNewPos >= 0)
		{
			m_pScrollBar->SetScrollPos( nNewPos, false );
			m_nCurrentPos = nNewPos;
		}
		break;
		
	case SB_PAGEUP:
		m_nSBCODE = nSBCode;
		nNewPos = m_pScrollBar->GetScrollPos() - m_pWndGannChart->GetNumLinesOnScreen();
		if( nNewPos  >= 0)
		{
			m_pScrollBar->SetScrollPos( nNewPos, false );
			m_nCurrentPos = nNewPos;
		}
		else
		{
			m_pScrollBar->SetScrollPos( 0, false );
			m_nCurrentPos = 0;
		}
		return;
		
	case SB_THUMBPOSITION:
		m_nSBCODE = nSBCode;
		m_pScrollBar->SetScrollPos(nPos, false);
		break;
		
	case SB_ENDSCROLL:
		if(	m_nSBCODE != SB_PAGEDOWN && m_nSBCODE != SB_LINEDOWN &&
			m_nSBCODE != SB_PAGEUP && m_nSBCODE != SB_LINEUP && 
			m_nSBCODE != SB_THUMBPOSITION ) 
			return;
		m_nSBCODE = nSBCode;
		break;
		
	default: 
		return;
	}
	
	// set the listbox
	int nStartIndex = m_pScrollBar->GetScrollPos();
	m_pListBoxWnd->SetTopIndex( nStartIndex );
	
	// set data to gannt row.
	RefreshAssignInfo( -1 );
		
	Invalidate();	// need it for refresh
}





























void CGannChartFrameWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	dc.SetBkMode( TRANSPARENT );
	dc.SetTextColor( GANN_GREEN_COLOR );
	
	
	// put the gate label
	/*	CRect rectClient;
	GetClientRect(&rectClient);
	int top = rectClient.top + TIMELINE_HEIGHT + GAP_TIMELINE_ROW;
	
	  int nCount;
	  nCount = m_vectGate.size();
	  
		int nStartIndex = m_pScrollBar->GetScrollPos();
		for( int i=nStartIndex; i<nCount; i++ )
		{
		CString csName;
		CAssignGate gate = m_vectGate[i];
		gate.GetGateName( csName );
		// draw a rect.
		if( i == m_nCurSel )
		{
		dc.SetTextColor( GANN_BLUE_COLOR );
		}
		else
		{
		dc.SetTextColor( GANN_GREEN_COLOR );
		}
		
		  CSize sizeText;
		  sizeText = dc.GetTextExtent( csName );
		  dc.TextOut( rectClient.left+(LABEL_WIDTH-sizeText.cx)/2, top+(GANNROW_HEIGHT-sizeText.cy)/2, csName );
		  top += GANNROW_HEIGHT;
}*/
	// Do not call CWnd::OnPaint() for painting messages
}

LRESULT CGannChartFrameWnd::OnGCLClick( WPARAM wParam, LPARAM lParam )
{
	// should check the vscroll and get the real index.
	GetParent()->SendMessage( WM_GC_LCLICK, wParam, lParam );
	return 0;
}

LRESULT CGannChartFrameWnd::OnGCLDBClick( WPARAM wParam, LPARAM lParam )
{
	// should check the vscroll and get the real index.
	GetParent()->SendMessage( WM_GC_LDBCLICK, wParam, lParam );
	return 0;
}


void CGannChartFrameWnd::ZoomIn()
{
	m_pWndGannChart->ZoomIn();
}

void CGannChartFrameWnd::ZoomOut()
{
	m_pWndGannChart->ZoomOut();
}


int CGannChartFrameWnd::GetIndex(HWND _hWnd)
{
	return m_pWndGannChart->GetIndex( _hWnd );
}


void CGannChartFrameWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	// check and get index.
	CRect rectClient;
	GetClientRect(&rectClient);
	int top = rectClient.top + TIMELINE_HEIGHT + GAP_TIMELINE_ROW;
	
	int nCount;
	nCount = m_vectGate.size();
	
	int nStartIndex = m_pScrollBar->GetScrollPos();
	int nHitIndex = -1;
	/*	for( int i=0; i<MAX_NUM_ROWS_ONSCREEN; i++ )
	{
	if( i + nStartIndex >= nCount )
	break;
	CRect thisRect( rectClient.left, top, rectClient.left + LABEL_WIDTH, top + GANNROW_HEIGHT );
	if( thisRect.PtInRect( point ) )
	{
	nHitIndex = i + nStartIndex;
	m_pWndGannChart->SetSelected( i );
	break;
	}
	top += GANNROW_HEIGHT;
}*/
	CRect rc;
	m_pListBoxWnd->GetWindowRect( rc );
	//	ScreenToClient( rc );
	if( rc.PtInRect( point ) )
	{
		nHitIndex = m_pListBoxWnd->GetCurSel();
		int	i = nHitIndex - nStartIndex;
		m_pWndGannChart->SetSelected( i );
	}
	
	// send message to parent.
	m_nCurSel = nHitIndex;
	
	GetParent()->SendMessage( WM_LB_LCLICK, nHitIndex, 0 );
	
	Invalidate();
	
	CWnd::OnLButtonDown(nFlags, point);
}




BOOL CGannChartFrameWnd::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect clientRect;
	GetClientRect( clientRect );
	
	CBrush brush;
	brush.CreateSolidBrush( GetSysColor( COLOR_BTNFACE ) );
	pDC->FillRect( clientRect, &brush );
	return CWnd::OnEraseBkgnd(pDC);
}

void CGannChartFrameWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CRect rectClient;
	GetClientRect( &rectClient );
	CRect rectChild = rectClient;
	rectChild.DeflateRect( constLeftLabelBarWidth, 0, constScrollbarWidth, 0 );
	m_pWndGannChart->MoveWindow( rectChild );
	
	// move scroll
	rectChild = rectClient;
	rectChild.DeflateRect( rectChild.Width() - constScrollbarWidth, LABEL_WIDTH, 0, constScrollbarWidth );
	m_pScrollBar->MoveWindow( rectChild );	
	int nStartIndex = m_pScrollBar->GetScrollPos();
	SetVScrollBar( nStartIndex );
	
//	m_pScrollBar->SetScrollPos( nStartIndex );
//	int nIdx = m_pListBoxWnd->GetCurSel();
//	SetListBox();
//	m_pListBoxWnd->SetTopIndex( nStartIndex );
//	if( nStartIndex <= nIdx )
//		m_pListBoxWnd->SetCurSel ( nIdx );
	
	// move label list box
	rectChild = rectClient;
	m_pWndGannChart->GetWindowRect( &rectClient );
	rectChild.DeflateRect( 0, LABEL_WIDTH+10, rectClient.Width()+constScrollbarWidth, 0 );
	m_pListBoxWnd->MoveWindow( rectChild );
	
}



int CGannChartFrameWnd::GetVPos(void)
{
	return m_pScrollBar->GetScrollPos();
}