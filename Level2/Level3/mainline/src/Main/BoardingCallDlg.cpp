#include "stdafx.h"
#include "termplan.h"
#include "BoardingCallDlg.h"
#include "termplandoc.h"
#include "flightdialog.h"
#include "..\inputs\FlightConWithProcIDDatabase.h"
#include "..\common\probdistmanager.h"
#include "..\Inputs\procdata.h"
#include "..\inputs\probab.h"
#include "DlgProbDIst.h"
#include "ProcesserDialog.h"
#include "SelectALTObjectDialog.h"
#include <Common/ProbabilityDistribution.h>
#include "..\AirsideGUI\DlgStandFamily.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	DWORD m_dataPointer;
	TreeNodeDataWithType(){ m_type = TREE_NODE_INVALID; m_dataPointer = NULL; }
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
	ReloadAllStages();
	return TRUE;
}

void CBoardingCallDlg::OnSize( UINT nType, int cx, int cy )
{
}

InputTerminal* CBoardingCallDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}

void CBoardingCallDlg::OnButtonSave()
{
	CWaitCursor wc;
	GetInputTerminal()->flightData->resortBoardingCallDB();
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

void CBoardingCallDlg::ReloadAllStages()
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
		TreeNodeDataWithType* nodedata1 = new TreeNodeDataWithType();
		nodedata1->m_type = TREE_NODE_STAGE;
		nodedata1->m_dataPointer = (DWORD)i;
		m_tree.SetItemData(hTreeItemStage, (DWORD)nodedata1);

		BoardingCallFlightTypeDatabase* pFlightTypeDB = pFlightData->GetFlightTypeDBInStage(i);
		ReloadAllFlightTypes(pFlightTypeDB, hTreeItemStage);
	}
	m_tree.Expand(m_tree.GetRootItem(),TVE_EXPAND);
	return;
}

void CBoardingCallDlg::ReloadAllFlightTypes(BoardingCallFlightTypeDatabase* pFlightTypeDB, HTREEITEM hTreeItemStage)
{
	RemoveTreeSubItem(hTreeItemStage);
	CString strItemText;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	int flightCount = pFlightTypeDB->getCount();
	for(int iFlight=0; iFlight<flightCount; iFlight++)
	{
		CString strFlightType;
		BoardingCallFlightTypeEntry* pFlightEntry = (BoardingCallFlightTypeEntry*)pFlightTypeDB->getItem(iFlight);
		FlightConstraint* pFlightConst = (FlightConstraint*)(pFlightEntry->getConstraint());
		pFlightConst->getFullID(strFlightType.GetBuffer(64));
		strFlightType.ReleaseBuffer();
		strItemText.Format("Flight Type: %s", strFlightType);
		HTREEITEM hTreeItemFlight = m_tree.InsertItem(strItemText, cni, FALSE, FALSE, hTreeItemStage);
		TreeNodeDataWithType* nodedata2 = new TreeNodeDataWithType();
		nodedata2->m_type = TREE_NODE_FLIGHT_TYPE;
		nodedata2->m_dataPointer = (DWORD)pFlightEntry;
		m_tree.SetItemData(hTreeItemFlight, (DWORD)nodedata2);

		BoardingCallStandDatabase* pStandDB = pFlightEntry->GetStandDatabase();
		ReloadAllStand(pStandDB, hTreeItemFlight);

	}
}

void CBoardingCallDlg::ReloadAllStand(BoardingCallStandDatabase* pStandDB, HTREEITEM hTreeItemFlight)
{
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
		pStandProcID->printID(strStand.GetBuffer(64));
		strStand.ReleaseBuffer();
		strItemText.Format("Stand: %s", strStand);
		HTREEITEM hTreeItemStand = m_tree.InsertItem(strItemText, cni, FALSE, FALSE, hTreeItemFlight);
		TreeNodeDataWithType* nodedata3 = new TreeNodeDataWithType();
		nodedata3->m_type = TREE_NODE_STAND;
		nodedata3->m_dataPointer = (DWORD)pStandEntry;
		m_tree.SetItemData(hTreeItemStand, (DWORD)nodedata3);

		BoardingCallPaxTypeDatabase* pPaxDB = pStandEntry->GetPaxTypeDatabase();
		ReloadAllPaxTypes(pPaxDB, hTreeItemStand);

	}
}

void CBoardingCallDlg::ReloadAllPaxTypes(BoardingCallPaxTypeDatabase* pPaxDB, HTREEITEM hTreeItemStand)
{
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
		TreeNodeDataWithType* nodedata4 = new TreeNodeDataWithType();
		nodedata4->m_type = TREE_NODE_PASSENGER_TYPE;
		nodedata4->m_dataPointer = (DWORD)pPaxEntry;
		m_tree.SetItemData(hTreeItemPax, (DWORD)nodedata4);

		CString strTrigCount;
		strTrigCount.Format("Number of triggers: %d", 2000);
		HTREEITEM hTriggerAll = m_tree.InsertItem(strTrigCount, cni, FALSE, FALSE, hTreeItemPax);
		TreeNodeDataWithType* nodedata5 = new TreeNodeDataWithType();
		nodedata5->m_type = TREE_NODE_TRIGGER_ALL;
		nodedata5->m_dataPointer = NULL;
		m_tree.SetItemData(hTriggerAll, (DWORD)nodedata5);
		std::vector<BoardingCallTrigger>* vTriggers = pPaxEntry->GetTriggersDatabase();
		ReloadAllTriggers(vTriggers, hTriggerAll);

	}
}

void CBoardingCallDlg::ReloadAllTriggers(std::vector<BoardingCallTrigger>* vTriggers, HTREEITEM hTriggerAll)
{
	RemoveTreeSubItem(hTriggerAll);
	CString strItemText;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	int triggerCount = vTriggers->size();
	for(int iTrigger=0; iTrigger<triggerCount; iTrigger++)
	{
		strItemText.Format("Trigger: %d", (iTrigger+1));
		HTREEITEM hTrigger = m_tree.InsertItem(strItemText, cni, FALSE, FALSE, hTriggerAll);
		TreeNodeDataWithType* nodedata6 = new TreeNodeDataWithType();
		nodedata6->m_type = TREE_NODE_TRIGGER;
		nodedata6->m_dataPointer = (DWORD)iTrigger;
		m_tree.SetItemData(hTrigger, (DWORD)nodedata6);
		BoardingCallTrigger* trigger = &vTriggers->at(iTrigger);
		CString strTime, strProp;
		long seconds = trigger->GetTriggerTime().getPrecisely()/1000;
		strTime.Format("Time range before STD(s): %d", seconds);
		nodedata6 = new TreeNodeDataWithType();
		nodedata6->m_type = TREE_NODE_TRIGGER_TIME;
		nodedata6->m_dataPointer = (DWORD)seconds;
		HTREEITEM hTriggerTime = m_tree.InsertItem(strTime, cni, FALSE, FALSE, hTrigger);
		m_tree.SetItemData(hTriggerTime, (DWORD)nodedata6);

		double prop = trigger->GetTriggerProportion();
		strProp.Format("Proportion of Pax: %2f", prop);
		nodedata6 = new TreeNodeDataWithType();
		nodedata6->m_type = TREE_NODE_TRIGGER_TIME;
		nodedata6->m_dataPointer = (DWORD)prop;
		HTREEITEM hTriggerProp = m_tree.InsertItem(strProp, cni, FALSE, FALSE, hTrigger);
		m_tree.SetItemData(hTriggerProp, (DWORD)nodedata6);
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
		m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_EDIT, TRUE);
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
		m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_DELETE, TRUE);
		m_toolbar.GetToolBarCtrl().EnableButton(ID_BOARDING_CALL_EDIT, TRUE);
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

	FlightData* pFlightData = GetInputTerminal()->flightData;
	pFlightData->AddStage();
	ReloadAllStages();
	return;
}

void CBoardingCallDlg::OnToolbarButtonAddFlightType()
{
	HTREEITEM hSelItem = m_tree.GetSelectedItem();
	TreeNodeDataWithType* pDataWithType = (TreeNodeDataWithType*)m_tree.GetItemData(hSelItem);
	ASSERT(pDataWithType->m_type == TREE_NODE_STAGE);
}

void CBoardingCallDlg::OnToolbarButtonAddStand()
{
	HTREEITEM hSelItem = m_tree.GetSelectedItem();
	TreeNodeDataWithType* pDataWithType = (TreeNodeDataWithType*)m_tree.GetItemData(hSelItem);
	ASSERT(pDataWithType->m_type == TREE_NODE_FLIGHT_TYPE);
}

void CBoardingCallDlg::OnToolbarButtonAddPaxType()
{
	HTREEITEM hSelItem = m_tree.GetSelectedItem();
	TreeNodeDataWithType* pDataWithType = (TreeNodeDataWithType*)m_tree.GetItemData(hSelItem);
	ASSERT(pDataWithType->m_type == TREE_NODE_STAND);
}

void CBoardingCallDlg::OnToolbarButtonDel()
{
	HTREEITEM hSelItem = m_tree.GetSelectedItem();
	if(!hSelItem)
		return;

	if(hSelItem == m_hRoot)
	{
		//Delete All Stages
		BOOL b_YesNo = MessageBox("This operation will delete all stages, then \
a default stage will be generated, continue?","Delete All Stages", MB_YESNO|MB_ICONWARNING);
		if(b_YesNo == IDYES)
		{
			int nDBCount=GetInputTerminal()->flightData->GetStageCount();
			for(int i=0;i<nDBCount;i++)
			{
				GetInputTerminal()->flightData->DeleteOneStageByIndex(0);
			}
			GetInputTerminal()->flightData->AddStage();
			ReloadAllStages();
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
			HTREEITEM hPrevSiblingItem = m_tree.GetPrevSiblingItem(hSelItem);
			if(hPrevSiblingItem == NULL)
			{
				HTREEITEM hNextSiblingItem = m_tree.GetNextSiblingItem(hSelItem);
				if(hNextSiblingItem == NULL)
				{
					MessageBox("Can't delete the last one!");
					return;
				}
				m_tree.SelectItem(hNextSiblingItem);
			}
			int stageIndex = (int)pDataWithType->m_dataPointer;
			FlightData* pFlightData = GetInputTerminal()->flightData;
			pFlightData->DeleteOneStageByIndex(stageIndex);
			ReloadAllStages();
			break;
		}
	case TREE_NODE_FLIGHT_TYPE:
		break;
	case TREE_NODE_STAND:
		break;
	case TREE_NODE_PASSENGER_TYPE:
		break;
	case TREE_NODE_TRIGGER_ALL:
		break;
	case TREE_NODE_TRIGGER:
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
	TreeNodeDataWithType* pDataWithType = (TreeNodeDataWithType*)m_tree.GetItemData(hSelItem);
	if(!pDataWithType)
		return;
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
	if(message==UM_CEW_COMBOBOX_BEGIN)	
	{
		CWnd* pWnd=m_tree.GetEditWnd((HTREEITEM)wParam);
		CComboBox* pCB=(CComboBox*)pWnd;
		if(pCB->GetCount()==0)
		{
			//---------------------------------------------------------------------------------------
			CStringList strList;
			CString s;
			s.LoadString( IDS_STRING_NEWDIST );
			pCB->AddString(s);
			int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
			for( int m=0; m<nCount; m++ )
			{
				CProbDistEntry* pPBEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( m );			
				pCB->AddString(pPBEntry->m_csName);
			}
			//---------------------------------------------------------------------------------------

		}			
	}
	else if(message==UM_CEW_COMBOBOX_SELCHANGE)
	{
		HTREEITEM hItemSeled=(HTREEITEM)wParam;
		int nIndexSeled=m_tree.GetCmbBoxCurSel(hItemSeled);
		ChangeProbDist(hItemSeled,nIndexSeled);
		m_btnSave.EnableWindow(TRUE);
	}
	else if(message==UM_CEW_SHOWMENU_BEGIN)
	{
		COOLTREE_NODE_INFO* pCNI=(COOLTREE_NODE_INFO*)lParam;
		HTREEITEM hItem=(HTREEITEM)wParam;
		if(pCNI)
		{
			pCNI->unMenuID=IDR_BOARDING_CALL;
		}
	}
	else if(message==UM_CEW_SHOWMENU_READY)
	{
		CMenu* pMenu=(CMenu*)lParam;
		if(pMenu)
		{
		}
	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CBoardingCallDlg::ChangeProbDist(HTREEITEM hItem, int nIndexSeled)
{
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

