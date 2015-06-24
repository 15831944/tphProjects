// LoadFactorsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "LoadFactorsDlg.h"
#include ".\loadfactorsdlg.h"


// CLoadFactorsDlg dialog

IMPLEMENT_DYNAMIC(CLoadFactorsDlg, CDialog)
CLoadFactorsDlg::CLoadFactorsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadFactorsDlg::IDD, pParent)
{
}

CLoadFactorsDlg::~CLoadFactorsDlg()
{
}

void CLoadFactorsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TREE, m_Static_tree);
}


BEGIN_MESSAGE_MAP(CLoadFactorsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CLoadFactorsDlg message handlers

void CLoadFactorsDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CLoadFactorsDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
