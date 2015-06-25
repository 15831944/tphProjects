// DlgACTypeSelection.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "DlgACTypeSelection.h"
#include "resource.h"
#include "../InputAirside/AircraftClassifications.h"

// DlgACTypeSelection dialog

IMPLEMENT_DYNAMIC(DlgACTypeSelection, CDialog)
DlgACTypeSelection::DlgACTypeSelection(int nProjID, CWnd* pParent /*=NULL*/)
: CDialog(IDD_DIALOG_ACTYPESELECTION, pParent)
,m_nProjID(nProjID)
{
	m_vAircraftClassifications.clear();
}

DlgACTypeSelection::~DlgACTypeSelection()
{
	int nSize = (int)m_vAircraftClassifications.size();
	for (int i = 0; i< nSize; i++)
	{
		AircraftClassifications* pCategory = m_vAircraftClassifications.at(i);
		if (pCategory != NULL)
		{
			delete pCategory;
			pCategory = NULL;
		}
	}
	m_vAircraftClassifications.clear();
}

void DlgACTypeSelection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ACCATEGORY, m_TreeACType);
}


BEGIN_MESSAGE_MAP(DlgACTypeSelection, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ACCATEGORY, OnTvnSelchangedTreeACType)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// DlgACTypeSelection message handlers
void DlgACTypeSelection::LoadTree()
{
	//WingspanBased
	//SurfaceBearingWeightBased
	//WakeVortexWightBased
	//ApproachSpeedBased

	HTREEITEM hRootItem = m_TreeACType.InsertItem(_T("AC Categories:"));

	HTREEITEM hSubItem = m_TreeACType.InsertItem(_T("WingspanBased"),hRootItem);
	AircraftClassifications* pAircraftClassifications = new AircraftClassifications(m_nProjID,WingspanBased);
	pAircraftClassifications->ReadData();
	m_vAircraftClassifications.push_back(pAircraftClassifications);
	m_TreeACType.SetItemData(hSubItem,(DWORD_PTR)pAircraftClassifications);


	m_TreeACType.Expand(hRootItem, TVE_EXPAND);
	int nCount = pAircraftClassifications->GetCount();
	for (int i = 0; i < nCount; i++ )
	{
		AircraftClassificationItem* pItem = pAircraftClassifications->GetItem(i);
		HTREEITEM hACTypeItem = m_TreeACType.InsertItem(pItem->getName(),hSubItem);
		m_TreeACType.SetItemData(hACTypeItem,(DWORD_PTR)pItem);
	}
	m_TreeACType.Expand(hSubItem,TVE_EXPAND);
	
	hSubItem = m_TreeACType.InsertItem(_T("SurfaceBearingWeightBased"),hRootItem);
	pAircraftClassifications = new AircraftClassifications(m_nProjID,SurfaceBearingWeightBased);
	pAircraftClassifications->ReadData();
	m_vAircraftClassifications.push_back(pAircraftClassifications);
	m_TreeACType.SetItemData(hSubItem,(DWORD_PTR)pAircraftClassifications);

	m_TreeACType.Expand(hRootItem, TVE_EXPAND);
	nCount = pAircraftClassifications->GetCount();
	for (int i = 0; i < nCount; i++ )
	{
		AircraftClassificationItem* pItem =pAircraftClassifications->GetItem(i);
		HTREEITEM hACTypeItem = m_TreeACType.InsertItem(pItem->getName(),hSubItem);
		m_TreeACType.SetItemData(hACTypeItem,(DWORD_PTR)pItem);
	}
	m_TreeACType.Expand(hSubItem,TVE_EXPAND);

	hSubItem = m_TreeACType.InsertItem(_T("WakeVortexWightBased"),hRootItem);
	pAircraftClassifications = new AircraftClassifications(m_nProjID,WakeVortexWightBased);
	pAircraftClassifications->ReadData();
	m_vAircraftClassifications.push_back(pAircraftClassifications);
	m_TreeACType.SetItemData(hSubItem,(DWORD_PTR)pAircraftClassifications);

	m_TreeACType.Expand(hRootItem, TVE_EXPAND);
	nCount = pAircraftClassifications->GetCount();
	for (int i = 0; i < nCount; i++ )
	{
		AircraftClassificationItem* pItem = pAircraftClassifications->GetItem(i);
		HTREEITEM hACTypeItem = m_TreeACType.InsertItem(pItem->getName(),hSubItem);
		m_TreeACType.SetItemData(hACTypeItem,(DWORD_PTR)pItem);
	}
	m_TreeACType.Expand(hSubItem,TVE_EXPAND);

	hSubItem = m_TreeACType.InsertItem(_T("ApproachSpeedBased"),hRootItem);
	pAircraftClassifications = new AircraftClassifications(m_nProjID,ApproachSpeedBased);
	pAircraftClassifications->ReadData();
	m_vAircraftClassifications.push_back(pAircraftClassifications);
	m_TreeACType.SetItemData(hSubItem,(DWORD_PTR)pAircraftClassifications);

	m_TreeACType.Expand(hRootItem, TVE_EXPAND);
	nCount = pAircraftClassifications->GetCount();
	for (int i = 0; i < nCount; i++ )
	{
		AircraftClassificationItem* pItem = pAircraftClassifications->GetItem(i);
		HTREEITEM hACTypeItem = m_TreeACType.InsertItem(pItem->getName(),hSubItem);
		m_TreeACType.SetItemData(hACTypeItem,(DWORD_PTR)pItem);
	}
	m_TreeACType.Expand(hSubItem,TVE_EXPAND);

}

BOOL DlgACTypeSelection::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void DlgACTypeSelection::OnTvnSelchangedTreeACType(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_TreeACType.GetSelectedItem();
	int nSel = GetCurACTypePos(hItem);
	if (nSel == 1)
	{
		HTREEITEM hCategoryItem = m_TreeACType.GetParentItem(hItem);
		AircraftClassifications* pCategory = (AircraftClassifications*)m_TreeACType.GetItemData(hCategoryItem);
		if (pCategory)
		{
			m_enumACCategory = pCategory->GetBasisType();
			AircraftClassificationItem* pACType = (AircraftClassificationItem*)m_TreeACType.GetItemData(hItem);
			if (pACType)
			{
				m_strACType = pACType->getName();
				m_nSelACTypeID = pACType->getID();
			}

		}

	}

	*pResult = 0;
}

void DlgACTypeSelection::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_TreeACType.GetSelectedItem();
	if (GetCurACTypePos(hItem) < 1)
	{
		AfxMessageBox("Please select an AC type!");
		return;
	}
		OnOK();
}

int DlgACTypeSelection::GetCurACTypePos(HTREEITEM hTreeItem)
{
	if (NULL == hTreeItem)
	{
		return -1;
	}

	HTREEITEM hParentItem = m_TreeACType.GetParentItem(hTreeItem);

	//root item
	if (NULL == hParentItem)
	{
		return -1;
	}

	hParentItem = m_TreeACType.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 0;
	}

	hParentItem = m_TreeACType.GetParentItem(hParentItem);

	if (NULL == hParentItem)
	{
		return 1;
	}

	return -1;
}

int DlgACTypeSelection::GetACTypeID()
{
	return m_nSelACTypeID;
}
CString DlgACTypeSelection::GetACTypeName()
{
	return m_strACType;
}
FlightClassificationBasisType DlgACTypeSelection::GetACCategory()
{
	return m_enumACCategory;
}