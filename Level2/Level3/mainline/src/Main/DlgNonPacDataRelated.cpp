// DlgNonPacDataRelated.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgNonPacDataRelated.h"
#include "DlgTimeRange.h"
#include <algorithm>
#include "DlgSelectLandsideVehicleType.h"
#include <Common/WinMsg.h>
#include "Landside/NonPaxRelatedDataContainer.h"

// CDlgNonPaxDataRelated dialog

IMPLEMENT_DYNAMIC(CDlgNonPaxDataRelated, CXTResizeDialog)
CDlgNonPaxDataRelated::CDlgNonPaxDataRelated(int nProjID,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgNonPaxDataRelated::IDD, pParent)
	,m_nProjID(nProjID)
{
	m_landsideNonPaxDataRelated = new NonPaxRelatedDataContainer();
	m_landsideNonPaxDataRelated->ReadData(nProjID);
}

CDlgNonPaxDataRelated::~CDlgNonPaxDataRelated()
{
	delete m_landsideNonPaxDataRelated;
}

void CDlgNonPaxDataRelated::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_DATA,m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgNonPaxDataRelated, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBAR_ADD,OnCmdNewItem)
	ON_COMMAND(ID_TOOLBAR_DEL,OnCmdDeleteItem)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnLvnItemchangedList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DATA, OnLvnEndlabeleditListContents)
	ON_MESSAGE(WM_NOEDIT_DBCLICK,OnDbClickListCtrl)
END_MESSAGE_MAP()

// CDlgNonPaxDataRelated message handlers
BOOL CDlgNonPaxDataRelated::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);

	getData().ReadData(-1);
	OnInitToolbar();
	InitListControl();
	SetListContent();
	UpdateToolBarState();

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_GROUP, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LIST_DATA, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgNonPaxDataRelated::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndToolBar.LoadToolBar(IDR_ADDDELTOOLBAR))
	{
		return -1;
	}

	return 0;
}

void CDlgNonPaxDataRelated::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction();
		getData().SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	CXTResizeDialog::OnOK();
}

void CDlgNonPaxDataRelated::OnInitToolbar()
{
	CRect rect;
	m_wndListCtrl.GetWindowRect( &rect );
	ScreenToClient( &rect );
	rect.top -= 26;
	rect.bottom = rect.top + 22;
	rect.left += 4;
	m_wndToolBar.MoveWindow(&rect);

	m_wndToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBAR_ADD, FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBAR_DEL, FALSE);
}

void CDlgNonPaxDataRelated::UpdateToolBarState()
{
	if (m_wndListCtrl.GetSelectedCount())
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
	}
	else
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
	}
}

void CDlgNonPaxDataRelated::InitListControl()
{
	m_wndListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	m_wndListCtrl.DeleteAllItems();
	int nColumnCount = m_wndListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nColumnCount;i++)
		m_wndListCtrl.DeleteColumn(0);

	//Group name
	LV_COLUMNEX	lvc;
	CStringList strList;
	lvc.csList = &strList;	
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("Vehicle Type");
	lvc.cx = 140;
	m_wndListCtrl.InsertColumn(0,&lvc);

	//Length
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("Time Window");
	lvc.cx = 160;
	m_wndListCtrl.InsertColumn(1,&lvc);

	//width
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Number");
	lvc.cx = 80;
	m_wndListCtrl.InsertColumn(2,&lvc);
}

void CDlgNonPaxDataRelated::SetListContent()
{
	int nCount = getData().GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		NonPaxRelatedData* pNonPaxRelateData = getData().GetItem(i);

		m_wndListCtrl.InsertItem(i,pNonPaxRelateData->GetVehicleType());
		//Time Window
		CString strTimeRange;
		TimeRange timeRange = pNonPaxRelateData->GetTimeRange();
		strTimeRange = timeRange.GetString();
		m_wndListCtrl.SetItemText(i,1,strTimeRange);

		//Number
		CString strNum;
		strNum.Format(_T("%d"),pNonPaxRelateData->GetNum());
		m_wndListCtrl.SetItemText(i,2,strNum);

		m_wndListCtrl.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_wndListCtrl.SetItemData(i,(DWORD_PTR)pNonPaxRelateData);
	}

	if (m_wndListCtrl.GetItemCount() > 0)
	{
		m_wndListCtrl.SetCurSel(0);
		m_wndListCtrl.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
}

void CDlgNonPaxDataRelated::OnCmdNewItem()
{
	CDlgSelectLandsideVehicleType dlg;
	int nCount = m_wndListCtrl.GetItemCount();
	if(dlg.DoModal() == IDOK)
	{
		CString strGroupName = dlg.GetName();
		if (strGroupName.IsEmpty())
		{
			MessageBox(_T("Please define vehicle type name in Vehicle Type Definition Dialog!."),_T("Warning"),MB_OK);
		}
		else if (getData().ExistSameName(strGroupName))
		{
			MessageBox(_T("Exist the same vehicle type!."),_T("Warning"),MB_OK);
		}
		else
		{
			NonPaxRelatedData* pNonPaxRelateData = new NonPaxRelatedData();
			pNonPaxRelateData->SetVehicleType(strGroupName);
			getData().AddNewItem(pNonPaxRelateData);
			m_wndListCtrl.InsertItem(nCount,pNonPaxRelateData->GetVehicleType());
			//Time Window
			CString strTimeRange;
			TimeRange timeRange = pNonPaxRelateData->GetTimeRange();
			strTimeRange = timeRange.GetString();
			m_wndListCtrl.SetItemText(nCount,1,strTimeRange);

			//Number
			CString strNum;
			strNum.Format(_T("%d"),pNonPaxRelateData->GetNum());
			m_wndListCtrl.SetItemText(nCount,2,strNum);

			m_wndListCtrl.SetItemState(nCount, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_wndListCtrl.SetItemData(nCount,(DWORD)pNonPaxRelateData);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
	}
}

void CDlgNonPaxDataRelated::OnCmdDeleteItem()
{
	int nCurSel = m_wndListCtrl.GetCurSel();
	if (nCurSel != LB_ERR)
	{
		NonPaxRelatedData* pNonPaxRelateData = (NonPaxRelatedData*)m_wndListCtrl.GetItemData(nCurSel);
		getData().DeleteItem(pNonPaxRelateData);
		m_wndListCtrl.DeleteItem(nCurSel);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgNonPaxDataRelated::OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	if((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
		return;
	}

	if(pnmv->uOldState & LVIS_SELECTED) 
	{
		if(!(pnmv->uNewState & LVIS_SELECTED)) 
		{
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
		}	
	}
	else if(pnmv->uNewState & LVIS_SELECTED) {
		m_wndListCtrl.SetCurSel(pnmv->iItem);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
	}
	else
		ASSERT(0);
}

void CDlgNonPaxDataRelated::OnLvnEndlabeleditListContents( NMHDR *pNMHDR, LRESULT *pResult )
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	int nCurSel = pDispInfo->item.iItem;
	if (nCurSel == LB_ERR)
		return;

	NonPaxRelatedData* pNonPaxRelatedData = (NonPaxRelatedData*)m_wndListCtrl.GetItemData(nCurSel);
	if (pNonPaxRelatedData == NULL)
		return;

	if (pDispInfo->item.iSubItem == 2)
	{
		int nNum = atoi(pDispInfo->item.pszText);
		pNonPaxRelatedData->SetNum(nNum);
	}

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	*pResult = 0;
}

void CDlgNonPaxDataRelated::OnBtnSave()
{
	try
	{
		CADODatabase::BeginTransaction();
		getData().SaveData(-1);
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

LRESULT CDlgNonPaxDataRelated::OnDbClickListCtrl( WPARAM wParam, LPARAM lParam )
{
	int nCurSel = (int)wParam;
	if (nCurSel == LB_ERR)
		return FALSE;

	NonPaxRelatedData* pNonPaxRelatedData = (NonPaxRelatedData*)m_wndListCtrl.GetItemData(nCurSel);
	if (pNonPaxRelatedData == NULL)
		return FALSE;

	int iSubItem = (int)lParam;
	if (iSubItem == 0)
	{
		CDlgSelectLandsideVehicleType dlg;
		if(dlg.DoModal() == IDOK)
		{
			CString strGroupName = dlg.GetName();
			if (!strGroupName.IsEmpty())//non empty
			{
				if (strGroupName.CompareNoCase(pNonPaxRelatedData->GetVehicleType()) != 0)//not the same name as the old name
				{
					if (getData().ExistSameName(strGroupName))
					{
						MessageBox(_T("Please define vehicle type name in Vehicle Type Definition Dialog!."),_T("Warning"),MB_OK);
						return FALSE;
					}
					
					pNonPaxRelatedData->SetVehicleType(strGroupName);
					m_wndListCtrl.SetItemText(nCurSel,0,strGroupName);
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					return TRUE;
				}
			}
		}
	}
	else if(iSubItem == 1)
	{
		TimeRange timeRange = pNonPaxRelatedData->GetTimeRange();
		CDlgTimeRange dlg(timeRange.GetStartTime(),timeRange.GetEndTime());
		if (dlg.DoModal() == IDOK)
		{
			pNonPaxRelatedData->SetTimeRange(dlg.GetStartTime(),dlg.GetEndTime());
			TimeRange timeRange = pNonPaxRelatedData->GetTimeRange();
			m_wndListCtrl.SetItemText(nCurSel,iSubItem,timeRange.GetString());
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			return TRUE;
		}
	}
	
	

	return FALSE;
}

