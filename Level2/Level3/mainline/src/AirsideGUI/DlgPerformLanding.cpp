// DlgPerformLanding.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgPerformLanding.h"
#include "../InputAirside/AirsideAircraft.h"
#include "..\Common\AirportDatabase.h"

#include <algorithm>
#include ".\dlgperformlanding.h"

// CDlgPerformLanding dialog
int CDlgPerformLanding::PrevColumnCount = 9;

IMPLEMENT_DYNAMIC(CDlgPerformLanding, CNECRelatedBaseDlg)
CDlgPerformLanding::CDlgPerformLanding(int nProjID, std::vector<CString>& vRunwayNames, PFuncSelectFlightType pSelectFlightType, CAirportDatabase* pAirportDB, LPCTSTR lpszCatpion,CWnd* pParent /*=NULL*/)
	:m_vRunwayNames(vRunwayNames)
	, CNECRelatedBaseDlg(pSelectFlightType, lpszCatpion, pParent)
{
	m_pPerformLandings = new PerformLandings(pAirportDB);
	m_pPerformLandings->ReadData(nProjID);
}

CDlgPerformLanding::~CDlgPerformLanding()
{
}

void CDlgPerformLanding::DoDataExchange(CDataExchange* pDX)
{
	CNECRelatedBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgPerformLanding, CNECRelatedBaseDlg)
END_MESSAGE_MAP()


// CDlgPerformLanding message handlers

void CDlgPerformLanding::ReloadData()
{
//	m_pPerformLandings->ReadData();
}

void CDlgPerformLanding::SetListColumn()
{
	CStringList strList;
	strList.RemoveAll();

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.cx = 100;
	lvc.csList = &strList;
	
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Min TouchDown(KTS)");
	m_wndListCtrl.InsertColumn(1, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Max TouchDown(KTS)");
	m_wndListCtrl.InsertColumn(2, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Min TouchDown Dist(m)");
	m_wndListCtrl.InsertColumn(3, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Max TouchDown Dist(m)");
	m_wndListCtrl.InsertColumn(4, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Max Decel(ft/sec2)");
	m_wndListCtrl.InsertColumn(5, &lvc);
	
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Normal Decel(ft/sec2)");
	m_wndListCtrl.InsertColumn(6, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Min Land Dist(m)");
	m_wndListCtrl.InsertColumn(7, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Max Land Dist(m)");
	m_wndListCtrl.InsertColumn(8, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Max Speed Acute(kts)");
	m_wndListCtrl.InsertColumn(9, &lvc);
	//lvc.fmt = LVCFMT_NUMBER;
	//lvc.pszText = _T("Min Landing Occupancy (sec)");
	//m_wndListCtrl.InsertColumn(9, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Max Speed 90 deg(kts)");
	m_wndListCtrl.InsertColumn(10, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Max Speed High Speed(kts)");
	m_wndListCtrl.InsertColumn(11, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Fuel Burn(lbs/hr)");
	m_wndListCtrl.InsertColumn(12, &lvc);

	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = _T("Cost ($)");
	m_wndListCtrl.InsertColumn(13, &lvc);

	size_t nSize = m_vRunwayNames.size();
	for (size_t i=0; i < nSize; i++)
	{
		lvc.fmt = LVCFMT_NUMBER;
		CString strLable;
		strLable.Format("Threshold To Flare Distance ( %s )", m_vRunwayNames[i]); 
		lvc.pszText = strLable.GetBuffer(0);
		m_wndListCtrl.InsertColumn(PrevColumnCount + 1 +i, &lvc);
	}

}
//Check if Runway's Define Changed (Runway Added Delete or Runway's Name changed)
BOOL CDlgPerformLanding::IsRunwayDefineChanged(PerformLanding& perfLand)
{
	if (m_vRunwayNames.size() != perfLand.GetRunwayThresholds().size())
		return TRUE;

	std::vector<RunwayThreshold>& vRWThreshold = perfLand.GetRunwayThresholds(); 
	std::vector<CString>::iterator it;
	for (std::vector<RunwayThreshold>::iterator iter = vRWThreshold.begin();
		 iter != vRWThreshold.end();
		 iter++)
	{
		it = std::find(m_vRunwayNames.begin(), m_vRunwayNames.end(), iter->m_strRunwayName);
		if (it == m_vRunwayNames.end())
			return TRUE;
	}
	return FALSE;
}
//if Runways's Define Have been changed, we need Update DB Table RunwayFlareDistance
void CDlgPerformLanding::UpdateFlareDistanceIfNeeded(std::vector<PerformLanding>& vPerfLands)
{
	if (vPerfLands.size() == 0 || FALSE == IsRunwayDefineChanged(vPerfLands.at(0)))
		return;

	for (std::vector<PerformLanding>::iterator it = vPerfLands.begin();
		it != vPerfLands.end();
		it++)
	{
		//Delete Old Data
//		it->DeleteRunwayThresholds();
		it->GetRunwayThresholds().clear();

		//Set Default Value
		for (std::vector<CString>::iterator itRunway = m_vRunwayNames.begin();
			itRunway != m_vRunwayNames.end();
			itRunway++)
		{
			RunwayThreshold rt;
			rt.m_dDistance = 0.0;
			rt.m_nFltPerformLandingID = it->GetID();
			rt.m_strRunwayName = *itRunway;
			it->InsertRunwayThreshold(rt);

			it->GetRunwayThresholds().push_back(rt);
		}
	}
}

void CDlgPerformLanding::SetListContent()
{
	std::vector<PerformLanding>& vPerfLands = m_pPerformLandings->GetPerformLandings();
	size_t nCount = vPerfLands.size();

	UpdateFlareDistanceIfNeeded(vPerfLands);

	for (size_t i=0; i<nCount; i++)
	{
		PerformLanding& perfLand = vPerfLands.at(i);

		char szFltType[1024] = {0};
		perfLand.m_fltType.screenPrint(szFltType);
		m_wndListCtrl.InsertItem((int)i, szFltType);

		CString strTempValue;

		strTempValue.Format("%.2f",perfLand.m_dMinTouchDownSpeed);
		m_wndListCtrl.SetItemText((int)i, 1, strTempValue);
		strTempValue.Format("%.2f", perfLand.m_dMaxTouchDownSpeed);
		m_wndListCtrl.SetItemText((int)i, 2, strTempValue);
		strTempValue.Format("%.2f", perfLand.m_dMinTouchDownDist);
		m_wndListCtrl.SetItemText((int)i, 3, strTempValue);
		strTempValue.Format("%.2f", perfLand.m_dMaxTouchDownDist);
		m_wndListCtrl.SetItemText((int)i, 4, strTempValue);
		strTempValue.Format("%.2f", perfLand.m_dMaxDeceleration);
		m_wndListCtrl.SetItemText((int)i, 5, strTempValue);

		strTempValue.Format("%.2f", perfLand.m_dNormalDeceleration);
		m_wndListCtrl.SetItemText((int)i, 6, strTempValue);
		strTempValue.Format("%.2f", perfLand.m_dMinLandDist);
		m_wndListCtrl.SetItemText((int)i, 7, strTempValue);
		strTempValue.Format("%.2f", perfLand.m_dMaxLandDist);
		m_wndListCtrl.SetItemText((int)i, 8, strTempValue);
		//strTempValue.Format("%.2f", perfLand.m_dMaxSpeedAcute);
		//m_wndListCtrl.SetItemText((int)i, 8, strTempValue);
		
		strTempValue.Format("%.2f", perfLand.m_dMaxSpeedAcute);
		m_wndListCtrl.SetItemText((int)i, 9, strTempValue);

		strTempValue.Format("%.2f", perfLand.m_dMaxSpeed90deg);
		m_wndListCtrl.SetItemText((int)i, 10, strTempValue);

		strTempValue.Format("%.2f", perfLand.m_dMaxSpeedHighSpeed);
		m_wndListCtrl.SetItemText((int)i, 11, strTempValue);

		strTempValue.Format("%.2f", perfLand.m_dFuelBurn);
		m_wndListCtrl.SetItemText((int)i, 12, strTempValue);

		strTempValue.Format("%.2f", perfLand.m_dCost);
		m_wndListCtrl.SetItemText((int)i, 13, strTempValue);

		std::vector<RunwayThreshold>& vRwThreshold = perfLand.GetRunwayThresholds();		
		for (size_t nRunwayIdx=0; nRunwayIdx<m_vRunwayNames.size(); nRunwayIdx++)
		{
			strTempValue.Format("%.2f", 
				perfLand.GetRunwayThresholdFlareDistanceByRwName(m_vRunwayNames.at(nRunwayIdx)));
			m_wndListCtrl.SetItemText((int)i, (int)(PrevColumnCount + 1 + (int)nRunwayIdx),strTempValue);
		}
	}
}
void CDlgPerformLanding::CheckListContent()
{
	std::vector<PerformLanding>& vPerfLands = m_pPerformLandings->GetPerformLandings();
	size_t nCount = vPerfLands.size();
	if(nCount == 0)
	{
		PerformLanding perfLand;

		try
		{
			m_pPerformLandings->InsertPerformLanding(perfLand);

			std::vector<PerformLanding>& vPerfPanding = m_pPerformLandings->GetPerformLandings();
			vPerfPanding.push_back(perfLand);

			PerformLanding& newPerfLand = vPerfPanding.at(vPerfPanding.size()-1);
			for (std::vector<CString>::iterator iter = m_vRunwayNames.begin();
				iter != m_vRunwayNames.end();
				iter++)
			{
				RunwayThreshold rwThreshold;
				rwThreshold.m_strRunwayName = (*iter);
				rwThreshold.m_dDistance = 0.0;
				rwThreshold.m_nFltPerformLandingID = perfLand.m_nID;	

				newPerfLand.InsertRunwayThreshold(rwThreshold);
				newPerfLand.GetRunwayThresholds().push_back(rwThreshold);
				size_t nsize = perfLand.GetRunwayThresholds().size();
			}
		}
		catch (_com_error& e)
		{
			AfxMessageBox((LPCSTR)e.Description());
		}
	}
}
void CDlgPerformLanding::OnNewItem(FlightConstraint& fltType)
{
	PerformLanding perfLand;
	perfLand.m_fltType = fltType;

	try
	{
		m_pPerformLandings->InsertPerformLanding(perfLand);

		std::vector<PerformLanding>& vPerfPanding = m_pPerformLandings->GetPerformLandings();
		vPerfPanding.push_back(perfLand);
		
		PerformLanding& newPerfLand = vPerfPanding.at(vPerfPanding.size()-1);
		for (std::vector<CString>::iterator iter = m_vRunwayNames.begin();
			iter != m_vRunwayNames.end();
			iter++)
		{
			RunwayThreshold rwThreshold;
			rwThreshold.m_strRunwayName = (*iter);
			rwThreshold.m_dDistance = 0.0;
			rwThreshold.m_nFltPerformLandingID = perfLand.m_nID;	
			
			newPerfLand.InsertRunwayThreshold(rwThreshold);
			newPerfLand.GetRunwayThresholds().push_back(rwThreshold);
			size_t nsize = perfLand.GetRunwayThresholds().size();
		}

		//for(int i = 1 ;i < (PrevColumnCount + 1 + (int)m_vRunwayNames.size()); i++)
		//	m_wndListCtrl.SetItemText(m_wndListCtrl.GetItemCount() - 1, i, "0.0");
		
		int i = m_wndListCtrl.GetItemCount() - 1;
		CString strTempValue;

		strTempValue.Format("%.2f",perfLand.m_dMinTouchDownSpeed);
		m_wndListCtrl.SetItemText((int)i, 1, strTempValue);
		strTempValue.Format("%.2f", perfLand.m_dMaxTouchDownSpeed);
		m_wndListCtrl.SetItemText((int)i, 2, strTempValue);
		strTempValue.Format("%.2f", perfLand.m_dMinTouchDownDist);
		m_wndListCtrl.SetItemText((int)i, 3, strTempValue);
		strTempValue.Format("%.2f", perfLand.m_dMaxTouchDownDist);
		m_wndListCtrl.SetItemText((int)i, 4, strTempValue);
		strTempValue.Format("%.2f", perfLand.m_dMaxDeceleration);
		m_wndListCtrl.SetItemText((int)i, 5, strTempValue);

		strTempValue.Format("%.2f", 4.0L);
		m_wndListCtrl.SetItemText((int)i, 6, strTempValue);
		strTempValue.Format("%.2f", perfLand.m_dMinLandDist);
		m_wndListCtrl.SetItemText((int)i, 7, strTempValue);
		strTempValue.Format("%.2f", perfLand.m_dMaxLandDist);
		m_wndListCtrl.SetItemText((int)i, 8, strTempValue);
		//strTempValue.Format("%.2f", perfLand.m_dMaxSpeedAcute);
		//m_wndListCtrl.SetItemText((int)i, 8, strTempValue);

		strTempValue.Format("%.2f", 10.0L);
		m_wndListCtrl.SetItemText((int)i, 9, strTempValue);

		strTempValue.Format("%.2f", 15.0L);
		m_wndListCtrl.SetItemText((int)i, 10, strTempValue);

		strTempValue.Format("%.2f", 40.0L);
		m_wndListCtrl.SetItemText((int)i, 11, strTempValue);

		strTempValue.Format("%.2f", 250.0L);
		m_wndListCtrl.SetItemText((int)i, 12, strTempValue);

		strTempValue.Format("%.2f", perfLand.m_dCost);
		m_wndListCtrl.SetItemText((int)i, 13, strTempValue);


	}
	catch (_com_error& e)
	{
		AfxMessageBox((LPCSTR)e.Description());
	}
}

void CDlgPerformLanding::OnDelItem()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_wndListCtrl.GetNextSelectedItem(pos);
			std::vector<PerformLanding>::iterator iter = m_pPerformLandings->GetPerformLandings().begin() + nItem;
			try
			{
				PerformLanding & perfLand = *iter;
//				perfLand.DeleteRunwayThresholds();

				m_pPerformLandings->DeletePerformLanding(*iter);
				m_pPerformLandings->GetPerformLandings().erase(iter);
				
				m_wndListCtrl.DeleteItemEx(nItem);

			}
			catch (_com_error& e)
			{
				AfxMessageBox((LPCSTR)e.Description());
			}
		}
	}
	
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

//void CDlgPerformLanding::OnEditItem()
//{
//	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
//	if (pos != NULL)
//		m_wndListCtrl.EditSubLabel(0, 1);
//}

void CDlgPerformLanding::OnListItemChanged(int nItem, int nSubItem)
{
	CString strCurVal = m_wndListCtrl.GetItemText(nItem, nSubItem);
	double dCurVal = atof(strCurVal.GetBuffer(0));
	PerformLanding& perfLand = m_pPerformLandings->GetPerformLandings().at(nItem);

	switch(nSubItem) {
	case  1:
		perfLand.m_dMinTouchDownSpeed = dCurVal;
		break;
	case  2:
		perfLand.m_dMaxTouchDownSpeed = dCurVal;
		break;
	case  3:
		perfLand.m_dMinTouchDownDist = dCurVal;
		break;
	case  4:
		perfLand.m_dMaxTouchDownDist = dCurVal;
		break;
	case  5:
		perfLand.m_dMaxDeceleration = dCurVal;
		break;
	case  6:
		perfLand.m_dNormalDeceleration = dCurVal;
		break;
	case  7:
		perfLand.m_dMinLandDist = dCurVal;
		break;
	case  8:
		perfLand.m_dMaxLandDist = dCurVal;
		break;
	case  9:
		perfLand.m_dMaxSpeedAcute = dCurVal;
		break;
	case  10:
		perfLand.m_dMaxSpeed90deg = dCurVal;
		break;
	case  11:
		perfLand.m_dMaxSpeedHighSpeed = dCurVal;
		break;
	case  12:
		perfLand.m_dFuelBurn = dCurVal;
		break;
	case  13:
		perfLand.m_dCost = dCurVal;
		break;
	default:
		if (nSubItem > PrevColumnCount)//Runway Threshold
			m_vRunwayNames;
			CString strRunwayName = m_vRunwayNames[nSubItem-PrevColumnCount-1];
			std::vector<RunwayThreshold>& vRwThreshold = perfLand.GetRunwayThresholds();
			int nIndex = perfLand.GetRunwayThresholdIndexByRwName(strRunwayName);
			if (nIndex < 0)	return;
			RunwayThreshold &rwRhreshold = vRwThreshold[nIndex];
			rwRhreshold.m_dDistance = dCurVal;
			try
			{
				perfLand.UpdateRunwayThreshold(rwRhreshold);
			}
			catch (_com_error& e)
			{
				AfxMessageBox((LPCSTR)e.Description());
			}
		break;
	}

	try
	{
		m_pPerformLandings->UpdatePerformLanding(perfLand);		
		
	}
	catch (_com_error& e)
	{
		AfxMessageBox((LPCSTR)e.Description());
	}
	
	GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}

void CDlgPerformLanding::OnNECItemChanged(int nItem, int nNewNECTableID)
{
	//PerformLanding& perfLand = m_pPerformLandings->GetPerformLandings().at(nItem);
	//perfLand.m_nNECID = nNewNECTableID;

	//try
	//{
	//	m_pPerformLandings->UpdatePerformLanding(perfLand);		
	//	
	//}
	//catch (_com_error& e)
	//{
	//	AfxMessageBox((LPCSTR)e.Description());
	//}
	//
	//GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
}
BOOL CDlgPerformLanding::OnInitDialog()
{
	CNECRelatedBaseDlg::OnInitDialog();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
