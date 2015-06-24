// RosterRulesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "RosterRulesDlg.h"
#include ".\rosterrulesdlg.h"


// CRosterRulesDlg dialog

IMPLEMENT_DYNAMIC(CRosterRulesDlg, CXTResizeDialog)
CRosterRulesDlg::CRosterRulesDlg(ProcessorID& _curSelProID,InputTerminal* P_inputter ,CWnd* pParent /*=NULL*/)
	:columNum(5),cout_proc(0),CXTResizeDialog(CRosterRulesDlg::IDD, pParent)
{

	
	m_RuleListColNames.push_back(_T("Passenger Type")) ;
	m_RuleListColNames.push_back(_T("Start Time")) ;
	m_RuleListColNames.push_back(_T("End Time")) ;
	m_RuleListColNames.push_back(_T("Assigment Relation")) ;
	m_RuleListColNames.push_back(_T("IsDaily")) ;
	m_RuleListColNames.push_back(_T("#Processor")) ;

	
	m_RuleScheduleListColNames.push_back(_T("Passenger Type")) ;
	m_RuleScheduleListColNames.push_back(_T("Open Interval Time")) ;
	m_RuleScheduleListColNames.push_back(_T("Close Interval Time")) ;
	m_RuleScheduleListColNames.push_back(_T("Assigment Relation")) ;
	m_RuleScheduleListColNames.push_back(_T("#Processor")) ;

	P_input = P_inputter ;
	initProcessorName(_curSelProID) ;
	current = 0 ;
	p_rulesDB = new CAutoRosterRulesDB(P_inputter) ;
	p_ruleSchedule = new CAutoRosterByScheduleRulesDB(P_inputter);
    p_rulesDB->ReadDataFromDB();
	p_ruleSchedule->ReadDataFromDB();
    p_GroupRules =  p_rulesDB->FindGroupRules(_curSelProID) ;
    p_RulesForSchedule = p_ruleSchedule->FindGroupRules(_curSelProID);
	if (p_GroupRules == NULL)
	{
		p_GroupRules = new CProcessGroupRules(_curSelProID);
		p_rulesDB->addItem(p_GroupRules) ;

	}
	if(p_RulesForSchedule == NULL)
	{
		p_RulesForSchedule = new CProcessGroupRules(_curSelProID);
		p_ruleSchedule->addItem(p_RulesForSchedule) ;

	}

		
}
void CRosterRulesDlg::initProcessorName(const ProcessorID& groupID)
{
	GroupIndex index = P_input->GetProcessorList()->getGroupIndex(groupID) ;
	TCHAR name[1024] = {0};
    CString Cname;
	ProcessorID pro_id ;
	pro_id.SetStrDict(P_input->inStrDict);
	if(index.start < 0 || index.end < 0)
		return ;
	for (int i = index.start ; i <= index.end ;i++)
	{
		P_input->GetProcessorList()->getNameAt(i,name) ;
		Cname.Format(name);
		Cname.Replace(',','-');
		pro_id.setID(Cname) ;
         proName_list.push_back(pro_id) ;
		cout_proc++ ;
	}
}
CRosterRulesDlg::~CRosterRulesDlg()
{
}

void CRosterRulesDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TOOLBAR, n_sta_new);
	DDX_Control(pDX, IDC_LIST_ROSTER_RULES, m_rulesList);
	DDX_Control(pDX, IDC_LIST_ROSTER_RULES_BYSCH, m_ruleList_Schedule);
	DDX_Control(pDX, IDC_CHECK_SCH, m_check_schedule);
	//DDX_Control(pDX, IDC_CHECK_CHECKALL, m_check_Sel);
	DDX_Control(pDX, IDC_STATIC_RULE, m_StaRules);
}


BEGIN_MESSAGE_MAP(CRosterRulesDlg, CXTResizeDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(ID_BUTTON_APPEND, OnBnClickedButtonAppend)
	ON_BN_CLICKED(ID_BUTTON_REPLACE, OnBnClickedButtonReplace)
	ON_BN_CLICKED(ID_BUTTON_CLOSE, OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_SAVERULE,ONBnClickButtonSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ROSTER_RULES, OnLvnItemchangedListRosterRules)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ROSTER_RULES_BYSCH, OnLvnItemchangedListRosterRules)
	ON_COMMAND(ID_TOOLBARBUTTONADD,OnRulesNew) 
	ON_COMMAND(ID_TOOLBARBUTTONDEL,OnRulesDelete)
	ON_COMMAND(ID_TOOLBARBUTTONEDIT,OnRulesEdit)
	ON_NOTIFY(HDN_ITEMCLICK, IDC_LIST_ROSTER_RULES, OnHdnItemclickListRosterRules)
	ON_NOTIFY(HDN_ITEMCLICK, IDC_LIST_ROSTER_RULES_BYSCH, OnHdnItemclickListRosterRules)
	//ON_BN_CLICKED(IDC_CHECK_CHECKALL, OnBnClickedCheckCheckall)
	ON_BN_CLICKED(IDC_CHECK_SCH, OnBnClickedCheckBySchedule)
	ON_WM_SIZE() 
END_MESSAGE_MAP()
void CRosterRulesDlg::OnSize(UINT nType, int cx, int cy) 
{
	CXTResizeDialog::OnSize(nType, cx, cy);
	//cx
	//	Specifies the new width of the client area.

	//cy
	//	Specifies the new height of the client area.
	if(m_rulesList.m_hWnd)
		m_rulesList.ResizeColumnWidth();
	if(m_ruleList_Schedule.m_hWnd)
		m_ruleList_Schedule.ResizeColumnWidth();
}
BOOL CRosterRulesDlg::OnInitDialog()
{
   CXTResizeDialog::OnInitDialog() ;
   CString DlgCaption;
   DlgCaption.Format( _T("Rules For: %s"),p_GroupRules->GetProcessorId()->GetIDString()) ;
   SetWindowText(DlgCaption) ;
   m_StaRules.SetWindowText(DlgCaption) ;
   InitToolBar() ;
   InitRulesList();
   InitRuleScheduleList() ;
   m_ruleList_Schedule.ShowWindow(SW_HIDE);
   m_rulesList.ShowWindow(SW_SHOW) ;
   GetDlgItem(ID_BUTTON_APPEND)->EnableWindow(FALSE) ;
   GetDlgItem(ID_BUTTON_REPLACE)->EnableWindow(FALSE) ;
   SetResize(IDC_STATIC_RULE,SZ_TOP_LEFT,SZ_TOP_LEFT) ;
   SetResize(IDC_STATIC_HX2,SZ_TOP_LEFT,SZ_TOP_RIGHT) ;

   SetResize(IDC_CHECK_SCH,SZ_TOP_LEFT,SZ_TOP_LEFT);
   SetResize(IDC_STATIC_HX,SZ_TOP_LEFT,SZ_TOP_RIGHT) ;
   SetResize(m_ToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT) ;
   SetResize(IDC_STATIC_TOOLBAR,SZ_TOP_LEFT,SZ_TOP_LEFT) ;
   SetResize(IDC_LIST_ROSTER_RULES,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
   SetResize(IDC_LIST_ROSTER_RULES_BYSCH,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;

	SetResize(ID_BUTTON_APPEND,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
    SetResize(ID_BUTTON_REPLACE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
    SetResize(ID_BUTTON_CLOSE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDC_BUTTON_SAVERULE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
   return TRUE ;
}
int CRosterRulesDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}
void CRosterRulesDlg::InitToolBar()
{
	n_sta_new.ShowWindow(SW_HIDE) ;
	CRect rec ;
	n_sta_new.GetWindowRect(rec) ;
	ScreenToClient(&rec) ;
	m_ToolBar.MoveWindow(&rec) ;
	m_ToolBar.ShowWindow(SW_SHOW) ;
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD) ;
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE) ;
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,FALSE) ;
}


void CRosterRulesDlg::OnBnClickedCheckBySchedule()
{
	if(m_check_schedule.GetCheck() == BST_CHECKED)
	{
		m_ruleList_Schedule.ShowWindow(SW_SHOW);
		m_rulesList.ShowWindow(SW_HIDE);
	}else
	{
		m_ruleList_Schedule.ShowWindow(SW_HIDE);
		m_rulesList.ShowWindow(SW_SHOW);
	}
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE)  ;
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,FALSE) ;
}
void CRosterRulesDlg::OnBnClickedButtonAppend()
{
	// TODO: Add your control notification handler code here
	if(m_check_schedule.GetCheck() == BST_CHECKED)
		ApplyRulesScheduleToRoster(RULE_APPEND) ;
	else
		ApplyRulesToRoster(RULE_APPEND) ;
	p_rulesDB->WriteDataToDB() ;
	p_ruleSchedule->WriteDataToDB();
	CDialog::OnOK() ;
}

void CRosterRulesDlg::OnBnClickedButtonReplace()
{
	// TODO: Add your control notification handler code here
	if(m_check_schedule.GetCheck() == BST_CHECKED)
		ApplyRulesScheduleToRoster(RULE_REPLACE) ;
	else
		ApplyRulesToRoster(RULE_REPLACE) ;
	p_rulesDB->WriteDataToDB() ;
	p_ruleSchedule->WriteDataToDB();
		CDialog::OnOK() ;
}
void CRosterRulesDlg::ApplyRulesScheduleToRoster(int oper)
{
	std::vector<ProcessorRosterSchedule*> schedules ;
	GetProAssignScheduleOfCurrentSelPro(schedules,oper) ;
	std::vector<CRosterRule*> selRules ;
	GetCurrentSelRules(selRules) ;
	CRosterRule* p_rule = NULL ;
	for (int i = 0 ; i < (int)selRules.size() ; i++)
	{
		p_rule = selRules[i] ;
		for (int j = 0 ,m = 0; j < (int)schedules.size() && m < p_rule->GetCount(); j++,m++)
			p_rule->AddRosterToSchedule(schedules[j]) ;
	}
}

void CRosterRulesDlg::ApplyRulesToRoster(int oper)
{
	std::vector<ProcessorRosterSchedule*> schedules ;
	GetProAssignScheduleOfCurrentSelPro(schedules,oper) ;
	std::vector<CRosterRule*> selRules ;
	GetCurrentSelRules(selRules) ;
	CRosterRule* p_rule = NULL ;
	for (int i = 0 ; i < (int)selRules.size() ; i++)
	{
		p_rule = selRules[i] ;
		for (int j = 0 , m = 0; j < (int)schedules.size() && m < p_rule->GetCount() ; j++,m++)
			p_rule->AddRosterToSchedule(schedules[j]) ;
	}
}
void CRosterRulesDlg::GetProAssignScheduleOfCurrentSelPro(std::vector<ProcessorRosterSchedule*>& _AssSchedule ,int oper)
{
	ProcAssignDatabase* basebase = P_input->procAssignDB ;
	ProcessorRosterSchedule* schedule = NULL ;
	ProcessorID id ;
	id = *p_RulesForSchedule->GetProcessorId() ;
	std::vector<ProcessorID> vectProcID;
	GroupIndex group = P_input->procList->getGroupIndex (id);
	if(group.start == -1 || group.end == -1)
         vectProcID.push_back( id );
	else
	{
		for( int i=group.start; i<=group.end; i++ )
		{
			id = *(P_input->procList->getProcessor(i)->getID());
			id.SetStrDict( P_input->inStrDict );
			vectProcID.push_back( id );
		}
	}
	int ndx =0 ;
	for (int m = 0 ; m < (int)vectProcID.size() ; m++)
	{
		ndx = basebase->findEntry(vectProcID[m]) ;
		if(ndx ==INT_MAX)
		{
			basebase->addEntry(vectProcID[m]) ;
			ndx  = basebase->findEntry(vectProcID[m]) ;	
			schedule = basebase->getDatabase(ndx) ;
		}else
		{
			schedule = basebase->getDatabase(ndx) ;
			for(int i = 0 ; i < schedule->getCount() ;i++)
			{
				if(schedule->getItem(i)->getFlag() == 0)
				{
                       oper = RULE_REPLACE ;
					   break ;
				}
			}
			if(oper == RULE_REPLACE)
				schedule->ClearAllAssignment() ;
		}
		_AssSchedule.push_back(schedule);
	}
}
void CRosterRulesDlg::GetCurrentSelRules(std::vector<CRosterRule*>&  _selRules)
{
	CListCtrl* p_List = NULL ;
	CRosterRule* p_rule = NULL ;
	if(m_check_schedule.GetCheck() == BST_CHECKED)
		p_List = &m_ruleList_Schedule ;
	else
		p_List = &m_rulesList ;
	for (int i = 0 ; i < p_List->GetItemCount() ; i++)
	{

			p_rule = (CRosterRule*)p_List->GetItemData(i) ;
			_selRules.push_back(p_rule) ;
	}
}

void CRosterRulesDlg::AddRosterToProcessor(int oper) 
{
	if(p_GroupRules->getCount() == 0)
		return ;
	ProcAssignDatabase* basebase = P_input->procAssignDB ;
	ProcessorRosterSchedule* schedule = NULL ;
	ProcessorRoster* p_Ass = NULL ;
	CRosterRule* p_rule = NULL ;
	//UnmergedAssignments m_unMergedAssgn;
	std::vector<ProcessorRosterSchedule*> schedules ;
	int ndx =0 ;
	for (int m = 0 ; m < cout_proc ; m++)
	{
		ndx = basebase->findEntry(proName_list[m]) ;
		if(ndx ==INT_MAX)
		{
			basebase->addEntry(proName_list[m]) ;
			ndx  = basebase->findEntry(proName_list[m]) ;	
			schedule = basebase->getDatabase(ndx) ;
		}else
		{
		schedule = basebase->getDatabase(ndx) ;
		if(oper == RULE_REPLACE)
			schedule->ClearAllAssignment() ;
		}
		schedules.push_back(schedule);
	}
	for (int i = 0 ; i < p_GroupRules->getCount() ;i++)
	{
		p_rule = p_GroupRules->getItem(i) ;
		if(p_rule->GetCount() > cout_proc)
			p_rule->SetCount(cout_proc) ;
		for (int j = 0; j < p_rule->GetCount()&&GetCurrentProcessor() < cout_proc; j++ ,MoveNextProcessor())
		{
			p_Ass = new ProcessorRoster ;
		//	p_rule->FormatProAssignment(*p_Ass) ;
         //   schedules[GetCurrentProcessor()]->addItem(p_Ass) ;
		}
	}
}
void CRosterRulesDlg::OnBnClickedButtonClose()
{
	// TODO: Add your control notification handler code here
	CXTResizeDialog::OnCancel() ;
}
void CRosterRulesDlg::ONBnClickButtonSave()
{
	p_rulesDB->WriteDataToDB() ;
	p_ruleSchedule->WriteDataToDB();
	CXTResizeDialog::OnOK() ;
}
void CRosterRulesDlg::OnLvnItemchangedListRosterRules(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	bool m_show = TRUE ;
	if(m_check_schedule.GetCheck() == BST_CHECKED)
	{
		if(m_ruleList_Schedule.GetSelectedCount() == 0 )
          m_show = FALSE ;
	}else
	{
		if(m_rulesList.GetSelectedCount() == 0)
			m_show = FALSE ;
	}
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,m_show)  ;
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,m_show) ;
	// TODO: Add your control notification handler code here
	*pResult = 0;
	OnHdnItemclickListRosterRules(pNMHDR,pResult);
}

void CRosterRulesDlg::OnHdnItemclickListRosterRules(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
    if(m_check_schedule.GetCheck() == BST_CHECKED)
	{
		int cout = m_ruleList_Schedule.GetItemCount() ;
		for (int i = 0 ; i < cout ; i++)
		{
			if(m_rulesList.GetCheck(i))
			{
				GetDlgItem(ID_BUTTON_APPEND)->EnableWindow(TRUE) ;
				GetDlgItem(ID_BUTTON_REPLACE)->EnableWindow(TRUE) ;
				return ;
			}
		}
	}else
	{
		int cout = m_rulesList.GetItemCount() ;
		for (int i = 0 ; i < cout ; i++)
		{
			if(m_rulesList.GetCheck(i))
			{
				GetDlgItem(ID_BUTTON_APPEND)->EnableWindow(TRUE) ;
				GetDlgItem(ID_BUTTON_REPLACE)->EnableWindow(TRUE) ;
				return ;
			}
		}
	}
	GetDlgItem(ID_BUTTON_APPEND)->EnableWindow(FALSE) ;
	GetDlgItem(ID_BUTTON_REPLACE)->EnableWindow(FALSE) ;
	*pResult = 0;
}

void CRosterRulesDlg::InitRulesList()
{
	InitRulesListView() ;
	InitRulesListData() ;
}
void CRosterRulesDlg::InitRulesListView()
{
	DWORD dwStyle = m_rulesList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_rulesList.SetExtendedStyle( dwStyle );
	int nwidth[] = {170,90,90,125,50,80} ;
	for (int i = 0 ; i < (int)m_RuleListColNames.size() ; i++)
	{
		m_rulesList.InsertColumn(i,m_RuleListColNames[i],LVCFMT_CENTER,nwidth[i]) ;
	}
	m_rulesList.InitColumnWidthPercent() ;
}
void CRosterRulesDlg::InitRulesListData()
{
	m_rulesList.DeleteAllItems() ;
	ProcessorID id ;
	if(p_GroupRules == NULL)
		return ;
	m_rulesList.DeleteAllItems() ;
	if(p_GroupRules == NULL)
		return ;
	int cout = p_GroupRules->getCount() ;
	CRosterRule* rule = NULL ;
	int ndx = 0 ;
	TCHAR passTY[1024] = {0} ;
	for (int i = 0 ; i< cout ; i++)
	{
		rule = p_GroupRules->getItem(i) ;
		const CMobileElemConstraint* pConst = rule->GetAssignment()->getConstraint( 0 );
		pConst->screenPrint(passTY) ;
		ndx = m_rulesList.InsertItem(0,passTY) ;
		m_rulesList.SetItemText(ndx,1,rule->GetOpenTime().printTime()) ;
		m_rulesList.SetItemText(ndx,2,rule->GetCloseTime().printTime()) ;
		m_rulesList.SetItemText(ndx,3,GetRelationName(rule->GetRelation()) ) ;
		if(rule->IsDaily())
			_tcscpy(passTY,_T("Yes"));
		else
			_tcscpy(passTY,_T("No"));
		m_rulesList.SetItemText(ndx,4,passTY) ;
		if(rule->GetCount() == cout_proc)
			StrCpy(passTY , _T("All")) ;
		else
		itoa(rule->GetCount(),passTY,10) ;
		m_rulesList.SetItemText(ndx,5,passTY) ;
		m_rulesList.SetItemData(ndx,(DWORD_PTR)rule) ;
	}
}
void CRosterRulesDlg::InitRuleScheduleList() 
{
	InitRuleScheduleListView() ;
	InitRuleScheduleListData() ;
}
void CRosterRulesDlg::InitRuleScheduleListView()
{
	DWORD dwStyle = m_ruleList_Schedule.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_ruleList_Schedule.SetExtendedStyle( dwStyle );
	int nwidth[] = {190,100,100,135,80} ;
	for (int i = 0 ; i < (int)m_RuleScheduleListColNames.size() ; i++)
	{
		m_ruleList_Schedule.InsertColumn(i,m_RuleScheduleListColNames[i],LVCFMT_CENTER,nwidth[i]) ;
	}
	m_ruleList_Schedule.InitColumnWidthPercent();
}
void CRosterRulesDlg::InitRuleScheduleListData()
{
	m_ruleList_Schedule.DeleteAllItems() ;
    
	for (int i = 0 ; i <  p_RulesForSchedule->getCount() ; i++)
		AddLineToRuleScheduleList(p_RulesForSchedule->getItem(i)) ;
}
void CRosterRulesDlg::AddLineToRulesList(CRosterRule* _rule)
{
	CMobileElemConstraint* p_con = _rule->GetAssignment()->getConstraint(0);
	if(p_con == NULL)
		return ;
	CString str ;
	p_con->screenPrint(str) ;
	int ndx = m_rulesList.InsertItem(0,str) ;
	m_rulesList.SetItemText(ndx,1,_rule->GetOpenTime().printTime()) ;
	m_rulesList.SetItemText(ndx,2,_rule->GetCloseTime().printTime()) ;
	m_rulesList.SetItemText(ndx,3,GetRelationName(_rule->GetRelation())) ;
	m_rulesList.SetItemText(ndx,4,GetIsDailyName(_rule->IsDaily())) ;
	int count = _rule->GetCount() ;
	char ProCount[10]  = {0} ;
	if(_rule->GetCount() == cout_proc)
		StrCpy(ProCount , _T("All")) ;
	else
	_itoa(count,ProCount,10) ;
	m_rulesList.SetItemText(ndx,5,ProCount) ;
	m_rulesList.SetItemData(ndx,(DWORD_PTR)_rule) ;
}
void CRosterRulesDlg::EditSelLineOfRulesList(CRosterRule* _rule)
{
	POSITION posi = m_rulesList.GetFirstSelectedItemPosition() ;
	if(posi == NULL)
		return ;
	int ndx = m_rulesList.GetNextSelectedItem(posi) ;
	CMobileElemConstraint* p_con = _rule->GetAssignment()->getConstraint(0);
	CString str ;
	p_con->screenPrint(str) ;
	char ProCount[10]  = {0} ;
	if(_rule != NULL)
	{
		m_rulesList.SetItemText(ndx,0,str) ;
		m_rulesList.SetItemText(ndx,1,_rule->GetOpenTime().printTime()) ;
		m_rulesList.SetItemText(ndx,2,_rule->GetCloseTime().printTime()) ;
		m_rulesList.SetItemText(ndx,3,GetRelationName(_rule->GetRelation())) ;
		m_rulesList.SetItemText(ndx,4,GetIsDailyName(_rule->IsDaily())) ;
		if(_rule->GetCount() == cout_proc)
			StrCpy(ProCount , _T("All")) ;
		else
		_itoa(_rule->GetCount(),ProCount,10) ;
		m_rulesList.SetItemText(ndx,5,ProCount) ;
	}
}
void CRosterRulesDlg::DelSelLineOfRulesList()
{
	POSITION posi = m_rulesList.GetFirstSelectedItemPosition() ;
	if(posi == NULL)
		return ;
	int ndx = m_rulesList.GetNextSelectedItem(posi) ;
	m_rulesList.DeleteItem(ndx) ;
}


CString CRosterRulesDlg::GetRelationName(int _rel)
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
ProcAssRelation CRosterRulesDlg::GetRelationByName(CString name)
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
CString CRosterRulesDlg::GetIsDailyName(int _data)
{
	CString name ;
	if(_data == 1)
		name = "TRUE" ;
	else
		name = "FALSE" ;
	return name ;
}

void CRosterRulesDlg::OnRulesNew()
{
	if(m_check_schedule.GetCheck() == BST_CHECKED )
		NewRulesByScheduleForProcessor() ;
	else
		NewRulesForProcessor() ;   
}
void CRosterRulesDlg::OnRulesEdit()
{
	if(m_check_schedule.GetCheck() == BST_CHECKED)
		EditRulesByScheduleForProcessor() ;
	else
		EditRulesForProcessor() ;
}
void CRosterRulesDlg::OnRulesDelete()
{
	if(m_check_schedule.GetCheck() == BST_CHECKED)
		DelRulesByScheduleForProcessor() ;
	else
		DelRulesForProcessor() ;
}
void CRosterRulesDlg::NewRulesForProcessor()
{

	CRosterRule* p_newRule = new CRosterRule(P_input) ;
	CRuleDefineDlg dlg(p_newRule,P_input,cout_proc) ;
	if( dlg.DoModal() == IDCANCEL)
		return ;
	p_GroupRules->addItem(p_newRule) ;
	AddLineToRulesList(p_newRule) ;
}
void CRosterRulesDlg::EditRulesForProcessor()
{
	POSITION pois = m_rulesList.GetFirstSelectedItemPosition() ;
	if(pois == NULL)
		return ;
	int ndx = m_rulesList.GetNextSelectedItem(pois) ;
	CRosterRule* p_newRule = (CRosterRule*)m_rulesList.GetItemData(ndx) ;
	CRuleDefineDlg dlg(p_newRule,P_input,cout_proc) ;
	dlg.SetDlgCaption("Rule Edit") ;
	if( dlg.DoModal() == IDCANCEL)
		return ;
	EditSelLineOfRulesList(p_newRule) ;
}
void CRosterRulesDlg::DelRulesForProcessor()
{
	POSITION pois = m_rulesList.GetFirstSelectedItemPosition() ;
	if(pois == NULL)
		return ;
	int ndx = m_rulesList.GetNextSelectedItem(pois) ;
	CRosterRule* p_newRule = (CRosterRule*)m_rulesList.GetItemData(ndx) ;
	p_GroupRules->RemoveRule(p_newRule);
	DelSelLineOfRulesList() ;
}

void CRosterRulesDlg::NewRulesByScheduleForProcessor()
{

	CRosterRule* p_newRule = new CRosterRulesBySchedule(P_input) ;
	CRuleByScheduleDefineDlg dlg(p_newRule,P_input,cout_proc) ;
	if( dlg.DoModal() == IDCANCEL)
		return ;
	p_RulesForSchedule->addItem(p_newRule) ;
	AddLineToRuleScheduleList(p_newRule) ;
}
void CRosterRulesDlg::EditRulesByScheduleForProcessor()
{
	POSITION _posi  = m_ruleList_Schedule.GetFirstSelectedItemPosition() ;
	if(_posi == NULL)
		return ;
	int ndx = m_ruleList_Schedule.GetNextSelectedItem(_posi) ;
	CRosterRule* _rule = (CRosterRule*)m_ruleList_Schedule.GetItemData(ndx) ;

	CRuleByScheduleDefineDlg dlg(_rule,P_input,cout_proc) ;
	dlg.SetDlgCaption("Rule Edit") ;
	if( dlg.DoModal() == IDCANCEL)
		return ;
	EditSelLineFromRuleScheduleList(_rule) ;
}
void CRosterRulesDlg::DelRulesByScheduleForProcessor()
{
	POSITION _posi  = m_ruleList_Schedule.GetFirstSelectedItemPosition() ;
	if(_posi == NULL)
		return ;
	int ndx = m_ruleList_Schedule.GetNextSelectedItem(_posi) ;
	CRosterRule* _rule = (CRosterRule*)m_ruleList_Schedule.GetItemData(ndx) ;
	p_RulesForSchedule->RemoveRule(_rule) ;
	DelSelLineOfRuleScheduleList() ;
}
CString CRosterRulesDlg::FormatOpenTimeForRuleScheduleList(CRosterRule* _rule)
{
	if(_rule == NULL)
		return CString() ;
	CString str ;
	if(_rule->GetOpenIfAdvance() == CRosterRule::BEFORE)
		str.Format(_T("-%s"),_rule->GetOpenTime().printTime()) ;
	else
		str.Format(_T("%s"),_rule->GetOpenTime().printTime()) ;
	return str ;
}
CString CRosterRulesDlg::FormatCloseTimeForRuleScheduleList(CRosterRule* _rule)
{
	if(_rule == NULL)
		return CString() ;
	CString str ;
	if(_rule->GetCloseIfAdvance() == CRosterRule::BEFORE)
		str.Format(_T("-%s"),_rule->GetCloseTime().printTime()) ;
	else
		str.Format(_T("%s"),_rule->GetCloseTime().printTime()) ;
	return str ;
}
void CRosterRulesDlg::AddLineToRuleScheduleList(CRosterRule* _rule)
{
	if(_rule == NULL)
		return ;
	CMobileElemConstraint* _con =  _rule->GetAssignment()->getConstraint(0) ;
	CString str ;
	if(_con != NULL)
		_con->screenPrint(str) ;
	int ndx = m_ruleList_Schedule.InsertItem(0,str) ;
	m_ruleList_Schedule.SetItemText(ndx,1,FormatOpenTimeForRuleScheduleList(_rule)) ;
	m_ruleList_Schedule.SetItemText(ndx,2,FormatCloseTimeForRuleScheduleList(_rule)) ;
	m_ruleList_Schedule.SetItemText(ndx,3,GetRelationName(_rule->GetRelation())) ;
	TCHAR cout[1024] = {0} ;
	if(_rule->GetCount() == cout_proc)
		StrCpy(cout , _T("All")) ;
	else
		_itoa(_rule->GetCount(),cout,10) ;
	m_ruleList_Schedule.SetItemText(ndx ,4,cout) ;
	m_ruleList_Schedule.SetItemData(ndx,(DWORD_PTR)_rule) ;
}
void CRosterRulesDlg::EditSelLineFromRuleScheduleList(CRosterRule* _rule)
{
	if(_rule == NULL)
		return ;
	CMobileElemConstraint* _con = _rule->GetAssignment()->getConstraint(0) ;
	CString str ;
	if(_con != NULL)
		_con->screenPrint(str) ;
	POSITION _posi = m_ruleList_Schedule.GetFirstSelectedItemPosition() ;
	if(_posi == NULL)
		return ;
	int ndx = m_ruleList_Schedule.GetNextSelectedItem(_posi) ;
	m_ruleList_Schedule.SetItemText(ndx,0,str) ;
	m_ruleList_Schedule.SetItemText(ndx,1,FormatOpenTimeForRuleScheduleList(_rule)) ;
	m_ruleList_Schedule.SetItemText(ndx,2,FormatCloseTimeForRuleScheduleList(_rule)) ;
	m_ruleList_Schedule.SetItemText(ndx,3,GetRelationName(_rule->GetRelation())) ;
	TCHAR cout[1024] = {0} ;
	if(_rule->GetCount() == cout_proc)
		StrCpy(cout , _T("All")) ;
	else
	_itoa(_rule->GetCount(),cout,10) ;
	m_ruleList_Schedule.SetItemText(ndx ,4,cout) ;
}
void CRosterRulesDlg::DelSelLineOfRuleScheduleList()
{
	POSITION _posi = m_ruleList_Schedule.GetFirstSelectedItemPosition() ;
	if(_posi == NULL)
		return ;
	int ndx = m_ruleList_Schedule.GetNextSelectedItem(_posi) ;
	m_ruleList_Schedule.DeleteItem(ndx) ;
}
//void CRosterRulesDlg::OnBnClickedCheckCheckall()
//{
//	if(m_check_Sel.GetCheck() != BST_CHECKED)
//	{
//		if(m_check_schedule.GetCheck() == BST_CHECKED)
//			SelAllRuleByScheduleList(FALSE) ;
//		else
//			SelAllRuleList(FALSE) ;
//		GetDlgItem(ID_BUTTON_APPEND)->EnableWindow(FALSE) ;
//		GetDlgItem(ID_BUTTON_REPLACE)->EnableWindow(FALSE) ;
//		return ;
//	}
//	// TODO: Add your control notification handler code here
//	if(m_check_schedule.GetCheck() == BST_CHECKED)
//		SelAllRuleByScheduleList() ;
//	else
//		SelAllRuleList() ;
//	GetDlgItem(ID_BUTTON_APPEND)->EnableWindow(TRUE) ;
//	GetDlgItem(ID_BUTTON_REPLACE)->EnableWindow(TRUE) ;
//
//
//}
void CRosterRulesDlg::SelAllRuleByScheduleList(BOOL sel)
{
	int cout = m_ruleList_Schedule.GetItemCount() ;
	for (int i = 0 ; i < cout ; i++)
	{
		m_ruleList_Schedule.SetCheck(i,sel) ;
		m_rulesList.EnsureVisible(i,TRUE) ;
	}
}
void CRosterRulesDlg::SelAllRuleList(BOOL sel)
{

	int cout = m_rulesList.GetItemCount() ;
	for (int i = 0 ; i < cout ; i++)
	{
		m_rulesList.SetCheck(i,sel) ;
		m_rulesList.EnsureVisible(i,TRUE) ;
	}

}
