// UnselectableListBox.cpp : implementation file
//

#include "stdafx.h"
#include "UnselectableListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnselectableListBox

CUnselectableListBox::CUnselectableListBox()
{
}

CUnselectableListBox::~CUnselectableListBox()
{
}


BEGIN_MESSAGE_MAP(CUnselectableListBox, CListBox)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CUnselectableListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int nItem = GetCurSel();
	CRect rt;
	if (GetItemRect(nItem, rt) != LB_ERR)
	{
		if(rt.PtInRect( point ))
		{
			SetCurSel(-1);
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)m_hWnd);
			return;
		}
	}

	CListBox::OnLButtonDown(nFlags, point);
}


