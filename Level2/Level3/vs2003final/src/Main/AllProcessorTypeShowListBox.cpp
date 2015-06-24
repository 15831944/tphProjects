// PipeShowListBox.cpp : implementation file
//

#include "stdafx.h"
#include "AllProcessorTypeShowListBox.h"
#include ".\allprocessortypeshowlistbox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAllProcessorTypeShowListBox

CAllProcessorTypeShowListBox::CAllProcessorTypeShowListBox()
{
}

CAllProcessorTypeShowListBox::~CAllProcessorTypeShowListBox()
{
}


BEGIN_MESSAGE_MAP(CAllProcessorTypeShowListBox, CCheckListBox)
	//{{AFX_MSG_MAP(CAllProcessorTypeShowListBox)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	ON_WM_DRAWITEM_REFLECT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAllProcessorTypeShowListBox message handlers

void CAllProcessorTypeShowListBox::OnKillFocus(CWnd* pNewWnd) 
{
	CCheckListBox::OnKillFocus(pNewWnd);
	if(pNewWnd!=GetParent())
		GetParent()->ShowWindow(SW_HIDE);

}
