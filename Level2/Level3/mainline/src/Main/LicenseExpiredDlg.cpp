// LicenseExpiredDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "LicenseExpiredDlg.h"
#include ".\licenseexpireddlg.h"

#include "Version.h"


// CLicenseExpiredDlg dialog

IMPLEMENT_DYNAMIC(CLicenseExpiredDlg, CDialog)
CLicenseExpiredDlg::CLicenseExpiredDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLicenseExpiredDlg::IDD, pParent)
{
}

CLicenseExpiredDlg::~CLicenseExpiredDlg()
{
}

void CLicenseExpiredDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLicenseExpiredDlg, CDialog)
	ON_BN_CLICKED(IDC_CORP_URL, OnBnClickedCorpUrl)
END_MESSAGE_MAP()


// CLicenseExpiredDlg message handlers

void CLicenseExpiredDlg::OnBnClickedCorpUrl()
{
	// TODO: Add your control notification handler code here

	// Launch website in browser...
	ShellExecute(NULL, "open", "http://www.arc-us-ca.com/", NULL, NULL, SW_SHOW);
}

BOOL CLicenseExpiredDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	CString str;
	str.Format("%s",ARCTERM_VERSION_STRING);// = "(3.1.039)";ARCTERM_VERSION_STRING
	SetDlgItemText(IDC_VERSION, str);

	// Format time remaining...
	//int nTemp = ((CTermPlanApp *) AfxGetApp())->SimulationControlLicense.GetProductLicenseSecondsRemaining();

	
	//if(nTemp == 0)           // Expired...
		str.Format("Expired");
	//else if(nTemp == -1)     // Unlimited...
	//	str.Format("Unlimited");
	//else if(nTemp > (24 * 60 * 60))   // More than one day...
	//	str.Format("%d days", (int) ((float) nTemp / (24.0f * 60.0f * 60.0f)));
	//else                              // Less than a day...
	//	str.Format("%d hours", (int) ((float) nTemp / (60.0f * 60.0f)));

	// Show time remaining...
	SetDlgItemText(IDC_LICENSE, str);


	//
	SetIcon(LoadIcon(NULL, IDI_ERROR), TRUE);
	SetIcon(LoadIcon(NULL, IDI_ERROR), FALSE);
	MessageBeep(MB_ICONINFORMATION);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
