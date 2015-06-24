// BmpBtnDropList.cpp: implementation of the CBmpBtnDropList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "BmpBtnDropList.h"
#include "Common\ProjectManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define IDC_COMBOLIST_BUTTON 4317         

CBmpBtnDropList::CBmpBtnDropList()
{
	m_nShapeSetIndex = -1;
}

CBmpBtnDropList::~CBmpBtnDropList()
{

}

BEGIN_MESSAGE_MAP(CBmpBtnDropList, CBitmapButton)
	//{{AFX_MSG_MAP(CBmpBtnDropList)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CBmpBtnDropList::DisplayListWnd()
{
	if(!m_listWnd.GetSafeHwnd() ) return;
	
	CRect rect(0,0,200,200);
	CalculateDroppedRect (&rect);
	m_listWnd.SetWindowPos (&wndNoTopMost, rect.left, rect.top,
		                 LISTWND_WIDTH,LISTWND_HEIGHT, SWP_SHOWWINDOW );
	m_listWnd.SetFocus();

}

BOOL CBmpBtnDropList::Create(CRect rect, UINT nID, CWnd *pParent)
{
	 CreateEx( 0, _T("BUTTON"), NULL,
		  BS_PUSHBUTTON | BS_NOTIFY | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
		  rect, pParent, IDC_COMBOLIST_BUTTON);

	LoadBitmaps (_T("CBU"), _T("CBD"));

	MoveWindow (&rect);
	SizeToContent ();

	return TRUE;

}

void CBmpBtnDropList::SetShapeSetIndex(int nShapeSetIndex)
{
	m_nShapeSetIndex = nShapeSetIndex;
}

void CBmpBtnDropList::CalculateDroppedRect(LPRECT lpDroppedRect)
{
	_ASSERTE (lpDroppedRect);

	if (!lpDroppedRect)
	{
		return;
	}

	CRect rectCombo;
	GetWindowRect(&rectCombo);

	//adjust to either the top or bottom
	int DropTop = rectCombo.bottom;
	int ScreenHeight = GetSystemMetrics (SM_CYSCREEN);
	if ((DropTop + 200) > ScreenHeight)
	{
		DropTop = rectCombo.top - 200;
	}

	//adjust to either the right or left
	int DropLeft = rectCombo.left;
	int ScreenWidth = GetSystemMetrics (SM_CXSCREEN);
	if ((DropLeft + 200) > ScreenWidth)
	{
		DropLeft = rectCombo.right - 200;
	}

	lpDroppedRect->left  = DropLeft;
	lpDroppedRect->top   = DropTop;
	lpDroppedRect->bottom = DropTop +400;
	lpDroppedRect->right  = DropLeft + 350;

}

int CBmpBtnDropList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_listWnd.SetShapeItem(m_nShapeSetIndex);
	CreateListWnd();
	return 0;
}


void CBmpBtnDropList::HideCtrl()
{
	ShowWindow(SW_HIDE);
}

BOOL CBmpBtnDropList::CreateListWnd()
{
	CRect listRect (0,0, 200, 200);

	//can't have a control Id with WS_POPUP style
	LPCTSTR className=AfxRegisterWndClass(CS_DBLCLKS, 
		AfxGetApp()->LoadStandardCursor(IDC_ARROW), (HBRUSH)GetStockObject(LTGRAY_BRUSH), 0);
    if (! m_listWnd.CreateEx(
						0, 
						className, NULL,
						WS_POPUP | WS_BORDER  ,
						listRect, this, 0, NULL) )
	{
		return FALSE;
	}
	
	m_listWnd.SetParentWnd( this );
	m_listWnd.SetFocus();

	
	return TRUE;

}
