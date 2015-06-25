// DlgMovie.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "TermPlanDoc.h"
#include "3DView.h"
#include "ChildFrm.h"
#include "Movie.h"
#include "DlgMovieWriter.h"
#include "common\WinMsg.h"
#include "results\EventLogBufManager.h"
#include "DlgMovie.h"
#include "MovieRenderEngine.h"
#include "Render3DView.h"
#include "Renderer/RenderEngine/RenderEngine.h"
#include "AnimationManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MINTIME	0
#define MAXTIME	8640000 //24*3600*100

#define TB_TEXTTIMESTEP	180	//6 hours

#define TB_BARWIDTH			720 //720,480,360,288
#define TB_BARHEIGHT		36
#define	TB_HOURTICK			18
#define TB_SUBTICK			9
#define TB_PIXELSPERHOUR	(TB_BARWIDTH/24)
#define TB_MINUTESPERPIXEL	(60/TB_PIXELSPERHOUR)

#define TB_TIMETOHOURDIV	360000
#define TB_TIMETOMINDIV		6000
#define TB_TIMETOSECDIV		100



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
// CDlgMovie dialog


CDlgMovie::CDlgMovie(CTermPlanDoc* pTPDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMovie::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMovie)
	//}}AFX_DATA_INIT
	m_pDoc = pTPDoc;
	//m_pView = pView;
	//m_hMarkerBitmap = ::LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_BITMAP_KEYFRAME));
}

CDlgMovie::~CDlgMovie()
{
	//if(m_hMarkerBitmap != NULL)
	//	::DeleteObject(m_hMarkerBitmap);
}

void CDlgMovie::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMovie)
	DDX_Control(pDX, IDC_TIMECTRL, m_ctrlKeyFrames);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Control(pDX, IDC_STATIC_BARFRAME, m_btnBarFrame);
	DDX_Control(pDX, IDC_LIST_MOVIES, m_lstMovies);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMovie, CDialog)
	//{{AFX_MSG_MAP(CDlgMovie)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_BUTTON_NEWMOVIE, OnAddMovie)
	ON_COMMAND(ID_BUTTON_DELETEMOVIE, OnDeleteMovie)
	ON_COMMAND(ID_BUTTON_RECORDMOVIE, OnRecordMovie)
	ON_COMMAND(ID_BUTTON_PREVIEWMOVIE, OnPreviewMovie)
	ON_COMMAND(ID_BUTTON_PREVIEWMOVIEWITHLOGS, OnPreviewMovieWithAnimation)
	//ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_MOVIES, OnGetDispInfoListMovies)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_MOVIES, OnEndLabelEditListMovies)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MOVIES, OnItemChangedListMovies)

	ON_MESSAGE(FCM_NOTIFYINSERT_KF, OnInsertKF)
	ON_MESSAGE(FCM_NOTIFYDELETE_KF, OnDeleteKF)
	ON_MESSAGE(FCM_NOTIFYSELECT_TIME, OnSelectTime)
	ON_MESSAGE(FCM_NOTIFYMOVE_KF, OnMoveKF)

	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMovie message handlers

BOOL CDlgMovie::OnInitDialog() 
{
	CDialog::OnInitDialog();

	Initialize();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMovie::Initialize()
{
	m_nSelectedTime = -1;
	m_nSelectedMovie = -1;
	m_bNewMovie = FALSE;

	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DELETEMOVIE, FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_RECORDMOVIE, FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_PREVIEWMOVIE, FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_PREVIEWMOVIEWITHLOGS, FALSE);

	CRect rcClient;
	GetClientRect(&rcClient);

	DWORD dwStyle = m_lstMovies.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_lstMovies.SetExtendedStyle( dwStyle );

	m_lstMovies.m_pDoc = m_pDoc;

	m_lstMovies.InsertColumn(0, "Name", LVCFMT_LEFT, rcClient.Width()/4);
	m_lstMovies.InsertColumn(1, "Number of Keyframes", LVCFMT_LEFT, rcClient.Width()/5);
	m_lstMovies.InsertColumn(2, "Animation Speed", LVCFMT_LEFT, rcClient.Width()/5);
	m_lstMovies.InsertColumn(3, "Movie Run Time", LVCFMT_LEFT, rcClient.Width()/5);

	m_bAutoGoto = FALSE;
	m_bRequestCancel = FALSE;

//	m_ctrlKeyFrames.SetTimeRange(0, 86400 * 3);
	CStartDate startDate ;
	startDate =m_pDoc->GetTerminal().flightSchedule->GetStartDate();
	m_ctrlKeyFrames.SetStartDate(startDate);

	//modify by adam 2007-05-30
	//if(startDate.IsAbsoluteDate())
	//{
	//	CStartDate endDate = m_pDoc->GetTerminal().flightSchedule->GetEndDate();
 //       
	//	COleDateTimeSpan span = endDate.GetDate() - startDate.GetDate();
	//	int nHours =(int) span.GetTotalHours();
	//	nHours = nHours < 24 ? 24 : nHours+24;
	//	m_ctrlKeyFrames.SetTimeRange(0, nHours*60*60);	
	//}
	//else
	//{
	//	m_ctrlKeyFrames.SetTimeRange(0, 86400 * 3);	
	//}

	CStartDate endDate = m_pDoc->GetTerminal().flightSchedule->GetEndDate();

	COleDateTimeSpan span = endDate.GetDate() - startDate.GetDate();
	int nHours =(int) span.GetTotalHours();
	nHours = nHours < 24 ? 24 : nHours+24;
	m_ctrlKeyFrames.SetTimeRange(0, nHours*60*60);
	//End modify by adam 2007-05-30

	//m_ctrlKeyFrames.SetTimeRange(0, 4320);
	//m_ctrlKeyFrames.SetTimeRange(m_pDoc->m_animData.nFirstEntryTime/100, m_pDoc->m_animData.nLastExitTime/100);
	m_ctrlKeyFrames.SetTimeScale(43200,TRUE,TRUE);
	UpdateMovieList();

	//m_pView->m_bAnimDirectionChanged=TRUE;
}

void CDlgMovie::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rcClient;
	GetClientRect(&rcClient);


	// Do not call CDialog::OnPaint() for painting messages
}

void CDlgMovie::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	CRect rcBtn, rcTB, rc;
	CWnd* pWnd = GetDlgItem(IDOK);
	pWnd->GetWindowRect(&rcBtn);

	m_toolbarcontenter.GetWindowRect(&rcTB);

	int nHeight1 = cy-40-(TB_BARHEIGHT+30)-rcBtn.Height()-rcTB.Height();
	m_btnBarFrame.MoveWindow(10,10,cx-20,nHeight1);
	m_toolbarcontenter.MoveWindow(12,16,cx-50,rcTB.Height());
	m_toolbarcontenter.GetWindowRect( &rcTB );
	ScreenToClient(&rcTB);
	m_toolbarcontenter.ShowWindow( SW_HIDE );
	m_ToolBar.MoveWindow(&rcTB);
	m_btnBarFrame.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.top+=rcTB.Height()+5;
	m_lstMovies.MoveWindow(&rc);

	int nHeight2 = rcTB.Height()+TB_BARHEIGHT+30;
	m_ctrlKeyFrames.MoveWindow(10,nHeight1+20,cx-20,nHeight2);

	pWnd = GetDlgItem(IDOK);
	pWnd->MoveWindow((cx/2)-rcBtn.Width()-5, nHeight1+nHeight2+30, rcBtn.Width(), rcBtn.Height());
	pWnd = GetDlgItem(IDCANCEL);
	pWnd->MoveWindow((cx/2)+5, nHeight1+nHeight2+30, rcBtn.Width(), rcBtn.Height());
}

int CDlgMovie::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_MOVIETOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	
	return 0;
}

void CDlgMovie::UpdateMovieList()
{
	m_lstMovies.DeleteAllItems();

	int nMovieCount = m_pDoc->m_movies.GetMovieCount();
	for(int i=0; i<nMovieCount; i++) {
		m_lstMovies.InsertMovie(i);// InsertItem(LVIF_TEXT | LVIF_PARAM, i, LPSTR_TEXTCALLBACK, 0, 0, 0, (LPARAM) i);
	}

	m_nSelectedMovie = -1;
	UpdateKeyFrames();
}

void CDlgMovie::UpdateKeyFrames()
{
	
	m_ctrlKeyFrames.DeleteAllKeyFrames();
	if(m_nSelectedMovie != -1) {
		m_ctrlKeyFrames.EnableCtrl(TRUE);
		CMovie* pMovie = m_pDoc->m_movies.GetMovie(m_nSelectedMovie);
		m_ctrlKeyFrames.SetMovie(pMovie);
		int nCount = pMovie->GetKFCount();
		for(int i=0; i<nCount; i++) {
			void* pVoid=m_ctrlKeyFrames.InsertKeyFrame(pMovie->GetKFTime(i));
			pMovie->SetRelationKeyFrame(i,pVoid);
		}
	}
	else {
		m_ctrlKeyFrames.SetMovie(NULL);
		m_ctrlKeyFrames.EnableCtrl(FALSE);
	}

	m_ctrlKeyFrames.Refresh();
}

void CDlgMovie::OnAddMovie()
{
	int c = m_pDoc->m_movies.GetMovieCount();
	m_pDoc->m_movies.AddMovie("undefined");
	m_bNewMovie = TRUE;
	m_lstMovies.InsertMovie(c);// InsertItem(LVIF_TEXT | LVIF_PARAM, c, LPSTR_TEXTCALLBACK, 0, 0, 0, (LPARAM) c);
	m_lstMovies.SetItemState(c, LVIS_SELECTED, LVIS_SELECTED);
	m_lstMovies.SetFocus();
	m_lstMovies.EditLabel(c);
}

void CDlgMovie::OnDeleteMovie()
{
	ASSERT(m_nSelectedMovie != -1);
	m_pDoc->m_movies.DeleteMovie(m_nSelectedMovie);
	UpdateMovieList();
}

void CDlgMovie::OnPreviewMovieWithAnimation()
{
	if(m_pDoc->m_eAnimState == CTermPlanDoc::anim_none) {
		if(!m_pDoc->StartAnimation(FALSE))
			return;
	}
	m_pDoc->m_eAnimState = CTermPlanDoc::anim_pause;
	OnPreviewMovie();
}

void CDlgMovie::OnPreviewMovie()
{
	ShowWindow(SW_HIDE);

	if(m_nSelectedMovie == -1)
		return;

	m_bRequestCancel = FALSE;

	CMovie* pMovie = m_pDoc->m_movies.GetMovie(m_nSelectedMovie);

	CAnimationManager::PreviewMovie(pMovie,m_pDoc,&m_bRequestCancel);

	ShowWindow(SW_SHOW);
}

void CDlgMovie::OnRecordMovie()
{
	CMovie* pMovie = m_pDoc->m_movies.GetMovie(m_nSelectedMovie);
	if (pMovie && pMovie->GetKFCount())
	{
		CDlgMovieWriter dlg(m_pDoc, CDlgMovieWriter::MOVIE, m_nSelectedMovie, this);
		dlg.DoFakeModal();
	}
	
}

LRESULT CDlgMovie::OnInsertKF(WPARAM wParam, LPARAM lParam)
{
	IRender3DView* pView= GetCur3DView();
	if(!pView)
		return FALSE;

	long nKFTime = (long) wParam * 100; 
	void* pVoid=(void*)lParam;
	
	if(m_nSelectedMovie != -1) {	
		int nIndex=m_pDoc->m_movies.GetMovie(m_nSelectedMovie)->InsertKF(*pView->GetCameraData(), nKFTime);
		m_pDoc->m_movies.GetMovie(m_nSelectedMovie)->SetRelationKeyFrame(nIndex,pVoid);
		m_lstMovies.Update(m_nSelectedMovie);
	}
	return TRUE;
}

LRESULT CDlgMovie::OnDeleteKF(WPARAM wParam, LPARAM lParam)
{
	long nKFTime = (long) wParam * 100;
	if(m_nSelectedMovie != -1) {
		m_pDoc->m_movies.GetMovie(m_nSelectedMovie)->DeleteKF(nKFTime);
		m_lstMovies.Update(m_nSelectedMovie);
	}
	return TRUE;
}

LRESULT CDlgMovie::OnMoveKF(WPARAM wParam, LPARAM lParam)
{
	long nPrevKFTime = (long) wParam * 100;
	long nNewKFTime = (long) lParam * 100;
	
	if(m_nSelectedMovie != -1) {	
		VERIFY(m_pDoc->m_movies.GetMovie(m_nSelectedMovie)->MoveKF(nPrevKFTime, nNewKFTime)>=0);
		//ASSERT(0); //NOT YET IMPLEMENTED
	}
	return TRUE;
}

LRESULT CDlgMovie::OnSelectTime(WPARAM wParam, LPARAM lParam)
{
	IRender3DView* pView= GetCur3DView();
	if(!pView)
		return FALSE;


	long nTime = ((long) wParam) * 100;
	BOOL bAutoGoto = (BOOL) lParam;

	if((m_nSelectedMovie != -1) && bAutoGoto) {
		C3DCamera cam;
		m_pDoc->m_movies.GetMovie(m_nSelectedMovie)->Interpolate(nTime, cam);
		pView->SetCameraData(cam);
	}
	else {
		
	}

	long nLastAnimTime = m_pDoc->m_animData.nLastAnimTime;
	m_pDoc->m_animData.nLastAnimTime = nTime;

	int nLastAnimDirection = pView->m_nAnimDirection;
	pView->m_nAnimDirection = nTime>nLastAnimTime?CEventLogBufManager::ANIMATION_FORWARD:CEventLogBufManager::ANIMATION_BACKWARD;
	if(nLastAnimDirection!=pView->m_nAnimDirection)
		pView->m_bAnimDirectionChanged=TRUE;
	
	pView->Invalidate(FALSE);
	return TRUE;
}

void CDlgMovie::OnGetDispInfoListMovies(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	CString sName;

	if(pDispInfo->item.mask & LVIF_TEXT) {
		int idx = (int) pDispInfo->item.lParam;
		switch(pDispInfo->item.iSubItem) {
		case 0:
			lstrcpy(pDispInfo->item.pszText, m_pDoc->m_movies.GetMovie(idx)->GetName());
			break;
		case 1:
			sprintf(pDispInfo->item.pszText, "%d", m_pDoc->m_movies.GetMovie(idx)->GetKFCount());
			break;
		default:
			ASSERT(0);
			break;
		}
	}	
	
	*pResult = 0;

}

void CDlgMovie::OnEndLabelEditListMovies(NMHDR* pNMHDR, LRESULT* pResult)
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
	
	int idxMovie = (int) m_lstMovies.GetItemData(nIdx);

	m_pDoc->m_movies.GetMovie(idxMovie)->SetName(pDispInfo->item.pszText);

	//m_lstMovies.SetItemText(nIdx,0,pDispInfo->item.pszText);

	UpdateKeyFrames();

	*pResult =1;
}

void CDlgMovie::OnItemChangedListMovies(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;
	
	if ((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
		return;

	if(pnmv->uOldState & LVIS_SELECTED) {
		if(!(pnmv->uNewState & LVIS_SELECTED)) {
			// TRACE("Unselected Item %d...\n", pnmv->iItem);
		}
	}
	else if(pnmv->uNewState & LVIS_SELECTED) {
		//a new item has been selected
		// TRACE("Selected Item %d...\n", pnmv->iItem);
		m_nSelectedMovie = (int) m_lstMovies.GetItemData(pnmv->iItem);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DELETEMOVIE, TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_RECORDMOVIE, TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_PREVIEWMOVIE, TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_PREVIEWMOVIEWITHLOGS, TRUE);
	}
	else
		ASSERT(0);

	UpdateKeyFrames();

}

void CDlgMovie::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgMovie::OnLButtonUp(UINT nFlags, CPoint point) 
{
	
	CDialog::OnLButtonUp(nFlags, point);
}

void CDlgMovie::OnMouseMove(UINT nFlags, CPoint point) 
{
	CDialog::OnMouseMove(nFlags, point);
}

void CDlgMovie::SelectTime(long _time)
{
	IRender3DView * pView = GetCur3DView();
	if(!pView)
		return;

	if(_time < m_pDoc->m_animData.nFirstEntryTime)
		_time = m_pDoc->m_animData.nFirstEntryTime;
	else if(_time > m_pDoc->m_animData.nLastExitTime)
		_time = m_pDoc->m_animData.nLastExitTime;

	m_nSelectedTime = _time;

	m_pDoc->m_animData.nLastAnimTime = _time;


	//m_nSelectedKF = -1;
	if(m_nSelectedMovie != -1) {
		int nKFCount = m_pDoc->m_movies.GetMovie(m_nSelectedMovie)->GetKFCount();
		//for(int i=0; i<nKFCount; i++) {
		//	if(m_pDoc->m_movies.GetMovie(m_nSelectedMovie)->GetKFTime(i) == _time) {
		//		m_nSelectedKF = i;
		//		break;
		//	}
		//}
		if(nKFCount > 0 && m_ctrlKeyFrames.m_bAutoGoto) {
			C3DCamera cam;
			m_pDoc->m_movies.GetMovie(m_nSelectedMovie)->Interpolate(m_nSelectedTime,cam);			
			pView->Invalidate(FALSE);
		}
	}
	pView->Invalidate(FALSE);

	//InvalidateRect(m_rcTimeline);
}

void CDlgMovie::OnOK() 
{
	m_pDoc->m_movies.saveDataSet(m_pDoc->m_ProjInfo.path, false);
	
	CDialog::OnOK();
}

void CDlgMovie::OnCancel() 
{
	m_pDoc->m_movies.loadDataSet(m_pDoc->m_ProjInfo.path);
	
	CDialog::OnCancel();
}

void CDlgMovie::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CDialog::OnLButtonDblClk(nFlags, point);
}

BOOL CDlgMovie::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
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

BOOL CDlgMovie::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) {
		m_bRequestCancel = TRUE;
		return TRUE;
	}
	else
		return CDialog::PreTranslateMessage(pMsg);
}

IRender3DView* CDlgMovie::GetCur3DView()
{
	//if(m_pDoc->m_animData.m_AnimationModels.IsOnBoardSel() || m_pDoc->m_animData.m_AnimationModels.IsLandsideSel())
	if(m_pDoc->m_systemMode == EnvMode_OnBoard|| m_pDoc->m_systemMode == EnvMode_LandSide)
	{
		CRender3DView* pRender3DView = m_pDoc->GetRender3DView();
		return pRender3DView;
	}
	else
	{
		C3DView* p3DView = m_pDoc->Get3DView();
		if (p3DView)
		{
			return p3DView;
		}
	}
	return NULL;

}


/////////////////////////////////////////////////////////////////////////////
// CMovieListCtrl

static const int ANIMSPEEDVALUES[] = {
	10,
	50,
	100,
	250,
	500,
	1000,
};

static const TCHAR* ANIMSPEEDLABELS[] = {
	"1x",
	"5x",
	"10x",
	"25x",
	"50x",
	"100x",
};

static int AnimSpeedToIdx(int nAnimSpeed) {
	int c = sizeof(ANIMSPEEDLABELS)/sizeof(int);
	for(int i=0; i<c; i++) {
		if(nAnimSpeed == ANIMSPEEDVALUES[i])
			return i;
	}
	return -1;
}

CMovieListCtrl::CMovieListCtrl()
{
	m_pDoc = NULL;
}

CMovieListCtrl::~CMovieListCtrl()
{

}

BEGIN_MESSAGE_MAP(CMovieListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CMovieListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()


void CMovieListCtrl::DrawItem( LPDRAWITEMSTRUCT lpdis ) 
{
	COLORREF clrTextSave, clrBkSave;
	CRect rcItem(lpdis->rcItem);
	CDC* pDC = CDC::FromHandle(lpdis->hDC);
	int nItem = lpdis->itemID;
	LVITEM lvi; 
    lvi.mask = LVIF_PARAM | LVIF_STATE; 
    lvi.iItem = nItem;
    lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;
    this->GetItem(&lvi); 
	
	BOOL bFocus = (GetFocus() == this);
	BOOL bSelected = (lvi.state & LVIS_SELECTED) || (lvi.state & LVIS_DROPHILITED);
	
	CRect rcAllLabels; 
    this->GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS); 
    CRect rcLabel; 
    this->GetItemRect(nItem, rcLabel, LVIR_LABEL);
	CRect rcKF;
	this->GetSubItemRect( nItem,1,LVIR_LABEL,rcKF );
	CRect rcAnimSpeed;
	this->GetSubItemRect( nItem,2,LVIR_LABEL,rcAnimSpeed );
	CRect rcRunTime;
	this->GetSubItemRect( nItem,3,LVIR_LABEL,rcRunTime );

	rcAllLabels.left = rcLabel.left;

    if(bSelected) { 
        clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
        clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
        pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT))); 
    } 
    else {
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
        pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_WINDOW)));
	}

	ASSERT(m_pDoc);
	int nMovieIdx = lvi.lParam;
	CMovie* pMovie = m_pDoc->m_movies.GetMovie(nMovieIdx);
	CString s;

	rcLabel.left+=5;
	//draw name
	pDC->DrawText(pMovie->GetName() , rcLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
	
	//draw # KF
	s.Format(" %d", pMovie->GetKFCount());
	pDC->DrawText(s, rcKF, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
	
	//draw anim speed
	s.Format(" %dx", pMovie->GetAnimSpeed()/10);
    pDC->DrawText( s, rcAnimSpeed, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

	//draw run time
	if(pMovie->GetKFCount()>0) {
		pDC->DrawText( pMovie->GetKFRunTimeFormated(pMovie->GetKFCount()-1), rcRunTime, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
	}
	else {
		pDC->DrawText( _T("00:00:00"), rcRunTime, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
	}
	
	if (lvi.state & LVIS_FOCUSED) 
        pDC->DrawFocusRect(rcAllLabels); 
	
    if (bSelected) {
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

void CMovieListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint ptMsg = GetMessagePos();
	this->ScreenToClient( &ptMsg );
	NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
	int nItem = pNMLW->iItem;
	if(nItem != -1) 
	{
		ASSERT(m_pDoc);
		int nMovieIdx = GetItemData(nItem);
		CMovie* pMovie = m_pDoc->m_movies.GetMovie(nMovieIdx);
		
		Invalidate();
	}
	
	*pResult = 0;
}

void CMovieListCtrl::InsertMovie(int _nIdx)
{
	ASSERT(m_pDoc);
	int nIdx = InsertItem( _nIdx , m_pDoc->m_movies.GetMovie(_nIdx)->GetName());
	SetItemData( nIdx, (DWORD)_nIdx );
	ASSERT(nIdx == _nIdx);
}


void CMovieListCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint ptMsg = GetMessagePos();
	this->ScreenToClient( &ptMsg );
	NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
	int nItem = pNMLW->iItem;
	if(nItem != -1) 
	{
		ASSERT(m_pDoc);
		int nMovieIdx = GetItemData(nItem);
		CMovie* pMovie = m_pDoc->m_movies.GetMovie(nMovieIdx);
		if(pNMLW->iSubItem == 2) 
		{// anim speed column
			CRect rcAnimSpeed;
			this->GetSubItemRect( nItem, 2, LVIR_LABEL, rcAnimSpeed);
			int nHeight = rcAnimSpeed.Height();
			rcAnimSpeed.bottom += nHeight * 5;
		
			m_nAnimSpeedSelItem = nItem;
			if (!m_AnimSpeedComboBox.GetSafeHwnd())
			{
				DWORD dwStyle = WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST|CBS_DISABLENOSCROLL;
				
				m_AnimSpeedComboBox.Create( dwStyle, rcAnimSpeed, this, IDC_COMBO_LEVEL );
				m_AnimSpeedComboBox.SetItemHeight( -1, nHeight );
				m_AnimSpeedComboBox.SetHorizontalExtent( 300 );
				COMBOBOXEXITEM cbItem;
				cbItem.mask = CBEIF_TEXT;
				TCHAR tmpstr[256];
				for(int i=_1X; i<=_100X; i++) {
					cbItem.iItem = i;
					cbItem.pszText = _tcscpy(tmpstr, ANIMSPEEDLABELS[i]);
					m_AnimSpeedComboBox.InsertItem( &cbItem );
				}
			}
			else
			{
				m_AnimSpeedComboBox.MoveWindow(&rcAnimSpeed);
			}

			

			m_AnimSpeedComboBox.SetCurSel( AnimSpeedToIdx(pMovie->GetAnimSpeed()) );
			m_AnimSpeedComboBox.ShowWindow(SW_SHOW);
			m_AnimSpeedComboBox.BringWindowToTop();
			m_AnimSpeedComboBox.SetFocus();		
		}
		Invalidate();
	}
	*pResult = 0;
}

int CMovieListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	
	return 0;
}

LRESULT CMovieListCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_INPLACE_COMBO)
	{
		if(GetItemCount()==0) return CListCtrl::DefWindowProc(message, wParam, lParam);
		int nIdx = (int) lParam;
		UINT nID = (UINT) wParam;

		if(nID == 0) { //anim speed
			ASSERT(m_pDoc);
			int nMovieIdx = GetItemData(m_nAnimSpeedSelItem);
			CMovie* pMovie = m_pDoc->m_movies.GetMovie(nMovieIdx);
			pMovie->SetAnimSpeed( ANIMSPEEDVALUES[nIdx] );
		}
		SetFocus();
		return TRUE;
	}
	
	return CListCtrl::DefWindowProc(message, wParam, lParam);
}
