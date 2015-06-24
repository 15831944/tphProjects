// DlgArrivalDelayTriggers.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgArrivalDelayTriggers.h"
#include ".\dlgarrivaldelaytriggers.h"

#include "FlightDialog.h"
#include "../InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"
#include "common\WINMSG.h"
#include "../Common/elaptime.h"
#include "../Common//AirportDatabase.h"
#include "../engine/terminal.h"
#include "../AirsideGUI/DlgSelectRunway.h"
#include "DlgTimeRange.h"
#include "DlgRunwayExitSelect.h"

namespace
{
	const UINT ID_NEW_ARRIVALDELAYTRIGGERLIST = 40;
	const UINT ID_DEL_ARRIVALDELAYTRIGGERLIST = 41;
	const UINT ID_EDIT_ARRIVALDELAYTRIGGERLIST = 42;
}

// CDlgArrivalDelayTriggers dialog

IMPLEMENT_DYNAMIC(CDlgArrivalDelayTriggers, CXTResizeDialog)
CDlgArrivalDelayTriggers::CDlgArrivalDelayTriggers(int nProjID, Terminal* pTerminal, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgArrivalDelayTriggers::IDD, pParent)
	,m_pTerminal(pTerminal)
	,m_arrivalDelayTrigger(pTerminal->m_pAirportDB)
{
	m_nProjID = nProjID;
//	m_pTerminal = pTerminal;
//	m_arrivalDelayTrigger.setAirportDB( m_pTerminal->m_pAirportDB );
}

CDlgArrivalDelayTriggers::~CDlgArrivalDelayTriggers()
{
}

void CDlgArrivalDelayTriggers::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TREE_CONTENT, m_treeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgArrivalDelayTriggers, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_ARRIVALDELAYTRIGGERLIST, OnNewOperation)
	ON_COMMAND(ID_DEL_ARRIVALDELAYTRIGGERLIST, OnDelOperation)
	ON_COMMAND(ID_EDIT_ARRIVALDELAYTRIGGERLIST, OnEditOperation)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CONTENT, OnTvnSelchangedTreeData)
	ON_MESSAGE(WM_OBJECTTREE_DOUBLECLICK, OnTreeDoubleClick)

END_MESSAGE_MAP()


// CDlgArrivalDelayTriggers message handlers

int CDlgArrivalDelayTriggers::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here	
	if(m_wndToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0))&&
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR_ARRIVALDETAILTRIGGER))
	{
		AfxMessageBox("Cann't create toolbar!");
		return FALSE;
	}

	m_wndToolBar.GetToolBarCtrl().SetCmdID(0, ID_NEW_ARRIVALDELAYTRIGGERLIST);
	m_wndToolBar.GetToolBarCtrl().SetCmdID(1, ID_DEL_ARRIVALDELAYTRIGGERLIST);
	m_wndToolBar.GetToolBarCtrl().SetCmdID(2, ID_EDIT_ARRIVALDELAYTRIGGERLIST);
	return 0;
}

BOOL CDlgArrivalDelayTriggers::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	InitToolbar();

	SetResize(m_wndToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_STATIC_FRAME,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_TREE_CONTENT,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	GetRunway();
	LoadArriDelayTriDatabase();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgArrivalDelayTriggers::InitToolbar()
{
	CRect rcToolbar;
	// set the position of the arrival delay trigger list toolbar	
	m_treeCtrl.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.top -= 23;
	rcToolbar.bottom = rcToolbar.top + 22;
	rcToolbar.left += 2;
	m_wndToolBar.MoveWindow(rcToolbar);

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_ARRIVALDELAYTRIGGERLIST);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ARRIVALDELAYTRIGGERLIST);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_ARRIVALDELAYTRIGGERLIST);


}

void CDlgArrivalDelayTriggers::GetRunway()
{
	int firstIndex = 0;
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		
		ALTObject::GetObjectList(ALT_RUNWAY,*iterAirportID,m_vRunways);

	}

}

void CDlgArrivalDelayTriggers::LoadArriDelayTriDatabase()
{
	try
	{
		m_arrivalDelayTrigger.ReadData();


		for (int nItem = 0; nItem < static_cast<int>(m_arrivalDelayTrigger.GetElementCount()); ++nItem)
		{
			AirsideArrivalDelayTrigger::CFlightTypeItem *pItem = m_arrivalDelayTrigger.GetItem(nItem);
			LoadFlightTypeItem(pItem);
		}
		if(m_treeCtrl.GetRootItem())
			m_treeCtrl.Expand(m_treeCtrl.GetRootItem(),TVE_EXPAND);

	}
	catch (CADOException& e)
	{
		e.ErrorMessage();
				
	}
}


void CDlgArrivalDelayTriggers::OnEditOperation()
{
	HTREEITEM hItemSelected = m_treeCtrl.GetSelectedItem();
	if(hItemSelected == NULL)
		return;

	CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hItemSelected);
	if(pItemDataEx == NULL)
		return;

	CTreeItemData *pItemData  = (CTreeItemData *)pItemDataEx->dwptrItemData;
	if(pItemData == NULL)
		return;

	if(pItemData->m_enumItemType == ItemType_FlightType)//one 
	{
		//pop up flight type definition dialog
		CFlightDialog flightDlg(NULL, true);
		if (IDOK != flightDlg.DoModal())
			return;

		FlightConstraint fltType = flightDlg.GetFlightSelection();
		fltType.SetAirportDB( m_pTerminal->m_pAirportDB );

		if(pItemData->m_FlightTypeItem)
			pItemData->m_FlightTypeItem->SetFlightConstraint(fltType);

		UpdateItemText(hItemSelected);
	}
	else if(pItemData->m_enumItemType == ItemType_TimeItem) //time
	{
		if(pItemData->m_FlightTypeItem && pItemData->m_timeRange)
		{
			
			CDlgTimeRange dlg(pItemData->m_timeRange->GetStartTime(), pItemData->m_timeRange->GetEndTime());
			if(dlg.DoModal() == IDOK)
			{
				pItemData->m_timeRange->SetStartTime(dlg.GetStartTime());
				pItemData->m_timeRange->SetEndTime(dlg.GetEndTime());

				UpdateItemText(hItemSelected);
			}
		}

	}
	else if(pItemData->m_enumItemType == ItemType_RunwayItem)// runway
	{
		if(pItemData->m_FlightTypeItem )
		{

		}
	}
	else if(pItemData->m_enumItemType == ItemType_RunwayExit)//delete trigger condition
	{
		if(pItemData->m_FlightTypeItem)
		{
			CDlgRunwayExitSelect dlg(&m_vRunways,true,this);
			if(dlg.DoModal() == IDOK)
			{
				std::vector<CSelectData>* vSelectData = dlg.GetSelectExitName();
				if(vSelectData->size() > 0)
				{
					CSelectData runwayExit = vSelectData->at(0);
					if(pItemData->m_pTriggerCondition)
						pItemData->m_pTriggerCondition->SetTakeOffPosition(runwayExit.m_ExitID);
					UpdateItemText(hItemSelected);
				}
			}
		}
	}
	else if(pItemData->m_enumItemType == ItemType_QueueLength)
	{
		if(pItemData->m_pTriggerCondition == NULL)
		{
			int nQueueLength =pItemData->m_pTriggerCondition->GetQueueLength();
			CString strQueueLength ;
			strQueueLength.Format(_T("%d"),nQueueLength);
			m_treeCtrl.SetDisplayNum(nQueueLength);
			m_treeCtrl.SetDisplayType(2);
			m_treeCtrl.SetSpinRange(0,10000);
			m_treeCtrl.SpinEditLabel(hItemSelected,strQueueLength);
		}


	}
	else if(pItemData->m_enumItemType == ItemType_MinsPerAircraft)
	{
		if(pItemData->m_pTriggerCondition == NULL)
		{
			double nMins =pItemData->m_pTriggerCondition->GetMinsPerAircraft();
			CString strMins ;
			strMins.Format(_T("%0f"),nMins);
			m_treeCtrl.SetDisplayNum(static_cast<int>(nMins));
			m_treeCtrl.SetDisplayType(2);
			m_treeCtrl.SetSpinRange(0,10000);
			m_treeCtrl.SpinEditLabel(hItemSelected,strMins);
		}
	}
}

// about arrival delay trigger functions
void CDlgArrivalDelayTriggers::OnNewOperation()
{

	HTREEITEM hItemSelected = m_treeCtrl.GetSelectedItem();
	if(hItemSelected == NULL)
	{
		//pop up flight type definition dialog
		CFlightDialog flightDlg(NULL, true);
		if (IDOK != flightDlg.DoModal())
			return;

		FlightConstraint fltType = flightDlg.GetFlightSelection();
		fltType.SetAirportDB( m_pTerminal->m_pAirportDB );


		AirsideArrivalDelayTrigger::CFlightTypeItem *pFlightItem = new AirsideArrivalDelayTrigger::CFlightTypeItem;
		pFlightItem->SetFlightConstraint(fltType);
		m_arrivalDelayTrigger.AddNewItem(pFlightItem);

		LoadFlightTypeItem(pFlightItem);
		return;
	}

	CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hItemSelected);
	if(pItemDataEx == NULL)
		return;

	CTreeItemData *pItemData  = (CTreeItemData *)pItemDataEx->dwptrItemData;
	if(pItemData == NULL)
		return;

	//ItemType_FlightType = 0,
	//	ItemType_TimeRoot,
	//	ItemType_TimeItem,
	//	ItemType_RunwayRoot,
	//	ItemType_RunwayItem,
	//	ItemType_TriggerRoot,
	//	ItemType_RunwayExit,
	//	ItemType_QueueLength,
	//	ItemType_MinsPerAircraft,

	if(pItemData->m_enumItemType == ItemType_FlightType)
	{
		//pop up flight type definition dialog
		CFlightDialog flightDlg(NULL, true);
		if (IDOK != flightDlg.DoModal())
			return;

		FlightConstraint fltType = flightDlg.GetFlightSelection();
		fltType.SetAirportDB( m_pTerminal->m_pAirportDB );


		AirsideArrivalDelayTrigger::CFlightTypeItem *pFlightItem = new AirsideArrivalDelayTrigger::CFlightTypeItem;
		pFlightItem->SetFlightConstraint(fltType);
		m_arrivalDelayTrigger.AddNewItem(pFlightItem);

		LoadFlightTypeItem(pFlightItem);
	}
	else if(pItemData->m_enumItemType == ItemType_TimeRoot)
	{
		ElapsedTime etStart, etEnd;
		CDlgTimeRange dlg(etStart, etEnd);
		if(dlg.DoModal() == IDOK)
		{
			AirsideArrivalDelayTrigger::CTimeRange* timeRange = new AirsideArrivalDelayTrigger::CTimeRange;
			timeRange->SetStartTime(dlg.GetStartTime());
			timeRange->SetEndTime(dlg.GetEndTime());

			if(pItemData->m_FlightTypeItem)
				pItemData->m_FlightTypeItem->AddItem(timeRange);

			LoadDateTimeItem(pItemData->m_FlightTypeItem,timeRange,hItemSelected);
		}
	}
	else if(pItemData->m_enumItemType == ItemType_RunwayRoot)
	{
		CDlgSelectRunway dlgRunway(m_nProjID,-1,this);
		if(dlgRunway.DoModal() == IDOK)
		{
			int nRunwayID = dlgRunway.GetSelectedRunway();
			int nRunwayMark = dlgRunway.GetSelectedRunwayMarkingIndex();

			if(pItemData->m_FlightTypeItem)
			{
				AirsideArrivalDelayTrigger::CRunwayMark *pRunwayMark = new AirsideArrivalDelayTrigger::CRunwayMark;
				pRunwayMark->SetRunwayMark(nRunwayID,(RUNWAY_MARK)nRunwayMark);
				pItemData->m_FlightTypeItem->AddItem(pRunwayMark);

				LoadLandingRunwayItem(pItemData->m_FlightTypeItem,pRunwayMark,hItemSelected);
			}
		}


	}
	else if(pItemData->m_enumItemType == ItemType_TriggerRoot)
	{
		CDlgRunwayExitSelect dlg(&m_vRunways,true,this);
		if(dlg.DoModal() == IDOK)
		{
			std::vector<CSelectData>* vSelectData = dlg.GetSelectExitName();
			if(vSelectData->size() > 0)
			{
				CSelectData runwayExit = vSelectData->at(0);
				AirsideArrivalDelayTrigger::CTriggerCondition *pTriggerCondition = new AirsideArrivalDelayTrigger::CTriggerCondition;
				pTriggerCondition->SetTakeOffPosition(runwayExit.m_ExitID);
                
				if(pItemData->m_FlightTypeItem)
				{
					pItemData->m_FlightTypeItem->AddItem(pTriggerCondition);

					LoadTriggerConditionItem(pItemData->m_FlightTypeItem,pTriggerCondition,hItemSelected);

				}
			}
		}
	}
}

void CDlgArrivalDelayTriggers::OnDelOperation()
{
	HTREEITEM hItemSelected = m_treeCtrl.GetSelectedItem();
	if(hItemSelected == NULL)
		return;

	CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hItemSelected);
	if(pItemDataEx == NULL)
		return;

	CTreeItemData *pItemData  = (CTreeItemData *)pItemDataEx->dwptrItemData;
	if(pItemData == NULL)
		return;

	if(pItemData->m_enumItemType == ItemType_FlightType)//one 
	{
		if(pItemData->m_FlightTypeItem)
		{
			m_arrivalDelayTrigger.DeleteItem(pItemData->m_FlightTypeItem);
			m_treeCtrl.DeleteItem(hItemSelected);
		}
		
	}
	else if(pItemData->m_enumItemType == ItemType_TimeItem) //time
	{
		if(pItemData->m_FlightTypeItem)
		{
			pItemData->m_FlightTypeItem->DelItem(pItemData->m_timeRange);
			m_treeCtrl.DeleteItem(hItemSelected);
		}

	}
	else if(pItemData->m_enumItemType == ItemType_RunwayItem)// runway
	{
		if(pItemData->m_FlightTypeItem)
		{
			pItemData->m_FlightTypeItem->DelItem(pItemData->m_runwayMark);
			m_treeCtrl.DeleteItem(hItemSelected);
		}
	}
	else if(pItemData->m_enumItemType == ItemType_RunwayExit)//delete trigger condition
	{
		if(pItemData->m_FlightTypeItem)
		{
			pItemData->m_FlightTypeItem->DelItem(pItemData->m_pTriggerCondition);
			m_treeCtrl.DeleteItem(hItemSelected);
		}
	}
	


}

void CDlgArrivalDelayTriggers::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction();
		m_arrivalDelayTrigger.SaveData( m_nProjID );
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
}

void CDlgArrivalDelayTriggers::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	try
	{
		CADODatabase::BeginTransaction();
		m_arrivalDelayTrigger.SaveData( m_nProjID );
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	OnOK();
}

void CDlgArrivalDelayTriggers::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CDlgArrivalDelayTriggers::LoadFlightTypeItem( AirsideArrivalDelayTrigger::CFlightTypeItem *pFlightItem )
{
	ASSERT(pFlightItem != NULL);
	if(pFlightItem == NULL)
		return;


	CARCTreeCtrlExWithColor::CItemData colorItemData;
	CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
	colorItemData.lSize = sizeof(colorItemData);
	colorStringSection.colorSection = RGB(0,0,255);

	HTREEITEM hItemName = NULL;
	//load flight type
	{
		CTreeItemData *pTreeItemData = new CTreeItemData(ItemType_FlightType,pFlightItem);
		colorItemData.dwptrItemData = (DWORD_PTR)pTreeItemData;
		colorItemData.vrItemStringSectionColorShow.clear();
		colorStringSection.strSection = pFlightItem->GetFlightTypeName();
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);
		CString strNodeText;
		strNodeText.Format(_T("Flight Type to be delayed: %s"),pFlightItem->GetFlightTypeName());

		hItemName = m_treeCtrl.InsertItem(strNodeText,TVI_ROOT);
		m_treeCtrl.SetItemDataEx(hItemName,colorItemData);
	}

	//date time
	{	
		HTREEITEM hItemDateTimeRoot = NULL;
		CTreeItemData *pTreeItemData = new CTreeItemData(ItemType_TimeRoot,pFlightItem);
		colorItemData.dwptrItemData = (DWORD_PTR)pTreeItemData;
		colorItemData.vrItemStringSectionColorShow.clear();
		//colorStringSection.strSection = pFlightItem->GetFlightTypeName();
		//colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);
		CString strNodeText;
		strNodeText.Format(_T("From date time to date time"));

		hItemDateTimeRoot = m_treeCtrl.InsertItem(strNodeText,hItemName);
		m_treeCtrl.SetItemDataEx(hItemDateTimeRoot,colorItemData);

		//load items
		LoadDateTime(pFlightItem,hItemDateTimeRoot);
	}
	//landing on runways
	{
		HTREEITEM hItemRunwayRoot = NULL;
		CTreeItemData *pTreeItemData = new CTreeItemData(ItemType_RunwayRoot,pFlightItem);
		colorItemData.dwptrItemData = (DWORD_PTR)pTreeItemData;
		colorItemData.vrItemStringSectionColorShow.clear();
		//colorStringSection.strSection = pFlightItem->GetFlightTypeName();
		//colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);
		CString strNodeText;
		strNodeText.Format(_T("Landing on runways"));

		hItemRunwayRoot = m_treeCtrl.InsertItem(strNodeText,hItemName);
		m_treeCtrl.SetItemDataEx(hItemRunwayRoot,colorItemData);

		//load items
		LoadLandingRunway(pFlightItem,hItemRunwayRoot);
	}

	//trigger condition
	{
		HTREEITEM hItemTriggerConditionRoot = NULL;
		CTreeItemData *pTreeItemData = new CTreeItemData(ItemType_TriggerRoot,pFlightItem);
		colorItemData.dwptrItemData = (DWORD_PTR)pTreeItemData;
		colorItemData.vrItemStringSectionColorShow.clear();
		//colorStringSection.strSection = pFlightItem->GetFlightTypeName();
		//colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);
		CString strNodeText;
		strNodeText.Format(_T("Triggered by queues on take off positions"));

		hItemTriggerConditionRoot = m_treeCtrl.InsertItem(strNodeText,hItemName);
		m_treeCtrl.SetItemDataEx(hItemTriggerConditionRoot,colorItemData);

		//load items
		LoadTriggerCondition(pFlightItem,hItemTriggerConditionRoot);
	}
	m_treeCtrl.Expand(hItemName,TVE_EXPAND);

}

void CDlgArrivalDelayTriggers::LoadDateTime( AirsideArrivalDelayTrigger::CFlightTypeItem *pFlightItem, HTREEITEM hRootItem )
{
	ASSERT(pFlightItem != NULL);
	if(pFlightItem == NULL)
		return;

	AirsideArrivalDelayTrigger::CTimeRangeList& vTimeRange = pFlightItem->GetTimeRangeList();
	for (int nTime = 0; nTime < static_cast<int>(vTimeRange.GetElementCount()); ++nTime)
	{
		AirsideArrivalDelayTrigger::CTimeRange* timeRange = vTimeRange.GetItem(nTime);

		LoadDateTimeItem(pFlightItem,timeRange,hRootItem);
	}


}
void CDlgArrivalDelayTriggers::LoadDateTimeItem( AirsideArrivalDelayTrigger::CFlightTypeItem *pFlightItem, AirsideArrivalDelayTrigger::CTimeRange *timeRange, HTREEITEM hRootItem)
{
	CARCTreeCtrlExWithColor::CItemData colorItemData;
	CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
	colorItemData.lSize = sizeof(colorItemData);
	colorStringSection.colorSection = RGB(0,0,255);

	HTREEITEM hItemTriggerConditionRoot = NULL;
	CTreeItemData *pTreeItemData = new CTreeItemData(ItemType_TimeItem,pFlightItem);
	pTreeItemData->m_timeRange = timeRange;

	colorItemData.dwptrItemData = (DWORD_PTR)pTreeItemData;
	colorItemData.vrItemStringSectionColorShow.clear();

	CString strStartTime = timeRange->GetStartTime().PrintDateTime();
	CString strEndTime = timeRange->GetEndTime().PrintDateTime();
	colorStringSection.strSection = strStartTime;
	colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

	colorStringSection.strSection = strEndTime;
	colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

	CString strNodeText;
	strNodeText.Format(_T(" %s - %s"),strStartTime ,strEndTime);

	hItemTriggerConditionRoot = m_treeCtrl.InsertItem(strNodeText,hRootItem);
	m_treeCtrl.SetItemDataEx(hItemTriggerConditionRoot,colorItemData);
	m_treeCtrl.Expand(hRootItem,TVE_EXPAND);


}

void CDlgArrivalDelayTriggers::LoadLandingRunway( AirsideArrivalDelayTrigger::CFlightTypeItem *pFlightItem, HTREEITEM hRootItem )
{
	ASSERT(pFlightItem != NULL);
	if(pFlightItem == NULL)
		return;

	AirsideArrivalDelayTrigger::CRunwayMarkList& vRunway = pFlightItem->GetRunwayList();
	for (int nRunway = 0; nRunway < static_cast<int>(vRunway.GetElementCount()); ++nRunway)
	{
		AirsideArrivalDelayTrigger::CRunwayMark* pRunway = vRunway.GetItem(nRunway);
		if(pRunway == NULL)
			continue;

		LoadLandingRunwayItem(pFlightItem,pRunway,hRootItem);

	}
}

void CDlgArrivalDelayTriggers::LoadTriggerCondition( AirsideArrivalDelayTrigger::CFlightTypeItem *pFlightItem, HTREEITEM hRootItem )
{
	ASSERT(pFlightItem != NULL);
	if(pFlightItem == NULL)
		return;

	AirsideArrivalDelayTrigger::CTriggerConditionList& vConditions = pFlightItem->GetTriggerConditionList();
	for (int nCondition = 0; nCondition < static_cast<int>(vConditions.GetElementCount()); ++nCondition)
	{
		AirsideArrivalDelayTrigger::CTriggerCondition* pCondition = vConditions.GetItem(nCondition);
		if(pCondition == NULL)
			continue;
		
		LoadTriggerConditionItem(pFlightItem,pCondition,hRootItem);
	}
}

void CDlgArrivalDelayTriggers::UpdateToolbarState()
{
	HTREEITEM hItemSelected = m_treeCtrl.GetSelectedItem();
	if(hItemSelected == NULL)
	{
		//only "New" available
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_ARRIVALDELAYTRIGGERLIST,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ARRIVALDELAYTRIGGERLIST,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_ARRIVALDELAYTRIGGERLIST,FALSE);
		return;
	}
	
	
	CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hItemSelected);
	if(pItemDataEx == NULL)
	{
		//only "New" available
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_ARRIVALDELAYTRIGGERLIST,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ARRIVALDELAYTRIGGERLIST,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_ARRIVALDELAYTRIGGERLIST,FALSE);
		return;
	}

	CTreeItemData *pItemData  = (CTreeItemData *)pItemDataEx->dwptrItemData;
	if(pItemData == NULL)
	{
		//only "New" available
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_ARRIVALDELAYTRIGGERLIST,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ARRIVALDELAYTRIGGERLIST,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_ARRIVALDELAYTRIGGERLIST,FALSE);
		return;
	}

	//ItemType_FlightType = 0,
	//	ItemType_TimeRoot,
	//	ItemType_TimeItem,
	//	ItemType_RunwayRoot,
	//	ItemType_RunwayItem,
	//	ItemType_TriggerRoot,
	//	ItemType_RunwayExit,
	//	ItemType_QueueLength,
	//	ItemType_MinsPerAircraft,
	if(pItemData->m_enumItemType == ItemType_FlightType)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_ARRIVALDELAYTRIGGERLIST,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ARRIVALDELAYTRIGGERLIST,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_ARRIVALDELAYTRIGGERLIST,TRUE);
	}
	else if(pItemData->m_enumItemType == ItemType_TimeRoot ||
		pItemData->m_enumItemType == ItemType_RunwayRoot ||
		pItemData->m_enumItemType == ItemType_TriggerRoot)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_ARRIVALDELAYTRIGGERLIST,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ARRIVALDELAYTRIGGERLIST,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_ARRIVALDELAYTRIGGERLIST,FALSE);

	}
	else if(pItemData->m_enumItemType == ItemType_TimeItem ||
		pItemData->m_enumItemType == ItemType_RunwayExit)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_ARRIVALDELAYTRIGGERLIST,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ARRIVALDELAYTRIGGERLIST,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_ARRIVALDELAYTRIGGERLIST,TRUE);
	}
	else if(pItemData->m_enumItemType == ItemType_RunwayItem )
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_ARRIVALDELAYTRIGGERLIST,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ARRIVALDELAYTRIGGERLIST,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_ARRIVALDELAYTRIGGERLIST,FALSE);
	}

	else if(pItemData->m_enumItemType == ItemType_QueueLength||
		pItemData->m_enumItemType == ItemType_MinsPerAircraft)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_ARRIVALDELAYTRIGGERLIST,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ARRIVALDELAYTRIGGERLIST,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_ARRIVALDELAYTRIGGERLIST,TRUE);
	}
}

void CDlgArrivalDelayTriggers::LoadLandingRunwayItem( AirsideArrivalDelayTrigger::CFlightTypeItem *pFlightItem,AirsideArrivalDelayTrigger::CRunwayMark *pRunway, HTREEITEM hRootItem )
{

	CARCTreeCtrlExWithColor::CItemData colorItemData;
	CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
	colorItemData.lSize = sizeof(colorItemData);
	colorStringSection.colorSection = RGB(0,0,255);

	HTREEITEM hItemTriggerConditionRoot = NULL;
	CTreeItemData *pTreeItemData = new CTreeItemData(ItemType_RunwayItem,pFlightItem);
	pTreeItemData->m_runwayMark = pRunway;

	colorItemData.dwptrItemData = (DWORD_PTR)pTreeItemData;
	colorItemData.vrItemStringSectionColorShow.clear();

	CString strRunwayName = pRunway->GetRunwayMark();

	colorStringSection.strSection = strRunwayName;
	colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

	CString strNodeText;
	strNodeText.Format(_T("%s"),strRunwayName);

	hItemTriggerConditionRoot = m_treeCtrl.InsertItem(strNodeText,hRootItem);
	m_treeCtrl.SetItemDataEx(hItemTriggerConditionRoot,colorItemData);
	m_treeCtrl.Expand(hRootItem,TVE_EXPAND);
}

void CDlgArrivalDelayTriggers::LoadTriggerConditionItem( AirsideArrivalDelayTrigger::CFlightTypeItem *pFlightItem, AirsideArrivalDelayTrigger::CTriggerCondition* pCondition, HTREEITEM hRootItem )
{

	CARCTreeCtrlExWithColor::CItemData colorItemData;
	CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
	colorItemData.lSize = sizeof(colorItemData);
	colorStringSection.colorSection = RGB(0,0,255);


	HTREEITEM hItemTriggerConditionRoot = NULL;
	CTreeItemData *pTreeItemData = new CTreeItemData(ItemType_RunwayExit,pFlightItem);
	pTreeItemData->m_pTriggerCondition = pCondition;

	colorItemData.dwptrItemData = (DWORD_PTR)pTreeItemData;
	colorItemData.vrItemStringSectionColorShow.clear();

	CString strRunwayExitName = pCondition->GetTakeOffPosition()->GetName();

	CString strQueueLength;
	strQueueLength.Format(_T("%d"),pCondition->GetQueueLength());

	CString strMinsPerAircraft;
	strMinsPerAircraft.Format(_T("%.0f"),pCondition->GetMinsPerAircraft());

	colorStringSection.strSection = strRunwayExitName;
	colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

	colorStringSection.strSection = strQueueLength;
	colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

	colorStringSection.strSection = strMinsPerAircraft;
	colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

	CString strNodeText;
	strNodeText.Format(_T("%s when Queue Length > %s aircraft by %s mins/aircraft"),strRunwayExitName,strQueueLength,strMinsPerAircraft);

	hItemTriggerConditionRoot = m_treeCtrl.InsertItem(strNodeText,hRootItem);
	m_treeCtrl.SetItemDataEx(hItemTriggerConditionRoot,colorItemData);

	//add queue length, delay times
	{
		CTreeItemData *pQueueLenItemData = new CTreeItemData(ItemType_QueueLength,pFlightItem);
		pQueueLenItemData->m_pTriggerCondition = pCondition;
		colorItemData.dwptrItemData = (DWORD_PTR)pQueueLenItemData;
		colorItemData.vrItemStringSectionColorShow.clear();
		CString strQueueLength;
		strQueueLength.Format(_T("%d"),pCondition->GetQueueLength());
		colorStringSection.strSection = strQueueLength;
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

		CString strNodeText;
		strNodeText.Format(_T("Queue Length: %s"),strQueueLength);
		HTREEITEM hItemQueueLength = m_treeCtrl.InsertItem(strNodeText,hItemTriggerConditionRoot);
		m_treeCtrl.SetItemDataEx(hItemQueueLength,colorItemData);

	}

	{
		CTreeItemData *pMinsItemData = new CTreeItemData(ItemType_MinsPerAircraft,pFlightItem);
		pMinsItemData->m_pTriggerCondition = pCondition;

		colorItemData.dwptrItemData = (DWORD_PTR)pMinsItemData;
		colorItemData.vrItemStringSectionColorShow.clear();
		CString strMinsPerAC;
		strMinsPerAC.Format(_T("%.0f"),pCondition->GetMinsPerAircraft());
		colorStringSection.strSection = strMinsPerAC;
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

		CString strNodeText;
		strNodeText.Format(_T("Mins: %s"),strMinsPerAC);
		HTREEITEM hItemMins = m_treeCtrl.InsertItem(strNodeText,hItemTriggerConditionRoot);
		m_treeCtrl.SetItemDataEx(hItemMins,colorItemData);

	}
	m_treeCtrl.Expand(hRootItem,TVE_EXPAND);
	m_treeCtrl.Expand(hItemTriggerConditionRoot,TVE_EXPAND);

}

void CDlgArrivalDelayTriggers::OnTvnSelchangedTreeData( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	UpdateToolbarState();
	*pResult = 0;
}
//ItemType_FlightType = 0,
//	ItemType_TimeRoot,
//	ItemType_TimeItem,
//	ItemType_RunwayRoot,
//	ItemType_RunwayItem,
//	ItemType_TriggerRoot,
//	ItemType_RunwayExit,
//	ItemType_QueueLength,
//	ItemType_MinsPerAircraft,
void CDlgArrivalDelayTriggers::UpdateItemText( HTREEITEM hItemUpdate)
{
	if(hItemUpdate == NULL)
		return;
	CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hItemUpdate);
	if(pItemDataEx == NULL)
		return;

	CTreeItemData *pItemData  = (CTreeItemData *)pItemDataEx->dwptrItemData;
	if(pItemData == NULL)
		return;


	CARCTreeCtrlExWithColor::CItemData colorItemData;
	CARCTreeCtrlExWithColor::CColorStringSection colorStringSection;
	colorItemData.lSize = sizeof(colorItemData);
	colorStringSection.colorSection = RGB(0,0,255);


	if(pItemData->m_enumItemType == ItemType_FlightType)
	{
		if(pItemData->m_FlightTypeItem == NULL)
			return;

		colorItemData.vrItemStringSectionColorShow.clear();
		colorStringSection.strSection = pItemData->m_FlightTypeItem->GetFlightTypeName();
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

		pItemDataEx->vrItemStringSectionColorShow = colorItemData.vrItemStringSectionColorShow;
		CString strNodeText;
		strNodeText.Format(_T("Flight Type to be delayed: %s"),pItemData->m_FlightTypeItem->GetFlightTypeName());
		m_treeCtrl.SetItemText(hItemUpdate,strNodeText);

	}
	else if (pItemData->m_enumItemType == ItemType_TimeItem)
	{
		if(pItemData->m_timeRange  == NULL)
			return;

		CString strStartTime = pItemData->m_timeRange->GetStartTime().PrintDateTime();
		CString strEndTime = pItemData->m_timeRange->GetEndTime().PrintDateTime();

		colorItemData.vrItemStringSectionColorShow.clear();
		colorStringSection.strSection = strStartTime;
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

		colorStringSection.strSection = strEndTime;
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

		pItemDataEx->vrItemStringSectionColorShow = colorItemData.vrItemStringSectionColorShow;
		CString strNodeText;
		strNodeText.Format(_T(" %s - %s"),strStartTime ,strEndTime);
		m_treeCtrl.SetItemText(hItemUpdate,strNodeText);

	}
	else if (pItemData->m_enumItemType == ItemType_RunwayItem)
	{
		if(pItemData->m_runwayMark == NULL)
			return;

		colorItemData.vrItemStringSectionColorShow.clear();

		CString strRunwayName = pItemData->m_runwayMark->GetRunwayMark();

		colorItemData.vrItemStringSectionColorShow.clear();
		colorStringSection.strSection = strRunwayName;
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

		pItemDataEx->vrItemStringSectionColorShow = colorItemData.vrItemStringSectionColorShow;

		CString strNodeText;
		strNodeText.Format(_T("%s"),strRunwayName);
		m_treeCtrl.SetItemText(hItemUpdate,strNodeText);

	}
	else if (pItemData->m_enumItemType == ItemType_RunwayExit)
	{
		if(pItemData->m_pTriggerCondition == NULL)
			return;

		colorItemData.vrItemStringSectionColorShow.clear();

		CString strRunwayExitName = pItemData->m_pTriggerCondition->GetTakeOffPosition()->GetName();

		CString strQueueLength;
		strQueueLength.Format(_T("%d"),pItemData->m_pTriggerCondition->GetQueueLength());

		CString strMinsPerAircraft;
		strMinsPerAircraft.Format(_T("%.0f"),pItemData->m_pTriggerCondition->GetMinsPerAircraft());

		colorStringSection.strSection = strRunwayExitName;
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

		colorStringSection.strSection = strQueueLength;
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);

		colorStringSection.strSection = strMinsPerAircraft;
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);
		
		pItemDataEx->vrItemStringSectionColorShow = colorItemData.vrItemStringSectionColorShow;

		CString strNodeText;
		strNodeText.Format(_T("%s when Queue Length > %s aircraft by %s mins/aircraft"),strRunwayExitName,strQueueLength,strMinsPerAircraft);
		m_treeCtrl.SetItemText(hItemUpdate,strNodeText);

	}
	else if (pItemData->m_enumItemType == ItemType_QueueLength)
	{
		if(pItemData->m_pTriggerCondition == NULL)
			return;

		colorItemData.vrItemStringSectionColorShow.clear();
		CString strQueueLength;
		strQueueLength.Format(_T("%d"),pItemData->m_pTriggerCondition->GetQueueLength());
		colorStringSection.strSection = strQueueLength;
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);
		pItemDataEx->vrItemStringSectionColorShow = colorItemData.vrItemStringSectionColorShow;
		CString strNodeText;
		strNodeText.Format(_T("Queue Length: %s"),strQueueLength);
		m_treeCtrl.SetItemText(hItemUpdate,strNodeText);

		if(m_treeCtrl.GetParentItem(hItemUpdate))
			UpdateItemText(m_treeCtrl.GetParentItem(hItemUpdate));

	}
	else if (pItemData->m_enumItemType == ItemType_MinsPerAircraft)
	{
		if(pItemData->m_pTriggerCondition == NULL)
			return;

		colorItemData.vrItemStringSectionColorShow.clear();
		CString strMinsPerAC;
		strMinsPerAC.Format(_T("%.0f"),pItemData->m_pTriggerCondition->GetMinsPerAircraft());
		colorStringSection.strSection = strMinsPerAC;
		colorItemData.vrItemStringSectionColorShow.push_back(colorStringSection);
		pItemDataEx->vrItemStringSectionColorShow = colorItemData.vrItemStringSectionColorShow;

		CString strNodeText;
		strNodeText.Format(_T("Mins: %s"),strMinsPerAC);
		m_treeCtrl.SetItemText(hItemUpdate,strNodeText);

		if(m_treeCtrl.GetParentItem(hItemUpdate))
			UpdateItemText(m_treeCtrl.GetParentItem(hItemUpdate));

	}

}

LRESULT CDlgArrivalDelayTriggers::OnTreeDoubleClick( WPARAM, LPARAM )
{
	HTREEITEM hItemSelected = m_treeCtrl.GetSelectedItem();
	if(hItemSelected == NULL)
		return 1;

	CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hItemSelected);
	if(pItemDataEx == NULL)
		return 1;

	CTreeItemData *pItemData  = (CTreeItemData *)pItemDataEx->dwptrItemData;
	if(pItemData == NULL)
		return 1;

	if(pItemData->m_enumItemType == ItemType_QueueLength)
	{
		if(pItemData->m_pTriggerCondition == NULL)
			return 1;

		int nQueueLength =pItemData->m_pTriggerCondition->GetQueueLength();
		CString strQueueLength ;
		strQueueLength.Format(_T("%d"),nQueueLength);
		m_treeCtrl.SetDisplayNum(nQueueLength);
		m_treeCtrl.SetDisplayType(2);
		m_treeCtrl.SetSpinRange(0,10000);
		m_treeCtrl.SpinEditLabel(hItemSelected,strQueueLength);
	}
	else if(pItemData->m_enumItemType == ItemType_MinsPerAircraft)
	{
		if(pItemData->m_pTriggerCondition == NULL)
			return 1;

		double nMins =pItemData->m_pTriggerCondition->GetMinsPerAircraft();
		CString strMins ;
		strMins.Format(_T("%0f"),nMins);
		m_treeCtrl.SetDisplayNum(static_cast<int>(nMins));
		m_treeCtrl.SetDisplayType(2);
		m_treeCtrl.SetSpinRange(0,10000);
		m_treeCtrl.SpinEditLabel(hItemSelected,strMins);

	}

	return 1;
}

LRESULT CDlgArrivalDelayTriggers::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( message == WM_INPLACE_SPIN )
	{

		HTREEITEM hItemSelected = m_treeCtrl.GetSelectedItem();
		if(hItemSelected == NULL)
			return 1;

		CARCTreeCtrlExWithColor::CItemData *pItemDataEx = m_treeCtrl.GetItemDataEx(hItemSelected);
		if(pItemDataEx == NULL)
			return 1;

		CTreeItemData *pItemData  = (CTreeItemData *)pItemDataEx->dwptrItemData;
		if(pItemData == NULL)
			return 1;

		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		int nValue = pst->iPercent;

		HTREEITEM hSelItem = m_treeCtrl.GetSelectedItem();

		if(pItemData->m_enumItemType == ItemType_QueueLength)
		{
			if(pItemData->m_pTriggerCondition)
			{
				pItemData->m_pTriggerCondition->SetQueueLength(nValue) ;
				UpdateItemText(hItemSelected);
			}

		}
		if(pItemData->m_enumItemType == ItemType_MinsPerAircraft)
		{
			if(pItemData->m_pTriggerCondition)
			{
				pItemData->m_pTriggerCondition->SetMinsPerAircraft(nValue) ;
				UpdateItemText(hItemSelected);
			}

		}

	}

	return CXTResizeDialog::DefWindowProc(message,wParam,lParam);
}








































