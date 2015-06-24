// DlgWalkthrough.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"

#include "DlgWalkthrough.h"

#include "3DView.h"
#include "AnimationTimeManager.h"
#include "ChildFrm.h"
#include "DlgMovieWriter.h"
#include "Movie.h"
#include "TermPlanDoc.h"
#include "AnimationData.h"

#include "common\WinMsg.h"
#include "results\EventLogBufManager.h"
#include "ModelessDlgManager.h"
#include "AnimationManager.h"
#include "Inputs\Walkthroughs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


inline
static BOOL PumpMessages() //returns true if ESC is pressed
{
	MSG msg;
	BOOL ret = FALSE;
	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if(msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
			ret = TRUE;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////////////////////
// CDlgWalkthrough dialog


CDlgWalkthrough::CDlgWalkthrough(CTermPlanDoc* pTPDoc, C3DView* pView, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgWalkthrough::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWalkthrough)
	m_endTime = COleDateTime::GetCurrentTime();
	m_startTime = COleDateTime::GetCurrentTime();
	//}}AFX_DATA_INIT
	m_bInitialized = FALSE;
	m_pDoc = pTPDoc;
	m_pView = pView;
	m_nSelectedPaxWalkthrough = -1;
	m_nSelectedFlightWalkthrough = -1;
	m_pParent = pParent;
	m_nSelectedPaxID = 0;
	m_nMinTime = 0;
	m_nMaxTime = 0;
}


void CDlgWalkthrough::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWalkthrough)
	DDX_Control(pDX, IDC_EDIT_PAX, m_editPax);
	DDX_Control(pDX, IDC_LIST_WALKTHROUGHS, m_lstWalkthrough);
	DDX_Control(pDX, IDC_COMBO_VIEWPOINT, m_cmbViewpoint);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_endTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START, m_startTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWalkthrough, CXTResizeDialog)
	//{{AFX_MSG_MAP(CDlgWalkthrough)
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_COMBO_VIEWPOINT, OnSelchangeComboViewpoint)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_START, OnDatetimepickerChangeStart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_END, OnDatetimepickerChangeEnd)
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_WALKTHROUGHS, OnItemChanged)
	ON_COMMAND(ID_BUTTON_NEWWT, OnAddMovie)
	ON_COMMAND(ID_BUTTON_DELETEWT, OnDeleteMovie)
	ON_COMMAND(ID_BUTTON_RECORDWT, OnRecordMovie)
	ON_COMMAND(ID_BUTTON_PREVIEWWALKTHROUGH, OnPreviewMovie)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_WALKTHROUGHS, OnGetDispInfo)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_WALKTHROUGHS, OnEndLabelEdit)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWalkthrough message handlers

int CDlgWalkthrough::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_WALKTHROUGHTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	
	return 0;
}

BOOL CDlgWalkthrough::OnInitDialog() 
{
	CXTResizeDialog::OnInitDialog();
	
	m_bInitialized = TRUE;

	m_nSelectedPaxWalkthrough = -1;
	m_nSelectedFlightWalkthrough = -1;
	m_bNewMovie = FALSE;

	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DELETEWT, FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_RECORDWT, FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_PREVIEWWALKTHROUGH, FALSE);
	m_ToolBar.GetToolBarCtrl().HideButton(ID_BUTTON_PREVIEWMOVIEWITHLOGS);
	CRect rc;
	GetDlgItem(IDC_STATIC_TOOLBARCONTENTER)->GetWindowRect(rc);
	ScreenToClient(rc);
	m_ToolBar.MoveWindow(rc);

	CRect rcClient;
	GetClientRect(&rcClient);

	m_lstWalkthrough.InsertColumn(0, "Name", LVCFMT_LEFT, rcClient.Width()/2);

	CWnd* pWnd = GetDlgItem(IDC_STATIC_PAX);
	if(EnvMode_Terminal == m_pDoc->m_systemMode) {
		m_cmbViewpoint.AddString("First-Person View");
		m_cmbViewpoint.AddString("Third-Person View");
		pWnd->SetWindowText(_T("Passenger"));
	}
	else if(EnvMode_AirSide == m_pDoc->m_systemMode) {
		m_cmbViewpoint.AddString("Pilot View");
		m_cmbViewpoint.AddString("Follow");
		pWnd->SetWindowText(_T("Aircraft"));
	}
	else
		ASSERT(FALSE);
	
	m_cmbViewpoint.SetCurSel(1);

	UpdateList();

	SetResize(IDC_STATIC_BARFRAME, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(m_ToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LIST_WALKTHROUGHS, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	SetResize(IDC_STATIC_FRAME2, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_PAX, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_EDIT_PAX, SZ_BOTTOM_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_VIEWPOINT, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_COMBO_VIEWPOINT, SZ_BOTTOM_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_STARTTIME, SZ_BOTTOM_CENTER, SZ_BOTTOM_CENTER);
	SetResize(IDC_DATETIMEPICKER_START, SZ_BOTTOM_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_ENDTIME, SZ_BOTTOM_CENTER, SZ_BOTTOM_CENTER);
	SetResize(IDC_DATETIMEPICKER_END, SZ_BOTTOM_CENTER, SZ_BOTTOM_RIGHT);

	SetResize(IDC_STATIC_ESC_MSG, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	
	SetResize(IDOK, SZ_BOTTOM_CENTER, SZ_BOTTOM_CENTER);
	SetResize(IDCANCEL, SZ_BOTTOM_CENTER, SZ_BOTTOM_CENTER);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWalkthrough::UpdatePAXData()
{
	if(EnvMode_Terminal == m_pDoc->m_systemMode)
	{
		if(m_nSelectedPaxWalkthrough==-1) {
			EnableControls(FALSE);
		}
		else 
		{
			EnableControls(TRUE);
		    const CWalkthrough* pWT = m_pDoc->m_walkthroughs->GetWalkthrough(m_nSelectedPaxWalkthrough);
		    m_nSelectedPaxID = pWT->GetPaxID();

		    //if (m_nSelectedPaxID <= 0)	return;
		    if (m_nSelectedPaxID < 0)	return;
		    m_editPax.SetWindowText(pWT->GetPaxIDDesc());
		    SetDataFromID(m_nSelectedPaxID);

		    bool bAbsDate;
		    COleDateTime outDate;
		    int nDateIdx;
		    COleDateTime outTime;

		    ElapsedTime eStartTime( m_nMinTime / TimePrecision );
		    m_pDoc->m_animData.m_startDate.GetDateTime( eStartTime, bAbsDate, outDate, nDateIdx, outTime );
		    m_startTime = outTime;

		    ElapsedTime eEndTime( m_nMaxTime / TimePrecision );
		    m_pDoc->m_animData.m_startDate.GetDateTime( eEndTime, bAbsDate, outDate, nDateIdx, outTime );
		    m_endTime = outTime;

     		m_cmbViewpoint.SetCurSel((int)m_pDoc->m_walkthroughs->GetWalkthrough(m_nSelectedPaxWalkthrough)->GetViewpoint());

	    	UpdateData(FALSE);
	     }
	}
	else if (EnvMode_AirSide == m_pDoc->m_systemMode)
	{
		if (m_nSelectedFlightWalkthrough==-1)
		{
			EnableControls(FALSE);
		}
		else
		{
			EnableControls(TRUE);
			const CWalkthrough* pWT = m_pDoc->m_walkthroughsFlight->GetWalkthrough(m_nSelectedFlightWalkthrough);
			m_nSelectedPaxID = pWT->GetPaxID();

			if(m_nSelectedPaxID < 0)   return;
			m_editPax.SetWindowText(pWT->GetPaxIDDesc());
			int tempPaxID = (m_nSelectedPaxID<C3DView::SEL_FLIGHT_OFFSET)?(C3DView::SEL_FLIGHT_OFFSET):m_nSelectedPaxID;
			SetDataFromID(tempPaxID);

			bool bAbsDate;
			COleDateTime outDate;
			int nDateIdx;
			COleDateTime outTime;

			ElapsedTime eStartTime( m_nMinTime / TimePrecision );
			m_pDoc->m_animData.m_startDate.GetDateTime( eStartTime, bAbsDate, outDate, nDateIdx, outTime );
			m_startTime = outTime;

			ElapsedTime eEndTime( m_nMaxTime / TimePrecision );
			m_pDoc->m_animData.m_startDate.GetDateTime( eEndTime, bAbsDate, outDate, nDateIdx, outTime );
			m_endTime = outTime;

			m_cmbViewpoint.SetCurSel((int)m_pDoc->m_walkthroughsFlight->GetWalkthrough(m_nSelectedFlightWalkthrough)->GetViewpoint());

			UpdateData(FALSE);
		}
	}
	else{
		ASSERT(FALSE);
	}
}

void CDlgWalkthrough::SelectPAX(UINT nPaxID, const CString& sDesc)
{
	if(EnvMode_Terminal == m_pDoc->m_systemMode)
	{
		if(m_nSelectedPaxWalkthrough!=-1) 
		{
			m_pDoc->m_walkthroughs->GetWalkthrough(m_nSelectedPaxWalkthrough)->SetPaxID(nPaxID);
			m_pDoc->m_walkthroughs->GetWalkthrough(m_nSelectedPaxWalkthrough)->SetPaxIDDesc(sDesc);

			m_nSelectedPaxID = nPaxID;

			SetDataFromID(nPaxID);

			m_pDoc->m_walkthroughs->GetWalkthrough(m_nSelectedPaxWalkthrough)->SetStartTime(m_nMinTime);
			m_pDoc->m_walkthroughs->GetWalkthrough(m_nSelectedPaxWalkthrough)->SetEndTime(m_nMaxTime);

			UpdatePAXData();		
		}
	}
	else if(EnvMode_AirSide == m_pDoc->m_systemMode)
	{
		if(m_nSelectedFlightWalkthrough!=-1) {
			m_pDoc->m_walkthroughsFlight->GetWalkthrough(m_nSelectedFlightWalkthrough)->SetPaxID(nPaxID);
			m_pDoc->m_walkthroughsFlight->GetWalkthrough(m_nSelectedFlightWalkthrough)->SetPaxIDDesc(sDesc);

			m_nSelectedPaxID = nPaxID;

			SetDataFromID(nPaxID);

			m_pDoc->m_walkthroughsFlight->GetWalkthrough(m_nSelectedFlightWalkthrough)->SetStartTime(m_nMinTime);
			m_pDoc->m_walkthroughsFlight->GetWalkthrough(m_nSelectedFlightWalkthrough)->SetEndTime(m_nMaxTime);

			UpdatePAXData();		
		}
	}
	else{
		ASSERT(FALSE);
	}

}

void CDlgWalkthrough::SetDataFromID(UINT nPaxID)
{
	m_nMinTime = m_nMaxTime = 0;
	if(EnvMode_Terminal == m_pDoc->m_systemMode) {	
		MobLogEntry element;
		m_pDoc->GetTerminal().paxLog->getItem(element, nPaxID);
		m_nMinTime = element.getEntryTime();
		m_nMaxTime = element.getExitTime();
	}
	else if(EnvMode_AirSide == m_pDoc->m_systemMode) {
		AirsideFlightLogEntry element;
		m_pDoc->GetAirsideSimLogs().m_airsideFlightLog.getItem(element, nPaxID-C3DView::SEL_FLIGHT_OFFSET);
		m_nMinTime = element.getEntryTime();
		m_nMaxTime = element.getExitTime();
	}
	else {
		ASSERT(FALSE);
	}
}


void CDlgWalkthrough::OnSelchangeComboViewpoint() 
{
	int idx = m_cmbViewpoint.GetCurSel();
	ASSERT(idx!=-1);
	if(EnvMode_Terminal == m_pDoc->m_systemMode)
	{
		m_pDoc->m_walkthroughs->GetWalkthrough(m_nSelectedPaxWalkthrough)->SetViewpoint((CWalkthrough::VIEWPOINT)idx);
	}
	else if(EnvMode_AirSide == m_pDoc->m_systemMode)
	{
    	m_pDoc->m_walkthroughsFlight->GetWalkthrough(m_nSelectedFlightWalkthrough)->SetViewpoint((CWalkthrough::VIEWPOINT)idx);
	}
	else
	{
		ASSERT(0);
	}

}


void CDlgWalkthrough::EnableControls(BOOL bEnable)
{
	m_editPax.EnableWindow(bEnable);
	m_cmbViewpoint.EnableWindow(bEnable);
	CWnd* pWnd = GetDlgItem(IDC_STATIC_PAX);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_STATIC_VIEWPOINT);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_DATETIMEPICKER_START);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_DATETIMEPICKER_END);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_STATIC_STARTTIME);
	pWnd->EnableWindow(bEnable);
	pWnd = GetDlgItem(IDC_STATIC_ENDTIME);
	pWnd->EnableWindow(bEnable);
}

void CDlgWalkthrough::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;
	
	if ((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
		return;

	if(pnmv->uOldState & LVIS_SELECTED)
	{
		if(!(pnmv->uNewState & LVIS_SELECTED))
		{
			// TRACE("Unselected Item %d...\n", pnmv->iItem);
		}
	}
	else if(pnmv->uNewState & LVIS_SELECTED)
	{
		//a new item has been selected
		// TRACE("Selected Item %d...\n", pnmv->iItem);
		if(EnvMode_Terminal == m_pDoc->m_systemMode)
		{
			m_nSelectedPaxWalkthrough = (int) m_lstWalkthrough.GetItemData(pnmv->iItem);
		}
		else if(EnvMode_AirSide == m_pDoc->m_systemMode)
		{
			m_nSelectedFlightWalkthrough = (int) m_lstWalkthrough.GetItemData(pnmv->iItem);
		}
		else
		{
			ASSERT(0);
		}
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DELETEWT, TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_RECORDWT, TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_PREVIEWWALKTHROUGH, TRUE);
	}
	else
		ASSERT(0);

	UpdatePAXData();

}

void CDlgWalkthrough::UpdateList()
{
	int nMovieCount=0,i;
	m_lstWalkthrough.DeleteAllItems();

	if(EnvMode_Terminal == m_pDoc->m_systemMode)
	{
		nMovieCount = m_pDoc->m_walkthroughs->GetWalkthroughCount();
		for(i=0; i<nMovieCount; i++) {
			m_lstWalkthrough.InsertItem(LVIF_TEXT | LVIF_PARAM, i, LPSTR_TEXTCALLBACK, 0, 0, 0, (LPARAM) i);
		}

		m_nSelectedPaxWalkthrough = -1;
	}
	else if(EnvMode_AirSide == m_pDoc->m_systemMode)
	{
		nMovieCount = m_pDoc->m_walkthroughsFlight->GetWalkthroughCount();
		for(i=0; i<nMovieCount; i++) {
			m_lstWalkthrough.InsertItem(LVIF_TEXT | LVIF_PARAM, i, LPSTR_TEXTCALLBACK, 0, 0, 0, (LPARAM) i);
		}

		m_nSelectedFlightWalkthrough = -1;
	}



	else
		ASSERT(0);

	UpdatePAXData();
}

void CDlgWalkthrough::OnAddMovie()
{
	int c=0;
	if(EnvMode_Terminal == m_pDoc->m_systemMode)
	{
		c = m_pDoc->m_walkthroughs->GetWalkthroughCount();
		m_pDoc->m_walkthroughs->AddWalkthrough(m_pDoc->m_systemMode, "undefined");
	}
	else if(EnvMode_AirSide == m_pDoc->m_systemMode)
	{
		c = m_pDoc->m_walkthroughsFlight->GetWalkthroughCount();
		m_pDoc->m_walkthroughsFlight->AddWalkthrough(m_pDoc->m_systemMode, "undefined");
	}
	else
	{
		ASSERT(FALSE);
	}

	m_bNewMovie = TRUE;
	m_lstWalkthrough.InsertItem(LVIF_TEXT | LVIF_PARAM, c, LPSTR_TEXTCALLBACK, 0, 0, 0, (LPARAM) c);
	m_lstWalkthrough.SetItemState(c, LVIS_SELECTED, LVIS_SELECTED);
	m_lstWalkthrough.SetFocus();
	m_lstWalkthrough.EditLabel(c);
}

void CDlgWalkthrough::OnDeleteMovie()
{
	if(EnvMode_Terminal == m_pDoc->m_systemMode)
	{
		ASSERT(m_nSelectedPaxWalkthrough != -1);
		m_pDoc->m_walkthroughs->DeleteWalkthrough(m_nSelectedPaxWalkthrough);
		UpdateList();
	}
	else if(EnvMode_AirSide == m_pDoc->m_systemMode)
	{
		ASSERT(m_nSelectedFlightWalkthrough != -1);
		m_pDoc->m_walkthroughsFlight->DeleteWalkthrough(m_nSelectedFlightWalkthrough);
		UpdateList();
	}
	else
	{
		ASSERT(FALSE);
	}

}

void CDlgWalkthrough::OnPreviewMovie()
{
	ANIMTIMEMGR->EndTimer();
	CMovie movie("temp");
	CWalkthrough* pWT;
	if(EnvMode_Terminal == m_pDoc->m_systemMode)
	{
		ASSERT(m_pDoc->m_walkthroughs->GetWalkthroughCount() > m_nSelectedPaxWalkthrough && 0 <= m_nSelectedPaxWalkthrough);
		pWT = m_pDoc->m_walkthroughs->GetWalkthrough(m_nSelectedPaxWalkthrough);
	}
	else if(EnvMode_AirSide == m_pDoc->m_systemMode)
	{
		ASSERT(m_pDoc->m_walkthroughsFlight->GetWalkthroughCount() > m_nSelectedFlightWalkthrough && 0 <= m_nSelectedFlightWalkthrough);
		pWT = m_pDoc->m_walkthroughsFlight->GetWalkthrough(m_nSelectedFlightWalkthrough);
	}
	else
		ASSERT(0);

	
	//if (pWT->GetPaxID() == 0)
	//{
	//	AfxMessageBox("Please select passenger from 3D View");
	//	return;
	//}

	CDlgMovieWriter::BuildMovieFromWalkthrough(pWT, m_pDoc, &movie);

	ShowWindow(SW_HIDE);

	BOOL bRequestCancel = FALSE;

	CMovie* pMovie = &movie;
	//let view highlight current pax
	m_pDoc->SetSelectPaxID(pWT->GetPaxID());
	CAnimationManager::PreviewMovie(pMovie,m_pDoc,&bRequestCancel);
		

	//int nKFCount = pMovie->GetKFCount();
	//if(nKFCount>0) {
	//	LARGE_INTEGER nPerfFreq, nThisPerfCount, nLastPerfCount;
	//	QueryPerformanceFrequency(&nPerfFreq);
	//	
	//	long nStartTime = pMovie->GetKFTime(0);
	//	long nEndTime = pMovie->GetKFTime(nKFCount-1);
	//	long nTime = nStartTime;
	//	

	//	QueryPerformanceCounter(&nLastPerfCount);

	//	while(nTime <= nEndTime) {
	//		
	//		pMovie->Interpolate(nTime, *(m_pView->GetCamera()));

	//		//set anim direction
	//		long nLastAnimTime = m_pDoc->m_animData.nLastAnimTime;
	//		m_pDoc->m_animData.nLastAnimTime = nTime;
	//		int nLastAnimDirection = m_pView->m_nAnimDirection;
	//		m_pView->m_nAnimDirection = nTime>nLastAnimTime?CEventLogBufManager::ANIMATION_FORWARD:CEventLogBufManager::ANIMATION_BACKWARD;
	//		if(nLastAnimDirection!=m_pView->m_nAnimDirection)
	//			m_pView->m_bAnimDirectionChanged=TRUE;

	//		//redraw view
	//		m_pView->Invalidate(FALSE);
	//		m_pView->UpdateWindow();
	//		CChildFrame* pFrame = (CChildFrame*)m_pView->GetParentFrame();
	//		pFrame->m_wndStatusBar.OnUpdateCmdUI(pFrame, FALSE);

	//		bRequestCancel = PumpMessages();

	//		if(bRequestCancel) {
	//			ShowWindow(SW_SHOW);
	//			return;
	//		}

	//		//calc real elapsed time since last nTime increment
	//		QueryPerformanceCounter(&nThisPerfCount);
	//		double dElapsedTime = static_cast<double>(nThisPerfCount.QuadPart - nLastPerfCount.QuadPart) / nPerfFreq.QuadPart;
	//		nLastPerfCount = nThisPerfCount;
	//		
	//		nTime += ((long)(dElapsedTime*10.0*pMovie->GetAnimSpeed()));
	//	}
	//}

	ShowWindow(SW_SHOW);	

	ANIMTIMEMGR->StartTimer();
}

void CDlgWalkthrough::OnRecordMovie()
{
	ANIMTIMEMGR->EndTimer();

	if(EnvMode_Terminal == m_pDoc->m_systemMode)
	{
	    CDlgMovieWriter dlg(m_pDoc, CDlgMovieWriter::WALKTHROUGH, m_nSelectedPaxWalkthrough);
		dlg.DoFakeModal();
	}
	else if(EnvMode_AirSide == m_pDoc->m_systemMode)
	{
		CDlgMovieWriter dlg(m_pDoc, CDlgMovieWriter::WALKTHROUGH, m_nSelectedFlightWalkthrough);
		dlg.DoFakeModal();
	}
	else
	{
		ASSERT(FALSE);
	}


	ANIMTIMEMGR->StartTimer();
}

void CDlgWalkthrough::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if(pDispInfo->item.mask & LVIF_TEXT) {
		int idx = (int) pDispInfo->item.lParam;
		switch(pDispInfo->item.iSubItem) {
		case 0:
			if(EnvMode_Terminal == m_pDoc->m_systemMode)
			{
				lstrcpy(pDispInfo->item.pszText, m_pDoc->m_walkthroughs->GetWalkthroughName(idx));
			}
			else if(EnvMode_AirSide == m_pDoc->m_systemMode)
			{
                lstrcpy(pDispInfo->item.pszText, m_pDoc->m_walkthroughsFlight->GetWalkthroughName(idx));
			}
			else
			{
				ASSERT(FALSE);
			}
			break;
		default:
			ASSERT(0);
			break;
		}
	}	
	
	*pResult = 0;

}

void CDlgWalkthrough::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	if(!pDispInfo->item.pszText) {
		if(m_bNewMovie) {
			//delete this new pdp set
			OnDeleteMovie();	
		}
		return;
	}

	m_bNewMovie=FALSE;

	int nIdx = pDispInfo->item.iItem;

	int idxMovie = (int) m_lstWalkthrough.GetItemData(nIdx);
	if(EnvMode_Terminal == m_pDoc->m_systemMode)
	{
		m_pDoc->m_walkthroughs->SetWalkthroughName(idxMovie, pDispInfo->item.pszText);
	}
	else if(EnvMode_AirSide == m_pDoc->m_systemMode)
	{
		m_pDoc->m_walkthroughsFlight->SetWalkthroughName(idxMovie, pDispInfo->item.pszText);
	}
	else
		ASSERT(0);

    m_lstWalkthrough.SetItemText(nIdx,0,pDispInfo->item.pszText);
	UpdatePAXData();

	*pResult =1;
}

void CDlgWalkthrough::PostNcDestroy() 
{
	CXTResizeDialog::PostNcDestroy();
	m_pDoc->m_pModelessDlgMan->DestroyDialogWalkThrough();
	//m_pParent->PostMessage(TPWM_DESTROY_DLGWALKTHROUGH,0,0);
}

void CDlgWalkthrough::OnOK() 
{
	if(UpdateData(TRUE))
    {
		if(EnvMode_Terminal == m_pDoc->m_systemMode)
		{
			m_pDoc->m_walkthroughs->saveDataSet(m_pDoc->m_ProjInfo.path, false);
		}
		else if(EnvMode_AirSide == m_pDoc->m_systemMode)
		{
			m_pDoc->m_walkthroughsFlight->saveDataSet(m_pDoc->m_ProjInfo.path, false);
		}
		else
		{
			ASSERT(FALSE);
		}

        DestroyWindow();
    }

}

void CDlgWalkthrough::OnCancel() 
{
	if(EnvMode_Terminal == m_pDoc->m_systemMode)
	{
		m_pDoc->m_walkthroughs->loadDataSet(m_pDoc->m_ProjInfo.path);
	}
	else if(EnvMode_AirSide == m_pDoc->m_systemMode)
	{
		m_pDoc->m_walkthroughsFlight->loadDataSet(m_pDoc->m_ProjInfo.path);
	}
	else
		ASSERT(0);

	DestroyWindow();
}

void CDlgWalkthrough::OnDatetimepickerChangeStart(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMDATETIMECHANGE lpChange = (LPNMDATETIMECHANGE)pNMHDR;
	UpdateData(TRUE);
	long nStartTime = m_startTime.GetHour()*360000+m_startTime.GetMinute()*6000+m_startTime.GetSecond()*100;
	long nEndTime = m_endTime.GetHour()*360000+m_endTime.GetMinute()*6000+m_endTime.GetSecond()*100;
	if(nStartTime<m_nMinTime)
		nStartTime = m_nMinTime;
	else if(nStartTime>=nEndTime)
		nStartTime=nEndTime-1;

	ElapsedTime t;
	t.setPrecisely(nStartTime);
	m_startTime.SetTime((int)t.asHours(), (int)t.asMinutes() % 60, (int)t.asSeconds() % 60 );
	UpdateData(FALSE);
	if(EnvMode_Terminal == m_pDoc->m_systemMode)
	{
		m_pDoc->m_walkthroughs->GetWalkthrough(m_nSelectedPaxWalkthrough)->SetStartTime(nStartTime);
	}
	else if(EnvMode_AirSide == m_pDoc->m_systemMode)
	{
		m_pDoc->m_walkthroughsFlight->GetWalkthrough(m_nSelectedFlightWalkthrough)->SetStartTime(nStartTime);
	}
	else
		ASSERT(0);

	*pResult = 0;
}

void CDlgWalkthrough::OnDatetimepickerChangeEnd(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMDATETIMECHANGE lpChange = (LPNMDATETIMECHANGE)pNMHDR;
	UpdateData(TRUE);
	long nStartTime = m_startTime.GetHour()*360000+m_startTime.GetMinute()*6000+m_startTime.GetSecond()*100;
	long nEndTime = m_endTime.GetHour()*360000+m_endTime.GetMinute()*6000+m_endTime.GetSecond()*100;
	if(nEndTime>m_nMaxTime)
		nEndTime = m_nMaxTime;
	else if(nEndTime<=nStartTime)
		nEndTime=nStartTime+1;

	ElapsedTime t;
	t.setPrecisely(nEndTime);
	m_endTime.SetTime((int)t.asHours(), (int)t.asMinutes() % 60, (int)t.asSeconds() % 60 );
	UpdateData(FALSE);
	if(EnvMode_Terminal == m_pDoc->m_systemMode)
	{
		m_pDoc->m_walkthroughs->GetWalkthrough(m_nSelectedPaxWalkthrough)->SetEndTime(nEndTime);
	}
	else if(EnvMode_AirSide == m_pDoc->m_systemMode)
	{
		m_pDoc->m_walkthroughsFlight->GetWalkthrough(m_nSelectedFlightWalkthrough)->SetEndTime(nEndTime);
	}	
	else
		ASSERT(0);
	*pResult = 0;
}

BOOL CDlgWalkthrough::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
    ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

    // if there is a top level routing frame then let it handle the message
    if (GetRoutingFrame() != NULL) return FALSE;

    // to be thorough we will need to handle UNICODE versions of the message also !!
    TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
    TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
    TCHAR szFullText[512];
    CString strTipText;
    UINT nID = pNMHDR->idFrom;

    if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
        pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
    {
        // idFrom is actually the HWND of the tool 
        nID = ::GetDlgCtrlID((HWND)nID);
    }

    if (nID != 0) // will be zero on a separator
    {
        AfxLoadString(nID, szFullText);
        strTipText=szFullText;

#ifndef _UNICODE
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
        }
        else
        {
            _mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
        }
#else
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            _wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
        }
        else
        {
            lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
        }
#endif

        *pResult = 0;

        // bring the tooltip window above other popup windows
        ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
            SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
        
        return TRUE;
    }

    return FALSE;
}