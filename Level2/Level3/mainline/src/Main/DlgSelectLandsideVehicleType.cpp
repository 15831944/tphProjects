// DlgSelectLandsideVehicleType.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSelectLandsideVehicleType.h"


// CDlgSelectLandsideVehicleType dialog

IMPLEMENT_DYNAMIC(CDlgSelectLandsideVehicleType, CDialog)
CDlgSelectLandsideVehicleType::CDlgSelectLandsideVehicleType(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectLandsideVehicleType::IDD, pParent)
{
	m_vehicleNodeList.ReadData(-1);
}

CDlgSelectLandsideVehicleType::~CDlgSelectLandsideVehicleType()
{
}

void CDlgSelectLandsideVehicleType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_DATA,m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgSelectLandsideVehicleType, CDialog)
END_MESSAGE_MAP()

// CDlgSelectLandsideVehicleType message handlers
BOOL CDlgSelectLandsideVehicleType::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetTreeContent();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgSelectLandsideVehicleType::InsertTreeItem( HTREEITEM hItem,LandsideVehicleTypeNode* pNode )
{
	HTREEITEM hNodeItem = m_wndTreeCtrl.InsertItem(pNode->Name(),hItem);
	m_wndTreeCtrl.SetItemData(hNodeItem,(DWORD_PTR)pNode);
	for (int i = 0; i < pNode->GetNodeCount(); i++)
	{
		LandsideVehicleTypeNode* pChildNode = (LandsideVehicleTypeNode*)pNode->GetNode(i);
		InsertTreeItem(hNodeItem,pChildNode);
	}
	m_wndTreeCtrl.Expand(hNodeItem,TVE_EXPAND);
}

void CDlgSelectLandsideVehicleType::SetTreeContent()
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

	for (int i=0; i< m_vehicleNodeList.GetNodeCount(); i++)
	{
		InsertTreeItem(TVI_ROOT,(LandsideVehicleTypeNode*)m_vehicleNodeList.GetNode(i));
	}
}

void CDlgSelectLandsideVehicleType::OnOK()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem)
	{
		LandsideVehicleTypeNode* pNode = (LandsideVehicleTypeNode*)m_wndTreeCtrl.GetItemData(hItem);
		m_strName = pNode->GetFullName();
	}

	if (m_vehicleNodeList.GetNodeCount() && hItem == NULL)
	{
		MessageBox(_T("Please choose vehicle type name!."),_T("Warning"));
		return;
	}
	CDialog::OnOK();
}


//////////////////////////////////////////////////////////////////////////

CDlgMultiSelectLandsideVehicleType::CDlgMultiSelectLandsideVehicleType(CWnd* pParent /*= NULL*/) :
    CDlgSelectLandsideVehicleType(pParent)
{
}

CDlgMultiSelectLandsideVehicleType::~CDlgMultiSelectLandsideVehicleType()
{
}

void CDlgMultiSelectLandsideVehicleType::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX,IDC_TREE_DATA,m_MultiSelTree);
}

void CDlgMultiSelectLandsideVehicleType::InsertTreeItem( HTREEITEM hItem,LandsideVehicleTypeNode* pNode )
{
    HTREEITEM hNodeItem = m_MultiSelTree.InsertItem(pNode->Name(),hItem);
    m_MultiSelTree.SetItemData(hNodeItem,(DWORD_PTR)pNode);
    for (int i = 0; i < pNode->GetNodeCount(); i++)
    {
        LandsideVehicleTypeNode* pChildNode = (LandsideVehicleTypeNode*)pNode->GetNode(i);
        InsertTreeItem(hNodeItem,pChildNode);
    }
    m_MultiSelTree.Expand(hNodeItem,TVE_EXPAND);
}

void CDlgMultiSelectLandsideVehicleType::SetTreeContent()
{
    m_MultiSelTree.DeleteAllItems();
    m_MultiSelTree.SetImageList(m_MultiSelTree.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

    for (int i=0; i< m_vehicleNodeList.GetNodeCount(); i++)
    {
        InsertTreeItem(TVI_ROOT,(LandsideVehicleTypeNode*)m_vehicleNodeList.GetNode(i));
    }
}

void CDlgMultiSelectLandsideVehicleType::OnOK()
{
    m_vSelVehicles.clear();

    HTREEITEM hItem = m_MultiSelTree.GetFirstSelectedItem();
    while(hItem)
    {
        LandsideVehicleTypeNode* pNode = (LandsideVehicleTypeNode*)m_MultiSelTree.GetItemData(hItem);
        m_vSelVehicles.push_back(pNode->GetFullName());
        hItem = m_MultiSelTree.GetNextSelectedItem(hItem);
    }

    if (m_vehicleNodeList.GetNodeCount() && m_vSelVehicles.size() == 0)
    {
        MessageBox(_T("Please choose vehicle type name!."),_T("Warning"));
        return;
    }
    CDialog::OnOK();
}
