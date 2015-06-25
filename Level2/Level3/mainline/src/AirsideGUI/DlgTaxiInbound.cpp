#include "stdafx.h"
#include "resource.h"
#include "..\InputAirside\TaxiInbound.h"
#include "..\Common\AirportDatabase.h"
#include ".\dlgtaxiinbound.h"

CDlgTaxiInbound::CDlgTaxiInbound(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB, LPCTSTR lpszCatpion, CWnd* pParent/* = NULL*/)
: CNECRelatedBaseDlg(pSelectFlightType, lpszCatpion, pParent)
{
	m_pTaxiInbound = new CTaxiInbound(pAirportDB);
	m_pTaxiInbound->ReadData(nProjID);
}

CDlgTaxiInbound::~CDlgTaxiInbound()
{
}

void CDlgTaxiInbound::DoDataExchange(CDataExchange* pDX)
{
	CNECRelatedBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgTaxiInbound, CNECRelatedBaseDlg)
END_MESSAGE_MAP()


// CDlgTaxiInbound message handlers
void CDlgTaxiInbound::ReloadData()
{
//	m_pTaxiInbound->ReadData();
}

void CDlgTaxiInbound::SetListColumn()
{
	CStringList strList;
	strList.RemoveAll();

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.csList = &strList;
    
	lvc.pszText = "Acceleration(m/s^2)";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_EDIT;
	m_wndListCtrl.InsertColumn(1, &lvc);

	lvc.pszText = "Deceleration(m/s^2)";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_EDIT;
	m_wndListCtrl.InsertColumn(2, &lvc);

	lvc.pszText = "Normal Taxi Speed (kts)";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_EDIT;
	m_wndListCtrl.InsertColumn(3, &lvc);

	lvc.pszText = "Max Taxi Speed(kts)";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_EDIT;
	m_wndListCtrl.InsertColumn(4, &lvc);

	lvc.pszText = "Intersection Buffer(m)";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_EDIT;
	m_wndListCtrl.InsertColumn(5, &lvc);

	lvc.pszText = "Min Separation in Q(m)";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_EDIT;
	m_wndListCtrl.InsertColumn(6, &lvc);

	lvc.pszText = "Max Separation in Q(m)";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_EDIT;
	m_wndListCtrl.InsertColumn(7, &lvc);

	lvc.pszText = "Longitudinal Separation(m)";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_EDIT;
	m_wndListCtrl.InsertColumn(8, &lvc);

	lvc.pszText = "Staggered Separation(m)";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_EDIT;
	m_wndListCtrl.InsertColumn(9, &lvc);


	lvc.pszText = "Fuel Burn(lbs/hr)";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_EDIT;
	m_wndListCtrl.InsertColumn(10, &lvc);


	lvc.pszText = "Cost($/hr)";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_EDIT;
	m_wndListCtrl.InsertColumn(11, &lvc);
}

void CDlgTaxiInbound::SetListContent()
{
	CTaxiInboundNECManager* pManager = m_pTaxiInbound->GetTaxiInboundNECList();
	for (int n = 0; n < pManager->GetCount(); n++)
	{
		CTaxiInboundNEC* pTaxiInboundNEC = pManager->GetRecordByIdx(n);
		
		char szFltType[1024] = {0};
		pTaxiInboundNEC->GetFltType().screenPrint(szFltType);
		m_wndListCtrl.InsertItem(n, szFltType);

		CString strText;

		strText.Format("%.2f", pTaxiInboundNEC->GetAcceleration());
		m_wndListCtrl.SetItemText(n, 1, strText);

		strText.Format("%.2f", pTaxiInboundNEC->GetDeceleration());
		m_wndListCtrl.SetItemText(n, 2, strText);

		strText.Format("%.2f", pTaxiInboundNEC->GetNormalSpeed());
		m_wndListCtrl.SetItemText(n, 3, strText);

		strText.Format("%.2f", pTaxiInboundNEC->GetMaxSpeed());
		m_wndListCtrl.SetItemText(n, 4, strText);

		strText.Format("%.2f", pTaxiInboundNEC->GetIntersectionBuffer());
		m_wndListCtrl.SetItemText(n, 5, strText);

		strText.Format("%.2f", pTaxiInboundNEC->GetMinSeparation());
		m_wndListCtrl.SetItemText(n, 6, strText);

		strText.Format("%.2f", pTaxiInboundNEC->GetMaxSeparation());
		m_wndListCtrl.SetItemText(n, 7, strText);

		strText.Format("%.2f", pTaxiInboundNEC->GetLongitudinalSeperation());
		m_wndListCtrl.SetItemText(n, 8, strText);

		strText.Format("%.2f", pTaxiInboundNEC->GetStaggeredSeperation());
		m_wndListCtrl.SetItemText(n, 9, strText);

		strText.Format("%.2f", pTaxiInboundNEC->GetFuelBurn());
		m_wndListCtrl.SetItemText(n, 10, strText);

		strText.Format("%.2f", pTaxiInboundNEC->GetCostPerHour());
		m_wndListCtrl.SetItemText(n, 11, strText);

		int nID = pTaxiInboundNEC->GetID();
		m_wndListCtrl.SetItemData(n, nID);
	}
}

void CDlgTaxiInbound::CheckListContent()
{
	CTaxiInboundNECManager* pManager = m_pTaxiInbound->GetTaxiInboundNECList();
	int nCount = pManager->GetCount();
	if(nCount == 0)
	{
		CTaxiInboundNEC tiNEC;
		try
		{
			m_pTaxiInbound->AddRecord(tiNEC);

			GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
		}
		catch (_com_error& e)
		{
			AfxMessageBox((LPCSTR)e.Description());
		}
	}
}
void CDlgTaxiInbound::OnNewItem(FlightConstraint& fltType)
{
	CTaxiInboundNEC tiNEC;
	tiNEC.SetFltType(fltType);

	try
	{
		m_pTaxiInbound->AddRecord(tiNEC);

		int nIndex = m_wndListCtrl.GetItemCount() - 1;

		CString strText;


		strText.Format("%.2f", tiNEC.GetAcceleration());
		m_wndListCtrl.SetItemText(nIndex, 1, strText);

		strText.Format("%.2f", tiNEC.GetDeceleration());
		m_wndListCtrl.SetItemText(nIndex, 2, strText);

		strText.Format("%.2f", tiNEC.GetNormalSpeed());
		m_wndListCtrl.SetItemText(nIndex, 3, strText);

		strText.Format("%.2f", tiNEC.GetMaxSpeed());
		m_wndListCtrl.SetItemText(nIndex, 4, strText);

		strText.Format("%.2f", tiNEC.GetIntersectionBuffer());
		m_wndListCtrl.SetItemText(nIndex, 5, strText);

		strText.Format("%.2f", tiNEC.GetMinSeparation());
		m_wndListCtrl.SetItemText(nIndex, 6, strText);

		strText.Format("%.2f", tiNEC.GetMaxSeparation());
		m_wndListCtrl.SetItemText(nIndex, 7, strText);

		strText.Format("%.2f", tiNEC.GetLongitudinalSeperation());
		m_wndListCtrl.SetItemText(nIndex, 8, strText);

		strText.Format("%.2f", tiNEC.GetStaggeredSeperation());
		m_wndListCtrl.SetItemText(nIndex, 9, strText);

		strText.Format("%.2f", tiNEC.GetFuelBurn());
		m_wndListCtrl.SetItemText(nIndex, 10, strText);

		strText.Format("%.2f", tiNEC.GetCostPerHour());
		m_wndListCtrl.SetItemText(nIndex, 11, strText);

		m_wndListCtrl.SetItemData(nIndex, tiNEC.GetID());

		GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	}
	catch (_com_error& e)
	{
		AfxMessageBox((LPCSTR)e.Description());
	}
}

void CDlgTaxiInbound::OnDelItem()
{
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_wndListCtrl.GetNextSelectedItem(pos);
			m_pTaxiInbound->DeleteRecord(nItem);
			m_wndListCtrl.DeleteItemEx(nItem);
			UpdateToolBarState();
		}
	}

	GetDlgItem(IDC_SAVE)->EnableWindow();
}


//void CDlgTaxiInbound::OnEditItem()
//{
//	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
//	if (pos != NULL)
//		m_wndListCtrl.EditSubLabel(0, 1);
//}

void CDlgTaxiInbound::OnListItemChanged(int nItem, int nSubItem)
{
	CTaxiInboundNECManager* pManager = m_pTaxiInbound->GetTaxiInboundNECList();
	int nValue = m_wndListCtrl.GetItemData(nItem);
	CTaxiInboundNEC* pTaxiInboundNEC = pManager->GetRecordByID(nValue);

	CString strCurVal = m_wndListCtrl.GetItemText(nItem, nSubItem);
	float dCurVal = (float)atof(strCurVal);

	switch(nSubItem) {
	case  1:
		pTaxiInboundNEC->SetAcceleration(dCurVal);
		break;
	case  2:
		pTaxiInboundNEC->SetDeceleration(dCurVal);
		break;
	case  3:
		pTaxiInboundNEC->SetNormalSpeed(dCurVal);
		break;
	case  4:
		pTaxiInboundNEC->SetMaxSpeed(dCurVal);
		break;
	case  5:
		pTaxiInboundNEC->SetIntersectionBuffer(dCurVal);
		break;
	case  6:
		pTaxiInboundNEC->SetMinSeparation(dCurVal);
		break;
	case  7:
		pTaxiInboundNEC->SetMaxSeparation(dCurVal);
		break;
	case  8:
		pTaxiInboundNEC->SetLongitudinalSeperation(dCurVal);
		break;
	case  9:
		pTaxiInboundNEC->SetStaggeredSeperation(dCurVal);
		break;
	case  10:
		pTaxiInboundNEC->SetFuelBurn(dCurVal);
		break;
	case  11:
		pTaxiInboundNEC->SetCostPerHour(dCurVal);
		break;

	default:
		break;
	}

	try
	{
		m_pTaxiInbound->GetTaxiInboundNECList()->UpdateItem(*pTaxiInboundNEC);
	}
	catch (_com_error& e)
	{
		AfxMessageBox((LPCSTR)e.Description());
	}

	UpdateToolBarState();
}

void CDlgTaxiInbound::OnNECItemChanged(int nItem, int nNewNECTableID)
{
	UpdateToolBarState();
}


BOOL CDlgTaxiInbound::OnInitDialog()
{
	CNECRelatedBaseDlg::OnInitDialog();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
