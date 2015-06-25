#include "stdafx.h"
#include "resource.h"
#include "DlgPerformanceTakeoff.h"
#include "../InputAirside/AirsideAircraft.h"
#include ".\dlgperformancetakeoff.h"


CDlgPerformanceTakeoff::CDlgPerformanceTakeoff(int nProjID, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB, LPCTSTR lpszCatpion, CWnd* pParent /*=NULL*/)
 : CNECRelatedBaseDlg(pSelectFlightType, lpszCatpion, pParent)
{
	m_pCompsiteACTakeoffs = new PerformTakeOffs(pAirportDB);
	m_pCompsiteACTakeoffs->ReadData(nProjID);
}

CDlgPerformanceTakeoff::~CDlgPerformanceTakeoff()
{
}

void CDlgPerformanceTakeoff::DoDataExchange(CDataExchange* pDX)
{
	CNECRelatedBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgPerformanceTakeoff, CNECRelatedBaseDlg)
END_MESSAGE_MAP()


// CDlgPerformanceTakeoff message handlers
void CDlgPerformanceTakeoff::ReloadData()
{
//	m_pCompsiteACTakeoffs->ReadData();
}

void CDlgPerformanceTakeoff::SetListColumn()
{
	CStringList strList;
	strList.RemoveAll();

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.cx = 100;
	lvc.csList = &strList;

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Min Acceleration (ft/sec2)");
	m_wndListCtrl.InsertColumn(1, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Max Acceleration (ft/sec2)");
	m_wndListCtrl.InsertColumn(2, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Min Lift Off(kts)");
	m_wndListCtrl.InsertColumn(3, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Max Lift Off(kts)");
	m_wndListCtrl.InsertColumn(4, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Min Position Time(sec)");
	m_wndListCtrl.InsertColumn(5, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Max Position Time(sec)");
	m_wndListCtrl.InsertColumn(6, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Min Take Off Roll(m)");
	m_wndListCtrl.InsertColumn(7, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Max Take Off Roll(m)");
	m_wndListCtrl.InsertColumn(8, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Fuel Burn(lbs/hr)");
	m_wndListCtrl.InsertColumn(9, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Cost($/hr)");
	m_wndListCtrl.InsertColumn(10, &lvc);
}

void CDlgPerformanceTakeoff::SetListContent()
{
	std::vector<PerformTakeOff>& vTakeoff = m_pCompsiteACTakeoffs->GetTakeoffs();
	size_t nTakeoffCount = vTakeoff.size();

	CString strTempValue;
	for (size_t i = 0; i < nTakeoffCount; i++)
	{
		PerformTakeOff& tf = vTakeoff.at(i);

		char szFltType[1024] = {0};
		tf.m_fltType.screenPrint(szFltType);
		m_wndListCtrl.InsertItem((int)i, szFltType);

		strTempValue.Format("%.2f", tf.m_nMinAcceleration);
		m_wndListCtrl.SetItemText((int)i, 1, strTempValue);

		strTempValue.Format("%.2f", tf.m_nMaxAcceleration);
		m_wndListCtrl.SetItemText((int)i, 2, strTempValue);

		strTempValue.Format("%.2f", tf.m_nMinLiftOff);
		m_wndListCtrl.SetItemText((int)i, 3, strTempValue);

		strTempValue.Format("%.2f", tf.m_nMaxLiftOff);
		m_wndListCtrl.SetItemText((int)i, 4, strTempValue);

		strTempValue.Format("%.2f", tf.m_nMinPositionTime);
		m_wndListCtrl.SetItemText((int)i, 5, strTempValue);

		strTempValue.Format("%.2f", tf.m_nMaxPositionTime);
		m_wndListCtrl.SetItemText((int)i, 6, strTempValue);

		strTempValue.Format("%.2f", tf.m_minTakeOffRoll);
		m_wndListCtrl.SetItemText((int)i, 7, strTempValue);

		strTempValue.Format("%.2f", tf.m_maxTakeOffRoll);
		m_wndListCtrl.SetItemText((int)i, 8, strTempValue);
		
		strTempValue.Format("%.2f", tf.m_fuelBurn);
		m_wndListCtrl.SetItemText((int)i, 9, strTempValue);

		strTempValue.Format("%.2f", tf.m_cost);
		m_wndListCtrl.SetItemText((int)i, 10, strTempValue);
	}
}

void CDlgPerformanceTakeoff::CheckListContent()
{
	std::vector<PerformTakeOff>& vTakeoff = m_pCompsiteACTakeoffs->GetTakeoffs();
	size_t nTakeoffCount = vTakeoff.size();
	if(nTakeoffCount == 0)
	{
		PerformTakeOff tf;
		try
		{
			m_pCompsiteACTakeoffs->InsertPerformTakeOff(tf);
			m_pCompsiteACTakeoffs->GetTakeoffs().push_back(tf);
		}
		catch (_com_error& e)
		{
			AfxMessageBox((LPCSTR)e.Description());
		}
		GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	}
}
void CDlgPerformanceTakeoff::OnNewItem(FlightConstraint& fltType)
{
	PerformTakeOff tf;
	tf.m_fltType = fltType;
	
	try
	{
		m_pCompsiteACTakeoffs->InsertPerformTakeOff(tf);
		m_pCompsiteACTakeoffs->GetTakeoffs().push_back(tf);

		int nIndex = m_wndListCtrl.GetItemCount() - 1;

		CString strTempValue;

		strTempValue.Format("%.2f", tf.m_nMinAcceleration);
		m_wndListCtrl.SetItemText(nIndex, 1, strTempValue);

		strTempValue.Format("%.2f", tf.m_nMaxAcceleration);
		m_wndListCtrl.SetItemText(nIndex, 2, strTempValue);

		strTempValue.Format("%.2f", tf.m_nMinLiftOff);
		m_wndListCtrl.SetItemText(nIndex, 3, strTempValue);

		strTempValue.Format("%.2f", tf.m_nMaxLiftOff);
		m_wndListCtrl.SetItemText(nIndex, 4, strTempValue);

		strTempValue.Format("%.2f", tf.m_nMinPositionTime);
		m_wndListCtrl.SetItemText(nIndex, 5, strTempValue);

		strTempValue.Format("%.2f", tf.m_nMaxPositionTime);
		m_wndListCtrl.SetItemText(nIndex, 6, strTempValue);

		strTempValue.Format("%.2f", tf.m_minTakeOffRoll);
		m_wndListCtrl.SetItemText(nIndex, 7, strTempValue);

		strTempValue.Format("%.2f", tf.m_maxTakeOffRoll);
		m_wndListCtrl.SetItemText(nIndex, 8, strTempValue);

		strTempValue.Format("%.2f", tf.m_fuelBurn);
		m_wndListCtrl.SetItemText(nIndex, 9, strTempValue);

		strTempValue.Format("%.2f", tf.m_cost);
		m_wndListCtrl.SetItemText(nIndex, 10, strTempValue);
	}
	catch (_com_error& e)
	{
		AfxMessageBox((LPCSTR)e.Description());
	}
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}


void CDlgPerformanceTakeoff::OnDelItem()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_wndListCtrl.GetNextSelectedItem(pos);
			std::vector<PerformTakeOff>::iterator iter
				= m_pCompsiteACTakeoffs->GetTakeoffs().begin()+ nItem;
			try
			{
				m_pCompsiteACTakeoffs->DeletePerformTakeOff(*iter);
				m_pCompsiteACTakeoffs->GetTakeoffs().erase(iter);
				m_wndListCtrl.DeleteItemEx(nItem);
				UpdateToolBarState();
			}
			catch (_com_error& e)
			{
				AfxMessageBox((LPCSTR)e.Description());
			}
		}
	}
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}


//void CDlgPerformanceTakeoff::OnEditItem()
//{
//	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
//	if (pos != NULL)
//		m_wndListCtrl.EditSubLabel(0, 1);
//}

void CDlgPerformanceTakeoff::OnListItemChanged(int nItem, int nSubItem)
{
	CString strCurVal = m_wndListCtrl.GetItemText(nItem, nSubItem);
	double dCurVal = atof(strCurVal.GetBuffer(0));	
	PerformTakeOff &takeOff = m_pCompsiteACTakeoffs->GetTakeoffs().at(nItem);
	
	switch(nSubItem) {
	case  1:
		takeOff.m_nMinAcceleration = dCurVal;
		break;
	case  2:
		takeOff.m_nMaxAcceleration = dCurVal;
		break;
	case  3:
		takeOff.m_nMinLiftOff = dCurVal;
		break;
	case  4:
		takeOff.m_nMaxLiftOff = dCurVal;
		break;
	case  5:
		takeOff.m_nMinPositionTime = dCurVal;
		break;
	case  6:
		takeOff.m_nMaxPositionTime = dCurVal;
		break;
	case  7:
		takeOff.m_minTakeOffRoll = dCurVal;
		break;
	case  8:
		takeOff.m_maxTakeOffRoll = dCurVal;
		break;
	case  9:
		takeOff.m_fuelBurn = dCurVal;
		break;
	case  10:
		takeOff.m_cost = dCurVal;
		break;
	
	default:
		break;
	}

	try
	{
		m_pCompsiteACTakeoffs->UpdatePerformTakeOff(takeOff);				
		GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	}
	catch (_com_error& e)
	{
		AfxMessageBox((LPCSTR)e.Description());
	}

	UpdateToolBarState();
}

void CDlgPerformanceTakeoff::OnNECItemChanged(int nItem, int nNewNECTableID)
{
	//PerformTakeOff& tf = m_pCompsiteACTakeoffs->GetTakeoffs().at(nItem);
	//tf.m_nNECID = nNewNECTableID;

	//try
	//{
	//	m_pCompsiteACTakeoffs->UpdatePerformTakeOff(tf);			
	//	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
	//}
	//catch (_com_error& e)
	//{
	//	AfxMessageBox((LPCSTR)e.Description());
	//}

	UpdateToolBarState();
}

BOOL CDlgPerformanceTakeoff::OnInitDialog()
{
	CNECRelatedBaseDlg::OnInitDialog();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
