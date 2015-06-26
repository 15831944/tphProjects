// EditListBox.cpp : implementation file
//

#include "stdafx.h"
#include "EditListBox.h"


// CEditListBox
static const int INPLACEEDITID = 123;

IMPLEMENT_DYNAMIC(CEditListBox, CListBox)

CEditListBox::CEditListBox()
: m_iEditingItem(-1)
{
}

CEditListBox::~CEditListBox()
{
}


BEGIN_MESSAGE_MAP(CEditListBox, CListBox)
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnLbnDblclk)
	ON_MESSAGE(WM_APP_ED_EDIT_FINISHED, OnEnKillfocus)
END_MESSAGE_MAP()



// CEditListBox message handlers


void CEditListBox::OnLbnDblclk()
{
	int nIndex = GetCurSel();
	if (nIndex < 0)
		return;
	
	EditItem(nIndex);
}

void CEditListBox::EditItem(int nIndex)
{
	if (nIndex < 0)
		return;

	CRect rcItem;
	GetItemRect(nIndex, &rcItem);
	rcItem.InflateRect(1, 1);
	CString strItem;
	GetText(nIndex, strItem);

	if (!::IsWindow(m_wndInplaceEdit.m_hWnd))
		m_wndInplaceEdit.Create(WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_BORDER, rcItem, this, INPLACEEDITID);

	m_wndInplaceEdit.SetFont(GetFont());
	m_wndInplaceEdit.ShowWindow(SW_SHOW);
	m_wndInplaceEdit.MoveWindow(&rcItem);
	m_wndInplaceEdit.SetWindowText(strItem);
	m_wndInplaceEdit.SetSel(0, -1);
	m_wndInplaceEdit.SetFocus();
    //SetCurSel(nIndex) ;
	m_iEditingItem = nIndex;
}

 LRESULT CEditListBox::OnEnKillfocus(WPARAM wParam, LPARAM lParam)
{
    
	if (m_iEditingItem < 0)
		return FALSE;
	
	CString strItem;
	m_wndInplaceEdit.GetWindowText(strItem);
	
	void * pItemData = (void*)GetItemData(m_iEditingItem);
	DeleteString(m_iEditingItem);
	InsertString(m_iEditingItem, strItem);
	SetItemData(m_iEditingItem, (DWORD_PTR)pItemData);
	SetCurSel(m_iEditingItem);

	m_wndInplaceEdit.ShowWindow(SW_HIDE);

	GetParent()->SendMessage(WM_LBITEMCHANGED, WPARAM(m_iEditingItem), LPARAM((LPCTSTR)strItem));
	//m_iEditingItem = -1;
	return FALSE ;
}

int CEditListBox::DeleteString( UINT nIndex )
{
	if (IsWindow(m_wndInplaceEdit.m_hWnd))
	{
		m_wndInplaceEdit.ShowWindow(SW_HIDE);
	}

	return CListBox::DeleteString(nIndex);
}