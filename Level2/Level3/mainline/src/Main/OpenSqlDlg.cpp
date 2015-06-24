// OpenSqlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "OpenSqlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenSqlDlg dialog


COpenSqlDlg::COpenSqlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpenSqlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COpenSqlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bCommonSql = TRUE;
}


void COpenSqlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenSqlDlg)
	DDX_Control(pDX, IDC_PASSWORD, m_edtPassword);
	DDX_Control(pDX, IDC_LOGIN, m_edtLoginName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpenSqlDlg, CDialog)
	//{{AFX_MSG_MAP(COpenSqlDlg)
	ON_BN_CLICKED(IDC_SQL_AUTHOR, OnSqlAuthor)
	ON_BN_CLICKED(IDC_WINDOWS_AUTHOR, OnWindowsAuthor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenSqlDlg message handlers

void COpenSqlDlg::InitControl()
{
	if (!m_bCommonSql)
	{
		GetDlgItem(IDC_FRAME_TABLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LABEL_TABLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHOOSE_TABLE)->ShowWindow(SW_HIDE);
		
		CRect rc;
		GetDlgItem(IDOK)->GetWindowRect(rc);
		ScreenToClient(rc);
		rc.top -= 70;
		rc.bottom -= 70;
		GetDlgItem(IDOK)->MoveWindow(rc);

		GetDlgItem(IDCANCEL)->GetWindowRect(rc);
		ScreenToClient(rc);
		rc.top -= 70;
		rc.bottom -= 70;
		GetDlgItem(IDCANCEL)->MoveWindow(rc);

		GetWindowRect(rc);
		rc.bottom -= 70;
		MoveWindow(rc);
	}

	CButton* pButton = (CButton*)GetDlgItem(IDC_SQL_AUTHOR);
	pButton->SetCheck(TRUE);
}

void COpenSqlDlg::SetCommonSql(BOOL bValue)
{
	m_bCommonSql = bValue;
}

BOOL COpenSqlDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitControl();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COpenSqlDlg::OnOK() 
{
	// TODO: Add extra validation here
	AfxMessageBox(_T("Failed to connect SQL Server!"));
	return;
	
	CDialog::OnOK();
}

void COpenSqlDlg::OnSqlAuthor() 
{
	// TODO: Add your control notification handler code here
	CButton* pButton = (CButton*)GetDlgItem(IDC_SQL_AUTHOR);
	if (pButton->GetCheck())
	{
		m_edtLoginName.EnableWindow(TRUE);
		m_edtPassword.EnableWindow(TRUE);
	}
}

void COpenSqlDlg::OnWindowsAuthor() 
{
	// TODO: Add your control notification handler code here
	CButton* pButton = (CButton*)GetDlgItem(IDC_WINDOWS_AUTHOR);
	if (pButton->GetCheck())
	{
		m_edtLoginName.EnableWindow(FALSE);
		m_edtPassword.EnableWindow(FALSE);
	}
}
