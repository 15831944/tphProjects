// EditHelper.cpp : implementation file
//

#include "stdafx.h"
#include "EditHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditHelper

CEditHelper::CEditHelper()
{
}

CEditHelper::~CEditHelper()
{
}


BEGIN_MESSAGE_MAP(CEditHelper, CEdit)
	//{{AFX_MSG_MAP(CEditHelper)
	ON_WM_KEYUP()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditHelper message handlers

void CEditHelper::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch( nChar )
	{
	case VK_RETURN:
		GetParent()->SendMessage( WM_APP_ED_EDIT_FINISHED, (WPARAM)TRUE );	// Commit changes
	break;

	case VK_ESCAPE:
		GetParent()->SendMessage( WM_APP_ED_EDIT_FINISHED, (WPARAM)FALSE );	// Disregard changes
	break;
	}
	
	CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CEditHelper::OnKillFocus(CWnd* pNewWnd) 
{
	GetParent()->SendMessage( WM_APP_ED_EDIT_FINISHED, (WPARAM)TRUE );	// Commit changes

	CEdit::OnKillFocus(pNewWnd);
}
