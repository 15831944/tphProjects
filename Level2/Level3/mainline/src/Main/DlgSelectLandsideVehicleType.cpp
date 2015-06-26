// DlgSelectLandsideVehicleType.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "MuliSelTreeCtrlEx.h"
#include "DlgSelectLandsideVehicleType.h"


// CDlgSelectLandsideVehicleType dialog

IMPLEMENT_DYNAMIC(CDlgSelectLandsideVehicleType, CDialog)
CDlgSelectLandsideVehicleType::CDlgSelectLandsideVehicleType(bool bEnableMultiSel, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectLandsideVehicleType::IDD, pParent)
    , m_bEnableMultiSel(bEnableMultiSel)
{
    m_vehicleNodeList.ReadData(-1);
    m_vSelVehicles.clear();
    if(bEnableMultiSel)
        m_pTree = new CMuliSeltTreeCtrlEx();
    else
        m_pTree = new CTreeCtrl();
}

CDlgSelectLandsideVehicleType::~CDlgSelectLandsideVehicleType()
{
}

void CDlgSelectLandsideVehicleType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_DATA,*m_pTree);
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
	HTREEITEM hNodeItem = m_pTree->InsertItem(pNode->Name(),hItem);
	m_pTree->SetItemData(hNodeItem,(DWORD_PTR)pNode);
	for (int i = 0; i < pNode->GetNodeCount(); i++)
	{
		LandsideVehicleTypeNode* pChildNode = (LandsideVehicleTypeNode*)pNode->GetNode(i);
		InsertTreeItem(hNodeItem,pChildNode);
	}
	m_pTree->Expand(hNodeItem,TVE_EXPAND);
}

void CDlgSelectLandsideVehicleType::SetTreeContent()
{
	m_pTree->DeleteAllItems();
	m_pTree->SetImageList(m_pTree->GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

	for (int i=0; i< m_vehicleNodeList.GetNodeCount(); i++)
	{
		InsertTreeItem(TVI_ROOT,(LandsideVehicleTypeNode*)m_vehicleNodeList.GetNode(i));
	}
}

void CDlgSelectLandsideVehicleType::OnOK()
{
    m_vSelVehicles.clear();
    if(m_bEnableMultiSel)
    {
        CMuliSeltTreeCtrlEx* pTree = (CMuliSeltTreeCtrlEx*)m_pTree;
        HTREEITEM hItem = pTree->GetFirstSelectedItem();
        while(hItem)
        {
            LandsideVehicleTypeNode* pNode = (LandsideVehicleTypeNode*)pTree->GetItemData(hItem);
            m_vSelVehicles.push_back(pNode->GetFullName());
            hItem = pTree->GetNextSelectedItem(hItem);
        }
    }
    else
    {
        HTREEITEM hItem = m_pTree->GetSelectedItem();
        LandsideVehicleTypeNode* pNode = (LandsideVehicleTypeNode*)m_pTree->GetItemData(hItem);
        m_vSelVehicles.push_back(pNode->GetFullName());
    }
	if (m_vehicleNodeList.GetNodeCount() && m_vSelVehicles.size() == 0)
	{
		MessageBox(_T("Please choose vehicle type name!."),_T("Warning"));
		return;
	}
	CDialog::OnOK();
}

CString CDlgSelectLandsideVehicleType::GetName()
{
    if(m_vSelVehicles.size() <= 0)
        return _T("");
    return m_vSelVehicles[0];
}


