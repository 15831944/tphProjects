// EditTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "EditTreeCtrl.h"
#include ".\edittreectrl.h"


// CEditTreeCtrl
UINT CEditTreeCtrl::WM_PRESHOWEDIT =  ::RegisterWindowMessage(_T("prepare show edit"));
UINT CEditTreeCtrl::WM_ENDSHOWEDIT =  ::RegisterWindowMessage(_T("end show edit"));

IMPLEMENT_DYNAMIC(CEditTreeCtrl, CTreeCtrl)
CEditTreeCtrl::CEditTreeCtrl()
{
	m_bShowEdit = FALSE;
	m_bSendMsg = FALSE;
}

CEditTreeCtrl::~CEditTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CEditTreeCtrl, CTreeCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CEditTreeCtrl message handlers


int CEditTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_wndEdit.m_hWnd)
	{
		if(!m_wndEdit.Create(WS_BORDER|WS_CHILD|ES_CENTER | ES_NUMBER |ES_WANTRETURN,CRect(0,0,0,0),this,1456))
			return -1;
		m_wndEdit.ShowWindow(SW_HIDE);
	}

	return 0;
}

long CEditTreeCtrl::GetNumEditValue(void)
{
	CString strValue = _T("");
	m_wndEdit.GetWindowText(strValue);
	return (_ttol(strValue));
}
void CEditTreeCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hSelItem = 0;
	hSelItem = GetSelectedItem();
	if(!m_wndEdit.m_hWnd)
	{
		if(!m_wndEdit.Create(WS_BORDER|WS_CHILD|ES_CENTER | ES_NUMBER |ES_WANTRETURN,CRect(0,0,0,0),this,1456))
			return ;
		m_wndEdit.ShowWindow(SW_HIDE);
	}

	GetParent()->SendMessage(WM_PRESHOWEDIT,0,(LPARAM)this);

	CRect rect;
	if(m_bShowEdit && GetItemRect(hSelItem,rect,TRUE))
	{
		rect += CSize(rect.Width(),0);
		if(rect.Width() < 100)
			rect.right = rect.left + 100;
		m_wndEdit.MoveWindow(rect);
		m_wndEdit.SetSel(0,-1);
		m_wndEdit.SetFocus();
		m_wndEdit.ShowWindow(m_bShowEdit);
		m_bSendMsg = TRUE;
	}

	*pResult = 0;
}

void CEditTreeCtrl::ShowEdit(BOOL bShow /*= TRUE*/)
{
	m_bShowEdit = bShow;
}

void CEditTreeCtrl::SetNumEditValue(long lValue)
{
	CString strValue = _T("");
	strValue.Format(_T("%d"),lValue);
	m_wndEdit.SetWindowText(strValue);
}

void CEditTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_bSendMsg)
	{
		CRect rect;
		m_wndEdit.GetWindowRect(rect);
		ScreenToClient(rect);
		if(!rect.PtInRect(point))
		{
			m_bShowEdit = FALSE;
			m_wndEdit.ShowWindow(m_bShowEdit);
			GetParent()->SendMessage(WM_ENDSHOWEDIT,0,(LPARAM)this);
		}
	}
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

BOOL CEditTreeCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class

	BOOL bReturn = CTreeCtrl::Create(dwStyle, rect, pParentWnd, nID);

	if(!m_wndEdit.m_hWnd)
	{
		if(!m_wndEdit.Create(WS_BORDER|WS_CHILD|ES_CENTER | ES_NUMBER |ES_WANTRETURN,CRect(0,0,0,0),this,1456))
			return (FALSE);
		m_wndEdit.ShowWindow(SW_HIDE);
	}

	return (bReturn);
}
