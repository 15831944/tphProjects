#include "StdAfx.h"
#include "resource.h"
#include ".\deicerouteassigmentdlg.h"
#include "..\InputAirside\BoundRouteAssignment.h"
#include "../InputAirside/ALTObjectGroup.h"
#include "DeicepadAndStandFamilySelectDlg.h"


CDeiceRouteAssigmentDlg::CDeiceRouteAssigmentDlg( int nProjID,PFuncSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb,LPCTSTR lpszTitle, CWnd* pParent /*= NULL*/ )
:CBoundRouteAssignmentDlg(nProjID,pSelectFlightType,pAirPortdb,lpszTitle, pParent)
{
	m_pBoundRouteAssignment = new CBoundRouteAssignment(DEICING, pAirPortdb);
	m_pBoundRouteAssignment->ReadData(m_nProjID);

}

void CDeiceRouteAssigmentDlg::InitListCtrl( void )
{
	DWORD dwStyle = m_ListCtrlExitPositionAndStandFamily.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_ListCtrlExitPositionAndStandFamily.SetExtendedStyle(dwStyle);

	CStringList strList;
	strList.RemoveAll();
	CString strCaption;

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
  
	strCaption.LoadString(IDS_STANDFAMILY);
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 170;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_ListCtrlExitPositionAndStandFamily.InsertColumn(0, &lvc);

	strCaption = _T("DeicePad Group");
	lvc.pszText = (LPSTR)(LPCTSTR)strCaption;
	lvc.cx = 170;
	lvc.fmt = LVCFMT_NOEDIT;
	m_ListCtrlExitPositionAndStandFamily.InsertColumn(1, &lvc);

}

void CDeiceRouteAssigmentDlg::SetListContent( void )
{
	m_ListCtrlExitPositionAndStandFamily.DeleteAllItems();

	if (NULL == m_pBoundRouteAssignment)
		return;

	size_t count = m_pCurTimeAssignment->GetElementCount();
	for (size_t i=0; i<count; i++)
	{
		CIn_OutBoundRoute *pOutboundRoute = m_pCurTimeAssignment->GetItem(i);

		//set stand name 	
		CString strStandFamily = "";
		if (pOutboundRoute->IsAllStand())
		{
			strStandFamily = "All";
		}
		else
		{
			std::vector<int> vStandFmailys;
			vStandFmailys.clear();
			pOutboundRoute->GetStandGroupIDList(vStandFmailys);
			int nSize = vStandFmailys.size();
			for (int j =0; j < nSize; j++)
			{
				int nStandFamilyID = vStandFmailys.at(j);
				ALTObjectGroup altObjGroup;
				altObjGroup.ReadData(nStandFamilyID);
				ALTObjectID altObjID = altObjGroup.getName();
				if (strStandFamily == "")
				{
					strStandFamily = altObjID.GetIDString();
				}
				else
				{
					strStandFamily += ", ";
					strStandFamily += altObjID.GetIDString();
				}
			}
		}

		int newItem = m_ListCtrlExitPositionAndStandFamily.InsertItem(i, strStandFamily);
		m_ListCtrlExitPositionAndStandFamily.SetItemData(i, (DWORD_PTR)pOutboundRoute);

		//set takeoff position name 
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

		m_ListCtrlExitPositionAndStandFamily.SetItemText(newItem, 1, strDeiceName);
	}
}

CDeiceRouteAssigmentDlg::~CDeiceRouteAssigmentDlg()
{

}

void CDeiceRouteAssigmentDlg::NewOriginAndDestination()
{
	CDeicepadAndStandFamilySelectDlg dlg(m_nProjID);

	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	CString strDeiceFamilyName = dlg.GetDeiceFamilyName();
	CString strStandFamilyName = dlg.GetStandFamilyName();
	BOOL bAllStand = dlg.IsAllStand();
	BOOL bAllDeice = dlg.IsAllDeice();
	std::vector<int> vDeices = dlg.GetDeiceGroupList();
	std::vector<int> vStands = dlg.GetStandGroupList();


	m_pCurBoundRoute = new CDeice_BoundRoute;
	m_pCurBoundRoute->SetAllDeice(bAllDeice);
	m_pCurBoundRoute->SetAllStand(bAllStand);
	m_pCurBoundRoute->SetDeiceGroupIDList(vDeices);
	m_pCurBoundRoute->SetStandGroupIDList(vStands);

	m_pCurTimeAssignment->AddNewItem(m_pCurBoundRoute);

	int newItem = m_ListCtrlExitPositionAndStandFamily.InsertItem(m_ListCtrlExitPositionAndStandFamily.GetItemCount(), strStandFamilyName);
	m_ListCtrlExitPositionAndStandFamily.SetItemData(newItem, (DWORD_PTR)m_pCurBoundRoute);
	m_ListCtrlExitPositionAndStandFamily.SetItemText(newItem, 1, strDeiceFamilyName);

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CDeiceRouteAssigmentDlg::EditOriginAndDestination()
{
	if (m_pCurBoundRoute == NULL)
		return;

	int nSelItem = m_ListCtrlExitPositionAndStandFamily.GetCurSel();

	CDeicepadAndStandFamilySelectDlg dlg(m_nProjID);
	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	CString strDeiceFamilyName = dlg.GetDeiceFamilyName();
	CString strStandFamilyName = dlg.GetStandFamilyName();
	BOOL bAllStand = dlg.IsAllStand();
	BOOL bAllDeice = dlg.IsAllDeice();
	std::vector<int> vDeices = dlg.GetDeiceGroupList();
	std::vector<int> vStands = dlg.GetStandGroupList();


	m_pCurBoundRoute->SetAllDeice(bAllDeice);
	m_pCurBoundRoute->SetAllStand(bAllStand);
	m_pCurBoundRoute->SetDeiceGroupIDList(vDeices);
	m_pCurBoundRoute->SetStandGroupIDList(vStands);

	m_ListCtrlExitPositionAndStandFamily.SetItemText(nSelItem, 0, strStandFamilyName);
	m_ListCtrlExitPositionAndStandFamily.SetItemText(nSelItem, 1, strDeiceFamilyName);

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}