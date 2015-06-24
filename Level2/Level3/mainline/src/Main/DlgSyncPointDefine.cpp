// DlgSyncPointDefine.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSyncPointDefine.h"


// CDlgSyncPointDefine dialog

IMPLEMENT_DYNAMIC(CDlgSyncPointDefine, CDialog)
CDlgSyncPointDefine::CDlgSyncPointDefine(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSyncPointDefine::IDD, pParent)
{
}

CDlgSyncPointDefine::~CDlgSyncPointDefine()
{
}

void CDlgSyncPointDefine::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT_SYNC,m_syncName);
}


BEGIN_MESSAGE_MAP(CDlgSyncPointDefine, CDialog)
END_MESSAGE_MAP()

// CDlgSyncPointDefine message handlers

BOOL CDlgSyncPointDefine::OnInitDialog()
{
	CDialog::OnInitDialog();


	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgSyncPointDefine::OnOK()
{
	UpdateData(TRUE);

	if (m_syncName.IsEmpty())
	{
		MessageBox(_T("Please input synchronization name"),"Error",MB_OK);
		return;
	}
	CDialog::OnOK();
}

CString CDlgSyncPointDefine::GetSyncName() const
{
	return m_syncName;
}