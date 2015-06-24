// PaxDispPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "CalloutDispSpecDlg.h"
#include "TermPlanDoc.h"
#include "common\winmsg.h"
#include "CalloutDispSpec.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalloutDispSpecDlg dialog


CCalloutDispSpecDlg::CCalloutDispSpecDlg(PFuncSelectFlightType pSelFltType, int nProjID, CTermPlanDoc* pTermPlanDoc, CWnd* pParent/* = NULL*/)
	: CDialog(CCalloutDispSpecDlg::IDD, pParent)
	, m_pSelFltType(pSelFltType)
	, m_nProjID(nProjID)
	, m_listctrlProp(pSelFltType, nProjID, pTermPlanDoc)
	, m_pTermPlanDoc(pTermPlanDoc)
	, m_calloutDispSpecs(pTermPlanDoc->m_calloutDispSpecs)
	, m_bNewName(FALSE)
{
	ASSERT(pTermPlanDoc);

	m_calloutDispSpecs.ReadData(m_nProjID, m_pTermPlanDoc);
}

void CCalloutDispSpecDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER2, m_toolbarcontenter2);
	DDX_Control(pDX, IDC_STATIC_BARFRAME2, m_btnBarFrame2);
	DDX_Control(pDX, IDC_LIST_DISPPROP2, m_listNamedSets);
	DDX_Control(pDX, IDC_STATIC_BARFRAME, m_btnBarFrame);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Control(pDX, IDC_LIST_DISPPROP, m_listctrlProp);
}


BEGIN_MESSAGE_MAP(CCalloutDispSpecDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_DISPPROP, OnClickListDispprop)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DISPPROP2, OnItemChangedListNameSets)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DISPPROP2, OnEndLabelEditListNameSets)
	ON_COMMAND(ID_NAME_ADD, OnNameAdd)
	ON_COMMAND(ID_NAME_DEL, OnNameDel)
	ON_COMMAND(ID_DATA_ADD, OnDataAdd)
	ON_COMMAND(ID_DATA_DEL, OnDataDel)
	ON_COMMAND(ID_DATA_EDIT, OnDataEdit)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCalloutDispSpecDlg message handlers

BOOL CCalloutDispSpecDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(_T("Callout Display Specification"));

	// TODO: Add extra initialization here
	InitToolbar();
	InitNameSetListHeader();
	m_listctrlProp.InitListHeader();

	// Load data.
	ResetNameListContent();
	// Data List Ctrl
	ResetDataListContent();

	UpdateUIState();


	m_btnSave.ShowWindow( SW_HIDE );

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void CCalloutDispSpecDlg::OnCancel() 
{
	CDialog::OnCancel();

	m_calloutDispSpecs.ReadData(m_nProjID, m_pTermPlanDoc);
}

void CCalloutDispSpecDlg::OnOK() 
{
	CString strError;
	if (false == m_calloutDispSpecs.CheckValid(strError))
	{
		MessageBox(strError);
		return;
	}
	m_calloutDispSpecs.SaveData(m_nProjID);

	CDialog::OnOK();
}


void CCalloutDispSpecDlg::InitToolbar()
{
	CRect rc;

	// NOTE: the top toolbar
	m_toolbarcontenter2.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar2.MoveWindow(&rc);
	m_ToolBar2.ShowWindow(SW_SHOW);
	m_toolbarcontenter2.ShowWindow(SW_HIDE);
	m_ToolBar2.GetToolBarCtrl().SetCmdID(0, ID_NAME_ADD);
	m_ToolBar2.GetToolBarCtrl().SetCmdID(1, ID_NAME_DEL);
	m_ToolBar2.GetToolBarCtrl().SetCmdID(2, ID_NAME_EDIT);
	m_ToolBar2.GetToolBarCtrl().EnableButton( ID_NAME_ADD );
	m_ToolBar2.GetToolBarCtrl().EnableButton( ID_NAME_DEL );
	m_ToolBar2.GetToolBarCtrl().HideButton( ID_NAME_EDIT );

	// NOTE: the bottom toolbar
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().SetCmdID(0, ID_DATA_ADD);
	m_ToolBar.GetToolBarCtrl().SetCmdID(1, ID_DATA_DEL);
	m_ToolBar.GetToolBarCtrl().SetCmdID(2, ID_DATA_EDIT);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_DATA_ADD );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_DATA_DEL );
	m_ToolBar.GetToolBarCtrl().HideButton( ID_DATA_EDIT );
}

int CCalloutDispSpecDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_ToolBar2.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar2.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void CCalloutDispSpecDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	CRect btnrc,toolbarrc,rc;
	m_btnSave.GetWindowRect( &btnrc );


	m_toolbarcontenter.GetWindowRect( &toolbarrc );

	m_btnBarFrame.MoveWindow( 9,(cy/3)+4,cx-20,(2*cy/3)-25-btnrc.Height() );
	m_toolbarcontenter.MoveWindow( 11,(cy/3)+10,cx-40,toolbarrc.Height() );
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_toolbarcontenter.ShowWindow( SW_HIDE );
	m_ToolBar.MoveWindow(&rc);
	m_btnBarFrame.GetWindowRect( &rc );
	ScreenToClient(&rc);
	rc.top += 30;
	m_listctrlProp.MoveWindow( 9,(cy/3)+35,rc.Width(),rc.Height() );


	m_btnBarFrame2.MoveWindow( 9,4,cx-20,(cy/3)-25 );
	m_toolbarcontenter2.MoveWindow( 11,10,cx-40,toolbarrc.Height() );
	m_toolbarcontenter2.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_toolbarcontenter2.ShowWindow( SW_HIDE );
	m_ToolBar2.MoveWindow(&rc);
	m_btnBarFrame2.GetWindowRect( &rc );
	ScreenToClient(&rc);
	rc.top = 35;
	m_listNamedSets.MoveWindow( 9,35,rc.Width(),rc.Height() );

	m_btnSave.MoveWindow( cx-20-3*btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnOk.MoveWindow( cx-15-2*btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnCancel.MoveWindow( cx-10-btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );

	InvalidateRect(NULL);
}

void CCalloutDispSpecDlg::OnDataAdd() 
{
	// TODO: Add your command handler code here
	CalloutDispSpecItem* pSpecItem = GetSelCalloutDispSpecItem();
	if (NULL == pSpecItem)
	{
		return;
	}
	CalloutDispSpecDataItem* pDataItem = new CalloutDispSpecDataItem();
	pDataItem->RefreshCientFromData(m_pTermPlanDoc);
	pSpecItem->GetData().AddNewItem(pDataItem);
	ResetDataListContent();
	UpdateUIState();
}

void CCalloutDispSpecDlg::OnDataDel() 
{
	// TODO: Add your command handler code here
	CalloutDispSpecItem* pSpecItem = GetSelCalloutDispSpecItem();
	if (NULL == pSpecItem)
	{
		return;
	}
	CalloutDispSpecDataItem* pDataItem = GetSelCalloutDispSpecDataItem();
	if (NULL == pDataItem)
	{
		return;
	}
	
	pSpecItem->GetData().DeleteItem(pDataItem);
	ResetDataListContent();

	UpdateUIState();
}


void CCalloutDispSpecDlg::OnDataEdit() 
{
	// TODO: Add your command handler code here
// 	CalloutDispSpecDataItem* pDataItem = GetSelCalloutDispSpecDataItem();
// 	if (NULL == pDataItem)
// 	{
// 		return;
// 	}
// 	CCalloutDispSpecDataDlg dlg(pDataItem, m_pSelFltType, m_nProjID);
// 	if (IDOK == dlg.DoModal())
// 	{
// 		ResetDataListContent();
// 	}
// 
// 	UpdateUIState();
}

void CCalloutDispSpecDlg::OnClickListDispprop(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if( m_listctrlProp.GetSelectedCount() > 0 )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONEDIT );
	}
	*pResult = 0;
}

void CCalloutDispSpecDlg::OnItemChangedListNameSets(NMHDR* pNMHDR, LRESULT* pResult)
{
	ResetDataListContent();

	UpdateUIState();
}


void CCalloutDispSpecDlg::OnNameAdd()
{
	CalloutDispSpecItem* pSpecItem = new CalloutDispSpecItem;
	m_calloutDispSpecs.AddNewItem(pSpecItem);
	int nIndex = m_listNamedSets.InsertItem(m_listNamedSets.GetItemCount(), _T("         "));
	m_listNamedSets.SetItemData(nIndex, (DWORD)pSpecItem);
	m_listNamedSets.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
	m_listNamedSets.SetFocus();
	m_listNamedSets.EditLabel(nIndex);

	UpdateUIState();

	m_bNewName = TRUE;
}

void CCalloutDispSpecDlg::OnNameDel()
{
	CalloutDispSpecItem* pSpecItem = GetSelCalloutDispSpecItem();
	m_calloutDispSpecs.DeleteItem(pSpecItem);

	ResetNameListContent();
	ResetDataListContent();

	UpdateUIState();
}

void CCalloutDispSpecDlg::OnEndLabelEditListNameSets(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	if(NULL == pDispInfo->item.pszText && m_bNewName)
	{
		OnNameDel();
		m_bNewName = FALSE;
		return;
	}
	m_bNewName = FALSE;


	if (NULL != pDispInfo->item.pszText)
	{
		int nIndex = pDispInfo->item.iItem;
		CalloutDispSpecItem* pSpecItem = (CalloutDispSpecItem*)m_listNamedSets.GetItemData(nIndex);
		if (pSpecItem)
		{
			pSpecItem->SetName(pDispInfo->item.pszText);
			m_listNamedSets.SetItemText(nIndex, 0, pDispInfo->item.pszText);
		}
	}

	*pResult = 1;
}

void CCalloutDispSpecDlg::ResetNameListContent()
{
	// Name List Ctrl
	m_listNamedSets.DeleteAllItems();
	size_t nNameCount = m_calloutDispSpecs.GetElementCount();
	for(size_t i=0;i<nNameCount;i++)
	{
		CalloutDispSpecItem* pSpecItem = m_calloutDispSpecs.GetItem(i);
		int nIndex = m_listNamedSets.InsertItem(i, pSpecItem->GetName());
		m_listNamedSets.SetItemData(nIndex, (DWORD)pSpecItem);
	}
	m_listNamedSets.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED );
}


void CCalloutDispSpecDlg::InitNameSetListHeader()
{
	DWORD dwStyle = m_listNamedSets.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listNamedSets.SetExtendedStyle( dwStyle );

	m_listNamedSets.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 120);
}

void CCalloutDispSpecDlg::ResetDataListContent()
{
	m_listctrlProp.ResetListContent(GetSelCalloutDispSpecItem());
}

CalloutDispSpecItem* CCalloutDispSpecDlg::GetSelCalloutDispSpecItem()
{
	CalloutDispSpecItem* pSpecItem = NULL;
	POSITION pos = m_listNamedSets.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nSelIndex = m_listNamedSets.GetNextSelectedItem(pos);
		pSpecItem = (CalloutDispSpecItem*)m_listNamedSets.GetItemData(nSelIndex);
	}
	return pSpecItem;
}

CalloutDispSpecDataItem* CCalloutDispSpecDlg::GetSelCalloutDispSpecDataItem()
{
	CalloutDispSpecDataItem* pDataItem = NULL;
	int nSelIndex = m_listctrlProp.GetCurSel();
	if (-1 != nSelIndex)
	{
		pDataItem = (CalloutDispSpecDataItem*)m_listctrlProp.GetItemData(nSelIndex);
	}
	return pDataItem;
}

void CCalloutDispSpecDlg::UpdateUIState()
{
	bool bNameItemSelected = m_listNamedSets.GetSelectedCount()>0;
	m_ToolBar2.GetToolBarCtrl().EnableButton(ID_NAME_DEL, bNameItemSelected);

	m_ToolBar.GetToolBarCtrl().EnableButton(ID_DATA_ADD, bNameItemSelected);
	m_listctrlProp.EnableWindow(bNameItemSelected);

	bool bDataItemSelected = bNameItemSelected && m_listctrlProp.GetSelectedCount()>0;
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_DATA_EDIT, bDataItemSelected);
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_DATA_DEL, bDataItemSelected);
}