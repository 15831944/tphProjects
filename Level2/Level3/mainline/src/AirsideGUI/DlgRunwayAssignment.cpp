// DlgTakeoffRunwayAssign.cpp : implementation file
//
#include "stdafx.h"
#include "resource.h"
#include "DlgRunwayAssignment.h"


#include "InputAirside/RunwayAssignmentStrategies.h"

#include "InputAirside/TakeoffRunwayAssignmentStrategies.h"
#include "InputAirside/LandingRunwayAssignmentStrategies.h"

#include "InputAirside/FlightTypeRunwayAssignmentStrategyItem.h"
#include "InputAirside/TimeRangeRunwayAssignmentStrategyItem.h"

#include "InputAirside/InputAirside.h"
#include "InputAirside/ALTAirport.h"
#include "InputAirside/Runway.h"

#include "../Database/ARCportDatabaseConnection.h"

#include "DlgTimeRange.h"
#include "DlgSelectRunway.h"
#include ".\dlgrunwayassignment.h"



CDlgRunwayAssignment::CDlgRunwayAssignment(int nProjID, PSelectFlightType pSelectFlightType, LPCTSTR lpszTitle, CWnd* pParent /*=NULL*/)
 : CXTResizeDialog(CDlgRunwayAssignment::IDD, pParent)
 , m_pSelectFlightType(pSelectFlightType)
 , m_strTitle(lpszTitle)
 ,m_nProjID(nProjID)
 ,m_hTimeRangeTreeItem(NULL)
{
}

CDlgRunwayAssignment::~CDlgRunwayAssignment()
{
	delete m_pStrategies;
	m_pStrategies = NULL;
}

void CDlgRunwayAssignment::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ASSIGNSTRATEGY, m_wndCmbAssignStrategy);
	DDX_Control(pDX, IDC_TREE_FLTTIME, m_wndTreeFltTime);
	DDX_Control(pDX, IDC_LIST_TAKEOFFRUNWAYASSIGN, m_wndListTakeoffRunwayAssign);
}

BEGIN_MESSAGE_MAP(CDlgRunwayAssignment, CXTResizeDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnMsgSave)
	ON_COMMAND(ID_NEW_FLTTYPE, OnMsgNewFltType)
	ON_COMMAND(ID_DEL_FLTTYPE, OnMsgDelFltType)
	ON_COMMAND(ID_EDIT_FLTTYPE, OnMsgEditFltType)
	ON_COMMAND(ID_NEW_RUNWAY, OnMsgNewRunway)
	ON_COMMAND(ID_DEL_RUNWAY, OnMsgDelRunway)
	ON_COMMAND(ID_EDIT_RUNWAY, OnMsgEditRunway)
	ON_CBN_SELCHANGE(IDC_COMBO_ASSIGNSTRATEGY, OnMsgAssignmentStrategyChange)
	ON_NOTIFY(NM_CLICK, IDC_TREE_FLTTIME, OnNMClickFltTimeTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FLTTIME, OnTvnSelchangedFltTimeTree)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_TAKEOFFRUNWAYASSIGN, OnLvnEndlabeleditListContents)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TAKEOFFRUNWAYASSIGN, OnLvnItemChangedTakeoffList)
	ON_COMMAND(ID_ADD_TIMERANGE, OnMsgAddTimeRange)
	ON_COMMAND(ID_DEL_TIMERANGE, OnMsgDelTimeRange)
	ON_COMMAND(ID_EDIT_TIMERANGE, OnMsgEditTimeRange)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

int CDlgRunwayAssignment::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndFltToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0),1000)
		|| !m_wndFltToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	if (!m_wndRunwayToolbar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0),1001)
		|| !m_wndRunwayToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT_RUNWAYASSIGN))
	{
		return -1;
	}

	CToolBarCtrl& fltToolbar = m_wndFltToolbar.GetToolBarCtrl();
	fltToolbar.SetCmdID(0, ID_NEW_FLTTYPE);
	fltToolbar.SetCmdID(1, ID_DEL_FLTTYPE);
	fltToolbar.SetCmdID(2, ID_EDIT_FLTTYPE);

	CToolBarCtrl& runwayToolbar = m_wndRunwayToolbar.GetToolBarCtrl();
	runwayToolbar.SetCmdID(0, ID_NEW_RUNWAY);
	runwayToolbar.SetCmdID(1, ID_DEL_RUNWAY);
	runwayToolbar.SetCmdID(2, ID_EDIT_RUNWAY);

	return 0;
}

BOOL CDlgRunwayAssignment::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	GetDlgItem(IDC_TREE_PRIORITY)->ShowWindow(SW_HIDE) ;
	GetDlgItem(IDC_TREE_PRIORITY)->EnableWindow(FALSE) ;
	SetWindowText(m_strTitle);

	m_wndCmbAssignStrategy.AddString(_T("Flight Planned Runway"));
	m_wndCmbAssignStrategy.AddString(_T("Closest Runway"));
	m_wndCmbAssignStrategy.AddString(_T("Managed assignment"));
	m_wndCmbAssignStrategy.SetCurSel(0);

	// set the position of the flt toolbar
	CRect rectFltToolbar;
	m_wndTreeFltTime.GetWindowRect(&rectFltToolbar);
	ScreenToClient(&rectFltToolbar);
	rectFltToolbar.top -= 26;
	rectFltToolbar.bottom = rectFltToolbar.top + 22;
	rectFltToolbar.left +=2;
	m_wndFltToolbar.MoveWindow(&rectFltToolbar);

	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_NEW_FLTTYPE);
	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_DEL_FLTTYPE);
	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_FLTTYPE);

	// set the position of the runway toolbar
	CRect rectRunwayToolbar;
	m_wndListTakeoffRunwayAssign.GetWindowRect(&rectRunwayToolbar);
	ScreenToClient(&rectRunwayToolbar);
	rectRunwayToolbar.top -= 26;
	rectRunwayToolbar.bottom = rectRunwayToolbar.top + 22;
	rectRunwayToolbar.left += 4;
	m_wndRunwayToolbar.MoveWindow(&rectRunwayToolbar);

	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAY);
	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY);
	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAY);

	// set list control window style
	DWORD dwStyle = m_wndListTakeoffRunwayAssign.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListTakeoffRunwayAssign.SetExtendedStyle(dwStyle);

	// set the runway assign list header contents
	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("Runway");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_wndListTakeoffRunwayAssign.InsertColumn(0, &lvc);

	lvc.pszText = _T("Percent");
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListTakeoffRunwayAssign.InsertColumn(1, &lvc);


	//set resize
	SetResize(IDC_STATIC_FLIGHTTYPE, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(m_wndFltToolbar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_CENTER);
	SetResize(IDC_TREE_FLTTIME, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	
	SetResize(IDC_STATIC_STRATEGY_TITLE, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(IDC_COMBO_ASSIGNSTRATEGY, SZ_TOP_CENTER, SZ_TOP_RIGHT);
	SetResize(IDC_STATIC_RUNWAYLST, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(m_wndRunwayToolbar.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_TOP_RIGHT);
	SetResize(IDC_LIST_TAKEOFFRUNWAYASSIGN, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);

	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	ReadData();
	InitTreeCtrl();
	UpdateState();
	return TRUE;
}

void CDlgRunwayAssignment::InitTreeCtrl()
{
	for (int i=0; i<m_pStrategies->GetStrategyCount(); i++)
	{
		FlightTypeRunwayAssignmentStrategyItem* pStrategyItem = m_pStrategies->GetStrategyItem(i);
		m_wndTreeFltTime.SetRedraw(FALSE);

		CString strFilghtType;
		pStrategyItem->GetFlightType().screenPrint(strFilghtType.GetBuffer(1024));

		HTREEITEM hFltTypeTreeItem = m_wndTreeFltTime.InsertItem(strFilghtType);
		m_wndTreeFltTime.SetItemData(hFltTypeTreeItem, (DWORD_PTR)pStrategyItem);
		for (int j=0; j<pStrategyItem->GetTimeRangeStrategyItemCount(); j++)
		{
			TimeRangeRunwayAssignmentStrategyItem* pTimeRangeItem
				= pStrategyItem->GetTimeRangeStrategyItem(j);
			CString strTimeRange = _T(""),strDay = _T("");
			TimeRange trInsert = pTimeRangeItem->GetTimeRange();
			ElapsedTime etInsert = trInsert.GetStartTime();
			long lSecond = etInsert.asSeconds();
			strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400 + 1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
			strTimeRange = strDay + _T(" - ");
			etInsert = trInsert.GetEndTime();
			lSecond = etInsert.asSeconds();
			strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400 + 1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
			strTimeRange += strDay;
			HTREEITEM hTimeRangeTreeItem = m_wndTreeFltTime.InsertItem(strTimeRange, hFltTypeTreeItem, TVI_LAST);
			m_wndTreeFltTime.SetItemData(hTimeRangeTreeItem, (DWORD_PTR)pTimeRangeItem);
		}
		m_wndTreeFltTime.Expand(hFltTypeTreeItem, TVE_EXPAND);
		m_wndTreeFltTime.SetRedraw(TRUE);
	}
}

void CDlgRunwayAssignment::SetListCtrlContents()
{
	m_wndListTakeoffRunwayAssign.DeleteAllItems();

	HTREEITEM hItem;
	if(!GetTreeItemByType(TREENODE_TIMERANGE, hItem))
		return;


	TimeRangeRunwayAssignmentStrategyItem* pTimeRangeItem = 
		(TimeRangeRunwayAssignmentStrategyItem*)m_wndTreeFltTime.GetItemData(hItem);
	ASSERT(pTimeRangeItem != NULL);

	TimeRangeRunwayAssignmentStrategyItem::RunwayAssignPercentVector rwVect
		= pTimeRangeItem->GetManagedAssignmentRWAssignPercent();

	for(int i=0; i<(int)rwVect.size(); i++)
	{
		RunwayAssignmentPercentItem* pItem = rwVect[i];

		Runway altRunway;
		altRunway.ReadObject(pItem->m_nRunwayID);
		ALTAirport altAirport;
		altAirport.ReadAirport(altRunway.getAptID());

		CString strMarking = (RUNWAYMARK_FIRST == pItem->m_nRunwayMarkIndex ? altRunway.GetMarking1().c_str() : altRunway.GetMarking2().c_str());
		CString strRunwayInfo;
		strRunwayInfo.Format("%s:Runway:%s", altAirport.getName(), strMarking);


		int nCount = m_wndListTakeoffRunwayAssign.GetItemCount();
		m_wndListTakeoffRunwayAssign.InsertItem(nCount, strRunwayInfo);
		CString strValue;
		strValue.Format("%.0f",pItem->m_dPercent);
		m_wndListTakeoffRunwayAssign.SetItemText(nCount, 1, strValue);

		m_wndListTakeoffRunwayAssign.SetItemData(nCount, (DWORD_PTR)pItem);
	}

}



void CDlgRunwayAssignment::OnMsgNewFltType()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	OnNewFltType(fltType);
}

void CDlgRunwayAssignment::OnMsgDelFltType()
{
	OnDelFltType();
}

void CDlgRunwayAssignment::OnMsgEditFltType()
{
	OnEditFltType();
}

void CDlgRunwayAssignment::OnMsgNewRunway()
{
	OnNewRunway();
}

void CDlgRunwayAssignment::OnMsgDelRunway()
{
	OnDelRunway();
}

void CDlgRunwayAssignment::OnMsgEditRunway()
{
	OnEditRunway();
}

void CDlgRunwayAssignment::OnMsgAssignmentStrategyChange()
{
	OnAssignmentStrategyChange();
}

void CDlgRunwayAssignment::OnMsgSave()
{
	OnSave();
}

void CDlgRunwayAssignment::OnNMClickFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult)
{

	OnClickFltTimeTree(pNMHDR, pResult);
	*pResult = 0;
}

void CDlgRunwayAssignment::OnTvnSelchangedFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	OnSelChangedFltTimeTree(pNMHDR, pResult);
	*pResult = 0;
}

void CDlgRunwayAssignment::OnContextMenu(CWnd* pWnd, CPoint point)
{
	ContextMenu(pWnd, point);
}

void CDlgRunwayAssignment::OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iTotals = 0 , nCount = 0;
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;
	int nItem = plvItem->iItem;
	int nSubItem = plvItem->iSubItem;
	if (nItem < 0)
		return;

	nCount = m_wndListTakeoffRunwayAssign.GetItemCount();
	for (int i = 0; i < nCount ; i++)
	{
		if(plvItem->iItem != i)
			iTotals += _ttol(m_wndListTakeoffRunwayAssign.GetItemText(i,1) );
	}
	if (iTotals + _ttol(m_wndListTakeoffRunwayAssign.GetItemText(nItem,nSubItem) ) > 100)
	{
		CString strReplace = _T("0");
		strReplace.Format(_T("%d"),100 - iTotals);
	    m_wndListTakeoffRunwayAssign.SetItemText(nItem,nSubItem,strReplace);
		MessageBox(_T("The sum of operation percent can not exceed 100%."));
		return;
	}

	m_wndListTakeoffRunwayAssign.OnEndlabeledit(pNMHDR, pResult);	
	
	OnListItemEndLabelEdit(nItem, nSubItem);
	*pResult = 0;
}

void CDlgRunwayAssignment::OnLvnItemChangedTakeoffList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;

	int nItem = plvItem->iItem;
	int nSubItem = plvItem->iSubItem;
	if (nItem < 0)
		return;

	OnListItemChanged(nItem, nSubItem);
	*pResult = 0;
}

void CDlgRunwayAssignment::OnMsgAddTimeRange()
{
	OnAddTimeRange();
}


void CDlgRunwayAssignment::OnMsgDelTimeRange()
{
	OnDelTimeRange();
}

void CDlgRunwayAssignment::OnMsgEditTimeRange()
{
	OnEditTimeRange();
}






void CDlgRunwayAssignment::OnNewFltType(FlightConstraint & flightType)
{

	char szBuffer[1024] = {0};
	flightType.screenPrint(szBuffer);

	CString strBuffer = szBuffer;
	HTREEITEM hItem = m_wndTreeFltTime.GetRootItem();
	while (hItem)
	{
		if (m_wndTreeFltTime.GetItemText(hItem) == strBuffer)
		{
			MessageBox(_T("Flight Type: \" ") +strBuffer+ _T(" \" already exists."));
			return;
		}
		hItem = m_wndTreeFltTime.GetNextItem(hItem , TVGN_NEXT);
	}

	FlightTypeRunwayAssignmentStrategyItem * pNewItem = new FlightTypeRunwayAssignmentStrategyItem(m_nProjID);
	flightType.WriteSyntaxStringWithVersion(szBuffer);
	pNewItem->setFlightType(szBuffer);
	m_pStrategies->AddStrategyItem(pNewItem);

	flightType.screenPrint(szBuffer);
	m_wndTreeFltTime.SetRedraw(FALSE);
	hItem = m_wndTreeFltTime.InsertItem(szBuffer);
	m_wndTreeFltTime.SetItemData(hItem, (DWORD_PTR)pNewItem);
	m_wndTreeFltTime.SetRedraw(TRUE);

}

void CDlgRunwayAssignment::OnDelFltType()
{
	HTREEITEM hItem;
	if(!GetTreeItemByType(TREENODE_FLTTYPE, hItem))
		return;

	FlightTypeRunwayAssignmentStrategyItem* pItem = (FlightTypeRunwayAssignmentStrategyItem*)m_wndTreeFltTime.GetItemData(hItem);

	m_pStrategies->DeleteStrategyItem(pItem);

	m_wndTreeFltTime.SetItemData(hItem, (DWORD_PTR)0);
	m_wndTreeFltTime.SetRedraw(FALSE);
	m_wndTreeFltTime.DeleteItem(hItem);
	m_wndTreeFltTime.SetRedraw(TRUE);

	SetListCtrlContents();
}

void CDlgRunwayAssignment::OnEditFltType()
{
	HTREEITEM hTreeItem;
	if(!GetTreeItemByType(TREENODE_FLTTYPE, hTreeItem))
		return;

	

	FlightTypeRunwayAssignmentStrategyItem * pItem =
		(FlightTypeRunwayAssignmentStrategyItem *)m_wndTreeFltTime.GetItemData(hTreeItem);


	if (m_pSelectFlightType == NULL)
		return;
	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);

	
	CString strBuffer = szBuffer;
	HTREEITEM hItem = m_wndTreeFltTime.GetRootItem();
	while (hItem)
	{
		if (hTreeItem != hItem && m_wndTreeFltTime.GetItemText(hItem) == strBuffer)
		{
			MessageBox(_T("Flight: \" ") +strBuffer+ _T(" \" already exists."));
			return;
		}
		hItem = m_wndTreeFltTime.GetNextItem(hItem , TVGN_NEXT);
	}

	fltType.WriteSyntaxStringWithVersion(szBuffer);
	pItem->setFlightType(szBuffer);

	m_wndTreeFltTime.SetItemText(hTreeItem, strBuffer);
}


void CDlgRunwayAssignment::OnAddTimeRange()
{
	HTREEITEM hTreeItem;
	if(!GetTreeItemByType(TREENODE_FLTTYPE, hTreeItem))
		return;
	m_wndTreeFltTime.SelectItem(hTreeItem);

	ElapsedTime etStart, etEnd;
	CDlgTimeRange dlg(etStart, etEnd);
ADDTIMERANGE:if(IDOK!= dlg.DoModal())
		return;
	TimeRange tRange(dlg.GetStartTime(), dlg.GetEndTime());	

	CString strTimeRange;
	strTimeRange.Format("%s - %s", dlg.GetStartTimeString(), dlg.GetEndTimeString());

	HTREEITEM hItem = m_wndTreeFltTime.GetChildItem(hTreeItem);
	while (hItem)
	{
		if (!IsvalidTimeRange(m_wndTreeFltTime.GetItemText(hItem), strTimeRange))
		{
			MessageBox(_T("Time Range: \" ") +strTimeRange+ _T(" \" invalid."));
			goto ADDTIMERANGE;
		}
		hItem = m_wndTreeFltTime.GetNextItem(hItem , TVGN_NEXT);
	}
	
	FlightTypeRunwayAssignmentStrategyItem* pStrategyItem = 
		(FlightTypeRunwayAssignmentStrategyItem*)m_wndTreeFltTime.GetItemData(hTreeItem);
	ASSERT(pStrategyItem != NULL);

	TimeRangeRunwayAssignmentStrategyItem *pNewTimeRangeItem = new TimeRangeRunwayAssignmentStrategyItem(pStrategyItem->getID());
	pNewTimeRangeItem->SetTimeRange(tRange);
	pStrategyItem->AddTimeRangeRWAssignStrategyItem(pNewTimeRangeItem);

	m_wndTreeFltTime.SetRedraw(FALSE);
	HTREEITEM hTreeItemRange =  m_wndTreeFltTime.InsertItem(strTimeRange, hTreeItem, TVI_LAST);
	m_wndTreeFltTime.SetItemData(hTreeItemRange, (DWORD_PTR)pNewTimeRangeItem);
	m_wndTreeFltTime.Expand(hTreeItem, TVE_EXPAND);
	m_wndTreeFltTime.SetRedraw(TRUE);
}

bool CDlgRunwayAssignment::IsvalidTimeRange(CString strTimeRange1,CString strTimeRange2)
{
	CString str1 = _T("");
	CString str2 = _T("");
	str1 =  strTimeRange1.Mid(strTimeRange1.Find('-') + 1);
	str2 = strTimeRange2.Left(strTimeRange2.Find('-'));
	str1.Trim();
	str2.Trim();
	str1 = str1.Left(str1.Find(' '));
	str2 = str2.Left(str2.Find(' '));
	if(str1 > str2)
		return (false);
	else if(str1 < str2)
		return (true);
	str1 =  strTimeRange1.Mid(strTimeRange1.Find('-') + 1);
	str2 = strTimeRange2.Left(strTimeRange2.Find('-'));
	str1.Trim();
	str2.Trim();
	str1 = str1.Mid(str1.Find(' ') + 1);
	str2 = str2.Mid(str2.Find(' ') + 1);
	if(str1 > str2)
		return (false);
	else
		return (true);
}

void CDlgRunwayAssignment::OnEditTimeRange()
{
	HTREEITEM hTreeItem;
	if(!GetTreeItemByType(TREENODE_TIMERANGE, hTreeItem))
		return;

	TimeRangeRunwayAssignmentStrategyItem *pTimeRangeItem = 
		(TimeRangeRunwayAssignmentStrategyItem *)m_wndTreeFltTime.GetItemData(hTreeItem);

	TimeRange trValue = pTimeRangeItem->GetTimeRange();
	CDlgTimeRange dlg(trValue.GetStartTime(), trValue.GetEndTime());
EDITTIMERANGE:if(IDOK!= dlg.DoModal())
		return;

	TimeRange tRange(dlg.GetStartTime(), dlg.GetEndTime());
	

	CString strTimeRange;
	strTimeRange.Format("%s - %s", dlg.GetStartTimeString(), dlg.GetEndTimeString());

	HTREEITEM hItem = m_wndTreeFltTime.GetChildItem(m_wndTreeFltTime.GetParentItem(hTreeItem));
	while (hItem)
	{
		if (!IsvalidTimeRange(m_wndTreeFltTime.GetItemText(hItem), strTimeRange)&&hTreeItem!=hItem)
		{
			MessageBox(_T("TimeRange: \" ") +strTimeRange+ _T(" \" invalid."));
			goto EDITTIMERANGE;
		}
		hItem = m_wndTreeFltTime.GetNextItem(hItem , TVGN_NEXT);
	}

	pTimeRangeItem->SetTimeRange(tRange);
	m_wndTreeFltTime.SetItemText(hTreeItem, strTimeRange);
}

void CDlgRunwayAssignment::OnDelTimeRange()
{

	HTREEITEM hTreeItem = m_wndTreeFltTime.GetSelectedItem();

	if(!GetTreeItemByType(TREENODE_TIMERANGE, hTreeItem))
		return;
	HTREEITEM hParantItem = m_wndTreeFltTime.GetParentItem(hTreeItem);

	FlightTypeRunwayAssignmentStrategyItem* pStrategyItem = 
		(FlightTypeRunwayAssignmentStrategyItem*)m_wndTreeFltTime.GetItemData(hParantItem);
	ASSERT(pStrategyItem != NULL);

	TimeRangeRunwayAssignmentStrategyItem *pTimeRangeItem = 
		(TimeRangeRunwayAssignmentStrategyItem *)m_wndTreeFltTime.GetItemData(hTreeItem);

	pStrategyItem->DeleteTimeRangeRWAssignStrategyItem(pTimeRangeItem);
	m_wndTreeFltTime.SetRedraw(FALSE);
	m_wndTreeFltTime.DeleteItem(hTreeItem);
	m_wndTreeFltTime.SetRedraw(TRUE);
}

void CDlgRunwayAssignment::OnNewRunway()
{
	HTREEITEM hTreeItem;
	if(!GetTreeItemByType(TREENODE_TIMERANGE, hTreeItem))
		return;

	CDlgSelectRunway dlg(m_nProjID);
	if(IDOK != dlg.DoModal())
		return;
	int nAirportID = dlg.GetSelectedAirport();
	int nRunwayID = dlg.GetSelectedRunway();

	ALTAirport altAirport;
	altAirport.ReadAirport(nAirportID);
	Runway altRunway;
	altRunway.ReadObject(nRunwayID);	

	CString strMarking = (RUNWAYMARK_FIRST == dlg.GetSelectedRunwayMarkingIndex() ? altRunway.GetMarking1().c_str() : altRunway.GetMarking2().c_str());
	CString strRunwayInfo;
	strRunwayInfo.Format("%s:Runway:%s", altAirport.getName(), strMarking);

	int nCount = m_wndListTakeoffRunwayAssign.GetItemCount();	
	CString strRunway = _T("");
	for(int j = 0;j < nCount;j++ )
	{
		strRunway = m_wndListTakeoffRunwayAssign.GetItemText(j,0);
		strRunway = strRunway.Mid(strRunway.Find(":")+1);
		strRunway = strRunway.Left(strRunway.Find(":"));
		if(strRunway == altRunway.GetObjNameString())
		{
			MessageBox(_T("Can not select existent runway ,please."));
			return;
		}
	}
	
	TimeRangeRunwayAssignmentStrategyItem *pTimeRangeItem = 
		(TimeRangeRunwayAssignmentStrategyItem*)m_wndTreeFltTime.GetItemData(hTreeItem);

	RunwayAssignmentPercentItem* pNewItem = new RunwayAssignmentPercentItem(pTimeRangeItem->getID());
	pNewItem->m_nRunwayMarkIndex = (int)dlg.GetSelectedRunwayMarkingIndex();
	pNewItem->m_nRunwayID = nRunwayID;

	pTimeRangeItem->AddRunwayAssignPercentItem(pNewItem);
	m_wndListTakeoffRunwayAssign.InsertItem(nCount, strRunwayInfo);
	m_wndListTakeoffRunwayAssign.SetItemData(nCount, (DWORD_PTR)pNewItem);

	CString strPercent = _T("");
	int i = 0;
	strPercent.Format(_T("%d"),100/(nCount + 1));
	for (i = 0; i < nCount + 1; i++)
	{
		if(i == nCount)
			strPercent.Format(_T("%d"),100-nCount*100/(nCount + 1));
		m_wndListTakeoffRunwayAssign.SetItemText(i,1,strPercent);
	}
	TimeRangeRunwayAssignmentStrategyItem::RunwayAssignPercentVector & vrRunwayAssignPercent = pTimeRangeItem->GetManagedAssignmentRWAssignPercent();
	i = 0;
	for(TimeRangeRunwayAssignmentStrategyItem::RunwayAssignPercentIter itrPercent = vrRunwayAssignPercent.begin();\
		itrPercent != vrRunwayAssignPercent.end();++itrPercent,i++)
	{
		if(i == nCount)
			(*itrPercent)->m_dPercent = 1.0*100-1.0*nCount*100/(nCount + 1);
		else
			(*itrPercent)->m_dPercent = 1.0*100/(nCount + 1);
	}
}

void CDlgRunwayAssignment::OnDelRunway()
{
	HTREEITEM hTreeItem;
	if(!GetTreeItemByType(TREENODE_TIMERANGE, hTreeItem))
		return;
	TimeRangeRunwayAssignmentStrategyItem* pTimeRangeItem = 
		(TimeRangeRunwayAssignmentStrategyItem*)m_wndTreeFltTime.GetItemData(hTreeItem);


	POSITION pos = m_wndListTakeoffRunwayAssign.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_wndListTakeoffRunwayAssign.GetNextSelectedItem(pos);

			RunwayAssignmentPercentItem* pItem = (RunwayAssignmentPercentItem*)m_wndListTakeoffRunwayAssign.GetItemData(nItem);

			pTimeRangeItem->DeleteRunwayAssignPercentItem(pItem);
			m_wndListTakeoffRunwayAssign.DeleteItemEx(nItem);
		}
	}
}

void CDlgRunwayAssignment::OnEditRunway()
{	
	HTREEITEM hTreeItem;
	if(!GetTreeItemByType(TREENODE_TIMERANGE, hTreeItem))
		return;

	POSITION pos = m_wndListTakeoffRunwayAssign.GetFirstSelectedItemPosition();
	if(pos == NULL)
		return;
	while(pos)
	{
		int nItem = m_wndListTakeoffRunwayAssign.GetNextSelectedItem(pos);
		//
		CDlgSelectRunway dlg(m_nProjID);
		if(IDOK != dlg.DoModal())
			return;
		int nAirportID = dlg.GetSelectedAirport();
		int nRunwayID = dlg.GetSelectedRunway();

		ALTAirport altAirport;
		altAirport.ReadAirport(nAirportID);
		Runway altRunway;
		altRunway.ReadObject(nRunwayID);
		
		CString strRunwayInfo;
		CString strMarking = (RUNWAYMARK_FIRST == dlg.GetSelectedRunwayMarkingIndex() ? altRunway.GetMarking1().c_str() : altRunway.GetMarking2().c_str());
		strRunwayInfo.Format("%s:Runway:%s", altAirport.getName(), strMarking);

		//CString strRunway = _T("");
		//strRunway = m_wndListTakeoffRunwayAssign.GetItemText(nItem,0);
		//strRunway = strRunway.Mid(strRunway.Find(":")+1);
		//strRunway = strRunway.Left(strRunway.Find(":"));
		//if (strRunway != altRunway.GetObjNameString())
		//{
		//	MessageBox(_T("It can be only edit the runway:\" ") +strRunway + _T("\".") );
		//	return;
		//}
		
		TimeRangeRunwayAssignmentStrategyItem *pTimeRangeItem = 
			(TimeRangeRunwayAssignmentStrategyItem*)m_wndTreeFltTime.GetItemData(hTreeItem);

		RunwayAssignmentPercentItem* pItem = (RunwayAssignmentPercentItem*)m_wndListTakeoffRunwayAssign.GetItemData(nItem);
		pTimeRangeItem->UpdateRunwayAssignPercentItem(pItem);
		m_wndListTakeoffRunwayAssign.SetItemText(nItem, 0, strRunwayInfo);
		break;
	}
}



void CDlgRunwayAssignment::OnListItemEndLabelEdit(int nItem, int nSubItem)
{
	CString strValue = m_wndListTakeoffRunwayAssign.GetItemText(nItem, nSubItem);
	RunwayAssignmentPercentItem* pItem = (RunwayAssignmentPercentItem*)m_wndListTakeoffRunwayAssign.GetItemData(nItem);
	pItem->m_dPercent = (float)(atof(strValue));
}

void CDlgRunwayAssignment::OnListItemChanged(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);

	UpdateState();
	//	RunwayAssignmentPercentItem* pItem = (RunwayAssignmentPercentItem*)m_wndListTakeoffRunwayAssign.GetItemData(nItem);
	//	m_wndCmbAssignStrategy.SetCurSel((int)pItem->m_emType);
}

void CDlgRunwayAssignment::OnAssignmentStrategyChange()
{
	if(m_hTimeRangeTreeItem == NULL)
		return;

	TimeRangeRunwayAssignmentStrategyItem *pTimeRangeItem = 
		(TimeRangeRunwayAssignmentStrategyItem*)m_wndTreeFltTime.GetItemData(m_hTimeRangeTreeItem);

	//pTimeRangeItem->setStrategyType((RunwayAssignmentStrategyType)m_wndCmbAssignStrategy.GetCurSel());
	pTimeRangeItem->UpdateRunwayAssignPercentItemType((RunwayAssignmentStrategyType)m_wndCmbAssignStrategy.GetCurSel());


	m_wndTreeFltTime.SetFocus();
	m_wndTreeFltTime.SelectItem(m_hTimeRangeTreeItem);

	SetListCtrlContents();
	UpdateState();
}

void CDlgRunwayAssignment::OnSelChangedFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(GetTreeItemByType(TREENODE_TIMERANGE, m_hTimeRangeTreeItem))
	{
		TimeRangeRunwayAssignmentStrategyItem *pTimeRangeItem = 
			(TimeRangeRunwayAssignmentStrategyItem*)m_wndTreeFltTime.GetItemData(m_hTimeRangeTreeItem);
		m_wndCmbAssignStrategy.SetCurSel( (int)pTimeRangeItem->getStrategyType());
	}

	SetListCtrlContents();

	UpdateState();
}

void CDlgRunwayAssignment::UpdateState()
{

	HTREEITEM hFltTypeTreeItem;
	BOOL bEnable = GetTreeItemByType(TREENODE_FLTTYPE, hFltTypeTreeItem);
	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_DEL_FLTTYPE, bEnable);
	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_FLTTYPE, bEnable);

	HTREEITEM hTreeItem;
	m_wndCmbAssignStrategy.EnableWindow(GetTreeItemByType(TREENODE_TIMERANGE, hTreeItem));

	bEnable = TRUE;
	bEnable = IsManagedAssignmentTimeRangeTreeItem();

	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_NEW_RUNWAY, bEnable);
	m_wndListTakeoffRunwayAssign.EnableWindow(bEnable);

	//	bEnable &= (m_wndListTakeoffRunwayAssign.GetItemCount() > 0);
	//	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAY, bEnable);
	//	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY, bEnable);

	POSITION pos = m_wndListTakeoffRunwayAssign.GetFirstSelectedItemPosition();
	bEnable = TRUE;
	if(pos == NULL)
		bEnable = FALSE;

	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_EDIT_RUNWAY, bEnable);
	m_wndRunwayToolbar.GetToolBarCtrl().EnableButton(ID_DEL_RUNWAY, bEnable);
}


void CDlgRunwayAssignment::OnClickFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	//SetListCtrlContents();
	*pResult = 0;
}

BOOL CDlgRunwayAssignment::GetTreeItemByType(TreeNodeType nodeType, HTREEITEM& hTreeItem)
{
	hTreeItem = NULL;
	HTREEITEM hTreeItemSel = m_wndTreeFltTime.GetSelectedItem();
	if (hTreeItemSel == NULL)
		return FALSE;

	switch(nodeType) {
	case TREENODE_FLTTYPE:
		{
			HTREEITEM hParent = m_wndTreeFltTime.GetParentItem(hTreeItemSel);
			if(hParent != NULL)
				return FALSE;

			hTreeItem = hTreeItemSel;
		}
		break;
	case TREENODE_TIMERANGE:
		{
			HTREEITEM hParent = m_wndTreeFltTime.GetParentItem(hTreeItemSel);
			if(hParent == NULL)
				return FALSE;

			hTreeItem = hTreeItemSel;
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL CDlgRunwayAssignment::IsManagedAssignmentTimeRangeTreeItem()
{
	HTREEITEM hTreeItem = m_wndTreeFltTime.GetSelectedItem();
	if (hTreeItem == NULL)
		return FALSE;

	HTREEITEM hParent = m_wndTreeFltTime.GetParentItem(hTreeItem);
	if(hParent == NULL)
		return FALSE;

	TimeRangeRunwayAssignmentStrategyItem *pTimeRangeItem = 
		(TimeRangeRunwayAssignmentStrategyItem*)m_wndTreeFltTime.GetItemData(hTreeItem);

	if(ManagedAssignment == pTimeRangeItem->getStrategyType())
		return TRUE;

	return FALSE;
}

void CDlgRunwayAssignment::ContextMenu(CWnd* pWnd, CPoint point)
{
	CRect  rtFltTypeTree;
	m_wndTreeFltTime.GetWindowRect(&rtFltTypeTree);
	if( !rtFltTypeTree.PtInRect(point))
		return;

	m_wndTreeFltTime.SetFocus();
	CPoint pt = point;
	m_wndTreeFltTime.ScreenToClient( &pt );
	UINT iRet;
	HTREEITEM hRClickItem = m_wndTreeFltTime.HitTest( pt, &iRet );
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
		hRClickItem = NULL;
	if (hRClickItem==NULL)
		return;

	m_wndTreeFltTime.SelectItem(hRClickItem);

	CMenu menuPopup;
	menuPopup.LoadMenu( IDR_CTX_MENU_STRATEGY_TIMERANGE );
	CMenu* pMenu=NULL;
	pMenu = menuPopup.GetSubMenu(0);

	if(!GetTreeItemByType(TREENODE_FLTTYPE, hRClickItem))
	{
		pMenu->EnableMenuItem( ID_ADD_TIMERANGE, MF_GRAYED );
	}
	else if (!GetTreeItemByType(TREENODE_TIMERANGE, hRClickItem))
	{
		pMenu->EnableMenuItem( ID_EDIT_TIMERANGE, MF_GRAYED );
		pMenu->EnableMenuItem( ID_DEL_TIMERANGE, MF_GRAYED );
	}


	if (pMenu)
		pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}
void CDlgRunwayAssignment::OnOK()
{
	int nCount = 0;
	int iTotals = 0;
	nCount = m_wndListTakeoffRunwayAssign.GetItemCount();
	if (nCount > 0)
	{
		for (int i = 0; i < nCount ; i++)
		{
			iTotals += _ttol(m_wndListTakeoffRunwayAssign.GetItemText(i,1) );
		}
		if (iTotals != 100)
		{
			MessageBox(_T("The sum of operation percent should equal 100%"));
			return;
		}
	}

	try
	{
		CADODatabase::BeginTransaction();
		m_pStrategies->SaveData();

		CADODatabase::CommitTransaction();
	}
	catch (CADOException &e) 
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
		MessageBox(_T("Save Data Error."));
	}

	CXTResizeDialog::OnOK();
}

void CDlgRunwayAssignment::OnSave()
{

}

void CDlgRunwayAssignment::OnCancel()
{
	CXTResizeDialog::OnCancel();
}
