
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "CustIE.h"

#include "MainFrm.h"

#include "CustIETools.h"

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_USER_INPUT_DONE, &CMainFrame::OnUserInputDone)
	ON_MESSAGE(WM_USER_DELAY_RELEASE_LOCK, &CMainFrame::OnUserDelayReleaseLock)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_uintStartTime = (unsigned int)time(NULL);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	
	SetTimer(TIMER_ID_OVERTIME_CHECK, 1000, NULL);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (cs.hMenu != NULL)
	{
		::DestroyMenu(cs.hMenu);      // delete menu if loaded
		cs.hMenu = NULL;              // no menu for this window
	}

	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style &= ~WS_MINIMIZEBOX;
	cs.style &= ~WS_MAXIMIZEBOX;

	return TRUE;
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	wchar_t szTitle [100] = L"";
	wsprintf(szTitle, L"LoginBot-[%u.%u]", (unsigned int)time(NULL), GetTickCount());
	SetWindowTextW(szTitle);
	return;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers



afx_msg LRESULT CMainFrame::OnUserInputDone(WPARAM wParam, LPARAM lParam)
{
	if (theApp.MinimizeWindow()){
		ShowWindow(SW_MINIMIZE);
	}
	if (TRUE){
		SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
	}

	if (TRUE){
		PUTLOG("* Notify KeyInput done");
		CCustIETools::SetKeyInputDone();

		PUTLOG("* Release KeyInput Lock, TID: %u", GetCurrentThreadId());
		CCustIETools::ReleaseKeyInputLock();
	}else{
		PUTLOG("* Delay for 500ms to release lock");
		CCustIETools::DelayWindowMessage(WM_USER_DELAY_RELEASE_LOCK, 500);
	}
	
	return 0;
}


afx_msg LRESULT CMainFrame::OnUserDelayReleaseLock(WPARAM wParam, LPARAM lParam)
{
	PUTLOG("* Notify KeyInput done");

	CCustIETools::SetKeyInputDone();

	PUTLOG("* Release KeyInput Lock");

	CCustIETools::ReleaseKeyInputLock();

	return 0;
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == TIMER_ID_OVERTIME_CHECK){
		PUTLOG("* Overtime check");
		unsigned int uintNow = (unsigned int)time(NULL);
		unsigned int nOverTime = theApp.GetDefaultTimeout(); 
		if (theApp.m_userConfig.isMember("overtime")){
			nOverTime = theApp.m_userConfig["overtime"].asUInt();
			if (nOverTime == 0){
				nOverTime = theApp.GetDefaultTimeout(); // default
			}
		}
		if (uintNow - m_uintStartTime >= nOverTime){
			PUTLOG("* LoginBot is overtime, report and quit");
			CCustIETools::StatusReport(theApp.m_userConfig, StatusCode(WAIT_OUTTIME), "{}");
		}
	}

	CFrameWnd::OnTimer(nIDEvent);
}


void CMainFrame::ActivateFrame(int nCmdShow)
{
	// TODO: Add your specialized code here and/or call the base class
	nCmdShow = SW_MINIMIZE;
	CFrameWnd::ActivateFrame(nCmdShow);
}

