#include "stdafx.h"
#include "termplan.h"
#include "PipeBar.h"

/////////////////////////////////////////////////////////////////////////////
// CPipeBar

CPipeBar::CPipeBar()
{
}

CPipeBar::~CPipeBar()
{
}

BEGIN_MESSAGE_MAP(CPipeBar, CToolBar24X24)		
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT_EX(TBN_DROPDOWN, OnDropDownPipe)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPipeBar message handlers

int CPipeBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBar24X24::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//Create m_wndPipeShow
	if(!m_DropPipeWnd.CreateEx(NULL, _T("STATIC"),"",
						   WS_BORDER|WS_POPUP,CRect(0,0,PIPEDROPWND_WIDTH,PIPEDROPWND_HEIGHT),
						   GetParent(),0))
	{
		// TRACE("Faile to create PipeShow window!\n");
		return -1;
	}
	
	if(!m_DropAirRouteWnd.CreateEx(NULL, _T("STATIC"),"", WS_BORDER|WS_POPUP,CRect(0,0,PIPEDROPWND_WIDTH,PIPEDROPWND_HEIGHT), GetParent(),0))
		return -1;
	
	if(!m_DropAllProcessorTypeWnd.CreateEx(NULL, _T("STATIC"),"", WS_BORDER|WS_POPUP,CRect(0,0,PIPEDROPWND_WIDTH,PIPEDROPWND_HEIGHT), GetParent(),0))
		return -1;
	InitImageList(IDB_TOOLBAR_PIPE_NORMAL, IDB_TOOLBAR_PIPE_HOT, IDB_TOOLBAR_PIPE_DISABLE,
		RGB(255,0,255));
	AppendImageList(IDB_TOOLBAR_A_NORMAL,IDB_TOOLBAR_A_HOT,IDB_TOOLBAR_A_DISABLE,
		RGB(255,0,255));
	AppendImageList(IDB_TOOLBAR_T_NORMAL,IDB_TOOLBAR_T_HOT,IDB_TOOLBAR_T_DISABLE,
		RGB(255,0,255));
	SetImageList();
	

	TBBUTTON pButtonInfo[5]; 

	for(int i=0;i<5;i++)
		CToolBar24X24::InitTBBUTTON(pButtonInfo[i]);
	
	pButtonInfo[0].iBitmap = 0;
	pButtonInfo[0].idCommand = ID_BUT_PIPEBAR;
	pButtonInfo[0].fsStyle =BTNS_WHOLEDROPDOWN;
	
	pButtonInfo[1].iBitmap = 0;
	pButtonInfo[1].idCommand = 0;
	pButtonInfo[1].fsStyle =BTNS_SEP;

	pButtonInfo[2].iBitmap = 1;
	pButtonInfo[2].idCommand = ID_BUT_AIROUTE;
	pButtonInfo[2].fsStyle = BTNS_WHOLEDROPDOWN;

	pButtonInfo[3].iBitmap = 0;
	pButtonInfo[3].idCommand = 0;
	pButtonInfo[3].fsStyle =BTNS_SEP;


	pButtonInfo[4].iBitmap = 2;
	pButtonInfo[4].idCommand = ID_BUT_ALLPROCESSORTYPE;
	pButtonInfo[4].fsStyle = BTNS_WHOLEDROPDOWN;

	AddButtons(5,pButtonInfo);
	m_nCountNotButton = 1;
	
	TBBUTTON tbb;
	CToolBar24X24::InitTBBUTTON(tbb) ;
	tbb.iBitmap = -1;
	tbb.idCommand =TEMP_ID_FOR_NOT_BUTTON+m_nCountNotButton;
	tbb.fsState &=~ TBSTATE_ENABLED;
	AddButton(&tbb,0,TRUE);
	

	SetWindowText (_T ("Display Override"));
	EnableDocking(CBRS_ALIGN_ANY);

	return 0;
}

BOOL CPipeBar::OnDropDownPipe(NMHDR* pNMHDR, LRESULT* pResult)
{
	if(GetToolBarCtrl().IsButtonPressed(ID_BUT_PIPEBAR))
	{
		CRect rItem, rTB;
		GetItemRect(0, &rItem);
		GetClientRect(&rTB);
		ClientToScreen(&rTB);
		rItem.OffsetRect(rTB.left, rTB.top);
		
		m_DropPipeWnd.ShowPipeWnd(CPoint(rItem.left, rItem.bottom)) ;
	}
	else if (GetToolBarCtrl().IsButtonPressed(ID_BUT_AIROUTE))
	{
		CRect rItem, rTB;
		GetItemRect(2, &rItem);
		GetClientRect(&rTB);
		ClientToScreen(&rTB);
		rItem.OffsetRect(rTB.left, rTB.top);

		m_DropAirRouteWnd.ShowAirRouteWnd(CPoint(rItem.left, rItem.bottom)) ;
	}
	else if (GetToolBarCtrl().IsButtonPressed(ID_BUT_ALLPROCESSORTYPE))
	{
		CRect rItem, rTB;
		GetItemRect(4, &rItem);
		GetClientRect(&rTB);
		ClientToScreen(&rTB);
		rItem.OffsetRect(rTB.left, rTB.top);

		m_DropAllProcessorTypeWnd.ShowAllProcessorTypeWnd(CPoint(rItem.left, rItem.bottom)) ;
	}
	return TRUE;
}
