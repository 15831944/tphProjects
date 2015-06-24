// PaxBulkEdit.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PaxBulkEdit.h"
#include ".\paxbulkedit.h"
#include "..\Common\elaptime.h"

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
	DDX_Control(pDX, IDC_LIST_DATA, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CPaxBulkEdit, CDialog)
	//{{AFX_MSG_MAP(CPaxBulkEdit)
	ON_BN_CLICKED(IDC_RADIO_after, OnRADIOafter)
	ON_BN_CLICKED(IDC_RADIO_before, OnRADIObefore)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DATA, OnLvnEndlabeleditPercent)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_KILLFOCUS, IDC_m_FrequencyTime, OnNMKillfocusmFrequencytime)
	ON_NOTIFY(NM_KILLFOCUS, IDC_BeginRangeTime, OnNMKillfocusBeginrangetime)
	ON_NOTIFY(NM_KILLFOCUS, IDC_EndRangeTime, OnNMKillfocusEndrangetime)
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

	InitListCtrlHeader();
	SetListCtrlContent();
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

	int iPercent = 0;
	for (int i = 0; i < (int)m_vBulkPercent.size(); i++)
	{
		iPercent += m_vBulkPercent[i];
	}
	if (iPercent != 100)
	{
		AfxMessageBox("The Sum of percent is not equal to 100");
		return;
	}
	CDialog::OnOK();
}

void CPaxBulkEdit::OnRADIOafter() 
{
	// TODO: Add your control notification handler code here
	m_BeforeCheck = false;
	UpdatePercentData();
	((CButton*)GetDlgItem(IDC_RADIO_before))->SetCheck(m_BeforeCheck);
	((CButton*)GetDlgItem(IDC_RADIO_after))->SetCheck(!m_BeforeCheck);
	
}

void CPaxBulkEdit::OnRADIObefore() 
{
	// TODO: Add your control notification handler code here
	m_BeforeCheck = true;
	UpdatePercentData();
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

void CPaxBulkEdit::InitListCtrlHeader()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );
	m_wndListCtrl.SetSpinDisplayType(1);
	CRect rect;
	m_wndListCtrl.GetClientRect(&rect);
	char* columnLabel[] = {	"Train", "Prob Will Try(%)"};
	int DefaultColumnWidth[] = {rect.Width()/5,rect.Width()/2};
	int nColFormat[] = 
	{	
		LVCFMT_NOEDIT,
			LVCFMT_EDIT
	};
	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.csList = &strList;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	for (int i = 0; i < 2; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		m_wndListCtrl.InsertColumn(i, &lvc);
	}
}

void CPaxBulkEdit::SetListCtrlContent()
{
	m_wndListCtrl.DeleteAllItems();
	int nCount = (int)m_vBulkPercent.size();
	for (int i = 0; i < nCount; i++)
	{
		int iPercent = m_vBulkPercent[i];
		CString strTrain;
		strTrain.Format(_T("%d"),i + 1);
		m_wndListCtrl.InsertItem(i,strTrain);
		CString strPercent;
		strPercent.Format(_T("%d"),iPercent);
		m_wndListCtrl.SetItemText(i,1,strPercent);
		m_wndListCtrl.SetItemData(i,(DWORD)iPercent);
	}

	if (m_wndListCtrl.GetItemCount()>0)
	{
		m_wndListCtrl.SetFocus();
		m_wndListCtrl.SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);
	}
}

void CPaxBulkEdit::OnLvnEndlabeleditPercent( NMHDR *pNMHDR, LRESULT *pResult )
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	if(!pDispInfo)
		return;	 
	
	int nItem = pDispInfo->item.iItem;
	if (nItem == LB_ERR)
		return;
	
	int iPercent = atoi(m_wndListCtrl.GetItemText(nItem, 1));
	m_vBulkPercent[nItem] = iPercent;
	*pResult = 0;
}

void CPaxBulkEdit::UpdatePercentData()
{
	UpdateData(TRUE);
	m_wndListCtrl.DeleteAllItems();
	m_vBulkPercent.clear();
	ElapsedTime eTimeRangeBegin;
	ElapsedTime eTimeRangeEnd;
	ElapsedTime eFrequencyTime;
	eTimeRangeBegin.set(m_BeginRangeTime.GetHour(),m_BeginRangeTime.GetMinute(),m_BeginRangeTime.GetSecond());
	eTimeRangeEnd.set(m_EndRangeTime.GetHour(),m_EndRangeTime.GetMinute(),m_EndRangeTime.GetSecond());
	eFrequencyTime.set(m_FrequencyTime.GetHour(),m_FrequencyTime.GetMinute(),m_FrequencyTime.GetSecond());

	if (eFrequencyTime.asMinutes() == 0)
		return;
	
	if (eTimeRangeBegin < eTimeRangeEnd)
	{
		std::swap(eTimeRangeBegin,eTimeRangeEnd);
	}
	
	int nBulkCount= ( eTimeRangeBegin.asMinutes() - eTimeRangeEnd.asMinutes() )/eFrequencyTime.asMinutes() +1;
	for (int i = 0; i < nBulkCount; i++)
	{
		int iPercent;
		if (i == 0)
		{
			iPercent = 100;
		}
		else
		{
			iPercent = 0;
		}

		CString strTrain;
		strTrain.Format(_T("%d"),i + 1);
		m_wndListCtrl.InsertItem(i,strTrain);
		CString strPercent;
		strPercent.Format(_T("%d"),iPercent);
		m_wndListCtrl.SetItemText(i,1,strPercent);
		m_wndListCtrl.SetItemData(i,(DWORD)iPercent);
		m_vBulkPercent.push_back(iPercent);
	}
}



void CPaxBulkEdit::OnNMKillfocusmFrequencytime(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	UpdatePercentData();
	*pResult = 0;
}

void CPaxBulkEdit::OnNMKillfocusBeginrangetime(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	UpdatePercentData();
	*pResult = 0;
}

void CPaxBulkEdit::OnNMKillfocusEndrangetime(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	UpdatePercentData();
	*pResult = 0;
}
