// GatePriorityDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "GatePriorityDlg.h"
#include "inputs\GatePriorityInfo.h"
#include "GateAssignDlg.h"
#include "ProcesserDialog.h"
#include "inputs\in_term.h"
#include ".\SelectALTObjectDialog.h"
#include "..\AirsideGUI\DlgStandFamily.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGatePriorityDlg dialog


CGatePriorityDlg::CGatePriorityDlg(InputTerminal* _pInTerm,int nAirportID, int nProjectID,CWnd* pParent /*=NULL*/)
	: CDialog(CGatePriorityDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGatePriorityDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_standIDList.clear();
	m_bPaintInit = false;
	m_pInTerm = _pInTerm;
	m_nAirportID = nAirportID;
	m_nProjID = nAirportID;
}


void CGatePriorityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGatePriorityDlg)
	DDX_Control(pDX, IDC_LIST_GATE_PRIORITY, m_listctrlData);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGatePriorityDlg, CDialog)
	//{{AFX_MSG_MAP(CGatePriorityDlg)
	ON_WM_CREATE()
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPeoplemoverNew)
	ON_COMMAND(ID_BUTTON_UP, OnArrowUp)
	ON_COMMAND(ID_BUTTON_DOWN, OnArrowDown)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_WM_PAINT()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_GATE_PRIORITY, OnItemchangedListGatePriority)
	ON_COMMAND(ID_BUTTON_EDIT, OnButtonEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GATE_PRIORITY, OnDblclkListGatePriority)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGatePriorityDlg message handlers

BOOL CGatePriorityDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//create the tool bar here.
	InitToolbar();

	//set the style of the list control.
	SetListCtrl();

	//Show the proc list in the list
//	ShowList();
	GetDlgItem(IDC_BUTTON_SAVE)->SetWindowText(_T("Save"));
	AfxGetApp()->EndWaitCursor();
	// Set the new button of toolbar is unable
	m_listctrlData.DeleteAllItems();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGatePriorityDlg::InitToolbar()
{
	CRect rc;
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_DOWN,FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_UP,FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_EDIT,FALSE );
}

int CGatePriorityDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_PRIORITY_ASSIGNMENT))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	
	return 0;
}


extern CTermPlanApp theApp;
InputTerminal* CGatePriorityDlg::GetInputTerminal()
{
//	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)(m_pParentWnd->GetParent()))->GetDocument();
	POSITION ps = theApp.m_pDocManager->GetFirstDocTemplatePosition();
	POSITION ps2 = theApp.m_pDocManager->GetNextDocTemplate(ps)->GetFirstDocPosition();
	CTermPlanDoc *pDoc = (CTermPlanDoc*)(theApp.m_pDocManager->GetNextDocTemplate(ps)->GetNextDoc(ps2));
    return (InputTerminal*)&pDoc->GetTerminal();
}

void CGatePriorityDlg::SetListCtrl()
{
	DWORD dwStyle = m_listctrlData.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listctrlData.SetExtendedStyle( dwStyle );

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[2][128];
	sprintf( columnLabel[0], "Order" );
	sprintf( columnLabel[1], "Gate Group" );
	int DefaultColumnWidth[] = { 150, 250 };	
	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER };
	for( int i=0; i<2; i++ )
	{
		//temporary string list.
		CStringList strList;
		strList.RemoveAll();
		lvc.csList = &strList;
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		if(i == 0)
			lvc.fmt = LVCFMT_NOEDIT;
		if(i == 1)
			lvc.fmt = LVCFMT_POPUP_GATE_SELECTION;
		m_listctrlData.InsertColumn( i, &lvc );	
	}
}

int CGatePriorityDlg::GetSelectedListItem()
{
	int nCount = m_listctrlData.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlData.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			return i;
	}
	return -1;
}

CString CGatePriorityDlg::GetProjPath()
{
	POSITION ps = theApp.m_pDocManager->GetFirstDocTemplatePosition();
	POSITION ps2 = theApp.m_pDocManager->GetNextDocTemplate(ps)->GetFirstDocPosition();
	CTermPlanDoc *pDoc = (CTermPlanDoc*)(theApp.m_pDocManager->GetNextDocTemplate(ps)->GetNextDoc(ps2));
	return pDoc->m_ProjInfo.path;
}


void CGatePriorityDlg::OnPeoplemoverDelete()
{
	int nIndex = m_listctrlData.GetCurSel();
	if(nIndex < 0 )
		return;
	m_listctrlData.DeleteItem(nIndex);
	m_standIDList.DeleteItem(nIndex);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );

	ResetOrder();
}

void CGatePriorityDlg::OnPeoplemoverNew()
{
	assert(m_pInTerm);
	//////////////////////////////////////////////////////////
	//CProcesserDialog procDlg( m_pInTerm );
	//procDlg.SetType( GateProc );
	//procDlg.SetType2(StandProcessor);
	//procDlg.SetOnlyShowACStandGate( true );
	//if( procDlg.DoModal() !=IDOK  )
	//	return;

	//// get process id
	////ProcessorID* pNewProcID = new ProcessorID;
	////if( !procDlg.GetProcessorID( *pNewProcID ) )
	////	return;
	////CString strNewProc = pNewProcID->GetIDString();

	////// check if the process id has exist in the list ctrl
	////for( int i=0; i< m_listctrlData.GetItemCount(); i++ )
	////{
	////	CString strItemText = m_listctrlData.GetItemText(i,1);
	////	if( strNewProc == strItemText )		// has exist in the list 
	////	{
	////		delete pNewProcID;
	////		m_listctrlData.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
	////		return;
	////	}
	////}
	////// add to proc_id list
	////m_procIDList.addItem( pNewProcID );
	//ALTObjectID newObjID;
	//if(!procDlg.getP)
	//
	////decide the order of the new item automatically.
	////set the index number.
	//CString strIndex;
	//strIndex.Format("%d", m_procIDList.getCount() );
	//int iNewIndex = m_listctrlData.InsertItem( m_listctrlData.GetItemCount(), strIndex );
	//m_listctrlData.SetItemText( iNewIndex, 1, strNewProc );
	//m_listctrlData.SetItemData( iNewIndex, 0 );

	//m_listctrlData.SetItemState( iNewIndex, LVIS_SELECTED, LVIS_SELECTED );
	
	/*CSelectALTObjectDialog objDlg(0,m_nAirportID);
	objDlg.SetType( ALT_STAND );*/
	CDlgStandFamily  objDlg(m_nProjID);
	if(objDlg.DoModal() != IDOK ) return;

	CString pnewIDstr;
	pnewIDstr = objDlg.GetSelStandFamilyName();
	if (pnewIDstr.IsEmpty())
		return;
	
//	if( !objDlg.GetObjectIDString(pnewIDstr) )  return;
	////check the objid has exist in the list ctrl
	//for(int i =0 ;i < m_listctrlData.GetItemCount();i++)	

	// check if the process id has exist in the list ctrl
	for( int i=0; i< m_listctrlData.GetItemCount(); i++ )
	{
		CString strItemText = m_listctrlData.GetItemText(i,1);
		if( pnewIDstr == strItemText )		// has exist in the list 
		{
			m_listctrlData.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
			return;
		}
	}
	// add to proc_id list
	m_standIDList.Add( (const char *) pnewIDstr );

	CString strIndex;
	strIndex.Format("%d", m_standIDList.size() );
	int iNewIndex = m_listctrlData.InsertItem( m_listctrlData.GetItemCount(), strIndex );
	m_listctrlData.SetItemText( iNewIndex, 1, pnewIDstr );
	m_listctrlData.SetItemData( iNewIndex, 0 );

	m_listctrlData.SetItemState( iNewIndex, LVIS_SELECTED, LVIS_SELECTED );
}

void CGatePriorityDlg::OnArrowUp()
{

	//divide into two parts, list ctrl & procList
	//with more clear logic.

	//the aspect of list ctrl
	int nSel = GetSelectedItem();
	int nUpper  = nSel - 1;
	CString strSelOrder, strUpperOrder;
	CString strSelGate, strUpperGate;

	//get the current text in the list.
	strSelGate = m_listctrlData.GetItemText(nSel, 1);
	strUpperGate = m_listctrlData.GetItemText(nUpper, 1);

	//swap the text in the list.
	m_listctrlData.SetItemText(nSel, 1, strUpperGate);
	m_listctrlData.SetItemText(nUpper, 1, strSelGate);

	m_listctrlData.SetItemState(nUpper, LVIS_SELECTED, LVIS_SELECTED);

}

void CGatePriorityDlg::OnArrowDown()
{

	//the aspect of list ctrl
	int nSel = GetSelectedItem();
	int nDown  = nSel + 1;
	CString strSelOrder, strDownOrder;
	CString strSelGate, strDownGate;

	//get the current text in the list.
	strSelGate = m_listctrlData.GetItemText(nSel, 1);
	strDownGate = m_listctrlData.GetItemText(nDown, 1);

	//swap the text in the list.
	m_listctrlData.SetItemText(nSel, 1, strDownGate);
	m_listctrlData.SetItemText(nDown, 1, strSelGate);

	m_listctrlData.SetItemState(nDown, LVIS_SELECTED, LVIS_SELECTED);


}

void CGatePriorityDlg::ShowList()
{
	GetInputTerminal()->gatePriorityInfo->loadDataSet(GetProjPath());
	m_standIDList = GetInputTerminal()->gatePriorityInfo->altObjectIDList;
	m_listctrlData.DeleteAllItems();
	int nCount =(int) m_standIDList.size();
	for(int i = 0; i < nCount; i++)
	{
		CString strTemp;
		strTemp.Format("%d", i+1);
		m_listctrlData.InsertItem(i, strTemp);
		m_listctrlData.SetItemText(i, 1, m_standIDList.at(i).GetIDString());
	}
	
}

//zero based selected item
int CGatePriorityDlg::GetSelectedItem()
{
	int nCount = m_listctrlData.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlData.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			return i;
	}
	return -1;
}

void CGatePriorityDlg::OnButtonSave() 
{
	//clear the proc list and re-get from the current list ctrl.
	m_standIDList.clear();

	int nCount = m_listctrlData.GetItemCount();
	CString strProc;
	for(int i = 0; i < nCount; i++)
	{
		/*ProcessorID *newProcID = new ProcessorID;
		newProcID->SetStrDict(GetInputTerminal()->inStrDict);
		strProc = m_listctrlData.GetItemText(i, 1);
		newProcID->setID(strProc.GetBuffer(strProc.GetLength() + 1));
		m_standIDList.Add(newProcID);*/
		ALTObjectID newObjID;
		newObjID = (const char*)(m_listctrlData.GetItemText(i,1));
		m_standIDList.Add(newObjID);
	}


	//save the proc list here.
	GetInputTerminal()->gatePriorityInfo->altObjectIDList = m_standIDList;
	CString strPath = GetProjPath();
	GetInputTerminal()->gatePriorityInfo->saveDataSet( strPath, true );
}

void CGatePriorityDlg::OnOK() 
{
	OnButtonSave();
	//((CGateAssignDlg *)GetParent())->GatePriorityAssign();
	CDialog::OnOK();
}

void CGatePriorityDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if(m_bPaintInit)
		return;
	try
	{
		GetInputTerminal()->gatePriorityInfo->loadDataSet(GetProjPath());
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;			
		return;
	}
	m_standIDList = GetInputTerminal()->gatePriorityInfo->altObjectIDList;
	m_listctrlData.DeleteAllItems();
	int nCount = (int)m_standIDList.size();
	for(int i = 0; i < nCount; i++)
	{
		CString strTemp;
		strTemp.Format("%d", i+1);
		m_listctrlData.InsertItem(i, strTemp);
		m_listctrlData.SetItemText(i, 1, m_standIDList.at(i).GetIDString());
	}
	m_bPaintInit = true;
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CGatePriorityDlg::OnItemchangedListGatePriority(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	//get selected item and determine how to display the toolbar button
	int nCount = m_listctrlData.GetItemCount();
	int nSel = GetSelectedItem();
	if(nCount <= 1 && nSel == -1)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_UP, FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_DOWN, FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_EDIT, FALSE);
	}

	if(nSel != -1 )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,TRUE );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_EDIT, TRUE);
		if(nSel == 0)
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_UP, FALSE);
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_DOWN	, TRUE);
		}
		else if(nSel == (int)m_standIDList.size() - 1)
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_DOWN, FALSE);
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_UP, TRUE);
		}
		else
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_DOWN, TRUE);
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_UP, TRUE);
		}

	}
	*pResult = 0;
}

void CGatePriorityDlg::OnButtonEdit() 
{
	int nSel = GetSelectedItem();
		
	if(nSel == -1)
		return;
	/*CGateSelectionDlg *gateSelectionDlg = new CGateSelectionDlg(this, nSel);
	gateSelectionDlg->DoModal();
	delete gateSelectionDlg;*/
/*	CSelectALTObjectDialog objDlg(0,m_nAirportID);
	objDlg.SetType( ALT_STAND );*/	
	CDlgStandFamily  objDlg(m_nProjID);
	if(objDlg.DoModal() == IDOK )
	{		
		m_listctrlData.SetItemText(nSel, 1, objDlg.GetSelStandFamilyName());		
	}		
}

void CGatePriorityDlg::OnDblclkListGatePriority(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	int nSel = GetSelectedItem();
		
	if(nSel == -1)
		return;

	//CSelectALTObjectDialog objDlg(0,m_nAirportID);
	//objDlg.SetType( ALT_STAND );
	//objDlg.SetSelectString(m_listctrlData.GetItemText(nSel,1));
	CDlgStandFamily  objDlg(m_nProjID);
	if(objDlg.DoModal() == IDOK )
	{		
		m_listctrlData.SetItemText(nSel, 1, objDlg.GetSelStandFamilyName());		
	}		

	*pResult = 0;
}


void CGatePriorityDlg::ResetOrder()
{
	int nCount = m_listctrlData.GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		CString strTemp;
		strTemp.Format("%d", i+1);
		m_listctrlData.SetItemText(i, 0, strTemp);
	}
}
