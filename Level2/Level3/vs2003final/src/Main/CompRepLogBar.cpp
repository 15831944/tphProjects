// CompRepLogBar.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "CompRepLogBar.h"
#include "../compare/ComparativeProject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "MainFrm.h"

/////////////////////////////////////////////////////////////////////////////
// CCompRepLogBar

CCompRepLogBar::CCompRepLogBar()
{
	//{{AFX_DATA_INIT(CCompRepLogBar)
	m_strLogText = _T("");
	//}}AFX_DATA_INIT
	m_bLButtonDown = FALSE;

	m_pProj = NULL;
}

CCompRepLogBar::~CCompRepLogBar()
{
	//if(m_vLogText.size() != 0)
	m_vLogText.clear();

	/*if(m_Font.m_hObject)
	{
		m_Font.DeleteObject();
	}*/
}

void CCompRepLogBar::DoDataExchange(CDataExchange* pDX) 
{
	// TODO: Add your specialized code here and/or call the base class
	CSizingControlBarG::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCompRepLogBar)
	DDX_Text(pDX, IDC_LOG_RUNLOGS, m_strLogText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCompRepLogBar, CSizingControlBarG)
	//{{AFX_MSG_MAP(CCompRepLogBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(IDC_LOG_EXPORT, OnLogExport)
	ON_COMMAND(IDC_LOG_PRINT, OnLogPrint)
	ON_COMMAND(IDC_LOG_CONTROL, OnLogControl)
	ON_COMMAND(IDC_LOG_CANCEL, OnLogCancel)
	ON_WM_LBUTTONUP()
	ON_WM_SIZING()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONUP()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_COMPARE_RUN, OnComparativeRun)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCompRepLogBar message handlers

int CCompRepLogBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSizingControlBarG::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
    m_Font.DeleteObject(); 
	m_Font.CreatePointFont(80, _T ("MS Sans Serif"));
	SetFont(&m_Font);

	m_edtRunLogs.Create(ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN 
		| WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | WS_VSCROLL,
        CRect(10, 10, 500, 100), this, IDC_LOG_RUNLOGS);
	m_edtRunLogs.SetFont(&m_Font);
   
    m_btnExport.Create(_T("Export"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
		CRect(110,110,200,130), this, IDC_LOG_EXPORT);
	m_btnExport.SetFont(&m_Font);
	m_btnExport.EnableWindow(FALSE);

	m_btnPrint.Create(_T("Print"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 
		CRect(210,110,300,130), this, IDC_LOG_PRINT);
	m_btnPrint.SetFont(&m_Font);
	m_btnPrint.EnableWindow(FALSE);

	m_btnControl.Create(_T("Pause"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 
		CRect(310,110,400,130), this, IDC_LOG_CONTROL);
	m_btnControl.SetFont(&m_Font);
	m_btnControl.EnableWindow(FALSE);

	m_btnCancel.Create(_T("Cancel"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 
		CRect(410,110,500,130), this, IDC_LOG_CANCEL);
	m_btnCancel.SetFont(&m_Font);

	m_prgProgress.Create(WS_CHILD|WS_VISIBLE|PBS_SMOOTH, 
		CRect(10,140,600,160), this, IDC_LOG_PROGRESS);
	m_prgProgress.SetFont(&m_Font);
	m_prgProgress.SendMessage(PBM_SETBARCOLOR, 0, (LPARAM) RGB(153,0, 0));

	m_prgProgress.SetRange(0, 100);

	return 0;
}


void CCompRepLogBar::OnSize(UINT nType, int cx, int cy) 
{
	CSizingControlBarG::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CRect rc(0, 0, cx, cy);
    GetClientRect(&rc);
	int nleft, ntop, nWidth, nHeight;
	static int iCount = 5;

	/*
	iCount = 0;
	
	nleft = 5;
	ntop = rc.bottom - 25;
	nWidth = rc.right - 15;
	nHeight = 20;
	m_prgProgress.MoveWindow(nleft, ntop, nWidth, nHeight);
	
	ntop -= (nHeight+5);
	nleft = rc.right - 25 - 430;
	nWidth = 100;
	nHeight = 20;
	m_btnExport.MoveWindow(nleft, ntop, nWidth, nHeight);
	
	nleft += (nWidth+10);
	m_btnPrint.MoveWindow(nleft, ntop, nWidth, nHeight);
	
	nleft += (nWidth+10);
	m_btnControl.MoveWindow(nleft, ntop, nWidth, nHeight);
	
	nleft += (nWidth+10);
	m_btnCancel.MoveWindow(nleft, ntop, nWidth, nHeight);
	
	nleft = 5;
	ntop = 5;
	nWidth = rc.right - 15;
	nHeight = rc.bottom - 60;
	m_edtRunLogs.MoveWindow(nleft, ntop, nWidth, nHeight);*/


	if(rc.bottom >= 0)// || m_bLButtonDown)
	{
		//iCount = 6;

		nleft = 5;
		ntop = rc.bottom - 25;
		nWidth = rc.right - 15;
		nHeight = 20;
		m_prgProgress.MoveWindow(nleft, ntop, nWidth, nHeight);

		ntop -= (nHeight+5);
		nleft = rc.right - 25 - 430;
		nWidth = 100;
		nHeight = 20;
		m_btnExport.MoveWindow(nleft, ntop, nWidth, nHeight);

		nleft += (nWidth+10);
		m_btnPrint.MoveWindow(nleft, ntop, nWidth, nHeight);

		nleft += (nWidth+10);
		m_btnControl.MoveWindow(nleft, ntop, nWidth, nHeight);

		nleft += (nWidth+10);
		m_btnCancel.MoveWindow(nleft, ntop, nWidth, nHeight);

		nleft = 5;
		ntop = 5;
		nWidth = rc.right - 15;
		nHeight = rc.bottom - 60;
		m_edtRunLogs.MoveWindow(nleft, ntop, nWidth, nHeight);

	}


	CRect rcWnd;
	
	GetWindowRect(&rcWnd);
				
	GetClientRect(&rc);
	
	((CMainFrame*)AfxGetMainWnd())->ChangeSize(rcWnd, rc, m_nIndex);

	
	
	//int nleft, ntop, nWidth, nHeight;
	//static int iCount = 5;
	//		if((rc.bottom >= 0 && iCount++ == 5))// || m_bLButtonDown)
}

void CCompRepLogBar::OnLogExport()
{
	// TODO: Add your command handler code here
	AddLogText("OnLogExport");
	SetProgressPos(33);
}

void CCompRepLogBar::OnLogPrint()
{
	// TODO: Add your command handler code here
	AddLogText("OnLogPrint");
	SetProgressPos(67);
}

void CCompRepLogBar::OnLogControl()
{
	// TODO: Add your command handler code here
	AddLogText("OnLogControl", 1);
	SetProgressPos(100);
	CString str;
	GetDlgItemText(IDC_LOG_CONTROL, str);
	if(str == "Pause")
		str = "Continue";
	else
		str = "Pause";
	SetDlgItemText(IDC_LOG_CONTROL, str);
}

void CCompRepLogBar::OnLogCancel()
{
	// TODO: Add your command handler code here
	//SetLogText("OnLogCancel");
	if(m_pProj == NULL)
		return ;
	ASSERT(m_pProj);
	m_pProj->Stop();
}

void CCompRepLogBar::SetProgressPos(int nPos)
{
	ASSERT(nPos >= 0 && nPos <= 100);

	m_prgProgress.SetPos(nPos);
	UpdateWindow();
}

int CCompRepLogBar::GetProgressPos()
{
	return m_prgProgress.GetPos();
}

void CCompRepLogBar::SetLogText(const CString &strText)
{
	if(strText.IsEmpty())
		return ;
	m_vLogText.clear();
	ASSERT(m_vLogText.size() == 0);
	m_vLogText.push_back(strText);

	ShowLogText();
}

void CCompRepLogBar::AddLogText(const CString& strText, UINT nFlag)
{
	if(strText == "")
		return ;

	if(nFlag == 0)
	{
		m_vLogText.push_back(strText);
	}
	else
	{
		m_vLogText.insert(m_vLogText.begin(), strText);
	}

	ShowLogText();
	UpdateWindow();
}

void CCompRepLogBar::ShowLogText()
{
	m_strLogText = "";
	for(int i=0; i<static_cast<int>(m_vLogText.size()); i++)
	{
		m_strLogText += m_vLogText[i] + "\r\n";
	}

	SetDlgItemText(IDC_LOG_RUNLOGS, m_strLogText);

	if(i >= 5)
		m_edtRunLogs.LineScroll(i);
}

void CCompRepLogBar::DeleteLogText()
{
	m_vLogText.clear();
	ShowLogText();
}

void CCompRepLogBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bLButtonDown = FALSE;

	SetTimer(0, 100, NULL);

	CSizingControlBarG::OnLButtonUp(nFlags, point);
}

void CCompRepLogBar::OnSizing(UINT fwSide, LPRECT pRect) 
{
	CSizingControlBarG::OnSizing(fwSide, pRect);
	
	// TODO: Add your message handler code here
	
}

void CCompRepLogBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CSizingControlBarG::OnWindowPosChanged(lpwndpos);
	
	// TODO: Add your message handler code here
	// TRACE("\n%d%d%d%d", lpwndpos->x, lpwndpos->y, lpwndpos->cx, lpwndpos->cy);
}

void CCompRepLogBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bLButtonDown = TRUE;

	CSizingControlBarG::OnLButtonDown(nFlags, point);
}

void CCompRepLogBar::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	if(nIDEvent == 0)
	{
		KillTimer(0);
		
		/*
		CRect rc, rcWnd;

		GetWindowRect(&rcWnd);
				
		GetClientRect(&rc);

		((CMainFrame*)AfxGetMainWnd())->ChangeSize(rcWnd, rc, m_nIndex);

		int nleft, ntop, nWidth, nHeight;
		static int iCount = 5;
//		if((rc.bottom >= 0 && iCount++ == 5))// || m_bLButtonDown)
		{
			iCount = 0;

			nleft = 5;
			ntop = rc.bottom - 25;
			nWidth = rc.right - 15;
			nHeight = 20;
			m_prgProgress.MoveWindow(nleft, ntop, nWidth, nHeight);

			ntop -= (nHeight+5);
			nleft = rc.right - 25 - 430;
			nWidth = 100;
			nHeight = 20;
			m_btnExport.MoveWindow(nleft, ntop, nWidth, nHeight);

			nleft += (nWidth+10);
			m_btnPrint.MoveWindow(nleft, ntop, nWidth, nHeight);

			nleft += (nWidth+10);
			m_btnControl.MoveWindow(nleft, ntop, nWidth, nHeight);

			nleft += (nWidth+10);
			m_btnCancel.MoveWindow(nleft, ntop, nWidth, nHeight);

			nleft = 5;
			ntop = 5;
			nWidth = rc.right - 15;
			nHeight = rc.bottom - 60;
			m_edtRunLogs.MoveWindow(nleft, ntop, nWidth, nHeight);
		}*/

		UpdateWindow();
	}
	CSizingControlBarG::OnTimer(nIDEvent);
}

void CCompRepLogBar::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CSizingControlBarG::OnClose();
}

UINT CCompRepLogBar::OnNcHitTest(CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CSizingControlBarG::OnNcHitTest(point);
}

void CCompRepLogBar::OnNcLButtonUp(UINT nHitTest, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (nHitTest == HTCLOSE)
		((CMainFrame*)AfxGetMainWnd())->ChangeSize(CRect(0,0,0,0), CRect(0,0,0,0), m_nIndex);

	CSizingControlBarG::OnNcLButtonUp(nHitTest, point);
}

void CCompRepLogBar::SetProgressRange(int nStart, int nEnd)
{
	ASSERT(nStart < nEnd);
	m_prgProgress.SetRange(nStart, nEnd);
}

LRESULT CCompRepLogBar::OnComparativeRun(WPARAM wParam, LPARAM lParam)
{
	MSG msg;
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(!IsDialogMessage(&msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);  
      }
    }
	
	return 0;
}