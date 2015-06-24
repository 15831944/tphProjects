// RowWnd.cpp : implementation file
//

#include "stdafx.h"
#include "GannRowWnd.h"
#include "GannCommon.h"
#include "inputs\flight.h"
#include "inputs\in_term.h"
#include "inputs\assigndb.h"
#include "common\WinMsg.h"
#include "inputs\schedule.h"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define MIN_PUCK_WIDTH 10


/////////////////////////////////////////////////////////////////////////////
// CGannRowWnd

CGannRowWnd::CGannRowWnd(InputTerminal* _pInTerm)
{
	m_bTimeLine = false;
	m_bSelected = false;
	m_pInTerm = _pInTerm;
	m_pProcSchd = NULL;
}

CGannRowWnd::~CGannRowWnd()
{
}


IMPLEMENT_DYNAMIC(CGannRowWnd,CWnd)
BEGIN_MESSAGE_MAP(CGannRowWnd, CWnd)
	//{{AFX_MSG_MAP(CGannRowWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_NEEDTEXTA, 0, OnToolTipText)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGannRowWnd message handlers

void CGannRowWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// set graphic items
	CBrush* pBkBrush = new CBrush( GANN_BK_COLOR );
	CBrush* pOldBrush = dc.SelectObject( pBkBrush );
	CPen* pBorderPen = new CPen( PS_SOLID, 0, GANN_BK_COLOR );
	CPen* pOldPen   = dc.SelectObject( pBorderPen );
	dc.SetBkColor( GANN_BK_COLOR );
	
	// set m_timeLeft/Right
	if( m_timeLeft == NULL)
	{
		m_timeRight = m_timeLeft + COleDateTimeSpan(1, 0, 0, 0);
	}


	// paint time line if it is
	if( m_bTimeLine )
	{
		PaintHeader( &dc );
		delete pBkBrush;
		delete pBorderPen;
		return;
	}
	
	CRect rectClient;
	GetClientRect(&rectClient);
	rectClient.DeflateRect( BLANK_SPACE-1, 4, BLANK_SPACE-1, 4 );

	// draw ibeam
	CRect rectTemp = rectClient;
	dc.DrawEdge( &rectTemp, EDGE_SUNKEN, BF_LEFT | BF_RIGHT );
	rectTemp.DeflateRect(2, 0);
	dc.DrawEdge(&rectTemp, EDGE_RAISED, BF_LEFT | BF_RIGHT );
	rectTemp = rectClient;
	rectTemp.DeflateRect( 2, 10 );
	dc.DrawEdge(&rectTemp, EDGE_SUNKEN, BF_TOP | BF_BOTTOM );
	rectTemp.DeflateRect(0, 1);
	dc.DrawEdge(&rectTemp, EDGE_RAISED, BF_TOP | BF_BOTTOM );

	// create vectRect
	m_vectLabel.clear();
	m_vectRect.clear();
	int nCount = 0;
	std::vector<BOOL> vectSel;

	if( m_bGateAssignment )
	{
		nCount = m_assignedGate.GetFlightCount();
		for( int i=0; i<nCount; i++ )
		{
			CFlightOperationForGateAssign aFlight = m_assignedGate.GetFlight( i );

			vectSel.push_back( aFlight.IsSelected() );
			
			char str[128];
			aFlight.getFlight()->getFullID( str );
			m_vectLabel.push_back( CString( str ) );

			ElapsedTime leftTime( 0L );
			ElapsedTime rightTime( 24L * 3600L );
			ElapsedTime gateTime;
			ElapsedTime duration;

			gateTime = aFlight.GetStartTime();
			///////////////////////////////////////////////////////////////////
			//if( aFlight.getFlightMode() == 'D' )
			//	gateTime = aFlight.getDepTime() - aFlight.getServiceTime();
			/////////////////////////////////////////////////////////////////////
			//if (aFlight.isTurnaround())
			//	duration = aFlight.getDepTime() - aFlight.getArrTime();
			//else
				duration = aFlight.GetEndTime() - aFlight.GetStartTime();


			CRect rectPuck = rectTemp;
			rectPuck.left = static_cast<LONG>(rectTemp.left + ( gateTime - leftTime ) / ( rightTime - leftTime )*rectTemp.Width());
			rectPuck.right = static_cast<LONG>(rectPuck.left + duration / ( rightTime - leftTime )*rectTemp.Width());

			if( rectPuck.Width() < MIN_PUCK_WIDTH )
				rectPuck.right = rectPuck.left + MIN_PUCK_WIDTH;

			m_vectRect.push_back( rectPuck );
		}
	}
	else
	{

		if( !m_pProcSchd )
			return;

		nCount = m_pProcSchd->getCount();

		for( int i=0; i<nCount; i++ )
		{
			ProcessorRoster* pProcAssignment = m_pProcSchd->getItem( i );
			CString tempStr;

			if( !pProcAssignment->isOpen() )
				continue;

			// get label
			const CMultiMobConstraint* pMultPaxConstr = pProcAssignment->getAssignment();
			int nContrCount = pMultPaxConstr->getCount();
			for( int j=0; j<nContrCount; j++ )
			{
				//char buf[256];
				CString buf;
				pMultPaxConstr->getConstraint(j)->screenPrint( buf );
				if( j > 0 )
					tempStr += CString(",");
				tempStr +=  buf ;
			}

			// get opentime / closetime
			ElapsedTime leftTime( 0L );
			ElapsedTime rightTime( 24L * 3600L );
			ElapsedTime openTime;
			ElapsedTime closeTime;

			CRect rectPuck = rectTemp;
			openTime = pProcAssignment->getAbsOpenTime();
			rectPuck.left = static_cast<LONG>(rectTemp.left + ( openTime - leftTime ) / ( rightTime - leftTime )*rectTemp.Width());

			if( i == nCount - 1 )
			{
				rectPuck.right = rectTemp.right;
			}
			else
			{
				closeTime = m_pProcSchd->getItem( i+1 )->getAbsOpenTime();
				if( closeTime == openTime )
					continue;
				rectPuck.right = static_cast<LONG>( rectPuck.left + ( closeTime - openTime ) / ( rightTime - leftTime )*rectTemp.Width() );
			}

			m_vectRect.push_back( rectPuck );
			m_vectLabel.push_back( tempStr );
		}
	}


	// base on 3 vectors draw pucks
	int nItemCount = m_vectRect.size();
	for( int i=0; i<nItemCount; i++ )
	{
		CRect rectPuck = m_vectRect[i];


		// draw rectangle.
		if( m_bSelected || ( m_bGateAssignment && vectSel[i] ) )
		{
			dc.FillSolidRect( rectPuck, GANN_BLUE_COLOR );
		}
		else
		{
			dc.FillSolidRect( rectPuck, GANN_GREEN_COLOR );
		}
		rectPuck.DeflateRect( 1,1 );
		dc.DrawEdge( &rectPuck, EDGE_RAISED, BF_RECT);

		// draw label
		rectPuck.DeflateRect( 1,1 );
		CFont newFont;
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		strcpy(lf.lfFaceName, "Arial");
		lf.lfHeight = rectPuck.Height() - 6;
		newFont.CreateFontIndirect(&lf);
		CFont* pOldFont = dc.SelectObject(&newFont);

		if( m_bSelected || ( m_bGateAssignment && vectSel[i] ) )
		{
			dc.SetTextColor( GANN_BK_COLOR );
			dc.SetBkColor( GANN_BLUE_COLOR );
		}
		else
		{
			dc.SetBkColor( GANN_GREEN_COLOR );
			dc.SetTextColor( GANN_BLACK_COLOR );
		}
		CString tempStr = m_vectLabel[i];
		CRect rectTemp( rectPuck );
		rectPuck.DeflateRect( 3, 1 );
		int nStrLen = tempStr.GetLength();
		while( nStrLen > 1 )
		{
			dc.DrawText( tempStr, rectTemp, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_CALCRECT );
			if( rectTemp.right < rectPuck.right )
				break;
			tempStr.Delete( nStrLen - 1 );
			nStrLen--;
		}
		dc.DrawText( tempStr, rectPuck, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		dc.SelectObject(pOldFont);

	}

	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
	delete pBkBrush;
	delete pBorderPen;
}



void CGannRowWnd::PaintHeader(CDC * pDC)
{

	CString csLeft;
	CString csRight;
	CRect rectClient;
	int nTextBottom = 0;
	CSize sizeText;

	GetClientRect(&rectClient);
	CPen pen(PS_SOLID, 2, RGB(0,0,0));
	pDC->SelectObject(&pen);
	
	// title
	if( !m_csTitle.IsEmpty() )
	{
		sizeText = pDC->GetTextExtent( m_csTitle );
		pDC->SetTextColor( GANN_GREEN_COLOR );
		pDC->TextOut( rectClient.left + ((rectClient.right - rectClient.left) / 2) - (sizeText.cx / 2),
						rectClient.top, m_csTitle );
		if( sizeText.cy > nTextBottom )
			nTextBottom = sizeText.cy;
	}
	rectClient.top += nTextBottom;
   
//   memset(DataPoint, 0, sizeof(int)*128);

	// set time string
	CSize pointText;
	// Place the Beginning Time in the LeftText Area
	csLeft.Format("%s", m_timeLeft.Format("%H:%M"));
	// Place the Ending Time in the RightText Area
	csRight.Format("%s", ( m_timeLeft + COleDateTimeSpan(0,(int)( m_timeRight - m_timeLeft ).GetTotalHours(),0,0)).Format("%H:%M") );
	sizeText = pDC->GetTextExtent( csLeft );
	pointText.cx = rectClient.left + sizeText.cx;
	pDC->SetTextColor( GANN_BLACK_COLOR );
	pDC->TextOut( rectClient.left, rectClient.top, csLeft );
	rectClient.left += BLANK_SPACE;
	if( sizeText.cy > nTextBottom )
		nTextBottom = sizeText.cy;
	sizeText = pDC->GetTextExtent( csRight );
	pointText.cy = rectClient.right - sizeText.cx;
	pDC->SetTextColor( GANN_BLACK_COLOR );
	pDC->TextOut( rectClient.right - sizeText.cx, rectClient.top, csRight );
	rectClient.right -= BLANK_SPACE;
	if( sizeText.cy > nTextBottom )
		nTextBottom = sizeText.cy;
	rectClient.top += nTextBottom;
	
	// Draw the Outer outline of DateLine
	pDC->MoveTo( rectClient.right, rectClient.top );
	pDC->LineTo( rectClient.right, rectClient.bottom );
	pDC->LineTo( rectClient.left, rectClient.bottom);
	pDC->LineTo( rectClient.left, rectClient.top);

	// Determine the Spacing of the 'Tick Marks'
	int nLeftPoint = rectClient.left; 
	COleDateTime timeTemp = m_timeLeft + COleDateTimeSpan(0,1,0,0);
	COleDateTimeSpan timeSpan = m_timeRight - m_timeLeft;
	int nSpan = (int)timeSpan.GetTotalHours() + 1;
	for(int i = 1; i < nSpan; i++)
	{
		int nLineSpace;
		CString csHour;
		nLineSpace = ( rectClient.right - nLeftPoint) / (nSpan - i);
		// Place the Time on the top line of TimeLine
		csHour.Format("%s", timeTemp.Format("%H:%M"));
		sizeText = pDC->GetTextExtent( csHour );
		if( (pointText.cx + 6) < ( nLeftPoint + nLineSpace - (sizeText.cx / 2)) && 
				(pointText.cy - 6) > (nLeftPoint + nLineSpace + (sizeText.cx / 2)))
		{
			pDC->SetTextColor( GANN_BLACK_COLOR );
			pDC->TextOut( nLeftPoint + nLineSpace - (sizeText.cx / 2),
							rectClient.top - sizeText.cy, csHour );
			pointText.cx = nLeftPoint + nLineSpace + (sizeText.cx / 2);
		}
		

		// write ticks
		int nGranularity = 0;
		if( nLineSpace / 4 > 20 )
			nGranularity = 2;
		else if( nLineSpace / 2 > 20 )
			nGranularity = 3;

		for( int nTicks = 1; nTicks < 12; nTicks++ )
		{
			if( nGranularity == 0 )
			{  
				if( nTicks%12 )
					continue;
			}
			else if( nGranularity == 1 )
			{
				if( nTicks%6 )
					continue;
			}
			else if( nGranularity == 2 )
			{
				if( nTicks%3 )
					continue;
			}
			if( !(nTicks%6) )
				pDC->MoveTo( nLeftPoint + MulDiv( nLineSpace, nTicks, 12 ), 
								rectClient.top + ( ( rectClient.bottom - rectClient.top ) / 3 ) );
			else if(!(nTicks%3))
				pDC->MoveTo( nLeftPoint + MulDiv( nLineSpace, nTicks, 12), 
								rectClient.bottom - ((rectClient.bottom - rectClient.top) / 2));
			else
				pDC->MoveTo( nLeftPoint + MulDiv( nLineSpace, nTicks, 12), 
								rectClient.bottom - ((rectClient.bottom - rectClient.top) / 3));
			pDC->LineTo( nLeftPoint + MulDiv( nLineSpace, nTicks, 12), rectClient.bottom );
		}
		// Move to the right of the 'Cell'
		nLeftPoint += nLineSpace;
		// Draw the vertical line of the 'Tick Mark'
		pDC->MoveTo( nLeftPoint, rectClient.top);
		pDC->LineTo( nLeftPoint, rectClient.bottom );
		// Increment the 'Index' by 1 hour
		timeTemp += COleDateTimeSpan(0,1,0,0);
	}// for
}


void CGannRowWnd::SetTimeLineFlag()
{
	m_bTimeLine = true;
}

void CGannRowWnd::SetTitle(CString _csTitle)
{
	m_csTitle = _csTitle;
}

void CGannRowWnd::SetAssignedGate( const CAssignGate& _gate )
{
	m_assignedGate = _gate;
}

void CGannRowWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int nIndex = GetRectIndex( point );
	GetParent()->SendMessage( WM_GC_LCLICK, (WPARAM)m_hWnd, (LPARAM)nIndex );  // -1 for no sel
	CWnd::OnLButtonDown(nFlags, point);
}

void CGannRowWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int nIndex = GetRectIndex( point );
	GetParent()->SendMessage( WM_GC_LDBCLICK, (WPARAM)m_hWnd, (LPARAM)nIndex );
		
	CWnd::OnLButtonDblClk(nFlags, point);
}

int CGannRowWnd::GetRectIndex(const CPoint &_point)
{
	int nCount = m_vectRect.size();
	for( int i=0; i<nCount; i++ )
	{
		if( m_vectRect[i].PtInRect( _point ) )
		{
			break;
		}
	}
	if( i < nCount )
		return i;
	return -1;
}


BOOL CGannRowWnd::OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	CPoint ptCur;
	VERIFY(::GetCursorPos(&ptCur));
	ScreenToClient(&ptCur);
	int nIndex = GetRectIndex( ptCur );	
	if( nIndex < 0 )
		return FALSE;

	assert( m_pInTerm );

	if( m_bGateAssignment )
		m_assignedGate.GetToolTips( nIndex, m_csToolTips, m_pInTerm->inStrDict);
	else
		m_csToolTips = m_vectLabel[nIndex];
	pTTTA->lpszText = (char*)(LPCSTR)m_csToolTips;
	return TRUE;
}

int CGannRowWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CRect rectClient;
	GetClientRect( rectClient );
	m_ctrlTooltip.Create( this, TTS_ALWAYSTIP );
	m_ctrlTooltip.AddTool( this, LPSTR_TEXTCALLBACK, rectClient, 1 );	
	m_ctrlTooltip.SetMaxTipWidth(SHRT_MAX);
	m_ctrlTooltip.SetDelayTime(TTDT_AUTOPOP, 15000);
	m_ctrlTooltip.Activate(true);	
	return 0;
}

BOOL CGannRowWnd::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( ::IsWindow(m_ctrlTooltip.m_hWnd) && pMsg->hwnd == m_hWnd)
	{
		switch(pMsg->message)
		{
		case WM_LBUTTONDOWN:
		case WM_MOUSEMOVE:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			m_ctrlTooltip.RelayEvent(pMsg);
			break;
		default: 
			break;
		}
	}	
	return CWnd::PreTranslateMessage(pMsg);
}

void CGannRowWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( ::IsWindow(m_ctrlTooltip.m_hWnd) )
	{
		int nIndex = GetRectIndex( point );	
		if( nIndex != m_nCurToolTipsIndex )
			m_ctrlTooltip.Pop();
		m_nCurToolTipsIndex = nIndex;
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

void CGannRowWnd::SetAssignType(bool _bGateAssignment)
{
	m_bGateAssignment = _bGateAssignment;
}


void CGannRowWnd::SetSelected(bool _bSelected)
{
	m_bSelected = _bSelected;
}

void CGannRowWnd::SetProcAssnSchd(ProcessorRosterSchedule *_pProcSchd)
{
	m_pProcSchd = _pProcSchd;
}
