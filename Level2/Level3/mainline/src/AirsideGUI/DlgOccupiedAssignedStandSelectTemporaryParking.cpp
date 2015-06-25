// DlgOccupiedSelectTemporaryParking.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "Resource.h"
#include "DlgOccupiedAssignedStandSelectTemporaryParking.h"
#include ".\dlgoccupiedAssignedstandselectTemporaryParking.h"


// CDlgOccupiedSelectTemporaryParking dialog

IMPLEMENT_DYNAMIC(CDlgOccupiedSelectTemporaryParking, CDialog)
CDlgOccupiedSelectTemporaryParking::CDlgOccupiedSelectTemporaryParking(AltObjectVectorMap* pTemporaryParkingVectorMap, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOccupiedSelectTemporaryParking::IDD, pParent)
	,m_pTemporaryParkingVectorMap(pTemporaryParkingVectorMap)
{
}

CDlgOccupiedSelectTemporaryParking::~CDlgOccupiedSelectTemporaryParking()
{
}

void CDlgOccupiedSelectTemporaryParking::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_OCCUPIEDSELECTTEMPORARYPARKING, m_wndSelectTemporaryParkingTree);
}


BEGIN_MESSAGE_MAP(CDlgOccupiedSelectTemporaryParking, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_OCCUPIEDSELECTTEMPORARYPARKING, OnTvnSelchangedTreeConflictselectTemporaryParking)
END_MESSAGE_MAP()


// CDlgOccupiedSelectTemporaryParking message handlers

void CDlgOccupiedSelectTemporaryParking::LoadTree()
{
	AltObjectVectorMapIter iter = m_pTemporaryParkingVectorMap->begin();
	for(; iter != m_pTemporaryParkingVectorMap->end(); iter++)
	{
		CString strAirportName = iter->first;
		HTREEITEM hAirport = m_wndSelectTemporaryParkingTree.InsertItem(strAirportName);
		AltObjectVector& vec = iter->second;
		for(AltObjectVectorIter it = vec.begin();
			it != vec.end(); it++)
		{
			CString strTemporaryParkingName = it->first;
			int nTemporaryParkingID = it->second;
			HTREEITEM hTemporaryParking = m_wndSelectTemporaryParkingTree.InsertItem(strTemporaryParkingName,hAirport);
			m_wndSelectTemporaryParkingTree.SetItemData(hTemporaryParking,nTemporaryParkingID);
		}
		m_wndSelectTemporaryParkingTree.Expand(hAirport,TVE_EXPAND);
	}
}

void CDlgOccupiedSelectTemporaryParking::OnTvnSelchangedTreeConflictselectTemporaryParking(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hSelItem = m_wndSelectTemporaryParkingTree.GetSelectedItem();
	HTREEITEM hParentItem = m_wndSelectTemporaryParkingTree.GetParentItem(hSelItem);
	if(hSelItem && hParentItem && !m_wndSelectTemporaryParkingTree.GetParentItem(hParentItem))
	{
		m_nTemporaryParkingID = m_wndSelectTemporaryParkingTree.GetItemData(hSelItem);
		m_strTemporaryParkingName = m_wndSelectTemporaryParkingTree.GetItemText(hSelItem);
	}
	*pResult = 0;
}

BOOL CDlgOccupiedSelectTemporaryParking::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	LoadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOccupiedSelectTemporaryParking::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	HTREEITEM hSel = m_wndSelectTemporaryParkingTree.GetSelectedItem();
	if(hSel == NULL || m_wndSelectTemporaryParkingTree.GetParentItem(hSel) == NULL)
	{
		AfxMessageBox("Please select a TemporaryParking.");
		return;
	}
	CDialog::OnOK();
}
