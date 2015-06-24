// DlgSelectRunwayOperation.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSelectRunwayOperation.h"
#include ".\dlgselectrunwayoperation.h"


// CDlgSelectRunwayOperation dialog

IMPLEMENT_DYNAMIC(CDlgSelectRunwayOperation, CDialog)
CDlgSelectRunwayOperation::CDlgSelectRunwayOperation(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectRunwayOperation::IDD, pParent)
{
	m_nCursel = -1;
}

CDlgSelectRunwayOperation::~CDlgSelectRunwayOperation()
{
}

void CDlgSelectRunwayOperation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_OPERATION, m_lbOperation);
}


BEGIN_MESSAGE_MAP(CDlgSelectRunwayOperation, CDialog)
END_MESSAGE_MAP()


// CDlgSelectRunwayOperation message handlers

BOOL CDlgSelectRunwayOperation::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	m_lbOperation.AddString(_T("All"));
	m_lbOperation.AddString(_T("Landing"));
	m_lbOperation.AddString(_T("Take off"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectRunwayOperation::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_lbOperation.GetCurSel() == LB_ERR)
	{
		MessageBox(_T("Please select one type."));
		return;
	}

	m_nCursel = m_lbOperation.GetCurSel();

	CDialog::OnOK();
}

void CDlgSelectRunwayOperation::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}
