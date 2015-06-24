// BaseCallOutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\TermPlan.h"
#include "BaseCallOutDlg.h"
// CBaseCallOutDlg dialog
#include "..\TermPlanDoc.h"
#include ".\basecalloutdlg.h"
IMPLEMENT_DYNAMIC(CBaseCallOutDlg, CDragDialog)

CBaseCallOutDlg::CBaseCallOutDlg(CTermPlanDoc* _TermPlanDoc, CWnd* pParent /*= NULL*/ ):
CDragDialog(CBaseCallOutDlg::IDD, pParent),m_TermPlanDoc(_TermPlanDoc)
{
	m_ButtonDown = FALSE ;
	m_PriPoint.x = 0 ;
	m_PriPoint.y = 0 ;
	m_OldX = 0 ;
	m_OldY = 0 ;
	m_bMouseTracking = FALSE ;
	m_isclose = FALSE ;
}
CBaseCallOutDlg::~CBaseCallOutDlg()
{
}

void CBaseCallOutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDragDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_STATIC_STARTVAL,m_StaticStartTime) ;
	DDX_Control(pDX,IDC_STATIC_ENDVAL,m_StaticEndTime) ;
	DDX_Control(pDX,IDC_STATIC_INTERVAL,m_StaticInterval) ;
	DDX_Control(pDX,IDC_STATIC_TIME,m_StaticCurrentTime) ;
	DDX_Control(pDX,IDC_STATIC_PRCOVAL,m_StaticProcssor) ;
	DDX_Control(pDX,IDC_BUTTON_CLOSE,m_CloseButton) ;
}

void CBaseCallOutDlg::OnSize(UINT nType, int cx, int cy)
{
	CDragDialog::OnSize(nType,cx,cy) ;
	if(m_OldX == 0)
	{
		m_OldX = cx   ;
		m_OldY = cy   ;
		return ;
	}
	if(GetSafeHwnd() == NULL)
		return ;
	int offX = cx - m_OldX ;
	int offY = cy - m_OldY ;
	
	CRect rect ;
	GetDlgItem(IDC_STATIC_ETCH)->GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	rect.right = rect.right + offX ;
	GetDlgItem(IDC_STATIC_ETCH)->MoveWindow(&rect) ;

	/*GetDlgItem(IDC_STATIC_PRCOVAL)->GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	rect.right = rect.right + offX*1/2 ;
	GetDlgItem(IDC_STATIC_PRCOVAL)->MoveWindow(&rect) ;

	GetDlgItem(IDC_STATIC_TIMENAME)->GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	rect.left = rect.left + offX*1/2;
	rect.right = rect.right + offX*1/2 ;
	GetDlgItem(IDC_STATIC_TIMENAME)->MoveWindow(&rect) ;

	m_StaticCurrentTime.GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	rect.left = rect.left + offX*1/2;
	rect.right = rect.right + offX ;
	m_StaticCurrentTime.MoveWindow(&rect) ;*/

	m_CloseButton.GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	int width = rect.Width() ;
	rect.right = cx ;
	rect.left = cx - width ;
	m_CloseButton.MoveWindow(&rect) ;

	
	/*m_StaticStartTime.GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	rect.right = rect.right + offX*1/3 ;
	m_StaticStartTime.MoveWindow(&rect) ;

	GetDlgItem(IDC_STATIC_ENDTIME)->GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	rect.left = rect.left + offX*1/3 ;
	rect.right = rect.right + offX*1/3 ;
	GetDlgItem(IDC_STATIC_ENDTIME)->MoveWindow(&rect) ;

	m_StaticEndTime.GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	rect.left = rect.left + offX*1/3 ;
	rect.right = rect.right + offX*2/3 ;
	m_StaticEndTime.MoveWindow(&rect) ;

	GetDlgItem(IDC_STATIC_INTERVAL_NAME)->GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	rect.left = rect.left + offX-(offX*1/3) ;
	rect.right = rect.right + offX-(offX*1/3) ;
	GetDlgItem(IDC_STATIC_INTERVAL_NAME)->MoveWindow(&rect) ;*/

	/*m_StaticInterval.GetWindowRect(&rect ) ;
	ScreenToClient(&rect) ;
	rect.left = rect.left + offX-(offX*1/3) ;
	rect.right = rect.right + offX ;
	m_StaticInterval.MoveWindow(&rect) ;*/

	GetDlgItem(IDC_STATIC_CHART)->GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	rect.right = rect.right + offX ;
	rect.bottom = rect.bottom + offY ;
	GetDlgItem(IDC_STATIC_CHART)->MoveWindow(&rect) ;


	m_Chart.GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	rect.right = rect.right + offX ;
	rect.bottom = rect.bottom + offY ;
	m_Chart.MoveWindow(&rect) ;

	m_OldX = cx ;
	m_OldY = cy ;

	GetWindowRect(&m_CurrentPosition) ;
	m_TermPlanDoc->UpdateAllViews(NULL);
}
BEGIN_MESSAGE_MAP(CBaseCallOutDlg, CDragDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(IDC_BUTTON_CLOSE,OnClose)
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave) 
	ON_STN_CLICKED(IDC_STATIC_STARTVAL, OnStnClickedStaticStartval)
	ON_STN_CLICKED(IDC_STATIC_TITLE, OnStnClickedStaticTitle)
END_MESSAGE_MAP()


void CBaseCallOutDlg::OnClose()
{
	CDragDialog::OnOK() ;
	m_isclose = TRUE ;
}
void CBaseCallOutDlg::OnCancel()
{
	CDragDialog::OnCancel() ;
	m_isclose = TRUE ;
}
void CBaseCallOutDlg::OnOK()
{
	CDragDialog::OnOK() ;
	m_isclose = TRUE ;
}
 HBRUSH CBaseCallOutDlg::OnCtlColor(
						  CDC* pDC,
						  CWnd* pWnd,
						  UINT nCtlColor 
						  )
{
	HBRUSH hbr = CDragDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// Are we painting the IDC_MYSTATIC control? We can use
	// CWnd::GetDlgCtrlID() to perform the most efficient test.
	if(nCtlColor == CTLCOLOR_DLG || nCtlColor == CTLCOLOR_STATIC)
	{
		if( nCtlColor == CTLCOLOR_STATIC)
			pDC->SetBkMode(TRANSPARENT);
		return m_brush ;
	}
	return hbr ;
}

int CBaseCallOutDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( CDragDialog::OnCreate(lpCreateStruct) ==- 1 )
		return -1 ;
	CRect rect;
	GetWindowRect(&rect);	
	m_Chart.Create(WS_CHILDWINDOW | WS_VISIBLE, rect,this,1);
	return  1;
}
BOOL CBaseCallOutDlg::OnInitDialog()
{
	CDragDialog::OnInitDialog() ;
	
	m_brush.CreateSolidBrush(RGB(255,   255,   225));
	m_TitileBush.CreateSolidBrush(RGB(37,22,238)) ;
	m_StaticStartTime.SetWindowText(m_StartTime.printTime()) ;
	m_StaticEndTime.SetWindowText(m_EndTime.printTime()) ;
	m_StaticInterval.SetWindowText(m_Interval.printTime()) ;
	m_StaticProcssor.SetWindowText(m_Processor) ;

	GetDlgItem(IDC_STATIC_TITLE)->SetWindowText(m_Caption) ;

	m_CloseButton.SetIcon( LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE( IDI_ICON_CLOSE)) );
	m_CloseButton.ShowWindow(SW_HIDE) ;


	CRect rect ;

	GetWindowRect(&rect) ;
	int m_nScreenX = GetSystemMetrics(SM_CXSCREEN); // You get the width of the users monitor
	int m_nScreenY = GetSystemMetrics(SM_CYSCREEN); // You get the height of the users monitor

	int ShowX = m_nScreenX - rect.Width() ;
	int showY = m_nScreenY - rect.Height() ;

	SetWindowPos(&CWnd::wndTop,m_DlgPosition.x,m_DlgPosition.y,rect.Width(),rect.Height(),SWP_SHOWWINDOW | SWP_NOSIZE) ;
	
	GetDlgItem(IDC_STATIC_CHART)->ShowWindow(SW_HIDE) ;
	GetDlgItem(IDC_STATIC_CHART)->GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	m_Chart.MoveWindow(&rect,TRUE) ;

	
	/*SetResize(IDC_STATIC_PROCESSOR,SZ_TOP_LEFT,SZ_TOP_LEFT) ;
	SetResize(IDC_STATIC_PRCOVAL,SZ_TOP_LEFT,SZ_TOP_CENTER) ;
	SetResize(IDC_BUTTON_CLOSE,SZ_TOP_RIGHT,SZ_TOP_RIGHT) ;

	SetResize(IDC_STATIC_STARTTIME,SZ_TOP_LEFT,SZ_TOP_LEFT) ;
	SetResize(IDC_STATIC_STARTVAL,SZ_TOP_LEFT,SZ_TOP_CENTER) ;

	SetResize(IDC_STATIC_ENDTIME,SZ_TOP_CENTER,SZ_TOP_CENTER) ;
	SetResize(IDC_STATIC_ENDVAL,SZ_TOP_CENTER,SZ_TOP_CENTER) ;

	SetResize(IDC_STATIC_INTERVALNAME,SZ_TOP_CENTER,SZ_TOP_CENTER) ;
	SetResize(IDC_STATIC_INTERVAL,SZ_TOP_CENTER,SZ_TOP_RIGHT) ;
	SetResize(IDC_STATIC_TIMENAME,SZ_TOP_CENTER,SZ_TOP_CENTER);
	SetResize(IDC_STATIC_TIME,SZ_TOP_CENTER,SZ_TOP_RIGHT);

	SetResize(m_Chart.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;	
	SetResize(IDC_STATIC_CHART,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;	*/
	GetWindowRect(&m_CurrentPosition) ;
	m_TermPlanDoc->UpdateAllViews(NULL);

	return TRUE ;

}

void CBaseCallOutDlg::UpdateCurrentTime( const ElapsedTime& _time )
{
	m_CurrentTime = _time ;
	//m_StaticCurrentTime.SetWindowText(_time.printTime()) ;
}

void CBaseCallOutDlg::OnMouseMove( UINT nFlags, CPoint point )
{
	
	if(!m_bMouseTracking)   
	{   
		TRACKMOUSEEVENT tme;   
		tme.cbSize=sizeof(TRACKMOUSEEVENT);   
		tme.dwFlags=TME_LEAVE;   
		tme.hwndTrack=this->m_hWnd;   

		if(::_TrackMouseEvent(&tme))   
		{   
			m_bMouseTracking=TRUE;   
		}   

		CRect rect ;
		GetWindowRect(&rect) ;
		ScreenToClient(&rect) ;
		if(rect.PtInRect(point))
		{
			m_CloseButton.ShowWindow(SW_SHOW) ;
		}
	}
	CDragDialog::OnMouseMove(nFlags,point) ;

	if(m_bDrag)
	{
		GetWindowRect(&m_CurrentPosition) ;
		m_TermPlanDoc->UpdateAllViews(NULL);
	}
}
LRESULT CBaseCallOutDlg::OnMouseLeave(WPARAM wParam,LPARAM lParam)   
{
	POINT  point ;
	GetCursorPos(&point) ;
	CRect rect ;
	m_CloseButton.GetWindowRect(&rect) ;
	if(rect.PtInRect(point))
		m_CloseButton.ShowWindow(SW_SHOW) ;
	else
		m_CloseButton.ShowWindow(SW_HIDE) ;
	m_bMouseTracking = FALSE ;
	return TRUE ;
}

LRESULT CBaseCallOutDlg::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch(message)
	{
	case UPDATE_TIME:
		m_StaticCurrentTime.SetWindowText(m_CurrentTime.printTime()) ;
		break ;
	case UPDATE_CHART:
		GetChartCtrl()->UpdateData() ;
		GetChartCtrl()->Invalidate(FALSE) ;
		break ;
	}
	return CDragDialog::DefWindowProc(message,wParam,lParam) ;
}
// CBaseCallOutDlg message handlers

void CBaseCallOutDlg::OnStnClickedStaticStartval()
{
	// TODO: Add your control notification handler code here
}

void CBaseCallOutDlg::OnStnClickedStaticTitle()
{
	// TODO: Add your control notification handler code here
}
