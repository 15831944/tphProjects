// SpinTree.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "SpinTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpinTree

CSpinTree::CSpinTree()
{
	m_hEditedItem = NULL;
}

CSpinTree::~CSpinTree()
{
}


BEGIN_MESSAGE_MAP(CSpinTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CSpinTree)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpinTree message handlers

CEdit* CSpinTree::EditLabel(HTREEITEM hItem)
{
	if (hItem == NULL) return NULL;
	
	m_hEditedItem = hItem;
	
	// Set title and percent
	CString strTitle, strPercent;
	strTitle = GetItemText(hItem);
	strTitle.Remove(')'); strTitle.Remove('%');
	int n = strTitle.ReverseFind('(');
	if (n >= 0)
	{
		strTitle.Remove('(');
		while(strTitle.GetLength() > n)
		{
			strPercent += strTitle.GetAt(n);
			strTitle.Delete(n);
		}
		
		strTitle.TrimLeft(); strTitle.TrimRight();
		strPercent.TrimLeft(); strPercent.TrimRight();
	}
	m_Edit.SetTitle(strTitle);
	
	int iPercent = 0;
	if (strPercent.GetLength() > 0)
	{
		iPercent = atoi(strPercent.GetBuffer(0));
	}
	m_Edit.SetPercent(iPercent);

	// Create or move it
	int iH = 15;
	if( iPercent < 10 )
		iH = 27;
	else if( iPercent < 100 )
		iH = 21;

	CRect rcItem, rcEdit;
	GetItemRect(hItem, rcItem, TRUE);
	
	rcEdit.left   = rcItem.left;
	rcEdit.top    = rcItem.top;
	rcEdit.right  = rcItem.right + iH;
	rcEdit.bottom = rcItem.bottom;
	
	if (!m_Edit.GetSafeHwnd())
		m_Edit.Create(_T("STATIC"), "", WS_CHILD|WS_VISIBLE|WS_BORDER, rcEdit, this, NULL);
	else
		m_Edit.MoveWindow(&rcEdit);
	
	m_Edit.ShowWindow(SW_SHOW);
	m_Edit.BringWindowToTop();
	m_Edit.SetFocus();
	
	return NULL;  // USER CAN NOT CHARGE IT.
//	return CTreeCtrl::EditLabel(hItem);
}

LRESULT CSpinTree::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_INPLACE_SPIN)
	{
		if (m_hEditedItem == NULL)
			return FALSE;

		LPSPINTEXT pst = (LPSPINTEXT) lParam;

		CString strItem, strPercent;
		strItem = pst->szTitle;
		strPercent.Format(" (%d", pst->iPercent);
		strItem += strPercent + "%)";

		SetItemText(m_hEditedItem, strItem);
		GetParent()->SendMessage( message, wParam, lParam );
		return TRUE;
	}
	
	return CTreeCtrl::DefWindowProc(message, wParam, lParam);
}

void CSpinTree::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default	
//	EditLabel(HitTest(point));
	
	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

int CSpinTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

void CSpinTree::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}
