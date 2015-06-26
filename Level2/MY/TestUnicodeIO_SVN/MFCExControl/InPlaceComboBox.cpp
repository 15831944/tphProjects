// InPlaceComboBox.cpp : implementation file
//

#include "stdafx.h"
//#include "termplan.h"
#include "InPlaceComboBox.h"
#include "WinMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInPlaceComboBox

CInPlaceComboBox::CInPlaceComboBox()
{
}

CInPlaceComboBox::~CInPlaceComboBox()
{
}


BEGIN_MESSAGE_MAP(CInPlaceComboBox, CComboBox)
	//{{AFX_MSG_MAP(CInPlaceComboBox)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceComboBox message handlers

int CInPlaceComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CFont* font = GetParent()->GetFont();
	SetFont( font );
	
	return 0;
}

void CInPlaceComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	int nSelIdx = GetCurSel();
	ShowWindow(SW_HIDE);

	GetParent()->SendMessage(WM_INPLACE_COMBO, -1, (LPARAM)nSelIdx);
}

void CInPlaceComboBox::OnCloseup() 
{
	// TODO: Add your control notification handler code here
	GetParent()->SetFocus();	
}
