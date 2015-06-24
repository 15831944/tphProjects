// DlgMapCodeInput.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgMapCodeInput.h"


// CDlgMapCodeInput dialog

IMPLEMENT_DYNAMIC(CDlgMapCodeInput, CDialog)
CDlgMapCodeInput::CDlgMapCodeInput(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMapCodeInput::IDD, pParent)
{
}

CDlgMapCodeInput::~CDlgMapCodeInput()
{
}

void CDlgMapCodeInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgMapCodeInput, CDialog)
END_MESSAGE_MAP()


// CDlgMapCodeInput message handlers
BOOL CDlgMapCodeInput::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgMapCodeInput::OnOK()
{
	GetDlgItem(IDC_EDIT_CODE)->GetWindowText(m_strCode);
	GetDlgItem(IDC_EDIT_PROCESS)->GetWindowText(m_strProcess);

	if (m_strCode.IsEmpty() || m_strProcess.IsEmpty())
	{
		::AfxMessageBox(_T("Code or Process is empty"));
		return;
	}
	CDialog::OnOK();
}