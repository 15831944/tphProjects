#include "stdafx.h"
#include "Resource.h"
#include "DlgAirsideSelectIntersectionNode.h"
#include "InputAirside\ALTAirport.h"
#include "InputAirside\IntersectionNodesInAirport.h"



IMPLEMENT_DYNAMIC(CDlgAirsideSelectIntersectionNode, CDialog)


CDlgAirsideSelectIntersectionNode::CDlgAirsideSelectIntersectionNode(int nSelID,int nProjID,CWnd* pParent /*=NULL*/)
: CDialog(CDlgAirsideSelectIntersectionNode::IDD, pParent) 
,m_nProjID(nProjID)
{ 
	m_nSelID = nSelID;
	IntersectionNode node;
	node.ReadData(m_nSelID);
	m_strSelNode = node.GetName();

	
}

CDlgAirsideSelectIntersectionNode::~CDlgAirsideSelectIntersectionNode()
{
}

void CDlgAirsideSelectIntersectionNode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DATA, m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgAirsideSelectIntersectionNode, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DATA, OnTvnSelchangedTreeTakeoffposi)
END_MESSAGE_MAP()
// CDlgSelectTakeoffPosi message handlers

BOOL CDlgAirsideSelectIntersectionNode::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText(_T("Select Intersection Node"));
	SetTreeContents();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAirsideSelectIntersectionNode::SetTreeContents(void)
{	
	m_wndTreeCtrl.SetRedraw(FALSE);
	//std::vector<int> vrInterNodeAdded;
	HTREEITEM selItem;
	std::vector<ALTAirport> vrAirport;
	ALTAirport::GetAirportList(m_nProjID,vrAirport);
	for (std::vector<ALTAirport>::iterator itrAirport = vrAirport.begin();itrAirport != vrAirport.end();++itrAirport)
	{
		HTREEITEM hAirport = m_wndTreeCtrl.InsertItem((*itrAirport).getName());
		m_wndTreeCtrl.SetItemData(hAirport,(*itrAirport).GetAirportID());
		IntersectionNodesInAirport vrInterNode;
		vrInterNode.ReadData((*itrAirport).GetAirportID());

		for (int i=0;i<vrInterNode.GetCount();++i)
		{
			IntersectionNode& nodeat = vrInterNode.NodeAt(i);
			//if(std::find(vrInterNodeAdded.begin(),vrInterNodeAdded.end(), nodeat.GetID() ) == vrInterNodeAdded.end())
			{
				CString itemText;
				itemText.Format(_T("(%d)%s"), nodeat.GetID(), nodeat.GetName().GetString() );
				HTREEITEM hInterNode = m_wndTreeCtrl.InsertItem( itemText, hAirport);
				m_wndTreeCtrl.SetItemData(hInterNode,nodeat.GetID());
				//vrInterNodeAdded.push_back((*itrNode).GetID());
				if(m_nSelID==nodeat.GetID())
					selItem = hInterNode;
			}
		}

		m_wndTreeCtrl.Expand(hAirport,TVE_EXPAND);
	}
	m_wndTreeCtrl.SetRedraw(TRUE);
	//m_wndTreeCtrl.SelectItem(selItem);

}

void CDlgAirsideSelectIntersectionNode::OnTvnSelchangedTreeTakeoffposi(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	HTREEITEM hSel = m_wndTreeCtrl.GetSelectedItem();
	if(m_wndTreeCtrl.GetParentItem(hSel))
	{		
		m_nSelID = m_wndTreeCtrl.GetItemData(hSel);
		IntersectionNode node;
		node.ReadData(m_nSelID);
		m_strSelNode = node.GetName();

		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else
	{
		m_strSelNode = _T("");
		m_nSelID = -1;
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}

	*pResult = 0;
}
