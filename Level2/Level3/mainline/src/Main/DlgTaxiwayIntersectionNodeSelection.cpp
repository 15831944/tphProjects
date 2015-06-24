#include "StdAfx.h"
#include "resource.h"
#include ".\dlgtaxiwayintersectionnodeselection.h"
#include "../InputAirside/Taxiway.h"
#include "../InputAirside/IntersectionNode.h"

IMPLEMENT_DYNAMIC(DlgTaxiwayIntersectionNodeSelection, CDialog)
DlgTaxiwayIntersectionNodeSelection::DlgTaxiwayIntersectionNodeSelection(int nProjID, int nTaxiwayID, CWnd* pParent /*=NULL*/)
: CDialog(DlgTaxiwayIntersectionNodeSelection::IDD, pParent)
, m_nProjectID(nProjID)
, m_nTaxiwayID(nTaxiwayID)
, m_nIntersectionNodeID(-1)
, m_strNodeName(_T(""))
{

}

DlgTaxiwayIntersectionNodeSelection::~DlgTaxiwayIntersectionNodeSelection(void)
{
}

void DlgTaxiwayIntersectionNodeSelection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ALTOBJECT, m_wndALTObjectTree);
}


BEGIN_MESSAGE_MAP(DlgTaxiwayIntersectionNodeSelection, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ALTOBJECT, OnSelChangedTree)
END_MESSAGE_MAP()


// DlgTaxiwayIntersectionNodeSelection message handlers
BOOL DlgTaxiwayIntersectionNodeSelection::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(_T("Select Intersection Node"));

	m_wndALTObjectTree.ModifyStyle(NULL,WS_VSCROLL);
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	Taxiway _taxiway;
	_taxiway.ReadObject(m_nTaxiwayID);

	HTREEITEM hRoot = m_wndALTObjectTree.InsertItem(_taxiway.GetObjNameString());

	IntersectionNodeList nodeList = _taxiway.GetIntersectNodes();

	HTREEITEM hItem = NULL;
	size_t nCount = nodeList.size();
	for (size_t i = 0; i < nCount; i++)
	{
		IntersectionNode node = nodeList.at(i);
		hItem = m_wndALTObjectTree.InsertItem(node.GetName(), hRoot,TVI_LAST);
		m_wndALTObjectTree.SetItemData(hItem,node.GetID());
	}
	m_wndALTObjectTree.Expand(hRoot,TVE_EXPAND);
	return TRUE;
}

int DlgTaxiwayIntersectionNodeSelection::GetNodeID()
{
	return m_nIntersectionNodeID;
}

const CString& DlgTaxiwayIntersectionNodeSelection::GetNodeName()
{
	return m_strNodeName;
}

void DlgTaxiwayIntersectionNodeSelection::OnSelChangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{	
	*pResult = 0;

	HTREEITEM hSelItem = m_wndALTObjectTree.GetSelectedItem();

	if (hSelItem == NULL)
		return;

	//select one node
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	m_nIntersectionNodeID = (int)m_wndALTObjectTree.GetItemData(hSelItem);
	m_strNodeName = m_wndALTObjectTree.GetItemText(hSelItem);

}