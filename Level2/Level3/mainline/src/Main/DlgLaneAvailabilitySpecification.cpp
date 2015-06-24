// DlgLandsideVehicleAssignment.cpp : implementation file

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgLaneAvailabilitySpecification.h"
#include "PassengerTypeDialog.h"
#include "DlgSelectLandsideVehicleType.h"
#include "Landside/LaneAvailabilitySpecification.h"
#include "DlgSelectLandsideObject.h"
#include "Common/WinMsg.h"
#include "landside/InputLandside.h"
#include "DlgTimeRange.h"
#include "DlgLaneRange.h"

enum ColumType
{
	col_Stretch = 0 ,
	col_Lane,
	col_TimeWindow,
	col_Vehicle,
	col_VehicleType,
	col_PaxType,
};

// CDlgLaneAvailabilitySpecification dialog

IMPLEMENT_DYNAMIC(CDlgLaneAvailabilitySpecification, CXTResizeDialog)
CDlgLaneAvailabilitySpecification::CDlgLaneAvailabilitySpecification(InputLandside* pInTerm,CWnd* pParent /*=NULL*/)
: CXTResizeDialog(CDlgLaneAvailabilitySpecification::IDD, pParent)
,m_pInLandside(pInTerm)
{
	m_laneAvailabilitySpec = new CLaneAvailabilitySpecificationList();
	m_laneAvailabilitySpec->ReadData(m_pInLandside->getInputTerminal());
}

CDlgLaneAvailabilitySpecification::~CDlgLaneAvailabilitySpecification()
{
	delete m_laneAvailabilitySpec;
}

void CDlgLaneAvailabilitySpecification::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_DATA,m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgLaneAvailabilitySpecification, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBAR_ADD,OnCmdNewItem)
	ON_COMMAND(ID_TOOLBAR_DEL,OnCmdDeleteItem)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnLvnItemchangedList)
	ON_MESSAGE(WM_NOEDIT_DBCLICK,OnDbClickListCtrl)
END_MESSAGE_MAP()

// CDlgLaneAvailabilitySpecification message handlers

BOOL CDlgLaneAvailabilitySpecification::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	InitListControl();
	SetListContent();
	OnInitToolbar();
	UpdateToolBarState();

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_GROUP, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_RIGHT);
	SetResize(IDC_LIST_DATA, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	SetWindowText(_T("Lane Availability Specification"));
	return TRUE;
}

int CDlgLaneAvailabilitySpecification::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP )
		|| !m_wndToolBar.LoadToolBar(IDR_ADDDELTOOLBAR))
	{
		return -1;
	}

	return 0;
}

void CDlgLaneAvailabilitySpecification::OnInitToolbar()
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

void CDlgLaneAvailabilitySpecification::UpdateToolBarState()
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

void CDlgLaneAvailabilitySpecification::InitListControl()
{
	m_wndListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	m_wndListCtrl.DeleteAllItems();
	int nColumnCount = m_wndListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nColumnCount;i++)
		m_wndListCtrl.DeleteColumn(0);

	//stretch
	CStringList strList;
	LV_COLUMNEX	lvc;
	lvc.csList = &strList;	
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("Stretch");
	lvc.cx = 100;
	m_wndListCtrl.InsertColumn(col_Stretch,&lvc);

	//lane
	lvc.csList = &strList;	
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("Lane");
	lvc.cx = 100;
	m_wndListCtrl.InsertColumn(col_Lane,&lvc);


	//time window
	lvc.csList = &strList;	
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("Time Window");
	lvc.cx = 150;
	m_wndListCtrl.InsertColumn(col_TimeWindow,&lvc);

	//vehicle
	strList.RemoveAll();
	strList.AddTail(_T("Yes"));
	strList.AddTail(_T("No"));
	lvc.csList = &strList;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.pszText = _T("Vehicle");
	lvc.cx = 100;
	m_wndListCtrl.InsertColumn(col_Vehicle,&lvc);

	//Vehicle Type
	lvc.csList = &strList;	
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("Vehicle Type");
	lvc.cx = 150;
	m_wndListCtrl.InsertColumn(col_VehicleType,&lvc);

	//Passenger type
	lvc.csList = &strList;	
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("Pax Type");
	lvc.cx = 200;
	m_wndListCtrl.InsertColumn(col_PaxType,&lvc);

}


void CDlgLaneAvailabilitySpecification::SetListContent()
{
	m_wndListCtrl.DeleteAllItems();
	int nCount = getAssignData()->GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		CLaneAvailabilitySpecification* pLaneAvailability = getAssignData()->GetItem(i);
		InsertItemToList(pLaneAvailability);
	}

	if (m_wndListCtrl.GetItemCount() > 0)
	{
		m_wndListCtrl.SetCurSel(0);
		m_wndListCtrl.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
}
void CDlgLaneAvailabilitySpecification::InsertItemToList( CLaneAvailabilitySpecification* pLaneAvailability  )
{
	int idx = m_wndListCtrl.GetItemCount();
	int nItemIndex = m_wndListCtrl.InsertItem(idx,pLaneAvailability->GetStretch().GetIDString());
	m_wndListCtrl.SetItemData(nItemIndex, (DWORD_PTR)pLaneAvailability);
	UpdateItem(nItemIndex, pLaneAvailability);

	m_wndListCtrl.SetCurSel(nItemIndex);
	m_wndListCtrl.SetItemState(nItemIndex, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

}
void CDlgLaneAvailabilitySpecification::UpdateItem( int nItemIndex, CLaneAvailabilitySpecification* pLaneAvailability )
{
	if(pLaneAvailability == NULL)
	{
		pLaneAvailability = (CLaneAvailabilitySpecification *)m_wndListCtrl.GetItemData(nItemIndex);
	}
	if(pLaneAvailability == NULL)
	{
		ASSERT(0);
		return;
	}

	m_wndListCtrl.SetItemText(nItemIndex, col_Stretch, pLaneAvailability->GetStretch().GetIDString());

	m_wndListCtrl.SetItemText(nItemIndex, col_Lane, pLaneAvailability->GetLane().GetLaneRange());

	m_wndListCtrl.SetItemText(nItemIndex, col_TimeWindow, pLaneAvailability->GetTimeRange().ScreenPrint());

	m_wndListCtrl.SetItemText(nItemIndex,col_Vehicle,pLaneAvailability->IsVehicle()?"Yes":"No");
	m_wndListCtrl.SetItemText(nItemIndex, col_VehicleType,pLaneAvailability->GetVehicleType());
	m_wndListCtrl.SetItemText(nItemIndex, col_PaxType,pLaneAvailability->GetPaxType());

}
void CDlgLaneAvailabilitySpecification::OnCmdNewItem()
{
	CDlgSelectLandsideObject dlg(&m_pInLandside->getObjectList(),this);
	dlg.AddObjType(ALT_LSTRETCH);

	if( dlg.DoModal() == IDOK )
	{
		CLaneAvailabilitySpecification* pLaneAvailability = new CLaneAvailabilitySpecification();
		ALTObjectID selectStretch = dlg.getSelectObject();
		if (m_laneAvailabilitySpec->FindStretch(selectStretch))
		{
			CString strerror;
			strerror.Format(_T("The stretch:%s is already exit."),selectStretch.GetIDString());
			MessageBox(strerror,NULL,MB_OK);
			return;
		}
		pLaneAvailability->SetStretch(selectStretch);
		getAssignData()->AddNewItem(pLaneAvailability);
		InsertItemToList(pLaneAvailability);
		SetModified(TRUE);

	}
}

void CDlgLaneAvailabilitySpecification::OnCmdDeleteItem()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel == LB_ERR)
		return;

	CLaneAvailabilitySpecification* pLaneAvailability = (CLaneAvailabilitySpecification*)m_wndListCtrl.GetItemData(nSel);
	getAssignData()->DeleteItem(pLaneAvailability);

	m_wndListCtrl.DeleteItem(nSel);
	if (nSel > 0)
	{
		m_wndListCtrl.SetCurSel(nSel - 1);
		m_wndListCtrl.SetItemState(nSel - 1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	} 
	else
	{
		m_wndListCtrl.SetCurSel(0);
		m_wndListCtrl.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgLaneAvailabilitySpecification::OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult )
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
	else if(pnmv->uNewState & LVIS_SELECTED)
	{
		m_wndListCtrl.SetCurSel(pnmv->iItem);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
	}
	else
		ASSERT(0);

	UpdateListStyle();

}

LRESULT CDlgLaneAvailabilitySpecification::OnDbClickListCtrl( WPARAM wParam, LPARAM lParam )
{
	int nCurSel = (int)wParam;
	if (nCurSel == LB_ERR)
		return FALSE;

	CLaneAvailabilitySpecification* pLaneAvailability = (CLaneAvailabilitySpecification*)m_wndListCtrl.GetItemData(nCurSel);
	if (pLaneAvailability == NULL)
		return FALSE;

	int iSubItem = (int)lParam;
	if (iSubItem == col_Stretch)
	{
		CDlgSelectLandsideObject dlg(&m_pInLandside->getObjectList(),this);
		dlg.AddObjType(ALT_LSTRETCH);
		if( dlg.DoModal() == IDOK )
		{
			ALTObjectID selectStretch = dlg.getSelectObject();
			pLaneAvailability->SetStretch(selectStretch);
			m_wndListCtrl.SetItemText(nCurSel,0,pLaneAvailability->GetStretch().GetIDString());
			SetModified();
			return TRUE;
		}
	}
	if (iSubItem == col_Lane)
	{
		if (pLaneAvailability->GetLane().GetStartLane() == 0 || pLaneAvailability->GetLane().GetEndLane() == 0)
		{
			pLaneAvailability->SetStartLane(1);
			pLaneAvailability->SetEndLane(1);
		}
		CDlgLaneRange dlg(pLaneAvailability->GetLane().GetStartLane(),pLaneAvailability->GetLane().GetEndLane());
		if (dlg.DoModal() == IDOK)
		{
			if (dlg.IsAll())
			{
				pLaneAvailability->SetStartLane(0);
				pLaneAvailability->SetEndLane(0);
			} 
			else
			{
				pLaneAvailability->SetStartLane(dlg.GetStartLane());
				pLaneAvailability->SetEndLane(dlg.GetEndLane());
				
			}			
			m_wndListCtrl.SetItemText(nCurSel,1,pLaneAvailability->GetLane().GetLaneRange());
			SetModified();
			return TRUE;
		}
	}
	if (iSubItem == col_TimeWindow)
	{
		CDlgTimeRange dlg(pLaneAvailability->GetStartTime(), pLaneAvailability->GetEndTime());
		if(dlg.DoModal() == IDOK)
		{
			ElapsedTime eStartTime = dlg.GetStartTime();
			pLaneAvailability->SetStartTime(eStartTime);

			ElapsedTime eEndTime = dlg.GetEndTime();
			pLaneAvailability->SetEndTime(eEndTime);

			UpdateItem(nCurSel, pLaneAvailability);
			SetModified();
			return TRUE;
		}
	}
	if (iSubItem == col_VehicleType)
	{
		CDlgSelectLandsideVehicleType dlg;
		if(dlg.DoModal() == IDOK)
		{
			CString strGroupName = dlg.GetName();
			if (!strGroupName.IsEmpty())//non empty
			{
				if (strGroupName.CompareNoCase(pLaneAvailability->GetVehicleType()) != 0)//not the same name as the old name
				{
					pLaneAvailability->SetVehicleType(strGroupName);
					m_wndListCtrl.SetItemText(nCurSel,4,strGroupName);
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					return TRUE;
				}
			}
		}
	}
	if (iSubItem == col_PaxType)
	{
		CPassengerTypeDialog dlg( this ,FALSE, TRUE );
		if( dlg.DoModal() == IDOK )
		{
			if (pLaneAvailability->EqualToPaxtype(dlg.GetMobileSelection()))
				return FALSE;

			pLaneAvailability->SetPaxType(dlg.GetMobileSelection());
			m_wndListCtrl.SetItemText(nCurSel,5,pLaneAvailability->GetPaxType());
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			return TRUE;
		}
	}

	return FALSE;
}

void CDlgLaneAvailabilitySpecification::OnBtnSave()
{
	try
	{
		CADODatabase::BeginTransaction();
		getAssignData()->SaveData(-1);
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
		CADODatabase::RollBackTransation();
		MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
		return;
	}
	SetModified(FALSE);
}

void CDlgLaneAvailabilitySpecification::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction();
		getAssignData()->SaveData(-1);
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

CLaneAvailabilitySpecificationList* CDlgLaneAvailabilitySpecification::getAssignData()
{
	return m_laneAvailabilitySpec;
}

LRESULT CDlgLaneAvailabilitySpecification::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if(message == WM_INPLACE_COMBO_KILLFOUCUS)
	{
		LV_DISPINFO* pInfo = (LV_DISPINFO*) lParam;

		int iCurSubItem = pInfo->item.iSubItem;
		int iCurItem = pInfo->item.iItem;
		int iSelCombox = (int)wParam;

		CLaneAvailabilitySpecification* pItem =(CLaneAvailabilitySpecification*)m_wndListCtrl.GetItemData(iCurItem);
		if(pItem == NULL)
			return 0;
		if(iCurSubItem == col_Vehicle)
		{
			if(iSelCombox == 0)//yes
			{
				if(!pItem->IsVehicle())
				{
					SetModified();
					pItem->SetVehicle(TRUE);				
					UpdateItem(iCurItem, pItem);
					SetModified();		
				}
			}	
			else
			{
				if(pItem->IsVehicle()){
					SetModified();
					pItem->SetVehicle(FALSE);				
					UpdateItem(iCurItem, pItem);
					SetModified();		
				}
			}
			UpdateListStyle();
		}
	}
	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}
void CDlgLaneAvailabilitySpecification::SetModified( BOOL bModified /*= TRUE*/ )
{
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(bModified);
}

void CDlgLaneAvailabilitySpecification::UpdateListStyle()
{
	int nCurSel = m_wndListCtrl.GetCurSel();

	LVCOLDROPLIST* pColumn = m_wndListCtrl.GetColumnStyle(col_Stretch);
	if(pColumn)
		pColumn->Style = NO_EDIT;

	pColumn = m_wndListCtrl.GetColumnStyle(col_Lane);
	if(pColumn)
		pColumn->Style = NO_EDIT;

	pColumn = m_wndListCtrl.GetColumnStyle(col_TimeWindow);
	if(pColumn)
		pColumn->Style = NO_EDIT;

	pColumn = m_wndListCtrl.GetColumnStyle(col_Vehicle);
	if(pColumn)
		pColumn->Style = DROP_DOWN;

	pColumn = m_wndListCtrl.GetColumnStyle(col_PaxType);
	if(pColumn)
		pColumn->Style = NO_EDIT;

	pColumn = m_wndListCtrl.GetColumnStyle(col_VehicleType);
	if(pColumn)
		pColumn->Style = NO_EDIT;
}


