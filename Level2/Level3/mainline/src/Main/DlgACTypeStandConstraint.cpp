// DlgACTypeStandConstraint.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "../inputs/IN_TERM.H"
#include ".\dlgactypestandconstraint.h"
#include "TermPlanDoc.h"
#include "SelectStandDialog.h"
#include "../InputAirside/InputAirside.h"
#include "DlgACTypeSelection.h"
#include "../InputAirside/ACTypeStandConstraint.h"
#include "../InputAirside/ALTObjectGroup.h"
#include "../Common/ALTObjectID.h"
#include "../Database/ADODatabase.h"
#include "DlgImportACTypeConstraintFromFile.h"
#include "../AirsideGUI/DlgStandFamily.h"
// DlgACTypeStandConstraint dialog

IMPLEMENT_DYNAMIC(DlgACTypeStandConstraint, CXTResizeDialog)
DlgACTypeStandConstraint::DlgACTypeStandConstraint(InputTerminal* pTerminal,int nAirportID,int nProjID,ACTypeStandConstraintList* pConstraints, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(DlgACTypeStandConstraint::IDD, pParent)
	,m_pInputTerminal(pTerminal)
	,m_nAirportID(nAirportID)
	,m_pACTypeStandConstraints(pConstraints)
	,m_nProjID(nProjID)
{
}

DlgACTypeStandConstraint::~DlgACTypeStandConstraint()
{
}

void DlgACTypeStandConstraint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_ACTYPECONSTRAINT, m_wndTreeCtrl);
}


BEGIN_MESSAGE_MAP(DlgACTypeStandConstraint, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD,AddNewItem)
	ON_COMMAND(ID_TOOLBARBUTTONDEL,RemoveItem)
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_ACTYPECONSTRAINT, OnSelchangedTree)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_IMPORTFROMFILE,OnBnClickedImportFromFile)
END_MESSAGE_MAP()


// DlgACTypeStandConstraint message handlers
void DlgACTypeStandConstraint::OnBnClickedImportFromFile()
{
	ACTypeStandConstraintList ImportCons;
	DlgImportACTypeConstraintFromFile dlg(m_nAirportID, &ImportCons);
	if (dlg.DoModal() == IDOK && !ImportCons.m_vDataList.empty())
	{
		m_pACTypeStandConstraints->m_vDataList.assign(ImportCons.m_vDataList.begin(),ImportCons.m_vDataList.end());
		ImportCons.m_vDataList.clear();

		OnInitTreeCtrl();
	}
}


void DlgACTypeStandConstraint::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pACTypeStandConstraints->SaveData();
		CADODatabase::CommitTransaction() ;
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	}
	catch (CADOException* error)
	{
		CADODatabase::RollBackTransation() ;
		CString strError = _T("");
		strError.Format("Database operation error: %s",error->ErrorMessage());
		MessageBox(strError);

		delete error;
		error = NULL;
		return;
	}
}

void DlgACTypeStandConstraint::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnBnClickedButtonSave();
	OnOK();
}

void DlgACTypeStandConstraint::OnBnClickedCancel()
{
	OnCancel();
}

BOOL DlgACTypeStandConstraint::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	CRect rect;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rect);
	GetDlgItem(IDC_STATIC_TOOLBAR)->ShowWindow(FALSE);
	ScreenToClient(&rect);
	m_wndToolbar.MoveWindow(&rect,true);
	m_wndToolbar.ShowWindow(SW_SHOW);

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,FALSE);
	m_wndToolbar.GetToolBarCtrl().HideButton(ID_TOOLBARBUTTONEDIT,TRUE);
	OnInitTreeCtrl();

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_TREE_ACTYPECONSTRAINT,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_IMPORTFROMFILE,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

int DlgACTypeStandConstraint::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS,CRect(0,0,0,0),IDR_ADDDELEDITTOOLBAR)||
		!m_wndToolbar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void DlgACTypeStandConstraint::OnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_hRclickItem = m_wndTreeCtrl.GetSelectedItem();

	if (m_hRclickItem == NULL)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
	}
	else 
	{
		HTREEITEM hParItem = m_wndTreeCtrl.GetParentItem(m_hRclickItem);

		HTREEITEM hGrandItem= NULL;
		if (hParItem)
			hGrandItem = m_wndTreeCtrl.GetParentItem(hParItem);

		if ( hParItem == NULL)
		{
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,TRUE);
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
		}
		else
		{
			if (hGrandItem == NULL)
			{
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,TRUE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,TRUE);
			}
			else
			{
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,FALSE);
				m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,TRUE);
			}

		}

	}
}

void DlgACTypeStandConstraint::OnInitTreeCtrl()
{
	m_wndTreeCtrl.DeleteAllItems();
	m_wndTreeCtrl.SetImageList(m_wndTreeCtrl.GetImageList(TVSIL_NORMAL),TVSIL_NORMAL);
	HTREEITEM hItem;
	HTREEITEM hChildItem;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;

	HTREEITEM hRoot = m_wndTreeCtrl.InsertItem("Constraints:",cni,FALSE,FALSE,TVI_ROOT,TVI_LAST);
	int nCount =  m_pACTypeStandConstraints->getConstaintCount();



	for(int i = 0; i < nCount; i++)
	{
		ACTypeStandConstraint* pData = m_pACTypeStandConstraints->GetConstraintByIdx(i);

		ASSERT(pData);	//pData cannot be null
		if (pData == NULL)
			continue;

		CString strName = pData->m_StandGroup.getName().GetIDString();
		if (strName.IsEmpty())
			strName = "All Stands";

		CString strStandGroup = "Stand Group: "+ strName;
		
		hItem = m_wndTreeCtrl.InsertItem(strStandGroup,cni,FALSE,FALSE,hRoot,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pData);

		POSITION pos;
		for( pos = pData->m_strACTypeList.GetHeadPosition(); pos != NULL; )
		{
			CString strAC = pData->m_strACTypeList.GetNext( pos );
			hChildItem = m_wndTreeCtrl.InsertItem(strAC,cni,FALSE,FALSE,hItem,TVI_LAST);			
		}
		m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	}
	m_wndTreeCtrl.Expand(hRoot,TVE_EXPAND);
}

void DlgACTypeStandConstraint::AddNewItem()
{

	HTREEITEM hSel = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hSel);
	if (hSel == NULL)
		return;

	if (hParentItem == NULL)
	{
		CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
		ASSERT( pMDIActive != NULL );

		CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
		ASSERT( pDoc!= NULL );

		std::vector<int> vAirport;
		InputAirside::GetAirportList(pDoc->GetProjectID(),vAirport);
		if(vAirport.size()<1) return ;

	/*	CSelectALTObjectDialog objDlg(0,vAirport[0]);
		objDlg.SetType(ALT_STAND);*/
		CDlgStandFamily objDlg(m_nProjID);
		if(objDlg.DoModal()!=IDOK) return ;

		CString newIDstr;
	/*	if( !objDlg.GetObjectIDString(newIDstr) )  
			return;	*/
		newIDstr = objDlg.GetSelStandFamilyName();
		if (newIDstr.IsEmpty())
			return;
		ACTypeStandConstraint* pData = new ACTypeStandConstraint;
		ALTObjectID GroupName(newIDstr.GetString());
		pData->m_StandGroup.setName(GroupName);

		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		cni.nt = NT_NORMAL;
		cni.net = NET_NORMAL;
		
		CString strName = "Stand Group: "+ newIDstr;
		HTREEITEM hItem = m_wndTreeCtrl.InsertItem(strName,cni,FALSE,FALSE,hSel,TVI_LAST);
		m_wndTreeCtrl.SetItemData(hItem,(DWORD_PTR)pData);
		strName = _T("All AC Types");
		pData->m_strACTypeList.AddTail(strName);
		HTREEITEM hChild = m_wndTreeCtrl.InsertItem(strName,cni,FALSE,FALSE,hItem,TVI_LAST);
		m_pACTypeStandConstraints->m_vDataList.push_back(pData);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		m_wndTreeCtrl.Expand(hSel,TVE_EXPAND);
		m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	}
	else
	{
		HTREEITEM hGrand = 	m_wndTreeCtrl.GetParentItem(hParentItem);
		if (hGrand == NULL)
		{
			AddACTypes();
			m_wndTreeCtrl.Expand(hSel,TVE_EXPAND);
		}
	}


}

void DlgACTypeStandConstraint::RemoveItem()
{
	HTREEITEM hSel = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hSel);
	if (hSel == NULL || hParentItem == NULL)
		return;

	HTREEITEM hGrand = 	m_wndTreeCtrl.GetParentItem(hParentItem);
	if (hGrand == NULL)
	{
		ACTypeStandConstraint* pDelData = (ACTypeStandConstraint*)m_wndTreeCtrl.GetItemData(hSel);
		m_pACTypeStandConstraints->DelConstraint(pDelData);
		m_wndTreeCtrl.DeleteItem(hSel);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
	else
	{
		DelACType();
	}
}

void DlgACTypeStandConstraint::AddACTypes()
{
	DlgACTypeSelections dlg(m_pInputTerminal);
	if (dlg.DoModal()!= IDOK)
		return;

	HTREEITEM hSel = m_wndTreeCtrl.GetSelectedItem();
	ACTypeStandConstraint* pData = (ACTypeStandConstraint*)m_wndTreeCtrl.GetItemData(hSel);
	if (pData == NULL)
		return;

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;

	HTREEITEM hChildItem = NULL;
	POSITION pos;
	for( pos = dlg.getSelACTyeps().GetHeadPosition(); pos != NULL; )
	{
		CString strAC = dlg.getSelACTyeps().GetNext( pos );
		pData->m_strACTypeList.AddTail(strAC);	
		hChildItem = m_wndTreeCtrl.InsertItem(strAC,cni,FALSE,FALSE,hSel,TVI_LAST);	
	}
}

void DlgACTypeStandConstraint::DelACType()
{
	HTREEITEM hSel = m_wndTreeCtrl.GetSelectedItem();
	HTREEITEM hParentItem = m_wndTreeCtrl.GetParentItem(hSel);	
	m_wndTreeCtrl.DeleteItem(hSel);

	ACTypeStandConstraint* pData = (ACTypeStandConstraint*)m_wndTreeCtrl.GetItemData(hParentItem);
	if (pData == NULL)
		return;

	pData->m_strACTypeList.RemoveAll();
	if (m_wndTreeCtrl.ItemHasChildren(hParentItem))
	{
		HTREEITEM hItem = m_wndTreeCtrl.GetChildItem(hParentItem);

		while (hItem != NULL)
		{
			CString strAC = m_wndTreeCtrl.GetItemText(hItem);
			pData->m_strACTypeList.AddTail(strAC);
			hItem = m_wndTreeCtrl.GetNextSiblingItem(hItem);
		}
	}

}