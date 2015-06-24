// DlgSelectedAnimationModels.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSelectedAnimationModels.h"
#include "dlgselectedanimationmodels.h"
#include "AnimationData.h"
#include ".\dlgselectedanimationmodels.h"


// CDlgSelectedAnimationModels dialog

IMPLEMENT_DYNAMIC(CDlgSelectedAnimationModels, CDialog)
CDlgSelectedAnimationModels::CDlgSelectedAnimationModels(AnimationData &amData,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectedAnimationModels::IDD, pParent)
	, m_bTerminalMode(FALSE)
	, m_bAirsideMode(FALSE)
	, m_bLandsideMode(FALSE)
	, m_bOnBoardMode(FALSE)
{
	m_pAnimData = &amData;
}

CDlgSelectedAnimationModels::~CDlgSelectedAnimationModels()
{
}

void CDlgSelectedAnimationModels::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_TERMINAL, m_bTerminalMode);
	DDX_Check(pDX, IDC_CHECK_AIRSIDE, m_bAirsideMode);
	DDX_Check(pDX, IDC_CHECK_LANDSIDE, m_bLandsideMode);
	DDX_Check(pDX, IDC_CHECK_ONBOARD, m_bOnBoardMode);
}


BEGIN_MESSAGE_MAP(CDlgSelectedAnimationModels, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSelectedAnimationModels message handlers

BOOL CDlgSelectedAnimationModels::OnInitDialog()
{
	CDialog::OnInitDialog();

	CTermPlanApp* pInst = (CTermPlanApp*)AfxGetApp();
	ASSERT(pInst != NULL);


	ASSERT(m_pAnimData != NULL);

	if(! pInst->SimulationControlLicense.m_bAirsideSim)
	{
		GetDlgItem(IDC_CHECK_AIRSIDE)->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_AIRSIDE))->SetCheck(BST_UNCHECKED);
	}
	else
	{
		if (m_pAnimData->m_AnimationModels.IsAirsideSel())
			m_bAirsideMode = TRUE;
	}

	if (!pInst->SimulationControlLicense.m_bLandsideSim)
	{
		GetDlgItem(IDC_CHECK_LANDSIDE)->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_LANDSIDE))->SetCheck(BST_UNCHECKED);
	}
	else
	{
		if (m_pAnimData->m_AnimationModels.IsLandsideSel())
			m_bLandsideMode = TRUE;
	}

	if (!pInst->SimulationControlLicense.m_bTerminalSim)
	{
		GetDlgItem(IDC_CHECK_TERMINAL)->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_TERMINAL))->SetCheck(BST_UNCHECKED);
	}
	else
	{
		if(m_pAnimData->m_AnimationModels.IsTerminalSel())
			m_bTerminalMode = TRUE;
	}

	if (!pInst->SimulationControlLicense.m_bOnBoardSim)
	{
		GetDlgItem(IDC_CHECK_ONBOARD)->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_ONBOARD))->SetCheck(BST_UNCHECKED);
	}
	else
	{
		if(m_pAnimData->m_AnimationModels.IsOnBoardSel())
			m_bOnBoardMode = TRUE;
	}

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectedAnimationModels::OnBnClickedOk()
{
	UpdateData(TRUE);
	m_pAnimData->m_AnimationModels.Reset();
	if (m_bAirsideMode||m_bTerminalMode||m_bLandsideMode || m_bOnBoardMode)
	{
		if (m_bTerminalMode)
			m_pAnimData->m_AnimationModels.SelTerminalModel();
		if (m_bAirsideMode)
			m_pAnimData->m_AnimationModels.SelAirsideModel();
		if (m_bLandsideMode)
			m_pAnimData->m_AnimationModels.SelLandsideModel();
		if (m_bOnBoardMode)
			m_pAnimData->m_AnimationModels.SelOnBoardModel();
	}
	else
	{
		MessageBox("Please select one or more environment modes to animationed",MB_OK);
		return;
	}
	OnOK();
}
