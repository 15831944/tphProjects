// PipeShowListBox.cpp : implementation file
//

#include "stdafx.h"
#include "PipeShowListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPipeShowListBox

CPipeShowListBox::CPipeShowListBox()
{
}

CPipeShowListBox::~CPipeShowListBox()
{
}


BEGIN_MESSAGE_MAP(CPipeShowListBox, CCheckListBox)
	//{{AFX_MSG_MAP(CPipeShowListBox)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPipeShowListBox message handlers

void CPipeShowListBox::OnKillFocus(CWnd* pNewWnd) 
{
	CCheckListBox::OnKillFocus(pNewWnd);
	if(pNewWnd!=GetParent())
		GetParent()->ShowWindow(SW_HIDE);
		
}
