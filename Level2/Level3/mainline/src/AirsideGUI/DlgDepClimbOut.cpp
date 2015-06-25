// DlgDepClimbOut.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "../InputAirside/AirsideAircraft.h"
#include "..\Common\AirportDatabase.h"
#include ".\dlgdepclimbout.h"

// CDlgDepClimbOut dialog

IMPLEMENT_DYNAMIC(CDlgDepClimbOut, CNECRelatedBaseDlg)
CDlgDepClimbOut::CDlgDepClimbOut(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB, LPCTSTR lpszCatpion, CWnd* pParent /*=NULL*/)
: CNECRelatedBaseDlg(pSelectFlightType, lpszCatpion, pParent)
{
	m_pDepClimbOuts = new DepClimbOuts(pAirportDB);
	m_pDepClimbOuts->ReadData(nProjID);
}

CDlgDepClimbOut::~CDlgDepClimbOut()
{
}

void CDlgDepClimbOut::DoDataExchange(CDataExchange* pDX)
{
	CNECRelatedBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgDepClimbOut, CNECRelatedBaseDlg)
END_MESSAGE_MAP()


// CDlgDepClimbOut message handlers


void CDlgDepClimbOut::ReloadData()
{
//	m_pDepClimbOuts->ReadData();
}

void CDlgDepClimbOut::SetListColumn()
{
	LV_COLUMNEX lvc;
	CStringList strList;
	strList.RemoveAll();

	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.cx = 120;
	lvc.csList = &strList;

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Min Horizontal Accel(ft/sec2)");
	m_wndListCtrl.InsertColumn(1, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Max Horizontal Accel(ft/sec2)");
	m_wndListCtrl.InsertColumn(2, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Min Vertical Speed To Enroute(ft/min");
	m_wndListCtrl.InsertColumn(3, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Max Vertical Speed To Enroute(ft/min");
	m_wndListCtrl.InsertColumn(4, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Fuel Burn(lbs/hr");
	m_wndListCtrl.InsertColumn(5, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Cost($/hr");
	m_wndListCtrl.InsertColumn(6, &lvc);
}

void CDlgDepClimbOut::SetListContent()
{
	std::vector<DepClimbOut>& vDepClimbs = m_pDepClimbOuts->GetDepClimbOuts();
	size_t nClimbOutCount = vDepClimbs.size();

	for (size_t i = 0; i < nClimbOutCount; i++)
	{
		DepClimbOut& depClimbOut = vDepClimbs.at(i);

		char szFltType[1024] = {0};
		depClimbOut.m_fltType.screenPrint(szFltType);
		m_wndListCtrl.InsertItem((int)i, szFltType);

		CString strTempValue;

		strTempValue.Format("%.2f", depClimbOut.m_nMinHorAccel);
		m_wndListCtrl.SetItemText((int)i, 1, strTempValue);

		strTempValue.Format("%.2f", depClimbOut.m_nMaxHorAccel);
		m_wndListCtrl.SetItemText((int)i, 2, strTempValue);

		strTempValue.Format("%.2f", depClimbOut.m_nMinVerticalSpeedToEntoute);
		m_wndListCtrl.SetItemText((int)i, 3, strTempValue);

		strTempValue.Format("%.2f", depClimbOut.m_nMaxVerticalSpeedToEntoute);
		m_wndListCtrl.SetItemText((int)i, 4, strTempValue);

		strTempValue.Format("%.2f", depClimbOut.m_fuelBurn);
		m_wndListCtrl.SetItemText((int)i, 5, strTempValue);

		strTempValue.Format("%.2f", depClimbOut.m_cost);
		m_wndListCtrl.SetItemText((int)i, 6, strTempValue);
	}

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
}

void CDlgDepClimbOut::CheckListContent()
{
	std::vector<DepClimbOut>& vDepClimbs = m_pDepClimbOuts->GetDepClimbOuts();
	size_t nClimbOutCount = vDepClimbs.size();
	if(nClimbOutCount == 0)
	{
		DepClimbOut depClimbOut;
		try
		{
			m_pDepClimbOuts->InsertDepClimbOut(depClimbOut);
			m_pDepClimbOuts->GetDepClimbOuts().push_back(depClimbOut);

		}
		catch (_com_error& e)
		{
			AfxMessageBox((LPCSTR)e.Description());
		}

		GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
		UpdateToolBarState();
	}
}
void CDlgDepClimbOut::OnNewItem(FlightConstraint& fltType)
{
	DepClimbOut depClimbOut;
	depClimbOut.m_fltType = fltType;

	try
	{
		m_pDepClimbOuts->InsertDepClimbOut(depClimbOut);
		m_pDepClimbOuts->GetDepClimbOuts().push_back(depClimbOut);

		int nIndex = m_wndListCtrl.GetItemCount() - 1;
		CString strTempValue;

		strTempValue.Format("%.2f", depClimbOut.m_nMinHorAccel);
		m_wndListCtrl.SetItemText(nIndex, 1, strTempValue);

		strTempValue.Format("%.2f", depClimbOut.m_nMaxHorAccel);
		m_wndListCtrl.SetItemText(nIndex, 2, strTempValue);

		strTempValue.Format("%.2f", depClimbOut.m_nMinVerticalSpeedToEntoute);
		m_wndListCtrl.SetItemText(nIndex, 3, strTempValue);

		strTempValue.Format("%.2f", depClimbOut.m_nMaxVerticalSpeedToEntoute);
		m_wndListCtrl.SetItemText(nIndex, 4, strTempValue);

		strTempValue.Format("%.2f", depClimbOut.m_fuelBurn);
		m_wndListCtrl.SetItemText(nIndex, 5, strTempValue);

		strTempValue.Format("%.2f", depClimbOut.m_cost);
		m_wndListCtrl.SetItemText(nIndex, 6, strTempValue);

	}
	catch (_com_error& e)
	{
		AfxMessageBox((LPCSTR)e.Description());
	}

	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	UpdateToolBarState();

}
void CDlgDepClimbOut::OnDelItem()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_wndListCtrl.GetNextSelectedItem(pos);
			std::vector<DepClimbOut>::iterator iter = m_pDepClimbOuts->GetDepClimbOuts().begin() + nItem;
			try
			{
				m_pDepClimbOuts->DeleteDepClimbOut(*iter);
				m_pDepClimbOuts->GetDepClimbOuts().erase(iter);
				m_wndListCtrl.DeleteItemEx(nItem);
				UpdateToolBarState();

				GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
			}
			catch (_com_error& e)
			{
				AfxMessageBox((LPCSTR)e.Description());
			}
		}
	}
}

//void CDlgDepClimbOut::OnEditItem()
//{
//	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
//	if (pos != NULL)
//		m_wndListCtrl.EditSubLabel(0, 1);
//}

void CDlgDepClimbOut::OnListItemChanged(int nItem, int nSubItem)
{
	CString strCurVal = m_wndListCtrl.GetItemText(nItem, nSubItem);
	double dCurVal = atof(strCurVal.GetBuffer(0));	
	
	DepClimbOut& depClimbOut = m_pDepClimbOuts->GetDepClimbOuts().at(nItem);


	switch(nSubItem)
	{
	case  1:
		depClimbOut.m_nMinHorAccel = dCurVal;
		break;

	case  2:
		depClimbOut.m_nMaxHorAccel = dCurVal;
		break;

	case  3:
		depClimbOut.m_nMinVerticalSpeedToEntoute = dCurVal;
		break;

	case  4:
		depClimbOut.m_nMaxVerticalSpeedToEntoute = dCurVal;
		break;

	case  5:
		depClimbOut.m_fuelBurn = dCurVal;
		break;

	case  6:
		depClimbOut.m_cost = dCurVal;
		break;

	default:
		break;
	}

	try
	{
		m_pDepClimbOuts->UpdateDepClimbOut(depClimbOut);				
		GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	}
	catch (_com_error& e)
	{
		AfxMessageBox((LPCSTR)e.Description());
	}

	UpdateToolBarState();

}

void CDlgDepClimbOut::OnNECItemChanged(int nItem, int nNewNECTableID)
{
	//DepClimbOut &depClimbOut = m_pDepClimbOuts->GetDepClimbOuts().at(nItem);
	//depClimbOut.m_nNECID = nNewNECTableID;

	//try
	//{
	//	m_pDepClimbOuts->UpdateDepClimbOut(depClimbOut);
	//	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	//}
	//catch (_com_error& e)
	//{
	//	AfxMessageBox((LPCSTR)e.Description());
	//}

	UpdateToolBarState();
}


BOOL CDlgDepClimbOut::OnInitDialog()
{
	CNECRelatedBaseDlg::OnInitDialog();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
