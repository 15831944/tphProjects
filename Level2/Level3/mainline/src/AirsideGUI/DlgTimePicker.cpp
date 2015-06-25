// DlgTimePicker.cpp : implementation file
//

#include "stdafx.h"
#include "DlgTimePicker.h"
#include ".\dlgtimepicker.h"
#include "resource.h"

// CDlgTimePicker dialog

IMPLEMENT_DYNAMIC(CDlgTimePicker, CDialog)
CDlgTimePicker::CDlgTimePicker(ElapsedTime eTime, CString strCaption/* = _T("")*/, CWnd* pParent /*=NULL*/, bool bSecFormat/* = true*/)
	: CDialog(IDD_DIALOG_TIME_PICKER, pParent)
	, m_time(eTime)
	,m_bSecFormat(bSecFormat)
	, m_strCaption(strCaption)
{
}

CDlgTimePicker::~CDlgTimePicker()
{
}

void CDlgTimePicker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TIME, m_timePicker);
}


BEGIN_MESSAGE_MAP(CDlgTimePicker, CDialog)
END_MESSAGE_MAP()


// CDlgTimePicker message handlers

void CDlgTimePicker::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	COleDateTime dateTime;
	m_timePicker.GetTime(dateTime);
	m_time.set(dateTime);
	CDialog::OnOK();
}

BOOL CDlgTimePicker::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText(m_strCaption.IsEmpty() ? _T("Pick Time") : m_strCaption);

	if (m_bSecFormat)
	{
		m_timePicker.SetFormat(_T("HH:mm:ss"));
	}
	else
	{
		m_timePicker.SetFormat(_T("HH:mm"));
	}

	COleDateTime dateTime = m_time.ToOleDateTime();
	m_timePicker.SetTime(dateTime);

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

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
