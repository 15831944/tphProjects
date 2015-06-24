// singleEventsDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "singleEventsDataDlg.h"
#include "resource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PassengerTypeDialog.h"
#include "TermPlanDoc.h"
#include ".\singleeventsdatadlg.h"
/////////////////////////////////////////////////////////////////////////////
// CsingleEventsDataDlg dialog


CsingleEventsDataDlg::CsingleEventsDataDlg(CWnd* pParent, BOOL _bCanModify ,Processor* pro,ProcessorRosterSchedule* p_scedule)
	: CDialog(CsingleEventsDataDlg::IDD, pParent),m_procAssign(GetInputTerminal())
	, m_startDate(COleDateTime::GetCurrentTime())
	, m_endDate(COleDateTime::GetCurrentTime())
	,P_proc(pro)
	,P_Schedule(p_scedule)
{
	//{{AFX_DATA_INIT(CsingleEventsDataDlg)
	m_strPaxType = _T("");
	m_timeEnd = COleDateTime::GetCurrentTime();
	m_timeBegin = COleDateTime::GetCurrentTime();
	m_iRelationRadio = -1;
	//}}AFX_DATA_INIT
	m_strWindowTitle = "Event for ";
	m_bCanModifyFlag = _bCanModify;
	m_startDate = COleDateTime::GetCurrentTime();
	m_endDate = COleDateTime::GetCurrentTime();
}


void CsingleEventsDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CsingleEventsDataDlg)
	DDX_Control(pDX, IDC_BUT_DEF_PAXTYPE, m_butDefPaxType);
	DDX_Control(pDX, IDC_COMBO_STATE, m_combState);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START, m_timeBeginCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_timeEndCtrl);
	DDX_Text(pDX, IDC_EDIT_PAXTYPE, m_strPaxType);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_timeEnd);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START, m_timeBegin);
	DDX_Radio(pDX, IDC_RADIO_INCLUSIVE, m_iRelationRadio);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_DATETIMEPICKER_STARTDATE, m_timeBeginDateCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER_ENDDATE, m_timeEndDateCtrl);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_STARTDATE, m_startDate);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_ENDDATE, m_endDate);
	DDX_Control(pDX, IDC_COMBO_STARTDAYS, m_comboStartDay);
	DDX_Control(pDX, IDC_COMBO_ENDDAYS, m_comboEndDay);
	DDX_Control(pDX, IDC_CHECK_Daily, m_checkDaily);
}


BEGIN_MESSAGE_MAP(CsingleEventsDataDlg, CDialog)
	//{{AFX_MSG_MAP(CsingleEventsDataDlg)
	ON_BN_CLICKED(IDC_BUT_DEF_PAXTYPE, OnButDefPaxtype)
	ON_CBN_SELCHANGE(IDC_COMBO_STATE, OnSelchangeComboState)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_ENDDATE, OnDtnDatetimechangeDatetimepickerEnddate)
	ON_BN_CLICKED(IDC_CHECK_Daily, OnBnClickedCheckDaily)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CsingleEventsDataDlg message handlers

BOOL CsingleEventsDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
    m_timeBeginCtrl.GetWindowRect(&pos_starttimeCtr) ;
	ScreenToClient(&pos_starttimeCtr) ;
	m_timeEndCtrl.GetWindowRect(&pos_endtimeCtr) ;
	ScreenToClient(&pos_endtimeCtr) ;
    isDaily = m_procAssign.IsDaily() ;
	// TODO: Add extra initialization here
	m_combState.SetCurSel( m_procAssign.isOpen()  ? 0 : 1  );
	m_combState.EnableWindow( m_bCanModifyFlag ? TRUE : FALSE );
	
	m_timeBeginCtrl.SetFormat( "HH:mm:ss" );
	m_timeEndCtrl.SetFormat( "HH:mm:ss" );

    //init check box 
	if(isDaily)
		m_checkDaily.SetCheck(BST_CHECKED) ;
	else
		m_checkDaily.SetCheck(BST_UNCHECKED) ;
	//get the flight schedule time , it is absolute time or relative time

	ElapsedTime startTime = m_procAssign.getAbsOpenTime();
	ElapsedTime endTime	  = m_procAssign.getAbsCloseTime();

	CStartDate flightDate =  GetInputTerminal()->flightSchedule->GetStartDate();

	if (flightDate.IsAbsoluteDate()) //absolute 
	{
		//hide the comboBox
		m_comboStartDay.ShowWindow(SW_HIDE);
		m_comboEndDay.ShowWindow(SW_HIDE);

		bool nIsAbs;
		int ndays;
		flightDate.GetDateTime(startTime,nIsAbs,m_startDate,ndays,m_timeBegin);
		flightDate.GetDateTime(endTime,nIsAbs,m_endDate,ndays,m_timeEnd);


	}
	else // relative
	{
		//hide the date control
		m_timeBeginDateCtrl.ShowWindow(SW_HIDE);
		m_timeEndDateCtrl.ShowWindow(SW_HIDE);

		int nStartDays =0;
		int nEndDays = 0;
		bool nIsAbs;
		COleDateTime oleTime;
		flightDate.GetDateTime(startTime,nIsAbs,oleTime,nStartDays,m_timeBegin);
		flightDate.GetDateTime(endTime,nIsAbs,oleTime,nEndDays,m_timeEnd);
		int nFlightEndDay = 0;
		//CStartDate flightEndDate = GetInputTerminal()->flightSchedule->GetEndDate();

		//COleDateTime oleEndDate = flightEndDate.GetDate();
//		flightEndDate.GetElapsedTime(false,);

//		flightDate.GetDateTime(userEndTime,nIsAbs,oleTime,nUserEndDay,oleTime);
		
		int nMaxDay = 1;
		//
        if(!isDaily)
		{
			FlightSchedule* fs = GetInputTerminal()->flightSchedule;
			if (fs->getCount() > 0)
				nMaxDay = fs->GetFlightScheduleEndTime().GetDay();
		}   
	   InitComboBeginAndEndDay(nMaxDay,nStartDays,nEndDays) ;
	   if(isDaily)
	   {
        HideComboBoxTime() ;
	   }
/*		
		if (nStartDays < 10 )
		{
			for (int i=0;i<10;i++)
			{
				CString strText;
				strText.Format("%d",i);
				m_comboStartDay.AddString(strText);
			}
			m_comboStartDay.SetCurSel(nStartDays);
		}
		else
		{
			for (int i =0 ;i <nStartDays+1;i++)
			{
				CString strText;
				strText.Format("%d",i);
				m_comboStartDay.AddString(strText);
			}
			m_comboStartDay.SetCurSel(nStartDays);		
		}

		if (nEndDays <10)
		{
			for (int i=0;i<10;i++)
			{

				CString strText;
				strText.Format("%d",i);
				m_comboEndDay.AddString(strText);
			}
			m_comboEndDay.SetCurSel(nEndDays);
		}
		else
		{
			for (int i =0 ;i <nEndDays+1;i++)
			{
				CString strText;
				strText.Format("%d",i);
				m_comboEndDay.AddString(strText);
			}
			m_comboEndDay.SetCurSel(nEndDays);		
		}
*/
	}

	CString szTmp;
	//char szTmp[256];

//	m_timeBegin.SetTime( startTime.asHours(), (int)startTime.asMinutes()%60, (int)startTime.asSeconds()%60);
//	m_timeEnd.SetTime( endTime.asHours(), (int)endTime.asMinutes()%60, (int)endTime.asSeconds()%60 );
	
	m_procAssign.getAssignment()->getConstraint(0)->screenPrint( szTmp, 0, 256 );
	m_strPaxType = szTmp;//CString( szTmp );
	
	m_iRelationRadio = m_procAssign.getAssRelation();

	SetWindowText( m_strWindowTitle );
	UpdateData( FALSE );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CsingleEventsDataDlg::InitComboBeginAndEndDay(int n_day,	int nStartDays ,int nenddays )
{
    m_comboStartDay.ResetContent() ;
	m_comboEndDay.ResetContent()  ;
	for (int i = 1 ;i<= n_day ;i++)
	{	
		CString strText;
		strText.Format("Day %d",i);
		m_comboStartDay.AddString(strText);
		m_comboEndDay.AddString(strText);	
	}
	m_comboStartDay.SetCurSel(nStartDays);	
	m_comboEndDay.SetCurSel(nenddays);
   if(m_checkDaily.GetCheck() == BST_CHECKED)
   {
        m_comboStartDay.EnableWindow(FALSE) ;
        m_comboEndDay.EnableWindow(FALSE) ;
   }else
   {
	   m_comboStartDay.EnableWindow(TRUE) ;
	   m_comboEndDay.EnableWindow(TRUE) ;
   }
}
void CsingleEventsDataDlg::SetDlgData(CString _strProc, ProcessorRoster &_procAssign)
{
	//m_strTitle = _strProc + m_strTitle;
	m_strWindowTitle += _strProc;
	m_procAssign =  _procAssign;
//	if( m_procAssign.getAbsCloseTime().asMinutes() == 24*60 )
//	{
//		m_procAssign.setAbsCloseTime(0l);
//	}
}

bool CsingleEventsDataDlg::GetDlgData(ProcessorRoster &_procAssign)
{
	_procAssign = m_procAssign;
	return true;
}

void CsingleEventsDataDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	int iCurlSelInComb = m_combState.GetCurSel();

	ElapsedTime startTime;
	ElapsedTime endTime;
	COleDateTime timeTmpEnd = m_timeEnd;

	CStartDate flightDate =  GetInputTerminal()->flightSchedule->GetStartDate();
	if(flightDate.IsAbsoluteDate())
	{
		int ndays=0;
		flightDate.GetElapsedTime(true,m_startDate,ndays,m_timeBegin,startTime);
		flightDate.GetElapsedTime(true,m_endDate,ndays,m_timeEnd,endTime);

		//make sure the input time is valid must between the start date and end date
		COleDateTime oleEndDate = GetInputTerminal()->flightSchedule->GetEndDate().GetDate();
//		CString strtemp = m_endDate.Format("%Y:%m:%d");
		CString strenddate = oleEndDate.Format("%Y:%m:%d");
		if (m_endDate > oleEndDate)
		{
			CString strError;
			strError.Format("The Event end time must less than the flight schedule end time (%s)!\r\nPlease input the time again.",strenddate);
			MessageBox(strError,NULL,MB_OK|MB_ICONINFORMATION );
			return;			
		}
	}
	else
	{
		COleDateTime oleTime;
		int nSatrtDay = 0 ;
		int nEndDay =  0 ;
		CString strDay ;
        if (!isDaily)
        {
          nSatrtDay = m_comboStartDay.GetCurSel();
		  nEndDay  = m_comboEndDay.GetCurSel();
        }
		flightDate.GetElapsedTime(false,m_startDate,nSatrtDay,m_timeBegin,startTime);	
		flightDate.GetElapsedTime(false,m_endDate,nEndDay,m_timeEnd,endTime);
	}
	if (startTime > endTime)
	{
		MessageBox("Start time must less than End time!\r\nPlease input the time again.",NULL,MB_OK|MB_ICONINFORMATION );
		return;
	}
/*
	if( m_timeEnd.GetHour()== 0 && 
		m_timeEnd.GetMinute()==0)		//0:00
	{
		timeTmpEnd.SetTime(23,59,59);
	}

	if( m_timeBegin >= timeTmpEnd )
	{
		MessageBox("Start time must less than End time!\r\nPlease input the time again.",NULL,MB_OK|MB_ICONINFORMATION );
		return;
	}
	
	startTime.set( m_timeBegin.GetHour(), m_timeBegin.GetMinute(), m_timeBegin.GetSecond());
	
	if(m_timeEnd.GetHour()== 0 && 
		m_timeEnd.GetMinute()== 0)		//0:00
	{
		endTime.set(24*60*60l);
	}
	else
	{
		endTime.set( m_timeEnd.GetHour(), m_timeEnd.GetMinute(), m_timeEnd.GetSecond());
	}
*/
	//m_procAssign.setFlag(1);
	m_procAssign.setFlag( iCurlSelInComb == 0 ? 1 : 0 );
	m_procAssign.setOpenTime( startTime );
	m_procAssign.setAbsOpenTime( startTime );
	m_procAssign.setCloseTime( endTime );
	m_procAssign.setAbsCloseTime( endTime );
	m_procAssign.setAssRelation((ProcAssRelation) m_iRelationRadio );
//	m_procAssign.setWithTimeConstraint((int)m_bWithTimeConstraint);
// set procesoo daily 
	m_procAssign.IsDaily(isDaily) ;
	CDialog::OnOK();
}

void CsingleEventsDataDlg::OnButDefPaxtype() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	CPassengerTypeDialog dlg( m_pParentWnd );
	if( dlg.DoModal() == IDCANCEL)
		return;

	CMobileElemConstraint m_mobCon = dlg.GetMobileSelection();
	m_procAssign.getAssignment()->setConstraint(m_mobCon,0);
	
	CString szTmp;
	//char szTmp[256];
	m_mobCon.screenPrint(szTmp,0,256);
	m_strPaxType =  szTmp;
	UpdateData( FALSE );
	return;
}

InputTerminal* CsingleEventsDataDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}

void CsingleEventsDataDlg::OnSelchangeComboState() 
{
	// TODO: Add your control notification handler code here
	int iCurlSelInComb = m_combState.GetCurSel();
	if( iCurlSelInComb <0 )
		return;
	
	if( iCurlSelInComb == 1)	//close
	{
		CMobileElemConstraint m_mobCon(GetInputTerminal());
		//m_mobCon.SetInputTerminal( GetInputTerminal() );
		m_procAssign.getAssignment()->setConstraint(m_mobCon,0);
		//char szTmp[256];
		CString szTmp;
		m_mobCon.screenPrint(szTmp,0,256);
		m_strPaxType =  szTmp;
		m_butDefPaxType.EnableWindow( FALSE );

		m_timeBegin.SetTime(0,0,0);
		m_timeEnd.SetTime(0,0,0);
		m_timeBeginCtrl.EnableWindow( FALSE );
		m_timeEndCtrl.EnableWindow( FALSE );

		m_iRelationRadio = 0;
		m_comboStartDay.EnableWindow(FALSE);
		m_comboEndDay.EnableWindow(FALSE);
		GetDlgItem( IDC_RADIO_INCLUSIVE )->EnableWindow( FALSE );
		GetDlgItem( IDC_RADIO_EXCLUSIVE )->EnableWindow( FALSE );
		GetDlgItem( IDC_RADIO_NOT )->EnableWindow( FALSE );
	}
	else
	{
		m_butDefPaxType.EnableWindow();
		m_timeBeginCtrl.EnableWindow(  );
		m_timeEndCtrl.EnableWindow( );
		m_comboStartDay.EnableWindow();
		m_comboEndDay.EnableWindow();

		GetDlgItem( IDC_RADIO_INCLUSIVE )->EnableWindow( TRUE );
		GetDlgItem( IDC_RADIO_EXCLUSIVE )->EnableWindow( TRUE );
		GetDlgItem( IDC_RADIO_NOT )->EnableWindow( TRUE );
	}
	
	UpdateData( FALSE );
}

void CsingleEventsDataDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CsingleEventsDataDlg::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
}

void CsingleEventsDataDlg::OnDtnDatetimechangeDatetimepickerEnddate(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
void CsingleEventsDataDlg::HideComboBoxTime()
{
	m_comboStartDay.ShowWindow(SW_HIDE);
	m_comboEndDay.ShowWindow(SW_HIDE) ;
	CRect rect ;
	m_comboStartDay.GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	m_timeBeginCtrl.MoveWindow(&rect);


	m_comboEndDay.GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;
	m_timeEndCtrl.MoveWindow(&rect) ;
}
void CsingleEventsDataDlg::showComboBoxTime()
{

	m_timeBeginCtrl.MoveWindow(pos_starttimeCtr) ;
	m_timeEndCtrl.MoveWindow(pos_endtimeCtr) ;

	m_timeBeginCtrl.ShowWindow(SW_SHOW) ;
	m_timeEndCtrl.ShowWindow(SW_SHOW) ;
	m_comboStartDay.ShowWindow(SW_SHOW) ;
	m_comboEndDay.ShowWindow(SW_SHOW) ;
}
void CsingleEventsDataDlg::OnBnClickedCheckDaily()
{
	int state = m_checkDaily.GetCheck() ;
	if(state == BST_CHECKED)
	{
		if( AssigmentData() )
		{
		  isDaily = TRUE ;
		  InitComboBeginAndEndDay(1,0,0) ;
		  HideComboBoxTime() ;
		}else
		{
			m_checkDaily.SetCheck(BST_UNCHECKED) ;
			showComboBoxTime() ;
		}
	}
	if(state == BST_UNCHECKED)
	{
		int nMaxDay = 1 ;
		isDaily = FALSE ;
		FlightSchedule* fs = GetInputTerminal()->flightSchedule;
		if (fs->getCount() > 0)
			nMaxDay = fs->GetFlightScheduleEndTime().GetDay();
        InitComboBeginAndEndDay(nMaxDay,0,0);
		showComboBoxTime() ;
	}
}
BOOL CsingleEventsDataDlg::AssigmentData()
{
	if(P_Schedule == NULL)
		return FALSE;
	CString mes ;
	mes.Format(_T("Will convert  event time into the one day, to be continued?"));
	ProcessorRoster* proAss = NULL ;
	if(MessageBox(mes,_T("Warning"),MB_OKCANCEL) == IDOK)
	{
	   for (int i = 0 ; i < P_Schedule->getCount() ; i++)
	   {
		   proAss = P_Schedule->getItem(i) ; 
		  ElapsedTime opentime =  proAss->getOpenTime() ;
		  ElapsedTime closetime = proAss->getCloseTime() ;
          if(opentime >= WholeDay)
			  opentime = opentime - WholeDay ;
		  if(closetime >= WholeDay)
			  closetime = closetime - WholeDay;
		  if(closetime <= opentime)
			  closetime = WholeDay -1 ;
		  proAss->setOpenTime(opentime) ;
		  proAss->setCloseTime(closetime) ;
		  proAss->setAbsOpenTime(opentime) ;
		  proAss->setAbsCloseTime(closetime) ;
	   }
	   return  TRUE ;
	}else
         return FALSE ;
}