// DlgSelectTakeoffPosi.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "Resource.h"
#include "DlgSelectIntersectionNode.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/taxiway.h"
// CDlgSelectTakeoffPosi dialog

IMPLEMENT_DYNAMIC(CDlgSelectIntersectionNode, CDialog)
CDlgSelectIntersectionNode::CDlgSelectIntersectionNode(int nProjID,CWnd* pParent /*=NULL*/)
: CDialog(CDlgSelectIntersectionNode::IDD, pParent) 
,m_nProjID(nProjID)
{ 
	m_strSelNode = _T("");
	m_nSelID = -1;
}

CDlgSelectIntersectionNode::~CDlgSelectIntersectionNode()
{
}

void CDlgSelectIntersectionNode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ALTOBJECT, m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgSelectIntersectionNode, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ALTOBJECT, OnTvnSelchangedTreeTakeoffposi)
END_MESSAGE_MAP()
// CDlgSelectTakeoffPosi message handlers

BOOL CDlgSelectIntersectionNode::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText(_T("Select Intersection Node"));
	SetTreeContents();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectIntersectionNode::SetTreeContents(void)
{	
	m_wndTreeCtrl.SetRedraw(FALSE);
	std::vector<int> vrInterNodeAdded;
	std::vector<ALTAirport> vrAirport;
	ALTAirport::GetAirportList(m_nProjID,vrAirport);
	for (std::vector<ALTAirport>::iterator itrAirport = vrAirport.begin();\
		itrAirport != vrAirport.end();++itrAirport) {
			HTREEITEM hAirport = m_wndTreeCtrl.InsertItem((*itrAirport).getName());
			m_wndTreeCtrl.SetItemData(hAirport,(*itrAirport).GetAirportID());
			std::vector<int> vrTaxiwayID;
			vrTaxiwayID.clear();
			ALTAirport::GetTaxiwaysIDs((*itrAirport).GetAirportID(),vrTaxiwayID);			
			for (std::vector<int>::iterator itrTaxiwayID = vrTaxiwayID.begin();itrTaxiwayID != vrTaxiwayID.end();++itrTaxiwayID) {
				Taxiway taxiway;
				IntersectionNodeList vrInterNode;
				taxiway.ReadObject(*itrTaxiwayID);
				vrInterNode.clear();
				vrInterNode = taxiway.GetIntersectNodes();
				for (IntersectionNodeList::iterator itrNode = vrInterNode.begin();itrNode != vrInterNode.end();++itrNode) {
					if(std::find(vrInterNodeAdded.begin(),vrInterNodeAdded.end(),(*itrNode).GetID()) == vrInterNodeAdded.end()){
						HTREEITEM hInterNode = m_wndTreeCtrl.InsertItem((*itrNode).GetName(), hAirport);
						m_wndTreeCtrl.SetItemData(hInterNode,(*itrNode).GetID());
						vrInterNodeAdded.push_back((*itrNode).GetID());
					}
				}
			}
			m_wndTreeCtrl.Expand(hAirport,TVE_EXPAND);
	}
	m_wndTreeCtrl.SetRedraw(TRUE);
}

void CDlgSelectIntersectionNode::OnTvnSelchangedTreeTakeoffposi(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	HTREEITEM hSel = m_wndTreeCtrl.GetFirstSelectedItem();
	if(m_wndTreeCtrl.GetParentItem(hSel)){
		m_strSelNode = m_wndTreeCtrl.GetItemText(hSel);
		m_nSelID = m_wndTreeCtrl.GetItemData(hSel);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}else{
		m_strSelNode = _T("");
		m_nSelID = -1;
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}

	*pResult = 0;
}
