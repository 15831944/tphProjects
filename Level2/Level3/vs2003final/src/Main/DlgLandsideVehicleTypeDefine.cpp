// DlgLandsideVehicleTypeDefine.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgLandsideVehicleTypeDefine.h"
#include "Landside/LandsideVehicleTypeNode.h"
#include "Database/ADODatabase.h"


// CDlgLandsideVehicleTypeDefine dialog
const UINT ID_NEW     = 10;
const UINT ID_MODIFY    = 11;
const UINT ID_DEL     = 12;

IMPLEMENT_DYNAMIC(CDlgLandsideVehicleTypeDefine, CXTResizeDialog)
CDlgLandsideVehicleTypeDefine::CDlgLandsideVehicleTypeDefine(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgLandsideVehicleTypeDefine::IDD, pParent)
{
	m_pNodeList = new LandsideVehicleBaseNode();
	m_pNodeList->ReadData(-1);
}

CDlgLandsideVehicleTypeDefine::~CDlgLandsideVehicleTypeDefine()
{
	if (m_pNodeList)
	{
		delete m_pNodeList;
		m_pNodeList = NULL;
	}
}

void CDlgLandsideVehicleTypeDefine::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DATA, m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgLandsideVehicleTypeDefine, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW, OnCmdNew)
	ON_COMMAND(ID_DEL, OnCmdDelete)
	ON_COMMAND(ID_MODIFY, OnCmdEdit)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedSave)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DATA, OnTvnSelchangedTreeItem)
END_MESSAGE_MAP()

// CDlgLandsideVehicleTypeDefine message handlers
BOOL CDlgLandsideVehicleTypeDefine::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	InitToolBar();

	SetTreeContent();

	UpdateToolBar();

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_GROUP, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(m_ToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TREE_DATA, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgLandsideVehicleTypeDefine::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_ToolBar.LoadToolBar(IDR_BAGDEV_GATE))
	{
		return -1;
	}

	CToolBarCtrl& toolbar = m_ToolBar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_NEW);
	toolbar.SetCmdID(1, ID_DEL);
	toolbar.SetCmdID(2, ID_MODIFY);

	return 0;
}

void CDlgLandsideVehicleTypeDefine::InitToolBar( void )
{
	CRect rect;
	m_wndTreeCtrl.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 4;
	m_ToolBar.MoveWindow(&rect);

	m_ToolBar.GetToolBarCtrl().EnableButton( ID_NEW, FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_DEL, FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_MODIFY, FALSE);
}

void CDlgLandsideVehicleTypeDefine::UpdateToolBar( void )
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_NEW, TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_DEL, TRUE );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_MODIFY, TRUE );
	}
	else
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_NEW, TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_DEL, FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_MODIFY, FALSE );
	}
}

LRESULT CDlgLandsideVehicleTypeDefine::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if (message == UM_CEW_LABLE_END)
	{
		HTREEITEM hItem = (HTREEITEM)wParam;
		CString strValue = *((CString*)lParam);
		LandsideVehicleTypeNode* pNodeData = (LandsideVehicleTypeNode*)m_wndTreeCtrl.GetItemData(hItem);
		if (strValue.IsEmpty() && pNodeData->Name().IsEmpty())
		{
			MessageBox(_T("Vechilce type name can not be empty"),_T("Warning"),MB_OK);
			m_wndTreeCtrl.DoEdit(hItem);
		}
		else if (pNodeData->Invalid(strValue))
		{
			MessageBox(_T("Vechilce type name is exist !."),_T("Warning"),MB_OK);
			m_wndTreeCtrl.DoEdit(hItem);
		}
		else if(!strValue.IsEmpty())
		{
			if (strValue.Find('-') != -1)
			{
				MessageBox(_T("Vechilce type name has illegal character '-'!."),_T("Warning"),MB_OK);
				m_wndTreeCtrl.DoEdit(hItem);
			}
			else
			{
				pNodeData->Name(strValue.MakeUpper());
			}
			
		}
		m_wndTreeCtrl.SetItemText(hItem,pNodeData->Name());
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}

	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgLandsideVehicleTypeDefine::OnCmdNew()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	LandsideVehicleBaseNode* pNode = new LandsideVehicleTypeNode();
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_LABLE;
	CString strCaption(_T(""));
	if (hItem)
	{
		LandsideVehicleBaseNode* pCurNode = (LandsideVehicleBaseNode*)m_wndTreeCtrl.GetItemData(hItem);
		pCurNode->AddNode(pNode);
		HTREEITEM hNodeItem = m_wndTreeCtrl.InsertItem(strCaption,cni,FALSE,FALSE,hItem,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hNodeItem, (DWORD_PTR)pNode);
		m_wndTreeCtrl.SelectItem(hNodeItem);
		m_wndTreeCtrl.DoEdit(hNodeItem);
	}
	else
	{
		m_pNodeList->AddNode(pNode);
		HTREEITEM hNodeItem = m_wndTreeCtrl.InsertItem(strCaption,cni,FALSE,FALSE,TVI_ROOT,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hNodeItem, (DWORD_PTR)pNode);
		m_wndTreeCtrl.SelectItem(hNodeItem);
		m_wndTreeCtrl.DoEdit(hNodeItem);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgLandsideVehicleTypeDefine::OnCmdDelete()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem)
	{
		LandsideVehicleBaseNode* pCurNode = (LandsideVehicleBaseNode*)m_wndTreeCtrl.GetItemData(hItem);
		pCurNode->RemoveFromParent();
		m_wndTreeCtrl.DeleteItem(hItem);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgLandsideVehicleTypeDefine::OnCmdEdit()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem)
	{
		m_wndTreeCtrl.DoEdit(hItem);
	}
}

void CDlgLandsideVehicleTypeDefine::InsertTreeItem( HTREEITEM hItem,LandsideVehicleTypeNode* pNode )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_LABLE;
	HTREEITEM hNodeItem = m_wndTreeCtrl.InsertItem(pNode->Name(),cni,FALSE,FALSE,hItem);
	m_wndTreeCtrl.SetItemData(hNodeItem,(DWORD_PTR)pNode);
	for (int i = 0; i < pNode->GetNodeCount(); i++)
	{
		LandsideVehicleTypeNode* pChildNode = (LandsideVehicleTypeNode*)pNode->GetNode(i);
		InsertTreeItem(hNodeItem,pChildNode);
	}
	m_wndTreeCtrl.Expand(hNodeItem,TVE_EXPAND);
}

void CDlgLandsideVehicleTypeDefine::SetTreeContent( void )
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);
	
	for (int i=0; i< m_pNodeList->GetNodeCount(); i++)
	{
		InsertTreeItem(TVI_ROOT,(LandsideVehicleTypeNode*)m_pNodeList->GetNode(i));
	}
}

void CDlgLandsideVehicleTypeDefine::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_pNodeList->SaveData(-1);
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
		MessageBox(e.ErrorMessage(),_T("Warning"),MB_OK);
	}

	CXTResizeDialog::OnOK();
}

void CDlgLandsideVehicleTypeDefine::OnBnClickedSave()
{
	try
	{
		CADODatabase::BeginTransaction();
		m_pNodeList->SaveData(-1);
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		MessageBox(e.ErrorMessage(),_T("Warning"),MB_OK);
	}

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CDlgLandsideVehicleTypeDefine::OnTvnSelchangedTreeItem( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if (pNMTreeView->itemNew.hItem)
	{
		m_wndTreeCtrl.SelectItem(pNMTreeView->itemNew.hItem);
	}
	UpdateToolBar();
}


