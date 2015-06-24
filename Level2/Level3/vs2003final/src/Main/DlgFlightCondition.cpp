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

// CDlgFlightCondition dialog

IMPLEMENT_DYNAMIC(CDlgFlightCondition, CDialog)
CDlgFlightCondition::CDlgFlightCondition(Terminal* _pTerm,FlightGroup* pGroup,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFlightCondition::IDD, pParent)
	, m_strGroupName(_T(""))
	, m_strAirline(_T(""))
	, m_strFlightID(_T(""))
	, m_strDay(_T(""))
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
	Item->SetFilter(_T("*-*-*"));
	int nbInsert = m_wndListCtrl.GetItemCount();
	
	m_wndListCtrl.InsertItem(nbInsert,_T("*"));
	m_wndListCtrl.SetItemText(nbInsert,1,_T("*"));
	m_wndListCtrl.SetItemText(nbInsert,2,_T("*"));
	
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
	char* columnLabel[] = {	"Airline", "ID","Day"};
	int DefaultColumnWidth[] = {rect.Width()/3,rect.Width()/3,rect.Width()/3};
	int nColFormat[] = 
	{	
		LVCFMT_EDIT,
		LVCFMT_EDIT,
		LVCFMT_EDIT
	};
	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.csList = &strList;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	for (int i = 0; i < 3; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		m_wndListCtrl.InsertColumn(i, &lvc);
	}
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
		 m_strAirline = strFilter.Left(nPos);
		 strFilter = strFilter.Right(strFilter.GetLength()-nPos-1);
		 nPos = strFilter.Find('-');
		 m_strFlightID = strFilter.Left(nPos);
		 m_strDay = strFilter.Right(strFilter.GetLength()-nPos-1);

		 m_wndListCtrl.InsertItem(i,m_strAirline);
		 m_wndListCtrl.SetItemText(i,1,m_strFlightID);
		 m_wndListCtrl.SetItemText(i,2,m_strDay);
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
	 m_strAirline = m_wndListCtrl.GetItemText(nItem,0);
	 m_strFlightID = m_wndListCtrl.GetItemText(nItem,1);
	 m_strDay = m_wndListCtrl.GetItemText(nItem,2);
	 if (m_strAirline.IsEmpty())
	 {
		 m_strAirline = _T("*");
		 m_wndListCtrl.SetItemText(nItem,0,m_strAirline);
	 }
	 if (m_strFlightID.IsEmpty())
	 {
		 m_strFlightID = _T("*");
		 m_wndListCtrl.SetItemText(nItem,1,m_strFlightID);
	 }
	 if(m_strDay.IsEmpty())
	 {
		 m_strDay = _T("*");
		 m_wndListCtrl.SetItemText(nItem,2,m_strDay);
	 }
	 pItem->SetFilter(m_strAirline + _T("-") + m_strFlightID + _T("-") + m_strDay);
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
