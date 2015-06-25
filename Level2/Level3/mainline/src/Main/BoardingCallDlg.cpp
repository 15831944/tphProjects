#include "stdafx.h"
#include "termplan.h"
#include "BoardingCallDlg.h"
#include "termplandoc.h"
#include "flightdialog.h"
#include "..\AirsideGUI\DlgStandFamily.h"
#include "..\Main\PassengerTypeDialog.h"
#include "Inputs\PROCDATA.H"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const UINT MENU_ADD_STAGE = 200;
static const UINT MENU_ADD_FLIGHTTYPE = 201;
static const UINT MENU_ADD_STAND = 202;
static const UINT MENU_ADD_PAXTYPE = 203;
static const UINT MENU_ADD_TRIGGER = 204;

static const UINT MENU_DEL_STAGE = 205;
static const UINT MENU_DEL_ALL_STAGE = 206;
static const UINT MENU_DEL_FLIGHTTYPE = 207;
static const UINT MENU_DEL_STAND = 208;
static const UINT MENU_DEL_PAXTYPE = 209;
static const UINT MENU_DEL_TRIGGER = 210;

static const UINT MENU_EDIT_FLIGHTTYPE = 211;
static const UINT MENU_EDIT_STAND = 212;
static const UINT MENU_EDIT_PAXTYPE = 213;
static const UINT MENU_EDIT_TRIGGERCOUNT = 211;
static const UINT MENU_EDIT_TRIGGERTIME = 212;
static const UINT MENU_EDIT_TRIGGERPROP = 213;
static const UINT MENU_UNAVAILABLE = 220;

typedef enum 
{
	TREE_NODE_INVALID = -1,
	TREE_NODE_STAGE = 0,
	TREE_NODE_FLIGHT_TYPE,
	TREE_NODE_STAND,
	TREE_NODE_PASSENGER_TYPE,
	TREE_NODE_TRIGGER_ALL,
	TREE_NODE_TRIGGER,
	TREE_NODE_TRIGGER_TIME,
	TREE_NODE_TRIGGER_PROP
} TREE_NODE_DATA_TYPE;

class TreeNodeDataWithType
{
public:
	TREE_NODE_DATA_TYPE m_type;
	DWORD m_data;
	TreeNodeDataWithType(){ m_type = TREE_NODE_INVALID; m_data = NULL; }
	~TreeNodeDataWithType(){}
};

CBoardingCallDlg::CBoardingCallDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBoardingCallDlg::IDD, pParent)
{
}


void CBoardingCallDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BOARDING_CALL_TREE, m_tree);
}


BEGIN_MESSAGE_MAP(CBoardingCallDlg, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_BOARDING_CALL_ADD_STAGE, OnToolbarButtonAddStage)
	ON_COMMAND(ID_BOARDING_CALL_ADD_FLIGHT, OnToolbarButtonAddFlightType)
	ON_COMMAND(ID_BOARDING_CALL_ADD_STAND, OnToolbarButtonAddStand)
	ON_COMMAND(ID_BOARDING_CALL_ADD_PAX, OnToolbarButtonAddPaxType)
	ON_COMMAND(ID_BOARDING_CALL_DELETE, OnToolbarButtonDel)
	ON_COMMAND(ID_BOARDING_CALL_EDIT, OnToolbarButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_NOTIFY(TVN_SELCHANGED, IDC_BOARDING_CALL_TREE, OnSelchangedBoardingCallTree)

	ON_WM_CONTEXTMENU()
	ON_COMMAND_RANGE(MENU_ADD_STAGE, MENU_UNAVAILABLE, OnChooseMenu)
END_MESSAGE_MAP()

int CBoardingCallDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolbar.LoadToolBar(IDR_BOARDING_CALL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	return 0;
}

BOOL CBoardingCallDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rect;
	m_tree.GetWindowRect(rect);
	ScreenToClient(rect);
	m_toolbar.SetWindowPos(NULL,rect.left,rect.top-26,rect.Width(),26,NULL);
	DisableAllToolBarButtons();

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	m_hRoot=m_tree.InsertItem("Boarding Calls",cni,FALSE);
	ReloadRoot();
	return TRUE;
}

void CBoardingCallDlg::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);
	if( m_btnOk.m_hWnd == NULL )
		return;

	CRect Barrc,Treerc,rc;
	m_btnCancel.GetWindowRect(&rc);
	m_btnCancel.MoveWindow(cx-rc.Width()-10,cy-15-rc.Height(),rc.Width(),rc.Height());
	m_btnOk.MoveWindow(cx-2*rc.Width()-30,cy-15-rc.Height(),rc.Width(),rc.Height());
	m_btnSave.MoveWindow(cx-3*rc.Width()-50,cy-15-rc.Height(),rc.Width(),rc.Height());	
	m_toolbar.GetWindowRect( &Barrc );
	m_toolbar.MoveWindow(12,10,cx-20,Barrc.Height());
	m_tree.MoveWindow(10,10+Barrc.Height(),cx-20,cy-20-rc.Height()-50);
	InvalidateRect(NULL);
}

InputTerminal* CBoardingCallDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}

void CBoardingCallDlg::OnButtonSave()
{
	CWaitCursor wc;
//	GetInputTerminal()->flightData->resortBoardingCallDB();
	GetInputTerminal()->flightData->saveDataSet( GetProjPath(), true );
	m_btnSave.EnableWindow( FALSE );
}

CString CBoardingCallDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
	
}

void CBoardingCallDlg::OnCancel() 
{
	if(!m_btnSave.IsWindowEnabled())
	{
		CDialog::OnCancel();
		return;
	}
	AfxGetApp()->BeginWaitCursor();
	
	try
	{
		GetInputTerminal()->flightData->loadDataSet(GetProjPath());
	}
	catch(FileVersionTooNewError* _pError)
	{
		char szBuf[128];
		_pError->getMessage(szBuf);
		MessageBox(szBuf, "Error", MB_OK|MB_ICONWARNING);
		delete _pError;			
	}
	AfxGetApp()->EndWaitCursor();
	
	CDialog::OnCancel();
}

void CBoardingCallDlg::OnOK() 
{
	if(!m_btnSave.IsWindowEnabled())
	{
		CDialog::OnOK();
		return;
	}
	
	AfxGetApp()->BeginWaitCursor();
	OnButtonSave();
	AfxGetApp()->EndWaitCursor();

	CDialog::OnOK();
}

void CBoardingCallDlg::ReloadRoot()
{
	RemoveTreeSubItem(m_hRoot);
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	FlightData* pFlightData = GetInputTerminal()->flightData;
	int stageCount = pFlightData->GetStageCount();
	for(int i=0; i<stageCount; i++)
	{
		CString strItemText;
		strItemText.Format("Stage: %d", (i+1));
		HTREEITEM hTreeItemStage = m_tree.InsertItem(strItemText, cni, FALSE, FALSE, m_hRoot);
		TreeNodeDataWithType* nodeDataStage = new TreeNodeDataWithType();
		nodeDataStage->m_type = TREE_NODE_STAGE;
		nodeDataStage->m_data = (DWORD)pFlightData->GetFlightTypeDBInStage(i);/* BoardingCallFlightTypeDatabase* */
		m_tree.SetItemData(hTreeItemStage, (DWORD)nodeDataStage);

		BoardingCallFlightTypeDatabase* pFlightTypeDB = pFlightData->GetFlightTypeDBInStage(i);
		ReloadStage(pFlightTypeDB, hTreeItemStage);
	}
	m_tree.Expand(m_tree.GetRootItem(),TVE_EXPAND);
	return;
}

void CBoardingCallDlg::ReloadStage(BoardingCallFlightTypeDatabase* pFlightTypeDB, HTREEITEM hTreeItemStage)
{
	RemoveTreeSubItem(hTreeItemStage);
	CString strItemText;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	int flightCount = pFlightTypeDB->getCount();
	for(int iFlight=0; iFlight<flightCount; iFlight++)
	{
		BoardingCallFlightTypeEntry* pFlightEntry = (BoardingCallFlightTypeEntry*)pFlightTypeDB->getItem(iFlight);
		FlightConstraint* pFlightConst = (FlightConstraint*)(pFlightEntry->getConstraint());
		CString strFlightType;
		pFlightConst->screenPrint(strFlightType);
		strItemText.Format("Flight Type: %s", strFlightType);
		HTREEITEM hTreeItemFlight = m_tree.InsertItem(strItemText, cni, FALSE, FALSE, hTreeItemStage);
		TreeNodeDataWithType* nodeDataFlight = new TreeNodeDataWithType();
		nodeDataFlight->m_type = TREE_NODE_FLIGHT_TYPE;
		nodeDataFlight->m_data = (DWORD)pFlightEntry;
		m_tree.SetItemData(hTreeItemFlight, (DWORD)nodeDataFlight);

		ReloadFlightType(pFlightEntry, hTreeItemFlight);
	}
}

void CBoardingCallDlg::ReloadFlightType( BoardingCallFlightTypeEntry* pFlightEntry, HTREEITEM hTreeItemFlight )
{
	BoardingCallStandDatabase* pStandDB = pFlightEntry->GetStandDatabase();
	RemoveTreeSubItem(hTreeItemFlight);
	CString strItemText;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	int standCount = pStandDB->getCount();
	for(int iStand=0; iStand<standCount; iStand++)
	{
		CString strStand;
		BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)pStandDB->getItem(iStand);
		const ProcessorID* pStandProcID = pStandEntry->getID();
		if(!pStandProcID->isBlank())
		{
			pStandProcID->printID(strStand.GetBuffer(64));
			strStand.ReleaseBuffer();
			strItemText.Format("Stand: %s", strStand);
		}
		else
		{
			strItemText = _T("Stand: All Stand");
		}
		HTREEITEM hTreeItemStand = m_tree.InsertItem(strItemText, cni, FALSE, FALSE, hTreeItemFlight);
		TreeNodeDataWithType* nodeDataStand = new TreeNodeDataWithType();
		nodeDataStand->m_type = TREE_NODE_STAND;
		nodeDataStand->m_data = (DWORD)pStandEntry;
		m_tree.SetItemData(hTreeItemStand, (DWORD)nodeDataStand);

		ReloadStand(pStandEntry, hTreeItemStand);
	}
}

void CBoardingCallDlg::ReloadStand(BoardingCallStandEntry* pStandEntry, HTREEITEM hTreeItemStand)
{
	BoardingCallPaxTypeDatabase* pPaxDB = pStandEntry->GetPaxTypeDatabase();
	RemoveTreeSubItem(hTreeItemStand);
	CString strItemText;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	int paxCount = pPaxDB->getCount();
	for(int iPax=0; iPax<paxCount; iPax++)
	{
		CString strPaxType;
		BoardingCallPaxTypeEntry* pPaxEntry = (BoardingCallPaxTypeEntry*)pPaxDB->getItem(iPax);
		CMobileElemConstraint* pMobElemConst = (CMobileElemConstraint*)(pPaxEntry->getConstraint());
		pMobElemConst->screenPrint(strPaxType);
		strItemText.Format("Passenger Type: %s", strPaxType);
		HTREEITEM hTreeItemPax = m_tree.InsertItem(strItemText, cni, FALSE, FALSE, hTreeItemStand);
		TreeNodeDataWithType* nodeDataPax = new TreeNodeDataWithType();
		nodeDataPax->m_type = TREE_NODE_PASSENGER_TYPE;
		nodeDataPax->m_data = (DWORD)pPaxEntry;
		m_tree.SetItemData(hTreeItemPax, (DWORD)nodeDataPax);

		ReloadPaxType(pPaxEntry, hTreeItemPax);
	}
}

void CBoardingCallDlg::ReloadPaxType( BoardingCallPaxTypeEntry* pPaxEntry, HTREEITEM hTreeItemPax )
{
	RemoveTreeSubItem(hTreeItemPax);
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	std::vector<BoardingCallTrigger>* vTriggers = pPaxEntry->GetTriggersDatabase();

	int triggerCount = vTriggers->size();
	cni.net = NET_EDITSPIN_WITH_VALUE;
	CString strItemText;
	strItemText.Format("Number of triggers: %d", triggerCount);
	HTREEITEM hTriggerAll = m_tree.InsertItem(strItemText, cni, FALSE, FALSE, hTreeItemPax);
	TreeNodeDataWithType* nodeDataAll = new TreeNodeDataWithType();
	nodeDataAll->m_type = TREE_NODE_TRIGGER_ALL;
	nodeDataAll->m_data = NULL;
	m_tree.SetItemData(hTriggerAll, (DWORD)nodeDataAll);

	ReloadAllTriggers(vTriggers, hTriggerAll);

}

void CBoardingCallDlg::ReloadAllTriggers(std::vector<BoardingCallTrigger>* vTriggers, HTREEITEM hTriggerAll)
{
	RemoveTreeSubItem(hTriggerAll);
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net = NET_EDITSPIN_WITH_VALUE;
	CString strItemText;
	int triggerCount = vTriggers->size();
	for(int triggerIndex=0; triggerIndex<triggerCount; triggerIndex++)
	{
		cni.net = NET_NORMAL;
		strItemText.Format("Trigger: %d", (triggerIndex+1));
		HTREEITEM hTrigger = m_tree.InsertItem(strItemText, cni, FALSE, FALSE, hTriggerAll);
		TreeNodeDataWithType* nodeDataTrigger = new TreeNodeDataWithType();
		nodeDataTrigger->m_type = TREE_NODE_TRIGGER;
		nodeDataTrigger->m_data = (DWORD)triggerIndex;
		m_tree.SetItemData(hTrigger, (DWORD)nodeDataTrigger);
		BoardingCallTrigger* trigger = &vTriggers->at(triggerIndex);
		CString strTime, strProp;
		// Add time.
		cni.net = NET_EDIT_WITH_VALUE;
		int seconds = trigger->GetTriggerTime().asSeconds();
		strTime.Format("Time range before STD(s): %d", seconds);
		nodeDataTrigger = new TreeNodeDataWithType();
		nodeDataTrigger->m_type = TREE_NODE_TRIGGER_TIME;
		nodeDataTrigger->m_data = (DWORD)seconds;
		HTREEITEM hTriggerTime = m_tree.InsertItem(strTime, cni, FALSE, FALSE, hTrigger);
		m_tree.SetItemData(hTriggerTime, (DWORD)nodeDataTrigger);
		// Add proportion.
		if(triggerIndex != triggerCount-1)
		{
			double prop = trigger->GetTriggerProportion();
			strProp.Format("Proportion of Pax: %.2f", prop);
			nodeDataTrigger = new TreeNodeDataWithType();
			nodeDataTrigger->m_type = TREE_NODE_TRIGGER_PROP;
			nodeDataTrigger->m_data = (DWORD)prop;
			HTREEITEM hTriggerProp = m_tree.InsertItem(strProp, cni, FALSE, FALSE, hTrigger);
			m_tree.SetItemData(hTriggerProp, (DWORD)nodeDataTrigger);
		}
		else
		{
			double prop = trigger->GetTriggerProportion();
			cni.net = NET_NORMAL;
			strProp = "Proportion of Pax: Residual";
			nodeDataTrigger = new TreeNodeDataWithType();
			nodeDataTrigger->m_type = TREE_NODE_TRIGGER_PROP;
			nodeDataTrigger->m_data = (DWORD)prop;
			HTREEITEM hTriggerProp = m_tree.InsertItem(strProp, cni, FALSE, FALSE, hTrigger);
			m_tree.SetItemData(hTriggerProp, (DWORD)nodeDataTrigger);
		}
	}

}

void CBoardingCallDlg::OnSelchangedBoardingCallTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM hSelItem = m_tree.GetSelectedItem();
	if(!hSelItem)
		return;
	DisableAllToolBarButtons();
	CString str = m_tree.GetItemText(hSelItem);
	if(hSelItem == m_hRoot)
	{
		m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_ADD_STAGE, TRUE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_DELETE, TRUE);
		return;
	}
	TreeNodeDataWithType* pDataWithType = (TreeNodeDataWithType*)m_tree.GetItemData(hSelItem);
	if(!pDataWithType)
		return;

	switch(pDataWithType->m_type)
	{
	case TREE_NODE_INVALID:
		break;
	case TREE_NODE_STAGE:
		m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_ADD_FLIGHT, TRUE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_DELETE, TRUE);
		break;
	case TREE_NODE_FLIGHT_TYPE:
		m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_ADD_STAND, TRUE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_DELETE, TRUE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_EDIT, TRUE);
		break;
	case TREE_NODE_STAND:
		m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_ADD_PAX, TRUE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_DELETE, TRUE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_EDIT, TRUE);
		break;
	case TREE_NODE_PASSENGER_TYPE:
		{
			m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_DELETE, TRUE);
			m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_EDIT, TRUE);
		}
		break;
	case TREE_NODE_TRIGGER_ALL:
		m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_EDIT, TRUE);
		break;
	case TREE_NODE_TRIGGER:
		m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_DELETE, TRUE);
		break;
	case TREE_NODE_TRIGGER_TIME:
	case TREE_NODE_TRIGGER_PROP:
		m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_EDIT, TRUE);
		break;
	default:
		break;
	}
}

void CBoardingCallDlg::OnToolbarButtonAddStage()
{
	HTREEITEM hSelItem = m_tree.GetSelectedItem();
	ASSERT(hSelItem == m_hRoot);

	GetInputTerminal()->flightData->AddStage();
	ReloadRoot();
	m_btnSave.EnableWindow(TRUE);
	return;
}

void CBoardingCallDlg::OnToolbarButtonAddFlightType()
{
	HTREEITEM hSelItem = m_tree.GetSelectedItem();
	TreeNodeDataWithType* pDataWithType = (TreeNodeDataWithType*)m_tree.GetItemData(hSelItem);
	ASSERT(pDataWithType->m_type == TREE_NODE_STAGE);

	BoardingCallFlightTypeDatabase* pFlightTypeDB = (BoardingCallFlightTypeDatabase*)pDataWithType->m_data;
	FlightConstraint fltConst;
	fltConst.SetAirportDB(GetInputTerminal()->m_pAirportDB);
	fltConst.SetFltConstraintMode(ENUM_FLTCNSTR_MODE_DEP);
	CFlightDialog flightTypeDlg(m_pParentWnd);
	flightTypeDlg.CustomizeDialog(fltConst, ENUM_DIALOG_TYPE_BOARDING_CALL);
	if( flightTypeDlg.DoModal() == IDOK )
	{
		fltConst = flightTypeDlg.GetFlightSelection();
		pFlightTypeDB->AddFlight(&fltConst, GetInputTerminal());
		ReloadStage(pFlightTypeDB, hSelItem);
		m_tree.Expand(hSelItem, TVE_EXPAND);
	}
}

void CBoardingCallDlg::OnToolbarButtonAddStand()
{
	HTREEITEM hSelItem = m_tree.GetSelectedItem();
	TreeNodeDataWithType* pDataWithType = (TreeNodeDataWithType*)m_tree.GetItemData(hSelItem);
	ASSERT(pDataWithType->m_type == TREE_NODE_FLIGHT_TYPE);

	BoardingCallFlightTypeEntry* pFlightTypeEntry = (BoardingCallFlightTypeEntry*)pDataWithType->m_data;
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	CDlgStandFamily standDlg(pDoc->GetProjectID());
	if(standDlg.DoModal()==IDOK)
	{
		CString strStand = standDlg.GetSelStandFamilyName();
		pFlightTypeEntry->GetStandDatabase()->AddStand(strStand.GetBuffer(), GetInputTerminal());
		ReloadFlightType(pFlightTypeEntry, hSelItem);
		m_tree.Expand(hSelItem, TVE_EXPAND);
		m_btnSave.EnableWindow(TRUE);
	}
}

void CBoardingCallDlg::OnToolbarButtonAddPaxType()
{
	HTREEITEM hSelItem = m_tree.GetSelectedItem();
	TreeNodeDataWithType* pDataWithType = (TreeNodeDataWithType*)m_tree.GetItemData(hSelItem);
	ASSERT(pDataWithType->m_type == TREE_NODE_STAND);

	BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)pDataWithType->m_data;

	CPassengerTypeDialog paxTypeDlg(m_pParentWnd);
	if(paxTypeDlg.DoModal() == IDOK)
	{
		CMobileElemConstraint mobileConst = paxTypeDlg.GetMobileSelection();
		mobileConst.SetInputTerminal(GetInputTerminal());
		pStandEntry->GetPaxTypeDatabase()->AddPax(&mobileConst, NULL);
		ReloadStand(pStandEntry, hSelItem);
		m_tree.Expand(hSelItem, TVE_EXPAND);
		m_btnSave.EnableWindow(TRUE);
	}
}

void CBoardingCallDlg::OnToolbarButtonDel()
{
	HTREEITEM hSelItem = m_tree.GetSelectedItem();
	if(!hSelItem)
		return;

	if(hSelItem == m_hRoot)
	{
		//Delete All Stages
		BOOL b_YesNo = MessageBox("Delete All Stages?","Delete All Stages", MB_YESNO|MB_ICONWARNING);
		if(b_YesNo == IDYES)
		{
			// Check if boarding call stage is used in Behavior.
			MiscProcessorData* pMiscDB = GetInputTerminal()->miscData->getDatabase( (int)HoldAreaProc );
			for (int nProcIdx = 0; nProcIdx < pMiscDB->getCount(); nProcIdx++)
			{
				MiscData* pMiscData = ((MiscDataElement*)pMiscDB->getItem( nProcIdx ))->getData();
				long lStageNumber= ((MiscHoldAreaData*)pMiscData)->GetStageNumber();
				if (lStageNumber > 1)
				{
					CString strTemp;
					strTemp.Format(_T("Boarding Calls Stage can't less than %d"), lStageNumber);
					MessageBox(strTemp,NULL,MB_OK);
					return;
				}
			}

			// Delete all stages and new a default one.
			int nDBCount=GetInputTerminal()->flightData->GetStageCount();
			for(int i=0;i<nDBCount;i++)
			{
				GetInputTerminal()->flightData->DeleteOneStageByIndex(0);
			}
			GetInputTerminal()->flightData->AddStage();
			ReloadRoot();
		}
	}

	TreeNodeDataWithType* pDataWithType = (TreeNodeDataWithType*)m_tree.GetItemData(hSelItem);
	if(!pDataWithType)
		return;
	switch(pDataWithType->m_type)
	{
	case TREE_NODE_INVALID:
		break;
	case TREE_NODE_STAGE:
		{
			MiscProcessorData* pMiscDB = GetInputTerminal()->miscData->getDatabase( (int)HoldAreaProc );
			for (int nProcIdx = 0; nProcIdx < pMiscDB->getCount(); nProcIdx++)
			{
				MiscData* pMiscData = ((MiscDataElement*)pMiscDB->getItem( nProcIdx ))->getData();
				long lStageNumber= ((MiscHoldAreaData*)pMiscData)->GetStageNumber();
				if (lStageNumber >= GetInputTerminal()->flightData->GetStageCount())
				{
					CString strTemp;
					strTemp.Format(_T("Boarding Calls Stage can't less than %d"), lStageNumber);
					MessageBox(strTemp,NULL,MB_OK);
					return;
				}
			}

			HTREEITEM hRootItem = m_tree.GetParentItem(hSelItem);
			HTREEITEM hPrevSiblingItem = m_tree.GetPrevSiblingItem(hSelItem);
			HTREEITEM hNextSiblingItem = m_tree.GetNextSiblingItem(hSelItem);
			BoardingCallFlightTypeDatabase* pFlightTypeDB = (BoardingCallFlightTypeDatabase*)pDataWithType->m_data;
			if(hPrevSiblingItem == NULL && hNextSiblingItem == NULL)
			{
				//Delete All Stages
				BOOL b_YesNo = MessageBox("Delete All Stages?","Delete Stage", MB_YESNO|MB_ICONWARNING);
				if(b_YesNo == IDNO)
				{
					return;
				}

				GetInputTerminal()->flightData->DeleteOneStageByFlightConstDB(pFlightTypeDB);
				m_tree.SelectItem(hRootItem);
				GetInputTerminal()->flightData->AddStage();
				ReloadRoot();
				m_btnSave.EnableWindow(TRUE);
				break;
			}
			GetInputTerminal()->flightData->DeleteOneStageByFlightConstDB(pFlightTypeDB);
			m_tree.SelectItem(hRootItem);
			ReloadRoot();
			m_btnSave.EnableWindow(TRUE);
			break;
		}
	case TREE_NODE_FLIGHT_TYPE:
		{
			HTREEITEM hPrevSiblingItem = m_tree.GetPrevSiblingItem(hSelItem);
			HTREEITEM hNextSiblingItem = m_tree.GetNextSiblingItem(hSelItem);
			BoardingCallFlightTypeEntry* pFlightTypeEntry = (BoardingCallFlightTypeEntry*)pDataWithType->m_data;
			HTREEITEM hStageItem = m_tree.GetParentItem(hSelItem);
			TreeNodeDataWithType* pParentData = (TreeNodeDataWithType*)m_tree.GetItemData(hStageItem);
			ASSERT(pParentData && pParentData->m_type == TREE_NODE_STAGE);
			BoardingCallFlightTypeDatabase* pFlightTypeDB = (BoardingCallFlightTypeDatabase*)(pParentData->m_data);

			if(hPrevSiblingItem == NULL && hNextSiblingItem == NULL)
			{
				BOOL b_YesNo = MessageBox("Delete All Flight Types?","Delete Flight Type", MB_YESNO|MB_ICONWARNING);
				if(b_YesNo == IDNO)
				{
					return;
				}

				pFlightTypeDB->deleteItem(pFlightTypeEntry);
				m_tree.SelectItem(hStageItem);
				pFlightTypeDB->AddFlight(NULL, GetInputTerminal());
				ReloadStage(pFlightTypeDB, hStageItem);
				m_btnSave.EnableWindow(TRUE);
				break;
			}

			pFlightTypeDB->deleteItem(pFlightTypeEntry);
			m_tree.SelectItem(hStageItem);
			// Reload all sibling item.
			ReloadStage(pFlightTypeDB, hStageItem);
			m_tree.Expand(hStageItem, TVE_EXPAND);
			m_btnSave.EnableWindow(TRUE);
		}
		break;
	case TREE_NODE_STAND:
		{
			HTREEITEM hPrevSiblingItem = m_tree.GetPrevSiblingItem(hSelItem);
			HTREEITEM hNextSiblingItem = m_tree.GetNextSiblingItem(hSelItem);

			BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)pDataWithType->m_data;

			// Reload all sibling item.
			HTREEITEM hFlightTypeItem = m_tree.GetParentItem(hSelItem);
			TreeNodeDataWithType* pParentData = (TreeNodeDataWithType*)m_tree.GetItemData(hFlightTypeItem);
			ASSERT(pParentData && pParentData->m_type == TREE_NODE_FLIGHT_TYPE);
			BoardingCallFlightTypeEntry* pFlightEntry = (BoardingCallFlightTypeEntry*)(pParentData->m_data);
			BoardingCallStandDatabase* pStandDB = pFlightEntry->GetStandDatabase();

			if(hPrevSiblingItem == NULL && hNextSiblingItem == NULL)
			{
				BOOL b_YesNo = MessageBox("Delete All Stands?","Delete Stand", MB_YESNO|MB_ICONWARNING);
				if(b_YesNo == IDNO)
				{
					return;
				}
				pStandDB->deleteItem(pStandEntry);
				m_tree.SelectItem(hFlightTypeItem);
				pStandDB->AddStand(NULL, GetInputTerminal());
				ReloadFlightType(pFlightEntry, hFlightTypeItem);
				m_btnSave.EnableWindow(TRUE);
				break;
			}
			pStandDB->deleteItem(pStandEntry);
			m_tree.SelectItem(hFlightTypeItem);
			ReloadFlightType(pFlightEntry, hFlightTypeItem);
			m_tree.Expand(hFlightTypeItem, TVE_EXPAND);
			m_btnSave.EnableWindow(TRUE);
		}
		break;
	case TREE_NODE_PASSENGER_TYPE:
		{
			HTREEITEM hPrevSiblingItem = m_tree.GetPrevSiblingItem(hSelItem);
			HTREEITEM hNextSiblingItem = m_tree.GetNextSiblingItem(hSelItem);

			BoardingCallPaxTypeEntry* pPaxEntry = (BoardingCallPaxTypeEntry*)pDataWithType->m_data;

			// Reload all sibling item.
			HTREEITEM hStandItem = m_tree.GetParentItem(hSelItem);
			TreeNodeDataWithType* pParentData = (TreeNodeDataWithType*)m_tree.GetItemData(hStandItem);
			ASSERT(pParentData && pParentData->m_type == TREE_NODE_STAND);
			BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)(pParentData->m_data);
			BoardingCallPaxTypeDatabase* pPaxTypeDB = pStandEntry->GetPaxTypeDatabase();

			if(hPrevSiblingItem == NULL && hNextSiblingItem == NULL)
			{
				BOOL b_YesNo = MessageBox("Delete All Passenger Types?","Delete Passenger Type", MB_YESNO|MB_ICONWARNING);
				if(b_YesNo == IDNO)
				{
					return;
				}
				pPaxTypeDB->deleteItem(pPaxEntry);
				m_tree.SelectItem(hStandItem);
				pPaxTypeDB->AddPax(NULL, GetInputTerminal());
				ReloadStand(pStandEntry, hStandItem);
				m_btnSave.EnableWindow(TRUE);
				break;
			}
			pPaxTypeDB->deleteItem(pPaxEntry);
			m_tree.SelectItem(hStandItem);
			ReloadStand(pStandEntry, hStandItem);
			m_tree.Expand(hStandItem, TVE_EXPAND);
			m_btnSave.EnableWindow(TRUE);
		}
		break;
	case TREE_NODE_TRIGGER_ALL:
		break;
	case TREE_NODE_TRIGGER:
		{
			HTREEITEM hPaxTypeItem = m_tree.GetParentItem(m_tree.GetParentItem(hSelItem));
			TreeNodeDataWithType* pParentData = (TreeNodeDataWithType*)m_tree.GetItemData(hPaxTypeItem);
			ASSERT(pParentData && pParentData->m_type == TREE_NODE_PASSENGER_TYPE);
			BoardingCallPaxTypeEntry* pPaxTypeEntry = (BoardingCallPaxTypeEntry*)pParentData->m_data;

			int triggerIndex = (int)pDataWithType->m_data;
			if(triggerIndex == (pPaxTypeEntry->GetTriggerCount()-1))
			{
				MessageBox("Can Not Delete The Default Trigger.");
				return;
			}
			pPaxTypeEntry->DeleteTrigger(triggerIndex);
			// Reload all sibling item.
			ReloadPaxType(pPaxTypeEntry, hPaxTypeItem);/* hParentItem will be deleted here. */
			m_tree.Expand(hPaxTypeItem, TVE_EXPAND);
			m_tree.Expand(m_tree.GetChildItem(hPaxTypeItem), TVE_EXPAND);/* hParentItem is deleted, so can't use here,
																		    use GetChildItem(hPaxTypeItem) instead. */
			m_tree.SelectItem(m_tree.GetChildItem(hPaxTypeItem));
			m_btnSave.EnableWindow(TRUE);
		}
		break;
	case TREE_NODE_TRIGGER_TIME:
		break;
	case TREE_NODE_TRIGGER_PROP:
		break;
	default:
		break;
	}
}

void CBoardingCallDlg::OnToolbarButtonEdit()
{	
	HTREEITEM hSelItem = m_tree.GetSelectedItem();
	if(!hSelItem || hSelItem == m_hRoot)
		return;

	TreeNodeDataWithType* pDataWithType = (TreeNodeDataWithType*)m_tree.GetItemData(hSelItem);
	if(!pDataWithType)
		return;
	switch(pDataWithType->m_type)
	{
	case TREE_NODE_INVALID:
		break;
	case TREE_NODE_STAGE:
		break;
	case TREE_NODE_FLIGHT_TYPE:
		{
			BoardingCallFlightTypeEntry* pFlightTypeEntry = (BoardingCallFlightTypeEntry*)pDataWithType->m_data;
			FlightConstraint* pOldConst = (FlightConstraint*)pFlightTypeEntry->getConstraint();

			CFlightDialog flightTypeDlg(m_pParentWnd);
			flightTypeDlg.CustomizeDialog(*pOldConst, ENUM_DIALOG_TYPE_BOARDING_CALL);
			if(flightTypeDlg.DoModal() == IDOK)
			{
				*pOldConst = flightTypeDlg.GetFlightSelection();

				// Reload all sibling item.
				HTREEITEM hStageItem = m_tree.GetParentItem(hSelItem);
				TreeNodeDataWithType* pParentData = (TreeNodeDataWithType*)m_tree.GetItemData(hStageItem);
				ASSERT(pParentData->m_type == TREE_NODE_STAGE);
				BoardingCallFlightTypeDatabase* pFlightTypeDB = (BoardingCallFlightTypeDatabase*)(pParentData->m_data);
				ReloadStage(pFlightTypeDB, hStageItem);
				m_tree.SelectItem(hStageItem);
				m_tree.Expand(hStageItem, TVE_EXPAND);
				m_btnSave.EnableWindow(TRUE);
			}
		}
		break;
	case TREE_NODE_STAND:
		{
			CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
			CDlgStandFamily standDlg(pDoc->GetProjectID());
			if(standDlg.DoModal()==IDOK)
			{
				BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)pDataWithType->m_data;
				CString strStand = standDlg.GetSelStandFamilyName();
				ProcessorID procID;
				procID.SetStrDict(GetInputTerminal()->inStrDict);
				procID.setID(strStand.GetBuffer());
				pStandEntry->SetProcessorID(procID);

				// Reload all sibling item.
				HTREEITEM hFlightTypeItem = m_tree.GetParentItem(hSelItem);
				TreeNodeDataWithType* pParentData = (TreeNodeDataWithType*)m_tree.GetItemData(hFlightTypeItem);
				ASSERT(pParentData && pParentData->m_type == TREE_NODE_FLIGHT_TYPE);
				BoardingCallFlightTypeEntry* pFlightEntry = (BoardingCallFlightTypeEntry*)(pParentData->m_data);
				ReloadFlightType(pFlightEntry, hFlightTypeItem);
				m_tree.SelectItem(hFlightTypeItem);
				m_tree.Expand(hFlightTypeItem, TVE_EXPAND);
				m_btnSave.EnableWindow(TRUE);
			}

		}
		break;
	case TREE_NODE_PASSENGER_TYPE:
		{
			CPassengerTypeDialog paxTypeDlg( m_pParentWnd );
			if( paxTypeDlg.DoModal() == IDOK )
			{
				BoardingCallPaxTypeEntry* pPaxTypeEntry = (BoardingCallPaxTypeEntry*)pDataWithType->m_data;
				CMobileElemConstraint selectedPaxConstraint = paxTypeDlg.GetMobileSelection();
				CMobileElemConstraint* pOldConst = (CMobileElemConstraint*)pPaxTypeEntry->getConstraint();
				*pOldConst = selectedPaxConstraint;

				// Reload all sibling item.
				HTREEITEM hStandItem = m_tree.GetParentItem(hSelItem);
				TreeNodeDataWithType* pParentData = (TreeNodeDataWithType*)m_tree.GetItemData(hStandItem);
				ASSERT(pParentData->m_type == TREE_NODE_STAND);
				BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)(pParentData->m_data);
				ReloadStand(pStandEntry, hStandItem);
				m_tree.SelectItem(hStandItem);
				m_tree.Expand(hStandItem, TVE_EXPAND);
				m_btnSave.EnableWindow(TRUE);
			}
		}
		break;
	case TREE_NODE_TRIGGER:
		break;
	case TREE_NODE_TRIGGER_ALL:
	case TREE_NODE_TRIGGER_TIME:
	case TREE_NODE_TRIGGER_PROP:
		m_tree.DoEdit(hSelItem);
		m_btnSave.EnableWindow(TRUE);
		break;
	default:
		break;
	}
}

void CBoardingCallDlg::DisableAllToolBarButtons()
{
	m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_ADD_STAGE, FALSE);
	m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_ADD_FLIGHT, FALSE);
	m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_ADD_STAND, FALSE);
	m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_ADD_PAX, FALSE);
	m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_DELETE, FALSE);
	m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_EDIT, FALSE);
}

LRESULT CBoardingCallDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	CString strItemText;
	CString oldItemText;
	CString strValue;
	int index;
	if(message==UM_CEW_COMBOBOX_BEGIN)	
	{
		CWnd* pWnd=m_tree.GetEditWnd((HTREEITEM)wParam);
		CComboBox* pCB=(CComboBox*)pWnd;
		if(pCB->GetCount()==0)
		{
		}			
	}
	if(message==UM_CEW_COMBOBOX_SELCHANGE)
	{
		HTREEITEM hSelItem=(HTREEITEM)wParam;
		int nIndexSeled=m_tree.GetCmbBoxCurSel(hSelItem);
		m_btnSave.EnableWindow(TRUE);
	}
	if(message==UM_CEW_SHOWMENU_READY)
	{
		CMenu* pMenu=(CMenu*)lParam;
		if(pMenu)
		{
		}
	}
	if(message == UM_CEW_EDITSPIN_BEGIN || message == UM_CEW_EDIT_BEGIN)
	{
		HTREEITEM hSelItem = (HTREEITEM)wParam;
		TreeNodeDataWithType* pDataWithType = (TreeNodeDataWithType*)m_tree.GetItemData(hSelItem);
		ASSERT(pDataWithType);
		switch(pDataWithType->m_type)
		{
		case TREE_NODE_TRIGGER_ALL:
			m_tree.SetItemValueRange(hSelItem,1,100);
			break;
		case TREE_NODE_TRIGGER_TIME:
			m_tree.SetItemValueRange(hSelItem,1,864000);
			break;
		case TREE_NODE_TRIGGER_PROP:
			m_tree.SetItemValueRange(hSelItem,1,100);
			break;
		default:
			break;
		}
		oldItemText = m_tree.GetItemText(hSelItem);
		index = oldItemText.ReverseFind(':') ;
		strItemText = oldItemText.Right(oldItemText.GetLength() - index - 2);
		m_tree.GetEditWnd(hSelItem)->SetWindowText(strItemText);
		((CEdit*)m_tree.GetEditWnd(hSelItem))->SetSel(0, -1, true); 
	}
	if(message == UM_CEW_EDITSPIN_END)
	{
		strValue=*((CString*)lParam);
		HTREEITEM hSelItem = (HTREEITEM)wParam;
		TreeNodeDataWithType* pDataWithType = (TreeNodeDataWithType*)m_tree.GetItemData(hSelItem);
		ASSERT(pDataWithType);
		switch(pDataWithType->m_type)
		{
		case TREE_NODE_TRIGGER_ALL:
			{
				HTREEITEM hPaxTypeItem = m_tree.GetParentItem(hSelItem);
				TreeNodeDataWithType* pDataWithType = (TreeNodeDataWithType*)m_tree.GetItemData(hPaxTypeItem);
				ASSERT(pDataWithType && pDataWithType->m_type == TREE_NODE_PASSENGER_TYPE);
				BoardingCallPaxTypeEntry* pPaxEntry = (BoardingCallPaxTypeEntry*)pDataWithType->m_data;
				std::vector<BoardingCallTrigger>* pVecTrigger = pPaxEntry->GetTriggersDatabase();
				int userSetCount = atoi(strValue.GetBuffer());
				strItemText.Format("Number of triggers: %d", userSetCount);
				pPaxEntry->SetTriggerCount(userSetCount);
				ReloadAllTriggers(pVecTrigger, hSelItem);
				m_tree.Expand(hSelItem, TVE_EXPAND);
			}
			break;
		case TREE_NODE_TRIGGER_TIME:
			{
				int userSetTime = atoi(strValue.GetBuffer());
				strItemText.Format("Time range before STD(s): %d", userSetTime);
				HTREEITEM hTriggerItem = m_tree.GetParentItem(hSelItem);
				TreeNodeDataWithType* pTriggerData = (TreeNodeDataWithType*)m_tree.GetItemData(hTriggerItem);
				ASSERT(pTriggerData && pTriggerData->m_type == TREE_NODE_TRIGGER);
				HTREEITEM hPaxTypeItem = m_tree.GetParentItem(m_tree.GetParentItem(hTriggerItem));
				TreeNodeDataWithType* pPaxTypeData = (TreeNodeDataWithType*)m_tree.GetItemData(hPaxTypeItem);
				ASSERT(pPaxTypeData && pPaxTypeData->m_type == TREE_NODE_PASSENGER_TYPE);
				BoardingCallPaxTypeEntry* pPaxTypeEntry = (BoardingCallPaxTypeEntry*)pPaxTypeData->m_data;

				int triggerIndex = (int)pTriggerData->m_data;
				pPaxTypeEntry->SetTriggerTime(triggerIndex, userSetTime);
				break;
			}
			break;
		case TREE_NODE_TRIGGER_PROP:
			{
				double userSetProp = atof(strValue.GetBuffer());
				strItemText.Format("Proportion of Pax: %.2f", userSetProp);
				HTREEITEM hTriggerItem = m_tree.GetParentItem(hSelItem);
				TreeNodeDataWithType* pTriggerData = (TreeNodeDataWithType*)m_tree.GetItemData(hTriggerItem);
				ASSERT(pTriggerData && pTriggerData->m_type == TREE_NODE_TRIGGER);
				HTREEITEM hPaxTypeItem = m_tree.GetParentItem(m_tree.GetParentItem(hTriggerItem));
				TreeNodeDataWithType* pPaxTypeData = (TreeNodeDataWithType*)m_tree.GetItemData(hPaxTypeItem);
				ASSERT(pPaxTypeData && pPaxTypeData->m_type == TREE_NODE_PASSENGER_TYPE);
				BoardingCallPaxTypeEntry* pPaxTypeEntry = (BoardingCallPaxTypeEntry*)pPaxTypeData->m_data;

				int triggerIndex = (int)pTriggerData->m_data;
				pPaxTypeEntry->SetTriggerProportion(triggerIndex, userSetProp);
			}
			break;
		default:
			break;
		}
		m_tree.SetItemText(hSelItem, strItemText);
	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CBoardingCallDlg::RemoveTreeSubItem(HTREEITEM hItem)
{
	if(hItem == NULL)
		return;
	HTREEITEM hChildItem;
	while((hChildItem = m_tree.GetChildItem(hItem)) != NULL)
	{
		RemoveTreeSubItem(hChildItem);
		RemoveTreeItemAndDeleteData(hChildItem);
	}
}

void CBoardingCallDlg::RemoveTreeItemAndDeleteData(HTREEITEM hItem)
{
	TreeNodeDataWithType* pDataWithType = (TreeNodeDataWithType*)m_tree.GetItemData(hItem);
	if(pDataWithType != NULL)
	{
		delete pDataWithType;
	}
	m_tree.DeleteItem(hItem);
}

void CBoardingCallDlg::OnContextMenu( CWnd* pWnd, CPoint point )
{
	CPoint pt = point;
	m_tree.ScreenToClient( &pt );

	UINT iRet;
	HTREEITEM hSelItem = m_tree.HitTest( pt, &iRet );
	if(hSelItem == NULL) 
		return ;
	if(hSelItem == m_hRoot)
	{
		CMenu menuProj;
		menuProj.CreatePopupMenu();
		menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_ADD_STAGE, _T("Add Stage"));
		menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_DEL_ALL_STAGE, _T("Delete All Stages"));
		menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Comments"));
		menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Help"));
		menuProj.TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
		return;
	}
	m_tree.SelectItem(hSelItem);

	TreeNodeDataWithType* pItemData = (TreeNodeDataWithType*)m_tree.GetItemData(hSelItem);
	ASSERT(pItemData);
	switch(pItemData->m_type)
	{
	case TREE_NODE_STAGE:
		{
			CMenu menuProj;
			menuProj.CreatePopupMenu();
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_ADD_FLIGHTTYPE, _T("Add Flight Type"));
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_DEL_STAGE, _T("Delete Stage"));
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Comments"));
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Help"));
			menuProj.TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
		}
		break;
	case TREE_NODE_FLIGHT_TYPE:
		{
			CMenu menuReport;
			menuReport.CreatePopupMenu();
			menuReport.AppendMenu(MF_STRING | MF_ENABLED , MENU_ADD_STAND, _T("Add Stand"));
			menuReport.AppendMenu(MF_STRING | MF_ENABLED , MENU_EDIT_FLIGHTTYPE, _T("Edit Flight Type"));
			menuReport.AppendMenu(MF_STRING | MF_ENABLED , MENU_DEL_FLIGHTTYPE, _T("Delete Flight Type"));
			menuReport.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Comments"));
			menuReport.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Help"));
			menuReport.TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
		}
		break;	
	case TREE_NODE_STAND:
		{
			CMenu menuReport;
			menuReport.CreatePopupMenu();
			menuReport.AppendMenu(MF_STRING | MF_ENABLED , MENU_ADD_PAXTYPE, _T("Add Passenger Type"));
			menuReport.AppendMenu(MF_STRING | MF_ENABLED , MENU_EDIT_STAND, _T("Edit Stand"));
			menuReport.AppendMenu(MF_STRING | MF_ENABLED , MENU_DEL_STAND, _T("Delete Stand"));
			menuReport.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Comments"));
			menuReport.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Help"));
			menuReport.TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
		}
		break;
	case TREE_NODE_PASSENGER_TYPE:
		{
			CMenu menuProj;
			menuProj.CreatePopupMenu();
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_EDIT_PAXTYPE, _T("Edit Passenger Type"));
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_DEL_PAXTYPE, _T("Delete Passenger Type"));
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Comments"));
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Help"));
			menuProj.TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);		
		}
		break;
	case TREE_NODE_TRIGGER_ALL:
		{
			CMenu menuProj;
			menuProj.CreatePopupMenu();
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_ADD_TRIGGER, _T("Add Trigger"));
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_EDIT_TRIGGERCOUNT, _T("Edit Trigger Count"));
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Comments"));
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Help"));
			menuProj.TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);		
		}
		break;
	case TREE_NODE_TRIGGER:
		{
			CMenu menuProj;
			menuProj.CreatePopupMenu();
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_DEL_TRIGGER, _T("Delete Trigger"));
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Comments"));
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Help"));
			menuProj.TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);		
		}
		break;
	case TREE_NODE_TRIGGER_TIME:
		{
			CMenu menuProj;
			menuProj.CreatePopupMenu();
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_EDIT_TRIGGERTIME, _T("Edit Trigger Time"));
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Comments"));
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Help"));
			menuProj.TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);		
		}
		break;
	case TREE_NODE_TRIGGER_PROP:
		{
			CMenu menuProj;
			menuProj.CreatePopupMenu();
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_EDIT_TRIGGERPROP, _T("Edit Trigger Proportion"));
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Comments"));
			menuProj.AppendMenu(MF_STRING | MF_ENABLED , MENU_UNAVAILABLE, _T("Help"));
			menuProj.TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);		
		}
		break;
	default:
		break;
	}
}

void CBoardingCallDlg::OnChooseMenu( UINT nID )
{
	if(nID == MENU_UNAVAILABLE)
		return;
	HTREEITEM hSelItem = m_tree.GetSelectedItem();
	if(hSelItem == NULL)
		return;
	if(hSelItem == m_hRoot)
	{
		switch(nID)
		{
		case MENU_ADD_STAGE:
			OnToolbarButtonAddStage();
			break;
		case MENU_DEL_ALL_STAGE:
			OnToolbarButtonDel();
			break;
		default:
			break;
		}
		return;
	}

	TreeNodeDataWithType* pTriggerData = (TreeNodeDataWithType*)m_tree.GetItemData(hSelItem);
	ASSERT(pTriggerData);
	switch (pTriggerData->m_type)
	{
	case TREE_NODE_STAGE:
		{
			switch(nID)
			{
			case MENU_ADD_FLIGHTTYPE:
				OnToolbarButtonAddFlightType();
				break;
			case MENU_DEL_STAGE:
				OnToolbarButtonDel();
				break;
			default:
				break;
			}
		}
		break;
	case TREE_NODE_FLIGHT_TYPE:
		{
			switch(nID)
			{
			case MENU_ADD_STAND:
				OnToolbarButtonAddStand();
				break;
			case MENU_DEL_FLIGHTTYPE:
				OnToolbarButtonDel();
				break;
			case MENU_EDIT_FLIGHTTYPE:
				OnToolbarButtonEdit();
				break;
			default:
				break;
			}
		}
		break;	
	case TREE_NODE_STAND:
		{
			switch(nID)
			{
			case MENU_ADD_PAXTYPE:
				OnToolbarButtonAddPaxType();
				break;
			case MENU_EDIT_STAND:
				OnToolbarButtonEdit();
				break;
			case MENU_DEL_STAND:
				OnToolbarButtonDel();
				break;
			default:
				break;
			}
		}
		break;
	case TREE_NODE_PASSENGER_TYPE:
		{
			switch(nID)
			{
			case MENU_EDIT_PAXTYPE:
				OnToolbarButtonEdit();
				break;
			case MENU_DEL_PAXTYPE:
				OnToolbarButtonDel();
				break;
			default:
				break;
			}
		}
		break;
	case TREE_NODE_TRIGGER_ALL:
		{
			switch(nID)
			{
			case MENU_EDIT_TRIGGERCOUNT:
				OnToolbarButtonEdit();
				break;
			default:
				break;
			}
		}
		break;
	case TREE_NODE_TRIGGER:
		{
			switch(nID)
			{
			case MENU_DEL_TRIGGER:
				OnToolbarButtonDel();
				break;
			default:
				break;
			}
		}
		break;
	case TREE_NODE_TRIGGER_TIME:
		{
			switch(nID)
			{
			case MENU_EDIT_TRIGGERTIME:
				OnToolbarButtonEdit();
				break;
			default:
				break;
			}
		}
		break;
	case TREE_NODE_TRIGGER_PROP:
		{
			switch(nID)
			{
			case MENU_EDIT_TRIGGERPROP:
				OnToolbarButtonEdit();
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	return;
}
