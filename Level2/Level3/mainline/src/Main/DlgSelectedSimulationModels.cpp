// DlgSelectedSimulationModels.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSelectedSimulationModels.h"
#include ".\dlgselectedsimulationmodels.h"
#include "SimualtionControlLicense.h"

// CDlgSelectedSimulationModels dialog

IMPLEMENT_DYNAMIC(CDlgSelectedSimulationModels, CDialog)
CDlgSelectedSimulationModels::CDlgSelectedSimulationModels(SelectedSimulationData &simData, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectedSimulationModels::IDD, pParent)
	, m_bTerminalMode(FALSE)
	, m_bAirsideMode(FALSE)
	, m_bLandsideMode(FALSE)
	, m_bOnBoardMode(FALSE)
{
	m_pSimData = &simData;
}

CDlgSelectedSimulationModels::~CDlgSelectedSimulationModels()
{
}

void CDlgSelectedSimulationModels::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_TERMINAL, m_bTerminalMode);
	DDX_Check(pDX, IDC_CHECK_AIRSIDE, m_bAirsideMode);
	DDX_Check(pDX, IDC_CHECK_LANDSIDE, m_bLandsideMode);
	DDX_Check(pDX, IDC_CHECK_ONBOARD, m_bOnBoardMode);
}


BEGIN_MESSAGE_MAP(CDlgSelectedSimulationModels, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_AIRSIDE, OnBnClickedCheckAirside)
END_MESSAGE_MAP()


// CDlgSelectedSimulationModels message handlers

BOOL CDlgSelectedSimulationModels::OnInitDialog()
{
	CDialog::OnInitDialog();

	CTermPlanApp* pInst = (CTermPlanApp*)AfxGetApp();
	ASSERT(pInst != NULL);


	ASSERT(m_pSimData != NULL);

	if(! pInst->SimulationControlLicense.m_bAirsideSim)
	{
		GetDlgItem(IDC_CHECK_AIRSIDE)->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_AIRSIDE))->SetCheck(BST_UNCHECKED);
	}
	else
	{
		if (m_pSimData->IsAirsideSel())
			m_bAirsideMode = TRUE;
	}

	if (!pInst->SimulationControlLicense.m_bLandsideSim)
	{
		GetDlgItem(IDC_CHECK_LANDSIDE)->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_LANDSIDE))->SetCheck(BST_UNCHECKED);
	}
	else
	{
		if (m_pSimData->IsLandsideSel())
			m_bLandsideMode = TRUE;
	}

	if (!pInst->SimulationControlLicense.m_bTerminalSim)
	{
		GetDlgItem(IDC_CHECK_TERMINAL)->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_TERMINAL))->SetCheck(BST_UNCHECKED);
	}
	else
	{
		if(m_pSimData->IsTerminalSel())
			m_bTerminalMode = TRUE;
	}

	if (!pInst->SimulationControlLicense.m_bOnBoardSim)
	{
		GetDlgItem(IDC_CHECK_ONBOARD)->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK_ONBOARD))->SetCheck(BST_UNCHECKED);
	}
	else
	{
		if(m_pSimData->IsOnBoardSel())
			m_bOnBoardMode = TRUE;
	}

//#ifdef _DEBUG 
//	if (m_pSimData->IsAirsideSel())
//		m_bAirsideMode = TRUE;
//	if (m_pSimData->IsLandsideSel())
//		m_bLandsideMode = TRUE;
//	if(m_pSimData->IsTerminalSel())
//		m_bTerminalMode = TRUE;
//#endif


	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectedSimulationModels::OnBnClickedOk()
{
	UpdateData(TRUE);
	m_pSimData->Reset();
	if (m_bAirsideMode||m_bTerminalMode||m_bLandsideMode || m_bOnBoardMode)
	{
		if (m_bTerminalMode)
			m_pSimData->SelTerminalModel();
		if (m_bAirsideMode)
			m_pSimData->SelAirsideModel();
		if (m_bLandsideMode)
			m_pSimData->SelLandsideModel();
		if (m_bOnBoardMode)
			m_pSimData->SelOnBoardModel();
	}
	else
	{
		MessageBox("Please select one or more environment modes",MB_OK);
		return;
	}
	OnOK();
}

void CDlgSelectedSimulationModels::OnBnClickedCheckAirside()
{
	// TODO: Add your control notification handler code here
}
