// DlgSectionPointProp.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSectionPointProp.h"
#include ".\dlgsectionpointprop.h"
#include <common/ARCStringConvert.h>
#include <common\UnitsManager.h>

// CDlgSectionPointProp dialog

IMPLEMENT_DYNAMIC(CDlgSectionPointProp, CDialog)
CDlgSectionPointProp::CDlgSectionPointProp(double dY, double dZ,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSectionPointProp::IDD, pParent)
{
	mdY = dY;
	mdZ = dZ;
}


CDlgSectionPointProp::~CDlgSectionPointProp()
{
}

void CDlgSectionPointProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSectionPointProp, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgSectionPointProp message handlers

void CDlgSectionPointProp::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	RetriveData();
	OnOK();
}

BOOL CDlgSectionPointProp::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	LoadData();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSectionPointProp::LoadData()
{

	CWnd* pEditY = GetDlgItem(IDC_EDITY);
	CWnd* PEditZ = GetDlgItem(IDC_EDITZ);
	if(pEditY){
		double d = UNITSMANAGER->ConvertLength(mdY);
		pEditY->SetWindowText( ARCStringConvert::toString(d) );
	}
	if(PEditZ)
	{
		double d  = UNITSMANAGER->ConvertLength(mdZ);
		PEditZ->SetWindowText( ARCStringConvert::toString(d));
	}
}

void CDlgSectionPointProp::RetriveData()
{
	CWnd* pEditY = GetDlgItem(IDC_EDITY);
	CWnd* PEditZ = GetDlgItem(IDC_EDITZ);
	if(pEditY){
		CString tex;
		pEditY->GetWindowText(tex);
		double d = atof(tex.GetString());
		mdY = UNITSMANAGER->UnConvertLength(d);		
	}
	if(PEditZ)
	{
		CString tex;
		PEditZ->GetWindowText(tex);
		double d = atof(tex.GetString());
		mdZ = UNITSMANAGER->UnConvertLength(d);		
	}
}

void CDlgSectionPointProp::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
