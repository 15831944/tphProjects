// DlgNewFlightType.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgNewFlightType.h"
#include ".\dlgnewflighttype.h"
#include "FlightDialog.h"

// DlgNewFlightType dialog

IMPLEMENT_DYNAMIC(DlgNewFlightType, CDialog)
DlgNewFlightType::DlgNewFlightType(const AirsideFlightType& fltType,CWnd* pParent /*=NULL*/)
	: CDialog(DlgNewFlightType::IDD, pParent)
{

	m_fltType = fltType;
}

DlgNewFlightType::~DlgNewFlightType()
{
}

void DlgNewFlightType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_ARRTODEP, m_btnArrToDep);
}


BEGIN_MESSAGE_MAP(DlgNewFlightType, CDialog)
	ON_BN_CLICKED(IDC_CHECK_ARRTODEP, OnBnClickedCheckArrtodep)
	ON_BN_CLICKED(IDC_BUTTON_CNST1, OnBnClickedButtonCnst1)
	ON_BN_CLICKED(IDC_BUTTON_CNST2, OnBnClickedButtonCnst2)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// DlgNewFlightType message handlers

void DlgNewFlightType::OnBnClickedCheckArrtodep()
{
	if (m_btnArrToDep.GetCheck())
	{
		GetDlgItem(IDC_STATIC_ARR)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_DEP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_CNST2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_CNST2)->ShowWindow(SW_SHOW);

		CString strFlight;
		m_fltType.GetFltConst2().screenPrint(strFlight);
		GetDlgItem(IDC_EDIT_CNST2)->SetWindowText(strFlight);
		m_fltType.SetArrToDep(true);
	}
	else
	{
		GetDlgItem(IDC_STATIC_ARR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_DEP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_CNST2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_CNST2)->ShowWindow(SW_HIDE);

		m_fltType.SetArrToDep(false);

		FlightConstraint fltconst;
		fltconst.initDefault();
		m_fltType.SetFltConst2(fltconst);
	}
}

void DlgNewFlightType::OnBnClickedButtonCnst1()
{
	CFlightDialog flightDlg(NULL);
	if(flightDlg.DoModal() != IDOK)
		return;	

	CString strFlight;
	flightDlg.GetFlightSelection().screenPrint(strFlight);
	m_fltType.SetFltConst1(flightDlg.GetFlightSelection());
	GetDlgItem(IDC_EDIT_CNST1)->SetWindowText(strFlight);
}

void DlgNewFlightType::OnBnClickedButtonCnst2()
{
	CFlightDialog flightDlg(NULL);
	if(flightDlg.DoModal() != IDOK)
		return;	

	CString strFlight;
	flightDlg.GetFlightSelection().screenPrint(strFlight);
	m_fltType.SetFltConst2(flightDlg.GetFlightSelection());
	GetDlgItem(IDC_EDIT_CNST2)->SetWindowText(strFlight);
}

void DlgNewFlightType::OnBnClickedOk()
{
	OnOK();
}

BOOL DlgNewFlightType::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strFlight;
	m_fltType.GetFltConst1().screenPrint(strFlight);
	GetDlgItem(IDC_EDIT_CNST1)->SetWindowText(strFlight);

	if (m_fltType.IsArrToDep())
	{
		GetDlgItem(IDC_STATIC_ARR)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_DEP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_CNST2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_CNST2)->ShowWindow(SW_SHOW);

		CString strFlight2;
		m_fltType.GetFltConst2().screenPrint(strFlight2);
		GetDlgItem(IDC_EDIT_CNST2)->SetWindowText(strFlight2);

		m_btnArrToDep.SetCheck(TRUE);
	}
	else
	{
		GetDlgItem(IDC_STATIC_ARR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_DEP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_CNST2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_CNST2)->ShowWindow(SW_HIDE);
		m_btnArrToDep.SetCheck(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

const AirsideFlightType& DlgNewFlightType::GetFltType()
{
	return m_fltType;
}