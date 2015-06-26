// SmartEdit.cpp : implementation file
//

#include "stdafx.h"
#include "common\WinMsg.h"
#include "SmartEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmartEdit

CSmartEdit::CSmartEdit( CString _csStr )
{
	m_csStr = _csStr;
}

CSmartEdit::~CSmartEdit()
{
}


BEGIN_MESSAGE_MAP(CSmartEdit, CEdit)
	//{{AFX_MSG_MAP(CSmartEdit)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmartEdit message handlers

void CSmartEdit::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	delete this;	
}
 
void CSmartEdit::OnKillfocus() 
{
	// TODO: Add your control notification handler code here
	PostMessage(WM_CLOSE, 0, 0);

	CString str;
	GetWindowText(str);
	
	GetParent()->SendMessage( WM_END_EDIT, (WPARAM)LPTSTR((LPCTSTR)str), (LPARAM)GetDlgCtrlID() );
	
}

BOOL CSmartEdit::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if(pMsg->wParam == VK_RETURN)
	{
		PostMessage(WM_CLOSE, 0, 0);
		return TRUE;
	}
	else if(pMsg->wParam == VK_ESCAPE)
	{
		PostMessage(WM_CLOSE, 0, 0);
		return FALSE;
	}
	
	return CEdit::PreTranslateMessage(pMsg);
}

int CSmartEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);

	SetWindowText( m_csStr );
	SetFocus();
	SetSel( 0, -1 );
	return 0;
	
}
