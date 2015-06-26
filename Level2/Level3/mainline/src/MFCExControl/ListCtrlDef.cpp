// ListCtrlDef.cpp : implementation file
//

#include "stdafx.h"
#include "ListCtrlDef.h"


// CListCtrlDef

IMPLEMENT_DYNAMIC(CListCtrlDef, CListCtrlEx)
CListCtrlDef::CListCtrlDef()
{
}

CListCtrlDef::~CListCtrlDef()
{
}


BEGIN_MESSAGE_MAP(CListCtrlDef, CListCtrlEx)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CListCtrlDef::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int nIndex = -1, nColumn = -1;
	if((nIndex = HitTestEx(point, &nColumn)) != -1)
	{
		if(nIndex == 0 && nColumn == 0)
			return ;
	}

	CListCtrlEx::OnLButtonDblClk(nFlags, point);
}


// CListCtrlDef message handlers

