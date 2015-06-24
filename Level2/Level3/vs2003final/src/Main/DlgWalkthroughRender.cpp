// DlgWalkthrough.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"

#include "DlgWalkthroughRender.h"
#include "Render3DView.h"
#include "Render3DFrame.h"

#include "AnimationTimeManager.h"
#include "DlgMovieWriter.h"
#include "Movie.h"
#include "TermPlanDoc.h"
#include "AnimationData.h"

#include "results\EventLogBufManager.h"
#include "ModelessDlgManager.h"
#include "Landside\LandsideSimLogs.h"
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
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if(msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
			ret = TRUE;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return ret;
}
/////////////////////////////////////////////////////////////////////////////
// CDlgWalkthroughRender dialog


CDlgWalkthroughRender::CDlgWalkthroughRender(CTermPlanDoc* pTPDoc, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgWalkthroughRender::IDD, pParent)
	, m_pDoc(pTPDoc)
	, m_pView(pTPDoc->GetRender3DView())
	, m_nSelWalkthrough(-1)
	, m_pParent(pParent)
	, m_selMobID(CWalkthrough::Mob_None, -1)
	, m_nMinTime(0)
	, m_nMaxTime(0)
	, m_bNewMovie(FALSE)
{
}


void CDlgWalkthroughRender::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PAX, m_editPax);
	DDX_Control(pDX, IDC_LIST_WALKTHROUGHS, m_lstWalkthrough);
	DDX_Control(pDX, IDC_COMBO_VIEWPOINT, m_cmbViewpoint);
	DDX_Control(pDX, IDC_STATIC_PAX, m_staticPax);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_endTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START, m_startTime);
}


BEGIN_MESSAGE_MAP(CDlgWalkthroughRender, CXTResizeDialog)
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_COMBO_VIEWPOINT, OnSelchangeComboViewpoint)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_START, OnDatetimepickerChangeStart)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_END, OnDatetimepickerChangeEnd)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_WALKTHROUGHS, OnItemChanged)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_WALKTHROUGHS, OnEndLabelEdit)
	ON_COMMAND(ID_BUTTON_NEWWT, OnAddMovie)
	ON_COMMAND(ID_BUTTON_DELETEWT, OnDeleteMovie)
	ON_COMMAND(ID_BUTTON_RECORDWT, OnRecordMovie)
	ON_COMMAND(ID_BUTTON_PREVIEWWALKTHROUGH, OnPreviewMovie)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWalkthroughRender message handlers

int CDlgWalkthroughRender::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

BOOL CDlgWalkthroughRender::OnInitDialog() 
{
	CXTResizeDialog::OnInitDialog();

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
	pWnd->SetWindowText(_T("Vehicle"));
	m_cmbViewpoint.AddString("First-Person View");
	m_cmbViewpoint.AddString("Third-Person View");
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

void CDlgWalkthroughRender::UpdatePAXData()
{
	if(m_nSelWalkthrough == -1)
	{
		EnableControls(FALSE);
	}
	else 
	{
		WalkthroughListData* listData = GetSelWalkthroughData();
		VERIFY(listData);
		const CWalkthrough* pWT = listData->pWalkthrough;
		m_selMobID.first = listData->mobType;
		m_selMobID.second = pWT->GetPaxID();

		EnableControls(m_selMobID.second>=0);
		ConfigControlsBySelMobType(listData->mobType);
		if (m_selMobID.second < 0)
			return;

		m_editPax.SetWindowText(pWT->GetPaxIDDesc());
		SetDataFromID(m_selMobID);

		bool bAbsDate;
		COleDateTime outDate;
		int nDateIdx;
		COleDateTime outTime;

		ElapsedTime eStartTime( pWT->GetStartTime() / TimePrecision );
		m_pDoc->m_animData.m_startDate.GetDateTime( eStartTime, bAbsDate, outDate, nDateIdx, outTime );
		m_startTime = outTime;

		ElapsedTime eEndTime( pWT->GetEndTime() / TimePrecision );
		m_pDoc->m_animData.m_startDate.GetDateTime( eEndTime, bAbsDate, outDate, nDateIdx, outTime );
		m_endTime = outTime;

     	m_cmbViewpoint.SetCurSel((int)pWT->GetViewpoint());

	    UpdateData(FALSE);
	}
}


void CDlgWalkthroughRender::SelectMob( CWalkthrough::WalkthroughMobType mobType, int nMobID, const CString& sDesc )
{
	WalkthroughListData* listData = GetSelWalkthroughData();
	if(listData) 
	{
		if (listData->mobType != mobType) // need switch data set
		{
			CWalkthroughs* walkthroughsOld = GetWalkthroughsByMobType(listData->mobType);
			CWalkthroughs* walkthroughsNew = GetWalkthroughsByMobType(mobType);
			if (walkthroughsOld && walkthroughsNew)
			{
				walkthroughsOld->MoveWalthroughToOther(walkthroughsNew, listData->pWalkthrough);
			}
		}
		else if (listData->pWalkthrough && listData->pWalkthrough->GetPaxID() == nMobID)
			return;

		listData->mobType = mobType;
		CWalkthrough* pWalkthrough = listData->pWalkthrough;
		switch (mobType)
		{
		case CWalkthrough::Mob_Pax:
			{
				pWalkthrough->SetEnvMode(EnvMode_Terminal);
			}
			break;
		case CWalkthrough::Mob_Flight:
			{
				pWalkthrough->SetEnvMode(EnvMode_AirSide);
			}
			break;
		case CWalkthrough::Mob_Vehicle:
			{
				pWalkthrough->SetEnvMode(EnvMode_LandSide);
			}
			break;
		default:
			{
				ASSERT(FALSE);
			}
			break;
		}
		pWalkthrough->SetPaxID(nMobID);
		pWalkthrough->SetPaxIDDesc(sDesc);

		m_selMobID = SelMobID(mobType, nMobID);

		SetDataFromID(m_selMobID);

		pWalkthrough->SetStartTime(m_nMinTime);
		pWalkthrough->SetEndTime(m_nMaxTime);

		UpdatePAXData();		
	}
}

void CDlgWalkthroughRender::SetDataFromID( SelMobID selMobID )
{
	m_nMinTime = m_nMaxTime = 0;
	switch (selMobID.first)
	{
	case CWalkthrough::Mob_Pax:
		{
			if (selMobID.second < m_pDoc->GetTerminal().paxLog->getCount())
			{
				MobLogEntry element;
				VERIFY(m_pDoc->GetTerminal().paxLog->GetItemByID(element, selMobID.second));
				m_nMinTime = element.getEntryTime();
				m_nMaxTime = element.getExitTime();
			}
		}
		break;
	case CWalkthrough::Mob_Flight:
		{
			if (selMobID.second < m_pDoc->GetAirsideSimLogs().m_airsideFlightLog.getCount())
			{
				AirsideFlightLogEntry element;
				VERIFY(m_pDoc->GetAirsideSimLogs().m_airsideFlightLog.GetItemByID(element, selMobID.second/* - CRender3DView::SEL_FLIGHT_OFFSET*/));
				m_nMinTime = element.getEntryTime();
				m_nMaxTime = element.getExitTime();
			}
		}
		break;
	case CWalkthrough::Mob_Vehicle:
		{
			if (selMobID.second < m_pDoc->GetLandsideSimLogs().getVehicleLog().getCount())
			{
				LandsideVehicleLogEntry element;
				VERIFY(m_pDoc->GetLandsideSimLogs().getVehicleLog().GetItemByID(element, selMobID.second/* - CRender3DView::SEL_FLIGHT_OFFSET*/));
				m_nMinTime = element.getEntryTime();
				m_nMaxTime = element.getExitTime();
			}
		}
		break;
	default:
		{
			ASSERT(FALSE);
		}
		break;
	}
}

void CDlgWalkthroughRender::OnSelchangeComboViewpoint() 
{
	WalkthroughListData* listData = GetSelWalkthroughData();
	if (listData)
	{
		listData->pWalkthrough->SetViewpoint((CWalkthrough::VIEWPOINT)m_cmbViewpoint.GetCurSel());
	}
}


void CDlgWalkthroughRender::EnableControls(BOOL bEnable)
{
	m_staticPax.EnableWindow(bEnable);
	m_editPax.EnableWindow(bEnable);
	m_cmbViewpoint.EnableWindow(bEnable);

	GetDlgItem(IDC_STATIC_VIEWPOINT)->EnableWindow(bEnable);
	GetDlgItem(IDC_DATETIMEPICKER_START)->EnableWindow(bEnable);
	GetDlgItem(IDC_DATETIMEPICKER_END)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_STARTTIME)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_ENDTIME)->EnableWindow(bEnable);
}

void CDlgWalkthroughRender::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
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
		m_nSelWalkthrough = pnmv->iItem;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DELETEWT, TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_RECORDWT, TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_PREVIEWWALKTHROUGH, TRUE);
	}
	else
		ASSERT(0);

	UpdatePAXData();

}

void CDlgWalkthroughRender::AddWalkthroughs2List(CWalkthroughs* walkthroughs, CWalkthrough::WalkthroughMobType mobType)
{
	int nExistCount = m_lstWalkthrough.GetItemCount();
	int nMovieCount = walkthroughs->GetWalkthroughCount();
	for(int i=0; i<nMovieCount; i++)
	{
		CWalkthrough* pWalkthrough = walkthroughs->GetWalkthrough(i);
		int nIndex = m_lstWalkthrough.InsertItem(nExistCount + i, walkthroughs->GetWalkthroughName(i));
		m_vListData.push_back(WalkthroughListData(mobType, pWalkthrough));
		m_lstWalkthrough.SetItemData(nIndex, m_vListData.size() - 1);
	}
}
void CDlgWalkthroughRender::UpdateList()
{
	m_lstWalkthrough.DeleteAllItems();

//	AddWalkthroughs2List(m_pDoc->m_walkthroughs, CWalkthrough::Mob_Pax);
//	AddWalkthroughs2List(m_pDoc->m_walkthroughsFlight, CWalkthrough::Mob_Flight);
	AddWalkthroughs2List(m_pDoc->m_walkthroughsVehicle, CWalkthrough::Mob_Vehicle);
	m_nSelWalkthrough = -1;
	UpdatePAXData();
}

void CDlgWalkthroughRender::OnAddMovie()
{
	int nListCount = m_lstWalkthrough.GetItemCount();
	EnvironmentMode sysMode = m_pDoc->m_systemMode;

	CWalkthroughs* walkthroughs = NULL;
	CWalkthrough::WalkthroughMobType mobType = CWalkthrough::Mob_None;
	if (EnvMode_Terminal == sysMode)
	{
		walkthroughs = m_pDoc->m_walkthroughs;
		mobType = CWalkthrough::Mob_Pax;
	}
	else if (EnvMode_AirSide == sysMode)
	{
		walkthroughs = m_pDoc->m_walkthroughsFlight;
		mobType = CWalkthrough::Mob_Flight;
	}
	else if (EnvMode_LandSide == sysMode)
	{
		walkthroughs = m_pDoc->m_walkthroughsVehicle;
		mobType = CWalkthrough::Mob_Vehicle;
	}
	else
		ASSERT(FALSE);

	if (!walkthroughs)
		return;

	int nDataIndex = walkthroughs->GetWalkthroughCount();
	walkthroughs->AddWalkthrough(sysMode, "undefined");
	CWalkthrough* pWalkthrough = walkthroughs->GetWalkthrough(nDataIndex);
	m_vListData.push_back(WalkthroughListData(mobType, pWalkthrough));

	m_bNewMovie = TRUE;
	int nIndex = m_lstWalkthrough.InsertItem(nListCount, walkthroughs->GetWalkthroughName(nDataIndex));
	m_lstWalkthrough.SetItemData(nIndex, m_vListData.size() - 1);
	m_lstWalkthrough.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
	m_lstWalkthrough.SetFocus();
	m_lstWalkthrough.EditLabel(nIndex);
}

CWalkthroughs* CDlgWalkthroughRender::GetWalkthroughsByMobType(CWalkthrough::WalkthroughMobType mobType) const
{
	switch (mobType)
	{
	case CWalkthrough::Mob_Pax:
		{
			return m_pDoc->m_walkthroughs;
		}
		break;
	case CWalkthrough::Mob_Flight:
		{
			return m_pDoc->m_walkthroughsFlight;
		}
		break;
	case CWalkthrough::Mob_Vehicle:
		{
			return m_pDoc->m_walkthroughsVehicle;
		}
		break;
	default:
		{
			ASSERT(FALSE);
		}
		break;
	}
	return NULL;

}
void CDlgWalkthroughRender::OnDeleteMovie()
{
	WalkthroughListData* listData = GetSelWalkthroughData();
	if (!listData)
		return;
	CWalkthroughs* walkthroughs = GetWalkthroughsByMobType(listData->mobType);
	walkthroughs->DeleteWalkthrough(listData->pWalkthrough);
	UpdateList();
}

void CDlgWalkthroughRender::OnPreviewMovie()
{
	ANIMTIMEMGR->EndTimer();
	ShowWindow(SW_HIDE);
	long lLastAnimaTime = m_pDoc->m_animData.nLastAnimTime;
	int nLastAnimaDirection = m_pView->m_nAnimDirection;
	CCameraData lastCamera = *m_pView->GetCameraData();

	m_pView->m_nAnimDirection = CEventLogBufManager::ANIMATION_FORWARD;

	WalkthroughListData* listData = GetSelWalkthroughData();
	CWalkthrough* pWT = listData->pWalkthrough;

	long nStartTime = pWT->GetStartTime();
	long nEndTime = pWT->GetEndTime();
	long nTime = nStartTime;

	if (m_pView->StartViewTrackMovie(listData->mobType, pWT->GetPaxID(), pWT->GetViewpoint()))
	{
		LARGE_INTEGER nPerfFreq, nThisPerfCount, nLastPerfCount;
		QueryPerformanceFrequency(&nPerfFreq);
		QueryPerformanceCounter(&nLastPerfCount);
	
		while(nTime <= nEndTime)
		{
			//set anim direction
			m_pDoc->m_animData.nLastAnimTime = nTime;
// 			m_pView->UpdateTrackMovieCamera();

			//redraw view
			m_pView->Invalidate(FALSE);
			m_pView->UpdateWindow();
			CRender3DFrame* pFrame = m_pView->GetParentFrame();
			pFrame->GetStatusBar().OnUpdateCmdUI(pFrame, FALSE);
	
			if(PumpMessages())
				break;
	
			QueryPerformanceCounter(&nThisPerfCount);
			double dElapsedTime = static_cast<double>(nThisPerfCount.QuadPart - nLastPerfCount.QuadPart) / nPerfFreq.QuadPart;
			nLastPerfCount = nThisPerfCount;
	
			nTime += ((long)(dElapsedTime*10.0*m_pDoc->m_animData.nAnimSpeed));
		}
	
		m_pView->EndViewMovie();
	}

	m_pView->SetCameraData(lastCamera);
	m_pView->m_nAnimDirection = nLastAnimaDirection;
	m_pDoc->m_animData.nLastAnimTime = lLastAnimaTime;
	ShowWindow(SW_SHOW);
	ANIMTIMEMGR->StartTimer();

	m_pView->Invalidate(FALSE);
}

void CDlgWalkthroughRender::OnRecordMovie()
{
	ANIMTIMEMGR->EndTimer();
	long lLastAnimaTime = m_pDoc->m_animData.nLastAnimTime;
	CCameraData lastCamera = *m_pView->GetCameraData();


	WalkthroughListData* listData = GetSelWalkthroughData();
	if (listData)
	{
		CWalkthrough* pWT = listData->pWalkthrough;
		if (m_pView->StartViewTrackMovie(listData->mobType, pWT->GetPaxID(), pWT->GetViewpoint()))
		{
			CDlgMovieWriter dlg(m_pDoc, CDlgMovieWriter::WALKTHROUGH, m_nSelWalkthrough);
			dlg.SetWalkthrough(listData->pWalkthrough);
			dlg.DoFakeModal();
	
			m_pView->EndViewMovie();
		}
	}

	m_pView->SetCameraData(lastCamera);
	m_pDoc->m_animData.nLastAnimTime = lLastAnimaTime;
	ANIMTIMEMGR->StartTimer();

	m_pView->Invalidate(FALSE);
}

void CDlgWalkthroughRender::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	if(!pDispInfo->item.pszText)
	{
		if(m_bNewMovie)
		{
			OnDeleteMovie();	
			m_bNewMovie=FALSE;
		}
		return;
	}

	m_bNewMovie=FALSE;

	int nIdx = pDispInfo->item.iItem;
	WalkthroughListData* listData = &m_vListData[m_lstWalkthrough.GetItemData(nIdx)];
	CWalkthroughs* walkthroughts = GetWalkthroughsByMobType(listData->mobType);
	walkthroughts->SetWalkthroughName(listData->pWalkthrough, pDispInfo->item.pszText);

    m_lstWalkthrough.SetItemText(nIdx,0,pDispInfo->item.pszText);
	UpdatePAXData();

	*pResult =1;
}

void CDlgWalkthroughRender::PostNcDestroy() 
{
	CXTResizeDialog::PostNcDestroy();
	ASSERT(m_pDoc->m_pModelessDlgMan->GetDlgWalkThroughRender() == this);
	m_pDoc->m_pModelessDlgMan->DestroyDialogWalkThroughRender();
}

void CDlgWalkthroughRender::OnOK() 
{
	if(UpdateData(TRUE))
    {
		m_pDoc->m_walkthroughs->saveDataSet(m_pDoc->m_ProjInfo.path, false);
		m_pDoc->m_walkthroughsFlight->saveDataSet(m_pDoc->m_ProjInfo.path, false);
		m_pDoc->m_walkthroughsVehicle->saveDataSet(m_pDoc->m_ProjInfo.path,false);
		DestroyWindow();
    }
}

void CDlgWalkthroughRender::OnCancel() 
{
	m_pDoc->m_walkthroughs->loadDataSet(m_pDoc->m_ProjInfo.path);
	m_pDoc->m_walkthroughsFlight->loadDataSet(m_pDoc->m_ProjInfo.path);
	m_pDoc->m_walkthroughsVehicle->loadDataSet(m_pDoc->m_ProjInfo.path);
	DestroyWindow();
}

void CDlgWalkthroughRender::OnDatetimepickerChangeStart(NMHDR* pNMHDR, LRESULT* pResult)
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

	WalkthroughListData* listData = GetSelWalkthroughData();
	if (listData)
	{
		listData->pWalkthrough->SetStartTime(nStartTime);
	}

	*pResult = 0;
}

void CDlgWalkthroughRender::OnDatetimepickerChangeEnd(NMHDR* pNMHDR, LRESULT* pResult)
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

	WalkthroughListData* listData = GetSelWalkthroughData();
	if (listData)
	{
		listData->pWalkthrough->SetEndTime(nEndTime);
	}

	*pResult = 0;
}

BOOL CDlgWalkthroughRender::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
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

void CDlgWalkthroughRender::ConfigControlsBySelMobType( CWalkthrough::WalkthroughMobType mobType )
{
	m_cmbViewpoint.ResetContent();
	switch (mobType)
	{
	case CWalkthrough::Mob_Pax:
		{
			m_cmbViewpoint.AddString("First-Person View");
			m_cmbViewpoint.AddString("Third-Person View");
			m_staticPax.SetWindowText(_T("Passenger"));
		}
		break;
	case CWalkthrough::Mob_Flight:
		{
			m_cmbViewpoint.AddString("Pilot View");
			m_cmbViewpoint.AddString("Follow");
			m_staticPax.SetWindowText(_T("Aircraft"));
		}
		break;
	case CWalkthrough::Mob_Vehicle:
		{
			m_cmbViewpoint.AddString("First-Person View");
			m_cmbViewpoint.AddString("Third-Person View");
			m_staticPax.SetWindowText(_T("Vehicle"));
		}
		break;
	default:
		{
			ASSERT(FALSE);
		}
		break;
	}
}

CDlgWalkthroughRender::WalkthroughListData* CDlgWalkthroughRender::GetSelWalkthroughData()
{
	if (m_nSelWalkthrough>=0)
	{
		return &m_vListData[m_lstWalkthrough.GetItemData(m_nSelWalkthrough)];
	}
	return NULL;
}