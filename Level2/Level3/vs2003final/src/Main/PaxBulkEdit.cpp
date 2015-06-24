// PaxBulkEdit.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PaxBulkEdit.h"
#include ".\paxbulkedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaxBulkEdit dialog


CPaxBulkEdit::CPaxBulkEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CPaxBulkEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPaxBulkEdit)
	m_BeforeCheck = FALSE;
	m_Capacity = 50;
	//}}AFX_DATA_INIT
}


void CPaxBulkEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaxBulkEdit)
	DDX_Control(pDX, IDC_m_FrequencyTime, m_frequencyCtrl);
	DDX_Control(pDX, IDC_StartTime, m_startTimeCtrl);
	DDX_Control(pDX, IDC_EndTime, m_endtimeCtrl);
	DDX_Control(pDX, IDC_EndRangeTime, m_endRangeCtrl);
	DDX_Control(pDX, IDC_BeginRangeTime, m_beginrangeCtrl);
	DDX_Control(pDX, IDC_SPIN1, m_Spin);
	DDX_DateTimeCtrl(pDX, IDC_BeginRangeTime, m_BeginRangeTime);
	DDX_Text(pDX, IDC_CAPACITY, m_Capacity);
	DDX_DateTimeCtrl(pDX, IDC_EndRangeTime, m_EndRangeTime);
	DDX_DateTimeCtrl(pDX, IDC_EndTime, m_FreqEndTime);
	DDX_DateTimeCtrl(pDX, IDC_m_FrequencyTime, m_FrequencyTime);
	DDX_DateTimeCtrl(pDX, IDC_StartTime, m_FreqStartTime);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDIT_FREQ_STARTDAY, m_editFreqStartDay);
	DDX_Control(pDX, IDC_EDIT_FREQ_ENDDAY, m_editFreqEndDay);
	DDX_Control(pDX, IDC_SPIN_FREQ_STARTDAY, m_spinFreqStartDay);
	DDX_Control(pDX, IDC_SPIN_FREQ_ENDDAY, m_spinFreqEndDay);
}


BEGIN_MESSAGE_MAP(CPaxBulkEdit, CDialog)
	//{{AFX_MSG_MAP(CPaxBulkEdit)
	ON_BN_CLICKED(IDC_RADIO_after, OnRADIOafter)
	ON_BN_CLICKED(IDC_RADIO_before, OnRADIObefore)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaxBulkEdit message handlers
BOOL CPaxBulkEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

	int dayFreqStart = 1;
	int dayFreqEnd = 1;
	InitTimeValues(dayFreqStart, dayFreqEnd);

	UpdateData(false);

	// Day
	m_spinFreqStartDay.SetBuddy(&m_editFreqStartDay);
	m_spinFreqStartDay.SetRange32(1, 366);
	m_spinFreqStartDay.SetPos32(dayFreqStart);

	m_spinFreqEndDay.SetBuddy(&m_editFreqEndDay);
	m_spinFreqEndDay.SetRange32(1, 366);
	m_spinFreqEndDay.SetPos32(dayFreqEnd);


	m_Spin.SetBuddy(GetDlgItem(IDC_CAPACITY));
	m_Spin.SetRange((short)0,(short)1000000);
	m_beginrangeCtrl.SetFormat("HH:mm:ss");
	m_endRangeCtrl.SetFormat("HH:mm:ss");
	m_startTimeCtrl.SetFormat("HH:mm:ss");
	m_endtimeCtrl.SetFormat("HH:mm:ss");
	m_frequencyCtrl.SetFormat("HH:mm:ss");
	CButton *pRadioAfter = (CButton *)GetDlgItem(IDC_RADIO_after);
	CButton *pRadioBefore = (CButton *)GetDlgItem(IDC_RADIO_before);

	if(!m_BeforeCheck)
	{
       pRadioBefore->SetCheck(0);
	   pRadioAfter->SetCheck(1);
	}
	else
	{
		pRadioBefore->SetCheck(1);
		pRadioAfter->SetCheck(0);
	}


	UpdateData(true);
	return true;
}
void CPaxBulkEdit::OnOK() 
{
	// TODO: Add extra validation here
    UpdateData(true);

	m_freqStartTime.m_dt   = m_FreqStartTime.m_dt   +   m_spinFreqStartDay.GetPos32() - 1;
	m_freqEndTime.m_dt     = m_FreqEndTime.m_dt     +   m_spinFreqEndDay.GetPos32() - 1;



	CButton *pRadioAfter = (CButton *)GetDlgItem(IDC_RADIO_after);
	CButton *pRadioBefore = (CButton *)GetDlgItem(IDC_RADIO_before);
	if(pRadioAfter->GetCheck()) m_BeforeCheck = false;
	else if(pRadioBefore->GetCheck())	m_BeforeCheck = true;

	if(m_freqEndTime < m_freqStartTime)
	{
		AfxMessageBox("DataInvalue: EndTime < startTime");
		return;
	}

	if(m_BeforeCheck)
	{
		if(m_EndRangeTime > m_BeginRangeTime) 
		{
			AfxMessageBox("DataInvalue: EndRangeTime < BeginRangeTime");
		    return;
		}	
	}
	else
	{
		if(m_EndRangeTime < m_BeginRangeTime)
		{ 
			AfxMessageBox("DataInvalue: EndRangeTime < BeginRangeTime");
			return;
		}
	}

	CDialog::OnOK();
}

void CPaxBulkEdit::OnRADIOafter() 
{
	// TODO: Add your control notification handler code here
	m_BeforeCheck = false;
	((CButton*)GetDlgItem(IDC_RADIO_before))->SetCheck(m_BeforeCheck);
	((CButton*)GetDlgItem(IDC_RADIO_after))->SetCheck(!m_BeforeCheck);
	
}

void CPaxBulkEdit::OnRADIObefore() 
{
	// TODO: Add your control notification handler code here
	m_BeforeCheck = true;
	((CButton*)GetDlgItem(IDC_RADIO_before))->SetCheck(m_BeforeCheck);
	((CButton*)GetDlgItem(IDC_RADIO_after))->SetCheck(!m_BeforeCheck);
}

void CPaxBulkEdit::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	int	nLowerBound;
	int	nUpperBound;
	
	pNMUpDown->iPos= m_Capacity;
	m_Spin.GetRange( nLowerBound, nUpperBound );
	if( pNMUpDown->iDelta > 0 )
	{
		m_Capacity = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_Capacity = m_Capacity > nUpperBound ? nUpperBound : m_Capacity;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_Capacity = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_Capacity = m_Capacity < nLowerBound ? nLowerBound : m_Capacity;
	}	
    UpdateData(false);
	*pResult = 0;
}

void CPaxBulkEdit::InitTimeValues(int& dayFreqStart, int& dayFreqEnd)
{
	// do not pass in invalid time value
	ASSERT(COleDateTime::valid == m_freqStartTime.m_status
		&& COleDateTime::valid == m_freqEndTime.m_status);

	if (0.0 == m_freqStartTime.m_dt && 0.0 == m_freqEndTime.m_dt) // whether modified
	{
		// if not
		m_freqStartTime.m_dt = 0.0;
		m_freqEndTime.m_dt = 1.0 - 1.0/24/60;
	} 

	dayFreqStart = (int)(m_freqStartTime.m_dt);
	m_FreqStartTime.m_dt = m_freqStartTime.m_dt - dayFreqStart;
	dayFreqEnd = (int)(m_freqEndTime.m_dt);
	m_FreqEndTime.m_dt = m_freqEndTime.m_dt - dayFreqEnd;

	dayFreqStart++;
	dayFreqEnd++;
}
