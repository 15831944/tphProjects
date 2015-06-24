// DlgMathSimSetting.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgMathSimSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMathSimSetting dialog


CDlgMathSimSetting::CDlgMathSimSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMathSimSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMathSimSetting)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nInterval = 1800;
}


void CDlgMathSimSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMathSimSetting)
	DDX_Control(pDX, IDC_DATETIME_MATHINTERVAL, m_dtInterval);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMathSimSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgMathSimSetting)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMathSimSetting message handlers

BOOL CDlgMathSimSetting::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_dtInterval.SetFormat(_T("HH:mm"));

	int nHour = int(m_nInterval / 3600);
	int nMinute = int((m_nInterval - (3600 * nHour)) / 60);

	COleDateTime dtInterval;
	dtInterval.SetTime(nHour, nMinute, 0);
	m_dtInterval.SetTime(dtInterval);
	
	return TRUE;
}

void CDlgMathSimSetting::OnOK() 
{
	COleDateTime dtInterval;
	m_dtInterval.GetTime(dtInterval);
	m_nInterval = dtInterval.GetHour() * 3600 + dtInterval.GetMinute() * 60;
	
	CDialog::OnOK();
}
