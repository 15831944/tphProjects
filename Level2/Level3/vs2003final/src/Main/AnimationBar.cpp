// AnimationBar.cpp: implementation of the CAnimationBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "AnimationBar.h"
#include "MainFrm.h"
#include "3dview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAnimationBar::CAnimationBar()
{

}

CAnimationBar::~CAnimationBar()
{

}

BEGIN_MESSAGE_MAP(CAnimationBar, CToolBar24X24)
	//{{AFX_MSG_MAP(CMainBar)
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_COMBO_PAXSELED,OnSelChangeSeledPaxComboBox)
	//}}AFX_MSG_MAP
	//ON_NOTIFY(TBN_DROPDOWN, ID_ANIMPICKSPEED, OnDropDownPickSpeed)
	ON_NOTIFY_REFLECT_EX(TBN_DROPDOWN,	 OnDropDownPickSpeed)
END_MESSAGE_MAP()

int CAnimationBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBar24X24::OnCreate(lpCreateStruct) == -1)
		return -1;

	//InitImageList(IDB_TOOLBAR_ANIMATION_NORMAL,IDB_TOOLBAR_ANIMATION_HOT,
	//	IDB_TOOLBAR_ANIMATION_DISABLE,RGB(255,0,255));
	InitImageList(IDB_TOOLBAR_ANIMATION_NORMAL,IDB_TOOLBAR_ANIMATION_HOT,
		IDB_TOOLBAR_ANIMATION_DISABLE,RGB(255,0,255));
	AppendImageList(IDB_TOOLBAR_ANIMATION_SPEED_NORMAL,IDB_TOOLBAR_ANIMATION_SPEED_HOT,
		IDB_TOOLBAR_ANIMATION_SPEED_DISABLE,RGB(255,0,255));
	AppendImageList(IDB_TOOLBAR_ANIMATION_PAX_NORMAL,IDB_TOOLBAR_ANIMATION_PAX_HOT,
		IDB_TOOLBAR_ANIMATION_PAX_DISABLE,RGB(255,0,255));
	AppendImageList(IDB_TOOLBAR_RUNDELTA_NORMAL,IDB_TOOLBAR_RUNDELTA_HOT,
		IDB_TOOLBAR_RUNDELTA_DISABLED,RGB(255,0,255));
	AppendImageList(IDB_TOOLBAR_FIRE_NORMAL,IDB_TOOLBAR_FIRE_HOT,
		IDB_TOOLBAR_FIRE_DISABLED,RGB(255,0,255));
	AppendImageList(IDB_TOOLBAR_HIDEACTAGS_NORMAL,IDB_TOOLBAR_HIDEACTAGS_HOT,
		IDB_TOOLBAR_HIDEACTAGS_DISABLED,RGB(255,0,255));
	AppendImageList(IDB_TOOLBAR_SCALE_NORMAL,IDB_TOOLBAR_SCALE_HOT,
		IDB_TOOLBAR_SCALE_DISABLE,RGB(255,0,255));
	SetImageList();
	
	static const int BUTTONCOUNT = 14;

	TBBUTTON pButtonInfo[BUTTONCOUNT]; 

	for(int i=0;i<BUTTONCOUNT;i++)
		CToolBar24X24::InitTBBUTTON(pButtonInfo[i]);

	i=0;
	pButtonInfo[i].fsState &=~TBSTATE_ENABLED;
	pButtonInfo[i].iBitmap = 0;
	pButtonInfo[i].idCommand = ID_ANIMREV;
	i++;
	pButtonInfo[i].iBitmap = 1;
	pButtonInfo[i].idCommand = ID_ANIMATION_START;
	i++;
	pButtonInfo[i].iBitmap = 2;
	pButtonInfo[i].idCommand = ID_ANIMPAUSE;
	i++;
	pButtonInfo[i].iBitmap = 3;
	pButtonInfo[i].idCommand = ID_ANIMSTOP;
	i++;
	pButtonInfo[i].iBitmap = 4;
	pButtonInfo[i].idCommand = ID_ANIMPICKTIME;
	i++;
	pButtonInfo[i].iBitmap = 7;
	pButtonInfo[i].idCommand = ID_ANIMPICKSPEED;
	pButtonInfo[i].fsStyle =BTNS_WHOLEDROPDOWN;
	i++;
	pButtonInfo[i].iBitmap = 0;
	pButtonInfo[i].idCommand = 0;
	pButtonInfo[i].fsStyle =BTNS_SEP;
	m_nCountNotButton++;
	i++;
	pButtonInfo[i].iBitmap = 13;
	pButtonInfo[i].idCommand = ID_PROJECT_RUNSIM;
	i++;
	pButtonInfo[i].iBitmap = 0;
	pButtonInfo[i].idCommand = 0;
	pButtonInfo[i].fsStyle =BTNS_SEP;
	m_nCountNotButton++;
	i++;
	pButtonInfo[i].iBitmap = 15;
	pButtonInfo[i].idCommand = ID_FIREIMPACT;
	i++;
	pButtonInfo[i].iBitmap = 0;
	pButtonInfo[i].idCommand = 0;
	pButtonInfo[i].fsStyle =BTNS_SEP;
	m_nCountNotButton++;
	i++;
	pButtonInfo[i].iBitmap = 16;
	pButtonInfo[i].idCommand = ID_ANIMATION_HIDEACTAGS;
	i++;
	pButtonInfo[i].iBitmap = 0;
	pButtonInfo[i].idCommand = 0;
	pButtonInfo[i].fsStyle =BTNS_SEP;
	m_nCountNotButton++;
	i++;
	pButtonInfo[i].iBitmap = 12;
	pButtonInfo[i].idCommand = ID_ANIMPAX;

	AddButtons(BUTTONCOUNT,pButtonInfo);

	AddTextComboBox(" ",&m_cbPax,CSize(170,160),IDC_COMBO_PAXSELED,CBS_DROPDOWNLIST);
	for(int j=0;j<5;j++)
	{
		TBBUTTON tbb;
		CToolBar24X24::InitTBBUTTON(tbb) ;
		tbb.iBitmap = -1;
		tbb.idCommand =TEMP_ID_FOR_NOT_BUTTON+m_nCountNotButton;
		tbb.fsState &=~ TBSTATE_ENABLED;
		AddButton(&tbb,0,TRUE);
	}

	TBBUTTON tbbb;
	CToolBar24X24::InitTBBUTTON(tbbb) ;
	tbbb.iBitmap = -1;
	tbbb.idCommand = 0;
	tbbb.fsStyle = TBSTYLE_SEP;
	tbbb.fsState = TBSTATE_INDETERMINATE;
	AddButton(&tbbb);

	TBBUTTON tb;
	CToolBar24X24::InitTBBUTTON(tb) ;
	tb.iBitmap = 17;
	tb.idCommand = ID_SCALE_SIZE;
	AddButton(&tb);
    SetWindowText (_T ("Animation"));
	EnableDocking(CBRS_ALIGN_ANY);
	
	return 0;
}

BOOL CAnimationBar::OnDropDownPickSpeed(NMHDR* pNMHDR, LRESULT* pResult)
{
	CNewMenu menu;

	menu.LoadMenu(IDR_CTX_PICKANIMSPEED);
	CNewMenu* pPopup = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));

	CRect rItem, rTB;
	CMainFrame* pMF=(CMainFrame*)AfxGetMainWnd();
	int idx = pMF->m_wndAnimationBar.CommandToIndex(ID_ANIMPICKSPEED);
	pMF->m_wndAnimationBar.GetItemRect(idx, &rItem);
	pMF->m_wndAnimationBar.GetClientRect(&rTB);
	pMF->m_wndAnimationBar.ClientToScreen(&rTB);
	rItem.OffsetRect(rTB.left, rTB.top);

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rItem.left, rItem.bottom, pMF);
	return TRUE;	
}

void CAnimationBar::OnSelChangeSeledPaxComboBox()
{
	int nSeled=m_cbPax.GetCurSel();
	DWORD nItemData=m_cbPax.GetItemData(nSeled);
	CMainFrame* pMF=(CMainFrame*)AfxGetMainWnd();
	CView* pView=pMF->GetActiveFrame()->GetActiveView();
	if(pView->IsKindOf(RUNTIME_CLASS(C3DView)))
	{
		C3DView* p3DView=(C3DView*)pView;
		p3DView->m_nPaxSeledID=nItemData;
	}
		
}

CComboBox* CAnimationBar::GetAnimPaxCB()
{
	return &m_cbPax;
}
