// DlgOnboardFlightSelection.cpp : implementation file
//

#include "stdafx.h"
#include "../Resource.h"
#include <Inputs/InputsDll.h>
#include "DlgOnboardFlightSelection.h"
#include ".\dlgonboardflightselection.h"
#include "InputOnboard\OnBoardAnalysisCondidates.h"
#include "Inputs\schedule.h"
#include "Common\FLT_CNST.H"
#include "Inputs\flight.h"
#include "Inputs\IN_TERM.H"
// DlgOnboardFlightSelection dialog

IMPLEMENT_DYNAMIC(DlgOnboardFlightSelection, CDialog)
DlgOnboardFlightSelection::DlgOnboardFlightSelection(InputTerminal* pTerminal, CFlightTypeCandidate* fltTypeCandidate, CWnd* pParent /*=NULL*/)
	: CDialog(DlgOnboardFlightSelection::IDD, pParent)
	,m_pTerminal(pTerminal)
{
	m_pFltTypeCandidate = fltTypeCandidate;
}

DlgOnboardFlightSelection::~DlgOnboardFlightSelection()
{
}

void DlgOnboardFlightSelection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ONBOARDFLIGHT, m_lFlightList);
}


BEGIN_MESSAGE_MAP(DlgOnboardFlightSelection, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// DlgOnboardFlightSelection message handlers

void DlgOnboardFlightSelection::OnBnClickedOk()
{
	int nSel = m_lFlightList.GetCurSel();
	if( nSel != LB_ERR )
	{
		m_pSelFlight = (Flight*)m_lFlightList.GetItemData(nSel);
		m_lFlightList.GetText(nSel,m_strFlight);
	}

	OnOK();
}

BOOL DlgOnboardFlightSelection::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_lFlightList.ResetContent();
	int nCount = m_pTerminal->flightSchedule->getCount();
	for (int i = 0; i < nCount; i++)
	{
		Flight* pFlight = m_pTerminal->flightSchedule->getItem(i);
		if (m_pFltTypeCandidate->GetFltType().fits(pFlight->getLogEntry()))
		{
			char szID[1024] = {0};
			char szACType[1024] = {0};
			CString strFlight = _T("");

			if(pFlight->isArriving() && m_pFltTypeCandidate->GetFltType().GetFltConstraintMode() != ENUM_FLTCNSTR_MODE_DEP)
			{
				pFlight->getFullID(szID,'A');
				pFlight->getACType(szACType);
				int nDay = pFlight->getArrTime().GetDay();

				if (m_pFltTypeCandidate->GetOnboardFlight(szID,szACType,ArrFlight,nDay) == NULL)
				{
					CString strFlight ;
					strFlight.Format(_T("Flight %s - ACTYPE %s Arr Day%d"),szID,szACType,nDay);

					int idx = m_lFlightList.AddString(strFlight);
					m_lFlightList.SetItemData(idx,(DWORD_PTR)pFlight);
				}
			}

			if (pFlight->isDeparting() && m_pFltTypeCandidate->GetFltType().GetFltConstraintMode() != ENUM_FLTCNSTR_MODE_ARR)
			{
				pFlight->getFullID(szID,'D');
				pFlight->getACType(szACType);
				int nDay = pFlight->getDepTime().GetDay();

				if (m_pFltTypeCandidate->GetOnboardFlight(szID,szACType,DepFlight,nDay) == NULL)
				{
					CString strFlight ;
					strFlight.Format(_T("Flight %s - ACTYPE %s Dep Day%d"),szID,szACType,nDay);

					int idx = m_lFlightList.AddString(strFlight);
					m_lFlightList.SetItemData(idx,(DWORD_PTR)pFlight);
				}
			}	
		}
	}

	return TRUE;  
}
