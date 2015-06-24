// PaxBulkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PaxBulkDlg.h"
#include "inputs\PaxBulkInfo.h"
#include "TermPlanDoc.h"
#include "PassengerTypeDialog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CPaxBulkDlg dialog


CPaxBulkDlg::CPaxBulkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPaxBulkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPaxBulkDlg)
	m_nCapacity =50;
	m_strPaxType =_T("");	
	m_timeEnd = COleDateTime::GetCurrentTime();
	m_timeStart = COleDateTime::GetCurrentTime();
	m_timeRangeB = COleDateTime::GetCurrentTime();
	m_timeRangeE = COleDateTime::GetCurrentTime();
	m_timeFrq = COleDateTime::GetCurrentTime();
	//}}AFX_DATA_INIT
	m_bTypeChanged = FALSE;
	m_bAdd = FALSE;
}

void CPaxBulkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaxBulkDlg)
	DDX_Control(pDX, IDC_DATETIMEPICKERFRQ, m_timeFrqctrl);
	DDX_Control(pDX, IDC_DATETIMEPICKERRANGE, m_timeRangeEctrl);
	DDX_Control(pDX, IDC_DATETIMEPICKERRANGEB, m_timeRangeBctrl);
	DDX_Control(pDX, IDC_DATETIMEPICKERSTART, m_StarttimeCtr);
	DDX_Control(pDX, IDC_DATETIMEPICKEREND, m_EndtimeCtr);
	DDX_Control(pDX, IDC_SPIN_CAPACITY, m_spinCapacity);
	DDX_Text(pDX, IDC_CAPACITY, m_nCapacity);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKEREND, m_timeEnd);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKERSTART, m_timeStart);
	DDX_Text(pDX, IDC_EDIT_PAXTYPE, m_strPaxType);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKERRANGEB, m_timeRangeB);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKERRANGE, m_timeRangeE);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKERFRQ, m_timeFrq);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPaxBulkDlg, CDialog)
	//{{AFX_MSG_MAP(CPaxBulkDlg)
	ON_BN_CLICKED(IDC_BTSELPAXTYPE, OnBtselpaxtype)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaxBulkDlg message handlers

BOOL CPaxBulkDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_timeFrqctrl.SetFormat("HH:mm" );
	m_StarttimeCtr.SetFormat( "HH:mm" );
	m_EndtimeCtr.SetFormat( "HH:mm" );
	m_timeRangeEctrl.SetFormat( "HH:mm" );
	m_timeRangeBctrl.SetFormat( "HH:mm" );	

	if (m_paxBulk.m_nCapacity >= 0 )
	{		
		//char sPassengerTypeName[256]; 
		CString sPassengerTypeName;
		m_paxBulk.m_mobileElemConst.screenPrint(sPassengerTypeName,0,256);
		m_strPaxType = sPassengerTypeName ;
		m_nCapacity = m_paxBulk.m_nCapacity;

		CString str = m_paxBulk.m_TimeFrequency.printTime(FALSE);
		int nHH,nMM;
		sscanf(str,"%d:%d",&nHH,&nMM);
		m_timeFrq.SetTime( nHH,nMM,0);

		str = m_paxBulk.m_TimeStart.printTime(FALSE);
		sscanf(str,"%d:%d",&nHH,&nMM);
		m_timeStart.SetTime( nHH,nMM,0);

		str = m_paxBulk.m_TimeEnd.printTime(FALSE);
		sscanf(str,"%d:%d",&nHH,&nMM);
		m_timeEnd.SetTime( nHH,nMM,0);

		str = m_paxBulk.m_TimeRangeBegin.printTime(FALSE);
		sscanf(str,"%d:%d",&nHH,&nMM);
		m_timeRangeB.SetTime( nHH,nMM,0);

		str = m_paxBulk.m_TimeRangeEnd.printTime(FALSE);
		sscanf(str,"%d:%d",&nHH,&nMM);
		m_timeRangeE.SetTime( nHH,nMM,0);		

//		m_timeStart.SetTime( m_paxBulk.m_TimeStart.asHours(),m_paxBulk.m_TimeStart.asMinutes(),m_paxBulk.m_TimeStart.asSeconds());
//		m_timeEnd.SetTime( m_paxBulk.m_TimeEnd.asHours(),m_paxBulk.m_TimeEnd.asMinutes(),m_paxBulk.m_TimeEnd.asSeconds());
//		m_timeRangeB.SetTime( m_paxBulk.m_TimeRangeBegin.asHours(),m_paxBulk.m_TimeRangeEnd.asMinutes(),m_paxBulk.m_TimeEnd.asSeconds());
//		m_timeRangeE.SetTime( m_paxBulk.m_TimeRangeEnd.asHours(),m_paxBulk.m_TimeRangeEnd.asMinutes(),m_paxBulk.m_TimeRangeEnd.asSeconds());
	}
	else
	{
		m_strPaxType ="";
		m_nCapacity =50;
		m_timeFrq.SetTime( 1,0,0);;
		m_timeStart.SetTime( 5,0,0);
		m_timeEnd.SetTime( 23,0,0);
		m_timeRangeB.SetTime( 1,0,0);
		m_timeRangeE.SetTime( 16,0,0);
		m_paxBulk.m_mobileElemConst.makeNone();
	}	
	
	m_spinCapacity.SetRange(1,1000);
	m_spinCapacity.SetBuddy(GetDlgItem(IDC_CAPACITY) ) ;

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPaxBulkDlg::OnOK() 
{
	// TODO: Add extra validation here
	if (m_bTypeChanged)
	{
		m_paxBulk.m_mobileElemConst = m_PaxType;
	}
	
	if ( m_bAdd && !m_bTypeChanged)
	{
		MessageBox( "no Passenger Type!","Error",MB_ICONERROR);
		return ;
	}	

	UpdateData();
	if (m_timeFrq.GetHour() == 0 && m_timeFrq.GetMinute() <= 0)
	{
		MessageBox( "Frequence Less One Minute!","Error",MB_ICONERROR);
		return ;		
	}
	if (m_timeStart >= m_timeEnd)
	{
		MessageBox( "Start Time as same as End time","Error",MB_ICONERROR);
		return ;				
	}
	if (m_timeRangeB >= m_timeRangeE)
	{
		MessageBox( "Time Range Error","Error",MB_ICONERROR);
		return ;						
	}
	if (m_timeRangeB>= m_timeEnd - m_timeStart)
	{
		MessageBox( "Time Range Error","Error",MB_ICONERROR);
		return ;								
	}
	if (m_timeRangeE>= m_timeEnd - m_timeStart)
	{
		MessageBox( "Time Range Error","Error",MB_ICONERROR);
		return ;								
	}

	m_paxBulk.m_nCapacity = m_nCapacity;
	m_paxBulk.m_TimeFrequency.set(m_timeFrq.GetHour(),m_timeFrq.GetMinute(),0);
	m_paxBulk.m_TimeStart.set(m_timeStart.GetHour(),m_timeStart.GetMinute(),0);
	m_paxBulk.m_TimeEnd.set( m_timeEnd.GetHour(),m_timeEnd.GetMinute(),0 );
	m_paxBulk.m_TimeRangeBegin.set( m_timeRangeB.GetHour(), m_timeRangeB.GetMinute(),0);
	m_paxBulk.m_TimeRangeEnd.set( m_timeRangeE.GetHour(), m_timeRangeE.GetMinute(),0);
	CDialog::OnOK();
	
}

void CPaxBulkDlg::OnBtselpaxtype() 
{

	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	
	CPassengerTypeDialog dlg(m_pParentWnd);
	if( dlg.DoModal() == IDOK )
	{
		CMobileElemConstraint mobileConstr ( dlg.GetMobileSelection() );
		m_PaxType= dlg.GetMobileSelection() ;
		CString sPassengerTypeName;//[256]; 
	//	mobileConstr.printConstraint(sPassengerTypeName);
		mobileConstr.screenPrint(sPassengerTypeName,0,256);
		m_strPaxType = sPassengerTypeName ;
		m_bTypeChanged = TRUE;
		UpdateData(FALSE);
	}	
}
