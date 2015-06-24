// DlgAssignmentConstraintSpec.cpp : implementation file
//
#include "stdafx.h"
#include "resource.h"
#include "DlgAssignmentConstraintSpec.h"
// DlgAssignmentConstraintSpec dialog

IMPLEMENT_DYNAMIC(DlgAssignmentConstraintSpec, CDialog)
DlgAssignmentConstraintSpec::DlgAssignmentConstraintSpec(StandAdjacencyConstraint* adjaconstrain,CWnd* pParent /*=NULL*/)
	: CDialog(DlgAssignmentConstraintSpec::IDD, pParent)
	,m_standAdjConstraint(adjaconstrain)
{
	
}

DlgAssignmentConstraintSpec::~DlgAssignmentConstraintSpec()
{
}

void DlgAssignmentConstraintSpec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_NAME, m_RadioName);
}

BEGIN_MESSAGE_MAP(DlgAssignmentConstraintSpec, CDialog)
	ON_COMMAND(IDOK, OnContinue)
END_MESSAGE_MAP()

BOOL DlgAssignmentConstraintSpec::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	if (m_standAdjConstraint->GetUseFlag())
	{
		CheckDlgButton(IDC_RADIO_NAME,TRUE);
		CheckDlgButton(IDC_RADIO_DIMENSION,FALSE);
	}
	else
	{
		CheckDlgButton(IDC_RADIO_NAME,FALSE);
		CheckDlgButton(IDC_RADIO_DIMENSION,TRUE);
	}
	return TRUE;
}

void DlgAssignmentConstraintSpec::OnCancel()
{
	CDialog::OnCancel();
}

void DlgAssignmentConstraintSpec::OnContinue()
{
	if (((CButton*)GetDlgItem(IDC_RADIO_NAME))->GetCheck())
	{
		m_standAdjConstraint->SetUseFlag(1);
	}	
	else
	{
		m_standAdjConstraint->SetUseFlag(0);
	}
	m_standAdjConstraint->writeData();
	CDialog::OnOK();
}

// DlgAssignmentConstraintSpec message handlers
