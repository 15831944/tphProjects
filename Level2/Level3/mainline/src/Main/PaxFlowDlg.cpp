// PaxFlowDlg.cpp : implementation file
//
#include "stdafx.h"
#include "termplan.h"
#include "PaxFlowDlg.h"
#include "PassengerTypeDialog.h"
#include "ProcesserDialog.h"
#include "3DView.h"
#include "ChildFrm.h"
#include "inputs\in_term.h"
#include "inputs\typelist.h"
#include "inputs\probab.h"
#include "inputs\procdist.h"
#include "Common\exeption.h"
#include "common\WinMsg.h"
#include "..\engine\proclist.h"
#include "TermPlanDoc.h"
#include "PaxFlowSelectPipes.h"
#include "inputs\PipeDataSet.h"
#include "FlightDialog.h"
#include "inputs\SubFlowList.h"
#include "inputs\SubFlow.h"
#include "MainFrm.h"
#include "InputValueDlg.h"
#include "PaxFlowAvoidDensityDlg.h"
#include "Finddlg.h"
#include "Inputs\DirectionUtil.h"
#include "..\inputs\InputFlowSync.h"
#include "..\Inputs\HandleSingleFlowLogic.h"
#include "..\Inputs\InputFlowSync.h"
#include "DlgSyncPointDefine.h"
#include "PickConveyorTree.h"

#define SHOW_STYLE SWP_NOZORDER|SWP_NOACTIVATE
#define PROCESS_COLOR RGB( 145,79,91 )
#define PROCESS_COLOR_GRAY RGB(128,128,128)
#define PROCESS_COLOR_RED RGB(128,0,0)

#define ARRIVE_DEPATURE_PAX_COLOR RGB(0,0,0)
#define TRANSFER_TRANSIT_PAX_COLOR RGB(80,80,80)
#define VISITOR_COLOR RGB(160,160,160)
const CString CS_CYCLIC_LABEL = "[ CYCLIC ]";

extern const CString c_setting_regsectionstring ;
const CString c_PaxFlow_sorting_entry = "Pax Flow Sorting String";
//the color of passenger type list
COLORREF COLER253[]={ 0xFFFFCC,0xCCFFCC,0xCCCCFF,0xFF33FF,0xFFCC99,0xFFCCCC,0xFF9999,0xCCFFFF,0x99CCFF,0xCC66FF/*10*/, 0xCC6600,0x00CC99,0x009933,0x9999FF,0xB8860B,0xA9A9A9,0x3399CC,0xBDB76B,0x8B008B,0x33CC99,0xFF8C00,0x9932CC,0x00CCFF,0xE9967A,0x8FBC8F,0x483D8B,0x00CCCC,0x00CED1,0x9400D3,0xFF1493,  0x00BFFF,0x696969,0x1E90FF,0x00FF99,0xFFFAF0,0x228B22,0xFF00FF,0xDCDCDC,0xF8F8FF,0xFFD700,0xDAA520,0x808080,0x008000,0xADFF2F,0xF0FFF0,0xFF69B4,0xCD5C5C,0x9999FF,0xFFFFF0,0xF0E68C, 0xE6E6FA,0xFFF0F5,0x7CFC00,0xFFFACD,0xADD8E6,0xF08080,0xE0FFFF,0xFAFAD2,0x90EE90,0xD3D3D3,0xFFB6C1,0xFFA07A,0x20B2AA,0x87CEFA,0x778899,0xB0C4DE,0xFFFFE0,0x00FF00,0x32CD32,0xFAF0E6,0xFF00FF,0x800000,0x66CDAA,0x0000CD,0xBA55D3,0x9370DB,0x3CB371,0x7B68EE,0x00FA9A,0x48D1CC,0xC71585,0x191970,0xF5FFFA,0xFFE4E1,0xFFE4B5,0xFFDEAD,0x000080,0xFDF5E6,0x808000,0x6B8E23,0xFFA500,0xFF4500,0xDA70D6,0xEEE8AA,0x98FB98,0xAFEEEE,0xDB7093,0xFFEFD5,0xFFDAB9,0xCD853F,0xFFC0CB,0xDDA0DD,0xB0E0E6,0x800080,0xFF0000,0xBC8F8F,0x4169E1,0x8B4513,0xFA8072,0xF4A460,0x2E8B57,0xFFF5EE,0xA0522D,0xC0C0C0,0x87CEEB,0x6A5ACD,0x708090,0xFFFAFA,0x00FF7F,0x4682B4,0xD2B48C,0x008080,0xD8BFD8,0xFF6347,0x40E0D0,0xEE82EE,0xF5DEB3,0xF5F5F5,0xFFFF00,0x9ACD32};

static const UINT SYNCPOINT_STARTID = 50000;

/////////////////////////////////////////////////////////////////////////////
// CPaxFlowDlg dialog
std::vector<ProcessorID> CPaxFlowDlg::m_vInterestInPath;

CPaxFlowDlg::CPaxFlowDlg(InputTerminal* _pTerm,CWnd* pParent)
: CDialog(CPaxFlowDlg::IDD, pParent),m_OperatedPaxFlow( _pTerm )
{
	//{{AFX_DATA_INIT(CPaxFlowDlg)
	//}}AFX_DATA_INIT
	m_bDragging				= false;
	m_nDragedProcess=-1;
	m_hRClickItem = NULL;
	m_hModifiedItem = NULL;
	m_hGateRClickItem =NULL;
	//m_pOperatedPaxFlow = NULL;
	m_pDragImage			= NULL;

	m_nCurSel = -1;
	m_bDisplayBaggageDeviceSign=FALSE;
	m_bChanged = false;
	m_iChangedIdx = -1;
	m_iListSelect =-1;
	m_pInTerm = _pTerm;
	m_nProcType = -1;
	m_hItemDragSrc			= NULL;
	m_hItemDragDes			= NULL;
	
	m_bGateChanged = false;
	m_iGateChangedIdx = -1;
	m_bExpand		= FALSE;
	m_bUpTree		= TRUE;
	m_bHaveToGate	= FALSE;
	m_bHaveFromGate = FALSE;
	m_sizeLastWnd=CSize(0,0);
	m_pNonPaxFlow = NULL;
	m_pPaxFlowAttachedToNonPaxFlow = NULL;
	m_bCheckConveyorBtn=FALSE;
	m_nSel = -1;
	m_nSelChange = -2;
	m_iPreSortByColumnIdx = -1 ;

	m_treeGate.setInputTerminal( _pTerm );
	m_FlowTree.setInputTerminal( _pTerm );
	m_vFoundItem.clear();
	m_iterator = m_vFoundItem.end();
	m_strFind = "";
	m_bMatchWholeWord = FALSE;
	m_vStrBuff.clear();
	m_b1x1SeqError = FALSE;
	m_nLevelLen = 0;
	m_vCopyItem.clear();
	m_vCopyProcessors.clear();
}

CPaxFlowDlg::~CPaxFlowDlg()
{
	// clear drag image
	if( m_pDragImage )
		delete m_pDragImage;

	m_vCopyItem.clear();
	m_vCopyProcessors.clear();
}


void CPaxFlowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaxFlowDlg)
	DDX_Control(pDX, IDC_CHECK_DISPLAY_FLOW, m_checkDisplayFlow);
	DDX_Control(pDX, IDC_STATIC_PROCESSOR, m_staticProcessor);
	DDX_Control(pDX, IDC_STATIC_PROCESS, m_staticProcess);
	DDX_Control(pDX, IDC_STATIC_FLOWTREE, m_staticFlowTree);
	DDX_Control(pDX, IDC_PROCESS_TREE, m_ProcessTree);
	DDX_Control(pDX, IDC_TREE_PROCESSOR, m_ProcTree);
	DDX_Control(pDX, IDC_STATIC_MTYPE_TAG, m_staticMobileTag);
	DDX_Control(pDX, IDC_FLIGHT_TITLE, m_staticFlightTitle);
	DDX_Control(pDX, IDC_GATE_TITLE, m_staticGateTitle);
	DDX_Control(pDX, IDC_BUT_RESTORE, m_butRestore);
	DDX_Control(pDX, IDC_TREE_GATE, m_treeGate);
	DDX_Control(pDX, IDC_LIST_FLIGHT, m_listFlight);
	DDX_Control(pDX, IDC_BUTTON_DETAIL, m_butAdvance);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_LIST_PAXTYPE, m_listCtrlPaxType);
	DDX_Control(pDX, IDC_TREE_FLOW, m_FlowTree);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CPaxFlowDlg, CDialog)
	//{{AFX_MSG_MAP(CPaxFlowDlg)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_DELETE, OnBtnDelete)
	ON_BN_CLICKED(IDC_BTN_NEW, OnBtnNew)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBtnSave)
	ON_WM_CONTEXTMENU()
	ON_LBN_SELCHANGE(IDC_LIST_PAXTYPE, OnSelchangeListPaxtype)
	ON_COMMAND(ID_PAXFLOW_ADDCHILDPROCESSOR, OnPaxflowAddchildprocessor)
	ON_COMMAND(ID_PAXFLOW_DELETEPROCESSORS, OnPaxflowDeleteprocessors)
	ON_COMMAND(ID_PAXFLOW_MODIFYPERCENT, OnPaxflowModifypercent)
	ON_COMMAND(ID_PAXFLOW_DELPERCENT, OnPaxflowDelpercent)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FLOW, OnSelchangedTreeFlow)
	ON_COMMAND(IDC_BUTTON_PRINT, OnButtonPrint)
	ON_COMMAND(ID_PAXFLOW_CUT_SOURCE_LINK, OnPaxflowCutSourceLink)
	ON_COMMAND(ID_PAXFLOW_TAKEOVER, OnPaxflowTakeover)
	ON_COMMAND(ID_PAXFLOW_TAKEOVER_BYALL, OnPaxflowTakeoverByall)
	ON_COMMAND(ID_PAXFLOW_INSERT_BETTWEEN, OnPaxflowInsertBettween)
	ON_COMMAND(ID_PAXFLOW_EDIT_PIPE, OnPaxflowEditPipe)
	ON_COMMAND(ID_PAXFLOW_EDIT_CONVEYOR, OnPaxflowEditConveyor)
	ON_COMMAND(ID_PAXFLOW_INSERT_AFTER, OnPaxflowInsertAfter)
	ON_COMMAND(ID_PAXFLOW_ADD_ISOLATE_NODE, OnPaxflowAddIsolateNode)
	ON_COMMAND(ID_PAXFLOW_ONETOONE, OnPaxflowOnetoone)
	ON_COMMAND(ID_PAXFLOW_INSERT_FROMGATE, OnPaxflowInsertFromgate)
	ON_COMMAND(ID_PAXFLOW_INSERT_TOGATE, OnPaxflowInsertTogate)
	ON_BN_CLICKED(IDC_BUTTON_DETAIL, OnButAdvance)
	ON_LBN_SELCHANGE(IDC_LIST_FLIGHT, OnSelchangeListFlight)
	ON_NOTIFY(NM_KILLFOCUS, IDC_TREE_GATE, OnKillfocusTreeGate)
	ON_BN_CLICKED(IDC_BUT_RESTORE, OnButRestore)
	ON_COMMAND(ID_PAXFLOW_EVEN_PERCENT, OnPaxflowEvenPercent)
	ON_COMMAND(ID_PAXFLOW_INSERT_END, OnPaxflowInsertEnd)
	ON_COMMAND(ID_PAXFLOW_INSERT_START, OnPaxflowInsertStart)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_COPY_FLOW, OnBtnCopyFlow)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROCESSOR, OnSelchangedTreeProcessor)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_PROCESSOR, OnBegindragTreeProcessor)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY(TVN_SELCHANGED, IDC_PROCESS_TREE, OnSelchangedProcessTree)
	ON_NOTIFY(NM_SETFOCUS, IDC_PROCESS_TREE, OnSetfocusProcessTree)
	ON_NOTIFY(NM_SETFOCUS, IDC_TREE_PROCESSOR, OnSetfocusTreeProcessor)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_PROCESS_TREE, OnBegindragProcessTree)
	ON_WM_GETMINMAXINFO()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_PAXFLOW_EDIT_PIPE_AUTO, OnPaxflowEditPipeAuto)
	ON_WM_CLOSE()
	ON_COMMAND(ID_PAXFLOW_FLOWCONDITION_PERCENTSPLIT, OnPaxflowFlowconditionPercentsplit)
	ON_COMMAND(ID_PAXFLOW_FLOWCONDITION_EVENREMAINING, OnPaxflowFlowconditionEvenremaining)
	ON_COMMAND(ID_PAXFLOW_FLOWCONDITION_QUEJULENGTHXX, OnPaxflowFlowconditionQuejulengthxx)
	ON_COMMAND(ID_PAXFLOW_FLOWCONDITION_SKIPWHENTIMELESSTHAN,OnPaxflowFlowconditonSkipWhenTimeLessThan)
	ON_COMMAND(ID_PAXFLOW_FLOWCONDITION_WAITTIMEXXMINS, OnPaxflowFlowconditionWaittimexxmins)
	ON_COMMAND(ID_PAXFLOW_SYNCPOINT, OnPaxflowConditionSyncPoint)
	ON_COMMAND(ID_PAXFLOW_ROUTECONDITION_PIPES_ENABLEAUTOMATIC, OnPaxflowRouteconditionPipesEnableautomatic)
	ON_COMMAND(ID_PAXFLOW_ROUTECONDITION_PIPES_USERSELECT, OnPaxflowRouteconditionPipesUserselect)
	ON_COMMAND(ID_PAXFLOW_ROUTECONDITION_STRAIGHTLINE, OnPaxflowRouteconditionStraightline)
	ON_COMMAND(ID_PAXFLOW_ROUTECONDITION_AVOIDFIXEDQUEUES, OnPaxflowRouteconditionAvoidfixedqueues)
	ON_COMMAND(ID_PAXFLOW_ROUTECONDITION_AVOIDOVERFLOWQUEUES, OnPaxflowRouteconditionAvoidoverflowqueues)
	ON_COMMAND(ID_PAXFLOW_ROUTECONDITION_AVOIDDENSITY, OnPaxflowRouteconditionAvoiddensity)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PAXTYPE, OnClickListPaxtype)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PAXTYPE, OnItemchangedListPaxtype)
	ON_COMMAND(ID_PAXFLOW_FIND, OnPaxflowFind)
	ON_COMMAND(ID_PAXFLOW_1X1START, OnPaxflow1x1start)
	ON_COMMAND(ID_PAXFLOW_1X1END, OnPaxflow1x1end)
	ON_COMMAND(ID_PAXFLOW_FOLLOWSTATE, OnPaxflowFollowState)
	ON_COMMAND(ID_PAXFLOW_CHANNELCONDITION_11_ON, OnPaxflowChannelcondition11On)
	ON_COMMAND(ID_PAXFLOW_CHANNELCONDITION_11_OFF, OnPaxflowChannelcondition11Off)	
	ON_COMMAND(ID_PAXFLOW_CHANNELCONDITION_1X1_END, OnPaxflowChannelcondition1x1End)
	ON_COMMAND(ID_PAXFLOW_CHANNELCONDITION_1X1_START, OnPaxflowChannelcondition1x1Start)
	ON_COMMAND(ID_PAXFLOW_CHANNELCONDITION_1X1_CLEAR, OnPaxflowChannelcondition1x1Clear)

	ON_NOTIFY(NM_CLICK, IDC_TREE_FLOW, OnClickTreeProcess)
	ON_COMMAND(ID_PAXFLOW_COPYPROCESSOR,OnCopyProcessors)
	ON_COMMAND(ID_PAXFLOW_ADDCOPY,OnAddCopyProcessors)
	ON_COMMAND(ID_PAXFLOW_INSERTCOPY,OnInsertCopyProcessors)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_MESSAGE(UM_PCTREE_SELECT_PROC, OnPCTreeSelectProc)
	ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnToolbarDropDown)
	ON_COMMAND_RANGE(SYNCPOINT_STARTID, SYNCPOINT_STARTID + 10000, OnSelectSyncPoint)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaxFlowDlg message handlers

BOOL CPaxFlowDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_ToolBarFlowTree.GetToolBarCtrl().HideButton(ID_BUTTON33714);
	m_butAdvance.EnableWindow( FALSE );
	m_listRightWnd.Add(&m_staticFlightTitle); 
	m_listRightWnd.Add(&m_staticGateTitle) ;
	m_listRightWnd.Add(&m_listFlight) ;
	m_listRightWnd.Add(&m_treeGate) ;
	m_listRightWnd.Add(&m_butRestore) ;

	m_listLeftBtn.Add(&m_butAdvance);
	m_listLeftBtn.Add(&m_btnCancel);
	m_listLeftBtn.Add(&m_btnOk);
	m_listLeftBtn.Add(&m_btnSave);
	
	ResizeToHideRightWnd();	

	m_image.Create ( IDB_PROCESS,16,1,RGB(255,0,255) );
	CBitmap bm;
	bm.LoadBitmap(IDB_PROCESSES);
	m_image.Add(&bm,RGB(255,0,255));
	m_ProcTree.SetImageList ( &m_image,TVSIL_NORMAL );

	m_ProcessTree.SetImageList ( &m_image,TVSIL_NORMAL );
	DisableFlowTreeToolBarBtn();

	
	
	CenterWindow(NULL);
    m_listCtrlPaxType.SetHeadings("Index,20;Non passenger type,85;Passenger type,300;AirLine,60 \
		;Airport,60;Stopover Airport,100;AcType,60;AirLineGroup,180;Sector,180;Stopover Sector,180;Category,180");//;OtherSector,300");

	m_nLevelLen = 0 ;
	const int * nBranches = GetInputTerminal()->inTypeList->getBranches();
	while (nBranches[m_nLevelLen] != 0)
	{
		m_nLevelLen++;		
	}

	for(int i = m_nLevelLen; i > 0; i-- )
	{
		CString strTmp;
		strTmp.Format("TypeLevel%d",i);
		m_listCtrlPaxType.InsertColumn( 3, strTmp, LVCFMT_LEFT, 70 ); 
		m_listCtrlPaxType.AddColumnCount();
	}	
	m_listCtrlPaxType.InsertColumn(m_listCtrlPaxType.GetColumnCount(),"OtherSector",LVCFMT_LEFT,180);
	m_listCtrlPaxType.AddColumnCount();

	m_listCtrlPaxType.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPaxFlowDlg::InitFlow()
{
	m_OperatedPaxFlow.SetInputTerm( m_pInTerm	);
	m_GateFlow.SetInputTerm( m_pInTerm );
	if(!InitConvetor())
		return FALSE;
	m_iListSelect =-1;
	m_OperatedPaxFlow.SetChangedFlag( false );
	m_iChangedIdx = -1;
	m_OperatedPaxFlow.ClearAllPair();
	m_FlowTree.DeleteAllItems();

	m_allFlightFlow.ClearAll();
	m_iGateChangedIdx = -1;

	m_pNonPaxFlow = NULL;
	m_pPaxFlowAttachedToNonPaxFlow = NULL;
	
	EnableBtnFromTBar(m_ToolBarMEType,IDC_BTN_COPY_FLOW,FALSE);
	
	// Passenger type combo
	ReloadPaxList();
	m_btnSave.EnableWindow( false );
	InitProcessCtrls();
	
	return TRUE;
}
BOOL CPaxFlowDlg::InitConvetor()
{
	m_paxFlowConvertor.SetInputTerm( GetInputTerminal() );
	m_allPaxFlow.ClearAll();
	m_allPaxFlow.FromOldToDigraphStructure( m_paxFlowConvertor );

	return TRUE;
}
//-------------------------------------------------------------------------------------------------
// new 
//-------------------------------------------------------------------------------------------------

CString CPaxFlowDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}


InputTerminal* CPaxFlowDlg::GetInputTerminal()
{
	ASSERT( m_pInTerm );
	return m_pInTerm;
/*
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
*/
}



// recursive function to load tree, start with "START";
// return false for no entry

// Load the child:
// if _bStationDep, it will load only from the StationDB;
// else it will load from normal database.

// Load child of child.
// if child is Station and !_bStationDep will call child with 

void CPaxFlowDlg::LoadSubTree( HTREEITEM _hItem )
{
	if( !m_hInterestItem )
	{
		if( IsInterestNode( _hItem ) )
		{
			m_hInterestItem = _hItem;
		}
	}
	int nTypeIndex = m_OperatedPaxFlow.GetPassengerConstrain()->GetTypeIndex();
	TREEITEMDATA* pTempData = (TREEITEMDATA*)m_FlowTree.GetItemData( _hItem );
	// TRACE("%s\n", pTempData->m_procId.GetIDString() );
	CProcessorDestinationList* tempPair = m_OperatedPaxFlow.GetEqualFlowPairAt( pTempData->m_procId );
	if( tempPair )
	{
		int iDestCount = tempPair->GetDestCount();
		for(int i=0; i<iDestCount; ++i )
		{
			if( !IfHaveCircle( _hItem, tempPair->GetDestProcAt( i ).GetProcID() ) 
				|| IsStation( tempPair->GetDestProcAt( i ).GetProcID() )
				|| IsStation( pTempData->m_procId ) )
			{
				if( IsStation( pTempData->m_procId ) )// it is a station processor
				{
					HTREEITEM hFather = m_FlowTree.GetParentItem( _hItem );
					if( hFather )
					{
						TREEITEMDATA* pFatherData = (TREEITEMDATA*)m_FlowTree.GetItemData( hFather );
						// TRACE( "father = %s\n", pFatherData->m_procId.GetIDString() );
						if( IsStation( pFatherData->m_procId ) ) // his father is a staion ,too.
						{
							const ProcessorID& pDestID = tempPair->GetDestProcAt( i ).GetProcID();
							if( IsStation( pDestID ) )
							{
								continue;
							}
							else
							{
								TREEITEMDATA* pTempData = new TREEITEMDATA;
								pTempData->m_iSpecialHandleType = 0;
								pTempData->m_procId = pDestID;
								pTempData->m_arcInfo = tempPair->GetDestProcArcInfo( i );
								m_vItemData.push_back( pTempData );
								CString sPercent;
								sPercent.Format("( %d%% )", pTempData->m_arcInfo->GetProbality() );								
								HTREEITEM hItem = m_FlowTree.InsertItem( pDestID.GetIDString() + GetArcInfoDescription( pTempData->m_arcInfo)  , _hItem );
								//color item 		
								if( pTempData->m_arcInfo != NULL )
								{
									//	if( nTypeIndex >0 && !pTempData->m_arcInfo->GetInheritedFlag() )
									if( nTypeIndex >0 && pTempData->m_arcInfo->GetTypeOfOwnership() == 0 )
									{	
										m_FlowTree.SetItemColor( hItem );																			
										m_FlowTree.SetItemBold( hItem, true );
									}
									else
									{
										if( IsProcess( pTempData->m_procId ) )
										{
											m_FlowTree.SetItemColor( hItem, PROCESS_COLOR );
											m_FlowTree.SetItemBold( hItem, true );
										}
										else
										{
											m_FlowTree.SetItemColor( hItem, m_treeTextDefColor );
										}
									}
								}
									
								//CString sDisplay ;
								//sDisplay.Format("( %d%% )", pTempData->m_arcInfo->GetProbality() ); 
								////HTREEITEM hItem = m_FlowTree.InsertItem( pDestID.GetIDString() + GetPipeString( pTempData->m_arcInfo ) + sPercent , _hItem );
								//HTREEITEM hItem = m_FlowTree.InsertItem( pDestID.GetIDString() + sDisplay , _hItem );
								m_FlowTree.SetItemData( hItem, (DWORD)pTempData );
								//tempPair->GetDestProcArcInfo( i )->SetVisitFlag( true );
								LoadSubTree( hItem );
								m_FlowTree.Expand( hItem,TVE_EXPAND   );
							}
						}
						else // his father is not a station
						{
							const ProcessorID& pDestID = tempPair->GetDestProcAt( i ).GetProcID();
							if( !IsStation( pDestID ) )
							{
								continue;
							}
							else
							{
								TREEITEMDATA* pTempData = new TREEITEMDATA;
								pTempData->m_iSpecialHandleType = 0;
								pTempData->m_procId = pDestID;
								pTempData->m_arcInfo = tempPair->GetDestProcArcInfo( i );
								m_vItemData.push_back( pTempData );
		
								CString sPercent;
								sPercent.Format("( %d%% )", pTempData->m_arcInfo->GetProbality() );								
								HTREEITEM hItem = m_FlowTree.InsertItem( pDestID.GetIDString() + GetArcInfoDescription( pTempData->m_arcInfo ) , _hItem );
								//color item 		
								if( pTempData->m_arcInfo != NULL )
								{
									//if( nTypeIndex >0 && !pTempData->m_arcInfo->GetInheritedFlag() )
									if( nTypeIndex >0 && pTempData->m_arcInfo->GetTypeOfOwnership() == 0 )
									{
										m_FlowTree.SetItemColor( hItem );										
										m_FlowTree.SetItemBold( hItem, true );
									}
									else
									{
										if( IsProcess( pTempData->m_procId ) )
										{
											m_FlowTree.SetItemColor( hItem, PROCESS_COLOR );
											m_FlowTree.SetItemBold( hItem, true );
										}
										else
										{
											m_FlowTree.SetItemColor( hItem, m_treeTextDefColor );
										}
									}
								}
								m_FlowTree.SetItemData( hItem, (DWORD)pTempData );
								//tempPair->GetDestProcArcInfo( i )->SetVisitFlag( true );
								LoadSubTree( hItem );
								m_FlowTree.Expand( hItem,TVE_EXPAND   );

							}
							
						}
						
					}
					else // he is a station, bu he has no father
					{
						const ProcessorID& pDestID = tempPair->GetDestProcAt( i ).GetProcID();
						if( !IsStation( pDestID ) )
						{
							continue;
						}
						else
						{
							TREEITEMDATA* pTempData = new TREEITEMDATA;
							pTempData->m_iSpecialHandleType = 0;
							pTempData->m_procId = pDestID;
							pTempData->m_arcInfo = tempPair->GetDestProcArcInfo( i );
							m_vItemData.push_back( pTempData );

							CString sPercent;
							sPercent.Format("( %d%% )", pTempData->m_arcInfo->GetProbality() );								
							HTREEITEM hItem = m_FlowTree.InsertItem( pDestID.GetIDString() + GetArcInfoDescription( pTempData->m_arcInfo ) , _hItem );						
							//color item 		
							if( pTempData->m_arcInfo != NULL )
							{
								//if( nTypeIndex >0 && !pTempData->m_arcInfo->GetInheritedFlag() )
								if( nTypeIndex >0 && pTempData->m_arcInfo->GetTypeOfOwnership() == 0 )
								{
									m_FlowTree.SetItemColor( hItem );									
									m_FlowTree.SetItemBold( hItem, true );
								}
								else
								{
									if( IsProcess( pTempData->m_procId ) )
									{
										m_FlowTree.SetItemColor( hItem, PROCESS_COLOR );
										m_FlowTree.SetItemBold( hItem, true );
									}
									else
									{
										m_FlowTree.SetItemColor( hItem, m_treeTextDefColor );
									}
								}
							}
							m_FlowTree.SetItemData( hItem, (DWORD)pTempData );
							//tempPair->GetDestProcArcInfo( i )->SetVisitFlag( true );
							LoadSubTree( hItem );
							m_FlowTree.Expand( hItem,TVE_EXPAND   );

						}
					}
				}
				else // he is not a station
				{
					TREEITEMDATA* pTempData = new TREEITEMDATA;
					pTempData->m_iSpecialHandleType = 0;
					pTempData->m_procId = tempPair->GetDestProcAt( i ).GetProcID();
					pTempData->m_arcInfo = tempPair->GetDestProcArcInfo( i );
					m_vItemData.push_back( pTempData );
					CString sPercent;
					sPercent.Format("( %d%% )", pTempData->m_arcInfo->GetProbality() );	
					HTREEITEM hItem;
					// show percent depend on AppointedPercentFlag
// 					if ( pTempData->m_arcInfo->GetAppointedPercentFlag())
					{
						 hItem = m_FlowTree.InsertItem( pTempData->m_procId.GetIDString() +GetArcInfoDescription( pTempData->m_arcInfo )  , _hItem );	
					}
//					else
//					{
//						 CString str = GetArcInfoDescription( pTempData->m_arcInfo );
//						 int nPos1 = str.Find('(',0);   // trip string "( percent% )" informat when no percent.
//						 if (nPos1 >= 0&&str.GetLength()>0) 
//						 {
//							 int nPos2 = str.Find(')',nPos1);
//							 if (nPos2 >= nPos1) 
//							 {
//								 
//								 if ( (nPos2 - nPos1) == str.GetLength()-1 )  //only have percent info in this string
//								 {
//									 str ="";							 
//								 }
//								 else
//								     str.Delete(nPos1,nPos2-nPos1+1);
//							 }
//
//						 }
//						 hItem = m_FlowTree.InsertItem( pTempData->m_procId.GetIDString()+str, _hItem );						
//					}
					
					//color item 		
					if( pTempData->m_arcInfo != NULL )
					{
						//if(nTypeIndex >0 && !pTempData->m_arcInfo->GetInheritedFlag() )
						if( nTypeIndex >0 && pTempData->m_arcInfo->GetTypeOfOwnership() == 0 )
						{
							m_FlowTree.SetItemColor( hItem );							
							m_FlowTree.SetItemBold( hItem, true );
						}
						else
						{
							if( IsProcess( pTempData->m_procId ) )
							{
								m_FlowTree.SetItemColor( hItem, PROCESS_COLOR );
								m_FlowTree.SetItemBold( hItem, true );
							}
							else
							{
								m_FlowTree.SetItemColor( hItem, m_treeTextDefColor );								
							}
							
//							if ( pTempData->m_arcInfo->GetAppointedPercentFlag())
//							{
//								m_FlowTree.SetItemColor(hItem,m_treeTextDefColor);
//							}
//							else
//							{
//								m_FlowTree.SetItemColor(hItem,PROCESS_COLOR_GRAY);
//							}
							
						}
					}
					////////////////////////////////////////////////////
					m_FlowTree.SetItemData( hItem, (DWORD)pTempData );
					//tempPair->GetDestProcArcInfo( i )->SetVisitFlag( true );
					LoadSubTree( hItem );
					m_FlowTree.Expand( hItem,TVE_EXPAND   );				
				}
			}
			else // have a circle
			{
				TREEITEMDATA* pTempData = new TREEITEMDATA;
				pTempData->m_iSpecialHandleType = 1;
				pTempData->m_procId = tempPair->GetDestProcAt( i ).GetProcID();
				pTempData->m_arcInfo = tempPair->GetDestProcArcInfo( i );
				m_vItemData.push_back( pTempData );

				CString sPercent;
				sPercent.Format("( %d%% )", pTempData->m_arcInfo->GetProbality() );								
				HTREEITEM hItem = m_FlowTree.InsertItem( pTempData->m_procId.GetIDString() +GetArcInfoDescription( pTempData->m_arcInfo )  , _hItem );						
				//color item 		
				if( pTempData->m_arcInfo != NULL )
				{
					//if( nTypeIndex >0 && !pTempData->m_arcInfo->GetInheritedFlag() )
					if( nTypeIndex >0 && pTempData->m_arcInfo->GetTypeOfOwnership() == 0 )
					{
						m_FlowTree.SetItemColor( hItem );						
						m_FlowTree.SetItemBold( hItem, true );
					}
					else
					{
						if( IsProcess( pTempData->m_procId ) )
						{
							m_FlowTree.SetItemColor( hItem,PROCESS_COLOR );
							m_FlowTree.SetItemBold( hItem, true );
						}
						else
						{
							m_FlowTree.SetItemColor( hItem, m_treeTextDefColor );
						}
					}
				}
				////////////////////////////////////////////////////
				m_FlowTree.SetItemData( hItem, (DWORD)pTempData );

				
				m_FlowTree.SetItemData (m_FlowTree.InsertItem(CS_CYCLIC_LABEL, hItem ), NULL );
				m_FlowTree.Expand( hItem,TVE_EXPAND   );
				m_FlowTree.Expand( _hItem,TVE_EXPAND   );
			}
		}
		m_FlowTree.Expand( _hItem,TVE_EXPAND   );
	}
}
bool CPaxFlowDlg::IfHaveCircle( HTREEITEM _testItem,const ProcessorID _procID ) const
{
	if( !_testItem )
		return false;
	
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( _testItem );
	// TRACE("\n%s     %s\n", pData->m_procId.GetIDString(), _procID.GetIDString());
	if ( pData->m_procId == _procID )
	{
		return true;
	}
	//Check if Processor Exist In SubFlow
	//Check if Processor Exist In SubFlowA & SubFlowB Not Added yet
	//if(IsProcess(pData->m_procId) && IsProcess(_procID))
	//

	if(IsProcess(pData->m_procId))
	{
		CString sID = pData->m_procId.GetIDString();
		sID.MakeUpper();
		CSubFlow *pSubFlow = m_pInTerm->m_pSubFlowList->GetProcessUnit(sID);
		if(pSubFlow)
		{
			CSinglePaxTypeFlow* pSinglePaxTypeFlow = pSubFlow->GetInternalFlow();
			if(pSinglePaxTypeFlow!=NULL && pSinglePaxTypeFlow->IfFits(_procID))//IfExist(_procID))
				return true;
		}
	}

	HTREEITEM hFather = m_FlowTree.GetParentItem( _testItem );
	if( !hFather )
	{
		return false;
	}

	return IfHaveCircle( hFather, _procID );
	
	
}
void CPaxFlowDlg::ReloadTree()
{
	CWaitCursor m_waitCur;
	m_FlowTree.ShowWindow( SW_HIDE );

	BuildCollapsedNodeVector();

	int iItemDataCount = m_vItemData.size();
	for( int i=0; i<iItemDataCount; ++i )
	{
		delete m_vItemData[i];
	}

	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );

	m_hInterestItem = NULL;
	m_vItemData.clear();
	m_FlowTree.DeleteAllItems();
	std::vector<ProcessorID> vAllRootProc;
	//m_OperatedPaxFlow.PrintAllStructure();
	m_OperatedPaxFlow.SetAllPairVisitFlag( false );
	m_OperatedPaxFlow.GetRootProc( vAllRootProc );

	const CMobileElemConstraint* pConstraint = m_OperatedPaxFlow.GetPassengerConstrain();
	if( pConstraint->GetTypeIndex() > 0 )// visitor
	{
		m_treeTextDefColor =VISITOR_COLOR ;
	}
	else
	{
		if( pConstraint->isTurround() )
		{
			m_treeTextDefColor =TRANSFER_TRANSIT_PAX_COLOR ;
		}
		else
		{
			m_treeTextDefColor = ARRIVE_DEPATURE_PAX_COLOR ;
		}
	}
	//m_hRoot = m_FlowTree.InsertItem( "--Passenger Flow--" );
	int iRootCount = vAllRootProc.size();
	if( 0 == iRootCount )
	{
		std::vector<ProcessorID> vAllInvolvedProc;
		m_OperatedPaxFlow.GetAllInvolvedProc( vAllInvolvedProc );
		if( vAllInvolvedProc.size() > 0 )// just have once cicle path, so it will be no root processor
		{
			HTREEITEM hItem = m_FlowTree.InsertItem( vAllInvolvedProc[ 0 ].GetIDString() );
			//need not color item
			TREEITEMDATA* pTempData = new TREEITEMDATA;
			pTempData->m_arcInfo=NULL;
			pTempData->m_procId = vAllInvolvedProc[ 0 ];
			m_vItemData.push_back( pTempData );
			m_FlowTree.SetItemData( hItem, (DWORD)pTempData );
			m_FlowTree.SetItemColor( hItem, m_treeTextDefColor );
			LoadSubTree( hItem );
		}
	}
	else
	{
		for(int i=0; i < iRootCount; ++i )
			{
				HTREEITEM hItem = m_FlowTree.InsertItem( vAllRootProc[ i ].GetIDString() );
//				CString strTemp = vAllRootProc[ i ].GetIDString();
				m_FlowTree.SetItemColor( hItem, m_treeTextDefColor );
				//check "START" processor
				if( vAllRootProc[i] == *pProcStart->getID() )
				{
					const CSinglePaxTypeFlow* pFlow =  m_allPaxFlow.GetSinglePaxTypeFlowAt( m_iListSelect )->GetParentPaxFlow();
					if( pFlow == NULL )
					{
						if( m_allPaxFlow.GetSinglePaxTypeFlowAt( m_iListSelect )->GetPassengerConstrain()->GetTypeIndex() > 0 )
						{
							if( IfStartProcIsOwned() )
							{
								m_FlowTree.SetItemColor( hItem );							
								m_FlowTree.SetItemBold( hItem, true );
							}
								
						}
						/*
						else
						{
							m_FlowTree.SetItemColor( hItem );
							m_FlowTree.SetItemBold( hItem, true );
						}
						*/
						
					}
				}
				
				//need not color item
				TREEITEMDATA* pTempData = new TREEITEMDATA;
				
//              pTempData->m_arcInfo=NULL;
				pTempData->m_procId = vAllRootProc[ i ];
				CString strTemp = vAllRootProc[ i ].GetIDString();				
				CProcessorDestinationList* tempPair = m_OperatedPaxFlow.GetEqualFlowPairAt( pTempData->m_procId );
				int n = tempPair->GetDestCount();
				if (n > 0)
				{
					pTempData->m_arcInfo = tempPair->GetDestProcArcInfo( 0 );
					pTempData->m_arcInfo->SetOneToOneFlag(FALSE); 				
				}
				else
				{
					pTempData->m_arcInfo=NULL;
				}
				m_vItemData.push_back( pTempData );
				m_FlowTree.SetItemData( hItem, (DWORD)pTempData );
				LoadSubTree( hItem );		
			}
	}

	CollapsedTree();
	if( m_hInterestItem )
	{
		m_FlowTree.SelectItem( m_hInterestItem );
		if( !m_FlowTree.SelectSetFirstVisible( m_hInterestItem ) )
		{
			m_FlowTree.EnsureVisible( m_hInterestItem );
		}
		
	}

	
	Update3DViewData();

	 //check if have @FROM_GATE ,@TO_GATE
	m_bHaveFromGate = FALSE;
	m_bHaveToGate	= FALSE;
	const Processor* pFromGate = GetInputTerminal()->procList->getProcessor( FROM_GATE_PROCESS_INDEX );
	const Processor* pToGate = GetInputTerminal()->procList->getProcessor( TO_GATE_PROCESS_INDEX );
	if( m_OperatedPaxFlow.IfExist( *(pFromGate->getID()) ) )
		m_bHaveFromGate = TRUE;
	if( m_OperatedPaxFlow.IfExist( *(pToGate->getID())) )
		m_bHaveToGate = TRUE;

	m_FlowTree.ShowWindow( SW_SHOW );
	m_butAdvance.EnableWindow( m_bHaveFromGate || m_bHaveToGate );
	ExpandWindow( FALSE );
	AdjustDragedProcess();
	if(m_FlowTree.GetSelectedItem())
		UpdateFlowTreeToolBarBtn(TVHT_ONITEMLABEL,m_FlowTree.GetSelectedItem());
	else
		DisableFlowTreeToolBarBtn();
}


bool CPaxFlowDlg::IfStartProcIsOwned(  )
{
	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
	if( m_pNonPaxFlow )
	{
		if( m_pNonPaxFlow->IfExist( *pProcStart->getID() ) )
			return true;
	}

	if( m_pPaxFlowAttachedToNonPaxFlow )
	{
		if( !m_pPaxFlowAttachedToNonPaxFlow->IfExist( *pProcStart->getID() ) )
		{
			return true;	
		}
	}

	return false;

}
void CPaxFlowDlg::ReloadPaxList()
{
	m_listCtrlPaxType.DeleteAllItems();

	int iSinglePaxFlowCount = m_allPaxFlow.GetSinglePaxTypeFlowCount();
	//char sPaxFlowName[256];
	CString sPaxFlowName;
	CString strNoPassengerType;
	CString strPassengerType;
	
	for(int i=0; i<iSinglePaxFlowCount; ++i )
	{
		CString strNum;
		CString strTemp;
		CString strAirAline("");
		CString strAirport("");
		CString strStopoverAirport("");
		CString strACType("");
		CString strAirLineGroup("");
		CString strFlightGroup("");
		CString strSector("");
		CString strStopoverSector("");
		CString strCategory("");
		CString strFlight("");
		CString strOtherSector("");
		CString strOtherStopoverSector("");
		char chTemp[256];
		
		strNum.Format("%d",i+1);
 		int nIdx ;
		const CMobileElemConstraint* pMobileElemConstraint=m_allPaxFlow.GetSinglePaxTypeFlowAt( i )->GetPassengerConstrain();
		
		//m_allPaxFlow.GetSinglePaxTypeFlowAt( i )->GetPassengerConstrain()
		pMobileElemConstraint->screenPrint( sPaxFlowName,0 , 255 );
		//pMobileElemConstraint->screenPrint(sPaxFlowName1,0,255);
		//strPassengerType.Format("%s",sPaxFlowName.GetBuffer(0));	
        strPassengerType.Format("%s",sPaxFlowName);
		pMobileElemConstraint->getAirline( chTemp );
		strAirAline = chTemp;
		pMobileElemConstraint->getAirport( chTemp );
		strAirport = chTemp;
		pMobileElemConstraint->getStopoverAirport( chTemp );
		strStopoverAirport = chTemp;
		pMobileElemConstraint->getACType(chTemp);
		strACType = chTemp;
		pMobileElemConstraint->getAirlineGroup(chTemp);
		strAirLineGroup = chTemp;
		pMobileElemConstraint->getFlightGroup(chTemp);
		strFlightGroup = chTemp;
		pMobileElemConstraint->getSector(chTemp);
		strSector = chTemp ;
		pMobileElemConstraint->getStopoverSector(chTemp);
		strStopoverSector = chTemp ;
		pMobileElemConstraint->getCategory(chTemp);
		strCategory = chTemp ;				
        if(pMobileElemConstraint->getIntrinsic()==4)
		{
			pMobileElemConstraint->getOtherFlightConstraint().getSector(chTemp);
			strOtherSector = chTemp;
			pMobileElemConstraint->getOtherFlightConstraint().getStopoverSector(chTemp);
			strOtherStopoverSector = chTemp;
		}
		int nTypeIndex = pMobileElemConstraint->GetTypeIndex();
		if (nTypeIndex > 0)
		{
			CString strTempPassengerType = strPassengerType;
			int nStart=strPassengerType.Find('(',0);
			if (nStart != -1)
			{
				strNoPassengerType = strPassengerType.Left(nStart);
			}
			else
			{
				strNoPassengerType = strTempPassengerType;	
			}
			strPassengerType = strPassengerType.Right(strPassengerType.GetLength()-nStart);

			CStringArray strTypeLevelArarry;
			strTypeLevelArarry.Add(strNum);
			strTypeLevelArarry.Add(strNoPassengerType);	
			if (strPassengerType.Find('(',1) != -1)
			{
				nStart = strPassengerType.Find(')',0);
				if ( nStart > 0 )
				{
					strPassengerType = strPassengerType.Left( nStart );
					strPassengerType = strPassengerType.Right(strPassengerType.GetLength()-1);
				}			

				for( int n = 0; n < m_nLevelLen + 1; n++)
				{
					CString str("");
					int nPos = strPassengerType.Find('-',0);
					if(nPos == -1)
					{
						str = strPassengerType;
						strPassengerType = "";
					}
					else
						str = strPassengerType.Left(nPos);
					strTypeLevelArarry.Add(str);
					int nLen = strPassengerType.GetLength() ;
					strPassengerType = strPassengerType.Right(nLen - nPos - 1);
				}	
			}
			else
			{
				int nArrivalPos = -1;
				int nDeparturePos = -1;
				int nArrivingPos = -1;
				int nDepartingPos = -1;
				nArrivalPos = strTempPassengerType.Find("ARRIVAL");
				nDeparturePos = strTempPassengerType.Find("DEPARTURE");
				nArrivingPos = strTempPassengerType.Find("ARRIVING");
				nDepartingPos = strTempPassengerType.Find("DEPARTING");
			
				if (nArrivalPos != -1)
				{
					strPassengerType = "ARRIVAL";
				}
				else if (nDeparturePos != -1)
				{
					strPassengerType = "DEPARTURE";
				}
				else if (nArrivingPos != -1)
				{
					strPassengerType = "ARRIVING";
				}
				else if (nDepartingPos != -1)
				{
					strPassengerType = "DEPARTING";
				}
				else
				{
					strPassengerType = "";
				}
				
				strTypeLevelArarry.Add(strPassengerType);

				for (int n = 0; n < m_nLevelLen; n++)
				{
					strTypeLevelArarry.Add("");
				}
			}

			strTypeLevelArarry.Add(strAirAline);			
			strTypeLevelArarry.Add(strAirport);	
			strTypeLevelArarry.Add(strStopoverAirport);	
			strTypeLevelArarry.Add(strACType);	
			if (strAirLineGroup.IsEmpty())
			{
				strTypeLevelArarry.Add(strFlightGroup);
			}
			else
			{
				strTypeLevelArarry.Add(strAirLineGroup);	
			}
			strTypeLevelArarry.Add(strSector);	
			strTypeLevelArarry.Add(strStopoverSector);	
			strTypeLevelArarry.Add(strCategory);	
			strTypeLevelArarry.Add(strOtherSector);
			strTypeLevelArarry.Add(strOtherStopoverSector);
			nIdx = m_listCtrlPaxType.AddItem(i,strTypeLevelArarry);
		}
		else
		{
			CString strTempPassengerType = strPassengerType;
			int nStart=strPassengerType.Find('(',0);
			strPassengerType = strPassengerType.Right(strPassengerType.GetLength()-nStart);

			CStringArray strTypeLevelArarry;

			strTypeLevelArarry.Add(strNum);
			strTypeLevelArarry.Add("");	
			
			if (strPassengerType.Find('(',1) != -1)
			{
				nStart = strPassengerType.Find(')',0);
				if ( nStart > 0 )
				{
					strPassengerType = strPassengerType.Left( nStart );
					strPassengerType = strPassengerType.Right(strPassengerType.GetLength()-1);
				}

				for( int n = 0; n < m_nLevelLen + 1; n++)
				{
					CString str("");
					int nPos = strPassengerType.Find('-',0);
					if(nPos == -1)
					{					
						str = strPassengerType;
						strPassengerType = "";
					}
					else
						str = strPassengerType.Left(nPos);
					strTypeLevelArarry.Add(str);
					int nLen = strPassengerType.GetLength() ;
					strPassengerType = strPassengerType.Right(nLen - nPos - 1);
				}	
			}
			else
			{
				int nArrivalPos = -1;
				int nDeparturePos = -1;
				int nArrivingPos = -1;
				int nDepartingPos = -1;
				nArrivalPos = strTempPassengerType.Find("ARRIVAL");
				nDeparturePos = strTempPassengerType.Find("DEPARTURE");
				nArrivingPos = strTempPassengerType.Find("ARRIVING");
				nDepartingPos = strTempPassengerType.Find("DEPARTING");
				if(nArrivalPos == -1 && nDeparturePos == -1 && nArrivingPos == -1 && nDepartingPos == -1)
					strPassengerType = "PASSENGER";
				else
				{
					if (nArrivalPos != -1)
					{
						strPassengerType = "ARRIVAL";
					}
					else if (nDeparturePos != -1)
					{
						strPassengerType = "DEPARTURE";
					}
					else if (nArrivingPos != -1)
					{
						strPassengerType = "ARRIVING";
					}
					else if (nDepartingPos != -1)
					{
						strPassengerType = "DEPARTING";
					}
				}
				strTypeLevelArarry.Add(strPassengerType);

				for (int n = 0; n < m_nLevelLen; n++)
				{
					strTypeLevelArarry.Add("");
				}
			}
			
		

			strTypeLevelArarry.Add(strAirAline);			
			strTypeLevelArarry.Add(strAirport);	
			strTypeLevelArarry.Add(strStopoverAirport);	
			strTypeLevelArarry.Add(strACType);	
			if (strAirLineGroup.IsEmpty())
			{
				strTypeLevelArarry.Add(strFlightGroup);
			}
			else
			{
				strTypeLevelArarry.Add(strAirLineGroup);	
			}	
			strTypeLevelArarry.Add(strSector);	
			strTypeLevelArarry.Add(strStopoverSector);	
			strTypeLevelArarry.Add(strCategory);			
			strTypeLevelArarry.Add(strOtherSector);
			strTypeLevelArarry.Add(strOtherStopoverSector);
			nIdx = m_listCtrlPaxType.AddItem(i,strTypeLevelArarry);
		}	
	}

	for(int i=0; i<iSinglePaxFlowCount; ++i )
	{
		char szNoPassengerTypeFirst[256];
		char szNoPassengerTypeAgain[256];
		
		m_listCtrlPaxType.GetItemText(i,1,szNoPassengerTypeFirst,256);
		if(strcmp(szNoPassengerTypeFirst,"") != 0)
		{
			CStringArray strNoPaxArray;		
			int nColCount = m_listCtrlPaxType.GetColumnCount();

			for(int n = 0; n < nColCount; n++)
			{
				m_listCtrlPaxType.SetItemColor(i,n,RGB( 0,0,0),COLER253[i%253]);
			}
			
			for(int n = 2; n < nColCount; n++)  //get noPaxType
			{
				strNoPaxArray.Add(m_listCtrlPaxType.GetItemText(i,n));
			}				
				
			for(int ii=0; ii<iSinglePaxFlowCount; ++ii )
			{
				m_listCtrlPaxType.GetItemText(ii,1,szNoPassengerTypeAgain,256);
				if (strcmp(szNoPassengerTypeAgain,"") == 0)
				{
//					m_listCtrlPaxType.GetItemText(ii,2,szPassengerTypeAgain,256);
					bool bEqual = TRUE ;
					for(int n = 2; n < nColCount; n++)  // get PaxType and compare with noPaxType
					{
						 if (strNoPaxArray[n-2] != m_listCtrlPaxType.GetItemText(ii,n))
						 {
							 bEqual = FALSE ;
							 break;
						 }					
					}	
					if (bEqual)
					{
						for(int n = 0; n < nColCount; n++)
						{
							m_listCtrlPaxType.SetItemColor(ii,n,RGB( 0,0,0),COLER253[i%253]);
						}			
					}
				}
								
			}				
		}	
	}		
	/*
	ReloadPaxList( false );
	ReloadPaxList( true );
	*/
	//m_listCtrlPaxType.SetCurSel( m_iListSelect );
	//ReloadTree();
	
//	m_listCtrlPaxType.SetTopIndex(m_listCtrlPaxType.GetCurSel());
	EnableBtnFromTBar(m_ToolBarMEType,IDC_BTN_DELETE,FALSE);
	if( iSinglePaxFlowCount <= 0 )
	{
		ExpandWindow( FALSE );
		m_butAdvance.EnableWindow( FALSE );
	}
	m_listCtrlPaxType.SetItemState( m_iListSelect, LVIS_SELECTED, LVIS_SELECTED);	
	m_listCtrlPaxType.Sort();
}

CSinglePaxTypeFlow* CPaxFlowDlg::GetNonPaxFlow(CSinglePaxTypeFlow* pParentFlow)
{
	//parent flow should be valid
	if (pParentFlow == NULL)
		return NULL;
	
	//parent flow should be passenger flow
	if (pParentFlow->GetPassengerConstrain()->GetTypeIndex() != 0)
		return NULL;
	
	//find child non passenger flow
	for (int i = 0; i < m_allPaxFlow.GetSinglePaxTypeFlowCount(); i++)
	{
		CSinglePaxTypeFlow* pSingleFlow = m_allPaxFlow.GetSinglePaxTypeFlowAt(i);
		if (pSingleFlow->GetPassengerConstrain()->GetTypeIndex() == 0)
			continue;
		
		if (pSingleFlow->GetParentPaxFlow() == pParentFlow)
		{
			return pSingleFlow;
		}
	}

	return NULL;
}

void CPaxFlowDlg::BuildHierarchyNonPaxFlow(CSinglePaxTypeFlow* pParentFlow)
{
	CSinglePaxTypeFlow* pNonPaxFlow = GetNonPaxFlow(pParentFlow);
	if (pNonPaxFlow == NULL)
		return;

	const CMobileElemConstraint* pConstrint = pNonPaxFlow->GetPassengerConstrain();
	CMobileElemConstraint attachedPaxConstraint( *pConstrint);
	CString strPconst;
	pConstrint->screenPrint( strPconst );
	attachedPaxConstraint.SetTypeIndex( 0 );
	attachedPaxConstraint.SetMobileElementType(enum_MobileElementType_PAX);
	attachedPaxConstraint.screenPrint( strPconst,0,256 );

	//pax flow
	CSinglePaxTypeFlow* pPaxFlowAttachedToNonPaxFlow = new CSinglePaxTypeFlow ( GetInputTerminal() );

	pPaxFlowAttachedToNonPaxFlow->SetPaxConstrain( &attachedPaxConstraint );
	pPaxFlowAttachedToNonPaxFlow->ClearAllPair();


	pPaxFlowAttachedToNonPaxFlow->GetPassengerConstrain()->screenPrint( strPconst );
	int iIdx = m_allPaxFlow.GetIdxIfExist( *pParentFlow );
	if( iIdx >= 0 )//exist
	{   
		m_allPaxFlow.BuildHierarchyFlow( m_paxFlowConvertor, iIdx , *pPaxFlowAttachedToNonPaxFlow );
	}

	//non pax flow
	CSinglePaxTypeFlow* pNewNonPaxFlow = new CSinglePaxTypeFlow ( GetInputTerminal() );
	
	pNewNonPaxFlow->SetPaxConstrain( pConstrint );
	iIdx = m_allPaxFlow.GetIdxIfExist( *pNewNonPaxFlow );
	ASSERT( iIdx >= 0 );

	if(m_allPaxFlow.GetSinglePaxTypeFlowAt(iIdx)->GetFlowPairCount() <= 0)
	{
		m_allPaxFlow.BuildHierarchyFlow( m_paxFlowConvertor, iIdx , *pNewNonPaxFlow );
	}
	else
	{   //get nonPaxFlow
		int nFlowPairCount = pNonPaxFlow->GetFlowPairCount();
		bool bHaveHead = FALSE;
		bool bHaveTail = FALSE;
		for (int n = 0 ; n < nFlowPairCount;n ++)
		{
			CProcessorDestinationList* tempFlowPair = pNonPaxFlow->GetFlowPairAt(n);
			pNewNonPaxFlow->AddFowPair(*tempFlowPair);
			ProcessorID startProcID = tempFlowPair->GetProcID();
			ProcessorID endProcID  ;

			if (!bHaveHead) 
			{
				bHaveHead = pPaxFlowAttachedToNonPaxFlow->IfExist(startProcID);
			}
			if (!bHaveTail) 
			{
				for (int i =0; i < tempFlowPair->GetDestCount(); i++)
				{
					endProcID = tempFlowPair->GetDestProcAt(0).GetProcID();
					bHaveTail = pPaxFlowAttachedToNonPaxFlow->IfExist(endProcID);
				}					
			}
		}		

		if(!bHaveHead||!bHaveTail)				
		{
			m_allPaxFlow.BuildHierarchyFlow( m_paxFlowConvertor, iIdx , *pNewNonPaxFlow );
		}
	}

	CSinglePaxTypeFlow resultFlow;
	m_paxFlowConvertor.BuildAttachedNonPaxFlow( *pPaxFlowAttachedToNonPaxFlow, *pNewNonPaxFlow, resultFlow );
	//store new NonPaxFlow to m_allPaxFlow
	if (pNewNonPaxFlow->IfFlowBeChanged()) 
	{			
		CSinglePaxTypeFlow* pNewFlow = new CSinglePaxTypeFlow( GetInputTerminal() );
		pNewFlow->SetPaxConstrain( pConstrint );

		int i = pNewNonPaxFlow->GetFlowPairCount();		
		for (int n = 0 ; n < i;n ++)
		{
			CProcessorDestinationList* tempFlowPair = pNewNonPaxFlow->GetFlowPairAt(n);
			pNewFlow->AddFowPair( *tempFlowPair );
		}	
		pNewFlow->SetChangedFlag(true);
		m_allPaxFlow.ReplaceSinglePasFlowAt( iIdx, pNewFlow );				
		m_allPaxFlow.InitHierachy();
	}

	delete pPaxFlowAttachedToNonPaxFlow;
	delete pNewNonPaxFlow;
}

void CPaxFlowDlg::OnSelchangeListPaxtype() 
{	
	if( m_bChanged && m_iChangedIdx >= 0 && m_OperatedPaxFlow.IfFlowBeChanged() )
	{
		m_bChanged = false;
		CSinglePaxTypeFlow* pNewFlow = new CSinglePaxTypeFlow( GetInputTerminal() );
		pNewFlow->SetPaxConstrain( m_OperatedPaxFlow.GetPassengerConstrain() );
		pNewFlow->SetParentPaxFlow( m_OperatedPaxFlow.GetParentPaxFlow() ); 
		
		if( !m_pNonPaxFlow )
		{
			m_allPaxFlow.BuildSinglePaxFlow( m_paxFlowConvertor, m_iChangedIdx, m_OperatedPaxFlow, *pNewFlow );
		}
		else
		{   // BuildNonPaxFlowToFile can create NonPaxFlow , attention BuildHierarchyFlow who can also create NonPaxFlow
			m_paxFlowConvertor.BuildNonPaxFlowToFile( *m_pPaxFlowAttachedToNonPaxFlow, *m_pNonPaxFlow, m_OperatedPaxFlow, *pNewFlow );
			delete m_pPaxFlowAttachedToNonPaxFlow;
			m_pPaxFlowAttachedToNonPaxFlow = NULL;
			
			delete m_pNonPaxFlow;
			m_pNonPaxFlow = NULL;			
		}
		pNewFlow->SetChangedFlag( true );
//		pNewFlow->PrintAllStructure();
		m_allPaxFlow.ReplaceSinglePasFlowAt( m_iChangedIdx, pNewFlow );		
		m_allPaxFlow.InitHierachy();

		BuildHierarchyNonPaxFlow(pNewFlow);
	}

	delete m_pPaxFlowAttachedToNonPaxFlow;
	m_pPaxFlowAttachedToNonPaxFlow = NULL;
			
	delete m_pNonPaxFlow;
	m_pNonPaxFlow = NULL;

	int nSel = m_nSel;
	
	if( nSel < 0 || nSel >= m_listCtrlPaxType.GetItemCount())
	{
		m_FlowTree.DeleteAllItems();
		return;
	}
	ItemData* pid = (ItemData*)m_listCtrlPaxType.GetItemData( nSel );
	int iIdx = pid->nACTermUser;
	ASSERT( iIdx >=0 && iIdx < m_allPaxFlow.GetSinglePaxTypeFlowCount() );
	m_iChangedIdx = iIdx;
	m_iListSelect = iIdx;
	m_bChanged = false;

	m_OperatedPaxFlow.ClearAllPair();

	m_vCopyItem.clear();

	const CMobileElemConstraint* pConstrint = m_allPaxFlow.GetSinglePaxTypeFlowAt( iIdx )->GetPassengerConstrain();
	m_OperatedPaxFlow.SetPaxConstrain( pConstrint );
	if( pConstrint->GetTypeIndex() == 0 )//pax constraint
	{
		m_allPaxFlow.BuildHierarchyFlow( m_paxFlowConvertor, iIdx , m_OperatedPaxFlow );
//		m_OperatedPaxFlow.PrintAllStructure();
	}
	else//non pax constrant
	{
		CMobileElemConstraint attachedPaxConstraint( *pConstrint);
		//char strPconst [ 256 ];
		CString strPconst;
		pConstrint->screenPrint( strPconst );
		// TRACE("\n%s\n", strPconst.GetBuffer(0));
		attachedPaxConstraint.SetTypeIndex( 0 );
		attachedPaxConstraint.SetMobileElementType(enum_MobileElementType_PAX);
		attachedPaxConstraint.screenPrint( strPconst,0,256 );
		// TRACE("\n%s\n", strPconst.GetBuffer(0) );

		//pax flow
		if( !m_pPaxFlowAttachedToNonPaxFlow )
		{
			m_pPaxFlowAttachedToNonPaxFlow = new CSinglePaxTypeFlow ( GetInputTerminal() );
		}
		
		m_pPaxFlowAttachedToNonPaxFlow->SetPaxConstrain( &attachedPaxConstraint );
		m_pPaxFlowAttachedToNonPaxFlow->ClearAllPair();
		
		
		m_pPaxFlowAttachedToNonPaxFlow->GetPassengerConstrain()->screenPrint( strPconst );
		// TRACE("\n%s\n", strPconst.GetBuffer(0) );
		int iIdx = m_allPaxFlow.GetIdxIfExist( *m_pPaxFlowAttachedToNonPaxFlow );
		if( iIdx < 0 )//not exist
		{
			CSinglePaxTypeFlow* pPaxFlowAttachedToNonPaxFlow = new CSinglePaxTypeFlow ( GetInputTerminal() );
			pPaxFlowAttachedToNonPaxFlow->SetPaxConstrain( &attachedPaxConstraint );
			//char str[ 64 ];
			CString str;
			attachedPaxConstraint.screenPrint( str );
			// TRACE("\n%s\n", str.GetBuffer(0));
			m_allPaxFlow.AddPaxFlow( pPaxFlowAttachedToNonPaxFlow );
			iIdx = m_allPaxFlow.GetIdxIfExist( *m_pPaxFlowAttachedToNonPaxFlow );
			ASSERT( iIdx >= 0 );
			m_allPaxFlow.BuildHierarchyFlow( m_paxFlowConvertor, iIdx , *m_pPaxFlowAttachedToNonPaxFlow );
			m_allPaxFlow.DeleteSinglePaxTypeFlowAt( iIdx );
		}
		else
		{   //create m_pPaxFlowAttachedToNonPaxFlow
			m_allPaxFlow.BuildHierarchyFlow( m_paxFlowConvertor, iIdx , *m_pPaxFlowAttachedToNonPaxFlow );
		}

		//non pax flow
		if( !m_pNonPaxFlow )
		{
			m_pNonPaxFlow = new CSinglePaxTypeFlow ( GetInputTerminal() );
		}

		m_pNonPaxFlow->SetPaxConstrain( pConstrint );
		iIdx = m_allPaxFlow.GetIdxIfExist( *m_pNonPaxFlow );
		ASSERT( iIdx >= 0 );
#ifdef _TRACE_ 
		// TRACE("NonPaxFlow before BuildHierarchyFlow\n");
//		m_pNonPaxFlow->PrintAllStructure();
#endif	// get stored nonPaxFlow , create it if it is Null;	

  	 	if(m_allPaxFlow.GetSinglePaxTypeFlowAt(m_iChangedIdx)->GetFlowPairCount() <= 0)
		{   //create new nonPaxFlow
			// BuildHierarchyFlow can create NonPaxFlow , attention BuildNonPaxFlowToFile who can also create NonPaxFlow
			m_allPaxFlow.BuildHierarchyFlow( m_paxFlowConvertor, iIdx , *m_pNonPaxFlow );
		}
		else
		{   //get nonPaxFlow
			CSinglePaxTypeFlow* curNonPaxflow = m_allPaxFlow.GetSinglePaxTypeFlowAt(m_iChangedIdx) ;
			int nFlowPairCount = curNonPaxflow->GetFlowPairCount();
			bool bHaveHead = FALSE;
			bool bHaveTail = FALSE;
			for (int n = 0 ; n < nFlowPairCount;n ++)
			{
				CProcessorDestinationList* tempFlowPair = curNonPaxflow->GetFlowPairAt(n);
				m_pNonPaxFlow->AddFowPair(*tempFlowPair);
				ProcessorID startProcID = tempFlowPair->GetProcID();
				ProcessorID endProcID  ;
							
				if (!bHaveHead) 
				{
					bHaveHead = m_pPaxFlowAttachedToNonPaxFlow->IfExist(startProcID);
				}
				if (!bHaveTail) 
				{
					for (int i =0; i < tempFlowPair->GetDestCount(); i++)
					{
						endProcID = tempFlowPair->GetDestProcAt(0).GetProcID();
						bHaveTail = m_pPaxFlowAttachedToNonPaxFlow->IfExist(endProcID);
					}					
				}
			}		
//			m_pNonPaxFlow->PrintAllStructure();
			
			if(!bHaveHead||!bHaveTail)				
			{
				m_allPaxFlow.BuildHierarchyFlow( m_paxFlowConvertor, iIdx , *m_pNonPaxFlow );
			}
		}
#ifdef _TRACE_ 
		// TRACE("NonPaxFlow After BuildHierarchyFlow\n");
//		m_pNonPaxFlow->PrintAllStructure();
		// TRACE("m_pPaxFlowAttachedToNonPaxFlow \n");
//		m_pPaxFlowAttachedToNonPaxFlow->PrintAllStructure();
#endif
		//get new NonPaxFlow and OperatedPaxFlow
		m_paxFlowConvertor.BuildAttachedNonPaxFlow( *m_pPaxFlowAttachedToNonPaxFlow, *m_pNonPaxFlow, m_OperatedPaxFlow );
        //store new NonPaxFlow to m_allPaxFlow
		if (m_pNonPaxFlow->IfFlowBeChanged()) 
		{			
			CSinglePaxTypeFlow* pNewFlow = new CSinglePaxTypeFlow( GetInputTerminal() );
			pNewFlow->SetPaxConstrain( pConstrint );
			
			int i = m_pNonPaxFlow->GetFlowPairCount();		
			for (int n = 0 ; n < i;n ++)
			{
				CProcessorDestinationList* tempFlowPair = m_pNonPaxFlow->GetFlowPairAt(n);
				pNewFlow->AddFowPair( *tempFlowPair );
			}		
//			pNewFlow->PrintAllStructure();
			m_allPaxFlow.ReplaceSinglePasFlowAt( m_iChangedIdx, pNewFlow );				
			m_allPaxFlow.InitHierachy();
			m_OperatedPaxFlow.SetChangedFlag(true);
			m_bChanged = true;
		}
		
#ifdef _TRACE_ 
		// TRACE("PaxFlowAttachedToNonPaxFlow After BuildAttachedNonPaxFlow\n");		
//		m_pPaxFlowAttachedToNonPaxFlow->PrintAllStructure();
		// TRACE("NonPaxFlow After BuildAttachedNonPaxFlow\n");		
//		m_pNonPaxFlow->PrintAllStructure();
		// TRACE("Result OperatedPaxFlow After BuildAttachedNonPaxFlow\n");				
//		m_OperatedPaxFlow.PrintAllStructure();	
#endif
	}

	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
	if( !m_OperatedPaxFlow.IfExist( *pProcStart->getID() ) )
	{
		if( m_OperatedPaxFlow.GetFlowPairCount() <= 0 )
			m_OperatedPaxFlow.AddIsolatedProc( *pProcStart->getID() );
	}	
//	m_OperatedPaxFlow.SetChangedFlag( false );
	m_GateFlow.ClearAllPair();// becaust of the flow display purpose
	m_vInterestInPath.clear();
	ReloadTree();
	EnableBtnFromTBar(m_ToolBarMEType,IDC_BTN_DELETE,TRUE);
//when select changed collapse window
	ExpandWindow( FALSE );
	EnableBtnFromTBar(m_ToolBarMEType,IDC_BTN_COPY_FLOW,TRUE);
	m_vFoundItem.clear();
	m_strFind = "";
}

void CPaxFlowDlg::OnBtnNew() 
{
	CPassengerTypeDialog dlg( m_pParentWnd ,FALSE, TRUE);
	//dlg.SetShowFlightIDFlag( FALSE );
	if (dlg.DoModal() == IDCANCEL)
		return;

	if( m_bChanged && m_iChangedIdx >= 0 && m_OperatedPaxFlow.IfFlowBeChanged() )
	{
		m_bChanged = false;
		//CSinglePaxTypeFlow m_tempFlow;// = m_allPaxFlow.GetSinglePaxTypeFlowAt( m_iChangedIdx );
		CSinglePaxTypeFlow* pNewFlow = new CSinglePaxTypeFlow( GetInputTerminal() );
		pNewFlow->SetPaxConstrain( m_OperatedPaxFlow.GetPassengerConstrain() );
		pNewFlow->SetParentPaxFlow( m_OperatedPaxFlow.GetParentPaxFlow() ); 

		if( !m_pNonPaxFlow )
		{
			m_allPaxFlow.BuildSinglePaxFlow( m_paxFlowConvertor, m_iChangedIdx, m_OperatedPaxFlow, *pNewFlow );
		}
		else
		{
			m_paxFlowConvertor.BuildNonPaxFlowToFile( *m_pPaxFlowAttachedToNonPaxFlow, *m_pNonPaxFlow, m_OperatedPaxFlow, *pNewFlow );
//			pNewFlow->PrintAllStructure();

			delete m_pPaxFlowAttachedToNonPaxFlow;
			m_pPaxFlowAttachedToNonPaxFlow = NULL;
			
			delete m_pNonPaxFlow;
			m_pNonPaxFlow = NULL;
			
		}
		pNewFlow->SetChangedFlag( true );
		m_allPaxFlow.ReplaceSinglePasFlowAt( m_iChangedIdx, pNewFlow );
		m_allPaxFlow.InitHierachy();
		BuildHierarchyNonPaxFlow(pNewFlow);
		//m_iChangedIdx = -1;
	}

	delete m_pPaxFlowAttachedToNonPaxFlow;
	m_pPaxFlowAttachedToNonPaxFlow = NULL;
			
	delete m_pNonPaxFlow;
	m_pNonPaxFlow = NULL;

	
    m_allPaxFlow.FromDigraphToOldStructure( m_paxFlowConvertor );
	
	CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();

	
	CSinglePaxTypeFlow* pNewPaxFlow = new CSinglePaxTypeFlow( GetInputTerminal() );

	pNewPaxFlow->SetPaxConstrain( &mobileConstr );
	int iIdx = m_allPaxFlow.GetIdxIfExist( *pNewPaxFlow );
	if( -1 != iIdx ) // exist
	{
		m_iListSelect = iIdx ;
		delete pNewPaxFlow;
		ReloadPaxList();
		OnSelchangeListPaxtype();
		m_vInterestInPath.clear();
		ReloadTree();
		return;
	}

	m_allPaxFlow.AddPaxFlow( pNewPaxFlow );
	m_iListSelect = m_allPaxFlow.GetIdxIfExist( *pNewPaxFlow );
	ASSERT( m_iListSelect >= 0 );
	ReloadPaxList();
	OnSelchangeListPaxtype();
	m_vInterestInPath.clear();
	ReloadTree();

	ExpandWindow( FALSE );
	EnableBtnFromTBar(m_ToolBarMEType,IDC_BTN_DELETE,TRUE);
	m_btnSave.EnableWindow();

	if( m_iPreSortByColumnIdx >= 0 )
	{
		CWaitCursor	wCursor;
	}	
}


void CPaxFlowDlg::OnBtnDelete() 
{	
	// for each source process find the pax type and delte.
 	// start from the root
	POSITION pos = m_listCtrlPaxType.GetFirstSelectedItemPosition();
	
	int nSel = m_listCtrlPaxType.GetNextSelectedItem(pos);
	if( nSel < 0 && nSel >= m_listCtrlPaxType.GetItemCount( ))
		return;

	m_FlowTree.DeleteAllItems();
	if( nSel < 0 || nSel >= m_listCtrlPaxType.GetItemCount())
		return;	
	ItemData* pid = (ItemData*)m_listCtrlPaxType.GetItemData( nSel );
	int nIdx = pid->nACTermUser;
	//int iSize = m_allPaxFlow.GetSinglePaxTypeFlowCount();
	GetInputTerminal()->m_pPassengerFlowDB->deletePaxType( m_allPaxFlow.GetSinglePaxTypeFlowAt( nIdx )->GetPassengerConstrain() );
	GetInputTerminal()->m_pStationPaxFlowDB->deletePaxType( m_allPaxFlow.GetSinglePaxTypeFlowAt( nIdx )->GetPassengerConstrain() );
	m_allPaxFlow.DeleteSinglePaxTypeFlowAt( nIdx );

	
	delete m_pPaxFlowAttachedToNonPaxFlow;
	delete m_pNonPaxFlow;

	m_pPaxFlowAttachedToNonPaxFlow = NULL;
	m_pNonPaxFlow = NULL;
	
	m_btnSave.EnableWindow();
	m_iChangedIdx = -1;
	m_iListSelect = -1;
	ReloadPaxList();

	ExpandWindow( FALSE );
	m_butAdvance.EnableWindow( FALSE );
	
	EnableBtnFromTBar(m_ToolBarMEType,IDC_BTN_COPY_FLOW,FALSE);

	if( m_iPreSortByColumnIdx >= 0 )
	{
		CWaitCursor	wCursor;
	}		
	
}

void CPaxFlowDlg::OnBtnSave() 
{
//	OnPaxflowEvenPercent();
//	if(!ConfirmOneXOneAvailable())
//		return;
	if( m_bChanged && m_iChangedIdx >= 0 && m_OperatedPaxFlow.IfFlowBeChanged() )
	{
		m_bChanged = false;
//CSinglePaxTypeFlow m_tempFlow;// = m_allPaxFlow.GetSinglePaxTypeFlowAt( m_iChangedIdx );
		CSinglePaxTypeFlow* pNewFlow = new CSinglePaxTypeFlow( GetInputTerminal() );
		pNewFlow->SetPaxConstrain( m_OperatedPaxFlow.GetPassengerConstrain() );
		pNewFlow->SetParentPaxFlow( m_OperatedPaxFlow.GetParentPaxFlow() ); 
		m_OperatedPaxFlow.PrintAllStructure();

		if( !m_pNonPaxFlow )
		{
			m_allPaxFlow.BuildSinglePaxFlow( m_paxFlowConvertor, m_iChangedIdx, m_OperatedPaxFlow, *pNewFlow );
		}
		else
		{
			m_paxFlowConvertor.BuildNonPaxFlowToFile( *m_pPaxFlowAttachedToNonPaxFlow, *m_pNonPaxFlow, m_OperatedPaxFlow, *pNewFlow );
			pNewFlow->PrintAllStructure();

			delete m_pPaxFlowAttachedToNonPaxFlow;
			m_pPaxFlowAttachedToNonPaxFlow = NULL;
			
			delete m_pNonPaxFlow;
			m_pNonPaxFlow = NULL;
			
		}
		
		pNewFlow->PrintAllStructure();
		pNewFlow->SetChangedFlag( true );
		m_allPaxFlow.ReplaceSinglePasFlowAt( m_iChangedIdx, pNewFlow );
		m_allPaxFlow.InitHierachy();
		BuildHierarchyNonPaxFlow(pNewFlow);
//m_iChangedIdx = -1;
	}

	delete m_pPaxFlowAttachedToNonPaxFlow;
	m_pPaxFlowAttachedToNonPaxFlow = NULL;
			
	delete m_pNonPaxFlow;
	m_pNonPaxFlow = NULL;

	if( m_bGateChanged && m_iGateChangedIdx >= 0 && m_GateFlow.IfFlowBeChanged() )
	{
		//InsertORUpdateFlightFlow();
		m_bGateChanged = false;
		CSinglePaxTypeFlow* pNewFlow = new CSinglePaxTypeFlow( GetInputTerminal() );
		pNewFlow->SetPaxConstrain( m_GateFlow.GetPassengerConstrain() );
		pNewFlow->SetParentPaxFlow( m_GateFlow.GetParentPaxFlow() ); 
		m_allFlightFlow.BuildSinglePaxFlow( m_paxFlowConvertor, m_iGateChangedIdx, m_GateFlow, *pNewFlow );
		pNewFlow->SetChangedFlag( true );
		m_allFlightFlow.ReplaceSinglePasFlowAt( m_iGateChangedIdx, pNewFlow );
		m_allFlightFlow.InitHierachy();
	}

	AfxGetApp()->BeginWaitCursor();
	if( m_allPaxFlow.IfAllFlowValid( true ) )
	{
		m_allPaxFlow.FromDigraphToOldStructure( m_paxFlowConvertor );
		// SAVE THE WHOLE DATA
		GetInputTerminal()->m_pPassengerFlowDB->saveDataSet( GetProjPath(), true );	
		GetInputTerminal()->m_pStationPaxFlowDB->saveDataSet( GetProjPath(), true );
		GetInputTerminal()->GetInputFlowSync()->saveDataSet(GetProjPath(), true);
	}
	else
	{
		return;
	}

	if( m_allFlightFlow.IfAllFlowValid( true ))
	{
		//m_allFlightFlow.FromDigraphToOldStructure( m_paxFlowConvertor );
		m_paxFlowConvertor.ToOldFlightFlow( m_allFlightFlow );
		GetInputTerminal()->m_pSpecificFlightPaxFlowDB->saveDataSet( GetProjPath() ,true );
	}
	else
	{
		return;
	}
	m_bChanged = false;
	m_btnSave.EnableWindow( FALSE );
	AfxGetApp()->EndWaitCursor();

	ExpandWindow( FALSE );
}

void CPaxFlowDlg::SaveSyncPointData()
{
	const CMobileElemConstraint* paxType = m_OperatedPaxFlow.GetPassengerConstrain();
	
	int nTypeIndex = paxType->GetTypeIndex();
	CInputFlowSync* pInputFlowSync = GetInputTerminal()->GetInputFlowSync();
	if (pInputFlowSync == NULL)
		return;
	
	for (int i = 0; i < pInputFlowSync->GetCount(); i++)
	{
		CMobileElemConstraint mobType;
		if (nTypeIndex == 0)
			mobType = *m_OperatedPaxFlow.GetPassengerConstrain();
		else
			mobType = *m_pPaxFlowAttachedToNonPaxFlow->GetPassengerConstrain();
		
		CInputFlowPaxSyncItem* pPaxItem = pInputFlowSync->GetItem(i);
		if (pPaxItem->GetMobElementConstraint().fits(mobType))//select pax flow or parent of select pax flow
		{
			if (nTypeIndex == 0)
			{
				if (pPaxItem->GetMobElementConstraint() == mobType)//exist in current pax flow
				{
					if(!m_OperatedPaxFlow.IfExist(pPaxItem->GetProcID()) )
					{
						pInputFlowSync->DeleteItem(pPaxItem);
					}
				}
			}
			else
			{
				for (int j = 0; j < pPaxItem->GetCount(); j++)
				{
					CInputFlowSyncItem* pNonPaxItem = pPaxItem->GetItem(j);
					if (*paxType == pNonPaxItem->GetMobElementConstraint())//current non passenger flow
					{
						if (!m_OperatedPaxFlow.IfExist(pNonPaxItem->GetProcID()))
						{
							pPaxItem->DeleteItem(pNonPaxItem);
						}
					}
				}
			}
		}
	}
}

void CPaxFlowDlg::OnOK() 
{
//	OnPaxflowEvenPercent();
//	if(!ConfirmOneXOneAvailable())
//		return;
		
	if( m_bChanged && m_iChangedIdx >= 0 && m_OperatedPaxFlow.IfFlowBeChanged() )
	{
		m_bChanged = false;
		//CSinglePaxTypeFlow m_tempFlow;// = m_allPaxFlow.GetSinglePaxTypeFlowAt( m_iChangedIdx );
		CSinglePaxTypeFlow* pNewFlow = new CSinglePaxTypeFlow( GetInputTerminal() );
		pNewFlow->SetPaxConstrain( m_OperatedPaxFlow.GetPassengerConstrain() );
		pNewFlow->SetParentPaxFlow( m_OperatedPaxFlow.GetParentPaxFlow() ); 
		if( !m_pNonPaxFlow )
		{
			m_allPaxFlow.BuildSinglePaxFlow( m_paxFlowConvertor, m_iChangedIdx, m_OperatedPaxFlow, *pNewFlow );
		}
		else
		{
			m_paxFlowConvertor.BuildNonPaxFlowToFile( *m_pPaxFlowAttachedToNonPaxFlow, *m_pNonPaxFlow, m_OperatedPaxFlow, *pNewFlow );
			pNewFlow->PrintAllStructure();

			delete m_pPaxFlowAttachedToNonPaxFlow;
			m_pPaxFlowAttachedToNonPaxFlow = NULL;
			
			delete m_pNonPaxFlow;
			m_pNonPaxFlow = NULL;
			
		}
		pNewFlow->SetChangedFlag( true );
//		pNewFlow->PrintAllStructure();
		m_allPaxFlow.ReplaceSinglePasFlowAt( m_iChangedIdx, pNewFlow );
		m_allPaxFlow.InitHierachy();
		BuildHierarchyNonPaxFlow(pNewFlow);
		//m_iChangedIdx = -1;
	}

	delete m_pPaxFlowAttachedToNonPaxFlow;
	m_pPaxFlowAttachedToNonPaxFlow = NULL;
			
	delete m_pNonPaxFlow;
	m_pNonPaxFlow = NULL;
	
	if( m_bGateChanged && m_iGateChangedIdx >= 0 && m_GateFlow.IfFlowBeChanged() )
	{
		//InsertORUpdateFlightFlow();
		m_bGateChanged = false;
		CSinglePaxTypeFlow* pNewFlow = new CSinglePaxTypeFlow( GetInputTerminal() );
		pNewFlow->SetPaxConstrain( m_GateFlow.GetPassengerConstrain() );
		pNewFlow->SetParentPaxFlow( m_GateFlow.GetParentPaxFlow() ); 
		m_allFlightFlow.BuildSinglePaxFlow( m_paxFlowConvertor, m_iGateChangedIdx, m_GateFlow, *pNewFlow );
		pNewFlow->SetChangedFlag( true );
		m_allFlightFlow.ReplaceSinglePasFlowAt( m_iGateChangedIdx, pNewFlow );
		m_allFlightFlow.InitHierachy();
	}

	Update3DViewData( NULL , NULL);
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		m_bCheckConveyorBtn=FALSE;
		SetViewPickConveyor();
		
		CDialog::OnOK();
		return;
	}

	AfxGetApp()->BeginWaitCursor();
	if( m_allPaxFlow.IfAllFlowValid( true ) )
	{
		m_allPaxFlow.FromDigraphToOldStructure( m_paxFlowConvertor );
		// SAVE THE WHOLE DATA
		GetInputTerminal()->m_pPassengerFlowDB->saveDataSet( GetProjPath(), true );	
		GetInputTerminal()->m_pStationPaxFlowDB->saveDataSet( GetProjPath(), true );
		GetInputTerminal()->GetInputFlowSync()->saveDataSet(GetProjPath(), true);
	}
	else
	{
		return;
	}

	if( m_allFlightFlow.IfAllFlowValid( true ))
	{
		//m_allFlightFlow.FromDigraphToOldStructure( m_paxFlowConvertor );
		m_paxFlowConvertor.ToOldFlightFlow( m_allFlightFlow );
		GetInputTerminal()->m_pSpecificFlightPaxFlowDB->saveDataSet( GetProjPath() ,true );
	}
	else
	{
		return;
	}
	AfxGetApp()->EndWaitCursor();

	ExpandWindow( FALSE );
	m_bCheckConveyorBtn=FALSE;
	SetViewPickConveyor();
	
	CDialog::OnOK();
}

void CPaxFlowDlg::OnCancel() 
{
	m_bCheckConveyorBtn=FALSE;
	SetViewPickConveyor();
	
	// TODO: Add extra cleanup here
	Update3DViewData( NULL , NULL);
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		ExpandWindow( FALSE );

		CDialog::OnCancel();
		return;
	}

	
	// RELOAD DATABASE
	AfxGetApp()->BeginWaitCursor();
	try
	{
		GetInputTerminal()->m_pPassengerFlowDB->loadDataSet( GetProjPath() );
		GetInputTerminal()->m_pStationPaxFlowDB->loadDataSet( GetProjPath() );

		GetInputTerminal()->m_pSpecificFlightPaxFlowDB->loadDataSet( GetProjPath() );
		GetInputTerminal()->GetInputFlowSync()->loadDataSet(GetProjPath());
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;			
	}
	AfxGetApp()->EndWaitCursor();

	delete m_pPaxFlowAttachedToNonPaxFlow;
	m_pPaxFlowAttachedToNonPaxFlow = NULL;
			
	delete m_pNonPaxFlow;
	m_pNonPaxFlow = NULL;
	ExpandWindow( FALSE );
	CDialog::OnCancel();
}

int CPaxFlowDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_ToolBarFlowTree.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS ) ||
		!m_ToolBarFlowTree.LoadToolBar(IDR_MOBILE_ELEMENT_FLOWTREE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	DWORD dwExStyle = TBSTYLE_EX_DRAWDDARROWS;
	m_ToolBarFlowTree.GetToolBarCtrl().SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)dwExStyle);
	DWORD dwStyle = m_ToolBarFlowTree.GetButtonStyle(m_ToolBarFlowTree.CommandToIndex(ID_PAXFLOW_SYNCPOINT));
	dwStyle |= TBSTYLE_DROPDOWN;
	m_ToolBarFlowTree.SetButtonStyle(m_ToolBarFlowTree.CommandToIndex(ID_PAXFLOW_SYNCPOINT), dwStyle);

	if (!m_ToolBarMEType.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS ) ||
		!m_ToolBarMEType.LoadToolBar(IDR_MOBILE_ELEMENT_TYPE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}


void CPaxFlowDlg::CheckToolBarEnable()
{

	m_hRClickItem = m_FlowTree.GetSelectedItem();

	if( m_hRClickItem == NULL )
		return;
}

void CPaxFlowDlg::OnSelchangedTreeFlow(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here	
//	CPoint pt;
//	GetCursorPos(&pt);
//	m_FlowTree.ScreenToClient(&pt);
///	UINT iRet;
//	HTREEITEM hItem = m_FlowTree.HitTest(pt, &iRet);
/////	Bird.hu changed 2004.5.17
	UINT iRet = TVHT_ONITEMLABEL;
	HTREEITEM hItem = m_FlowTree.GetSelectedItem();
////		
	UpdateFlowTreeToolBarBtn(iRet,hItem);

	CheckToolBarEnable();
	if(hItem!=NULL)
	{
		if( m_FlowTree.GetItemText(hItem).CompareNoCase(CS_CYCLIC_LABEL) == 0)
			return;

		TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( hItem );
		ASSERT( pData );
		const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
		if(pData->m_procId==*pProcStart->getID())
		{
			*pResult = 0;
			return;
		}
		
		TBBUTTONINFO tbBtnInfo;
		tbBtnInfo.cbSize=sizeof(TBBUTTONINFO);
		tbBtnInfo.dwMask=TBIF_IMAGE;
		if(pData->m_arcInfo->GetTypeOfUsingPipe()==1)
		{
			CImageList* pIL=m_ToolBarFlowTree.GetToolBarCtrl().GetImageList();
			
			tbBtnInfo.iImage=pIL->GetImageCount()-2;
		}	
		else
		{
			tbBtnInfo.iImage=9;
		}
		BOOL b=m_ToolBarFlowTree.GetToolBarCtrl().SetButtonInfo(ID_PAXFLOW_EDIT_PIPE_AUTO,&tbBtnInfo);
	}
	*pResult = 0;
}

void CPaxFlowDlg::OnButtonPrint() 
{
	// TODO: Add your command handler code here
	m_bUpTree = TRUE;
	m_FlowTree.PrintTree("Tree header","Tree footer");
}




bool CPaxFlowDlg::IsStation( const ProcessorID& _procID )
{
	GroupIndex gIndex = GetInputTerminal()->procList->getGroupIndex( _procID );
	Processor* pProc = GetInputTerminal()->procList->getProcessor( gIndex.start );
	if( pProc == NULL )
		return false;
	return pProc->getProcessorType() == IntegratedStationProc;
}


void CPaxFlowDlg::PaxFlowCurSourceLink(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( hItem );
	ASSERT( pData );

	HTREEITEM hFather = m_FlowTree.GetParentItem( hItem );	
	TREEITEMDATA* pFatherData = NULL;
	// modified in 2003-2-9
	if( hFather )
	{
		pFatherData = (TREEITEMDATA* )m_FlowTree.GetItemData( hFather );
		ASSERT( pFatherData ); 
		m_OperatedPaxFlow.CutLinkWithSourceProc( pFatherData->m_procId, pData->m_procId );
		BuildInterestInPath( hFather );

	}
	else
	{
		//prune whole tree from root
		m_OperatedPaxFlow.CutLinkWithSourceProc( pData->m_procId, pData->m_procId );
		m_vInterestInPath.clear();
	}
}

void CPaxFlowDlg::OnPaxflowCutSourceLink() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		PaxFlowCurSourceLink(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			PaxFlowCurSourceLink(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	HTREEITEM hFather = m_FlowTree.GetParentItem( m_hRClickItem );
	if( hFather )
	{
		BuildInterestInPath( hFather );
	}
	else
	{
		m_vInterestInPath.clear();
	}

	SaveSyncPointData();
	m_bChanged = true;
	m_btnSave.EnableWindow();
	ReloadTree();	
}

void CPaxFlowDlg::PaxFlowAddDestNode(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( hItem );
	ASSERT( pData );

	int iType =-1;
	if( IsStation( pData->m_procId ) )
	{
		HTREEITEM hFather = m_FlowTree.GetParentItem( hItem );	
		if( hFather )
		{
			TREEITEMDATA* pFatherData = (TREEITEMDATA* )m_FlowTree.GetItemData( hFather );
			ASSERT( pFatherData );
			if( IsStation( pFatherData->m_procId) )
			{
				iType = 3; // he is a station and have a father , his father is a station			
			}
			else
			{
				iType = 2; // he is a station and have a father , but father is not a station
			}
		}
		else
		{
			iType = 1; // he is a station , but no father		
		}
	}
	else
	{
		iType = 0; // he is not station		
	}




	ProcessorID id;
	if( !GetProcessorID(id) )
		return;

	//const Processor* pProcEnd = GetInputTerminal()->procList->getProcessor( END_PROCESSOR_INDEX );
	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
	if( id == *(pProcStart->getID())  )
	{
		MessageBox( "START processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
		return ;
	}

	if( iType == 3 && IsStation( id ) )
	{
		MessageBox( "STATION processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
		return ;
	}

	CFlowDestination tempDestInfo;
	tempDestInfo.SetProcID( id );

	m_OperatedPaxFlow.AddDestProc( pData->m_procId, tempDestInfo );

	CProcessorDestinationList* pFlowPair=m_OperatedPaxFlow.GetEqualFlowPairAt(pData->m_procId);
	ASSERT(pFlowPair!=NULL);
	pFlowPair->EvenPercent();
	m_OperatedPaxFlow.GetEqualFlowPairAt( pData->m_procId )->SetAllDestInherigeFlag( 0 );

}

void CPaxFlowDlg::OnPaxflowAddchildprocessor() 
{	
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		PaxFlowAddDestNode(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			PaxFlowAddDestNode(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_bChanged = true; 
	m_btnSave.EnableWindow();
	BuildInterestInPath( m_hRClickItem );

	m_OperatedPaxFlow.PrintAllStructure();
	ReloadTree();
}

void CPaxFlowDlg::OnPaxflowDeleteprocessors() 
{
	if( !m_bUpTree )
	{
		GateflowDeleteprocessors();
		return;
	}
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( m_hRClickItem );
	ASSERT( pData );
	 
	// // TRACE("%s, %s\n", pData->m_procId.GetIDString(), id.GetIDString() );
	const CMobileElemConstraint* paxType = m_OperatedPaxFlow.GetPassengerConstrain();
	int nTypeIndex = paxType->GetTypeIndex();
	if (nTypeIndex == 0)
	{
		CInputFlowPaxSyncItem* pPaxSyncItem = m_pInTerm->GetInputFlowSync()->GetFlowPaxSync(*paxType,pData->m_procId);
		if (pPaxSyncItem)
		{
			m_pInTerm->GetInputFlowSync()->DeleteItem(pPaxSyncItem);
		}
	}
	else
	{
		CInputFlowPaxSyncItem* pPaxSyncItem = m_pInTerm->GetInputFlowSync()->GetFlowParentNonPaxSync(*paxType,pData->m_procId);
		if (pPaxSyncItem)
		{
			CInputFlowSyncItem* pNonPaxSyncItem = m_pInTerm->GetInputFlowSync()->GetFlowNonPaxSync(*paxType,pData->m_procId);
			if (pNonPaxSyncItem)
			{
				pPaxSyncItem->DeleteItem(pNonPaxSyncItem);
			}
		}
	}
	m_OperatedPaxFlow.DeleteProcNode( pData->m_procId );
	m_bChanged = true;
	m_btnSave.EnableWindow();
	//ClearUselessTree();
	ReloadTree();	
}

void CPaxFlowDlg::OnPaxflowModifypercent() 
{
	if( !m_bUpTree )
	{
		GateflowModifypercent();
		return;
	}

	if( !m_hRClickItem || !m_FlowTree.GetParentItem(m_hRClickItem) )
		return;    // Must be a child item

	m_bChanged = true;
	m_btnSave.EnableWindow();
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( m_hRClickItem );		
	// Show edit box
	m_hModifiedItem = m_hRClickItem;
	//m_FlowTree.SpinEditLabel(m_hRClickItem);	
	m_FlowTree.SetDisplayType( 4 );
	m_FlowTree.SetDisplayNum( pData->m_arcInfo->GetProbality() );
	m_FlowTree.SetSpinRange( 0,100 );
	CString sPercentStr;
// 	if (pData->m_arcInfo->GetAppointedPercentFlag())
	{
		sPercentStr.Format(" %d%% ",pData->m_arcInfo->GetProbality() );		
	}
//	else
//	{
//		sPercentStr= "";
//		pData->m_arcInfo->SetAppointedPercentFlag(TRUE);
//		m_OperatedPaxFlow.SetChangedFlag( true );			
//	}
//	sPercent.Format(" %d%% ",pData->m_arcInfo->GetProbality() );
	m_FlowTree.SpinEditLabel( m_hRClickItem ,sPercentStr );
	m_btnSave.EnableWindow();
}

void CPaxFlowDlg::OnPaxflowDelpercent() 
{
	if( !m_bUpTree )
	{
		GateflowModifypercent();
		return;
	}
	
	if( !m_hRClickItem || !m_FlowTree.GetParentItem(m_hRClickItem) )
		return;    // Must be a child item
	
	m_bChanged = true;
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();
	
	m_OperatedPaxFlow.SetChangedFlag( true );				
	m_btnSave.EnableWindow();
	//////////////////////////////////////////////////////////////////////////
	HTREEITEM hFather = m_FlowTree.GetParentItem( m_hRClickItem );	
	ASSERT( hFather );
	TREEITEMDATA* pFatherData = (TREEITEMDATA* )m_FlowTree.GetItemData( hFather );
	ASSERT( pFatherData ); 
	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( m_hRClickItem );
	ASSERT( pData );	
//	pData->m_arcInfo->SetAppointedPercentFlag(FALSE);	
	//////////////////////////////////////////////////////////////////////////
	OnPaxflowEvenPercent();
	ReloadTree();
}

void CPaxFlowDlg::PaxFlowEventRemaining(HTREEITEM hItem)
{
	HTREEITEM hFather = m_FlowTree.GetParentItem( hItem );
	if ( hFather == NULL) return ;		
	TREEITEMDATA* pFatherData = (TREEITEMDATA* )m_FlowTree.GetItemData( hFather );
	ASSERT( pFatherData ); 
	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( hItem );
	ASSERT( pData );

	m_OperatedPaxFlow.EvenRemainDestPercent( pFatherData->m_procId, pData->m_procId );

}

void CPaxFlowDlg::OnPaxflowEvenPercent() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		PaxFlowEventRemaining(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			PaxFlowEventRemaining(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}


	m_btnSave.EnableWindow();
	BuildInterestInPath( m_hRClickItem );
	ReloadTree();	
	
		
}

void CPaxFlowDlg::PaxFlowDeleteNodeOnly(HTREEITEM hItem)
{
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( hItem );
	ASSERT( pData );

	HTREEITEM hFather = m_FlowTree.GetParentItem( hItem );
	TREEITEMDATA* pFatherData = NULL;
	if( hFather )
	{
		pFatherData =(TREEITEMDATA* )m_FlowTree.GetItemData( hFather );
		ASSERT( pFatherData ); 
		m_OperatedPaxFlow.TakeOverDestProc( pFatherData->m_procId, pData->m_procId );
	}
	else
	{
		m_OperatedPaxFlow.TakeOverDestProc( pData->m_procId, pData->m_procId );
	}

}

void CPaxFlowDlg::OnPaxflowTakeover() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		PaxFlowDeleteNodeOnly(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			PaxFlowDeleteNodeOnly(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	SaveSyncPointData();

	HTREEITEM hFather = m_FlowTree.GetParentItem( m_hRClickItem );
	if( hFather )
	{
		BuildInterestInPath( hFather );
	}
	else
	{
		m_vInterestInPath.clear();
	}

	m_bChanged = true;
	m_btnSave.EnableWindow();
	ReloadTree();	
	
}
void CPaxFlowDlg::PaxFlowDeleteNodeAll(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( hItem );
	ASSERT( pData );

	// // TRACE("%s, %s\n", pData->m_procId.GetIDString(), id.GetIDString() );
	m_OperatedPaxFlow.TakeOverDestProc( pData->m_procId );
}

void CPaxFlowDlg::OnPaxflowTakeoverByall() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		PaxFlowDeleteNodeAll(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			PaxFlowDeleteNodeAll(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	SaveSyncPointData();

	HTREEITEM hFather = m_FlowTree.GetParentItem( m_hRClickItem );
	if( hFather )
	{
		BuildInterestInPath( hFather );
	}
	else
	{
		m_vInterestInPath.clear();
	}

	m_bChanged = true;
	m_btnSave.EnableWindow();
	ReloadTree();		
}

void CPaxFlowDlg::PaxFlowInsertAfterNode(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( hItem );
	ASSERT( pData );



	ProcessorID id;
	if( !GetProcessorID(id) )
		return;

	if( IsStation( id ) )
	{
		MessageBox( "STATION processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
		return ;
	}

	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
	if( id  == *(pProcStart->getID()) )
	{
		MessageBox( "START processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
		return ;
	}

	const Processor* pProcEnd = GetInputTerminal()->procList->getProcessor( END_PROCESSOR_INDEX );
	if( id == *(pProcEnd->getID())  )
	{
		MessageBox( "END processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
		return ;
	}



	m_OperatedPaxFlow.InsertProceoosorAfter( pData->m_procId, id );
}

void CPaxFlowDlg::OnPaxflowInsertAfter() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		PaxFlowInsertAfterNode(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			PaxFlowInsertAfterNode(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_bChanged = true;
	m_btnSave.EnableWindow();

	BuildInterestInPath( m_hRClickItem );
	ReloadTree();		
}

BOOL CPaxFlowDlg::CheckIfHaveCircle(CSinglePaxTypeFlow* _pFlow)
{
	InputTerminal* pTerm = _pFlow->GetInputTerm();
	ASSERT( pTerm );
	std::vector<ProcessorID> vProcessID;
	int iPairCount = _pFlow->GetFlowPairCount();
	for( int i=iPairCount-1; i>=0; --i )
	{
		CString sStr = _pFlow->GetFlowPairAt( i )->GetProcID().GetIDString();
		if( pTerm->m_pSubFlowList->IfProcessUnitExist( sStr ) )
		{
			vProcessID.push_back( _pFlow->GetFlowPairAt( i )->GetProcID() );
		}
	}

	int iProcessCount = vProcessID.size();
	for(int i=0; i<iProcessCount; ++i )
	{
		CSubFlow* pProcess = pTerm->m_pSubFlowList->GetProcessUnit( vProcessID[i].GetIDString() );
		ASSERT( pProcess );
		CSinglePaxTypeFlow* pFlowInProcess = pProcess->GetInternalFlow();

		std::vector<ProcessorID> vSourceID;
		_pFlow->GetSourceProc( vProcessID[i] ,vSourceID );

		std::vector<ProcessorID> vRootInProcess;
		pFlowInProcess->GetRootProc( vRootInProcess );

		std::vector<ProcessorID> vLeafInProcess;
		pFlowInProcess->GetLeafNode( vLeafInProcess );

		int iSourceCount = vSourceID.size();
		int iRootCount = vRootInProcess.size();
		int iLeafCount = vLeafInProcess.size();

		for( int j=0; j<iSourceCount; ++j )
		{
			CProcessorDestinationList* pPair = _pFlow->GetFlowPairAt( vSourceID[ j ] );
			ASSERT( pPair );
			for( int k=0; k<iRootCount; ++k )
			{
				CFlowDestination tempDest;
				tempDest.SetProcID( vRootInProcess[ k] );
				pPair->AddDestProc( tempDest, false );
			}

			CProcessorDestinationList* pProcessPair = _pFlow->GetFlowPairAt( vProcessID[ i ] );
			int iPairDest = pProcessPair->GetDestCount();
			for(int k=0; k<iLeafCount; ++k )
			{
				for( int s=0; s<iPairDest; ++s )
				{
					pProcessPair = _pFlow->GetFlowPairAt( vProcessID[ i ] );// must get pProcessPair here
					CFlowDestination tempDest;
					tempDest.SetProcID( pProcessPair->GetDestProcAt( s ).GetProcID() );		

					_pFlow->AddDestProc( vLeafInProcess[ k ], tempDest, false );

					
					if(HandleSingleFlowLogic::IfHaveCircleFromProc(_pFlow,vLeafInProcess[ k ]))
					{
						CString strText;
						strText.Format("If you add the processor,will make a circle!\r\nSo can not add it:\r\n   Sub flow: %s\r\n   The processor: %s",
							pProcess->GetProcessUnitName(), vLeafInProcess[ k ].GetIDString()); 
						MessageBox(strText, "Error", MB_OK|MB_ICONWARNING );
						_pFlow->ResetDesitinationFlag(false);
						return TRUE;			
					}
				}
			}
		}

		_pFlow->MergeTwoFlow( *pFlowInProcess );
		//		_pFlow->PrintAllStructure();
		_pFlow->DeleteProcNode( vProcessID[ i ] );
	}

	_pFlow->ResetDesitinationFlag(false);
	return FALSE;
}

void CPaxFlowDlg::PaxFlowInsertBeforeNode(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( hItem );
	ASSERT( pData );
	int iType =-1;
	if( IsStation( pData->m_procId ) )
	{
		HTREEITEM hFather = m_FlowTree.GetParentItem( hItem );
		if( hFather )
		{
			TREEITEMDATA* pFatherData = (TREEITEMDATA* )m_FlowTree.GetItemData( hFather );
			ASSERT( pFatherData );
			if( IsStation( pFatherData->m_procId) )
			{
				iType = 3; // he is a station and have a father , his father is a station
				return;
			}
			else
			{
				iType = 2; // he is a station and have a father , but father is not a station
			}
		}
		else
		{
			iType = 1; // he is a station , but no father
			return;
		}
	}
	else
	{

		HTREEITEM hFather = m_FlowTree.GetParentItem( hItem );
		if( hFather )
		{
			TREEITEMDATA* pFatherData = (TREEITEMDATA* )m_FlowTree.GetItemData( hFather );
			ASSERT( pFatherData );
			if( IsStation( pFatherData->m_procId) )
			{
				iType = 4; // he is not a station and have a father , his father is a station
			}
			else
			{
				iType = 5; // he is not a station and have a father , but father is not a station
			}
		}
		else
		{
			return;
		}
	}


	ProcessorID id;
	if( !GetProcessorID(id) )
		return;

	//if( iType == 4 && IsStation( id ) )
	if( IsStation( id ) )
	{
		MessageBox( "STATION processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
		return ;
	}

	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
	if( id  == *(pProcStart->getID()) )
	{
		MessageBox( "START processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
		return ;
	}

	const Processor* pProcEnd = GetInputTerminal()->procList->getProcessor( END_PROCESSOR_INDEX );
	if( id == *(pProcEnd->getID())  )
	{
		MessageBox( "END processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
		return ;
	}


	HTREEITEM hFather = m_FlowTree.GetParentItem( hItem );	
	TREEITEMDATA* pFatherData = (TREEITEMDATA* )m_FlowTree.GetItemData( hFather );
	ASSERT( pData );
	ASSERT( pFatherData ); 
	CFlowDestination temp;
	temp.SetProcID( id );

	m_OperatedPaxFlow.InsertBetwwen( pFatherData->m_procId, pData->m_procId, temp );
	m_bChanged = true;
	m_btnSave.EnableWindow();

	BuildInterestInPath( hFather );
	m_vInterestInPath.insert(m_vInterestInPath.begin(), id );
	m_vInterestInPath.insert(m_vInterestInPath.begin(), pData->m_procId );

	ReloadTree();
}

void CPaxFlowDlg::OnPaxflowInsertBettween() 
{	
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		PaxFlowInsertBeforeNode(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			PaxFlowInsertBeforeNode(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}
	
	m_bChanged = true;
	m_btnSave.EnableWindow();

	ReloadTree();
	
}

void CPaxFlowDlg::OnPaxflowEditConveyor()
{
	m_bCheckConveyorBtn=!m_bCheckConveyorBtn;
	SetViewPickConveyor();
}

void CPaxFlowDlg::PaxFlowEditPipManual(HTREEITEM hItem,CPaxFlowSelectPipes* pSelectPipeDlg)
{
	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( hItem );
	ASSERT( pData );
	
	ASSERT(pSelectPipeDlg->m_bSelectPipe );

	if (pSelectPipeDlg->m_vPipeIdx.size() <= 0) 
	{
		pData->m_arcInfo->SetTypeOfUsingPipe( 0 );			
	}
	else
	{
		pData->m_arcInfo->SetTypeOfUsingPipe( 2 );
	}
	pData->m_arcInfo->GetPipeVector() = pSelectPipeDlg->m_vPipeIdx;

}

void CPaxFlowDlg::OnPaxflowEditPipe() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( m_hRClickItem );

	CPaxFlowSelectPipes dlg( GetInputTerminal(), pData->m_arcInfo->GetPipeVector(), pData->m_arcInfo->GetTypeOfUsingPipe() );
	if( dlg.DoModal() == IDOK )
	{
		if (m_vCopyItem.empty())
		{
			PaxFlowEditPipManual(m_hRClickItem,&dlg);
		}
		else
		{
			for (unsigned i = 0; i < m_vCopyItem.size(); i++)
			{
				TreeNodeInfor& nodeData = m_vCopyItem[i];
				PaxFlowEditPipManual(nodeData.m_hTreeNode,&dlg);
			}
			m_vCopyItem.clear();
		}
	}

	m_bChanged = true;
	m_btnSave.EnableWindow();
	m_OperatedPaxFlow.SetChangedFlag( true );

	BuildInterestInPath( m_hRClickItem );
	ReloadTree();			
}

void CPaxFlowDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	m_hRClickItem = NULL;

	if( m_treeGate.IsWindowVisible() )
	{
		CRect rectGateTree;
		m_treeGate.GetWindowRect( &rectGateTree);
		if( rectGateTree.PtInRect( point))
		{
			GateFlowPopMenu( pWnd, point );
			return;
		}
	}
	PopProcessDefineMenu(point);
}




LRESULT CPaxFlowDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_INPLACE_SPIN)
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		if (pst->iPercent >= 0)
		{
			SetPercent( pst->iPercent );
		}
//		m_OperatedPaxFlow.SetChangedFlag( true );		
//		OnPaxflowEvenPercent();
		return TRUE;
	}
	else if (message == WM_NOTIFY)
	{

	}
	if (message == WM_NOTIFY)
	{
		switch(wParam)
		{
		case ID_WND_SPLITTER1:
		case ID_WND_SPLITTER2:
		case ID_WND_SPLITTER3:
		case ID_WND_SPLITTER4:
		case ID_WND_SPLITTER5:
			{
				SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
				DoResize(pHdr->delta,wParam);
			}
			break;
		}
	}
	if (message == WM_KEYDOWN )
	{
		char chCharCode = (TCHAR) wParam;    // character code 
		long lKeyData = lParam;   
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}


void CPaxFlowDlg::SetPercent(int _nPercent)
{
	if( !m_bUpTree )
	{
		SetGatePercent( _nPercent );
		return;
	}
	ASSERT( m_hRClickItem );
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( m_hRClickItem );
	ASSERT( pData );
	pData->m_arcInfo->SetProbality( _nPercent );
	m_bChanged = true;
	m_btnSave.EnableWindow();
	m_OperatedPaxFlow.SetChangedFlag( true );

	BuildInterestInPath( m_hRClickItem );
	ReloadTree();
}

CString CPaxFlowDlg::GetPipeString( CFlowDestination* _pArcInfo )
{
	CString sReturnStr = "";
	if( _pArcInfo->GetTypeOfUsingPipe() == 1 )// via pipe system
	{
		sReturnStr = " [ Via :Pipe System ] ";
		return sReturnStr;
	}
	else if(_pArcInfo->GetTypeOfUsingPipe() == 0 )
		return "";

	int iCount = _pArcInfo->GetPipeVector().size();	
	if( iCount <=0 )
		return "";

	sReturnStr = " [ Via :";
	InputTerminal* pTerm = GetInputTerminal();
	for( int i=0; i < iCount-1; ++i )
	{
		sReturnStr += pTerm->m_pPipeDataSet->GetPipeAt( _pArcInfo->GetPipeVector()[ i ] )->GetPipeName();
		sReturnStr += " --> ";
	}
	sReturnStr += pTerm->m_pPipeDataSet->GetPipeAt( _pArcInfo->GetPipeVector()[ iCount-1 ] )->GetPipeName();
	sReturnStr += " ] ";
	return sReturnStr;
}
CString CPaxFlowDlg::GetOtherInfo(  CFlowDestination* _pArcInfo )
{
	CString sReturnStr("");
	if( _pArcInfo->GetOneToOneFlag() )
	{
		sReturnStr = " {1:1} ";
	}

	if (_pArcInfo->GetOneXOneState() == ChannelStart)
	{
		sReturnStr = sReturnStr + "{1:X:1 Start}";
		if (m_stack1x1.empty()) 
		{
			m_b1x1SeqError =FALSE;
		}
		m_stack1x1.push(ChannelStart);
	}

	if (_pArcInfo->GetOneXOneState() == ChannelEnd)
	{
		sReturnStr = sReturnStr + "{1:X:1 End}";
		if (m_stack1x1.empty()) 
		{
			m_b1x1SeqError = TRUE;
		}
		else
		{
			m_stack1x1.pop();
		}
	}

	if (_pArcInfo->GetFollowState() == TRUE)
	{
		sReturnStr = sReturnStr + "(Follower)";		
	}
	const CMobileElemConstraint* pPaxType = m_OperatedPaxFlow.GetPassengerConstrain();
	int nTypeIndex = pPaxType->GetTypeIndex();
	CInputFlowPaxSyncItem* pPaxSyncItem = NULL;
	if (nTypeIndex == 0)
	{
		pPaxSyncItem = m_pInTerm->GetInputFlowSync()->GetFlowPaxSync(*pPaxType,_pArcInfo->GetProcID());
	}
	else
	{
		pPaxSyncItem = m_pInTerm->GetInputFlowSync()->GetFlowParentNonPaxSync(*pPaxType,_pArcInfo->GetProcID());
	}
	
	if (pPaxSyncItem)
	{
		CString strSyncPoint;
		strSyncPoint.Format(_T("{%s}"), pPaxSyncItem->GetSyncName());
		sReturnStr += strSyncPoint;
	}

	return sReturnStr;
}

CString CPaxFlowDlg::GetArcInfoDescription( CFlowDestination* _pArcInfo , bool _bNeedDetail /*= false*/,bool* _pbHasComplementData /*=NULL*/)
{
	if( !_pArcInfo )
	{
		return "";
	}
	bool _bHasComplementData=false;
	if( _pArcInfo->GetDensityOfArea() > 0 || _pArcInfo->GetAvoidFixQueue() > 0 
		|| _pArcInfo->GetAvoidOverFlowQueue() >0 || _pArcInfo->GetMaxQueueLength() > 0 
		|| _pArcInfo->GetMaxWaitMins() > 0 || _pArcInfo->GetMaxSkipTime()>0)
	{
			_bHasComplementData  = true;
	}
	else
	{
		_bHasComplementData  = false;
	}
	
	CString sPercent;
	sPercent.Format("( %d%% )", _pArcInfo->GetProbality() );	

	if( _bNeedDetail )
	{
		CString sReturnStr;
		sReturnStr += sPercent + GetPipeString( _pArcInfo )+GetOtherInfo(_pArcInfo);

		if( _pArcInfo->GetMaxQueueLength() >0 )
		{
			CString sQueueLengthStr;
			sQueueLengthStr.Format("%d",_pArcInfo->GetMaxQueueLength() );
			sReturnStr += " Queue Length (<=" + sQueueLengthStr + " ),";
		}
		if( _pArcInfo->GetMaxWaitMins() > 0 )
		{
			CString sQueueTimeStr;
			sQueueTimeStr.Format("%d",_pArcInfo->GetMaxWaitMins() );
			sReturnStr += " Wait Time (<=" + sQueueTimeStr + " mins),";
		}
		 
		if(_pArcInfo->GetMaxSkipTime()>0)
		{
           CString sSkipWhenTimeLessThan;
		   sSkipWhenTimeLessThan.Format("%d",_pArcInfo->GetMaxSkipTime());
		   sReturnStr +="Skip When Time to STD Less Than"+sSkipWhenTimeLessThan+"mins";
		}

		if( _pArcInfo->GetDensityOfArea() > 0 )
		{
			sReturnStr += " Avoid Density: ";
			CString sDensity;
			sDensity.Format("%.2f ,",_pArcInfo->GetDensityOfArea() );
			sReturnStr += sDensity;
			int iSize = _pArcInfo->GetDensityArea().size();
			for( int i=0; i<iSize; ++i )
			{
				CString sNumber;
				sNumber.Format("(%d)",i );
				sReturnStr += sNumber + _pArcInfo->GetDensityArea().at( i );
				sReturnStr+=",";
			}		
		}

		if( _pArcInfo->GetAvoidFixQueue() )
		{
			sReturnStr += " Avoid Fixed Queue ,";
		}

		if( _pArcInfo->GetAvoidOverFlowQueue() )
		{
			sReturnStr += " Avoid  Overflow Queue ,";
		}

		if(_pbHasComplementData)
			*_pbHasComplementData  = _bHasComplementData;
		return sReturnStr;
		
	}
	else
	{
		if(_pbHasComplementData)
			*_pbHasComplementData  = _bHasComplementData;

		if(_bHasComplementData)
		{
			return sPercent + GetPipeString( _pArcInfo ) + GetOtherInfo( _pArcInfo ) +" ...";
		}
		else
		{
			return  sPercent + GetPipeString( _pArcInfo ) + GetOtherInfo( _pArcInfo );
		}
	}
}
void CPaxFlowDlg::Update3DViewData(const CSinglePaxTypeFlow* _pSPTF, const CSinglePaxTypeFlow* _pSPTFForGateAssaign)
{
	
		if(C3DView* p3DView = Get3DView())
		{
			if( m_checkDisplayFlow.GetCheck() == 1 )
			{
				p3DView->GetParentFrame()->UpdateFlowGeometry(_pSPTF, _pSPTFForGateAssaign);
			}
			else
			{
				p3DView->GetParentFrame()->UpdateFlowGeometry(NULL, NULL);
			}
		}
	
}

void CPaxFlowDlg::Update3DViewData()
{
	if(C3DView* p3DView = Get3DView())
	{
		if( m_checkDisplayFlow.GetCheck() == 1 )
		{
			p3DView->GetParentFrame()->UpdateFlowGeometry( &m_OperatedPaxFlow, &m_GateFlow );
		}
		else
		{
			p3DView->GetParentFrame()->UpdateFlowGeometry(NULL, NULL);
		}
	}
}

C3DView* CPaxFlowDlg::Get3DView()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	C3DView* p3DView = pDoc->Get3DView();
	/*
	if( p3DView == NULL )
	{
		pDoc->GetMainFrame()->CreateOrActivateFrame(  theApp.m_p3DTemplate, RUNTIME_CLASS(C3DView) );
		p3DView = pDoc->Get3DView();
	}
	*/
	return p3DView;
}
void CPaxFlowDlg::ClearUselessTree()
{
	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
	if( m_OperatedPaxFlow.IfExist( *pProcStart->getID() ) )
	{
		m_OperatedPaxFlow.ClearUselessTree( *pProcStart->getID() );	
	}
	else
	{
		if( m_hRClickItem )
		{
			HTREEITEM hParent;
			HTREEITEM hChild = m_hRClickItem;
			hParent = m_FlowTree.GetParentItem( hChild );
			while( 1 )
			{
				if( hParent )
				{
					hChild = hParent;
					hParent = m_FlowTree.GetParentItem( hChild );
				}
				else
				{
					break;
				}
			}
		 TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( hChild );
		 ASSERT( pData );
		 m_OperatedPaxFlow.ClearUselessTree( pData->m_procId );

		}
	}
}


void CPaxFlowDlg::OnPaxflowAddIsolateNode() 
{
	CProcesserDialog dlg( GetInputTerminal() );
	dlg.SetDisplayProcess( true );

	
	if (dlg.DoModal() != IDOK)
		return;

	ProcessorID id;
	if( !dlg.GetProcessorID(id) )
		return;

	if( !m_bUpTree )
	{
		m_GateFlow.AddIsolatedProc( id );
		m_bGateChanged = true;
		m_btnSave.EnableWindow();
		LoadGateTree();	
		return;
	}
	
	
	m_OperatedPaxFlow.AddIsolatedProc( id );
	//m_OperatedPaxFlow.PrintAllStructure();
	m_bChanged = true;
	m_btnSave.EnableWindow();
	ReloadTree();
}

void CPaxFlowDlg::PaxFlowSetOneToOne(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( hItem );
	ASSERT( pData );
	pData->m_arcInfo->SetOneToOneFlag( !pData->m_arcInfo->GetOneToOneFlag() );
}

void CPaxFlowDlg::OnPaxflowOnetoone() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		PaxFlowSetOneToOne(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			PaxFlowSetOneToOne(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_bChanged = true;
	m_btnSave.EnableWindow();
	m_OperatedPaxFlow.SetChangedFlag( true );
	BuildInterestInPath( m_hRClickItem );
	ReloadTree();
	
}
/*
void CPaxFlowDlg::OnUpdatePaxflowOnetoone(CCmdUI* pCmdUI) 
{
	
	
}
*/
///////////////////////////////////////////////////
//add by Bird
//////////////////////////////////////////////////
void CPaxFlowDlg::OnPaxflowInsertFromgate() 
{
	// TODO: Add your command handler code here
	//Bird add 2003-1-18
	//insert a @FROM GATE to flow
	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
	const Processor* pFromGate	= GetInputTerminal()->procList->getProcessor( FROM_GATE_PROCESS_INDEX );
	m_OperatedPaxFlow.InsertProceoosorAfter( *pProcStart->getID(), *pFromGate->getID() );
	m_bChanged = true;
	m_btnSave.EnableWindow();

	m_vInterestInPath.clear();
	ReloadTree();	
}

void CPaxFlowDlg::OnPaxflowInsertTogate() 
{
	// TODO: Add your command handler code here
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();
	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( m_hRClickItem );
	ASSERT( pData );

	const Processor* pProcEnd = GetInputTerminal()->procList->getProcessor( END_PROCESSOR_INDEX );
	const Processor* pToGate = GetInputTerminal()->procList->getProcessor(TO_GATE_PROCESS_INDEX );

	CFlowDestination mDestInfo;
	mDestInfo.SetProcID( *(pToGate->getID()) );
	m_OperatedPaxFlow.AddDestProc( pData->m_procId, mDestInfo);

	mDestInfo.SetProcID( *(pProcEnd->getID() ));
	m_OperatedPaxFlow.AddDestProc( *pToGate->getID(), mDestInfo );
	
	m_bChanged = true;
	m_btnSave.EnableWindow();
	BuildInterestInPath( m_hRClickItem );
	ReloadTree();		
}

//inser a START/END processor from pop menu
void CPaxFlowDlg::OnPaxflowInsertStart() 
{
	// TODO: Add your command handler code here
	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
	m_OperatedPaxFlow.AddIsolatedProc( *(pProcStart->getID()) );
	m_bChanged = true;
	m_btnSave.EnableWindow();
	m_vInterestInPath.clear();
	ReloadTree();
}

void CPaxFlowDlg::PaxFlowAddEndProcessor(HTREEITEM hItem)
{	
	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( hItem );
	ASSERT( pData );

	const Processor* pProcEnd = GetInputTerminal()->procList->getProcessor( END_PROCESSOR_INDEX );
	CFlowDestination mDestInfo;
	mDestInfo.SetProcID( *(pProcEnd->getID() ));
	m_OperatedPaxFlow.AddDestProc( pData->m_procId, mDestInfo );
}

void CPaxFlowDlg::OnPaxflowInsertEnd() 
{
	// TODO: Add your command handler code here
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		PaxFlowAddEndProcessor(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			PaxFlowAddEndProcessor(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}
	
	m_bChanged = true;
	m_btnSave.EnableWindow();

	BuildInterestInPath( m_hRClickItem );
	ReloadTree();				
}



void CPaxFlowDlg::OnButAdvance() 
{
	// TODO: Add your control notification handler code here
	CWaitCursor m_waitCursor;
	
	if( m_bExpand == TRUE )	
	{
		ExpandWindow( FALSE );
	}
	else 
	{
		if( m_allFlightFlow.GetSinglePaxTypeFlowCount() <= 0)
		{
			//ASSERT( m_pInTerm->m_pSpecificFlightPaxFlowDB->getCount() > 0 );
			if( m_pInTerm->m_pSpecificFlightPaxFlowDB->getCount() <= 0 )
			{
				AfxMessageBox( " Flight Schedule, Gate Assignment, or Linkage data is not avaiable !" );
				return;
			}
			m_paxFlowConvertor.ToDigraphStructureFromSinglePaxDB( m_allFlightFlow, m_pInTerm->m_pSpecificFlightPaxFlowDB );
		}

		FilterFlight();
		ExpandWindow( TRUE );
	}
}

void CPaxFlowDlg::FilterFlight()
{
	CString strLabel;
	int i,j;
	//char szTmp[256];
    CString szTmp1;
	// start from the root
	
	int nSel = m_nSel;
	if( nSel < 0 || nSel >= m_listCtrlPaxType.GetItemCount())
		return;
	ItemData* pid = (ItemData*)m_listCtrlPaxType.GetItemData( nSel );
	int nIdx = pid->nACTermUser;
	const CMobileElemConstraint* pMobileCon = m_allPaxFlow.GetSinglePaxTypeFlowAt( nIdx )->GetPassengerConstrain();
	m_listFlight.ResetContent();
	m_treeGate.DeleteAllItems();
	
	std::vector<const CMobileElemConstraint*> m_vFlight;
	//get flight list
//	m_allFlightFlow.PrintAllStructure();
	for( i=0; i<m_allFlightFlow.GetSinglePaxTypeFlowCount(); i++ )
	{
		const CMobileElemConstraint* pCon = m_allFlightFlow.GetSinglePaxTypeFlowAt( i )->GetPassengerConstrain();
		pCon->screenPrint( szTmp1 );
		// TRACE("=========>Mobile Element:%s\n", szTmp1);
		for( j=m_vFlight.size()-1; j>=0 ; j--)
		{
			if( ((FlightConstraint)*pCon).isEqual( m_vFlight.at(j) ) )
				break;
		}
		if( j<0 )
		{
			m_vFlight.push_back( pCon );
			pCon->screenPrint(szTmp1);
			// TRACE("Insert Flight(%d): %s\n",m_vFlight.size(),szTmp1.GetBuffer(0));
		}
	}	

	std::vector<const CSinglePaxTypeFlow*> m_vGateFlow;
	for( i=0; i<static_cast<int>(m_vFlight.size()); i++ )
	{
		const CMobileElemConstraint* pFlight = m_vFlight.at(i);
		//filter flight
		if( ((FlightConstraint*) pMobileCon)->fits( *pFlight ) )
		{
			//poly a new constraint
			CMobileElemConstraint mPolyCon = *pMobileCon;
			mPolyCon.polyConstraint( pFlight );

			//create a new flow 
			CSinglePaxTypeFlow* pNewPaxFlow = new CSinglePaxTypeFlow( GetInputTerminal() );
			pNewPaxFlow->SetPaxConstrain( &mPolyCon );
			int iIdx = m_allFlightFlow.GetIdxIfExist( *pNewPaxFlow );
			if( -1 != iIdx ) // exist
			{
				m_vGateFlow.push_back( m_allFlightFlow.GetSinglePaxTypeFlowAt( iIdx ) );
				delete pNewPaxFlow;
				continue;
			}
			m_allFlightFlow.AddPaxFlow( pNewPaxFlow, false );
			m_vGateFlow.push_back( pNewPaxFlow );

			m_btnSave.EnableWindow();
		}
	}

	//sort and built hierachy
	m_allFlightFlow.Sort();
	m_allFlightFlow.InitHierachy();

	//insert a string to flight list and set itemdata
	for( i=0; i<static_cast<int>(m_vGateFlow.size()); i++ )
	{
		const CSinglePaxTypeFlow* pSingleFlow = m_vGateFlow.at(i);
		int iIndxInAllFlow = m_allFlightFlow.GetIdxIfExist( *pSingleFlow);
		ASSERT( iIndxInAllFlow>=0 );
		pSingleFlow->GetPassengerConstrain()->screenPrint( szTmp1 );

		int Idx = m_listFlight.AddString( szTmp1.GetBuffer(0) );
		m_listFlight.SetItemData( Idx, iIndxInAllFlow );
	}
	
	m_bGateChanged = false;
	m_iGateChangedIdx = -1;
}

void CPaxFlowDlg::OnSelchangeListFlight() 
{
	// TODO: Add your control notification handler code here
	/////////////////////////////////////////////////////////
	if( m_bGateChanged && m_iGateChangedIdx >= 0 && m_GateFlow.IfFlowBeChanged() )
	{
		//InsertORUpdateFlightFlow();
		m_bGateChanged = false;
		CSinglePaxTypeFlow* pNewFlow = new CSinglePaxTypeFlow( GetInputTerminal() );
		pNewFlow->SetPaxConstrain( m_GateFlow.GetPassengerConstrain() );
		pNewFlow->SetParentPaxFlow( m_GateFlow.GetParentPaxFlow() ); 
		m_allFlightFlow.BuildSinglePaxFlow( m_paxFlowConvertor, m_iGateChangedIdx, m_GateFlow, *pNewFlow );
		pNewFlow->SetChangedFlag( true );
		m_allFlightFlow.ReplaceSinglePasFlowAt( m_iGateChangedIdx, pNewFlow );
		m_allFlightFlow.InitHierachy();
	}
	
	int iIdx = (int) m_listFlight.GetItemData( m_listFlight.GetCurSel() );
	ASSERT( iIdx >=0 && iIdx < m_allFlightFlow.GetSinglePaxTypeFlowCount() );
	m_iGateChangedIdx = iIdx;
	m_bGateChanged = false;

	m_GateFlow.ClearAllPair();
	m_GateFlow.SetPaxConstrain( m_allFlightFlow.GetSinglePaxTypeFlowAt( iIdx )->GetPassengerConstrain() );	
//	m_allFlightFlow.GetSinglePaxTypeFlowAt( iIdx )->PrintAllStructure();
	int iSize = m_allFlightFlow.GetSinglePaxTypeFlowAt( iIdx )->GetFlowPairCount();
	m_allFlightFlow.BuildHierarchyFlow( m_paxFlowConvertor, iIdx , m_GateFlow );
//	m_GateFlow.PrintAllStructure();

	m_GateFlow.SetChangedFlag( false );
	LoadGateTree();
	EnableBtnFromTBar(m_ToolBarMEType,IDC_BTN_DELETE,TRUE);
	//m_butRestore.EnableWindow();
}

void CPaxFlowDlg::LoadGateTree()
{
	int iItemDataCount = m_vGateItemData.size();
	for( int i=0; i<iItemDataCount; ++i )
	{
		delete m_vGateItemData[i];
	}

	const Processor* pFromGate = GetInputTerminal()->procList->getProcessor( FROM_GATE_PROCESS_INDEX );
	const Processor* pToGate = GetInputTerminal()->procList->getProcessor( TO_GATE_PROCESS_INDEX );

	m_vGateItemData.clear();
	m_treeGate.DeleteAllItems();
	std::vector<ProcessorID> vAllRootProc;
	
	m_GateFlow.SetAllPairVisitFlag( false );
	m_GateFlow.GetRootProc( vAllRootProc );
	
	int iRootCount = vAllRootProc.size();
	if( 0 == iRootCount )
	{
		std::vector<ProcessorID> vAllInvolvedProc;
		m_GateFlow.GetAllInvolvedProc( vAllInvolvedProc );
		if( vAllInvolvedProc.size() > 0 )// just have once cicle path, so it will be no root processor
		{
			HTREEITEM hItem = m_treeGate.InsertItem( vAllInvolvedProc[ 0 ].GetIDString() );
			//need not for color
			TREEITEMDATA* pTempData = new TREEITEMDATA;
			pTempData->m_arcInfo=NULL;
			pTempData->m_procId = vAllInvolvedProc[ 0 ];
			m_vGateItemData.push_back( pTempData );
			m_treeGate.SetItemData( hItem, (DWORD)pTempData );
			LoadGateSubTree( hItem );
		}
	}
	else
	{
		for(int i=0; i < iRootCount; ++i )
		{
			if( !m_bHaveFromGate && vAllRootProc[i] == *pFromGate->getID())
				continue;
			if( !m_bHaveToGate && vAllRootProc[i] == *pToGate->getID())
				continue;
	
			HTREEITEM hItem = m_treeGate.InsertItem( vAllRootProc[ i ].GetIDString() );
			//need not for color
			TREEITEMDATA* pTempData = new TREEITEMDATA;
			pTempData->m_arcInfo=NULL;
			pTempData->m_procId = vAllRootProc[ i ];
			m_vGateItemData.push_back( pTempData );
			m_treeGate.SetItemData( hItem, (DWORD)pTempData );
			LoadGateSubTree( hItem );		
		}
	}	
}


void CPaxFlowDlg::LoadGateSubTree( HTREEITEM _hItem )
{
	TREEITEMDATA* pTempData = (TREEITEMDATA*)m_treeGate.GetItemData( _hItem );
	// TRACE("%s\n", pTempData->m_procId.GetIDString() );
	CProcessorDestinationList* tempPair = m_GateFlow.GetEqualFlowPairAt( pTempData->m_procId );
	if( tempPair )
	{
		int iDestCount = tempPair->GetDestCount();
		for(int i=0; i<iDestCount; ++i )
		{
			if( !IfHaveCircle( _hItem, tempPair->GetDestProcAt( i ).GetProcID() ) )
			{
				
					TREEITEMDATA* pTempData = new TREEITEMDATA;
					pTempData->m_iSpecialHandleType = 0;
					pTempData->m_procId = tempPair->GetDestProcAt( i ).GetProcID();
					pTempData->m_arcInfo = tempPair->GetDestProcArcInfo( i );
					m_vGateItemData.push_back( pTempData );
					CString sPercent;
					sPercent.Format("( %d%% )", pTempData->m_arcInfo->GetProbality() );								
					HTREEITEM hItem = m_treeGate.InsertItem( pTempData->m_procId.GetIDString() + sPercent , _hItem );						
					//color item 		
					/*
					if( pTempData->m_arcInfo != NULL )
					{
						//if( pTempData->m_arcInfo->GetInheritedFlag() )
						{
							m_FlowTree.SetItemColor( hItem );
							m_FlowTree.SetItemBold( hItem, true );
						}
					}
					*/
					m_treeGate.SetItemData( hItem, (DWORD)pTempData );
					LoadGateSubTree( hItem );
					m_treeGate.Expand( hItem,TVE_EXPAND   );				
				
			}
			else // have a circle
			{
				TREEITEMDATA* pTempData = new TREEITEMDATA;
				pTempData->m_iSpecialHandleType = 1;
				pTempData->m_procId = tempPair->GetDestProcAt( i ).GetProcID();
				pTempData->m_arcInfo = tempPair->GetDestProcArcInfo( i );
				m_vGateItemData.push_back( pTempData );

				CString sPercent;
				sPercent.Format("( %d%% )", pTempData->m_arcInfo->GetProbality() );								
				HTREEITEM hItem = m_FlowTree.InsertItem( pTempData->m_procId.GetIDString() + sPercent, _hItem );	
				//color item 		
				/*
				if( pTempData->m_arcInfo != NULL )
				{
					if( pTempData->m_arcInfo->GetInheritedFlag() )
					{
						m_FlowTree.SetItemColor( hItem );
						m_FlowTree.SetItemBold( hItem, true );
					}
				}
				*/
				m_treeGate.SetItemData( hItem, (DWORD)pTempData );

				
				m_treeGate.SetItemData (m_treeGate.InsertItem(CS_CYCLIC_LABEL, hItem ), NULL );
				m_treeGate.Expand( hItem,TVE_EXPAND   );
				m_treeGate.Expand( _hItem,TVE_EXPAND   );
			}
		}
		m_treeGate.Expand( _hItem,TVE_EXPAND   );
	}
	Update3DViewData( );
}

void CPaxFlowDlg::ExpandWindow(BOOL _bExpand)
{
	CRect m_rectWindow;
	GetWindowRect( &m_rectWindow );
	BOOL bOldStatus = m_bExpand;
	m_bExpand = _bExpand;
	CRect rectWnd;
	GetWindowRect(rectWnd);
	
	if( ! m_bExpand && bOldStatus)
	{
		ShowRightWnd(FALSE);
		CRect rectRight;
		m_FlowTree.GetWindowRect(rectRight);
		ScreenToClient(rectRight);
		SetWindowPos(NULL,0,0,rectRight.right+20,rectWnd.Height(),SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
		m_butAdvance.SetWindowText(_T("Detail >>"));
	}
	else if( m_bExpand && !bOldStatus)
	{
		ShowRightWnd(TRUE);
		SetWindowPos(NULL,0,0,rectWnd.Width()+240,rectWnd.Height(),SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
		m_butAdvance.SetWindowText(_T("Detail <<"));
		CRect rectWnd;
		GetClientRect(rectWnd);
		OnSizeEx(rectWnd.Width(),rectWnd.Height());
		Invalidate();
	}
	
}

void CPaxFlowDlg::GateFlowPopMenu(CWnd *pWnd, CPoint point)
{
	if( m_iGateChangedIdx <0 )
	{
		return;
	}
	m_hGateRClickItem = NULL;
	m_treeGate.SetFocus();   // Set focus
	m_bUpTree = FALSE;
	
	CPoint pt = point;
	m_treeGate.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hItem = m_treeGate.HitTest(pt, &iRet);

	CMenu menuPopup;
	menuPopup.LoadMenu( IDR_MENU_POPUP );
	CMenu* pMenu = menuPopup.GetSubMenu( 2 );
	//delete some item
	//pMenu->RemoveMenu(ID_PAXFLOW_ADD_ISOLATE_NODE,MF_BYCOMMAND);
	pMenu->RemoveMenu(ID_PAXFLOW_EDIT_PIPE,MF_BYCOMMAND);
	pMenu->RemoveMenu(ID_PAXFLOW_ONETOONE,MF_BYCOMMAND);
	pMenu->RemoveMenu(ID_PAXFLOW_INSERT_FROMGATE,MF_BYCOMMAND);
	pMenu->RemoveMenu(ID_PAXFLOW_INSERT_TOGATE,MF_BYCOMMAND);
	pMenu->RemoveMenu(ID_PAXFLOW_INSERT_START,MF_BYCOMMAND);
	pMenu->RemoveMenu(ID_PAXFLOW_INSERT_END,MF_BYCOMMAND);
	pMenu->RemoveMenu(11,MF_BYPOSITION);
	pMenu->RemoveMenu( ID_PAXFLOW_DELETEPROCESSORS, MF_BYCOMMAND );
	
	pMenu->EnableMenuItem( ID_PAXFLOW_ADDCHILDPROCESSOR, MF_GRAYED );
	pMenu->EnableMenuItem( ID_PAXFLOW_MODIFYPERCENT, MF_GRAYED );
	pMenu->EnableMenuItem( ID_PAXFLOW_DELPERCENT, MF_GRAYED );
	
	//pMenu->EnableMenuItem( ID_PAXFLOW_DELETEPROCESSORS, MF_GRAYED );
	pMenu->EnableMenuItem( ID_PAXFLOW_CUT_SOURCE_LINK, MF_GRAYED );
	pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER, MF_GRAYED );
	pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER_BYALL, MF_GRAYED );
	pMenu->EnableMenuItem( ID_PAXFLOW_EDIT_PIPE, MF_GRAYED );
	pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_AFTER, MF_GRAYED );
	pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_BETTWEEN, MF_GRAYED );
	pMenu->EnableMenuItem( ID_PAXFLOW_ONETOONE, MF_GRAYED );
	pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_TOGATE, MF_GRAYED );
	pMenu->EnableMenuItem( ID_PAXFLOW_EVEN_PERCENT,  MF_GRAYED );

	pMenu->EnableMenuItem(ID_PAXFLOW_COPYPROCESSOR,MF_GRAYED);
	pMenu->EnableMenuItem(ID_PAXFLOW_ADDCOPY,MF_GRAYED);
	pMenu->EnableMenuItem(ID_PAXFLOW_INSERTCOPY,MF_GRAYED);

	const Processor* pProcEnd = GetInputTerminal()->procList->getProcessor( END_PROCESSOR_INDEX );
	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
	const Processor* pFromGate = GetInputTerminal()->procList->getProcessor( FROM_GATE_PROCESS_INDEX );
	const Processor* pToGate = GetInputTerminal()->procList->getProcessor( TO_GATE_PROCESS_INDEX );

	if( hItem )
	{
		pMenu->RemoveMenu(ID_PAXFLOW_ADD_ISOLATE_NODE,MF_BYCOMMAND);
	}
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
	{
		//pMenu->EnableMenuItem( ID_PAXFLOW_NODE, MF_ENABLED );
	}
	else 
	{	
		m_hGateRClickItem = hItem;
		TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeGate.GetItemData( hItem );
		if( !pItemData )
			return;
		
		if (!m_vCopyItem.empty())
		{
			pMenu->EnableMenuItem(ID_PAXFLOW_COPYPROCESSOR,MF_ENABLED);
		}
		
		if (!m_vCopyProcessors.empty())
		{
			pMenu->EnableMenuItem(ID_PAXFLOW_ADDCOPY,MF_ENABLED);
			pMenu->EnableMenuItem(ID_PAXFLOW_INSERTCOPY,MF_ENABLED);
		}
		
		
		HTREEITEM hFather = m_treeGate.GetParentItem( m_hGateRClickItem );
		HTREEITEM hChild = m_treeGate.GetChildItem( m_hGateRClickItem );

		TREEITEMDATA* pFatherItemData = NULL;
		if( hFather )
			 pFatherItemData = (TREEITEMDATA*)m_treeGate.GetItemData( hFather );
		
		if( *(pProcEnd->getID()) == pItemData->m_procId  )// " END "
		{


			
			if( hFather )
			{
				pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_BETTWEEN, MF_ENABLED );
			}
		}
		else if( *pProcStart->getID()  == pItemData->m_procId  )	// "START"
		{		
			pMenu->EnableMenuItem( ID_PAXFLOW_ADDCHILDPROCESSOR, MF_ENABLED );
		}
		else 
		{
			pMenu->EnableMenuItem( ID_PAXFLOW_CUT_SOURCE_LINK, MF_ENABLED );
			if( hFather )
			{
				pMenu->EnableMenuItem( ID_PAXFLOW_ADDCHILDPROCESSOR, MF_ENABLED );
				pMenu->EnableMenuItem( ID_PAXFLOW_MODIFYPERCENT, MF_ENABLED );
				pMenu->EnableMenuItem( ID_PAXFLOW_DELPERCENT, MF_ENABLED );
				pMenu->EnableMenuItem( ID_PAXFLOW_EDIT_PIPE, MF_ENABLED );
				pMenu->EnableMenuItem( ID_PAXFLOW_EVEN_PERCENT,  MF_ENABLED );

	            if( pFatherItemData->m_procId == *pProcStart->getID() ) 
				{
					pMenu->EnableMenuItem( ID_PAXFLOW_ONETOONE, MF_GRAYED );
				}
				else
				{
					pMenu->EnableMenuItem( ID_PAXFLOW_ONETOONE, MF_ENABLED );
				}
				
				
				HTREEITEM hGrandFather = m_treeGate.GetParentItem( hFather );
				if( hGrandFather )
				{
					if( !IsStation( pItemData->m_procId ) )
					{
						pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER, MF_ENABLED );
						pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER_BYALL, MF_ENABLED );
					}
				}	
				
				if(  (IsStation( pFatherItemData->m_procId ) && IsStation( pItemData->m_procId ) ) )
				{
					pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER, MF_GRAYED );
					pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER_BYALL, MF_GRAYED );
					pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_BETTWEEN, MF_GRAYED );
					pMenu->EnableMenuItem( ID_PAXFLOW_EDIT_PIPE, MF_GRAYED );
					
				}
				else
				{
					pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_BETTWEEN, MF_ENABLED );
				}
				
			}
			else
			{
				pMenu->EnableMenuItem( ID_PAXFLOW_ADDCHILDPROCESSOR, MF_ENABLED );
				pMenu->EnableMenuItem( ID_PAXFLOW_MODIFYPERCENT, MF_GRAYED );
				pMenu->EnableMenuItem( ID_PAXFLOW_DELPERCENT,MF_GRAYED );
				pMenu->EnableMenuItem( ID_PAXFLOW_EDIT_PIPE, MF_GRAYED );
			}
		}

		
		if( !hChild ||  IsStation( pItemData->m_procId ) )
		{
			if( !hChild)
			{
				pMenu->EnableMenuItem( ID_PAXFLOW_CUT_SOURCE_LINK, MF_ENABLED );
			}
			else
			{
				pMenu->EnableMenuItem( ID_PAXFLOW_CUT_SOURCE_LINK, MF_GRAYED );
			}
		}
		else
		{			
			pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_AFTER, MF_ENABLED );
		}

		if( 1 == pItemData->m_iSpecialHandleType )
		{
			pMenu->EnableMenuItem( ID_PAXFLOW_ADDCHILDPROCESSOR, MF_GRAYED );
			pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER, MF_GRAYED );
			pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER_BYALL, MF_GRAYED );
			pMenu->EnableMenuItem( ID_PAXFLOW_EDIT_PIPE, MF_GRAYED );

		}
		
		if( !hChild && !(pItemData->m_procId == *pProcEnd->getID()) )		
		{
			pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_TOGATE, MF_ENABLED );
		}
		if( pItemData->m_procId == *pFromGate->getID() || pItemData->m_procId == *pToGate->getID() )
		{
			pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER, MF_ENABLED );
		}
		m_treeGate.SelectItem( m_hGateRClickItem );   // Select it
	}

	
	pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
}

void CPaxFlowDlg::GateflowAddchildprocessor()
{
	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeGate.GetItemData( m_hGateRClickItem );
	ASSERT( pData );
	int iType =-1;
	if( IsStation( pData->m_procId ) )
	{
		HTREEITEM hFather = m_treeGate.GetParentItem( m_hGateRClickItem );
		if( hFather )
		{
			TREEITEMDATA* pFatherData = (TREEITEMDATA* )m_treeGate.GetItemData( hFather );
			ASSERT( pFatherData );
			if( IsStation( pFatherData->m_procId) )
			{
				iType = 3; // he is a station and have a father , his father is a station			
			}
			else
			{
				iType = 2; // he is a station and have a father , but father is not a station
			}
		}
		else
		{
			iType = 1; // he is a station , but no father		
		}
	}
	else
	{
		iType = 0; // he is not station		
	}
	
	
	CProcesserDialog dlg( GetInputTerminal() );
	dlg.SetDisplayBaggageDeviceFlag( false );
	
	if( iType == 1 || iType == 2 )
	{
		dlg.SetType( IntegratedStationProc );
	}
	if (dlg.DoModal() != IDOK)
		return;

	ProcessorID id;
	if( !dlg.GetProcessorID(id) )
		return;

	//const Processor* pProcEnd = GetInputTerminal()->procList->getProcessor( END_PROCESSOR_INDEX );
	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
	if( id == *(pProcStart->getID())  )
	{
		MessageBox( "START processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
		return ;
	}

	if( iType == 3 && IsStation( id ) )
	{
		MessageBox( "STATION processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
		return ;
	}
	
	m_bGateChanged = true; 
	m_btnSave.EnableWindow();
	CFlowDestination tempDestInfo;
	tempDestInfo.SetProcID( id );
	// TRACE("%s, %s\n", pData->m_procId.GetIDString(), id.GetIDString() );
	m_GateFlow.AddDestProc( pData->m_procId, tempDestInfo );
	//m_GateFlow.PrintAllStructure();
	LoadGateTree();
}

void CPaxFlowDlg::GateflowInsertBettween()
{
	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeGate.GetItemData( m_hGateRClickItem );
	ASSERT( pData );
	int iType =-1;
	if( IsStation( pData->m_procId ) )
	{
		HTREEITEM hFather = m_treeGate.GetParentItem( m_hGateRClickItem );
		if( hFather )
		{
			TREEITEMDATA* pFatherData = (TREEITEMDATA* )m_treeGate.GetItemData( hFather );
			ASSERT( pFatherData );
			if( IsStation( pFatherData->m_procId) )
			{
				iType = 3; // he is a station and have a father , his father is a station
				return;
			}
			else
			{
				iType = 2; // he is a station and have a father , but father is not a station
			}
		}
		else
		{
			iType = 1; // he is a station , but no father
			return;
		}
	}
	else
	{
			
		HTREEITEM hFather = m_treeGate.GetParentItem( m_hGateRClickItem );
		if( hFather )
		{
			TREEITEMDATA* pFatherData = (TREEITEMDATA* )m_treeGate.GetItemData( hFather );
			ASSERT( pFatherData );
			if( IsStation( pFatherData->m_procId) )
			{
				iType = 4; // he is not a station and have a father , his father is a station
			}
			else
			{
				iType = 5; // he is not a station and have a father , but father is not a station
			}
		}
		else
		{
			return;
		}
	}
	
	CProcesserDialog dlg( GetInputTerminal() );
	dlg.SetDisplayBaggageDeviceFlag( false );
	if (dlg.DoModal() != IDOK)
		return;

	ProcessorID id;
	if( !dlg.GetProcessorID(id) )
		return;

	//if( iType == 4 && IsStation( id ) )
	if( IsStation( id ) )
	{
		MessageBox( "STATION processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
		return ;
	}
	
	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
	if( id  == *(pProcStart->getID()) )
	{
		MessageBox( "START processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
		return ;
	}

	const Processor* pProcEnd = GetInputTerminal()->procList->getProcessor( END_PROCESSOR_INDEX );
	if( id == *(pProcEnd->getID())  )
	{
		MessageBox( "END processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
		return ;
	}
	

	HTREEITEM hFather = m_treeGate.GetParentItem( m_hGateRClickItem );	
	TREEITEMDATA* pFatherData = (TREEITEMDATA* )m_treeGate.GetItemData( hFather );
	ASSERT( pData );
	ASSERT( pFatherData ); 
	CFlowDestination temp;
	temp.SetProcID( id );
	 
	// TRACE("%s, %s\n", pData->m_procId.GetIDString(), id.GetIDString() );
	m_GateFlow.InsertBetwwen( pFatherData->m_procId, pData->m_procId, temp );
	m_bGateChanged = true;
	m_btnSave.EnableWindow();
	LoadGateTree();
}

void CPaxFlowDlg::GateflowInsertAfter()
{
	
	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeGate.GetItemData( m_hGateRClickItem );
	ASSERT( pData );

	CProcesserDialog dlg( GetInputTerminal() );
	dlg.SetDisplayBaggageDeviceFlag( false );
	if (dlg.DoModal() != IDOK)
		return;

	ProcessorID id;
	if( !dlg.GetProcessorID(id) )
		return;
	
	if( IsStation( id ) )
	{
		MessageBox( "STATION processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
		return ;
	}
	
	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
	if( id  == *(pProcStart->getID()) )
	{
		MessageBox( "START processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
		return ;
	}

	const Processor* pProcEnd = GetInputTerminal()->procList->getProcessor( END_PROCESSOR_INDEX );
	if( id == *(pProcEnd->getID())  )
	{
		MessageBox( "END processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
		return ;
	}
	
	m_GateFlow.InsertProceoosorAfter( pData->m_procId, id );
	m_GateFlow.SetChangedFlag( true );
	m_bGateChanged = true;
	m_btnSave.EnableWindow();
	LoadGateTree();
}
void CPaxFlowDlg::GateflowDeleteprocessors()
{
	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeGate.GetItemData( m_hGateRClickItem );
	ASSERT( pData );
	 
	// // TRACE("%s, %s\n", pData->m_procId.GetIDString(), id.GetIDString() );
	m_GateFlow.DeleteProcNode( pData->m_procId );
	m_bGateChanged = true;
	m_btnSave.EnableWindow();

	//ClearGateUselessTree();
	
	LoadGateTree();
}
void CPaxFlowDlg::GateflowCutSourceLink()
{
	//m_GateFlow.PrintAllStructure();
	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeGate.GetItemData( m_hGateRClickItem );
	ASSERT( pData );
	TREEITEMDATA* pFatherData = NULL;
	HTREEITEM hFather = m_treeGate.GetParentItem( m_hGateRClickItem );
	if( hFather )
	{
		pFatherData = (TREEITEMDATA* )m_treeGate.GetItemData( hFather );
		ASSERT( pFatherData ); 
		m_GateFlow.CutLinkWithSourceProc( pFatherData->m_procId, pData->m_procId );
	}
	else
	{
		//prune whole tree from root
		m_GateFlow.CutLinkWithSourceProc( pData->m_procId, pData->m_procId );
	}
	// // TRACE("%s, %s\n", pData->m_procId.GetIDString(), id.GetIDString() );
	//m_GateFlow.CutLinkWithSourceProc( pFatherData->m_procId, pData->m_procId );
	m_bGateChanged = true;
	m_btnSave.EnableWindow();
	//m_GateFlow.PrintAllStructure();
	//ClearGateUselessTree();
	LoadGateTree();	
}
void CPaxFlowDlg::GateflowTakeover()
{
	HTREEITEM hFather = m_treeGate.GetParentItem( m_hGateRClickItem );	
	TREEITEMDATA* pFatherData = (TREEITEMDATA* )m_treeGate.GetItemData( hFather );
	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeGate.GetItemData( m_hGateRClickItem );
	ASSERT( pData );
	ASSERT( pFatherData ); 
	 
	// // TRACE("%s, %s\n", pData->m_procId.GetIDString(), id.GetIDString() );
	m_GateFlow.TakeOverDestProc( pFatherData->m_procId, pData->m_procId );
	m_bGateChanged = true;
	LoadGateTree();	
}
void CPaxFlowDlg::GateflowTakeOverByall()
{
	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeGate.GetItemData( m_hGateRClickItem );
	ASSERT( pData );
	 
	// // TRACE("%s, %s\n", pData->m_procId.GetIDString(), id.GetIDString() );
	m_GateFlow.TakeOverDestProc( pData->m_procId );
	m_bGateChanged = true;
	m_btnSave.EnableWindow();
	LoadGateTree();
}
void CPaxFlowDlg::GateflowModifypercent()
{
	if( !m_hGateRClickItem || !m_treeGate.GetParentItem(m_hGateRClickItem) )
		return;    // Must be a child item

	m_bGateChanged = true;
	m_btnSave.EnableWindow();
	TREEITEMDATA* pData = (TREEITEMDATA*)m_treeGate.GetItemData( m_hGateRClickItem );
	// Show edit box
	m_hModifiedItem = m_hGateRClickItem;
	m_treeGate.SetDisplayType( 4 );
	m_treeGate.SetDisplayNum( pData->m_arcInfo->GetProbality() );
	m_treeGate.SetSpinRange( 1,100 );
	CString sPercent= " ";
//	sPercent.Format(" %d%% ",pData->m_arcInfo->GetProbality() );
	m_treeGate.SpinEditLabel( m_hGateRClickItem ,sPercent );
	m_btnSave.EnableWindow();

}

void CPaxFlowDlg::SetGatePercent( int _nPercent )
{
	ASSERT( m_hGateRClickItem );
	TREEITEMDATA* pData = (TREEITEMDATA*)m_treeGate.GetItemData( m_hGateRClickItem );
	ASSERT( pData );
	pData->m_arcInfo->SetProbality( _nPercent );
	m_bGateChanged = true;
	m_btnSave.EnableWindow();
	m_GateFlow.SetChangedFlag( true );
	LoadGateTree();
}

void CPaxFlowDlg::ClearGateUselessTree()
{
	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
	if( m_GateFlow.IfExist( *pProcStart->getID() ) )
	{
		m_GateFlow.ClearUselessTree( *pProcStart->getID() );	
	}
	else
	{
		if( m_hGateRClickItem )
		{
			HTREEITEM hParent;
			HTREEITEM hChild = m_hGateRClickItem;
			hParent = m_treeGate.GetParentItem( hChild );
			while( 1 )
			{
				if( hParent )
				{
					hChild = hParent;
					hParent = m_treeGate.GetParentItem( hChild );
				}
				else
				{
					break;
				}
			}
		 TREEITEMDATA* pData = (TREEITEMDATA*)m_treeGate.GetItemData( hChild );
		 ASSERT( pData );
		 m_GateFlow.ClearUselessTree( pData->m_procId );

		}
	}
}


void CPaxFlowDlg::OnKillfocusTreeGate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if( m_bGateChanged && m_iGateChangedIdx >= 0 && m_GateFlow.IfFlowBeChanged() )
	{
		//InsertORUpdateFlightFlow();
		m_bGateChanged = false;
		CSinglePaxTypeFlow* pNewFlow = new CSinglePaxTypeFlow( GetInputTerminal() );
		pNewFlow->SetPaxConstrain( m_GateFlow.GetPassengerConstrain() );
		pNewFlow->SetParentPaxFlow( m_GateFlow.GetParentPaxFlow() ); 
		m_allFlightFlow.BuildSinglePaxFlow( m_paxFlowConvertor, m_iGateChangedIdx, m_GateFlow, *pNewFlow );
		pNewFlow->SetChangedFlag( true );
		m_allFlightFlow.ReplaceSinglePasFlowAt( m_iGateChangedIdx, pNewFlow );
		m_allFlightFlow.InitHierachy();
	}
	
	*pResult = 0;
}


void CPaxFlowDlg::OnButRestore() 
{
	// TODO: Add your control notification handler code here
	//delete Detail Gate Flow
	//m_allFlightFlow.DeleteSinglePaxTypeFlowAt( m_iGateChangedIdx );

	//refresh the m_allFlightFlow
	CWaitCursor m_waitCursor;
	m_allFlightFlow.ClearAll();
	if(!m_paxFlowConvertor.BuildSpecFlightFlowFromGateAssign( m_allFlightFlow ))
		return;
//	m_allFlightFlow.PrintAllStructure();

	if( m_allFlightFlow.IfAllFlowValid( true ))
	{
		m_paxFlowConvertor.ToOldFlightFlow( m_allFlightFlow );
		GetInputTerminal()->m_pSpecificFlightPaxFlowDB->saveDataSet( GetProjPath() ,true );
	}

	FilterFlight();
	//ExpandWindow( FALSE );
}



void CPaxFlowDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	HWND hwndT=::GetWindow(m_hWnd, GW_CHILD);
	CRect rectCW;
	CRgn rgnCW;
	rgnCW.CreateRectRgn(0,0,0,0);
	while (hwndT != NULL)
	{
		CWnd* pWnd=CWnd::FromHandle(hwndT)  ;
		if(
			pWnd->IsKindOf(RUNTIME_CLASS(CListBox))||
			pWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl)))
		{
			if(!pWnd->IsWindowVisible())
			{
				hwndT=::GetWindow(hwndT,GW_HWNDNEXT);
				continue;
			}
			pWnd->GetWindowRect(rectCW);
			ScreenToClient(rectCW);
			CRgn rgnTemp;
			rgnTemp.CreateRectRgnIndirect(rectCW);
			rgnCW.CombineRgn(&rgnCW,&rgnTemp,RGN_OR);
		}
		hwndT=::GetWindow(hwndT,GW_HWNDNEXT);
		
	}
	CRect rect;
	GetClientRect(&rect);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);
	CRgn rgnDraw;
	rgnDraw.CreateRectRgn(0,0,0,0);
	rgnDraw.CombineRgn(&rgn,&rgnCW,RGN_DIFF);
	CBrush br(GetSysColor(COLOR_BTNFACE));
	dc.FillRgn(&rgnDraw,&br);
	CRect rectRight=rect;
	rectRight.left=rectRight.right-10;
	dc.FillRect(rectRight,&br);
	rectRight=rect;
	rectRight.top=rect.bottom-44;
	dc.FillRect(rectRight,&br);
}


void CPaxFlowDlg::OnBtnCopyFlow() 
{
	CPassengerTypeDialog dlg( m_pParentWnd,FALSE, TRUE );
	//dlg.SetShowFlightIDFlag( FALSE );
	if (dlg.DoModal() == IDCANCEL)
		return;

	if( m_bChanged && m_iChangedIdx >= 0 && m_OperatedPaxFlow.IfFlowBeChanged() )
	{
		m_bChanged = false;
		//CSinglePaxTypeFlow m_tempFlow;// = m_allPaxFlow.GetSinglePaxTypeFlowAt( m_iChangedIdx );
		CSinglePaxTypeFlow* pNewFlow = new CSinglePaxTypeFlow( GetInputTerminal() );
		pNewFlow->SetPaxConstrain( m_OperatedPaxFlow.GetPassengerConstrain() );
		pNewFlow->SetParentPaxFlow( m_OperatedPaxFlow.GetParentPaxFlow() ); 

		if( !m_pNonPaxFlow )
		{
			m_allPaxFlow.BuildSinglePaxFlow( m_paxFlowConvertor, m_iChangedIdx, m_OperatedPaxFlow, *pNewFlow );
		}
		else
		{
			m_paxFlowConvertor.BuildNonPaxFlowToFile( *m_pPaxFlowAttachedToNonPaxFlow, *m_pNonPaxFlow, m_OperatedPaxFlow, *pNewFlow );
//			pNewFlow->PrintAllStructure();

			delete m_pPaxFlowAttachedToNonPaxFlow;
			m_pPaxFlowAttachedToNonPaxFlow = NULL;
			
			delete m_pNonPaxFlow;
			m_pNonPaxFlow = NULL;
			
		}
		pNewFlow->SetChangedFlag( true );
		m_allPaxFlow.ReplaceSinglePasFlowAt( m_iChangedIdx, pNewFlow );
		m_allPaxFlow.InitHierachy();
		BuildHierarchyNonPaxFlow(pNewFlow);
		//m_iChangedIdx = -1;
	}
    m_allPaxFlow.FromDigraphToOldStructure( m_paxFlowConvertor );
	
	CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();

	if( mobileConstr.GetTypeIndex() > 0 )
	{
		AfxMessageBox("Non-passenger type is not allowed here !");
		return;
	}

	m_copyflow = m_OperatedPaxFlow;
	m_copyflow.SetPaxConstrain( &mobileConstr );
	CSinglePaxTypeFlow* pNewPaxFlow = new CSinglePaxTypeFlow( GetInputTerminal() );

	pNewPaxFlow->SetPaxConstrain( &mobileConstr );
	int iIdx = m_allPaxFlow.GetIdxIfExist( *pNewPaxFlow );
	if( -1 != iIdx ) // exist
	{
		m_iListSelect = iIdx ;
		delete pNewPaxFlow;
		ReloadPaxList();
		OnSelchangeListPaxtype();
		m_OperatedPaxFlow = m_copyflow;
		m_bChanged = true;
		m_OperatedPaxFlow.SetChangedFlag( true );
		m_vInterestInPath.clear();
		ReloadTree();
		ExpandWindow( FALSE );
		EnableBtnFromTBar(m_ToolBarMEType,IDC_BTN_DELETE,TRUE);
		m_btnSave.EnableWindow();
		return;
	}


	m_allPaxFlow.AddPaxFlow( pNewPaxFlow );
	m_iListSelect = m_allPaxFlow.GetIdxIfExist( *pNewPaxFlow );
	ASSERT( m_iListSelect >= 0 );
	ReloadPaxList();
	OnSelchangeListPaxtype();
	m_OperatedPaxFlow = m_copyflow;
	m_bChanged = true;
	m_OperatedPaxFlow.SetChangedFlag( true );
	
	m_vInterestInPath.clear();
	ReloadTree();

	ExpandWindow( FALSE );
	EnableBtnFromTBar(m_ToolBarMEType,IDC_BTN_DELETE,TRUE);
	m_btnSave.EnableWindow();

	if( m_iPreSortByColumnIdx >= 0 )
	{
		CWaitCursor	wCursor;
	}	
}

void CPaxFlowDlg::BuildInterestInPath( HTREEITEM _hCurrentItem  ,std::vector<ProcessorID>& _vInterestInPath  )
{
	_vInterestInPath.clear();
	HTREEITEM hItem = _hCurrentItem;
	while( hItem )
	{
		TREEITEMDATA* pTempData =(TREEITEMDATA*) m_FlowTree.GetItemData( hItem );
		ASSERT( pTempData );
		_vInterestInPath.push_back( pTempData->m_procId );
		hItem = m_FlowTree.GetParentItem( hItem );
	}
	
}
CString CPaxFlowDlg::BuildPathKeyString( HTREEITEM _hCurrentItem )
{
	CString sReturnValue;
	HTREEITEM hItem = _hCurrentItem;
	while( hItem )
	{
		TREEITEMDATA* pTempData =(TREEITEMDATA*) m_FlowTree.GetItemData( hItem );
		ASSERT( pTempData );
		sReturnValue += pTempData->m_procId.GetIDString();
		hItem = m_FlowTree.GetParentItem( hItem );
	}
	return sReturnValue;
}

void CPaxFlowDlg::BuildCollapsedNodeVector()
{
	m_vCollapsedNode.clear();
	BuildCollapsedNodeVectorSubRoute( m_FlowTree.GetRootItem() );
}

void CPaxFlowDlg::BuildCollapsedNodeVectorSubRoute( HTREEITEM _hRoot )
{
	if( !_hRoot || !m_FlowTree.ItemHasChildren( _hRoot ) )
		return;
	UINT result = TVIS_EXPANDED & m_FlowTree.GetItemState( _hRoot , TVIS_EXPANDED );
	if( !result )//collapsed
		m_vCollapsedNode.push_back( BuildPathKeyString( _hRoot ) );

	HTREEITEM hChildItem = m_FlowTree.GetChildItem( _hRoot );
	BuildCollapsedNodeVectorSubRoute( hChildItem );
	while( hChildItem = m_FlowTree.GetNextSiblingItem( hChildItem ) )
	{
		BuildCollapsedNodeVectorSubRoute( hChildItem );
	}
	
}
void CPaxFlowDlg::CollapsedTree()
{
	if( m_vCollapsedNode.size() <= 0 )
		return;
	HTREEITEM hRoot = m_FlowTree.GetRootItem();
	CollapsedSpecificNode( hRoot );
	
}
void CPaxFlowDlg::CollapsedSpecificNode( HTREEITEM _hCurrentItem  )
{
	if( !_hCurrentItem || !m_FlowTree.ItemHasChildren( _hCurrentItem ) )
		return;
	
	if( IfThisItemShouldBeCollapsed(_hCurrentItem ) )
	{
		m_FlowTree.Expand( _hCurrentItem, TVE_COLLAPSE );
	}

	HTREEITEM hChildItem = m_FlowTree.GetChildItem( _hCurrentItem );
	CollapsedSpecificNode( hChildItem );
	while( hChildItem = m_FlowTree.GetNextSiblingItem( hChildItem ) )
	{
		CollapsedSpecificNode( hChildItem );
	}
}
bool CPaxFlowDlg::IfThisItemShouldBeCollapsed(  HTREEITEM _hCurrentItem )
{
	CString sPathKey = BuildPathKeyString( _hCurrentItem );
	if( std::find( m_vCollapsedNode.begin(), m_vCollapsedNode.end(),sPathKey ) != m_vCollapsedNode.end() )
		return true;
	else
		return false;
}
bool CPaxFlowDlg::IsInterestNode( HTREEITEM _hCurrentItem )
{
	int iInterestPathNodeCount = m_vInterestInPath.size();
	if( iInterestPathNodeCount <= 0 )
		return false;

	std::vector<ProcessorID> vTestPath;
	BuildInterestInPath( _hCurrentItem, vTestPath );

	if( iInterestPathNodeCount != vTestPath.size() )
	{
		return false;
	}

	for( int i=0; i<iInterestPathNodeCount; ++i )
	{
		if( !(m_vInterestInPath[i] == vTestPath[i]) )
			return false;
	}

	return true;
	
}

bool CPaxFlowDlg::IsProcess( const ProcessorID& _testID )const
{
	CString sID = _testID.GetIDString();
	sID.MakeUpper();
	return m_pInTerm->m_pSubFlowList->IfProcessUnitExist( sID );
}

void CPaxFlowDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	OnSizeEx(cx,cy);
	Invalidate(FALSE);
}

BOOL CPaxFlowDlg::InitProcessCtrls()
{
	CWaitCursor wc;
	m_ProcTree.DeleteAllItems();
	m_ProcessTree.DeleteAllItems();
	m_vectID.clear();

	ProcessorList *procList = m_pInTerm->procList;
	if (procList == NULL)
		return TRUE;

	// Set data to processor tree
	ProcessorID id, *pid = NULL;
	id.SetStrDict( m_pInTerm->inStrDict );
	int includeBarriers = 0;
	CString strTemp, strTemp1, strTemp2, strTemp3, str;

	TVITEM ti;
	TVINSERTSTRUCT tis;
	HTREEITEM hItem = NULL, hItem1 = NULL, hItem2 = NULL, hItem3 = NULL;	
	int i = 0, j = 0;
	
	StringList strDict;
	procList->getAllGroupNames (strDict, m_nProcType);
	if (includeBarriers && !strDict.getCount())
		procList->getMemberNames (id, strDict, BarrierProc);

	if( m_bDisplayBaggageDeviceSign)
	{
		const Processor* pProcBaggageDevice =procList->getProcessor( BAGGAGE_DEVICE_PROCEOR_INDEX );
		m_vectID.push_back( *( pProcBaggageDevice->getID() ) );	
		CString sDisplay = " ";
		sDisplay += pProcBaggageDevice->getID()->GetIDString();
		m_ProcTree.SetItemData(m_ProcTree.InsertItem( sDisplay ), 0 );
	}
	
	for (j = 0; j < strDict.getCount(); j++)   // Level 1
	{
		strTemp = strDict.getString (j);
		strTemp.TrimLeft(); strTemp.TrimRight();
		if (strTemp.IsEmpty())
			continue;
		
		ti.mask       = TCIF_TEXT;
		ti.cChildren  = 1;
		ti.pszText    = strTemp.GetBuffer(0);
		ti.cchTextMax = 256;
		ti.iImage=0;
		tis.hParent = TVI_ROOT;
		tis.hInsertAfter = TVI_SORT;
		tis.item = ti;
			
		hItem = m_ProcTree.InsertItem(&tis);
		
		if (hItem == NULL)
			continue;
		
		str = strTemp;
		id.setID((LPCSTR) str);

		//Added by Xie Bo for debug
//		char buf[256];
//		id.printID(buf,"-");
//		AfxMessageBox(buf);


		m_vectID.push_back( id );
		m_ProcTree.SetItemData(hItem, (DWORD) ( m_vectID.size()-1 ) );
		
		StringList strDict1;
		procList->getMemberNames (id, strDict1, m_nProcType);
		for (int m = 0; m < strDict1.getCount(); m++)   // Level 2
		{
			strTemp1 = strDict1.getString (m);
			strTemp1.TrimLeft(); strTemp1.TrimRight();
			if (strTemp1.IsEmpty())
				continue;
			
			ti.pszText  = strTemp1.GetBuffer(0);
			tis.hParent = hItem;
			tis.item = ti;
			
			hItem1 = m_ProcTree.InsertItem(&tis);
			if (hItem1 == NULL)
				continue;
			
			str = strTemp;
			str += "-";
			str += strTemp1;

			id.setID((LPCSTR) str);

			m_vectID.push_back( id );
			m_ProcTree.SetItemData(hItem1, (DWORD) ( m_vectID.size()-1 ) );

			StringList strDict2;
			procList->getMemberNames (id, strDict2, m_nProcType);
			for (int n = 0; n < strDict2.getCount(); n++)   // Level 3
			{
				strTemp2 = strDict2.getString (n);
				strTemp2.TrimLeft(); strTemp2.TrimRight();
				if (strTemp2.IsEmpty())
					continue;
				
				ti.pszText  = strTemp2.GetBuffer(0);
				tis.hParent = hItem1;
				tis.item    = ti;
				
				hItem2 = m_ProcTree.InsertItem(&tis);
				if (hItem2 == NULL)
					continue;

				str = strTemp;   str += "-";
				str += strTemp1; str += "-";
				str += strTemp2;

				id.setID((LPCSTR) str);

				m_vectID.push_back( id );
				m_ProcTree.SetItemData(hItem2, (DWORD) ( m_vectID.size()-1 ) );

				StringList strDict3;
				procList->getMemberNames (id, strDict3, m_nProcType);
				for (int x = 0; x < strDict3.getCount(); x++)   // Level 4
				{
					strTemp3 = strDict3.getString (x);
					strTemp3.TrimLeft(); strTemp3.TrimRight();
					if (strTemp3.IsEmpty())
						continue;
					
					ti.pszText  = strTemp3.GetBuffer(0);
					tis.hParent = hItem2;
					tis.item    = ti;
					
					
					hItem3 = m_ProcTree.InsertItem(&tis);

					str = strTemp;   str += "-";
					str += strTemp1; str += "-";
					str += strTemp2; str += "-";
					str += strTemp3;
					
					id.setID((LPCSTR) str);

					m_vectID.push_back( id );
					m_ProcTree.SetItemData(hItem3, (DWORD) ( m_vectID.size()-1 ) );
				}
			}
		}
	}
	int iProcessCount = m_pInTerm->m_pSubFlowList->GetProcessUnitCount();
	for(i=0; i<iProcessCount; ++i )
	{
		CSubFlow* pFlow =  m_pInTerm->m_pSubFlowList->GetProcessUnitAt( i );
		ASSERT( pFlow  );
		m_ProcessTree.InsertItem( pFlow->GetProcessUnitName(),1,1);
	}
	return TRUE;
}

void CPaxFlowDlg::SetType(int _nProcType)
{
	m_nProcType = _nProcType;
	
}

void CPaxFlowDlg::OnSelchangedTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hTI=m_ProcessTree.GetSelectedItem();
	if(hTI)
	{
		m_ProcessTree.SetItemState(hTI,0,TVIS_SELECTED);
	}
	
	m_nDragedProcess=0;
	
	if(m_FlowTree.GetSelectedItem())
		UpdateFlowTreeToolBarBtn(TVHT_ONITEMLABEL,m_FlowTree.GetSelectedItem());


	
	*pResult = 0;
}

BOOL CPaxFlowDlg::GetProcessorID(ProcessorID &_ID)
{

	switch(m_nDragedProcess)
	{
	case 0://Add By Clicking ProcessorTree
		{

			HTREEITEM hItem = m_ProcTree.GetSelectedItem();
			if(hItem)
			{
				m_nCurSel = m_ProcTree.GetItemData(hItem);
				if( m_nCurSel >= 0 )
				{
					_ID = m_vectID[m_nCurSel];   
					return TRUE;
				}
			}
				
		}
		break;
	case 1://Add By Clicking ProcessTree
		{
			HTREEITEM hItem = m_ProcessTree.GetSelectedItem();
			if(hItem)
			{
				m_sSelectedProcess=m_ProcessTree.GetItemText(hItem);
				_ID.SetStrDict( m_pInTerm->inStrDict );
				_ID.setID( m_sSelectedProcess );
				return TRUE;
			}
			
		}
		break;
	case 2://Add By Dragging ProcessorTree
		{
			if( m_nCurSel >= 0 )
			{
				_ID = m_vectID[m_nCurSel];   
				return TRUE;
			}
		}
		break;
	case 3://Add By Dragging ProcessTree
		{
			_ID.SetStrDict( m_pInTerm->inStrDict );
			_ID.setID( m_sSelectedProcess );
			return TRUE;
		}
		break;
	}
	
	return false;
}

void CPaxFlowDlg::EnableBtnFromTBar(CToolBar &pTB, UINT ID, BOOL bEnable)
{
	pTB.GetToolBarCtrl().EnableButton(ID,bEnable);
}

void CPaxFlowDlg::OnBegindragTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	//////////////////////////////////////////////////////////////////
	m_hItemDragSrc = pNMTreeView->itemNew.hItem;
	m_hItemDragDes = NULL;
	
	if(m_pDragImage)
		delete m_pDragImage;
	
	m_pDragImage = m_ProcTree.CreateDragImage( m_hItemDragSrc);
	if( !m_pDragImage )
		return;
	
	m_bDragging = true;
	m_pDragImage->BeginDrag ( 0,CPoint(8,8) );
	CPoint  pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter ( GetDesktopWindow (),pt );  

	m_nCurSel = m_ProcTree.GetItemData(m_hItemDragSrc);


	m_nDragedProcess=2;
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	SetCapture();

/*
	UINT iRet;
	HTREEITEM hItem = m_ProcTree.HitTest(pNMTreeView->ptDrag, &iRet);
	if(hItem)
	{
		m_ProcTree.SetItemState(hItem,TVIS_SELECTED,TVIS_SELECTED);
		UpdateFlowTreeToolBarBtn(iRet,hItem);
	}*/


}

void CPaxFlowDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	HTREEITEM  hItem;
	UINT       flags;

	if( m_bDragging )
	{
		CPoint pt (point);
		ClientToScreen (&pt);

		// move the drag image
		VERIFY (m_pDragImage->DragMove (pt));
		// unlock window updates
		m_pDragImage->DragShowNolock (FALSE);
		//////////////////////////////////////////////////////
		CRect rectProcessTree;
		m_FlowTree.GetWindowRect( &rectProcessTree );
		//ClientToScreen( &rectProcessTree );
		if( rectProcessTree.PtInRect( pt) )	
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			CPoint ptTest( point );
			ClientToScreen(&ptTest);
			m_FlowTree.ScreenToClient(&ptTest);
			
			// TRACE("x = %d, y = %d \r\n", ptTest.x,ptTest.y );
			hItem = m_FlowTree.HitTest( ptTest,&flags);
			if( hItem != NULL )
			{
				// TRACE( "ENTERY" );
				m_FlowTree.SelectDropTarget( hItem );
				m_hItemDragDes = hItem;
			}
			else
			{
				m_FlowTree.SelectDropTarget( NULL );
				m_hItemDragDes = NULL;
			}
		}
		else
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		}
		m_pDragImage->DragShowNolock (TRUE);
		m_ProcTree.Invalidate(FALSE);
		m_ProcessTree.Invalidate(FALSE);
	}
	
	
	CDialog::OnMouseMove(nFlags, point);
}

void CPaxFlowDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_bDragging )
	{
		CPoint pt (point);
		ClientToScreen (&pt);
		PopProcessDefineMenu( pt );
	
		m_bDragging = false;
		ReleaseCapture();
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		// end dragging
		m_pDragImage->DragLeave (GetDesktopWindow ());
		m_pDragImage->EndDrag ();
		delete m_pDragImage;
		m_pDragImage = NULL;
	}	
	CDialog::OnLButtonUp(nFlags, point);
}

void CPaxFlowDlg::PopProcessDefineMenu(CPoint point)
{
	
	if( m_iListSelect <0 )
	{
		return;
	}

	CRect rcFlowTree;
	GetDlgItem(IDC_TREE_FLOW)->GetWindowRect(&rcFlowTree);
	if (!rcFlowTree.PtInRect(point)) 
		return;
	
	m_FlowTree.SetFocus();   // Set focus
	m_bUpTree = TRUE;
	
	CPoint pt = point;
	m_FlowTree.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hItem = m_FlowTree.HitTest(pt, &iRet);
	if( hItem == NULL )
		return;
	
	CMenu menuPopup;
	menuPopup.LoadMenu( IDR_MENU_POPUP );
	CMenu* pMenu = menuPopup.GetSubMenu( 2 );
	int nRemoveMenuItemCount=0;
	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( hItem );
	ASSERT( pData );
	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
	if(!(pData->m_procId==*pProcStart->getID()))
	{
		CMenu* pMenuPipes;
		pMenuPipes=pMenu->GetSubMenu(11);
		if(pData->m_arcInfo->GetTypeOfUsingPipe()==0)
		{
			pMenuPipes->ModifyMenu(ID_PAXFLOW_EDIT_PIPE_AUTO,
				MF_BYCOMMAND|MF_STRING,ID_PAXFLOW_EDIT_PIPE_AUTO,"Enable Automatic Pipes");
		}
		else
		{
			pMenuPipes->ModifyMenu(ID_PAXFLOW_EDIT_PIPE_AUTO,
				MF_BYCOMMAND|MF_STRING,ID_PAXFLOW_EDIT_PIPE_AUTO,"Disable Automatic Pipes");
		}
	}
	


	pMenu->EnableMenuItem( ID_PAXFLOW_ADDCHILDPROCESSOR, MF_GRAYED );

	pMenu->EnableMenuItem( ID_PAXFLOW_MODIFYPERCENT, MF_GRAYED );
	pMenu->EnableMenuItem( ID_PAXFLOW_DELPERCENT, MF_GRAYED );

	//pMenu->EnableMenuItem( ID_PAXFLOW_DELETEPROCESSORS, MF_GRAYED );
	pMenu->RemoveMenu(ID_PAXFLOW_DELETEPROCESSORS,MF_BYCOMMAND);
	nRemoveMenuItemCount++;

	pMenu->EnableMenuItem( ID_PAXFLOW_CUT_SOURCE_LINK, MF_GRAYED );

	pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER, MF_ENABLED );

	pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER_BYALL, MF_GRAYED );

	pMenu->EnableMenuItem( ID_PAXFLOW_EDIT_PIPE, MF_GRAYED );
	
	pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_AFTER, MF_GRAYED );
	
	//pMenu->EnableMenuItem( ID_PAXFLOW_NODE, MF_GRAYED );
	pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_BETTWEEN, MF_GRAYED );
	
	pMenu->EnableMenuItem( ID_PAXFLOW_ONETOONE, MF_GRAYED );
	
	pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_TOGATE, MF_GRAYED );
	
	pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_END, MF_GRAYED );
	
	pMenu->EnableMenuItem( ID_PAXFLOW_EVEN_PERCENT,  MF_GRAYED );
	
	pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_FROMGATE, MF_GRAYED );

	pMenu->EnableMenuItem(ID_PAXFLOW_COPYPROCESSOR,MF_GRAYED);
	pMenu->EnableMenuItem(ID_PAXFLOW_ADDCOPY,MF_GRAYED);
	pMenu->EnableMenuItem(ID_PAXFLOW_INSERTCOPY,MF_GRAYED);
	
	
	if( m_OperatedPaxFlow.GetFlowPairCount() > 0 )
	{
		pMenu->RemoveMenu(ID_PAXFLOW_ADD_ISOLATE_NODE,MF_BYCOMMAND);
		nRemoveMenuItemCount++;
	}
	else
	{
		pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER, MF_GRAYED );
	}
	
	const Processor* pProcEnd = GetInputTerminal()->procList->getProcessor( END_PROCESSOR_INDEX );
//	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
	const Processor* pFromGate = GetInputTerminal()->procList->getProcessor( FROM_GATE_PROCESS_INDEX );
	const Processor* pToGate = GetInputTerminal()->procList->getProcessor( TO_GATE_PROCESS_INDEX );
	
	if( m_OperatedPaxFlow.IfExist( *(pProcStart->getID())) )
	{
		pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_START, MF_GRAYED );
	}

	
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
	{
		//pMenu->EnableMenuItem( ID_PAXFLOW_NODE, MF_ENABLED );
	}
	else 
	{	
		m_hRClickItem = hItem;
		TREEITEMDATA* pItemData = (TREEITEMDATA*)m_FlowTree.GetItemData( hItem );
		if( !pItemData )
			return;
		HTREEITEM hFather = m_FlowTree.GetParentItem( m_hRClickItem );
		HTREEITEM hChild = m_FlowTree.GetChildItem( m_hRClickItem );

		if (!m_vCopyItem.empty())
		{
			pMenu->EnableMenuItem(ID_PAXFLOW_COPYPROCESSOR,MF_ENABLED);
		}
		if (!m_vCopyProcessors.empty())
		{
			pMenu->EnableMenuItem(ID_PAXFLOW_ADDCOPY,MF_ENABLED);
			pMenu->EnableMenuItem(ID_PAXFLOW_INSERTCOPY,MF_ENABLED);
		}


		TREEITEMDATA* pFatherItemData = NULL;
		if( hFather )
			 pFatherItemData = (TREEITEMDATA*)m_FlowTree.GetItemData( hFather );
		
		if( *(pProcEnd->getID()) == pItemData->m_procId  )// " END "
		{	
			if( hFather )
			{
				if( ! ( pFatherItemData->m_procId == *pToGate->getID() ) )
				{
					pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_BETTWEEN, MF_ENABLED );
				}
				
			}
		}
		else 
		{
			pMenu->EnableMenuItem( ID_PAXFLOW_CUT_SOURCE_LINK, MF_ENABLED );
			if( hFather )
			{
				pMenu->EnableMenuItem( ID_PAXFLOW_ADDCHILDPROCESSOR, MF_ENABLED );

				pMenu->EnableMenuItem( ID_PAXFLOW_MODIFYPERCENT, MF_ENABLED );

				pMenu->EnableMenuItem( ID_PAXFLOW_DELPERCENT, MF_ENABLED );
				
				pMenu->EnableMenuItem( ID_PAXFLOW_EDIT_PIPE, MF_ENABLED );
				
				pMenu->EnableMenuItem( ID_PAXFLOW_EVEN_PERCENT,  MF_ENABLED );
	            if( pFatherItemData->m_procId == *pProcStart->getID() ) 
				{
					pMenu->EnableMenuItem( ID_PAXFLOW_ONETOONE, MF_GRAYED );
				}
				else
				{
					pMenu->EnableMenuItem( ID_PAXFLOW_ONETOONE, MF_ENABLED );				
				}
				
				
				HTREEITEM hGrandFather = m_FlowTree.GetParentItem( hFather );
				if( hGrandFather )
				{
					if( !IsStation( pItemData->m_procId ) )
					{
						pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER, MF_ENABLED );

						pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER_BYALL, MF_ENABLED );
					}
				}	
				
				if(  (IsStation( pFatherItemData->m_procId ) && IsStation( pItemData->m_procId ) ) )
				{
					pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER, MF_GRAYED );

					pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER_BYALL, MF_GRAYED );
					
					pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_BETTWEEN, MF_GRAYED );
					
					pMenu->EnableMenuItem( ID_PAXFLOW_EDIT_PIPE, MF_GRAYED );
					
					
				}
				else
				{
					pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_BETTWEEN, MF_ENABLED );
					
				}
				
			}
			else
			{
				pMenu->EnableMenuItem( ID_PAXFLOW_ADDCHILDPROCESSOR, MF_ENABLED );

				pMenu->EnableMenuItem( ID_PAXFLOW_MODIFYPERCENT, MF_GRAYED );
				pMenu->EnableMenuItem( ID_PAXFLOW_DELPERCENT, MF_GRAYED );			
				pMenu->EnableMenuItem( ID_PAXFLOW_EDIT_PIPE, MF_GRAYED );
			}
		}

		if( !hChild ||  IsStation( pItemData->m_procId ) )
		{
			if( !hChild)
			{
				pMenu->EnableMenuItem( ID_PAXFLOW_CUT_SOURCE_LINK, MF_ENABLED );
			}
			else
			{
				pMenu->EnableMenuItem( ID_PAXFLOW_CUT_SOURCE_LINK, MF_GRAYED );
			}
		}
		else
		{			
			pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_AFTER, MF_ENABLED );
		}

		if( 1 == pItemData->m_iSpecialHandleType )
		{
			pMenu->EnableMenuItem( ID_PAXFLOW_ADDCHILDPROCESSOR, MF_GRAYED );
			
			pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER, MF_GRAYED );

			pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER_BYALL, MF_GRAYED );

			pMenu->EnableMenuItem( ID_PAXFLOW_EDIT_PIPE, MF_GRAYED );
			

		}

		pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_TOGATE, MF_GRAYED );	
		if( !hChild && !(pItemData->m_procId == *pProcEnd->getID()) )		
		{
			
			if( m_OperatedPaxFlow.GetPassengerConstrain()->isDeparture() || m_OperatedPaxFlow.GetPassengerConstrain()->isTurround() )
			{
				if( !IfHaveCircle( m_hRClickItem, *pToGate->getID() ) )// this brance don not has togate processor
				{
					pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_TOGATE, MF_ENABLED );	
				}
				
			}
			
			pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_END, MF_ENABLED );
		}
		

		if( *pProcStart->getID()  == pItemData->m_procId  )	// "START"
		{		
			pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_FROMGATE, MF_GRAYED );
			if( m_OperatedPaxFlow.GetPassengerConstrain()->isArrival() || m_OperatedPaxFlow.GetPassengerConstrain()->isTurround() )
			{
				if( !m_OperatedPaxFlow.IfExist( *pFromGate->getID() ) )	
				{
					pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_FROMGATE, MF_ENABLED );
				}
				else
				{
					pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_AFTER, MF_GRAYED );
				}
			}
			
		}
		else if( pItemData->m_procId == *pFromGate->getID() )
		{
			pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_BETTWEEN, MF_GRAYED );
			pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER, MF_ENABLED );
		}
		else if( pItemData->m_procId == *pToGate->getID() )
		{
			pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER, MF_ENABLED );
			pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_AFTER, MF_GRAYED );
			
		}
		
		m_FlowTree.SelectItem( m_hRClickItem );   // Select it
	}
	if(m_nDragedProcess==2||m_nDragedProcess==2)//Dragging
	{
		pMenu->AppendMenu(MF_SEPARATOR);
		//pMenu->AppendMenu(MF_STRING, ID_MENU_CANCEL, "Cancel");
	}
	BOOL bAddItem=IsEnableAddItem();
	//check if is station processor
	ProcessorID pprocID;
	BOOL bIsStation = ( GetProcessorID(pprocID) && IsStation( pprocID ) ) ? TRUE : FALSE;
	
	pMenu->EnableMenuItem( ID_PAXFLOW_ADDCHILDPROCESSOR, bAddItem?MF_ENABLED:MF_GRAYED );
	pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_BETTWEEN, (bAddItem && !bIsStation) ? MF_ENABLED : MF_GRAYED );
	pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_AFTER, (bAddItem && !bIsStation ) ? MF_ENABLED : MF_GRAYED );
	
	int nRemoveItemIndex=14-nRemoveMenuItemCount;
	pMenu->RemoveMenu(nRemoveItemIndex--,MF_BYPOSITION);
	nRemoveMenuItemCount++;
	pMenu->RemoveMenu(nRemoveItemIndex--,MF_BYPOSITION);
	nRemoveMenuItemCount++;
	pMenu->RemoveMenu(nRemoveItemIndex--,MF_BYPOSITION);
	nRemoveMenuItemCount++;
	pMenu->RemoveMenu(nRemoveItemIndex--,MF_BYPOSITION);
	nRemoveMenuItemCount++;
	pMenu->RemoveMenu(nRemoveItemIndex--,MF_BYPOSITION);
	nRemoveMenuItemCount++;
	
	CMenu* pMenuFlowCondition,* pMenuChannelCondition,*pMenuRouteCondition;
	// Flow condition
	pMenuFlowCondition=pMenu->GetSubMenu(15-nRemoveMenuItemCount);
	pMenuChannelCondition=pMenu->GetSubMenu(16-nRemoveMenuItemCount);
	pMenuRouteCondition=pMenu->GetSubMenu(17-nRemoveMenuItemCount);


	long _nPercent;
	CString strMenu;

	if(!(pData->m_procId==*pProcStart->getID())) 
	{
		_nPercent=pData->m_arcInfo->GetProbality();
		strMenu.Format("&1.  Percent split (%d%%)...",_nPercent);
		pMenuFlowCondition->ModifyMenu(0,MF_BYPOSITION|MF_STRING,ID_PAXFLOW_FLOWCONDITION_PERCENTSPLIT,strMenu);

		_nPercent=pData->m_arcInfo->GetMaxQueueLength();
		if(_nPercent==0)
		{
			strMenu="&2.  Queue length ...";
		}
		else
		{
			strMenu.Format("&2.  Queue length (<=%d)...",_nPercent);
			pMenuFlowCondition->CheckMenuItem(2,MF_BYPOSITION|MF_CHECKED);
		}
		pMenuFlowCondition->ModifyMenu(2,MF_BYPOSITION|MF_STRING,ID_PAXFLOW_FLOWCONDITION_QUEJULENGTHXX,strMenu);
		pMenuFlowCondition->CheckMenuItem(2,MF_BYPOSITION|(_nPercent==0?MF_UNCHECKED:MF_CHECKED));
		
		_nPercent=pData->m_arcInfo->GetMaxWaitMins();
		if(_nPercent==0)
		{
			strMenu="&3.  Wait time ...";
		}
		else
		{
			strMenu.Format("&3.  Wait time (<=%d mins)...",_nPercent);
		}
		pMenuFlowCondition->ModifyMenu(3,MF_BYPOSITION|MF_STRING,ID_PAXFLOW_FLOWCONDITION_WAITTIMEXXMINS,strMenu);
		pMenuFlowCondition->CheckMenuItem(3,MF_BYPOSITION|(_nPercent==0?MF_UNCHECKED:MF_CHECKED));
        
		
		_nPercent=pData->m_arcInfo->GetMaxSkipTime();
		if(_nPercent==0)
		{
			strMenu="&4.  Skip When Time to STD Less Than";
		}
		else
		{
			strMenu.Format("&4.  Skip When Time to STD Less Than %d min",_nPercent);
		}
		pMenuFlowCondition->ModifyMenu(4,MF_BYPOSITION|MF_STRING,ID_PAXFLOW_FLOWCONDITION_SKIPWHENTIMELESSTHAN,strMenu);
		pMenuFlowCondition->CheckMenuItem(4,MF_BYPOSITION|(_nPercent==0?MF_UNCHECKED:MF_CHECKED));
		
		pMenuChannelCondition->CheckMenuRadioItem(ID_PAXFLOW_CHANNELCONDITION_11_ON,ID_PAXFLOW_CHANNELCONDITION_11_OFF,
			pData->m_arcInfo->GetOneToOneFlag()?ID_PAXFLOW_CHANNELCONDITION_11_ON:ID_PAXFLOW_CHANNELCONDITION_11_OFF,MF_BYCOMMAND);
		if (NonChannel != pData->m_arcInfo->GetOneXOneState())
		{
			pMenuChannelCondition->CheckMenuRadioItem(ID_PAXFLOW_CHANNELCONDITION_1X1_START,ID_PAXFLOW_CHANNELCONDITION_1X1_CLEAR,
		(pData->m_arcInfo->GetOneXOneState() == 1)?ID_PAXFLOW_CHANNELCONDITION_1X1_START:ID_PAXFLOW_CHANNELCONDITION_1X1_END,MF_BYCOMMAND);
		}
		else
		{
			pMenuChannelCondition->CheckMenuRadioItem(ID_PAXFLOW_CHANNELCONDITION_1X1_START,ID_PAXFLOW_CHANNELCONDITION_1X1_CLEAR,
			ID_PAXFLOW_CHANNELCONDITION_1X1_CLEAR ,MF_BYCOMMAND);			
		}
		
		// Update State
		pMenuRouteCondition->CheckMenuItem(ID_PAXFLOW_ROUTECONDITION_STRAIGHTLINE,MF_BYCOMMAND|(pData->m_arcInfo->GetTypeOfUsingPipe()==0?MF_CHECKED:MF_UNCHECKED));
		pMenuRouteCondition->CheckMenuItem(ID_PAXFLOW_ROUTECONDITION_AVOIDFIXEDQUEUES,MF_BYCOMMAND|(pData->m_arcInfo->GetAvoidFixQueue()==1?MF_CHECKED:MF_UNCHECKED));
		pMenuRouteCondition->CheckMenuItem(ID_PAXFLOW_ROUTECONDITION_AVOIDOVERFLOWQUEUES,MF_BYCOMMAND|(pData->m_arcInfo->GetAvoidOverFlowQueue()==1?MF_CHECKED:MF_UNCHECKED));
		pMenuRouteCondition->CheckMenuItem(ID_PAXFLOW_ROUTECONDITION_AVOIDDENSITY,MF_BYCOMMAND|(pData->m_arcInfo->GetDensityArea().size()==0?MF_UNCHECKED:MF_CHECKED));

		CMenu* pPipes=pMenuRouteCondition->GetSubMenu(1);
		pPipes->CheckMenuItem(0,MF_BYPOSITION|(pData->m_arcInfo->GetTypeOfUsingPipe()==2?MF_CHECKED:MF_UNCHECKED));
		pPipes->CheckMenuItem(1,MF_BYPOSITION|(pData->m_arcInfo->GetTypeOfUsingPipe()==1?MF_CHECKED:MF_UNCHECKED));
		//pMenuRouteCondition->EnableMenuItem(ID_PAXFLOW_ROUTECONDITION_AVOIDDENSITY,MF_GRAYED);
	}
	else
	{
		pMenu->EnableMenuItem( ID_PAXFLOW_MODIFYPERCENT, MF_GRAYED );		
		pMenu->EnableMenuItem( ID_PAXFLOW_DELPERCENT, MF_GRAYED );		

		pMenu->EnableMenuItem( ID_PAXFLOW_CUT_SOURCE_LINK, MF_GRAYED );

		pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER, MF_GRAYED );

		pMenu->EnableMenuItem( ID_PAXFLOW_TAKEOVER_BYALL, MF_GRAYED );

		pMenu->EnableMenuItem( ID_PAXFLOW_EDIT_PIPE, MF_GRAYED );
		
		pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_BETTWEEN, MF_GRAYED );
		
		pMenu->EnableMenuItem( ID_PAXFLOW_ONETOONE, MF_GRAYED );
		
		pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_TOGATE, MF_GRAYED );
		
		pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_END, MF_GRAYED );
		
		pMenu->EnableMenuItem( ID_PAXFLOW_EVEN_PERCENT,  MF_GRAYED );
		
		pMenu->EnableMenuItem( ID_PAXFLOW_INSERT_FROMGATE, MF_GRAYED );

		pMenu->EnableMenuItem( ID_PAXFLOW_FLOWCONDITION_PERCENTSPLIT , MF_GRAYED );

		pMenu->EnableMenuItem( ID_PAXFLOW_FLOWCONDITION_EVENREMAINING , MF_GRAYED );

		pMenu->EnableMenuItem( ID_PAXFLOW_FLOWCONDITION_QUEJULENGTHXX , MF_GRAYED );

		pMenu->EnableMenuItem( ID_PAXFLOW_FLOWCONDITION_WAITTIMEXXMINS , MF_GRAYED );

		pMenu->EnableMenuItem(ID_PAXFLOW_FLOWCONDITION_SKIPWHENTIMELESSTHAN,MF_GRAYED);

		pMenu->EnableMenuItem( ID_PAXFLOW_CHANNELCONDITION_1N , MF_GRAYED );

		pMenu->EnableMenuItem( ID_PAXFLOW_CHANNELCONDITION_11 , MF_GRAYED );

		pMenu->EnableMenuItem( ID_PAXFLOW_ROUTECONDITION_STRAIGHTLINE , MF_GRAYED );

		pMenu->EnableMenuItem( ID_PAXFLOW_ROUTECONDITION_PIPES_USERSELECT , MF_GRAYED );

		pMenu->EnableMenuItem( ID_PAXFLOW_ROUTECONDITION_PIPES_ENABLEAUTOMATIC , MF_GRAYED );

		pMenu->EnableMenuItem( ID_PAXFLOW_ROUTECONDITION_AVOIDFIXEDQUEUES , MF_GRAYED );

		pMenu->EnableMenuItem( ID_PAXFLOW_ROUTECONDITION_AVOIDOVERFLOWQUEUES , MF_GRAYED );

		pMenu->EnableMenuItem( ID_PAXFLOW_ROUTECONDITION_AVOIDDENSITY , MF_GRAYED );		
	}
	
	pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
}

void CPaxFlowDlg::UpdateFlowTreeToolBarBtn(UINT iRet,HTREEITEM hItem)
{
	

	if(hItem==NULL) return;
	DisableFlowTreeToolBarBtn();
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_TAKEOVER,TRUE);
	
	if( m_OperatedPaxFlow.GetFlowPairCount() > 0 )
	{
	}
	else
	{
		EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_TAKEOVER,FALSE);
	}
	const Processor* pProcEnd = GetInputTerminal()->procList->getProcessor( END_PROCESSOR_INDEX );
	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
	const Processor* pFromGate = GetInputTerminal()->procList->getProcessor( FROM_GATE_PROCESS_INDEX );
	const Processor* pToGate = GetInputTerminal()->procList->getProcessor( TO_GATE_PROCESS_INDEX );
	
	
	if( m_OperatedPaxFlow.IfExist( *(pProcStart->getID())) )
	{
		EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_START,FALSE);
	}

	
	if (iRet == TVHT_ONITEMLABEL)   // Must click on the label
	{	
		m_hRClickItem = hItem;
		TREEITEMDATA* pItemData = (TREEITEMDATA*)m_FlowTree.GetItemData( hItem );
		if( !pItemData )
		{
			DisableFlowTreeToolBarBtn();
			return;
		}
		HTREEITEM hFather = m_FlowTree.GetParentItem( m_hRClickItem );
		HTREEITEM hChild = m_FlowTree.GetChildItem( m_hRClickItem );

		TREEITEMDATA* pFatherItemData = NULL;
		if( hFather )
			 pFatherItemData = (TREEITEMDATA*)m_FlowTree.GetItemData( hFather );
		if( *(pProcEnd->getID()) == pItemData->m_procId  )// " END "
		{	
			if( hFather )
			{
				if( ! ( pFatherItemData->m_procId == *pToGate->getID() ) )
				{
					EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_BETTWEEN,TRUE);
				}
				
			}
		}
		else 
		{
			EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_CUT_SOURCE_LINK,TRUE);
			if( hFather )
			{
				EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_ADDCHILDPROCESSOR,TRUE);
				
				EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_MODIFYPERCENT,TRUE);

				EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_DELPERCENT,TRUE);
				
				EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_EDIT_PIPE,TRUE);
				EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_EDIT_PIPE_AUTO,TRUE);
				
				EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_EVEN_PERCENT,TRUE);

				EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_1X1START,TRUE);
				EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_1X1END,TRUE);
				
	            if( pFatherItemData->m_procId == *pProcStart->getID() ) 
				{
					EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_ONETOONE,FALSE);
				}
				else
				{
					EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_ONETOONE,TRUE);
				}

				const CMobileElemConstraint* pConstraint = m_OperatedPaxFlow.GetPassengerConstrain();
				if( pConstraint->GetTypeIndex() > 0 )// visitor
				{			
					if (pItemData->m_arcInfo->GetTypeOfOwnership() == 0 )  	// 0 mine; 1 inherited; 2 combined		
					{
						EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_FOLLOWSTATE,TRUE);					
					}
					else
					{
						EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_FOLLOWSTATE,FALSE);					
					}
				}
				else
				{
					EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_FOLLOWSTATE,FALSE);
				}
				
				HTREEITEM hGrandFather = m_FlowTree.GetParentItem( hFather );
				if( hGrandFather )
				{
					if( !IsStation( pItemData->m_procId ) )
					{
						EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_TAKEOVER,TRUE);

						EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_TAKEOVER_BYALL,TRUE);
					}
				}	
				
				if(  (IsStation( pFatherItemData->m_procId ) && IsStation( pItemData->m_procId ) ) )
				{
					EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_TAKEOVER,FALSE);

					EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_TAKEOVER_BYALL,FALSE);
					
					EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_BETTWEEN,FALSE);
					
					EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_EDIT_PIPE,FALSE);
					EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_EDIT_PIPE_AUTO,FALSE);
					
					
				}
				else
				{
					EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_BETTWEEN,TRUE);
				}

				if (pItemData->m_arcInfo->GetTypeOfOwnership() == 0)
					EnableBtnFromTBar(m_ToolBarFlowTree, ID_PAXFLOW_SYNCPOINT, TRUE);
				else
					EnableBtnFromTBar(m_ToolBarFlowTree, ID_PAXFLOW_SYNCPOINT, FALSE);
			}
			else
			{
				EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_ADDCHILDPROCESSOR,TRUE);
				EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_MODIFYPERCENT,FALSE);
				EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_DELPERCENT,FALSE);
				EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_EDIT_PIPE,FALSE);
				EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_EDIT_PIPE_AUTO,FALSE);
			}
		}

		if( !hChild ||  IsStation( pItemData->m_procId ) )
		{
			if( !hChild)
			{
				EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_CUT_SOURCE_LINK,TRUE);
			}
			else
			{
				EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_CUT_SOURCE_LINK,FALSE);
			}
		}
		else
		{			
			EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_AFTER,TRUE);
		}

		if( 1 == pItemData->m_iSpecialHandleType )
		{
			EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_ADDCHILDPROCESSOR,FALSE);
			
			EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_TAKEOVER,FALSE);

			EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_TAKEOVER_BYALL,FALSE);

// 			EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_EDIT_PIPE,FALSE);
// 			EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_EDIT_PIPE_AUTO,FALSE);
			

		}

		EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_TOGATE,FALSE);
		if( !hChild && !(pItemData->m_procId == *pProcEnd->getID()) )		
		{
			
			if( m_OperatedPaxFlow.GetPassengerConstrain()->isDeparture() || m_OperatedPaxFlow.GetPassengerConstrain()->isTurround() )
			{
				if( !IfHaveCircle( m_hRClickItem, *pToGate->getID() ) )// this brance don not has togate processor
				{
					EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_TOGATE,TRUE);
				}
				
			}
			
			EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_END,TRUE);
		}
		

		if( *pProcStart->getID()  == pItemData->m_procId  )	// "START"
		{		
			EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_FROMGATE,FALSE);
			if( m_OperatedPaxFlow.GetPassengerConstrain()->isArrival() || m_OperatedPaxFlow.GetPassengerConstrain()->isTurround() )
			{
				if( !m_OperatedPaxFlow.IfExist( *pFromGate->getID() ) )	
				{
					EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_FROMGATE,TRUE);
				}
				else
				{
					EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_AFTER,FALSE);
				}
			}
			
		}
		else if( pItemData->m_procId == *pFromGate->getID() )
		{
			EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_BETTWEEN,FALSE);
			EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_TAKEOVER,TRUE);
		}
		else if( pItemData->m_procId == *pToGate->getID() )
		{
			EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_TAKEOVER,TRUE);
			EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_AFTER,FALSE);
		}
	
	}
	BOOL bAddItem=IsEnableAddItem();
	EnableBtnFromTBar( m_ToolBarFlowTree,ID_PAXFLOW_ADDCHILDPROCESSOR, bAddItem?TRUE:FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree, ID_PAXFLOW_INSERT_BETTWEEN,  bAddItem?TRUE:FALSE );
	EnableBtnFromTBar( m_ToolBarFlowTree,ID_PAXFLOW_INSERT_AFTER,  bAddItem?TRUE:FALSE );
	
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_EDIT_CONVEYOR,m_FlowTree.GetSelectedItem()!=NULL);
	
}

void CPaxFlowDlg::DisableFlowTreeToolBarBtn()
{
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_ADDCHILDPROCESSOR,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_MODIFYPERCENT,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_DELPERCENT,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_CUT_SOURCE_LINK,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_TAKEOVER,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_TAKEOVER_BYALL,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_EDIT_PIPE,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_EDIT_PIPE_AUTO,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_AFTER,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_BETTWEEN,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_ONETOONE,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_SYNCPOINT,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_1X1START,FALSE);	
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_1X1END,FALSE);	
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_TOGATE,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_END,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_START,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_EVEN_PERCENT,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_INSERT_FROMGATE,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_EDIT_CONVEYOR,FALSE);
	EnableBtnFromTBar(m_ToolBarFlowTree,ID_PAXFLOW_FOLLOWSTATE,FALSE);
}

void CPaxFlowDlg::OnSelchangedProcessTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_hRClickItem = pNMTreeView->itemNew.hItem;
	HTREEITEM hTI = m_hRClickItem;
	if(hTI)
	{
		//UINT nState=m_ProcTree.GetItemState(hTI,TVIS_SELECTED);
		m_ProcTree.SetItemState(hTI,0,TVIS_SELECTED);
	}
	m_nDragedProcess=1;

	if(m_FlowTree.GetSelectedItem())
		UpdateFlowTreeToolBarBtn(TVHT_ONITEMLABEL,m_FlowTree.GetSelectedItem());
	*pResult = 0;
}

void CPaxFlowDlg::OnSetfocusProcessTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	HTREEITEM hTI=m_ProcessTree.GetSelectedItem();
	if(hTI)
	{
		m_ProcessTree.SetItemState(hTI,TVIS_SELECTED,TVIS_SELECTED);
		hTI=m_ProcTree.GetSelectedItem();
		if(hTI)
		{
			//UINT nState=m_ProcTree.GetItemState(hTI,TVIS_SELECTED);
			m_ProcTree.SetItemState(hTI,0,TVIS_SELECTED);
		}
	}
	*pResult = 0;
}

void CPaxFlowDlg::OnSetfocusTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	HTREEITEM hTI=m_ProcTree.GetSelectedItem();
	if(hTI)
	{
		m_ProcTree.SetItemState(hTI,TVIS_SELECTED,TVIS_SELECTED);
		hTI=m_ProcessTree.GetSelectedItem();
		if(hTI)
		{
			//UINT nState=m_ProcTree.GetItemState(hTI,TVIS_SELECTED);
			m_ProcessTree.SetItemState(hTI,0,TVIS_SELECTED);
		}
	}
	*pResult = 0;
}

void CPaxFlowDlg::OnBegindragProcessTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	//////////////////////////////////////////////////////////////////
	m_hItemDragSrc = pNMTreeView->itemNew.hItem;
	m_hItemDragDes = NULL;
	
	if(m_pDragImage)
		delete m_pDragImage;
	
	m_pDragImage = m_ProcessTree.CreateDragImage( m_hItemDragSrc);
	if( !m_pDragImage )
		return;
	
	m_bDragging = true;
	m_pDragImage->BeginDrag ( 0,CPoint(8,8) );
	CPoint  pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter ( GetDesktopWindow (),pt );  
	m_sSelectedProcess=m_ProcessTree.GetItemText(m_hItemDragSrc);
	m_nDragedProcess=3;
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	SetCapture();
}

BOOL CPaxFlowDlg::IsEnableAddItem()
{
	if( m_nDragedProcess==-1 )
		return FALSE;
	else
	{
		
		return TRUE;
	}
}

void CPaxFlowDlg::AdjustDragedProcess()
{
	if(m_nDragedProcess==2)
	{
		HTREEITEM hItem=m_ProcTree.GetSelectedItem();
		if(hItem)
			m_nDragedProcess=0;
		else
		{
			hItem=m_ProcessTree.GetSelectedItem();
			if(hItem)
				m_nDragedProcess=1;
		}
		
	}
	if(m_nDragedProcess==3)
	{
		HTREEITEM hItem=m_ProcessTree.GetSelectedItem();
		if(hItem)
			m_nDragedProcess=1;
		else
		{
			hItem=m_ProcTree.GetSelectedItem();
			if(hItem)
				m_nDragedProcess=0;
		}
	}
	if(m_nDragedProcess==2||m_nDragedProcess==3)
		m_nDragedProcess=-1;
}

void CPaxFlowDlg::ResizeToHideRightWnd()
{
	CRect rectBtn,rectWnd;
	m_butAdvance.GetWindowRect(rectBtn);
	ScreenToClient(rectBtn);

	GetWindowRect(rectWnd);
	SetWindowPos(NULL,0,0,rectBtn.right +20,rectWnd.Height(),SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
	ShowRightWnd(FALSE);
}

void CPaxFlowDlg::ShowRightWnd(BOOL bShow)
{
	m_bExpand=bShow;
	for(int i=0;i<m_listRightWnd.GetSize();i++)
	{
		m_listRightWnd[i]->ShowWindow(bShow?SW_SHOW:SW_HIDE);
	}
}

void CPaxFlowDlg::ResizeLeftWnd(CSize size,CSize sizeLast)
{
	CRect rectWnd;
	int nTop;
	int nSplitterWidth=9;
	int nLeft=nSplitterWidth;
	int nSubWidth,nSubHeight=48,nWidthCur,nHeightCur;
	if(m_bExpand)
	{
		nSubWidth=nSplitterWidth*4;
		nWidthCur=int((size.cx-nSubWidth)*2.5/13.0);
		
	}
	else
	{
		nSubWidth=nSplitterWidth*3;
		nWidthCur=int((size.cx-nSubWidth)*2.5/10.0);
	}
	//Processor Static
	nHeightCur=int((size.cy-nSubHeight)*7/10.0);
	m_staticProcessor.GetWindowRect(rectWnd);
	m_staticProcessor.SetWindowPos(NULL,nLeft,nSplitterWidth,nWidthCur,rectWnd.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
	m_staticProcessor.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	//Processor Tree
	nTop=rectWnd.bottom+2;
	m_ProcTree.SetWindowPos(NULL,nLeft,nTop,nWidthCur,nHeightCur,SWP_NOACTIVATE|SWP_NOZORDER);
	m_ProcTree.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	//Splitter1
	int nSplitter1Top=rectWnd.top+nSplitterWidth;
	if(!m_wndSplitter1.m_hWnd)//Create Splitter1
	{
		m_wndSplitter1.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_WND_SPLITTER1);
		m_wndSplitter1.SetStyle(SPS_HORIZONTAL);
	}


	nTop=rectWnd.bottom;
	nHeightCur=nSplitterWidth;
	m_wndSplitter1.SetWindowPos(NULL,nLeft,nTop,nWidthCur,nHeightCur,SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndSplitter1.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	//Process Static
	nTop=rectWnd.bottom;
	m_staticProcess.GetWindowRect(rectWnd);
	m_staticProcess.SetWindowPos(NULL,nLeft,nTop,nWidthCur,rectWnd.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
	m_staticProcess.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	
	int nSplitter1Bottom=size.cy-nSubHeight-rectWnd.Height()-2-nSplitterWidth;
	m_wndSplitter1.SetRange(nSplitter1Top,nSplitter1Bottom);
	//Process Tree
	nTop=rectWnd.bottom+2;
	nHeightCur=size.cy-nSubHeight-nTop;
	m_ProcessTree.SetWindowPos(NULL,nLeft,nTop,nWidthCur,nHeightCur,SWP_NOACTIVATE|SWP_NOZORDER);
	m_ProcessTree.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);

	nLeft=rectWnd.left;
	nTop=rectWnd.bottom+nSplitterWidth;
	
	m_checkDisplayFlow.SetWindowPos(NULL,nLeft,nTop,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
}

void CPaxFlowDlg::ResizeCenterWnd(CSize size,CSize sizeLast)
{
	CRect rectWnd;
	int nTop;
	int nSplitterWidth=9;
	int nSubWidth,nSubHeight=48,nWidthCur,nHeightCur;
	m_ProcTree.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	int nLeft=rectWnd.right+nSplitterWidth;
	if(m_bExpand)
	{
		nSubWidth=nSplitterWidth*4;
		nWidthCur=int((size.cx-nSubWidth)*7.5/13.0);
		
	}
	else
	{
		nSubWidth=nSplitterWidth*3;
		nWidthCur=int((size.cx-nSubWidth)*7.5/10.0);
	}
	nHeightCur=int((size.cy-nSubHeight)*3/10.0);
	//MobileTag static
	m_staticMobileTag.GetWindowRect(rectWnd);
	m_staticMobileTag.SetWindowPos(NULL,nLeft,nSplitterWidth,nWidthCur,rectWnd.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
	m_staticMobileTag.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	//PaxType List
	nTop=rectWnd.bottom+2+26;
	m_listCtrlPaxType.SetWindowPos(NULL,nLeft,nTop,nWidthCur,nHeightCur,SWP_NOACTIVATE|SWP_NOZORDER);
	m_listCtrlPaxType.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	//Splitter3
	int nSplitter3Top=rectWnd.top+nSplitterWidth;
	if(!m_wndSplitter3.m_hWnd)//Create Splitter1
	{
		m_wndSplitter3.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_WND_SPLITTER3);
		m_wndSplitter3.SetStyle(SPS_HORIZONTAL);
	}


	nTop=rectWnd.bottom;
	nHeightCur=nSplitterWidth;
	m_wndSplitter3.SetWindowPos(NULL,nLeft,nTop,nWidthCur,nHeightCur,SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndSplitter3.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	//Flow Static
	nTop=rectWnd.bottom;
	m_staticFlowTree.GetWindowRect(rectWnd);
	m_staticFlowTree.SetWindowPos(NULL,nLeft,nTop,nWidthCur,rectWnd.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
	m_staticFlowTree.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);

	int nSplitter3Bottom=size.cy-nSubHeight-rectWnd.Height()-2-nSplitterWidth-26;
	m_wndSplitter3.SetRange(nSplitter3Top,nSplitter3Bottom);
	//Flow Tree
	nTop=rectWnd.bottom+2+26;
	nHeightCur=size.cy-nSubHeight-nTop;
	m_FlowTree.SetWindowPos(NULL,nLeft,nTop,nWidthCur,nHeightCur,SWP_NOACTIVATE|SWP_NOZORDER);
	m_FlowTree.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	//Splitter2-------------------------------------------------------------------------------------------
	if(!m_wndSplitter2.m_hWnd)
	{
		m_wndSplitter2.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_WND_SPLITTER2);
		m_wndSplitter2.SetStyle(SPS_VERTICAL);
	}
	m_wndSplitter2.SetWindowPos(NULL,rectWnd.left-nSplitterWidth,nSplitterWidth,nSplitterWidth,
		rectWnd.bottom-nSplitterWidth,SWP_NOACTIVATE|SWP_NOZORDER);
	int nSplitter2Left,nSplitter2Right;
	nSplitter2Left=nSplitterWidth*2;
	nSplitter2Right=rectWnd.right-nSplitterWidth;
	m_wndSplitter2.SetRange(nSplitter2Left,nSplitter2Right);
	//----------------------------------------------------------------------------------------------------
	//ToolBar
	CRect rectFT;
	m_FlowTree.GetWindowRect(rectFT);
	ScreenToClient(rectFT);
	m_ToolBarFlowTree.SetWindowPos(NULL,rectFT.left-1,rectFT.top-26	,rectFT.Width(),26,SWP_NOACTIVATE|SWP_NOZORDER);

	m_listCtrlPaxType.GetWindowRect(rectFT);
	ScreenToClient(rectFT);
	m_ToolBarMEType.SetWindowPos(NULL,rectFT.left-1,rectFT.top-26	,rectFT.Width(),26,SWP_NOACTIVATE|SWP_NOZORDER);

	

}

void CPaxFlowDlg::ResizeRightWnd(CSize size,CSize sizeLast)
{
	if(!m_bExpand) return;

	CRect rectWnd;
	int nTop;
	int nSplitterWidth=9;
	int nSubWidth,nSubHeight=48,nWidthCur,nHeightCur;
	m_FlowTree.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	int nLeft=rectWnd.right+nSplitterWidth;
	nSubWidth=nSplitterWidth*4;
	nWidthCur=int((size.cx-nSubWidth)*3/13.0)+2;
		
	nHeightCur=int((size.cy-nSubHeight)*3/10.0)+26;
	//FlightTitle Static
	m_staticFlightTitle.GetWindowRect(rectWnd);
	m_staticFlightTitle.SetWindowPos(NULL,nLeft,nSplitterWidth,nWidthCur,rectWnd.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
	m_staticFlightTitle.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	//Flight List
	nTop=rectWnd.bottom+2;
	m_listFlight.SetWindowPos(NULL,nLeft,nTop,nWidthCur,nHeightCur,SWP_NOACTIVATE|SWP_NOZORDER);
	m_listFlight.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	//Splitter5
	int nSplitter5Top=rectWnd.top+nSplitterWidth;
	if(!m_wndSplitter5.m_hWnd)//Create Splitter1
	{
		m_wndSplitter5.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_WND_SPLITTER5);
		m_wndSplitter5.SetStyle(SPS_HORIZONTAL);
		m_listRightWnd.Add(&m_wndSplitter5);
	}


	nTop=rectWnd.bottom;
	nHeightCur=nSplitterWidth;
	m_wndSplitter5.SetWindowPos(NULL,nLeft,nTop,nWidthCur,nHeightCur,SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndSplitter5.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);

	//GateTitle Static
	nTop=rectWnd.bottom;
	m_staticGateTitle.GetWindowRect(rectWnd);
	m_staticGateTitle.SetWindowPos(NULL,nLeft,nTop,nWidthCur,rectWnd.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
	m_staticGateTitle.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);

	int nSplitter5Bottom=size.cy-nSubHeight-rectWnd.Height()-2-nSplitterWidth;
	m_wndSplitter5.SetRange(nSplitter5Top,nSplitter5Bottom);
	//Gate Tree
	nTop=rectWnd.bottom+2;
	nHeightCur=size.cy-nSubHeight-nTop;
	m_treeGate.SetWindowPos(NULL,nLeft,nTop,nWidthCur,nHeightCur,SWP_NOACTIVATE|SWP_NOZORDER);
	m_treeGate.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	//Splitter4-------------------------------------------------------------------------------------------
	if(!m_wndSplitter4.m_hWnd)
	{
		m_wndSplitter4.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, ID_WND_SPLITTER4);
		m_wndSplitter4.SetStyle(SPS_VERTICAL);
		m_listRightWnd.Add(&m_wndSplitter4);
	}
	m_wndSplitter4.SetWindowPos(NULL,rectWnd.left-nSplitterWidth,nSplitterWidth,nSplitterWidth,
		rectWnd.bottom-nSplitterWidth,SWP_NOACTIVATE|SWP_NOZORDER);
	int nSplitter4Left,nSplitter4Right;
	
	
	nSplitter4Right=rectWnd.right-nSplitterWidth;
	m_butRestore.GetClientRect(rectWnd);
	nSplitter4Right-=rectWnd.Width();
	m_FlowTree.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	nSplitter4Left=rectWnd.left+nSplitterWidth;
	m_butAdvance.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	int nRightTemp=rectWnd.right;
	m_btnSave.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	int nLeftTemp=rectWnd.left;
	int nWidthTemp=nRightTemp-nLeftTemp+nSplitterWidth;
	if(nWidthTemp>nSplitter4Left)
		nSplitter4Left=nWidthTemp;
	m_wndSplitter4.SetRange(nSplitter4Left,nSplitter4Right);
	//----------------------------------------------------------------------------------------------------

}

void CPaxFlowDlg::ResizeBottomWnd(CSize size, CSize sizeLast)
{
	int nSubHeight=48;
	int nSplitterWidth=9;
	int nTop=size.cy-nSubHeight+nSplitterWidth,nLeft;
	CRect rectWnd,rectFlowTree;
	m_butAdvance.GetClientRect(rectWnd);
	m_FlowTree.GetWindowRect(rectFlowTree);
	ScreenToClient(rectFlowTree);
	nLeft=rectFlowTree.right-rectWnd.Width();
	for(int i=0;i<m_listLeftBtn.GetSize();i++)
	{
		m_listLeftBtn[i]->SetWindowPos(NULL,nLeft,nTop,0,0,SWP_NOSIZE
			|SWP_NOACTIVATE|SWP_NOZORDER);
		nLeft-=(4+rectWnd.Width());
	}
	m_treeGate.GetWindowRect(rectFlowTree);
	ScreenToClient(rectFlowTree);
	nLeft=rectFlowTree.right-rectWnd.Width();
	m_butRestore.SetWindowPos(NULL,nLeft,nTop,0,0,SWP_NOSIZE|
		SWP_NOACTIVATE|SWP_NOZORDER);
}

void CPaxFlowDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_bExpand)
		lpMMI->ptMinTrackSize.x =900;
	else
		lpMMI->ptMinTrackSize.x =600;
	lpMMI->ptMinTrackSize.y=400; 
	
	CDialog::OnGetMinMaxInfo(lpMMI);
}



BOOL CPaxFlowDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;
}

void CPaxFlowDlg::DoResize(int delta,UINT nIDSplitter)
{
	CRect rectWnd;
	switch(nIDSplitter)
	{
	case ID_WND_SPLITTER1:
		{
			CSplitterControl::ChangeHeight(&m_ProcTree, delta,CW_TOPALIGN);
			CSplitterControl::ChangeHeight(&m_ProcessTree,-delta,CW_BOTTOMALIGN);
			m_staticProcess.GetWindowRect(rectWnd);
			ScreenToClient(rectWnd);
			m_staticProcess.SetWindowPos(NULL,rectWnd.left,rectWnd.top+delta,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
		}
		break;
	case ID_WND_SPLITTER2:
		{
			CSplitterControl::ChangeWidth(&m_ProcTree, delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_ProcessTree,delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_staticProcessor, delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_staticProcess,delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_wndSplitter1,delta,CW_LEFTALIGN);

			CSplitterControl::ChangeWidth(&m_listCtrlPaxType, -delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_FlowTree,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_staticMobileTag,- delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_staticFlowTree,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_wndSplitter3,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_ToolBarFlowTree,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_ToolBarMEType,-delta,CW_RIGHTALIGN);


		}
		break;
	case ID_WND_SPLITTER3:
		{
			CSplitterControl::ChangeHeight(&m_listCtrlPaxType, delta,CW_TOPALIGN);
			CSplitterControl::ChangeHeight(&m_FlowTree,-delta,CW_BOTTOMALIGN);
			m_staticFlowTree.GetWindowRect(rectWnd);
			ScreenToClient(rectWnd);
			m_staticFlowTree.SetWindowPos(NULL,rectWnd.left,rectWnd.top+delta,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);

			m_ToolBarFlowTree.GetWindowRect(rectWnd);
			ScreenToClient(rectWnd);
			m_ToolBarFlowTree.SetWindowPos(NULL,rectWnd.left,rectWnd.top+delta,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
		}
		break;
	case ID_WND_SPLITTER4:
		{
			
			CSplitterControl::ChangeWidth(&m_listCtrlPaxType, delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_FlowTree,delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_staticMobileTag,delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_staticFlowTree,delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_wndSplitter3,delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_ToolBarFlowTree,delta,CW_LEFTALIGN);
			CSplitterControl::ChangeWidth(&m_ToolBarMEType,delta,CW_LEFTALIGN);
			for(int i=0;i<m_listLeftBtn.GetSize();i++)
			{
				m_listLeftBtn[i]->GetWindowRect(rectWnd);
				ScreenToClient(rectWnd);
				m_listLeftBtn[i]->SetWindowPos(NULL,rectWnd.left+delta,rectWnd.top,0,0,
					SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
			}
			CSplitterControl::ChangeWidth(&m_listFlight, -delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_treeGate,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_staticGateTitle, -delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_staticFlightTitle,-delta,CW_RIGHTALIGN);
			CSplitterControl::ChangeWidth(&m_wndSplitter5,-delta,CW_RIGHTALIGN);

		}
		break;
	case ID_WND_SPLITTER5:
		{
			CSplitterControl::ChangeHeight(&m_listFlight, delta,CW_TOPALIGN);
			CSplitterControl::ChangeHeight(&m_treeGate,-delta,CW_BOTTOMALIGN);
			m_staticGateTitle.GetWindowRect(rectWnd);
			ScreenToClient(rectWnd);
			m_staticGateTitle.SetWindowPos(NULL,rectWnd.left,rectWnd.top+delta,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
		}
		break;
	}
	
	Invalidate();
}

void CPaxFlowDlg::OnSizeEx(int cx, int cy)
{
	if(m_sizeLastWnd.cx!=0&&m_sizeLastWnd.cy!=0)
	{
		ResizeLeftWnd(CSize(cx,cy),m_sizeLastWnd);
		ResizeCenterWnd(CSize(cx,cy),m_sizeLastWnd);
		ResizeRightWnd(CSize(cx,cy),m_sizeLastWnd);
		ResizeBottomWnd(CSize(cx,cy),m_sizeLastWnd);
	}
	// TODO: Add your message handler code here
	m_sizeLastWnd=CSize(cx,cy);
}

BOOL CPaxFlowDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
    ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

    // if there is a top level routing frame then let it handle the message
    if (GetRoutingFrame() != NULL) return FALSE;

    // to be thorough we will need to handle UNICODE versions of the message also !!
    TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
    TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
    TCHAR szFullText[512];
    CString strTipText;
    UINT nID = pNMHDR->idFrom;

    if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
        pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
    {
        // idFrom is actually the HWND of the tool 
        nID = ::GetDlgCtrlID((HWND)nID);
    }

    if (nID != 0) // will be zero on a separator
    {
		if(nID==ID_PAXFLOW_EDIT_PIPE_AUTO)
		{
			HTREEITEM hItem=m_FlowTree.GetSelectedItem();
			if(hItem==NULL)
			{
				AfxLoadString(nID, szFullText);
			}
			else
			{
				TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData(hItem);
				ASSERT( pData );
				const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
				if(!(pData->m_procId==*pProcStart->getID()))
				{
					if(pData->m_arcInfo->GetTypeOfUsingPipe()==0)
					{
						strcpy(szFullText,"Enable Automatic Pipes");
					}
					else
					{
						strcpy(szFullText,"Disable Automatic Pipes");
					}
				}
				
			}
			
		}
		else
	        AfxLoadString(nID, szFullText);
        strTipText=szFullText;

#ifndef _UNICODE
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
        }
        else
        {
            _mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
        }
#else
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            _wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
        }
        else
        {
            lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
        }
#endif

        *pResult = 0;

        // bring the tooltip window above other popup windows
        ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
            SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
        
        return TRUE;
    }

    return FALSE;
}



void CPaxFlowDlg::OnPaxflowEditElevator() 
{
/*
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();
	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( m_hRClickItem );
	ASSERT( pData );
	
	CPaxFlowSelectPipes dlg( GetInputTerminal(), pData->m_arcInfo->GetPipeVector(), pData->m_arcInfo->GetTypeOfUsingPipe() );
	if( dlg.DoModal() == IDOK )
	{
		if( dlg.m_bSelectPipe )
		{
			int iTypeOfUsingPipe = dlg.m_vPipeIdx.size() <= 0 ? 0 : 2;
			pData->m_arcInfo->SetTypeOfUsingPipe( iTypeOfUsingPipe );
			pData->m_arcInfo->GetPipeVector() = dlg.m_vPipeIdx;
		}
		else
		{
			pData->m_arcInfo->SetTypeOfUsingPipe( 1 );
		}
		m_bChanged = true;
		m_btnSave.EnableWindow();
		m_OperatedPaxFlow.SetChangedFlag( true );
		
		BuildInterestInPath( m_hRClickItem );
		ReloadTree();
		
	}*/
/*
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();
	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( m_hRClickItem );
	ASSERT( pData );
	
	CPaxFlowSelectElevators dlg(GetInputTerminal(), 
		pData->m_arcInfo->GetPipeVector(), 
		pData->m_arcInfo->GetTypeOfUsingPipe() );
	if(dlg.DoModal()==IDOK)
	{
	}*/

	
}

void CPaxFlowDlg::PaxFlowEditPipeAuto(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( hItem );
	ASSERT( pData );
	const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
	if(pData->m_procId==*pProcStart->getID()) return;

	if(pData->m_arcInfo->GetTypeOfUsingPipe() == 1)
	{
		pData->m_arcInfo->SetTypeOfUsingPipe(0);
		pData->m_arcInfo->GetPipeVector().clear();

		TBBUTTONINFO tbBtnInfo;
		tbBtnInfo.cbSize=sizeof(TBBUTTONINFO);
		tbBtnInfo.dwMask=TBIF_IMAGE;
		tbBtnInfo.iImage=9;

		m_ToolBarFlowTree.GetToolBarCtrl().SetButtonInfo(ID_PAXFLOW_EDIT_PIPE_AUTO,&tbBtnInfo);

		return;
	}

	if(pData->m_arcInfo->GetTypeOfUsingPipe() == 2)
	{
		pData->m_arcInfo->SetTypeOfUsingPipe(0);
		pData->m_arcInfo->GetPipeVector().clear();		
	}		

	pData->m_arcInfo->SetTypeOfUsingPipe( 1 );


}

void CPaxFlowDlg::OnPaxflowEditPipeAuto() 
{

	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		PaxFlowEditPipeAuto(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			PaxFlowEditPipeAuto(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_bChanged = true;
	m_btnSave.EnableWindow();
	m_OperatedPaxFlow.SetChangedFlag( true );

	BuildInterestInPath( m_hRClickItem );
	
	if(m_hRClickItem!=NULL)
	{
		TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( m_hRClickItem );
		ASSERT( pData );
		const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
		if(pData->m_procId==*pProcStart->getID())
		{
			return;
		}

		TBBUTTONINFO tbBtnInfo;
		tbBtnInfo.cbSize=sizeof(TBBUTTONINFO);
		tbBtnInfo.dwMask=TBIF_IMAGE;
		if(pData->m_arcInfo->GetTypeOfUsingPipe()==1)
		{
			CImageList* pIL=m_ToolBarFlowTree.GetToolBarCtrl().GetImageList();

			tbBtnInfo.iImage=pIL->GetImageCount()-2;
		}	
		else
		{
			tbBtnInfo.iImage=9;
		}
		BOOL b=m_ToolBarFlowTree.GetToolBarCtrl().SetButtonInfo(ID_PAXFLOW_EDIT_PIPE_AUTO,&tbBtnInfo);
	}	
	ReloadTree();
}

LRESULT CPaxFlowDlg::OnPCTreeSelectProc(WPARAM wParam,LPARAM lParam)
{
	ProcessorID* pProcID=(ProcessorID*)wParam;
	HTREEITEM hItemSeled=m_hRClickItem;
	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( hItemSeled );
	ASSERT(pData);

	m_bChanged = true; 
	m_btnSave.EnableWindow();
				
	switch(lParam)
	{
	case 0:// Add Destination
		{
			CFlowDestination tempDestInfo;
			tempDestInfo.SetProcID( *pProcID );
			//// TRACE("%s, %s\n", pData->m_procId.GetIDString(), id.GetIDString() );
			m_OperatedPaxFlow.AddDestProc( pData->m_procId, tempDestInfo );
			m_OperatedPaxFlow.GetEqualFlowPairAt( pData->m_procId )->SetAllDestInherigeFlag( 0 );
			BuildInterestInPath( hItemSeled );			
		}
		break;
	case 1://Insert After
		{
			m_OperatedPaxFlow.InsertProceoosorAfter( pData->m_procId, *pProcID );
			BuildInterestInPath( hItemSeled );			
		}
		break;
	case 2://Insert Before
		{
			HTREEITEM hFather = m_FlowTree.GetParentItem( hItemSeled );	
			TREEITEMDATA* pFatherData = (TREEITEMDATA* )m_FlowTree.GetItemData( hFather );
			//	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( m_hRClickItem );
			
			ASSERT( pFatherData ); 
			CFlowDestination temp;
			temp.SetProcID( *pProcID );
			
//			// TRACE("%s, %s\n", pData->m_procId.GetIDString(), id.GetIDString() );
			m_OperatedPaxFlow.InsertBetwwen( pFatherData->m_procId, pData->m_procId, temp );
			
			
			BuildInterestInPath( hFather );
			m_vInterestInPath.insert(m_vInterestInPath.begin(), *pProcID );
			m_vInterestInPath.insert(m_vInterestInPath.begin(), pData->m_procId );
			
		}
		break;
	}

	
	
	//	m_OperatedPaxFlow.PrintAllStructure();
	ReloadTree();
	return TRUE;
}

void CPaxFlowDlg::SetViewPickConveyor(BOOL bActive3DView)
{
	m_ToolBarFlowTree.GetToolBarCtrl().CheckButton(ID_PAXFLOW_EDIT_CONVEYOR,m_bCheckConveyorBtn);
	// Open 3D View-------------------------------------------------------------------------------------
	if(!bActive3DView) return;

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	C3DView* p3DView = pDoc->Get3DView();
	if( p3DView == NULL )
	{
		pDoc->GetMainFrame()->CreateOrActive3DView();
		p3DView = pDoc->Get3DView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}
	p3DView->m_bPickConveyor=m_bCheckConveyorBtn;		
}

void CPaxFlowDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	m_bCheckConveyorBtn=FALSE;
	SetViewPickConveyor(FALSE);
	CDialog::OnClose();
}

void CPaxFlowDlg::OnPaxflowFlowconditionPercentsplit() 
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( m_hRClickItem );
	ASSERT( pData );
	int _nPercent;
	_nPercent=pData->m_arcInfo->GetProbality();
	
	CInputValueDlg dlg("Please input a value for percent split (%)",_nPercent,0,100);
	if(dlg.DoModal()==IDOK)
	{
		SetPercent( dlg.m_nValue);
		m_bChanged=true;
		m_btnSave.EnableWindow();
		m_OperatedPaxFlow.SetChangedFlag(true);
		
	}	
}

void CPaxFlowDlg::OnPaxflowFlowconditionEvenremaining() 
{
	OnPaxflowEvenPercent();
}

void CPaxFlowDlg::OnPaxflowFlowconditionQuejulengthxx() 
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( m_hRClickItem );
	ASSERT( pData );
	long _nPercent;
	_nPercent=pData->m_arcInfo->GetMaxQueueLength();
	if(_nPercent==0)
	{
		CInputValueDlg dlg("Please input a value for queue length (<=)",max(_nPercent,1),1,100);
		if(dlg.DoModal()==IDOK)
		{
			pData->m_arcInfo->SetMaxQueueLength(dlg.m_nValue);
			m_bChanged=true;
			m_btnSave.EnableWindow();
			m_OperatedPaxFlow.SetChangedFlag(true);
			
			BuildInterestInPath( m_hRClickItem );
			ReloadTree();
			
		}	
	}
	else
	{
		pData->m_arcInfo->SetMaxQueueLength(0);
		m_bChanged=true;
		m_btnSave.EnableWindow();
		m_OperatedPaxFlow.SetChangedFlag(true);
		
		BuildInterestInPath( m_hRClickItem );
		ReloadTree();
	}
}

void CPaxFlowDlg::OnPaxflowFlowconditionWaittimexxmins() 
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( m_hRClickItem );
	ASSERT( pData );
	long _nPercent;
	_nPercent=pData->m_arcInfo->GetMaxWaitMins();
	if(_nPercent==0)
	{
		CInputValueDlg dlg("Please input a value for wait time (<= mins)",max(_nPercent,1),1,60);
		if(dlg.DoModal()==IDOK)
		{
			pData->m_arcInfo->SetMaxWaitMins(dlg.m_nValue);
			m_bChanged=true;
			m_btnSave.EnableWindow();
			m_OperatedPaxFlow.SetChangedFlag(true);
			
			BuildInterestInPath( m_hRClickItem );
			ReloadTree();
			
		}	
	}
	else
	{
		pData->m_arcInfo->SetMaxWaitMins(0);
		m_bChanged=true;
		m_btnSave.EnableWindow();
		m_OperatedPaxFlow.SetChangedFlag(true);
		
		BuildInterestInPath( m_hRClickItem );
		ReloadTree();

	}	
}

void CPaxFlowDlg::OnPaxflowConditionSyncPoint()
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData(m_hRClickItem );
	ASSERT(pData);

	int nTypeIndex = m_OperatedPaxFlow.GetPassengerConstrain()->GetTypeIndex();
	if (nTypeIndex != 0)
		return;
	
	CInputFlowPaxSyncItem* pPaxFlowSyncItem = m_pInTerm->GetInputFlowSync()->GetFlowPaxSync(*m_OperatedPaxFlow.GetPassengerConstrain(),pData->m_arcInfo->GetProcID());
	if (pPaxFlowSyncItem)
	{
		m_pInTerm->GetInputFlowSync()->DeleteItem(pPaxFlowSyncItem);
	}
	else
	{
		CDlgSyncPointDefine dlg;
		if(dlg.DoModal() == IDOK)
		{
			pPaxFlowSyncItem = new CInputFlowPaxSyncItem();
			pPaxFlowSyncItem->SetSyncName(dlg.GetSyncName());
			pPaxFlowSyncItem->SetMobElementConstraint(*m_OperatedPaxFlow.GetPassengerConstrain());
			pPaxFlowSyncItem->SetProcID(pData->m_arcInfo->GetProcID());
			m_pInTerm->GetInputFlowSync()->AddItem(pPaxFlowSyncItem);
		}
	}
	m_bChanged = true;
	m_btnSave.EnableWindow();
	m_OperatedPaxFlow.SetChangedFlag(true);
	BuildInterestInPath(m_hRClickItem);
	ReloadTree();
}

void CPaxFlowDlg::OnPaxflowFlowconditonSkipWhenTimeLessThan() 
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( m_hRClickItem );
	ASSERT( pData );

	long _nPercent;
	_nPercent=pData->m_arcInfo->GetMaxSkipTime();
	if(_nPercent==0)
	{
		CInputValueDlg dlg(" Skip When Time to STD Less Than",max(_nPercent,0),0,10000);
		if(dlg.DoModal()==IDOK)
		{
			pData->m_arcInfo->SetMaxSkipTime(dlg.m_nValue);
			m_bChanged=true;
			m_btnSave.EnableWindow();
			m_OperatedPaxFlow.SetChangedFlag(true);
			
			BuildInterestInPath( m_hRClickItem );
			ReloadTree();
			
		}	
	}
	else
	{
		pData->m_arcInfo->SetMaxSkipTime(0);
		m_bChanged=true;
		m_btnSave.EnableWindow();
		m_OperatedPaxFlow.SetChangedFlag(true);
		
		BuildInterestInPath( m_hRClickItem );
		ReloadTree();

	}	
}


void CPaxFlowDlg::OnPaxflowRouteconditionPipesEnableautomatic() 
{
	OnPaxflowEditPipeAuto();

	
}

void CPaxFlowDlg::OnPaxflowRouteconditionPipesUserselect() 
{
	OnPaxflowEditPipe();
	
}

void CPaxFlowDlg::OnPaxflowRouteconditionStraightline() 
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( m_hRClickItem );
	ASSERT( pData );
	pData->m_arcInfo->SetTypeOfUsingPipe(0);
	pData->m_arcInfo->GetPipeVector().clear();
	m_bChanged=true;
	m_btnSave.EnableWindow();
	m_OperatedPaxFlow.SetChangedFlag(true);

	BuildInterestInPath( m_hRClickItem );
	ReloadTree();
	
}

void CPaxFlowDlg::OnPaxflowRouteconditionAvoidfixedqueues() 
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( m_hRClickItem );
	ASSERT( pData );
	pData->m_arcInfo->SetAvoidFixQueue(pData->m_arcInfo->GetAvoidFixQueue()==0?1:0);
	m_bChanged=true;
	m_btnSave.EnableWindow();
	m_OperatedPaxFlow.SetChangedFlag(true);	
	
	BuildInterestInPath( m_hRClickItem );
	ReloadTree();
		
}

void CPaxFlowDlg::OnPaxflowRouteconditionAvoidoverflowqueues() 
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( m_hRClickItem );
	ASSERT( pData );
	pData->m_arcInfo->SetAvoidOverFlowQueue(pData->m_arcInfo->GetAvoidOverFlowQueue()==0?1:0);
	m_bChanged=true;
	m_btnSave.EnableWindow();
	m_OperatedPaxFlow.SetChangedFlag(true);
	
	BuildInterestInPath( m_hRClickItem );
	ReloadTree();
	
}

void CPaxFlowDlg::OnPaxflowRouteconditionAvoiddensity() 
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( m_hRClickItem );
	ASSERT( pData );
	if(pData->m_arcInfo->GetDensityArea().size()==0)
	{
		CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
		
		CPaxFlowAvoidDensityDlg dlg(pDoc,&(pDoc->GetTerminal().m_pAreas->m_vAreas),pData->m_arcInfo,this);
		if(dlg.DoModal()==IDOK)
		{
			m_bChanged=true;
			m_btnSave.EnableWindow();
			m_OperatedPaxFlow.SetChangedFlag(true);
			
			BuildInterestInPath( m_hRClickItem );
			ReloadTree();			
		}
	}
	else
	{
		pData->m_arcInfo->GetDensityArea().clear();
		pData->m_arcInfo->SetDensityOfArea(0);

		m_bChanged=true;
		m_btnSave.EnableWindow();
		m_OperatedPaxFlow.SetChangedFlag(true);
		
		BuildInterestInPath( m_hRClickItem );
		ReloadTree();
	}	
}


void CPaxFlowDlg::OnClickListPaxtype(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_listCtrlPaxType.GetFirstSelectedItemPosition();
	if ( pos != NULL ) 
	{
		m_nSel = m_listCtrlPaxType.GetNextSelectedItem(pos);	
	}
	else
	{
		m_nSel = -1 ;  //haven't  select item
	}
	if (m_nSelChange != m_nSel)
	{
		m_nSelChange = m_nSel;
	}		
	OnSelchangeListPaxtype();	
	*pResult = 0;
}

void CPaxFlowDlg::OnItemchangedListPaxtype(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	POSITION pos = m_listCtrlPaxType.GetFirstSelectedItemPosition();
	if ( pos != NULL ) 
	{
		m_nSel = m_listCtrlPaxType.GetNextSelectedItem(pos);
		if (m_nSelChange != m_nSel)
		{
			m_nSelChange = m_nSel;
			OnSelchangeListPaxtype();
		}
	}		
	*pResult = 0;
}

//search the string in flowtree. 
//store the found Item in m_vFoundItem, the found Item means item's string as same as user want search.
void CPaxFlowDlg::SearchItemInTree( HTREEITEM _hItem,CString _strSearch ,BOOL _bMatchAll)
{
	HTREEITEM hChildItem;
	if (_hItem == NULL) { return; }	
	TREEITEMDATA* pTempData = (TREEITEMDATA*)m_FlowTree.GetItemData(_hItem );
	CString  strProcId = pTempData->m_procId.GetIDString();
//	// TRACE(strProcId+'\n');
	BOOL bFindItem = FALSE ;

	strProcId.MakeLower();
	_strSearch.MakeLower();	
	if (_bMatchAll) //if user want search string watch whole word
	{
		if(strcmp(strProcId,_strSearch) == 0)	bFindItem = TRUE ;
	}
	else
	{
		if (strProcId.Find(_strSearch) != -1)	bFindItem = TRUE ;
	}
	if ( bFindItem)
	{
		m_vFoundItem.push_back(_hItem);
	}
    hChildItem = m_FlowTree.GetChildItem(_hItem); 
	if (hChildItem == NULL) return ;
	TREEITEMDATA* pTempData2 = (TREEITEMDATA*)m_FlowTree.GetItemData(hChildItem );
	CString  strProcId2 = pTempData2->m_procId.GetIDString();	
	SearchItemInTree( hChildItem, _strSearch );
	while (hChildItem = m_FlowTree.GetNextItem( hChildItem, TVGN_NEXT) )  
	{
		SearchItemInTree( hChildItem, _strSearch );
	}
	return;	
}

void CPaxFlowDlg::OnPaxflowFind() 
{
	CFindDlg dlg;
	CString strFind;
	dlg.m_strFind = m_strFind; 
	dlg.m_bMatchWholeWord = m_bMatchWholeWord;
	dlg.m_vStrBuff=m_vStrBuff;
	if ( dlg.DoModal() == IDOK )
	{
		strFind = dlg.m_strFind;
		m_bMatchWholeWord = dlg.m_bMatchWholeWord == TRUE ? TRUE:FALSE;
		
		if (strcmp(strFind,"") == 0) return;   // nothing input  ,return;
		if ( strcmp(m_strFind,strFind) != 0)  //don't need not find again,if user input the same word
		{			
			std::vector<CString>::iterator p;
			for(p=m_vStrBuff.begin();p<m_vStrBuff.end();p++)
			{
				if ( strcmp(strFind,*p)==0 )
				{
					m_vStrBuff.erase(p);
				}
			}
			m_vStrBuff.push_back(strFind);
			m_vFoundItem.clear();
			SearchItemInTree(m_FlowTree.GetRootItem(),strFind,m_bMatchWholeWord); // store the found Item in m_vFoundItem, 
			m_iterator = m_vFoundItem.begin();
		}
		if ( m_vFoundItem.size()>0)   
		{
			if (dlg.m_bType)     //mark or find
			{
				ReloadTree();     //reload tree changed Item information ,so need reSearch
				m_vFoundItem.clear();
				SearchItemInTree(m_FlowTree.GetRootItem(),strFind,m_bMatchWholeWord); // store the found Item in m_vFoundItem, 
				m_iterator = m_vFoundItem.begin();				
				std::vector<HTREEITEM>::iterator n;       //mark word
				for(n=m_vFoundItem.begin();n<m_vFoundItem.end();n++)
				{
					m_FlowTree.SetItemColor((HTREEITEM)*n,RGB(0xFF, 0x00, 0x00));					
				}
				m_FlowTree.RedrawWindow();
			}
			else                        //select found item;
			{				
				m_FlowTree.SelectItem((HTREEITEM)*m_iterator); 
				m_iterator++;
				if (m_iterator == m_vFoundItem.end()) 
				{
					m_iterator = m_vFoundItem.begin();
				}
			}
		}
		m_strFind = strFind;   // remenber input last time;
	}	
}

BOOL CPaxFlowDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN )
	{
		TCHAR chCharCode = (TCHAR) pMsg->wParam;    // character code 
		if ( chCharCode == (TCHAR)114)      //press key F3
		{	
			if (m_vFoundItem.size()>0)    
			{		
				m_iterator++;
				if (m_iterator == m_vFoundItem.end()) 
				{
					m_iterator = m_vFoundItem.begin();
				}
				m_FlowTree.SelectItem((HTREEITEM)*m_iterator); //select found item;	
			}		
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CPaxFlowDlg::PaxFlowSetOneXOneStart(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( hItem );
	ASSERT( pData );

	switch(pData->m_arcInfo->GetOneXOneState()) 
	{
	case NonChannel : pData->m_arcInfo->SetOneXOneState(ChannelStart);
		break;
	case ChannelStart:pData->m_arcInfo->SetOneXOneState(NonChannel);
		break;
	default: pData->m_arcInfo->SetOneXOneState(NonChannel);
		break;
	}
}

void CPaxFlowDlg::OnPaxflow1x1start() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		PaxFlowSetOneXOneStart(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			PaxFlowSetOneXOneStart(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	
	m_bChanged = true;
	m_btnSave.EnableWindow();
	m_OperatedPaxFlow.SetChangedFlag( true );
	BuildInterestInPath( m_hRClickItem );
	ReloadTree();	

}

void CPaxFlowDlg::PaxFlowSetOneXOneEnd(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( hItem );
	ASSERT( pData );
	switch(pData->m_arcInfo->GetOneXOneState()) 
	{
	case NonChannel : pData->m_arcInfo->SetOneXOneState(ChannelEnd);
		break;
	case ChannelEnd:pData->m_arcInfo->SetOneXOneState(NonChannel);
		break;
	default: pData->m_arcInfo->SetOneXOneState(NonChannel);
		break;
	}
}

void CPaxFlowDlg::OnPaxflow1x1end() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		PaxFlowSetOneXOneEnd(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			PaxFlowSetOneXOneEnd(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_bChanged = true;
	m_btnSave.EnableWindow();
	m_OperatedPaxFlow.SetChangedFlag( true );
	BuildInterestInPath( m_hRClickItem );
	ReloadTree();			
}

//Confirm input OneXOne's data Available 
//bool CPaxFlowDlg::ConfirmOneXOneAvailable()
//{
//	while (!m_stack1x1.empty())
//	{
//		m_stack1x1.pop();
//	}
//	m_b1x1SeqError = FALSE ;
//	SearchOneXOne(m_FlowTree.GetRootItem());
//	if ( m_b1x1SeqError )
//	{
//		MessageBox("Miss {1:X:1 Start} before {1:X:1 End}","Input Error" , MB_ICONINFORMATION);
//		return FALSE;
//	}
//	if ( !m_stack1x1.empty()) 
//	{
//		MessageBox("Miss {1:X:1 End} After {1:X:1 Start}","Input Error" , MB_ICONINFORMATION);		
//		return FALSE;
//	}
//	return TRUE;
//}

// Search all 1x1start, 1x1end in flow
// return result with m_b1x1SeqError and m_stack1x1
/*
void CPaxFlowDlg::SearchOneXOne( HTREEITEM _hItem)
{
	HTREEITEM hChildItem;
	if (_hItem == NULL) { return ;}	
	TREEITEMDATA* pTempData = (TREEITEMDATA*)m_FlowTree.GetItemData(_hItem );
	CString  strProcId = pTempData->m_procId.GetIDString();
    if (pTempData->m_arcInfo != NULL)
	{		
		if (pTempData->m_arcInfo->GetOneXOneState() == ChannelStart)
		{
			m_stack1x1.push(ChannelStart);
		}
		if (pTempData->m_arcInfo->GetOneXOneState() == ChannelEnd)
		{
			if (m_stack1x1.empty()) 
			{
				m_b1x1SeqError = TRUE;
			}
			else
			{
				m_stack1x1.pop();
			}
		}		
	}
    hChildItem = m_FlowTree.GetChildItem(_hItem); 
	if (hChildItem == NULL) return ;
	TREEITEMDATA* pTempData2 = (TREEITEMDATA*)m_FlowTree.GetItemData(hChildItem );
	CString  strProcId2 = pTempData2->m_procId.GetIDString();	
	SearchOneXOne( hChildItem );
	while (hChildItem = m_FlowTree.GetNextItem( hChildItem, TVGN_NEXT) )  
	{
		SearchOneXOne( hChildItem);
	}
	return ;		
}
*/

void CPaxFlowDlg::OnPaxflowFollowState() 
{
	ASSERT( m_hRClickItem );
	
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( m_hRClickItem );
	ASSERT( pData );
	int nCurState = pData->m_arcInfo->GetTypeOfOwnership();
	bool bCurFollowState = pData->m_arcInfo->GetFollowState();
	//change current selected item
	pData->m_arcInfo->SetFollowState(!bCurFollowState);
	
	//change ahead
	HTREEITEM hUpItem;
	hUpItem = m_FlowTree.GetParentItem(m_hRClickItem);
	// TRACE(m_FlowTree.GetItemText(hUpItem)+"\n");
	TREEITEMDATA* pUpData = (TREEITEMDATA*)m_FlowTree.GetItemData( hUpItem );
	while (hUpItem != NULL&&(pUpData->m_arcInfo->GetTypeOfOwnership() == nCurState))
	{
		// TRACE(m_FlowTree.GetItemText(hUpItem)+"\n");		
		pUpData->m_arcInfo->SetFollowState(!bCurFollowState);
		pData->m_arcInfo->SetFollowState(!bCurFollowState);
		pUpData = (TREEITEMDATA*)m_FlowTree.GetItemData( hUpItem );		
		hUpItem = m_FlowTree.GetParentItem(hUpItem);			
	}

	//change behind
	HTREEITEM hDownItem;
	hDownItem =m_FlowTree.GetChildItem(m_hRClickItem);
	// TRACE(m_FlowTree.GetItemText(hDownItem)+"\n");	
	TREEITEMDATA* pDownData = (TREEITEMDATA*)m_FlowTree.GetItemData( hDownItem );	
	while (hDownItem != NULL&&(pDownData->m_arcInfo->GetTypeOfOwnership() == nCurState))
	{
		// TRACE(m_FlowTree.GetItemText(hDownItem)+"\n");	
		if (m_FlowTree.GetItemText(hDownItem).Find("END")!=-1) break; //if end flow break
		pDownData->m_arcInfo->SetFollowState(!bCurFollowState);
		pDownData = (TREEITEMDATA*)m_FlowTree.GetItemData( hDownItem );		
		hDownItem = m_FlowTree.GetChildItem(hDownItem);			
	}
	
	m_bChanged = true;
	m_btnSave.EnableWindow();
	m_OperatedPaxFlow.SetChangedFlag( true );
	BuildInterestInPath( m_hRClickItem );
	ReloadTree();				
}

 void CPaxFlowDlg::OnPaxflowChannelcondition11Off() 
 {
	 if(!m_hRClickItem)
		 m_hRClickItem = m_FlowTree.GetSelectedItem();

	 if (m_vCopyItem.empty())
	 {
		 PaxFlowSetOneToOne(m_hRClickItem);
	 }
	 else
	 {
		 for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		 {
			 TreeNodeInfor& nodeData = m_vCopyItem[i];
			 PaxFlowSetOneToOne(nodeData.m_hTreeNode);
		 }
		 m_vCopyItem.clear();
	 }

	 m_bChanged = true;
	 m_btnSave.EnableWindow();
	 m_OperatedPaxFlow.SetChangedFlag( true );
	 BuildInterestInPath( m_hRClickItem );
	 ReloadTree();	

 	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( m_hRClickItem );
 	ASSERT( pData );
 	if(!(pData->m_arcInfo->GetOneToOneFlag())) return;
 	
 	OnPaxflowOnetoone();	
 }

void CPaxFlowDlg::OnPaxflowChannelcondition11On() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		PaxFlowSetOneToOne(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			PaxFlowSetOneToOne(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_bChanged = true;
	m_btnSave.EnableWindow();
	m_OperatedPaxFlow.SetChangedFlag( true );
	BuildInterestInPath( m_hRClickItem );
	ReloadTree();	
}

void CPaxFlowDlg::PaxFlowchannelSetOneXOneEnd(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( hItem );
	ASSERT( pData );

	if (NonChannel != pData->m_arcInfo->GetOneXOneState() )
	{
		pData->m_arcInfo->SetOneXOneState(NonChannel);
	} 	

	switch(pData->m_arcInfo->GetOneXOneState()) 
	{
	case NonChannel : pData->m_arcInfo->SetOneXOneState(ChannelEnd);
		break;
	case ChannelEnd:pData->m_arcInfo->SetOneXOneState(NonChannel);
		break;
	default: pData->m_arcInfo->SetOneXOneState(NonChannel);
		break;
	}
	
}

void CPaxFlowDlg::OnPaxflowChannelcondition1x1End() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		PaxFlowchannelSetOneXOneEnd(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			PaxFlowchannelSetOneXOneEnd(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_bChanged = true;
	m_btnSave.EnableWindow();
	m_OperatedPaxFlow.SetChangedFlag( true );
	BuildInterestInPath( m_hRClickItem );
	ReloadTree();		
}

void CPaxFlowDlg::PaxFlowChannelSetOneXOneStart(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( hItem );
	ASSERT( pData );

	if (NonChannel != pData->m_arcInfo->GetOneXOneState() )
	{
		pData->m_arcInfo->SetOneXOneState(NonChannel);
	} 


	switch(pData->m_arcInfo->GetOneXOneState()) 
	{
	case NonChannel : pData->m_arcInfo->SetOneXOneState(ChannelStart);
		break;
	case ChannelStart:pData->m_arcInfo->SetOneXOneState(NonChannel);
		break;
	default: pData->m_arcInfo->SetOneXOneState(NonChannel);
		break;
	}
	
}

void CPaxFlowDlg::OnPaxflowChannelcondition1x1Start() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		PaxFlowChannelSetOneXOneStart(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			PaxFlowChannelSetOneXOneStart(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_bChanged = true;
	m_btnSave.EnableWindow();
	m_OperatedPaxFlow.SetChangedFlag( true );
	BuildInterestInPath( m_hRClickItem );
	ReloadTree();	
}

void CPaxFlowDlg::PaxFlowChannelSetOneXOneClear(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData( hItem );
	ASSERT( pData );

	if (NonChannel != pData->m_arcInfo->GetOneXOneState() )
	{
		pData->m_arcInfo->SetOneXOneState(NonChannel);
	} 	
}

void CPaxFlowDlg::OnPaxflowChannelcondition1x1Clear() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		PaxFlowChannelSetOneXOneClear(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			PaxFlowChannelSetOneXOneClear(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_bChanged = true;
	m_btnSave.EnableWindow();
	m_OperatedPaxFlow.SetChangedFlag( true );
	BuildInterestInPath( m_hRClickItem );
	ReloadTree();
}

void CPaxFlowDlg::OnToolbarDropDown(NMHDR* pNMHDR, LRESULT* plRes)
{
	NMTOOLBAR* pNMTB = (NMTOOLBAR*) pNMHDR;
	if (pNMTB->iItem != ID_PAXFLOW_SYNCPOINT)
		return;

	int nTypeIndex = m_OperatedPaxFlow.GetPassengerConstrain()->GetTypeIndex();
	if (nTypeIndex == 0) // pax type
		return;

	// get the sync point of this processor group
	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData(m_hRClickItem);
	ASSERT(pData);
	int nSelSyncPoint = GetSyncPointIndex(pData->m_arcInfo->GetProcID());

	// create the menu to show the owner syncpoint
	CMenu menu;
	menu.CreatePopupMenu();

	InputFlowSyncItemList owenerSyncPoints;
	m_pInTerm->GetInputFlowSync()->GetFlowPaxSync(*m_pPaxFlowAttachedToNonPaxFlow->GetPassengerConstrain(),owenerSyncPoints);
	CInputFlowPaxSyncItem* pFlowSyncItem =  m_pInTerm->GetInputFlowSync()->GetFlowParentNonPaxSync(*m_OperatedPaxFlow.GetPassengerConstrain(),pData->m_arcInfo->GetProcID());

	int nStartRest = SYNCPOINT_STARTID;
	for (InputFlowSyncItemList::iterator iter = owenerSyncPoints.begin(); iter != owenerSyncPoints.end(); ++iter)
	{
		CInputFlowPaxSyncItem* pItem = *iter;
		CString strSyncPoint;
		strSyncPoint.Format(_T("%s"), pItem->GetSyncName());

		UINT nFlag = MF_STRING;
			nFlag |= MF_ENABLED;

		if (pFlowSyncItem == pItem)
			nFlag |= MF_CHECKED;
		else
			nFlag |= MF_UNCHECKED;

		menu.AppendMenu(nFlag, nStartRest++, strSyncPoint);
	}

	CRect rc;
	m_ToolBarFlowTree.SendMessage(TB_GETRECT, pNMTB->iItem, (LPARAM)&rc);
	m_ToolBarFlowTree.ClientToScreen(&rc);

	menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,	rc.left, rc.bottom, this, &rc);
}

void CPaxFlowDlg::OnSelectSyncPoint(UINT nID)
{
	int nTypeIndex = m_OperatedPaxFlow.GetPassengerConstrain()->GetTypeIndex();
	if (nTypeIndex == 0) // pax type
		return;

	int nSelSyncPoint = nID - SYNCPOINT_STARTID;

	TREEITEMDATA* pData = (TREEITEMDATA*)m_FlowTree.GetItemData(m_hRClickItem);
	ASSERT(pData);

	InputFlowSyncItemList owenerSyncPoints;
	m_pInTerm->GetInputFlowSync()->GetFlowPaxSync(*m_pPaxFlowAttachedToNonPaxFlow->GetPassengerConstrain(),owenerSyncPoints);
	CInputFlowPaxSyncItem* pFlowSyncItem =  m_pInTerm->GetInputFlowSync()->GetFlowParentNonPaxSync(*m_OperatedPaxFlow.GetPassengerConstrain(),pData->m_arcInfo->GetProcID());
	CInputFlowSyncItem* pFlowNonSyncItem = m_pInTerm->GetInputFlowSync()->GetFlowNonPaxSync(*m_OperatedPaxFlow.GetPassengerConstrain(),pData->m_arcInfo->GetProcID());
	int nCount = (int)owenerSyncPoints.size();
	if (nSelSyncPoint < nCount)
	{
		CInputFlowPaxSyncItem* pNewSelectSyncPaxItem = owenerSyncPoints[nSelSyncPoint];
		if(pNewSelectSyncPaxItem != pFlowSyncItem)
		{
			if (pFlowSyncItem)
			{
				pFlowSyncItem->DeleteItem(pFlowNonSyncItem);
			}
			
			CInputFlowSyncItem* pNewNonPaxFlowSyncItem = new CInputFlowSyncItem();
			pNewNonPaxFlowSyncItem->SetProcID(pData->m_arcInfo->GetProcID());
			pNewNonPaxFlowSyncItem->SetMobElementConstraint(*m_OperatedPaxFlow.GetPassengerConstrain());
			pNewSelectSyncPaxItem->AddItem(pNewNonPaxFlowSyncItem);
		}
		else
		{
			if (pFlowSyncItem)
			{
				pFlowSyncItem->DeleteItem(pFlowNonSyncItem);
			}
		}
		m_bChanged = true;
		m_btnSave.EnableWindow();
		m_OperatedPaxFlow.SetChangedFlag(true);
		BuildInterestInPath(m_hRClickItem);
		ReloadTree();
	}
}

int CPaxFlowDlg::GetSyncPointIndex(const ProcessorID& procID)
{
	CString strID = procID.GetIDString();

	int nSyncPoint = -1;
	//const CMobileElemConstraint* pPaxType = m_OperatedPaxFlow.GetPassengerConstrain();
	//int nTypeIndex = m_OperatedPaxFlow.GetPassengerConstrain()->GetTypeIndex();

	//if (nTypeIndex == 0) // pax type
	//	nSyncPoint = m_pInTerm->GetInputFlowSync()->FindPaxSyncPoint(*pPaxType, procID);
	//else
	//	nSyncPoint = m_pInTerm->GetInputFlowSync()->FindNonPaxSyncPoint(nTypeIndex, procID);

	return nSyncPoint;
}

void CPaxFlowDlg::OnClickTreeProcess( NMHDR *pNMHDR, LRESULT *pResult )
{
	CPoint pt = GetMessagePos();
	m_FlowTree.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hItem = m_FlowTree.HitTest(pt, &iRet);
	if( hItem == NULL )
		return;

	if (hItem == m_FlowTree.GetRootItem())
		return;
	
	CWaitCursor	wCursor;
	if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
	{
		TreeNodeInfor nodeInfor;
		nodeInfor.m_hTreeNode = hItem;
		nodeInfor.m_color = m_FlowTree.GetItemColor(hItem);
		nodeInfor.m_bBold = m_FlowTree.GetItemBold(hItem)?TRUE:FALSE;
		m_FlowTree.SetItemColor(hItem,PROCESS_COLOR_RED);
		m_FlowTree.SetItemBold(hItem,TRUE);
		if (std::find(m_vCopyItem.begin(),m_vCopyItem.end(),nodeInfor) == m_vCopyItem.end())
		{
			m_vCopyItem.push_back(nodeInfor);
		}
	}
	else
	{
		for (size_t i = 0; i < m_vCopyItem.size(); i++)
		{
			const TreeNodeInfor& nodeInfor = m_vCopyItem.at(i);
			m_FlowTree.SetItemColor(nodeInfor.m_hTreeNode,nodeInfor.m_color);
			m_FlowTree.SetItemBold(nodeInfor.m_hTreeNode,nodeInfor.m_bBold);
		}
		m_vCopyItem.clear();
	}

	m_FlowTree.Invalidate(FALSE);
}

void CPaxFlowDlg::OnCopyProcessors()
{
	m_vCopyProcessors.clear();

	std::deque<TreeNodeInfor> vDeque = m_vCopyItem;
	while (!vDeque.empty())
	{
		TreeNodeInfor nodeInfor = vDeque.front();
		TREEITEMDATA* pNodeData = (TREEITEMDATA*)m_FlowTree.GetItemData( nodeInfor.m_hTreeNode); 
		if (pNodeData == NULL)
			return;
		
		vDeque.pop_front();
		HTREEITEM hParentITem = m_FlowTree.GetParentItem(nodeInfor.m_hTreeNode);
		if (hParentITem == NULL)
			return;
		
		TREEITEMDATA* pParentNodeData = (TREEITEMDATA*)m_FlowTree.GetItemData( hParentITem);
		if (pParentNodeData == NULL)
			return;

		TreeNodeInfor parentInfor;
		parentInfor.m_hTreeNode = hParentITem;
		if (std::find(vDeque.begin(),vDeque.end(),parentInfor) != vDeque.end())
		{
			vDeque.push_back(nodeInfor);
		}
		else
		{
			if (!m_vCopyProcessors.empty())
			{
				const CFlowDestination& flowDest = m_vCopyProcessors.back();
				ProcessorID id = flowDest.GetProcID();
				if (!(id == pParentNodeData->m_procId))
				{
					m_vCopyProcessors.clear();
					MessageBox( "Sequence processor is allowed in this case", "Error", MB_OK|MB_ICONWARNING );
					return ;
				}
			}
			m_vCopyProcessors.push_back(*pNodeData->m_arcInfo);
		}
	}

	m_vCopyItem.clear();
}

int CPaxFlowDlg::GetStationType(int nLevel)
{
	if (!m_hRClickItem)
		return -1;

	//retrieve station type
	if (nLevel < 0 || nLevel > (int)m_vCopyProcessors.size())
		return -1;
	
	ProcessorID id;
	const CFlowDestination& flowDest = m_vCopyProcessors.at(nLevel);
	id = flowDest.GetProcID();
	if (IsStation(id ))
	{
		if (nLevel == 0)
		{
			HTREEITEM hFather = m_FlowTree.GetParentItem( m_hRClickItem );	
			if( hFather )
			{
				TREEITEMDATA* pFatherData = (TREEITEMDATA* )m_FlowTree.GetItemData( hFather );
				ASSERT( pFatherData );
				if( IsStation( pFatherData->m_procId) )
				{
					return 3; // he is a station and have a father , his father is a station			
				}
				else
				{
					return 2; // he is a station and have a father , but father is not a station
				}
			}
			else
			{
				return 1;
			}
		}
		else
		{
			ProcessorID fatherID;
			const CFlowDestination& parentDest = m_vCopyProcessors.at(nLevel - 1);
			
			fatherID = parentDest.GetProcID();
			if( IsStation( fatherID) )
			{
				return 3; // he is a station and have a father , his father is a station			
			}
			else
			{
				return 2; // he is a station and have a father , but father is not a station
			}
		}
	}
	else
	{
		return 0;
	}
}
void CPaxFlowDlg::OnAddCopyProcessors()
{
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	if (!m_hRClickItem)
		return;

	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( m_hRClickItem );
	if (pData == NULL)
		return;
	
	ProcessorID preID;
	preID = pData->m_procId;
	for (size_t i = 0; i < m_vCopyProcessors.size(); i++)
	{	
		ProcessorID id;
		const CFlowDestination& flowDest = m_vCopyProcessors.at(i);
		id = flowDest.GetProcID();

		int iType = GetStationType(i);;

		const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
		if( id == *(pProcStart->getID())  )
		{
			MessageBox( "START processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
			return ;
		}

		if( iType == 3 && IsStation( id ) )
		{
			MessageBox( "STATION processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
			return ;
		}


		m_OperatedPaxFlow.AddDestProc( preID, flowDest );

		CProcessorDestinationList* pFlowPair=m_OperatedPaxFlow.GetEqualFlowPairAt(preID);
		ASSERT(pFlowPair!=NULL);
		pFlowPair->EvenPercent();
		m_OperatedPaxFlow.GetEqualFlowPairAt( preID )->SetAllDestInherigeFlag( 0 );

		preID = id;
	}

	m_bChanged = true; 
	m_btnSave.EnableWindow();
	BuildInterestInPath( m_hRClickItem );

	ReloadTree();
}

void CPaxFlowDlg::OnInsertCopyProcessors()
{
	if(!m_hRClickItem)
		m_hRClickItem = m_FlowTree.GetSelectedItem();

	TREEITEMDATA* pData = (TREEITEMDATA* )m_FlowTree.GetItemData( m_hRClickItem );
	ASSERT( pData );
	
	ProcessorID preID;
	preID = pData->m_procId;
	for (size_t i = 0; i < m_vCopyProcessors.size(); i++)
	{
		const CFlowDestination& flowDest = m_vCopyProcessors.at(i);
		
		ProcessorID id;
		id = flowDest.GetProcID();

		if( IsStation( id ) )
		{
			MessageBox( "STATION processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
			return ;
		}

		const Processor* pProcStart = GetInputTerminal()->procList->getProcessor( START_PROCESSOR_INDEX );
		if( id  == *(pProcStart->getID()) )
		{
			MessageBox( "START processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
			return ;
		}

		const Processor* pProcEnd = GetInputTerminal()->procList->getProcessor( END_PROCESSOR_INDEX );
		if( id == *(pProcEnd->getID())  )
		{
			MessageBox( "END processor is not allowed in this case", "Error", MB_OK|MB_ICONWARNING );
			return ;
		}
		m_OperatedPaxFlow.InsertProceoosorAfter( preID, id );
		SetCopyFlowDestion(preID,flowDest);

		preID = id;
	}

	m_bChanged = true;
	m_btnSave.EnableWindow();
	BuildInterestInPath( m_hRClickItem );
	ReloadTree();
}

void CPaxFlowDlg::SetCopyFlowDestion( const ProcessorID& procID,const CFlowDestination& flowDest )
{
	CProcessorDestinationList* flowPair = m_OperatedPaxFlow.GetEqualFlowPairAt(procID);
	if (flowPair == NULL)
		return;
	
	CFlowDestination* destInfo = flowPair->GetDestProcArcInfo(flowDest.GetProcID());
	if (destInfo == NULL)
		return;
	
	*destInfo = flowDest;
}