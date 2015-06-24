// GetSectionNunmDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "GetSectionNunmDlg.h"
#include ".\getsectionnunmdlg.h"


// GetSectionNunmDlg dialog

IMPLEMENT_DYNAMIC(GetSectionNunmDlg, CDialog)
GetSectionNunmDlg::GetSectionNunmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GetSectionNunmDlg::IDD, pParent)
{
	m_point_num = 0;
}

GetSectionNunmDlg::~GetSectionNunmDlg()
{
}

void GetSectionNunmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(GetSectionNunmDlg, CDialog)
	ON_BN_CLICKED(IDC_OK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// GetSectionNunmDlg message handlers

void GetSectionNunmDlg::OnBnClickedOk()
{
	char buf[128];
	GetDlgItem(IDC_EDIT1)->GetWindowText(buf, 128);
	m_point_num = atoi(buf);
	EndDialog(0);
}

void GetSectionNunmDlg::OnBnClickedCancel()
{
	m_point_num = 0;
	EndDialog(0);
}
