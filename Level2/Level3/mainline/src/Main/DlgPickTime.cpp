// DlgPickTime.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgPickTime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MINTIME	0
#define MAXTIME	8640000 //24*3600*100

#define TIMETOHOURDIV	360000
#define TIMETOMINDIV		6000
#define TIMETOSECDIV		100

static LPCTSTR s_lpstrDayDesc[] = {
	_T("Day 1"), _T("Day 2"), _T("Day 3"), _T("Day 4"), _T("Day 5")
};

/////////////////////////////////////////////////////////////////////////////
// CDlgPickTime dialog
static COleDateTime GetOleDateTime(long _nTime)
{
	COleDateTime ret = COleDateTime::GetCurrentTime();
	int nHour = _nTime/TIMETOHOURDIV;
	int nMin = (_nTime-nHour*TIMETOHOURDIV)/TIMETOMINDIV;
	int nSec = (_nTime-nHour*TIMETOHOURDIV-nMin*TIMETOMINDIV)/TIMETOSECDIV;
	ret.SetTime(nHour,nMin,nSec);
	return ret;
}

CDlgPickTime::CDlgPickTime(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPickTime::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPickTime)
	m_dtAnimPickTime = COleDateTime::GetCurrentTime();
	m_dtAnimPickDate = COleDateTime::GetCurrentTime();
	m_nDayIndex = -1;
	//}}AFX_DATA_INIT
	//m_dtAnimPickTime = GetOleDateTime(_nTime);
	//long nTime = m_dtAnimPickTime.GetHour()*TIMETOHOURDIV+m_dtAnimPickTime.GetMinute()*TIMETOMINDIV+m_dtAnimPickTime.GetSecond()*TIMETOSECDIV;

	m_tc = TC_RELATIVE;

}


void CDlgPickTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPickTime)
	DDX_Control(pDX, IDC_ANIMDATE, m_dtCtrlAnimDate);
	DDX_Control(pDX, IDC_ANIMDATEINDEX, m_cmbAnimDateIndex);
	DDX_DateTimeCtrl(pDX, IDC_DTANIMTIME, m_dtAnimPickTime);
	DDX_DateTimeCtrl(pDX, IDC_ANIMDATE, m_dtAnimPickDate);
	DDX_CBIndex(pDX, IDC_ANIMDATEINDEX, m_nDayIndex);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPickTime, CDialog)
	//{{AFX_MSG_MAP(CDlgPickTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPickTime message handlers

void CDlgPickTime::OnOK() 
{
	UpdateData(TRUE);
	m_nTime = m_dtAnimPickTime.GetHour()*TIMETOHOURDIV+m_dtAnimPickTime.GetMinute()*TIMETOMINDIV+m_dtAnimPickTime.GetSecond()*TIMETOSECDIV;
	
	CDialog::OnOK();
}


void CDlgPickTime::SetDateTime(const CStartDate& _sDate, AnimationData& animData)
{
	m_startDate = _sDate;
	bool bAbsDate;
	COleDateTime date;
	int nDtIdx;
	COleDateTime time;
	ElapsedTime _eTime;
	_eTime=animData.nLastAnimTime/100;
	m_startDate.GetDateTime( _eTime, bAbsDate, date, nDtIdx, time );		
	if( bAbsDate )
	{
		m_dtAnimPickDate = date;
		m_tc = TC_ABSOLUTE;
	}	
	else
	{
		m_nDayIndex = nDtIdx;
		m_tc = TC_RELATIVE;
	}
	m_dtAnimPickTime = time;
    m_EndTime=animData.nLastExitTime;
}


void CDlgPickTime::InitControl()
{
	if (m_tc == TC_ABSOLUTE)
	{
		m_cmbAnimDateIndex.ShowWindow(SW_HIDE);
		m_dtCtrlAnimDate.ShowWindow(SW_SHOW);
	}
	else
	{
		ElapsedTime estMaxDayTime( m_EndTime/TimePrecision );
		int nMaxDay = estMaxDayTime.GetDay();
		for (int i=0; i<nMaxDay; i++)
		{
			CString strDay;
			strDay.Format("Day %d", i+1);

			m_cmbAnimDateIndex.AddString(strDay);
			
		}
		CRect rc;

		/*for (int i = 0; i < sizeof(s_lpstrDayDesc) / sizeof(s_lpstrDayDesc[0]); i++)
			m_cmbAnimDateIndex.AddString(s_lpstrDayDesc[i]);*/
		
		m_dtCtrlAnimDate.GetWindowRect(rc);
		this->ScreenToClient(rc);
		m_cmbAnimDateIndex.SetWindowPos(NULL, rc.left, rc.top, 0, 0, SWP_NOSIZE);
		m_dtCtrlAnimDate.ShowWindow(SW_HIDE);
		m_cmbAnimDateIndex.ShowWindow(SW_SHOW);
	}
}

BOOL CDlgPickTime::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitControl();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPickTime::GetDateTime(COleDateTime &_dtDateTime)
{
	ASSERT(m_tc == TC_ABSOLUTE);
	_dtDateTime.SetDateTime(m_dtAnimPickDate.GetYear(), m_dtAnimPickDate.GetMonth(), m_dtAnimPickDate.GetDay(),
		m_dtAnimPickTime.GetHour(), m_dtAnimPickTime.GetMinute(), m_dtAnimPickTime.GetSecond());
}



void CDlgPickTime::GetDateTime(CStartDate &_sDate, ElapsedTime &_eTime)
{
	ASSERT(m_tc == TC_ABSOLUTE);
	_sDate.SetDate( m_dtAnimPickDate );

	_eTime.set(m_dtAnimPickTime.GetHour(), m_dtAnimPickTime.GetMinute(), m_dtAnimPickTime.GetSecond());
}


void CDlgPickTime::GetDateTime(ElapsedTime &_eTime, int &_nDayIndex)
{
	ASSERT(m_tc == TC_RELATIVE);
	_eTime.set(m_dtAnimPickTime.GetHour(), m_dtAnimPickTime.GetMinute(), m_dtAnimPickTime.GetSecond());
	_nDayIndex = m_nDayIndex;
}

Time_Category& CDlgPickTime::GetTimeCategory()
{
	return m_tc;
}

ElapsedTime CDlgPickTime::GetTime()
{
	ElapsedTime et;
	if (m_tc == TC_ABSOLUTE)
	{
		COleDateTime dt, dtStart;
		GetDateTime(dt);
		dtStart = m_startDate.GetDate();
		COleDateTimeSpan dtSpan = dt - dtStart;
		et.set(static_cast<long>(dtSpan.GetTotalSeconds()));
	}
	else	//	TC_RELATIVE
	{
		int nDayIndex;
		GetDateTime(et, nDayIndex);
		et += (long)(nDayIndex * 3600 * 24);
	}

	return et;
}