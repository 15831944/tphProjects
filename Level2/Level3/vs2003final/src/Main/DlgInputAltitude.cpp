// DlgInputAltitude.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgInputAltitude.h"
#include ".\dlginputaltitude.h"
#include "Common\UnitsManager.h"
#include "common/ARCStringConvert.h"

// CDlgInputAltitude dialog

IMPLEMENT_DYNAMIC(CDlgInputAltitude, CDialog)
CDlgInputAltitude::CDlgInputAltitude(double dalt, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputAltitude::IDD, pParent)
{
	m_dAlt = dalt;
}

CDlgInputAltitude::~CDlgInputAltitude()
{
}

void CDlgInputAltitude::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ALTITUDE, m_editAlt);
	//DDX_Control(pDX, IDC_CHECK_REMBER, m_chkRember);
}


BEGIN_MESSAGE_MAP(CDlgInputAltitude, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgInputAltitude message handlers

BOOL CDlgInputAltitude::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	/*if(CWnd* pwnd = GetDlgItem(IDC_STATIC_UNIT))
	{		
		pwnd->SetWindowText(UNITSMANAGER->GetLengthUnitString());
	}*/
	double daltDispaly = m_dAlt;//UNITSMANAGER->ConvertLength(UM_LEN_CENTIMETERS,UNITSMANAGER->GetLengthUnits(),m_dAlt);
	CString s;
	s.Format(_T("%.2f"),daltDispaly);
	m_editAlt.SetWindowText(s);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

double CDlgInputAltitude::getAlt() const
{	
	return m_dAlt;
}

//BOOL CDlgInputAltitude::remberAlt() const
//{
//	return m_bCheckRem;
//}
void CDlgInputAltitude::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString str;
	m_editAlt.GetWindowText(str);
	double d;
	if(ARCStringConvert::isNumber(str,d))
	{
		m_dAlt = d;//UNITSMANAGER->ConvertLength(UNITSMANAGER->GetLengthUnits(),UM_LEN_CENTIMETERS,d);
	}
	else
	{
		AfxMessageBox(_T("Please Input a Number!"));
		return;
	}
	//m_bCheckRem = m_chkRember.GetCheck();

	OnOK();
}
