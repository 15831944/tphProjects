// DlgTimeRange.cpp : implementation file
//
#include "stdafx.h"
#include "resource.h"
#include "DlgLaneRange.h"


CDlgLaneRange::CDlgLaneRange(int nstartLane, int nendLane, CWnd* pParent /*=NULL*/)
: CDialog(CDlgLaneRange::IDD, pParent)
, m_nStartLane(nstartLane)
, m_nEndLane(nendLane)
{
	m_bAll = FALSE;
}

CDlgLaneRange::~CDlgLaneRange()
{
}

void CDlgLaneRange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Text(pDX, IDC_EDIT_STARTLANE, m_nStartLane);
	DDV_MinMaxInt(pDX, m_nStartLane, 1, 100);
	DDX_Text(pDX, IDC_EDIT_ENDLANE, m_nEndLane);
	DDV_MinMaxInt(pDX, m_nEndLane, 1, 100);
	DDX_Control(pDX, IDC_SPIN_STARTLANE, m_spinStartLane);
	DDX_Control(pDX, IDC_SPIN_ENDLANE, m_spinEndLane);
	DDX_Check(pDX, IDC_CHECK_ALL, m_bAll);
}

BEGIN_MESSAGE_MAP(CDlgLaneRange, CDialog)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_STARTLANE, OnDeltaposSpinStartLane)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ENDLANE, OnDeltaposSpinEndLane)
	ON_BN_CLICKED(IDC_CHECK_ALL, OnCheckAlll)
END_MESSAGE_MAP()


BOOL CDlgLaneRange::OnInitDialog()
{
	CDialog::OnInitDialog();

	//------------------------------------------------------------
	// 
	CPoint pt;
	::GetCursorPos(&pt);
	CRect rc;
	GetWindowRect(rc);

	rc.MoveToXY(pt.x - rc.Width()/2, pt.y - rc.Height()/2);
	MoveWindow(rc);

	m_spinStartLane.SetRange32(1,100);
	m_spinEndLane.SetRange32(1,100);
	
	if(m_bAll)
	{
		m_spinStartLane.EnableWindow(FALSE);
		m_spinEndLane.EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STARTLANE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_ENDLANE)->EnableWindow(FALSE);
	}

	UpdateData(FALSE);

	return TRUE;
}


void CDlgLaneRange::OnOK()
{
	UpdateDataEx();

	//Start <= End
	if(m_nStartLane > m_nEndLane )
	{
		MessageBox(_T("The start lane can not large than the end lane, please."));
		return;
	}

	CDialog::OnOK();
}

void CDlgLaneRange::OnCancel()
{
	UpdateDataEx();
	CDialog::OnCancel();
}



void CDlgLaneRange::UpdateDataEx()
{
	UpdateData();
}


void CDlgLaneRange::OnDeltaposSpinStartLane(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	

	*pResult = 0;
}

void CDlgLaneRange::OnDeltaposSpinEndLane(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	*pResult = 0;
}

int CDlgLaneRange::GetStartLane()
{
	return m_nStartLane;
}

int CDlgLaneRange::GetEndLane()
{
	return m_nEndLane;
}

void CDlgLaneRange::OnCheckAlll()
{
	if (IsDlgButtonChecked(IDC_CHECK_ALL))
	{
		m_spinStartLane.EnableWindow(FALSE);
		m_spinEndLane.EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STARTLANE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_ENDLANE)->EnableWindow(FALSE);
		m_bAll = TRUE;
	}
	else
	{
		m_spinStartLane.EnableWindow(TRUE);
		m_spinEndLane.EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STARTLANE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_ENDLANE)->EnableWindow(TRUE);
		m_bAll = FALSE;
	}
}

BOOL CDlgLaneRange::IsAll()
{
	return m_bAll;
}
