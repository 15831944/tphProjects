#include "StdAfx.h"
#include "resource.h"
#include "InputAirside\LandingRunwayAssignmentStrategies.h"
#include ".\dlglandingrunwayassign.h"

CDlgLandingRunwayAssign::CDlgLandingRunwayAssign(int nProjID, PSelectFlightType pSelectFlightType, CAirportDatabase *pAirPortdb, CWnd* pParent /* = NULL */)
 : CDlgRunwayAssignment(nProjID, pSelectFlightType, _T("Landing Runway Assignment Strategies"), pParent)
 , m_pAirportDatabase(pAirPortdb)
{
}

CDlgLandingRunwayAssign::~CDlgLandingRunwayAssign()
{
}

void CDlgLandingRunwayAssign::ReadData()
{
	m_pStrategies = new LandingRunwayAssignmentStrategies(m_nProjID);
	m_pStrategies->SetAirportDatabase(m_pAirportDatabase);
	m_pStrategies->ReadData();
}

BOOL CDlgLandingRunwayAssign::OnInitDialog()
{
	CDlgRunwayAssignment::OnInitDialog();

	GetDlgItem(IDC_STATIC_STRATEGY_TITLE)->SetWindowText(_T("Landing runway assignment strategy"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
