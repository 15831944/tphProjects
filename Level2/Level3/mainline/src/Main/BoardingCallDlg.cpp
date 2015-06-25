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
#include "Common\ProbDistEntry.h"
#include "Common\ProbDistManager.h"
#include "DlgProbDist.h"
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
	std::vector<BoardingCallTrigger*>& vTriggers = pPaxEntry->GetTriggersDatabase();

	int triggerCount = vTriggers.size();
	cni.net = NET_EDITSPIN_WITH_VALUE;
	CString strItemText;
	strItemText.Format("Number of triggers: %d", triggerCount);
	HTREEITEM hTriggerAll = m_tree.InsertItem(strItemText, cni, FALSE, FALSE, hTreeItemPax);
	TreeNodeDataWithType* nodeDataAll = new TreeNodeDataWithType();
	nodeDataAll->m_type = TREE_NODE_TRIGGER_ALL;
	nodeDataAll->m_data = NULL;
	m_tree.SetItemData(hTriggerAll, (DWORD)nodeDataAll);

	ReloadTriggers(vTriggers, hTriggerAll);
}

void CBoardingCallDlg::ReloadTriggers(std::vector<BoardingCallTrigger*>& vTriggers, HTREEITEM hTriggerAll)
{
	RemoveTreeSubItem(hTriggerAll);
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	CString strItemText;
	int triggerCount = vTriggers.size();
	for(int iTrigger=0; iTrigger<triggerCount; iTrigger++)
	{
		strItemText.Format("Trigger: %d", (iTrigger+1));
		cni.net = NET_NORMAL;
		HTREEITEM hTrigger = m_tree.InsertItem(strItemText, cni, FALSE, FALSE, hTriggerAll);
		TreeNodeDataWithType* nodeDataTrigger = new TreeNodeDataWithType();
		nodeDataTrigger->m_type = TREE_NODE_TRIGGER;
		nodeDataTrigger->m_data = (DWORD)vTriggers.at(iTrigger);
		m_tree.SetItemData(hTrigger, (DWORD)nodeDataTrigger);
		BoardingCallTrigger* trigger = vTriggers.at(iTrigger);
		CString strTime, strProp;
		// Add time.
		strTime.Format("Time range before STD(seconds): %s", trigger->m_time->screenPrint());
		nodeDataTrigger = new TreeNodeDataWithType();
		nodeDataTrigger->m_type = TREE_NODE_TRIGGER_TIME;
		nodeDataTrigger->m_data = (DWORD)trigger->m_time;
		cni.net = NET_COMBOBOX;
		HTREEITEM hTriggerTime = m_tree.InsertItem(strTime, cni, FALSE, FALSE, hTrigger);
		m_tree.SetItemData(hTriggerTime, (DWORD)nodeDataTrigger);
		
		// Add proportion, the last one is 'residual', so don't show its proportion.
		if(iTrigger != triggerCount-1)
		{
			// user define trigger.
			strProp.Format("Proportion of Pax: %s", trigger->m_prop->screenPrint());
			nodeDataTrigger = new TreeNodeDataWithType();
			nodeDataTrigger->m_type = TREE_NODE_TRIGGER_PROP;
			nodeDataTrigger->m_data = (DWORD)trigger->m_prop;
			cni.net = NET_COMBOBOX;
			HTREEITEM hTriggerProp = m_tree.InsertItem(strProp, cni, FALSE, FALSE, hTrigger);
			m_tree.SetItemData(hTriggerProp, (DWORD)nodeDataTrigger);
		}
		else
		{
			// the 'residual' trigger.
			strProp = "Proportion of Pax: Residual";
			nodeDataTrigger = new TreeNodeDataWithType();
			nodeDataTrigger->m_type = TREE_NODE_TRIGGER_PROP;
			nodeDataTrigger->m_data = (DWORD)-1;
			cni.net = NET_NORMAL;
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
	while( flightTypeDlg.DoModal() == IDOK )
	{
		fltConst = flightTypeDlg.GetFlightSelection();
		if(pFlightTypeDB->findItemByConstraint(&fltConst) != INT_MAX)
		{
			MessageBox("Selected Flight Type is already exists.");
		}
		else 
		{
			pFlightTypeDB->AddFlightType(&fltConst, GetInputTerminal());
			ReloadStage(pFlightTypeDB, hSelItem);
			m_tree.Expand(hSelItem, TVE_EXPAND);
			m_btnSave.EnableWindow(TRUE);
			break;
		}
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
	while(standDlg.DoModal()==IDOK)
	{
		CString strStand = standDlg.GetSelStandFamilyName();
		ProcessorID procID;
		if(strStand.Compare("All Stand") == 0)
		{
			procID.init();
			procID.SetStrDict(GetInputTerminal()->inStrDict);
		}
		else
		{
			procID.SetStrDict(GetInputTerminal()->inStrDict);
			procID.setID(strStand.GetBuffer());
		}
		if(pFlightTypeEntry->GetStandDatabase()->findEntry(procID) != INT_MAX)
		{
			MessageBox("Selected Stand is already exists.");
		}
		else
		{
			pFlightTypeEntry->GetStandDatabase()->AddStand(&procID, GetInputTerminal());
			ReloadFlightType(pFlightTypeEntry, hSelItem);
			m_tree.Expand(hSelItem, TVE_EXPAND);
			m_btnSave.EnableWindow(TRUE);
			break;
		}
	}
}

void CBoardingCallDlg::OnToolbarButtonAddPaxType()
{
	HTREEITEM hSelItem = m_tree.GetSelectedItem();

	CPassengerTypeDialog paxTypeDlg(m_pParentWnd);
	while(paxTypeDlg.DoModal() == IDOK)
	{
		CMobileElemConstraint mobElemConst;
		mobElemConst.SetInputTerminal(GetInputTerminal());
		mobElemConst = paxTypeDlg.GetMobileSelection();
		TreeNodeDataWithType* pDataWithType = (TreeNodeDataWithType*)m_tree.GetItemData(hSelItem);
		ASSERT(pDataWithType->m_type == TREE_NODE_STAND);
		BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)pDataWithType->m_data;
		if(pStandEntry->GetPaxTypeDatabase()->FindItemByConstraint(&mobElemConst) != NULL)
		{
			MessageBox("Selected Stand is already exists.");
		}
		else
		{
			pStandEntry->GetPaxTypeDatabase()->AddPaxType(&mobElemConst, NULL);
			ReloadStand(pStandEntry, hSelItem);
			m_tree.Expand(hSelItem, TVE_EXPAND);
			m_btnSave.EnableWindow(TRUE);
			break;
		}
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
		int b_YesNo = MessageBox("Delete All Stages?","Delete All Stages", MB_YESNO|MB_ICONWARNING);
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
				if (lStageNumber >= GetInputTerminal()->flightData->GetStageCount() && lStageNumber!=1)
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
				pFlightTypeDB->AddFlightType(NULL, GetInputTerminal());
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
			BoardingCallPaxTypeEntry* pPaxEntry = (BoardingCallPaxTypeEntry*)pDataWithType->m_data;

			// Reload all sibling item.
			HTREEITEM hStandItem = m_tree.GetParentItem(hSelItem);
			TreeNodeDataWithType* pParentData = (TreeNodeDataWithType*)m_tree.GetItemData(hStandItem);
			ASSERT(pParentData && pParentData->m_type == TREE_NODE_STAND);
			BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)(pParentData->m_data);
			BoardingCallPaxTypeDatabase* pPaxTypeDB = pStandEntry->GetPaxTypeDatabase();

			if(pPaxTypeDB->Find(pPaxEntry)!=INT_MAX && pStandEntry->getID()->isBlank())
			{
				MessageBox("Can't delete the DEFAULT Passenger Type.");
				return;
			}

			HTREEITEM hPrevSiblingItem = m_tree.GetPrevSiblingItem(hSelItem);
			HTREEITEM hNextSiblingItem = m_tree.GetNextSiblingItem(hSelItem);
			if(hPrevSiblingItem == NULL && hNextSiblingItem == NULL)
			{
				BOOL b_YesNo = MessageBox("Delete All Passenger Types?","Delete Passenger Type", MB_YESNO|MB_ICONWARNING);
				if(b_YesNo == IDNO)
				{
					return;
				}
				pPaxTypeDB->deleteItem(pPaxEntry);
				m_tree.SelectItem(hStandItem);
				pPaxTypeDB->AddPaxType(NULL, GetInputTerminal());
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

			HTREEITEM hStageItem = m_tree.GetParentItem(hSelItem);
			TreeNodeDataWithType* pParData = (TreeNodeDataWithType*)m_tree.GetItemData(hStageItem);
			if(!pParData)
				return;
			ASSERT(pParData->m_type == TREE_NODE_STAGE);
			BoardingCallFlightTypeDatabase* pfltTypeDB = (BoardingCallFlightTypeDatabase*)pParData->m_data;

			while(flightTypeDlg.DoModal() == IDOK)
			{
				FlightConstraint fltConst = flightTypeDlg.GetFlightSelection();
				if(*pOldConst == fltConst)
				{
					break;
				}
				else if(pfltTypeDB->findItemByConstraint(&fltConst) != INT_MAX)
				{
					MessageBox("Selected Flight Type is already exists.");
				}
				else
				{
					*pOldConst = fltConst;

					// Reload all sibling item.
					HTREEITEM hStageItem = m_tree.GetParentItem(hSelItem);
					TreeNodeDataWithType* pParentData = (TreeNodeDataWithType*)m_tree.GetItemData(hStageItem);
					ASSERT(pParentData->m_type == TREE_NODE_STAGE);
					BoardingCallFlightTypeDatabase* pFlightTypeDB = (BoardingCallFlightTypeDatabase*)(pParentData->m_data);
					ReloadStage(pFlightTypeDB, hStageItem);
					m_tree.SelectItem(hStageItem);
					m_tree.Expand(hStageItem, TVE_EXPAND);
					m_btnSave.EnableWindow(TRUE);
					break;
				}
			}
		}
		break;
	case TREE_NODE_STAND:
		{
			CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
			CDlgStandFamily standDlg(pDoc->GetProjectID());

			HTREEITEM hFltTypeItem = m_tree.GetParentItem(hSelItem);
			TreeNodeDataWithType* pFltTypeData = (TreeNodeDataWithType*)m_tree.GetItemData(hFltTypeItem);
			ASSERT(pFltTypeData->m_type == TREE_NODE_FLIGHT_TYPE);
			BoardingCallFlightTypeEntry* pFltTypeEntry = (BoardingCallFlightTypeEntry*)pFltTypeData->m_data;
			BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)pDataWithType->m_data;
			while(standDlg.DoModal()==IDOK)
			{
				CString strStand = standDlg.GetSelStandFamilyName();
				ProcessorID procID;
				procID.SetStrDict(GetInputTerminal()->inStrDict);
				if(strStand.Compare("All Stand") == 0)
				{
					procID.init();
				}
				else
				{
					procID.setID(strStand.GetBuffer());
				}

				if(pFltTypeEntry->GetStandDatabase()->findEntry(procID) != INT_MAX)
				{
					MessageBox("Selected Stand is already exists.");
				}
				else
				{
					pStandEntry->SetProcessorID(procID);

					// Reload all sibling item.
					HTREEITEM hFlightTypeItem = m_tree.GetParentItem(hSelItem);
					TreeNodeDataWithType* pParentData = (TreeNodeDataWithType*)m_tree.GetItemData(hFlightTypeItem);
					ASSERT(pParentData && pParentData->m_type == TREE_NODE_FLIGHT_TYPE);
					BoardingCallFlightTypeEntry* pFlightEntry = (BoardingCallFlightTypeEntry*)(pParentData->m_data);
					m_tree.SelectItem(hFlightTypeItem);
					ReloadFlightType(pFlightEntry, hFlightTypeItem);
					m_tree.Expand(hFlightTypeItem, TVE_EXPAND);
					m_btnSave.EnableWindow(TRUE);
					break;
				}
			}
		}
		break;
	case TREE_NODE_PASSENGER_TYPE:
		{
			BoardingCallPaxTypeEntry* pPaxTypeEntry = (BoardingCallPaxTypeEntry*)pDataWithType->m_data;

			HTREEITEM hStandItem = m_tree.GetParentItem(hSelItem);
			TreeNodeDataWithType* pStandData = (TreeNodeDataWithType*)m_tree.GetItemData(hStandItem);
			BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)pStandData->m_data;

			if(pStandEntry->GetPaxTypeDatabase()->Find(pPaxTypeEntry)!=INT_MAX &&
				pStandEntry->getID()->isBlank())
			{
				MessageBox("Can't change the DEFAULT Passenger Type.");
				return;
			}
			CPassengerTypeDialog paxTypeDlg( m_pParentWnd );
			while(paxTypeDlg.DoModal() == IDOK)
			{
				
				CMobileElemConstraint mobElemConst = paxTypeDlg.GetMobileSelection();
				CMobileElemConstraint* pOldConst = (CMobileElemConstraint*)pPaxTypeEntry->getConstraint();
				
				if(pStandEntry->GetPaxTypeDatabase()->FindEqual(mobElemConst) != NULL)
				{
					MessageBox("Selected Stand is already exists.");
				}
				else
				{
					*pOldConst = mobElemConst;
					// Reload all sibling item.
					HTREEITEM hStandItem = m_tree.GetParentItem(hSelItem);
					TreeNodeDataWithType* pParentData = (TreeNodeDataWithType*)m_tree.GetItemData(hStandItem);
					ASSERT(pParentData->m_type == TREE_NODE_STAND);
					BoardingCallStandEntry* pStandEntry = (BoardingCallStandEntry*)(pParentData->m_data);
					ReloadStand(pStandEntry, hStandItem);
					m_tree.SelectItem(hStandItem);
					m_tree.Expand(hStandItem, TVE_EXPAND);
					m_btnSave.EnableWindow(TRUE);
					break;
				}
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
		HTREEITEM hSelItem = (HTREEITEM)wParam;
		CWnd* pWnd=m_tree.GetEditWnd(hSelItem);
		CComboBox* pCB=(CComboBox*)pWnd;
		pCB->ResetContent();
		pCB->SetDroppedWidth(250);
		TreeNodeDataWithType* pDataWithType = (TreeNodeDataWithType*)m_tree.GetItemData(hSelItem);
		ASSERT(pDataWithType);
		switch(pDataWithType->m_type)
		{
		case TREE_NODE_TRIGGER_TIME:
		case TREE_NODE_TRIGGER_PROP:
			{
				pCB->AddString( "NEW PROBABILITY DISTRIBUTION" );
				CProbDistManager* pProbDistMan = GetInputTerminal()->m_pAirportDB->getProbDistMan();
				int nCount = static_cast<int>(pProbDistMan->getCount());
				for( int m=0; m<nCount; m++ )
				{
					CProbDistEntry* pPBEntry = pProbDistMan->getItem( m );
					pCB->AddString( pPBEntry->m_csName );
				}
			}
			break;
		default:
			break;
		}
	}
	if(message==UM_CEW_COMBOBOX_SELCHANGE)
	{
		HTREEITEM hSelItem=(HTREEITEM)wParam;
		CWnd* pWnd=m_tree.GetEditWnd(hSelItem);
		CComboBox* pCB=(CComboBox*)pWnd;

		int nIndexSeled=m_tree.GetCmbBoxCurSel(hSelItem);
		TreeNodeDataWithType* pSelItemData = (TreeNodeDataWithType*)m_tree.GetItemData(hSelItem);
		ASSERT(pSelItemData);
		switch(pSelItemData->m_type)
		{
		case TREE_NODE_TRIGGER_TIME:
			{
				HTREEITEM hPareItem = m_tree.GetParentItem(hSelItem);
				TreeNodeDataWithType* pParentData = (TreeNodeDataWithType*)m_tree.GetItemData(hPareItem);
				ASSERT(pParentData->m_type == TREE_NODE_TRIGGER);
				BoardingCallTrigger* pTrigger = (BoardingCallTrigger*)pParentData->m_data;

				int nIndex = pCB->GetCurSel();
				int LBTextLen = pCB->GetLBTextLen(nIndex);
				CString strSel;
				pCB->GetLBText(nIndex, strSel.GetBuffer(LBTextLen));
				strSel.ReleaseBuffer();
				ProbabilityDistribution* pProbDist = NULL;
				CProbDistManager* pProbDistMan = GetInputTerminal()->m_pAirportDB->getProbDistMan();
				if(strSel == "NEW PROBABILITY DISTRIBUTION")
				{
					CProbDistEntry* pPDEntry = NULL;
					CDlgProbDist dlg( GetInputTerminal()->m_pAirportDB,true,this);
					if (dlg.DoModal() == IDOK)
					{
						pPDEntry = dlg.GetSelectedPD();
						if(pPDEntry != NULL)
						{
							ProbabilityDistribution* pProbDist = CopyProbDistribution(pPDEntry->m_pProbDist);
							ASSERT(pProbDist);
							pTrigger->m_time = pProbDist;
							pSelItemData->m_data = DWORD(pTrigger->m_time);
						}
					}
				}
				else
				{
					CProbDistEntry* pPDEntry = NULL;
					int nCount = pProbDistMan->getCount();
					for( int i=0; i<nCount; i++ )
					{
						pPDEntry = pProbDistMan->getItem( i );
						if( strcmp( pPDEntry->m_csName, strSel ) == 0 )
							break;
					}
					ASSERT(i<nCount);
					ProbabilityDistribution* pProbDist = CopyProbDistribution(pPDEntry->m_pProbDist);
					ASSERT(pProbDist);
					ProbabilityDistribution* pTT = pTrigger->m_time;
					if(pTrigger->m_time) 
						delete pTrigger->m_time;
					pTrigger->m_time = pProbDist;
					pSelItemData->m_data = DWORD(pTrigger->m_time);
				}
				CString strTime;
				strTime.Format("Time range before STD(seconds): %s", pTrigger->m_time->screenPrint());
				m_tree.SetItemText(hSelItem, strTime);
			}
			break;
		case TREE_NODE_TRIGGER_PROP:
			{
				HTREEITEM hPareItem = m_tree.GetParentItem(hSelItem);
				TreeNodeDataWithType* pParentData = (TreeNodeDataWithType*)m_tree.GetItemData(hPareItem);
				ASSERT(pParentData->m_type == TREE_NODE_TRIGGER);
				BoardingCallTrigger* pTrigger = (BoardingCallTrigger*)pParentData->m_data;

				int nIndex = pCB->GetCurSel();
				int LBTextLen = pCB->GetLBTextLen(nIndex);
				CString strSel;
				pCB->GetLBText(nIndex, strSel.GetBuffer(LBTextLen));
				strSel.ReleaseBuffer();
				ProbabilityDistribution* pProbDist = NULL;
				CProbDistManager* pProbDistMan = GetInputTerminal()->m_pAirportDB->getProbDistMan();
				if(strSel == "NEW PROBABILITY DISTRIBUTION")
				{
					CProbDistEntry* pPDEntry = NULL;
					CDlgProbDist dlg( GetInputTerminal()->m_pAirportDB,true,this);
					if (dlg.DoModal() == IDOK)
					{
						pPDEntry = dlg.GetSelectedPD();
						if(pPDEntry != NULL)
						{
							ProbabilityDistribution* pProbDist = CopyProbDistribution(pPDEntry->m_pProbDist);
							ASSERT(pProbDist);
							pTrigger->m_prop = pProbDist;
							pSelItemData->m_data = DWORD(pTrigger->m_prop);
						}
					}
				}
				else
				{
					CProbDistEntry* pPDEntry = NULL;
					int nCount = pProbDistMan->getCount();
					for( int i=0; i<nCount; i++ )
					{
						pPDEntry = pProbDistMan->getItem( i );
						if( strcmp( pPDEntry->m_csName, strSel ) == 0 )
							break;
					}
					ASSERT(i<nCount);
					ProbabilityDistribution* pProbDist = CopyProbDistribution(pPDEntry->m_pProbDist);
					ASSERT(pProbDist);
					ProbabilityDistribution* pTT = pTrigger->m_time;
					if(pTrigger->m_prop) 
						delete pTrigger->m_prop;
					pTrigger->m_prop = pProbDist;
					pSelItemData->m_data = DWORD(pTrigger->m_prop);
				}
				CString strTime;
				strTime.Format("Proportion of Pax: %s", pTrigger->m_prop->screenPrint());
				m_tree.SetItemText(hSelItem, strTime);
			}
			break;
		default:
			break;
		}
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
			m_tree.SetItemValueRange(hSelItem, 1, 100);
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
				std::vector<BoardingCallTrigger*>& vTrigger = pPaxEntry->GetTriggersDatabase();
				int userSetCount = atoi(strValue.GetBuffer());
				strItemText.Format("Number of triggers: %d", userSetCount);
				pPaxEntry->SetTriggerCount(userSetCount);
				ReloadTriggers(vTrigger, hSelItem);
				m_tree.Expand(hSelItem, TVE_EXPAND);
				m_btnSave.EnableWindow(TRUE);
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

ProbabilityDistribution* CBoardingCallDlg::CopyProbDistribution(ProbabilityDistribution* _pPD)
{
	if(!_pPD)
		return NULL;

	ProbabilityDistribution* pDist = NULL;
	switch( _pPD->getProbabilityType() ) 
	{
	case BERNOULLI:
		pDist = new BernoulliDistribution( ((BernoulliDistribution*)_pPD)->getValue1(),
			((BernoulliDistribution*)_pPD)->getValue2(),
			((BernoulliDistribution*)_pPD)->getProb1()	);
		break;
	case BETA:
		pDist = new BetaDistribution( ((BetaDistribution*)_pPD)->getMin(),
			((BetaDistribution*)_pPD)->getMax(),
			((BetaDistribution*)_pPD)->getAlpha(),
			((BetaDistribution*)_pPD)->getBeta() );
		break;
	case CONSTANT:
		pDist = new ConstantDistribution( ((ConstantDistribution*)_pPD)->getMean() );
		break;
	case EXPONENTIAL:
		pDist = new ExponentialDistribution( ((ExponentialDistribution*)_pPD)->getLambda() );
		break;
	case NORMAL:
		pDist = new NormalDistribution( ((NormalDistribution*)_pPD)->getMean(),
			((NormalDistribution*)_pPD)->getStdDev(),
			((NormalDistribution*)_pPD)->getTruncation());
		break;
	case UNIFORM:
		pDist = new UniformDistribution( ((UniformDistribution*)_pPD)->getMin(),
			((UniformDistribution*)_pPD)->getMax() );
		break;
	case WEIBULL:
		pDist = new WeibullDistribution( ((WeibullDistribution*)_pPD)->getAlpha(),
			((WeibullDistribution*)_pPD)->getGamma(),
			((WeibullDistribution*)_pPD)->getMu());
		break;
	case GAMMA:
		pDist = new GammaDistribution( ((GammaDistribution*)_pPD)->getGamma(),
			((GammaDistribution*)_pPD)->getBeta(),
			((GammaDistribution*)_pPD)->getMu());
		break;
	case ERLANG:
		pDist = new ErlangDistribution( ((ErlangDistribution*)_pPD)->getGamma(),
			((ErlangDistribution*)_pPD)->getBeta(),
			((ErlangDistribution*)_pPD)->getMu() );
		break;
	case TRIANGLE:
		pDist = new TriangleDistribution( ((TriangleDistribution*)_pPD)->getMin(),
			((TriangleDistribution*)_pPD)->getMax(),
			((TriangleDistribution*)_pPD)->getMode());
		break;
	case EMPIRICAL:
		{
			pDist = new EmpiricalDistribution();
			char szDist[1024];
			((EmpiricalDistribution*)_pPD)->printDistribution( szDist );
			char* pStr = strstr( szDist, ":" ) + 1;
			((EmpiricalDistribution*)pDist)->setDistribution( pStr );
			break;
		}
	case HISTOGRAM:
		{
			pDist = new HistogramDistribution();
			char szDist[1024];
			((HistogramDistribution*)_pPD)->printDistribution( szDist );
			char* pStr = strstr( szDist, ":" ) + 1;
			((HistogramDistribution*)pDist)->setDistribution( pStr );
			break;
		}
	default:
		ASSERT(0);
	}

	return pDist;
}
