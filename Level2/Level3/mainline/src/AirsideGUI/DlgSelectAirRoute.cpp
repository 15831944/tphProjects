// DlgSelectAirRoute.cpp : implementation file
//
#include "stdafx.h"
#include "Resource.h"
#include "DlgSelectAirRoute.h"
#include "../InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/AirRoute.h"
#include "InputAirside/FlightPlan.h"

// CDlgSelectAirRoute dialog

CDlgSelectAirRoute::CDlgSelectAirRoute(int nProjID,ns_FlightPlan::FlightPlanSpecific *pCurFltPlan, int nSelAirRouteID, CWnd* pParent /*=NULL*/)
 : CDialog(IDD_SELECTAIRROUTE, pParent)
 , m_nProjID(nProjID)
 , m_nSelAirRouteID(nSelAirRouteID)
{
	ASSERT(pCurFltPlan != NULL);
	m_pCurFltPlan = pCurFltPlan;
}

CDlgSelectAirRoute::~CDlgSelectAirRoute()
{
}

void CDlgSelectAirRoute::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_AIRROUTES, m_wndAirRouteTree);
}


BEGIN_MESSAGE_MAP(CDlgSelectAirRoute, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_AIRROUTES, OnSelChangedTreeAirroutes)
END_MESSAGE_MAP()

BOOL CDlgSelectAirRoute::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	SetTreeContents();

	return TRUE;
}

void CDlgSelectAirRoute::SetTreeContents()
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		airport.getName();
		HTREEITEM hAirport = m_wndAirRouteTree.InsertItem(airport.getName());
		m_wndAirRouteTree.SetItemData(hAirport, *iterAirportID);

		AirRouteList airRouteList;
		airRouteList.ReadData(m_nProjID);
		int nAirRouteCount = airRouteList.GetAirRouteCount();
		for (int i = 0; i < nAirRouteCount; i++)
		{
			CAirRoute* pAirRoute = airRouteList.GetAirRoute(i);
			AddAirRouteToTree(pAirRoute,hAirport);
			//if (m_nSelAirRouteID == pAirRoute->getID())
			//	m_wndAirRouteTree.SelectItem(hAirRoute);
		}

		m_wndAirRouteTree.Expand(hAirport, TVE_EXPAND);
	}
}
void CDlgSelectAirRoute::AddAirRouteToTree(CAirRoute* pAirRoute,HTREEITEM hParentItem)
{	
	if(m_pCurFltPlan->GetOperationType() == ns_FlightPlan::FlightPlanSpecific::takeoff)
	{
		if (pAirRoute->getRouteType() == CAirRoute::STAR || pAirRoute->getRouteType() == CAirRoute::CIRCUIT)
			return;

		if (m_pCurFltPlan->GetAirRouteCount() == 0)
		{

		}
		else
		{
			if (pAirRoute->getRouteType() == CAirRoute::SID)
				return;

		}
	}

	if (m_pCurFltPlan->GetOperationType() == ns_FlightPlan::FlightPlanSpecific::enroute)
	{
		if (pAirRoute->getRouteType() == CAirRoute::STAR || pAirRoute->getRouteType() == CAirRoute::SID 
				|| pAirRoute->getRouteType() == CAirRoute::CIRCUIT)
			return;

	}

	if (m_pCurFltPlan->GetOperationType() == ns_FlightPlan::FlightPlanSpecific::landing)
	{
		if (pAirRoute->getRouteType() == CAirRoute::SID || pAirRoute->getRouteType() == CAirRoute::CIRCUIT)
			return;

		int nCount = m_pCurFltPlan->GetAirRouteCount();
		if(nCount > 0 )
		{
			if(m_pCurFltPlan->GetItem(nCount-1)->GetAirRoute().getRouteType() == CAirRoute::STAR)
				return;
		}
	}

	if (m_pCurFltPlan->GetOperationType() == ns_FlightPlan::FlightPlanSpecific::Circuit)
	{
		if (pAirRoute->getRouteType() != CAirRoute::CIRCUIT)
			return;

	}
	
	//don't have intersection
	size_t nFltPlanARCount = m_pCurFltPlan->GetAirRouteCount();
	if ( nFltPlanARCount> 0)
	{
		if(m_pCurFltPlan->GetItem(nFltPlanARCount -1 )->GetAirRoute().HasIntersection(pAirRoute) == -1)
			return;
	}
	
	HTREEITEM hAirRoute = m_wndAirRouteTree.InsertItem(pAirRoute->getName(), hParentItem);
	m_wndAirRouteTree.SetItemData(hAirRoute, pAirRoute->getID());
	


}
void CDlgSelectAirRoute::OnSelChangedTreeAirroutes(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hSelItem = m_wndAirRouteTree.GetSelectedItem();
	if (hSelItem != NULL && !IsAirportItem(hSelItem))
	{
		m_nSelAirRouteID = m_wndAirRouteTree.GetItemData(hSelItem);
		GetDlgItem(IDOK)->EnableWindow();
	}
	else
	{
		m_nSelAirRouteID = -1;
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}

	*pResult = 0;
}

bool CDlgSelectAirRoute::IsAirportItem(HTREEITEM hItem)
{
	if (m_wndAirRouteTree.GetParentItem(hItem) == NULL)
		return true;

	return false;
}
