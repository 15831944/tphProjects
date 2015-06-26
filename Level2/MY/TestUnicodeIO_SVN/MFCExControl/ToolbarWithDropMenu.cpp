// ToolbarWithDropMenu.cpp : implementation file
//

#include "stdafx.h"
#include "ToolbarWithDropMenu.h"


// CToolbarWithDropMenu

IMPLEMENT_DYNAMIC(CToolbarWithDropMenu, CToolBar)
CToolbarWithDropMenu::CToolbarWithDropMenu()
{
	m_bDropDownStyle=FALSE;
	m_vDisabledMenuItems.clear();
}

CToolbarWithDropMenu::~CToolbarWithDropMenu()
{
	m_DDBtnArray.RemoveAll();
	m_vDisabledMenuItems.clear();
}


BEGIN_MESSAGE_MAP(CToolbarWithDropMenu, CToolBar)
	ON_NOTIFY_REFLECT(TBN_DROPDOWN,OnDropDownBtn)

END_MESSAGE_MAP()



// CToolbarWithDropMenu message handlers

void CToolbarWithDropMenu::OnDropDownBtn( NMHDR * pNotifyStruct, LRESULT * result )
{
	LPNMTOOLBAR pnmtb=(LPNMTOOLBAR)pNotifyStruct;
	UINT nIDMenu=NULL;
	//
	for(int i=0;i<m_DDBtnArray.GetCount();i++)
	{
		if(((DROPDOWNLIST)m_DDBtnArray[i]).nIDButton==pnmtb->iItem)
			nIDMenu=((DROPDOWNLIST)m_DDBtnArray[i]).nIDMenu;
	}

	if(!nIDMenu)
		return;

	CMenu menu;
	menu.LoadMenu(nIDMenu);
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup);

	size_t nSize = m_vDisabledMenuItems.size();
	for (size_t i = 0; i < nSize; i++ )
	{
		pPopup->EnableMenuItem( m_vDisabledMenuItems.at(i), MF_GRAYED | MF_DISABLED );
	}

	CRect rc;
	SendMessage(TB_GETRECT, pnmtb->iItem, (LPARAM)&rc);
	ClientToScreen(&rc);
	pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,rc.left, rc.bottom, this, &rc);
}

void CToolbarWithDropMenu::AddDropDownBtn(UINT nIDBUTTON, UINT nIDMENU)
{
	if(!m_bDropDownStyle)
	{
		DWORD dwExStyle = TBSTYLE_EX_DRAWDDARROWS;
		GetToolBarCtrl().SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)dwExStyle);
		m_bDropDownStyle=TRUE;
	}

	DROPDOWNLIST DropdownBtn={nIDBUTTON,nIDMENU};
	DWORD dwStyle =GetButtonStyle(CommandToIndex(nIDBUTTON));
	dwStyle |= TBSTYLE_DROPDOWN;
	SetButtonStyle(CommandToIndex(nIDBUTTON), dwStyle);
	m_DDBtnArray.Add(DropdownBtn);
}

void CToolbarWithDropMenu::RemoveDropDownBtn(UINT nIDBUTTON)
{
	for(int i=0;i<m_DDBtnArray.GetCount();i++)
	{
		if(((DROPDOWNLIST)m_DDBtnArray[i]).nIDButton==nIDBUTTON)
		{
			DWORD dwStyle =GetButtonStyle(CommandToIndex(nIDBUTTON));
			dwStyle &=~TBSTYLE_DROPDOWN;
			SetButtonStyle(CommandToIndex(nIDBUTTON), dwStyle);
			m_DDBtnArray.RemoveAt(i);
		}
	}

	if(m_DDBtnArray.IsEmpty())
	{
		DWORD dwExStyle =NULL;
		GetToolBarCtrl().SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)dwExStyle);
		m_bDropDownStyle=FALSE;
	}

}

void CToolbarWithDropMenu::AddDisabledMenuItem( UINT nIDMeunItem )
{
	m_vDisabledMenuItems.push_back(nIDMeunItem);
}

void CToolbarWithDropMenu::ClearDisabledMenuItems()
{
	m_vDisabledMenuItems.clear();
}