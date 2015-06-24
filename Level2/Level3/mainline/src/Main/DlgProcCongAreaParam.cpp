// DlgProcCongAreaParam.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgProcCongAreaParam.h"
#include ".\dlgproccongareaparam.h"


// CDlgProcCongAreaParam dialog

IMPLEMENT_DYNAMIC(CDlgProcCongAreaParam, CDialog)
CDlgProcCongAreaParam::CDlgProcCongAreaParam(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcCongAreaParam::IDD, pParent)
	, m_dLambda(0.0)
	, m_dTrigger(0.0)
{
}

CDlgProcCongAreaParam::~CDlgProcCongAreaParam()
{
}

void CDlgProcCongAreaParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LAMBDA, m_dLambda);
	DDX_Text(pDX, IDC_EDIT_TRIGGER, m_dTrigger);
}


BEGIN_MESSAGE_MAP(CDlgProcCongAreaParam, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgProcCongAreaParam message handlers

void CDlgProcCongAreaParam::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	OnOK();
}

BOOL CDlgProcCongAreaParam::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
