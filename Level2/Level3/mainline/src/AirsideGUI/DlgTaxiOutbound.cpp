#include "StdAfx.h"
#include "resource.h"
#include ".\dlgtaxioutbound.h"
#include "..\Common\AirportDatabase.h"

CDlgTaxiOutbound::CDlgTaxiOutbound(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB)
	:CNECRelatedBaseDlg(pSelectFlightType, _T("Taxi Outbound Performance Specification"))
{
	m_pTaxiOutbound = new CTaxiOutbound(pAirportDB);
	m_pTaxiOutbound->ReadData(nProjID);
}

CDlgTaxiOutbound::~CDlgTaxiOutbound(void)
{

}


void CDlgTaxiOutbound::ReloadData()
{
//	m_pTaxiOutbound->ReadData();
}

void CDlgTaxiOutbound::SetListColumn()
{
	CStringList strList;

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.fmt = LVCFMT_EDIT;
	lvc.csList = &strList;


	lvc.pszText = "Acceleration(m/s^2)";
	lvc.cx = 130;
	lvc.fmt = LVCFMT_EDIT;
	m_wndListCtrl.InsertColumn(1, &lvc);

	lvc.pszText = "Deceleration(m/s^2)";
	lvc.cx = 130;
	lvc.fmt = LVCFMT_EDIT;
	m_wndListCtrl.InsertColumn(2, &lvc);

	lvc.pszText = "Normal Taxi Speed (knots)";
	lvc.cx = 150;
	m_wndListCtrl.InsertColumn(3, &lvc);

	lvc.pszText = "Max Taxi Speed (knots)";
	m_wndListCtrl.InsertColumn(4, &lvc);

	lvc.pszText = "Intersection Buffer(m)";
	m_wndListCtrl.InsertColumn(5, &lvc);

	lvc.pszText = "Min Separation in Q(m)";
	m_wndListCtrl.InsertColumn(6, &lvc);

	lvc.pszText = "Max Separation in Q(m)";
	m_wndListCtrl.InsertColumn(7, &lvc);

	lvc.pszText = "Longitudinal Separation(m)";
	m_wndListCtrl.InsertColumn(8, &lvc);

	lvc.pszText = "Staggered Separation(m)";
	m_wndListCtrl.InsertColumn(9, &lvc);


	lvc.pszText = "Fuel Burn(lhs/hr)";
	m_wndListCtrl.InsertColumn(10, &lvc);

	lvc.pszText = "Cost($/hr)";
	lvc.cx = 120;
	m_wndListCtrl.InsertColumn(11, &lvc);
}

void CDlgTaxiOutbound::SetListContent()
{
	int nCount = m_pTaxiOutbound->GetCount();

	for (int n = 0; n < nCount; n++)
	{
		CTaxiOutboundNEC* pTaxiOutboundNEC = m_pTaxiOutbound->GetRecordByIdx(n);

		char szFltType[1024] = {0};
		pTaxiOutboundNEC->GetFltType().screenPrint(szFltType);
		m_wndListCtrl.InsertItem(n, szFltType);

		CString strText;

		strText.Format("%.2f", pTaxiOutboundNEC->GetAcceleration());
		m_wndListCtrl.SetItemText(n, 1, strText);

		strText.Format("%.2f", pTaxiOutboundNEC->GetDeceleration());
		m_wndListCtrl.SetItemText(n, 2, strText);

		strText.Format("%.2f", pTaxiOutboundNEC->GetNormalSpeed());
		m_wndListCtrl.SetItemText(n, 3, strText);

		strText.Format("%.2f", pTaxiOutboundNEC->GetMaxSpeed());
		m_wndListCtrl.SetItemText(n, 4, strText);

		strText.Format("%.2f", pTaxiOutboundNEC->GetIntersectionBuffer());
		m_wndListCtrl.SetItemText(n, 5, strText);

		strText.Format("%.2f", pTaxiOutboundNEC->GetMinSeparation());
		m_wndListCtrl.SetItemText(n, 6, strText);

		strText.Format("%.2f", pTaxiOutboundNEC->GetMaxSeparation());
		m_wndListCtrl.SetItemText(n, 7, strText);

		strText.Format("%.2f", pTaxiOutboundNEC->GetLongitudinalSeperation());
		m_wndListCtrl.SetItemText(n, 8, strText);

		strText.Format("%.2f", pTaxiOutboundNEC->GetStaggeredSeperation());
		m_wndListCtrl.SetItemText(n, 9, strText);

		strText.Format("%.2f", pTaxiOutboundNEC->GetFuelBurn());
		m_wndListCtrl.SetItemText(n, 10, strText);

		strText.Format("%.2f", pTaxiOutboundNEC->GetCostPerHour());
		m_wndListCtrl.SetItemText(n, 11, strText);

		int nID = pTaxiOutboundNEC->GetID();
		m_wndListCtrl.SetItemData(n, nID);
	}	
}
void CDlgTaxiOutbound::CheckListContent()
{
	int nCount = m_pTaxiOutbound->GetCount();
	if(nCount == 0)
	{
		CTaxiOutboundNEC toNEC;
		try
		{
			m_pTaxiOutbound->AddRecord(toNEC);
			GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
		}
		catch(_com_error& e)
		{
			AfxMessageBox(e.Description());
		}
	}
}
void CDlgTaxiOutbound::OnNewItem(FlightConstraint& fltType)
{
	CTaxiOutboundNEC toNEC;
	toNEC.SetFltType(fltType);

	try
	{
		m_pTaxiOutbound->AddRecord(toNEC);

		int nIndex = m_wndListCtrl.GetItemCount()-1;
		CString strText;

		strText.Format("%.2f", toNEC.GetAcceleration());
		m_wndListCtrl.SetItemText(nIndex, 1, strText);

		strText.Format("%.2f", toNEC.GetDeceleration());
		m_wndListCtrl.SetItemText(nIndex, 2, strText);

		strText.Format("%.2f", toNEC.GetNormalSpeed() );
		m_wndListCtrl.SetItemText(nIndex, 3, strText);

		strText.Format("%.2f", toNEC.GetMaxSpeed());
		m_wndListCtrl.SetItemText(nIndex, 4, strText);

		strText.Format("%.2f", toNEC.GetIntersectionBuffer());
		m_wndListCtrl.SetItemText(nIndex, 5, strText);

		strText.Format("%.2f", toNEC.GetMinSeparation());
		m_wndListCtrl.SetItemText(nIndex, 6, strText);

		strText.Format("%.2f", toNEC.GetMaxSeparation());
		m_wndListCtrl.SetItemText(nIndex, 7, strText);

		strText.Format("%.2f", toNEC.GetLongitudinalSeperation());
		m_wndListCtrl.SetItemText(nIndex, 8, strText);

		strText.Format("%.2f", toNEC.GetStaggeredSeperation());
		m_wndListCtrl.SetItemText(nIndex, 9, strText);

		strText.Format("%.2f", toNEC.GetFuelBurn());
		m_wndListCtrl.SetItemText(nIndex, 10, strText);

		strText.Format("%.2f", toNEC.GetCostPerHour());
		m_wndListCtrl.SetItemText(nIndex, 11, strText);

		m_wndListCtrl.SetItemData(nIndex, toNEC.GetID());
	}
	catch(_com_error& e)
	{
		AfxMessageBox(e.Description());
	}

}

void CDlgTaxiOutbound::OnDelItem()
{
	int nItem = GetSelectedListItem();
	m_pTaxiOutbound->DeleteRecord(nItem);
	m_wndListCtrl.DeleteItemEx(nItem);
}

//void CDlgTaxiOutbound::OnEditItem()
//{
//	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
//	if (pos != NULL)
//		m_wndListCtrl.EditSubLabel(0, 1);
//
//}

void CDlgTaxiOutbound::OnListItemChanged(int nItem, int nSubItem)
{
	CTaxiOutboundNEC* pToNEC = m_pTaxiOutbound->GetRecordByIdx(nItem);

	CString strText = m_wndListCtrl.GetItemText(nItem, nSubItem);
	float fValue = (float)::atof(strText.GetBuffer(0));

	switch(nSubItem)
	{
	case  1:
		pToNEC->SetAcceleration(fValue);
		break;
	case  2:
		pToNEC->SetDeceleration(fValue);
		break;
	case 3:
		pToNEC->SetNormalSpeed(fValue);
		break;

	case 4:
		pToNEC->SetMaxSpeed(fValue);
		break;

	case 5:
		pToNEC->SetIntersectionBuffer(fValue);
		break;

	case 6:
		pToNEC->SetMinSeparation(fValue);
		break;

	case 7:
		pToNEC->SetMaxSeparation(fValue);
		break;

	case 8:
		pToNEC->SetLongitudinalSeperation(fValue);
		break;

	case 9:
		pToNEC->SetStaggeredSeperation(fValue);
		break;

	case 10:
		pToNEC->SetFuelBurn(fValue);

	case 11:
		pToNEC->SetCostPerHour(fValue);

	default:
		break;
	}

	m_pTaxiOutbound->UpdateRecord(*pToNEC);
}

void CDlgTaxiOutbound::OnNECItemChanged(int nItem, int nNewNECTableID)
{
	//CTaxiOutboundNEC* pToNEC = m_pTaxiOutbound->GetRecordByIdx(nItem);

	//pToNEC->SetNECID(nNewNECTableID);

	//m_pTaxiOutbound->UpdateRecord(*pToNEC);
}

BOOL CDlgTaxiOutbound::OnInitDialog()
{
	CNECRelatedBaseDlg::OnInitDialog();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
