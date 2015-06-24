// SimTimeRangeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "SimTimeRangeDlg.h"
#include ".\simtimerangedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimTimeRangeDlg dialog


CSimTimeRangeDlg::CSimTimeRangeDlg( CSimParameter* _pSimParam, CWnd* pParent /*=NULL*/)
	: CDialog(CSimTimeRangeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSimTimeRangeDlg)
	m_dtStart = COleDateTime::GetCurrentTime();
	m_dtInitPeriod = COleDateTime::GetCurrentTime();
	m_dtFollowupPeriod = COleDateTime::GetCurrentTime();
	m_PlusDayValue = 1;
	m_PlusDayValue2 = 1;

	m_dtEnd = COleDateTime::GetCurrentTime();
	//}}AFX_DATA_INIT
//	m_bPlusDay = FALSE;
	m_pSimParam = _pSimParam;
}


void CSimTimeRangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
//	DDX_Control(pDX, IDC_CHECK_PLUS_DAY, m_btnPlusDay);
//	DDX_Check(pDX, IDC_CHECK_PLUS_DAY, m_bPlusDay);

	//{{AFX_DATA_MAP(CSimTimeRangeDlg)
	DDX_Control(pDX, IDC_STATIC_DAY, m_StaticDayCtrl);
	DDX_Control(pDX, IDC_STATIC_DAYSTART, m_StaticDayCtrl2);
	DDX_Control(pDX, IDC_PLUSDAYSPIN, m_PlusDaySpinCtrl);
	DDX_Control(pDX, IDC_PLUSDAYSPIN2, m_PlusDaySpinCtrl2);
	DDX_Control(pDX, IDC_PLUSDAYEDIT, m_PlusDayEditCtrl);
	DDX_Control(pDX, IDC_PLUSDAYEDIT2, m_PlusDayEditCtrl2);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_dtCtrlEnd);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START, m_dtCtrlStart);
	DDX_Control(pDX, IDC_DATETIMEPICKER_FOLLOWUPPERIOD, m_dtCtrlFollowupPeriod);
	DDX_Control(pDX, IDC_DATETIMEPICKER_INITPERIOD, m_dtCtrlInitPeriod);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START, m_dtStart);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_INITPERIOD, m_dtInitPeriod);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_FOLLOWUPPERIOD, m_dtFollowupPeriod);
	DDX_Text(pDX, IDC_PLUSDAYEDIT, m_PlusDayValue);
	DDX_Text(pDX, IDC_PLUSDAYEDIT2, m_PlusDayValue2);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_dtEnd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSimTimeRangeDlg, CDialog)
	//{{AFX_MSG_MAP(CSimTimeRangeDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PLUSDAYSPIN, OnDeltaposPlusdayspin)
	//}}AFX_MSG_MAP
	ON_NOTIFY(UDN_DELTAPOS, IDC_PLUSDAYSPIN2, OnDeltaposPlusdayspin2)
	ON_EN_KILLFOCUS(IDC_PLUSDAYEDIT2, OnEnKillfocusPlusdayedit2)
	ON_EN_KILLFOCUS(IDC_PLUSDAYEDIT, OnEnKillfocusPlusdayedit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimTimeRangeDlg message handlers

void CSimTimeRangeDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();

	//delete by adam 2007-05-09
	//bool bAbsDate;
	//if(m_pSimParam->GetStartDate().IsAbsoluteDate())
	//	bAbsDate = true;
	//else
	//	bAbsDate = false;
	//End delete by adam 2007-05-09
	

	//Modify by adam 2007-05-09
	//COleDateTime oDate;
	//COleDateTime oTime;

//	// start;
//	//oDate = m_dtStart;
//	oDate.SetDate(  m_dtStart.GetYear(), m_dtStart.GetMonth(), m_dtStart.GetDay() );
//	oTime = m_dtStart;
//	oTime.SetTime( m_dtStart.GetHour(), m_dtStart.GetMinute(), m_dtStart.GetSecond() );
//	ElapsedTime eTime;
//
////	CStartDate sDate;
////	sDate.SetDate(oDate.GetYear(), oDate.GetMonth(), oDate.GetDay());
////	m_pSimParam->SetStartDate(sDate);
//	m_pSimParam->GetStartDate().GetElapsedTime( bAbsDate, oDate, m_PlusDayValue2, oTime, eTime );
//	m_pSimParam->SetStartTime( eTime );

	ElapsedTime estTime;
	estTime.SetDay(m_PlusDayValue2);
	estTime.SetHour(m_dtStart.GetHour());
	estTime.SetMinute(m_dtStart.GetMinute());
	estTime.SetSecond(m_dtStart.GetSecond());

	//// start;
	//oDate.SetDate(  m_dtEnd.GetYear(), m_dtEnd.GetMonth(), m_dtEnd.GetDay() );
	//oTime.SetTime( m_dtEnd.GetHour(), m_dtEnd.GetMinute(), m_dtEnd.GetSecond() );

	//m_pSimParam->GetStartDate().GetElapsedTime( bAbsDate, oDate, m_PlusDayValue, oTime, eTime );
	//m_pSimParam->SetEndTime( eTime );

	ElapsedTime estEndTime;
	estEndTime.SetDay(m_PlusDayValue);
	estEndTime.SetHour(m_dtEnd.GetHour());
	estEndTime.SetMinute(m_dtEnd.GetMinute());
	estEndTime.SetSecond(m_dtEnd.GetSecond());

	//if end time is small than start time 
	if (estEndTime < estTime)
	{
		MessageBox( "Start Time / End Time are not valid.","Error", MB_ICONEXCLAMATION | MB_OK );
		return;
	}

	//ElapsedTime estTimeSpan = estEndTime - estTime;
	//if (3 < estTimeSpan.GetDay())
	//{
	//	AfxMessageBox(_T("Can not more than Three day(s)"));
	//	return;
	//}

	m_pSimParam->SetStartTime( estTime );
	m_pSimParam->SetEndTime( estEndTime );
	//End modify by adam 2007-05-09

	ElapsedTime estInitPeriodTime;
	estInitPeriodTime.set( m_dtInitPeriod.GetHour(), m_dtInitPeriod.GetMinute(), 0 );
	m_pSimParam->SetInitPeriod( estInitPeriodTime );

	ElapsedTime estFollowupPeriodTime;
	estFollowupPeriodTime.set( m_dtFollowupPeriod.GetHour(), m_dtFollowupPeriod.GetMinute(), 0 );
	m_pSimParam->SetFollowupPeriod( estFollowupPeriodTime );

	CDialog::OnOK();
}

void CSimTimeRangeDlg::InitControl()
{	
	m_dtCtrlInitPeriod.SetFormat("HH:mm");
	m_dtCtrlFollowupPeriod.SetFormat("HH:mm");

	//modify by adam 2007-05-09
//	if ( m_pSimParam->GetStartDate().IsAbsoluteDate() )
//	{
//		m_dtCtrlStart.SetFormat("yyyy-MM-dd HH:mm:ss");
//		m_dtCtrlEnd.SetFormat("yyyy-MM-dd HH:mm:ss");
//		m_PlusDayValue = 0;
//		m_PlusDayValue2 = 0;
//	}
//	else
//	{
//		m_dtCtrlStart.SetFormat("HH:mm:ss");
//		m_dtCtrlEnd.SetFormat("HH:mm:ss");
////		m_btnPlusDay.EnableWindow();
//		m_PlusDaySpinCtrl.ShowWindow(SW_SHOW);
//		m_PlusDaySpinCtrl2.ShowWindow(SW_SHOW);
//		m_PlusDayEditCtrl.ShowWindow(SW_SHOW);
//		m_PlusDayEditCtrl2.ShowWindow(SW_SHOW);
//		m_StaticDayCtrl.ShowWindow(SW_SHOW);
//		m_StaticDayCtrl2.ShowWindow(SW_SHOW);
//	}

	m_dtCtrlStart.SetFormat("HH:mm:ss");
	m_dtCtrlEnd.SetFormat("HH:mm:ss");
	m_PlusDaySpinCtrl.ShowWindow(SW_SHOW);
	m_PlusDaySpinCtrl2.ShowWindow(SW_SHOW);
	m_PlusDayEditCtrl.ShowWindow(SW_SHOW);
	m_PlusDayEditCtrl2.ShowWindow(SW_SHOW);
	m_StaticDayCtrl.ShowWindow(SW_SHOW);
	m_StaticDayCtrl2.ShowWindow(SW_SHOW);
	//End modify by adam 2007-05-09

	
	UpdateData(FALSE);
}

BOOL CSimTimeRangeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitControl();

	COleDateTime dt = COleDateTime::GetCurrentTime();
	//CStartDate sDate = m_pSimParam->GetStartDate();  //delete by adam 2007-05-09
	ElapsedTime tStart = m_pSimParam->GetStartTime();
	ElapsedTime tEnd = m_pSimParam->GetEndTime();

	//modify by adam 2007-05-09
	//bool bAbsDate;
	//COleDateTime outDate;
	//int nOutIdx;
	//COleDateTime outTime;
	//sDate.GetDateTime( tStart, bAbsDate, outDate, nOutIdx, outTime );
	//if( bAbsDate )
	//{
	//	m_dtStart = outDate;
	//	m_dtStart += COleDateTimeSpan( 0, outTime.GetHour(), outTime.GetMinute(), outTime.GetSecond() );
	//}
	//else
	//{
	//	m_dtStart = outTime;
	//	m_PlusDayValue2 = nOutIdx;
	//}

	m_dtStart.SetTime(tStart.GetHour(), tStart.GetMinute(), tStart.GetSecond());
	m_PlusDayValue2 = tStart.GetDay();
	//End modify by adam 2007-05-09


	//modify by adam 2007-05-09
	//sDate.GetDateTime( tEnd, bAbsDate, outDate, nOutIdx, outTime );
	//if( bAbsDate )
	//{
	//	m_dtEnd = outDate;
	//	m_dtEnd += COleDateTimeSpan( 0, outTime.GetHour(), outTime.GetMinute(), outTime.GetSecond() );
	//}
	//else
	//{
	//	m_PlusDayValue = nOutIdx;
	//	m_dtEnd = outTime;
	//}
	m_dtEnd.SetTime(tEnd.GetHour(), tEnd.GetMinute(), tEnd.GetSecond());
	m_PlusDayValue = tEnd.GetDay();
	//End modify by adam 2007-05-09


	ElapsedTime eTime = m_pSimParam->GetInitPeriod();
	m_dtInitPeriod.SetTime( eTime.asHours(), eTime.asMinutes() % 60, 0);

	eTime = m_pSimParam->GetFollowupPeriod();
	m_dtFollowupPeriod.SetTime( eTime.asHours(), eTime.asMinutes() % 60, 0 );

	//modify by adam 2007-05-09
	//m_PlusDaySpinCtrl.SetRange(0,100);
	//m_PlusDaySpinCtrl2.SetRange(0,100);
	m_PlusDaySpinCtrl.SetRange(1,365);
	m_PlusDaySpinCtrl2.SetRange(1,365);
	//End modify by adam 2007-05-09

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSimTimeRangeDlg::OnDeltaposPlusdayspin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	UpdateData(TRUE);
	int	nLowerBound;
	int	nUpperBound;
	m_PlusDaySpinCtrl.GetRange( nLowerBound, nUpperBound );
	if( pNMUpDown->iDelta > 0 )
	{
		m_PlusDayValue = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_PlusDayValue  = m_PlusDayValue > nUpperBound ? nUpperBound : m_PlusDayValue;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_PlusDayValue = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_PlusDayValue = m_PlusDayValue < nLowerBound ? nLowerBound : m_PlusDayValue;
	}
	UpdateData(FALSE);
	
	*pResult = 0;
}

void CSimTimeRangeDlg::OnDeltaposPlusdayspin2(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	UpdateData(TRUE);
	int	nLowerBound;
	int	nUpperBound;
	m_PlusDaySpinCtrl2.GetRange( nLowerBound, nUpperBound );
	if( pNMUpDown->iDelta > 0 )
	{
		m_PlusDayValue2 = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_PlusDayValue2  = m_PlusDayValue2 > nUpperBound ? nUpperBound : m_PlusDayValue2;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_PlusDayValue2 = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_PlusDayValue2 = m_PlusDayValue2 < nLowerBound ? nLowerBound : m_PlusDayValue2;
	}
	UpdateData(FALSE);

	*pResult = 0;
}

void CSimTimeRangeDlg::OnEnKillfocusPlusdayedit2()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (m_PlusDayValue2<1 || 365<m_PlusDayValue2)
	{
		AfxMessageBox("Day must between 1 and 365", MB_ICONEXCLAMATION | MB_OK);
		GetDlgItem(IDC_PLUSDAYEDIT2)->SetFocus();
	}
}

void CSimTimeRangeDlg::OnEnKillfocusPlusdayedit()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (m_PlusDayValue<1 || 365<m_PlusDayValue)
	{
		AfxMessageBox("Day must between 1 and 365", MB_ICONEXCLAMATION | MB_OK);
		GetDlgItem(IDC_PLUSDAYEDIT)->SetFocus();
	}
}
