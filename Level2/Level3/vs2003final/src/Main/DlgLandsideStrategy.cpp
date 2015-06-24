#include "StdAfx.h"
#include "TermPlan.h"
#include ".\dlglandsidestrategy.h"
#include "landside/LandsideLayoutObject.h"
#include "TermPlanDoc.h"
#include "landside/InputLandside.h"


IMPLEMENT_DYNAMIC(CDlgLandsideStrategy, CXTResizeDialog)
CDlgLandsideStrategy::CDlgLandsideStrategy(CWnd* pParent /*=NULL*/)
: CXTResizeDialog(CDlgLandsideStrategy::IDD, pParent)
{
}

CDlgLandsideStrategy::~CDlgLandsideStrategy(void)
{
	ClearTreeNodeData();
}

void CDlgLandsideStrategy::DoDataExchange( CDataExchange* pDX )
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_DATA,m_wndTreeCtrl);
}

BEGIN_MESSAGE_MAP(CDlgLandsideStrategy, CXTResizeDialog)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_DATA,OnLvnItemchangedList)
	ON_NOTIFY(TVN_DELETEITEM, IDC_TREE_DATA, OnDeleteTreeItem)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
	ON_COMMAND(ID_TOOLBAR_NEW,OnCmdNewItem)
	ON_COMMAND(ID_TOOLBAR_DELETE,OnCmdDeleteItem)
	ON_MESSAGE(WM_LBUTTONDBLCLK,OnLeftDoubleClick)
END_MESSAGE_MAP()

BOOL CDlgLandsideStrategy::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	InitToolBar();
	SetTitleHeard();

	ReadData();
	LoadTreeContent();

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_GROUP, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolbar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TREE_DATA, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgLandsideStrategy::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction();
		SaveData();
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	DestroyWindow();
//	CXTResizeDialog::OnOK();
}

int CDlgLandsideStrategy::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndToolbar.LoadToolBar(IDR_ADDDELTOOLBAR))
	{
		return -1;
	}

	CToolBarCtrl& toolbar = m_wndToolbar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_TOOLBAR_NEW);
	toolbar.SetCmdID(1, ID_TOOLBAR_DELETE);

	return 0;
}

void CDlgLandsideStrategy::InitToolBar( void )
{
	CRect rect;
	m_wndTreeCtrl.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 4;
	m_wndToolbar.MoveWindow(&rect);

	m_wndToolbar.GetToolBarCtrl().EnableButton( ID_TOOLBAR_NEW, TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton( ID_TOOLBAR_DELETE, FALSE);
}

void CDlgLandsideStrategy::LoadTreeContent()
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);

	SetTreeContent();

	HTREEITEM hItem = m_wndTreeCtrl.GetRootItem();
	if (hItem)
	{
		m_wndTreeCtrl.SelectSetFirstVisible(hItem);
		m_wndTreeCtrl.SetFocus();
		m_wndTreeCtrl.SetItemState(hItem,TVIS_SELECTED,TVIS_SELECTED);
	}
}

void CDlgLandsideStrategy::DeleteTreeNodeData( TreeNodeData* pNodeData )
{
	std::vector<TreeNodeData*>::iterator iter = std::find(m_vNodeData.begin(),m_vNodeData.end(),pNodeData);
	if (iter != m_vNodeData.end())
	{
		m_vNodeData.erase(iter);
		delete pNodeData;
	}
}

void CDlgLandsideStrategy::ClearTreeNodeData()
{
	for (unsigned i = 0; i < m_vNodeData.size(); i++)
	{
		delete m_vNodeData[i];
	}
	m_vNodeData.clear();
}

LandsideFacilityLayoutObjectList* CDlgLandsideStrategy::GetLandsideLayoutObjectList()
{
	InputLandside* pInputLandside = GetInputLandside();
	if (pInputLandside == NULL)
		return NULL;

	return &pInputLandside->getObjectList();
}

InputLandside* CDlgLandsideStrategy::GetInputLandside()
{
	if (m_pParentWnd == NULL)
		return NULL;

	CTermPlanDoc* pDoc = (CTermPlanDoc*)(((CView*)m_pParentWnd)->GetDocument());
	if (pDoc == NULL)
		return NULL;

	return pDoc->getARCport()->GetInputLandside();
}

void CDlgLandsideStrategy::OnCmdNewItem()
{
	OnAddTreeItem();
}

void CDlgLandsideStrategy::OnCmdDeleteItem()
{
	OnDeleteTreeItem();
}

void CDlgLandsideStrategy::OnContextMenu( CWnd* pWnd, CPoint point )
{
	CRect rectTree;
	m_wndTreeCtrl.GetWindowRect(&rectTree);
	if (!rectTree.PtInRect(point)) 
		return;

	m_wndTreeCtrl.SetFocus();
	CPoint pt = point;
	m_wndTreeCtrl.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hRClickItem = m_wndTreeCtrl.HitTest(pt, &iRet);
	if (iRet != TVHT_ONITEMLABEL)
	{
		hRClickItem = NULL;
		return;
	}

	m_wndTreeCtrl.SelectItem(hRClickItem);
	TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hRClickItem);
	if (pNodeData == NULL)
		return;

	OnDisplayMenu(pNodeData,point);
}

void CDlgLandsideStrategy::OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult )
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	if (pNMTreeView->itemNew.hItem)
	{
		m_wndTreeCtrl.SelectItem(pNMTreeView->itemNew.hItem);
		if (pNMTreeView->itemNew.hItem != pNMTreeView->itemOld.hItem)
		{
			UpdateToolbar(pNMTreeView->itemNew.hItem);
		}
		return;
	}
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEW,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELETE,FALSE);
}

void CDlgLandsideStrategy::OnDeleteTreeItem( NMHDR *pNMHDR, LRESULT *pResult )
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	UpdateDeleteOperationToolbar();
}

LRESULT CDlgLandsideStrategy::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	HandleTreeMessage(message,wParam,lParam);
	return CXTResizeDialog::DefWindowProc(message,wParam,lParam);
}

LRESULT CDlgLandsideStrategy::OnLeftDoubleClick( WPARAM wParam, LPARAM lParam )
{
	HandLDoubleClickMessage(wParam,lParam);
	return 0;
}

void CDlgLandsideStrategy::OnBtnSave()
{
	try
	{
		CADODatabase::BeginTransaction();
		SaveData();
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

bool CDlgLandsideStrategy::FindFinalPriorityItem( HTREEITEM hItem, HTREEITEM& hFinalItem )
{
	HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(hItem);
	while (hChildItem)
	{
		TreeNodeData* pNodeData = (TreeNodeData*)m_wndTreeCtrl.GetItemData(hChildItem);
		if (pNodeData && pNodeData->m_emType == TreeNodeData::FINALPRIORIY_Node)
		{
			return true;
		}
		else
		{
			hFinalItem = hChildItem;
		}
		hChildItem = m_wndTreeCtrl.GetNextSiblingItem(hChildItem);
	}
	return false;
}

void CDlgLandsideStrategy::OnCancel()
{
	DestroyWindow();
}
