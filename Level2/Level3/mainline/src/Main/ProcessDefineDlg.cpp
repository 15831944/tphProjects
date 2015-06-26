// ProcessDefineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ProcessDefineDlg.h"
#include "Mainfrm.h"

#include "TermPlanDoc.h"
#include "inputs\SubFlowList.h"
#include "inputs\SubFlow.h"
#include "common\WinMsg.h"
#include ".\processdefinedlg.h"

#include "../Inputs/SingleProcessFlow.h"
#include "PaxFlowSelectPipes.h"
#include "../Inputs/PipeDataSet.h"
#include "..\Inputs\HandleSingleFlowLogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CString CS_CYCLIC_LABEL = "@ CYCLIC @";
#define ADD_NEW		1
#define ADD_DEST	2
#define ADD_BEFORE	3
#define ADD_AFTER	4
#define PROCESS_COLOR_RED RGB(128,0,0)
/////////////////////////////////////////////////////////////////////////////
// CProcessDefineDlg dialog

IMPLEMENT_DYNAMIC(CProcessDefineDlg, CDialog)

std::vector<ProcessorID> CProcessDefineDlg::m_vFocusInPath;
CProcessDefineDlg::CProcessDefineDlg(CTermPlanDoc* pDoc,CWnd* pParent /*=NULL*/)
: CDialog(CProcessDefineDlg::IDD)
{
	//{{AFX_DATA_INIT(CProcessDefineDlg)
	//}}AFX_DATA_INIT
	pEdit					= NULL;
	m_pCurFlow				= NULL;
	m_bDragging				= false;
	m_pDragImage			= NULL;
	m_hItemDragSrc			= NULL;
	m_hItemDragDes			= NULL;
	m_hRClickItem			= NULL;
	
	m_hAllProcessorSelItem	= NULL;
	m_hPocessSelItem		= NULL; 
	m_sizeLastWnd           =CSize(0,0);

	m_hModifiedItem 		= NULL;
	m_pDoc = pDoc;

	m_b1x1SeqError = FALSE;
	m_vCopyItem.clear();
	m_vCopyProcessors.clear();
}

CProcessDefineDlg::~CProcessDefineDlg()
{
	// delete the edit
	if( pEdit)
		delete pEdit;
	
	// free tree data vect
	int iItemDataCount = m_vTreeItemData.size();
	for( int i=0; i<iItemDataCount; ++i )
	{
		delete m_vTreeItemData[i];
	}
	m_vTreeItemData.clear();
	
	// clear drag image
	if( m_pDragImage )
		delete m_pDragImage;

	m_vCopyItem.clear();
	m_vCopyProcessors.clear();
}

void CProcessDefineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcessDefineDlg)
	DDX_Control(pDX, IDC_STATIC2, m_static2);
	DDX_Control(pDX, IDC_STATIC1, m_static1);
	DDX_Control(pDX, IDC_STATIC_SPLITTER, m_splitter);
	DDX_Control(pDX, IDMYOK, m_ok);
	DDX_Control(pDX, IDMYCANCEL, m_cancel);
	DDX_Control(pDX, IDC_STATIC_TREE_FRAME, m_staticTreeFrame);
	DDX_Control(pDX, IDC_STATIC_TOOLBAR_TREE, m_staticTreeToolBar);
	DDX_Control(pDX, IDC_STATIC_CONTAIN, m_staticFrame);
	DDX_Control(pDX, IDC_TREE_PROCESS, m_treeProcess);
	DDX_Control(pDX, IDC_TREE_ALLPROCESSOR, m_treeAllProcessor);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_listProcess);
	DDX_Control(pDX, IDC_BUT_SAVE, m_butSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProcessDefineDlg, CDialog)
	//{{AFX_MSG_MAP(CProcessDefineDlg)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_LBN_SELCHANGE(IDC_LIST_PROCESS, OnSelchangeListProcess)
	ON_MESSAGE(WM_END_EDIT,OnEndEdit)
	ON_BN_CLICKED(IDC_BUT_SAVE, OnButSave)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_ALLPROCESSOR, OnBegindragTreeAllprocessor)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_PROCESS_ADD_ISOLATE_NODE, OnProcessAddIsolateNode)
	ON_COMMAND(ID_PROCESS_ADD_DEST_NODE, OnProcessAddDestNode)
	ON_COMMAND(ID_PROCESS_ADD_BEFORE, OnProcessAddBefore)
	ON_COMMAND(ID_PROESS_ADD_AFTER, OnProessAddAfter)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROCESS_CUT_SOURCE_LINK, OnProcessCutSourceLink)
	ON_COMMAND(ID_PROCESS_TAKEOVER, OnProcessTakeover)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROCESS, OnSelchangedTreeProcess)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ALLPROCESSOR, OnSelchangedTreeAllprocessor)
	ON_COMMAND(ID_PD_ADD_DEST, OnPdAddDest)
	ON_COMMAND(ID_PD_ADD_NEW, OnPdAddNew)
	ON_COMMAND(ID_PD_INSERT_AFTER, OnPdInsertAfter)
	ON_COMMAND(ID_PD_INSERT_BEFORE, OnPdInsertBefore)
	ON_COMMAND(ID_PD_PRUNE, OnPdPrune)
	ON_COMMAND(ID_PD_REMOVE_PROCESS, OnPdRemoveProcess)
	ON_COMMAND(ID_PROCESS_ONETOONE, OnProcessOnetoone)
	ON_COMMAND(ID_PD_ONETOONE, OnPdOnetoone)
	ON_COMMAND(ID_PD_1X1START, OnPd1x1start)
	ON_COMMAND(ID_PD_1X1END, OnPd1x1end)
	ON_BN_CLICKED(IDMYOK, OnMyok)
	ON_BN_CLICKED(IDMYCANCEL, OnMycancel)
	ON_WM_SIZE()
	ON_COMMAND(ID_PD_MODIFYPERCENT, OnProcessModifypercent)
	ON_COMMAND(ID_PD_EVEN_PERCENT, OnProcessEvenpercent)
	ON_COMMAND(ID_PD_PIPE_AUTO, OnProcessEditPipeAuto)
	ON_COMMAND(ID_PD_PIPE, OnProcessEditPipe)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_UPDATE_COMMAND_UI(ID_TOOLBARBUTTONDEL, OnUpdateBtnDel)

	ON_UPDATE_COMMAND_UI(ID_PD_ADD_NEW, OnUpdateBtnAddNew)
	ON_UPDATE_COMMAND_UI(ID_PD_ADD_DEST, OnUpdateBtnAddDest)
	ON_UPDATE_COMMAND_UI(ID_PD_INSERT_BEFORE, OnUpdateBtnInsertBefore)
	ON_UPDATE_COMMAND_UI(ID_PD_INSERT_AFTER, OnUpdateBtnInsertAfter)

	ON_UPDATE_COMMAND_UI(ID_PD_PRUNE, OnUpdateBtnPrune)
	ON_UPDATE_COMMAND_UI(ID_PD_REMOVE_PROCESS, OnUpdateBtnRemoveProcess)
	ON_UPDATE_COMMAND_UI(ID_PD_ONETOONE, OnUpdateBtnOntToOne)
	ON_UPDATE_COMMAND_UI(ID_PD_1X1START, OnUpdateBtn1X1Start)
	ON_UPDATE_COMMAND_UI(ID_PD_1X1END, OnUpdateBtn1X1End)

	ON_UPDATE_COMMAND_UI(ID_PD_PIPE, OnUpdateBtnPipe)
	ON_UPDATE_COMMAND_UI(ID_PD_PIPE_AUTO, OnUpdateBtnPipeAuto)
	ON_UPDATE_COMMAND_UI(ID_PD_MODIFYPERCENT, OnUpdateBtnModifyPercent)
	ON_UPDATE_COMMAND_UI(ID_PD_EVEN_PERCENT, OnUpdateBtnEvenPercent)
	ON_COMMAND(ID_PROCESS_EVENPERCENT, OnProcessModifypercent)
	ON_COMMAND(ID_PIPES_USERSELECT, OnPipesUserselect)
	ON_COMMAND(ID_PIPES_ENABLEAUTOMATIC, OnPipesEnableautomatic)
	ON_COMMAND(ID_CHANNEL_OFF, OnChannelOff)
	ON_COMMAND(ID_CHANNEL_ON, OnChannelOn)
	ON_COMMAND(ID_PD_CHANNELCONDITION_1X1_END, OnPdChannelcondition1x1End)
	ON_COMMAND(ID_PD_CHANNELCONDITION_1X1_START, OnPdChannelcondition1x1Start)
	ON_COMMAND(ID_PD_CHANNELCONDITION_1X1_CLEAR, OnPdChannelcondition1x1Clear)
	ON_COMMAND(ID_PERCENT_PERCENTSPLIT, OnPercentPercentsplit)
	ON_COMMAND(ID_PERCENT_EVENREMAINING, OnPercentEvenremaining)

	ON_COMMAND(ID_PAXFLOW_COPYPROCESSOR,OnCopyProcessors)
	ON_COMMAND(ID_PAXFLOW_ADDCOPY,OnAddCopyProcessors)
	ON_COMMAND(ID_PAXFLOW_INSERTCOPY,OnInsertCopyProcessors)
	ON_NOTIFY(NM_CLICK, IDC_TREE_PROCESS, OnClickTreeProcess)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessDefineDlg message handlers

int CProcessDefineDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS /*| CBRS_FLYBY |CBRS_HIDE_INPLACE*/) ||
		!m_toolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	if (!m_toolBarProcessTree.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS /*| CBRS_FLYBY | CBRS_SIZE_DYNAMIC*/) ||
		!m_toolBarProcessTree.LoadToolBar(IDR_PROCESS_DEFINE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	return 0;
}

void CProcessDefineDlg::InitToolBar()
{
	CRect rc;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect( &rc );
	GetDlgItem(IDC_STATIC_TOOLBAR)->ShowWindow(SW_HIDE);	
	ScreenToClient(&rc);
	m_toolBar.MoveWindow(&rc,true);
	
	m_toolBar.GetToolBarCtrl().DeleteButton(2);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,TRUE);

	
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);
//	m_toolBar.GetToolBarCtrl().HideButton(ID_TOOLBARBUTTONDEL);
	
	m_toolBar.ShowWindow( SW_SHOW );
	
	m_staticTreeToolBar.GetWindowRect( &rc );
	ScreenToClient( &rc );
	m_toolBarProcessTree.MoveWindow(&rc,true);
	m_staticTreeToolBar.ShowWindow(SW_HIDE);
	
	m_toolBarProcessTree.GetToolBarCtrl().HideButton(ID_PD_PIPLE_DISABLE);
	m_toolBarProcessTree.GetToolBarCtrl().HideButton(ID_BUTTON33971);
	m_toolBarProcessTree.GetToolBarCtrl().HideButton(ID_PD_ADD_NEW);

	m_toolBarProcessTree.ShowWindow( SW_SHOW );	

	
}

BOOL CProcessDefineDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CWaitCursor wc;
	InitToolBar();
	
	m_image.Create ( IDB_PROCESS,16,1,RGB(255,0,255) );
	m_treeAllProcessor.SetImageList ( &m_image,TVSIL_NORMAL );
	LoadAllProcessor();
	
	LoadProcessData();
	InitSpiltter();
	
	m_treeProcess.SetTreeDefColor(RGB(0,0,0));

	m_treeProcess.EnableWindow( false );
	m_butSave.EnableWindow( false );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProcessDefineDlg::InitDialog()
{
	m_pCurFlow				= NULL;
	m_bDragging				= false;
	m_pDragImage			= NULL;
	m_hItemDragSrc			= NULL;
	m_hItemDragDes			= NULL;
	m_hRClickItem			= NULL;

	m_hAllProcessorSelItem	= NULL;
	m_hPocessSelItem		= NULL; 
	//m_sizeLastWnd           =CSize(0,0);

	m_hModifiedItem 		= NULL;

	m_b1x1SeqError = FALSE;
	m_vCopyItem.clear();
	m_vCopyProcessors.clear();


	m_treeAllProcessor.DeleteAllItems();
	if(m_hAllProcessorSelItem)//the item will be set value if deleting one item, so here to reset it to NULL
		m_hAllProcessorSelItem	= NULL;

	LoadAllProcessor();
	m_treeProcess.DeleteAllItems();
	LoadProcessData();
	m_treeProcess.SetTreeDefColor(RGB(0,0,0));
	m_treeProcess.EnableWindow( false );
	m_butSave.EnableWindow( false );


}

InputTerminal* CProcessDefineDlg::GetInputTerminal()
{
    return (InputTerminal*)&m_pDoc->GetTerminal();
}

CString CProcessDefineDlg::GetProjectPath()
{
	return m_pDoc->m_ProjInfo.path;
}

void CProcessDefineDlg::OnButSave() 
{
	// TODO: Add your control notification handler code here
	if (GetInputTerminal()->m_pSubFlowList->IfAllFlowValid())
	{
		GetInputTerminal()->m_pSubFlowList->saveDataSet( GetProjectPath() ,true);
		m_butSave.EnableWindow( false );
	}
}


// load all processor tree
void CProcessDefineDlg::LoadAllProcessor()
{
	ProcessorList *procList = GetInputTerminal()->GetProcessorList();
	if (procList == NULL)
		return ;
	
	// Set data to processor tree
	ProcessorID id, *pid = NULL;
	id.SetStrDict( GetInputTerminal()->inStrDict );
	//int includeBarriers = 0;
	CString strTemp, strTemp1, strTemp2, strTemp3, str;
	
	TVITEM ti;
	TVINSERTSTRUCT tis;
	HTREEITEM hItem = NULL, hItem1 = NULL, hItem2 = NULL, hItem3 = NULL;	
	int i = 0, j = 0;
	
	StringList strDict;
	procList->getAllGroupNames (strDict, -1);
	//if (includeBarriers && !strDict.getCount())
	//	procList->getMemberNames (id, strDict, BarrierProc);
	
	/*if( m_bDisplayBaggageDeviceSign)
	{
	const Processor* pProcBaggageDevice =procList->getProcessor( BAGGAGE_DEVICE_PROCEOR_INDEX );
	m_vectID.push_back( *( pProcBaggageDevice->getID() ) );	
	CString sDisplay = " ";
	sDisplay += pProcBaggageDevice->getID()->GetIDString();
	m_ProcTree.SetItemData(m_ProcTree.InsertItem( sDisplay ), 0 );
}*/
	
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
		
		tis.hParent = TVI_ROOT;
		tis.hInsertAfter = TVI_SORT;
		tis.item = ti;
		
		hItem = m_treeAllProcessor.InsertItem(&tis);
		if (hItem == NULL)
			continue;
		
		str = strTemp;
		id.setID((LPCSTR) str);
		
		m_vectProcessorID.push_back( id );
		m_treeAllProcessor.SetItemData(hItem, (DWORD) ( m_vectProcessorID.size()-1 ) );
		
		StringList strDict1;
		procList->getMemberNames (id, strDict1, -1);
		for (int m = 0; m < strDict1.getCount(); m++)   // Level 2
		{
			strTemp1 = strDict1.getString (m);
			strTemp1.TrimLeft(); strTemp1.TrimRight();
			if (strTemp1.IsEmpty())
				continue;
			
			ti.pszText  = strTemp1.GetBuffer(0);
			tis.hParent = hItem;
			tis.item = ti;
			
			hItem1 = m_treeAllProcessor.InsertItem(&tis);
			if (hItem1 == NULL)
				continue;
			
			str = strTemp;
			str += "-";
			str += strTemp1;
			
			id.setID((LPCSTR) str);
			
			m_vectProcessorID.push_back( id );
			m_treeAllProcessor.SetItemData(hItem1, (DWORD) ( m_vectProcessorID.size()-1 ) );
			
			StringList strDict2;
			procList->getMemberNames (id, strDict2, -1);
			for (int n = 0; n < strDict2.getCount(); n++)   // Level 3
			{
				strTemp2 = strDict2.getString (n);
				strTemp2.TrimLeft(); strTemp2.TrimRight();
				if (strTemp2.IsEmpty())
					continue;
				
				ti.pszText  = strTemp2.GetBuffer(0);
				tis.hParent = hItem1;
				tis.item    = ti;
				
				hItem2 = m_treeAllProcessor.InsertItem(&tis);
				if (hItem2 == NULL)
					continue;
				
				str = strTemp;   str += "-";
				str += strTemp1; str += "-";
				str += strTemp2;
				
				id.setID((LPCSTR) str);
				
				m_vectProcessorID.push_back( id );
				m_treeAllProcessor.SetItemData(hItem2, (DWORD) ( m_vectProcessorID.size()-1 ) );
				
				StringList strDict3;
				procList->getMemberNames (id, strDict3, -1);
				for (int x = 0; x < strDict3.getCount(); x++)   // Level 4
				{
					strTemp3 = strDict3.getString (x);
					strTemp3.TrimLeft(); strTemp3.TrimRight();
					if (strTemp3.IsEmpty())
						continue;
					
					ti.pszText  = strTemp3.GetBuffer(0);
					tis.hParent = hItem2;
					tis.item    = ti;
					
					hItem3 = m_treeAllProcessor.InsertItem(&tis);
					
					str = strTemp;   str += "-";
					str += strTemp1; str += "-";
					str += strTemp2; str += "-";
					str += strTemp3;
					
					id.setID((LPCSTR) str);
					
					m_vectProcessorID.push_back( id );
					m_treeAllProcessor.SetItemData(hItem3, (DWORD) ( m_vectProcessorID.size()-1 ) );
				}
			}
		}
	}
}


void CProcessDefineDlg::LoadProcessData()
{
	// clear all string
	m_listProcess.ResetContent();
	
	// insert all process to the list
	int iCount = GetInputTerminal()->m_pSubFlowList->GetProcessUnitCount();
	for(int i=0; i<iCount; i++ )
	{
		CSubFlow* pProcessFlow = GetInputTerminal()->m_pSubFlowList->GetProcessUnitAt(i);
		CString strProcessName = pProcessFlow->GetProcessUnitName();
		int iIdx = m_listProcess.AddString( strProcessName );
		m_listProcess.SetItemData( iIdx, i );
	}
}

bool CProcessDefineDlg::IfExistInProcessor(CString _strName)
{
	for( int i=0; i<static_cast<int>(m_vectProcessorID.size()); i++ )
	{
		CString strProcName = m_vectProcessorID[i].GetIDString();
		if( strProcName == _strName )
			return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
void CProcessDefineDlg::OnSelchangeListProcess() 
{
	// TODO: Add your control notification handler code here
	int iCurSel = m_listProcess.GetCurSel();
	if( iCurSel == LB_ERR )
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,true);
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,false);
		//m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,false);
		
		// clear the tree ctrl
		m_treeProcess.DeleteAllItems();
		m_pCurFlow = NULL;
		m_treeProcess.EnableWindow( false );
	}
	else
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,true);
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,true);
		//m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,true);
		
		// reaload the process flow
		int idx = m_listProcess.GetItemData( iCurSel );
		m_pCurFlow = GetInputTerminal()->m_pSubFlowList->GetProcessUnitAt(idx)->GetInternalFlow();
		ReloadProcessFlow(  );
		m_treeProcess.EnableWindow( true );
	}
	m_vFocusInPath.clear();
	m_hPocessSelItem = NULL; 		// make sure no item is selected
	SetProcessTreeToolBar();
}

void CProcessDefineDlg::ReloadProcessFlow()
{
	ASSERT( m_pCurFlow!= NULL );
	BuildCollapsedNodeVector();
	// clear ...
	int iItemDataCount = m_vTreeItemData.size();
	for( int i=0; i<iItemDataCount; ++i )
	{
		delete m_vTreeItemData[i];
	}
	m_hInterestItem = NULL;
	m_vTreeItemData.clear();
	m_treeProcess.DeleteAllItems();
	
	//Modified by hans 2005-11-17
	//Add PROCESSSTART to PROCESS view
	//PROCESSSTART
	//    A-1

	CProcessorDestinationList *pStartPair = ((CSingleProcessFlow *)m_pCurFlow)->GetStartPair();
	if (pStartPair == NULL)
		return;
	

	std::vector<ProcessorID> vAllRootProc;
	m_pCurFlow->SetAllPairVisitFlag( false );
	m_pCurFlow->GetRootProc( vAllRootProc );
	int iRootCount = vAllRootProc.size();
	if( 0 == iRootCount )
	{
		std::vector<ProcessorID> vAllInvolvedProc;
		m_pCurFlow->GetAllInvolvedProc( vAllInvolvedProc );
		if( vAllInvolvedProc.size() > 0 )// just have once cicle path, so it will be no root processor
		{
			ProcessorID ProcID;
			if (pStartPair->GetDestCount() == 0)
			{
				ProcID = vAllInvolvedProc.front();
			}
			else
			{
				//if have stored the root information in the pStartPair
				ProcID = pStartPair->GetDestProcAt(0).GetProcID();
			}
			HTREEITEM hStartRoot = m_treeProcess.InsertItem(pStartPair->GetProcID().GetIDString());

			TREEITEMDATA *pStartData = new TREEITEMDATA;
			pStartData->m_procId = pStartPair->GetProcID();
			m_vTreeItemData.push_back(pStartData);
			m_treeProcess.SetItemData(hStartRoot,(DWORD)pStartData);
			LoadSubTree(hStartRoot,m_pCurFlow->GetEqualFlowPairAt(ProcID));

		}
		else if (vAllInvolvedProc.size() == 0)
		{
			HTREEITEM hStartRoot = m_treeProcess.InsertItem(pStartPair->GetProcID().GetIDString());

			TREEITEMDATA *pStartData = new TREEITEMDATA;
			pStartData->m_procId = pStartPair->GetProcID();
			m_vTreeItemData.push_back(pStartData);
			m_treeProcess.SetItemData(hStartRoot,(DWORD)pStartData);
			LoadSubTree(hStartRoot,pStartPair);
				
		}
		
	}
	else
	{	
		CString strProc = pStartPair->GetProcID().GetIDString();
		HTREEITEM hStartRoot = m_treeProcess.InsertItem(strProc);

		TREEITEMDATA *pStartData = new TREEITEMDATA;
		pStartData->m_procId = pStartPair->GetProcID();

		m_vTreeItemData.push_back(pStartData);
		m_treeProcess.SetItemData(hStartRoot,(DWORD)pStartData);

		LoadSubTree(hStartRoot,pStartPair);

	}
	
	CollapsedTree();
	if( m_hInterestItem )
	{
		m_treeProcess.SelectItem( m_hInterestItem );
		if( !m_treeProcess.SelectSetFirstVisible( m_hInterestItem ) )
		{
			m_treeProcess.EnsureVisible( m_hInterestItem );
		}

	}

	m_hPocessSelItem = m_treeProcess.GetSelectedItem(); 		// make sure no item is selected
	SetProcessTreeToolBar();
}


void CProcessDefineDlg::LoadSubTree(HTREEITEM hStartRoot, CProcessorDestinationList *startPair)
{
	CProcessorDestinationList *tempPair = startPair;
	int iDestCount = tempPair->GetDestCount();
	for(int i=0; i<iDestCount; ++i )
	{
		if( !IfHaveCircle( hStartRoot, tempPair->GetDestProcAt( i ).GetProcID() ) )
		{

			TREEITEMDATA* pTempData = new TREEITEMDATA;
			pTempData->m_iSpecialHandleType = 0;
			pTempData->m_procId = tempPair->GetDestProcAt( i ).GetProcID();
			pTempData->m_arcInfo = tempPair->GetDestProcArcInfo( i );
			m_vTreeItemData.push_back( pTempData );
			//CString sPercent;
			//sPercent.Format("( %d%% )", pTempData->m_arcInfo->GetProbality() );								
			//HTREEITEM hItem = m_treeProcess.InsertItem( pTempData->m_procId.GetIDString() + GetOtherInfo( pTempData->m_arcInfo )+sPercent , _hItem );
			HTREEITEM hItem = m_treeProcess.InsertItem( pTempData->m_procId.GetIDString() + GetArcInfoDescription(pTempData->m_arcInfo) , hStartRoot );

			m_treeProcess.SetItemData( hItem, (DWORD)pTempData );
			LoadSubTree( hItem );
			m_treeProcess.Expand( hItem,TVE_EXPAND   );				

		}
		else // have a circle
		{
			TREEITEMDATA* pTempData = new TREEITEMDATA;
			pTempData->m_iSpecialHandleType = 1;
			pTempData->m_procId = tempPair->GetDestProcAt( i ).GetProcID();
			pTempData->m_arcInfo = tempPair->GetDestProcArcInfo( i );
			m_vTreeItemData.push_back( pTempData );

			//CString sPercent;
			//sPercent.Format("( %d%% )", pTempData->m_arcInfo->GetProbality() );								
			//HTREEITEM hItem = m_treeProcess.InsertItem( pTempData->m_procId.GetIDString() + GetOtherInfo( pTempData->m_arcInfo ) , _hItem );	
			
			HTREEITEM hItem = m_treeProcess.InsertItem( pTempData->m_procId.GetIDString() + GetArcInfoDescription(pTempData->m_arcInfo) , hStartRoot );			
			m_treeProcess.SetItemData( hItem, (DWORD)pTempData );


			m_treeProcess.SetItemData (m_treeProcess.InsertItem(CS_CYCLIC_LABEL, hItem ), NULL );
			m_treeProcess.Expand( hItem,TVE_EXPAND   );
			m_treeProcess.Expand( hStartRoot,TVE_EXPAND   );
		}
	}
	m_treeProcess.Expand( hStartRoot,TVE_EXPAND   );
}
void CProcessDefineDlg::LoadSubTree(HTREEITEM _hItem)
{
	if( !m_hInterestItem )
	{
		if( IsInterestNode( _hItem ) )
		{
			m_hInterestItem = _hItem;
		}
	}

	TREEITEMDATA* pTempData = (TREEITEMDATA*)m_treeProcess.GetItemData( _hItem );
	//// TRACE("%s\n", pTempData->m_procId.GetIDString() );
	CProcessorDestinationList* tempPair = m_pCurFlow->GetEqualFlowPairAt( pTempData->m_procId );
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
				m_vTreeItemData.push_back( pTempData );
				//CString sPercent;
				//sPercent.Format("( %d%% )", pTempData->m_arcInfo->GetProbality() );								
				//HTREEITEM hItem = m_treeProcess.InsertItem( pTempData->m_procId.GetIDString() + GetOtherInfo( pTempData->m_arcInfo )+sPercent , _hItem );						
				HTREEITEM hItem = m_treeProcess.InsertItem( pTempData->m_procId.GetIDString() + GetArcInfoDescription(pTempData->m_arcInfo) , _hItem );				
				m_treeProcess.SetItemData( hItem, (DWORD)pTempData );
				LoadSubTree( hItem );
				m_treeProcess.Expand( hItem,TVE_EXPAND   );				
				
			}
			else // have a circle
			{
				TREEITEMDATA* pTempData = new TREEITEMDATA;
				pTempData->m_iSpecialHandleType = 1;
				pTempData->m_procId = tempPair->GetDestProcAt( i ).GetProcID();
				pTempData->m_arcInfo = tempPair->GetDestProcArcInfo( i );
				m_vTreeItemData.push_back( pTempData );
				
				//CString sPercent;
				//sPercent.Format("( %d%% )", pTempData->m_arcInfo->GetProbality() );								
				//HTREEITEM hItem = m_treeProcess.InsertItem( pTempData->m_procId.GetIDString() + GetOtherInfo( pTempData->m_arcInfo ) , _hItem );	
				HTREEITEM hItem = m_treeProcess.InsertItem( pTempData->m_procId.GetIDString() + GetArcInfoDescription(pTempData->m_arcInfo) , _hItem );

				m_treeProcess.SetItemData( hItem, (DWORD)pTempData );
				
				
				m_treeProcess.SetItemData (m_treeProcess.InsertItem(CS_CYCLIC_LABEL, hItem ), NULL );
				m_treeProcess.Expand( hItem,TVE_EXPAND   );
				m_treeProcess.Expand( _hItem,TVE_EXPAND   );
			}
		}
		m_treeProcess.Expand( _hItem,TVE_EXPAND   );
	}
}

bool CProcessDefineDlg::IfHaveCircle(HTREEITEM _testItem, const ProcessorID _procID) const
{
	if( !_testItem )
		return false;
	
	TREEITEMDATA* pData = (TREEITEMDATA*)m_treeProcess.GetItemData( _testItem );
	if ( pData->m_procId == _procID )
	{
		return true;
	}
	
	HTREEITEM hFather = m_treeProcess.GetParentItem( _testItem );
	if( !hFather )
	{
		return false;
	}
	
	return IfHaveCircle( hFather, _procID );
}

CString CProcessDefineDlg::GetOtherInfo(CFlowDestination *_pArcInfo)
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

	return sReturnStr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CProcessDefineDlg::OnToolbarbuttonadd() 
{
	// TODO: Add your command handler code here
	int iCount = m_listProcess.GetCount();
	m_listProcess.InsertString(iCount,"");
	m_listProcess.SetCurSel( iCount );
	
	EditItem( iCount );	
    
	m_butSave.EnableWindow( true );
}

void CProcessDefineDlg::OnToolbarbuttondel() 
{
	// TODO: Add your command handler code here
	int iCurSel = m_listProcess.GetCurSel();
	if( iCurSel == LB_ERR )
		return;
	
	int iItemDate = m_listProcess.GetItemData( iCurSel );
	
	CString sName = GetInputTerminal()->m_pSubFlowList->GetProcessUnitAt( iItemDate )->GetProcessUnitName();
	ProcessorID temp;
	temp.SetStrDict( GetInputTerminal()->inStrDict );
	temp.setID( sName );
	if( GetInputTerminal()->m_pPassengerFlowDB->findEntry( temp ) != INT_MAX )
	{
		if( MessageBox("Since the passenger flow use this process, the system will update consistently when you delete this process ! Are you sure to delete the item?",NULL,MB_YESNO|MB_ICONQUESTION) == IDNO )
			return;
		else
		{
			GetInputTerminal()->m_pPassengerFlowDB->removeAllEntries( temp );
		}
	}
	else
	{
		if( MessageBox("Are you sure to delete the item?",NULL,MB_YESNO|MB_ICONQUESTION) == IDNO )
			return;
		else
		{
			GetInputTerminal()->m_pPassengerFlowDB->removeAllEntries( temp );
		}
	}
	
	
	
	// make sure 
	
	// delete the item form  database;
	GetInputTerminal()->m_pSubFlowList->DeleteProcessUnitAt( iItemDate );
	// reload the process list
	LoadProcessData();
	
	m_listProcess.SetCurSel(-1);
	OnSelchangeListProcess();
	m_butSave.EnableWindow( true );
}



void CProcessDefineDlg::EditItem(int _idx)
{
	CRect rectItem;
	CRect rectLB;
	m_listProcess.GetWindowRect( &rectLB );
	ScreenToClient( &rectLB );
	m_listProcess.GetItemRect( _idx, &rectItem );
	
	rectItem.OffsetRect( rectLB.left+2, rectLB.top+1 );
	
	rectItem.right += 1;
	rectItem.bottom += 4;
	
	CString csItemText;
	m_listProcess.GetText( _idx, csItemText );
	
	if( pEdit )
	{
		delete  pEdit;
		pEdit = NULL;
	}
	CSmartEdit* pEdit = new CSmartEdit( csItemText );
	DWORD dwStyle = ES_LEFT | WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL |ES_UPPERCASE ;
	pEdit->Create( dwStyle, rectItem, this, IDC_IPEDIT );	
	
	m_toolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD,FALSE );
	m_toolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );	
	//m_toolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONEDIT,FALSE );	
}	


LONG CProcessDefineDlg::OnEndEdit( WPARAM p_wParam, LPARAM p_lParam )
{
	if( p_lParam != IDC_IPEDIT )
		return 0;
	
	int iCurSel = m_listProcess.GetCurSel();
	if( iCurSel == LB_ERR )
		return 0;
	
	CString csStr( (LPTSTR)p_wParam );
	if( csStr.IsEmpty() )
	{
		MessageBox( "Empty String", "Error", MB_OK|MB_ICONWARNING );
		m_listProcess.DeleteString( iCurSel );
		m_listProcess.SetCurSel( -1 );
	}
	else
	{
		// can not have space and "'"
		csStr.TrimLeft();
		csStr.TrimRight();
		csStr.Remove(_T(','));
		csStr.Replace( _T(' '), _T('_') );
		csStr.MakeUpper();
		
		// still exist 
		if( IfExistInProcessor( csStr) || GetInputTerminal()->m_pSubFlowList->IfProcessUnitExist( csStr ) )
		{
			CString strMsg;
			strMsg.Format("%s\r\n the name still exist,can not use the name!",csStr );
			MessageBox(strMsg, NULL,MB_OK|MB_ICONWARNING );
			m_listProcess.DeleteString( iCurSel );
			m_listProcess.SetCurSel( -1 );
		}
		// add a new item 
		else
		{
			m_listProcess.InsertString( iCurSel,(LPTSTR) p_wParam );
			m_listProcess.DeleteString( iCurSel +1 );
			m_listProcess.SetCurSel( iCurSel );
			
			// insert a new item to database
			CSubFlow m_ProcessFlow( GetInputTerminal() );
			m_ProcessFlow.SetProcessUnitName( csStr );

			ProcessorID tempid;
			tempid.SetStrDict(GetInputTerminal()->inStrDict );
			tempid.setID((LPCTSTR)"PROCESSSTART");

			CProcessorDestinationList *pStartPair = new CProcessorDestinationList(GetInputTerminal());
			pStartPair->ClearAllDestProc();			
			pStartPair->SetProcID(tempid);
			((CSingleProcessFlow *)m_ProcessFlow.GetInternalFlow())->SetStartPair(pStartPair);
			GetInputTerminal()->m_pSubFlowList->AddProcessUnit( m_ProcessFlow );
			m_listProcess.SetItemData( iCurSel, GetInputTerminal()->m_pSubFlowList->GetProcessUnitCount()-1 );
		}
	}
	// set tool bar status
	OnSelchangeListProcess();
	
	if( pEdit )
	{
		delete  pEdit;
		pEdit = NULL;
	}
	return 0;
}




////////////////////////////////////////////////////////////////////////////////////////
// drag and drop
void CProcessDefineDlg::OnBegindragTreeAllprocessor(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	//////////////////////////////////////////////////////////////////
	m_hItemDragSrc = pNMTreeView->itemNew.hItem;
	m_hItemDragDes = NULL;
	
	if(m_pDragImage)
		delete m_pDragImage;
	
	m_pDragImage = m_treeAllProcessor.CreateDragImage( m_hItemDragSrc);
	if( !m_pDragImage )
		return;
	
	m_bDragging = true;
	m_pDragImage->BeginDrag ( 0,CPoint(8,8) );
	CPoint  pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter ( GetDesktopWindow (),pt );  
	
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	SetCapture();
}


void CProcessDefineDlg::OnMouseMove(UINT nFlags, CPoint point) 
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
		m_treeProcess.GetWindowRect( &rectProcessTree );
		//ClientToScreen( &rectProcessTree );
		if( rectProcessTree.PtInRect( pt) )	
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			CPoint ptTest( point );
			ClientToScreen(&ptTest);
			m_treeProcess.ScreenToClient(&ptTest);
			
			// TRACE("x = %d, y = %d \r\n", ptTest.x,ptTest.y );
			hItem = m_treeProcess.HitTest( ptTest,&flags);
			if( hItem != NULL )
			{
				// TRACE( "ENTERY" );
				m_treeProcess.SelectDropTarget( hItem );
				m_hItemDragDes = hItem;
			}
			else
			{
				m_treeProcess.SelectDropTarget( NULL );
				m_hItemDragDes = NULL;
			}
		}
		else
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		}
		m_pDragImage->DragShowNolock (TRUE);
		m_treeAllProcessor.Invalidate(FALSE);
	}
	
	CDialog::OnMouseMove(nFlags, point);
}

void CProcessDefineDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_bDragging )
	{
		CPoint pt (point);
		ClientToScreen (&pt);
		
		CRect rectProcessTree;
		m_treeProcess.GetWindowRect( &rectProcessTree );
		if( rectProcessTree.PtInRect( pt) && m_treeProcess.IsWindowEnabled() )	
		{
			PopProcessDefineMenu( point );
		}
		
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

void CProcessDefineDlg::PopProcessDefineMenu(CPoint point)
{
	CMenu mMenu;
	mMenu.LoadMenu( IDR_MENU_POPUP );
	CMenu* pPopMenu = mMenu.GetSubMenu(27);
	pPopMenu->RemoveMenu(ID_PROCESS_ADD_ISOLATE_NODE,MF_BYCOMMAND);
	
	ASSERT( m_hItemDragSrc != NULL );
	if( m_hItemDragDes == NULL	|| m_hItemDragSrc == NULL)	//
	{
		pPopMenu->EnableMenuItem( ID_PROCESS_ADD_DEST_NODE,MF_GRAYED );
		pPopMenu->EnableMenuItem( ID_PROCESS_ADD_BEFORE,MF_GRAYED );
		pPopMenu->EnableMenuItem( ID_PROESS_ADD_AFTER,MF_GRAYED );
	}
	else 
	{
		TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hItemDragDes );
		if( pItemData== NULL )
		{
			return;
		}
		else
		{
			if (pItemData->m_procId.GetIDString().CompareNoCase("PROCESSSTART") == 0)
			{
				pPopMenu->EnableMenuItem(ID_PROCESS_ADD_BEFORE,MF_GRAYED);
				pPopMenu->EnableMenuItem(ID_PROESS_ADD_AFTER,MF_GRAYED);
			}
			HTREEITEM pParentItem = m_treeProcess.GetParentItem(m_hItemDragDes);
			if (pParentItem != NULL)
			{
				TREEITEMDATA *pFatherData = (TREEITEMDATA*)m_treeProcess.GetItemData( pParentItem );
				if (pFatherData != NULL)
				{
					if (pFatherData->m_procId.GetIDString().CompareNoCase("PROCESSSTART") == 0)
					{
						pPopMenu->EnableMenuItem(ID_PROCESS_ADD_BEFORE,MF_GRAYED);
					}
				}
			}
            
			if( pItemData->m_iSpecialHandleType ==1 )
			{
				pPopMenu->EnableMenuItem( ID_PROCESS_ADD_DEST_NODE,MF_GRAYED );
			}
			if( m_treeProcess.GetParentItem( m_hItemDragDes) == NULL )
			{
				pPopMenu->EnableMenuItem( ID_PROCESS_ADD_BEFORE,MF_GRAYED );
			}
			if( !m_treeProcess.ItemHasChildren( m_hItemDragDes) )
			{
				pPopMenu->EnableMenuItem( ID_PROESS_ADD_AFTER,MF_GRAYED );
			}
		}
	}
	
	ClientToScreen( &point );
	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
}

void CProcessDefineDlg::OnProcessAddIsolateNode() 
{
	// TODO: Add your command handler code here
	
	
	
	ASSERT( m_pCurFlow!= NULL );
	
	int iIdx = m_treeAllProcessor.GetItemData( m_hItemDragSrc );
	ASSERT( iIdx>=0 && iIdx < static_cast<int>(m_vectProcessorID.size()) );
	
	ProcessorID id = m_vectProcessorID[iIdx];
	
	m_pCurFlow->AddIsolatedProc( id );
	m_pCurFlow->SetChangedFlag( true );
	BuildInterestInPath( m_hItemDragDes );
	m_butSave.EnableWindow( true );
	ReloadProcessFlow();
	
}

void CProcessDefineDlg::SubFlowAddDestNode(HTREEITEM hItem)
{
	ASSERT( m_pCurFlow!= NULL );
	ASSERT( m_hItemDragDes != NULL );
	ASSERT(m_hItemDragSrc != NULL);
	if (m_hItemDragSrc == NULL)
		return;

	int iIdx = m_treeAllProcessor.GetItemData( m_hItemDragSrc );
	ASSERT( iIdx>=0 && iIdx < static_cast<int>(m_vectProcessorID.size()) );

	ProcessorID id = m_vectProcessorID[iIdx];

	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeProcess.GetItemData( hItem );
	ASSERT( pData );

	CFlowDestination tempDestInfo;
	tempDestInfo.SetProcID( id );

	if (pData->m_procId.GetIDString().CompareNoCase("PROCESSSTART") == 0)
	{
		CProcessorDestinationList *pStartPair = ((CSingleProcessFlow *)m_pCurFlow)->GetStartPair();
		if (pStartPair == NULL)
			return;
		pStartPair->AddDestProc(tempDestInfo);
		pStartPair->EvenPercent();

		int iIdx = m_treeAllProcessor.GetItemData( m_hItemDragSrc );
		ASSERT( iIdx>=0 && iIdx < static_cast<int>(m_vectProcessorID.size()) );

		ProcessorID id = m_vectProcessorID[iIdx];

		m_pCurFlow->AddIsolatedProc( id );
	}
	else
	{
		// check if exist a circle
		CSinglePaxTypeFlow tmpFlow( *m_pCurFlow );
		tmpFlow.AddDestProc( pData->m_procId,tempDestInfo);
		tmpFlow.GetEqualFlowPairAt( pData->m_procId )->SetAllDestInherigeFlag( 0 );
		tmpFlow.SetChangedFlag( true );

		tmpFlow.ResetDesitinationFlag(false);
		if(HandleSingleFlowLogic::IfHaveCircleFromProc(&tmpFlow,pData->m_procId))
		{
			MessageBox("If you add the processor,will make a circle!\r\nSo can not add the processor",NULL, MB_OK|MB_ICONINFORMATION );
			return;
		}


		m_pCurFlow->AddDestProc( pData->m_procId, tempDestInfo );
		m_pCurFlow->GetEqualFlowPairAt( pData->m_procId )->SetAllDestInherigeFlag( 0 );
	}
}

void CProcessDefineDlg::OnProcessAddDestNode() 
{
	// TODO: Add your command handler code here
	
	if(!m_hItemDragDes || !m_hItemDragSrc)
		return;

	if (m_vCopyItem.empty())
	{
		SubFlowAddDestNode(m_hItemDragDes);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			SubFlowAddDestNode(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}


	m_pCurFlow->SetChangedFlag( true );
	
	m_butSave.EnableWindow();
	BuildInterestInPath( m_hItemDragDes );
	ReloadProcessFlow();
	
}

void CProcessDefineDlg::SubFlowInsertBeforeNode(HTREEITEM hItem)
{
	// TODO: Add your command handler code here
	ASSERT( m_pCurFlow!= NULL );
	ASSERT(m_hItemDragSrc != NULL);
	if(m_hItemDragSrc == NULL)
		return;

	int iIdx = m_treeAllProcessor.GetItemData( m_hItemDragSrc );
	ASSERT( iIdx>=0 && iIdx < static_cast<int>(m_vectProcessorID.size()) );

	ProcessorID id = m_vectProcessorID[iIdx];
	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeProcess.GetItemData( hItem );
	ASSERT( pData );

	HTREEITEM hFather = m_treeProcess.GetParentItem( hItem );	
	TREEITEMDATA* pFatherData = (TREEITEMDATA* )m_treeProcess.GetItemData( hFather );
	ASSERT( pFatherData ); 
	CFlowDestination temp;
	temp.SetProcID( id );

	if (pFatherData->m_procId.GetIDString().CompareNoCase("PROCESSSTART") == 0)
	{

		m_pCurFlow->AddIsolatedProc( id );

		CProcessorDestinationList *pStartPair = ((CSingleProcessFlow *)m_pCurFlow)->GetStartPair();
		if (pStartPair == NULL)
			return;
		pStartPair->AddDestProc(temp);
		pStartPair->EvenPercent();
	}
	else
	{
		// check if exist a circle
		CSinglePaxTypeFlow tmpFlow( *m_pCurFlow );
		tmpFlow.InsertBetween( pFatherData->m_procId, pData->m_procId, temp );
		tmpFlow.SetChangedFlag( true );


		if(HandleSingleFlowLogic::IfHaveCircleFromProc(&tmpFlow,pFatherData->m_procId))
		{

			MessageBox("If you add the processor,will make a circle!\r\nSo can not add the processor",NULL, MB_OK|MB_ICONINFORMATION );
			return;
		}

		m_pCurFlow->InsertBetween( pFatherData->m_procId, pData->m_procId, temp );
	}

	
}

void CProcessDefineDlg::OnProcessAddBefore() 
{
	if(!m_hItemDragDes || ! m_hItemDragSrc)
		return;

	if (m_vCopyItem.empty())
	{
		SubFlowInsertBeforeNode(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			SubFlowInsertBeforeNode(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}



	m_pCurFlow->SetChangedFlag( true );
	m_butSave.EnableWindow(true);
	
	int iIdx = m_treeAllProcessor.GetItemData( m_hItemDragSrc );
	ASSERT( iIdx>=0 && iIdx < static_cast<int>(m_vectProcessorID.size()) );

	ProcessorID id = m_vectProcessorID[iIdx];

	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeProcess.GetItemData( m_hItemDragDes );
	HTREEITEM hFather = m_treeProcess.GetParentItem( m_hItemDragDes );	
	BuildInterestInPath( hFather );
	m_vFocusInPath.insert(m_vFocusInPath.begin(), id );
	m_vFocusInPath.insert(m_vFocusInPath.begin(), pData->m_procId );
	ReloadProcessFlow();
	
}

void CProcessDefineDlg::SubFlowInsertAfterNode(HTREEITEM hItem)
{

	ASSERT( m_pCurFlow!= NULL );

	int iIdx = m_treeAllProcessor.GetItemData( m_hItemDragSrc );
	ASSERT( iIdx>=0 && iIdx < static_cast<int>(m_vectProcessorID.size()) );

	ProcessorID id = m_vectProcessorID[iIdx];

	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeProcess.GetItemData( m_hItemDragDes );
	ASSERT( pData );

	CFlowDestination temp;
	temp.SetProcID( id );

	if (pData->m_procId.GetIDString().CompareNoCase("PROCESSSTART") == 0)
	{
// 		m_pCurFlow->AddIsolatedProc( id );
// 	
		CProcessorDestinationList *pStartPair = ((CSingleProcessFlow *)m_pCurFlow)->GetStartPair();
		if (pStartPair == NULL)
			return;
// 		pStartPair->AddDestProc(temp);
// 		pStartPair->EvenPercent();
	}
	//else
	{
		// check if exist a circle
		CSinglePaxTypeFlow tmpFlow( *m_pCurFlow );
		tmpFlow.InsertProceoosorAfter( pData->m_procId, id );
		tmpFlow.SetChangedFlag( true );


		if( HandleSingleFlowLogic::IfHaveCircleFromProc(&tmpFlow,pData->m_procId))
		{
			MessageBox("If you add the processor,will make a circle!\r\nSo can not add the processor",NULL, MB_OK|MB_ICONINFORMATION );
			return;
		}


		m_pCurFlow->InsertProceoosorAfter( pData->m_procId, id );
	}
}

void CProcessDefineDlg::OnProessAddAfter() 
{
	// TODO: Add your command handler code here
	
	if(!m_hItemDragDes)
		return;

	if (m_vCopyItem.empty())
	{
		SubFlowInsertAfterNode(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			SubFlowInsertAfterNode(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_pCurFlow->SetChangedFlag( true );
	m_butSave.EnableWindow( true );
	BuildInterestInPath( m_hItemDragDes );
	ReloadProcessFlow();
	
	
}

void CProcessDefineDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	CRect rcTree;
	m_treeProcess.GetWindowRect(&rcTree);
	if (!rcTree.PtInRect(point)) 
		return;
	
	m_treeProcess.SetFocus();   // Set focus
	
	CPoint pt = point;
	m_treeProcess.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hItem = m_treeProcess.HitTest(pt, &iRet);
	
	if( hItem == NULL )
		return;
	
	//m_treeProcess.SetCheck( hItem, true );
	m_hRClickItem = hItem;
	m_treeProcess.SelectItem(m_hRClickItem);
	TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( hItem );
	if( !pItemData )
		return;

	CMenu m_Menu;
	m_Menu.LoadMenu( IDR_MENU_POPUP );
	CMenu* pPopMenu = m_Menu.GetSubMenu( 28 );

	pPopMenu->RemoveMenu(ID_PROCESS_ONETOONE,MF_BYCOMMAND);
	pPopMenu->RemoveMenu(ID_PROCESS_EVENPERCENT,MF_BYCOMMAND);
/*
	if (pItemData->m_procId.GetIDString().CompareNoCase("PROCESSSTART") == 0)
	{
		pPopMenu->EnableMenuItem( ID_PERCENT_PERCENTSPLIT, MF_GRAYED );
		pPopMenu->EnableMenuItem( ID_PERCENT_EVENREMAINING, MF_GRAYED );
		pPopMenu->EnableMenuItem( ID_CHANNEL_ON, MF_GRAYED );
		pPopMenu->EnableMenuItem( ID_CHANNEL_OFF, MF_GRAYED );
		pPopMenu->EnableMenuItem( ID_PIPES_USERSELECT, MF_GRAYED );
		pPopMenu->EnableMenuItem( ID_PIPES_ENABLEAUTOMATIC, MF_GRAYED );
	}
*/
	if( m_pCurFlow->GetFlowPairCount() <= 0 || 
		1 == pItemData->m_iSpecialHandleType )
	{
		pPopMenu->EnableMenuItem( ID_PROCESS_TAKEOVER, MF_GRAYED );
	}
	
	if( m_treeProcess.GetParentItem( m_hRClickItem ) == NULL )	// root node can not one to one
	{
		pPopMenu->EnableMenuItem( ID_PERCENT_PERCENTSPLIT, MF_GRAYED );
		pPopMenu->EnableMenuItem( ID_PERCENT_EVENREMAINING, MF_GRAYED );
		pPopMenu->EnableMenuItem( ID_CHANNEL_ON, MF_GRAYED );
		pPopMenu->EnableMenuItem( ID_CHANNEL_OFF, MF_GRAYED );
		pPopMenu->EnableMenuItem( ID_PD_CHANNELCONDITION_1X1_END, MF_GRAYED );
		pPopMenu->EnableMenuItem( ID_PD_CHANNELCONDITION_1X1_START, MF_GRAYED );
		pPopMenu->EnableMenuItem( ID_PD_CHANNELCONDITION_1X1_CLEAR, MF_GRAYED );
		pPopMenu->EnableMenuItem( ID_PIPES_USERSELECT, MF_GRAYED );
		pPopMenu->EnableMenuItem( ID_PIPES_ENABLEAUTOMATIC, MF_GRAYED );
		pPopMenu->EnableMenuItem(ID_PAXFLOW_COPYPROCESSOR,MF_GRAYED);
		pPopMenu->EnableMenuItem(ID_PAXFLOW_ADDCOPY,MF_GRAYED);
		pPopMenu->EnableMenuItem(ID_PAXFLOW_INSERTCOPY,MF_GRAYED);
//		pPopMenu->EnableMenuItem( ID_PROCESS_ONETOONE, MF_GRAYED );	
//		pPopMenu->EnableMenuItem( ID_PROCESS_EVENPERCENT, MF_GRAYED );	
	}
	else
	{	
//		CMenu* pMenuFlowCondition;
		CMenu* pMenuChannelCondition;
		CMenu* pMenuRouteCondition;
		// Flow condition
//		pMenuFlowCondition=pPopMenu->GetSubMenu(2);
		pMenuChannelCondition=pPopMenu->GetSubMenu(4);
		pMenuRouteCondition=pPopMenu->GetSubMenu(5);

		pMenuChannelCondition->CheckMenuRadioItem(ID_CHANNEL_ON,ID_CHANNEL_OFF,
			pItemData->m_arcInfo->GetOneToOneFlag()?ID_CHANNEL_ON:ID_CHANNEL_OFF,MF_BYCOMMAND);
		if (NonChannel != pItemData->m_arcInfo->GetOneXOneState())
		{
			pMenuChannelCondition->CheckMenuRadioItem(ID_PD_CHANNELCONDITION_1X1_START,ID_PD_CHANNELCONDITION_1X1_CLEAR,
				(pItemData->m_arcInfo->GetOneXOneState() == 1)?ID_PD_CHANNELCONDITION_1X1_START:ID_PD_CHANNELCONDITION_1X1_END,MF_BYCOMMAND);
		}
		else
		{
			pMenuChannelCondition->CheckMenuRadioItem(ID_PD_CHANNELCONDITION_1X1_START,ID_PD_CHANNELCONDITION_1X1_CLEAR,
				ID_PD_CHANNELCONDITION_1X1_CLEAR ,MF_BYCOMMAND);			
		}

//		CMenu* pPipes=pMenuRouteCondition->GetSubMenu(1);
		pMenuRouteCondition->CheckMenuItem(0,MF_BYPOSITION|(pItemData->m_arcInfo->GetTypeOfUsingPipe()==2?MF_CHECKED:MF_UNCHECKED));
		pMenuRouteCondition->CheckMenuItem(1,MF_BYPOSITION|(pItemData->m_arcInfo->GetTypeOfUsingPipe()==1?MF_CHECKED:MF_UNCHECKED));

		pPopMenu->EnableMenuItem(ID_PAXFLOW_COPYPROCESSOR,MF_GRAYED);
		pPopMenu->EnableMenuItem(ID_PAXFLOW_ADDCOPY,MF_GRAYED);
		pPopMenu->EnableMenuItem(ID_PAXFLOW_INSERTCOPY,MF_GRAYED);
		if (!m_vCopyItem.empty())
		{
			pPopMenu->EnableMenuItem(ID_PAXFLOW_COPYPROCESSOR,MF_ENABLED);
		}
		if (!m_vCopyProcessors.empty())
		{
			pPopMenu->EnableMenuItem(ID_PAXFLOW_ADDCOPY,MF_ENABLED);
			pPopMenu->EnableMenuItem(ID_PAXFLOW_INSERTCOPY,MF_ENABLED);
		}

	}
	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
}

void CProcessDefineDlg::SubFlowCurSourceLink(HTREEITEM hItem)
{
	ASSERT( m_pCurFlow!= NULL );

	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeProcess.GetItemData( hItem );
	ASSERT( pData );

	HTREEITEM hFather = m_treeProcess.GetParentItem( hItem );	
	TREEITEMDATA* pFatherData = NULL;
	// modified in 2003-2-9
	if( hFather )
	{
		pFatherData = (TREEITEMDATA* )m_treeProcess.GetItemData( hFather );
		ASSERT( pFatherData );
		if (pFatherData->m_procId.GetIDString().CompareNoCase("PROCESSSTART") == 0)
		{
			m_pCurFlow->CutLinkWithSourceProc( pData->m_procId, pData->m_procId );
			CProcessorDestinationList *pStartPair = ((CSingleProcessFlow *)m_pCurFlow)->GetStartPair();
			if (pStartPair == NULL)
				return;
			pStartPair->DeleteDestProc(pData->m_procId);

			CString strProc = pData->m_procId.GetIDString();
			int ij = pStartPair->GetDestCount();

		}
		else
		{
			m_pCurFlow->CutLinkWithSourceProc( pFatherData->m_procId, pData->m_procId );
		}
	}
	else
	{
		CProcessorDestinationList *pStartPair = ((CSingleProcessFlow *)m_pCurFlow)->GetStartPair();
		if (pStartPair == NULL)
			return;
		int nCount = pStartPair->GetDestCount();

		while(nCount > 0)
		{
			ProcessorID ProcID = pStartPair->GetDestProcArcInfo(0)->GetProcID();
			m_pCurFlow->CutLinkWithSourceProc( ProcID, ProcID );
			pStartPair->DeleteDestProc(ProcID);
			nCount = pStartPair->GetDestCount();
		}
	}
}

void CProcessDefineDlg::OnProcessCutSourceLink() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_treeProcess.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		SubFlowCurSourceLink(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			SubFlowCurSourceLink(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	HTREEITEM hFather = m_treeProcess.GetParentItem( m_hRClickItem );
	if( hFather )
	{
		BuildInterestInPath( hFather );
	}
	else
	{
		m_vFocusInPath.clear();
	}

	m_pCurFlow->SetChangedFlag( true ); 	
	m_butSave.EnableWindow( true );
	ReloadProcessFlow();
}

void CProcessDefineDlg::SubFlowDeleteNodeOnly(HTREEITEM hItem)
{
	ASSERT( m_pCurFlow!= NULL );

	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeProcess.GetItemData( hItem );
	ASSERT( pData );

	HTREEITEM hFather = m_treeProcess.GetParentItem( hItem );
	TREEITEMDATA* pFatherData = NULL;
	if( hFather )
	{
		pFatherData =(TREEITEMDATA* )m_treeProcess.GetItemData( hFather );
		ASSERT( pFatherData ); 
		if (pFatherData->m_procId.GetIDString().CompareNoCase("PROCESSSTART") == 0)
		{
			((CSingleProcessFlow *)m_pCurFlow)->TakeOverDestProc(pFatherData->m_procId, pData->m_procId);
		}
		else
		{
			m_pCurFlow->TakeOverDestProc( pFatherData->m_procId, pData->m_procId );
		}
	}
}

void CProcessDefineDlg::OnProcessTakeover() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_treeProcess.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		SubFlowDeleteNodeOnly(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			SubFlowDeleteNodeOnly(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	HTREEITEM hFather = m_treeProcess.GetParentItem( m_hRClickItem );
	m_pCurFlow->SetChangedFlag( true );
	m_butSave.EnableWindow();

	BuildInterestInPath( hFather );
	ReloadProcessFlow();

}

void CProcessDefineDlg::SubFlowSetOneToOne(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_treeProcess.GetItemData( hItem );
	ASSERT( pData );
	pData->m_arcInfo->SetOneToOneFlag( !pData->m_arcInfo->GetOneToOneFlag() );
}

void CProcessDefineDlg::OnProcessOnetoone() 
{
	ASSERT( m_pCurFlow!= NULL );
	if(!m_hRClickItem)
		m_hRClickItem = m_treeProcess.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		SubFlowSetOneToOne(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			SubFlowSetOneToOne(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_pCurFlow->SetChangedFlag( true );
	m_butSave.EnableWindow();
	
	BuildInterestInPath( m_hRClickItem );
	ReloadProcessFlow();	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// splitter
void CProcessDefineDlg::InitSpiltter()
{
	CRect rc;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_SPLITTER);
	pWnd->GetWindowRect(rc);
	ScreenToClient(rc);
	m_wndSplitter.Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_STATIC_SPLITTER);
	m_wndSplitter.SetRange(10,800);
}

LRESULT CProcessDefineDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_INPLACE_SPIN)
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;

		SetPercent( pst->iPercent );
		return TRUE;
	}	
	if (message == WM_NOTIFY)
	{
		if (wParam == IDC_STATIC_SPLITTER )
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			DoResize(pHdr->delta);
		}
	}

	return CDialog::DefWindowProc(message, wParam, lParam);	
}


void CProcessDefineDlg::DoResize(int delta)
{
	CSplitterControl::ChangeWidth(&m_treeAllProcessor, delta);
	CSplitterControl::ChangeWidth(&m_toolBar,-delta,CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_staticFrame, -delta, CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_listProcess,-delta,CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_toolBarProcessTree,-delta,CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_staticTreeFrame,-delta,CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_treeProcess,-delta,CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_static1,-delta,CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_static2,-delta,CW_RIGHTALIGN);
	
	/*
	CRect rc;
	m_coolBtn.GetWindowRect( &rc );
	if( rc.Width()+delta<= 120 )
	CSplitterControl::ChangeWidth(&m_coolBtn, delta);
	*/
	Invalidate();
}



//////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// add button
void CProcessDefineDlg::OnClickDropdown()
{
	CRect rc;
	
	CMenu mMenu;
	mMenu.LoadMenu( IDR_MENU_POPUP );
	CMenu* pPopMenu = mMenu.GetSubMenu(27);
	
	//ASSERT( m_hItemDragSrc != NULL );
	if( m_hItemDragSrc == NULL )
		return;
	pPopMenu->RemoveMenu(4,MF_BYPOSITION);
	pPopMenu->RemoveMenu(4,MF_BYPOSITION);
	if( m_hItemDragDes == NULL	)	//
	{
		pPopMenu->EnableMenuItem( ID_PROCESS_ADD_DEST_NODE,MF_GRAYED );
		pPopMenu->EnableMenuItem( ID_PROCESS_ADD_BEFORE,MF_GRAYED );
		pPopMenu->EnableMenuItem( ID_PROESS_ADD_AFTER,MF_GRAYED );
	}
	else if( m_treeProcess.GetParentItem( m_hItemDragDes) == NULL )
	{
		pPopMenu->EnableMenuItem( ID_PROCESS_ADD_BEFORE,MF_GRAYED );
	}
	pPopMenu->TrackPopupMenu(TPM_LEFTALIGN, rc.left,rc.bottom, this);	
	
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// Process Define Tool Bar
void CProcessDefineDlg::OnSelchangedTreeProcess(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_hPocessSelItem = m_treeProcess.GetSelectedItem();
	m_hRClickItem = m_hPocessSelItem;
	SetProcessTreeToolBar();

	*pResult = 0;
	//set the status of the button in the tool bar
	if (m_hPocessSelItem  == NULL)
		return;
	
	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeProcess.GetItemData( m_hPocessSelItem );
	ASSERT( pData );
	
	if(pData->m_procId.GetIDString().CompareNoCase("PROCESSSTART") == 0)
		return;
	if (pData->m_arcInfo->GetTypeOfUsingPipe() == 1)
	{
		TBBUTTONINFO tbBtnInfo;
		tbBtnInfo.cbSize=sizeof(TBBUTTONINFO);
		tbBtnInfo.dwMask=TBIF_IMAGE;
		tbBtnInfo.iImage=11;
		m_toolBarProcessTree.GetToolBarCtrl().SetButtonInfo(ID_PD_PIPE_AUTO,&tbBtnInfo);
	}
	if (pData->m_arcInfo->GetTypeOfUsingPipe() == 0 ||pData->m_arcInfo->GetTypeOfUsingPipe() == 2)
	{
		TBBUTTONINFO tbBtnInfo;
		tbBtnInfo.cbSize=sizeof(TBBUTTONINFO);
		tbBtnInfo.dwMask=TBIF_IMAGE;
		tbBtnInfo.iImage=8;
		m_toolBarProcessTree.GetToolBarCtrl().SetButtonInfo(ID_PD_PIPE_AUTO,&tbBtnInfo);
	}
}

void CProcessDefineDlg::OnSelchangedTreeAllprocessor(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_hAllProcessorSelItem = m_treeAllProcessor.GetSelectedItem();
	m_treeProcess.SetItemState(m_hRClickItem,/*TVIS_FOCUSED |*/TVIS_SELECTED,NULL);
	SetProcessTreeToolBar();
	*pResult = 0;
}

void CProcessDefineDlg::SetProcessTreeToolBar()
{
	SetToolbarAddBut();
	SetToolBarRemoveBut();
	UpdateToolBarState(ID_PD_MODIFYPERCENT);
	UpdateToolBarState(ID_PD_EVEN_PERCENT);
	UpdateToolBarState(ID_PD_PIPE);
	UpdateToolBarState(ID_PD_PIPE_AUTO);
	/*if( m_pCurFlow == NULL || m_hAllProcessorSelItem == NULL)
	{
	// set all disable
	m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_NEW,false );
	m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_DEST,false );
	m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_BEFORE,false );
	m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_AFTER,false );
	/////////////////////////////////////////////////////////////////////////
	m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_PRUNE,false );
	m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_REMOVE_PROCESS,false );
	
	  return;
	  }
	  
		if( m_hPocessSelItem == NULL )
		{
		// set add disable
		// set remove able
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_NEW,true );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_DEST,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_BEFORE,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_AFTER,false );
		/////////////////////////////////////////////////////////////////////////
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_PRUNE,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_REMOVE_PROCESS,false );
		
		  return;
		  }
		  
			m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_NEW,true);
			m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_DEST,true );
			m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_BEFORE,true );
			m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_AFTER,true );
			/////////////////////////////////////////////////////////////////////////
			m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_PRUNE,true );
			m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_REMOVE_PROCESS,true );
			
			  if( m_treeProcess.GetParentItem( m_hPocessSelItem)==NULL )
			  {
			  m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_BEFORE,false );
			  }
			  
				if( !m_treeProcess.ItemHasChildren( m_hPocessSelItem) )
				{
				m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_AFTER,false );
				}
				
				  TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hPocessSelItem );
				  if( !pItemData )
				  {
				  // set all disable
				  m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_NEW,false );
				  m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_DEST,false );
				  m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_BEFORE,false );
				  m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_AFTER,false );
				  /////////////////////////////////////////////////////////////////////////
				  m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_PRUNE,false );
				  m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_REMOVE_PROCESS,false );
				  }
				  else if( 1 == pItemData->m_iSpecialHandleType )
				  {
				  m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_DEST,false );
				  m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_REMOVE_PROCESS,false );		
}*/
}


void CProcessDefineDlg::SetToolbarAddBut()
{
	if( m_pCurFlow == NULL || m_hAllProcessorSelItem == NULL)
	{
		// set all disable
		
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_NEW,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_DEST,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_BEFORE,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_AFTER,false );
		return;
	}
	
	if( m_hPocessSelItem == NULL )
	{
		// set add disable
		// set remove able
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_NEW,true );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_DEST,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_BEFORE,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_AFTER,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton(ID_PD_EVEN_PERCENT,false);
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton(ID_PD_MODIFYPERCENT,false);
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton(ID_PD_PIPE,false);
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton(ID_PD_PIPE_AUTO,false);

		return;
	}
	
	m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_NEW,true);
	m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_DEST,true );
	m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_BEFORE,true );
	m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_AFTER,true );
	/////////////////////////////////////////////////////////////////////////
	if( m_treeProcess.GetParentItem( m_hPocessSelItem)==NULL )
	{
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_BEFORE,false );
		//m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_AFTER,false );
	}
	else
	{
		if (m_treeProcess.GetParentItem(m_treeProcess.GetParentItem( m_hPocessSelItem)) == NULL)
		{
			m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_BEFORE,false );
		}	
	}
	
	if( !m_treeProcess.ItemHasChildren( m_hPocessSelItem) )
	{
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_AFTER,false );
	}
	
	
}

void CProcessDefineDlg::SetToolBarRemoveBut()
{
	if( m_pCurFlow == NULL || m_hPocessSelItem == NULL )
	{
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_PRUNE,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_REMOVE_PROCESS,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ONETOONE,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_1X1START,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_1X1END,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton(ID_PD_EVEN_PERCENT,false);
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton(ID_PD_MODIFYPERCENT,false);
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton(ID_PD_PIPE,false);
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton(ID_PD_PIPE_AUTO,false);
		return;
	}
	
	m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_PRUNE,true );
	m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_REMOVE_PROCESS,true );
	m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ONETOONE,true );
	m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_1X1START,true );
	m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_1X1END,true );
	
	TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hPocessSelItem );
	if( !pItemData )
	{
		// set all disable
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_NEW,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_DEST,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_BEFORE,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_INSERT_AFTER,false );
		/////////////////////////////////////////////////////////////////////////
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_PRUNE,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_REMOVE_PROCESS,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ONETOONE,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_1X1START,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_1X1END,false );
	}
	
	else if( 1 == pItemData->m_iSpecialHandleType )
	{
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ADD_DEST,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_REMOVE_PROCESS,false );		
	}
	if( m_treeProcess.GetParentItem( m_hPocessSelItem) == NULL)
	{
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_ONETOONE,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_1X1START,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton( ID_PD_1X1END,false );
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton(ID_PD_EVEN_PERCENT,FALSE);
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton(ID_PD_MODIFYPERCENT,FALSE);
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton(ID_PD_PIPE,FALSE);
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton(ID_PD_PIPE_AUTO,FALSE);
	}
}


void CProcessDefineDlg::OnPdAddNew() 
{
	m_hItemDragSrc = m_hAllProcessorSelItem;
	m_hItemDragDes = m_hPocessSelItem;
	
	OnProcessAddIsolateNode();
}

void CProcessDefineDlg::OnPdAddDest() 
{
	m_hItemDragSrc = m_hAllProcessorSelItem;
	m_hItemDragDes = m_hPocessSelItem;
	
	OnProcessAddDestNode();
}

void CProcessDefineDlg::OnPdInsertAfter() 
{
	m_hItemDragSrc = m_hAllProcessorSelItem;
	m_hItemDragDes = m_hPocessSelItem;
	
	OnProessAddAfter();
}

void CProcessDefineDlg::OnPdInsertBefore() 
{
	m_hItemDragSrc = m_hAllProcessorSelItem;
	m_hItemDragDes = m_hPocessSelItem;
	
	OnProcessAddBefore();
}

void CProcessDefineDlg::OnPdPrune() 
{
	//m_hItemDragSrc = m_hAllProcessorSelItem;
	//m_hItemDragDes = m_hPocessSelItem;
	m_hRClickItem = m_hPocessSelItem;
	
	OnProcessCutSourceLink();
}

void CProcessDefineDlg::OnPdRemoveProcess() 
{
	//m_hItemDragSrc = m_hAllProcessorSelItem;
	//m_hItemDragDes = m_hPocessSelItem;
	m_hRClickItem = m_hPocessSelItem;
	
	OnProcessTakeover();	
}

void CProcessDefineDlg::OnPdOnetoone() 
{
	// TODO: Add your command handler code here
	m_hRClickItem = m_hPocessSelItem;
	
	OnProcessOnetoone();
}

BOOL CProcessDefineDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
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


void CProcessDefineDlg::SetActiveDoc(CDocument *pDoc)
{
// 	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
// 	m_pDocument=(CTermPlanDoc*)pDoc;
	
	
}

// void CProcessDefineDlg::OnInitialUpdate() 
// {
// 	CDialog::OnInitialUpdate();
// 	
// 	OnInitDialog() ;
// 	m_sizeFormView=GetTotalSize();
// 	// TODO: Add your specialized code here and/or call the base class
// 	
//}

#ifdef _DEBUG
void CProcessDefineDlg::AssertValid() const
{
	CDialog::AssertValid();
}


//CA1Doc* CA1View::GetDocument() // non-debug version is inline
//{
//	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CA1Doc)));
//	return (CA1Doc*)m_pDocument;
//}
#endif //_DEBUG




void CProcessDefineDlg::OnMyok() 
{
	// TODO: Add your control notification handler code here
	//if( m_butSave.IsWindowEnabled() )
	//{
	//	OnButSave();
	//}
	if( m_butSave.IsWindowEnabled() )
	{
		if (GetInputTerminal()->m_pSubFlowList->IfAllFlowValid())
		{
			GetInputTerminal()->m_pSubFlowList->saveDataSet( GetProjectPath() ,true);
			m_butSave.EnableWindow( false );
			CDialog::OnOK();
		}
	}
	else
	{
		CDialog::OnOK();
	}

	//	
	//	SendMessage(WM_CLOSE );
	
}

void CProcessDefineDlg::OnMycancel() 
{
	// TODO: Add your control notification handler code here
	GetInputTerminal()->m_pSubFlowList->loadDataSet( GetProjectPath() );	
	CDialog::OnCancel();
}

void CProcessDefineDlg::OnSize(UINT nType, int cx, int cy) 
{
	
	//set the dlg default width and Height	
	CPoint pt;
	pt.x=400;
	pt.y=200;
	ClientToScreen(&pt);
	
	if(m_sizeLastWnd.cx!=0&&m_sizeLastWnd.cy!=0&&m_staticTreeFrame.GetSafeHwnd()!=NULL)
	{
		if(pt.x<cx&&pt.y <cy/*&& nType != SIZE_MAXIMIZED*/ )
		{			
			int nCxAdd = cx - pt.x;
			int nCyAdd = cy - pt.x;
			
			CRect rectListProcess;
			CRect rectStaticContain;
			CRect rectTreeProcess;
			CRect rectStaticTree;
			CRect rectTreeAllProc;
			
			m_listProcess.GetWindowRect(&rectListProcess);
			m_staticFrame.GetWindowRect(&rectStaticContain);
			m_treeProcess.GetWindowRect(&rectTreeProcess);
			m_staticTreeFrame.GetWindowRect(&rectStaticTree);
			m_treeAllProcessor.GetWindowRect(&rectTreeAllProc);
			
			ScreenToClient(rectListProcess);
			ScreenToClient(rectStaticContain);
			ScreenToClient(rectTreeProcess);
			ScreenToClient(rectStaticTree);
			ScreenToClient(rectTreeAllProc);		


			m_listProcess.SetWindowPos(NULL,
				rectListProcess.left,
				rectListProcess.top,
				cx - rectListProcess.left - 30,
				rectListProcess.Height(),
				SWP_SHOWWINDOW
				);
			
			m_staticFrame.SetWindowPos(NULL,
				rectStaticContain.left,
				rectStaticContain.top,
				cx - rectStaticContain.left - 30,
				rectStaticContain.Height(),
				SWP_SHOWWINDOW
				);
			
			m_treeProcess.SetWindowPos(NULL,
				rectTreeProcess.left,
				rectTreeProcess.top,
				cx - rectTreeProcess.left - 30,
				cy - rectTreeProcess.top - 40,
				SWP_SHOWWINDOW
				);
			
			m_staticTreeFrame.SetWindowPos(NULL,
				rectStaticTree.left,
				rectStaticTree.top,
				cx - rectStaticTree.left - 30,
				cy - rectStaticTree.top - 40,
				SWP_SHOWWINDOW
				);
			
			m_treeAllProcessor.SetWindowPos(NULL,
				rectTreeAllProc.left,
				rectTreeAllProc.top,
				rectTreeAllProc.Width(),
				cy - rectTreeAllProc.top -40  ,
				SWP_SHOWWINDOW
				);			
			
			m_butSave.SetWindowPos(NULL,cx-300,cy-30,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
			m_ok.SetWindowPos(NULL,cx-200,cy-30,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
			m_cancel.SetWindowPos(NULL,cx-100,cy-30,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
			
			m_sizeFormView.cx=cx;
			m_sizeFormView.cy=cy;			
		}
// 		else
// 		{
// 			if (m_sizeFormView.cx>0&&m_sizeFormView.cy>0)
// 			{
// 				SetScrollSizes(MM_TEXT,m_sizeFormView);
// 			}
// 			
// 		}
	}
	m_sizeLastWnd=CSize( cx,cy);	
	CDialog::OnSize(nType, cx, cy);	
	// TODO: Add your message handler code here
}



void CProcessDefineDlg::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	//		InitToolBar();
	
}

void CProcessDefineDlg::OnUpdateBtnDel(CCmdUI* pCmdUI)
{

	int iCurSel = m_listProcess.GetCurSel();
	if( iCurSel == LB_ERR )
	{
		pCmdUI->Enable(FALSE);
	}		
	else
	{
		pCmdUI->Enable(TRUE);	
	}
}

void CProcessDefineDlg::OnUpdateBtnAddNew(CCmdUI* pCmdUI)
{
	if( m_pCurFlow == NULL || m_hAllProcessorSelItem == NULL)
	{
		pCmdUI->Enable(FALSE);
		return;
	}	

	if( m_hPocessSelItem == NULL )
	{
		pCmdUI->Enable(TRUE);
		return ;
	}
	pCmdUI->Enable(TRUE);	

	TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hPocessSelItem );
	if( !pItemData )
	{
		pCmdUI->Enable(FALSE);
	}
	
}

void CProcessDefineDlg::OnUpdateBtnAddDest(CCmdUI* pCmdUI)
{
	if( m_pCurFlow == NULL || m_hAllProcessorSelItem == NULL)
	{
		pCmdUI->Enable(FALSE);
		return ;
	}
	
	if( m_hPocessSelItem == NULL )
	{
		pCmdUI->Enable(FALSE);
		return ;
	}
	
	pCmdUI->Enable(TRUE);	

	TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hPocessSelItem );
	if( !pItemData )
	{
		pCmdUI->Enable(FALSE);
	}	
	else if( 1 == pItemData->m_iSpecialHandleType )
	{
		pCmdUI->Enable(FALSE);
	}	
}

void CProcessDefineDlg::OnUpdateBtnInsertBefore(CCmdUI* pCmdUI)
{
	if( m_pCurFlow == NULL || m_hAllProcessorSelItem == NULL)
	{
		pCmdUI->Enable(FALSE);
		return ;
	}
	
	if( m_hPocessSelItem == NULL )
	{
		pCmdUI->Enable(FALSE);
		return ;
	}
	
	pCmdUI->Enable(TRUE);
	
	if( m_treeProcess.GetParentItem( m_hPocessSelItem)==NULL )
	{
		pCmdUI->Enable(FALSE);		
	}	

	TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hPocessSelItem );
	if( !pItemData )
	{
		pCmdUI->Enable(FALSE);
	}
	
}

void CProcessDefineDlg::OnUpdateBtnInsertAfter(CCmdUI* pCmdUI)
{
	if( m_pCurFlow == NULL || m_hAllProcessorSelItem == NULL)
	{
		pCmdUI->Enable(FALSE);
		return ;
	}
	
	if( m_hPocessSelItem == NULL )
	{
		pCmdUI->Enable(FALSE);
		return ;
	}
	
	pCmdUI->Enable(TRUE);		

	if( !m_treeProcess.ItemHasChildren( m_hPocessSelItem) )
	{
		pCmdUI->Enable(FALSE);		
	}

	TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hPocessSelItem );
	if( !pItemData )
	{
		pCmdUI->Enable(FALSE);
	}
	
}

void CProcessDefineDlg::OnUpdateBtnPrune(CCmdUI* pCmdUI)
{
	if( m_pCurFlow == NULL || m_hPocessSelItem == NULL )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(TRUE);			

	TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hPocessSelItem );
	if( !pItemData )
	{
		pCmdUI->Enable(FALSE);
	}
}

void CProcessDefineDlg::OnUpdateBtnRemoveProcess(CCmdUI* pCmdUI)
{
	if( m_pCurFlow == NULL || m_hPocessSelItem == NULL )
	{
		pCmdUI->Enable(FALSE);
		return;
	}	
	
	pCmdUI->Enable(TRUE);			
	
	TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hPocessSelItem );
	if( !pItemData )
	{
		pCmdUI->Enable(FALSE);
	}	
	else if( 1 == pItemData->m_iSpecialHandleType )
	{
		pCmdUI->Enable(FALSE);
	}
}

void CProcessDefineDlg::OnUpdateBtn1X1Start(CCmdUI* pCmdUI)
{
	if( m_pCurFlow == NULL || m_hPocessSelItem == NULL )
	{
		pCmdUI->Enable(FALSE);
		return;
	}	

	pCmdUI->Enable(TRUE);		

	TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hPocessSelItem );
	if( !pItemData )
	{
		pCmdUI->Enable(FALSE);
	}	

	if( m_treeProcess.GetParentItem( m_hPocessSelItem) == NULL)
	{
		pCmdUI->Enable(FALSE);		
	}
}

void CProcessDefineDlg::OnUpdateBtn1X1End(CCmdUI* pCmdUI)
{
	if( m_pCurFlow == NULL || m_hPocessSelItem == NULL )
	{
		pCmdUI->Enable(FALSE);
		return;
	}	

	pCmdUI->Enable(TRUE);		

	TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hPocessSelItem );
	if( !pItemData )
	{
		pCmdUI->Enable(FALSE);
	}	

	if( m_treeProcess.GetParentItem( m_hPocessSelItem) == NULL)
	{
		pCmdUI->Enable(FALSE);		
	}
}

void CProcessDefineDlg::OnUpdateBtnOntToOne(CCmdUI* pCmdUI)
{
	if( m_pCurFlow == NULL || m_hPocessSelItem == NULL )
	{
		pCmdUI->Enable(FALSE);
		return;
	}	

	pCmdUI->Enable(TRUE);		

	TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hPocessSelItem );
	if( !pItemData )
	{
		pCmdUI->Enable(FALSE);
	}	
	
	if( m_treeProcess.GetParentItem( m_hPocessSelItem) == NULL)
	{
		pCmdUI->Enable(FALSE);		
	}	
}

void CProcessDefineDlg::UpdateToolBarState(int nToolBarID)
{
	if( m_pCurFlow == NULL || m_hPocessSelItem == NULL )
	{
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton(nToolBarID,false);
		return;
	}	

	m_toolBarProcessTree.GetToolBarCtrl().EnableButton(nToolBarID,true);	

	TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hPocessSelItem );
	if( !pItemData )
	{
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton(nToolBarID,false);
	}	

	if( m_treeProcess.GetParentItem( m_hPocessSelItem) == NULL)
	{
		m_toolBarProcessTree.GetToolBarCtrl().EnableButton(nToolBarID,false);
	}
}

void CProcessDefineDlg::OnUpdateBtnPipe(CCmdUI* pCmdUI)
{
	if( m_pCurFlow == NULL || m_hPocessSelItem == NULL )
	{
		pCmdUI->Enable(FALSE);
		return;
	}	

	pCmdUI->Enable(TRUE);		

	TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hPocessSelItem );
	if( !pItemData )
	{
		pCmdUI->Enable(FALSE);
	}	

	if( m_treeProcess.GetParentItem( m_hPocessSelItem) == NULL)
	{
		pCmdUI->Enable(FALSE);		
	}
}

void CProcessDefineDlg::OnUpdateBtnPipeAuto(CCmdUI* pCmdUI)
{
	if( m_pCurFlow == NULL || m_hPocessSelItem == NULL )
	{
		pCmdUI->Enable(FALSE);
		return;
	}	

	pCmdUI->Enable(TRUE);		

	TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hPocessSelItem );
	if( !pItemData )
	{
		pCmdUI->Enable(FALSE);
	}	

	if( m_treeProcess.GetParentItem( m_hPocessSelItem) == NULL)
	{
		pCmdUI->Enable(FALSE);		
	}
}

void CProcessDefineDlg::OnUpdateBtnModifyPercent(CCmdUI* pCmdUI)
{
	if( m_pCurFlow == NULL || m_hPocessSelItem == NULL )
	{
		pCmdUI->Enable(FALSE);
		return;
	}	

	pCmdUI->Enable(TRUE);		

	TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hPocessSelItem );
	if( !pItemData )
	{
		pCmdUI->Enable(FALSE);
	}	

	if( m_treeProcess.GetParentItem( m_hPocessSelItem) == NULL)
	{
		pCmdUI->Enable(FALSE);		
	} 
}

void CProcessDefineDlg::OnUpdateBtnEvenPercent(CCmdUI* pCmdUI)
{
	if( m_pCurFlow == NULL || m_hPocessSelItem == NULL )
	{
		pCmdUI->Enable(FALSE);
		return;
	}	

	pCmdUI->Enable(TRUE);		

	TREEITEMDATA* pItemData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hPocessSelItem );
	if( !pItemData )
	{
		pCmdUI->Enable(FALSE);
	}	

	if( m_treeProcess.GetParentItem( m_hPocessSelItem) == NULL)
	{
		pCmdUI->Enable(FALSE);		
	} 
}

void CProcessDefineDlg::OnProcessModifypercent()
{
	m_hRClickItem = m_hPocessSelItem;
	m_treeProcess.SpinEditLabel(m_hRClickItem);	
//	m_btnSave.EnableWindow();

	
}

void CProcessDefineDlg::SetPercent(int _nPercent)
{
	if(!m_hRClickItem)
		m_hRClickItem = m_treeProcess.GetSelectedItem();
	if (m_hRClickItem == NULL) return;
	m_hModifiedItem = m_hRClickItem;
	// TODO: Add your command handler code here
	if( !m_hModifiedItem )
		return;

	//int nSelIdx = m_listProc.GetCurSel();
	//if( nSelIdx == LB_ERR )
	//	return;

	//CMobileElemConstraintDatabase* pConDB = GetDBBaseOnSelection( nSelIdx );

	//int nDataIdx = m_treeProcess.GetItemData( m_hModifiedItem );
	//HTREEITEM hParItem = m_treeProcess.GetParentItem( m_hModifiedItem );
	//if( hParItem == NULL )
	//	return;

	//int nConIdx = m_treeFlow.GetItemData( hParItem );
	//ProcessorDistribution* pProcDist = (ProcessorDistribution*)pConDB->getDist( nConIdx );
	//pProcDist->setProb( _nPercent, nDataIdx );
	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeProcess.GetItemData( m_hModifiedItem );
	ASSERT( pData );
	pData->m_arcInfo->SetProbality(_nPercent);

	m_pCurFlow->SetChangedFlag( true );
	m_butSave.EnableWindow();

	BuildInterestInPath( m_hRClickItem );
	ReloadProcessFlow();
}


CString CProcessDefineDlg::GetPipeString( CFlowDestination* _pArcInfo )
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

CString CProcessDefineDlg::GetArcInfoDescription( CFlowDestination* _pArcInfo , bool _bNeedDetail /*= false*/,bool* _pbHasComplementData /*=NULL*/)
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
			sQueueTimeStr.Format("%d",_pArcInfo->GetMaxQueueLength() );
			sReturnStr += " Wait Time (<" + sQueueTimeStr + " mins),";
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
		if(_bHasComplementData )

		{
			return sPercent + GetPipeString( _pArcInfo ) + GetOtherInfo( _pArcInfo ) +" ...";
		}
		else
		{
			return  sPercent + GetPipeString( _pArcInfo ) + GetOtherInfo( _pArcInfo );
		}
	}
}

void CProcessDefineDlg::SubFlowEventRemaining(HTREEITEM hItem)
{
	HTREEITEM hFather = m_treeProcess.GetParentItem( hItem );
	if ( hFather == NULL) return ;		
	TREEITEMDATA* pFatherData = (TREEITEMDATA* )m_treeProcess.GetItemData( hFather );
	ASSERT( pFatherData ); 
	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeProcess.GetItemData( hItem );
	ASSERT( pData );
	if (pFatherData->m_procId.GetIDString().CompareNoCase("PROCESSSTART") == 0)
	{
		CProcessorDestinationList *pStartPair = ((CSingleProcessFlow *)m_pCurFlow)->GetStartPair();
		if (pStartPair == NULL)
			return;
		pStartPair->EvenRemainPercent(pData->m_procId);
	}
	else
	{
		m_pCurFlow->EvenRemainDestPercent( pFatherData->m_procId, pData->m_procId );
	}

}

void CProcessDefineDlg::OnProcessEvenpercent() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_treeProcess.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		SubFlowEventRemaining(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			SubFlowEventRemaining(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_butSave.EnableWindow();
	BuildInterestInPath( m_hRClickItem );
	ReloadProcessFlow();	

}

void CProcessDefineDlg::SubFlowEditPipManual(HTREEITEM hItem,CPaxFlowSelectPipes* pSelectPipeDlg)
{
	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeProcess.GetItemData( hItem );
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

void CProcessDefineDlg::OnProcessEditPipe() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_treeProcess.GetSelectedItem();
	if (m_hRClickItem == NULL) return;	

	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeProcess.GetItemData( m_hRClickItem );
	ASSERT( pData );

	CPaxFlowSelectPipes dlg( GetInputTerminal(), pData->m_arcInfo->GetPipeVector(), pData->m_arcInfo->GetTypeOfUsingPipe() );
	if( dlg.DoModal() == IDOK )
	{
		if (m_vCopyItem.empty())
		{
			SubFlowEditPipManual(m_hRClickItem,&dlg);
		}
		else
		{
			for (unsigned i = 0; i < m_vCopyItem.size(); i++)
			{
				TreeNodeInfor& nodeData = m_vCopyItem[i];
				SubFlowEditPipManual(nodeData.m_hTreeNode,&dlg);
			}
			m_vCopyItem.clear();
		}
	}

	m_butSave.EnableWindow();
	m_pCurFlow->SetChangedFlag( true );

	BuildInterestInPath( m_hRClickItem );
	ReloadProcessFlow();			
}

void CProcessDefineDlg::SubFlowEditPipeAuto(HTREEITEM hItem)
{

	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeProcess.GetItemData( hItem );
	ASSERT( pData );


	if(pData->m_arcInfo->GetTypeOfUsingPipe() == 1)
	{

		pData->m_arcInfo->SetTypeOfUsingPipe(0);
		pData->m_arcInfo->GetPipeVector().clear();
		return;
	}

	if(pData->m_arcInfo->GetTypeOfUsingPipe() == 2)
	{
		pData->m_arcInfo->SetTypeOfUsingPipe(0);
		pData->m_arcInfo->GetPipeVector().clear();
	}		

	pData->m_arcInfo->SetTypeOfUsingPipe( 1 );

}

void CProcessDefineDlg::OnProcessEditPipeAuto()
{
	if(!m_hRClickItem)
		m_hRClickItem = m_treeProcess.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		SubFlowEditPipeAuto(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			SubFlowEditPipeAuto(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_butSave.EnableWindow();
	m_pCurFlow->SetChangedFlag( true );

	BuildInterestInPath( m_hRClickItem );
	ReloadProcessFlow();

}
void CProcessDefineDlg::OnProcessRouteconditionStraightline() 
{
	if (m_hRClickItem == NULL) return;	
	TREEITEMDATA* pData = (TREEITEMDATA*)m_treeProcess.GetItemData( m_hRClickItem );
	ASSERT( pData );
	pData->m_arcInfo->SetTypeOfUsingPipe(0);
	pData->m_arcInfo->GetPipeVector().clear();
//	m_bChanged=true;
	m_butSave.EnableWindow();
	m_pCurFlow->SetChangedFlag(true);

	BuildInterestInPath( m_hRClickItem );
//	ReloadTree();
	ReloadProcessFlow();

}
void CProcessDefineDlg::OnPipesUserselect()
{
	m_hRClickItem = m_hPocessSelItem;
	OnProcessEditPipe();
}

void CProcessDefineDlg::OnPipesEnableautomatic()
{
	m_hRClickItem = m_hPocessSelItem;
	OnProcessEditPipeAuto();
}

void CProcessDefineDlg::OnChannelOff()
{
	m_hRClickItem = m_hPocessSelItem;
	if(!m_hRClickItem)
		m_hRClickItem = m_treeProcess.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		SubFlowSetOneToOne(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			SubFlowSetOneToOne(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_pCurFlow->SetChangedFlag( true );
	m_butSave.EnableWindow();

	BuildInterestInPath( m_hRClickItem );
	ReloadProcessFlow();	
}

void CProcessDefineDlg::OnChannelOn()
{
	m_hRClickItem = m_hPocessSelItem;
	if(!m_hRClickItem)
		m_hRClickItem = m_treeProcess.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		SubFlowSetOneToOne(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			SubFlowSetOneToOne(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_pCurFlow->SetChangedFlag( true );
	m_butSave.EnableWindow();

	BuildInterestInPath( m_hRClickItem );
	ReloadProcessFlow();	
}

void CProcessDefineDlg::OnPercentPercentsplit()
{
	m_hRClickItem = m_hPocessSelItem;
	OnProcessModifypercent();
}

void CProcessDefineDlg::OnPercentEvenremaining()
{
	m_hRClickItem = m_hPocessSelItem;
	OnProcessEvenpercent();
}

void CProcessDefineDlg::SubFlowSetOneXOneStart(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_treeProcess.GetItemData( hItem );
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

void CProcessDefineDlg::OnPd1x1start() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_treeProcess.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		SubFlowSetOneXOneStart(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			SubFlowSetOneXOneStart(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_pCurFlow->SetChangedFlag( true );
	m_butSave.EnableWindow();

	BuildInterestInPath( m_hRClickItem );
	ReloadProcessFlow();	
}

void CProcessDefineDlg::SubFlowSetOneXOneEnd(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_treeProcess.GetItemData( hItem );
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

void CProcessDefineDlg::OnPd1x1end() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_treeProcess.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		SubFlowSetOneXOneEnd(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			SubFlowSetOneXOneEnd(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_pCurFlow->SetChangedFlag( true );
	m_butSave.EnableWindow();
	BuildInterestInPath( m_hRClickItem );

	ReloadProcessFlow();	
}

void CProcessDefineDlg::SubFlowchannelSetOneXOneEnd(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_treeProcess.GetItemData( hItem );
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

void CProcessDefineDlg::OnPdChannelcondition1x1End() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_treeProcess.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		SubFlowchannelSetOneXOneEnd(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			SubFlowchannelSetOneXOneEnd(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_pCurFlow->SetChangedFlag( true );
	m_butSave.EnableWindow();
	BuildInterestInPath( m_hRClickItem );
	ReloadProcessFlow();	
}

void CProcessDefineDlg::SubFlowChannelSetOneXOneStart(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_treeProcess.GetItemData( hItem );
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

void CProcessDefineDlg::OnPdChannelcondition1x1Start() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_treeProcess.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		SubFlowChannelSetOneXOneStart(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			SubFlowChannelSetOneXOneStart(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_pCurFlow->SetChangedFlag( true );
	m_butSave.EnableWindow();
	BuildInterestInPath( m_hRClickItem );
	ReloadProcessFlow();	
}

void CProcessDefineDlg::SubFlowChannelSetOneXOneClear(HTREEITEM hItem)
{
	TREEITEMDATA* pData = (TREEITEMDATA*)m_treeProcess.GetItemData( hItem );
	ASSERT( pData );

	if (NonChannel != pData->m_arcInfo->GetOneXOneState() )
	{
		pData->m_arcInfo->SetOneXOneState(NonChannel);
	} 	
}

void CProcessDefineDlg::OnPdChannelcondition1x1Clear() 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_treeProcess.GetSelectedItem();

	if (m_vCopyItem.empty())
	{
		SubFlowChannelSetOneXOneClear(m_hRClickItem);
	}
	else
	{
		for (unsigned i = 0; i < m_vCopyItem.size(); i++)
		{
			TreeNodeInfor& nodeData = m_vCopyItem[i];
			SubFlowChannelSetOneXOneClear(nodeData.m_hTreeNode);
		}
		m_vCopyItem.clear();
	}

	m_pCurFlow->SetChangedFlag( true );
	m_butSave.EnableWindow();
	BuildInterestInPath( m_hRClickItem );
	ReloadProcessFlow();	
}

void CProcessDefineDlg::OnCopyProcessors()
{
	m_vCopyProcessors.clear();

	std::deque<TreeNodeInfor> vDeque = m_vCopyItem;
	while (!vDeque.empty())
	{
		TreeNodeInfor nodeInfor = vDeque.front();
		TREEITEMDATA* pNodeData = (TREEITEMDATA*)m_treeProcess.GetItemData( nodeInfor.m_hTreeNode); 
		if (pNodeData == NULL)
			return;

		vDeque.pop_front();
		HTREEITEM hParentITem = m_treeProcess.GetParentItem(nodeInfor.m_hTreeNode);
		if (hParentITem == NULL)
			return;

		TREEITEMDATA* pParentNodeData = (TREEITEMDATA*)m_treeProcess.GetItemData( hParentITem);
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

bool CProcessDefineDlg::CopyProcessorOperationVaild( CopyProcessorOperation emtype,CString& strError ) 
{
	if(!m_hRClickItem)
		m_hRClickItem = m_treeProcess.GetSelectedItem();

	if (!m_hRClickItem)
	{
		strError = _T("Does not Select item of processor");
		return false;
	}

	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeProcess.GetItemData( m_hRClickItem );
	if (pData == NULL)
	{
		strError = _T("Retrieve data of tree node error");
		return false;
	}

	ProcessorID preID;
	preID = pData->m_procId;
	CSinglePaxTypeFlow tmpFlow( *m_pCurFlow );
	for (size_t i = 0; i < m_vCopyProcessors.size(); i++)
	{	
		ProcessorID id;
		const CFlowDestination& flowDest = m_vCopyProcessors.at(i);
		id = flowDest.GetProcID();

		if (preID.GetIDString().CompareNoCase("PROCESSSTART") == 0)
		{
			CProcessorDestinationList *pStartPair = ((CSingleProcessFlow *)m_pCurFlow)->GetStartPair();
			if (pStartPair == NULL)
			{
				strError = _T("Current flow define error");
				return false;
			}
		}
		else
		{
			if (emtype == Add_CopyProcessor_Operation)
			{
				tmpFlow.AddDestProc( preID,flowDest);
				tmpFlow.GetEqualFlowPairAt( pData->m_procId )->SetAllDestInherigeFlag( 0 );
			}
			else
			{
				tmpFlow.InsertProceoosorAfter( preID, id );
				CProcessorDestinationList* flowPair = tmpFlow.GetEqualFlowPairAt(preID);
				if (flowPair)
				{
					CFlowDestination* destInfo = flowPair->GetDestProcArcInfo(flowDest.GetProcID());
					if (destInfo)
					{
						*destInfo = flowDest;
					}
				}
			}
			
			tmpFlow.SetChangedFlag( true );

			tmpFlow.ResetDesitinationFlag(false);
			if(HandleSingleFlowLogic::IfHaveCircleFromProc(&tmpFlow,pData->m_procId))
			{
				strError.Format(_T("If you add the processor: %s,will make a circle!\r\nSo can not add the processor"),id.GetIDString());
				return false;
			}
		}
		preID = id;
	}
	return true;
}

void CProcessDefineDlg::OnAddCopyProcessors()
{
	CString strError;
	if (!CopyProcessorOperationVaild(Add_CopyProcessor_Operation,strError))
	{
		MessageBox(strError,NULL, MB_OK|MB_ICONINFORMATION );
		return;
	}

	if(!m_hRClickItem)
		m_hRClickItem = m_treeProcess.GetSelectedItem();

	if (!m_hRClickItem)
		return;

	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeProcess.GetItemData( m_hRClickItem );
	if (pData == NULL)
		return;

	ProcessorID preID;
	preID = pData->m_procId;
	for (size_t i = 0; i < m_vCopyProcessors.size(); i++)
	{	
		ProcessorID id;
		const CFlowDestination& flowDest = m_vCopyProcessors.at(i);
		id = flowDest.GetProcID();

		if (preID.GetIDString().CompareNoCase("PROCESSSTART") == 0)
		{
			CProcessorDestinationList *pStartPair = ((CSingleProcessFlow *)m_pCurFlow)->GetStartPair();
			if (pStartPair == NULL)
				return;
			
			pStartPair->AddDestProc(flowDest);
			pStartPair->EvenPercent();
			m_pCurFlow->AddIsolatedProc( id );
		}
		else
		{
			// check if exist a circle
			m_pCurFlow->AddDestProc(preID, flowDest );
			m_pCurFlow->GetEqualFlowPairAt( preID )->SetAllDestInherigeFlag( 0 );
		}

		preID = id;
	}

	m_pCurFlow->SetChangedFlag( true );
	m_butSave.EnableWindow( true );
	BuildInterestInPath( m_hRClickItem );
	ReloadProcessFlow();
}

void CProcessDefineDlg::OnInsertCopyProcessors()
{
	CString strError;
	if (!CopyProcessorOperationVaild(Insert_CopyProcessor_Operation,strError))
	{
		MessageBox(strError,NULL, MB_OK|MB_ICONINFORMATION );
		return;
	}

	if(!m_hRClickItem)
		m_hRClickItem = m_treeProcess.GetSelectedItem();

	TREEITEMDATA* pData = (TREEITEMDATA* )m_treeProcess.GetItemData( m_hRClickItem );
	ASSERT( pData );

	ProcessorID preID;
	preID = pData->m_procId;
	for (size_t i = 0; i < m_vCopyProcessors.size(); i++)
	{
		const CFlowDestination& flowDest = m_vCopyProcessors.at(i);

		ProcessorID id;
		id = flowDest.GetProcID();

		if( preID.GetIDString().CompareNoCase("PROCESSSTART") == 0 )
		{
			m_pCurFlow->AddIsolatedProc( id );
			CProcessorDestinationList *pStartPair = ((CSingleProcessFlow *)m_pCurFlow)->GetStartPair();
			if (pStartPair == NULL)
				return;
			pStartPair->AddDestProc(flowDest);
			pStartPair->EvenPercent();
		}
		else
		{
			// check if exist a circle
			m_pCurFlow->InsertProceoosorAfter( preID, id );
			CProcessorDestinationList* flowPair = m_pCurFlow->GetEqualFlowPairAt(preID);
			if (flowPair)
			{
				CFlowDestination* destInfo = flowPair->GetDestProcArcInfo(flowDest.GetProcID());
				if (destInfo)
				{
					*destInfo = flowDest;
				}
			}
		}

		preID = id;
	}

	m_pCurFlow->SetChangedFlag( true );
	m_butSave.EnableWindow( true );
	BuildInterestInPath( m_hRClickItem );
	ReloadProcessFlow();
}

void CProcessDefineDlg::OnClickTreeProcess( NMHDR *pNMHDR, LRESULT *pResult )
{
	CPoint pt = GetMessagePos();
	m_treeProcess.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hItem = m_treeProcess.HitTest(pt, &iRet);
	if( hItem == NULL )
		return;

	if (hItem == m_treeProcess.GetRootItem())
		return;

	CWaitCursor	wCursor;
	if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
	{
		TreeNodeInfor nodeInfor;
		nodeInfor.m_hTreeNode = hItem;
		nodeInfor.m_color = m_treeProcess.GetItemColor(hItem);
		nodeInfor.m_bBold = m_treeProcess.GetItemBold(hItem)?TRUE:FALSE;
		m_treeProcess.SetItemColor(hItem,PROCESS_COLOR_RED);
		m_treeProcess.SetItemBold(hItem,TRUE);
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
			m_treeProcess.SetItemColor(nodeInfor.m_hTreeNode,nodeInfor.m_color);
			m_treeProcess.SetItemBold(nodeInfor.m_hTreeNode,nodeInfor.m_bBold);
		}
		m_vCopyItem.clear();
	}

	m_treeProcess.Invalidate(FALSE);
}

void CProcessDefineDlg::BuildInterestInPath( HTREEITEM _hCurrentItem ,std::vector<ProcessorID>& _vInterestInPath /*= m_vFocusInPath */ )
{
	_vInterestInPath.clear();
	HTREEITEM hItem = _hCurrentItem;
	while( hItem ) 
	{
		TREEITEMDATA* pTempData =(TREEITEMDATA*) m_treeProcess.GetItemData( hItem );
		ASSERT( pTempData );
		_vInterestInPath.push_back( pTempData->m_procId );
		hItem = m_treeProcess.GetParentItem( hItem );
	}
}

bool CProcessDefineDlg::IsInterestNode( HTREEITEM _hCurrentItem )
{
	int iInterestPathNodeCount = m_vFocusInPath.size();
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
		if( !(m_vFocusInPath[i] == vTestPath[i]) )
			return false;
	}

	return true;
}

CString CProcessDefineDlg::BuildPathKeyString( HTREEITEM _hCurrentItem )
{
	CString sReturnValue;
	HTREEITEM hItem = _hCurrentItem;
	while( hItem )
	{
		TREEITEMDATA* pTempData =(TREEITEMDATA*) m_treeProcess.GetItemData( hItem );
		ASSERT( pTempData );
		sReturnValue += pTempData->m_procId.GetIDString();
		hItem = m_treeProcess.GetParentItem( hItem );
	}
	return sReturnValue;
}

void CProcessDefineDlg::BuildCollapsedNodeVector()
{
	m_vCollapsedNode.clear();
	BuildCollapsedNodeVectorSubRoute( m_treeProcess.GetRootItem() );
}

void CProcessDefineDlg::BuildCollapsedNodeVectorSubRoute( HTREEITEM _hRoot )
{
	if( !_hRoot || !m_treeProcess.ItemHasChildren( _hRoot ) )
		return;
	UINT result = TVIS_EXPANDED & m_treeProcess.GetItemState( _hRoot , TVIS_EXPANDED );
	if( !result )//collapsed
		m_vCollapsedNode.push_back( BuildPathKeyString( _hRoot ) );

	HTREEITEM hChildItem = m_treeProcess.GetChildItem( _hRoot );
	BuildCollapsedNodeVectorSubRoute( hChildItem );
	while( hChildItem = m_treeProcess.GetNextSiblingItem( hChildItem ) )
	{
		BuildCollapsedNodeVectorSubRoute( hChildItem );
	}
}

bool CProcessDefineDlg::IfThisItemShouldBeCollapsed( HTREEITEM _hCurrentItem )
{
	CString sPathKey = BuildPathKeyString( _hCurrentItem );
	if( std::find( m_vCollapsedNode.begin(), m_vCollapsedNode.end(),sPathKey ) != m_vCollapsedNode.end() )
		return true;
	else
		return false;
}

void CProcessDefineDlg::CollapsedTree()
{
	if( m_vCollapsedNode.size() <= 0 )
		return;
	HTREEITEM hRoot = m_treeProcess.GetRootItem();
	CollapsedSpecificNode( hRoot );
}

void CProcessDefineDlg::CollapsedSpecificNode( HTREEITEM _hCurrentItem )
{
	if( !_hCurrentItem || !m_treeProcess.ItemHasChildren( _hCurrentItem ) )
		return;

	if( IfThisItemShouldBeCollapsed(_hCurrentItem ) )
	{
		m_treeProcess.Expand( _hCurrentItem, TVE_COLLAPSE );
	}

	HTREEITEM hChildItem = m_treeProcess.GetChildItem( _hCurrentItem );
	CollapsedSpecificNode( hChildItem );
	while( hChildItem = m_treeProcess.GetNextSiblingItem( hChildItem ) )
	{
		CollapsedSpecificNode( hChildItem );
	}
}


