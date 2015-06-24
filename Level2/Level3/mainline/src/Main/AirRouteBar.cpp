#include "StdAfx.h"
#include "AirRouteBar.h"
#include "resource.h"

#define PIPEDROPWND_WIDTH 220
#define PIPEDROPWND_HEIGHT 152
#define PIPEDROPWND_TOOLBAR_HEIGHT 32

CAirRouteBar::CAirRouteBar()
{
}

CAirRouteBar::~CAirRouteBar()
{
}

BEGIN_MESSAGE_MAP(CAirRouteBar, CToolBar24X24)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT_EX(TBN_DROPDOWN, OnDropDownPipe)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPipeBar message handlers

int CAirRouteBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CToolBar24X24::OnCreate(lpCreateStruct) == -1)
		return -1;

	//Create m_wndPipeShow
	if(!m_DropWnd.CreateEx(NULL, _T("STATIC"),"", WS_BORDER|WS_POPUP,CRect(0,0,PIPEDROPWND_WIDTH,PIPEDROPWND_HEIGHT), GetParent(),0))
		return -1;

	InitImageList(IDB_TOOLBAR_PIPE_NORMAL, IDB_TOOLBAR_PIPE_HOT, IDB_TOOLBAR_PIPE_DISABLE, RGB(255,0,255));
	SetImageList();

	TBBUTTON pButtonInfo[2]; 
	for(int i=0;i<2;i++)
		CToolBar24X24::InitTBBUTTON(pButtonInfo[i]);

	pButtonInfo[0].iBitmap = 0;
	pButtonInfo[0].idCommand = ID_BUT_PIPEBAR;
	pButtonInfo[0].fsStyle = BTNS_WHOLEDROPDOWN;

	pButtonInfo[1].iBitmap = 0;
	pButtonInfo[1].idCommand = 0;
	pButtonInfo[1].fsStyle =BTNS_SEP;

	AddButtons(2,pButtonInfo);
	SetWindowText (_T ("Air Route"));
	EnableDocking(CBRS_ALIGN_ANY);
	return 0;
}

BOOL CAirRouteBar::OnDropDownPipe(NMHDR* pNMHDR, LRESULT* pResult)
{
	CRect rItem, rTB;
	GetItemRect(0, &rItem);
	GetClientRect(&rTB);
	ClientToScreen(&rTB);
	rItem.OffsetRect(rTB.left, rTB.top);

	m_DropWnd.ShowAirRouteWnd(CPoint(rItem.left, rItem.bottom)) ;
	return TRUE;
}
