#include "stdafx.h"
#include "ComboBoxListBox.h"


/////////////////////////////////////////////////////////////////////////////
// CComboBoxListBox

CComboBoxListBox::CComboBoxListBox()
{
}

CComboBoxListBox::~CComboBoxListBox()
{
}


BEGIN_MESSAGE_MAP(CComboBoxListBox, CSimpleToolTipListBox)
	ON_CBN_KILLFOCUS(WM_COMBO_IN_LISTBOX, OnComboKillfocus)
	ON_CBN_SELCHANGE(WM_COMBO_IN_LISTBOX, OnComboSelChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboBoxListBox  message handlers

int CComboBoxListBox::AddItem(CStringList* pStrList)
{	
	CString strFirst = pStrList->GetCount()>0 ? pStrList->GetHead() : _T("");
	int nAddIndex = AddString(strFirst);
	SetCurSel(nAddIndex);
	SetFocus();

	CRect rcItem;
	GetItemRect(nAddIndex, &rcItem);
	rcItem.InflateRect(1, 1);

	if (!::IsWindow(m_curComboBox.m_hWnd))
	{
		m_curComboBox.Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL|WS_TABSTOP|CBS_DROPDOWNLIST,
			rcItem, this, WM_COMBO_IN_LISTBOX);
		
		// set string
		for (POSITION pos = pStrList->GetHeadPosition(); pos != NULL;)
		{
			CString str = pStrList->GetNext(pos);
			m_curComboBox.AddString(str);
		}
	}
	
	m_curComboBox.SetWindowPos(&wndTop, 1, 100, 
		(rcItem.right-rcItem.left), 100, SWP_FRAMECHANGED);

	m_curComboBox.SetFont(GetFont());
	m_curComboBox.ShowWindow(SW_SHOW);
	m_curComboBox.MoveWindow(&rcItem);
	m_curComboBox.SetCurSel(0);
	m_curComboBox.SetFocus();
    
	return nAddIndex;

}

void CComboBoxListBox::DelItem(int nIndex)
{
	if (nIndex == 	LB_ERR)
		return;

	if (::IsWindow(m_curComboBox.m_hWnd))
		m_curComboBox.ShowWindow(SW_HIDE);
	DeleteString(nIndex);

}

void CComboBoxListBox::OnComboKillfocus()
{
	m_curComboBox.ShowWindow(SW_HIDE);

	int nSelIdx = m_curComboBox.GetCurSel();
	GetParent()->SendMessage(WM_COMBO_IN_LISTBOX, -1, (LPARAM)nSelIdx);
}

void CComboBoxListBox::SetCurText()
{	
	int nCurListSel = GetCurSel();
	if (nCurListSel == 	LB_ERR)
		return;

	int nCurBoxSel = m_curComboBox.GetCurSel();
	CString strText;
	m_curComboBox.GetLBText(nCurBoxSel,strText);
	DeleteString(nCurListSel);
	InsertString(nCurListSel, strText);
	SetCurSel(nCurListSel);
}

void CComboBoxListBox::OnComboSelChange()
{
	SetCurText();
}

void CComboBoxListBox::KillCombofocus()
{
	if (::IsWindow(m_curComboBox.m_hWnd))
		m_curComboBox.ShowWindow(SW_HIDE);
}

