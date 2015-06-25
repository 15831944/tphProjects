#include "StdAfx.h"
#include "resource.h"
#include ".\postdeicingrouteassignmentdlg.h"
#include "..\InputAirside\BoundRouteAssignment.h"
#include "../InputAirside/PostDeice_BoundRoute.h"
#include "../InputAirside/Runway.h"
#include "DeicepadAndRunwaySelectDlg.h"
#include "InputAirside\ALTObjectGroup.h"

CPostDeicingRouteAssignmentDlg::CPostDeicingRouteAssignmentDlg(int nProjID,PFuncSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb,LPCTSTR lpszTitle,  CWnd* pParent)
:CBoundRouteAssignmentDlg(nProjID,pSelectFlightType,pAirPortdb,lpszTitle, pParent)
{
	m_pBoundRouteAssignment = new CBoundRouteAssignment(POSTDEICING, pAirPortdb);
	m_pBoundRouteAssignment->ReadData(m_nProjID);

}

CPostDeicingRouteAssignmentDlg::~CPostDeicingRouteAssignmentDlg(void)
{
}

void CPostDeicingRouteAssignmentDlg::NewOriginAndDestination()
{
	CDeicepadAndRunwaySelectDlg dlg(m_nProjID);

	if (dlg.DoModal() != IDOK)
		return;

	CString strDeiceFamilyName = dlg.GetDeiceFamilyName();
	CString strRunwaysName = dlg.GetRunwaysName();
	BOOL bAllRwy = dlg.IsAllRunway();
	BOOL bAllDeice = dlg.IsAllDeice();
	std::vector<int> vDeices = dlg.GetDeiceGroupList();
	std::vector<CPostDeice_BoundRoute::LogicRwyID> vRunways = dlg.GetRunwayList();


	m_pCurBoundRoute = new CPostDeice_BoundRoute;
	m_pCurBoundRoute->SetAllDeice(bAllDeice);
	m_pCurBoundRoute->SetAllRunway(bAllRwy);
	m_pCurBoundRoute->SetDeiceGroupIDList(vDeices);
	((CPostDeice_BoundRoute*)m_pCurBoundRoute)->SetLogicRunwayIDList(vRunways);

	m_pCurTimeAssignment->AddNewItem(m_pCurBoundRoute);

	int newItem = m_ListCtrlExitPositionAndStandFamily.InsertItem(m_ListCtrlExitPositionAndStandFamily.GetItemCount(), strDeiceFamilyName);
	m_ListCtrlExitPositionAndStandFamily.SetItemData(newItem, (DWORD_PTR)m_pCurBoundRoute);
	m_ListCtrlExitPositionAndStandFamily.SetItemText(newItem, 1, strRunwaysName);

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CPostDeicingRouteAssignmentDlg::EditOriginAndDestination()
{
	if (m_pCurBoundRoute == NULL)
		return;

	int nSelItem = m_ListCtrlExitPositionAndStandFamily.GetCurSel();

	CDeicepadAndRunwaySelectDlg dlg(m_nProjID);
	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	CString strDeiceFamilyName = dlg.GetDeiceFamilyName();
	CString strRunwaysName = dlg.GetRunwaysName();
	BOOL bAllRwy = dlg.IsAllRunway();
	BOOL bAllDeice = dlg.IsAllDeice();
	std::vector<int> vDeices = dlg.GetDeiceGroupList();
	std::vector< pair<int, int> > vRunways = dlg.GetRunwayList();


	m_pCurBoundRoute->SetAllDeice(bAllDeice);
	m_pCurBoundRoute->SetAllRunway(bAllRwy);
	m_pCurBoundRoute->SetDeiceGroupIDList(vDeices);
	((CPostDeice_BoundRoute*)m_pCurBoundRoute)->SetLogicRunwayIDList(vRunways);

	m_ListCtrlExitPositionAndStandFamily.SetItemText(nSelItem, 0, strDeiceFamilyName);
	m_ListCtrlExitPositionAndStandFamily.SetItemText(nSelItem, 1, strRunwaysName);

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CPostDeicingRouteAssignmentDlg::InitListCtrl( void )
{
	DWORD dwStyle = m_ListCtrlExitPositionAndStandFamily.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_ListCtrlExitPositionAndStandFamily.SetExtendedStyle(dwStyle);

	CStringList strList;
	strList.RemoveAll();
	CString strCaption;

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;

	strCaption= _T("DeicePad Group");
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 175;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_ListCtrlExitPositionAndStandFamily.InsertColumn(0, &lvc);

	strCaption = _T("Runway");
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 175;
	lvc.fmt = LVCFMT_NOEDIT;
	m_ListCtrlExitPositionAndStandFamily.InsertColumn(1, &lvc);

}

void CPostDeicingRouteAssignmentDlg::SetListContent( void )
{
	m_ListCtrlExitPositionAndStandFamily.DeleteAllItems();

	if (NULL == m_pBoundRouteAssignment)
		return;

	size_t count = m_pCurTimeAssignment->GetElementCount();
	for (size_t i=0; i<count; i++)
	{
		CIn_OutBoundRoute *pOutboundRoute = m_pCurTimeAssignment->GetItem(i);

		//set deice pad name 
		CString strDeiceName = "";
		if (pOutboundRoute->IsAllDeice())
		{
			strDeiceName = "All";
		}
		else
		{
			std::vector<int> vDeicepads;
			vDeicepads.clear();
			pOutboundRoute->GetDeiceGroupIDList(vDeicepads);
			int nSize = vDeicepads.size();
			for (int idxDeice =0; idxDeice < nSize; idxDeice++)
			{
				int nDeiceGrpID = vDeicepads.at(idxDeice);
				ALTObjectGroup altObjGroup;
				altObjGroup.ReadData(nDeiceGrpID);
				ALTObjectID altObjID = altObjGroup.getName();
				if (strDeiceName == "")
				{
					strDeiceName = altObjID.GetIDString();
				}
				else
				{
					strDeiceName += ", ";
					strDeiceName += altObjID.GetIDString();
				}
			}
		}

		int newItem = m_ListCtrlExitPositionAndStandFamily.InsertItem(i, strDeiceName);
		m_ListCtrlExitPositionAndStandFamily.SetItemData(i, (DWORD_PTR)pOutboundRoute);


		//set runway name 	
		CString strRunway = "";
		if (pOutboundRoute->IsAllRunway())
		{
			strRunway= "All";
		}
		else
		{
			std::vector<CPostDeice_BoundRoute::LogicRwyID> vRwyports;
			vRwyports.clear();
			((CPostDeice_BoundRoute*)pOutboundRoute)->GetLogicRunwayIDList(vRwyports) ;
			size_t nSize = vRwyports.size();
			for (size_t j =0; j < nSize; j++)
			{
				int nRwyID = vRwyports.at(j).first;
				Runway altRwy;
				altRwy.ReadObject(nRwyID);

				CString strName;
				if (vRwyports.at(j).second == 0)
					strName = altRwy.GetMarking1().c_str();
				else
					strName = altRwy.GetMarking2().c_str();

				if (strRunway == "")
				{
					strRunway = strName;
				}
				else
				{
					strRunway += ", ";
					strRunway += strName;
				}
			}
		}
		m_ListCtrlExitPositionAndStandFamily.SetItemText(newItem, 1, strRunway);
	}
}