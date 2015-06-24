// DlgNewCarryonItemName.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgNewCarryonItemName.h"
#include ".\dlgnewcarryonitemname.h"


// DlgNewCarryonItemName dialog

IMPLEMENT_DYNAMIC(DlgNewCarryonItemName, CDialog)
DlgNewCarryonItemName::DlgNewCarryonItemName(CWnd* pParent /*=NULL*/)
	: CDialog(DlgNewCarryonItemName::IDD, pParent)
{
}

DlgNewCarryonItemName::~DlgNewCarryonItemName()
{
}

void DlgNewCarryonItemName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgNewCarryonItemName, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// DlgNewCarryonItemName message handlers

void DlgNewCarryonItemName::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_strName =""; 
	GetDlgItemText(IDC_EDIT_ITEMNAME, m_strName);
	OnOK();
}

void DlgNewCarryonItemName::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
