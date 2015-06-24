// TimeSelector.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "TimeSelector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MINTIME	0
#define MAXTIME	8640000 //24*3600*100

/////////////////////////////////////////////////////////////////////////////
// CTimeSelector

CTimeSelector::CTimeSelector()
{
	m_nMaxTime = MAXTIME-1;
	m_nMinTime = MINTIME;
	m_nStartTime = 9*3600*100;	//9am
	m_nEndTime = 17*3600*100;	//5pm
	m_hMarkerBitmap = ::LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_SLIDERMARKER));
	// TRACE("windows version = %X\n",WINVER);
}

CTimeSelector::~CTimeSelector()
{
	if(m_hMarkerBitmap != NULL)
		::DeleteObject(m_hMarkerBitmap);
}


BEGIN_MESSAGE_MAP(CTimeSelector, CStatic)
	//{{AFX_MSG_MAP(CTimeSelector)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTimeSelector::SetMaxTime(long nTime)
{
	ASSERT(nTime >= MINTIME && nTime < MAXTIME);
	m_nMaxTime = nTime;
	if(m_nMaxTime < m_nMinTime)
		m_nMinTime = m_nMaxTime;
}

void CTimeSelector::SetMinTime(long nTime)
{
	ASSERT(nTime >= MINTIME && nTime < MAXTIME);
	m_nMinTime = nTime;
	if(m_nMinTime > m_nMaxTime)
		m_nMaxTime = m_nMinTime;
}

void CTimeSelector::SetStartTime(long nTime)
{
	ASSERT(nTime >= MINTIME && nTime < MAXTIME);
	m_nStartTime = nTime;
}

void CTimeSelector::SetEndTime(long nTime)
{
	ASSERT(nTime >= MINTIME && nTime < MAXTIME);
	m_nEndTime = nTime;
}

/////////////////////////////////////////////////////////////////////////////
// CTimeSelector message handlers

void CTimeSelector::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect r, r2;
	GetClientRect(&r);
	
	dc.FillSolidRect(r, RGB(255,0,0));
	
	r2 = r;
	double d = m_nStartTime/((double) MAXTIME);
	r2.left += (int) (d*r.Width());

	d = m_nEndTime/((double) MAXTIME);
	r2.right = r.left + (int) (d*r.Width());

	dc.FillSolidRect(r2, RGB(0,0,255));


	//draw markers at anim start and anim end
	ASSERT(m_hMarkerBitmap != NULL);
	HDC hMemDC;
	hMemDC=::CreateCompatibleDC(dc.GetSafeHdc());
	ASSERT(hMemDC != NULL);
	SelectObject(hMemDC,m_hMarkerBitmap);
	//::TransparentBlt(dc.GetSafeHdc(),r2.left-5,10,11,21,hMemDC,0,0,11,21,RGB(255,0,255));
	::StretchBlt(dc.GetSafeHdc(),r2.left-5,10,11,21,hMemDC,0,0,11,21,SRCCOPY);
	::DeleteDC(hMemDC);
	
	// Do not call CStatic::OnPaint() for painting messages
}
