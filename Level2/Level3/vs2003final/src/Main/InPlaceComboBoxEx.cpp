// InPlaceComboBoxEx.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "InPlaceComboBoxEx.h"
#include "common\WinMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInPlaceComboBoxEx

CInPlaceComboBoxEx::CInPlaceComboBoxEx()
{
	m_nID = 0;
}

CInPlaceComboBoxEx::CInPlaceComboBoxEx(UINT nID)
{
	m_nID = nID;
}

CInPlaceComboBoxEx::~CInPlaceComboBoxEx()
{
}


BEGIN_MESSAGE_MAP(CInPlaceComboBoxEx, CComboBoxEx)
	//{{AFX_MSG_MAP(CInPlaceComboBoxEx)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceComboBoxEx message handlers

void CInPlaceComboBoxEx::OnCloseup() 
{
	// TODO: Add your control notification handler code here
	GetParent()->SetFocus();		
}

void CInPlaceComboBoxEx::OnKillfocus() 
{
	// TODO: Add your control notification handler code here
	int nSelIdx = GetCurSel();
	ShowWindow(SW_HIDE);

	GetParent()->SendMessage(WM_INPLACE_COMBO, (WPARAM)m_nID, (LPARAM)nSelIdx);	
}

int CInPlaceComboBoxEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBoxEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CFont* font = GetParent()->GetFont();
	SetFont( font );
	
	return 0;
}
