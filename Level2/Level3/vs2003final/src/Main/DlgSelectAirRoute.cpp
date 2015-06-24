// DlgSelectAirRoute.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgSelectAirRoute.h"
#include ".\dlgselectairroute.h"
#include "..\InputAirside\AirRoute.h"
#include "../InputAirside/InputAirside.h"

// CDlgSelectAirRoute dialog

IMPLEMENT_DYNAMIC(CDlgReportSelectAirRoute, CDialog)
CDlgReportSelectAirRoute::CDlgReportSelectAirRoute(int nProjID, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_REPORTSELECTAIRROUTE, pParent)
{
	m_nProjID = nProjID;
}

CDlgReportSelectAirRoute::~CDlgReportSelectAirRoute()
{
	std::vector<AirRouteList *>::iterator iter = m_vAirRouteList.begin();
	for (;iter != m_vAirRouteList.end(); ++iter)
	{
		delete *iter;
	}
	m_vAirRouteList.clear();

	m_vAirARouteSel.clear();
}

void CDlgReportSelectAirRoute::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_AIRROUTE, m_lbAirRoute);
}


BEGIN_MESSAGE_MAP(CDlgReportSelectAirRoute, CDialog)
END_MESSAGE_MAP()


// CDlgSelectAirRoute message handlers

void CDlgReportSelectAirRoute::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_lbAirRoute.GetSelCount() <= 0)
	{
		MessageBox(_T("Please select at leat one item."));
		return;
	}
	
	int nSelCount = m_lbAirRoute.GetSelCount();
	CArray<int,int>   aryListBoxSel;   
	aryListBoxSel.SetSize(nSelCount);   
	m_lbAirRoute.GetSelItems(nSelCount, aryListBoxSel.GetData());

	for (int nAirRoute = 0; nAirRoute < m_lbAirRoute.GetSelCount(); ++nAirRoute)
	{
		int nIndex = aryListBoxSel[nAirRoute];
		CAirRoute *pAirRoute = (CAirRoute *)m_lbAirRoute.GetItemData(nIndex);
		if(pAirRoute == NULL)//select all
		{
			m_vAirARouteSel.clear();
			m_vAirARouteSel.push_back(pAirRoute);
			break;
		}

		m_vAirARouteSel.push_back(pAirRoute);
	}

	CDialog::OnOK();
}

void CDlgReportSelectAirRoute::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	




	CDialog::OnCancel();
}

BOOL CDlgReportSelectAirRoute::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	int nAllIndex = m_lbAirRoute.AddString(_T("ALL"));
	m_lbAirRoute.SetItemData(nAllIndex,(DWORD_PTR)NULL);


	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);


		AirRouteList* airRouteList = new AirRouteList;
		m_vAirRouteList.push_back(airRouteList);
		airRouteList->ReadData(m_nProjID);
		int nAirRouteCount = airRouteList->GetAirRouteCount();
		for (int i = 0; i < nAirRouteCount; i++)
		{
			CAirRoute* pAirRoute = airRouteList->GetAirRoute(i);
			int nIndex = m_lbAirRoute.AddString(pAirRoute->getName());
			m_lbAirRoute.SetItemData(nIndex,(DWORD_PTR)pAirRoute);
			//if (m_nSelAirRouteID == pAirRoute->getID())
			//	m_wndAirRouteTree.SelectItem(hAirRoute);

		}

	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
