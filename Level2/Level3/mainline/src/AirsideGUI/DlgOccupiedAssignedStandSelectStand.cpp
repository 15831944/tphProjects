// DlgOccupiedSelectStand.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "Resource.h"
#include "DlgOccupiedAssignedStandSelectStand.h"
#include ".\dlgoccupiedassignedstandselectstand.h"


// CDlgOccupiedSelectStand dialog

IMPLEMENT_DYNAMIC(CDlgOccupiedSelectStand, CDialog)
CDlgOccupiedSelectStand::CDlgOccupiedSelectStand(AltObjectVectorMap* pStandVectorMap, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOccupiedSelectStand::IDD, pParent)
	,m_pStandVectorMap(pStandVectorMap)
{
}

CDlgOccupiedSelectStand::~CDlgOccupiedSelectStand()
{
}

void CDlgOccupiedSelectStand::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_OCCUPIEDSELECTSTAND, m_wndSelectStandTree);
}


BEGIN_MESSAGE_MAP(CDlgOccupiedSelectStand, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_OCCUPIEDSELECTSTAND, OnTvnSelchangedTreeConflictselectStand)
END_MESSAGE_MAP()


// CDlgOccupiedSelectStand message handlers

void CDlgOccupiedSelectStand::LoadTree()
{
	AltObjectVectorMapIter iter = m_pStandVectorMap->begin();
	for(; iter != m_pStandVectorMap->end(); iter++)
	{
		CString strAirportName = iter->first;
		HTREEITEM hAirport = m_wndSelectStandTree.InsertItem(strAirportName);
		AltObjectVector& vec = iter->second;
		for(AltObjectVectorIter it = vec.begin();
			it != vec.end(); it++)
		{
			CString strStandName = it->first;
			int nStandID = it->second;
			HTREEITEM hStand = m_wndSelectStandTree.InsertItem(strStandName,hAirport);
			m_wndSelectStandTree.SetItemData(hStand,nStandID);
		}
		m_wndSelectStandTree.Expand(hAirport,TVE_EXPAND);
	}
}

void CDlgOccupiedSelectStand::OnTvnSelchangedTreeConflictselectStand(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hSelItem = m_wndSelectStandTree.GetSelectedItem();
	HTREEITEM hParentItem = m_wndSelectStandTree.GetParentItem(hSelItem);
	if(hSelItem && hParentItem && !m_wndSelectStandTree.GetParentItem(hParentItem))
	{
		m_nStandID = m_wndSelectStandTree.GetItemData(hSelItem);
		m_strStandName = m_wndSelectStandTree.GetItemText(hSelItem);
	}
	*pResult = 0;
}

BOOL CDlgOccupiedSelectStand::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



void CDlgOccupiedSelectStand::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	HTREEITEM hSel = m_wndSelectStandTree.GetSelectedItem();
	if(hSel == NULL || m_wndSelectStandTree.GetParentItem(hSel) == NULL)
	{
		AfxMessageBox("Please select a stand.");
		return;
	}

	CDialog::OnOK();
}
