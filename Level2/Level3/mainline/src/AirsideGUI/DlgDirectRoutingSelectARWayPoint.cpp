// DlgDirectRoutingSelectARWayPoint.cpp : implementation file
//

#include "stdafx.h"
//#include "AirsideGUI.h"
#include "DlgDirectRoutingSelectARWayPoint.h"
#include "..\InputAirside\AirRoute.h"
#include ".\dlgdirectroutingselectarwaypoint.h"
#include "../Database//DBElementCollection_Impl.h"
// CDlgDirectRoutingSelectARWayPoint dialog

IMPLEMENT_DYNAMIC(CDlgDirectRoutingSelectARWayPoint, CDialog)
CDlgDirectRoutingSelectARWayPoint::CDlgDirectRoutingSelectARWayPoint(int nProjID, CDirectRouting* pDirectRouting, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDirectRoutingSelectARWayPoint::IDD, pParent)
	,m_nProjID(nProjID)
	,m_pDirectRouting(pDirectRouting)
{
}

CDlgDirectRoutingSelectARWayPoint::~CDlgDirectRoutingSelectARWayPoint()
{
}

void CDlgDirectRoutingSelectARWayPoint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_WAYPOINTSTART, m_wndWayPointStartTree);
	DDX_Control(pDX, IDC_TREE_WAYPOINTEND, m_wndWayPointEndTree);
	DDX_Control(pDX, IDC_TREE_WAYPOINTMAX, m_wndWayPointMaxTree);
}


BEGIN_MESSAGE_MAP(CDlgDirectRoutingSelectARWayPoint, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_WAYPOINTSTART, OnTvnSelchangedTreeWaypointstart)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_WAYPOINTEND, OnTvnSelchangedTreeWaypointend)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_WAYPOINTMAX, OnTvnSelchangedTreeWaypointmax)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgDirectRoutingSelectARWayPoint message handlers

void CDlgDirectRoutingSelectARWayPoint::SetWayPointStartTree()
{		
	HTREEITEM hWayPointStart = m_wndWayPointStartTree.InsertItem(_T("WayPointStart:"));
	AirRouteList airRouteList;
	airRouteList.ReadData(m_nProjID);
	int nAirRouteCount = airRouteList.GetAirRouteCount();
	for (int i = 0; i < nAirRouteCount; i++)
	{
		CAirRoute* pAirRoute = airRouteList.GetAirRoute(i);
		HTREEITEM hAirRoute = m_wndWayPointStartTree.InsertItem(pAirRoute->getName(), hWayPointStart);
		
		int nARWayPointCount = pAirRoute->GetWayPointCount();
		for (int j = 0; j < nARWayPointCount; j++)
		{
			ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(j);
			HTREEITEM hARWaypoint = m_wndWayPointStartTree.InsertItem(pARWaypoint->getWaypoint().GetObjNameString(), hAirRoute);
			m_wndWayPointStartTree.SetItemData(hARWaypoint, pARWaypoint->getWaypoint().getID());
		}
		m_wndWayPointStartTree.Expand(hAirRoute, TVE_EXPAND);
	}
	m_wndWayPointStartTree.Expand(hWayPointStart, TVE_EXPAND);	
}
void CDlgDirectRoutingSelectARWayPoint::SetWayPointEndTree()
{
	m_wndWayPointEndTree.DeleteAllItems();
	HTREEITEM hWayPointEnd = m_wndWayPointEndTree.InsertItem(_T("WayPointEnd:"));
	HTREEITEM hSelItem = m_wndWayPointStartTree.GetSelectedItem();
	int nARWaypointStartID = m_wndWayPointStartTree.GetItemData(hSelItem);
	AirRouteList airRouteList;
	airRouteList.ReadData(m_nProjID);
	int nAirRouteCount = airRouteList.GetAirRouteCount();
	for (int i = 0; i < nAirRouteCount; i++)
	{
		CAirRoute* pAirRoute = airRouteList.GetAirRoute(i);
		int nARWayPointCount = pAirRoute->GetWayPointCount();
		for (int j = 0; j < nARWayPointCount; j++)
		{
			ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(j);
			if(pARWaypoint->getWaypoint().getID() == nARWaypointStartID)
			{
				HTREEITEM hAirRoute = m_wndWayPointEndTree.InsertItem(pAirRoute->getName(), hWayPointEnd);
				if(j > 0)
				{
					ARWaypoint* pARWaypointPre = pAirRoute->GetWayPointByIdx(j - 1);
					HTREEITEM hARWaypointPre = m_wndWayPointEndTree.InsertItem(pARWaypointPre->getWaypoint().GetObjNameString(), hAirRoute);
					m_wndWayPointEndTree.SetItemData(hARWaypointPre,pARWaypointPre->getWaypoint().getID());
				}
				if(j < nARWayPointCount - 1)
				{
					ARWaypoint* pARWaypointNext = pAirRoute->GetWayPointByIdx(j + 1);
					HTREEITEM hARWaypointNext = m_wndWayPointEndTree.InsertItem(pARWaypointNext->getWaypoint().GetObjNameString(), hAirRoute);	
					m_wndWayPointEndTree.SetItemData(hARWaypointNext,pARWaypointNext->getWaypoint().getID());
				}
				m_wndWayPointEndTree.Expand(hAirRoute, TVE_EXPAND);
			}			
		}
	}
	m_wndWayPointEndTree.Expand(hWayPointEnd, TVE_EXPAND);
}
void CDlgDirectRoutingSelectARWayPoint::SetWayPointMaxTree()
{
	m_wndWayPointMaxTree.DeleteAllItems();
	HTREEITEM hWayPointMax = m_wndWayPointMaxTree.InsertItem(_T("WayPointMax:"));
	HTREEITEM hSelStartItem = m_wndWayPointStartTree.GetSelectedItem();
	HTREEITEM hSelEndItem = m_wndWayPointEndTree.GetSelectedItem();
	int nARWaypointStartID = m_wndWayPointStartTree.GetItemData(hSelStartItem);
	int nARWaypointEndID = m_wndWayPointEndTree.GetItemData(hSelEndItem);
	m_pDirectRouting->SetARWaypointStartID(nARWaypointStartID);
	m_pDirectRouting->SetARWaypointEndID(nARWaypointEndID);
	AirRouteList airRouteList;
	airRouteList.ReadData(m_nProjID);
	int nAirRouteCount = airRouteList.GetAirRouteCount();
	for (int i = 0; i < nAirRouteCount; i++)
	{
		CAirRoute* pAirRoute = airRouteList.GetAirRoute(i);
		int nARWayPointCount = pAirRoute->GetWayPointCount();
		for (int j = 0; j < nARWayPointCount - 1; j++)
		{
			ARWaypoint* pARWaypoint1 = pAirRoute->GetWayPointByIdx(j);
			ARWaypoint* pARWaypoint2 = pAirRoute->GetWayPointByIdx(j+1);
			if((pARWaypoint1->getWaypoint().getID() == nARWaypointStartID && pARWaypoint2->getWaypoint().getID() == nARWaypointEndID)
				|| (pARWaypoint1->getWaypoint().getID() == nARWaypointEndID && pARWaypoint2->getWaypoint().getID() == nARWaypointStartID))
			{
				HTREEITEM hAirRoute = m_wndWayPointMaxTree.InsertItem(pAirRoute->getName(), hWayPointMax);
				for (int k = 0; k < nARWayPointCount; k++)
				{
					ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(k);
					HTREEITEM hARWaypoint = m_wndWayPointMaxTree.InsertItem(pARWaypoint->getWaypoint().GetObjNameString(), hAirRoute);
					m_wndWayPointMaxTree.SetItemData(hARWaypoint, pARWaypoint->getWaypoint().getID());
				}
				m_wndWayPointMaxTree.Expand(hAirRoute, TVE_EXPAND);
			}
		}
	}
	m_wndWayPointMaxTree.Expand(hWayPointMax, TVE_EXPAND);
}
BOOL CDlgDirectRoutingSelectARWayPoint::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWayPointStartTree();
	m_wndWayPointEndTree.InsertItem(_T("WayPointEnd:"));
	m_wndWayPointMaxTree.InsertItem(_T("WayPointMax:"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDirectRoutingSelectARWayPoint::OnTvnSelchangedTreeWaypointstart(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_wndWayPointMaxTree.DeleteAllItems();
	SetWayPointEndTree();
	*pResult = 0;
}

void CDlgDirectRoutingSelectARWayPoint::OnTvnSelchangedTreeWaypointend(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	SetWayPointMaxTree();
	*pResult = 0;
}

void CDlgDirectRoutingSelectARWayPoint::OnTvnSelchangedTreeWaypointmax(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hSelItem = m_wndWayPointMaxTree.GetSelectedItem();
	int nARWaypointMaxID = m_wndWayPointMaxTree.GetItemData(hSelItem);
	m_pDirectRouting->SetARWaypointMaxID(nARWaypointMaxID);
	*pResult = 0;
}


void CDlgDirectRoutingSelectARWayPoint::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	OnOK();
}
