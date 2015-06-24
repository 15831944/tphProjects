// DlgSelecteAircraftClassification.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSelecteAircraftClassification.h"
#include ".\dlgselecteaircraftclassification.h"


// CDlgSelecteAircraftClassification dialog

IMPLEMENT_DYNAMIC(CDlgSelecteAircraftClassification, CDialog)
CDlgSelecteAircraftClassification::CDlgSelecteAircraftClassification(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelecteAircraftClassification::IDD, pParent)
{
}

CDlgSelecteAircraftClassification::~CDlgSelecteAircraftClassification()
{
}

void CDlgSelecteAircraftClassification::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ACCLASS, m_lbAcClass);
}


BEGIN_MESSAGE_MAP(CDlgSelecteAircraftClassification, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSelecteAircraftClassification message handlers

void CDlgSelecteAircraftClassification::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	int nSel = m_lbAcClass.GetCurSel();
	if(nSel == LB_ERR)
	{
		MessageBox(_T("Please select one classification."),_T("Warning"), MB_OK);
		return;
	}

	m_selectType = (FlightClassificationBasisType)nSel;


	CDialog::OnOK();
}

BOOL CDlgSelecteAircraftClassification::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//WingspanBased = 0,
	//	SurfaceBearingWeightBased,
	//	WakeVortexWightBased,
	//	ApproachSpeedBased,
	//	NoneBased,

	//	CategoryType_Count,

	for (int nType = WingspanBased; nType < NoneBased; ++ nType)
	{
	
		m_lbAcClass.AddString(FlightClassificationBasisType_Nname[nType]);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelecteAircraftClassification::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
