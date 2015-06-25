// DlgPanorama.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgPanorama.h"
#include "../Common/ARCUnit.h"
#include "../Common/latlong.h"
#include "../Common/FloatUtils.h"
#include "../common/UnitsManager.h"
#include ".\dlgpanorama.h"
#include <CommonData/Fallback.h>
#include "../Common/WinMsg.h"
#include "./TermPlanDoc.h"
#include "./MainFrm.h"
#include "./3DView.h"
#include "./Movies.h"
#include "./Movie.h"
#include "ChildFrm.h"
#include "DlgMovieWriter.h"

#include "AnimationTimeManager.h"

#include <Renderer/RenderEngine/RenderEngine.h>
#include <Results/EventLogBufManager.h>
#include "Render3DView.h"
#include "Render3DFrame.h"

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
// CDlgPanorama dialog

IMPLEMENT_DYNAMIC(CDlgPanorama, CXTResizeDialog)
CDlgPanorama::CDlgPanorama(int nPrjID,int nAirportID, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgPanorama::IDD, pParent)
	/*, m_iInclination(0)
	, m_iEndBearing(0)
	, m_iStartBearing(0)
	, m_iTurnSpeed(0)
	, m_iAltitude(0)*/
{
	m_dataList.ReadData(nAirportID);
	m_nProjID = nPrjID;
	m_nAirportID = nAirportID; 
	m_AirportInfo.ReadAirport(nAirportID);

	m_logfirstTime = 0;
	m_logEndTime =0;


	
}

CDlgPanorama::~CDlgPanorama()
{
}

void CDlgPanorama::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_PANORAMAVIEWS, m_ListCtrl);
	DDX_Control(pDX, IDC_EDIT_POSX, m_editPosx);
	DDX_Control(pDX, IDC_EDIT_POSY, m_editPosy);
	DDX_Control(pDX, IDC_EDIT_ALTITUDE, m_editAlt);
	DDX_Control(pDX, IDC_EDIT_TUNRSPEED, m_editTurnSpd);
	DDX_Control(pDX, IDC_EDIT_STARTBEARING, m_editStartBearing);
	DDX_Control(pDX, IDC_EDIT_ENDBEARING, m_editEndBearing);
	DDX_Control(pDX, IDC_EDIT_INCLINATION, m_editInclination);
	//DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_staticToolBar);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_saveButton);


}


BEGIN_MESSAGE_MAP(CDlgPanorama, CXTResizeDialog)
	//{{AFX_MSG_MAP(CDlgPanorama)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_BUTTON_NEWPANORAM, OnAddPanorama)
	ON_COMMAND(ID_BUTTON_DELETEPANORAMA, OnDeletePanorama)
	ON_COMMAND(ID_BUTTON_RECORDPANORAMA, OnRecordPanorama)
	ON_COMMAND(ID_BUTTON_PREVIEWPANORAMA, OnPreviewPanorama)
	ON_COMMAND(ID_BUTTON_PREVIEWPANORAMAWITHLOGS, OnPreviewPanoramaWithAnima)	
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PANORAMAVIEWS, OnItemChangedListPanoramas)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_EN_KILLFOCUS(IDC_EDIT_TUNRSPEED, OnEnKillfocusEditTunrspeed)
	ON_EN_KILLFOCUS(IDC_EDIT_ALTITUDE, OnEnKillfocusEditAltitude)
	ON_EN_KILLFOCUS(IDC_EDIT_STARTBEARING, OnEnKillfocusEditStartbearing)
	ON_EN_KILLFOCUS(IDC_EDIT_ENDBEARING, OnEnKillfocusEditEndbearing)
	ON_EN_KILLFOCUS(IDC_EDIT_INCLINATION, OnEnKillfocusEditInclination)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_PANORAMAVIEWS, OnLvnEndlabeleditListPanoramaviews)
	ON_EN_CHANGE(IDC_EDIT_POSX, OnEnChangeEditPosx)
	ON_EN_CHANGE(IDC_EDIT_POSY, OnEnChangeEditPosy)
	ON_EN_CHANGE(IDC_EDIT_ALTITUDE, OnEnChangeEditAltitude)
	ON_BN_CLICKED(IDC_BUTTON_PICK, OnBnClickedButtonPick)
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
	ON_EN_CHANGE(IDC_EDIT_TUNRSPEED, OnEnChangeEditTunrspeed)
	ON_EN_CHANGE(IDC_EDIT_STARTBEARING, OnEnChangeEditStartbearing)
	ON_EN_CHANGE(IDC_EDIT_ENDBEARING, OnEnChangeEditEndbearing)
	ON_EN_CHANGE(IDC_EDIT_INCLINATION, OnEnChangeEditInclination)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
END_MESSAGE_MAP()


CTermPlanDoc* CDlgPanorama::GetDocument() const
{
	return (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
}


void CDlgPanorama::OnOK()
{
	OnBnClickedButtonSave();
	CXTResizeDialog::OnOK();
}


int CDlgPanorama::OnCreate( LPCREATESTRUCT lpCreateStruct )
{

	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP 
		| CBRS_TOOLTIPS ) ||
		!m_ToolBar.LoadToolBar(IDR_MOVIETOOLBAR_PANORAMA))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	
	if(CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CDlgPanorama::UpdateToolBar()
{

	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_NEWPANORAM, TRUE);
	if(m_dataList.GetSelectItem() )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DELETEPANORAMA, TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_RECORDPANORAMA, TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_PREVIEWPANORAMA, TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_PREVIEWPANORAMAWITHLOGS,TRUE);
	}
	else
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DELETEPANORAMA, FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_RECORDPANORAMA, FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_PREVIEWPANORAMA, FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_PREVIEWPANORAMAWITHLOGS,FALSE);
	}
}

BOOL CDlgPanorama::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	
 	CRect rcToolbar;
 	m_ListCtrl.GetWindowRect(&rcToolbar);
 	ScreenToClient(&rcToolbar);
 	rcToolbar.top -= 27;
 	rcToolbar.bottom = rcToolbar.top + 22;
 	rcToolbar.left += 4;
 	m_ToolBar.MoveWindow(rcToolbar);
 
 	InitListCtrl();
 	InitLatLongBox();
 	
 	SetResize(m_ToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
 	SetResize(IDC_LIST_PANORAMAVIEWS, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);
 	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
 	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
 	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT );
 	SetResize(IDC_STATIC_GROUP_LIST, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
 	SetResize(IDC_STATIC_GROUPROTATION, SZ_BOTTOM_CENTER,SZ_BOTTOM_RIGHT);
 	SetResize(IDC_STATIC_GROUPPOS, SZ_BOTTOM_LEFT, SZ_BOTTOM_CENTER);
 	
 	SetResize(IDC_STATIC_LAT, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
 	SetResize(m_editLat.GetDlgCtrlID(), SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
 	SetResize(IDC_STATIC_LONG,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
 	SetResize(m_editLong.GetDlgCtrlID(),SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
 	SetResize(IDC_STATIC_X, SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
 	SetResize(IDC_STATIC_Y, SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
 	SetResize(IDC_STATIC_POSXUNIT, SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
 	SetResize(IDC_STATIC_POSYUNIT, SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
 	SetResize(IDC_EDIT_POSX, SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
 	SetResize(IDC_EDIT_POSY, SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
 	SetResize(IDC_STATIC_ALT, SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
 	SetResize(IDC_STATIC_ALTUNIT, SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
 	SetResize(IDC_BUTTON_PICK, SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
 	SetResize(IDC_EDIT_ALTITUDE, SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
 
 	SetResize(IDC_STATIC_TURNSPD, SZ_BOTTOM_CENTER,SZ_BOTTOM_CENTER);
 	SetResize(IDC_STATIC_TURNSPDUNIT, SZ_BOTTOM_CENTER,SZ_BOTTOM_CENTER);
 	SetResize(IDC_STATIC_STARBEAR, SZ_BOTTOM_CENTER,SZ_BOTTOM_CENTER);
 	SetResize(IDC_STATIC_STARTBEARUNIT, SZ_BOTTOM_CENTER,SZ_BOTTOM_CENTER);
 	SetResize(IDC_STATIC_ENDBEAR, SZ_BOTTOM_CENTER,SZ_BOTTOM_CENTER);
 	SetResize(IDC_STATIC_ENDBEARUNIT, SZ_BOTTOM_CENTER,SZ_BOTTOM_CENTER);
 	SetResize(IDC_STATIC_INC, SZ_BOTTOM_CENTER,SZ_BOTTOM_CENTER);
 	SetResize(IDC_STATIC_INCUNIT, SZ_BOTTOM_CENTER,SZ_BOTTOM_CENTER);
 	SetResize(IDC_EDIT_TUNRSPEED, SZ_BOTTOM_CENTER,SZ_BOTTOM_CENTER);
 	SetResize(IDC_EDIT_STARTBEARING, SZ_BOTTOM_CENTER,SZ_BOTTOM_CENTER);
 	SetResize(IDC_EDIT_ENDBEARING,SZ_BOTTOM_CENTER,SZ_BOTTOM_CENTER);
 	SetResize(IDC_EDIT_INCLINATION, SZ_BOTTOM_CENTER,SZ_BOTTOM_CENTER);
 
 	SetResize(IDC_STATIC_ESC,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
 
 
 
 	UpdateToolBar();
 	UpdateListCtrl();
 	UpdateEditData();
 	EnableSave(TRUE);
 
 	//update unit text
 	/*CString strUnit;
 	strUnit.Format("(%s)", UNITSMANAGER->GetLengthUnitString() );
 	GetDlgItem(IDC_STATIC_POSXUNIT)->SetWindowText(strUnit);
 	GetDlgItem(IDC_STATIC_POSYUNIT)->SetWindowText(strUnit);*/
 
 	CTermPlanDoc* pDoc	= GetDocument();	
 	if(pDoc)
 		pDoc->LoadAirsideLogs(m_logfirstTime,m_logEndTime);
 
 	return TRUE;
}

void CDlgPanorama::UpdateListCtrl()
{
	
	m_ListCtrl.DeleteAllItems();
	int nCount = (int) m_dataList.GetElementCount();
	for(int i = 0;i < nCount; ++i)
	{
		PanoramaViewData* pData = m_dataList.GetItem(i);
		m_ListCtrl.InsertItem(i,pData->m_strName );
		m_ListCtrl.SetItemData(i, i);
	}
	
	//set sel
	if (nCount > 0 && !m_dataList.GetSelectItem())
	{
		m_dataList.SelectItem(m_dataList.GetItem(nCount-1));
	}

	//if(m_dataList.GetSelectItem())
	{
		m_ListCtrl.SetItemState( m_dataList.GetSelectItemIndex(), LVIS_SELECTED, LVIS_SELECTED );
		m_ListCtrl.SetItemState( m_dataList.GetSelectItemIndex(), LVIS_FOCUSED , LVIS_FOCUSED);
	}
}


void CDlgPanorama::OnPreviewPanoramaWithAnima()
{
	CTermPlanDoc* pDoc	= GetDocument();	


	if(pDoc->m_eAnimState == CTermPlanDoc::anim_none) {
		if(!pDoc->StartAnimation(TRUE))
			return;
	}
	pDoc->m_eAnimState = CTermPlanDoc::anim_pause;	
	
	PanoramaViewData* pData = m_dataList.GetSelectItem();
	pData->SetTimeRange( pDoc->m_animData.nAnimStartTime ,  pDoc->m_animData.nAnimEndTime);

	if ((pDoc->m_animData.m_AnimationModels.IsOnBoardSel() || pDoc->m_animData.m_AnimationModels.IsLandsideSel()) && pDoc->GetRender3DView())
		PreviewPanoramaRender(pData);
	else
	{
		C3DView* pView = pDoc->Get3DView();
		if(!pView)
			return;

		CWaitCursor wc;
		CMovie movie("temp");
		CDlgMovieWriter::BuildMovieFromPanorama(pData, pDoc, &movie);
		wc.Restore();

		HideDialog(pDoc->GetMainFrame());

		BOOL bRequestCancel = FALSE;

		CMovie* pMovie = &movie;
		int nKFCount = pMovie->GetKFCount();
		if(nKFCount>0) {
			LARGE_INTEGER nPerfFreq, nThisPerfCount, nLastPerfCount;
			QueryPerformanceFrequency(&nPerfFreq);

			long nStartTime = pMovie->GetKFTime(0);
			long nEndTime = pMovie->GetKFTime(nKFCount-1);
			long nTime = nStartTime;


			QueryPerformanceCounter(&nLastPerfCount);

			while(nTime <= nEndTime) {

				pMovie->Interpolate(nTime, *(pView->GetCamera()));

				pView->Invalidate(FALSE);
				pView->UpdateWindow();

				CChildFrame* pFrame = (CChildFrame*)pView->GetParentFrame();
				pFrame->m_wndStatusBar.OnUpdateCmdUI(pFrame, FALSE);

				bRequestCancel = PumpMessages();

				if(bRequestCancel) {
				ShowWindow(SW_SHOW);
				return;
				}

				//calc real elapsed time since last nTime increment
				QueryPerformanceCounter(&nThisPerfCount);
				double dElapsedTime = static_cast<double>(nThisPerfCount.QuadPart - nLastPerfCount.QuadPart) / nPerfFreq.QuadPart;
				nLastPerfCount = nThisPerfCount;

				nTime += ((long)(dElapsedTime*10.0*pMovie->GetAnimSpeed()));
			}
		}

		ShowDialog(pDoc->GetMainFrame());	
	}

	EnableSave(TRUE);
}

void CDlgPanorama::OnAddPanorama()
{
	PanoramaViewData* pNewData = m_dataList.CreateNewData();
	ASSERT(pNewData);
	//get airside animation time;
	CTermPlanDoc* pDoc	= GetDocument();		
	pNewData->SetTimeRange( m_logfirstTime, m_logEndTime );
	m_dataList.SelectItem(pNewData);
	UpdateToolBar();
	UpdateListCtrl();	
	UpdateEditData();

	EnableSave(TRUE);

}

void CDlgPanorama::OnRecordPanorama()
{
	PanoramaViewData* theData = m_dataList.GetSelectItem();
	if(!theData)
		return;

	CTermPlanDoc* pDoc	= GetDocument();
	if(pDoc->m_eAnimState != CTermPlanDoc::anim_none) {
		pDoc->StopAnimation();
	}
	if(pDoc->m_eAnimState != CTermPlanDoc::anim_none)
		return;
	if(!pDoc->StartAnimation(TRUE))
		return;
	pDoc->m_eAnimState = CTermPlanDoc::anim_pause;
	theData->SetTimeRange( pDoc->m_animData.nAnimStartTime ,  pDoc->m_animData.nAnimEndTime);

	if ((pDoc->m_animData.m_AnimationModels.IsOnBoardSel() || pDoc->m_animData.m_AnimationModels.IsLandsideSel()) && pDoc->GetRender3DView())
	{
		RecordPanoramaRender(theData);
		return;
	}
	

	ANIMTIMEMGR->EndTimer();
	CDlgMovieWriter dlg(pDoc, CDlgMovieWriter::PANORAMA, 0);
	dlg.SetPanoramData(theData);
	dlg.DoFakeModal();
	ANIMTIMEMGR->StartTimer();
}

void CDlgPanorama::OnDeletePanorama()
{
	if( m_dataList.GetSelectItem() )
	{
		m_dataList.DeleteItemFromIndex(m_dataList.GetSelectItemIndex());		

		UpdateToolBar();
		UpdateListCtrl();
		UpdateEditData();
		
		EnableSave(TRUE);
	}
	
}




void CDlgPanorama::OnPreviewPanorama()
{	
	if(!m_dataList.GetSelectItem())
		return;
	PanoramaViewData* pData = m_dataList.GetSelectItem();
	long dOnRoundTime = long( 100.0 * abs(pData->m_dStartBearing - pData->m_dEndBearing) / pData->m_dTurnSpeed );
	PanoramaViewData datacopy = *pData;
	datacopy.SetTimeRange(0, dOnRoundTime);

	CTermPlanDoc* pDoc = GetDocument();
	if ((pDoc->m_animData.m_AnimationModels.IsOnBoardSel() || pDoc->m_animData.m_AnimationModels.IsLandsideSel()) && pDoc->GetRender3DView())
		PreviewPanoramaRender(&datacopy);
	else
	{
		C3DView* pView = pDoc->Get3DView();
		if(!pView)
			return;

		C3DCamera CopyCam(*(pView->GetCamera()));

		CWaitCursor wc;
		CMovie movie("temp");
		CDlgMovieWriter::BuildMovieFromPanorama(&datacopy, pDoc, &movie);
		wc.Restore();
	
	
		HideDialog(pDoc->GetMainFrame());
	
		BOOL bRequestCancel = FALSE;
	
		CMovie* pMovie = &movie;
		int nKFCount = pMovie->GetKFCount();
		if(nKFCount>0) {
			LARGE_INTEGER nPerfFreq, nThisPerfCount, nLastPerfCount;
			QueryPerformanceFrequency(&nPerfFreq);
	
			long nStartTime = pMovie->GetKFTime(0);
			long nEndTime = pMovie->GetKFTime(nKFCount-1);
			long nTime = nStartTime;
		
	
			QueryPerformanceCounter(&nLastPerfCount);
	
			while(nTime <= nEndTime) {
 				pMovie->Interpolate(nTime, *(pView->GetCamera()));
 				pView->Invalidate(FALSE);
 				pView->UpdateWindow();

 				CChildFrame* pFrame = (CChildFrame*)pView->GetParentFrame();
				pFrame->m_wndStatusBar.OnUpdateCmdUI(pFrame, FALSE);
 	
 				bRequestCancel = PumpMessages();
 	
 				if(bRequestCancel) {
					*(pView->GetCamera())=CopyCam;
					pView->Invalidate(FALSE);
					pView->UpdateWindow();
					ShowWindow(SW_SHOW);	
 					return;
 				}
	
				//calc real elapsed time since last nTime increment
				QueryPerformanceCounter(&nThisPerfCount);
				double dElapsedTime = static_cast<double>(nThisPerfCount.QuadPart - nLastPerfCount.QuadPart) / nPerfFreq.QuadPart;
				nLastPerfCount = nThisPerfCount;
	
				nTime += ((long)(dElapsedTime*10.0*pMovie->GetAnimSpeed()));
			}
		}
		*(pView->GetCamera())=CopyCam;
		pView->Invalidate(FALSE);
		pView->UpdateWindow();
	}
	ShowDialog(pDoc->GetMainFrame());
}

// CDlgPanorama message handlers
BOOL CDlgPanorama::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
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

void CDlgPanorama::OnItemChangedListPanoramas( NMHDR* pNMHDR, LRESULT* pResult )
{
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	if(pnmv->uNewState & LVIS_SELECTED) {
		//a new item has been selected
		// TRACE("Selected Item %d...\n", pnmv->iItem);
		int selectItem = (int) m_ListCtrl.GetItemData(pnmv->iItem);
		m_dataList.SelectItem(selectItem);
		UpdateToolBar();
		UpdateEditData();

		EnableSave(TRUE);
	}
}


void CDlgPanorama::InitListCtrl()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	ASSERT(::IsWindow(m_ListCtrl.m_hWnd));

	//init listctrl
	m_ListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS| LVS_SINGLESEL );
	DWORD dwStyle = m_ListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_ListCtrl.SetExtendedStyle( dwStyle );

	CStringList strlist;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.csList = & strlist;
	lvc.pszText = "Name";
	lvc.cx = rcClient.Width()/2;
	lvc.fmt = LVCFMT_EDIT;
	m_ListCtrl.InsertColumn(0,&lvc);

}

void CDlgPanorama::InitLatLongBox()
{
	CRect rectLongitude, rectLatitude;
	GetDlgItem(IDC_LONGITUDE)->GetWindowRect(&rectLongitude);
	GetDlgItem(IDC_LATITUDE)->GetWindowRect(&rectLatitude);
	ScreenToClient(rectLongitude);
	ScreenToClient(rectLatitude);
	GetDlgItem(IDC_LONGITUDE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LATITUDE)->ShowWindow(SW_HIDE);
	m_editLong.Create(NULL, NULL, WS_CHILD|WS_VISIBLE|WS_TABSTOP, CRect(0,0,0,0), this, 10);
	m_editLat.Create(NULL, NULL, WS_CHILD|WS_VISIBLE|WS_TABSTOP, CRect(0,0,0,0), this, 11);
	m_editLong.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	m_editLat.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	m_editLong.SetWindowPos(&CWnd::wndTop, rectLongitude.left, rectLongitude.top,
		rectLongitude.Width(), rectLongitude.Height(), SWP_SHOWWINDOW);
	m_editLat.SetWindowPos(&CWnd::wndTop, rectLatitude.left, rectLatitude.top, 
		rectLatitude.Width(), rectLatitude.Height(), SWP_SHOWWINDOW);

	m_editLong.SetComboBoxItem("E");
 	m_editLong.SetComboBoxItem("W");
	m_editLong.SetItemRange(180);	

	m_editLat.SetComboBoxItem("N");
 	m_editLat.SetComboBoxItem("S");
	m_editLat.SetItemRange(90);
}

void CDlgPanorama::OnEnKillfocusEditTunrspeed()
{
	// TODO: Add your control notification handler code here
	UpdateEditTurnSpeed();		
}

void CDlgPanorama::OnEnKillfocusEditAltitude()
{
	// TODO: Add your control notification handler code here
	UpdateEditAltitude();		
}

void CDlgPanorama::OnEnKillfocusEditStartbearing()
{
	// TODO: Add your control notification handler code here	
	UpdateEditStartBearing();	
}

void CDlgPanorama::OnEnKillfocusEditEndbearing()
{
	// TODO: Add your control notification handler code here
	UpdateEditEndBearing();			
}

void CDlgPanorama::OnEnKillfocusEditInclination()
{	// TODO: Add your control notification handler code here
	UpdateEditInclination();	
}


void CDlgPanorama::OnEnChangeEditPosx()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CXTResizeDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_dataList.GetSelectItem() )
	{
		PanoramaViewData* pData = m_dataList.GetSelectItem();
		CString strPosx;
		m_editPosx.GetWindowText(strPosx);
		double dPosx = atof(strPosx);
		dPosx = dPosx * 100;
		pData->m_pos.setX(dPosx);
		UpdateEditLatLong();
		EnableSave(TRUE);
	}
	
}

void CDlgPanorama::OnEnChangeEditPosy()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CXTResizeDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if( m_dataList.GetSelectItem() )
	{
		PanoramaViewData* pData = m_dataList.GetSelectItem();
		CString strPosy;
		m_editPosy.GetWindowText(strPosy);
		double dPosy = atof(strPosy);
		dPosy = dPosy * 100;
		pData->m_pos.setY(dPosy);
		UpdateEditLatLong();
		EnableSave(TRUE);
	}
}

void CDlgPanorama::OnEnChangeEditAltitude()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CXTResizeDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if(m_dataList.GetSelectItem())
	{
		PanoramaViewData* pData = m_dataList.GetSelectItem();
		CString strAlt;
		m_editAlt.GetWindowText(strAlt);
		double dAlt = atof(strAlt);
		dAlt = ARCUnit::ConvertLength(dAlt, ARCUnit::FEET, ARCUnit::CM);
		pData->SetAltitude(dAlt);
		EnableSave(TRUE);
	}
}



void CDlgPanorama::OnEnChangeEditTunrspeed()
{
	// TODO:  Add your control notification handler code here
	if( m_dataList.GetSelectItem() )
	{		
		PanoramaViewData* pData = m_dataList.GetSelectItem();
		CString strTurnSpd;
		m_editTurnSpd.GetWindowText(strTurnSpd);
		double dTurnSpd = atof( strTurnSpd );
		pData->SetTurnSpeed(dTurnSpd);	
		EnableSave(TRUE);
	}
}

void CDlgPanorama::OnEnChangeEditStartbearing()
{
	// TODO:  Add your control notification handler code here
	if( m_dataList.GetSelectItem() )
	{
		PanoramaViewData* pData = m_dataList.GetSelectItem();
		CString strSb;
		m_editStartBearing.GetWindowText(strSb);
		double dSb = atof( strSb);
		pData->SetStartBearing(dSb);	
		EnableSave(TRUE);

	}
}

void CDlgPanorama::OnEnChangeEditEndbearing()
{
	// TODO:  Add your control notification handler code here
	if( m_dataList.GetSelectItem() )
	{
		PanoramaViewData* pData = m_dataList.GetSelectItem();
		CString strEb;
		m_editEndBearing.GetWindowText(strEb);
		double eb = atof(strEb);
		pData->SetEndBearing(eb);		
		EnableSave(TRUE);
	}
}

void CDlgPanorama::OnEnChangeEditInclination()
{
	// TODO:  Add your control notification handler code here
	if( m_dataList.GetSelectItem() )
	{
		PanoramaViewData* pData = m_dataList.GetSelectItem();
		CString strInc;
		m_editInclination.GetWindowText(strInc);
		double dInc = atof(strInc);
		if( dInc > pData->GetMaxInclination() /*PanoramaViewData::MaxInclination*/ || dInc < pData->GetMinInclination() /* PanoramaViewData::MinInclination*/ )
		{
			CString strWarning;
			strWarning.Format("The Inclination should between %d and %d ", (int)pData->GetMinInclination()/*PanoramaViewData::MinInclination*/, (int)pData->GetMaxInclination() /*PanoramaViewData::MaxInclination*/ );
			if( IDOK == MessageBox(strWarning, NULL, MB_OKCANCEL) )
			{
				m_editInclination.SetFocus();
				return;
			}
		}
		else
		{
			pData->SetInclination(dInc);
			EnableSave(TRUE);
		}
	}	
}

void CDlgPanorama::OnLvnEndlabeleditListPanoramaviews(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;

	int nItem = plvItem->iItem;
	if( nItem < 0 )
		return;

	CString strValue = m_ListCtrl.GetItemText(nItem, plvItem->iSubItem);
	m_dataList.SelectItem(nItem);
	if( m_dataList.GetSelectItem() )
	{
		PanoramaViewData* pData = m_dataList.GetSelectItem();
		pData->SetName(strValue);
		EnableSave(TRUE);
	}	
	

}


void CDlgPanorama::UpdateEditLatLong()
{
	if( m_dataList.GetSelectItem() )
	{	
		m_editLat.EnableWindow(TRUE);
		m_editLong.EnableWindow(TRUE);
		PanoramaViewData * pData = m_dataList.GetSelectItem();
		CLatitude clat; CLongitude cLong;
		m_AirportInfo.GetPosLatLong(pData->m_pos,clat, cLong );
		CString strLat; CString strLong;
		clat.GetValue(strLat); 
		cLong.GetValue(strLong);
		m_editLat.SetCtrlText(strLat);
		m_editLong.SetCtrlText(strLong);
	}
	else
	{
		m_editLat.EnableWindow(FALSE);
		m_editLong.EnableWindow(FALSE);
	}
}

void CDlgPanorama::UpdateEditPosXY()
{
	if(  m_dataList.GetSelectItem() )
	{
		m_editPosx.EnableWindow(TRUE);
		m_editPosy.EnableWindow(TRUE);	
		PanoramaViewData * pData = m_dataList.GetSelectItem();
		double dPosX = RoundDouble(( pData->m_pos.getX()/100),2);
		double dPosY = RoundDouble(( pData->m_pos.getY()/100),2);
		CString strPosx; strPosx.Format("%.2f",dPosX);
		CString strPosy; strPosy.Format("%.2f",dPosY);
		m_editPosx.SetWindowText(strPosx);
		m_editPosy.SetWindowText(strPosy);
	}
	else
	{
		m_editPosx.EnableWindow(FALSE);
		m_editPosy.EnableWindow(FALSE);
	}
}

void CDlgPanorama::UpdateEditAltitude()
{
	if( m_dataList.GetSelectItem() )
	{
		m_editAlt.EnableWindow(TRUE);
		PanoramaViewData * pData = m_dataList.GetSelectItem();
		int iAltitude = (int)RoundDouble(ARCUnit::ConvertLength(pData->m_dAltitude,ARCUnit::CM, ARCUnit::FEET),2 );
		CString strAlt ; strAlt.Format("%d",iAltitude);
		m_editAlt.SetWindowText(strAlt);
	}
	else
	{
		m_editAlt.EnableWindow(FALSE);
	}
}

void CDlgPanorama::UpdateEditTurnSpeed()
{
	if( m_dataList.GetSelectItem() )
	{
		m_editTurnSpd.EnableWindow(TRUE);		
		PanoramaViewData * pData = m_dataList.GetSelectItem();
		double dTurnSpeed = RoundDouble(pData->m_dTurnSpeed,2);
		CString strTSpd; strTSpd.Format("%.2f", dTurnSpeed);
		m_editTurnSpd.SetWindowText(strTSpd);
	}
	else
	{
		m_editTurnSpd.EnableWindow(FALSE);
	}
}

void CDlgPanorama::UpdateEditStartBearing()
{
	if( m_dataList.GetSelectItem() )
	{
		m_editStartBearing.EnableWindow(TRUE);
		PanoramaViewData * pData = m_dataList.GetSelectItem();
		int iStartBearing = (int)RoundDouble(pData->m_dStartBearing ,2);
		CString  strSBear; strSBear.Format("%d", iStartBearing);
		m_editStartBearing.SetWindowText(strSBear);
	}
	else
	{
		m_editStartBearing.EnableWindow(FALSE);
	}
}

void CDlgPanorama::UpdateEditEndBearing()
{
	if( m_dataList.GetSelectItem() )
	{
		m_editEndBearing.EnableWindow(TRUE);
		PanoramaViewData * pData = m_dataList.GetSelectItem();
		int iEndBearing = (int)RoundDouble(pData->m_dEndBearing ,2);
		CString strEBear; strEBear.Format("%d", iEndBearing);
		m_editEndBearing.SetWindowText(strEBear);
	}
	else
	{
		m_editEndBearing.EnableWindow(FALSE);
	}
}

void CDlgPanorama::UpdateEditInclination()
{
	if( m_dataList.GetSelectItem() )
	{
		m_editInclination.EnableWindow(TRUE);
		PanoramaViewData * pData = m_dataList.GetSelectItem();
		int iInclination = (int)RoundDouble(pData->m_dInclination ,2);
		CString strInc; strInc.Format("%d", iInclination);
		m_editInclination.SetWindowText(strInc);	
	}
	else
	{
		m_editInclination.EnableWindow(FALSE);
	}
}

void CDlgPanorama::UpdateEditData()
{
	UpdateEditLatLong();	
	UpdateEditPosXY();	
	UpdateEditAltitude();
	UpdateEditTurnSpeed();
	UpdateEditStartBearing();
	UpdateEditEndBearing();
	UpdateEditInclination();

}
void CDlgPanorama::OnBnClickedButtonPick()
{
	// TODO: Add your control notification handler code here
	CTermPlanDoc* pDoc	= GetDocument();
	C3DView* p3DView = pDoc->Get3DView();
	if( p3DView == NULL )
	{
		pDoc->GetMainFrame()->CreateOrActive3DView();
		p3DView = pDoc->Get3DView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}

	HideDialog( pDoc->GetMainFrame() );
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();
	enum FallbackReason enumReason;

	enumReason= PICK_ONEPOINT;


	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
}

LRESULT CDlgPanorama::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{

	ShowWindow(SW_SHOW);	
	EnableWindow();
	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);

	CTermPlanDoc* pDoc	= GetDocument();	

	if(pData->size()>0)
	{
		//		double dx,dy;
		Point ptList[MAX_POINTS];
		ARCVector3 v3D = pData->at(0);
		CString strTemp;
		double dx= /*UNITSMANAGER->ConvertLength*/(v3D[VX]);
		double dy= /*UNITSMANAGER->ConvertLength*/(v3D[VY]);

		if( m_dataList.GetSelectItem()) 
		{
			PanoramaViewData* pData = m_dataList.GetSelectItem();
			pData->m_pos.setX(dx);
			pData->m_pos.setY(dy);
			UpdateEditLatLong();
			UpdateEditPosXY();
		}
	}
	
	return TRUE;
}

void CDlgPanorama::HideDialog( CWnd* parentWnd )
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CDlgPanorama::ShowDialog( CWnd* parentWnd )
{
	while (!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		parentWnd->ShowWindow(SW_SHOW);
		parentWnd->EnableWindow(FALSE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(FALSE);
	ShowWindow(SW_SHOW);	
	EnableWindow();
}


void CDlgPanorama::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction();
		m_dataList.SaveData(m_nAirportID);
		CADODatabase::CommitTransaction();

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	EnableSave(FALSE);
	GetDlgItem(IDOK)->SetFocus();
}

void CDlgPanorama::EnableSave( BOOL b )
{
	CWnd* pwnd = GetDlgItem(IDC_BUTTON_SAVE);
	if(pwnd)
		pwnd->EnableWindow(b);
}

void CDlgPanorama::PreviewPanoramaRender( const PanoramaViewData* pData )
{
	CTermPlanDoc* pDoc = GetDocument();	
	CRender3DView* pRender3DView = pDoc->GetRender3DView();
	if (!pRender3DView)
		return;

	ANIMTIMEMGR->EndTimer();
	HideDialog(pDoc->GetMainFrame());
	long lLastAnimaTime = pDoc->m_animData.nLastAnimTime;
	int nLastAnimaDirection = pRender3DView->m_nAnimDirection;
	CCameraData lastCamera = *pRender3DView->GetCameraData();

	pRender3DView->m_nAnimDirection = CEventLogBufManager::ANIMATION_FORWARD;

	ALTAirport theAirport;
	int aptID = InputAirside::GetAirportID(pDoc->GetProjectID());
	theAirport.ReadAirport(aptID);
	double dAngleFrom = 90 -  pData->m_dStartBearing - theAirport.GetMagnectVariation().GetRealVariation(); 
	double dAngleTo = 90 - pData->m_dEndBearing - theAirport.GetMagnectVariation().GetRealVariation();
	double dAngleRange = abs(dAngleTo - dAngleFrom);

	long nStartTime = pData->GetStartTime();
	long nEndTime = pData->GetEndTime();
	long nTime = nStartTime;

	if (pRender3DView->StartViewPanoramaMovie(pData))
	{
		LARGE_INTEGER nPerfFreq, nThisPerfCount, nLastPerfCount;
		QueryPerformanceFrequency(&nPerfFreq);
		QueryPerformanceCounter(&nLastPerfCount);
		double dRealTime = 0.0;

		while(nTime <= nEndTime)
		{
			//set anim direction
			pDoc->m_animData.nLastAnimTime = nTime;
			double dAngle = (nTime - nStartTime)*pData->m_dTurnSpeed/100;
			dAngle -= long(dAngle/dAngleRange)*dAngleRange;
			if (dAngleTo > dAngleFrom)
				dAngle += dAngleFrom;
			else
				dAngle = dAngleFrom - dAngle;
			pRender3DView->UpdatePanoramaMovieCamera(dAngle);

			//redraw view
			pRender3DView->Invalidate(FALSE);
			pRender3DView->UpdateWindow();
			CRender3DFrame* pFrame = pRender3DView->GetParentFrame();
			pFrame->GetStatusBar().OnUpdateCmdUI(pFrame, FALSE);

			if(PumpMessages())
				break;

			QueryPerformanceCounter(&nThisPerfCount);
			double dElapsedTime = static_cast<double>(nThisPerfCount.QuadPart - nLastPerfCount.QuadPart) / nPerfFreq.QuadPart;
			nLastPerfCount = nThisPerfCount;

			nTime += ((long)(dElapsedTime*10.0*pDoc->m_animData.nAnimSpeed));
		}

		pRender3DView->EndViewMovie();
	}

	pRender3DView->SetCameraData(lastCamera);
	pRender3DView->m_nAnimDirection = nLastAnimaDirection;
	pDoc->m_animData.nLastAnimTime = lLastAnimaTime;
	ShowDialog(pDoc->GetMainFrame());
	ANIMTIMEMGR->StartTimer();

	pRender3DView->Invalidate(FALSE);
}

void CDlgPanorama::RecordPanoramaRender( PanoramaViewData* theData )
{
	CTermPlanDoc* pDoc = GetDocument();	
	CRender3DView* pRender3DView = pDoc->GetRender3DView();
	if (!pRender3DView)
		return;

	ANIMTIMEMGR->EndTimer();
// 	ShowWindow(SW_HIDE);
	long lLastAnimaTime = pDoc->m_animData.nLastAnimTime;
	CCameraData lastCamera = *pRender3DView->GetCameraData();

	if (pRender3DView->StartViewPanoramaMovie(theData))
	{
		CDlgMovieWriter dlg(pDoc, CDlgMovieWriter::PANORAMA, 0);
		dlg.SetPanoramData(theData);
		dlg.DoFakeModal();

		pRender3DView->EndViewMovie();
	}

	pRender3DView->SetCameraData(lastCamera);
	pDoc->m_animData.nLastAnimTime = lLastAnimaTime;
// 	ShowWindow(SW_SHOW);
	ANIMTIMEMGR->StartTimer();

	pRender3DView->Invalidate(FALSE);

}