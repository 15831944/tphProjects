// DlgTimeRange.cpp : implementation file
//
#include "stdafx.h"
#include "resource.h"
#include "DlgTimeRange.h"
#include ".\dlgtimerange.h"

CDlgTimeRange::CDlgTimeRange(const ElapsedTime& startTime, const ElapsedTime& endTime, BOOL bDaily, CWnd* pParent /*=NULL*/)
: CDialog(CDlgTimeRange::IDD, pParent)
 , m_nStartDay(1)
 , m_nEndDay(1)
 , m_bDaily(bDaily)
{
	ConvertElapsedTime(m_nStartDay, m_startTime, startTime);
	ConvertElapsedTime(m_nEndDay, m_endTime, endTime);

	if (startTime == 0l
		&& endTime == 0l)
	{
		m_endTime.SetTime(23, 59, 59);
	}
}

CDlgTimeRange::~CDlgTimeRange()
{
}

void CDlgTimeRange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_COMBO_STARTDAY, m_wndCmbStartDay);
	//DDX_Control(pDX, IDC_COMBO_ENDDAY, m_wndCmbEndDay);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_STARTTIME, m_startTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_ENDTIME, m_endTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER_STARTTIME, m_StarTimeCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER_ENDTIME, m_EndTimeCtrl);

	DDX_Text(pDX, IDC_EDIT_STARTDAY, m_nStartDay);
	DDV_MinMaxInt(pDX, m_nStartDay, 1, 366);
	DDX_Text(pDX, IDC_EDIT_ENDDAY, m_nEndDay);
	DDV_MinMaxInt(pDX, m_nEndDay, 1, 366);
	DDX_Control(pDX, IDC_SPIN_STARTDAY, m_spinStartDay);
	DDX_Control(pDX, IDC_SPIN_ENDDAY, m_spinEndDay);
}

BEGIN_MESSAGE_MAP(CDlgTimeRange, CDialog)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_STARTDAY, OnDeltaposSpinStartday)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ENDDAY, OnDeltaposSpinEndday)
END_MESSAGE_MAP()


BOOL CDlgTimeRange::OnInitDialog()
{
	CDialog::OnInitDialog();

	//------------------------------------------------------------
	// 
	CPoint pt;
	::GetCursorPos(&pt);
	CRect rc;
	GetWindowRect(rc);

	rc.MoveToXY(pt.x - rc.Width()/2, pt.y - rc.Height()/2);
// 	ScreenToClient(rc);

	MoveWindow(rc);
	//------------------------------------------------------------


	//m_wndCmbStartDay.AddString(_T("Day 1"));
	//m_wndCmbStartDay.AddString(_T("Day 2"));
	//m_wndCmbStartDay.AddString(_T("Day 3"));
	//m_wndCmbStartDay.SetCurSel(m_nStartDay);

	//m_wndCmbEndDay.AddString(_T("Day 1"));
	//m_wndCmbEndDay.AddString(_T("Day 2"));
	//m_wndCmbEndDay.AddString(_T("Day 3"));
	//m_wndCmbEndDay.SetCurSel(m_nEndDay);

	m_spinStartDay.SetRange32(1,366);
	m_spinEndDay.SetRange32(1,366);
	m_StarTimeCtrl.SetFormat("HH:mm:ss");
	m_EndTimeCtrl.SetFormat("HH:mm:ss");
	if(m_bDaily)
	{
		m_spinStartDay.EnableWindow(FALSE);
		m_spinEndDay.EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STARTDAY)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_ENDDAY)->EnableWindow(FALSE);
	}

	UpdateData(FALSE);
	
	return TRUE;
}


void CDlgTimeRange::OnOK()
{
	UpdateDataEx();

	//Start <= End
	if(GetStartTime() >= GetEndTime() )
	{
		MessageBox(_T("Make sure there has a time range,please."));
		return;
	}

	CDialog::OnOK();
}

void CDlgTimeRange::OnCancel()
{
	UpdateDataEx();
	CDialog::OnCancel();
}

void CDlgTimeRange::ConvertElapsedTime(int& nDay, COleDateTime& time, const ElapsedTime& elapsedTime)
{
	ElapsedTime estTime = elapsedTime;
	nDay = estTime.GetDay();
	int nHours = estTime.GetHour();
	int nMinutes = estTime.GetMinute();
	int nSeconds = estTime.GetSecond();	

	time.SetTime(nHours, nMinutes, nSeconds);
}

void CDlgTimeRange::UpdateDataEx()
{
	UpdateData();
	//m_nStartDay = m_wndCmbStartDay.GetCurSel();
	//m_nEndDay = m_wndCmbEndDay.GetCurSel();
}

ElapsedTime CDlgTimeRange::GetStartTime() 
{
	ElapsedTime estStartTime;
	estStartTime.SetDay(m_nStartDay);
	estStartTime.SetHour(m_startTime.GetHour());
	estStartTime.SetMinute(m_startTime.GetMinute());
	estStartTime.SetSecond(m_startTime.GetSecond());

	return estStartTime;
}

ElapsedTime CDlgTimeRange::GetEndTime() 
{
	ElapsedTime estEndTime;
	estEndTime.SetDay(m_nEndDay);
	estEndTime.SetHour(m_endTime.GetHour());
	estEndTime.SetMinute(m_endTime.GetMinute());
	estEndTime.SetSecond(m_endTime.GetSecond());

	return estEndTime;
}

CString CDlgTimeRange::GetStartTimeString()
{
	CString strStartTime;
	strStartTime.Format("Day%d %02d:%02d:%02d", m_nStartDay, 
		m_startTime.GetHour(),
		m_startTime.GetMinute(),
		m_startTime.GetSecond());
	return strStartTime;
}

CString CDlgTimeRange::GetEndTimeString()
{
	CString strEndTime;
	strEndTime.Format("Day%d %02d:%02d:%02d", m_nEndDay, 
		m_endTime.GetHour(),
		m_endTime.GetMinute(),
		m_endTime.GetSecond());
	return strEndTime;
}
void CDlgTimeRange::OnDeltaposSpinStartday(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here

	//CString strDay;
	//m_editDayOne.GetWindowText(strDay);
	//int nDay = atoi(strDay);

	////up
	//if( pNMUpDown->iDelta > 0 )
	//{
	//	if (nDay < 365)
	//	{
	//		nDay++;
	//		m_estArrTime.SetDay(nDay);
	//	}		
	//}
	////down
	//else
	//{		
	//	if (1 < nDay)
	//	{
	//		nDay--;
	//		m_estArrTime.SetDay(nDay);
	//	}
	//}

	*pResult = 0;
}

void CDlgTimeRange::OnDeltaposSpinEndday(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;
}
