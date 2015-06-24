// DlgNewPassengerType.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgNewPassengerType.h"
#include ".\dlgnewpassengertype.h"
#include "../Engine/terminal.h"
#include "../Inputs/typelist.h"
// CDlgNewPassengerType dialog

IMPLEMENT_DYNCREATE(CDlgNewPassengerType, CXTResizeDialog)
CDlgNewPassengerType::CDlgNewPassengerType(CWnd* pParent ):CXTResizeDialog(CDlgNewPassengerType::IDD, pParent)
{

}
CDlgNewPassengerType::CDlgNewPassengerType(CPassengerType* _paxType ,InputTerminal* _terminal ,CWnd* pParent)
:CXTResizeDialog(CDlgNewPassengerType::IDD, pParent)
,m_paxType(_paxType)
,m_terminal(_terminal)
{
	m_strOrigin = m_paxType->GetStringForDatabase();
}

CDlgNewPassengerType::~CDlgNewPassengerType()
{
}

void CDlgNewPassengerType::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_PAXTYPE_LEVEL, m_PaxTyTree);
	DDX_Control(pDX, IDC_STATIC_PAXNAME, m_Static_paxTY);
}

BOOL CDlgNewPassengerType::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
    InitLevelTree() ;
	InitPaxTypeText();
	SetResize(IDC_STATIC_PAXTY,SZ_TOP_LEFT,SZ_TOP_RIGHT) ;
	SetResize(IDC_STATIC_PASSENGERTY,SZ_TOP_LEFT,SZ_TOP_LEFT) ;
	SetResize(IDC_STATIC_PAXNAME,SZ_TOP_LEFT,SZ_TOP_RIGHT) ;
	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDC_STATIC_TEXT,SZ_TOP_LEFT,SZ_TOP_LEFT) ;
	SetResize(IDC_TREE_PAXTYPE_LEVEL,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CDlgNewPassengerType, CXTResizeDialog)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnButtonCancel)
	//ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PAXTYPE_LEVEL, OnTvnSelchangedTreePaxtypeLevel)
	//ON_NOTIFY(NM_RCLICK, IDC_TREE_PAXTYPE_LEVEL, OnNMRclickTreePaxtypeLevel)
	//ON_NOTIFY(NM_CLICK, IDC_TREE_PAXTYPE_LEVEL, OnNMClickTreePaxtypeLevel)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CDlgNewPassengerType::OnOK()
{
	CXTResizeDialog::OnOK() ;
}

void CDlgNewPassengerType::OnSize(UINT nType, int cx, int cy) 
{
	CXTResizeDialog::OnSize(nType,cx,cy) ;
}

void CDlgNewPassengerType::OnButtonCancel()
{
	m_paxType->FormatLevelPaxTypeFromString(m_strOrigin) ;
	CXTResizeDialog::OnCancel() ;
}

// CDlgNewPassengerType message handlers
void CDlgNewPassengerType::InitLevelTree()
{
	for( int i=0; i<MAX_PAX_TYPES; i++ )
	{
		int nBranch = m_terminal->inTypeList->getBranchesAtLevel( i );
		if( nBranch == 0 )
			break;

		CString csLabel;
		int nValue = m_paxType->getUserType(i);
		if (nValue >0)
		{
			csLabel.Format("%s :   %s",m_terminal->inTypeList->GetLevelName(i), m_terminal->inStrDict->getString (nValue)) ;
			HTREEITEM hItem = InsertTreeNode(csLabel,i) ;
			m_PaxTyTree.SetCheckStatus(hItem, TRUE);
		}
		else
		{
			csLabel.Format( "%s",m_terminal->inTypeList->GetLevelName(i));
			InsertTreeNode(csLabel,i) ;
		}
	}
}

HTREEITEM CDlgNewPassengerType::InsertTreeNode(CString _itemText,int level )
{
	COOLTREE_NODE_INFO info ;
	CCoolTree::InitNodeInfo(this,info);
	info.nt=NT_CHECKBOX;
	info.net = NET_COMBOBOX;
	info.bAutoSetItemText = FALSE ;
	HTREEITEM item = m_PaxTyTree.InsertItem(_itemText,info,TRUE) ;
	m_PaxTyTree.SetItemData(item,level) ;
	m_PaxTyTree.SetCheckStatus(item,FALSE) ;
	return item;
}

void CDlgNewPassengerType::InitComboString(CComboBox* _combox , int _level)
{
	if( _level >= MAX_PAX_TYPES )
		return;

	int nBranch = m_terminal->inTypeList->getBranchesAtLevel( _level );
	for( int i=0; i<nBranch; i++ )
	{
		int index = _combox->AddString( m_terminal->inTypeList->getTypeName( i, _level ) );
	}
}

void CDlgNewPassengerType::InitPaxTypeText()
{
	m_Static_paxTY.SetWindowText(m_paxType->PrintStringForShow()) ;
}

LRESULT CDlgNewPassengerType::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == UM_CEW_COMBOBOX_BEGIN)
	{
		CWnd* pWnd = m_PaxTyTree.GetEditWnd((HTREEITEM)wParam);
		CRect rectWnd;
		HTREEITEM hItem = (HTREEITEM)wParam;
		if(m_PaxTyTree.IsCheckItem(hItem) != 1)
			return 0;

		m_PaxTyTree.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
		pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
		CComboBox* pCB=(CComboBox*)pWnd;
		int level = m_PaxTyTree.GetItemData(hItem) ;
		if (pCB->GetCount() != 0)
		{
			pCB->ResetContent();
		}

		if (pCB->GetCount() == 0)
		{
			InitComboString(pCB,level) ;
			pCB->SetCurSel(0);
		}	
	}
	else if (message == UM_CEW_COMBOBOX_END)
	{
		HTREEITEM hItem = (HTREEITEM)wParam;
		if(m_PaxTyTree.IsCheckItem(hItem) == 1)
		{
			int nLevel = m_PaxTyTree.GetItemData(hItem) ;
			CWnd* pWnd=m_PaxTyTree.GetEditWnd(hItem);
			CComboBox* pTCB=(CComboBox*)pWnd;
			int index = m_PaxTyTree.GetCmbBoxCurSel(hItem) ;
			CString str ="";
			pTCB->GetLBText(index,str) ;
			m_paxType->setUserType(nLevel,m_terminal->inStrDict->findString(str));
			SetTreeNodeText((HTREEITEM)wParam);
			InitPaxTypeText();
		}
		
	}
	else if(message == UM_CEW_STATUS_CHANGE)
	{
		HTREEITEM item = (HTREEITEM)wParam;
		int level = (int)m_PaxTyTree.GetItemData(item) ;
		if(m_PaxTyTree.IsCheckItem(item) != 1)
		{
			m_paxType->setUserType(level,0);
			CString node ;
			node.Format("%s",m_terminal->inTypeList->GetLevelName(level)) ;
			m_PaxTyTree.SetItemText(item,node) ;
		}
		else
		{
			CString node ;
			node.Format("%s: All",m_terminal->inTypeList->GetLevelName(level)) ;
			m_PaxTyTree.SetItemText(item,node) ;
			m_paxType->setUserType(level,0);
		}
		InitPaxTypeText();
	}
	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgNewPassengerType::SetTreeNodeText(HTREEITEM Item)
{
	CWnd* pWnd=m_PaxTyTree.GetEditWnd(Item);
	CComboBox* pTCB=(CComboBox*)pWnd;
	int index = m_PaxTyTree.GetCmbBoxCurSel(Item) ;
	CString str ;
	pTCB->GetLBText(index,str) ;
	int level = m_PaxTyTree.GetItemData(Item) ;
	CString NodeName ;
	NodeName.Format("%s :   %s",m_terminal->inTypeList->GetLevelName(level),str) ;
	m_PaxTyTree.SetItemText(Item,NodeName);
}

//void CDlgNewPassengerType::OnTvnSelchangedTreePaxtypeLevel(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//    //pNMTreeView->
//
//	*pResult = 0;
//}
//
//void CDlgNewPassengerType::OnNMRclickTreePaxtypeLevel(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}
//
//void CDlgNewPassengerType::OnNMClickTreePaxtypeLevel(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}
