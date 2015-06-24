// ProcAssignDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ProcAssignDlg.h"
#include "TermPlanDoc.h"
#include "ProcesserDialog.h"
#include "PassengerTypeDialog.h"
#include "AssignFlightDlg.h"
#include "singleEventsDataDlg.h"
#include "rosterwizarddlg.h"
#include "..\inputs\AssignDB.h"
#include "..\inputs\Assign.h"
#include "..\inputs\Schedule.h"
#include "..\common\WinMsg.h"
#include "..\engine\AutoRoster.h"
#include ".\procassigndlg.h"
#include "../engine/proclist.h"
#include "RosterList.h"
#include "GTCtrlDlg.h"
#include "DlgAutoRosterRules.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CANNOT_MODIFY_COLOR		RGB(128,128,128)
#define MANUAL_DEFINE_COLOR		RGB(0,255,255)
#define FROM_SCHEDULE_COLOR		RGB(255,255,0)
/////////////////////////////////////////////////////////////////////////////
// CProcAssignDlg dialog

IMPLEMENT_DYNAMIC(CProcAssignDlg, CXTResizeDialog)
CProcAssignDlg::CProcAssignDlg(CWnd* pParent)
	: CXTResizeDialog(CProcAssignDlg::IDD, pParent)
{

	//{{AFX_DATA_INIT(CProcAssignDlg)
	//}}AFX_DATA_INIT
	m_bCopied = false;
	m_bDetailProc = false;
	m_iGTmode = DETAIL_MODE;
	m_pParent=pParent;
	ndays = 1 ;
	m_treeProc.setInputTerminal( GetInputTerminal() );

	m_nMaxDayCount = 3;//Default

    
	m_RosterListDlg = new CRosterList(this,pParent) ;
	m_GTDlg = new CGTCtrlDlg(this,pParent) ;
	m_TabCtrl.SetMaimDlg(this);
   
 //  	p_rulesDB = new CAutoRosterRulesDB(GetInputTerminal()) ;
	//p_ruleSchedule = new CAutoRosterByScheduleRulesDB(GetInputTerminal());
	m_RosterListColNames.push_back(_T("Processor")) ;
	m_RosterListColNames.push_back(_T("Pax Type")) ;
	m_RosterListColNames.push_back(_T("Open Time")) ;
	m_RosterListColNames.push_back(_T("Close Time")) ;
	m_RosterListColNames.push_back(_T("Assignment Relation")) ;
	m_RosterListColNames.push_back(_T("IsDaily")) ;
}


void CProcAssignDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDOK, m_butOK);
	DDX_Control(pDX, IDCANCEL, m_butCancel);
    DDX_Control(pDX,IDC_BUTTON_COPY,m_btnCopy);
	DDX_Control(pDX,IDC_BUTTON_PASTE,m_btnPaste);
	DDX_Control(pDX, IDC_TREE_PROCESSOR, m_treeProc);
	DDX_Control(pDX, IDC_TAB_ROSTER, m_TabCtrl);
	DDX_Control(pDX, IDC_LIST_ROSTER_GROUP, m_rosterGroup);
	DDX_Control(pDX, IDC_STATIC_PROCESSOR, m_StaticPro);
	DDX_Control(pDX, IDC_STATIC_RULE, m_staticRule);
    DDX_Control(pDX,IDC_CLOSE_PROCESSOR,m_check_close) ;
}


BEGIN_MESSAGE_MAP(CProcAssignDlg, CXTResizeDialog)
	//{{AFX_MSG_MAP(CProcAssignDlg)
	ON_WM_CREATE()


	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROCESSOR, OnSelchangedTreeProcessor)


    ON_COMMAND(ID_PROCASSIGN_NEW,NewRosterForProcessorGroup) 
	ON_COMMAND(ID_PROCASSIGN_EDIT,EditRosterForProcessorGroup)
	ON_COMMAND(ID_PROCASSIGN_DELETE , DelRosterForProcessorGroup) 
	ON_COMMAND(ID_BUTTON_EXPORT_NEW,ExportFileFromRosterList)
	ON_COMMAND(ID_BUTTON_IMPORT_NEW,ImportFileForRosterList)
	ON_WM_SIZE()
	ON_WM_PAINT()


	ON_BN_CLICKED(IDC_BUTTON_WIZARD, OnButtonWizard)
    ON_BN_CLICKED(IDC_BUTTON_PASTE, OnButtonPaste)
	ON_BN_CLICKED(IDC_BUTTON_COPY, OnButtonCopy)
	ON_BN_CLICKED(IDC_BUTTON_AUTO,OnButtonAutoAss)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	//}}AFX_MSG_MAP
	ON_STN_CLICKED(IDC_STATIC_LISTTOOLBARCONTENTER, OnStnClickedStaticListtoolbarcontenter)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnTcnSelchangeTab1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ROSTER_GROUP, OnLvnItemchangedListRules)
	//ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RULES_SCHEDULE, OnLvnItemchangedListRules)

	ON_NOTIFY(HDN_ITEMCLICK, IDC_LIST_ROSTER_GROUP, OnHdnItemclickListRules)
 //   ON_NOTIFY(HDN_ITEMCLICK, IDC_LIST_RULES_SCHEDULE, OnHdnItemclickListRules)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_ROSTER, OnTcnSelchangeTabRoster)
	ON_WM_CTLCOLOR() 
//	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipText) 
    ON_BN_CLICKED(IDC_CLOSE_PROCESSOR,OnClickCloseProcessor)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
//// CProcAssignDlg message handlers
//void CProcAssignDlg::OnDailyEvent()
//{
//	CString mes ;
//	mes.Format(_T("Will convert all the event time into the one day, to be continued ?")) ;
//	if (MessageBox(mes,_T("Warning"),MB_OKCANCEL) == IDOK)
//	{
//		HTREEITEM item = m_treeEvents.GetSelectedItem() ;
//		CString proname = m_treeEvents.GetItemText(item) ;
//		SetProcessorDaily(proname) ;
//		m_btnSave.EnableWindow();
//	}
//}
//void CProcAssignDlg::SetProcessorDaily(CString& ProID)
//{
//	ProcessorID id ;
//	id.SetStrDict( GetInputTerminal()->inStrDict );
//	id.setID( ProID );
//	int index  = GetInputTerminal()->procAssignDB->findEntry(id) ;
//	if(index == INT_MAX )
//		return ;
//	ProcAssignSchedule*  sched = GetInputTerminal()->procAssignDB->getDatabase( index );
//    if(sched == NULL)
//		return ;
//	ProcAssignment* pro_ass = NULL ;
//    for (int i = 0 ; i < sched->getCount() ;i++)
//    {
//       pro_ass =  sched->getItem(i) ;
//       AssigmentData(pro_ass) ;
//    }
//     HTREEITEM item = m_treeEvents.GetSelectedItem() ;
//	 HTREEITEM hPareant = m_treeEvents.GetParentItem(item) ;
//	IDENTICAL* p_iden = SearchIdentical(hPareant);
//	if( p_iden == NULL )
//	{
//		HTREEITEM hProcItem = m_treeProc.GetSelectedItem();
//		bool bOwn;
//		SetSchdEvent(hProcItem, bOwn,true);
//	}
//	else
//	{
//		UpdateProcessorEvents(p_iden,index);
//	}
//}
//void CProcAssignDlg::AssigmentData(ProcAssignment* proAss)
//{
//    if(proAss == NULL)
//		return  ;
//	ElapsedTime opentime =  proAss->getOpenTime() ;
//	ElapsedTime closetime = proAss->getCloseTime() ;
//	if(opentime >= WholeDay)
//		opentime = opentime - WholeDay ;
//	if(closetime >= WholeDay)
//		closetime = closetime - WholeDay;
//	if(closetime <= opentime)
//		closetime = WholeDay -1 ;
//	proAss->setOpenTime(opentime) ;
//	proAss->setCloseTime(closetime) ;
//	proAss->setAbsOpenTime(opentime) ;
//	proAss->setAbsCloseTime(closetime) ;
//    proAss->IsDaily(TRUE) ;
//
//    
//}

BOOL CProcAssignDlg::OnInitDialog() 
{
	CXTResizeDialog::OnInitDialog();
	AfxGetApp()->BeginWaitCursor();
	CString _path = GetProjPath() ;
//	GetInputTerminal()->procAssignDB->loadDataSet(_path) ;
	//p_rulesDB->ReadDataFromDB() ;
	//p_ruleSchedule->ReadDataFromDB() ;
	m_flightSchedule = GetInputTerminal()->flightSchedule;

	////////////////////////////////////////////////
	//GetInputTerminal()->m_pAutoRoster->addAutoRostorToDateSet( 0, GetInputTerminal() );
	////////////////////////////////////////////////
	m_bDetailProc = true;
	InitToolbar();
	InitProcessorTree() ;
	m_treeProc.m_bDisplayAll=TRUE;
	//init static text 
	HTREEITEM _selItem = m_treeProc.GetSelectedItem() ;

	//	InitStatic(CString("All Processor")) ;
	//set 
	AfxGetApp()->EndWaitCursor();
    
	//Init Tab 
	InitTabCtrl() ;
	//Init rule list 
	
	//set days of schedule 
	FlightSchedule* fs = GetInputTerminal()->flightSchedule;
	if (fs->getCount() > 0)
		ndays = fs->GetFlightScheduleEndTime().GetDay();
    InitRosterForGroupList();
    m_treeProc.SelectItem(m_treeProc.GetRootItem()) ;

	m_btnPaste.EnableWindow(FALSE);

	SetResize(IDC_STATIC_PRO, SZ_TOP_LEFT, SZ_MIDDLE_CENTER);
	SetResize(IDC_STATIC_FRAME,SZ_TOP_CENTER,SZ_MIDDLE_RIGHT) ;
	SetResize(IDC_STATIC_GROUP,SZ_MIDDLE_LEFT,SZ_BOTTOM_RIGHT) ;

	SetResize(IDC_STATIC_PROCESSOR,SZ_TOP_LEFT,SZ_TOP_CENTER) ;
	SetResize(m_treeProc.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_MIDDLE_CENTER) ;

	SetResize(IDC_STATIC_RULE,SZ_TOP_CENTER,SZ_TOP_CENTER) ;
	SetResize(IDC_STATIC_HX,SZ_TOP_CENTER,SZ_TOP_RIGHT) ;


	SetResize(IDC_LIST_ROSTER_GROUP,SZ_TOP_CENTER,SZ_MIDDLE_RIGHT) ;
    SetResize(m_check_close.GetDlgCtrlID(),SZ_MIDDLE_CENTER,SZ_MIDDLE_RIGHT) ;


	SetResize(IDC_TAB_ROSTER,SZ_MIDDLE_LEFT,SZ_BOTTOM_RIGHT) ;
	//SetResize(m_RosterListDlg->GetDlgCtrlID(),SZ_MIDDLE_LEFT,SZ_BOTTOM_RIGHT);
	//SetResize(IDD_DIALOG_GT,SZ_MIDDLE_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;

	SetResize(IDC_BUTTON_WIZARD,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT) ;
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDC_BUTTON_PASTE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDC_BUTTON_COPY,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDC_BUTTON_AUTO,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;

	SetResize(m_RukeToolBar.GetDlgCtrlID(),SZ_TOP_CENTER,SZ_TOP_CENTER) ;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CProcAssignDlg::Resizedialog()
{

}
void CProcAssignDlg::InitProcessorTree()
{
	GetInputTerminal()->procAssignDB->computeAbsolutTime( GetInputTerminal(), GetInputTerminal()->flightSchedule );
	m_treeProc.LoadData( GetInputTerminal(), (ProcessorDatabase*)GetInputTerminal()->procAssignDB );
}
void CProcAssignDlg::InitTabCtrl()
{
	InitTabCtrlView() ;
	InitTabCtrlData() ;
}
void CProcAssignDlg::InitTabCtrlView()
{

 BOOL val =   m_RosterListDlg->Create(IDD_DIALOG_ROSTERLIST,&m_TabCtrl);
 int code = GetLastError() ;
   m_GTDlg->Create(IDD_DIALOG_GT,&m_TabCtrl) ;
  // m_RosterListDlg->SetWindowPos (NULL,10,30,400,100,SWP_SHOWWINDOW); 
  // m_GTDlg->SetWindowPos (NULL,10,30,400,100,SWP_HIDEWINDOW ); 

   m_TabCtrl.AddSSLPage (_T("Roster Table"),0, m_RosterListDlg);
   m_TabCtrl.AddSSLPage (_T("Graph"), 1, m_GTDlg);

   m_TabCtrl.SetCurSel(0) ;
   m_RosterListDlg->ShowWindow(TRUE) ;
}
void CProcAssignDlg::InitTabCtrlData()
{


}

	

CString CProcAssignDlg::GetRelationName(int _rel)
{
	CString _name ;
	switch(_rel)
	{
	case Inclusive:
		_name.Append(_T("Inclusive")) ;
		break;
	case Exclusive:
		_name.Append(_T("Exclusive")) ;
		break;
	case Not:
		_name.Append(_T("Not")) ;
		break;
	}
	return _name ;
}
ProcAssRelation CProcAssignDlg::GetRelationByName(CString name)
{
	ProcAssRelation rel ;
	if(name == "Inclusive")
		rel = Inclusive ;
	if(name == "Exclusive")
		rel = Exclusive ;
	if(name == "Not")
		rel = Not ;
	return rel ;
}
CString CProcAssignDlg::GetIsDailyName(int _data)
{
	CString name ;
	if(_data == 1)
		name = "TRUE" ;
	else
		name = "FALSE" ;
	return name ;
}
CProcAssignDlg::~CProcAssignDlg()
{


	if(m_RosterListDlg != NULL)
		delete m_RosterListDlg ;
	if(m_GTDlg != NULL)
		delete m_GTDlg ;

}
void CProcAssignDlg::InitStatic(CString& _name)
{
 //set static name of processor
	if(strcmp(_name,"All Processors") == 0)
	{
		m_StaticPro.SetWindowText(_T("All Processor")) ;
		m_staticRule.SetWindowText(_T("Roster For:")) ;
	}else
	{
		m_StaticPro.SetWindowText(_T("Processor of ") + _name) ;
		m_staticRule.SetWindowText(_T("Roster For ") + _name +" :") ;
	}
}


InputTerminal* CProcAssignDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}

CString CProcAssignDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}



int CProcAssignDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	if (!m_RukeToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP) ||
		!m_RukeToolBar.LoadToolBar(IDR_PROCASSIGN_NEW))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}


void CProcAssignDlg::InitToolbar()
{
    InitRuleToolBar() ;

}
void CProcAssignDlg::InitRuleToolBar()
{
	CRect rc ;
	m_rosterGroup.GetWindowRect(&rc) ;
	ScreenToClient(&rc);
	rc.top -= 24;
	rc.bottom = rc.top + 22;
	rc.left += 4;
	m_RukeToolBar.MoveWindow(&rc) ;
	m_RukeToolBar.ShowWindow(SW_SHOW) ;
	m_RukeToolBar.GetToolBarCtrl().EnableButton( ID_PROCASSIGN_NEW ,TRUE);	
	m_RukeToolBar.GetToolBarCtrl().EnableButton( ID_PROCASSIGN_DELETE ,FALSE);	
	m_RukeToolBar.GetToolBarCtrl().EnableButton( ID_PROCASSIGN_EDIT, FALSE);
	m_RukeToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_EXPORT_NEW, TRUE);
    m_RukeToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_IMPORT_NEW, TRUE);
}



void CProcAssignDlg::OnSelchangedTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	LoadProcessorData();
}

void CProcAssignDlg::LoadProcessorData()
{
	HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	if(hSelItem == NULL )
		return ;
	m_treeProc.Expand(hSelItem,TVE_EXPAND) ;
	if(!GetCurrentSelProcessID(ProcessorID()))
	{
		m_RosterListDlg->InitALLProcessorData();
		m_GTDlg->InitAllProcessorData() ;
		m_check_close.EnableWindow(FALSE) ;
	}else
		m_check_close.EnableWindow(TRUE) ;
	m_btnCopy.EnableWindow(TRUE);
	ProcessorID id ;
	GetCurrentSelProcessID(id) ;
	InitStatic(id.GetIDString()) ;
	m_RosterListDlg->InitRosterListData() ;
	m_GTDlg->InitGTCtrlData() ;
	InitRosterForGroupListData();
	InitCheckCloseProcessorState();
}


void CProcAssignDlg::OnOK() 
{

	ResetAllProcessorDailyData();
	AfxGetApp()->BeginWaitCursor();
	GetInputTerminal()->procAssignDB->saveDataSet( GetProjPath(), true );
	AfxGetApp()->EndWaitCursor();
	CDialog::OnOK();
}

void CProcAssignDlg::OnButtonSave()
{
	ResetAllProcessorDailyData();
	AfxGetApp()->BeginWaitCursor();
	GetInputTerminal()->procAssignDB->saveDataSet( GetProjPath(), true );
	AfxGetApp()->EndWaitCursor();
}

void CProcAssignDlg::OnCancel() 
{
	CString _path = GetProjPath() ;
    GetInputTerminal()->procAssignDB->loadDataSet(_path) ;
	CDialog::OnCancel();
}


int CProcAssignDlg::GetCurrentSelProcessorGroupNumber()
{
	ProcessorID id ;
	if(!GetCurrentSelProcessID(id))
		return 0 ;
	GroupIndex group = GetInputTerminal()->procList->getGroupIndex (id);
	int cout = 0 ;
	if(group.start < 0 || group.end < 0)
		cout = 1 ;
	else
		cout  = group.end - group.start ;
	return cout + 1 ;
}


BEGIN_EVENTSINK_MAP(CProcAssignDlg, CXTResizeDialog)
 //   //{{AFX_EVENTSINK_MAP(CProcAssignDlg)
	//ON_EVENT(CProcAssignDlg, IDC_GTCHARTCTRL, 1 /* ClickLine */, OnClickLineGtchartctrl, VTS_I4)
	//ON_EVENT(CProcAssignDlg, IDC_GTCHARTCTRL, 7 /* DblClickItem */, OnDblClickItemGtchartctrl, VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_I4 VTS_I4)
	//ON_EVENT(CProcAssignDlg, IDC_GTCHARTCTRL, 9 /* ItemResized */, OnItemResizedGtchartctrl, VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_I4)
	//ON_EVENT(CProcAssignDlg, IDC_GTCHARTCTRL, 10 /* ItemMoved */, OnItemMovedGtchartctrl, VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_I4)
	//ON_EVENT(CProcAssignDlg, IDC_GTCHARTCTRL, 14 /* ClickBtnModeDetail */, OnClickBtnModeDetailGtchartctrl, VTS_NONE)
	//ON_EVENT(CProcAssignDlg, IDC_GTCHARTCTRL, 15 /* ClickBtnModeSimple */, OnClickBtnModeSimpleGtchartctrl, VTS_NONE)
	//ON_EVENT(CProcAssignDlg, IDC_GTCHARTCTRL, 16 /* ShowItemToolTip */, OnShowItemToolTipGtchartctrl, VTS_I4 VTS_I4 VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()





//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
void CProcAssignDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x = 750;
	lpMMI->ptMinTrackSize.y = 500;
	CXTResizeDialog::OnGetMinMaxInfo(lpMMI);
}


void CProcAssignDlg::OnSize(UINT nType, int cx, int cy) 
{
	CXTResizeDialog::OnSize(nType, cx, cy);
	//cx
	//	Specifies the new width of the client area.

	//cy
	//	Specifies the new height of the client area.

	if(m_TabCtrl.m_hWnd){
		int nActPage = m_TabCtrl.GetSSLActivePage();
		int nPageCount = m_TabCtrl.GetSSLPageCount();
		for (int i = 0;i < nPageCount;++i) {
			if (i != nActPage) {
				m_TabCtrl.ActivateSSLPage (i);
				break;
			}
		}
		m_TabCtrl.ActivateSSLPage (nActPage);
	}
	if(m_rosterGroup.m_hWnd)
        m_rosterGroup.ResizeColumnWidth();
}

void CProcAssignDlg::OnPaint() 
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
			pWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl))||
			pWnd->IsKindOf(RUNTIME_CLASS(CGTChart)))
		{
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

BOOL CProcAssignDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	//return CDialog::OnEraseBkgnd(pDC);
	return true;
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////

void CProcAssignDlg::ResetAllProcessorDailyData()
{
	ProcAssignDatabase* database =  GetInputTerminal()->procAssignDB ;
	StringList str_list ;
	TCHAR proname[1024] = { 0} ;
	ProcessorRosterSchedule* sche = NULL ;
	int index =0 ;
	GetInputTerminal()->GetProcessorList()->getAllGroupNames (str_list, -1 );
	
	for(int i = 0 ;i < GetInputTerminal()->GetProcessorList()->getProcessorCount() ; i++)
	{
		GetInputTerminal()->GetProcessorList()->getNameAt(i,proname) ;
		CString name(proname) ;
		name.Replace(',','-') ;
		ProcessorID id;
		id.SetStrDict( GetInputTerminal()->inStrDict );
		id.setID( name);
         index  = database->findEntry(id) ;
		 if(index == INT_MAX )
			 continue ;
		 sche = GetInputTerminal()->procAssignDB->getDatabase( index );
		sche->ResetDailyData(ndays) ;
	}
}
BOOL CProcAssignDlg::GetCurrentSelProcessID(ProcessorID& _Id) 
{
	HTREEITEM hsellItem = m_treeProc.GetSelectedItem() ;
	if(hsellItem == NULL || strcmp(m_treeProc.GetItemText(hsellItem),"ALL PROCESSOR") == 0 )
		return FALSE;
	CString proname ;
	proname = m_treeProc.GetItemText(hsellItem) ;
	_Id.SetStrDict( GetInputTerminal()->inStrDict );
	_Id.setID(proname );
	return TRUE ;
}
BOOL CProcAssignDlg::GetCurrentSelProcessID(HTREEITEM _proNode,ProcessorID& _Id) 
{
	if(_proNode == NULL)
		return FALSE ;
	CString proname ;
	proname = m_treeProc.GetItemText(_proNode) ;
	_Id.SetStrDict( GetInputTerminal()->inStrDict );
	_Id.setID(proname );
	return TRUE ;
}
BOOL CProcAssignDlg::FindDefinedRosterProcessor(HTREEITEM _proNode,ProcessorID& _Id)
{
	if(_proNode == NULL)
		return FALSE ;
	HTREEITEM hPar = m_treeProc.GetParentItem(_proNode) ;
	while(hPar)
	{
		if(hPar == NULL || strcmp(m_treeProc.GetItemText(hPar),"ALL PROCESSOR") == 0 )
			return FALSE;
		CString proname ;
		proname = m_treeProc.GetItemText(hPar) ;
		_Id.SetStrDict( GetInputTerminal()->inStrDict );
		_Id.setID(proname );
		int ndx = GetInputTerminal()->procAssignDB->findEntry(_Id) ;
		if(ndx == INT_MAX)
			hPar = m_treeProc.GetParentItem(hPar) ;
		else
		{
			if( GetInputTerminal()->procAssignDB->getDatabase(ndx)->getCount() != 0)
				return TRUE ;
			else
                hPar = m_treeProc.GetParentItem(hPar) ;
		}
			
	}
	return FALSE ;
}
BOOL CProcAssignDlg::getCurrentSelParentProcessID(ProcessorID& _id)
{
	HTREEITEM hsellItem = m_treeProc.GetSelectedItem() ;
	if(hsellItem == NULL || strcmp(m_treeProc.GetItemText(hsellItem),"ALL PROCESSOR") == 0 )
		return FALSE;
	CString proname ;
	HTREEITEM hPar = m_treeProc.GetParentItem(hsellItem) ;
	proname = m_treeProc.GetItemText(hPar) ;
	_id.SetStrDict( GetInputTerminal()->inStrDict );
	_id.setID(proname );
	return TRUE ;
}

void CProcAssignDlg::OnStnClickedStaticListtoolbarcontenter()
{
	// TODO: Add your control notification handler code here
}

void CProcAssignDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CProcAssignDlg::OnLvnItemchangedListRules(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_RukeToolBar.GetToolBarCtrl().EnableButton( ID_PROCASSIGN_DELETE ,TRUE);	
	m_RukeToolBar.GetToolBarCtrl().EnableButton( ID_PROCASSIGN_EDIT, TRUE);
	m_RosterListDlg->InitRosterListData();
	*pResult = 0;
}
HBRUSH CProcAssignDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC_PROCESSOR:
		pDC->SetBkMode(TRANSPARENT); 
		pDC->SetTextColor(RGB(255,255,255)) ;
		HBRUSH B = CreateSolidBrush(RGB(37,22,238)); //COLOR
		return (HBRUSH) B; 
	}
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}



void CProcAssignDlg::OnHdnItemclickListRules(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_RukeToolBar.GetToolBarCtrl().EnableButton( ID_PROCASSIGN_DELETE ,TRUE);	
	m_RukeToolBar.GetToolBarCtrl().EnableButton( ID_PROCASSIGN_EDIT, TRUE);
		//m_RosterListDlg->InitRosterListData();
	*pResult = 0;
}

void CProcAssignDlg::OnTcnSelchangeTabRoster(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

}

CTabCtrlSSLEx::CTabCtrlSSLEx():CTabCtrlSSL()
{

}
CTabCtrlSSLEx::~CTabCtrlSSLEx()
{

}
void CTabCtrlSSLEx::OnActivatePage (int nIndex, int nPageID)
{
	if(m_maindlg == NULL)
		return ;
	ProcessorID id ;
	if(!m_maindlg->GetCurrentSelProcessID(id))
		return ;

	if(nIndex == 0)
		m_maindlg->GetRosterList()->InitRosterListData() ;
	else
		//inti gt 
		m_maindlg->GetGTListDlg()->InitGTCtrlData() ;
}
void CTabCtrlSSLEx::SetMaimDlg(CProcAssignDlg* _mainDlg)
{
	m_maindlg = _mainDlg ;
}
void CProcAssignDlg::OnButtonWizard() 
{
	CRosterWizardDlg dlg( GetInputTerminal(), (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument(),GetProjPath(),m_pParent);
	if( dlg.DoModal() == IDOK )
	{
		ReloadDatabase();
		
	}	
}
void CProcAssignDlg::ReloadDatabase()
{
	CWaitCursor wc;

	GetInputTerminal()->procAssignDB->computeAbsolutTime( GetInputTerminal(), GetInputTerminal()->flightSchedule );
	m_treeProc.LoadData( GetInputTerminal(), (ProcessorDatabase*)GetInputTerminal()->procAssignDB );
	m_treeProc.SelectItem( m_treeProc.GetRootItem() );
    m_RosterListDlg->InitRosterListData() ;
	m_GTDlg->InitGTCtrlData() ;

}
void CProcAssignDlg::OnButtonCopy() 
{
	m_unMergedAssgn_readyToPaste.clear();
	m_bCopied = true;
	//do the copy here.
	//get the current selected item in the tree ctrl
	HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	//if no item selected
	if(hSelItem == NULL)
	{
		m_btnCopy.EnableWindow(FALSE);
		return;

	}
	//get the roster info of the current selected item.
	//	HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	int nDBIdx = m_treeProc.GetItemData( hSelItem );
	while( nDBIdx == -1 && hSelItem != NULL ) 
	{
		hSelItem = m_treeProc.GetParentItem( hSelItem );
		if( hSelItem == NULL )
			break;
		nDBIdx = m_treeProc.GetItemData( hSelItem );

	}
	if(nDBIdx == -1)
	{
		//use the default data for copying.
		m_unMergedAssgn_readyToPaste.clear();

		ProcessorRoster *newAssign = new ProcessorRoster( GetInputTerminal() );
		m_unMergedAssgn_readyToPaste.addItem (newAssign);

		m_btnCopy.EnableWindow(FALSE);
		return;
	}
	//get the roster info from the input database.
	ProcessorRosterSchedule* pProcSchd = GetInputTerminal()->procAssignDB->getDatabase( nDBIdx );	
	//set the unMergedAssign for a serial of roster assignments.
	m_unMergedAssgn_readyToPaste.SetInputTerminal( GetInputTerminal() );
	m_unMergedAssgn_readyToPaste.split( pProcSchd );
	//dis-enable the copy button
	m_btnCopy.EnableWindow(FALSE);
	m_btnPaste.EnableWindow(TRUE);
}
void CProcAssignDlg::OnButtonPaste() 
{

	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL ) 
		return;
	// if the selected item is not defined yet.
	// add a new item into the input database.
	int nDBIdx = m_treeProc.GetItemData( hItem );
	if( nDBIdx == -1 )
	{
		ProcessorID id;
		CString str = m_treeProc.GetItemText( hItem );
		id.SetStrDict( GetInputTerminal()->inStrDict );
		id.setID( str );

		GetInputTerminal()->procAssignDB->addEntry( id );

		m_treeProc.ResetTreeData( NULL );

		int nIdx = GetInputTerminal()->procAssignDB->findEntry( id );
		assert( nIdx != INT_MAX );
		ProcessorRosterSchedule* pSchd = GetInputTerminal()->procAssignDB->getDatabase( nIdx );
		m_unMergedAssgn_readyToPaste.merge(pSchd);
	}
	// else if the selected item existed.
	// just find the corresponding item in the input database.
	// and substitute it with the current pasting one.
	else
	{
		ProcessorRosterSchedule* pProcSchd = GetInputTerminal()->procAssignDB->getDatabase( nDBIdx );
		pProcSchd->clear();
		m_unMergedAssgn_readyToPaste.merge( pProcSchd );
	}
	m_RosterListDlg->InitRosterListData();
	LoadProcessorData();
	// disable the paste button here.
	//m_btnPaste.EnableWindow(FALSE);

}
void CProcAssignDlg::InitRosterForGroupList()
{
	InitRosterForGroupListView();
	InitRosterForGroupListData() ;
}
void CProcAssignDlg::InitRosterForGroupListView()
{
	DWORD dwStyle = m_rosterGroup.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_rosterGroup.SetExtendedStyle( dwStyle );
	int nwidth[] = {160,160,90,90,60,50} ;
	for (int i = 0 ; i < (int)m_RosterListColNames.size() ; i++)
	{
		m_rosterGroup.InsertColumn(i,m_RosterListColNames[i],LVCFMT_CENTER,nwidth[i]) ;
	}
	m_rosterGroup.InitColumnWidthPercent() ;
}
void CProcAssignDlg::InitRosterForGroupListData()
{
	m_rosterGroup.DeleteAllItems() ;
	// sort processor name in gt_ctrl
	ProcessorID id ;
	GetCurrentSelProcessID(id) ;
	InsertRosterOfProcessorGroup(id) ;
	InitCheckCloseProcessorState();
}
void CProcAssignDlg::ShowAllProcessorGroupRoster()
{
    m_rosterGroup.DeleteAllItems() ;
	StringList C_Array ;
	ProcessorID id ;
	id.SetStrDict( GetInputTerminal()->inStrDict );
	GetInputTerminal()->procList->getAllGroupNames(C_Array,-1) ;
	for (int i = 0 ; i < (int)C_Array.getCount();i++)
	{
		id.setID(C_Array.getString(i));
		InsertRosterOfProcessorGroup(id) ;
	}
}
void CProcAssignDlg::InsertRosterOfProcessorGroup(ProcessorID& _id)
{
	CString str = _id.GetIDString();
	int ndx = GetInputTerminal()->procAssignDB->findEntry(_id) ;
	if(ndx == INT_MAX)
		return ;
	ProcessorRosterSchedule* _schedule = GetInputTerminal()->procAssignDB->getDatabase(ndx) ;
	if(_schedule == NULL)
		return ;
	ProcessorRoster* _ass = NULL;
	ItemRosterList _data ;
	for (int i =0 ; i < _schedule->getCount() ; i++)
	{
		if(_schedule->getItem(i)->getFlag() == 0)
			return ;
	}
	for (int i =0 ; i < _schedule->getCount() ; i++)
	{
		_ass = _schedule->getItem(i) ;
		_data.c_Processor = _id.GetIDString() ;
		_ass->getAssignment()->getConstraint(0)->screenPrint( _data.c_PaxTy) ;
		_data.c_closeTime = _ass->getAbsCloseTime().printTime() ;
		_data.c_openTime = _ass->getAbsOpenTime().printTime() ;
		_data.c_relation =GetRelationName(_ass->getAssRelation()) ;
		_data.c_isDaily = GetIsDailyName(_ass->IsDaily()) ;
		AddLineToRosterForGroupList(_data,_ass) ;
		m_treeProc.SetItemColor( m_treeProc.GetSelectedItem(), RGB(53,151,53) );;
		m_treeProc.SetItemBold(  m_treeProc.GetSelectedItem(), true );
	}
}
void CProcAssignDlg::NewRosterForProcessorGroup()
{
	ProcessorID id ;
	if(!GetCurrentSelProcessID(id))
		return ;
	ProcessorRoster* pProcAssn = new ProcessorRoster( GetInputTerminal() );
	pProcAssn->setCloseTime((ElapsedTime)(86400.0 *3 -1));
	pProcAssn->setAbsCloseTime((ElapsedTime)(86400.0 *3 -1));
	int nIdx = GetInputTerminal()->procAssignDB->findEntry( id );
	Processor* p_pro = GetInputTerminal()->GetProcessorList()->getProcessor(id) ;
	if(nIdx == INT_MAX)
		GetInputTerminal()->procAssignDB->addEntry(id ) ;
	nIdx = GetInputTerminal()->procAssignDB->findEntry( id );
	ProcessorRosterSchedule* pSchd = GetInputTerminal()->procAssignDB->getDatabase( nIdx );
	if(m_check_close.GetCheck() == BST_CHECKED)
		pSchd->RemoveAllProcAssignment() ;
	CsingleEventsDataDlg dlg( m_pParentWnd ,TRUE ,p_pro,pSchd);
	dlg.SetDlgData( id.GetIDString(), *pProcAssn );
	if( dlg.DoModal()!=IDOK)
	{
		delete pProcAssn;
		return;
	}
	dlg.GetDlgData( *pProcAssn);
	//////////////////////////////////////////////////////////////////////////
	if( (!pSchd->processorIsClose() && pProcAssn->isOpen() == 0)			//(open->close) now delete all other event
		|| (pSchd->processorIsClose() && pProcAssn->isOpen()) )			//(close->open) now delete all old event( close event ) 
	{
		pSchd->clear();
		pSchd->ClearAllDailyData() ;
	}
	else if( pSchd->processorIsClose() && pProcAssn->isOpen() == 0 )	//(close->close) can't do this
	{
		MessageBox("The processor already closed!",NULL,MB_OK|MB_ICONINFORMATION );
		delete pProcAssn;	
		return;
	}
	pSchd->addItem(pProcAssn) ;
	if(pProcAssn->getFlag() == 0)
		return ;
	ItemRosterList _RosterItem ;
	_RosterItem.c_Processor = id.GetIDString() ;
	const CMultiMobConstraint* pMultiConst = pProcAssn->getAssignment();	
	const CMobileElemConstraint* pConst = pMultiConst->getConstraint( 0 );
	pConst->screenPrint(_RosterItem.c_PaxTy) ;
	_RosterItem.c_openTime = pProcAssn->getAbsOpenTime().printTime() ;
	_RosterItem.c_closeTime = pProcAssn->getAbsCloseTime().printTime() ;
	_RosterItem.c_relation = GetRelationName(pProcAssn->getAssRelation()) ;
	_RosterItem.c_isDaily = GetIsDailyName(pProcAssn->IsDaily()) ;
	AddLineToRosterForGroupList(_RosterItem ,pProcAssn) ;
	m_treeProc.SetItemColor( m_treeProc.GetSelectedItem(), RGB(53,151,53) );
	m_treeProc.SetItemBold( m_treeProc.GetSelectedItem(), true );
	m_RosterListDlg->InitRosterListData();
	m_GTDlg->InitGTCtrlData() ;

}
void CProcAssignDlg::EditRosterForProcessorGroup()
{
	POSITION  posi = m_rosterGroup.GetFirstSelectedItemPosition() ;
	ProcessorRoster* p_procAssn = NULL ;
	if(posi == NULL)
		return ;
	int ndx = 0 ;
	ndx =  m_rosterGroup.GetNextSelectedItem(posi) ;
	p_procAssn = (ProcessorRoster*)m_rosterGroup.GetItemData(ndx) ;
	if(p_procAssn == NULL)
		//delete GT 
	{
		return ;
	}
	ProcessorRosterSchedule* pProcSchd = GetCurSelProcSchdFromRosterList();
	if( !pProcSchd )
		return;
	if(pProcSchd->findItem(p_procAssn) == INT_MAX )
		return ;
	if( pProcSchd->processorIsClose() )
		return;
	ProcessorID id;
	if(!GetCurrentSelProcessID(id))
		return ;
	Processor* p_pro = GetInputTerminal()->GetProcessorList()->getProcessor(id) ;
	// edit the event

	CsingleEventsDataDlg dlg( m_pParentWnd,FALSE,p_pro, pProcSchd);
	dlg.SetDlgData( id.GetIDString(), *p_procAssn );
	if( dlg.DoModal()!=IDOK)	return;
	dlg.GetDlgData(*p_procAssn);
	ItemRosterList _RosterItem ;
	_RosterItem.c_Processor = id.GetIDString() ;
	const CMultiMobConstraint* pMultiConst = p_procAssn->getAssignment();	
	const CMobileElemConstraint* pConst = pMultiConst->getConstraint( 0 );
	pConst->screenPrint(_RosterItem.c_PaxTy) ;
	_RosterItem.c_openTime = p_procAssn->getAbsOpenTime().printTime() ;
	_RosterItem.c_closeTime = p_procAssn->getAbsCloseTime().printTime() ;
	_RosterItem.c_relation = GetRelationName(p_procAssn->getAssRelation()) ;
	_RosterItem.c_isDaily = GetIsDailyName(p_procAssn->IsDaily()) ;
	EditSelRosterForGroupList(_RosterItem) ;
	m_RosterListDlg->InitRosterListData();
	m_GTDlg->InitGTCtrlData() ;
}
ProcessorRosterSchedule* CProcAssignDlg::GetCurSelProcSchdFromRosterList()
{
	POSITION _pos = m_rosterGroup.GetFirstSelectedItemPosition() ;
	if(_pos == NULL)
		return NULL;
	int inx = m_rosterGroup.GetNextSelectedItem(_pos) ;
	TCHAR name[1024] = {0} ;
	m_rosterGroup.GetItemText(inx,0,name,1024) ;
	ProcessorID id ;
	id.SetStrDict(GetInputTerminal()->inStrDict) ;
	id.setID(name) ;
	inx = GetInputTerminal()->procAssignDB->findEntry(id) ;
	if(inx == INT_MAX)
		return NULL ;
	return  GetInputTerminal()->procAssignDB->getDatabase(inx) ;
}
void CProcAssignDlg::DelRosterForProcessorGroup()
{


	ProcessorRosterSchedule* pProcSchd = GetCurSelProcSchdFromRosterList();
	if( !pProcSchd )
		return;
	POSITION  posi = m_rosterGroup.GetFirstSelectedItemPosition() ;
	ProcessorRoster* p_Ass = NULL ;
	int selcount = 0 ;
	while(posi != NULL)
	{
		int ndx = 0 ;
		ndx =  m_rosterGroup.GetNextSelectedItem(posi) ;

		p_Ass = (ProcessorRoster*)m_rosterGroup.GetItemData(ndx) ;
		pProcSchd->RemoveProcAssignment(p_Ass) ;

		m_rosterGroup.SetItemData(ndx,NULL) ;
		selcount++ ;
	}

	if (pProcSchd->getCount() == 0)
	{
		ProcessorID id;
		if(!GetCurrentSelProcessID(id))
			return ;
		int nIdx = GetInputTerminal()->procAssignDB->findEntry( id );
		GetInputTerminal()->procAssignDB->deleteItem(nIdx);
		m_treeProc.SetItemColor(m_treeProc.GetSelectedItem(),RGB(0,0,0)) ;
		m_treeProc.SetItemBold(m_treeProc.GetSelectedItem(),false) ;
	}
	for ( int i = 0 ; i < selcount ;i++)
	{
		for (int j = 0 ; j < m_rosterGroup.GetItemCount() ;j++)
		{
			ProcessorRoster*	p_Ass = (ProcessorRoster*)m_rosterGroup.GetItemData(j) ;
			if(p_Ass == NULL)
			{
				m_rosterGroup.DeleteItem(j) ;
				break ;
			}
		}
	}
	m_RosterListDlg->InitRosterListData();
	m_GTDlg->InitGTCtrlData() ;
}
void CProcAssignDlg::AddLineToRosterForGroupList(ItemRosterList& _data,ProcessorRoster* _PAss)
{
	int ndx = m_rosterGroup.InsertItem(0,_data.c_Processor) ;
	m_rosterGroup.SetItemText(ndx,1,_data.c_PaxTy) ;
	m_rosterGroup.SetItemText(ndx,2,_data.c_openTime) ;
	m_rosterGroup.SetItemText(ndx,3,_data.c_closeTime ) ;
	m_rosterGroup.SetItemText(ndx,4,_data.c_relation) ;
	m_rosterGroup.SetItemText(ndx,5,_data.c_isDaily) ;
	m_rosterGroup.SetItemData(ndx,(DWORD_PTR)_PAss) ;
}
void CProcAssignDlg::DeleteSelLinRosterForGroupList() 
{
	int nCount = m_rosterGroup.GetItemCount();

	// Delete all of the items from the list view control.
	for (int i=0;i < nCount;i++)
	{
		ProcessorRoster*	p_Ass = (ProcessorRoster*)m_rosterGroup.GetItemData(0) ;
		m_rosterGroup.DeleteItem(0);
	}

}
void CProcAssignDlg::EditSelRosterForGroupList(ItemRosterList& _data) 
{
	POSITION posi = m_rosterGroup.GetFirstSelectedItemPosition() ;
	if(posi!= NULL)
	{
		int ndx = m_rosterGroup.GetNextSelectedItem(posi) ;
		m_rosterGroup.SetItemText(ndx,0,_data.c_Processor) ;
		m_rosterGroup.SetItemText(ndx,1,_data.c_PaxTy) ;
		m_rosterGroup.SetItemText(ndx,2,_data.c_openTime) ;
		m_rosterGroup.SetItemText(ndx,3,_data.c_closeTime ) ;
		m_rosterGroup.SetItemText(ndx,4,_data.c_relation) ;
		m_rosterGroup.SetItemText(ndx,5,_data.c_isDaily) ;
	}
}

void CProcAssignDlg::GetNodeProcessors(HTREEITEM hChildItem, std::vector<ProcessorID> &vProcID)
{
	// sort processor name in gt_ctrl
	std::vector<ProcessorID> vectProcID;
	if(!m_treeProc.ItemHasChildren(hChildItem))
	{
		ProcessorID _id;
		GetCurrentSelProcessID(hChildItem,_id) ;
		vProcID.push_back(_id);
		return ;
	}
	else
	{
		HTREEITEM pchild = m_treeProc.GetChildItem(hChildItem) ;
		while(pchild)
		{ 
			GetNodeProcessors(pchild, vProcID) ;
			pchild = m_treeProc.GetNextSiblingItem(pchild) ;
		}
	}
}


void CProcAssignDlg::OnButtonAutoAss()
{
	HTREEITEM hsellItem = m_treeProc.GetSelectedItem() ;
	if(hsellItem == NULL)
		return ;
	CString proname ;
	BOOL newInstance1 = FALSE ;
	BOOL newInstance2 = FALSE ;
	proname = m_treeProc.GetItemText(hsellItem) ;

	ProcessorID id;
    GetCurrentSelProcessID(id) ;

	//CRosterRulesDlg dlg(id,GetInputTerminal()) ;
	//dlg.DoModal();
	std::vector<ProcessorID> vSelectedProcID;
	GetNodeProcessors(hsellItem, vSelectedProcID);

	CDlgAutoRosterRules dlg(id, vSelectedProcID, GetInputTerminal()) ;
	dlg.DoModal();
	InitRosterForGroupListData()  ;

	m_RosterListDlg->InitRosterListData();
	m_GTDlg->InitGTCtrlData() ;
	m_treeProc.Invalidate(FALSE);

}

///////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CProcAssignDlg::ImportFileForRosterList() 
{
	CFileDialog filedlg( TRUE,"csv", NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_SHAREAWARE , \
		"CSV File (*.csv)|*.csv;*.CSV|All type (*.*)|*.*|", NULL, 0, FALSE );
	filedlg.m_ofn.lpstrTitle = "Import Roster"  ;
	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();
	if( !ReadTextFromFile( csFileName ) )
		return;
	InitRosterForGroupListData();
}
void CProcAssignDlg::ExportFileFromRosterList()
{
	CFileDialog filedlg( FALSE,"csv", NULL, OFN_CREATEPROMPT| OFN_SHAREAWARE| OFN_OVERWRITEPROMPT , \
		"CSV File (*.csv)|*.csv;*.CSV|All type (*.*)|*.*|", NULL, 0 , FALSE );
	filedlg.m_ofn.lpstrTitle = "Export Roster"  ;

	if(filedlg.DoModal()!=IDOK)
		return;
	CString csFileName=filedlg.GetPathName();
	if( WriteRosterToFile( csFileName ) )
		return;
}
BOOL CProcAssignDlg::WriteRosterToFile(const CString& filename)
{
	CStdioFile file(filename,CFile::modeCreate | CFile::modeWrite | CFile::typeText ) ;
	CStringArray strItemText;
	GetStringArrayFromRosterList(strItemText) ;
	for (int i = 0 ;i<strItemText.GetCount() ; i++)
		file.WriteString(strItemText.GetAt(i)) ;
	file.Close() ;
	return TRUE ;
}
void CProcAssignDlg::GetStringArrayFromRosterList(CStringArray& strArray)
{

	CString strlen ;
	CString processId ;
	CString paxTy ;
	CString OpenTime ;
	CString CloseTime ;
	CString relation ;
	CString isdaily ;
	for (int i = 0 ; i < m_rosterGroup.GetItemCount() ; i++)
	{
		processId.Format( m_rosterGroup.GetItemText(i,0) );
		paxTy.Format(m_rosterGroup.GetItemText(i,1)) ;
		OpenTime.Format(m_rosterGroup.GetItemText(i,2)) ;
		CloseTime.Format(m_rosterGroup.GetItemText(i,3)) ;
		relation.Format(m_rosterGroup.GetItemText(i,4)) ;
		isdaily.Format(m_rosterGroup.GetItemText(i,5)) ;
		strlen.Format(_T("%s,%s,%s,%s,%s,%s"),processId,paxTy,OpenTime,CloseTime,relation,isdaily) ;
		strlen.Append(_T("\n"));
		strArray.Add(strlen);
	}
}
bool CProcAssignDlg::ReadTextFromFile(const CString &_szFileName)
{
	CStdioFile file;
	if( !file.Open( _szFileName, CFile::modeRead) )
	{
		CString strMsg;
		strMsg.Format("Can not open the file!\r\n%s", _szFileName );
		MessageBox( strMsg, "ERROR", MB_OK|MB_ICONINFORMATION );
		return false;
	}


	CString strLine;
	std::vector<CStringArray*> AllLine ;
	CStringArray* strItemText = NULL;
	while( file.ReadString(strLine) )
	{
		if( strLine.IsEmpty() )
			continue;
		strItemText = new CStringArray ;
		int iColumnNum = getColNumFromStr( strLine, ",", *strItemText );
		AllLine.push_back(strItemText) ;
	}

	//remove all 
	ProcessorID id ;
	id.SetStrDict(GetInputTerminal()->inStrDict) ;
	ProcessorRosterSchedule* pSchd = NULL ;
	for (int i =0 ; i <(int)AllLine.size() ;i++)
	{
		id.setID(AllLine[i]->GetAt(0)) ;
		int nIdx = GetInputTerminal()->procAssignDB->findEntry( id );
		if( nIdx != INT_MAX )
		{
			pSchd = GetInputTerminal()->procAssignDB->getDatabase( nIdx );
			pSchd->clear() ;
		}	  
	}

	for (int i =0 ; i <(int)AllLine.size() ;i++)
	{
		AddRosterFromStrList(*AllLine[i]) ;	
		delete AllLine[i] ;
	}

	file.Close() ;
	return true;
}
void CProcAssignDlg::AddRosterFromStrList(const CStringArray& strItem)
{
	InputTerminal* m_pInTerm = GetInputTerminal() ; 
	ProcessorID id ;
	id.SetStrDict(GetInputTerminal()->inStrDict) ;
	id.setID(strItem.GetAt(0)) ;

	ElapsedTime opentime ;
	ElapsedTime closetime ;
	opentime.SetTime(strItem.GetAt(2)) ;
	closetime.SetTime(strItem.GetAt(3)) ;

	CMobileElemConstraint mob(GetInputTerminal());
	//mob.SetInputTerminal( m_pInTerm );

	CString strAirLine = strItem.GetAt(1);
	strAirLine.Remove('(') ;
	strAirLine.Remove(')') ;
	strAirLine.MakeUpper(); strAirLine.TrimLeft(), strAirLine.TrimRight();
	if( !strAirLine.IsEmpty() && strAirLine.CompareNoCase("default") != 0 )
	{
		if( strAirLine.GetLength() >= AIRLINE_LEN )
		{
			CString  strMsg ;
			strMsg.Format("Airline: %s\r\n is too length( more than %d )",strAirLine, AIRLINE_LEN );
			MessageBox( strMsg, "ERROR", MB_OK | MB_ICONINFORMATION );
			return ;
		}
		mob.setAirline( strAirLine.Left( AIRLINE_LEN) );
	}
	CString CIsDaily = strItem.GetAt(5) ;
	//////////////////////////////////////////////////////////////////////////
	// create assign event
	ProcessorRoster* pProcAssn = new ProcessorRoster( GetInputTerminal() );
	pProcAssn->setFlag(1 );
	pProcAssn->setOpenTime( opentime );
	pProcAssn->setCloseTime( closetime );
	pProcAssn->setAbsOpenTime(opentime) ;
	pProcAssn->setAbsCloseTime(closetime);
	CMultiMobConstraint multiMob;
	multiMob.addConstraint( mob );
	pProcAssn->setAssignment(multiMob );
	if(CIsDaily == "FALSE")
		pProcAssn->IsDaily(FALSE) ;
	else
		pProcAssn->IsDaily(TRUE) ;
	pProcAssn->setAssRelation(GetRelationByName(strItem.GetAt(4))) ;
	int nIdx = m_pInTerm->procAssignDB->findEntry( id );
	if( nIdx == INT_MAX )
	{
		m_pInTerm->procAssignDB->addEntry( id );
		nIdx = m_pInTerm->procAssignDB->findEntry( id );
		assert( nIdx != INT_MAX  );
	}
	ProcessorRosterSchedule* pSchd = m_pInTerm->procAssignDB->getDatabase( nIdx );
	pSchd->addItem(pProcAssn) ;
}

int CProcAssignDlg::getColNumFromStr(CString strText, CString strSep, CStringArray &strArray)
{	
	strArray.RemoveAll();
	int nColNum=1;
	int nPos=strText.Find(strSep,0);
	while(nPos!=-1)
	{
		nColNum++;
		strArray.Add(strText.Left(nPos));
		strText=strText.Right(strText.GetLength()-nPos-1);
		nPos=strText.Find(strSep,0);
	}
	strArray.Add(strText);
	return nColNum;
}
void CProcAssignDlg::OnClickCloseProcessor()
{
	ProcessorID id ;
	if(!GetCurrentSelProcessID(id))
		return ;
	int   nIdx = GetInputTerminal()->procAssignDB->findEntry( id );
	if(nIdx == INT_MAX)
		GetInputTerminal()->procAssignDB->addEntry(id ) ;
	nIdx = GetInputTerminal()->procAssignDB->findEntry( id );
	ProcessorRosterSchedule* pSchd = GetInputTerminal()->procAssignDB->getDatabase( nIdx );
   if(m_check_close.GetCheck() == BST_CHECKED)
   {
		pSchd->RemoveAllProcAssignment();
		pSchd->ClearAllDailyData();
#ifdef DEBUG
		int _nCount_0 = pSchd->getCount();
#endif
		ProcessorRoster* pProcAssn = new ProcessorRoster( GetInputTerminal() );
		pProcAssn->setAbsCloseTime(ElapsedTime());
		pProcAssn->setAbsCloseTime(ElapsedTime());
		pProcAssn->setFlag(0) ;
		pProcAssn->IsDaily(FALSE);

		pSchd->addItem(pProcAssn) ;
		m_treeProc.SetItemColor( m_treeProc.GetSelectedItem(), RGB(53,151,53) );;
		m_treeProc.SetItemBold(  m_treeProc.GetSelectedItem(), true );
#ifdef DEBUG
		int _nCount_1 = pSchd->getCount();
		_nCount_1 = 0;
#endif
   }
   else
   {
	   int nIdx = GetInputTerminal()->procAssignDB->findEntry( id );
	   if(nIdx >=0)
			GetInputTerminal()->procAssignDB->deleteItem(nIdx);

      //pSchd->RemoveAllProcAssignment();
	  m_treeProc.SetItemColor( m_treeProc.GetSelectedItem(), RGB(0,0,0) );
	  m_treeProc.SetItemBold( m_treeProc.GetSelectedItem(), false );
   }
   m_rosterGroup.DeleteAllItems() ;
   m_RosterListDlg->InitRosterListData();
   m_GTDlg->InitGTCtrlData() ;
}
void CProcAssignDlg::InitCheckCloseProcessorState()
{
	ProcessorID id ;
	if(!GetCurrentSelProcessID(id))
		return ;
	int   nIdx = GetInputTerminal()->procAssignDB->findEntry( id );
	if(nIdx == INT_MAX)
	{
		m_check_close.SetCheck(BST_UNCHECKED) ;
		return ;
	}
	nIdx = GetInputTerminal()->procAssignDB->findEntry( id );
	ProcessorRosterSchedule* pSchd = GetInputTerminal()->procAssignDB->getDatabase( nIdx );
	for (int i = 0 ; i < pSchd->getCount() ;i++)
	{
		if(pSchd->getItem(i)->getFlag() == 0)
		{
			m_check_close.SetCheck(BST_CHECKED) ;
			return ;
		}
	}
	m_check_close.SetCheck(BST_UNCHECKED) ;
}
//BOOL CProcAssignDlg::OnToolTipText(UINT,NMHDR* pNMHDR,LRESULT* pResult)
//{
//	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);
//
//	// if there is a top level routing frame then let it handle the message
//	if (GetRoutingFrame() != NULL) return FALSE;
//
//	// to be thorough we will need to handle UNICODE versions of the message also !!
//	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
//	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
//	TCHAR szFullText[512];
//	CString strTipText;
//	UINT nID = pNMHDR->idFrom;
//
//	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
//		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
//	{
//		// idFrom is actually the HWND of the tool 
//		nID = ::GetDlgCtrlID((HWND)nID);
//	}
//
//	if (nID!= 0) // will be zero on a separator
//	{
//
//		AfxLoadString(nID, szFullText);
//		strTipText=szFullText;
//
//#ifndef _UNICODE
//		if (pNMHDR->code == TTN_NEEDTEXTA)
//		{
//			lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
//		}
//		else
//		{
//			_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
//		}
//#else
//		if (pNMHDR->code == TTN_NEEDTEXTA)
//		{
//			_wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
//		}
//		else
//		{
//			lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
//		}
//#endif
//
//		*pResult = 0;
//
//		// bring the tooltip window above other popup windows
//		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
//			SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
//
//		return TRUE;
//	}
//
//	return FALSE;
//}
