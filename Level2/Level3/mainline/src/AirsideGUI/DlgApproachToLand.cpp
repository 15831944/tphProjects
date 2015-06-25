// DlgApproachToLand.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "../InputAirside/AirsideAircraft.h"
#include "..\Common\AirportDatabase.h"
#include ".\dlgapproachtoland.h"

// CDlgApproachToLand dialog

CDlgApproachToLand::CDlgApproachToLand(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB, LPCTSTR lpszCatpion, CWnd* pParent /*=NULL*/)
 : CNECRelatedBaseDlg(pSelectFlightType, lpszCatpion, pParent)
{
	m_pApproachLands = new ApproachLands(pAirportDB);
	m_pApproachLands->ReadData(nProjID);
}

CDlgApproachToLand::~CDlgApproachToLand()
{
}

void CDlgApproachToLand::DoDataExchange(CDataExchange* pDX)
{
	CNECRelatedBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgApproachToLand, CNECRelatedBaseDlg)
END_MESSAGE_MAP()


// CDlgApproachToLand message handlers
void CDlgApproachToLand::ReloadData()
{
//	m_pApproachLands->ReadData();
}

void CDlgApproachToLand::SetListColumn()
{
	CStringList strList;
	strList.RemoveAll();
	
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("Fuel Burn(lbs/hr)");
	lvc.cx = 120;
	lvc.fmt = LVCFMT_NUMBER;
	lvc.csList = &strList;
	m_wndListCtrl.InsertColumn(1, &lvc);
	lvc.pszText = _T("Min Approach Length (nm)");
	
	lvc.cx = 120;
	lvc.fmt = LVCFMT_NUMBER;
	lvc.csList = &strList;
	m_wndListCtrl.InsertColumn(2, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Min Approach Speed (KTS)");
	m_wndListCtrl.InsertColumn(3, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Max Approach Speed (KTS)");
	m_wndListCtrl.InsertColumn(4, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("cost (h/hr)");
	m_wndListCtrl.InsertColumn(5, &lvc);
}

void CDlgApproachToLand::SetListContent()
{
	std::vector<ApproachLand>& vAppLoads = m_pApproachLands->GetApproachLands();
	size_t nCount = vAppLoads.size();

	for (size_t i=0; i<nCount; i++)
	{
		ApproachLand& appLand = vAppLoads.at(i);
		char szFltType[1024] = {0};
		appLand.m_fltType.screenPrint(szFltType);
		m_wndListCtrl.InsertItem((int)i, szFltType);
		
		CString strTempValue;
		//fuel burn
		strTempValue.Format("%.2f",appLand.m_dFuelBurn);
		m_wndListCtrl.SetItemText((int)i, 1, strTempValue);		
		strTempValue.Format("%.2f",appLand.m_dMinLength);
		m_wndListCtrl.SetItemText((int)i, 2, strTempValue);
		strTempValue.Format("%.2f", appLand.m_dMinSpeed);
		m_wndListCtrl.SetItemText((int)i, 3, strTempValue);
		strTempValue.Format("%.2f", appLand.m_dMaxSpeed);
		m_wndListCtrl.SetItemText((int)i, 4, strTempValue);
		strTempValue.Format("%.2f", appLand.m_dCost);
		m_wndListCtrl.SetItemText((int)i, 5, strTempValue);
	}
}
void CDlgApproachToLand::CheckListContent()
{
	std::vector<ApproachLand>& vAppLoads = m_pApproachLands->GetApproachLands();
	size_t nCount = vAppLoads.size();
	if(nCount == 0)
	{
		ApproachLand appLand;
		try
		{
			m_pApproachLands->InsertApproachLand(appLand);
			m_pApproachLands->GetApproachLands().push_back(appLand);
		}
		catch (_com_error& e)
		{
			AfxMessageBox((LPCSTR)e.Description());
		}
	}
}

void CDlgApproachToLand::OnNewItem(FlightConstraint& fltType)
{
	ApproachLand appLand;
	appLand.m_fltType = fltType;

	try
	{
		m_pApproachLands->InsertApproachLand(appLand);
		m_pApproachLands->GetApproachLands().push_back(appLand);
		//for(int i = 1 ;i < 5; i++)
		//	m_wndListCtrl.SetItemText(m_wndListCtrl.GetItemCount() - 1, i, "0.0");
		
		int nIndex = m_wndListCtrl.GetItemCount() - 1;
		CString strTempValue;
		//fuel burn
		strTempValue.Format("%.2f", appLand.m_dFuelBurn);
		m_wndListCtrl.SetItemText(nIndex, 1, strTempValue);		
		strTempValue.Format("%.2f", appLand.m_dMinLength);
		m_wndListCtrl.SetItemText(nIndex, 2, strTempValue);
		strTempValue.Format("%.2f", appLand.m_dMinSpeed);
		m_wndListCtrl.SetItemText(nIndex, 3, strTempValue);
		strTempValue.Format("%.2f", appLand.m_dMaxSpeed);
		m_wndListCtrl.SetItemText(nIndex, 4, strTempValue);
		strTempValue.Format("%.2f", appLand.m_dCost);
		m_wndListCtrl.SetItemText(nIndex, 5, strTempValue);

	}
	catch (_com_error& e)
	{
		AfxMessageBox((LPCSTR)e.Description());
	}
}


void CDlgApproachToLand::OnDelItem()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_wndListCtrl.GetNextSelectedItem(pos);
			std::vector<ApproachLand>::iterator iter = m_pApproachLands->GetApproachLands().begin() + nItem;
			try
			{
				m_pApproachLands->DeleteApproachLand(*iter);
				m_pApproachLands->GetApproachLands().erase(iter);

				m_wndListCtrl.DeleteItemEx(nItem);
				UpdateToolBarState();

				//GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
			}
			catch (_com_error& e)
			{
				AfxMessageBox((LPCSTR)e.Description());
			}
		}
	}
}


//void CDlgApproachToLand::OnEditItem()
//{
//	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
//	if (pos != NULL)
//		m_wndListCtrl.EditSubLabel(0, 1);
//}

void CDlgApproachToLand::OnListItemChanged(int nItem, int nSubItem)
{
	CString strCurVal = m_wndListCtrl.GetItemText(nItem, nSubItem);
	double dCurVal = atof(strCurVal.GetBuffer(0));

	ApproachLand& appLand = m_pApproachLands->GetApproachLands().at(nItem);
	
	switch(nSubItem)
	{
	case 1:
		appLand.m_dFuelBurn = dCurVal;
		break;

	case 2 :
		appLand.m_dMinLength = dCurVal;
		break;

	case 3 :
		appLand.m_dMinSpeed = dCurVal;
		break;

	case 4 :
		appLand.m_dMaxSpeed = dCurVal;
		break;

	case 5 :
		appLand.m_dCost = dCurVal;
		break;

	default:
		break;
	}

	try
	{
		m_pApproachLands->UpdateApproachLand(appLand);	
		GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	}
	catch (_com_error& e)
	{
		AfxMessageBox((LPCSTR)e.Description());
	}

	UpdateToolBarState();
}

void CDlgApproachToLand::OnNECItemChanged(int nItem, int nNewNECTableID)
{
	//ApproachLand& appLand = m_pApproachLands->GetApproachLands().at(nItem);
	//appLand.m_nNECID = nNewNECTableID;
	//try
	//{
	//	m_pApproachLands->UpdateApproachLand(appLand);	
	//	//GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	//}
	//catch (_com_error& e)
	//{
	//	AfxMessageBox((LPCSTR)e.Description());
	//}

	UpdateToolBarState();

}
BOOL CDlgApproachToLand::OnInitDialog()
{
	CNECRelatedBaseDlg::OnInitDialog();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
