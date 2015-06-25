// NECRelatedBaseDlg.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "NECRelatedBaseDlg.h"
#include "../Database/DBElementCollection_Impl.h"
#include "..\Database\ARCportDatabaseConnection.h"
#include "../InputAirside/AirsideAircraft.h"
#include "../common/flt_cnst.h"
#include "../Database/ADODatabase.h"

using namespace ADODB;

// CNECRelatedBaseDlg dialog
CNECRelatedBaseDlg::CNECRelatedBaseDlg(PFuncSelectFlightType pSelectFlightType, LPCTSTR lpszCatpion, CWnd* pParent)
 : CToolTipDialog(CNECRelatedBaseDlg::IDD, pParent)
 , m_pSelectFlightType(pSelectFlightType)
 , m_strCaption(lpszCatpion)
{
}

CNECRelatedBaseDlg::~CNECRelatedBaseDlg()
{
}

void CNECRelatedBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_NEC, m_wndListCtrl);
}

BEGIN_MESSAGE_MAP(CNECRelatedBaseDlg, CToolTipDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_NEW_NECRELATEDITEM, OnCmdNewItem)
	ON_COMMAND(ID_DEL_NECRELATEDITEM, OnCmdDeleteItem)
	ON_COMMAND(ID_EDIT_NECRELATEDITEM, OnCmdEditItem)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_NEC, OnLvnItemchangedList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_NEC, OnLvnEndlabeleditListContents)
	ON_NOTIFY(NM_SETFOCUS, IDC_LIST_NEC, OnNMSetfocusListNec)
	ON_NOTIFY(NM_KILLFOCUS, IDC_LIST_NEC, OnNMKillfocusListNec)
END_MESSAGE_MAP()

// CNECRelatedBaseDlg message handlers
BOOL CNECRelatedBaseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	//if (conPtr.GetInterfacePtr() == NULL)
	//	return FALSE;

	//conPtr->BeginTrans();
	CADODatabase::BeginTransaction();

	// set caption of dialog
	SetWindowText(m_strCaption);

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

	// set list control window style
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle(dwStyle);

	// set the layout of child controls
	RefreshLayout();

	// init the list control
	InitListControl();

	// set the toolbar button state
	UpdateToolBarState();

	return TRUE;
}


int CNECRelatedBaseDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CToolTipDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	CToolBarCtrl& toolbar = m_wndToolBar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_NEW_NECRELATEDITEM);
	toolbar.SetCmdID(1, ID_DEL_NECRELATEDITEM);
	toolbar.SetCmdID(2, ID_EDIT_NECRELATEDITEM);

	return 0;
}

void CNECRelatedBaseDlg::OnSize(UINT nType, int cx, int cy)
{
	RefreshLayout(cx, cy);
}

void CNECRelatedBaseDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 400;
	lpMMI->ptMinTrackSize.y = 200;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

// refresh the layout of child controls
void CNECRelatedBaseDlg::RefreshLayout(int cx, int cy)
{
	if (cx == -1 && cy == -1)
	{
		CRect rectClient;
		GetClientRect(&rectClient);
		cx = rectClient.Width();
		cy = rectClient.Height();
	}

	CWnd* pWndSave = GetDlgItem(IDC_BUTTON_SAVE);
	CWnd* pWndOK = GetDlgItem(IDOK);
	CWnd* pWndCancel = GetDlgItem(IDCANCEL);
	if (pWndSave == NULL || pWndOK == NULL || pWndCancel == NULL)
		return;

	CRect rcCancel;
	pWndCancel->GetClientRect(&rcCancel);
	pWndCancel->MoveWindow(cx - rcCancel.Width() - 10,
		cy - rcCancel.Height() - 10, 
		rcCancel.Width(), 
		rcCancel.Height());

	CRect rcOK;
	pWndOK->GetClientRect(&rcOK);
	pWndOK->MoveWindow(cx - rcCancel.Width() - rcOK.Width() - 20,
						cy - rcOK.Height() - 10, 
						rcOK.Width(),
						rcOK.Height());

	CRect rcSave;
	pWndSave->GetClientRect(&rcSave);
	pWndSave->MoveWindow(cx - rcCancel.Width() - rcOK.Width() - rcSave.Width() - 30,
		cy - rcSave.Height() - 10, 
		rcSave.Width(),
		rcSave.Height());

	CWnd* pGroupBox = GetDlgItem(IDC_GROUPBOX_NECRELATED);
	if (pGroupBox == NULL)
		return;
	pGroupBox->MoveWindow(10, 5, cx - 20, cy - rcOK.Height() - 27);

	m_wndListCtrl.MoveWindow(10, 35, cx - 20, cy - rcOK.Height() - 57);

	if (!::IsWindow(m_wndToolBar.m_hWnd))
		return;
	m_wndToolBar.MoveWindow(11, 11, cx - 25, 22);

	Invalidate();
}

// set the toolbar button state
void CNECRelatedBaseDlg::UpdateToolBarState()
{
	if (!::IsWindow(m_wndToolBar.m_hWnd) || !::IsWindow(m_wndListCtrl.m_hWnd))
		return;

	BOOL bEnable = TRUE;
	//if (m_wndListCtrl.GetItemCount() >= m_compositeIndex.GetCount())
	//	bEnable = FALSE;
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_NECRELATEDITEM, bEnable);

	// delete & edit button
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos == NULL)
		bEnable = FALSE;
	else
		bEnable = TRUE;

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_NECRELATEDITEM, bEnable);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_NECRELATEDITEM, bEnable);
}

void CNECRelatedBaseDlg::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	UpdateToolBarState();

	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	if((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
		return;

	if(pnmv->uOldState & LVIS_SELECTED) 
	{
		if(!(pnmv->uNewState & LVIS_SELECTED)) 
		{
			// TRACE("Unselected Item %d...\n", pnmv->iItem);
		}	
	}
	else if(pnmv->uNewState & LVIS_SELECTED) {
		//a new item has been selected
		// TRACE("Selected Item %d...\n", pnmv->iItem);
	}
	else
		ASSERT(0);
}

void CNECRelatedBaseDlg::OnNMSetfocusListNec(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateToolBarState();
	*pResult = 0;
}

void CNECRelatedBaseDlg::OnNMKillfocusListNec(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_NECRELATEDITEM, FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_NECRELATEDITEM, FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_NECRELATEDITEM, FALSE);
	*pResult = 0;
}

void CNECRelatedBaseDlg::InitListControl()
{
	if (!::IsWindow(m_wndListCtrl.m_hWnd))
		return;

	// set the list header contents;
	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("Flight Type");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_wndListCtrl.InsertColumn(0, &lvc);
	
	SetListColumn();
	CheckListContent();
	SetListContent();
}

void CNECRelatedBaseDlg::OnCmdNewItem()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType;
	if (FALSE == (*m_pSelectFlightType)(NULL, fltType))
	{
		return;
	}
	//if (fltType.isDefault())
	//	return;

	// check whether the newly created Flight Type is already presented.
	// if yes, don't do anything
	TCHAR szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);
	int nCount = m_wndListCtrl.GetItemCount();
	CString strFlightType;
	for (int i=0;i<nCount;i++)
	{
		strFlightType = m_wndListCtrl.GetItemText(i, 0);
		if (strFlightType == szBuffer)
		{
			CString strInfo;
			strInfo.Format(_T("The flight type \"%s\" has already been defined."), szBuffer);
			MessageBox(strInfo, _T("Fail to Define"));
			return;
		}
	}

	m_wndListCtrl.InsertItem(m_wndListCtrl.GetItemCount(), szBuffer);
	//CString sBuffer;
	//sBuffer = szBuffer;

	OnNewItem(fltType);
	UpdateToolBarState();

	GetDlgItem(IDC_SAVE)->EnableWindow();
}

void CNECRelatedBaseDlg::OnCmdDeleteItem()
{
	OnDelItem();
	UpdateToolBarState();

	GetDlgItem(IDC_SAVE)->EnableWindow();
}

void CNECRelatedBaseDlg::OnCmdEditItem()
{
	OnEditItem();

	GetDlgItem(IDC_SAVE)->EnableWindow();
}

void CNECRelatedBaseDlg::OnOK()
{
/*	_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	conPtr->CommitTrans()*/;

	CADODatabase::CommitTransaction();

	CToolTipDialog::OnOK();
}

void CNECRelatedBaseDlg::OnCancel()
{
	//_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	//if (conPtr.GetInterfacePtr() == NULL)
	//	return;

	//conPtr->RollbackTrans();
//	ReloadData();
	CADODatabase::RollBackTransation();

	CToolTipDialog::OnCancel();
}

void CNECRelatedBaseDlg::OnBnClickedButtonSave()
{
	/*_ConnectionPtr& conPtr = DATABASECONNECTION.GetConnection();
	if (conPtr.GetInterfacePtr() == NULL)
		return;

	conPtr->CommitTrans();
	conPtr->BeginTrans();*/
	CADODatabase::CommitTransaction();

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
}

void CNECRelatedBaseDlg::OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_wndListCtrl.OnEndlabeledit(pNMHDR, pResult);
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;

	int nItem = plvItem->iItem;
	int nSubItem = plvItem->iSubItem;
	if (nItem < 0)
		return;

	OnListItemChanged(nItem, nSubItem);
	GetDlgItem(IDC_SAVE)->EnableWindow();
	*pResult = 0;
}

int CNECRelatedBaseDlg::GetSelectedListItem()
{
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos)
		return m_wndListCtrl.GetNextSelectedItem(pos);

	return -1;
}

void CNECRelatedBaseDlg::OnEditItem()
{
	int nItem = GetSelectedListItem();
	m_wndListCtrl.EditSubLabel(nItem, 1);
}