// DlgLandsideVehicleAssignment.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgLandsideVehicleAssignment.h"
#include "PassengerTypeDialog.h"
#include "DlgSelectLandsideVehicleType.h"
#include "Common/WinMsg.h"
#include <algorithm>
#include "../Landside/LandsideVehicleAssignment.h"
#include "landside/InputLandside.h"
#include "../Inputs/IN_TERM.H"
#include "../common/ProbDistEntry.h"
#include "../Inputs/PROBAB.H"
#include "../common/ProbDistManager.h"
#include "DlgTimeRange.h"
#include "DlgProbDist.h"

enum ColumType
{
	col_paxtype = 0 ,
	col_vehicletype,
	col_type,
	col_scheduled,
	col_daily,
	col_timewindow,
	col_freq,
	col_stop,
};

// CDlgLandsideVehicleAssignment dialog

IMPLEMENT_DYNAMIC(CDlgLandsideVehicleAssignment, CXTResizeDialog)
CDlgLandsideVehicleAssignment::CDlgLandsideVehicleAssignment(InputLandside* pInTerm,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgLandsideVehicleAssignment::IDD, pParent)
	,m_pInLandside(pInTerm)
{
	m_vehicleAssignment = new LandsideVehicleAssignContainer();
	m_vehicleAssignment->ReadData(pInTerm->getInputTerminal());
}

CDlgLandsideVehicleAssignment::~CDlgLandsideVehicleAssignment()
{
	delete m_vehicleAssignment;
}

void CDlgLandsideVehicleAssignment::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_DATA,m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgLandsideVehicleAssignment, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBAR_ADD,OnCmdNewItem)
	ON_COMMAND(ID_TOOLBAR_DEL,OnCmdDeleteItem)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnBtnSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnLvnItemchangedList)
	ON_MESSAGE(WM_NOEDIT_DBCLICK,OnDbClickListCtrl)
END_MESSAGE_MAP()

// CDlgLandsideVehicleAssignment message handlers

BOOL CDlgLandsideVehicleAssignment::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	//m_vehicleAssignment->ReadData(m_pInTerm);
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
	SetWindowText(_T("Passenger Related Vehicle Assignment"));
	return TRUE;
}

int CDlgLandsideVehicleAssignment::OnCreate( LPCREATESTRUCT lpCreateStruct )
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

void CDlgLandsideVehicleAssignment::OnInitToolbar()
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

void CDlgLandsideVehicleAssignment::UpdateToolBarState()
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

void CDlgLandsideVehicleAssignment::InitListControl()
{
	m_wndListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	m_wndListCtrl.DeleteAllItems();
	int nColumnCount = m_wndListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nColumnCount;i++)
		m_wndListCtrl.DeleteColumn(0);

	//Passenger type
	CStringList strList;
	LV_COLUMNEX	lvc;
	lvc.csList = &strList;	
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("PaxType");
	lvc.cx = 180;
	m_wndListCtrl.InsertColumn(col_paxtype,&lvc);

	//Vehicle Type
	lvc.csList = &strList;	
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("Vehicle Type");
	lvc.cx = 150;
	m_wndListCtrl.InsertColumn(col_vehicletype,&lvc);


	// Type Individual, Scheduled 
	strList.RemoveAll();
	strList.AddTail(_T("Individual"));
	strList.AddTail(_T("Public"));
	lvc.csList = &strList;	
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.pszText = _T("Type");
	lvc.cx = 100;
	m_wndListCtrl.InsertColumn(col_type,&lvc);

	//Start lane change
	strList.RemoveAll();
	strList.AddTail(_T("Yes"));
	strList.AddTail(_T("No"));
	lvc.csList = &strList;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.pszText = _T("Scheduled");
	lvc.cx = 100;
	m_wndListCtrl.InsertColumn(col_scheduled,&lvc);

	//Start lane change
	strList.RemoveAll();
	strList.AddTail(_T("Yes"));
	strList.AddTail(_T("No"));
	lvc.csList = &strList;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.pszText = _T("Daily");
	lvc.cx = 50;
	m_wndListCtrl.InsertColumn(col_daily,&lvc);

	strList.RemoveAll();
	lvc.csList = &strList;	
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.pszText = _T("Time Window");
	lvc.cx = 170;
	m_wndListCtrl.InsertColumn(col_timewindow,&lvc);

	strList.RemoveAll();
	lvc.csList = &strList;	
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_DATETIME;
	lvc.pszText = _T("Frequency(hh:mm)");
	lvc.cx = 100;
	m_wndListCtrl.InsertColumn(col_freq,&lvc);


// 	strList.RemoveAll();
// 
// 	GetProbDropdownList(strList);
// 	lvc.csList = &strList;	
// 	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
// 	lvc.fmt = LVCFMT_DROPDOWN;
// 	lvc.pszText = _T("Entry Offset(mins)");
// 	lvc.cx = 160;
// 	m_wndListCtrl.InsertColumn(col_entryoffset,&lvc);


	strList.RemoveAll();
	GetProbDropdownList(strList);
	lvc.csList = &strList;	
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.pszText = _T("Stop(minutes)");
	lvc.cx = 160;
	m_wndListCtrl.InsertColumn(col_stop,&lvc);

	m_wndListCtrl.SetHHmmTimeFormat();


}
void CDlgLandsideVehicleAssignment::GetProbDropdownList( CStringList& strList )
{
	CString s;
	s.LoadString(IDS_STRING_NEWDIST);
	strList.InsertAfter( strList.GetTailPosition(), s );

	int nCount = _g_GetActiveProbMan(  m_pInLandside->getInputTerminal() )->getCount();
	for( int m=0; m<nCount; m++ )
	{
		CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInLandside->getInputTerminal() )->getItem( m );			
		strList.InsertAfter( strList.GetTailPosition(), pPBEntry->m_csName );
	}
}

void CDlgLandsideVehicleAssignment::SetListContent()
{
	m_wndListCtrl.DeleteAllItems();
	int nCount = getAssignData().GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		LandsideVehicleAssignEntry* pVehicleEntry = getAssignData().GetItem(i);
		InsertItemToList(pVehicleEntry);
	}

	if (m_wndListCtrl.GetItemCount() > 0)
	{
		m_wndListCtrl.SetCurSel(0);
		m_wndListCtrl.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
}
void CDlgLandsideVehicleAssignment::InsertItemToList( LandsideVehicleAssignEntry* pVehicleEntry  )
{
	int idx = m_wndListCtrl.GetItemCount();
	int nItemIndex = m_wndListCtrl.InsertItem(idx,pVehicleEntry->GetPaxType());
	m_wndListCtrl.SetItemData(nItemIndex, (DWORD_PTR)pVehicleEntry);
	UpdateItem(nItemIndex, pVehicleEntry);

}
void CDlgLandsideVehicleAssignment::UpdateItem( int nItemIndex, LandsideVehicleAssignEntry* pVehicleEntry )
{
	if(pVehicleEntry == NULL)
	{
		pVehicleEntry = (LandsideVehicleAssignEntry *)m_wndListCtrl.GetItemData(nItemIndex);
	}
	if(pVehicleEntry == NULL)
	{
		ASSERT(0);
		return;
	}


	//vehicle type
	m_wndListCtrl.SetItemText(nItemIndex, col_paxtype, pVehicleEntry->GetPaxType());

	m_wndListCtrl.SetItemText(nItemIndex, col_vehicletype, pVehicleEntry->GetVehicleName());

	//Type
	m_wndListCtrl.SetItemText(nItemIndex, col_type, pVehicleEntry->getTypeName());

	//Schedule
	m_wndListCtrl.SetItemText(nItemIndex,col_scheduled, "");
	m_wndListCtrl.SetItemText(nItemIndex, col_daily, "");
	m_wndListCtrl.SetItemText(nItemIndex, col_timewindow, "");
	m_wndListCtrl.SetItemText(nItemIndex, col_freq, _T(""));
//	m_wndListCtrl.SetItemText(nItemIndex, col_entryoffset, _T(""));
	m_wndListCtrl.SetItemText(nItemIndex, col_stop, _T(""));

	if(pVehicleEntry->getType() == LandsideVehicleAssignEntry::AssignType_Public  )
	{
		m_wndListCtrl.SetItemText(nItemIndex,col_scheduled, pVehicleEntry->isScheduled()?"Yes":"No");

		if(pVehicleEntry->isScheduled())
		{

			//daily
			CString strDaily = _T("No");
			if(pVehicleEntry->isDaily())
				strDaily = _T("Yes");
			m_wndListCtrl.SetItemText(nItemIndex, col_daily, strDaily);

			//start day
			CString strTimeWindow;
			if(pVehicleEntry->isDaily())
			{
				strTimeWindow.Format(_T("%s - %s"), pVehicleEntry->getStartTime().printTime(), pVehicleEntry->getEndTime().printTime());
			}
			else
			{
				strTimeWindow.Format(_T("%s - %s"), pVehicleEntry->getStartTime().PrintDateTime(), pVehicleEntry->getEndTime().PrintDateTime());
			}
			m_wndListCtrl.SetItemText(nItemIndex, col_timewindow, strTimeWindow);


			m_wndListCtrl.SetItemText(nItemIndex, col_freq, pVehicleEntry->getFrequency().printTime(0));

			//entry offset
			//m_wndListCtrl.SetItemText(nItemIndex, col_entryoffset, pVehicleEntry->getDstOffset()->getPrintDist());

			//stop hours
			m_wndListCtrl.SetItemText(nItemIndex, col_stop, pVehicleEntry->getDstStop()->getDistName());
		}	
	}
	
}
void CDlgLandsideVehicleAssignment::OnCmdNewItem()
{
	CPassengerTypeDialog dlg( this ,FALSE, TRUE );

	if( dlg.DoModal() == IDOK )
	{
		LandsideVehicleAssignEntry* pVehicleEntry = new LandsideVehicleAssignEntry();
		pVehicleEntry->SetPaxType(dlg.GetMobileSelection());
		getAssignData().AddNewItem(pVehicleEntry);

		InsertItemToList(pVehicleEntry);
		SetModified(TRUE);
	}
}

void CDlgLandsideVehicleAssignment::OnCmdDeleteItem()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel == LB_ERR)
		return;

	LandsideVehicleAssignEntry* pVehicleEntry = (LandsideVehicleAssignEntry*)m_wndListCtrl.GetItemData(nSel);
	getAssignData().DeleteItem(pVehicleEntry);

	m_wndListCtrl.DeleteItem(nSel);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgLandsideVehicleAssignment::OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult )
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

LRESULT CDlgLandsideVehicleAssignment::OnDbClickListCtrl( WPARAM wParam, LPARAM lParam )
{
	int nCurSel = (int)wParam;
	if (nCurSel == LB_ERR)
		return FALSE;

	LandsideVehicleAssignEntry* pVehicleEntry = (LandsideVehicleAssignEntry*)m_wndListCtrl.GetItemData(nCurSel);
	if (pVehicleEntry == NULL)
		return FALSE;

	int iSubItem = (int)lParam;
	if (iSubItem == col_paxtype)
	{
		CPassengerTypeDialog dlg( this ,FALSE, TRUE );
		if( dlg.DoModal() == IDOK )
		{
			if (pVehicleEntry->EqualToPaxtype(dlg.GetMobileSelection()))
				return FALSE;

			pVehicleEntry->SetPaxType(dlg.GetMobileSelection());
			m_wndListCtrl.SetItemText(nCurSel,0,pVehicleEntry->GetPaxType());
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
			return TRUE;
		}
	}
	else if (iSubItem == col_vehicletype)
	{
		CDlgSelectLandsideVehicleType dlg;
		if(dlg.DoModal() == IDOK)
		{
			CString strGroupName = dlg.GetName();
			if (!strGroupName.IsEmpty())//non empty
			{
				if (strGroupName.CompareNoCase(pVehicleEntry->GetVehicleName()) != 0)//not the same name as the old name
				{
					pVehicleEntry->SetVehicleName(strGroupName);
					m_wndListCtrl.SetItemText(nCurSel,1,strGroupName);
					GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
					return TRUE;
				}
			}
		}
	}
	if(iSubItem == col_timewindow && pVehicleEntry->getType() == LandsideVehicleAssignEntry::AssignType_Public && pVehicleEntry->isScheduled() )//time window
	{
		CDlgTimeRange dlg(pVehicleEntry->getStartTime(), pVehicleEntry->getEndTime(), pVehicleEntry->isDaily(), this);
		if(dlg.DoModal() == IDOK)
		{
			ElapsedTime eStartTime = dlg.GetStartTime();
			pVehicleEntry->setStartTime(eStartTime);

			ElapsedTime eEndTime = dlg.GetEndTime();
			pVehicleEntry->setEndTime(eEndTime);

			UpdateItem(nCurSel, pVehicleEntry);
			SetModified();
		}
	}	

	return FALSE;
}

void CDlgLandsideVehicleAssignment::OnBtnSave()
{
	if (!getAssignData().ValidData())
	{
		MessageBox(_T("Please define vehicle type for every item"), _T("Warning"),MB_OK);
		return;
	}

	try
	{
		CADODatabase::BeginTransaction();
		getAssignData().SaveData(-1);
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

void CDlgLandsideVehicleAssignment::OnOK()
{
	if (!getAssignData().ValidData())
	{
		MessageBox(_T("Please define vehicle type for every item"), _T("Warning"),MB_OK);
	}
	else
	{
		try
		{
			CADODatabase::BeginTransaction();
			getAssignData().SaveData(-1);
			CADODatabase::CommitTransaction();
		}
		catch (CADOException& e)
		{
			e.ErrorMessage();
			CADODatabase::RollBackTransation();
			MessageBox(_T("Save database error"),_T("ERROR"),MB_OK);
			return;
		}
		CXTResizeDialog::OnOK();;
	}
}

LandsideVehicleAssignContainer& CDlgLandsideVehicleAssignment::getAssignData()
{
	return *m_vehicleAssignment;
}

LRESULT CDlgLandsideVehicleAssignment::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if(message == WM_INPLACE_COMBO_KILLFOUCUS)
	{
		LV_DISPINFO* pInfo = (LV_DISPINFO*) lParam;

		int iCurSubItem = pInfo->item.iSubItem;
		int iCurItem = pInfo->item.iItem;
		int iSelCombox = (int)wParam;

		LandsideVehicleAssignEntry *pItem =(LandsideVehicleAssignEntry *)m_wndListCtrl.GetItemData(iCurItem);
		if(pItem == NULL)
			return 0;
		if(iCurSubItem == col_type)
		{
			if(iSelCombox == LB_ERR)
			{

			}
			else if(iSelCombox == 0)//Individual 
			{
				if(pItem->getType() != LandsideVehicleAssignEntry::AssignType_Individual)// Individual -> Scheduled
				{
					pItem->setType(LandsideVehicleAssignEntry::AssignType_Individual);

					UpdateItem(iCurItem, pItem);
					SetModified();
				}
			}
			else //public
			{
				if(pItem->getType() != LandsideVehicleAssignEntry::AssignType_Public)//
				{
					pItem->setType(LandsideVehicleAssignEntry::AssignType_Public);
					UpdateItem(iCurItem, pItem);
					SetModified();
				}
			}

			UpdateListStyle();
		}
		else if(iCurSubItem == col_scheduled)//daily
		{
			if(iSelCombox == 0)//yes
			{
				if(!pItem->isScheduled())
				{
					SetModified();
					pItem->SetScheduled(TRUE);				
					UpdateItem(iCurItem, pItem);
					SetModified();		
				}
			}	
			else
			{
				if(pItem->isScheduled()){
					SetModified();
					pItem->SetScheduled(FALSE);				
					UpdateItem(iCurItem, pItem);
					SetModified();		
				}
			}
			UpdateListStyle();
		}
		else if(iCurSubItem == col_daily)//daily
		{
			if(iSelCombox == LB_ERR)
			{

			}
			else if(iSelCombox == 0)//yes
			{
				if(!pItem->isDaily())// NO->YES
				{
					pItem->setDaily(TRUE);
					pItem->setStartTime(ElapsedTime(0L));
					pItem->setEndTime(ElapsedTime(WholeDay -1));
					CDlgTimeRange dlg(ElapsedTime(0L), ElapsedTime(WholeDay -1), pItem->isDaily(), this);
					if(dlg.DoModal() == IDOK)
					{
						ElapsedTime eStartTime = dlg.GetStartTime();
						pItem->setStartTime(eStartTime);

						ElapsedTime eEndTime = dlg.GetEndTime();
						pItem->setEndTime(eEndTime);

					}
					UpdateItem(iCurItem, pItem);
					SetModified();

				}
			}
			else 
			{
				if(pItem->isDaily())
				{
					pItem->setDaily(FALSE);

					CDlgTimeRange dlg(pItem->getStartTime(), pItem->getEndTime(), pItem->isDaily(), this);
					if(dlg.DoModal() == IDOK)
					{
						ElapsedTime eStartTime = dlg.GetStartTime();
						pItem->setStartTime(eStartTime);

						ElapsedTime eEndTime = dlg.GetEndTime();
						pItem->setEndTime(eEndTime);
					}
					UpdateItem(iCurItem, pItem);
					SetModified();

				}
			}
		}
		else if( /*iCurSubItem == col_entryoffset ||*/ iCurSubItem == col_stop )
		{

			if(iSelCombox == 0)
			{
				CDlgProbDist dlg(  m_pInLandside->getInputTerminal()->m_pAirportDB, true );
				if(dlg.DoModal()==IDOK) 
				{
					int idxPD = dlg.GetSelectedPDIdx();
					ASSERT(idxPD!=-1);
					CProbDistEntry* pde = _g_GetActiveProbMan( m_pInLandside->getInputTerminal() )->getItem(idxPD);

// 					if(iCurSubItem == col_entryoffset)
// 						pItem->setDstOffset(pde);
// 					else
					pItem->setDstStop(pde);
					UpdateItem(iCurItem, pItem);
					SetModified();

					//update distribution list
					CStringList strList;
					GetProbDropdownList(strList);
// 					LVCOLDROPLIST* pColumnStyle = m_wndListCtrl.GetColumnStyle(col_entryoffset);
// 					pColumnStyle->List.RemoveAll();
// 					pColumnStyle->List.AddTail(&strList);

					LVCOLDROPLIST* pColumnStyle = m_wndListCtrl.GetColumnStyle(6);
					pColumnStyle->List.RemoveAll();
					pColumnStyle->List.AddTail(&strList);


				}
			}
			else
			{
				ASSERT( iSelCombox-1 >= 0 && iSelCombox-1 < static_cast<int>(_g_GetActiveProbMan( m_pInLandside->getInputTerminal()  )->getCount()) );
				CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pInLandside->getInputTerminal()  )->getItem( iSelCombox-1 );
// 				if(iCurSubItem == col_entryoffset)
// 					pItem->setDstOffset(pPBEntry);
// 				else
				pItem->setDstStop(pPBEntry);
				UpdateItem(iCurItem, pItem);	
				SetModified();

			}
		}

	}
	else if (message == WM_INPLACE_SPIN)
	{
		//LPSPINTEXT pst = (LPSPINTEXT) lParam;
		////m_wndListCtrl.getse
		//if(pst->nColumn == 3)
		//{
		//	LandsideVehicleAssignEntry *pItem =(LandsideVehicleAssignEntry *)m_wndListCtrl.GetItemData(pst->nItem);
		//	if(pItem == NULL)
		//		return 0;
		//	pItem->setNumber(pst->iPercent);
		//	SetModified();
		//}

	}
	if(message == WM_INPLACE_DATETIME)
	{
		switch( wParam ) 
		{
		case 1:
			{
				LV_DISPINFO* pDispinfo=(LV_DISPINFO*)lParam;
				m_nEditItemForOleTime = pDispinfo->item.iItem;
				//int iColumnIdx = pDispinfo->item.iSubItem;
			}
			break;
		case 2:
			{
				COleDateTime *pTime = (COleDateTime *)lParam;
				ElapsedTime newTime(pTime->GetHour()*3600l + pTime->GetMinute()*60l + pTime->GetSecond()*1l);

				LandsideVehicleAssignEntry *pItem =(LandsideVehicleAssignEntry *)m_wndListCtrl.GetItemData(m_nEditItemForOleTime);
				if(pItem != NULL)
				{
					pItem->setFrequency(newTime);
					UpdateItem(m_nEditItemForOleTime, pItem);
					SetModified();
				}
				
			}
			break;	
		}
	}

	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}
void CDlgLandsideVehicleAssignment::SetModified( BOOL bModified /*= TRUE*/ )
{
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(bModified);
}

void CDlgLandsideVehicleAssignment::UpdateListStyle()
{
	int nCurSel = m_wndListCtrl.GetCurSel();

	//init to no edit
	LVCOLDROPLIST* pColumn = m_wndListCtrl.GetColumnStyle(col_scheduled);//scheduled
	if(pColumn)
		pColumn->Style = NO_EDIT;

	pColumn = m_wndListCtrl.GetColumnStyle(col_daily);//daily
	if(pColumn)
		pColumn->Style = NO_EDIT;

	pColumn = m_wndListCtrl.GetColumnStyle(col_timewindow);//Time window
	if(pColumn)
		pColumn->Style = NO_EDIT;

	pColumn = m_wndListCtrl.GetColumnStyle(col_freq);//Frequency
	if(pColumn)
		pColumn->Style = NO_EDIT;

// 	pColumn = m_wndListCtrl.GetColumnStyle(col_entryoffset);//Distribution Offset
// 	if(pColumn)
// 		pColumn->Style = NO_EDIT;

	pColumn = m_wndListCtrl.GetColumnStyle(col_stop);//Distribution Stop
	if(pColumn)
		pColumn->Style = NO_EDIT;

	LandsideVehicleAssignEntry* pVehicleEntry = (LandsideVehicleAssignEntry*)m_wndListCtrl.GetItemData(nCurSel);
	if (pVehicleEntry)
	{
		if(pVehicleEntry->getType() == LandsideVehicleAssignEntry::AssignType_Public )
		{
			LVCOLDROPLIST* pColumn = m_wndListCtrl.GetColumnStyle(col_scheduled);//scheduled
			if(pColumn)
				pColumn->Style = DROP_DOWN;

			if(pVehicleEntry->isScheduled())
			{
				pColumn = m_wndListCtrl.GetColumnStyle(col_daily);//daily
				if(pColumn)
					pColumn->Style = DROP_DOWN;

				pColumn = m_wndListCtrl.GetColumnStyle(col_timewindow);//Time window
				if(pColumn)
					pColumn->Style = NO_EDIT;

				pColumn = m_wndListCtrl.GetColumnStyle(col_freq);//Frequency
				if(pColumn)
					pColumn->Style = DATE_TIME;

// 				pColumn = m_wndListCtrl.GetColumnStyle(col_entryoffset);//Distribution Offset
// 				if(pColumn)
// 					pColumn->Style = DROP_DOWN;

				pColumn = m_wndListCtrl.GetColumnStyle(col_stop);//Distribution Stop
				if(pColumn)
					pColumn->Style = DROP_DOWN;

				
			}			
			
		}
	}


		
}


