// DlgSelectStretchIntersectionNode.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "Resource.h"
#include "DlgSelectStretchIntersectionNode.h"
#include ".\dlgselectstretchintersectionnode.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/Stretch.h"
#include "../InputAirside/IntersectItem.h"
#include "../InputAirside/IntersectionNode.h"

// CDlgSelectStretchIntersectionNode dialog

IMPLEMENT_DYNAMIC(CDlgSelectStretchIntersectionNode, CDialog)
CDlgSelectStretchIntersectionNode::CDlgSelectStretchIntersectionNode(int nAirportID,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectStretchIntersectionNode::IDD, pParent),m_nAirportID(nAirportID)
{
	m_strName = _T("");
	m_nSelectID = -1;
}

CDlgSelectStretchIntersectionNode::~CDlgSelectStretchIntersectionNode()
{
}

void CDlgSelectStretchIntersectionNode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_STRETCH, m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgSelectStretchIntersectionNode, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_STRETCH, OnTvnSelchangedTreeStretch)
END_MESSAGE_MAP()


// CDlgSelectStretchIntersectionNode message handlers

BOOL CDlgSelectStretchIntersectionNode::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	SetWindowText(_T("Select Stretch Intersection Node"));
	SetTreeContent();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectStretchIntersectionNode::SetTreeContent()
{
	BOOL bSelect;
	std::vector<int> addNode;
	HTREEITEM hAirport = m_wndTreeCtrl.InsertItem(_T("Stretch Intersection Node:"));
	m_wndTreeCtrl.SetItemData(hAirport,m_nAirportID);
	std::vector<int> vStretchID;
	vStretchID.clear();
	ALTAirport::GetStretchsIDs(m_nAirportID,vStretchID);
	for (int i=0; i<(int)vStretchID.size();i++)
	{
		Stretch pStretch;
		std::vector<IntersectionNode> vIntersect;
		pStretch.ReadObject(vStretchID[i]);
        vIntersect = pStretch.GetIntersectNodes();
		for (int j=0; j<(int)vIntersect.size(); j++)
		{
			bSelect = TRUE;
			std::vector<IntersectItem*> intersectItems = vIntersect[j].GetItemList();
			for (int k=0; k<(int)intersectItems.size();k++)
			{
				if (intersectItems[k]->GetType() != IntersectItem::STRETCH_INTERSECT)
				{
					bSelect = FALSE;
					break;
				}
			}
			if (bSelect == TRUE)
			{
				if(std::find(addNode.begin(),addNode.end(),vIntersect[j].GetID()) == addNode.end()){
					HTREEITEM hInterNode = m_wndTreeCtrl.InsertItem(vIntersect[j].GetName(), hAirport);
					m_wndTreeCtrl.SetItemData(hInterNode,vIntersect[j].GetID());
					addNode.push_back(vIntersect[j].GetID());
				}
			}

		}
	}
	m_wndTreeCtrl.Expand(hAirport,TVE_EXPAND);
	m_wndTreeCtrl.SetRedraw(TRUE);
}


void CDlgSelectStretchIntersectionNode::OnTvnSelchangedTreeStretch(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	HTREEITEM hSel = m_wndTreeCtrl.GetFirstSelectedItem();
	if(m_wndTreeCtrl.GetParentItem(hSel)){
		m_strName = m_wndTreeCtrl.GetItemText(hSel);
		m_nSelectID = m_wndTreeCtrl.GetItemData(hSel);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}else{
		m_strName = _T("");
		m_nSelectID = -1;
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
	*pResult = 0;
}
