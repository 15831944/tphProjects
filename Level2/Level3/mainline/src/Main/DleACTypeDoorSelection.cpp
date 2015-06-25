// DlgACTypeDoorSelection.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DleACTypeDoorSelection.h"
#include ".\dleactypedoorselection.h"


// DlgACTypeDoorSelection dialog

IMPLEMENT_DYNAMIC(DlgACTypeDoorSelection, CDialog)
DlgACTypeDoorSelection::DlgACTypeDoorSelection(ACTYPEDOORLIST* pAcDoors,const CString& strACName,CWnd* pParent /*=NULL*/)
	: CDialog(DlgACTypeDoorSelection::IDD, pParent)
	,m_pAcDoors(pAcDoors)
	,m_strACType(strACName)
	,m_pDoor(NULL)
{
}

DlgACTypeDoorSelection::~DlgACTypeDoorSelection()
{
}

void DlgACTypeDoorSelection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ACDOORSEL, m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(DlgACTypeDoorSelection, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ACDOORSEL, OnTvnSelchangedTreeAcdoorsel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// DlgACTypeDoorSelection message handlers

void DlgACTypeDoorSelection::OnTvnSelchangedTreeAcdoorsel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void DlgACTypeDoorSelection::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	HTREEITEM hSelItem = m_wndTreeCtrl.GetSelectedItem();
	m_strDoorName = m_wndTreeCtrl.GetItemText(hSelItem);
	m_nDoorID = (int)m_wndTreeCtrl.GetItemData(hSelItem);
	for (unsigned i = 0; i < m_pAcDoors->size(); i++)
	{
		ACTypeDoor* pDoor = m_pAcDoors->at(i);
		if (m_nDoorID == pDoor->GetID())
		{
			m_pDoor = pDoor;
			break;
		}
	}
	OnOK();
}

void DlgACTypeDoorSelection::OnInitDoors()
{

	HTREEITEM hFltItem = m_wndTreeCtrl.InsertItem(m_strACType,TVI_ROOT,TVI_LAST);
	HTREEITEM hDoor = NULL;
	int nCount = m_pAcDoors->size();
	for (int i =0; i < nCount; i++)
	{
		ACTypeDoor* pData = m_pAcDoors->at(i);
		hDoor = m_wndTreeCtrl.InsertItem(pData->m_strName, hFltItem, TVI_LAST);
		m_wndTreeCtrl.SetItemData(hDoor,pData->GetID());
	}
	m_wndTreeCtrl.Expand(hFltItem,TVE_EXPAND);
}

BOOL DlgACTypeDoorSelection::OnInitDialog()
{
	CDialog::OnInitDialog();

	OnInitDoors();

	return TRUE;

}