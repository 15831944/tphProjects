// ProcDataPage.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ProcDataPage.h"
#include "ProcDataSheet.h"
#include "ProcesserDialog.h"
#include "SelectStandDialog.h"
#include "common\exeption.h"
#include "inputs\paxflow.h"
#include "common\WinMsg.h"
#include "TermPlanDoc.h"
#include "inputs\MobileElemTypeStrDB.h"
#include "..\inputs\fltdata.h"
#include "../common/UnitsManager.h"

#include "..\engine\lineproc.h"
#include "..\engine\baggage.h"
#include "..\engine\hold.h"
#include "..\engine\MovingSideWalk.h"
#include "..\engine\Barrier.h"
#include "..\engine\ElevatorProc.h"
#include "..\engine\Conveyor.h"
#include "..\engine\IntegratedStation.h"
#include "..\engine\Stair.h"
#include "..\engine\Escalator.h"
#include ".\procdatapage.h"
#include "engine\ElevatorProc.h"

#include "..\inputs\MobileElemTypeStrDB.h"
#include "..\inputs\CapacityAttributes.h"
#include "Floor2.h"


#include "..\engine\proclist.h"

#include "DlgBehaviorCapacityCombinationNameDefine.h"
#define MOBILE_ELEMENT_MENU_START 0x10000
#define MOBILE_ELEMENT_MENU_COMBINATION 0x11000
#define MOBILE_ELEMENT_MENU_ALLNOPAX 0x11100
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define  STR_ALL_NOPAX _T("ALL NON-PAX") 
#define  STR_COMBINATION _T("COMBINATION")
/////////////////////////////////////////////////////////////////////////////
// CProcDataPage property page
UINT nIDCaption[] = { IDS_POINT_PROCESSOR,
IDS_DEPENDENT_SOURCE,
IDS_DEPENDENT_SINK,
IDS_LINE_PROCESSOR,
IDS_BAGGAGE_DEVICE,
IDS_HOLDING_AREA,
IDS_GATE_PROCESSOR,
IDS_FLOORCHANGE_PROCESSOR,
IDS_BARRIER_PROCESSOR,
IDS_STATION_PROCESSOR,
IDS_MOVINGSIDEWALK_PROCESSOR,
IDS_ELEVATOR_PROCESSOR,
IDS_CONVEY_ROCESSOR,
IDS_STAIR_PROCESSOR,
IDS_ESCALATE_PROCESSOR,
IDS_BILLBOARD_PROCESSOR,
IDS_BRIDGECONNECTOR_PROCESSOR,
IDS_RETAIL_PROCESSOR,






//////////////////////////////////////////////////////////////////////////
//add new terminal item before this line

IDS_ARP_PROCESSOR,
IDS_RUNWAY_PROCESSOR,
IDS_TAXIWAY_PROCESSOR,
IDS_STAND_PROCESSOR,
IDS_NODE_PROCESSOR,
IDS_DEICEBAY_PROCESSOR,
IDS_FIX_PROCESSOR,
};

IMPLEMENT_DYNCREATE(CProcDataPage, CResizablePage)

CProcDataPage::CProcDataPage() : CResizablePage(CProcDataPage::IDD)
{
	
	//{{AFX_DATA_INIT(CProcDataPage)
	//}}AFX_DATA_INIT
	m_hLinkage=NULL;
	m_hPSS=NULL;

	m_hTreeItemCapacity = NULL;
	m_hApplyServiceTimeAfterGreeting = NULL;
}

CProcDataPage::CProcDataPage( enum PROCDATATYPE _enumProcDataType, InputTerminal* _pInTerm, const CString& _csProjPath )
: CResizablePage(CProcDataPage::IDD, nIDCaption[(int)_enumProcDataType] )
{
	m_pInTerm = _pInTerm;
	m_csProjPath = _csProjPath;
	m_nProcDataType = _enumProcDataType;
	m_hLinkage=NULL;
	m_hPSS=NULL;
	m_hGreetingHall=NULL;

	m_hTreeItemCapacity = NULL;
	m_hApplyServiceTimeAfterGreeting = NULL;

	m_hCorralService = NULL;
	m_hTimeToCorralService = NULL;
}

CProcDataPage::~CProcDataPage()
{
}

void CProcDataPage::DoDataExchange(CDataExchange* pDX)
{
	CResizablePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcDataPage)
	DDX_Control(pDX, IDC_STATIC_LINKAGE, m_staticLinkage);
	DDX_Control(pDX, IDC_TREE_DATA, m_TreeData);
	DDX_Control(pDX, IDC_LIST_PROCESSOR, m_listboxProcessor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProcDataPage, CResizablePage)
//{{AFX_MSG_MAP(CProcDataPage)
	ON_LBN_SELCHANGE(IDC_LIST_PROCESSOR, OnSelchangeListProcessor)
	ON_BN_CLICKED(ID_PROCESSOR_DATA_ADD, OnButtonAdd)
	ON_BN_CLICKED(ID_PROCESSOR_DATA_DEL, OnButtonDel)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DATA, OnSelchangedTreeData)
	ON_WM_SIZE()
	ON_COMMAND(ID_LINKAGE_ONETOONE, OnLinkageOnetoone)
	ON_COMMAND(ID_EDIT_OR_SPIN_VALUE, OnEditOrSpinValue)
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipText )
	//}}AFX_MSG_MAP
//	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
//	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_COMMAND_RANGE(MOBILE_ELEMENT_MENU_START, MOBILE_ELEMENT_MENU_START + 100, OnSelectMobileElementType)
	ON_COMMAND(MOBILE_ELEMENT_MENU_ALLNOPAX,OnSelAllNoPax) 
	ON_COMMAND(MOBILE_ELEMENT_MENU_COMBINATION,OnSelCombination)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void CProcDataPage::OnSelAllNoPax()
{
	MiscData* pMiscData = GetCurMiscData();
	if (pMiscData == NULL || hCurrentTreeItem == NULL) return;
	if(m_TreeData.GetParentItem(hCurrentTreeItem) == m_hTreeItemCapacity)
	{
		CapacityAttribute* pAttribute = (CapacityAttribute*)m_TreeData.GetItemNodeInfo(hCurrentTreeItem)->nMaxCharNum ;
		if(pAttribute != NULL && pAttribute->m_Type == CapacityAttributes::TY_Combination)
		{
			pAttribute->AddCombinationContainTypeString(CString(STR_ALL_NOPAX)) ;

			COOLTREE_NODE_INFO cni;
			CCoolTree::InitNodeInfo(this,cni);
			cni.nt=NT_NORMAL;
			cni.net = NET_NORMAL ;
			m_TreeData.InsertItem(CString(STR_ALL_NOPAX) , 
				cni, FALSE, FALSE, hCurrentTreeItem);
			return ;
		}
	}
	if(hCurrentTreeItem != m_hTreeItemCapacity)
		return ;
	//Remove NO LIMIT 
	HTREEITEM hTreeItemCapacityChild =  m_TreeData.GetChildItem(m_hTreeItemCapacity);
	if (0 ==m_TreeData.GetItemText(hTreeItemCapacityChild).CompareNoCase(_T("No Limit")))
		m_TreeData.DeleteItem(hTreeItemCapacityChild);

	CapacityAttributes* pAttributes = pMiscData->GetCapacityAttributes();
	CapacityAttribute* pAttribute = new CapacityAttribute;
	pAttribute->m_nCapacity = 1;
	pAttribute->m_nMobElementIndex = -1 ;
	pAttribute->m_Type = CapacityAttributes::TY_AllNO_PAX ;
	pAttribute->m_Name = STR_ALL_NOPAX ;
	pAttributes->AddCapacityAttribute(pAttribute);


	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL;
	cni.net=NET_EDITSPIN_WITH_VALUE;
		cni.nMaxCharNum = (DWORD_PTR)pAttribute ;
	CString strLabel;
	strLabel.Format("%s:  %d",STR_ALL_NOPAX, 1);//Fibre

	HTREEITEM hTreeItemPaxCapacity = m_TreeData.InsertItem(strLabel , 
		cni, FALSE, FALSE, m_hTreeItemCapacity);

	


	m_TreeData.Expand(m_hTreeItemCapacity, TVE_EXPAND);


	m_TreeData.SetItemData(hTreeItemPaxCapacity,1);
}
void CProcDataPage::OnSelCombination()
{
	MiscData* pMiscData = GetCurMiscData();
	if (pMiscData == NULL) return;

	CDlgBehaviorCapacityCombinationNameDefine dlg ;
	dlg.DoModal() ;

	//Remove NO LIMIT 
	HTREEITEM hTreeItemCapacityChild =  m_TreeData.GetChildItem(m_hTreeItemCapacity);
	if (0 ==m_TreeData.GetItemText(hTreeItemCapacityChild).CompareNoCase(_T("No Limit")))
		m_TreeData.DeleteItem(hTreeItemCapacityChild);

	CapacityAttributes* pAttributes = pMiscData->GetCapacityAttributes();
	CapacityAttribute* pAttribute = new CapacityAttribute;
	pAttribute->m_nCapacity = 1;
	pAttribute->m_nMobElementIndex = -1 ;
	pAttribute->m_Type = CapacityAttributes::TY_Combination ;
	pAttribute->m_Name = dlg.GetSelectName() ;
	pAttributes->AddCapacityAttribute(pAttribute);


	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL;
	cni.net=NET_EDITSPIN_WITH_VALUE;
			cni.nMaxCharNum = (DWORD_PTR)pAttribute ;
	CString strLabel;
	strLabel.Format("%s:  %d",dlg.GetSelectName(), 1);//Fibre

	HTREEITEM hTreeItemPaxCapacity = m_TreeData.InsertItem(strLabel , 
		cni, FALSE, FALSE, m_hTreeItemCapacity);
	m_TreeData.Expand(m_hTreeItemCapacity, TVE_EXPAND);
	m_TreeData.SetItemData(hTreeItemPaxCapacity,1);
}
/////////////////////////////////////////////////////////////////////////////
// CProcDataPage message handlers

BOOL CProcDataPage::OnInitDialog() 
{
	CResizablePage::OnInitDialog();
	// TODO: Add extra initialization here
	InitToolbar();
	m_csTreeItemLabel = "";
	

	LoadProcessorList();

	//Init FlowConvetor,use to update Flow( only temp)
	InitFlowConvetor();
	/////////////////////////////////////////////////
	InitSpiltter();
	//Left Wnd
	AddAnchor(m_ToolBarLeft, CSize(0,0),CSize(40,0));
	AddAnchor(IDC_LIST_PROCESSOR, TOP_LEFT,CSize(40,100));

	//Right Wnd
	AddAnchor(m_wndSplitter,CSize(40,0),CSize(40,100));
	AddAnchor(IDC_STATIC_LINKAGE,CSize(40,0),CSize(100,0));
	AddAnchor(m_ToolBar, CSize(40,0),CSize(100,0));
	AddAnchor(IDC_TREE_DATA, CSize(40,0),CSize(100,100));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CProcDataPage::SetControl()
{
}



void CProcDataPage::ReLoadDetailed()
{
	ClearControl();
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;

	// disallow group flag
	if( pMiscData->GetDisallowGroupFlag() )
		m_TreeData.SetCheckStatus(m_hDisallowGroups,TRUE);
	else
		m_TreeData.SetCheckStatus(m_hDisallowGroups,FALSE);
    
	if(pMiscData->GetWaitInQueue())
		m_TreeData.SetCheckStatus(m_hWaitInQueue,TRUE);
	else
		m_TreeData.SetCheckStatus(m_hWaitInQueue,FALSE);

	ReloadGateList( pMiscData );
	ReloadLinkedDestList(pMiscData);
	ReloadWaitAreaList(pMiscData, m_nProcDataType == ConveyorProc);
	ReloadLinkProc1List( pMiscData );
	ReloadStationList(pMiscData);
	ReloadGrouSerTimeMod(pMiscData);

	ReloadDependentProcessorList( pMiscData );

	if ( m_nProcDataType == BaggageProc)
	{
		ReloadHoldArerList(pMiscData);
	}
	if ( m_nProcDataType == ConveyorProc )
	{
		ReloadExitBeltList(pMiscData);
	}

	if (m_nProcDataType == GateProc)
	{
		ReloadBridgeConnectorList(pMiscData);
	}

	if (m_nProcDataType == BridgeConnectorProc)
	{
		ReloadStandConnectorList(pMiscData);
	}

	SetValue(pMiscData);
}

// set control to the default.
// enable or disable the control
void CProcDataPage::ClearControl()
{
	BOOL bEnable = FALSE;
	int nIdx = m_listboxProcessor.GetCurSel();
	if( nIdx != LB_ERR )
		bEnable = TRUE;	
	m_ToolBarLeft.GetToolBarCtrl().EnableButton( ID_PROCESSOR_DATA_DEL,bEnable );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, bEnable );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE );
	if(m_hLinkage==NULL) return;
	switch(m_nProcDataType)
	{
	case BaggageProc:
		m_TreeData.SetItemText(m_hCapOrDR,"Capacity:  5");
		m_TreeData.SetItemData(m_hCapOrDR,5);
		break;
	case HoldAreaProc:
		m_TreeData.SetItemText(m_hCapOrDR,"Distribution Radius:  5");
		m_TreeData.SetItemData(m_hCapOrDR,5);
		break;
	}	
	
	m_TreeData.SetCheckStatus(m_hDisallowGroups,FALSE);
	m_TreeData.SetCheckStatus(m_hWaitInQueue,TRUE);
}

void CProcDataPage::OnSelchangeListProcessor() 
{
	if(m_hLinkage==NULL)
	{
		SetTree();
	}
	ReloadCapacityList();
	// TODO: Add your control notification handler code here
	ReLoadDetailed();	
}




// Hightlight the gate in the list. if not exist, return false.
bool CProcDataPage::SelectGate(ProcessorID _id)
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return false;
	
	if( m_hGateItem == NULL )
		return false;

	ProcessorIDList* pProcIDList = pMiscData->getGateList();
	HTREEITEM hItem = m_TreeData.GetChildItem( m_hGateItem );
	while( hItem )
	{
		int nDBIdx = m_TreeData.GetItemData( hItem );

		const ProcessorID* pProcID = pProcIDList->getID( nDBIdx );
		if( *pProcID == _id )
		{
			m_TreeData.SelectItem( hItem );
			return true;
		}
		hItem = m_TreeData.GetNextItem( hItem, TVGN_NEXT );
	}	
	return false;
}

bool CProcDataPage::SelectStand(const ProcessorID& _id)
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return false;

	if( m_hGateItem == NULL )
		return false;

	MiscProcessorIDList* pProcIDList = ((MiscBridgeConnectorData*)pMiscData)->GetBridgeConnectorLinkedProcList();
	HTREEITEM hItem = m_TreeData.GetChildItem( m_hStandConnect );
	while( hItem )
	{
		int nDBIdx = m_TreeData.GetItemData( hItem );

		const ProcessorID* pProcID = pProcIDList->getID( nDBIdx );
		if( *pProcID == _id )
		{
			m_TreeData.SelectItem( hItem );
			return true;
		}
		hItem = m_TreeData.GetNextItem( hItem, TVGN_NEXT );
	}	
	return false;
}
//------------- Link Proc Functions -------------------------------
ProcessorIDList* CProcDataPage::GetLinkProc1List(  MiscData* _pMiscData  )
{
	ProcessorIDList* pProcIDList = NULL;
	switch( m_nProcDataType )
	{
	case DepSourceProc:
	case DepSinkProc:
		pProcIDList = ((MiscDependentData*)_pMiscData)->getDependentList();
		break;
	case BaggageProc:
		pProcIDList = ((MiscBaggageData*)_pMiscData)->getBypassList ();
		break;
	case FloorChangeProc:
		pProcIDList = ((MiscFloorChangeData*)_pMiscData)->getConnections ();
		break;
	case BridgeConnectorProc:
		pProcIDList = ((MiscBridgeConnectorData*)_pMiscData)->GetBridgeConnectorLinkedProcList();
		break;
	}
	return pProcIDList;
}



// Hightlight the link proc in the list. if not exist, return false.
bool CProcDataPage::SelectLinkProc1(ProcessorID _id)
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return false;
	
	ProcessorIDList* pProcIDList = GetLinkProc1List( pMiscData );
	if( pProcIDList == NULL )
		return false;

	if( !m_hProc1Item )
		return false;

	HTREEITEM hItem = m_TreeData.GetChildItem( m_hProc1Item );
	while( hItem )
	{
		int nDBIdx = m_TreeData.GetItemData( hItem );

		const ProcessorID* pProcID = pProcIDList->getID( nDBIdx );
		if( *pProcID == _id )
		{
			m_TreeData.SelectItem( hItem );
			return true;
		}
		hItem = m_TreeData.GetNextItem( hItem, TVGN_NEXT );
	}	
	return false;
}


//------------------- value related functions ---------------------
void CProcDataPage::SetValue( MiscData* _pMiscData )
{
	if(	_pMiscData )
	{
		int nCyclicFreq = _pMiscData->GetCyclicFreq();
		CString str;
		str.Format("Recycle Frequency:  %d",nCyclicFreq);
		m_TreeData.SetItemText(m_hRecycleFreq,str);
		m_TreeData.SetItemData(m_hRecycleFreq,nCyclicFreq);
	}
	UpdateData( FALSE );

	CString csVal;
	switch( m_nProcDataType )
	{
	case BaggageProc:
		{
			float fValue=!_pMiscData?(float)20.0:static_cast<float>(((MiscBaggageData*)_pMiscData)->getCapacity());
			csVal.Format( "Capacity:  %.0f", fValue);
			m_TreeData.SetItemText(m_hCapOrDR,csVal);
			m_TreeData.SetItemData(m_hCapOrDR,static_cast<DWORD>(fValue));
		}
		break;
	case EscalatorProc:
		{
			float fValue=static_cast<float>(((MiscEscalatorData*)_pMiscData)->GetMovingSpeed());
			csVal.Format("Speed ( m/s ) : %.1f",fValue);
			m_TreeData.SetItemText(m_hEscalatorSpeed,csVal);
			m_TreeData.SetItemData(m_hEscalatorSpeed,static_cast<DWORD>(fValue*100));
		}
		break;
	case Elevator:
		{
			float fValue=static_cast<float>(((MiscElevatorData*)_pMiscData)->getSpeed(MiscElevatorData::EST_ACCELERATION));
			csVal.Format("Acceleration Speed ( m/s ) : %.1f",fValue);
			m_TreeData.SetItemText(m_hAccSpeedItem,csVal);
			m_TreeData.SetItemData(m_hAccSpeedItem,static_cast<DWORD>(fValue*100));
			
			fValue=static_cast<float>(((MiscElevatorData*)_pMiscData)->getSpeed(MiscElevatorData::EST_NORMAL));
			csVal.Format("Normal Speed ( m/s ) : %.1f",fValue);
			m_TreeData.SetItemText(m_hNormalSpeedItem,csVal);
			m_TreeData.SetItemData(m_hNormalSpeedItem,static_cast<DWORD>(fValue*100));
			
			fValue=static_cast<float>(((MiscElevatorData*)_pMiscData)->getSpeed(MiscElevatorData::EST_DECELERATION));
			csVal.Format("Deceleration Speed ( m/s ) : %.1f",fValue);
			m_TreeData.SetItemText(m_hDecSpeedItem,csVal);
			m_TreeData.SetItemData(m_hDecSpeedItem,static_cast<DWORD>(fValue*100));
			
			int nMinStopFloor = -1;
			int nMaxStopFloor = -1;
	
			MiscProcessorData* pMiscDB = m_pInTerm->miscData->getDatabase( (int)m_nProcDataType );
			int nMiscDataCount = pMiscDB->getCount();
			for (int i = 0; i < nMiscDataCount; ++i)
			{
				MiscDataElement* pMiscDataElement = (MiscDataElement*)pMiscDB->getItem(i);
				if (_pMiscData == pMiscDataElement->getData())
				{
					const ProcessorID* pProcessorID  =  pMiscDataElement->getID();
					CString str = pProcessorID->GetIDString();
					TRACE( "%s\n", str );
					ElevProcessorFamily* tempProcessorFamily=NULL;
					tempProcessorFamily=(ElevProcessorFamily*)(m_pInTerm->procList->getFamilyIndex( *pProcessorID ));
					tempProcessorFamily->GetMaxMinFloorOfFamily(m_pInTerm->procList,nMaxStopFloor,nMinStopFloor);

				}
			}
			
			if (((MiscElevatorData*)_pMiscData)->getStopAfFloorCount() == 0)
			{
				CProcDataSheet* pWnd=(CProcDataSheet*)GetParent();
				CView* pView=(CView*)(pWnd->m_pParent);

				//get the count of the floor
				CTermPlanDoc* pDoc	= (CTermPlanDoc*)(pView->GetDocument());
				int nFloorNumber=pDoc->GetCurModeFloor().m_vFloors.size();

				MiscData* pMiscData = GetCurMiscData();
				((MiscElevatorData*)pMiscData)->clearStopAtFloor();
				for(int i=0;i<nFloorNumber;i++)
				{
					BOOL bStop = FALSE;
					if (i <= nMaxStopFloor && i >= nMinStopFloor)
						bStop = TRUE;

					((MiscElevatorData*)pMiscData)->addStopAtFloor(bStop);
				}
			}
			else
			{	
				HTREEITEM hOld=m_hStopAtFloor;
				COOLTREE_NODE_INFO cni;
				CCoolTree::InitNodeInfo(this,cni);
				cni.net=NET_NORMAL;
				m_hStopAtFloor=m_TreeData.InsertItem("Stop At Floor",cni,FALSE,FALSE,m_hOtherBehavior,m_hRecycleFreq/*m_hCapOrDR*/);
				CString strFloor;
				cni.nt=NT_CHECKBOX;
				cni.net=NET_NORMAL;
				CProcDataSheet* pWnd=(CProcDataSheet*)GetParent();
				CView* pView=(CView*)(pWnd->m_pParent);
				CTermPlanDoc* pDoc	= (CTermPlanDoc*)(pView->GetDocument());
				CFloorList& floorList = pDoc->GetCurModeFloor().m_vFloors;
				for(int i = nMinStopFloor; i <= nMaxStopFloor; i++)
				{
					CFloor2* pFloor = floorList.at(i);
					strFloor.Format("Floor %s",pFloor->FloorName());
					m_TreeData.InsertItem(strFloor,cni,((MiscElevatorData*)_pMiscData)->getStopAtFloor(i),FALSE,m_hStopAtFloor);
				}
				m_TreeData.SelectItem(NULL);
				m_TreeData.DeleteItem(hOld);
			}

		}
		break;
	case HoldAreaProc:
		{
			float fValue=!_pMiscData?(float)5.0: static_cast<float>(((MiscHoldAreaData*)_pMiscData)->getRadius());
			csVal.Format( "Distribution Radius:  %.1f", fValue);
			m_TreeData.SetItemText(m_hCapOrDR,csVal);
			m_TreeData.SetItemData(m_hCapOrDR,static_cast<DWORD>(fValue));
			long lCapacity= ((MiscHoldAreaData*)_pMiscData)->GetStageNumber();
			bool bApplyServiceTimeAfterGreeting = ((MiscHoldAreaData*)_pMiscData)->getApplyServiceTimeAfterGreeting();
			if(lCapacity!=0)
			{
				m_TreeData.SetCheckStatus(m_hBCSN,TRUE);
				CString strCapacity;
				strCapacity.Format("Boarding Call Stage Number:  %d",lCapacity);
				m_TreeData.SetItemText(m_hBCSN,strCapacity);
				m_TreeData.SetItemData(m_hBCSN,lCapacity);
				
			}
			else
			{
				m_TreeData.SetCheckStatus(m_hBCSN,FALSE);
				m_TreeData.SetItemText(m_hBCSN,"Boarding Call Stage Number:  1");	
				m_TreeData.SetItemData(m_hBCSN,1);
				
			}
			// greeting time
			m_TreeData.SetCheckStatus( m_hApplyServiceTimeAfterGreeting, bApplyServiceTimeAfterGreeting? TRUE : FALSE );

			long lValue = ((MiscHoldAreaData*)_pMiscData)->getTimeToService();
			bool bValue = ((MiscHoldAreaData*)_pMiscData)->IsCorralService();
			
			m_TreeData.SetCheckStatus(m_hCorralService,bValue ? TRUE : FALSE);
			
			
		
			CString strTime;
			strTime.Format("Start service after waiting (Seconds): %d", lValue);
			m_TreeData.SetItemText(m_hTimeToCorralService,strTime);
			m_TreeData.SetItemData(m_hTimeToCorralService,lValue);

			if(bValue)
			{
				m_TreeData.SetEnableStatus(m_hTimeToCorralService,TRUE);
				m_TreeData.Expand(m_hCorralService, TVE_EXPAND);
			}
			else
				m_TreeData.SetEnableStatus(m_hTimeToCorralService,FALSE);

		}
		break;
	case ConveyorProc:
		{
			// set type
			enum SUBCONVEYORTYPE eType = _pMiscData ? ((MiscConveyorData*)_pMiscData)->GetSubType() : SIMPLE_CONVEYOR;
			switch( eType ) 
			{
			case LOADER:
				m_TreeData.SetRadioStatus( m_hItemLoader, TRUE );
				break;
			case SIMPLE_CONVEYOR:
				m_TreeData.SetRadioStatus( m_hItemSimpleConveyor, TRUE );
				break;
			case UNIT_BELT:
				m_TreeData.SetRadioStatus( m_hItemUnitBelt, TRUE );
				break;
			case MERGE_BOX:
				m_TreeData.SetRadioStatus( m_hItemMergeBox, TRUE );
				break;
			case FEEDER:
				m_TreeData.SetRadioStatus( m_hItemFeeder, TRUE );
				break;
			case SCANNER:
				m_TreeData.SetRadioStatus( m_hItemScanner, TRUE );
				break;
			case SPLITER:
				m_TreeData.SetRadioStatus( m_hItemSplitter, TRUE );
				break;
			case PUSHER:
				m_TreeData.SetRadioStatus( m_hItemPusher, TRUE );
				break;
			case FLOW_BELT:
				m_TreeData.SetRadioStatus( m_hItemFlowBelt, TRUE );
				break;
			case SORTER:
				m_TreeData.SetRadioStatus( m_hItemSorter, TRUE );
				break;
			}	

			if( _pMiscData && ((MiscConveyorData*)_pMiscData)->GetPusherReleasedByNextAvail() )
			{
				m_TreeData.SetRadioStatus( m_hNextProcAvail, TRUE );
				m_TreeData.SetRadioStatus( m_hOtherCondition, FALSE );
			}
			else
			{
				m_TreeData.SetRadioStatus( m_hNextProcAvail, FALSE );
				m_TreeData.SetRadioStatus( m_hOtherCondition, TRUE );

				if( _pMiscData && ((MiscConveyorData*)_pMiscData)->GetPusherReleasedByFull() )
					m_TreeData.SetCheckStatus( m_hConveyorIsFull, TRUE );
				else 
					m_TreeData.SetCheckStatus( m_hConveyorIsFull, FALSE );
			
				if( _pMiscData && ((MiscConveyorData*)_pMiscData)->GetPusherReleasedByMaxTime() )
					m_TreeData.SetCheckStatus( m_hRandomAmountOfTimePasses, TRUE );
				else 
					m_TreeData.SetCheckStatus( m_hRandomAmountOfTimePasses, FALSE );
				
				if( _pMiscData && ((MiscConveyorData*)_pMiscData)->GetPusherReleasedBySchedPick() )
					m_TreeData.SetCheckStatus( m_hScheduledPickedUp, TRUE );
				else 
					m_TreeData.SetCheckStatus( m_hScheduledPickedUp, FALSE );
				
			}
			if( _pMiscData && ((MiscConveyorData*)_pMiscData)->GetReversible() )
				m_TreeData.SetCheckStatus( m_hReversible, TRUE );
			else 
				m_TreeData.SetCheckStatus( m_hReversible, FALSE );
			
			DistanceUnit dValue= _pMiscData ?  ((MiscConveyorData*)_pMiscData)->GetSpeed() : (1.0 * SCALE_FACTOR) ;
			dValue = UNITSMANAGER->ConvertLength( dValue );
			csVal.Format( "Speed (%s/s): %.2f", UNITSMANAGER->GetLengthUnitString(), dValue );
			m_TreeData.SetItemText( m_hItemSpeed, csVal );
			m_TreeData.SetItemData( m_hItemSpeed, static_cast<DWORD>(dValue*100 ));
			m_TreeData.SetItemValueRange( m_hItemSpeed,1,100 );
			
			long lCapacity=_pMiscData->GetQueueCapacity();
			if( lCapacity == -1 )
			{
				m_TreeData.SetEnableStatus( m_hCapacityFather, TRUE );				
				m_TreeData.SetRadioStatus( m_hAutoCapacity, TRUE );
			}
			else
			{
				m_TreeData.SetEnableStatus( m_hCapacityFather, TRUE );
				m_TreeData.SetRadioStatus( m_hCapacity, TRUE );
				csVal.Format("Capacity:  %d",lCapacity);
				m_TreeData.SetItemText(m_hCapacity,csVal);
				m_TreeData.SetItemData(m_hCapacity,lCapacity);
				m_TreeData.SetItemValueRange( m_hCapacity, 1, 1000 );
			}
			

			ElapsedTime time = ((MiscConveyorData*)_pMiscData)->GetRandomAmountTime();
			csVal.Format( "Random amount of time Passes (s) :  %d", (int)time.asSeconds() );
			m_TreeData.SetItemText( m_hRandomAmountOfTimePasses, csVal );
			m_TreeData.SetItemData( m_hRandomAmountOfTimePasses, time.asSeconds() );

			time = ((MiscConveyorData*)_pMiscData)->GetScheduleTime();
			csVal.Format( "Scheduled picked up (s) :  %d", (int)time.asSeconds() );
			m_TreeData.SetItemText( m_hScheduledPickedUp, csVal );
			m_TreeData.SetItemData( m_hScheduledPickedUp, time.asSeconds() );
		}
		break;
	case RetailProc:
		{
			//ASSERT(0);

		}
		break;
	}

// 	long lCapacity=_pMiscData->GetQueueCapacity();
// 	if(lCapacity>0)
// 	{
// 		m_TreeData.SetCheckStatus(m_hPopulationCapacity,TRUE);
// 		CString strCapacity;
// 		strCapacity.Format("Population Capacity:  %d",lCapacity);
// 		m_TreeData.SetItemText(m_hPopulationCapacity,strCapacity);
// 		m_TreeData.SetItemData(m_hPopulationCapacity,lCapacity);
// 				
// 	}
// 	else
// 	{
// 		m_TreeData.SetCheckStatus(m_hPopulationCapacity,FALSE);
// 		m_TreeData.SetItemText(m_hPopulationCapacity,"Population Capacity:  1");	
// 		m_TreeData.SetItemData(m_hPopulationCapacity,1);
// 		
// 	}

	long lTime = _pMiscData->GetTerminateTime();
	if(lTime >= 0)
	{
		m_TreeData.SetCheckStatus(m_hTerminateTime, TRUE);
		CString strTime;
		strTime.Format("Terminate when time to departure less than (Min):  %d",lTime);
		m_TreeData.SetItemText(m_hTerminateTime,strTime);
		m_TreeData.SetItemData(m_hTerminateTime,lTime);
				
	}
	else
	{
		m_TreeData.SetCheckStatus(m_hTerminateTime,FALSE);
		m_TreeData.SetItemText(m_hTerminateTime,"Terminate when time to departure less than (Min):  0");	
		m_TreeData.SetItemData(m_hTerminateTime,0);		
	}

	m_TreeData.RedrawWindow();
}

BOOL CProcDataPage::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_pInTerm->miscData->saveDataSet( m_csProjPath, true );
	m_pInTerm->procList->setMiscData(*(m_pInTerm->miscData));
	//Update Detail Gate Flow
	UpdateFlow();
	//////////////////////////////////////////////////////
	return CResizablePage::OnApply();
}

void CProcDataPage::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	AfxGetApp()->BeginWaitCursor();
	try
	{
		m_pInTerm->miscData->loadDataSet( m_csProjPath );
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;			
	}
	AfxGetApp()->EndWaitCursor();
	
	CResizablePage::OnCancel();
}

MiscData* CProcDataPage::GetCurMiscData()
{
	int nItemIdx = m_listboxProcessor.GetCurSel();
	if( nItemIdx == LB_ERR )
		return NULL;
	
	int nProcIdx = m_listboxProcessor.GetItemData( nItemIdx );
	
	MiscProcessorData* pMiscDB = m_pInTerm->miscData->getDatabase( (int)m_nProcDataType );
	MiscData* pMiscData = ((MiscDataElement*)pMiscDB->getItem( nProcIdx ))->getData();
	return pMiscData;
}



// ------------------- entry proc related functions -------------------------

void CProcDataPage::LoadProcessorList()
{
	m_listboxProcessor.ResetContent();
	MiscProcessorData* pMiscDB = m_pInTerm->miscData->getDatabase( (int)m_nProcDataType );
	int nProcCount = pMiscDB->getCount();
	for( int i=0; i < nProcCount; i++ )
	{
		char str[80];
		const ProcessorID *procID = pMiscDB->getProcName (i);

        procID->printID (str);
		int nIdx = m_listboxProcessor.AddString( str );
		m_listboxProcessor.SetItemData( nIdx, i );				
	}

	m_listboxProcessor.SetCurSel( -1 );
	ReLoadDetailed();	
}



// add new entry
void CProcDataPage::OnButtonAdd() 
{
	// TODO: Add your control notification handler code here
	CProcesserDialog dlg( m_pInTerm );
	dlg.SetType( m_nProcDataType );
	if( dlg.DoModal() == IDCANCEL )
		return;
	
	MiscProcessorData* pMiscDB = m_pInTerm->miscData->getDatabase( (int)m_nProcDataType );
	if( pMiscDB == NULL )
		return;	

	ProcessorID id;

	ProcessorIDList idList;
	ProcessorIDList idListShow;
	
	if( !dlg.GetProcessorIDList(idList) )
		return;	
	idListShow.clear();
	
	int nIDcount = idList.getCount();
	for(int i = 0; i < nIDcount; i++)
	{
		if (!SelectEntryProc(*(idList.getID(i))))  // check if exist
		{
			idListShow.Add(idList[i]);
		}
	}

	nIDcount = idListShow.getCount();
	for (i = 0; i < nIDcount; i++ )
	{
		id = *(idListShow.getID(i));	
        		       
		OnSelchangeListProcessor();

		MiscDataElement* pDataElement = new MiscDataElement( id );
		
		MiscData* pMiscData = NULL;
		switch( m_nProcDataType )
		{
		case PointProc:
			pMiscData = new MiscData;
			break;
		case DepSourceProc:
			pMiscData = new MiscDepSourceData;
			break;
		case DepSinkProc:
			pMiscData = new MiscDepSinkData;
			break;
		case LineProc:
			pMiscData = new MiscLineData;
			break;
		case BaggageProc:
			pMiscData = new MiscBaggageData;
			break;
		case HoldAreaProc:
			pMiscData = new MiscHoldAreaData;
			break;
		case GateProc:
			pMiscData = new MiscGateData;
			break;
		case FloorChangeProc:
			pMiscData = new MiscFloorChangeData;
			break;
		case BarrierProc:
			break;		
		case MovingSideWalkProc:
			pMiscData = new MiscMovingSideWalkData;
			break;
		case Elevator:
			pMiscData = new MiscElevatorData;
			break;
		case ConveyorProc:
			pMiscData = new MiscConveyorData;
			break;
		case StairProc:
			pMiscData = new MiscStairData;
			break;
		case EscalatorProc:
			pMiscData = new MiscEscalatorData;
			break;
		case IntegratedStationProc:
			pMiscData = new MiscIntegratedStation;
			break;
		case BillboardProcessor:
			pMiscData = new MiscBillboardData;
			break;
		case BridgeConnectorProc:
			pMiscData = new MiscBridgeConnectorData;
			break;
		case RetailProc:
			pMiscData = new MiscRetailProcData;
			break;


		default:
			{
				ASSERT(0);
			}


		}		
		
		pDataElement->setData( pMiscData );
		
		pMiscDB->addItem( pDataElement );
		
	}
	if ( nIDcount > 0)
	{
		LoadProcessorList();
		SelectEntryProc( id );
		SetModified();

		OnSelchangeListProcessor();
	}
}


// Hightlight the proc in the list. if not exist, return false.
bool CProcDataPage::SelectEntryProc(ProcessorID _id)
{
	MiscProcessorData* pMiscDB = m_pInTerm->miscData->getDatabase( m_nProcDataType );
	if( pMiscDB == NULL )
		return false;
	
	int nProcCount = pMiscDB->getCount();
	for( int i=0; i<nProcCount; i++ )
	{
		const ProcessorID* pProcID = pMiscDB->getProcName( m_listboxProcessor.GetItemData(i));
		if( *pProcID == _id )
		{
			m_listboxProcessor.SetCurSel( i );
			ReLoadDetailed();	
			return true;
		}
	}
	return false;
}


void CProcDataPage::OnButtonDel() 
{
	// TODO: Add your control notification handler code here
	int nIdx = m_listboxProcessor.GetCurSel();
	if( nIdx == LB_ERR )
		return;
	
	MiscProcessorData* pMiscDB = m_pInTerm->miscData->getDatabase( m_nProcDataType );
	if( pMiscDB == NULL )
		return;
	
	pMiscDB->deleteItem( m_listboxProcessor.GetItemData(nIdx));
	m_listboxProcessor.DeleteString( nIdx );

	LoadProcessorList();
	SetModified();
	if(m_listboxProcessor.GetCount()==0)
	{
		m_TreeData.DeleteAllItems();
		m_hLinkage=NULL;
	}
	
	/*m_listboxProcessor.SetCurSel( -1 );
	ReLoadDetailed();	*/
	
}

void CProcDataPage::SetTree()
{
	m_hGateItem = NULL;
	m_hWaitArea	= NULL;
	m_hProc1Item = NULL;
	m_hLinkedDestItem=NULL;
	m_TreeData.DeleteAllItems();
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	m_hLinkage=m_TreeData.InsertItem("Linkage",cni,FALSE);
	m_hGateItem = m_TreeData.InsertItem( "Gate",cni,FALSE,FALSE,m_hLinkage );
	m_hWaitArea=m_TreeData.InsertItem( "Wait Area",cni,FALSE,FALSE,m_hLinkage );
	m_hLinkedDestItem=m_TreeData.InsertItem("Linked Destination",cni,FALSE,FALSE,m_hLinkage);
	//m_TreeData.SetItemData( m_hGateItem, 0 );

	CString csLabel = "";
	switch( m_nProcDataType )
	{
	case DepSourceProc:
	case DepSinkProc:
		csLabel = "Dependents";
		break;
	case BaggageProc:
		csLabel = "Bypass Processors";
		break;
	case FloorChangeProc:
		csLabel = "Connected Processors";
		break;
	}

	if( !csLabel.IsEmpty() )
	{
		m_hProc1Item = m_TreeData.InsertItem( csLabel,cni,FALSE,FALSE,m_hLinkage );
		m_TreeData.SetItemData( m_hProc1Item, 1 );
	}

	if ( m_nProcDataType == BaggageProc)
	{
		m_hGreetingHall = m_TreeData.InsertItem("Greeting Hall",cni,FALSE,FALSE,m_hLinkage);
	}

	if (m_nProcDataType == BridgeConnectorProc)
	{
		cni.nt = NT_NORMAL;
		cni.net = NET_NORMAL;
		m_hStandConnect = m_TreeData.InsertItem("Connected Stands", cni,FALSE,FALSE,m_hLinkage);
	}

	m_TreeData.Expand(m_hLinkage,TVE_EXPAND);
	
	
	InitSpecificBehvaior();
	//Capacity

	//Hide Capacity Tree Item for some Processor Type
	if ( m_nProcDataType != ConveyorProc &&  m_nProcDataType != BaggageProc
		&& m_nProcDataType != BillboardProcessor 
		&& m_nProcDataType != IntegratedStationProc)
	{
		m_hTreeItemCapacity = m_TreeData.InsertItem("Capacity",cni,FALSE);
	}

	InsertTreeForBehavior();
}

void CProcDataPage::ReloadGateList( MiscData* _pMiscData )
{
	// get the item handler
	if( !m_hGateItem )
		return;

	DeleteAllChildItems( m_hGateItem );
	
	// inster item
    MiscProcessorIDList* pProcIDList = (MiscProcessorIDList*)_pMiscData->getGateList ();
    char str[80];
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
    for (int i = 0; i < pProcIDList->getCount (); i++)
    {
		const MiscProcessorIDWithOne2OneFlag* pProcID = (MiscProcessorIDWithOne2OneFlag*)pProcIDList->getID( i );
        pProcID->printID( str );
		if( pProcID->getOne2OneFlag() )
			strcat(str ,"{1:1}");
		
		HTREEITEM hItem = m_TreeData.InsertItem( str,cni,FALSE,FALSE, m_hGateItem );
		m_TreeData.SetItemData( hItem, i );
    }
	m_TreeData.Expand( m_hGateItem, TVE_EXPAND );
	m_TreeData.SelectItem(m_hGateItem);
}

void CProcDataPage::ReloadLinkProc1List( MiscData* _pMiscData )
{
	// get the item handler
	if( !m_hProc1Item )
		return;

	DeleteAllChildItems( m_hProc1Item );
	
	// inster item
	MiscProcessorIDList* pProcIDList = (MiscProcessorIDList*)GetLinkProc1List( _pMiscData );
	if( !pProcIDList )
		return;
	
	char str[80];
	const MiscProcessorIDWithOne2OneFlag *procID = NULL;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	for (int i = 0; i < pProcIDList->getCount(); i++)
	{
		procID = (MiscProcessorIDWithOne2OneFlag*)pProcIDList->getID (i);
		procID->printID (str);
		if( procID->getOne2OneFlag() )
			strcat(str, "{1:1}");

		HTREEITEM hItem = m_TreeData.InsertItem( str,cni,FALSE,FALSE, m_hProc1Item );
		m_TreeData.SetItemData( hItem, i );
	}	
	m_TreeData.Expand( m_hProc1Item, TVE_EXPAND );
	m_TreeData.SelectItem(m_hProc1Item);
}


void CProcDataPage::InitToolbar()
{
	CRect rc;
	m_TreeData.GetWindowRect(rc);
	ScreenToClient(rc);
	rc.bottom=rc.top+26;
	rc.OffsetRect(0,-26);
	
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);

	m_listboxProcessor.GetWindowRect(rc);
	ScreenToClient(rc);
	rc.bottom=rc.top+26;
	rc.OffsetRect(0,-26);
	
	m_ToolBarLeft.MoveWindow(&rc);
	m_ToolBarLeft.ShowWindow(SW_SHOW);
	m_ToolBarLeft.GetToolBarCtrl().EnableButton( ID_PROCESSOR_DATA_ADD );
	m_ToolBarLeft.GetToolBarCtrl().EnableButton( ID_PROCESSOR_DATA_DEL );

	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD );
	m_ToolBar.GetToolBarCtrl().HideButton( ID_TOOLBARBUTTONEDIT );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );		
	if(m_nProcDataType==Elevator)
	{
		m_ToolBar.GetToolBarCtrl().HideButton( ID_LINKAGE_ONETOONE);		
	}
}

int CProcDataPage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_LINKAGETOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	if (!m_ToolBarLeft.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBarLeft.LoadToolBar(IDR_ADDDEL_PROCESSOR_DATA))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

void CProcDataPage::OnToolbarbuttonadd() 
{
	// TODO: Add your command handler code here
	HTREEITEM hItem = m_hCurrentSelItem;
	
	if (hItem == NULL)
	{
		return;
	}		
	
	if( hItem == m_hGateItem )
		AddGate();
	else if(hItem==m_hWaitArea)
		AddWaitArea();
	else if( hItem == m_hProc1Item )
		AddPro1();
	else if( hItem == m_hHoldingAreaOfLoader )
		AddWaitAreaForLoader();
	else if(hItem==m_hPSS)
		AddPSS();
	else if(hItem==m_hLinkedDestItem)
		AddLinkedDest();
	else if( hItem == m_hExitBeltOfSorter)
		AddExitBeltOfSorter();	
	else if( hItem == m_hGreetingHall )
		AddGreetingHall();
	else if( hItem == m_hDependentProcessors )
		AddDependentProcessors();
	else if (hItem == m_hConnectBridge)
		AddBridgeConnector();
	else if (hItem = m_hStandConnect)
		AddStandConnector();

}

void CProcDataPage::OnToolbarbuttondel() 
{
	HTREEITEM hItem = m_hCurrentSelItem;
	if (hItem == NULL)
	{
		return;
	}		
	
	HTREEITEM hParentItem = m_TreeData.GetParentItem( hItem );
	if( hParentItem == m_hGateItem )
		DelGate();
	else if(hParentItem == m_hWaitArea)
		DelWaitArea();
	else if( hParentItem == m_hProc1Item )
		DelPro1();
	else if( hParentItem == m_hHoldingAreaOfLoader )
		DelWaitAreaForLoader();
	else if(hParentItem == m_hPSS)
		DelPSS();
	else if (hParentItem == m_hLinkedDestItem)
		DelLinkedDest();
	else if( hParentItem == m_hExitBeltOfSorter)
		DelExitBeltOfSorter();	
	else if( hParentItem == m_hGreetingHall)
		DelGreetingHall();
	else if (hParentItem == m_hTreeItemCapacity)
		DeleteCapacityItem(hItem);
	else if(hParentItem == m_hDependentProcessors)
		DeleteDependentProcessors(hItem);
	else if(m_TreeData.GetParentItem(hParentItem) == m_hTreeItemCapacity)
	{
		DeleteCapacityCombinationItem(hParentItem,hItem) ;
	}
	else if (hParentItem == m_hConnectBridge)
		DelBridgeConnector(hItem);
	else if (hParentItem = m_hStandConnect)
		DelStandConnector(hItem);
}

void CProcDataPage::AddGate()
{
	CProcesserDialog dlg( m_pInTerm );
	dlg.SetType( GateProc );
	if( dlg.DoModal() == IDCANCEL )
		return;	
	
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;

	ProcessorID id;	
	ProcessorIDList idList;
	ProcessorIDList idListShow;

	if( !dlg.GetProcessorIDList(idList) )
		return;	
	
	int nIDcount = idList.getCount();
	for(int i = 0; i < nIDcount; i++)
	{
		if (!SelectGate(*(idList.getID(i))))  // check if exist
		{
			idListShow.Add(idList[i]);
		}
	}
	
	nIDcount = idListShow.getCount();
	for (i = 0; i < nIDcount; i++ )
	{
		id = *(idListShow.getID(i));		
		MiscProcessorIDList* pProcIDList = (MiscProcessorIDList*)pMiscData->getGateList();
		MiscProcessorIDWithOne2OneFlag* pID = new MiscProcessorIDWithOne2OneFlag( id );
		pProcIDList->addItem( pID );
	}
	if ( nIDcount > 0)
	{
		ReloadGateList( pMiscData );
		SelectGate( id );	// select new item.
		SetModified();		
	}
	
}



void CProcDataPage::DelGate()
{
	HTREEITEM hItem = m_hCurrentSelItem;
	if( hItem == NULL )
		return;
		
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;
	
	ProcessorIDList* pProcIDList = pMiscData->getGateList();
	if( !pProcIDList )
		return;

	int nDBIdx = m_TreeData.GetItemData( hItem );
	pProcIDList->removeItem( nDBIdx );
	ReloadGateList( pMiscData );	
	SetModified();
}



void CProcDataPage::AddPro1()
{
	CProcesserDialog dlg( m_pInTerm );
	int nProcType = -1;
	switch( m_nProcDataType )
	{
	case DepSourceProc:
		nProcType = DepSinkProc;
		break;
	case DepSinkProc:
		nProcType = DepSourceProc;
		break;
	case FloorChangeProc:
		nProcType = FloorChangeProc;
		break;
	case BridgeConnectorProc:
		nProcType = StandProcessor;
		break;
	default:
		ASSERT(0);
		break;
	}
	dlg.SetType( nProcType );
	if( dlg.DoModal() == IDCANCEL )
		return;
	
//	if( !dlg.GetProcessorID(id) )
//		return;
//	
//	if( SelectLinkProc1(id) )	// check if exist.
	//	return;	
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;

	ProcessorID id;	
	ProcessorIDList idList;
	ProcessorIDList idListShow;
	if( !dlg.GetProcessorIDList(idList) )
		return;	
	
	int nIDcount = idList.getCount();
	for(int i = 0; i < nIDcount; i++)
	{
		if (!SelectLinkProc1(*(idList.getID(i))))  // check if exist
		{
			idListShow.addItem(idList[i]);
		}
	}
	
	nIDcount = idListShow.getCount();
	for (i = 0; i < nIDcount; i++ )
	{
		id = *(idListShow.getID(i));			
		
		MiscProcessorIDList* pProcIDList = (MiscProcessorIDList*)GetLinkProc1List( pMiscData );
		MiscProcessorIDWithOne2OneFlag* pID = new MiscProcessorIDWithOne2OneFlag( id );
		pProcIDList->addItem( pID );
	}
	if ( nIDcount > 0 )
	{
		ReloadLinkProc1List( pMiscData );
		SelectLinkProc1( id );	// select new item.
		SetModified();	
	}
}




void CProcDataPage::DelPro1()
{
	HTREEITEM hItem = m_hCurrentSelItem; 
	if( hItem == NULL )
		return;

	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;

	int nDBIdx = m_TreeData.GetItemData( hItem );
	
	ProcessorIDList* pProcIDList = GetLinkProc1List( pMiscData );
	if( pProcIDList == NULL )
		return;
	
	pProcIDList->removeItem( nDBIdx );
	ReloadLinkProc1List( pMiscData );
	SetModified();	
}

void CProcDataPage::DeleteAllChildItems(HTREEITEM _hParentItem)
{
	HTREEITEM hItem = m_TreeData.GetChildItem( _hParentItem );
	while( hItem )
	{
		DeleteAllChildItems( hItem );
		m_TreeData.DeleteItem( hItem );
		hItem = m_TreeData.GetChildItem( _hParentItem );
	}
}

void CProcDataPage::AddLinkedDest()
{
	CProcesserDialog dlg( m_pInTerm );
	dlg.SetType( LinkedDestProc );
	if( dlg.DoModal() == IDCANCEL )
		return;
	
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;

	
//	ProcessorID id;
//	if( !dlg.GetProcessorID(id) )
//		return;
//	
//	if( SelectLinkedDest( id ) )	// check if exist.
//		return;

	ProcessorID id;	
	ProcessorIDList idList;
	ProcessorIDList idListShow;
	if( !dlg.GetProcessorIDList(idList) )
		return;	

	int nIDcount = idList.getCount();
	for(int i = 0; i < nIDcount; i++)
	{
		if (!SelectLinkedDest(*(idList.getID(i))))  // check if exist
		{
			idListShow.addItem(idList[i]);
		}
	}
	
	nIDcount = idListShow.getCount();
	for (i = 0; i < nIDcount; i++ )
	{
		id = *(idListShow.getID(i));	
		ProcessorIDList* pProcIDList = (ProcessorIDList*)pMiscData->getLinkedDestList();
		MiscProcessorIDWithOne2OneFlag* pID = new MiscProcessorIDWithOne2OneFlag( id );
		pProcIDList->addItem( pID );		
	}
	if ( nIDcount > 0)
	{
		ReloadLinkedDestList( pMiscData );
		SelectLinkedDest( id );	// select new item.
		SetModified();
	}
}

void CProcDataPage::DelLinkedDest()
{
	HTREEITEM hItem = m_hCurrentSelItem;
	if( hItem == NULL )
		return;
	
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;
	
	ProcessorIDList* pProcIDList = pMiscData->getLinkedDestList();
	if( !pProcIDList )
		return;
	
	int nDBIdx = m_TreeData.GetItemData( hItem );
	pProcIDList->removeItem( nDBIdx );
	ReloadLinkedDestList( pMiscData );	
	SetModified();
}

void CProcDataPage::AddDependentProcessors()
{
	CProcesserDialog dlg( m_pInTerm );
	//dlg.SetType( GateProc );
	if( dlg.DoModal() == IDCANCEL )
		return;	


	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;

	ProcessorID id;	
	ProcessorIDList idList;
	ProcessorIDList idListShow;
	if( !dlg.GetProcessorIDList(idList) )
		return;	

	int nIDcount = idList.getCount();
	for(int i = 0; i < nIDcount; i++)
	{
		if (!SelectDependentProcessor(*(idList.getID(i))))  // check if exist
		{
			idListShow.addItem(idList[i]);
		}
	}

	nIDcount = idListShow.getCount();
	for (i = 0; i < nIDcount; i++ )
	{
		id = *(idListShow.getID(i));	
		ProcessorIDList* pProcIDList = (ProcessorIDList*)pMiscData->getDependentProcList();
		MiscProcessorIDWithOne2OneFlag* pID = new MiscProcessorIDWithOne2OneFlag( id );
		pProcIDList->addItem( pID );
	}
	if ( nIDcount > 0)
	{
		ReloadDependentProcessorList( pMiscData );
		SelectDependentProcessor( id );	// select new item.
		SetModified();
	}
}

void CProcDataPage::DeleteDependentProcessors(HTREEITEM hItem)
{
	if( hItem == NULL )
		return;

	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;

	ProcessorIDList* pProcIDList = pMiscData->getDependentProcList();
	if( !pProcIDList )
		return;

	int nDBIdx = m_TreeData.GetItemData( hItem );
	pProcIDList->removeItem( nDBIdx );
	ReloadDependentProcessorList( pMiscData );	
	SetModified();
}

void CProcDataPage::ReloadDependentProcessorList( MiscData* _pMiscData )
{
	m_TreeData.SetCheckStatus( m_hDependentProcessors, _pMiscData->getUseDependentProcFlag());
	
	DeleteAllChildItems( m_hDependentProcessors);

	// insert items
	MiscProcessorIDList* pProcIDList = (MiscProcessorIDList*)_pMiscData->getDependentProcList();
	char str[80];
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	for (int i = 0; i < pProcIDList->getCount (); i++)
	{
		const MiscProcessorIDWithOne2OneFlag* pProcID = (MiscProcessorIDWithOne2OneFlag*)pProcIDList->getID( i );
		pProcID->printID( str );
		HTREEITEM hItem = m_TreeData.InsertItem( str,cni,FALSE,FALSE, m_hDependentProcessors );
		m_TreeData.SetItemData( hItem, i );
	}

	m_TreeData.Expand( m_hDependentProcessors, TVE_EXPAND );
	m_TreeData.SelectItem(m_hDependentProcessors);

}

bool CProcDataPage::SelectDependentProcessor(ProcessorID _id)
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return false;

	if( m_hDependentProcessors == NULL )
		return false;

	ProcessorIDList* pProcIDList = pMiscData->getDependentProcList();
	HTREEITEM hItem = m_TreeData.GetChildItem( m_hDependentProcessors);
	while( hItem )
	{
		int nDBIdx = m_TreeData.GetItemData( hItem );

		const ProcessorID* pProcID = pProcIDList->getID( nDBIdx );
		if( *pProcID == _id )
		{
			m_TreeData.SelectItem( hItem );
			return true;
		}
		hItem = m_TreeData.GetNextItem( hItem, TVGN_NEXT );
	}	
	return false;
}

void CProcDataPage::OnSelchangedTreeData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM  hItem=pNMTreeView->itemNew.hItem;
	m_hCurrentSelItem = hItem;
//	HTREEITEM  hItem2=(HTREEITEM)pNMTreeView->itemOld.hItem;
	
	// TODO: Add your control notification handler code here	
	SetToolBarState( hItem );		
	*pResult = 0;
}

BOOL CProcDataPage::InitFlowConvetor()
{
	m_paxFlowConvertor.SetInputTerm( m_pInTerm );
	m_allFlightFlow.ClearAll();
	
	return TRUE;
}

void CProcDataPage::UpdateFlow()
{
	//refresh the m_allFlightFlow
	CWaitCursor m_waitCursor;
	m_allFlightFlow.ClearAll();
	if(!m_paxFlowConvertor.BuildSpecFlightFlowFromGateAssign( m_allFlightFlow ))
		return;
	//m_allFlightFlow.PrintAllStructure();

	if( m_allFlightFlow.IfAllFlowValid( true ))
	{
		m_paxFlowConvertor.ToOldFlightFlow( m_allFlightFlow );
		m_pInTerm->m_pSpecificFlightPaxFlowDB->saveDataSet( m_csProjPath ,true );
	}
}


void CProcDataPage::OnSize(UINT nType, int cx, int cy) 
{
	CResizablePage::OnSize(nType, cx, cy);
	SetSplitterRange();
	
	// TODO: Add your message handler code here
	
}

void CProcDataPage::InitSpiltter()
{
	CRect rcList,rcTree,rect;

	CWnd* pWnd = GetDlgItem(IDC_LIST_PROCESSOR);
	pWnd->GetWindowRect(rcList);
	ScreenToClient(rcList);

	pWnd=GetDlgItem(IDC_TREE_DATA);
	pWnd->GetWindowRect(rcTree);
	ScreenToClient(rcTree);
	rect=CRect(rcList.right,rcList.top,rcTree.left,rcList.bottom);
	m_wndSplitter.Create(WS_CHILD | WS_VISIBLE, rect, this, IDC_STATIC_SPLITTER);
	SetSplitterRange();
		
}

LRESULT CProcDataPage::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	CString _strItemText;

	if (message == WM_NOTIFY)
	{
		if (wParam == IDC_STATIC_SPLITTER )
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			DoResize(pHdr->delta);
		}
	}
	if(message==UM_CEW_STATUS_CHANGE)
	{
		HTREEITEM hItem=(HTREEITEM)wParam;
		//if(hItem==m_hPopulationCapacity)
		//	SaveDataForPopulationCapacity();
		/*else*/ if(hItem==m_hTerminateTime)
			SaveDataForTerminateTime();
		else if(hItem==m_hDisallowGroups)
			SaveDataForDisallowGroups();
		else if(hItem==m_hWaitInQueue)
			SaveDataForWaitInQueue();
		else if( hItem == m_hApplyServiceTimeAfterGreeting )
			SaveDataForApplyServiceTimeAfterGreeting();
		else if(hItem==m_hBCSN)
		{
			MiscData* pMiscData = GetCurMiscData();
			if(pMiscData)
			{
				UpdateData();
				BOOL b=IsCheckTreeItem(m_hBCSN);
				if(b)
				{
					int nCapacityValue=m_TreeData.GetItemData(m_hBCSN);
					((MiscHoldAreaData*)pMiscData)->SetStageNumber(nCapacityValue);
				}
				else
				{
					((MiscHoldAreaData*)pMiscData)->SetStageNumber(0);
					
				}
				SetModified();
			}
			
		}
		else if(hItem == m_hCorralService)
		{
			MiscData* pMiscData = GetCurMiscData();
			if(pMiscData)
			{
				UpdateData();
				BOOL b=IsCheckTreeItem(m_hCorralService);
				((MiscHoldAreaData*)pMiscData)->setCorralService(b == TRUE ? true : false);

				if(b)
				{
					m_TreeData.SetEnableStatus(m_hTimeToCorralService, TRUE);
				}
				else
				{
					m_TreeData.SetEnableStatus(m_hTimeToCorralService, FALSE);
				}

				SetModified(TRUE);
			}
		}
		else if(m_TreeData.GetParentItem(hItem)==m_hStopAtFloor)
		{
			MiscData* pMiscData = GetCurMiscData();
			if(pMiscData)
			{
				int nMinStopFloor =-1;
				int nMaxStopFloor =-1;



				MiscProcessorData* pMiscDB = m_pInTerm->miscData->getDatabase( (int)m_nProcDataType );
				int nMiscDataCount = pMiscDB->getCount();
				for (int i = 0; i < nMiscDataCount; ++i)
				{
					MiscDataElement* pMiscDataElement = (MiscDataElement*)pMiscDB->getItem(i);
					if (pMiscData == pMiscDataElement->getData())
					{
						const ProcessorID* pProcessorID  =  pMiscDataElement->getID();
						ElevProcessorFamily* tempProcessorFamily=NULL;
						tempProcessorFamily=(ElevProcessorFamily*)(m_pInTerm->procList->getFamilyIndex( *pProcessorID ));
						tempProcessorFamily->GetMaxMinFloorOfFamily(m_pInTerm->procList,nMaxStopFloor,nMinStopFloor);

					}
				}




				HTREEITEM hChild=m_TreeData.GetChildItem(m_hStopAtFloor);
				for(int i=nMinStopFloor;i<=nMaxStopFloor;i++)
				{
					BOOL bCheck=m_TreeData.IsCheckItem(hChild)==1;
					((MiscElevatorData*)pMiscData)->setStopAtFloor(i,bCheck);
					hChild=m_TreeData.GetNextSiblingItem(hChild);
				}
				SetModified();
				m_TreeData.SelectItem(NULL);

			}
			
		}
		else if( hItem == m_hItemLoader || hItem == m_hItemSimpleConveyor || hItem == m_hItemUnitBelt
					|| hItem == m_hItemMergeBox || hItem == m_hItemFeeder || hItem == m_hItemScanner
					|| hItem == m_hItemSplitter || hItem == m_hItemPusher || hItem == m_hItemFlowBelt 
					|| hItem == m_hItemSorter )
		{
			SetConveyorType( hItem );
			
			// if the select item is unit_belt or merge_bax, set capacity to 1;
			if( hItem == m_hItemUnitBelt || hItem == m_hItemMergeBox )
			{
				SetCapacityValue(1);
				m_TreeData.SetEnableStatus( m_hCapacityFather, FALSE );
				m_TreeData.SetEnableStatus( m_hCapacity, FALSE );
				m_TreeData.SetEnableStatus( m_hAutoCapacity,FALSE );
			}
			else
			{
				m_TreeData.SetEnableStatus( m_hCapacity, TRUE );
			}
		}
		else if( hItem == m_hAutoCapacity )
		{
			MiscData* pMiscData = GetCurMiscData();
			if( pMiscData )
			{
				((MiscConveyorData*)pMiscData)->SetQueueCapacity( -1 );
			}
		}
		else if ( hItem == m_hCapacity )
		{
			MiscData* pMiscData = GetCurMiscData();
			if( pMiscData )
			{
				UpdateData();
				int nValue = m_TreeData.GetItemData( m_hCapacity );
				((MiscConveyorData*)pMiscData)->SetQueueCapacity( nValue );
			}										
		}
		else if( hItem == m_hNextProcAvail )
		{
			MiscData* pMiscData = GetCurMiscData();
			if(pMiscData)
			{
				UpdateData();
				BOOL b = IsCheckTreeItem( hItem );
				if( b == BST_CHECKED )
					((MiscConveyorData*)pMiscData)->SetPusherReleasedByNextAvail( true );
				else
					((MiscConveyorData*)pMiscData)->SetPusherReleasedByNextAvail( false );
				SetModified();			
				// let other continue uncheck
				m_TreeData.SetCheckStatus( m_hConveyorIsFull, FALSE);
				m_TreeData.SetCheckStatus( m_hScheduledPickedUp, FALSE );
				m_TreeData.SetCheckStatus( m_hRandomAmountOfTimePasses, FALSE );

				((MiscConveyorData*)pMiscData)->SetPusherReleasedByFull( false );
				((MiscConveyorData*)pMiscData)->SetPusherReleasedBySchedPick( false );
				((MiscConveyorData*)pMiscData)->SetPusherReleasedByMaxTime( false );
			}
		}
		else if( hItem == m_hOtherCondition )
		{
			MiscData* pMiscData = GetCurMiscData();
			if(pMiscData)
			{
				((MiscConveyorData*)pMiscData)->SetPusherReleasedByNextAvail( false );
			}
		}
		
		else if( hItem == m_hConveyorIsFull )
		{
			MiscData* pMiscData = GetCurMiscData();
			if(pMiscData)
			{
				UpdateData();
				BOOL b = IsCheckTreeItem( hItem );
				if( b == BST_CHECKED )
					((MiscConveyorData*)pMiscData)->SetPusherReleasedByFull( true );
				else
					((MiscConveyorData*)pMiscData)->SetPusherReleasedByFull( false );								
				SetModified();			
			}				
		}
		
		else if( hItem == m_hScheduledPickedUp )
		{
			MiscData* pMiscData = GetCurMiscData();
			if(pMiscData)
			{
				UpdateData();
				BOOL b = IsCheckTreeItem( hItem );
				if( b == BST_CHECKED )
				{
					((MiscConveyorData*)pMiscData)->SetPusherReleasedBySchedPick( true );
					int nValue = m_TreeData.GetItemData( m_hScheduledPickedUp );
					((MiscConveyorData*)pMiscData)->SetScheduleTime( ElapsedTime((long)nValue) );					
				}
				else
					((MiscConveyorData*)pMiscData)->SetPusherReleasedBySchedPick( false );								
				SetModified();			
			}
		}
		else if( hItem == m_hRandomAmountOfTimePasses )
		{			
			MiscData* pMiscData = GetCurMiscData();
			if(pMiscData)
			{
				UpdateData();
				BOOL b = IsCheckTreeItem( hItem );
				if( b == BST_CHECKED )
				{
					((MiscConveyorData*)pMiscData)->SetPusherReleasedByMaxTime( true );
					int nValue = m_TreeData.GetItemData( m_hRandomAmountOfTimePasses );
					((MiscConveyorData*)pMiscData)->SetRandomAmountTime( ElapsedTime((long)nValue) );					
				}
				else
					((MiscConveyorData*)pMiscData)->SetPusherReleasedByMaxTime( false );								
				SetModified();			
			}
		}
		else if( hItem == m_hReversible )
		{
			MiscData* pMiscData = GetCurMiscData();
			if(pMiscData)
			{
				UpdateData();
				BOOL b = IsCheckTreeItem( hItem );
				if( b == BST_CHECKED )
					((MiscConveyorData*)pMiscData)->SetReversible( true );
				else
					((MiscConveyorData*)pMiscData)->SetReversible( false );
				SetModified();			
			}
		}
		else if(m_TreeData.GetParentItem(hItem) == m_hGroupSerTimeMod )
		{
			MiscData* pMiscData = GetCurMiscData();
			int nIndex = 0;
			if(pMiscData)
			{
				UpdateData();				
				if (hItem == m_hSumOfGroupSerTimeMod) 
				{
					nIndex = 0;
				}
				else if (hItem == m_hAverageOfGroupSerTimeMod)
				{
					nIndex = 1;					
				}
				else if (hItem ==m_hMinimumOfGroupSerTimeMod )
				{
					nIndex = 2;					
				}
				else if (hItem == m_hMaximumOfGroupSerTimeMod)
				{
					nIndex = 3;					
				}
				else if (hItem == m_hRandomOfGroupSerTimeMod)
				{
					nIndex = 4;					
				}				
				pMiscData->SetGroupSerTimeModIndex(nIndex);
			}				
		}			
		else if( hItem == m_hDependentProcessors)
		{
			MiscData* pMiscData = GetCurMiscData();
			if(pMiscData)
			{
				BOOL bCheck = IsCheckTreeItem(m_hDependentProcessors);
				((MiscHoldAreaData*)pMiscData)->setUseDependentProcFlag(bCheck ? true : false );

				//m_TreeData.SetEnableStatus( m_hDependentProcessors, bCheck );

				SetModified();	

			}
		}
	}

	if(message==UM_CEW_EDITSPIN_BEGIN)
	{
		HTREEITEM hItem=(HTREEITEM)wParam;
		SetEditValueRange(hItem);
	}
	if(message==UM_CEW_EDIT_BEGIN)
	{
		HTREEITEM hItem=(HTREEITEM)wParam;
		if(hItem==m_hCapOrDR&&HoldAreaProc==m_nProcDataType)
		{
			MiscData* pMiscData = GetCurMiscData();
			float fValue=((MiscHoldAreaData*)pMiscData)->getRadius();
			CString strValue;
			strValue.Format("%.1f",fValue);
			m_TreeData.GetEditWnd(hItem)->SetWindowText(strValue);
		}
		else if(hItem==m_hAccSpeedItem)
		{
			MiscData* pMiscData = GetCurMiscData();
			float fValue=static_cast<float>(((MiscElevatorData*)pMiscData)->getSpeed(MiscElevatorData::EST_ACCELERATION));
			CString strValue;
			strValue.Format("%.1f",fValue);
			m_TreeData.GetEditWnd(hItem)->SetWindowText(strValue);
		}
		else if(hItem==m_hDecSpeedItem)
		{
			MiscData* pMiscData = GetCurMiscData();
			float fValue=static_cast<float>(((MiscElevatorData*)pMiscData)->getSpeed(MiscElevatorData::EST_DECELERATION));
			CString strValue;
			strValue.Format("%.1f",fValue);
			m_TreeData.GetEditWnd(hItem)->SetWindowText(strValue);
		}
		else if(hItem==m_hNormalSpeedItem)
		{
			MiscData* pMiscData = GetCurMiscData();
			float fValue=static_cast<float>(((MiscElevatorData*)pMiscData)->getSpeed(MiscElevatorData::EST_NORMAL));
			CString strValue;
			strValue.Format("%.1f",fValue);
			m_TreeData.GetEditWnd(hItem)->SetWindowText(strValue);
		}
		else if(hItem==m_hEscalatorSpeed )
		{
			MiscData* pMiscData = GetCurMiscData();
			float fValue=static_cast<float>(((MiscEscalatorData *)pMiscData)->GetMovingSpeed());
			CString strValue;
			strValue.Format("%.1f",fValue);
			m_TreeData.GetEditWnd(hItem)->SetWindowText(strValue);
		}
		
	}
	if(message==UM_CEW_COMBOBOX_BEGIN)
	{
			CWnd* pWnd=m_TreeData.GetEditWnd((HTREEITEM)wParam);
			CComboBox* pCB=(CComboBox*)pWnd;
			if(m_nProcDataType==ConveyorProc)
			{
				if(pCB->GetCount()==0)
				{
					CProcDataSheet* pWnd=(CProcDataSheet*)GetParent();
					CView* pView=(CView*)(pWnd->m_pParent);
					
					CTermPlanDoc* pDoc	= (CTermPlanDoc*)(pView->GetDocument());
					int nAreaCount = pDoc->GetTerminal().m_pAreas->m_vAreas.size();
					for( int i=0; i<nAreaCount; i++ )
					{
						pCB->AddString( pDoc->GetTerminal().m_pAreas->m_vAreas[i]->name );
					}
				}
			}
			else
			{
				if(pCB->GetCount()==0)
				{
					
					int nCount = m_pInTerm->m_pMobElemTypeStrDB->GetCount();
					
					CString strFloor;
					for(int i=1;i<nCount;i++)
					{
						strFloor=m_pInTerm->m_pMobElemTypeStrDB->GetString(i);
						pCB->AddString(strFloor);
					}		
				}

			}			
	}
	if(message==UM_CEW_EDITSPIN_END)
	{
		MiscData* pMiscData = GetCurMiscData();
		if( pMiscData != NULL )
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pInfo = m_TreeData.GetItemNodeInfo( hItem );
			CString strValue=*((CString*)lParam);
			float fValue=static_cast<float>(atof(strValue));
			
			if(hItem==m_hItemSpeed)
			{
				DistanceUnit _dSpeed = UNITSMANAGER->UnConvertLength( fValue );
				((MiscConveyorData*)pMiscData)->SetSpeed( _dSpeed );
				_strItemText.Format("Speed (%s/s)",UNITSMANAGER->GetLengthUnitString() ); 
				//SetItemTextEx(hItem,pInfo, _strItemText, fValue );

				CString strValue;
				strValue.Format("%.2f",fValue);
				CString strItem = _strItemText+CString(":  ")+strValue;

				m_TreeData.SetItemText( hItem, strItem );
				m_TreeData.SetItemData( hItem, static_cast<DWORD>(fValue*100));

			}
			else if(hItem==m_hAccSpeedItem)
			{
				((MiscElevatorData*)pMiscData)->setSpeed(MiscElevatorData::EST_ACCELERATION,fValue);	
				//SetItemTextEx(hItem,pInfo,"Acceleration Speed ( m/s ) ",fValue);
				CString strValue;
				strValue.Format("%.2f",fValue);
				CString strItem = CString("Acceleration Speed ( m/s ) ")+CString(":  ")+strValue;
				
				m_TreeData.SetItemText( hItem, strItem );
				m_TreeData.SetItemData( hItem, static_cast<DWORD>(fValue*100));
				
			}
			else if(hItem==m_hDecSpeedItem)
			{
				((MiscElevatorData*)pMiscData)->setSpeed(MiscElevatorData::EST_DECELERATION,fValue);	
				//SetItemTextEx(hItem,pInfo,"Deceleration Speed ( m/s ) ",fValue);
				CString strValue;
				strValue.Format("%.2f",fValue);
				CString strItem = CString("Deceleration Speed ( m/s ) ")+CString(":  ")+strValue;
				
				m_TreeData.SetItemText( hItem, strItem );
				m_TreeData.SetItemData( hItem, static_cast<DWORD>(fValue*100));
				
			}
			else if(hItem==m_hNormalSpeedItem)
			{
				((MiscElevatorData*)pMiscData)->setSpeed(MiscElevatorData::EST_NORMAL,fValue);	
				//SetItemTextEx(hItem,pInfo,"Normal Speed ( m/s ) ",fValue);
				CString strValue;
				strValue.Format("%.2f",fValue);
				CString strItem = CString("Normal Speed ( m/s ) ")+CString(":  ")+strValue;
				
				m_TreeData.SetItemText( hItem, strItem );
				m_TreeData.SetItemData( hItem, static_cast<DWORD>(fValue*100));
				
			}
			else if(hItem==m_hEscalatorSpeed )
			{
				((MiscEscalatorData *)pMiscData)->SetMovingSpeed((fValue));	
				//SetItemTextEx(hItem,pInfo,"Normal Speed ( m/s ) ",fValue);
				CString strValue;
				strValue.Format("%.2f",fValue);
				CString strItem = CString("Speed ( m/s ) ")+CString(":  ")+strValue;
				
				m_TreeData.SetItemText( hItem, strItem );
				m_TreeData.SetItemData( hItem, static_cast<DWORD>(fValue*100));
				
			}			
			else if(hItem == m_hSumOfGroupSerTimeMod)
			{
				if (fValue > 100||fValue < 0) fValue =100;   // Percent no more the 100%
				pMiscData->SetGroupSerTimeModSum( static_cast<int>(fValue) );
				CString strValue;
				strValue.Format("%d",(int)fValue);
				CString strItem = CString("Sum: ")+strValue+"%";				
				m_TreeData.SetItemText( hItem, strItem );
				m_TreeData.SetItemData( hItem, static_cast<DWORD>(fValue));				
			}			
			else if (hItem == m_hTimeToCorralService)
			{
				int nTimeValue = static_cast<int>(fValue);
	
				CString strTime;
				strTime.Format("Start service after waiting (Seconds): %d", nTimeValue);
				m_TreeData.SetItemText(m_hTimeToCorralService,strTime);
				m_TreeData.SetItemData(m_hTimeToCorralService, nTimeValue);

				((MiscHoldAreaData*)pMiscData)->setTimeToService(nTimeValue);

				SetModified();
			}

		}			
	}
	if(	message==UM_CEW_EDITSPIN_END||
		message==UM_CEW_COMBOBOX_END)
	{
		MiscData* pMiscData = GetCurMiscData();
		if( pMiscData != NULL )
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pInfo = m_TreeData.GetItemNodeInfo( hItem );
			CString strValue=*((CString*)lParam);
			int nValue=atoi(strValue);
			if(hItem==m_hRecycleFreq)
			{
				pMiscData->SetCyclicFreq(nValue);	
				SetItemTextEx(hItem,pInfo,"Recycle Frequency ",nValue);
			}
		//	else if(hItem==m_hPopulationCapacity)
		//	{
		//		pMiscData->SetQueueCapacity(nValue);
		//		SetItemTextEx(hItem,pInfo,"Population Capacity ",nValue);
		//	}
			else if (m_hTreeItemCapacity == m_TreeData.GetParentItem(hItem))
			{
				
				CapacityAttributes* pAttributes = pMiscData->GetCapacityAttributes();
				CapacityAttribute* pAttribute = (CapacityAttribute*)pInfo->nMaxCharNum ;
				pAttributes->UpdateCapacityAttribute(pAttribute, nValue);

				CString strMob;
				if(!pAttribute->m_Name.IsEmpty())
					strMob.Format("%s: %d", pAttribute->m_Name,nValue);
				else
				{
					if(pAttribute->m_nMobElementIndex != -1)
					{
					strMob.Format("%s: %d", GetMobileElementString(pAttribute->m_nMobElementIndex),nValue);
					}
				}
				m_TreeData.SetItemText(hItem,strMob) ;
			}
			else if(hItem==m_hTerminateTime)
			{
				pMiscData->SetTerminateTime(nValue);
				SetItemTextEx(hItem,pInfo,"Terminate when time to departure less than (Min)",nValue);
			}
			// set start 
			else if(hItem==m_hCapacity)
			{
				pMiscData->SetQueueCapacity(nValue);
				SetItemTextEx(hItem,pInfo,"Capacity ",nValue);
			}
			else if(hItem==m_hRandomAmountOfTimePasses)
			{
				((MiscConveyorData*)pMiscData)->SetRandomAmountTime( ElapsedTime((long)nValue) );
				SetItemTextEx(hItem,pInfo,"Random amount of time Passes (s)",nValue);
			}
			else if(hItem==m_hScheduledPickedUp)
			{
				((MiscConveyorData*)pMiscData)->SetScheduleTime( ElapsedTime((long)nValue) );
				SetItemTextEx(hItem,pInfo,"Scheduled picked up (s)",nValue);
			}

			else if(hItem==m_hBCSN)
			{
				
				((MiscHoldAreaData*)pMiscData)->SetStageNumber(nValue);
				SetItemTextEx(hItem,pInfo,"Boarding Call Stage Number ",nValue);
			}
			else if (hItem == m_hCorralService)
			{
				((MiscHoldAreaData*)pMiscData)->setTimeToService(nValue);
				SetItemTextEx(hItem,pInfo,"Corral Service after Full ",static_cast<DWORD>(nValue));
				strValue.Format("Corral Service after Full:  %d",nValue);
				m_TreeData.SetItemText(hItem,strValue);
			}

			else if(hItem==m_hCapOrDR)
			{
				
				int nValue=atoi(strValue);
				switch( m_nProcDataType )
				{
				case BaggageProc:
					{
						int nValue=atoi(strValue);
						((MiscBaggageData*)pMiscData)->setCapacity(nValue);
						SetItemTextEx(hItem,pInfo,"Capacity ",nValue);
					}
				
				break;
				case HoldAreaProc:
					{
						float fValue=static_cast<float>(atof(strValue));
						((MiscHoldAreaData*)pMiscData)->setRadius( fValue);
						SetItemTextEx(hItem,pInfo,"Distribution Radius ",static_cast<DWORD>(fValue));
						strValue.Format("Distribution Radius:  %.1f",fValue);
						m_TreeData.SetItemText(hItem,strValue);
					}
				break;
				}
			}
			
			SetModified();			
		}
		
		
	}
	return CResizablePage::DefWindowProc(message, wParam, lParam);
}

void CProcDataPage::DoResize(int nDelta)
{
	//Resize Left Wnd
	CSplitterControl::ChangeWidth(&m_ToolBarLeft,nDelta,CW_LEFTALIGN);
	CSplitterControl::ChangeWidth(&m_listboxProcessor,nDelta,CW_LEFTALIGN);
	//Resize Right Wnd
	CSplitterControl::ChangeWidth(&m_staticLinkage,-nDelta,CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_ToolBar,-nDelta,CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_TreeData,-nDelta,CW_RIGHTALIGN);
	
	Invalidate();
}

void CProcDataPage::SetSplitterRange()
{
	if(m_TreeData.m_hWnd&&m_wndSplitter.m_hWnd)
	{
		CRect rect;
		m_TreeData.GetWindowRect(rect);
		ScreenToClient(rect);
		m_wndSplitter.SetRange(20,rect.right-180);
		Invalidate();
	}
}

InputTerminal* CProcDataPage::GetInputTerm()
{
	return m_pInTerm;
}

void CProcDataPage::OnLinkageOnetoone() 
{
	HTREEITEM hItem = m_hCurrentSelItem;
	if( hItem == NULL )
		return;
	
	HTREEITEM hParentItem = m_TreeData.GetParentItem( hItem );
	if( hParentItem == m_hGateItem )
	{
		MiscData* pMiscData = GetCurMiscData();
		if( pMiscData == NULL )
			return;
		
		ProcessorIDList* pProcIDList = pMiscData->getGateList();
		if( !pProcIDList )
			return;
		
		int nDBIdx = m_TreeData.GetItemData( hItem );
		MiscProcessorIDWithOne2OneFlag* pProc = (MiscProcessorIDWithOne2OneFlag*)pProcIDList->getItem( nDBIdx );
		pProc->setOne2OneFlag( !pProc->getOne2OneFlag() );
		ReloadGateList( pMiscData );	
		SetModified();
	}
	//////////////////////////////////////////////////////////////////////////
	if( hParentItem == m_hLinkedDestItem )
	{
		MiscData* pMiscData = GetCurMiscData();
		if( pMiscData == NULL )
			return;
		
		ProcessorIDList* pProcIDList = pMiscData->getLinkedDestList();
		if( !pProcIDList )
			return;
		
		int nDBIdx = m_TreeData.GetItemData( hItem );
		MiscProcessorIDWithOne2OneFlag* pProc = (MiscProcessorIDWithOne2OneFlag*)pProcIDList->getItem( nDBIdx );
		pProc->setOne2OneFlag( !pProc->getOne2OneFlag() );
		ReloadLinkedDestList( pMiscData );	
		SetModified();
	}
	
	//////////////////////////////////////////////////////////////////////////	
	if( hParentItem == m_hWaitArea )
	{
		MiscData* pMiscData = GetCurMiscData();
		if( pMiscData == NULL )
			return;
		
		ProcessorIDList* pProcIDList = pMiscData->getWaitAreaList();
		if( !pProcIDList )
			return;
		
		int nDBIdx = m_TreeData.GetItemData( hItem );
		MiscProcessorIDWithOne2OneFlag* pProc = (MiscProcessorIDWithOne2OneFlag*)pProcIDList->getItem( nDBIdx );
		pProc->setOne2OneFlag( !pProc->getOne2OneFlag() );
		ReloadWaitAreaList( pMiscData );	
		SetModified();
	}
	else if( hParentItem == m_hProc1Item )
	{
		MiscData* pMiscData = GetCurMiscData();
		if( pMiscData == NULL )
			return;

		ProcessorIDList* pProcIDList = GetLinkProc1List( pMiscData );
		if( !pProcIDList )
			return;
		
		int nDBIdx = m_TreeData.GetItemData( hItem );
		MiscProcessorIDWithOne2OneFlag* pProc = (MiscProcessorIDWithOne2OneFlag*)pProcIDList->getItem( nDBIdx );
		pProc->setOne2OneFlag( !pProc->getOne2OneFlag() );
		ReloadLinkProc1List( pMiscData );
		SetModified();	
	}

	//////////////////////////////////////////////////////////////////////////
	if( hParentItem == m_hHoldingAreaOfLoader )//holding area
	{
		MiscData* pMiscData = GetCurMiscData();
		if( pMiscData == NULL )
			return ; 

		ProcessorIDList* pProcIDList = pMiscData->getWaitAreaList();
		if (!pProcIDList)
			return;
		
		int nDBIdx = m_TreeData.GetItemData( hItem );
		MiscProcessorIDWithOne2OneFlag* pProc = (MiscProcessorIDWithOne2OneFlag*)pProcIDList->getItem( nDBIdx );
		pProc->setOne2OneFlag( !pProc->getOne2OneFlag() );

		ReloadWaitAreaList(pMiscData, true);
		SetModified();
	}

	//////////////////////////////////////////////////////////////////////////	
	if (hParentItem == m_hConnectBridge)
	{
		MiscData* pMiscData = GetCurMiscData();
		if( pMiscData == NULL )
			return ; 

		ProcessorIDList* pProcIDList = pMiscData->getBridgeConnectorList();
		if (!pProcIDList)
			return;

		int nDBIdx = m_TreeData.GetItemData( hItem );
		MiscProcessorIDWithOne2OneFlag* pProc = (MiscProcessorIDWithOne2OneFlag*)pProcIDList->getItem( nDBIdx );
		pProc->setOne2OneFlag( !pProc->getOne2OneFlag() );

		ReloadBridgeConnectorList(pMiscData);
		SetModified();
	}

	///////////////////////////////////////////////////////////////////////////
	if (hParentItem == m_hStandConnect)
	{
		MiscData* pMiscData = GetCurMiscData();
		if( pMiscData == NULL )
			return ; 

		ProcessorIDList* pProcIDList = ((MiscBridgeConnectorData*)pMiscData)->GetBridgeConnectorLinkedProcList();
		if (!pProcIDList)
			return;

		int nDBIdx = m_TreeData.GetItemData( hItem );
		MiscProcessorIDWithOne2OneFlag* pProc = (MiscProcessorIDWithOne2OneFlag*)pProcIDList->getItem( nDBIdx );
		pProc->setOne2OneFlag( !pProc->getOne2OneFlag() );

		ReloadStandConnectorList(pMiscData);
		SetModified();
	}
}

void CProcDataPage::InsertTreeForBehavior()
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	m_hOtherBehavior=m_TreeData.InsertItem("Other Behaviors",cni,FALSE);

	cni.nt=NT_NORMAL;
	m_hGroupSerTimeMod=m_TreeData.InsertItem("Group service time modifier",cni,FALSE,FALSE,m_hOtherBehavior);
	
	cni.nt=NT_RADIOBTN;
	cni.net=NET_EDITSPIN_WITH_VALUE;
	cni.fMaxValue = 100;
	cni.fMinValue =0 ;
				
	m_hSumOfGroupSerTimeMod = m_TreeData.InsertItem("Sum: 100%",cni,TRUE,FALSE,m_hGroupSerTimeMod);
	m_TreeData.SetItemData(m_hSumOfGroupSerTimeMod,100);	
	cni.nt=NT_RADIOBTN;
	cni.net=NET_NORMAL;
	m_hAverageOfGroupSerTimeMod = m_TreeData.InsertItem("Average",cni,FALSE,FALSE,m_hGroupSerTimeMod);
	m_hMinimumOfGroupSerTimeMod = m_TreeData.InsertItem("Minimum",cni,FALSE,FALSE,m_hGroupSerTimeMod);
	m_hMaximumOfGroupSerTimeMod = m_TreeData.InsertItem("Maximum",cni,FALSE,FALSE,m_hGroupSerTimeMod);
	m_hRandomOfGroupSerTimeMod  = m_TreeData.InsertItem("Random for group",cni,FALSE,FALSE,m_hGroupSerTimeMod);
				
	

	cni.nt=NT_CHECKBOX;
	m_hDisallowGroups=m_TreeData.InsertItem("Disallow Groups",cni,TRUE,FALSE,m_hOtherBehavior);
	m_hWaitInQueue = m_TreeData.InsertItem("WaitInQueueToOpen",cni,TRUE,FALSE,m_hOtherBehavior);

	HTREEITEM hDNPME=m_TreeData.InsertItem("Disallow Non-Passenger Mobile Element",cni,FALSE,FALSE,m_hOtherBehavior);
	cni.nt=NT_NORMAL;
	cni.net=NET_COMBOBOX;
	
	m_TreeData.InsertItem("[New Non-Passenger Mobile Element]",cni,FALSE,FALSE,hDNPME);
	cni.nt=NT_CHECKBOX;
	cni.net=NET_EDITSPIN_WITH_VALUE;
	//m_hPopulationCapacity=m_TreeData.InsertItem("Population Capacity:  0",cni,TRUE,FALSE,m_hOtherBehavior);
	//m_TreeData.SetItemData(m_hPopulationCapacity,0);
	m_hTerminateTime=m_TreeData.InsertItem("Terminate when time to departure less than (Min):  0",cni,TRUE,FALSE,m_hOtherBehavior);
	m_TreeData.SetItemData(m_hTerminateTime,0);
	if(m_nProcDataType==HoldAreaProc)
	{
		m_hBCSN=m_TreeData.InsertItem("Boarding Call Stage Number:  1",cni,FALSE,FALSE,m_hLinkage);
		m_TreeData.SetItemData(m_hBCSN,1);
		int nDBCount=m_pInTerm->flightData->getBoardingCallsStageSize();
		m_TreeData.SetItemValueRange(m_hBCSN,1,nDBCount);

	}
	cni.nt=NT_NORMAL;
	m_hRecycleFreq=m_TreeData.InsertItem("Recycle Frequency:  10",cni,FALSE,FALSE,m_hOtherBehavior);
	m_TreeData.SetItemData(m_hRecycleFreq,10);

	CCoolTree::InitNodeInfo(this,cni);
	switch(m_nProcDataType)
	{
	case BaggageProc:
		cni.net=NET_EDITSPIN_WITH_VALUE;
		m_hCapOrDR=m_TreeData.InsertItem("Capacity:  5",cni,FALSE,FALSE,m_hOtherBehavior);
		m_TreeData.SetItemData(m_hCapOrDR,5);
		break;
	case HoldAreaProc:
		cni.net=NET_EDITSPIN_WITH_VALUE;
		m_hCapOrDR=m_TreeData.InsertItem("Distribution Radius:  5",cni,FALSE,FALSE,m_hOtherBehavior);
		m_TreeData.SetItemData(m_hCapOrDR,5);

		// holding area's greeting time
		cni.net=NET_NORMAL;
		cni.nt=NT_CHECKBOX;
		m_hApplyServiceTimeAfterGreeting = m_TreeData.InsertItem("Apply Service Time After Converge",cni,TRUE,FALSE,m_hOtherBehavior);

		break;
	case Elevator:
		{
			cni.net=NET_NORMAL;
			m_hStopAtFloor=m_TreeData.InsertItem("Stop At Floor",cni,FALSE,FALSE,m_hOtherBehavior);

			m_hSpeed=m_TreeData.InsertItem("Speed",cni,FALSE,FALSE,m_hOtherBehavior);

			//cni.net=NET_EDITSPIN_WITH_VALUE;
			cni.net=NET_EDIT_WITH_VALUE;
			m_hAccSpeedItem = m_TreeData.InsertItem("Acceleration Speed ( m/s ) : 1.0",cni,FALSE,FALSE,m_hSpeed);
			m_TreeData.SetItemData(m_hAccSpeedItem ,1*100);
			m_hNormalSpeedItem = m_TreeData.InsertItem("Normal Speed ( m/s ) : 2.0",cni,FALSE,FALSE,m_hSpeed);
			m_TreeData.SetItemData(m_hNormalSpeedItem ,2*100);
			m_hDecSpeedItem = m_TreeData.InsertItem("Deceleration Speed ( m/s ) : 1.0",cni,FALSE,FALSE,m_hSpeed);
			m_TreeData.SetItemData(m_hDecSpeedItem ,1*100);
			m_TreeData.Expand(m_hSpeed,TVE_EXPAND);

			//Stop At Floor

			int nMinStopFloor = -1;
			int nMaxStopFloor = -1;

			MiscProcessorData* pMiscDB = m_pInTerm->miscData->getDatabase( (int)m_nProcDataType );
			int nMiscDataCount = pMiscDB->getCount();
			MiscData* pMiscData = GetCurMiscData(); 
			for (int i = 0; i < nMiscDataCount; ++i)
			{
				MiscDataElement* pMiscDataElement = (MiscDataElement*)pMiscDB->getItem(i);
				if (pMiscData == pMiscDataElement->getData())
				{
					const ProcessorID* pProcessorID  =  pMiscDataElement->getID();
					ElevProcessorFamily* tempProcessorFamily=NULL;
					tempProcessorFamily=(ElevProcessorFamily*)(m_pInTerm->procList->getFamilyIndex( *pProcessorID ));
					tempProcessorFamily->GetMaxMinFloorOfFamily(m_pInTerm->procList,nMaxStopFloor,nMinStopFloor);

				}
			}
			CString strFloor;
			cni.nt=NT_CHECKBOX;
			cni.net=NET_NORMAL;
			CProcDataSheet* pWnd=(CProcDataSheet*)GetParent();
			CView* pView=(CView*)(pWnd->m_pParent);
			CTermPlanDoc* pDoc	= (CTermPlanDoc*)(pView->GetDocument());
			CFloorList& floorList = pDoc->GetCurModeFloor().m_vFloors;
			for(int i = nMinStopFloor;i<nMaxStopFloor; i++)
			{
				CFloor2* pFloor = floorList.at(i);
				strFloor.Format("Floor %s",pFloor->FloorName());
				m_TreeData.InsertItem(strFloor,cni,((MiscElevatorData*)pMiscData)->getStopAtFloor(i),FALSE,m_hStopAtFloor);
			}
			m_TreeData.Expand(m_hStopAtFloor, TVE_EXPAND);
		}
		break;
	case EscalatorProc:
		{		
			cni.net=NET_EDIT_WITH_VALUE;
			m_hEscalatorSpeed=m_TreeData.InsertItem("Speed",cni,FALSE,FALSE,m_hOtherBehavior);			
			m_TreeData.SetItemData(m_hEscalatorSpeed ,2*100);			
		}
		break;
	case ConveyorProc:
		{

			m_TreeData.SetEnableStatus(m_hLinkage,TRUE);
			m_TreeData.SetEnableStatus(m_hGateItem,FALSE);
			m_TreeData.SetEnableStatus(m_hWaitArea,FALSE);
			m_TreeData.SetEnableStatus(m_hLinkedDestItem,TRUE);			
			
			m_TreeData.SetEnableStatus(m_hDisallowGroups,FALSE);
			m_TreeData.SetEnableStatus(hDNPME,FALSE);
		//	m_TreeData.SetEnableStatus(m_hPopulationCapacity,FALSE);
			m_TreeData.SetEnableStatus(m_hTerminateTime,FALSE);
			m_TreeData.SetEnableStatus(m_hRecycleFreq,TRUE);
			cni.nt=NT_NORMAL;
			cni.net=NET_NORMAL;
			HTREEITEM hItemType=m_TreeData.InsertItem("Type",cni,FALSE,FALSE,m_hOtherBehavior);
			cni.nt=NT_RADIOBTN;
			cni.net=NET_NORMAL;
			m_hItemLoader = m_TreeData.InsertItem("Loader",cni,FALSE,FALSE,hItemType);
			m_hItemSimpleConveyor = m_TreeData.InsertItem("Simple Conveyor",cni,TRUE,FALSE,hItemType);
			m_hItemUnitBelt = m_TreeData.InsertItem("Unit Belt",cni,FALSE,FALSE,hItemType);
			m_hItemMergeBox = m_TreeData.InsertItem("Merge Box",cni,FALSE,FALSE,hItemType);
			m_hItemFeeder = m_TreeData.InsertItem("Feeder",cni,FALSE,FALSE,hItemType);
			m_hItemScanner=m_TreeData.InsertItem("Scanner",cni,FALSE,FALSE,hItemType);
			m_hItemSplitter = m_TreeData.InsertItem("Splitter",cni,FALSE,FALSE,hItemType);
			m_hItemPusher=m_TreeData.InsertItem("Pusher",cni,FALSE,FALSE,hItemType);
			m_hItemFlowBelt = m_TreeData.InsertItem("Flow Belt",cni,FALSE,FALSE,hItemType);
			m_hItemSorter = m_TreeData.InsertItem("Sorter",cni,FALSE,FALSE,hItemType);
			
			cni.net=NET_NORMAL;
			cni.nt=NT_NORMAL;
			HTREEITEM hItemHodeBagUntil=m_TreeData.InsertItem("Hold Bags Until",cni,FALSE,FALSE,m_hItemPusher);
			cni.nt=NT_RADIOBTN;
			m_hNextProcAvail = m_TreeData.InsertItem("Next processor is available",cni,TRUE,FALSE,hItemHodeBagUntil);
			m_hOtherCondition = m_TreeData.InsertItem("Other Condition",cni, FALSE, FALSE, hItemHodeBagUntil );
			cni.nt=NT_CHECKBOX;
			m_hConveyorIsFull = m_TreeData.InsertItem("The conveyor is full",cni,FALSE,FALSE,m_hOtherCondition);
			
			cni.net=NET_EDITSPIN_WITH_VALUE;
			m_hRandomAmountOfTimePasses=m_TreeData.InsertItem("Random amount of time passes : 0",cni,FALSE,FALSE,m_hOtherCondition);
			m_TreeData.SetItemData(m_hRandomAmountOfTimePasses,0);

			m_hScheduledPickedUp=m_TreeData.InsertItem("Scheduled picked up : 0",cni,FALSE,FALSE,m_hOtherCondition);
			m_TreeData.SetItemData(m_hScheduledPickedUp,0);

			cni.nt=NT_NORMAL;
//			cni.net=NET_EDITSPIN_WITH_VALUE;
			cni.net=NET_EDIT_WITH_VALUE;
			
			CString csLabel;
			csLabel.Format( "Speed (%s/s): 1.0", UNITSMANAGER->GetLengthUnitString() );
			m_hItemSpeed=m_TreeData.InsertItem( csLabel,cni,FALSE,FALSE,m_hOtherBehavior );
			m_TreeData.SetItemData(m_hItemSpeed,0*100);

			cni.net=NET_NORMAL;
			cni.nt=NT_CHECKBOX;
			m_hReversible = m_TreeData.InsertItem("Reversible",cni,FALSE,FALSE,m_hOtherBehavior);


			cni.nt=NT_NORMAL;
			cni.net=NET_NORMAL;
			m_hCapacityFather = m_TreeData.InsertItem( "Capacity Definition",cni,FALSE,FALSE,m_hOtherBehavior );

			cni.nt=NT_RADIOBTN;
			m_hAutoCapacity = m_TreeData.InsertItem( "Auto",cni,FALSE,FALSE,m_hCapacityFather );
			
			cni.nt=NT_RADIOBTN;
			cni.net=NET_EDITSPIN_WITH_VALUE;
			m_hCapacity=m_TreeData.InsertItem( "Capacity : 0",cni,FALSE,FALSE,m_hCapacityFather );
			m_TreeData.SetItemData(m_hCapacity,0);
			
			// Loader Child Node
			cni.nt=NT_NORMAL;
			cni.net=NET_NORMAL;
			m_hHoldingAreaOfLoader = m_TreeData.InsertItem("Holding Area",cni,FALSE,FALSE,m_hItemLoader);

			// Loader Child Node of Sorter
			cni.nt=NT_NORMAL;
			cni.net=NET_NORMAL;
			m_hExitBeltOfSorter = m_TreeData.InsertItem("Exit Belt",cni,FALSE,FALSE,m_hItemSorter);

			m_TreeData.Expand(m_hCapacityFather,TVE_EXPAND);
		}
		break;
	case IntegratedStationProc:
		{

			m_TreeData.SetEnableStatus(m_hLinkage,TRUE);
			m_TreeData.SetEnableStatus(m_hOtherBehavior,FALSE);
			
			m_TreeData.SetEnableStatus(m_hGateItem,FALSE);
			m_TreeData.SetEnableStatus(m_hWaitArea,FALSE);

			cni.nt=NT_NORMAL;
			cni.net=NET_NORMAL;
			m_hPSS=m_TreeData.InsertItem("Platform Share Stations",cni,FALSE,FALSE,m_hLinkage);
		}
		break;
	}

	//dependent processors
    cni.nt = NT_CHECKBOX;
	cni.net = NET_NORMAL;
	m_hDependentProcessors = 
		m_TreeData.InsertItem("Can not start new service if all the follow processors are occupied",
		cni,FALSE,FALSE,m_hOtherBehavior);

	if (m_nProcDataType == GateProc)
	{
		cni.nt = NT_NORMAL;
		cni.net = NET_NORMAL;
		m_hConnectBridge = m_TreeData.InsertItem("Bridge Connector", cni,FALSE,FALSE,m_hLinkage);
	}

	if (m_nProcDataType == HoldAreaProc)
	{
		cni.nt=NT_CHECKBOX;
		cni.net=NET_NORMAL;
		m_hCorralService = m_TreeData.InsertItem("Corral Service",cni,FALSE,FALSE,m_hOtherBehavior);

		cni.nt=NT_NORMAL;
		cni.net=NET_EDITSPIN_WITH_VALUE;
		m_hTimeToCorralService = m_TreeData.InsertItem("Start service after waiting (Seconds): 60",cni,FALSE,FALSE,m_hCorralService);

	}

	m_TreeData.Expand(m_hOtherBehavior,TVE_EXPAND);
	m_TreeData.Expand(m_hGroupSerTimeMod,TVE_EXPAND);
//	m_TreeData.Expand(m_hPopulationCapacity,TVE_EXPAND);
	m_TreeData.Expand(m_hTerminateTime,TVE_EXPAND);
	m_TreeData.Expand(m_hRecycleFreq,TVE_EXPAND);
	m_TreeData.Expand(m_hCapOrDR,TVE_EXPAND);
}

void CProcDataPage::OnEditOrSpinValue() 
{
  // TODO: Add your command handler code here
  HTREEITEM hItem=m_hCurrentSelItem ;
  SetEditValueRange(hItem);
  m_TreeData.DoEdit(hItem);
}

BOOL CProcDataPage::IsCheckTreeItem(HTREEITEM hItem)
{

	return m_TreeData.IsCheckItem(hItem)==1;
}

float CProcDataPage::GetValueTreeItem(HTREEITEM hItem)
{
	CString strValue=m_TreeData.GetItemText(hItem);
	return static_cast<float>(atof(strValue));
}

void CProcDataPage::SaveDataForDisallowGroups()
{
	MiscData* pMiscData = GetCurMiscData();
	if( !pMiscData )
		return;
	BOOL bCheck =IsCheckTreeItem(m_hDisallowGroups);
	pMiscData->SetDisallowGroupFlag( bCheck ? true : false );	
	SetModified();	
}
void CProcDataPage::SaveDataForWaitInQueue()
{
	MiscData* pMiscData = GetCurMiscData();
	if( !pMiscData )
		return;
	BOOL bCheck =IsCheckTreeItem(m_hWaitInQueue);
	((MiscHoldAreaData*)pMiscData)->SetWaitInQueue( bCheck ? true : false );
	SetModified();	
}

void CProcDataPage::SaveDataForApplyServiceTimeAfterGreeting()
{
	MiscData* pMiscData = GetCurMiscData();
	if( !pMiscData )
		return;
	BOOL bCheck =IsCheckTreeItem( m_hApplyServiceTimeAfterGreeting );
	((MiscHoldAreaData*)pMiscData)->setApplyServiceTimeAfterGreeting(bCheck ? true : false);
	SetModified();	
}


void CProcDataPage::SaveDataForTerminateTime()
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;
	UpdateData();
	BOOL b=IsCheckTreeItem(m_hTerminateTime);
	if(b==BST_CHECKED)
	{
		int nTimeValue=m_TreeData.GetItemData(m_hTerminateTime);
		if(nTimeValue>0)
			pMiscData->SetTerminateTime(nTimeValue);
		else
			pMiscData->SetTerminateTime(0);
	}
	else
	{
		pMiscData->SetTerminateTime(0);

	}
	SetModified();
}
//droped
void CProcDataPage::SaveDataForPopulationCapacity()
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;
	UpdateData();
	BOOL b=IsCheckTreeItem(m_hPopulationCapacity);
	if(b==BST_CHECKED)
	{
		int nCapacityValue=m_TreeData.GetItemData(m_hPopulationCapacity);
		if(nCapacityValue>0)
			pMiscData->SetQueueCapacity(nCapacityValue);
		else
			pMiscData->SetQueueCapacity(1);
	}
	else
	{
		pMiscData->SetQueueCapacity(0);

	}
	SetModified();
}

void CProcDataPage::SetEditValueRange(HTREEITEM hItem)
{
	int nMin=0,nMax=100;
	if(hItem==m_hPopulationCapacity)
	{
		m_TreeData.SetItemValueRange(hItem,1,10000);
	}
	else if(hItem==m_hRecycleFreq)
	{
		m_TreeData.SetItemValueRange(hItem,1,1000);
	}
	else if(hItem==m_hTerminateTime)
	{
		m_TreeData.SetItemValueRange(hItem,0,1440);
	}
}

void CProcDataPage::SetItemTextEx(HTREEITEM hItem, COOLTREE_NODE_INFO *pInfo, CString strText,DWORD dwValue)
{
	if( pInfo->net == NET_EDITSPIN_WITH_VALUE||pInfo->net==NET_EDIT_WITH_VALUE )
	{
		CString strValue;
		strValue.Format("%d",dwValue);
		CString strItem = strText+CString(":  ");
		strItem +=strValue;
		m_TreeData.SetItemText( hItem, strItem );
		m_TreeData.SetItemData( hItem, dwValue);
	}	
}

bool CProcDataPage::SelectWaitArea(ProcessorID _id)
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return false;
	
	if( m_hWaitArea == NULL )
		return false;
	
	ProcessorIDList* pProcIDList = pMiscData->getWaitAreaList();
	HTREEITEM hItem = m_TreeData.GetChildItem( m_hWaitArea );
	while( hItem )
	{
		int nDBIdx = m_TreeData.GetItemData( hItem );
		
		const ProcessorID* pProcID = pProcIDList->getID( nDBIdx );
		if( *pProcID == _id )
		{
			m_TreeData.SelectItem( hItem );
			return true;
		}
		hItem = m_TreeData.GetNextItem( hItem, TVGN_NEXT );
	}	
	return false;
}

void CProcDataPage::ReloadWaitAreaList(MiscData *_pMiscData, bool _bReloadLoaderHolding )
{
	HTREEITEM _hWaitArea = _bReloadLoaderHolding ? m_hHoldingAreaOfLoader : m_hWaitArea ;
	// get the item handler
	if( !_hWaitArea )
		return;
	
	DeleteAllChildItems( _hWaitArea );
	
	// inster item
    MiscProcessorIDList* pProcIDList = (MiscProcessorIDList*)_pMiscData->getWaitAreaList ();
    char str[80];
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
    for (int i = 0; i < pProcIDList->getCount (); i++)
    {
		const MiscProcessorIDWithOne2OneFlag* pProcID = (MiscProcessorIDWithOne2OneFlag*)pProcIDList->getID( i );
        pProcID->printID( str );
		if( pProcID->getOne2OneFlag() )
			strcat(str ,"{1:1}");
		
		HTREEITEM hItem = m_TreeData.InsertItem( str,cni,FALSE,FALSE, _hWaitArea );
		m_TreeData.SetItemData( hItem, i );
    }
	m_TreeData.Expand( _hWaitArea, TVE_EXPAND );
	m_TreeData.SelectItem(_hWaitArea);
}

void CProcDataPage::AddWaitArea()
{
	CProcesserDialog dlg( m_pInTerm );
	std::vector<int> procTypeList;
	procTypeList.push_back( HoldAreaProc );
	procTypeList.push_back( LineProc );
	dlg.SetTypeList( procTypeList );
	
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;


	if( dlg.DoModal() == IDCANCEL )
		return;
	
//	ProcessorID id;
//	if( !dlg.GetProcessorID(id) )
//		return;
//	
//	if( SelectWaitArea( id ) )	// check if exist.
//		return;

	ProcessorID id;	
	ProcessorIDList idList;
	ProcessorIDList idListShow;
	if( !dlg.GetProcessorIDList(idList) )
		return;	
	
	int nIDcount = idList.getCount();
	for(int i = 0; i < nIDcount; i++)
	{
		if (!SelectWaitArea(*(idList.getID(i))))  // check if exist
		{
			idListShow.addItem(idList[i]);
		}
	}
	
	nIDcount = idListShow.getCount();
	for (i = 0; i < nIDcount; i++ )
	{
		id = *(idListShow.getID(i));	
		MiscProcessorIDList* pProcIDList = (MiscProcessorIDList*)pMiscData->getWaitAreaList();
		MiscProcessorIDWithOne2OneFlag* pID = new MiscProcessorIDWithOne2OneFlag( id );
		pProcIDList->addItem( pID );
	}
	if ( nIDcount > 0 )
	{
		ReloadWaitAreaList( pMiscData );
		SelectWaitArea( id );	// select new item.
		SetModified();
	}
}

void CProcDataPage::DelWaitArea()
{
	HTREEITEM hItem = m_hCurrentSelItem;
	if( hItem == NULL )
		return;
	
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;
	
	ProcessorIDList* pProcIDList = pMiscData->getWaitAreaList();
	if( !pProcIDList )
		return;
	
	int nDBIdx = m_TreeData.GetItemData( hItem );
	pProcIDList->removeItem( nDBIdx );
	ReloadWaitAreaList( pMiscData );	
	SetModified();

}


// Set Conveyor Type in Misc Data;
void CProcDataPage::SetConveyorType( HTREEITEM _hItem )
{
	MiscData* pMiscData = GetCurMiscData();
	if( !pMiscData )
		return;

	UpdateData();
	BOOL b = IsCheckTreeItem( _hItem );
	if( b == BST_CHECKED )
	{
		enum SUBCONVEYORTYPE eType;
		if( _hItem == m_hItemLoader )
			eType = LOADER;
		else if( _hItem == m_hItemSimpleConveyor )
			eType = SIMPLE_CONVEYOR;
		else if( _hItem == m_hItemUnitBelt )
			eType = UNIT_BELT;
		else if( _hItem == m_hItemMergeBox )
			eType = MERGE_BOX;
		else if( _hItem == m_hItemFeeder )
			eType = FEEDER;
		else if( _hItem == m_hItemScanner )
			eType = SCANNER;
		else if( _hItem == m_hItemSplitter )
			eType = SPLITER;
		else if( _hItem == m_hItemPusher )
			eType = PUSHER;
		else if( _hItem == m_hItemFlowBelt )
			eType = FLOW_BELT;
		else if(_hItem == m_hItemSorter )
			eType = SORTER;
		else
			return;
		((MiscConveyorData*)pMiscData)->SetSubType( eType );
		SetModified();			
	}
}

void CProcDataPage::AddWaitAreaForLoader()
{
	CProcesserDialog dlg( m_pInTerm );
	dlg.SetType( HoldAreaProc );

	if( dlg.DoModal()  == IDCANCEL )
		return;
	
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;
	
//	ProcessorID id;
//	if( !dlg.GetProcessorID( id ) )
//		return;
	
//	MiscProcessorIDList* pProcIDList = (MiscProcessorIDList*)pMiscData->getWaitAreaList();
//	// check the processor if exist 
//	for( int i= 0; i< pProcIDList->getCount(); i++ )
//	{
//		const ProcessorID* pProcID = pProcIDList->getID( i );
//		if( *pProcID == id )	// still exist in the list
//			return;
//	}

	ProcessorID id;	
	ProcessorIDList idList;
	ProcessorIDList idListShow;
	if( !dlg.GetProcessorIDList(idList) )
		return;	
	
	int nIDcount = idList.getCount();
	for(int i = 0; i < nIDcount; i++)
	{
		if (!SelectWaitAreaForLoader(*(idList.getID(i))))  // check if exist
		{
			idListShow.Add(idList[i]);
		}
	}
	
	nIDcount = idListShow.getCount();
	for (i = 0; i < nIDcount; i++ )
	{
		id = *(idListShow.getID(i));		
		MiscProcessorIDList* pProcIDList = (MiscProcessorIDList*)pMiscData->getWaitAreaList();
		MiscProcessorIDWithOne2OneFlag* pID = new MiscProcessorIDWithOne2OneFlag( id );
		pProcIDList->addItem( pID );
	}	
	
/*
	MiscProcessorIDList* pProcIDList = (MiscProcessorIDList*)pMiscData->getWaitAreaList();
	int nIDcount = idList.getCount();
	for(int i = 0; i < nIDcount; i++)
	{
		bool bExist = FALSE;
		// check the processor if exist 
		for(int j= 0; j< pProcIDList->getCount(); j++ )
		{
			const ProcessorID* pProcID = pProcIDList->getID( j );
			if( *pProcID == *idList[i] )	// still exist in the list
			{
				bExist = TRUE;
				break;
			}				
		}
		if (!bExist)
		{
			idListShow.addItem(idList[i]);
		}
		
	}
	
	nIDcount = idListShow.getCount();
	for (i = 0; i < nIDcount; i++ )
	{
		id = *(idListShow.getID(i));		
		MiscProcessorIDWithOne2OneFlag* pID = new MiscProcessorIDWithOne2OneFlag( id );
		pProcIDList->addItem( pID );
	}
*/
	if ( nIDcount > 0 )
	{
		ReloadWaitAreaList( pMiscData, true );
		SetModified();
	}
}

void CProcDataPage::DelWaitAreaForLoader()
{
	HTREEITEM hItem = m_hCurrentSelItem;
	if( hItem == NULL )
		return;
	
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;
	
	ProcessorIDList* pProcIDList = pMiscData->getWaitAreaList();
	if( !pProcIDList )
		return;
	
	int nDBIdx = m_TreeData.GetItemData( hItem );
	pProcIDList->removeItem( nDBIdx );
	ReloadWaitAreaList( pMiscData, true );	
	SetModified();
	
}

void CProcDataPage::SetCapacityValue( int _iValue )
{
	COOLTREE_NODE_INFO* pInfo = m_TreeData.GetItemNodeInfo( m_hCapacity );
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData && pInfo)
	{
		pMiscData->SetQueueCapacity(_iValue);
		SetItemTextEx(m_hCapacity,pInfo,"Capacity ",_iValue);
	}
}

// For Adding Platform Share Station 
void CProcDataPage::AddPSS()
{
	CProcesserDialog dlg( m_pInTerm );
	std::vector<int> procTypeList;
	procTypeList.push_back( IntegratedStationProc);
	dlg.SetTypeList( procTypeList );
	
	if( dlg.DoModal() == IDCANCEL )
		return;

	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;
	
//	ProcessorID id;
//	if( !dlg.GetProcessorID(id) )
//		return;
//	
//	if( SelectPSS( id ) )	// check if exist.
//		return;
	
	ProcessorID id;	
	ProcessorIDList idList;
	ProcessorIDList idListShow;
	if( !dlg.GetProcessorIDList(idList) )
		return;	
	
	int nIDcount = idList.getCount();
	for(int i = 0; i < nIDcount; i++)
	{
		if (!SelectPSS(*(idList.getID(i))))  // check if exist
		{
			idListShow.addItem(idList[i]);
		}
	}
	
	nIDcount = idListShow.getCount();
	for (i = 0; i < nIDcount; i++ )
	{
		id = *(idListShow.getID(i));		
		MiscProcessorIDList* pProcIDList =(MiscProcessorIDList*)(((MiscIntegratedStation*)pMiscData)->GetStationList());
		MiscProcessorIDWithOne2OneFlag* pID = new MiscProcessorIDWithOne2OneFlag( id );
		pProcIDList->addItem( pID );
	}
	if ( nIDcount > 0)
	{
		ReloadStationList( pMiscData );
		SelectPSS( id );	// select new item.
		SetModified();
	}
}

void CProcDataPage::DelPSS()
{
	HTREEITEM hItem = m_hCurrentSelItem;
	if( hItem == NULL )
		return;
	
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;
	
	ProcessorIDList* pProcIDList = ((MiscIntegratedStation*)pMiscData)->GetStationList();
	if( !pProcIDList )
		return;
	
	int nDBIdx = m_TreeData.GetItemData( hItem );
	pProcIDList->removeItem( nDBIdx );
	ReloadStationList( pMiscData );	
	SetModified();
}

bool CProcDataPage::SelectPSS(ProcessorID _id)
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return false;
	
	if( m_hPSS == NULL )
		return false;
	
	ProcessorIDList* pProcIDList = (MiscProcessorIDList*)(((MiscIntegratedStation*)pMiscData)->GetStationList());
	HTREEITEM hItem = m_TreeData.GetChildItem( m_hPSS );
	while( hItem )
	{
		int nDBIdx = m_TreeData.GetItemData( hItem );
		
		const ProcessorID* pProcID = pProcIDList->getID( nDBIdx );
		if( *pProcID == _id )
		{
			m_TreeData.SelectItem( hItem );
			return true;
		}
		hItem = m_TreeData.GetNextItem( hItem, TVGN_NEXT );
	}	
	return false;
}

void CProcDataPage::ReloadStationList(MiscData *_pMiscData)
{
	// get the item handler
	if( !m_hPSS )
		return;
	
	DeleteAllChildItems( m_hPSS );
	
	// inster item
    MiscProcessorIDList* pProcIDList = (MiscProcessorIDList*)(((MiscIntegratedStation*)_pMiscData)->GetStationList());
    char str[80];
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
    for (int i = 0; i < pProcIDList->getCount (); i++)
    {
		const MiscProcessorIDWithOne2OneFlag* pProcID = (MiscProcessorIDWithOne2OneFlag*)pProcIDList->getID( i );
        pProcID->printID( str );
//		if( pProcID->getOne2OneFlag() )
//			strcat(str ,"{1:1}");
		
		HTREEITEM hItem = m_TreeData.InsertItem( str,cni,FALSE,FALSE, m_hPSS );
		m_TreeData.SetItemData( hItem, i );
    }
	m_TreeData.Expand( m_hPSS, TVE_EXPAND );
	m_TreeData.SelectItem(m_hPSS);
}

BOOL CProcDataPage::OnToolTipText(UINT, NMHDR *pNMHDR, LRESULT *pResult)
{
  return TRUE;
}



void CProcDataPage::ReloadLinkedDestList(MiscData *_pMiscData)
{
	// get the item handler
	// get the item handler
	if( !m_hLinkedDestItem )
		return;
	
	DeleteAllChildItems( m_hLinkedDestItem );
	
	// inster item
    MiscProcessorIDList* pProcIDList = (MiscProcessorIDList*)_pMiscData->getLinkedDestList();
    char str[80];
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
    for (int i = 0; i < pProcIDList->getCount (); i++)
    {
		const MiscProcessorIDWithOne2OneFlag* pProcID = (MiscProcessorIDWithOne2OneFlag*)pProcIDList->getID( i );
        pProcID->printID( str );
		if( pProcID->getOne2OneFlag() )
			strcat(str ,"{1:1}");
		
		HTREEITEM hItem = m_TreeData.InsertItem( str,cni,FALSE,FALSE, m_hLinkedDestItem );
		m_TreeData.SetItemData( hItem, i );
    }
	m_TreeData.Expand( m_hLinkedDestItem, TVE_EXPAND );
	m_TreeData.SelectItem(m_hLinkedDestItem);
}

bool CProcDataPage::SelectLinkedDest(ProcessorID _id)
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return false;
	
	if( m_hGateItem == NULL )
		return false;
	
	ProcessorIDList* pProcIDList = pMiscData->getLinkedDestList();
	HTREEITEM hItem = m_TreeData.GetChildItem( m_hLinkedDestItem);
	while( hItem )
	{
		int nDBIdx = m_TreeData.GetItemData( hItem );
		
		const ProcessorID* pProcID = pProcIDList->getID( nDBIdx );
		if( *pProcID == _id )
		{
			m_TreeData.SelectItem( hItem );
			return true;
		}
		hItem = m_TreeData.GetNextItem( hItem, TVGN_NEXT );
	}	
	return false;
}

bool CProcDataPage::SelectWaitAreaForLoader(ProcessorID _id)
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return false;
	
	if( m_hGateItem == NULL )
		return false;
	
	ProcessorIDList* pProcIDList = pMiscData->getWaitAreaList();
	HTREEITEM hItem = m_TreeData.GetChildItem( m_hHoldingAreaOfLoader );
	while( hItem )
	{
		int nDBIdx = m_TreeData.GetItemData( hItem );
		
		const ProcessorID* pProcID = pProcIDList->getID( nDBIdx );
		if( *pProcID == _id )
		{
			m_TreeData.SelectItem( hItem );
			return true;
		}
		hItem = m_TreeData.GetNextItem( hItem, TVGN_NEXT );
	}	
	return false;
}

void CProcDataPage::AddExitBeltOfSorter()
{

	CProcesserDialog dlg( m_pInTerm );
	dlg.SetType( ConveyorProc );
	
	if( dlg.DoModal()  == IDCANCEL )
		return;
	
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;
	
//	ProcessorID id;
//	if( !dlg.GetProcessorID( id ) )
//		return;
//
//	if( SelectExitBelt( id ) )	// check if exist.
//		return;	

	ProcessorID id;	
	ProcessorIDList idList;
	ProcessorIDList idListShow;
	if( !dlg.GetProcessorIDList(idList) )
		return;	
	
	int nIDcount = idList.getCount();
	for(int i = 0; i < nIDcount; i++)
	{
		if (!SelectExitBelt(*(idList.getID(i))))  // check if exist
		{
			idListShow.addItem(idList[i]);
		}
	}

	nIDcount = idListShow.getCount();
	for (i = 0; i < nIDcount; i++ )
	{
		id = *(idListShow.getID(i));			
		
		ProcessorIDList* pProcIDList = (ProcessorIDList*)((MiscConveyorData *)pMiscData)->getExitBeltList();
		ProcessorID  * pID = new ProcessorID( id );
		pProcIDList->addItem( pID );
	}
	
	if ( nIDcount > 0)
	{
		ReloadExitBeltList( pMiscData );
		SelectExitBelt( id );
		SetModified();	
	}
}

void CProcDataPage::DelExitBeltOfSorter()
{

	HTREEITEM hItem = m_hCurrentSelItem;
	if( hItem == NULL )
		return;
	
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;
	
	ProcessorIDList* pProcIDList = (ProcessorIDList*)((MiscConveyorData *)pMiscData)->getExitBeltList();
	if( !pProcIDList )
		return;
	
	int nDBIdx = m_TreeData.GetItemData( hItem );
	pProcIDList->removeItem( nDBIdx );
	ReloadExitBeltList( pMiscData);	
	SetModified();	

}

void CProcDataPage::ReloadExitBeltList(MiscData *_pMiscData)
{
	HTREEITEM _hExitBelt = m_hExitBeltOfSorter ;
	// get the item handler
	if( !_hExitBelt )
		return;
	
	DeleteAllChildItems( _hExitBelt );	
	// inster item
	ProcessorIDList* pProcIDList = (ProcessorIDList*)(((MiscConveyorData *)_pMiscData)->getExitBeltList());
	char str[80];
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	for (int i = 0; i < pProcIDList->getCount (); i++)
	{
		const ProcessorID* pProcID = pProcIDList->getID( i );
		pProcID->printID( str );
		
		HTREEITEM hItem = m_TreeData.InsertItem( str,cni,FALSE,FALSE, _hExitBelt );
		m_TreeData.SetItemData( hItem, i );
	}
	m_TreeData.Expand( _hExitBelt, TVE_EXPAND );
	m_TreeData.SelectItem( _hExitBelt );
}

bool CProcDataPage::SelectExitBelt(ProcessorID _id)
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return false;
	
	if( m_hExitBeltOfSorter == NULL )
		return false;
	
	ProcessorIDList* pProcIDList = (ProcessorIDList*)(((MiscConveyorData *)pMiscData)->getExitBeltList());
	HTREEITEM hItem = m_TreeData.GetChildItem( m_hExitBeltOfSorter);
	while( hItem )
	{
		int nDBIdx = m_TreeData.GetItemData( hItem );
		
		const ProcessorID* pProcID = pProcIDList->getID( nDBIdx );
		if( *pProcID == _id )
		{
			m_TreeData.SelectItem( hItem );
			return true;
		}
		hItem = m_TreeData.GetNextItem( hItem, TVGN_NEXT );
	}	
	return false;
}


void CProcDataPage::SetToolBarState(const HTREEITEM hItem )
{

	if( !hItem )
		return;
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );
	COOLTREE_NODE_INFO* pCNI=m_TreeData.GetItemNodeInfo(hItem);
	if(pCNI->net==NET_EDITSPIN_WITH_VALUE||pCNI->net==NET_EDIT_WITH_VALUE)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_OR_SPIN_VALUE,m_TreeData.IsEnableItem(hItem));
	}
	else if(hItem!=m_hOtherBehavior&&hItem!=m_hDisallowGroups&&
		hItem!=m_hCapOrDR&&hItem!=m_hPopulationCapacity&&hItem!=m_hTerminateTime&&
		hItem!=m_hRecycleFreq)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_OR_SPIN_VALUE,FALSE);
		
		if( m_TreeData.GetParentItem( hItem ) ==m_hLinkage ||
			hItem == m_hHoldingAreaOfLoader||hItem == m_hExitBeltOfSorter)
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, TRUE );
		else if(m_TreeData.GetChildItem(hItem)==NULL)
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, TRUE );
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, TRUE );
		}
	}
	else
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_OR_SPIN_VALUE,FALSE);
	
	if(m_nProcDataType==IntegratedStationProc)
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );
	
	if(m_TreeData.IsEnableItem(hItem)==FALSE)
	{	
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_OR_SPIN_VALUE,FALSE);
	}
	//Enable Delete Button for capacity item
	if(m_TreeData.GetParentItem(hItem) == m_hTreeItemCapacity 
		|| m_TreeData.GetParentItem(hItem) == m_hDependentProcessors)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, TRUE );
	}

	if(hItem == m_hDependentProcessors)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );
	}
	HTREEITEM paritem = m_TreeData.GetParentItem(hItem) ;
	if(paritem!= NULL && m_TreeData.GetParentItem(paritem) != m_hTreeItemCapacity )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, TRUE );
	}

	if (hItem == m_hGateItem||
		hItem == m_hWaitArea||
		hItem == m_hLinkedDestItem||
		hItem == m_hConnectBridge)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );
	}

	if(hItem == m_hStandConnect)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );
	}
	if(hItem == m_hTreeItemCapacity)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );

	}
}


void CProcDataPage::ReloadGrouSerTimeMod(MiscData *_pMiscData)
{
	int nSel;
	int nSumPercent;
	nSel = _pMiscData->GetGroupSerTimeModIndex();
	//make input data available
	if ( nSel >5 || nSel < 0){ nSel = 0;}
	nSumPercent = _pMiscData->GetGroupSerTimeModSum();
	if (nSumPercent>100 || nSumPercent < 0) { nSumPercent = 100;}
   
	CString strValue;
	strValue.Format("%d",nSumPercent);
	CString strItem = CString("Sum: ")+strValue+"%";				
	m_TreeData.SetItemText( m_hSumOfGroupSerTimeMod , strItem );
	m_TreeData.SetItemData( m_hSumOfGroupSerTimeMod, nSel);	

	HTREEITEM hSelItem;	
	switch(nSel)
	{
	case 0:
		hSelItem = m_hSumOfGroupSerTimeMod;
		break;
	case 1:
		hSelItem = m_hAverageOfGroupSerTimeMod;		
		break;
	case 2:
		hSelItem = m_hMinimumOfGroupSerTimeMod;		
		break;
	case 3:
		hSelItem = m_hMaximumOfGroupSerTimeMod;		
		break;
	case 4:
		hSelItem = m_hRandomOfGroupSerTimeMod ;		
		break;
	default:
		hSelItem = m_hSumOfGroupSerTimeMod;
	}
	m_TreeData.SetRadioStatus(hSelItem,TRUE);
}

void CProcDataPage::AddGreetingHall()
{

	CProcesserDialog dlg( m_pInTerm );
	dlg.SetType( HoldAreaProc );
	
	if( dlg.DoModal()  == IDCANCEL )
		return;
	
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;
//	ProcessorID id;
//	if( !dlg.GetProcessorID( id ) )
//		return;
//	
//	if( SelectGreetingHall( id ) )	// check if exist.
//		return;	
//	

	ProcessorID id;	
	ProcessorIDList idList;
	ProcessorIDList idListShow;
	if( !dlg.GetProcessorIDList(idList) )
		return;	
	
	int nIDcount = idList.getCount();
	for(int i = 0; i < nIDcount; i++)
	{
		if (!SelectGreetingHall(*(idList.getID(i))))  // check if exist
		{
			idListShow.addItem(idList[i]);
		}
	}
	
	nIDcount = idListShow.getCount();
	for (i = 0; i < nIDcount; i++ )
	{
		id = *(idListShow.getID(i));		
		
		ProcessorIDList* pProcIDList = (ProcessorIDList*)((MiscBaggageData *)pMiscData)->getHoldAreaList();
		ProcessorID  * pID = new ProcessorID( id );
		pProcIDList->addItem( pID );
	}
	if ( nIDcount > 0 )
	{
		ReloadHoldArerList( pMiscData );
		SelectGreetingHall( id );   // select new item.
		SetModified();		
	}
}

bool CProcDataPage::SelectGreetingHall(ProcessorID _id)
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return false;
	
	if( m_hGreetingHall == NULL )
		return false;
	
	ProcessorIDList* pProcIDList = (ProcessorIDList*)(((MiscBaggageData *)pMiscData)->getHoldAreaList());
	HTREEITEM hItem = m_TreeData.GetChildItem( m_hGreetingHall);
	while( hItem )
	{
		int nDBIdx = m_TreeData.GetItemData( hItem );
		
		const ProcessorID* pProcID = pProcIDList->getID( nDBIdx );
		if( *pProcID == _id )
		{
			m_TreeData.SelectItem( hItem );
			return true;
		}
		hItem = m_TreeData.GetNextItem( hItem, TVGN_NEXT );
	}	
	return false;	
}

void CProcDataPage::ReloadHoldArerList(MiscData *_pMiscData)
{
	// get the item handler
	if( !m_hGreetingHall )
		return;
	
	DeleteAllChildItems( m_hGreetingHall );
	
	// inster item
    ProcessorIDList* pProcIDList = (ProcessorIDList*)(((MiscBaggageData *)_pMiscData)->getHoldAreaList());
    char str[80];
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
    for (int i = 0; i < pProcIDList->getCount (); i++)
    {
		const ProcessorID * pProcID = (ProcessorID *)pProcIDList->getID( i );
        pProcID->printID( str );
		
		HTREEITEM hItem = m_TreeData.InsertItem( str,cni,FALSE,FALSE, m_hGreetingHall );
		m_TreeData.SetItemData( hItem, i );
    }
	m_TreeData.Expand( m_hGreetingHall, TVE_EXPAND );
	m_TreeData.SelectItem(m_hGreetingHall);	
}

void CProcDataPage::DelGreetingHall()
{
	HTREEITEM hItem = m_hCurrentSelItem;
	if( hItem == NULL )
		return;
	
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;
	
	ProcessorIDList* pProcIDList = (ProcessorIDList*)((MiscBaggageData *)pMiscData)->getHoldAreaList();
	if( !pProcIDList )
		return;
	
	int nDBIdx = m_TreeData.GetItemData( hItem );
	pProcIDList->removeItem( nDBIdx );
	ReloadHoldArerList(pMiscData);	
	SetModified();		
}

void CProcDataPage::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	m_TreeData.SetFocus();
	CPoint pt = point;
	m_TreeData.ScreenToClient( &pt );
	
	UINT iRet;
	hCurrentTreeItem = m_TreeData.HitTest( pt, &iRet );
	if(hCurrentTreeItem == NULL)
		return ;
	if(hCurrentTreeItem != m_hTreeItemCapacity)
	{
		if(m_TreeData.GetParentItem(hCurrentTreeItem) != m_hTreeItemCapacity)
			return ;
		if (0 ==m_TreeData.GetItemText(hCurrentTreeItem).CompareNoCase(_T("No Limit")))
			return ;

		CapacityAttribute* pAttribute = (CapacityAttribute*)m_TreeData.GetItemNodeInfo(hCurrentTreeItem)->nMaxCharNum ;
		if(pAttribute->m_Type != CapacityAttributes::TY_Combination)
			return ;
	}
	//Get Defined Pax
	std::vector<CString> vPaxDefined;
	HTREEITEM hItemChild= m_TreeData.GetChildItem(hCurrentTreeItem);
	while(hItemChild)
	{
		CString strMob = m_TreeData.GetItemText(hItemChild);
		int curPos = 0;
		CString strToken = strMob.Tokenize(":", curPos);
		vPaxDefined.push_back(strToken);
		hItemChild= m_TreeData.GetNextSiblingItem(hItemChild);
	}


	CMenu menuMobileElement;
	menuMobileElement.CreatePopupMenu();
	
	CMenu sepMenu;
	sepMenu.CreateMenu();
	
	//add conbinaton 
	if(hCurrentTreeItem == m_hTreeItemCapacity)
		sepMenu.AppendMenu(MF_STRING | MF_ENABLED , MOBILE_ELEMENT_MENU_COMBINATION, STR_COMBINATION) ;
	//add "all non pax"
	if(std::find(vPaxDefined.begin(),vPaxDefined.end(),CString(STR_ALL_NOPAX)) == vPaxDefined.end() && hCurrentTreeItem == m_hTreeItemCapacity)
		sepMenu.AppendMenu(MF_STRING | MF_ENABLED , MOBILE_ELEMENT_MENU_ALLNOPAX, STR_ALL_NOPAX) ;
	CMobileElemTypeStrDB* pMobElement = m_pInTerm->m_pMobElemTypeStrDB;
	for (int i =0; i< pMobElement->GetCount(); i++)
	{
		CString strMobElement = pMobElement->GetString(i);
		if(std::find(vPaxDefined.begin(), vPaxDefined.end(), strMobElement) != vPaxDefined.end())
			continue;
		sepMenu.AppendMenu(MF_STRING | MF_ENABLED , MOBILE_ELEMENT_MENU_START + i +1, strMobElement);
	}
	menuMobileElement.InsertMenu(0, MF_BYPOSITION|MF_POPUP, (UINT) sepMenu.m_hMenu, _T("Add..."));

	menuMobileElement.TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);

}


void CProcDataPage::OnSelectMobileElementType(UINT nID)
{
	MiscData* pMiscData = GetCurMiscData();
	if (pMiscData == NULL || hCurrentTreeItem == NULL) return;
	if(m_TreeData.GetParentItem(hCurrentTreeItem) == m_hTreeItemCapacity)
	{
		CapacityAttribute* pAttribute = (CapacityAttribute*)m_TreeData.GetItemNodeInfo(hCurrentTreeItem)->nMaxCharNum ;
		if(pAttribute != NULL && pAttribute->m_Type == CapacityAttributes::TY_Combination )
		{
			pAttribute->AddCombinationContainTypeString(GetMobileElementString(nID - 1)) ;

			COOLTREE_NODE_INFO cni;
			CCoolTree::InitNodeInfo(this,cni);
			cni.nt=NT_NORMAL;
			cni.net = NET_NORMAL ;
			HTREEITEM item =  m_TreeData.InsertItem(GetMobileElementString(nID - 1) , 
				cni, FALSE, FALSE, hCurrentTreeItem);
			m_TreeData.Expand(hCurrentTreeItem,TVE_EXPAND) ;
			 return ;
		}
	}
	if(hCurrentTreeItem != m_hTreeItemCapacity)
		return ;
	//Remove NO LIMIT 
	HTREEITEM hTreeItemCapacityChild =  m_TreeData.GetChildItem(m_hTreeItemCapacity);
	if (0 ==m_TreeData.GetItemText(hTreeItemCapacityChild).CompareNoCase(_T("No Limit")))
		m_TreeData.DeleteItem(hTreeItemCapacityChild);
	CapacityAttributes* pAttributes = pMiscData->GetCapacityAttributes();
	CapacityAttribute* pAttribute = new CapacityAttribute;
	pAttribute->m_nCapacity = 1;
	pAttribute->m_nMobElementIndex = nID -1 ;
	pAttributes->AddCapacityAttribute(pAttribute);
	pAttribute->m_Name = GetMobileElementString(nID - 1) ;


	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL;
	cni.net=NET_EDITSPIN_WITH_VALUE;
	cni.nMaxCharNum = (DWORD_PTR)pAttribute ;
	CString strLabel;
	strLabel.Format("%s:  %d",GetMobileElementString(nID - 1), 1);//Fibre

	HTREEITEM hTreeItemPaxCapacity = m_TreeData.InsertItem(strLabel , 
		cni, FALSE, FALSE, m_hTreeItemCapacity);
	

	m_TreeData.SetItemData(hTreeItemPaxCapacity,1);
	m_TreeData.Expand(m_hTreeItemCapacity,TVE_EXPAND);
}

CString CProcDataPage::GetMobileElementString(int nIndex)
{
	ASSERT(m_pInTerm);
	ASSERT(nIndex >=0 && nIndex < m_pInTerm->m_pMobElemTypeStrDB->GetCount());
	
	return m_pInTerm->m_pMobElemTypeStrDB->GetString(nIndex);
}

void CProcDataPage::ReloadCapacityList()
{
	if(m_hTreeItemCapacity == NULL)
		return;

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL;

	MiscData* _pMiscData = GetCurMiscData();
	if( _pMiscData == NULL)
	{//first time define
		m_TreeData.InsertItem(_T("No Limit"),cni, FALSE, FALSE, m_hTreeItemCapacity);
		m_TreeData.Expand(m_hTreeItemCapacity, TVE_EXPAND);
		return;
	}

	//Delete OldData
	while (m_TreeData.ItemHasChildren(m_hTreeItemCapacity))
	{
		HTREEITEM hTreeItemChild = m_TreeData.GetChildItem(m_hTreeItemCapacity);
		m_TreeData.DeleteItem(hTreeItemChild);
	}

	m_mapMobTreeItemMobElement.clear();

	CapacityAttributes* pAttributes =  _pMiscData->GetCapacityAttributes();
	if (pAttributes->GetCount() < 1)
	{
		m_TreeData.InsertItem(_T("No Limit") , 
			cni, FALSE, FALSE, m_hTreeItemCapacity);
		m_TreeData.Expand(m_hTreeItemCapacity, TVE_EXPAND);
		return;
	}

	cni.net=NET_EDITSPIN_WITH_VALUE;

	for (int i = 0; i < pAttributes->GetCount(); i++)
	{
		CapacityAttribute* pAttribute = pAttributes->GetItem(i);
			cni.nMaxCharNum = (DWORD_PTR)pAttribute ;
		CString strMobElement  ;
		if(pAttribute->m_nMobElementIndex != -1)
			strMobElement = GetMobileElementString(pAttribute->m_nMobElementIndex);
		else
			strMobElement = pAttribute->m_Name;

		CString strLabel;
		strLabel.Format("%s:  %d", strMobElement, pAttribute->m_nCapacity);
		HTREEITEM hTreeItemPaxCapacity = m_TreeData.InsertItem(strLabel , 
			cni, FALSE, FALSE, m_hTreeItemCapacity);
		m_TreeData.SetItemData(hTreeItemPaxCapacity, pAttribute->m_nCapacity);
		
		for (int i = 0 ; i < (int)pAttribute->m_ContainData.size() ;i++)
		{
				COOLTREE_NODE_INFO ItemCnit;
			CCoolTree::InitNodeInfo(this,ItemCnit);
			ItemCnit.net = NET_NORMAL ;
			ItemCnit.nt = NT_NORMAL ;
			m_TreeData.InsertItem(pAttribute->m_ContainData[i] , 
				ItemCnit, FALSE, FALSE, hTreeItemPaxCapacity);
		}
		m_TreeData.Expand(m_hTreeItemCapacity, TVE_EXPAND);
		
	}

}

void CProcDataPage::DeleteCapacityItem(HTREEITEM hItem)
{

	if(hItem == NULL)
		return ;
	CapacityAttribute* pattr = (CapacityAttribute*)m_TreeData.GetItemNodeInfo(hItem)->nMaxCharNum ;
	if(pattr == NULL)
		return ;
	MiscData* pMiscData = GetCurMiscData();
	if (pMiscData == NULL) return;

	CapacityAttributes* pAttributes = pMiscData->GetCapacityAttributes();
	pAttributes->RemoveCapacityAttribute(pattr);


	m_TreeData.DeleteItem(hItem);

	if (pAttributes->GetCount() < 1)
	{
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		cni.nt=NT_NORMAL;

		m_TreeData.InsertItem(_T("No Limit") , 
			cni, FALSE, FALSE, m_hTreeItemCapacity);
		m_TreeData.Expand(m_hTreeItemCapacity, TVE_EXPAND);
	}

	m_TreeData.SetFocus();
}
void CProcDataPage::DeleteCapacityCombinationItem(HTREEITEM hItem,HTREEITEM hSunItem)
{
	if(hItem == NULL)
		return ;
	CapacityAttribute* pattr = (CapacityAttribute*)m_TreeData.GetItemNodeInfo(hItem)->nMaxCharNum ;
	if(pattr == NULL)
		return ;
	pattr->RemoveCombinationContainTypeString(m_TreeData.GetItemText(hSunItem)) ;
	m_TreeData.DeleteItem(hSunItem) ;
}

void CProcDataPage::AddBridgeConnector()
{
	CProcesserDialog dlg( m_pInTerm );
	dlg.SetType( BridgeConnectorProc );
	if( dlg.DoModal() == IDCANCEL )
		return;	

	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;

	ProcessorID id;	
	ProcessorIDList idList;
	ProcessorIDList idListShow;

	if( !dlg.GetProcessorIDList(idList) )
		return;	

	int nIDcount = idList.getCount();
	for(int i = 0; i < nIDcount; i++)
	{
		if (!SelectGate(*(idList.getID(i))))  // check if exist
		{
			idListShow.Add(idList[i]);
		}
	}

	nIDcount = idListShow.getCount();
	for (i = 0; i < nIDcount; i++ )
	{
		id = *(idListShow.getID(i));		
		MiscProcessorIDList* pProcIDList = (MiscProcessorIDList*)pMiscData->getBridgeConnectorList();
		MiscProcessorIDWithOne2OneFlag* pID = new MiscProcessorIDWithOne2OneFlag( id );
		pProcIDList->addItem( pID );
	}
	if ( nIDcount > 0)
	{
		ReloadBridgeConnectorList( pMiscData );
		SelectBridgeConnector( id );	// select new item.
		SetModified();
	}
}

void CProcDataPage::DelBridgeConnector(HTREEITEM hItem)
{
	if( hItem == NULL )
		return;

	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;

	ProcessorIDList* pProcIDList = pMiscData->getBridgeConnectorList();
	if( !pProcIDList )
		return;

	int nDBIdx = m_TreeData.GetItemData( hItem );
	pProcIDList->removeItem( nDBIdx );
	ReloadBridgeConnectorList( pMiscData );	
	SetModified();
}

bool CProcDataPage::SelectBridgeConnector( ProcessorID _id )
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return false;

	if( m_hGateItem == NULL )
		return false;

	ProcessorIDList* pProcIDList = pMiscData->getBridgeConnectorList();
	HTREEITEM hItem = m_TreeData.GetChildItem( m_hConnectBridge);
	while( hItem )
	{
		int nDBIdx = m_TreeData.GetItemData( hItem );

		const ProcessorID* pProcID = pProcIDList->getID( nDBIdx );
		if( *pProcID == _id )
		{
			m_TreeData.SelectItem( hItem );
			return true;
		}
		hItem = m_TreeData.GetNextItem( hItem, TVGN_NEXT );
	}	
	return false;
}

void CProcDataPage::ReloadBridgeConnectorList( MiscData* _pMiscData )
{
	// get the item handler
	if( !m_hConnectBridge )
		return;

	DeleteAllChildItems( m_hConnectBridge );

	// inster item
	MiscProcessorIDList* pProcIDList = (MiscProcessorIDList*)_pMiscData->getBridgeConnectorList();
	char str[80];
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	for (int i = 0; i < pProcIDList->getCount (); i++)
	{
		const MiscProcessorIDWithOne2OneFlag* pProcID = (MiscProcessorIDWithOne2OneFlag*)pProcIDList->getID( i );
		pProcID->printID( str );
		if( pProcID->getOne2OneFlag() )
			strcat(str ,"{1:1}");

		HTREEITEM hItem = m_TreeData.InsertItem( str,cni,FALSE,FALSE, m_hConnectBridge );
		m_TreeData.SetItemData( hItem, i );
	}
	m_TreeData.Expand( m_hConnectBridge, TVE_EXPAND );
	m_TreeData.SelectItem(m_hConnectBridge);
}

void CProcDataPage::AddStandConnector()
{
	CProcDataSheet* pWnd=(CProcDataSheet*)GetParent();
	CView* pView=(CView*)(pWnd->m_pParent);

	//get the count of the floor
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)(pView->GetDocument());
	std::vector<int> vAirport;
	InputAirside::GetAirportList(pDoc->GetProjectID(),vAirport);
	if(vAirport.size()<1) return ;


	CSelectALTObjectDialog dlg(0,vAirport.at(0));
	dlg.SetType(ALT_STAND);
	if( dlg.DoModal() == IDCANCEL )
		return;	

	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;

	std::vector<CString> standList;
	ProcessorID id;	
	ProcessorIDList idList;
	ProcessorIDList idListShow;

	if( !dlg.GetObjectIDStringList(standList) )
		return;	

	for (int i = 0; i < (int)standList.size(); i++)
	{
		ProcessorID standID;
		standID.SetStrDict(m_pInTerm->inStrDict);
		standID.setID(standList.at(i));
		idList.Add(&standID);
	}

	int nIDcount = idList.getCount();
	for(i = 0; i < nIDcount; i++)
	{
		if (!SelectStand(*(idList.getID(i))))  // check if exist
		{
			idListShow.Add(idList[i]);
		}
	}

	nIDcount = idListShow.getCount();
	for (i = 0; i < nIDcount; i++ )
	{
		id = *(idListShow.getID(i));		
		MiscProcessorIDList* pProcIDList = ((MiscBridgeConnectorData*)pMiscData)->GetBridgeConnectorLinkedProcList();
		MiscProcessorIDWithOne2OneFlag* pID = new MiscProcessorIDWithOne2OneFlag( id );
		pProcIDList->addItem( pID );
	}
	if ( nIDcount > 0)
	{
		ReloadStandConnectorList( pMiscData );
		SelectStandConnector( id );	// select new item.
		SetModified();
	}
}

void CProcDataPage::DelStandConnector(HTREEITEM hItem)
{
	if( hItem == NULL )
		return;

	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;

	ProcessorIDList* pProcIDList = ((MiscBridgeConnectorData*)pMiscData)->GetBridgeConnectorLinkedProcList();
	if( !pProcIDList )
		return;

	int nDBIdx = m_TreeData.GetItemData( hItem );
	pProcIDList->removeItem( nDBIdx );
	ReloadStandConnectorList( pMiscData );	
	SetModified();
}

bool CProcDataPage::SelectStandConnector(ProcessorID _id)
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return false;

	if( m_hStandConnect == NULL )
		return false;

	ProcessorIDList* pProcIDList = ((MiscBridgeConnectorData*)pMiscData)->GetBridgeConnectorLinkedProcList();
	HTREEITEM hItem = m_TreeData.GetChildItem( m_hStandConnect);
	while( hItem )
	{
		int nDBIdx = m_TreeData.GetItemData( hItem );

		const ProcessorID* pProcID = pProcIDList->getID( nDBIdx );
		if( *pProcID == _id )
		{
			m_TreeData.SelectItem( hItem );
			return true;
		}
		hItem = m_TreeData.GetNextItem( hItem, TVGN_NEXT );
	}	
	return false;
}

void CProcDataPage::ReloadStandConnectorList(MiscData* _pMiscData)
{
	// get the item handler
	if( !m_hStandConnect )
		return;

	DeleteAllChildItems( m_hStandConnect );

	// inster item
	MiscProcessorIDList* pProcIDList = ((MiscBridgeConnectorData*)_pMiscData)->GetBridgeConnectorLinkedProcList();
	char str[80];
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	for (int i = 0; i < pProcIDList->getCount (); i++)
	{
		const MiscProcessorIDWithOne2OneFlag* pProcID = (MiscProcessorIDWithOne2OneFlag*)pProcIDList->getID( i );
		pProcID->printID( str );
		if( pProcID->getOne2OneFlag() )
			strcat(str ,"{1:1}");

		HTREEITEM hItem = m_TreeData.InsertItem( str,cni,FALSE,FALSE, m_hStandConnect );
		m_TreeData.SetItemData( hItem, i );
	}
	m_TreeData.Expand( m_hStandConnect, TVE_EXPAND );
	m_TreeData.SelectItem(m_hStandConnect);
}

void CProcDataPage::InitSpecificBehvaior()
{
	
}