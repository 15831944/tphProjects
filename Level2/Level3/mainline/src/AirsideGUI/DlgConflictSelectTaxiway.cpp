// DlgConflictSelectTaxiway.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "Resource.h"
#include "DlgConflictSelectTaxiway.h"
#include ".\dlgconflictselecttaxiway.h"


// CDlgConflictSelectTaxiway dialog

IMPLEMENT_DYNAMIC(CDlgConflictSelectTaxiway, CDialog)
CDlgConflictSelectTaxiway::CDlgConflictSelectTaxiway(TaxiwayVectorMap* pTaxiwayVectorMap, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConflictSelectTaxiway::IDD, pParent)
	,m_pTaxiwayVectorMap(pTaxiwayVectorMap)
{
}

CDlgConflictSelectTaxiway::~CDlgConflictSelectTaxiway()
{
}

void CDlgConflictSelectTaxiway::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_CONFLICTSELECTTAXIWAY, m_wndSelectTaxiwayTree);
}


BEGIN_MESSAGE_MAP(CDlgConflictSelectTaxiway, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CONFLICTSELECTTAXIWAY, OnTvnSelchangedTreeConflictselecttaxiway)
END_MESSAGE_MAP()


// CDlgConflictSelectTaxiway message handlers

void CDlgConflictSelectTaxiway::LoadTree()
{
	TaxiwayVectorMapIter iter = m_pTaxiwayVectorMap->begin();
	for(; iter != m_pTaxiwayVectorMap->end(); iter++)
	{
		CString strAirportName = iter->first;
		HTREEITEM hAirport = m_wndSelectTaxiwayTree.InsertItem(strAirportName);
		TaxiwayVector& vec = iter->second;
		for(TaxiwayVectorIter it = vec.begin();
			it != vec.end(); it++)
		{
			CString strTaxiwayName = it->first;
			int nTaxiwayID = it->second;
			HTREEITEM hTaxiway = m_wndSelectTaxiwayTree.InsertItem(strTaxiwayName,hAirport);
			m_wndSelectTaxiwayTree.SetItemData(hTaxiway,nTaxiwayID);
		}
		m_wndSelectTaxiwayTree.Expand(hAirport,TVE_EXPAND);
	}
}

void CDlgConflictSelectTaxiway::OnTvnSelchangedTreeConflictselecttaxiway(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hSelItem = m_wndSelectTaxiwayTree.GetSelectedItem();
	HTREEITEM hParentItem = m_wndSelectTaxiwayTree.GetParentItem(hSelItem);
	if(hSelItem && hParentItem && !m_wndSelectTaxiwayTree.GetParentItem(hParentItem))
	{
		m_nTaxiwayID = m_wndSelectTaxiwayTree.GetItemData(hSelItem);
		m_strTaxiwayName = m_wndSelectTaxiwayTree.GetItemText(hSelItem);
	}
	*pResult = 0;
}

BOOL CDlgConflictSelectTaxiway::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
