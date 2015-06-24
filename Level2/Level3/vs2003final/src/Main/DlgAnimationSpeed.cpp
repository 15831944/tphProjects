// DlgAnimationSpeed.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgAnimationSpeed.h"
#include ".\dlganimationspeed.h"


// DlgAnimationSpeed dialog

IMPLEMENT_DYNAMIC(DlgAnimationSpeed, CDialog)
DlgAnimationSpeed::DlgAnimationSpeed(int iAnimationSpeed, CWnd* pParent /*=NULL*/)
: CDialog(DlgAnimationSpeed::IDD, pParent)
,m_nAnimationSpeed(iAnimationSpeed)
{

}

DlgAnimationSpeed::~DlgAnimationSpeed()
{
}

void DlgAnimationSpeed::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgAnimationSpeed, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
END_MESSAGE_MAP()


// DlgAnimationSpeed message handlers

void DlgAnimationSpeed::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString sAnimationSpeed;
	int Speed;
	GetDlgItem(IDC_EDIT1)->GetWindowText(sAnimationSpeed);
	Speed = atoi(sAnimationSpeed);
	if (Speed>0)
		m_nAnimationSpeed = Speed*10;
	OnOK();
}

BOOL DlgAnimationSpeed::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString sAnimationSpeed;
	sAnimationSpeed.Format("%d",m_nAnimationSpeed/10);
	GetDlgItem(IDC_EDIT1)->SetWindowText(sAnimationSpeed);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int DlgAnimationSpeed::getAnimationSpeed() const
{
	return m_nAnimationSpeed;
}

void DlgAnimationSpeed::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
