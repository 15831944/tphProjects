// DlgRunwayTakeoffPosition.cpp : implementation file
//
#include "stdafx.h"
#include "Resource.h"
#include "DlgRunwayTakeoffPosition.h"
#include "DlgTimeRange.h"
#include "InputAirside/ALTAirport.h"
#include ".\dlgrunwaytakeoffposition.h"
#include "..\common\TimeRange.h"
#include "../InputAirside/HoldPosition.h"
#include "..\InputAirside\InputAirside.h"
//#include "..\Common\AirportDatabase.h"

// CDlgRunwayTakeoffPosition dialog

static const UINT ID_NEW_FLTTIMERANGE = 10;
static const UINT ID_DEL_FLTTIMERANGE = 11;
static const UINT ID_EDIT_FLTTIMERANGE = 12;

CDlgRunwayTakeoffPosition::CDlgRunwayTakeoffPosition(int nProjID, PSelectFlightType pSelectFlightType, InputAirside* pInputAirside, CWnd* pParent /*=NULL*/)
 : CXTResizeDialog(CDlgRunwayTakeoffPosition::IDD, pParent)
 , m_pSelectFlightType(pSelectFlightType)
 , m_nProjID(nProjID)
 , m_runwayTakeoffPos(nProjID)
 , m_nRemainFt(0.0)
{
	m_runwayTakeoffPos.SetAirportDB(pInputAirside->m_pAirportDB);
	m_runwayTakeoffPos.ReadData();
	m_runwayTakeoffPos.RemoveInvalidData();
	m_runwayTakeoffPos.UpdateRwyTakeoffPosData();
//	m_pAirportDB = pInputAirside->m_pAirportDB;

}

CDlgRunwayTakeoffPosition::~CDlgRunwayTakeoffPosition()
{
}

void CDlgRunwayTakeoffPosition::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_RUNWAY, m_wndTreeRunway);
	DDX_Control(pDX, IDC_TREE_FLTTIMERANGE, m_wndTreeFltTimeRange);
	DDX_Control(pDX, IDC_TREE_INTERSECTIONS, m_wndTreeIntersection);
	
	DDX_Control(pDX, IDC_CHECK_ELIGIBLE, m_bEligible);
	DDX_Control(pDX, IDC_EDIT_REMAINING, m_CtrlRemainFt);
	
	DDX_Control(pDX, IDC_SPIN_REMAINING, m_spin);
}

BEGIN_MESSAGE_MAP(CDlgRunwayTakeoffPosition, CXTResizeDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnSave)
	ON_COMMAND(ID_NEW_FLTTIMERANGE, OnNewFltTimeRange)
	ON_COMMAND(ID_DEL_FLTTIMERANGE, OnDelFltTimeRange)
	ON_COMMAND(ID_EDIT_FLTTIMERANGE, OnEditFltTimeRange)
	ON_COMMAND(ID_FLTTYPE_NEW, OnNewFltType)
	ON_COMMAND(ID_FLTTYPE_DEL, OnDelFltType)
	ON_COMMAND(ID_FLTTYPE_EDIT, OnEditFltType)
	ON_COMMAND(ID_TAKEOFFPOS_MAXBACKTRACK, OnMaxBackTrackOp)
	ON_COMMAND(ID_TAKEOFFPOS_MINBACKTRACK, OnMinBackTrackOp)
	ON_COMMAND(ID_TAKEOFFPOS_NOBACKTRACK, OnNoBackTrackOp)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FLTTIMERANGE, OnTvnSelchangedFltTimeTree)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_RUNWAY, OnTvnSelchangedTreeRunway)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_INTERSECTIONS, OnTvnSelchangedTreeTakeoffPosition)
	ON_BN_CLICKED(IDC_CHECK_ELIGIBLE, OnBnClickedCheckEligible)
	ON_EN_CHANGE(IDC_EDIT_REMAINING, OnEnChangeEditRemaining)
END_MESSAGE_MAP()

int CDlgRunwayTakeoffPosition::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	CToolBarCtrl& wndToolbar = m_wndToolbar.GetToolBarCtrl();
	wndToolbar.SetCmdID(0, ID_NEW_FLTTIMERANGE);
	wndToolbar.SetCmdID(1, ID_DEL_FLTTIMERANGE);
	wndToolbar.SetCmdID(2, ID_EDIT_FLTTIMERANGE);

	return 0;
}

BOOL CDlgRunwayTakeoffPosition::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	// set the position of the runway toolbar
	CRect rectToolbar;
	m_wndTreeFltTimeRange.GetWindowRect(&rectToolbar);
	ScreenToClient(&rectToolbar);
	rectToolbar.top -= 26;
	rectToolbar.bottom = rectToolbar.top + 22;
	rectToolbar.left += 4;
	m_wndToolbar.MoveWindow(&rectToolbar);

	CToolBarCtrl& wndToolbar = m_wndToolbar.GetToolBarCtrl();
	wndToolbar.EnableButton(ID_NEW_FLTTIMERANGE,FALSE);
	wndToolbar.EnableButton(ID_DEL_FLTTIMERANGE,FALSE);
	wndToolbar.EnableButton(ID_EDIT_FLTTIMERANGE,FALSE);

	m_wndTreeFltTimeRange.EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_ELIGIBLE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
//	GetDlgItem(IDC_EDIT_REMAINING)->EnableWindow(FALSE);
//	GetDlgItem(IDC_SPIN_REMAINING)->EnableWindow(FALSE);	
	
	InitAirPortRunwayTreeCtrl();
	init = 1;
	m_nRemainFt = 0.0;
	m_spin.SetRange(0,1000); 

	UpdateData(FALSE);
	m_CtrlRemainFt.SetWindowText("");

	SetResize(m_wndToolbar.GetDlgCtrlID(), SZ_MIDDLE_CENTER, SZ_MIDDLE_CENTER); 
	SetResize(IDC_STATIC_INTERSECTIONS, SZ_TOP_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_PROPERTIES, SZ_MIDDLE_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_DATA, SZ_MIDDLE_CENTER , SZ_BOTTOM_RIGHT);
	
	SetResize(IDC_TREE_RUNWAY, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_TREE_INTERSECTIONS, SZ_TOP_CENTER , SZ_MIDDLE_RIGHT);
	SetResize(IDC_TREE_FLTTIMERANGE, SZ_MIDDLE_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_CHECK_ELIGIBLE, SZ_MIDDLE_CENTER , SZ_MIDDLE_CENTER);
	SetResize(IDC_STATIC_REMAINING, SZ_MIDDLE_CENTER , SZ_MIDDLE_CENTER);
	SetResize(IDC_EDIT_REMAINING, SZ_MIDDLE_CENTER , SZ_MIDDLE_CENTER);
	SetResize(IDC_SPIN_REMAINING, SZ_MIDDLE_CENTER , SZ_MIDDLE_CENTER);
	SetResize(IDC_STATIC_M, SZ_MIDDLE_CENTER , SZ_MIDDLE_CENTER);	
		
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_DESCRIPTION,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void CDlgRunwayTakeoffPosition::OnNewFltTimeRange()
{

	CDlgTimeRange dlg(ElapsedTime(0L), ElapsedTime(0L));
	if(IDOK != dlg.DoModal())
		return;
	CString strTimeRange(dlg.GetStartTimeString());
	strTimeRange += _T(" - ");
	strTimeRange += dlg.GetEndTimeString();
	TimeRange tRange(dlg.GetStartTime(), dlg.GetEndTime());

	HTREEITEM hRootItem = m_wndTreeFltTimeRange.GetRootItem();
	while (hRootItem)
	{
		CRunwayTakeOffTimeRangeData* pTimeRangeItem = (CRunwayTakeOffTimeRangeData*)m_wndTreeFltTimeRange.GetItemData(hRootItem);
		if (pTimeRangeItem->GetEndTime() <= tRange.GetStartTime() || pTimeRangeItem->GetStartTime() >= tRange.GetEndTime())
		{
			hRootItem = m_wndTreeFltTimeRange.GetNextItem(hRootItem , TVGN_NEXT);
		}
		else
		{
			MessageBox(_T("Time Range: \" ") +strTimeRange+ _T(" \" conflict."));
			//	break;
			return;
		}

	}

	m_wndTreeFltTimeRange.SetRedraw(FALSE);
	HTREEITEM hTimeTreeItem = m_wndTreeFltTimeRange.InsertItem(strTimeRange);
	m_wndTreeFltTimeRange.SetRedraw(TRUE);
	HTREEITEM hInterTreeItem = m_wndTreeIntersection.GetSelectedItem();
	CRunwayTakeOffPositionData* pItem = (CRunwayTakeOffPositionData*)m_wndTreeIntersection.GetItemData(hInterTreeItem);
	CRunwayTakeOffTimeRangeData* pNewItem = new CRunwayTakeOffTimeRangeData;
	pNewItem->SetRwTwIntID(pItem->getID());
	pNewItem->SetStartTime(dlg.GetStartTime());
	pNewItem->SetEndTime(dlg.GetEndTime());
	m_wndTreeFltTimeRange.SetItemData(hTimeTreeItem,(DWORD_PTR)pNewItem);
	pItem->AddData(pNewItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgRunwayTakeoffPosition::OnDelFltTimeRange()
{
	HTREEITEM hTimeTreeItem = m_wndTreeFltTimeRange.GetSelectedItem();
	ASSERT(hTimeTreeItem);
	CRunwayTakeOffTimeRangeData* pTimeItem = (CRunwayTakeOffTimeRangeData*)m_wndTreeFltTimeRange.GetItemData(hTimeTreeItem);
	ASSERT(hTimeTreeItem != NULL);
	m_wndTreeFltTimeRange.DeleteItem(hTimeTreeItem);

	HTREEITEM hInterTreeItem = m_wndTreeIntersection.GetSelectedItem();
	CRunwayTakeOffPositionData* pItem = (CRunwayTakeOffPositionData*)m_wndTreeIntersection.GetItemData(hInterTreeItem);
	pItem->DelData(pTimeItem);

	// To check whether there is still item listed
	// if none, disable the 'Delete' and 'Edit'
	hTimeTreeItem = m_wndTreeFltTimeRange.GetSelectedItem();
	CToolBarCtrl& wndToolbar = m_wndToolbar.GetToolBarCtrl();
	wndToolbar.EnableButton(ID_DEL_FLTTIMERANGE, (BOOL)hTimeTreeItem);
	wndToolbar.EnableButton(ID_EDIT_FLTTIMERANGE, (BOOL)hTimeTreeItem);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgRunwayTakeoffPosition::OnEditFltTimeRange()
{		
	HTREEITEM hTimeTreeItem = m_wndTreeFltTimeRange.GetSelectedItem();
	ASSERT(hTimeTreeItem != NULL);
//	CRunwayTakeOffTimeRangeData* pTimeItem1 = (CRunwayTakeOffTimeRangeData*)m_wndTreeFltTimeRange.GetItemData(hTimeTreeItem);
//	CRunwayTakeOffTimeRangeData* pTimeItem2 = new CRunwayTakeOffTimeRangeData;
//	pTimeItem2->setID(pTimeItem1->getID());
	CRunwayTakeOffTimeRangeData* pTimeItem = (CRunwayTakeOffTimeRangeData*)m_wndTreeFltTimeRange.GetItemData(hTimeTreeItem);
	CDlgTimeRange dlg(pTimeItem->GetStartTime(), pTimeItem->GetEndTime());
	if(IDOK != dlg.DoModal())
		return;
	CString strTimeRange(dlg.GetStartTimeString());
	strTimeRange += _T(" - ");
	strTimeRange += dlg.GetEndTimeString();	

	TimeRange tRange(dlg.GetStartTime(), dlg.GetEndTime());

	HTREEITEM hRootItem = m_wndTreeFltTimeRange.GetRootItem();
	while (hRootItem)
	{
		if(hRootItem != hTimeTreeItem)
		{
			CRunwayTakeOffTimeRangeData* pTimeRangeItem = (CRunwayTakeOffTimeRangeData*)m_wndTreeFltTimeRange.GetItemData(hRootItem);
			if (pTimeRangeItem->GetEndTime() <= tRange.GetStartTime() || pTimeRangeItem->GetStartTime() >= tRange.GetEndTime())
			{
				
			}
			else
			{
				MessageBox(_T("Time Range: \" ") +strTimeRange+ _T(" \" conflict."));
				//	break;
				return;
			}				
		}
		hRootItem = m_wndTreeFltTimeRange.GetNextItem(hRootItem , TVGN_NEXT);
	}

	m_wndTreeFltTimeRange.SetRedraw(FALSE);
	m_wndTreeFltTimeRange.SetItemText(hTimeTreeItem, strTimeRange);
	m_wndTreeFltTimeRange.SetRedraw(TRUE);
	pTimeItem->SetStartTime(dlg.GetStartTime());
	pTimeItem->SetEndTime(dlg.GetEndTime());
//	pTimeItem2->SetStartTime(dlg.GetStartTime());
//	pTimeItem2->SetEndTime(dlg.GetEndTime());
//	m_wndTreeFltTimeRange.SetItemData(hTimeTreeItem,(DWORD_PTR)pTimeItem2);

//	HTREEITEM hInterTreeItem = m_wndTreeIntersection.GetSelectedItem();
//	CRunwayTakeOffPositionData* pItem = (CRunwayTakeOffPositionData*)m_wndTreeIntersection.GetItemData(hInterTreeItem);
//	pItem->UpdateVData(pTimeItem1,pTimeItem2);


}

void CDlgRunwayTakeoffPosition::OnTvnSelchangedFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hSelItem = m_wndTreeFltTimeRange.GetSelectedItem();

	BOOL bEnable = FALSE;
	if (hSelItem != NULL && m_wndTreeFltTimeRange.GetParentItem(hSelItem) == NULL)
		bEnable = TRUE;

	CToolBarCtrl& wndToolbar = m_wndToolbar.GetToolBarCtrl();
	wndToolbar.EnableButton(ID_DEL_FLTTIMERANGE, bEnable);
	wndToolbar.EnableButton(ID_EDIT_FLTTIMERANGE, bEnable);

	*pResult = 0;
}

void CDlgRunwayTakeoffPosition::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CRect rectTree;
	m_wndTreeFltTimeRange.GetWindowRect(&rectTree);
	if (!rectTree.PtInRect(point)) 
		return;

	m_wndTreeFltTimeRange.SetFocus();
	CPoint pt = point;
	m_wndTreeFltTimeRange.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hRClickItem = m_wndTreeFltTimeRange.HitTest(pt, &iRet);
	if (iRet != TVHT_ONITEMLABEL)
	{
		hRClickItem = NULL;
		return;
	}

	m_wndTreeFltTimeRange.SelectItem(hRClickItem);

	CMenu menuPopup;
	menuPopup.LoadMenu(IDR_MENU_RUNWAYTAKEOFFPOSITION);
	CMenu* pSubMenu = NULL;

	HTREEITEM hParentItem = m_wndTreeFltTimeRange.GetParentItem(hRClickItem);
	if ( hParentItem == NULL)
		pSubMenu = menuPopup.GetSubMenu(0);
	else
	{
		if (m_wndTreeFltTimeRange.GetParentItem(hParentItem) == NULL)
			pSubMenu = menuPopup.GetSubMenu(1);
	}

	if (pSubMenu != NULL)
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}

void CDlgRunwayTakeoffPosition::OnNewFltType()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);
	HTREEITEM hSelItem = m_wndTreeFltTimeRange.GetSelectedItem();
	ASSERT(hSelItem != NULL);

	CString strFlightType;
	char szBuffer[1024] = {0};
	fltType.WriteSyntaxStringWithVersion(szBuffer);
	strFlightType = szBuffer;

	CRunwayTakeOffTimeRangeData* pItem = (CRunwayTakeOffTimeRangeData*)m_wndTreeFltTimeRange.GetItemData(hSelItem);
	CRunwayTakeOffTimeRangeFltType* pNewItem = new CRunwayTakeOffTimeRangeFltType;

	pNewItem->SetTimeRangeID(pItem->getID());
	pNewItem->SetFltType(fltType);
	pItem->AddData(pNewItem);
	AddFlightTypeToTree(pNewItem,hSelItem);

	//fltType.screenPrint(szBuffer);
	//strFlightType = szBuffer;
	//m_wndTreeFltTimeRange.SetRedraw(FALSE);
	//HTREEITEM hFltTypeItem = m_wndTreeFltTimeRange.InsertItem(strFlightType, hSelItem);
	//m_wndTreeFltTimeRange.SetRedraw(TRUE);
	//m_wndTreeFltTimeRange.SetItemData(hFltTypeItem,(DWORD_PTR)pNewItem);
	m_wndTreeFltTimeRange.Expand(hSelItem, TVE_EXPAND);

}

void CDlgRunwayTakeoffPosition::OnDelFltType()
{
	HTREEITEM hFltTypeItem = m_wndTreeFltTimeRange.GetSelectedItem();
	CRunwayTakeOffTimeRangeFltType* pFltTypeItem = (CRunwayTakeOffTimeRangeFltType*)m_wndTreeFltTimeRange.GetItemData(hFltTypeItem);
	ASSERT(hFltTypeItem != NULL);
	
	HTREEITEM hTimeTreeItem = m_wndTreeFltTimeRange.GetParentItem(hFltTypeItem);
	CRunwayTakeOffTimeRangeData* pItem = (CRunwayTakeOffTimeRangeData*)m_wndTreeFltTimeRange.GetItemData(hTimeTreeItem);
	pItem->DelData(pFltTypeItem);
	m_wndTreeFltTimeRange.DeleteItem(hFltTypeItem);
	// TODO: delete data from database
}

void CDlgRunwayTakeoffPosition::OnEditFltType()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);
	HTREEITEM hFltTypeItem = m_wndTreeFltTimeRange.GetSelectedItem();
	ASSERT(hFltTypeItem != NULL);
	CRunwayTakeOffTimeRangeFltType* pFltTypeItem = (CRunwayTakeOffTimeRangeFltType*)m_wndTreeFltTimeRange.GetItemData(hFltTypeItem);
//	CRunwayTakeOffTimeRangeFltType* pFltTypeItem1 = (CRunwayTakeOffTimeRangeFltType*)m_wndTreeFltTimeRange.GetItemData(hFltTypeItem);
//	CRunwayTakeOffTimeRangeFltType* pFltTypeItem2 = new CRunwayTakeOffTimeRangeFltType;
//	pFltTypeItem2->setID(pFltTypeItem1->getID());

	CString strFlightType;
	fltType.screenPrint(strFlightType);
	m_wndTreeFltTimeRange.SetRedraw(FALSE);
	m_wndTreeFltTimeRange.SetItemText(hFltTypeItem, strFlightType);
	m_wndTreeFltTimeRange.SetRedraw(TRUE);
	pFltTypeItem->SetFltType(fltType);//

//	pFltTypeItem2->SetFltType(strFlightType);	
//	m_wndTreeFltTimeRange.SetItemData(hFltTypeItem,(DWORD_PTR)pFltTypeItem2);
//	HTREEITEM hTimeTreeItem = m_wndTreeFltTimeRange.GetParentItem(hFltTypeItem);
//	CRunwayTakeOffTimeRangeData* pItem = (CRunwayTakeOffTimeRangeData*)m_wndTreeFltTimeRange.GetItemData(hTimeTreeItem);
//	pItem->UpdateVData(pFltTypeItem1,pFltTypeItem2);
}

void CDlgRunwayTakeoffPosition::OnMaxBackTrackOp()
{
	HTREEITEM hFltTypeItem = m_wndTreeFltTimeRange.GetSelectedItem();
	CRunwayTakeOffTimeRangeFltType* pFltTypeItem = (CRunwayTakeOffTimeRangeFltType*)m_wndTreeFltTimeRange.GetItemData(hFltTypeItem);
	HTREEITEM hBackTrackItem = m_wndTreeFltTimeRange.GetChildItem(hFltTypeItem);

	CString strData = _T("Max Backtrack Operation");
	m_wndTreeFltTimeRange.SetRedraw(FALSE);

	if (hBackTrackItem)
	{
		m_wndTreeFltTimeRange.SetItemText( hBackTrackItem,strData);
	}
	else
	{
		hBackTrackItem = m_wndTreeFltTimeRange.InsertItem(strData, hFltTypeItem);
	}

	m_wndTreeFltTimeRange.SetRedraw(TRUE);
	m_wndTreeFltTimeRange.Expand(hFltTypeItem, TVE_EXPAND);
	pFltTypeItem->SetBacktrack(CRunwayTakeOffTimeRangeFltType::MaxBacktrack);

}

void CDlgRunwayTakeoffPosition::OnMinBackTrackOp()
{
	HTREEITEM hFltTypeItem = m_wndTreeFltTimeRange.GetSelectedItem();
	CRunwayTakeOffTimeRangeFltType* pFltTypeItem = (CRunwayTakeOffTimeRangeFltType*)m_wndTreeFltTimeRange.GetItemData(hFltTypeItem);
	HTREEITEM hBackTrackItem = m_wndTreeFltTimeRange.GetChildItem(hFltTypeItem);

	CString strData = _T("Min Backtrack Operation");
	m_wndTreeFltTimeRange.SetRedraw(FALSE);

	if (hBackTrackItem)
	{
		m_wndTreeFltTimeRange.SetItemText( hBackTrackItem,strData);
	}
	else
	{
		hBackTrackItem = m_wndTreeFltTimeRange.InsertItem(strData, hFltTypeItem);
	}

	m_wndTreeFltTimeRange.SetRedraw(TRUE);
	m_wndTreeFltTimeRange.Expand(hFltTypeItem, TVE_EXPAND);
	pFltTypeItem->SetBacktrack(CRunwayTakeOffTimeRangeFltType::MinBacktrack);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgRunwayTakeoffPosition::OnNoBackTrackOp()
{
	HTREEITEM hFltTypeItem = m_wndTreeFltTimeRange.GetSelectedItem();
	CRunwayTakeOffTimeRangeFltType* pFltTypeItem = (CRunwayTakeOffTimeRangeFltType*)m_wndTreeFltTimeRange.GetItemData(hFltTypeItem);
	HTREEITEM hBackTrackItem = m_wndTreeFltTimeRange.GetChildItem(hFltTypeItem);

	CString strData = _T("No Backtrack Operation");
	m_wndTreeFltTimeRange.SetRedraw(FALSE);

	if (hBackTrackItem)
	{
		m_wndTreeFltTimeRange.SetItemText( hBackTrackItem,strData);
	}
	else
	{
		hBackTrackItem = m_wndTreeFltTimeRange.InsertItem(strData, hFltTypeItem);
	}

	m_wndTreeFltTimeRange.SetRedraw(TRUE);
	m_wndTreeFltTimeRange.Expand(hFltTypeItem, TVE_EXPAND);
	pFltTypeItem->SetBacktrack(CRunwayTakeOffTimeRangeFltType::NoBacktrack);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgRunwayTakeoffPosition::OnOK()
{
	if (m_runwayTakeoffPos.IfHaveInvalidData())
	{
		if(AfxMessageBox(_T("Are you sure to delete the invalid data(red item)?"), MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			m_runwayTakeoffPos.KeepInvalidData();
		}
	}
	try
	{
		CADODatabase::BeginTransaction();
		m_runwayTakeoffPos.SaveData();
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
		MessageBox(_T("Save data failed."));
	}

	CXTResizeDialog::OnOK();
}

void CDlgRunwayTakeoffPosition::OnCancel()
{
	CXTResizeDialog::OnCancel();
}

void CDlgRunwayTakeoffPosition::OnSave()
{
	if (m_runwayTakeoffPos.IfHaveInvalidData())
	{
		if(AfxMessageBox(_T("Are you sure to delete the invalid data(red item)?"), MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			m_runwayTakeoffPos.KeepInvalidData();
		}
	}
	try
	{
		CADODatabase::BeginTransaction();
		m_runwayTakeoffPos.SaveData();
		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
		MessageBox(_T("Save data failed."));
	}

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
//	CXTResizeDialog::OnOK();
}

void CDlgRunwayTakeoffPosition::InitAirPortRunwayTreeCtrl()
{
	for (int i=0; i<m_runwayTakeoffPos.GetAirportCount(); i++)
	{
		CRunwayTakeOffAirportData* pAirtPortItem = m_runwayTakeoffPos.GetAirportItem(i);
		ALTAirport* altAirport = pAirtPortItem->getAirport();
		HTREEITEM hAirportTreeItem = m_wndTreeRunway.InsertItem(altAirport->getName());
		m_wndTreeRunway.SetItemData(hAirportTreeItem, (DWORD_PTR)pAirtPortItem);
		for (int j=0; j<pAirtPortItem->GetRunwayCount(); j++)
		{
			CRunwayTakeOffRunwayData* pRunwayItem = pAirtPortItem->GetRunwayItem(j);
			Runway* altRunway = pRunwayItem->getRunway();
			//pRunwayItem->ReadData(m_nProjID);
			CString strMarkTemp1,strMarkTemp2;
			strMarkTemp1 = altRunway->GetMarking1().c_str();
			strMarkTemp2 = altRunway->GetMarking2().c_str();

			HTREEITEM hMarking1Item = m_wndTreeRunway.InsertItem(strMarkTemp1, hAirportTreeItem, TVI_LAST);
			CRunwayTakeOffRunwayWithMarkData* runwayMark1Data = pRunwayItem->getRunwayMark1Data(); 
			ASSERT(runwayMark1Data != NULL);
			//if(runwayMark1Data)
			//	runwayMark1Data->setRunwayMarkIndex(0);
			m_wndTreeRunway.SetItemData(hMarking1Item, (DWORD_PTR)runwayMark1Data);

			HTREEITEM hMarking2Item = m_wndTreeRunway.InsertItem(strMarkTemp2, hAirportTreeItem, TVI_LAST);	
			CRunwayTakeOffRunwayWithMarkData* runwayMark2Data = pRunwayItem->getRunwayMark2Data(); 
			ASSERT(runwayMark2Data != NULL);
			//if(runwayMark2Data)
			//	runwayMark2Data->setRunwayMarkIndex(1);	
			m_wndTreeRunway.SetItemData(hMarking2Item, (DWORD_PTR)runwayMark2Data);
		}
		m_wndTreeRunway.Expand(hAirportTreeItem, TVE_EXPAND);
	}
}
void CDlgRunwayTakeoffPosition::InitRwyTakeoffPositions()
{
	m_wndTreeIntersection.SelectItem(NULL);
	m_wndTreeIntersection.DeleteAllItems();
	HTREEITEM hSelItem = m_wndTreeRunway.GetSelectedItem();

	if (NULL == hSelItem)
		return;

	if (m_wndTreeRunway.GetParentItem(hSelItem) == NULL)
		return;

	m_wndTreeIntersection.SetRedraw(FALSE);
	CRunwayTakeOffRunwayWithMarkData* runwayMarkData = (CRunwayTakeOffRunwayWithMarkData*)m_wndTreeRunway.GetItemData(hSelItem);

	int nCount = runwayMarkData->GetRwyTakeoffPosCount();
	for (int i =0; i < nCount; i++)
	{
		CString strMarkTemp = "";
		CRunwayTakeOffPositionData* pPosItem = runwayMarkData->GetRwyTakeoffPosItem(i);
		
 		strMarkTemp = pPosItem->GetRunwayExitDescription().GetName();
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		cni.nt = NT_NORMAL;
		cni.net = NET_NORMAL;
 		HTREEITEM hTakeoffPosItem = m_wndTreeIntersection.InsertItem(strMarkTemp,cni,FALSE);
 		m_wndTreeIntersection.SetItemData(hTakeoffPosItem, (DWORD_PTR)pPosItem);
	}

	int nInvalidCount = (int)runwayMarkData->GetInvalidTakeoffPos().size();
	for (int ii = 0; ii< nInvalidCount; ii++)
	{
		CString strMarkTemp = _T("");
		CRunwayTakeOffPositionData* pPosItem = runwayMarkData->GetInvalidTakeoffPos().at(ii);
		strMarkTemp = pPosItem->GetRunwayExitDescription().GetName();
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		cni.nt = NT_NORMAL;
		cni.net = NET_NORMAL;
		cni.bInvalidData = TRUE;
		HTREEITEM hTakeoffPosItem = m_wndTreeIntersection.InsertItem(strMarkTemp,cni,FALSE);
		m_wndTreeIntersection.SetItemData(hTakeoffPosItem, (DWORD_PTR)pPosItem);
	}


	//for (int i=0; i<runwayMarkData->GetTaxiwayCount(); i++)
	//{
	//	CRunwayTakeOffRMTaxiwayData* pTaxiwayItem = runwayMarkData->GetRwyTakeoffPosItem(i);
	//	ALTObject* pIntersectObject = pTaxiwayItem->getIntersectObject();
	//	if(pIntersectObject == NULL)
	//		continue;
	//	CString taxiwayName = pIntersectObject->GetObjectName().GetIDString();

	//	HTREEITEM hTaxiwayTreeItem = m_wndTreeIntersection.InsertItem(taxiwayName);
	//	m_wndTreeIntersection.SetItemData(hTaxiwayTreeItem, (DWORD_PTR)pTaxiwayItem);
	//	int nInterCount = pTaxiwayItem->GetInterCount();
	//	for (int j=0; j<nInterCount; j++)
	//	{
	//		CRunwayTakeOffPositionData* pInterItem
	//			= pTaxiwayItem->GetInterItem(j);
	//		CString strMarkIntTemp;
	//		if(nInterCount == 1)
	//			strMarkIntTemp.Format("%s&%s",strMarkTemp,taxiwayName);
	//		else
	//		{
	//			int nIntersecNodeID = pInterItem->GetRwyExitID();
	//			IntersectionNode intersectionNode;
	//			intersectionNode.ReadData(nIntersecNodeID);
	//			int idx = intersectionNode.GetIndex();		
	//			strMarkIntTemp.Format("%s&%s(%d)",strMarkTemp,taxiwayName,idx+1);
	//		}
	//		HTREEITEM hInterTreeItem = m_wndTreeIntersection.InsertItem(strMarkIntTemp, hTaxiwayTreeItem, TVI_LAST);
	//		m_wndTreeIntersection.SetItemData(hInterTreeItem, (DWORD_PTR)pInterItem);		
	//	}
	//	m_wndTreeIntersection.Expand(hTaxiwayTreeItem, TVE_EXPAND);		
	//}

	m_wndTreeIntersection.SetRedraw(TRUE);
}

void CDlgRunwayTakeoffPosition::OnTvnSelchangedTreeRunway(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	
	GetDlgItem(IDC_CHECK_ELIGIBLE)->EnableWindow(FALSE);
//	GetDlgItem(IDC_EDIT_REMAINING)->EnableWindow(FALSE);
	m_CtrlRemainFt.SetWindowText("");
//	GetDlgItem(IDC_SPIN_REMAINING)->EnableWindow(FALSE);
	CToolBarCtrl& wndToolbar = m_wndToolbar.GetToolBarCtrl();
	wndToolbar.EnableButton(ID_NEW_FLTTIMERANGE,FALSE);
	wndToolbar.EnableButton(ID_DEL_FLTTIMERANGE,FALSE);
	wndToolbar.EnableButton(ID_EDIT_FLTTIMERANGE,FALSE);
	InitRwyTakeoffPositions();
	m_wndTreeFltTimeRange.DeleteAllItems();
	m_wndTreeFltTimeRange.SetRedraw(FALSE);
	m_wndTreeFltTimeRange.EnableWindow(FALSE);
	m_wndTreeFltTimeRange.SetRedraw(TRUE);
	*pResult = 0;
}

void CDlgRunwayTakeoffPosition::InitTimeRangeFltType()
{
	m_wndTreeFltTimeRange.EnableWindow(TRUE);
	m_wndTreeFltTimeRange.DeleteAllItems();
	m_wndTreeFltTimeRange.SetRedraw(FALSE);
	HTREEITEM hItem;
	hItem = m_wndTreeIntersection.GetSelectedItem();
	CRunwayTakeOffPositionData* IntData = (CRunwayTakeOffPositionData*)m_wndTreeIntersection.GetItemData(hItem);
	
	UpdateData(FALSE);

	std::vector<IntersectionNode> lstIntersecNode;
	lstIntersecNode.clear();
	IntersectionNode::ReadIntersectNodeList(IntData->GetRunwayExitDescription().GetRunwayID(),IntData->GetRunwayExitDescription().GetTaxiwayID(),lstIntersecNode);

	int nCount = (int)lstIntersecNode.size();
	if ( nCount == 0)
	{
		return;
	}
	CPoint2008 posTaxiInt;
	for (int nIdx = 0; nIdx < nCount; nIdx++)
	{
		IntersectionNode& node = lstIntersecNode.at(nIdx);
		if (node.GetIndex() == IntData->GetRunwayExitDescription().GetIndex())
		{
			posTaxiInt = node.GetPosition();
			break;
		}
	}
	 

	HTREEITEM hRunMarkItem;
	hRunMarkItem = m_wndTreeRunway.GetSelectedItem();
	CRunwayTakeOffRunwayWithMarkData* runwayMarkData = (CRunwayTakeOffRunwayWithMarkData*)m_wndTreeRunway.GetItemData(hRunMarkItem);

	Runway altRunway;
	altRunway.ReadObject(runwayMarkData->getRunwayID());

	CPoint2008 posRunMark = altRunway.getPath().GetIndexPoint((float)(runwayMarkData->getRunwayMarkIndex()));
	DistanceUnit rwLength = altRunway.getPath().GetTotalLength();
	
	double distance = rwLength- posRunMark.distance(posTaxiInt);
	m_nRemainFt = distance/100.0;
	
	CString strRemainFt;
	strRemainFt.Format("%.2f",m_nRemainFt);
	m_CtrlRemainFt.SetWindowText(strRemainFt);
	
	for (int i=0; i<IntData->GetTimeCount(); i++)
	{
		CRunwayTakeOffTimeRangeData* pTimeItem = IntData->GetTimeItem(i);
		CString strTimeRange = _T("");

		ElapsedTime etInsert = pTimeItem->GetStartTime();
		strTimeRange = etInsert.PrintDateTime() + _T(" - ");

		etInsert = pTimeItem->GetEndTime();
		strTimeRange += etInsert.PrintDateTime();

		HTREEITEM hTimeTreeItem = m_wndTreeFltTimeRange.InsertItem(strTimeRange);
		m_wndTreeFltTimeRange.SetItemData(hTimeTreeItem, (DWORD_PTR)pTimeItem);

		for (int j=0; j<pTimeItem->GetFltTypeCount(); j++)
		{
			CRunwayTakeOffTimeRangeFltType* pFltTypeItem = pTimeItem->GetFltTypeItem(j);
			AddFlightTypeToTree(pFltTypeItem,hTimeTreeItem);
		}

		m_wndTreeFltTimeRange.Expand(hTimeTreeItem, TVE_EXPAND);
		m_wndTreeFltTimeRange.SetRedraw(TRUE);
	}
}

void CDlgRunwayTakeoffPosition::OnTvnSelchangedTreeTakeoffPosition(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_wndTreeIntersection.GetSelectedItem();
	if (hItem == NULL)
	{
		*pResult = 0;
		return;
	}

	CRunwayTakeOffPositionData* pItem = (CRunwayTakeOffPositionData*)m_wndTreeIntersection.GetItemData(hItem);
	GetDlgItem(IDC_CHECK_ELIGIBLE)->EnableWindow(TRUE);
	if (pItem && pItem->GetEligible())
	{
		m_bEligible.SetCheck(TRUE);
		InitTimeRangeFltType();
		CToolBarCtrl& wndToolbar = m_wndToolbar.GetToolBarCtrl();
		wndToolbar.EnableButton(ID_NEW_FLTTIMERANGE,TRUE);
		wndToolbar.EnableButton(ID_DEL_FLTTIMERANGE,FALSE);
		wndToolbar.EnableButton(ID_EDIT_FLTTIMERANGE,FALSE);
	}
	else
	{
		m_bEligible.SetCheck(FALSE);
		m_wndTreeFltTimeRange.EnableWindow(FALSE);
		m_wndTreeFltTimeRange.DeleteAllItems();

		CToolBarCtrl& wndToolbar = m_wndToolbar.GetToolBarCtrl();
		wndToolbar.EnableButton(ID_NEW_FLTTIMERANGE,FALSE);
		wndToolbar.EnableButton(ID_DEL_FLTTIMERANGE,FALSE);
		wndToolbar.EnableButton(ID_EDIT_FLTTIMERANGE,FALSE);		
	}
	*pResult = 0;
}

void CDlgRunwayTakeoffPosition::OnBnClickedCheckEligible()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	HTREEITEM hInterTreeItem = m_wndTreeIntersection.GetSelectedItem();
	CRunwayTakeOffPositionData* pItem = (CRunwayTakeOffPositionData*)m_wndTreeIntersection.GetItemData(hInterTreeItem);


	if (m_bEligible.GetCheck())
	{	
		pItem->SetEligible(TRUE);
		InitTimeRangeFltType();
		CToolBarCtrl& wndToolbar = m_wndToolbar.GetToolBarCtrl();
		wndToolbar.EnableButton(ID_NEW_FLTTIMERANGE,TRUE);
		wndToolbar.EnableButton(ID_DEL_FLTTIMERANGE,FALSE);
		wndToolbar.EnableButton(ID_EDIT_FLTTIMERANGE,FALSE);
	}
	else
	{
		pItem->SetEligible(FALSE);
		m_wndTreeFltTimeRange.EnableWindow(FALSE);
		m_wndTreeFltTimeRange.DeleteAllItems();

		CToolBarCtrl& wndToolbar = m_wndToolbar.GetToolBarCtrl();
		wndToolbar.EnableButton(ID_NEW_FLTTIMERANGE,FALSE);
		wndToolbar.EnableButton(ID_DEL_FLTTIMERANGE,FALSE);
		wndToolbar.EnableButton(ID_EDIT_FLTTIMERANGE,FALSE);
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgRunwayTakeoffPosition::OnEnChangeEditRemaining()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if(init == 1)
	{
		HTREEITEM hInterTreeItem = m_wndTreeIntersection.GetSelectedItem();
		// if item selected and its has a parent item
		if(hInterTreeItem && m_wndTreeIntersection.GetParentItem(hInterTreeItem))
		{
			UpdateData();
			CRunwayTakeOffPositionData* pItem = (CRunwayTakeOffPositionData*)m_wndTreeIntersection.GetItemData(hInterTreeItem);
			ASSERT(pItem);
			pItem->SetRemainFt(m_nRemainFt);
			GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		}
	}

}

void CDlgRunwayTakeoffPosition::AddFlightTypeToTree( CRunwayTakeOffTimeRangeFltType* pFltTypeItem,HTREEITEM hParentItem )
{
	FlightConstraint fltType = pFltTypeItem->GetFltType();
	CString strFltType;
	fltType.screenPrint(strFltType);

	HTREEITEM hFltTypeTreeItem = m_wndTreeFltTimeRange.InsertItem(strFltType, hParentItem, TVI_LAST);
	m_wndTreeFltTimeRange.SetItemData(hFltTypeTreeItem, (DWORD_PTR)pFltTypeItem);

	CRunwayTakeOffTimeRangeFltType::BACKTRACK eBacktrack = pFltTypeItem->getBacktrakOp();
	CString strData;
	if (eBacktrack == CRunwayTakeOffTimeRangeFltType::MaxBacktrack)
	{
		strData = _T("Max Backtrack Operation");
	}
	else if (eBacktrack == CRunwayTakeOffTimeRangeFltType::MinBacktrack)
	{
		strData = _T("Min Backtrack Operation");
	}
	else
	{
		strData = _T("No Backtrack Operation");
	}

	HTREEITEM hBacktrackItem = m_wndTreeFltTimeRange.InsertItem(strData, hFltTypeTreeItem, TVI_LAST);
	m_wndTreeFltTimeRange.Expand(hFltTypeTreeItem, TVE_EXPAND);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}