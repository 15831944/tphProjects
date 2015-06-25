#include "StdAfx.h"
#include "termplan.h"
#include "BoardingCallFlightDialog.h"
#include "TermPlanDoc.h"

IMPLEMENT_DYNAMIC(BoardingCallFlightDialog, CFlightDialog)
BoardingCallFlightDialog::BoardingCallFlightDialog(CWnd* pParent, dialog_mode mode, bool bShowThroughout) 
	: CFlightDialog(pParent, bShowThroughout), m_mode(mode)
{
}


BoardingCallFlightDialog::~BoardingCallFlightDialog(void)
{
}

BEGIN_MESSAGE_MAP(BoardingCallFlightDialog,CFlightDialog)
END_MESSAGE_MAP()

void BoardingCallFlightDialog::ResetSeletion()
{
	if (!::IsWindow(GetSafeHwnd()))
		return;
	CFlightDialog::ResetSeletion();
	CButton* pBtn = NULL;
	switch(m_mode)
	{
	case DIALOG_MODE_BOARDINGCALL:
		pBtn = (CButton *) GetDlgItem(IDC_RADIO_ALLFLIGHTS_FLT);
		if(pBtn) 
			pBtn->EnableWindow(FALSE);
		pBtn = (CButton *) GetDlgItem(IDC_RADIO_ARRIVING_FLT);
		if(pBtn) 
			pBtn->EnableWindow(FALSE);
		pBtn = (CButton *) GetDlgItem(IDC_RADIO_THROUGHOUT_FLT);
		if(pBtn) 
			pBtn->EnableWindow(FALSE);
		break;
	default:
		break;
	}
}

void BoardingCallFlightDialog::InitFltConst( const FlightConstraint& FltConst)
{
	m_FlightSelection = FltConst;
	ResetSeletion();
}
BOOL BoardingCallFlightDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	AfxGetApp()->BeginWaitCursor();
	ResetSeletion();
	AfxGetApp()->EndWaitCursor();
	//m_FlightSelection.SetInputTerminal( GetInputTerminal() );
	m_FlightSelection.SetAirportDB(GetInputTerminal()->m_pAirportDB);

	CButton* pButton = (CButton*) GetDlgItem(IDC_RADIO_THROUGHOUT_FLT);
	if (pButton == NULL)
		return TRUE;
	if (!m_bShowThroughoutMode)
		pButton->ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

