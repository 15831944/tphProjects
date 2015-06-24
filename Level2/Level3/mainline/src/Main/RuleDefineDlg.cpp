// RuleDefineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "RuleDefineDlg.h"
#include ".\ruledefinedlg.h"


// CRuleDefineDlg dialog

IMPLEMENT_DYNAMIC(CRuleDefineDlg, CDialog)
CRuleDefineDlg::CRuleDefineDlg(CRosterRule* p_rule,InputTerminal* P_input,int cout_Pro,CWnd* pParent /*=NULL*/)
	: P_Rule(p_rule),isDaily(TRUE),InputTer(P_input),num_Pro(cout_Pro),CDialog(CRuleDefineDlg::IDD, pParent)
{
	m_startDate = COleDateTime::GetCurrentTime();
	m_endDate = COleDateTime::GetCurrentTime();
	m_mobCon = *P_Rule->GetAssignment()->getConstraint(0);
	m_TY = TY_RULE ;
	m_Caption = "Rule Define" ;
}

CRuleDefineDlg::~CRuleDefineDlg()
{
}

void CRuleDefineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PAXTY, m_Edit_PaxTY);
	DDX_Control(pDX, IDC_COMBO2, m_ComboStartDay);
	DDX_Control(pDX, IDC_COMBO3, m_ComboEndDay);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START, m_start_TimeCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START3, m_end_TimeCtrl);
	DDX_Control(pDX, IDC_CHECK_Daily, m_Daily_Check);
	DDX_Control(pDX, IDC_SPIN_NUM, m_spin_num);
	DDX_Control(pDX, IDC_STATIC_START, m_staTime);
	DDX_Control(pDX, IDC_STATIC_END, m_staEndTime);
	DDX_Control(pDX, IDC_COMBO_START, m_Com_Start);
	DDX_Control(pDX, IDC_COMBO_END, m_Com_end);
	DDX_Control(pDX, IDC_CHECK_DEFINE_NUMBER, m_CheckBoxNumberOfPro);
	DDX_Control(pDX, IDC_STATIC_NUMBER, m_StaticNumber);
	DDX_Control(pDX, IDC_EDIT_NUM, m_editNum);
}


BEGIN_MESSAGE_MAP(CRuleDefineDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_Daily, OnBnClickedCheckDaily)
	ON_BN_CLICKED(IDC_BUTTON_PAXTY, OnBnClickedButtonPaxty)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_DEFINE_NUMBER, OnBnClickedCheckDefineNumber)
	ON_CBN_SELCHANGE(IDC_COMBO_END, OnCbnSelchangeComboEnd)
END_MESSAGE_MAP()

BOOL CRuleDefineDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText(m_Caption) ;
	isDaily = P_Rule->IsDaily() ;
	// TODO: Add extra initialization here
	m_spin_num.SetRange(1,num_Pro) ;
	if(P_Rule->GetCount() == num_Pro)
	{
		m_CheckBoxNumberOfPro.SetCheck(BST_UNCHECKED) ;
		m_spin_num.SetPos(1) ;
	}
	else
	{
	
		if(P_Rule->GetCount() != 0)
		{
			m_CheckBoxNumberOfPro.SetCheck(BST_CHECKED) ;
			m_spin_num.SetPos(P_Rule->GetCount()) ;
			m_spin_num.EnableWindow(TRUE);
			m_StaticNumber.EnableWindow(TRUE) ;
			m_editNum.EnableWindow(TRUE) ;
		}
		else
		{
			m_CheckBoxNumberOfPro.SetCheck(BST_UNCHECKED) ;
			m_spin_num.SetPos(num_Pro) ;
			m_spin_num.EnableWindow(FALSE);
			m_StaticNumber.EnableWindow(FALSE) ;
			m_editNum.EnableWindow(FALSE) ;
		}
	}

   
	m_start_TimeCtrl.SetFormat( "HH:mm:ss" );
	m_end_TimeCtrl.SetFormat( "HH:mm:ss" );
	m_start_TimeCtrl.GetWindowRect(&pos_starttimeCtr) ;
	ScreenToClient(&pos_starttimeCtr) ;
	m_end_TimeCtrl.GetWindowRect(&pos_endtimeCtr) ;
	ScreenToClient(&pos_endtimeCtr) ;
	//init check box 
	if(isDaily)
		m_Daily_Check.SetCheck(BST_CHECKED) ;
	else
		m_Daily_Check.SetCheck(BST_UNCHECKED) ;
	//get the flight schedule time , it is absolute time or relative time

	ElapsedTime startTime = P_Rule->GetOpenTime();
	ElapsedTime endTime	  = P_Rule->GetCloseTime();

	CStartDate flightDate =  InputTer->flightSchedule->GetStartDate();
	bool isab ;
	COleDateTime _data ;
	COleDateTime _time ;
	int n_startdays = 0 ;
	int n_enddays = 0 ;
	flightDate.GetDateTime(startTime,isab,_data,n_startdays,_time) ;
    m_start_TimeCtrl.SetTime(_time) ;
	flightDate.GetDateTime(endTime,isab,_data,n_enddays,_time) ;
	m_end_TimeCtrl.SetTime(_time) ;
   
    int nMaxDay = 1;
	if(!isDaily)
	{
	FlightSchedule* fs = InputTer->flightSchedule;
		if (fs->getCount() > 0)
			nMaxDay = fs->GetFlightScheduleEndTime().GetDay();
	} else
		HideComboBoxTime();
	InitComboBeginAndEndDay(nMaxDay,n_startdays,n_enddays) ;
	CString szTmp;
	const CMobileElemConstraint* constraint = P_Rule->GetAssignment()->getConstraint(0) ;
	if(constraint!= NULL)
		constraint->screenPrint( szTmp, 0, 256 );
	m_Edit_PaxTY.SetWindowText(szTmp) ;
	UpdateData( FALSE );
	m_Com_Start.ShowWindow(SW_HIDE) ;
	m_Com_end.ShowWindow(SW_HIDE) ;
	return TRUE;  
}
void CRuleDefineDlg::HideComboBoxTime()
{
	m_ComboStartDay.ShowWindow(SW_HIDE);
	m_ComboEndDay.ShowWindow(SW_HIDE) ;
	CRect rect ;
	m_ComboStartDay.GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	m_start_TimeCtrl.MoveWindow(&rect);


	m_ComboEndDay.GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	m_end_TimeCtrl.MoveWindow(&rect) ;
}
void CRuleDefineDlg::showComboBoxTime()
{

	m_start_TimeCtrl.MoveWindow(pos_starttimeCtr) ;
	m_end_TimeCtrl.MoveWindow(pos_endtimeCtr) ;

	m_start_TimeCtrl.ShowWindow(SW_SHOW) ;
	m_end_TimeCtrl.ShowWindow(SW_SHOW) ;
	m_ComboStartDay.ShowWindow(SW_SHOW) ;
	m_ComboEndDay.ShowWindow(SW_SHOW) ;
}
// CRuleDefineDlg message handlers
void CRuleDefineDlg::InitComboBeginAndEndDay(int n_day,	int nStartDays ,int nenddays )
{
	m_ComboStartDay.ResetContent() ;
	m_ComboEndDay.ResetContent()  ;
	for (int i = 1 ;i<= n_day ;i++)
	{	
		CString strText;
		int ndx = 0 ;
		strText.Format("Day %d",i);
		ndx = m_ComboStartDay.AddString(strText);
		 m_ComboStartDay.SetItemData(ndx,i-1) ;
		ndx = m_ComboEndDay.AddString(strText);
       	 m_ComboEndDay.SetItemData(ndx,i-1) ;
	}
	m_ComboStartDay.SetCurSel(nStartDays);	
	m_ComboEndDay.SetCurSel(nenddays);
	if(m_Daily_Check.GetCheck() == BST_CHECKED)
	{
		m_ComboStartDay.EnableWindow(FALSE) ;
		m_ComboEndDay.EnableWindow(FALSE) ;
	}else
	{
		m_ComboStartDay.EnableWindow(TRUE) ;
		m_ComboEndDay.EnableWindow(TRUE) ;
	}
}
void CRuleDefineDlg::OnBnClickedCheckDaily()
{
	// TODO: Add your control notification handler code here
	int state = m_Daily_Check.GetCheck() ;
	if(state == BST_CHECKED)
	{
			isDaily = TRUE ;
			InitComboBeginAndEndDay(1,0,0) ;
			HideComboBoxTime() ;
	}
	if(state == BST_UNCHECKED)
	{
		int nMaxDay = 1 ;
		isDaily = FALSE ;
		FlightSchedule* fs = InputTer->flightSchedule;
		if (fs->getCount() > 0)
			nMaxDay = fs->GetFlightScheduleEndTime().GetDay();
		InitComboBeginAndEndDay(nMaxDay,0,0);
		showComboBoxTime() ;
	}
}

void CRuleDefineDlg::OnBnClickedButtonPaxty()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CPassengerTypeDialog dlg( m_pParentWnd );
	if( dlg.DoModal() == IDCANCEL)
		return;

	m_mobCon = dlg.GetMobileSelection();
	CString szTmp;
	//char szTmp[256];
	m_mobCon.screenPrint(szTmp,0,256);
	m_Edit_PaxTY.SetWindowText(szTmp) ;
	UpdateData( FALSE );
	return;
}

void CRuleDefineDlg::OnBnClickedOk()
{
   P_Rule->GetAssignment()->setConstraint(m_mobCon,0);
   ElapsedTime startTime;
   ElapsedTime endTime;
   COleDateTime time ;
   COleDateTime day ;
   int nday = 0 ;
   CStartDate flightDate =  InputTer->flightSchedule->GetStartDate();
   m_start_TimeCtrl.GetTime(time) ;
   nday = m_ComboStartDay.GetItemData(m_ComboStartDay.GetCurSel()) ;
   day.SetDate(0,0,nday);
   flightDate.GetElapsedTime(FALSE,day,nday,time,startTime) ;
   m_end_TimeCtrl.GetTime(time) ;
   nday = m_ComboEndDay.GetItemData(m_ComboEndDay.GetCurSel()) ;
   day.SetDate(0,0,nday) ;
   flightDate.GetElapsedTime(FALSE,day,nday,time,endTime) ;

   int ndx = m_Com_Start.GetCurSel() ;
   CRosterRule::INTERVAL_TIME startAdvance = (CRosterRule::INTERVAL_TIME)m_Com_Start.GetItemData(ndx) ;
   ndx = m_Com_end.GetCurSel() ;
   CRosterRule::INTERVAL_TIME endAdvance = (CRosterRule::INTERVAL_TIME)m_Com_end.GetItemData(ndx) ;
   if ( m_TY == TY_RULE && startTime > endTime)
   {
	   MessageBox("Open time must less than close time!\r\nPlease input the time again.",NULL,MB_OK|MB_ICONINFORMATION );
	   return;
   }
   if(m_TY == TY_SCHEDULE_RULE )
   {
	   if(startAdvance == CRosterRule::AFTER && endAdvance == CRosterRule::BEFORE)
	   {
	   MessageBox("Open time  must before the close time !\r\nPlease input the time again.",NULL,MB_OK|MB_ICONINFORMATION );
	   return;
	   }
	   if(startAdvance == CRosterRule::AFTER && endAdvance == CRosterRule::AFTER && startTime > endTime)
	   {
		   MessageBox("Open time Interval must bigger than Close time Interval!\r\nPlease input the time again.",NULL,MB_OK|MB_ICONINFORMATION );
		   return;
	   }
	   if(startAdvance == CRosterRule::BEFORE && endAdvance == CRosterRule::BEFORE && startTime < endTime)
	   {
		   MessageBox("Open time  must before the close time !\r\nPlease input the time again.",NULL,MB_OK|MB_ICONINFORMATION );
		   return;
	   }
   }
   if(m_CheckBoxNumberOfPro.GetCheck() == BST_CHECKED)
		num_Pro = m_spin_num.GetPos() ;
   P_Rule->SetOpenTime(startTime) ;
   P_Rule->SetCloseTime(endTime) ;
   P_Rule->IsDaily(isDaily) ;
   P_Rule->SetCount(num_Pro) ;
   P_Rule->SetOpenAdvance(startAdvance) ;
   P_Rule->SetCloseAdvance(endAdvance) ;
	// TODO: Add your control notification handler code here
	OnOK();
}

void CRuleDefineDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
CRuleByScheduleDefineDlg::CRuleByScheduleDefineDlg(CRosterRule* p_rule,InputTerminal* P_input,int cout_Pro,CWnd* pParent)
: CRuleDefineDlg(p_rule,P_input,cout_Pro,pParent)
{
   m_day= 0 ;
   m_Caption = _T("Rule By Schedule Define") ;
}
CRuleByScheduleDefineDlg::~CRuleByScheduleDefineDlg()
{

}
BOOL CRuleByScheduleDefineDlg::OnInitDialog()
{
	CRuleDefineDlg::OnInitDialog() ;
	m_staTime.SetWindowText(_T("Open Interval Time:")) ;
	m_staEndTime.SetWindowText(_T("Close Interval Time:")) ;
	m_Daily_Check.ShowWindow(SW_HIDE) ;
	m_TY = TY_SCHEDULE_RULE ;
	m_Com_Start.ShowWindow(SW_SHOW) ;
	m_Com_end.ShowWindow(SW_SHOW) ;
	InitAheadCombox(m_Com_Start) ;
	InitAheadCombox(m_Com_end) ;
    if(P_Rule->GetOpenIfAdvance() == CRosterRule::BEFORE)
		m_Com_Start.SetCurSel(0) ;
	else
		m_Com_Start.SetCurSel(1) ;

	if(P_Rule->GetCloseIfAdvance() == CRosterRule::BEFORE)
		m_Com_end.SetCurSel(0) ;
	else
		m_Com_end.SetCurSel(1) ;
	return TRUE ;
}
void CRuleByScheduleDefineDlg::InitAheadCombox(CComboBox& _comboxCtrl) 
{
	int ndx = _comboxCtrl.InsertString(-1,_T("Before")) ;
	_comboxCtrl.SetItemData(ndx,CRosterRule::BEFORE) ;
	ndx =  _comboxCtrl.InsertString(-1,_T("After")) ;
	_comboxCtrl.SetItemData(ndx,CRosterRule::AFTER) ;
}
//void CRuleByScheduleDefineDlg::OnBnClickedCheckDaily()
//{
//	int state = m_Daily_Check.GetCheck() ;
//	if(state == BST_CHECKED)
//	{
//		isDaily = TRUE ;
//	
//	}
//	if(state == BST_UNCHECKED)
//	{
//		isDaily = FALSE ;
//		m_day =  m_ComboStartDay.GetItemData(m_ComboStartDay.GetCurSel()) ;
//	}
//}
void CRuleDefineDlg::OnBnClickedCheckDefineNumber()
{
	// TODO: Add your control notification handler code here
   if(m_CheckBoxNumberOfPro.GetCheck() == BST_CHECKED)
   {
	  m_spin_num.EnableWindow(TRUE);
	  m_StaticNumber.EnableWindow(TRUE) ;
	  m_editNum.EnableWindow(TRUE) ;
   }else
   {
	   m_spin_num.EnableWindow(FALSE);
	   m_StaticNumber.EnableWindow(FALSE) ;
	   m_editNum.EnableWindow(FALSE) ;
   }
}

void CRuleDefineDlg::OnCbnSelchangeComboEnd()
{
	// TODO: Add your control notification handler code here
}
