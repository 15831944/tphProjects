#include "StdAfx.h"
#include "Resource.h"
#include "DlgSelectRunway.h"
#include "../InputAirside/InputAirside.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/Runway.h"

CDlgSelectRunway::CDlgSelectRunway(int nProjID, int nSelRunwayID /*= -1*/, CWnd* pParent /*= NULL*/)
 : CDialog(IDD_SELECTAIRROUTE, pParent)
 , m_nProjID(nProjID)
 , m_nSelRunwayID(nSelRunwayID)
{
}

CDlgSelectRunway::~CDlgSelectRunway()
{
}

void CDlgSelectRunway::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_AIRROUTES, m_wndRunwayTree);
}


BEGIN_MESSAGE_MAP(CDlgSelectRunway, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_AIRROUTES, OnSelChangedTreeRunways)
END_MESSAGE_MAP()

BOOL CDlgSelectRunway::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(_T("Select Runway"));

	SetTreeContents();
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	return TRUE;
}

void CDlgSelectRunway::SetTreeContents()
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		airport.getName();
		HTREEITEM hAirport = m_wndRunwayTree.InsertItem(airport.getName());
		m_wndRunwayTree.SetItemData(hAirport, *iterAirportID);

		std::vector<int> vRunways;
		ALTAirport::GetRunwaysIDs(*iterAirportID, vRunways);
		for (std::vector<int>::iterator iterRunwayID = vRunways.begin(); iterRunwayID != vRunways.end(); ++iterRunwayID)
		{
			Runway runway;
			runway.ReadObject(*iterRunwayID);
			HTREEITEM hRunwayItem = m_wndRunwayTree.InsertItem(runway.GetObjNameString(), hAirport);
			m_wndRunwayTree.SetItemData(hRunwayItem, *iterRunwayID);
			
			HTREEITEM hMarking1Item = m_wndRunwayTree.InsertItem(runway.GetMarking1().c_str(), hRunwayItem);
			m_wndRunwayTree.SetItemData(hMarking1Item, (int)RUNWAYMARK_FIRST);
			HTREEITEM hMarking2Item = m_wndRunwayTree.InsertItem(runway.GetMarking2().c_str(), hRunwayItem);
			m_wndRunwayTree.SetItemData(hMarking2Item, (int)RUNWAYMARK_SECOND);

			m_wndRunwayTree.Expand(hRunwayItem, TVE_EXPAND);

		}

		m_wndRunwayTree.Expand(hAirport, TVE_EXPAND);
	}
}

void CDlgSelectRunway::OnSelChangedTreeRunways(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hSelItem = m_wndRunwayTree.GetSelectedItem();
	if (hSelItem != NULL && IsRunwayMarkingItem(hSelItem))
	{
		m_nSelRunwayMarkingIndex = m_wndRunwayTree.GetItemData(hSelItem);
		HTREEITEM hRunwayItem = m_wndRunwayTree.GetParentItem(hSelItem);
		m_nSelRunwayID = m_wndRunwayTree.GetItemData(hRunwayItem);
		HTREEITEM hAirportItem = m_wndRunwayTree.GetParentItem(hRunwayItem);
		m_nSelAirportID = m_wndRunwayTree.GetItemData(hAirportItem);
		
		GetDlgItem(IDOK)->EnableWindow();
	}
	else
	{
		m_nSelRunwayID = -1;
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}

	*pResult = 0;
}

bool CDlgSelectRunway::IsAirportItem(HTREEITEM hItem)
{
	if (m_wndRunwayTree.GetParentItem(hItem) == NULL)
		return true;

	return false;
}

bool CDlgSelectRunway::IsRunwayMarkingItem(HTREEITEM hItem)
{
	HTREEITEM hParentItem = m_wndRunwayTree.GetParentItem(hItem);
	return (hParentItem != NULL && m_wndRunwayTree.GetParentItem(hParentItem) != NULL);
}

