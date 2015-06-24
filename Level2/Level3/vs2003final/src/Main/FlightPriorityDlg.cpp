// FlightPriorityDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "FlightPriorityDlg.h"
#include "GateAssignDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlightPriorityDlg dialog


CFlightPriorityDlg::CFlightPriorityDlg(int nPrjID, int nAirportID,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CFlightPriorityDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFlightPriorityDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bPaintInit = false;
	m_nAirportID = nAirportID;
	m_nprjID = nPrjID;

	m_treeProc.setInputTerminal( GetInputTerminal() );
	m_listctrlData.SetInputTerminal(GetInputTerminal());
}


void CFlightPriorityDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFlightPriorityDlg)
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_LIST_FLIGHT_PRIORITY, m_listctrlData);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	//}}AFX_DATA_MAP
}

void CFlightPriorityDlg::InitToolbar()
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


InputTerminal* CFlightPriorityDlg::GetInputTerminal()
{
//	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)(m_pParentWnd->GetParent()))->GetDocument();
	POSITION ps = theApp.m_pDocManager->GetFirstDocTemplatePosition();
	POSITION ps2 = theApp.m_pDocManager->GetNextDocTemplate(ps)->GetFirstDocPosition();
	CTermPlanDoc *pDoc = (CTermPlanDoc*)(theApp.m_pDocManager->GetNextDocTemplate(ps)->GetNextDoc(ps2));
    return (InputTerminal*)&pDoc->GetTerminal();

}

void CFlightPriorityDlg::SetListCtrl()
{
	DWORD dwStyle = m_listctrlData.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listctrlData.SetExtendedStyle( dwStyle );

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;

	CStringList strList;
	strList.RemoveAll();
	lvc.csList = &strList;
	lvc.fmt = LVCFMT_NOEDIT;

	lvc.pszText = "Order";
	lvc.cx = 100;
	m_listctrlData.InsertColumn(0, &lvc );	

	lvc.pszText = "Flight/Sector";
	m_listctrlData.InsertColumn(1, &lvc);

	lvc.pszText = "Gate Group";
	lvc.cx = 150;
	m_listctrlData.InsertColumn(2, &lvc);	
}



BEGIN_MESSAGE_MAP(CFlightPriorityDlg, CXTResizeDialog)
	//{{AFX_MSG_MAP(CFlightPriorityDlg)
	ON_WM_CREATE()
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPeoplemoverNew)
	ON_COMMAND(ID_BUTTON_UP, OnArrowUp)
	ON_COMMAND(ID_BUTTON_DOWN, OnArrowDown)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FLIGHT_PRIORITY, OnDblclkListFlightPriority)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_COMMAND(ID_BUTTON_EDIT, OnButtonEdit)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FLIGHT_PRIORITY, OnItemchangedListFlightPriority)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlightPriorityDlg message handlers

int CFlightPriorityDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
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

BOOL CFlightPriorityDlg::OnInitDialog() 
{
	CXTResizeDialog::OnInitDialog();
	
	m_treeProc.m_bDisplayAll=TRUE;

	//create the tool bar here.
	InitToolbar();

	SetResize(IDC_LIST_FLIGHT_PRIORITY,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	//set the style of the list control.
	//not finished, to be modified later.
	SetListCtrl();
	
	AfxGetApp()->EndWaitCursor();
	
	m_listctrlData.DeleteAllItems();

	m_btnSave.EnableWindow( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CFlightPriorityDlg::GetSelectedListItem()
{
	int nCount = m_listctrlData.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlData.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			return i;
	}
	return -1;
}


CString CFlightPriorityDlg::GetProjPath()
{
	POSITION ps = theApp.m_pDocManager->GetFirstDocTemplatePosition();
	POSITION ps2 = theApp.m_pDocManager->GetNextDocTemplate(ps)->GetFirstDocPosition();
	CTermPlanDoc *pDoc = (CTermPlanDoc*)(theApp.m_pDocManager->GetNextDocTemplate(ps)->GetNextDoc(ps2));
	return pDoc->m_ProjInfo.path;
}

void CFlightPriorityDlg::OnPeoplemoverDelete()
{
	int nIndex = m_listctrlData.GetCurSel();
	if(nIndex < 0 )
		return;
	m_listctrlData.DeleteItem(nIndex);
	//erase in the m_vectFlightGate;
	m_vectFlightGate.erase(m_vectFlightGate.begin() + nIndex);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );


	ResetOrder();

	m_btnSave.EnableWindow();
}

void CFlightPriorityDlg::OnPeoplemoverNew()
{
	//Popup the dialog and make the selection
	CFlightGateSelectionDlg flightGateSelectionDlg(this,m_nprjID,m_nAirportID);
	if(flightGateSelectionDlg.DoModal() != IDOK)
		return;

	if(!flightGateSelectionDlg.m_bChanged)
		return;

	//Decide the order of the new item automatically.
	//set the index number.
	int nCount = m_vectFlightGate.size();
	char strText[1024];
//	itoa(nCount, strText, 200);
	CString strTemp;
	strTemp.Format("%d",nCount);
	strcpy(strText,strTemp.GetBuffer(0));
	int nData = m_listctrlData.InsertItem(nCount-1, strText);
	m_listctrlData.SetItemData( nData, 0 );

	//print the flight constraint.
	CString sType = m_vectFlightGate[nCount - 1].flight.getPrintString();
	//m_vectFlightGate[nCount - 1].flight.screenPrint(sType);
	m_listctrlData.SetItemText(nCount-1, 1, sType);

	//print the gate processor id.
	m_vectFlightGate[nCount - 1].procID.printID(strText);
	m_listctrlData.SetItemText(nCount-1, 2, strText);

	m_listctrlData.SetItemState(nCount - 1, LVIS_SELECTED, LVIS_SELECTED);
	m_btnSave.EnableWindow();
	strTemp.ReleaseBuffer();
}


void CFlightPriorityDlg::OnArrowUp()
{
	//divide into two parts, list ctrl & procList
	//with more clear logic.

	//the aspect of list ctrl
	int nSel = GetSelectedListItem();
	int nUpper  = nSel - 1;
	CString strSelFlight, strUpperFlight;
	CString strSelGate, strUpperGate;

	//get the current text in the list.
	strSelFlight = m_listctrlData.GetItemText(nSel, 1);
	strUpperFlight = m_listctrlData.GetItemText(nUpper, 1);
	strSelGate = m_listctrlData.GetItemText(nSel, 2);
	strUpperGate = m_listctrlData.GetItemText(nUpper, 2);

	//swap the text in the list.
	m_listctrlData.SetItemText(nSel, 1, strUpperFlight);
	m_listctrlData.SetItemText(nUpper, 1, strSelFlight);
	m_listctrlData.SetItemText(nSel, 2, strUpperGate);
	m_listctrlData.SetItemText(nUpper, 2, strSelGate);

	m_listctrlData.SetItemState(nUpper, LVIS_SELECTED, LVIS_SELECTED);

	//Flight-Gate
	FlightGate tempFlightGate;
	tempFlightGate.flight = m_vectFlightGate[nSel - 1].flight;
	tempFlightGate.procID = m_vectFlightGate[nSel - 1].procID;

	m_vectFlightGate[nSel - 1].flight = m_vectFlightGate[nSel].flight;
	m_vectFlightGate[nSel - 1].procID = m_vectFlightGate[nSel].procID;

	m_vectFlightGate[nSel].flight = tempFlightGate.flight;
	m_vectFlightGate[nSel].procID = tempFlightGate.procID;
	m_btnSave.EnableWindow();
}

void CFlightPriorityDlg::OnArrowDown()
{

	//the aspect of list ctrl
	int nSel = GetSelectedListItem();
	int nDown  = nSel + 1;
	CString strSelFlight, strDownFlight;
	CString strSelGate, strDownGate;

	//get the current text in the list.
	strSelFlight = m_listctrlData.GetItemText(nSel, 1);
	strDownFlight = m_listctrlData.GetItemText(nDown, 1);
	strSelGate = m_listctrlData.GetItemText(nSel, 2);
	strDownGate = m_listctrlData.GetItemText(nDown, 2);

	//swap the text in the list.
	m_listctrlData.SetItemText(nSel, 1, strDownFlight);
	m_listctrlData.SetItemText(nDown, 1, strSelFlight);
	m_listctrlData.SetItemText(nSel, 2, strDownGate);
	m_listctrlData.SetItemText(nDown, 2, strSelGate);

	m_listctrlData.SetItemState(nDown, LVIS_SELECTED, LVIS_SELECTED);

	//Flight-Gate
	FlightGate tempFlightGate;
	tempFlightGate.flight = m_vectFlightGate[nSel + 1].flight;
	tempFlightGate.procID = m_vectFlightGate[nSel + 1].procID;

	m_vectFlightGate[nSel + 1].flight = m_vectFlightGate[nSel].flight;
	m_vectFlightGate[nSel + 1].procID = m_vectFlightGate[nSel].procID;

	m_vectFlightGate[nSel].flight = tempFlightGate.flight;
	m_vectFlightGate[nSel].procID = tempFlightGate.procID;
	m_btnSave.EnableWindow();
}

void CFlightPriorityDlg::OnDblclkListFlightPriority(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnButtonEdit();

}

void CFlightPriorityDlg::OnButtonSave()
{	

	//save the flight-gate info here.
	if( m_btnSave.IsWindowEnabled() )
	{
		GetInputTerminal()->flightPriorityInfo->m_vectFlightGate = m_vectFlightGate;
		CString strPath = GetProjPath();
		GetInputTerminal()->flightPriorityInfo->saveDataSet( strPath, true );
		m_btnSave.EnableWindow( FALSE );
	}
}

void CFlightPriorityDlg::OnButtonEdit()
{
	int nSel = GetSelectedItem();
	if(nSel == -1)
		return;
		
	CFlightGateSelectionDlg *flightGateSelectionDlg = new CFlightGateSelectionDlg(this,m_nAirportID, nSel);
	flightGateSelectionDlg->m_flight = m_vectFlightGate[nSel].flight;
	if( flightGateSelectionDlg->DoModal() == IDCANCEL )
	{
		delete flightGateSelectionDlg;
		return;
	}
	delete flightGateSelectionDlg;

	char strText[256];
	CString sType = m_vectFlightGate[nSel].flight.getPrintString();
	//print the flight constraint.
	//m_vectFlightGate[nSel].flight.screenPrint(sType);
	m_listctrlData.SetItemText(nSel, 1, sType);

	//print the gate processor id.
	m_vectFlightGate[nSel].procID.printID(strText);
	m_listctrlData.SetItemText(nSel, 2, strText);

	m_listctrlData.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
	m_btnSave.EnableWindow();
}

void CFlightPriorityDlg::OnOK() 
{
	OnButtonSave();
	//((CGateAssignDlg *)GetParent())->FlightPriorityAssign();	
	CXTResizeDialog::OnOK();
}

void CFlightPriorityDlg::OnItemchangedListFlightPriority(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	//get selected item and determine how to display the toolbar button
	int nCount = m_listctrlData.GetItemCount();
	int nSel = GetSelectedItem();
	if(nCount <= 1 && nSel == -1)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_UP, FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_DOWN, FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_EDIT, FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE, TRUE);

	}

	if(nSel != -1 )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_EDIT, TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE, TRUE);
		if(nSel == 0)
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_UP, FALSE);
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_DOWN	, TRUE);
		}
		else if(nSel == m_vectFlightGate.size() - 1)
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

//zero based selected item
int CFlightPriorityDlg::GetSelectedItem()
{
	int nCount = m_listctrlData.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlData.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			return i;
	}
	return -1;
}

void CFlightPriorityDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if(m_bPaintInit)
		return;
	try
	{
		GetInputTerminal()->flightPriorityInfo->loadDataSet(GetProjPath());
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;			
		return;
	}
	m_vectFlightGate = GetInputTerminal()->flightPriorityInfo->m_vectFlightGate;
	m_listctrlData.DeleteAllItems();
	int nCount = m_vectFlightGate.size();
	for(int i = 0; i < nCount; i++)
	{
		//order
		CString strText = _T("");
		strText.Format("%d",i+1);
	//	itoa(i+1, strText, 200);
		m_listctrlData.InsertItem(i, strText);

		//flight/sector
		CString sType = m_vectFlightGate[i].flight.getPrintString();
		//m_vectFlightGate[i].flight.screenPrint(sType);
		m_listctrlData.SetItemText(i, 1, sType);

		//gate
		char szbuff[200];
		m_vectFlightGate[i].procID.printID(szbuff);
		m_listctrlData.SetItemText(i, 2, szbuff);
	}
	m_bPaintInit = true;
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CFlightPriorityDlg::ResetOrder()
{
	int nCount = m_listctrlData.GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		char strOrder[200];
		itoa(i+1, strOrder, 10);
		m_listctrlData.SetItemText(i, 0, strOrder);
	}
}
