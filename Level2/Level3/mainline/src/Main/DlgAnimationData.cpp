// DlgAnimationData.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgAnimationData.h"
#include "TermPlanDoc.h"
#include "Common\SimAndReportManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define MINTIME	0
//#define MAXTIME	8640000 //24*3600*100

#define TB_TEXTTIMESTEP	180	//6 hours

#define TB_BARWIDTH			720 //720,480,360,288
#define TB_BARHEIGHT		36
#define	TB_HOURTICK			18
#define TB_SUBTICK			9

//#define TB_PIXELSPERHOUR	(TB_BARWIDTH/24)
//#define TB_MINUTESPERPIXEL	(60/TB_PIXELSPERHOUR)

#define TB_TIMETOHOURDIV	360000
#define TB_TIMETOMINDIV		6000
#define TB_TIMETOSECDIV		100


#define TB_PIXELSPERHOUR_FORPAINT (TB_BARWIDTH/24)


#define TB_BARTOP		40
#define TB_MARKERTOP	60

/////////////////////////////////////////////////////////////////////////////
// CDlgAnimationData dialog
static int nHour = 0;
static int nMin = 0;
static int nSec = 0;

CDlgAnimationData::CDlgAnimationData(CTermPlanDoc* pDoc,AnimationData& animData, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAnimationData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAnimationData)
	m_tEndTime = COleDateTime::GetCurrentTime();
	m_tStartTime = COleDateTime::GetCurrentTime();
	//}}AFX_DATA_INIT
	m_pDoc=pDoc;
	m_nMinTime = animData.nFirstEntryTime;//__max( animData.nAnimStartTime, animData.nFirstEntryTime );
	m_nMaxTime = animData.nLastExitTime;//__min( animData.nAnimEndTime, animData.nLastExitTime );
	
	m_nMinSelectTime = m_nMinTime;
	m_nMaxSelectTime = m_nMaxTime;

	m_nStartTime = m_nMinSelectTime;
	m_nEndTime = m_nMaxSelectTime;

	m_pAnimData = &animData;

	int TotalDay = (m_nEndTime - m_nStartTime) /100 /3600 /24 + 1;
	m_PIXELSPERDay = TB_BARWIDTH / TotalDay;

// format min time and max time;
	bool bAbsDate;
	COleDateTime outDate;
	int nDateIdx;
	COleDateTime outTime;
	const COleDateTime InitTime(0,0,0,0,0,0);
	ElapsedTime elptime;

	//m_nMinTime
	m_pAnimData->m_startDate.GetDateTime( m_nStartTime / TimePrecision, bAbsDate, outDate, nDateIdx, outTime );
	m_pAnimData->m_startDate.GetElapsedTime( bAbsDate, outDate, nDateIdx, InitTime, elptime );
	m_nMinTime = elptime.asSeconds() * TimePrecision;


	//m_nMaxTime
	m_pAnimData->m_startDate.GetDateTime( m_nEndTime / TimePrecision, bAbsDate, outDate, nDateIdx, outTime );
	m_pAnimData->m_startDate.GetElapsedTime( bAbsDate, outDate, nDateIdx, InitTime, elptime );
	m_nMaxTime = elptime.asSeconds() * TimePrecision;	
	if( !(outTime.GetHour()==0 && outTime.GetMinute()==0) )
		m_nMaxTime += 24 * 3600 * TimePrecision;


//

	m_bDragStart = FALSE;
	m_bDragEnd = FALSE;
	if( animData.m_startDate.IsAbsoluteDate() )
		m_bAbsDate = true;
	else
		m_bAbsDate = false;
	m_hMarkerBitmap = ::LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_SLIDERMARKER));
	
//	int nMaxHour = ((m_nMaxTime - m_nMinTime) / 100) / 3600 + 1;
	double dt = (((double)(m_nMaxTime - m_nMinTime) / 100.0) / 3600.0);
	if( dt != 0)
	{
		TB_PIXELSPERHOUR = (double)TB_BARWIDTH / dt;
	}
	else
	{//
		// TRACE("Error");
		TB_PIXELSPERHOUR = 1.0;
	}

	TB_MINUTESPERPIXEL	= 60.0 / TB_PIXELSPERHOUR ;

	animData.bMoveDay = FALSE;

	// TRACE("windows version = %X\n",WINVER);
}

CDlgAnimationData::~CDlgAnimationData()
{
	if(m_hMarkerBitmap != NULL)
		::DeleteObject(m_hMarkerBitmap);
}

void CDlgAnimationData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAnimationData)
	DDX_Control(pDX, IDC_TIME_END, m_EndTimeCtrl);
	DDX_Control(pDX, IDC_TIME_START, m_StartTimeCtrl);
	DDX_Control(pDX, IDC_STARTDAYCOMB, m_StartDayCombCtrl);
	DDX_Control(pDX, IDC_ENDDAYCOMB, m_EndDayCombCtrl);
	DDX_DateTimeCtrl(pDX, IDC_TIME_END, m_tEndTime);
	DDX_DateTimeCtrl(pDX, IDC_TIME_START, m_tStartTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAnimationData, CDialog)
	//{{AFX_MSG_MAP(CDlgAnimationData)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TIME_START, OnDatetimechangeTimeStart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TIME_END, OnDatetimechangeTimeEnd)
	//ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TIME_START2, OnDatetimechangeTimeStart2)
	//ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TIME_END2, OnDatetimechangeTimeEnd2)
	ON_CBN_SELCHANGE(IDC_STARTDAYCOMB, OnSelchangeStartdaycomb)
	ON_CBN_SELCHANGE(IDC_ENDDAYCOMB, OnSelchangeEnddaycomb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAnimationData message handlers

BOOL CDlgAnimationData::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//LPCTSTR lpstrDayDesc[] =	{ _T("Day 1"), _T("Day 2"), _T("Day 3"), _T("Day 4"), _T("Day 5") };
	//for (int i = 0; i < sizeof(lpstrDayDesc) / sizeof(lpstrDayDesc[0]); i++)
	//{
	//	m_StartDayCombCtrl.AddString(lpstrDayDesc[i]);
	//	m_EndDayCombCtrl.AddString(lpstrDayDesc[i]);
	//}
	ElapsedTime estMaxDayTime( m_nEndTime / TimePrecision );
	int nMaxDay = estMaxDayTime.GetDay();
	for (int i=0; i<nMaxDay; i++)
	{
		CString strDay;
		strDay.Format("Day %d", i+1);

		m_StartDayCombCtrl.AddString(strDay);
		m_EndDayCombCtrl.AddString(strDay);
	}
	
	//bool bAbsDate;
	//COleDateTime outDate;
	//int nDateIdx;
	//COleDateTime outTime;

	ElapsedTime eStartTime( m_nStartTime / TimePrecision );
//	animData.m_startDate.GetDateTime( eStartTime, bAbsDate1, m_tStartTime2, m_StartDayIndx, m_tStartTime );
	//m_pAnimData->m_startDate.GetDateTime( eStartTime, bAbsDate, outDate, nDateIdx, outTime );
	//m_tStartTime = outTime;
	m_tStartTime.SetTime(eStartTime.GetHour(), eStartTime.GetMinute(), eStartTime.GetSecond());

	//if( m_bAbsDate )
	//{
	//	m_tStartTime2 = outDate;
	//}
	//else
	//{
//		if( 1 == nDateIdx )
//		{// move a day only for view!
//			nDateIdx -= 1;
//			m_pAnimData->bMoveDay = TRUE;
//		}
		m_StartDayIndx = eStartTime.GetDay();
		m_StartDayCombCtrl.SetCurSel(m_StartDayIndx - 1);
	//}

	ElapsedTime eEndTime( m_nEndTime / TimePrecision );
//	animData.m_startDate.GetDateTime( eEndTime, bAbsDate, m_tEndTime2, m_EndDayIndx, m_tEndTime );
	//m_pAnimData->m_startDate.GetDateTime( eEndTime, bAbsDate, outDate, nDateIdx, outTime );
	//m_tEndTime = outTime;

	m_tEndTime.SetTime(eEndTime.GetHour(), eEndTime.GetMinute(), eEndTime.GetSecond());
/*	if( m_bAbsDate )
	{
		m_tEndTime2 = outDate;
	}
	else
	{	*/	
//		if( m_pAnimData->bMoveDay )
//			nDateIdx -= 1;
		m_EndDayIndx = eEndTime.GetDay();
		m_EndDayCombCtrl.SetCurSel(m_EndDayIndx - 1);
	//}


	UpdateData( false );

	
	CRect rClient;
	GetClientRect(&rClient);
	
	CPoint ptCenter = rClient.CenterPoint();
	m_rTimeBar.left = ptCenter.x - (TB_BARWIDTH/2);
	m_rTimeBar.right = m_rTimeBar.left + TB_BARWIDTH;
	m_rTimeBar.top = TB_BARTOP;//ptCenter.y - (TB_BARHEIGHT/2);
	m_rTimeBar.bottom = m_rTimeBar.top + TB_BARHEIGHT;

	TB_MINUTESPERPIXEL =  ((m_nMaxTime - m_nMinTime)/100) / m_rTimeBar.Width();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAnimationData::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rMinMax, rTime;
	
	//draw 0-24 bar
	//bar width = 720 (0-24 hr.. 30 pixels/hr)
	m_rTimeBar.right++;
	dc.FillSolidRect(m_rTimeBar,RGB(255,255,255));
		
	m_rTimeBar.right--;


	//draw min/max bar
	rMinMax = m_rTimeBar;
	double d = 0.0;
	rMinMax.left += (int) (d*m_rTimeBar.Width());
	d = 1.0;
	rMinMax.right = m_rTimeBar.left + (int) (d*m_rTimeBar.Width()) + 1;
	dc.FillSolidRect(rMinMax, RGB(160,160,160));

	//draw time bar
	rTime = m_rTimeBar;
	d = (m_nStartTime-m_nMinTime)/((double) (m_nMaxTime-m_nMinTime));
	rTime.left += (int) (d*m_rTimeBar.Width());
	d = (m_nEndTime-m_nMinTime)/((double) (m_nMaxTime-m_nMinTime) );
	rTime.right = m_rTimeBar.left + (int) (d*m_rTimeBar.Width()) + 1;
	dc.FillSolidRect(rTime, RGB(0,175,60));

	//draw time ticks
	CPen penBlack(PS_SOLID, 1, RGB(0,0,0));
	CPen penGray(PS_SOLID, 1, RGB(128,128,128));
	CPen penRed(PS_SOLID, 1, RGB(255,0,0));
	CPen *pOldPen;
	pOldPen = dc.SelectObject(&penBlack);


	CPen penStopRed(PS_SOLID, 2, RGB(255,0,0));
	long FrontPos = m_nMinSelectTime - m_nMinTime; // min pos
	d = (double)FrontPos / ((double) (m_nMaxTime - m_nMinTime));
	m_LeftPos =  m_rTimeBar.left + (int) (d * m_rTimeBar.Width());
		
	dc.SelectObject(&penStopRed);
	dc.MoveTo(m_LeftPos, m_rTimeBar.top);
	dc.LineTo(m_LeftPos, m_rTimeBar.bottom);

	long BackPos = m_nMaxTime - m_nMaxSelectTime; // max pos
	d = (double)BackPos/((double) (m_nMaxTime - m_nMinTime));
	m_RightPos =  m_rTimeBar.right - (int) (d * m_rTimeBar.Width());

	dc.SelectObject(&penStopRed);
	dc.MoveTo(m_RightPos, m_rTimeBar.top);
	dc.LineTo(m_RightPos, m_rTimeBar.bottom);


	int i;
	CPen penBlue(PS_SOLID, 1, RGB(0,0,255));
	dc.SelectObject(&penBlue);
	if(m_PIXELSPERDay > 1)
	{
		for( i=m_rTimeBar.left + m_PIXELSPERDay; i<=m_rTimeBar.right+1; i += m_PIXELSPERDay) 
		{
			dc.MoveTo(i, m_rTimeBar.top);
			dc.LineTo(i, m_rTimeBar.top + TB_HOURTICK+5);
		}
	}

	dc.SelectObject(&penBlack);
	for( i=m_rTimeBar.left; i<=m_rTimeBar.right+1; i+=TB_PIXELSPERHOUR_FORPAINT)
	{
		dc.MoveTo(i, m_rTimeBar.top);
		dc.LineTo(i, m_rTimeBar.top+TB_HOURTICK);
	}
	dc.SelectObject(&penGray);
	for(i=m_rTimeBar.left+(TB_PIXELSPERHOUR_FORPAINT/2); i<=m_rTimeBar.right+1; i+=TB_PIXELSPERHOUR_FORPAINT) 
	{
		dc.MoveTo(i, m_rTimeBar.top);
		dc.LineTo(i, m_rTimeBar.top+TB_SUBTICK);
	}


/*
	for(int i=m_rTimeBar.left; i<=m_rTimeBar.right+1; i+=TB_PIXELSPERHOUR) {
		dc.MoveTo(i, m_rTimeBar.top);
		dc.LineTo(i, m_rTimeBar.top+TB_HOURTICK);
	}
	dc.SelectObject(&penGray);
	for(i=m_rTimeBar.left+(TB_PIXELSPERHOUR/2); i<=m_rTimeBar.right+1; i+=TB_PIXELSPERHOUR) {
		dc.MoveTo(i, m_rTimeBar.top);
		dc.LineTo(i, m_rTimeBar.top+TB_SUBTICK);
	}
*/
	dc.SelectObject(&penRed);
	dc.MoveTo(rTime.left, m_rTimeBar.top);
	dc.LineTo(rTime.left, m_rTimeBar.bottom);
	dc.MoveTo(rTime.right-1, m_rTimeBar.top);
	dc.LineTo(rTime.right-1, m_rTimeBar.bottom);
	dc.SelectObject(pOldPen);
	CRect rect=m_rTimeBar;
	rect.InflateRect(0,1,1,1);
	dc.Draw3dRect(rect,EDGE_ETCHED,BF_BOTTOMRIGHT);
	
	//draw markers at anim start and anim end
	ASSERT(m_hMarkerBitmap != NULL);
	HDC hMemDC;
	hMemDC=::CreateCompatibleDC(dc.GetSafeHdc());
	ASSERT(hMemDC != NULL);
	SelectObject(hMemDC,m_hMarkerBitmap);
	::TransparentBlt(dc.GetSafeHdc(),rTime.left-5,TB_MARKERTOP,11,21,hMemDC,0,0,11,21,RGB(255,0,255));
	::TransparentBlt(dc.GetSafeHdc(),rTime.right-6,TB_MARKERTOP,11,21,hMemDC,0,0,11,21,RGB(255,0,255));
	::DeleteDC(hMemDC);
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CDlgAnimationData::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
	
	//check to see if a marker is selected
	//ie. is mouse pointer within 5 pixels in x from either StartTime or EndTime?
	double d = (m_nStartTime-m_nMinTime)/((double) (m_nMaxTime-m_nMinTime));
	int nStartPos = m_rTimeBar.left + (int) (d*m_rTimeBar.Width());
	d = (m_nEndTime-m_nMinTime)/((double) (m_nMaxTime-m_nMinTime));
	int nEndPos = m_rTimeBar.left + (int) (d*m_rTimeBar.Width());

	if(abs(point.x - nStartPos) <= 5 && abs(point.y - (TB_MARKERTOP+10)) <= 10 ) {
		// TRACE("Clicked on StartPos!\n");
		m_bDragStart = TRUE;
	}

	if(abs(point.x - nEndPos) <= 5 && abs(point.y - (TB_MARKERTOP+10)) <= 10) {
		// TRACE("Clicked on EndPos!\n");
		m_bDragEnd = TRUE;
	}

	//m_ptMarkerClickOffset.x = point.x - (nStartPos-5);
	//m_ptMarkerClickOffset.y = point.y - TB_MARKERTOP;
	m_ptStartDrag = point;

	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgAnimationData::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	m_bDragEnd = FALSE;
	m_bDragStart = FALSE;
	CDialog::OnLButtonUp(nFlags, point);
}

void CDlgAnimationData::OnMouseMove(UINT nFlags, CPoint point) 
{	
	int xdiff = point.x - m_ptStartDrag.x;

	if(m_bDragStart)
	{
		double d = (m_nStartTime-m_nMinTime)/((double) (m_nMaxTime-m_nMinTime));

		long tntime;
		if( point.x >= (m_LeftPos) )
		{
			tntime = m_nMinSelectTime + static_cast<long>(TB_MINUTESPERPIXEL * (point.x - m_LeftPos) * 100);
		}
		else
		{
			tntime = m_nMinSelectTime;
		}
		if( tntime > m_nEndTime )
			tntime = m_nEndTime;

		m_nStartTime = tntime;

//		double d = (m_nStartTime-m_nMinTime)/((double) (m_nMaxTime-m_nMinTime));
//		long newStartTime = m_nStartTime + xdiff*TB_MINUTESPERPIXEL*60*100;
//		double newd = (newStartTime-m_nMinTime)/((double) (m_nMaxTime-m_nMinTime));		
//		if(newStartTime >= m_nMinSelectTime && newStartTime <= m_nEndTime) 
//		{
//			m_nStartTime = newStartTime;					
//		}
//		else
//		{
//			if(newStartTime < m_nMinSelectTime)
//				m_nStartTime = m_nMinSelectTime;
//			else
//				m_nStartTime = m_nEndTime;
//			m_bDragStart = FALSE;	
//
//		}

//		CRect invRect;
//		int tmapv = 6;
//		invRect.top = m_rTimeBar.top;
//		invRect.right = (m_rTimeBar.left + (int) (d*m_rTimeBar.Width())) + ((xdiff<0)?(tmapv):(-tmapv));
//		invRect.left = (m_rTimeBar.left + (int) (newd*m_rTimeBar.Width())) + ((xdiff<0)?(-tmapv):(tmapv));
//		invRect.bottom = m_rTimeBar.bottom + 5;

		m_ptStartDrag = point;
		//bool bAbsDate;
		//m_pAnimData->m_startDate.GetDateTime( m_nStartTime / TimePrecision, bAbsDate, m_tStartTime2, m_StartDayIndx, m_tStartTime );
		ElapsedTime estStartTime(m_nStartTime / TimePrecision);
		m_StartDayIndx = estStartTime.GetDay();
		m_tStartTime.SetTime(estStartTime.GetHour(), estStartTime.GetMinute(), estStartTime.GetSecond());

		if( m_pAnimData->bMoveDay )
			m_StartDayIndx -= 1;

		//if(!bAbsDate)
			m_StartDayCombCtrl.SetCurSel(m_StartDayIndx - 1);
		
		CRect invRect;
		int tmapv = 6;
		long newStartTime = m_nStartTime;
		double newd = (newStartTime-m_nMinTime)/((double) (m_nMaxTime-m_nMinTime));		

		invRect.top = m_rTimeBar.top;
		invRect.right = (m_rTimeBar.left + (int) (d*m_rTimeBar.Width())) + ((xdiff<0)?(tmapv):(-tmapv));
		invRect.left = (m_rTimeBar.left + (int) (newd*m_rTimeBar.Width())) + ((xdiff<0)?(-tmapv):(tmapv));
		invRect.bottom = m_rTimeBar.bottom + tmapv;
		UpdateData(FALSE);
		this->InvalidateRect(invRect);
	}
	else if(m_bDragEnd) 
	{
		double d = (m_nEndTime-m_nMinTime)/((double) (m_nMaxTime-m_nMinTime));
		long tntime;
		if( point.x >= (m_LeftPos) )
		{
			tntime = m_nMinSelectTime + static_cast<long>(TB_MINUTESPERPIXEL * (point.x - m_LeftPos) * 100);
		}
		else
		{
			tntime = m_nMinSelectTime;
		}
		if( tntime > m_nMaxSelectTime )
			tntime = m_nMaxSelectTime;

		m_nEndTime = tntime;

//		double d = (m_nEndTime-m_nMinTime)/((double) (m_nMaxTime-m_nMinTime));
//		long newEndTime = m_nEndTime + xdiff*TB_MINUTESPERPIXEL*60*100;
//		double newd = (newEndTime-m_nMinTime)/((double) (m_nMaxTime-m_nMinTime));
//		if(newEndTime >= m_nStartTime && newEndTime <= m_nMaxSelectTime)
//		{
//			m_nEndTime = newEndTime;
//			
//			if( point.x >= (m_LeftPos) )
//			{
//				m_nEndTime += TB_MINUTESPERPIXEL * (point.x - m_LeftPos) * 100;
//			}
//		}
//		else 
//		{
//			if(newEndTime < m_nStartTime)
//				m_nEndTime = m_nStartTime;
//			else
//				m_nEndTime = m_nMaxSelectTime;
//			m_bDragStart = FALSE;
//		}
//		CRect invRect;
//		invRect.top = m_rTimeBar.top;
//		invRect.right = (m_rTimeBar.left + (int) (d*m_rTimeBar.Width())) + ((xdiff<0)?(6):(-6));
//		invRect.left = (m_rTimeBar.left + (int) (newd*m_rTimeBar.Width())) + ((xdiff<0)?(-6):(6));
//		invRect.bottom = m_rTimeBar.bottom + 5;
		m_ptStartDrag = point;
		//bool bAbsDate;
		//m_pAnimData->m_startDate.GetDateTime( m_nEndTime / TimePrecision, bAbsDate, m_tEndTime2, m_EndDayIndx, m_tEndTime );
		ElapsedTime estEndTime(m_nEndTime / TimePrecision);
		m_EndDayIndx = estEndTime.GetDay();
		m_tEndTime.SetTime(estEndTime.GetHour(), estEndTime.GetMinute(), estEndTime.GetSecond());

		//if(!bAbsDate)
		//{
			if( m_pAnimData->bMoveDay )
				m_EndDayIndx -= 1;
			m_EndDayCombCtrl.SetCurSel(m_EndDayIndx-1);
		//}

		long newEndTime = m_nEndTime ;
		double newd = (newEndTime-m_nMinTime)/((double) (m_nMaxTime-m_nMinTime));

		CRect invRect;
		invRect.top = m_rTimeBar.top;
		invRect.right = (m_rTimeBar.left + (int) (d*m_rTimeBar.Width())) + ((xdiff<0)?(6):(-6));
		invRect.left = (m_rTimeBar.left + (int) (newd*m_rTimeBar.Width())) + ((xdiff<0)?(-6):(6));
		invRect.bottom = m_rTimeBar.bottom + 5;
		UpdateData(FALSE);
		this->InvalidateRect(invRect);
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CDlgAnimationData::OnDatetimechangeTimeStart(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);
	
//	long newStartTime = m_tStartTime.GetHour()*TB_TIMETOHOURDIV + 
//						m_tStartTime.GetMinute()*TB_TIMETOMINDIV + 
//						m_tStartTime.GetSecond()*TB_TIMETOSECDIV;
	long newStartTime;
	ElapsedTime tElapTime;
	m_StartDayIndx = m_StartDayCombCtrl.GetCurSel();
	if( m_pAnimData->bMoveDay )
		m_StartDayIndx += 1;

	//m_pAnimData->m_startDate.GetElapsedTime(m_bAbsDate, m_tStartTime2, m_StartDayIndx, m_tStartTime, tElapTime);
	tElapTime.SetDay(m_StartDayIndx+1);
	tElapTime.SetHour(m_tStartTime.GetHour());
	tElapTime.SetMinute(m_tStartTime.GetMinute());
	tElapTime.SetSecond(m_tStartTime.GetSecond());

	newStartTime = tElapTime;
	//newStartTime += m_StartDayIndx * 24 * 3600 * 100;
	if(newStartTime < m_nMinTime) 
	{
		newStartTime = m_nMinTime;
	}
	else if(newStartTime > m_nEndTime) 
	{
		newStartTime = m_nEndTime;
	}
	m_nStartTime = newStartTime;
//	nHour = m_nStartTime/TB_TIMETOHOURDIV;
//	nMin = (m_nStartTime-nHour*TB_TIMETOHOURDIV)/TB_TIMETOMINDIV;
//	nSec = (m_nStartTime-nHour*TB_TIMETOHOURDIV-nMin*TB_TIMETOMINDIV)/TB_TIMETOSECDIV;
//	m_tStartTime.SetTime(nHour,nMin,nSec);
	//bool bAbsDate;
	//m_pAnimData->m_startDate.GetDateTime(newStartTime / TimePrecision, bAbsDate, m_tStartTime2, m_StartDayIndx, m_tStartTime);

	ElapsedTime estNewStartTime(newStartTime / TimePrecision);
	m_StartDayIndx = estNewStartTime.GetDay();
	m_tStartTime.SetTime(estNewStartTime.GetHour(), estNewStartTime.GetMinute(), estNewStartTime.GetSecond());

	if( m_pAnimData->bMoveDay )
		m_StartDayIndx -= 1;

	m_StartDayCombCtrl.SetCurSel(m_StartDayIndx - 1);

	CRect r = m_rTimeBar;
	r.bottom+=5;
	UpdateData(FALSE);
	InvalidateRect(r);
	*pResult = 0;
}

void CDlgAnimationData::OnDatetimechangeTimeEnd(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);
//	long newEndTime = m_tEndTime.GetHour()*TB_TIMETOHOURDIV+m_tEndTime.GetMinute()*TB_TIMETOMINDIV+m_tEndTime.GetSecond()*TB_TIMETOSECDIV;
	long newEndTime;
	ElapsedTime tElapTime;
	m_EndDayIndx = m_EndDayCombCtrl.GetCurSel();
	if( m_pAnimData->bMoveDay )
		m_EndDayIndx += 1;

	//m_pAnimData->m_startDate.GetElapsedTime(m_bAbsDate, m_tEndTime2, m_EndDayIndx, m_tEndTime, tElapTime);
	tElapTime.SetDay(m_EndDayIndx+1);
    tElapTime.SetHour(m_tEndTime.GetHour());
	tElapTime.SetMinute(m_tEndTime.GetMinute());
	tElapTime.SetSecond(m_tEndTime.GetSecond());

	newEndTime = tElapTime;
	//newEndTime += m_EndDayIndx * 24 * 3600 * 100;
	if(newEndTime > m_nMaxTime)
	{
		newEndTime = m_nMaxTime;
	}
	else if(newEndTime < m_nStartTime) {
		newEndTime = m_nStartTime;
	}
	m_nEndTime = newEndTime;
//	nHour = m_nEndTime/TB_TIMETOHOURDIV;
//	nMin = (m_nEndTime-nHour*TB_TIMETOHOURDIV)/TB_TIMETOMINDIV;
//	nSec = (m_nEndTime-nHour*TB_TIMETOHOURDIV-nMin*TB_TIMETOMINDIV)/TB_TIMETOSECDIV;
//	m_tEndTime.SetTime(nHour,nMin,nSec);
	//bool bAbsDate;
	//m_pAnimData->m_startDate.GetDateTime(newEndTime / TimePrecision, bAbsDate, m_tEndTime2, m_EndDayIndx, m_tEndTime);
	ElapsedTime estNewEndTime(newEndTime / TimePrecision);
	m_EndDayIndx = estNewEndTime.GetDay();
	m_tEndTime.SetTime(estNewEndTime.GetHour(), estNewEndTime.GetMinute(), estNewEndTime.GetSecond());

	if( m_pAnimData->bMoveDay )
		m_EndDayIndx -= 1;
	m_EndDayCombCtrl.SetCurSel(m_EndDayIndx-1);
	
	CRect r = m_rTimeBar;
	r.bottom+=5;
	UpdateData(FALSE);
	InvalidateRect(r);
	*pResult = 0;
}

void CDlgAnimationData::OnOK() 
{
	UpdateData(TRUE);
	m_pAnimData->nAnimStartTime = m_nStartTime;
	m_pAnimData->nAnimEndTime = m_nEndTime;
	CDialog::OnOK();
}


//void CDlgAnimationData::OnDatetimechangeTimeStart2(NMHDR* pNMHDR, LRESULT* pResult) 
//{
//	UpdateData(TRUE);
//	
//	long newStartTime;
//	ElapsedTime tElapTime;
//	m_pAnimData->m_startDate.GetElapsedTime(m_bAbsDate, m_tStartTime2, m_StartDayIndx, m_tStartTime, tElapTime);
//	newStartTime = tElapTime;
//	newStartTime += m_StartDayIndx * 24 * 3600 * 100;
//	if(newStartTime < m_nMinTime) 
//	{
//		newStartTime = m_nMinTime;
//	}
//	else if(newStartTime > m_nEndTime) 
//	{
//		newStartTime = m_nEndTime;
//	}
//	m_nStartTime = newStartTime;
//	bool bAbsDate;
//	m_pAnimData->m_startDate.GetDateTime(newStartTime / TimePrecision, bAbsDate, m_tStartTime2, m_StartDayIndx, m_tStartTime);
//
//	CRect r = m_rTimeBar;
//	r.bottom+=5;
//	UpdateData(FALSE);
//	InvalidateRect(r);	
//	*pResult = 0;
//}

//void CDlgAnimationData::OnDatetimechangeTimeEnd2(NMHDR* pNMHDR, LRESULT* pResult) 
//{
//	UpdateData(TRUE);
//	long newEndTime;
//	ElapsedTime tElapTime;
//	m_pAnimData->m_startDate.GetElapsedTime(m_bAbsDate, m_tEndTime2, m_EndDayIndx, m_tEndTime, tElapTime);
//	newEndTime = tElapTime;
//	newEndTime += m_EndDayIndx * 24 * 3600 * 100;
//	if(newEndTime > m_nMaxTime)
//	{
//		newEndTime = m_nMaxTime;
//	}
//	else if(newEndTime < m_nStartTime) {
//		newEndTime = m_nStartTime;
//	}
//	m_nEndTime = newEndTime;
//	bool bAbsDate;
//	m_pAnimData->m_startDate.GetDateTime(newEndTime / TimePrecision, bAbsDate, m_tEndTime2, m_EndDayIndx, m_tEndTime);
//	
//	CRect r = m_rTimeBar;
//	r.bottom+=5;
//	UpdateData(FALSE);
//	InvalidateRect(r);	
//	*pResult = 0;
//}

void CDlgAnimationData::OnSelchangeStartdaycomb() 
{
	assert(false == m_bAbsDate);

	UpdateData(TRUE);
	
	long newStartTime;
	ElapsedTime tElapTime;
	int DayIndx = m_StartDayCombCtrl.GetCurSel();
	if( m_pAnimData->bMoveDay )
		DayIndx += 1;

	//m_pAnimData->m_startDate.GetElapsedTime(m_bAbsDate, m_tStartTime2, DayIndx , m_tStartTime, tElapTime);
	tElapTime.SetDay(DayIndx+1);
	tElapTime.SetHour(m_tStartTime.GetHour());
	tElapTime.SetMinute(m_tStartTime.GetMinute());
	tElapTime.SetSecond(m_tStartTime.GetSecond());

	newStartTime = tElapTime;
//	newStartTime += DayIndx * 24 * 3600 * 100;
	if(newStartTime < m_nMinTime) 
	{
		newStartTime = m_nMinTime;
	}
	else if(newStartTime > m_nEndTime) 
	{
		newStartTime = m_nEndTime;
	}
	m_nStartTime = newStartTime;
	//bool bAbsDate;
	//m_pAnimData->m_startDate.GetDateTime(newStartTime / TimePrecision, bAbsDate, m_tStartTime2, m_StartDayIndx, m_tStartTime);
	ElapsedTime estNewStartTime(newStartTime / TimePrecision);
	m_StartDayIndx = estNewStartTime.GetDay();
	m_tStartTime.SetTime(estNewStartTime.GetHour(), estNewStartTime.GetMinute(), estNewStartTime.GetSecond());

	if( m_pAnimData->bMoveDay )
		m_StartDayIndx -= 1;
	m_StartDayCombCtrl.SetCurSel(m_StartDayIndx-1);

	CRect r = m_rTimeBar;
	r.bottom+=5;
	UpdateData(FALSE);
	InvalidateRect(r);		
}

void CDlgAnimationData::OnSelchangeEnddaycomb() 
{
	assert(false == m_bAbsDate);
	
	UpdateData(TRUE);
	
	long newEndTime;
	ElapsedTime tElapTime;
	int DayIndx = m_EndDayCombCtrl.GetCurSel();
	if( m_pAnimData->bMoveDay )
		DayIndx += 1;

	//m_pAnimData->m_startDate.GetElapsedTime(m_bAbsDate, m_tEndTime2, DayIndx, m_tEndTime, tElapTime);
	tElapTime.SetDay(DayIndx+1);
	tElapTime.SetHour(m_tEndTime.GetHour());
	tElapTime.SetMinute(m_tEndTime.GetMinute());
	tElapTime.SetSecond(m_tEndTime.GetSecond());

	newEndTime = tElapTime;
//	newEndTime += DayIndx * 24 * 3600 * 100;
	if(newEndTime > m_nMaxTime)
	{
		newEndTime = m_nMaxTime;
	}
	else if(newEndTime < m_nStartTime) {
		newEndTime = m_nStartTime;
	}
	m_nEndTime = newEndTime;
	//bool bAbsDate;
	//m_pAnimData->m_startDate.GetDateTime(newEndTime / TimePrecision, bAbsDate, m_tEndTime2, m_EndDayIndx, m_tEndTime);
	ElapsedTime estNewEndTime(newEndTime / TimePrecision);
	m_EndDayIndx = estNewEndTime.GetDay();
	m_tEndTime.SetTime(estNewEndTime.GetHour(), estNewEndTime.GetMinute(), estNewEndTime.GetSecond());

	if( m_pAnimData->bMoveDay )
		m_EndDayIndx -= 1;
	m_EndDayCombCtrl.SetCurSel(m_EndDayIndx-1);
	
	CRect r = m_rTimeBar;
	r.bottom+=5;
	UpdateData(FALSE);
	InvalidateRect(r);		
}
