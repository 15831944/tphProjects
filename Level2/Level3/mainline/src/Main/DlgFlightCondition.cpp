// DlgFlightCondition.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "engine\terminal.h"
#include "..\Common\FlightManager.h"
#include "inputs\flight.h"
#include ".\dlgflightcondition.h"
#include "common\WINMSG.h"
#include "../Common/AirportDatabase.h"
#include "..\AirsideGUI\DlgTimePicker.h"
// CDlgFlightCondition dialog

IMPLEMENT_DYNAMIC(CDlgFlightCondition, CDialog)
CDlgFlightCondition::CDlgFlightCondition(Terminal* _pTerm,FlightGroup* pGroup,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFlightCondition::IDD, pParent)
{
	m_pCurGroup = pGroup;
	m_nCurSel = 0;
	m_pTerm = _pTerm;
}

CDlgFlightCondition::~CDlgFlightCondition()
{
}

void CDlgFlightCondition::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_GROUP_NAME, m_strGroupName);
	DDX_Control(pDX, IDC_LIST_CONDITION, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgFlightCondition, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_EN_KILLFOCUS(IDC_EDIT_GROUP_NAME, OnEnKillfocusEditGroupName)
	ON_COMMAND(ID_AIRCATFILTER_NEW,OnAddFilter)
	ON_COMMAND(ID_AIRCATFILTER_DELETE,OnRemoveFilter)
	ON_LBN_SELCHANGE(IDC_LIST_CONDITION, OnLbnSelchangeListCondition)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_CONDITION, OnLvnEndlabeleditListCondition)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_LIST_CONDITION, OnLvnItemchangingListCondition)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnMsgComboChange)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgFlightCondition message handlers

BOOL CDlgFlightCondition::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS) ||
		!m_wndToolBar.LoadToolBar(IDR_AIRCRAFTCAT_FILTER))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	InitStringList();
	CRect rect;
	GetDlgItem( IDC_STATIC_TOOLBAR )->GetWindowRect( &rect );
	ScreenToClient(rect);
	m_wndToolBar.SetWindowPos(NULL,rect.left,rect.top,rect.Width(),26,NULL);

	m_strGroupName = m_pCurGroup->getGroupName();
	
	for (size_t nFilter = 0; nFilter < m_pCurGroup->GetFilterCount();++ nFilter)
	{
		FlightGroup::CFlightGroupFilter *pFilter = new FlightGroup::CFlightGroupFilter;
		*pFilter = *(m_pCurGroup->GetFilterByIndex(nFilter));
		m_vFilter.push_back(pFilter);
	}


	UpdateData(FALSE);
    OnInitListCtrl();
 	return TRUE;
}
 

void CDlgFlightCondition::OnBnClickedOk()
{
	m_pCurGroup->Clear();
	for(size_t nFilter = 0; nFilter<m_vFilter.size();++nFilter)
	{
		m_pCurGroup->AddFilter(m_vFilter[nFilter]);
	}

	OnOK();
}

void CDlgFlightCondition::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

 void CDlgFlightCondition::OnEnKillfocusEditGroupName()
 {
 	// TODO: Add your control notification handler code here
 	UpdateData(TRUE);
 	m_pCurGroup->setGroupName(m_strGroupName);
 }
 
 void CDlgFlightCondition::OnAddFilter()
 {
	FlightGroup::CFlightGroupFilter* Item = new FlightGroup::CFlightGroupFilter;
	Item->SetFilter(_T("*-*-*-*-*-*-*"));
	int nbInsert = m_wndListCtrl.GetItemCount();
	
	m_wndListCtrl.InsertItem(nbInsert,_T("*"));
	m_wndListCtrl.SetItemText(nbInsert,1,_T("*"));
	m_wndListCtrl.SetItemText(nbInsert,2,_T("*"));
	m_wndListCtrl.SetItemText(nbInsert,3,_T("*"));
	m_wndListCtrl.SetItemText(nbInsert,4,_T("*"));
	m_wndListCtrl.SetItemText(nbInsert,5,_T("*"));
	m_wndListCtrl.SetItemText(nbInsert,6,_T("*"));
	
	m_vFilter.push_back(Item);
	m_wndListCtrl.SetFocus();
	m_wndListCtrl.SetItemData(nbInsert,(DWORD_PTR)Item);
	m_wndListCtrl.SetItemState(nbInsert,LVIS_SELECTED,LVIS_SELECTED);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_AIRCATFILTER_DELETE,TRUE);
 }

 void CDlgFlightCondition::OnRemoveFilter()
 {
	int nSelect = m_wndListCtrl.GetCurSel();
	if (nSelect != LB_ERR)
	{
		delete m_vFilter[nSelect];
		m_vFilter.erase(m_vFilter.begin() + nSelect);
		m_wndListCtrl.DeleteItem(nSelect);
	}
 }

 void CDlgFlightCondition::OnInitListCtrl()
 {
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );
	CRect rect;
	m_wndListCtrl.GetClientRect(&rect);
	char* columnLabel[] = {	"Airline", "ID","Day","Start Time(hh:mm)","End Time(hh:mm)","Minimum Turnaround Time(hh:mm)","Maximum Turnaround Time(hh:mm)"};
	int DefaultColumnWidth[] = {rect.Width()/10,rect.Width()/10,rect.Width()/10,rect.Width()*3/20,rect.Width()*3/20,rect.Width()/5,rect.Width()/5};
	int nColFormat[] = 
	{	
		LVCFMT_EDIT,
		LVCFMT_EDIT,
		LVCFMT_EDIT,
		LVCFMT_DROPDOWN,
		LVCFMT_DROPDOWN,
		LVCFMT_DROPDOWN,
		LVCFMT_DROPDOWN
	};
	LV_COLUMNEX lvc;
	lvc.csList = &m_stringList;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	for (int i = 0; i < 7; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		m_wndListCtrl.InsertColumn(i, &lvc);
	}
	m_wndListCtrl.SetHHmmTimeFormat();
	DisplayFilter();
 }

 void CDlgFlightCondition::OnLbnSelchangeListCondition()
 {
	 // TODO: Add your control notification handler code here
 }

 void CDlgFlightCondition::DisplayFilter()
 {
	 m_wndListCtrl.DeleteAllItems();
	 int nPos = -1;
	 for (int i = 0; i < (int)m_vFilter.size(); i++)
	 {
		 FlightGroup::CFlightGroupFilter* item = m_vFilter.at(i);
		 CString strFilter = item->GetFilter();
		 nPos = strFilter.Find('-');
		 CString strAirline = strFilter.Left(nPos);
		 strFilter = strFilter.Right(strFilter.GetLength()-nPos-1);

		 nPos = strFilter.Find('-');
		 CString strFlightID = strFilter.Left(nPos);
		 strFilter = strFilter.Right(strFilter.GetLength()-nPos-1);

		 nPos = strFilter.Find('-');
		 CString strDay = strFilter.Left(nPos);
		 strFilter = strFilter.Right(strFilter.GetLength()-nPos-1);

		 nPos = strFilter.Find('-');
		 CString strStartTime = strFilter.Left(nPos);
		 strFilter = strFilter.Right(strFilter.GetLength()-nPos-1);

		  nPos = strFilter.Find("-");
		 CString strEndTime = strFilter.Left(nPos);
		 strFilter = strFilter.Right(strFilter.GetLength()-nPos-1);

		 nPos = strFilter.Find("-");
		 CString strMinTurnaroundTime = strFilter.Left(nPos);
		 CString strMaxTurnaroundTime = strFilter.Right(strFilter.GetLength()-nPos-1);

		 m_wndListCtrl.InsertItem(i,strAirline);
		 m_wndListCtrl.SetItemText(i,1,strFlightID);
		 m_wndListCtrl.SetItemText(i,2,strDay);
		 //start time
		 m_wndListCtrl.SetItemText(i,3,strStartTime);
		 //end time
		 m_wndListCtrl.SetItemText(i,4,strEndTime);

		 m_wndListCtrl.SetItemText(i,5,strMinTurnaroundTime);
		 m_wndListCtrl.SetItemText(i,6,strMaxTurnaroundTime);
		 m_wndListCtrl.SetItemData(i,(DWORD_PTR)item);
	 }
	 if (m_wndListCtrl.GetItemCount()>0)
	 {
		 m_wndListCtrl.SetFocus();
		 m_wndListCtrl.SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);
		  m_wndToolBar.GetToolBarCtrl().EnableButton(ID_AIRCATFILTER_DELETE,TRUE);
	 }
 }
 void CDlgFlightCondition::OnLvnEndlabeleditListCondition(NMHDR *pNMHDR, LRESULT *pResult)
 {
	 NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	 // TODO: Add your control notification handler code here
	 *pResult = 0;
	 int nItem = m_wndListCtrl.GetCurSel();
	 FlightGroup::CFlightGroupFilter* pItem = (FlightGroup::CFlightGroupFilter*)m_wndListCtrl.GetItemData(nItem);
	 CString strAirline = m_wndListCtrl.GetItemText(nItem,0);
	 CString strFlightID = m_wndListCtrl.GetItemText(nItem,1);
	 CString strDay = m_wndListCtrl.GetItemText(nItem,2);

	 if (strAirline.IsEmpty())
	 {
		 strAirline = _T("*");
		 m_wndListCtrl.SetItemText(nItem,0,strAirline);
	 }
	 if (strFlightID.IsEmpty())
	 {
		 strFlightID = _T("*");
		 m_wndListCtrl.SetItemText(nItem,1,strFlightID);
	 }
	 if(strDay.IsEmpty())
	 {
		 strDay = _T("*");
		 m_wndListCtrl.SetItemText(nItem,2,strDay);
	 }
	 pItem->SetAirline(strAirline);
	 pItem->SetFlightID(strFlightID);
	 pItem->SetDay(strDay);
	// pItem->SetFilter(strAirline + _T("-") + strFlightID + _T("-") + strDay + _T("-") + strStartTime + _T("-") + strEndTime);
 }

 void CDlgFlightCondition::OnLvnItemchangingListCondition(NMHDR *pNMHDR, LRESULT *pResult)
 {
	 LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	 // TODO: Add your control notification handler code here
	 *pResult = 0;
	 if (m_wndListCtrl.GetCurSel() != LB_ERR)
	 {
		 m_wndToolBar.GetToolBarCtrl().EnableButton(ID_AIRCATFILTER_DELETE,TRUE);
	 }
	 else
	 {
		 m_wndToolBar.GetToolBarCtrl().EnableButton(ID_AIRCATFILTER_DELETE,FALSE);
	 }
 }

 void CDlgFlightCondition::OnSize(UINT nType, int cx, int cy)
 {
	 CDialog::OnSize(nType, cx, cy);

	 int nMargin=7;
	 int nMargin2=nMargin*2;
	 CRect rc, rc2;
	 CRect btnrc;

	 if(!GetDlgItem(IDC_STATIC_NAME))
		 return;

	 GetDlgItem(IDC_STATIC_NAME)->GetWindowRect(&rc);
	 ScreenToClient(&rc);
	 this->GetClientRect(&rc2);

	 nMargin=rc.left;

	 GetDlgItem(IDC_EDIT_GROUP_NAME)->MoveWindow(rc.right+24, rc.top-5, cx-rc.right-24-nMargin, 23);
	 GetDlgItem(IDC_EDIT_GROUP_NAME)->GetWindowRect(&rc2);
	 ScreenToClient(&rc2);

	 GetDlgItem(IDC_STATIC)->MoveWindow(rc.left, rc.bottom+16, rc2.right-rc.left, cy-82);
	 GetDlgItem(IDC_STATIC)->GetWindowRect(&rc);
	 ScreenToClient(&rc);

	 GetDlgItem(IDC_STATIC_SEP)->MoveWindow(rc.left, rc.top+23, rc.Width(), 2);
	 GetDlgItem(IDC_STATIC_TOOLBAR)->MoveWindow(rc.left+2, rc.top+26, 225, 26);
	 GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rc2);
	 ScreenToClient(&rc2);

	 GetDlgItem(IDC_LIST_CONDITION)->MoveWindow(rc.left, rc2.bottom+2, rc.Width(), cy-137);

	 GetDlgItem(IDCANCEL)->MoveWindow(cx-89,cy-30, 75, 23);
	 GetDlgItem(IDOK)->MoveWindow(cx-175, cy-30, 75, 23);
	 // TODO: Add your message handler code here
 }

 void CDlgFlightCondition::InitStringList()
 {
	m_stringList.AddTail(_T("*"));
	m_stringList.AddTail(_T("Pick Time"));
 }

 LRESULT CDlgFlightCondition::OnMsgComboChange( WPARAM wParam, LPARAM lParam )
 {
	 LV_DISPINFO* pDispInfo = (LV_DISPINFO*)lParam;
	 LV_ITEM* plvItem = &pDispInfo->item;

	 int nComboxSel = (int)wParam;
	 if (nComboxSel == LB_ERR)
		 return 0;

	 int nItem = plvItem->iItem;
	 int nSubItem = plvItem->iSubItem;
	 if (nItem < 0)
		 return 0;

	FlightGroup::CFlightGroupFilter* pItem = (FlightGroup::CFlightGroupFilter*)m_wndListCtrl.GetItemData(nItem);
	if (pItem == NULL)
		return 0;

	CString strStartTime = pItem->GetStartTime();
	CString strEndTime = pItem->GetEndTime();
	CString strMinTurnaround = pItem->GetMinTurnaround();
	CString strMaxTurnaround = pItem->GetMaxTurnaround();

	ElapsedTime eTime;
	CString strTime;
	if (nSubItem == 3)
	{
		if (strStartTime != "*")
		{
			eTime.SetTime(strStartTime);
		}
		strTime = strStartTime;
		eTime.SetTime(strTime);
	}
	else if (nSubItem == 4)
	{
		if (strEndTime != "*")
		{
			eTime.SetTime(strEndTime);
		}
		strTime = strEndTime;
		eTime.SetTime(strTime);
	}
	else if (nSubItem == 5)
	{
		if (strMinTurnaround != "*")
		{
			eTime.SetTime(strMinTurnaround);
		}
		strTime = strMinTurnaround;
		eTime.SetTime(strTime);
	}
	else if (nSubItem == 6)
	{
		if (strMaxTurnaround != "*")
		{
			eTime.SetTime(strMaxTurnaround);
		}
		strTime = strMaxTurnaround;
		eTime.SetTime(strTime);
	}
	 
	if (nComboxSel == 1)//pick time
	{
		CDlgTimePicker dlg(eTime,"Pick Time(hh:mm)",this,false);
		if(dlg.DoModal() == TRUE)
		{
			eTime = dlg.GetTime();
			if (nSubItem == 3)
			{
				pItem->SetStartTime(eTime.printTime(0));
			}
			else if (nSubItem == 4)
			{
				pItem->SetEndTime(eTime.printTime(0));
			}
			else if (nSubItem == 5)
			{
				pItem->SetMinTurnaround(eTime.printTime(0));
			}
			else if (nSubItem == 6)
			{
				pItem->SetMaxTurnaround(eTime.printTime(0));
			}
			m_wndListCtrl.SetItemText(nItem,nSubItem,eTime.printTime(0));
		}
		else
		{
			m_wndListCtrl.SetItemText(nItem,nSubItem,strTime);
		}
	}
	else
	{
		if (nSubItem == 3)
		{
			pItem->SetStartTime(_T("*"));
		}
		else if (nSubItem == 4)
		{
			pItem->SetEndTime(_T("*"));
		}
		else if (nSubItem == 5)
		{
			pItem->SetMinTurnaround(_T("*"));
		}
		else if (nSubItem == 6)
		{
			pItem->SetMaxTurnaround(_T("*"));
		}
	}

	 return 1;
 }
