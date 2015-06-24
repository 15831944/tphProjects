// AutoRosterRules.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgAutoRosterRules.h"
#include ".\dlgautorosterrules.h"
#include "../Inputs/AutoRosterRule.h"
#include "DlgProcessorSelection.h"
#include "../Inputs/assign.h"
#include "../Engine/proclist.h"
#include "PassengerTypeDialog.h"
#include "../Inputs/MultiMobConstraint.h"
#include "../Inputs/schedule.h"
#include "../Common/elaptime.h"
#include "../Inputs/procdb.h"
#include "../Inputs/assigndb.h"
#include "DlgRosterGranttChart.h"
// CAutoRosterRules dialog

#define AFX_IDW_TOOLBAR1 10
#define AFX_IDW_TOOLBAR2 11

IMPLEMENT_DYNAMIC(CDlgAutoRosterRules, CXTResizeDialog)
CDlgAutoRosterRules::CDlgAutoRosterRules(const ProcessorID& selProcID,std::vector<ProcessorID> vSelectedProcID, InputTerminal* pInTerm, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgAutoRosterRules::IDD, pParent)
	, m_pInTerm(pInTerm)
	, m_selProcID(selProcID)
	, m_pCurRosterRule(NULL)
{
	m_pProcRosterRules =  new CProcRosterRules(pInTerm);;
	m_pProcRosterRules->ReadDataFromDB(m_selProcID);

	m_vProcList = vSelectedProcID;
	//initProcessorName(selProcID);
}

CDlgAutoRosterRules::~CDlgAutoRosterRules()
{
}

void CDlgAutoRosterRules::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PAXTYPE, m_listPaxType);
	DDX_Control(pDX, IDC_CHECK_BYSCHEDTIME, m_btnRelativeToSched);
	DDX_Control(pDX, IDC_CHECK_DAILYTIME, m_btnDaily);
	//DDX_Control(pDX, IDC_DATETIME_OPENBYSCHED, m_ctlOpenTimeBySched);
	//DDX_Control(pDX, IDC_DATETIME_CLOSEBYSCHED, m_ctlCloseTimeBySched);
	//DDX_Control(pDX, IDC_DATETIME_OPENBYABSOLUTE, m_ctlOpenTimeByAbsolute);
	//DDX_Control(pDX, IDC_DATETIME_CLOSEBYABSOLUTE, m_ctlCloseTimeByAbsolute);
	DDX_Control(pDX, IDC_COMBO_OPENTYPE, m_cmbOpenType);
	DDX_Control(pDX, IDC_COMBO_CLOSETYPE, m_cmbCloseType);
	DDX_Control(pDX, IDC_COMBO_OPENDAY, m_cmbOpenDay);
	DDX_Control(pDX, IDC_COMBO_CLOSEDAY, m_cmbCloseDay);
	DDX_Control(pDX, IDC_CHECK_FOREACHFLIGHT, m_btnForEachFlight);
	DDX_Control(pDX, IDC_CHECK_NUMPROC, m_btnNumProc);
	DDX_Control(pDX, IDC_SPIN_NUMPROC, m_spinNumProc);
	DDX_Control(pDX, IDC_TREE_PRIORITY, m_ctlPriorityTree);
	DDX_Control(pDX, IDC_STATIC_PAXTOOLBAR, m_StaticPaxType);
	DDX_Control(pDX, IDC_STATIC_PROTOOLBAR, m_StaticPriority);
	DDX_Control(pDX, IDC_DATETIME_OPENBYSCHED, m_ctlOpenTime);
	DDX_Control(pDX, IDC_DATETIME_CLOSEBYSCHED, m_ctlCloseTime);
}


BEGIN_MESSAGE_MAP(CDlgAutoRosterRules, CXTResizeDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD,OnRulesNew) 
	ON_COMMAND(ID_TOOLBARBUTTONDEL,OnRulesDelete)
	ON_COMMAND(ID_TOOLBARBUTTONEDIT,OnRulesEdit)

	ON_COMMAND(ID_BUT_PIPEPRO_NEW,OnPriorityNew) 
	ON_COMMAND(ID_BUT_PIPEPRO_DEL,OnPriorityDelete)
	ON_LBN_SELCHANGE(IDC_LIST_PAXTYPE, OnLbnSelchangeListPaxType)
	ON_BN_CLICKED(IDC_CHECK_BYSCHEDTIME, OnBnClickedCheckByschedtime)
	ON_BN_CLICKED(IDC_CHECK_DAILYTIME, OnBnClickedCheckDailytime)
	ON_BN_CLICKED(IDC_CHECK_NUMPROC, OnBnClickedCheckNumproc)
	ON_NOTIFY(NM_SETFOCUS, IDC_TREE_PRIORITY, OnNMSetfocusTreePriority)
	ON_EN_KILLFOCUS(IDC_EDIT_NUMPROC, OnEnKillfocusEditNumproc)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PRIORITY, OnTvnSelchangedTreePriority)
	ON_BN_CLICKED(IDC_CHECK_FOREACHFLIGHT, OnBnClickedCheckForeachflight)
	ON_BN_CLICKED(IDC_BUTTON_SHOWGRANTTCHART, OnBnClickedShowGranttChart)

	ON_WM_SIZE()
END_MESSAGE_MAP()


// CAutoRosterRules message handlers
BOOL CDlgAutoRosterRules::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	
	CString strTitle;
	strTitle.Format("Rules for: %s", m_selProcID.GetIDString());
	SetWindowText(strTitle);

	InitToolBar();
	m_spinNumProc.SetRange(1, m_vProcList.size());

	SetResize(IDC_STATIC_PAXTOOLBAR, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(AFX_IDW_TOOLBAR1,SZ_TOP_LEFT,SZ_TOP_LEFT);

	SetResize(IDC_STATIC_PAXTYPE, SZ_TOP_LEFT,SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_PAXTYPE, SZ_TOP_LEFT,SZ_BOTTOM_CENTER) ;

	SetResize(IDC_CHECK_BYSCHEDTIME,SZ_TOP_CENTER,SZ_TOP_CENTER) ;
	SetResize(IDC_CHECK_DAILYTIME,SZ_TOP_CENTER,SZ_TOP_CENTER) ;

	SetResize(IDC_STATIC_OPEN,SZ_TOP_CENTER,SZ_TOP_CENTER) ;
	SetResize(IDC_STATIC_CLOSE,SZ_TOP_CENTER,SZ_TOP_CENTER) ;

	SetResize(IDC_DATETIME_OPENBYSCHED,SZ_TOP_CENTER,SZ_TOP_CENTER) ;
	SetResize(IDC_DATETIME_CLOSEBYSCHED,SZ_TOP_CENTER,SZ_TOP_CENTER) ;

	SetResize(IDC_COMBO_OPENDAY,SZ_TOP_CENTER,SZ_TOP_CENTER) ;
	SetResize(IDC_COMBO_CLOSEDAY,SZ_TOP_CENTER,SZ_TOP_CENTER) ;

	SetResize(IDC_COMBO_OPENTYPE,SZ_TOP_CENTER,SZ_TOP_CENTER) ;
	SetResize(IDC_COMBO_CLOSETYPE,SZ_TOP_CENTER,SZ_TOP_CENTER) ;

	SetResize(IDC_STATIC_TIME,SZ_TOP_CENTER,SZ_TOP_RIGHT) ;
	SetResize(IDC_STATIC_PRIORITY,SZ_TOP_CENTER,SZ_BOTTOM_RIGHT) ;

	SetResize(IDC_CHECK_FOREACHFLIGHT,SZ_TOP_CENTER,SZ_TOP_CENTER) ;
	SetResize(IDC_CHECK_NUMPROC,SZ_TOP_CENTER,SZ_TOP_CENTER) ;

	SetResize(IDC_EDIT_NUMPROC,SZ_TOP_CENTER,SZ_TOP_CENTER) ;
	SetResize(IDC_SPIN_NUMPROC,SZ_TOP_CENTER,SZ_TOP_CENTER) ;

	SetResize(IDC_STATIC_PROTOOLBAR,SZ_TOP_CENTER,SZ_TOP_CENTER) ;
	SetResize(AFX_IDW_TOOLBAR2,SZ_TOP_CENTER,SZ_TOP_CENTER) ;

	SetResize(IDC_TREE_PRIORITY,SZ_TOP_CENTER,SZ_BOTTOM_RIGHT);

	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDC_BUTTON_SHOWGRANTTCHART,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;




	m_ctlCloseTime.SetFormat("HH:mm:ss");
	m_ctlOpenTime.SetFormat("HH:mm:ss");

	m_ctlOpenTime.GetWindowRect( &m_rcOpenTime);
	ScreenToClient(&m_rcOpenTime);

	m_ctlCloseTime.GetWindowRect( &m_rcCloseTime);
	ScreenToClient(&m_rcCloseTime);

	SetListContent();
	//m_listPaxType.SetSel(0); 

	DisableSetting();


	//GetDlgItem(IDC_BUTTON_SHOWGRANTTCHART)->ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAutoRosterRules::initProcessorName( const ProcessorID& groupID )
{
	GroupIndex index = m_pInTerm->GetProcessorList()->getGroupIndex(groupID) ;
	TCHAR name[1024] = {0};
	CString Cname;
	ProcessorID procID ;
	procID.SetStrDict(m_pInTerm->inStrDict);
	if(index.start < 0 || index.end < 0)
	{
		m_vProcList.push_back(groupID);
		return ;
	}
	for (int i = index.start ; i <= index.end ;i++)
	{
		m_pInTerm->GetProcessorList()->getNameAt(i,name) ;
		Cname.Format(name);
		Cname.Replace(',','-');
		procID.setID(Cname) ;
		m_vProcList.push_back(procID) ;
	}
}

int CDlgAutoRosterRules::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	//mobile element toolbar 
	if (!m_PaxTypeToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,CRect(0,0,0,0), AFX_IDW_TOOLBAR1)||
		!m_PaxTypeToolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	//priority toolbar
	if (!m_PriorityToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,CRect(0,0,0,0), AFX_IDW_TOOLBAR2) ||
		!m_PriorityToolBar.LoadToolBar(IDR_TOOLBAR_PIPE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	
	return 0;
}

void CDlgAutoRosterRules::InitToolBar()
{
	m_StaticPaxType.ShowWindow(SW_HIDE) ;
	m_StaticPriority.ShowWindow(SW_HIDE);

	CRect rec ;
	m_StaticPaxType.GetWindowRect(rec) ;
	ScreenToClient(&rec) ;
	m_PaxTypeToolBar.MoveWindow(&rec) ;
	m_PaxTypeToolBar.ShowWindow(SW_SHOW) ;
	m_PaxTypeToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD, TRUE) ;
	m_PaxTypeToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,TRUE) ;
	m_PaxTypeToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,TRUE) ;

	CRect rc;
	m_StaticPriority.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_PriorityToolBar.MoveWindow(&rc);
	m_PriorityToolBar.ShowWindow(SW_SHOW);
	m_PriorityToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_NEW, FALSE );
	m_PriorityToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_DEL, FALSE );
}

void CDlgAutoRosterRules::OnRulesNew()
{
	CPassengerTypeDialog dlg(this);
	if( dlg.DoModal() == IDCANCEL)
		return;

	CMultiMobConstraint paxType;
	paxType.addConstraint(dlg.GetMobileSelection());
	CString szTmp;
	paxType.getConstraint(0)->screenPrint(szTmp, 0, 256) ;

	CAutoRosterRule* pCurRosterRule = new CRosterRulesByAbsoluteTime(m_pInTerm);
	pCurRosterRule->SetProcessorID(m_selProcID);
	pCurRosterRule->SetPaxType(paxType);

	int nIdx = m_listPaxType.AddString( szTmp );
	m_listPaxType.SetItemData(nIdx, (DWORD_PTR)pCurRosterRule);
	m_listPaxType.SetCurSel(nIdx);
	OnLbnSelchangeListPaxType();
}

void CDlgAutoRosterRules::OnRulesEdit()
{
	if (m_listPaxType.GetCount() ==0)
		return;

	CPassengerTypeDialog dlg(this);
	if( dlg.DoModal() == IDCANCEL)
		return;

	CMultiMobConstraint paxType;
	paxType.addConstraint(dlg.GetMobileSelection());
	CString szTmp;
	paxType.getConstraint(0)->screenPrint(szTmp, 0, 256) ;

	int nSel = m_listPaxType.GetCurSel();
	m_pCurRosterRule = (CRosterRulesByAbsoluteTime*)m_listPaxType.GetItemData(nSel);
	m_pCurRosterRule->SetPaxType(paxType);
	m_listPaxType.DeleteString(nSel);

	m_listPaxType.InsertString(nSel, szTmp );
	m_listPaxType.SetItemData(nSel, (DWORD_PTR)m_pCurRosterRule);
	m_listPaxType.SetSel(nSel);
	
}

void CDlgAutoRosterRules::OnRulesDelete()
{
	if (m_listPaxType.GetCount() ==0)
		return;

	int nSel = m_listPaxType.GetCurSel();
	m_listPaxType.DeleteString(nSel);
	m_ctlPriorityTree.DeleteAllItems();

	int nCount = m_listPaxType.GetCount();
	if (nCount ==0)
	{
		m_pCurRosterRule = NULL;

		DisableSetting();

		m_PriorityToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_NEW, FALSE );
		m_PriorityToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_DEL, FALSE );

	}
	else
	{
		m_listPaxType.SetCurSel(0);
		OnLbnSelchangeListPaxType();
	}
}

void CDlgAutoRosterRules::SetListContent()
{
	m_listPaxType.ResetContent();

	int nCount = m_pProcRosterRules->GetDataCount();
	for (int i =0; i < nCount; i++)
	{
		CAutoRosterRule* pCurRosterRule = m_pProcRosterRules->GetData(i);
		CMultiMobConstraint paxType = pCurRosterRule->GetPaxType();

		CString szTmp;
		paxType.getConstraint(0)->screenPrint(szTmp, 0, 256) ;
		int nIdx = m_listPaxType.AddString(szTmp);
		m_listPaxType.SetItemData(nIdx, (DWORD_PTR)pCurRosterRule);
	}
}

void CDlgAutoRosterRules::OnLbnSelchangeListPaxType()
{
	if (m_pCurRosterRule)
	{
		SaveTimeSettingToCurrentRule();
	}

	int nSel = m_listPaxType.GetCurSel();
	m_pCurRosterRule = (CAutoRosterRule*)m_listPaxType.GetItemData(nSel);

	if (m_pCurRosterRule != NULL)
	{
		m_btnRelativeToSched.EnableWindow(TRUE);
		m_btnDaily.EnableWindow(TRUE);
		m_ctlCloseTime.EnableWindow(TRUE);
		m_ctlOpenTime.EnableWindow(TRUE);
		m_cmbOpenDay.EnableWindow(TRUE);
		m_cmbCloseDay.EnableWindow(TRUE);
		m_btnForEachFlight.EnableWindow(TRUE);
		m_btnNumProc.EnableWindow(TRUE);
	}

	if (m_pCurRosterRule->IsForEachFlight())
		m_btnForEachFlight.SetCheck(BST_CHECKED);
	else
		m_btnForEachFlight.SetCheck(BST_UNCHECKED);

	InitRuleTimeSetting();

	if (m_pCurRosterRule->IsDefNumProc())
	{
		m_btnNumProc.SetCheck(BST_CHECKED);
		GetDlgItem(IDC_EDIT_NUMPROC)->ShowWindow(SW_SHOW);
		
		CString strCon ="";
		strCon.Format("%d", m_pCurRosterRule->GetNumOfProc());
		GetDlgItem(IDC_EDIT_NUMPROC)->SetWindowText(strCon);
		m_spinNumProc.ShowWindow(SW_SHOW);

		m_PaxTypeToolBar.EnableWindow(TRUE);
		m_ctlPriorityTree.EnableWindow(TRUE);
		InitPriorityTree();
	}
	else			//not define num of proc
	{
		m_btnNumProc.SetCheck(BST_UNCHECKED);
		GetDlgItem(IDC_EDIT_NUMPROC)->ShowWindow(SW_HIDE);
		m_spinNumProc.ShowWindow(SW_HIDE);

		m_ctlPriorityTree.DeleteAllItems();
		m_ctlPriorityTree.EnableWindow(FALSE);

		m_PriorityToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_NEW, FALSE );
		m_PriorityToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_DEL, FALSE );
	}
}

void CDlgAutoRosterRules::InitRuleTimeSetting()
{
	if (m_pCurRosterRule->IsBySchedule())		//by schedule time
	{
		m_btnRelativeToSched.SetCheck(BST_CHECKED);
		CRosterRulesBySchedTime* pRule = (CRosterRulesBySchedTime*)m_pCurRosterRule;

		m_ctlOpenTime.MoveWindow(&m_rcOpenTime);
		m_ctlCloseTime.MoveWindow(&m_rcCloseTime);

		m_ctlOpenTime.ShowWindow(SW_SHOW);
		m_ctlCloseTime.ShowWindow(SW_SHOW);
		m_cmbOpenType.ShowWindow(SW_SHOW);
		m_cmbCloseType.ShowWindow(SW_SHOW);

		m_cmbOpenDay.ShowWindow(SW_HIDE);
		m_cmbCloseDay.ShowWindow(SW_HIDE);
		m_btnDaily.ShowWindow(SW_HIDE);

		m_cmbOpenType.EnableWindow(TRUE);
		m_cmbCloseType.EnableWindow(TRUE);

		COleDateTime tTime;
		tTime.SetTime(pRule->GetOpenTime().GetHour(), pRule->GetOpenTime().GetMinute(), pRule->GetOpenTime().GetSecond());
		m_ctlOpenTime.SetTime(tTime);

		tTime.SetTime(pRule->GetCloseTime().GetHour(), pRule->GetCloseTime().GetMinute(), pRule->GetCloseTime().GetSecond());
		m_ctlCloseTime.SetTime(tTime);

		m_cmbOpenType.ResetContent();
		m_cmbCloseType.ResetContent();

		int nIdx = m_cmbOpenType.AddString("Before");
		m_cmbOpenType.SetItemData(nIdx, 0);
		nIdx = m_cmbOpenType.AddString("After");
		m_cmbOpenType.SetItemData(nIdx, 1);

		nIdx = m_cmbCloseType.AddString("Before");
		m_cmbCloseType.SetItemData(nIdx, 0);
		nIdx = m_cmbCloseType.AddString("After");
		m_cmbCloseType.SetItemData(nIdx, 1);

		m_cmbOpenType.SetCurSel((int)pRule->GetOpenIntervalTimeType());
		m_cmbCloseType.SetCurSel((int)pRule->GetCloseIntervalTimeType());

	}
	else		//by absolute time
	{
		m_btnRelativeToSched.SetCheck(BST_UNCHECKED);
		CRosterRulesByAbsoluteTime* pRule = (CRosterRulesByAbsoluteTime*)m_pCurRosterRule;

		m_ctlOpenTime.ShowWindow(SW_SHOW);
		m_ctlCloseTime.ShowWindow(SW_SHOW);
		m_cmbOpenType.ShowWindow(SW_HIDE);
		m_cmbCloseType.ShowWindow(SW_HIDE);

		m_btnDaily.ShowWindow(SW_SHOW);

		COleDateTime tTime;
		tTime.SetTime(pRule->GetOpenTime().GetHour(), pRule->GetOpenTime().GetMinute(), pRule->GetOpenTime().GetSecond());
		m_ctlOpenTime.SetTime(tTime);

		tTime.SetTime(pRule->GetCloseTime().GetHour(), pRule->GetCloseTime().GetMinute(), pRule->GetCloseTime().GetSecond());
		m_ctlCloseTime.SetTime(tTime);

		InitDailySetting(pRule);

	}
}

void CDlgAutoRosterRules::InitDailySetting(CRosterRulesByAbsoluteTime* pRule)
{
	if (pRule->IsDaily())
	{
		m_cmbOpenDay.ShowWindow(SW_HIDE);
		m_cmbCloseDay.ShowWindow(SW_HIDE);

		m_ctlOpenTime.MoveWindow(&m_rcOpenTime);
		m_ctlCloseTime.MoveWindow(&m_rcCloseTime);

		m_btnDaily.SetCheck(BST_CHECKED);
	}
	else
	{
		CRect rc1, rc2;
		m_cmbOpenType.GetWindowRect(rc1);
		m_cmbCloseType.GetWindowRect(rc2);

		ScreenToClient(&rc1) ;
		m_ctlOpenTime.MoveWindow(&rc1);
		ScreenToClient(&rc2) ;
		m_ctlCloseTime.MoveWindow(&rc2);

		m_cmbOpenDay.ShowWindow(SW_SHOW);
		m_cmbCloseDay.ShowWindow(SW_SHOW);
		m_btnDaily.SetCheck(BST_UNCHECKED);

		int nMaxDay = 1 ;
		FlightSchedule* pFlightSched = m_pInTerm->flightSchedule;
		if (pFlightSched->getCount() > 0)
			nMaxDay = pFlightSched->GetFlightScheduleEndTime().GetDay();

		m_cmbOpenDay.ResetContent() ;
		m_cmbCloseDay.ResetContent()  ;
		for (int i = 0 ;i< nMaxDay ;i++)
		{	
			CString strText;
			int ndx = 0 ;
			strText.Format("Day %d",i+1);
			ndx = m_cmbOpenDay.AddString(strText);
			m_cmbOpenDay.SetItemData(ndx,i+1) ;
			ndx = m_cmbCloseDay.AddString(strText);
			m_cmbCloseDay.SetItemData(ndx,i+1) ;
		}
		m_cmbOpenDay.SetCurSel(pRule->GetOpenTime().GetDay() -1);	
		m_cmbCloseDay.SetCurSel(pRule->GetCloseTime().GetDay() -1);
	}

}

void CDlgAutoRosterRules::OnBnClickedCheckByschedtime()
{
	int nSel = m_listPaxType.GetCurSel();

	int nState = m_btnRelativeToSched.GetCheck();
	if (nState == BST_CHECKED && !m_pCurRosterRule->IsBySchedule())
	{
		CAutoRosterRule* pNewRosterRule = new CRosterRulesBySchedTime(m_pInTerm);
		pNewRosterRule->CopyData(m_pCurRosterRule) ;
		pNewRosterRule->SetBySchedule(true);

		if (m_pProcRosterRules->FindData(m_pCurRosterRule))
			m_pProcRosterRules->DelData(m_pCurRosterRule);
		else
			delete m_pCurRosterRule;
		m_pCurRosterRule = pNewRosterRule;
		m_pProcRosterRules->AddData(m_pCurRosterRule);

		InitRuleTimeSetting();
	}
	
	if (nState == BST_UNCHECKED && m_pCurRosterRule->IsBySchedule())
	{

		CAutoRosterRule* pNewRosterRule = new CRosterRulesByAbsoluteTime(m_pInTerm);
		pNewRosterRule->CopyData(m_pCurRosterRule) ;
		pNewRosterRule->SetBySchedule(false);

		if (m_pProcRosterRules->FindData(m_pCurRosterRule))
			m_pProcRosterRules->DelData(m_pCurRosterRule);
		else
			delete m_pCurRosterRule;
		m_pCurRosterRule = pNewRosterRule;
		m_pProcRosterRules->AddData(m_pCurRosterRule);

		InitRuleTimeSetting();
	}
	m_listPaxType.SetItemData(nSel, (DWORD_PTR)m_pCurRosterRule);
}

void CDlgAutoRosterRules::OnBnClickedCheckDailytime()
{
	int nState = m_btnDaily.GetCheck() ;
	CRosterRulesByAbsoluteTime* pRule = (CRosterRulesByAbsoluteTime*)m_pCurRosterRule;
	pRule->SetDaily(nState == BST_CHECKED? true:false);

	InitDailySetting(pRule);
}

void CDlgAutoRosterRules::OnBnClickedCheckNumproc()
{
	int nState = m_btnNumProc.GetCheck();
	m_pCurRosterRule->SetDefNumProc(nState == BST_CHECKED? true:false);

	if (m_pCurRosterRule->IsDefNumProc())
	{
		m_btnNumProc.SetCheck(BST_CHECKED);
		GetDlgItem(IDC_EDIT_NUMPROC)->ShowWindow(SW_SHOW);

		CString strCon ="";
		strCon.Format("%d", (int)m_vProcList.size());
		GetDlgItem(IDC_EDIT_NUMPROC)->SetWindowText(strCon);
		m_spinNumProc.ShowWindow(SW_SHOW);

		m_PaxTypeToolBar.EnableWindow(TRUE);
		m_ctlPriorityTree.EnableWindow(TRUE);
		InitPriorityTree();
	}
	else			//not define num of proc
	{
		m_btnNumProc.SetCheck(BST_UNCHECKED);
		GetDlgItem(IDC_EDIT_NUMPROC)->ShowWindow(SW_HIDE);
		m_spinNumProc.ShowWindow(SW_HIDE);

		m_ctlPriorityTree.DeleteAllItems();
		m_PaxTypeToolBar.EnableWindow(FALSE);
		m_ctlPriorityTree.EnableWindow(FALSE);
		m_PriorityToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_NEW, FALSE );
	}
}

void CDlgAutoRosterRules::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	SaveRosterRules();
	GenerateProcRoster();
	OnOK();
}


void CDlgAutoRosterRules::InitPriorityTree()
{
	ASSERT(m_pCurRosterRule);
	m_PriorityToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_NEW, TRUE );

	m_ctlPriorityTree.DeleteAllItems();
	
	std::vector<CPriorityRule*> vPriorities = m_pCurRosterRule->GetPriorityRules();

	size_t nSize = vPriorities.size();
	for (size_t i =0; i < nSize; i++)
	{
		CPriorityRule* pPriority = vPriorities.at(i);
		pPriority->SetPriorityID(i +1);
		SetPriorityItem(pPriority);
	}
	
}

void CDlgAutoRosterRules::SetPriorityItem( CPriorityRule* pPriority )
{

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_NORMAL;
	cni.nt=NT_NORMAL;
	cni.bAutoSetItemText = FALSE;

	CString strContent;
	strContent.Format("Priority %d:", pPriority->GetPriorityID());


	HTREEITEM hPriority = m_ctlPriorityTree.InsertItem(strContent, cni, FALSE);
	m_ctlPriorityTree.SetItemData(hPriority, (DWORD_PTR)pPriority);

	HTREEITEM hProc = m_ctlPriorityTree.InsertItem("Processors", cni, FALSE, FALSE, hPriority);
	std::vector<CString> vProcs = pPriority->GetProcList();

	size_t nProc = vProcs.size();
	if (nProc == 0)
	{
		CString strProc = m_vProcList.at(0).GetIDString();
		m_ctlPriorityTree.InsertItem(strProc, cni, FALSE, FALSE, hProc);
		pPriority->AddProcID(strProc);
	}
	else
	{
		for (size_t j = 0; j < nProc; j++)
		{
			m_ctlPriorityTree.InsertItem(vProcs.at(j), cni, FALSE, FALSE, hProc);
		}
	}

	m_ctlPriorityTree.Expand(hProc,TVE_EXPAND);

	cni.nt = NT_NORMAL;
	cni.net = NET_COMBOBOX;
	if (pPriority->IsAdjacent())
		strContent = "Adjacent: Yes";
	else
		strContent = "Adjacent: No";
	m_ctlPriorityTree.InsertItem(strContent, cni, FALSE, FALSE, hPriority);

	cni.net = NET_EDITSPIN_WITH_VALUE;
	strContent.Format("Minimum: %d", pPriority->GetMinNum());
	m_ctlPriorityTree.InsertItem(strContent, cni, FALSE, FALSE, hPriority);

	m_ctlPriorityTree.Expand(hPriority,TVE_EXPAND);
	m_ctlPriorityTree.SetRedraw(TRUE);
}

void CDlgAutoRosterRules::OnPriorityNew()
{
	HTREEITEM hSel = m_ctlPriorityTree.GetSelectedItem();
	if (hSel == NULL)
	{
		NewPriority();
		return;
	}

	HTREEITEM hParItem = m_ctlPriorityTree.GetParentItem(hSel);
	if (hParItem == NULL)		
		NewPriority();
	else
	{
		//HTREEITEM hGrandItem = m_ctlPriorityTree.GetParentItem(hParItem);
		CPriorityRule* pPriority = (CPriorityRule*)m_ctlPriorityTree.GetItemData(hParItem);
		NewProcessor(pPriority);
	}
}

void CDlgAutoRosterRules::OnPriorityDelete()
{
	HTREEITEM hSel = m_ctlPriorityTree.GetSelectedItem();
	HTREEITEM hParItem = m_ctlPriorityTree.GetParentItem(hSel);
	if (hParItem == NULL)		
	{
		CPriorityRule* pPriority = (CPriorityRule*)m_ctlPriorityTree.GetItemData(hSel);
		m_pCurRosterRule->RemovePriorityRule(pPriority);
		InitPriorityTree();

	}
	else
	{
		HTREEITEM hGrandItem = m_ctlPriorityTree.GetParentItem(hParItem);
		CPriorityRule* pPriority = (CPriorityRule*)m_ctlPriorityTree.GetItemData(hGrandItem);

		CString strProc = m_ctlPriorityTree.GetItemText(hSel);
		pPriority->DelProcID(strProc);
		m_ctlPriorityTree.DeleteItem(hSel);
	}
}

void CDlgAutoRosterRules::OnTvnSelchangedTreePriority(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hSel = m_ctlPriorityTree.GetSelectedItem();
	if (hSel == NULL)
	{
		m_PriorityToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_NEW, TRUE );
		m_PriorityToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_DEL, FALSE );

		return;
	}

	HTREEITEM hParItem = m_ctlPriorityTree.GetParentItem(hSel);
	if (hParItem == NULL)		
	{
		m_PriorityToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_NEW, TRUE );
		m_PriorityToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_DEL, TRUE );
	}
	else
	{
		HTREEITEM hGrandItem = m_ctlPriorityTree.GetParentItem(hParItem);
		if (hGrandItem == NULL)
		{
			m_PriorityToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_NEW, TRUE );
			m_PriorityToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_DEL, FALSE );
		}
		else
		{
			m_PriorityToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_NEW, FALSE );
			m_PriorityToolBar.GetToolBarCtrl().EnableButton( ID_BUT_PIPEPRO_DEL, TRUE );
		}
	}
	*pResult = 0;
}


void CDlgAutoRosterRules::OnNMSetfocusTreePriority(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlgAutoRosterRules::NewPriority()
{
	CDlgProcessorSelection dlg(m_selProcID,m_pInTerm, this);
	if (dlg.DoModal() != IDOK)
		return;

	CPriorityRule* pPriority = new CPriorityRule(m_pInTerm);
	int nCount = m_pCurRosterRule->GetPriorityRules().size();
	pPriority->SetPriorityID(nCount +1);

	CString strProc = dlg.GetSelProcName(); 
	pPriority->AddProcID(strProc);

	SetPriorityItem(pPriority);
	m_pCurRosterRule->AddPriorityRule(pPriority);
}

void CDlgAutoRosterRules::NewProcessor(CPriorityRule* pPriority)
{
	CDlgProcessorSelection dlg(m_selProcID,m_pInTerm, this);
	if (dlg.DoModal() != IDOK)
		return;

	CString strProc = dlg.GetSelProcName(); 
	pPriority->AddProcID(strProc);

	HTREEITEM hSel = m_ctlPriorityTree.GetSelectedItem();

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_NORMAL;
	cni.nt=NT_NORMAL;
	cni.bAutoSetItemText = FALSE;

	m_ctlPriorityTree.InsertItem(strProc, cni, FALSE, FALSE, hSel);
	m_ctlPriorityTree.Expand(hSel,TVE_EXPAND);
}

void CDlgAutoRosterRules::SaveRosterRules()
{
	SaveTimeSettingToCurrentRule();
	m_pProcRosterRules->clear();

	int nCount = m_listPaxType.GetCount();
	for (int i =0; i < nCount; i++)
	{
		CAutoRosterRule* pRosterRule = (CAutoRosterRule*)m_listPaxType.GetItemData(i);

		m_pProcRosterRules->AddData(pRosterRule);
	}

	m_pProcRosterRules->SaveDataToDB();
}

LRESULT CDlgAutoRosterRules::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{	
		case UM_CEW_EDITSPIN_END:
			{
				//-----------------
				HTREEITEM hItem=(HTREEITEM)wParam;
				CString strValue = *((CString*)lParam);
				int nValue = (int)atoi( strValue.GetBuffer() );

				HTREEITEM hParentItem = m_ctlPriorityTree.GetParentItem(hItem);
				CPriorityRule* pPriority = (CPriorityRule*)m_ctlPriorityTree.GetItemData(hParentItem);
				pPriority->SetMinNum(nValue);

				strValue.Format("Minimum: %d", nValue);
				m_ctlPriorityTree.SetItemText(hItem, strValue);

			}
			break;
		case UM_CEW_COMBOBOX_BEGIN:
			{
				HTREEITEM hItem=(HTREEITEM)wParam;
				CWnd* pWnd=m_ctlPriorityTree.GetEditWnd(hItem);
				CRect rectWnd;
				m_ctlPriorityTree.GetItemRect(hItem,rectWnd,TRUE);
				pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
				CComboBox* pCB=(CComboBox*)pWnd;
				pCB->ResetContent();

				int nIdx = pCB->AddString("Yes");
				pCB->SetItemData(nIdx,0);
				nIdx = pCB->AddString("No");
				pCB->SetItemData(nIdx,1);

			}
			break;
		case UM_CEW_COMBOBOX_SELCHANGE:
			{
				CWnd* pWnd = m_ctlPriorityTree.GetEditWnd((HTREEITEM)wParam);
				CComboBox* pCB = (CComboBox*)pWnd;
				HTREEITEM hItem = (HTREEITEM)wParam;
				HTREEITEM hParentItem = m_ctlPriorityTree.GetParentItem(hItem);
				CPriorityRule* pPriority = (CPriorityRule*)m_ctlPriorityTree.GetItemData(hParentItem);

				int nSel = pCB->GetCurSel();	
				if(nSel == 0)
					pPriority->SetAdjacent(true);
				else
					pPriority->SetAdjacent(false);

				CString strContent;
				if (pPriority->IsAdjacent())
					strContent = "Adjacent: Yes";
				else
					strContent = "Adjacent: No";
				m_ctlPriorityTree.SetItemText(hItem, strContent);
			}
			break;
		default:
			break;
	}
	return CXTResizeDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgAutoRosterRules::OnEnKillfocusEditNumproc()
{
	// TODO: Add your control notification handler code here
	ASSERT(m_pCurRosterRule);

	CString strValue;
	GetDlgItemText(IDC_EDIT_NUMPROC, strValue);

	int nValue = (int)atoi(strValue.GetBuffer());
	m_pCurRosterRule->SetNumOfProc(nValue);

}

void CDlgAutoRosterRules::GenerateProcRoster()
{
	std::vector<ProcAssignEntry*> vProcAssignEntries ;
	GetProcAssignEntries(vProcAssignEntries) ;
	m_pProcRosterRules->DoAutoRosterAssignment(vProcAssignEntries);
}

void CDlgAutoRosterRules::GetProcAssignEntries( std::vector<ProcAssignEntry*>& vProAssignEntries )
{
	ProcAssignDatabase* pAssignDB = m_pInTerm->procAssignDB ;
	ProcAssignEntry* pEntry = NULL ;

	size_t nSize = m_vProcList.size();
	for (size_t i = 0 ; i < nSize ; i++)
	{
		ProcessorID procID = m_vProcList.at(i);
		int nIdx = pAssignDB->findEntry(procID) ;
		if(nIdx ==INT_MAX)
		{
			pAssignDB->addEntry(procID) ;
			nIdx  = pAssignDB->findEntry(procID) ;	
			pEntry = (ProcAssignEntry*)pAssignDB->getItem(nIdx) ;
		}
		else
		{
			pEntry = (ProcAssignEntry*)pAssignDB->getItem(nIdx) ;
			pEntry->getSchedule()->ClearAllAssignment() ;
		}
		vProAssignEntries.push_back(pEntry);
	}
}

void CDlgAutoRosterRules::OnBnClickedCheckForeachflight()
{
	int nState = m_btnForEachFlight.GetCheck() ;
	m_pCurRosterRule->SetForEachFlight(nState == BST_CHECKED? true:false);
}

void CDlgAutoRosterRules::DisableSetting()
{
	m_cmbOpenDay.ShowWindow(SW_HIDE);
	m_cmbCloseDay.ShowWindow(SW_HIDE);

	m_cmbOpenType.ShowWindow(SW_SHOW);
	m_cmbCloseType.ShowWindow(SW_SHOW);

	m_ctlCloseTime.MoveWindow(m_rcCloseTime);
	m_ctlOpenTime.MoveWindow(m_rcOpenTime);

	m_btnRelativeToSched.EnableWindow(FALSE);
	m_btnRelativeToSched.SetCheck(BST_UNCHECKED);

	m_btnDaily.EnableWindow(FALSE);
	m_btnDaily.SetCheck(BST_UNCHECKED);

	m_ctlCloseTime.EnableWindow(FALSE);
	m_ctlOpenTime.EnableWindow(FALSE);

	m_btnForEachFlight.EnableWindow(FALSE);
	m_btnForEachFlight.SetCheck(BST_UNCHECKED);

	m_btnNumProc.EnableWindow(FALSE);
	m_btnNumProc.EnableWindow(BST_UNCHECKED);

	m_cmbCloseType.EnableWindow(FALSE);
	m_cmbOpenType.EnableWindow(FALSE);

	m_ctlPriorityTree.EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_NUMPROC)->ShowWindow(SW_HIDE);
	m_spinNumProc.ShowWindow(SW_HIDE);
}

void CDlgAutoRosterRules::SaveTimeSettingToCurrentRule()
{
	if (m_pCurRosterRule == NULL)
		return;

	if (m_pCurRosterRule->IsBySchedule())
	{
		CRosterRulesBySchedTime* pRule = (CRosterRulesBySchedTime*)m_pCurRosterRule;
		pRule->SetOpenIntervalTimeType((CRosterRulesBySchedTime::TIMEINTERVALTYPE)m_cmbOpenType.GetCurSel());
		pRule->SetCloseIntervalTimeType((CRosterRulesBySchedTime::TIMEINTERVALTYPE)m_cmbCloseType.GetCurSel());

		COleDateTime timeTime;
		ElapsedTime tTime;

		m_ctlOpenTime.GetTime(timeTime);
		tTime.SetHour(timeTime.GetHour());
		tTime.SetMinute(timeTime.GetMinute());
		tTime.SetSecond(timeTime.GetSecond());
		pRule->SetOpenTime(tTime);

		m_ctlCloseTime.GetTime(timeTime);
		tTime.SetHour(timeTime.GetHour());
		tTime.SetMinute(timeTime.GetMinute());
		tTime.SetSecond(timeTime.GetSecond());
		pRule->SetCloseTime(tTime);

	}
	else
	{
		CRosterRulesByAbsoluteTime* pRule = (CRosterRulesByAbsoluteTime*)m_pCurRosterRule;

		ElapsedTime tOpenTime, tCloseTime;
		if (m_btnDaily.GetCheck() != BST_CHECKED)
		{
			int nSel = m_cmbOpenDay.GetCurSel();
			int nDay = m_cmbOpenDay.GetItemData(nSel);
			tOpenTime.SetDay(nDay);

			nSel = m_cmbCloseDay.GetCurSel();
			nDay = m_cmbCloseDay.GetItemData(nSel);
			tCloseTime.SetDay(nDay);			
		}

		COleDateTime timeTime;

		m_ctlOpenTime.GetTime(timeTime);
		tOpenTime.SetHour(timeTime.GetHour());
		tOpenTime.SetMinute(timeTime.GetMinute());
		tOpenTime.SetSecond(timeTime.GetSecond());
		pRule->SetOpenTime(tOpenTime);

		m_ctlCloseTime.GetTime(timeTime);
		tCloseTime.SetHour(timeTime.GetHour());
		tCloseTime.SetMinute(timeTime.GetMinute());
		tCloseTime.SetSecond(timeTime.GetSecond());
		pRule->SetCloseTime(tCloseTime);
	}

	CString strValue;
	GetDlgItemText(IDC_EDIT_NUMPROC, strValue);

	int nValue = (int)atoi(strValue.GetBuffer());
	m_pCurRosterRule->SetNumOfProc(nValue);
}
void CDlgAutoRosterRules::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize(nType, cx, cy);

	if (m_listPaxType.GetSafeHwnd())
	{
		//CRect rec ;
		//m_StaticPaxType.GetWindowRect(rec) ;
		//ScreenToClient(&rec) ;
		//m_PaxTypeToolBar.MoveWindow(&rec) ;

		//CRect rc;
		//m_StaticPriority.GetWindowRect( &rc );
		//ScreenToClient(&rc);
		//m_PriorityToolBar.MoveWindow(&rc);

		m_ctlOpenTime.GetWindowRect( &m_rcOpenTime);
		ScreenToClient(&m_rcOpenTime);

		m_ctlCloseTime.GetWindowRect( &m_rcCloseTime);
		ScreenToClient(&m_rcCloseTime);

		int nSel = m_listPaxType.GetCurSel();
		if (nSel >=0 && m_btnDaily.GetCheck() == BST_UNCHECKED && m_btnRelativeToSched.GetCheck() == BST_UNCHECKED)
		{
			CRect rc1, rc2;
			m_cmbOpenType.GetWindowRect(rc1);
			m_cmbCloseType.GetWindowRect(rc2);

			ScreenToClient(&rc1) ;
			m_ctlOpenTime.MoveWindow(&rc1);
			ScreenToClient(&rc2) ;
			m_ctlCloseTime.MoveWindow(&rc2);

			m_cmbOpenDay.ShowWindow(SW_SHOW);
			m_cmbCloseDay.ShowWindow(SW_SHOW);

			m_cmbOpenType.ShowWindow(SW_HIDE);
			m_cmbCloseType.ShowWindow(SW_HIDE);
		}
	}
}

void CDlgAutoRosterRules::OnBnClickedShowGranttChart()
{
	SaveTimeSettingToCurrentRule();
	CDlgRosterGranttChart dlg(m_pProcRosterRules,m_vProcList ,m_pInTerm,this);
	if(dlg.DoModal() == IDOK)
		OnOK();
}
