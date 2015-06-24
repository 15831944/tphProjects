// DlgComments.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgComments.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgComments dialog


CDlgComments::CDlgComments(TCHAR* pBuffer, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgComments::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgComments)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pBuffer = pBuffer;
}


void CDlgComments::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgComments)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgComments, CDialog)
	//{{AFX_MSG_MAP(CDlgComments)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgComments message handlers

void CDlgComments::OnOK() 
{
	CEdit* pEditCtrl = (CEdit*) GetDlgItem(IDC_EDIT_COMMENTS);
	HLOCAL h = pEditCtrl->GetHandle();
	LPCTSTR lpszText = (LPCTSTR) ::LocalLock(h);
	_tcscpy(m_pBuffer, lpszText);
	::LocalUnlock(h);

	CDialog::OnOK();
}

BOOL CDlgComments::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CEdit* pEditCtrl = (CEdit*) GetDlgItem(IDC_EDIT_COMMENTS);
	pEditCtrl->SetLimitText(4096);

	pEditCtrl->SetWindowText(m_pBuffer);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
