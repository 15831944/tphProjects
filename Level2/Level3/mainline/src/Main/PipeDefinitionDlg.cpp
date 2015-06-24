// PipeDefinitionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PipeDefinitionDlg.h"
#include "..\Inputs\Pipe.h"
#include "..\Inputs\SubFlowList.h"

#include "inputs\paxflow.h"
#include "3DView.h"
#include "MainFrm.h"
#include <CommonData/Fallback.h>
#include "common\WinMsg.h"
#include "AttachWalkDlg.h"
#include "../common/UnitsManager.h"
#include "ARCPort.h"
#include "Render3DView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPipeDefinitionDlg dialog

#define PIPEWIDTH 1000	//cm
#define BUT_OTHER	0 
#define BUT_POINT 1
#define BUT_SIDE 2
#define BUT_POINT_CHILD 3
#define BUT_SIDE_CHILD 4


CPipeDefinitionDlg::CPipeDefinitionDlg(Terminal* _pTerm,CTermPlanDoc* _pDoc,CWnd* pParent )
	: CDialog(CPipeDefinitionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPipeDefinitionDlg)
	m_pTerm = _pTerm;
	m_bPaxFlowChanged = false;
	m_strPipeName = _T("");
	//}}AFX_DATA_INIT
	m_iCurListSel = -1;
	m_pPipeDataSet = _pTerm->m_pPipeDataSet;
	m_pParent = pParent;
	//m_tempPipeDataSet = *_pPipeDataSet;
	m_pDoc = _pDoc;
	iPointIndex = 0;
	m_iButOption = BUT_OTHER;
	m_bHasShow = FALSE;

	//////////////////////////////////////////////////////////////////////////
	m_treePipePro.setInputTerminal( _pTerm );
}


void CPipeDefinitionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPipeDefinitionDlg)
	DDX_Control(pDX, IDOK, m_butOk);
	DDX_Control(pDX, IDCANCEL, m_butCancel);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_butSave);
	DDX_Control(pDX, IDC_BUTTON_NEW, m_butNew);
	DDX_Control(pDX, IDC_BUTTON_DEL, m_butDel);
	DDX_Control(pDX, IDC_TREE_PIPEPRO, m_treePipePro);
	DDX_Control(pDX, IDC_STATIC_TOOLBAR_POS, m_staticToolBarPos);
	DDX_Control(pDX, IDC_LIST_PIPE, m_listPipe);
	DDX_Control(pDX, IDC_EDIT_PIPENAME, m_editPipeName);
	DDX_Text(pDX, IDC_EDIT_PIPENAME, m_strPipeName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPipeDefinitionDlg, CDialog)
	//{{AFX_MSG_MAP(CPipeDefinitionDlg)
	ON_WM_CREATE()
	ON_LBN_SELCHANGE(IDC_LIST_PIPE, OnSelchangeListPipe)
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	ON_COMMAND(ID_PICKUP_FORM_MAP, OnPickupFormMap)
	ON_EN_KILLFOCUS(IDC_EDIT_PIPENAME, OnKillfocusPipename)
	ON_COMMAND(ID_PIPE_POINT_DEL, OnPipePointDel)
	ON_COMMAND(ID_PIPE_POINT_DELALL, OnPipePointDelall)
	ON_COMMAND(ID_PIPE_POINT_WIDTH_ADJUST, OnPointWidthAdjust)
	ON_COMMAND(ID_PIPE_POINT_WIDTH_ADJUST_ALL, OnPointWidthAdjustAll)
	ON_COMMAND(ID_PIPE_WALK_ATTACH, OnWalkAttach)
	ON_COMMAND(ID_PIPE_WALK_DETTACH, OnWalkDettach)
	ON_COMMAND(ID_BUT_PIPEPRO_NEW, OnButPipeproNew)
	ON_COMMAND(ID_BUT_PIPEPRO_DEL, OnButPipeproDel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PIPEPRO, OnSelchangedTreePipepro)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPipeDefinitionDlg message handlers

int CPipeDefinitionDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_TOOLBAR_PIPE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}		
	
	return 0;
}


BOOL CPipeDefinitionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitToolBar();
	InitTree();
	LoadList();

	//CCongestionAreaManager& dataset = m_pTerm->m_congManager;
	CCongestionAreaManager& dataset = m_pDoc->getARCport()->getCongestionAreaManager();
	m_pPipes = dataset.GetPipesVect();
	m_vPipes = *m_pPipes;

	m_editPipeName.EnableWindow( FALSE );
	m_treePipePro.EnableWindow( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPipeDefinitionDlg::InitToolBar()
{

	CRect mRect;
	m_staticToolBarPos.GetWindowRect( &mRect );
	ScreenToClient( &mRect );

	m_ToolBar.MoveWindow( &mRect );
	m_ToolBar.ShowWindow( SW_SHOW );
	m_staticToolBarPos.ShowWindow( SW_HIDE );

	m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_NEW, FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_DEL, FALSE );
}


void CPipeDefinitionDlg::LoadList( int _iSelIndex )
{
	m_listPipe.ResetContent();
	
	int nPipeCount	= m_pPipeDataSet->GetPipeCount();

	for(int i=0; i<nPipeCount; i++ )
	{
		m_listPipe.InsertString(i, m_pPipeDataSet->GetPipeAt(i)->GetPipeName());
	}
	
	if( _iSelIndex!= -1)
		m_listPipe.SetCurSel( _iSelIndex );
}

void CPipeDefinitionDlg::InitTree()
{
	m_treePipePro.DeleteAllItems();
	m_hPointRoot	= m_treePipePro.InsertItem( "Control Point" );
	m_hMovingRoot	= m_treePipePro.InsertItem( "Moving Side Walk");
	m_treePipePro.Expand( m_hPointRoot, TVE_EXPAND );
	m_treePipePro.Expand( m_hMovingRoot,TVE_EXPAND );

}

void CPipeDefinitionDlg::OnSelchangeListPipe() 
{
	// TODO: Add your control notification handler code here
	m_iCurListSel = m_listPipe.GetCurSel();

	ASSERT( m_iCurListSel != LB_ERR );

	ShowPipePro( m_iCurListSel );		
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUT_PIPEPRO_NEW,FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUT_PIPEPRO_DEL,FALSE);

	m_editPipeName.EnableWindow( TRUE );
	m_treePipePro.EnableWindow( TRUE );

}


void CPipeDefinitionDlg::ShowPipePro( int nIndex )
{
	InitTree();
	
	if(nIndex>=0 && nIndex<=m_pPipeDataSet->GetPipeCount() )
	{

		CPipe* pPipe	= m_pPipeDataSet->GetPipeAt( nIndex );
		m_strPipeName	= pPipe->GetPipeName();

		CString strName,strLabel;
		HTREEITEM hTreeItem;
		//show all control point
		int nPointCount	= pPipe->GetPipePointCount();
		for(int i=1; i<= nPointCount; i++ )
		{
			strName.Format("Point%d",i);
			hTreeItem	= m_treePipePro.InsertItem(strName,m_hPointRoot);

			strLabel = CString("Location(") + UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( "): " );
			strName.Format( "x = %.2f; y = %.2f", UNITSMANAGER->ConvertLength( pPipe->GetPipePointAt(i-1).m_point.getX() ), UNITSMANAGER->ConvertLength( pPipe->GetPipePointAt(i-1).m_point.getY() ) );
			strLabel += strName;
			m_treePipePro.InsertItem(strLabel,hTreeItem);

			strLabel	=  CString("Width(") + UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( ")" );
			strName.Format("%.2f", UNITSMANAGER->ConvertLength( pPipe->GetPipePointAt(i-1).m_width ));
			strLabel += strName;
			m_treePipePro.InsertItem(strLabel,hTreeItem);

			//m_treePipePro.Expand( hTreeItem, TVE_EXPAND );
		}
		m_treePipePro.Expand( m_hPointRoot, TVE_EXPAND );

		//show all moving side walk
		int nMovingCount = pPipe->GetSideWalkCount();
		for(int j=0; j< nMovingCount; j++)
		{
			//strLabel.Format("Moving Side Walk%d",j+1);
			strLabel = pPipe->GetSideWalkAt(j)->getID()->GetIDString();
			hTreeItem = m_treePipePro.InsertItem( strLabel, m_hMovingRoot );
			m_treePipePro.SetItemData( hTreeItem, j );

			strLabel = CString("Speed(") + UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( "/s): " );
			strName.Format( "%.2f",UNITSMANAGER->ConvertLength(pPipe->GetSideWalkAt(j)->GetMovingSpeed()) );
			strLabel += strName;
			m_treePipePro.InsertItem(strLabel,hTreeItem);

			strLabel = CString("Width(") + UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( "): " );
			strName.Format( "%.2f",UNITSMANAGER->ConvertLength(pPipe->GetSideWalkAt( j )->GetWidth()));
			strLabel += strName;
			m_treePipePro.InsertItem(strLabel,hTreeItem);
		}
	}

	UpdateData( FALSE );

	m_editPipeName.SetFocus();
	m_editPipeName.SetSel(0,0);
	
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUT_PIPEPRO_NEW,FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUT_PIPEPRO_DEL,FALSE);
	m_iButOption = BUT_OTHER;
	return;
}

void CPipeDefinitionDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here

	CRect mRect;
	m_treePipePro.GetWindowRect( &mRect );
	if( !mRect.PtInRect( point ))
		return;

	m_treePipePro.SetFocus();
	CPoint pt = point;
	m_treePipePro.ScreenToClient( &pt );
	UINT iRet;
	m_hRClickItem	= m_treePipePro.HitTest( pt, &iRet );
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
	{
		m_hRClickItem = NULL;
		return;
	}

	CMenu menuPop;
	CMenu*	pMenu = NULL;
	
	if( m_hRClickItem == m_hPointRoot)			//click on "contrl point" label
	{
		menuPop.LoadMenu( IDR_DEFINE_PIPE );
		pMenu	= menuPop.GetSubMenu( 1 );
		//pMenu->EnableMenuItem( ID_PICKUP_FORM_MAP,MF_GRAYED);
		pMenu->EnableMenuItem( ID_PIPE_POINT_DEL,MF_GRAYED);
		pMenu->EnableMenuItem( ID_PIPE_POINT_DELALL,MF_GRAYED );
		pMenu->EnableMenuItem( ID_PIPE_POINT_WIDTH_ADJUST,MF_GRAYED);
		pMenu->EnableMenuItem( ID_PIPE_POINT_WIDTH_ADJUST_ALL,MF_GRAYED);
		pMenu->EnableMenuItem( ID_PIPE_WALK_ATTACH,MF_GRAYED );
		pMenu->EnableMenuItem( ID_PIPE_WALK_DETTACH,MF_GRAYED );
	}
	else if( m_hRClickItem == m_hMovingRoot )	//clisk on "moving side walk" label
	{
		menuPop.LoadMenu( IDR_DEFINE_PIPE );
		pMenu	= menuPop.GetSubMenu( 1 );
		pMenu->EnableMenuItem( ID_PICKUP_FORM_MAP,MF_GRAYED);
		pMenu->EnableMenuItem( ID_PIPE_POINT_DEL,MF_GRAYED);
		pMenu->EnableMenuItem( ID_PIPE_POINT_DELALL,MF_GRAYED );
		pMenu->EnableMenuItem( ID_PIPE_POINT_WIDTH_ADJUST,MF_GRAYED);
		pMenu->EnableMenuItem( ID_PIPE_POINT_WIDTH_ADJUST_ALL,MF_GRAYED);
		//pMenu->EnableMenuItem( ID_PIPE_WALK_ATTACH,MF_GRAYED );
		pMenu->EnableMenuItem( ID_PIPE_WALK_DETTACH,MF_GRAYED );
	}
	else 
	{
		HTREEITEM hParent = m_treePipePro.GetParentItem( m_hRClickItem );

		if( hParent == m_hPointRoot )			//click on one control point
		{
			menuPop.LoadMenu( IDR_DEFINE_PIPE );
			pMenu	= menuPop.GetSubMenu( 1 );
			pMenu->EnableMenuItem( ID_PICKUP_FORM_MAP,MF_GRAYED);
			//pMenu->EnableMenuItem( ID_PIPE_POINT_DEL,MF_GRAYED);
			//pMenu->EnableMenuItem( ID_PIPE_POINT_DELALL,MF_GRAYED );
			pMenu->EnableMenuItem( ID_PIPE_POINT_WIDTH_ADJUST,MF_GRAYED);
			pMenu->EnableMenuItem( ID_PIPE_POINT_WIDTH_ADJUST_ALL,MF_GRAYED);
			pMenu->EnableMenuItem( ID_PIPE_WALK_ATTACH,MF_GRAYED );
			pMenu->EnableMenuItem( ID_PIPE_WALK_DETTACH,MF_GRAYED );
		}
		else if( hParent == m_hMovingRoot)		//click on a moving side walk
  		{
			menuPop.LoadMenu( IDR_DEFINE_PIPE );
			pMenu	= menuPop.GetSubMenu( 1 );
			pMenu->EnableMenuItem( ID_PICKUP_FORM_MAP,MF_GRAYED);
			pMenu->EnableMenuItem( ID_PIPE_POINT_DEL,MF_GRAYED);
			pMenu->EnableMenuItem( ID_PIPE_POINT_DELALL,MF_GRAYED );
			pMenu->EnableMenuItem( ID_PIPE_POINT_WIDTH_ADJUST,MF_GRAYED);
			pMenu->EnableMenuItem( ID_PIPE_POINT_WIDTH_ADJUST_ALL,MF_GRAYED);
			//pMenu->EnableMenuItem( ID_PIPE_WALK_ATTACH,MF_GRAYED );
			//pMenu->EnableMenuItem( ID_PIPE_WALK_DETTACH,MF_GRAYED );
		}
		else
		{
			hParent =  m_treePipePro.GetParentItem( hParent );
			if( hParent == m_hPointRoot )		
			{
				if(m_treePipePro.GetItemText( m_hRClickItem ).Left( 5 )=="Width") //click on control point's width
				{
					menuPop.LoadMenu( IDR_DEFINE_PIPE );
					pMenu	= menuPop.GetSubMenu( 1 );
					pMenu->EnableMenuItem( ID_PICKUP_FORM_MAP,MF_GRAYED);
					pMenu->EnableMenuItem( ID_PIPE_POINT_DEL,MF_GRAYED);
					pMenu->EnableMenuItem( ID_PIPE_POINT_DELALL,MF_GRAYED );
					//pMenu->EnableMenuItem( ID_PIPE_POINT_WIDTH_ADJUST,MF_GRAYED);
					//pMenu->EnableMenuItem( ID_PIPE_POINT_WIDTH_ADJUST_ALL,MF_GRAYED);
					pMenu->EnableMenuItem( ID_PIPE_WALK_ATTACH,MF_GRAYED );
					pMenu->EnableMenuItem( ID_PIPE_WALK_DETTACH,MF_GRAYED );
				}
			}
		}
		
	}
	
	if( pMenu!= NULL )
	{
		pMenu->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, this );
	}
	
}


void CPipeDefinitionDlg::OnButtonNew() 
{
	// TODO: Add your control notification handler code here

	CPipe mNewPipe( &(m_pDoc->GetTerminal()) );
	mNewPipe.SetPipeName( "NEW PIPE" );
	m_pPipeDataSet->AddPipe( mNewPipe);


	LoadList();
	m_iCurListSel	= m_listPipe.GetCount()-1;
	m_listPipe.SetCurSel( m_iCurListSel );
	ShowPipePro( m_iCurListSel );

	m_editPipeName.EnableWindow( TRUE );
	m_treePipePro.EnableWindow( TRUE );
	UpdatePipeInRender3D(mNewPipe.getGuid());
}


void CPipeDefinitionDlg::OnButtonDel() 
{
	// TODO: Add your control notification handler code here
	//	m_iCurListSel	= m_listPipe.GetCurSel();

	if( m_iCurListSel != LB_ERR )
	{
		CString strFlowName = _T("");
		CString strValue = _T("");
		CMobileElemConstraint* database = NULL;
		if( m_pTerm->m_pPassengerFlowDB->IfUseThisPipe(m_iCurListSel,database))
		{
			database->screenPrint(strFlowName,0,255);
			strValue.Format(_T("Passenger flow: %s use this pipe, cannot delete it!"),strFlowName);
			AfxMessageBox(strValue,MB_OK);
			return;
		}
		
		if(m_pTerm->m_pSubFlowList->IfUseThisPipe(m_iCurListSel,strFlowName))
		{
			strValue.Format(_T("Process flow: %s is using this pipe,cannot delete it"),strFlowName);
			AfxMessageBox(strValue,MB_OK);
			return;
		}
		//modify the pipe index after m_iCurListSel
		CGuid delGuid = m_pPipeDataSet->GetPipeAt(m_iCurListSel)->getGuid();
		m_bPaxFlowChanged = true;
		m_pTerm->m_pPassengerFlowDB->deletePipe( m_iCurListSel ) ;
		m_pDoc->DeletePipe(m_iCurListSel);
		m_pPipeDataSet->DeletePipeAt( m_iCurListSel );

		//delete pipe from the congestionparam file
		int j = 0;
		for(j = 0; j < static_cast<int>(m_vPipes.size()); j++){
			if(m_iCurListSel == m_vPipes[j]){
				m_vPipes.erase(m_vPipes.begin()+j);
				break;
			}
		}
		for(j = 0; j < static_cast<int>(m_vPipes.size()); j++){
			if(m_iCurListSel < m_vPipes[j]){
				m_vPipes[j]--;
			}
		}
		//end delete from file

		LoadList();
		
		m_strPipeName = "";

		m_iCurListSel = m_listPipe.GetCount()-1;
		m_listPipe.SetCurSel( m_iCurListSel );
		ShowPipePro( m_iCurListSel );

		UpdatePipeInRender3D(delGuid);
	}
}


void CPipeDefinitionDlg::OnPickupFormMap() 
{
	// TODO: Add your command handler code here
	GetDlgItem(IDC_EDIT_PIPENAME)->GetWindowText( m_strPipeName );

	if (!m_pDoc->GetMainFrame()->CreateOrActive3DView())
	{
		ASSERT(FALSE);
		return;
	}

	HideDialog( m_pDoc->GetMainFrame() );

	
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();
	enum FallbackReason enumReason;
	enumReason = PICK_MANYPOINTS;

	
	if( !m_pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( m_pDoc->GetMainFrame() );
		return;
	}
}


void CPipeDefinitionDlg::HideDialog(CWnd *parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();

}

void CPipeDefinitionDlg::ShowDialog(CWnd *parentWnd)
{
	while (!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		parentWnd->ShowWindow(SW_SHOW);
		parentWnd->EnableWindow(FALSE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(FALSE);
	ShowWindow(SW_SHOW);	
	EnableWindow();
}



LRESULT CPipeDefinitionDlg::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	CWnd* wnd = GetParent();
	while (!wnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		wnd->ShowWindow(SW_SHOW);
		wnd->EnableWindow(false);
		wnd = wnd->GetParent();
	}
	wnd->EnableWindow(false); 
	ShowWindow(SW_SHOW);	
	EnableWindow();
	std::vector<ARCVector3>* pData = (std::vector<ARCVector3>*) wParam;

	if(pData->size()>1)
	{
		CPipe* pPipe	= m_pPipeDataSet->GetPipeAt( m_iCurListSel );
		pPipe->ClearALlPipePoints();
		int nCount = pData->size();
		for( int i=1; i<=nCount; i++ )
		{
			PIPEPOINT pp;
			ARCVector3 v3D = pData->at(i-1);
			pp.m_point.setX( v3D[VX] );
			pp.m_point.setY( v3D[VY] );
			pp.m_point.setZ( m_pDoc->m_nActiveFloor * SCALE_FACTOR );
			pp.m_width	= PIPEWIDTH;
			pPipe->AddPipePoint( pp );
		}

		pPipe->CalculateTheBisectLine();
		ShowPipePro( m_iCurListSel );
		m_pDoc->UpdateAllViews(NULL);
		UpdatePipeInRender3D(pPipe->getGuid());
		return TRUE;
	}
	else
	{
		MessageBox("You must pick more than one point!");
		return FALSE;
	}
}

void CPipeDefinitionDlg::OnKillfocusPipename() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem( IDC_EDIT_PIPENAME )->GetWindowText( m_strPipeName );
	m_strPipeName.MakeUpper();

	if( m_iCurListSel>=0 && m_iCurListSel<= m_pPipeDataSet->GetPipeCount() )
	{
		if(m_strPipeName == "")
		{
			AfxMessageBox("Please input a pipe name");
			m_editPipeName.SetFocus();
			m_editPipeName.SetSel(0,0);

			return;
		}
		if( !CheckPipeName())
		{
			CString strMsg = "The '"+m_strPipeName+"'already exist! Please change the pipe name!";
			AfxMessageBox( strMsg );

			
			m_editPipeName.SetSel(0,0);

			return;
		}
		m_pPipeDataSet->GetPipeAt( m_iCurListSel )->SetPipeName( m_strPipeName );

		LoadList( m_iCurListSel );
	}
}

void CPipeDefinitionDlg::OnPipePointDel() 
{
	// TODO: Add your command handler code here
	ASSERT( m_hRClickItem )	;

	CString strPointName = m_treePipePro.GetItemText( m_hRClickItem );
	CString strPointNum	= strPointName.Mid( 5 );		//Point
	int iPointIndex	= atoi( LPCSTR( strPointNum ) );

	CPipe* pPipe = m_pPipeDataSet->GetPipeAt( m_iCurListSel );
	pPipe->DeletePipePointAt( iPointIndex-1 );
	pPipe->CalculateTheBisectLine();
	ShowPipePro( m_iCurListSel );
	UpdatePipeInRender3D(pPipe->getGuid());
}

void CPipeDefinitionDlg::OnPipePointDelall() 
{
	// TODO: Add your command handler code here
	ASSERT( m_hRClickItem )	;

	CPipe* pPipe = m_pPipeDataSet->GetPipeAt( m_iCurListSel );
	pPipe->ClearALlPipePoints();
	ShowPipePro( m_iCurListSel );
	UpdatePipeInRender3D(pPipe->getGuid());
}

void CPipeDefinitionDlg::OnPointWidthAdjust() 
{
	// TODO: Add your command handler code here
	HTREEITEM hParent = m_treePipePro.GetParentItem( m_hRClickItem );
	CString strPointName = m_treePipePro.GetItemText( hParent );
	CString strPointNum	= strPointName.Mid( 5 );		//"Point"

	iPointIndex= atoi( LPCSTR( strPointNum ))-1;

	double fWidth	= UNITSMANAGER->ConvertLength(m_pPipeDataSet->GetPipeAt( m_iCurListSel)->GetPipePointAt( iPointIndex ).m_width);

	int iDisplayNum = (int) fWidth;

	CString strWidth = CString("Width(") + UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( ")" );
	
	m_treePipePro.SetDisplayType( 99 );
	m_treePipePro.SetDisplayNum( iDisplayNum );
	m_treePipePro.SetSpinRange( 1, 10000);
	m_treePipePro.SpinEditLabel( m_hRClickItem ,strWidth);	
}

void CPipeDefinitionDlg::OnPointWidthAdjustAll() 
{
	// TODO: Add your command handler code here
	HTREEITEM hParent = m_treePipePro.GetParentItem( m_hRClickItem );
	CString strPointName = m_treePipePro.GetItemText( hParent );
	CString strPointNum	= strPointName.Mid( 5 );

	iPointIndex= atoi( LPCSTR( strPointNum ))-1;

	double fWidth	= UNITSMANAGER->ConvertLength(m_pPipeDataSet->GetPipeAt( m_iCurListSel)->GetPipePointAt( iPointIndex ).m_width);

	int iDisplayNum = (int) fWidth;

	CString strWidth = CString("Width(") + UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()) + CString( ")" );

	m_treePipePro.SetDisplayType( 99 );
	m_treePipePro.SetDisplayNum( iDisplayNum );
	m_treePipePro.SetSpinRange( 1, 10000);
	m_treePipePro.SpinEditLabel( m_hRClickItem ,strWidth);	

	iPointIndex	= 0xffff;
}

LRESULT CPipeDefinitionDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_INPLACE_SPIN )
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		CPipe* pPipe = m_pPipeDataSet->GetPipeAt(m_iCurListSel);
		if( iPointIndex == 0xFFFF )			//adjust all
		{
			for( int i=0; i<pPipe->GetPipePointCount(); i++)
			{
				pPipe->GetPipePoint( i)->m_width = UNITSMANAGER->UnConvertLength(pst->iPercent );
			}

			ShowPipePro( m_iCurListSel);
		}
		else								//adjust one
		{
			pPipe->GetPipePoint( iPointIndex)->m_width = UNITSMANAGER->UnConvertLength(pst->iPercent );
		}

		pPipe->CalculateTheBisectLine();
		OnSelchangeListPipe();
		UpdatePipeInRender3D(pPipe->getGuid());
		return TRUE;

	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CPipeDefinitionDlg::OnWalkAttach() 
{
	// TODO: Add your command handler code here
	CPipe* pPipe	= m_pPipeDataSet->GetPipeAt( m_iCurListSel );
	CAttachWalkDlg dlg( pPipe,m_pDoc );
	if( dlg.DoModal()==IDOK )
	{
		ShowPipePro( m_iCurListSel );
	}
}

void CPipeDefinitionDlg::OnWalkDettach() 
{
	// TODO: Add your command handler code here
	int iIndex	= m_treePipePro.GetItemData( m_hRClickItem );
	CPipe* pPipe	= m_pPipeDataSet->GetPipeAt( m_iCurListSel );
	pPipe->DetachSideWalk( *(pPipe->GetSideWalkAt( iIndex )->getID()) );

	ShowPipePro( m_iCurListSel );
}


void CPipeDefinitionDlg::OnButPipeproNew() 
{
	// TODO: Add your command handler code here
	switch( m_iButOption )
	{
	case BUT_SIDE:
	case BUT_SIDE_CHILD:
		OnWalkAttach();
		break;
	default:
		break;
	}
}

void CPipeDefinitionDlg::OnButPipeproDel() 
{
	// TODO: Add your command handler code here
	switch( m_iButOption )
	{
	case BUT_POINT_CHILD:
		m_hRClickItem = m_hNode;
		OnPipePointDel();
		break;
	case BUT_SIDE_CHILD:
		m_hRClickItem = m_hNode;
		OnWalkDettach();
		break;
	default:
		break;
	}
}



void CPipeDefinitionDlg::OnSelchangedTreePipepro(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_hNode	= m_treePipePro.GetSelectedItem();

	if( m_hNode == m_hPointRoot )		//Control point label selected
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUT_PIPEPRO_NEW,FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUT_PIPEPRO_DEL,FALSE);
		m_iButOption = BUT_POINT;
	}
	else if( m_hNode == m_hMovingRoot )	//Moving side walk label selected
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUT_PIPEPRO_NEW,TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUT_PIPEPRO_DEL,FALSE);
		m_iButOption = BUT_SIDE;
	}
	else
	{
		HTREEITEM hParent = m_treePipePro.GetParentItem( m_hNode );
		if( hParent == m_hPointRoot )	//A control point label selected
		{
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUT_PIPEPRO_NEW,FALSE);
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUT_PIPEPRO_DEL,TRUE);
			m_iButOption = BUT_POINT_CHILD;
		}
		else if( hParent == m_hMovingRoot )	//A moving side walk label selected
		{
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUT_PIPEPRO_NEW,TRUE);
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUT_PIPEPRO_DEL,TRUE);
			m_iButOption = BUT_SIDE_CHILD; 
		}
		else
		{
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUT_PIPEPRO_NEW,FALSE);
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUT_PIPEPRO_DEL,FALSE);
			m_iButOption = BUT_OTHER;
		}
	}


	*pResult = 0;
}

void CPipeDefinitionDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	//check all pipe to make sure the number of pipe point than 1
	for(int i=0; i<m_pPipeDataSet->GetPipeCount(); i++)
	{
		if( m_pPipeDataSet->GetPipeAt( i )->GetPipePointCount()<2 )
		{
			CString strMsg;
			strMsg = "The "+m_pPipeDataSet->GetPipeAt(i)->GetPipeName() +" pipe's point number less than 2!\r\n";
			MessageBox( strMsg );

			m_listPipe.SetCurSel( i );
			OnSelchangeListPipe();
			return;
		}
	}

	CString path	= m_pDoc->m_ProjInfo.path;
	m_pPipeDataSet->saveDataSet( path, FALSE );
	if( m_bPaxFlowChanged )
	{
		m_pTerm->m_pPassengerFlowDB->saveDataSet( path, FALSE );
		m_bPaxFlowChanged = false;
	}

	CString strPath = m_pDoc->m_ProjInfo.path + "\\INPUT";
	m_pPipes->clear();
	*m_pPipes = m_vPipes;
	m_pDoc->getARCport()->getCongestionAreaManager().saveDataSet(strPath, FALSE);
}

void CPipeDefinitionDlg::OnOK() 
{
	// TODO: Add extra validation here
	//check all pipe to make sure the number of pipe point than 1
	for(int i=0; i<m_pPipeDataSet->GetPipeCount(); i++)
	{
		if( m_pPipeDataSet->GetPipeAt( i )->GetPipePointCount()<2 )
		{
			CString strMsg;
			strMsg = "The "+m_pPipeDataSet->GetPipeAt(i)->GetPipeName() +" pipe's point number less than 2!\r\n";
			MessageBox( strMsg );

			m_listPipe.SetCurSel( i );
			OnSelchangeListPipe();
			return;
		}
	}

	CString path	= m_pDoc->m_ProjInfo.path;
	m_pPipeDataSet->saveDataSet( path, FALSE );
	if( m_bPaxFlowChanged )
	{
		m_pTerm->m_pPassengerFlowDB->saveDataSet( path, FALSE );
	}

	CString strPath = m_pDoc->m_ProjInfo.path + "\\INPUT";
	m_pPipes->clear();
	*m_pPipes = m_vPipes;
	m_pDoc->getARCport()->getCongestionAreaManager().saveDataSet(strPath, FALSE);

	CDialog::OnOK();
}

void CPipeDefinitionDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	//CString path = ((CTermPlanDoc*)((CView*)m_pParent)->GetDocument())->m_ProjInfo.path;
	CString path	= m_pDoc->m_ProjInfo.path;
	try
	{
		m_pPipeDataSet->loadDataSet( path );
		if( m_bPaxFlowChanged )
		{
			m_pTerm->m_pPassengerFlowDB->loadDataSet( path );
		}
		UpdateAllPipesInRender3D();
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;			
	}
	CDialog::OnCancel();
}


BOOL CPipeDefinitionDlg::CheckPipeName()
{
	int iSameName = 0;
	for( int i=0; i<m_pPipeDataSet->GetPipeCount(); i++ )
	{
		if(m_pPipeDataSet->GetPipeAt( i )->GetPipeName() == m_strPipeName && i != m_iCurListSel)
			iSameName++;
	}

	return iSameName>0 ? FALSE : TRUE;
}


void CPipeDefinitionDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if( nType == SIZE_MAXIMIZED )
	{
		m_bHasShow = TRUE;

		SizeWindow(cx,cy);
	}
	else if( m_bHasShow && nType == SIZE_MINIMIZED)
	{
		SizeWindow(cx,cy);
	}
	else if(GetDlgItem(IDC_STATIC))
	{
		CRect rc, rc1, rc2;
 
 		GetDlgItem(IDC_LIST_PIPE)->GetWindowRect(&rc);
 		ScreenToClient(&rc);
 		GetDlgItem(IDC_LIST_PIPE)->MoveWindow(rc.left, rc.top, int((cx-33)*0.4), cy-81);//
 
  		GetDlgItem(IDC_LIST_PIPE)->GetWindowRect(&rc1);
 		ScreenToClient(&rc1);
		GetDlgItem(IDC_STATIC_PIPNAME)->GetWindowRect(&rc);
		ScreenToClient(&rc);
 		GetDlgItem(IDC_STATIC_PIPNAME)->MoveWindow(rc1.right+13, rc.top, int((cx-33)*0.6), rc.Height());
 
 		GetDlgItem(IDC_EDIT_PIPENAME)->GetWindowRect(&rc2);
 		ScreenToClient(&rc2);
 		GetDlgItem(IDC_EDIT_PIPENAME)->MoveWindow(rc1.right+13, rc2.top, int((cx-33)*0.6), rc2.Height());
 
 		GetDlgItem(IDC_STATIC_PRO)->GetWindowRect(&rc2);
 		ScreenToClient(&rc2);
 		GetDlgItem(IDC_STATIC_PRO)->MoveWindow(rc1.right+13, rc2.top, int((cx-33)*0.6), rc2.Height());
 
 		GetDlgItem(IDC_STATIC_FRAME)->GetWindowRect(&rc2);
 		ScreenToClient(&rc2);
 		GetDlgItem(IDC_STATIC_FRAME)->MoveWindow(rc1.right+13, rc2.top, int((cx-33)*0.6), cy-52-rc2.top);
 		
 		m_staticToolBarPos.GetWindowRect(rc2);
 		ScreenToClient(&rc2);
 		m_staticToolBarPos.MoveWindow(rc1.right+13, rc2.top, int((cx-33)*0.6), rc2.Height());
 		m_ToolBar.MoveWindow(rc1.right+13, rc2.top, int((cx-33)*0.6), rc2.Height());
 
		GetDlgItem(IDC_TREE_PIPEPRO)->GetWindowRect(&rc2);
		ScreenToClient(&rc2);
 		GetDlgItem(IDC_TREE_PIPEPRO)->MoveWindow(rc1.right+13, rc2.top, int((cx-33)*0.6), cy-52-rc2.top);
 
 		GetDlgItem(IDCANCEL)->GetWindowRect(&rc2);
 		ScreenToClient(&rc2);
 		GetDlgItem(IDCANCEL)->MoveWindow(cx-9-rc2.Width(), cy-34, rc2.Width(), rc2.Height());
 		GetDlgItem(IDOK)->MoveWindow(cx-9*2-rc2.Width()*2, cy-34, rc2.Width(), rc2.Height());
 		GetDlgItem(IDC_BUTTON_SAVE)->MoveWindow(cx-9*3-rc2.Width()*3, cy-34, rc2.Width(), rc2.Height());
 		GetDlgItem(IDC_BUTTON_DEL)->MoveWindow(cx-9*4-rc2.Width()*4, cy-34, rc2.Width(), rc2.Height());
 		GetDlgItem(IDC_BUTTON_NEW)->MoveWindow(cx-9*5-rc2.Width()*5, cy-34, rc2.Width(), rc2.Height());

	}	
	Invalidate();
}

void CPipeDefinitionDlg::SizeWindow(int cx,int cy)
{
	CRect mWndRect;

	//size button
	m_butCancel.GetWindowRect( mWndRect );
	ScreenToClient( mWndRect );
	CSize mOffSet(cx-mWndRect.right-12,cy-mWndRect.bottom-12 );
	m_butCancel.MoveWindow( mWndRect+mOffSet);
	m_butOk.GetWindowRect( mWndRect );
	ScreenToClient( mWndRect );
	m_butOk.MoveWindow( mWndRect+mOffSet);
	m_butSave.GetWindowRect( mWndRect );
	ScreenToClient( mWndRect );
	m_butSave.MoveWindow( mWndRect+mOffSet);
	m_butDel.GetWindowRect( mWndRect );
	ScreenToClient( mWndRect );
	m_butDel.MoveWindow( mWndRect+mOffSet);
	m_butNew.GetWindowRect( mWndRect );
	ScreenToClient( mWndRect );
	m_butNew.MoveWindow( mWndRect+mOffSet);

	//size list box
	m_listPipe.GetWindowRect( mWndRect );
	ScreenToClient( mWndRect );
	m_listPipe.MoveWindow(mWndRect.left,mWndRect.top,mWndRect.Width(), mWndRect.Height()+mOffSet.cy );

	//size edit box
	m_editPipeName.GetWindowRect( mWndRect);
	ScreenToClient( mWndRect);
	m_editPipeName.MoveWindow(mWndRect.left,mWndRect.top, mWndRect.Width()+mOffSet.cx, mWndRect.Height() );

	//size Toolbar static
	m_staticToolBarPos.GetWindowRect( mWndRect );
	ScreenToClient( mWndRect );
	m_staticToolBarPos.MoveWindow( mWndRect.left, mWndRect.top, mWndRect.Width()+mOffSet.cx, mWndRect.Height() );

	//size pro tree
	m_treePipePro.GetWindowRect( mWndRect );
	ScreenToClient( mWndRect );
	m_treePipePro.MoveWindow( mWndRect.left, mWndRect.top, mWndRect.Width()+mOffSet.cx, mWndRect.Height()+mOffSet.cy );

	//size static
	GetDlgItem( IDC_STATIC_FRAME )->GetWindowRect( mWndRect );
	ScreenToClient( mWndRect );
	GetDlgItem( IDC_STATIC_FRAME )->MoveWindow( mWndRect.left, mWndRect.top, mWndRect.Width()+mOffSet.cx, mWndRect.Height()+mOffSet.cy );
}	

void CPipeDefinitionDlg::UpdateAllPipesInRender3D()
{
	CRender3DView* pRender3DView = m_pDoc->GetRender3DView();
	if (pRender3DView)
	{
		//pRender3DView->GetModelEditScene().Update3DObjectList(CPipe::getTypeGUID());
	}
}

void CPipeDefinitionDlg::UpdatePipeInRender3D(const CGuid& guid)
{
	m_pDoc->UpdateRender3DObject(guid, CPipe::getTypeGUID());
}
