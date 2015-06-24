// LogWarningDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "LogWarningDlg.h"
#include ".\logwarningdlg.h"

#include "..\common\EchoSystem.h"
#include "LogSettingDlg.h"


// CLogWarningDlg dialog

IMPLEMENT_DYNAMIC(CLogWarningDlg, CDialog)
CLogWarningDlg::CLogWarningDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogWarningDlg::IDD, pParent)
{
}

CLogWarningDlg::~CLogWarningDlg()
{
}

void CLogWarningDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLogWarningDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_TURNOFF, OnBnClickedButtonTurnoff)
	ON_BN_CLICKED(IDC_BUTTON_KEEPON, OnBnClickedButtonKeepon)
	ON_BN_CLICKED(IDC_BUTTON_LOGSETTING, OnBnClickedButtonLogsetting)
END_MESSAGE_MAP()


// CLogWarningDlg message handlers

void CLogWarningDlg::OnBnClickedButtonTurnoff()
{
	// TODO: Add your control notification handler code here
	ECHOLOG->SetMasterLogDisable();
	SendMessage(WM_COMMAND, IDOK);
}

void CLogWarningDlg::OnBnClickedButtonKeepon()
{
	// TODO: Add your control notification handler code here
	SendMessage(WM_COMMAND, IDOK);
}

void CLogWarningDlg::OnBnClickedButtonLogsetting()
{
	// TODO: Add your control notification handler code here
	CLogSettingDlg dlg;
	dlg.DoModal();
}
