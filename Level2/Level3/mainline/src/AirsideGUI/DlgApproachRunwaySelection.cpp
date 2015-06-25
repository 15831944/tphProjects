// DlgApproachRunwaySelection.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgApproachRunwaySelection.h"
#include ".\dlgapproachrunwayselection.h"
#include "../InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/runway.h"

// CDlgApproachRunwaySelection dialog

IMPLEMENT_DYNAMIC(CDlgApproachRunwaySelection, CDialog)
CDlgApproachRunwaySelection::CDlgApproachRunwaySelection(int nProjID,AIRCRAFTOPERATIONTYPE emAppType,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgApproachRunwaySelection::IDD, pParent),m_nProjID(nProjID),m_emAppType(emAppType)
{
	m_nFirstRunwayID = -1;
	m_nSecondRunwayID = -1;
	m_nFlag = -1;
}

CDlgApproachRunwaySelection::~CDlgApproachRunwaySelection()
{
}

void CDlgApproachRunwaySelection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_wndFirstTree);
	DDX_Control(pDX, IDC_TREE2, m_wndSecondTree);
}


BEGIN_MESSAGE_MAP(CDlgApproachRunwaySelection, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgApproachRunwaySelection message handlers

BOOL CDlgApproachRunwaySelection::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strTitle = _T("Second Runway:");
	CString FirstStrTitle = _T("Landing Aircraft Runway:") ;
	switch(m_emAppType) {
	case AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED:
		strTitle = _T("Landed Aircraft Runway:");
		break;
	case AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF:
		strTitle = _T("Take Off Aircraft Runway:");
		break;
	case AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED:
		{
			FirstStrTitle.Format(_T("%s"),_T("Taking Off Runway:")) ;
			strTitle.Format(_T("%s"),_T("Landed Runway")) ;
			break ;
		}
	case AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF:
		{
			FirstStrTitle.Format(_T("%s"),_T("Taking Off Runway:")) ;
			strTitle.Format(_T("%s"),_T("Taken Off Runway")) ;
			break ;
		}
	default:
		break;
	}
	GetDlgItem(IDC_STATIC_SECOND_RUNWAY)->SetWindowText(strTitle);
    GetDlgItem(IDC_STATIC_FIRSTRUNWAY)->SetWindowText(FirstStrTitle) ;
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		HTREEITEM hAirport1 = m_wndFirstTree.InsertItem(airport.getName());
		HTREEITEM hAirport2 = m_wndSecondTree.InsertItem(airport.getName());

		std::vector<int> vrRunwayIDs;
		vrRunwayIDs.clear();
		ALTAirport::GetRunwaysIDs(*iterAirportID,vrRunwayIDs);
		for (std::vector<int>::iterator itrRunwayID = vrRunwayIDs.begin();itrRunwayID != vrRunwayIDs.end();++itrRunwayID) {
			Runway runway;
			runway.ReadObject(*itrRunwayID);
			HTREEITEM hRunway1 = m_wndFirstTree.InsertItem(runway.GetObjNameString(),hAirport1);
			m_wndFirstTree.SetItemData(hRunway1,*itrRunwayID);
			HTREEITEM hRunway1Mark1 = m_wndFirstTree.InsertItem(runway.GetMarking1().c_str(),hRunway1);
			m_wndFirstTree.SetItemData(hRunway1Mark1,(DWORD_PTR)RUNWAYMARK_FIRST);
			HTREEITEM hRunway1Mark2 = m_wndFirstTree.InsertItem(runway.GetMarking2().c_str(),hRunway1);
			m_wndFirstTree.SetItemData(hRunway1Mark2,(DWORD_PTR)RUNWAYMARK_SECOND);
			m_wndFirstTree.Expand(hRunway1,TVE_EXPAND);

			HTREEITEM hRunway2 = m_wndSecondTree.InsertItem(runway.GetObjNameString(),hAirport2);
			m_wndSecondTree.SetItemData(hRunway2,*itrRunwayID);
			HTREEITEM hRunway2Mark1 = m_wndSecondTree.InsertItem(runway.GetMarking1().c_str(),hRunway2);
			m_wndSecondTree.SetItemData(hRunway2Mark1,(DWORD_PTR)RUNWAYMARK_FIRST);
			HTREEITEM hRunway2Mark2 = m_wndSecondTree.InsertItem(runway.GetMarking2().c_str(),hRunway2);
			m_wndSecondTree.SetItemData(hRunway2Mark2,(DWORD_PTR)RUNWAYMARK_SECOND);
			m_wndSecondTree.Expand(hRunway2,TVE_EXPAND);
		}
		m_wndFirstTree.Expand(hAirport1,TVE_EXPAND);
		m_wndSecondTree.Expand(hAirport2,TVE_EXPAND);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgApproachRunwaySelection::OnBnClickedOk()
{
	HTREEITEM hTree1Sel = m_wndFirstTree.GetSelectedItem();
	HTREEITEM hTree2Sel = m_wndSecondTree.GetSelectedItem();
	if (m_wndFirstTree.GetChildItem(hTree1Sel)) {
		MessageBox(_T("Invalid Landing Runway Mark."));
		return;
	}

	CString strTitle = _T("Invalid Second Runway Mark.");
	switch(m_emAppType) {
	case AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED:
		strTitle = _T("Invalid Landed Aircraft Runway Mark.");
		break;
	case AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF:
		strTitle = _T("Invalid Take Off Aircraft Runway Mark.");
		break;
	default:
		break;
	}
	if (m_wndSecondTree.GetChildItem(hTree2Sel)) {
		MessageBox(strTitle);
		return;
	}

	m_nFirstRunwayID  = (int)m_wndFirstTree.GetItemData(m_wndFirstTree.GetParentItem(hTree1Sel));
	m_emFirstRunwayMark = (RUNWAY_MARK)m_wndFirstTree.GetItemData(hTree1Sel);
	m_nSecondRunwayID = (int)m_wndSecondTree.GetItemData(m_wndSecondTree.GetParentItem(hTree2Sel));
	m_emSecondRunwayMark = (RUNWAY_MARK)m_wndSecondTree.GetItemData(hTree2Sel);
	if(m_nFlag != 0x1){//not allow same runway
		if (m_nFirstRunwayID == m_nSecondRunwayID) {
			if (m_emFirstRunwayMark == m_emSecondRunwayMark) {
				strTitle = _T("Can not select the same runway mark.");
				MessageBox(strTitle);
				return;
			}
		}
	}
	OnOK();
}

void CDlgApproachRunwaySelection::SetFlag(int nFlag)
{
	m_nFlag = nFlag;
}
