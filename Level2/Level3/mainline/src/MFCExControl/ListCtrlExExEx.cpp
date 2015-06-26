#include "StdAfx.h"
#include "ListCtrlExExEx.h"
#include "resource.h"


BEGIN_MESSAGE_MAP(CListCtrlExExEx, CListCtrlEx)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

CListCtrlExExEx::CListCtrlExExEx(void)
{	
}

CListCtrlExExEx::~CListCtrlExExEx(void)
{	
}


void CListCtrlExExEx::OnLButtonDown(UINT nFlags, CPoint point)
{	
	CListCtrlEx::OnLButtonDown(nFlags, point);
	CListCtrl::OnLButtonDown(nFlags, point);
}
