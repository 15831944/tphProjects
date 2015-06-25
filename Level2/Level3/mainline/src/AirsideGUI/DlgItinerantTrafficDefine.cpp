#include "stdafx.h"
//#include "..\Main\TermPlanDoc.h"
#include "DlgItinerantTrafficDefine.h"
#include ".\dlgitineranttrafficdefine.h"
#include "../InputAirside/ALTAirport.h"
#include "DlgWayPointStandFamily.h"
#include "DlgTimeRange.h"
#include "..\InputAirside\ALTObjectGroup.h"
#include "..\common\ProbDistManager.h"
#include "..\common\AirportDatabase.h"
#include "..\Common\Airport.h"
#include "..\Common\AirportsManager.h"
#include "..\Common\ACTypesManager.h"
#include "..\common\AirlineManager.h"

#include "..\common\ProbabilityDistribution.h"
#include "../Database/DBElementCollection_Impl.h"


#define PROBABILITY_DEFINE_STRING _T("NEW PROBABILITY DISTRIBUTION")

IMPLEMENT_DYNAMIC(CDlgItinerantTrafficDefine, CDialog)
CDlgItinerantTrafficDefine::CDlgItinerantTrafficDefine(int nProjID, PSelectFlightType pSelectFlightType, 
													   InputAirside* pInputAirside,  PSelectProbDistEntry pSelectProbDistEntry, 
													   ItinerantFlight* pItinerantFlight, BOOL bNewItem/*=FALSE*/, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgItinerantTrafficDefine::IDD, pParent)
	,m_nProjID(nProjID)
	,m_pSelectFlightType(pSelectFlightType)
	,m_pInputAirside(pInputAirside)
	,m_pSelectProbDistEntry(pSelectProbDistEntry)
	,m_pItinerantFlight(pItinerantFlight)
	,m_strArrDistScreenPrint(_T(""))
	,m_strDepDistScreenPrint(_T(""))
	,m_strArrDistPrint(_T(""))
	,m_strDepDistPrint(_T(""))
	,m_bArrDist(FALSE)
	,m_bDepDist(FALSE)
	,m_bArrTime(FALSE)
	,m_bDepTime(FALSE)
	,m_bEntry(FALSE)
	,m_bExit(FALSE)
	,m_nFlightCount(1)
	,m_bInit(FALSE)
	,m_bNewItem(bNewItem)
{

	m_strCode = pItinerantFlight->GetName();

	m_nFlightCount = pItinerantFlight->GetFltCount();
	m_strArrDistName = pItinerantFlight->GetArrDistName();
	m_strDepDistName = pItinerantFlight->GetDepDistName();
	m_strArrDistScreenPrint = pItinerantFlight->GetArrDistScreenPrint();
	GetAllStand();
	CString strTempValue;

	int nEntryFlag = pItinerantFlight->GetEntryFlag();
	int nEntryID = pItinerantFlight->GetEntryID();
	if(nEntryFlag == ALTOBJECT_TYPE(ALT_WAYPOINT))
	{
		for (int i = 0; i < static_cast<int>(m_vWaypoint.size()); ++ i)
		{
			if(m_vWaypoint[i].getID() == nEntryID)
			{
				CString strEntry = m_vWaypoint[i].GetObjNameString();
				strTempValue.Format("%s",strEntry);
				break;
			}
		}
	}
	else
	{
		ALTObjectGroup altObjGroup;
		altObjGroup.ReadData(nEntryID);	
		strTempValue = altObjGroup.getName().GetIDString();
	}

	m_strEntry = strTempValue;

	int nExitFlag = pItinerantFlight->GetExitFlag();
	int nExitID = pItinerantFlight->GetExitID();
	if(nExitFlag == ALTOBJECT_TYPE(ALT_WAYPOINT))
	{
		for (int i = 0; i < static_cast<int>(m_vWaypoint.size()); ++ i)
		{
			if(m_vWaypoint[i].getID() == nExitID)
			{
				CString strExit = m_vWaypoint[i].GetObjNameString();
				strTempValue.Format("%s",strExit);
				break;
			}
		}
	}
	else
	{
		ALTObjectGroup altObjGroup;
		altObjGroup.ReadData(nExitID);	
		strTempValue = altObjGroup.getName().GetIDString();
	}

	m_strExit = strTempValue;

	CString strTimeRange = _T(""),strDay = _T("");

	ElapsedTime* etInsertStart = pItinerantFlight->GetArrStartTime();
	long lSecond = etInsertStart->asSeconds();
	strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
	strTimeRange = strDay + _T(" - ");
	ElapsedTime* etInsertEnd = pItinerantFlight->GetArrEndTime();
	if (*etInsertStart == 0L
		&& *etInsertEnd == 0L)
	{
		etInsertEnd->SetTime("23:59:59");
	}
	lSecond = etInsertEnd->asSeconds();
	strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
	strTimeRange += strDay;
	strTempValue = strTimeRange;

	m_strArrTimeWindow = strTempValue;

	etInsertStart = pItinerantFlight->GetDepStartTime();
	lSecond = etInsertStart->asSeconds();
	strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
	strTimeRange = strDay + _T(" - ");
	etInsertEnd = pItinerantFlight->GetDepEndTime();
	if (*etInsertStart == 0L
		&& *etInsertEnd == 0L)
	{
		etInsertEnd->SetTime("23:59:59");
	}
	lSecond = etInsertEnd->asSeconds();
	strDay.Format(_T("Day%d %02d:%02d:%02d"),lSecond/86400+1,(lSecond % 86400)/3600,(lSecond % 86400 % 3600)/60,lSecond % 86400 % 3600 % 60);
	strTimeRange += strDay;
	strTempValue = strTimeRange;

	m_strDepTimeWindow = strTempValue;
}

CDlgItinerantTrafficDefine::~CDlgItinerantTrafficDefine()
{
}

void CDlgItinerantTrafficDefine::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_COMBO_CODE, m_strCode);
	DDX_Control(pDX,IDC_COMBO_CODE,m_comboAirline);
//	DDX_Text(pDX, IDC_EDIT_FLTTYPE, m_strFltType);
	DDX_Text(pDX, IDC_EDIT_ENTRY, m_strEntry);
	DDX_Text(pDX, IDC_EDIT_EXIT, m_strExit);
	DDX_Text(pDX, IDC_EDIT_ARRTIMEWINDOW, m_strArrTimeWindow);
	DDX_Control(pDX,IDC_EDIT_ARRTIMEWINDOW,m_ArrTimeEdit);
	DDX_Text(pDX, IDC_EDIT_DEPTIMEWINDOW,m_strDepTimeWindow);
	DDX_Control(pDX,IDC_EDIT_DEPTIMEWINDOW,m_DepTimtEdit);
	DDX_Text(pDX, IDC_EDIT_NUMBEROFFLIGHTS, m_nFlightCount);
	DDX_Control(pDX, IDC_COMBO_DISTRIBUTION, m_DepComBoDistribution);
	DDX_Control(pDX,IDC_COMBO_ARRDISTRIBUTION,m_ArrComBoDistribution);
	DDX_Control(pDX,IDC_COMBO_ORIGIN,m_comboOrig);
	DDX_CBString(pDX, IDC_COMBO_ORIGIN, m_csOrig);
	DDX_Control(pDX,IDC_COMBO_DEST,m_comboDest);
	DDX_CBString(pDX, IDC_COMBO_DEST, m_csDest);
	DDX_Control(pDX,IDC_COMBO_ACTYPE,m_comboACType);
	DDX_CBString(pDX, IDC_COMBO_ACTYPE, m_csACType);
	DDX_Control(pDX, IDC_CHECK_ENROUTE, m_btnEnRoute);
	DDX_Control(pDX, IDC_CHECK_DEP, m_btnDep);
	DDX_Control(pDX, IDC_CHECK_ARR, m_btnArr);
//	DDX_Control(pDX, IDC_EDIT_FLTTYPE, m_wndEditFltType);
	DDX_Control(pDX, IDC_EDIT_ENTRY, m_wndEditEntry);
	DDX_Control(pDX, IDC_EDIT_EXIT, m_wndEditExit);
	DDX_Control(pDX, IDC_EDIT_NUMBEROFFLIGHTS, m_wndEditNumOfFlts);
}


BEGIN_MESSAGE_MAP(CDlgItinerantTrafficDefine, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_FLTTYPE, OnBnClickedButtonFlttype)
	ON_BN_CLICKED(IDC_BUTTON_ENTRY, OnBnClickedButtonEntry)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_ARRTIMEWINDOW, OnBnClickedButtonTimewindow)
	ON_BN_CLICKED(IDC_BUTTON_DEPTIMEWINDOW,OnBnClickedButtonDepTimeWindow)
//	ON_EN_CHANGE(IDC_EDIT_NAME, OnEnChangeEditName)
	ON_EN_CHANGE(IDC_EDIT_NUMBEROFFLIGHTS, OnEnChangeEditNumberofflights)
	ON_CBN_SELCHANGE(IDC_COMBO_DISTRIBUTION, OnCbnSelchangeComboDistribution)
	ON_CBN_SELCHANGE(IDC_COMBO_ARRDISTRIBUTION,OnCbnSelchangeComboArrDistribution)
//	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_ENROUTE, OnBnClickedCheckEnroute)
	ON_BN_CLICKED(IDC_CHECK_ARR, OnBnClickedCheckArr)
	ON_BN_CLICKED(IDC_CHECK_DEP, OnBnClickedCheckDep)
END_MESSAGE_MAP()


// CDlgItinerantTrafficDefine message handlers

void CDlgItinerantTrafficDefine::OnBnClickedButtonFlttype()
{
	CDlgTimeRange dlg(ElapsedTime(0L), ElapsedTime(0L));
	if(IDOK != dlg.DoModal())
		return;
	CString strTimeRange(dlg.GetStartTimeString());
	strTimeRange += _T(" - ");
	strTimeRange += dlg.GetEndTimeString();
	m_strDepTimeWindow = strTimeRange;
	UpdateData(FALSE);
	m_strDepStartTime = dlg.GetStartTime();
	m_strDepEndTime = dlg.GetEndTime();
//	m_pItinerantFlight->SetDepStartTime(dlg.GetStartTime());
//	m_pItinerantFlight->SetDepEndTime(dlg.GetEndTime());

}

void CDlgItinerantTrafficDefine::OnBnClickedButtonEntry()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CDlgWayPointStandFamily  dlg(m_nProjID);
	if(IDOK == dlg.DoModal())
	{
		m_nEntryWayStandFlag = dlg.GetWayStandFlag();
	//	m_pItinerantFlight->SetEntryFlag(nWayStandFlag);
		if(m_nEntryWayStandFlag == ALTOBJECT_TYPE(ALT_WAYPOINT))
		{
			//m_pItinerantFlight->SetEntryID(dlg.GetSelWayPointID());
			m_nEntryID = dlg.GetSelWayPointID();
			m_strEntry = dlg.GetSelWayPointName();
		}
		else
		{
			//m_pItinerantFlight->SetEntryID(dlg.GetSelStandFamilyID());
			m_nEntryID = dlg.GetSelStandFamilyID();
			m_strEntry = dlg.GetSelStandFamilyName();
		}
		m_bEntry = TRUE;
	}
	UpdateData(FALSE);
}

void CDlgItinerantTrafficDefine::OnBnClickedButtonExit()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CDlgWayPointStandFamily  dlg(m_nProjID);
	if(IDOK == dlg.DoModal())
	{
		m_nExitWayStandFlag = dlg.GetWayStandFlag();
	//	m_pItinerantFlight->SetExitFlag(nWayStandFlag);
		if(m_nExitWayStandFlag == ALTOBJECT_TYPE(ALT_WAYPOINT))
		{
		//	m_pItinerantFlight->SetExitID(dlg.GetSelWayPointID());
			m_nExitID = dlg.GetSelWayPointID();
			m_strExit = dlg.GetSelWayPointName();
		}
		else
		{
		//	m_pItinerantFlight->SetExitID(dlg.GetSelStandFamilyID());
			m_nExitID = dlg.GetSelStandFamilyID();
			m_strExit = dlg.GetSelStandFamilyName();
		}
		m_bExit = TRUE;
	}
	UpdateData(FALSE);
}

void CDlgItinerantTrafficDefine::OnBnClickedButtonTimewindow()
{
	// TODO: Add your control notification handler code here
	CDlgTimeRange dlg(ElapsedTime(0L), ElapsedTime(0L));
	if(IDOK != dlg.DoModal())
		return;
	CString strTimeRange(dlg.GetStartTimeString());
	strTimeRange += _T(" - ");
	strTimeRange += dlg.GetEndTimeString();
	m_strArrTimeWindow = strTimeRange;
	UpdateData(FALSE);
	m_strArrStartTime = dlg.GetStartTime();
	m_strArrEndTime = dlg.GetEndTime();
	m_bArrTime = TRUE;
//	m_pItinerantFlight->SetArrStartTime(dlg.GetStartTime());
//	m_pItinerantFlight->SetArrEndTime(dlg.GetEndTime());
}

void CDlgItinerantTrafficDefine::OnBnClickedButtonDepTimeWindow()
{
	CDlgTimeRange dlg(ElapsedTime(0L), ElapsedTime(0L));
	if(IDOK != dlg.DoModal())
		return;
	CString strTimeRange(dlg.GetStartTimeString());
	strTimeRange += _T(" - ");
	strTimeRange += dlg.GetEndTimeString();
	m_strDepTimeWindow = strTimeRange;
	UpdateData(FALSE);
	m_strDepStartTime = dlg.GetStartTime();
	m_strDepEndTime = dlg.GetEndTime();
	m_bDepTime = TRUE;
//	m_pItinerantFlight->SetDepStartTime(dlg.GetStartTime());
//	m_pItinerantFlight->SetDepEndTime(dlg.GetEndTime());
}

BOOL CDlgItinerantTrafficDefine::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_ArrComBoDistribution.AddString( PROBABILITY_DEFINE_STRING );
	m_DepComBoDistribution.AddString( PROBABILITY_DEFINE_STRING );
	CProbDistManager* pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();
	int nCount = static_cast<int>(pProbDistMan ->getCount());
	for( int m=0; m<nCount; m++ )
	{
		CProbDistEntry* pPBEntry = pProbDistMan->getItem( m );
		m_ArrComBoDistribution.AddString(pPBEntry->m_csName);
		m_DepComBoDistribution.AddString( pPBEntry->m_csName );
	}
	int nIndex = 0;
	if ((nIndex=m_ArrComBoDistribution.FindString(nIndex, m_strArrDistName)) != CB_ERR)
	{
		m_ArrComBoDistribution.SetCurSel(nIndex);
	}
	
	if ((nIndex=m_DepComBoDistribution.FindString(nIndex, m_strDepDistName)) != CB_ERR)
	{
		m_DepComBoDistribution.SetCurSel( nIndex );
	}
	EnableToolTips(TRUE);

	( (CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_FLTCOUNT))->SetRange(1, 2000);

	CAIRLINELIST* pvAirlineList;
	pvAirlineList = _g_GetActiveAirlineMan( m_pInputAirside->m_pAirportDB )->GetAirlineList();
	for(int i=0; i<static_cast<int>(pvAirlineList->size()); i++) 
	{
		CAirline* pAirline = (*pvAirlineList)[i];
		int nIdx = m_comboAirline.AddString( pAirline->m_sAirlineIATACode );
		m_comboAirline.SetItemData( nIdx, i );
	}
	m_comboAirline.SetCurSel( -1 );

	CAIRPORTLIST* pvAirportList;
	pvAirportList = _g_GetActiveAirportMan( m_pInputAirside->m_pAirportDB )->GetAirportList();
	for( int i=0; i<static_cast<int>(pvAirportList->size()); i++) 
	{
		CAirport* pAirport = (*pvAirportList)[i];
		int nIdx = m_comboOrig.AddString( pAirport->m_sIATACode );
		m_comboOrig.SetItemData( nIdx, i );
		nIdx = m_comboDest.AddString( pAirport->m_sIATACode );
		m_comboDest.SetItemData( nIdx, i );
	}
	m_comboOrig.SetCurSel( -1 );
	m_comboDest.SetCurSel( -1 );

	CACTYPELIST* pvACTList;
	pvACTList = _g_GetActiveACMan( m_pInputAirside->m_pAirportDB )->GetACTypeList();
	for(int ii=0; ii<static_cast<int>(pvACTList->size()); ii++) 
	{
		CACType* pACT = (*pvACTList)[ii];
		int nIdx = m_comboACType.AddString( pACT->GetIATACode() );
		m_comboACType.SetItemData( nIdx, ii );
	}
	m_comboACType.SetCurSel( -1 );

	if (!m_bNewItem)
	{
		if (m_pItinerantFlight->GetEnRoute())
		{
			m_btnEnRoute.SetCheck(TRUE);
		}
		else
		{
			m_btnEnRoute.SetCheck(FALSE);
		}
		CString strTemp = _T("");
		m_strCode = m_pItinerantFlight->GetName();
		int nIdx = m_comboAirline.FindString(-1,m_strCode);
		m_comboAirline.SetCurSel(nIdx);

		m_csOrig = m_pItinerantFlight->GetOrig();
		nIdx = m_comboOrig.FindString( -1, m_csOrig );
		m_comboOrig.SetCurSel(nIdx);

		m_csDest = m_pItinerantFlight->GetDest();
		nIdx = m_comboDest.FindString(-1,m_csDest);
		m_comboDest.SetCurSel(nIdx);

		m_csACType = m_pItinerantFlight->GetACType();
		nIdx = m_comboACType.FindString(-1,m_csACType);
		m_comboACType.SetCurSel(nIdx);
		if (!m_pItinerantFlight->GetEnRoute())
		{
			m_btnEnRoute.SetCheck(FALSE);
			if (m_pItinerantFlight->GetEntryID() != -1)
			{
				m_btnArr.SetCheck(TRUE);
				InitArrControl(TRUE);
			}
			else
			{
				m_btnArr.SetCheck(FALSE);
				InitArrControl(FALSE);
			}

			if (m_pItinerantFlight->GetExitID() != -1)
			{
				m_btnDep.SetCheck(TRUE);
				InitDepControl(TRUE);
			}
			else
			{
				m_btnDep.SetCheck(FALSE);
				InitDepControl(FALSE);
			}
		}
		else
		{
			m_btnArr.EnableWindow(FALSE);
			m_btnDep.EnableWindow(FALSE);
			m_btnArr.SetCheck(TRUE);
			m_btnDep.SetCheck(TRUE);

			m_comboOrig.EnableWindow(FALSE);
			m_ArrComBoDistribution.EnableWindow(TRUE);
			m_ArrTimeEdit.EnableWindow(TRUE);
			m_wndEditEntry.EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_ENTRY)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_ARRTIMEWINDOW)->EnableWindow(TRUE);

			m_comboDest.EnableWindow(FALSE);
			m_DepComBoDistribution.EnableWindow(FALSE);
			m_wndEditExit.EnableWindow(TRUE);
			m_DepTimtEdit.EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_DEPTIMEWINDOW)->EnableWindow(FALSE);
		}
	}
	else
	{
		m_btnEnRoute.SetCheck(FALSE);
		m_btnArr.EnableWindow(TRUE);
		InitArrControl(FALSE);
		m_btnDep.EnableWindow(TRUE);
		InitDepControl(FALSE);
	}

	m_bInit = TRUE;
	UpdateData(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}


void CDlgItinerantTrafficDefine::UpDateUIState()
{
	UpdateData(FALSE);

	if(m_nFlightCount > 0 
		&& !m_strCode.IsEmpty()) 
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}

}

void CDlgItinerantTrafficDefine::OnEnChangeEditNumberofflights()
{
	if(!m_bInit)
		return;

	UpdateData(TRUE);
//	m_pItinerantFlight->SetFltCount(m_nFlightCount);

	UpDateUIState();
}

void CDlgItinerantTrafficDefine::OnCbnSelchangeComboDistribution()
{
	// TODO: Add your control notification handler code here	
	int nIndex = m_DepComBoDistribution.GetCurSel();
	CString strSel;

	int n = m_DepComBoDistribution.GetLBTextLen( nIndex );
	m_DepComBoDistribution.GetLBText( nIndex, strSel.GetBuffer(n) );
	strSel.ReleaseBuffer();

	ProbabilityDistribution* pProbDist = NULL;
	CProbDistManager* pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();
	if( strcmp( strSel, PROBABILITY_DEFINE_STRING ) == 0 )
	{
		CProbDistEntry* pPDEntry = NULL;
		pPDEntry = (*m_pSelectProbDistEntry)(NULL, m_pInputAirside);
		if(pPDEntry == NULL)
			return;
		pProbDist = pPDEntry->m_pProbDist;
		assert( pProbDist );
		CString strDistName = pPDEntry->m_csName;
		//if(strDistName == m_strDistName)
		//	return;

		m_strDepDistName = strDistName;
		char szBuffer[1024] = {0};
		pProbDist->screenPrint(szBuffer);
		m_strDepDistScreenPrint = szBuffer;

// 		m_pItinerantFlight->SetDepProbTypes((ProbTypes)pProbDist->getProbabilityType());
 		pProbDist->printDistribution(szBuffer);
// 		m_pItinerantFlight->SetDepPrintDist(szBuffer);
		m_emDepProbType = (ProbTypes)pProbDist->getProbabilityType();
		m_strDepDistPrint = szBuffer;
		int nIndex = 0;
		if ((nIndex=m_DepComBoDistribution.FindString(nIndex, m_strDepDistName)) != CB_ERR)
			m_DepComBoDistribution.SetCurSel( nIndex );	
		else
		{
			nIndex = m_DepComBoDistribution.AddString(m_strDepDistName);
			m_DepComBoDistribution.SetCurSel(nIndex);
		}		
	}
	else
	{
		//if(m_strDistName == strSel)
		//	return;
		CProbDistEntry* pPDEntry = NULL;
		
		int nCount = pProbDistMan->getCount();
		for( int i=0; i<nCount; i++ )
		{
			pPDEntry = pProbDistMan->getItem( i );
			if( strcmp( pPDEntry->m_csName, strSel ) == 0 )
				break;
		}
		//assert( i < nCount );
		pProbDist = pPDEntry->m_pProbDist;
		assert( pProbDist );
		m_strDepDistName = strSel;
		char szBuffer[1024] = {0};
		pProbDist->screenPrint(szBuffer);
		m_strDepDistScreenPrint = szBuffer;
	//	m_pItinerantFlight->SetDepProbTypes((ProbTypes)pProbDist->getProbabilityType());
		pProbDist->printDistribution(szBuffer);
		m_emDepProbType = (ProbTypes)pProbDist->getProbabilityType();
		m_strDepDistPrint = szBuffer;
	//	m_pItinerantFlight->SetDepPrintDist(szBuffer);
	}
	m_bDepDist = TRUE;
//	m_pItinerantFlight->SetDepDistName(m_strDepDistName);
//	m_pItinerantFlight->SetDepDistScreenPrint(m_strDepDistScreenPrint);
}

void CDlgItinerantTrafficDefine::OnCbnSelchangeComboArrDistribution()
{
	int nIndex = m_ArrComBoDistribution.GetCurSel();
	CString strSel;

	int n = m_ArrComBoDistribution.GetLBTextLen( nIndex );
	m_ArrComBoDistribution.GetLBText( nIndex, strSel.GetBuffer(n) );
	strSel.ReleaseBuffer();

	ProbabilityDistribution* pProbDist = NULL;
	CProbDistManager* pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();
	if( strcmp( strSel, PROBABILITY_DEFINE_STRING ) == 0 )
	{
		CProbDistEntry* pPDEntry = NULL;
		pPDEntry = (*m_pSelectProbDistEntry)(NULL, m_pInputAirside);
		if(pPDEntry == NULL)
			return;
		pProbDist = pPDEntry->m_pProbDist;
		assert( pProbDist );
		CString strDistName = pPDEntry->m_csName;
		//if(strDistName == m_strDistName)
		//	return;

		m_strArrDistName = strDistName;
		char szBuffer[1024] = {0};
		pProbDist->screenPrint(szBuffer);
		m_strArrDistScreenPrint = szBuffer;

	//	m_pItinerantFlight->SetArrProbTypes((ProbTypes)pProbDist->getProbabilityType());
		m_emArrProbTypes = (ProbTypes)pProbDist->getProbabilityType();
		pProbDist->printDistribution(szBuffer);
		m_strArrDistPrint = szBuffer;
	//	m_pItinerantFlight->SetArrPrintDist(szBuffer);

		int nIndex = 0;
		if ((nIndex=m_DepComBoDistribution.FindString(nIndex, m_strArrDistName)) != CB_ERR)
			m_ArrComBoDistribution.SetCurSel( nIndex );	
		else
		{
			nIndex = m_ArrComBoDistribution.AddString(m_strArrDistName);
			m_ArrComBoDistribution.SetCurSel(nIndex);
		}		
	}
	else
	{
		//if(m_strDistName == strSel)
		//	return;
		CProbDistEntry* pPDEntry = NULL;

		int nCount = pProbDistMan->getCount();
		for( int i=0; i<nCount; i++ )
		{
			pPDEntry = pProbDistMan->getItem( i );
			if( strcmp( pPDEntry->m_csName, strSel ) == 0 )
				break;
		}
		//assert( i < nCount );
		pProbDist = pPDEntry->m_pProbDist;
		assert( pProbDist );
		m_strArrDistName = strSel;
		char szBuffer[1024] = {0};
		pProbDist->screenPrint(szBuffer);
		m_strArrDistScreenPrint = szBuffer;
	//	m_pItinerantFlight->SetArrProbTypes((ProbTypes)pProbDist->getProbabilityType());
		pProbDist->printDistribution(szBuffer);
		m_emArrProbTypes = (ProbTypes)pProbDist->getProbabilityType();
		m_strArrDistPrint = szBuffer;
	//	m_pItinerantFlight->SetArrPrintDist(szBuffer);
	}
	m_bArrDist = TRUE;
// 	m_pItinerantFlight->SetArrDistName(m_strArrDistName);
// 	m_pItinerantFlight->SetArrDistScreenPrint(m_strArrDistScreenPrint);

}

void CDlgItinerantTrafficDefine::GetAllStand()
{
	//std::vector<int> vAirportIds;
	//InputAirside::GetAirportList(m_nProjID, vAirportIds);
	//for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	//{
	//	std::vector<ALTObject> vWaypoint;
	//	ALTObject::GetObjectList(ALT_WAYPOINT,*iterAirportID,vWaypoint);
	//	std::vector<ALTObject>::iterator iter = vWaypoint.begin();
	//	for (; iter != vWaypoint.end(); ++iter)	
	//		m_vWaypoint.push_back(*iter);
	//}
	ALTObject::GetObjectList(ALT_WAYPOINT,m_nProjID,m_vWaypoint);
}

//void CDlgItinerantTrafficDefine::OnLButtonDblClk(UINT nFlags, CPoint point)
//{
//	// TODO: Add your message handler code here and/or call default
//	AfxMessageBox("6");
//	CDialog::OnLButtonDblClk(nFlags, point);
//}

void CDlgItinerantTrafficDefine::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	int nIdx = -1;
	m_strCode.MakeUpper();
	nIdx = _g_GetActiveAirlineMan(m_pInputAirside->m_pAirportDB)->FindOrAddEntry(m_strCode);
	if (nIdx == -1)
	{
		MessageBox("Invalid Airline Code","Error",MB_ICONEXCLAMATION | MB_OK);
		return;
	}
//	m_pItinerantFlight->SetCode(m_strCode);
	
	if (m_btnArr.GetCheck())
	{
		if (!m_btnEnRoute.GetCheck())
		{	
			m_csOrig.TrimLeft();
			m_csOrig.TrimRight();
			if( !m_csOrig.IsEmpty() )
			{
				m_csOrig.MakeUpper();
				nIdx = _g_GetActiveAirportMan( m_pInputAirside->m_pAirportDB )->FindOrAddEntry( m_csOrig );
				if( nIdx == -1 )
				{
					MessageBox( "Invalid Airport Code", "Error", MB_ICONEXCLAMATION | MB_OK );
					return;
				}
			}
			else
			{
				MessageBox( "Please Define Airport Code", "Error", MB_ICONEXCLAMATION | MB_OK );
				return;
			}
		}
		if(m_strArrDistName.IsEmpty())
		{
			MessageBox( "Please Define Inter-arr Distribution", "Error", MB_ICONEXCLAMATION | MB_OK );
			return;
		}
		if (m_strEntry.IsEmpty())
		{
			MessageBox( "Please Define Entry", "Error", MB_ICONEXCLAMATION | MB_OK );
			return;
		}
	}
	else
	{
			
		m_pItinerantFlight->SetArrDistScreenPrint(_T(""));
		m_pItinerantFlight->SetArrProbTypes(CONSTANT);
		m_pItinerantFlight->SetArrPrintDist(_T("Constant:0.00"));
		m_pItinerantFlight->SetEntryID(-1);
		m_pItinerantFlight->SetEntryFlag(0);
		m_pItinerantFlight->SetOrig(_T(""));
		m_pItinerantFlight->SetArrDistName(_T(""));
		ElapsedTime startTime;
		m_pItinerantFlight->SetArrStartTime(startTime);
		m_pItinerantFlight->SetArrEndTime(startTime);
	}

	if (m_btnDep.GetCheck())
	{
		if (!m_btnEnRoute.GetCheck())
		{
			m_csDest.TrimLeft();
			m_csDest.TrimRight();
			if( !m_csDest.IsEmpty() )
			{
				//MessageBox( "No Dest Airport has been defined", "Error", MB_ICONEXCLAMATION | MB_OK );
				//return;

				m_csDest.MakeUpper();
				nIdx = _g_GetActiveAirportMan( m_pInputAirside->m_pAirportDB )->FindOrAddEntry( m_csDest );
				if( nIdx == -1 )
				{
					MessageBox( "Invalid Airport Code", "Error", MB_ICONEXCLAMATION | MB_OK );
					return;
				}
			}
			else
			{
				MessageBox( "Please Define Airport Code", "Error", MB_ICONEXCLAMATION | MB_OK );
				return;
			}

			if(m_strDepDistName.IsEmpty())
			{
				MessageBox( "Please Define Inter-dep Distribution", "Error", MB_ICONEXCLAMATION | MB_OK );
				return;
			}
		}

		if (m_strExit.IsEmpty())
		{
			MessageBox( "Please Define Exit", "Error", MB_ICONEXCLAMATION | MB_OK );
			return;
		}
	}
	else
	{
		m_pItinerantFlight->SetExitID(-1);
		m_pItinerantFlight->SetDepDistName(_T(""));
		m_pItinerantFlight->SetDepDistScreenPrint(_T(""));	
		m_pItinerantFlight->SetDepProbTypes(CONSTANT);
		m_pItinerantFlight->SetDepPrintDist(_T("Constant:0.00"));
		m_pItinerantFlight->SetDest(_T(""));
		m_pItinerantFlight->SetExitFlag(0);
		ElapsedTime depTime;
		m_pItinerantFlight->SetDepStartTime(depTime);
		m_pItinerantFlight->SetDepEndTime(depTime);
	}

	if(!m_btnArr.GetCheck() && !m_btnDep.GetCheck())
	{
		MessageBox("Please check arrival or departure","Error",MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	
	if (m_btnArr.GetCheck() && m_btnDep.GetCheck() && !m_btnEnRoute.GetCheck())
	{
		ElapsedTime DepStartTime;
		ElapsedTime ArrStartTime;
		if (m_bArrTime)
		{
			ArrStartTime = m_strArrStartTime;
		}
		else
		{
			ArrStartTime = *(m_pItinerantFlight->GetArrStartTime()) ;
		}

		if (m_bDepTime)
		{
			DepStartTime = m_strDepStartTime;
		}
		else
		{
			DepStartTime = *(m_pItinerantFlight->GetDepStartTime());
		}
		if (DepStartTime < ArrStartTime)
		{
			MessageBox( "Dep Time / Arr Time are not valid.","Error", MB_ICONEXCLAMATION | MB_OK );
			return;
		}
	
	}
	m_csACType.TrimLeft();
	m_csACType.TrimRight();
	if( m_csACType.IsEmpty() )
	{
		MessageBox( "No AC Type has been defined", "Error", MB_ICONEXCLAMATION );
		return;
	}
	m_csACType.MakeUpper();
	CACType* pactype = NULL ;
	pactype = _g_GetActiveACMan( m_pInputAirside->m_pAirportDB )->getACTypeItem( m_csACType );
	if( pactype == NULL )
	{
		CString errormsg ;
		errormsg.Format(_T("Invalid ACType,please define the actype entry for \'%s\'"),m_csACType) ;
		MessageBox(errormsg ,"Error", MB_ICONEXCLAMATION | MB_OK );
		return;
	}
	m_pItinerantFlight->SetCode(m_strCode);
	m_pItinerantFlight->SetFltCount(m_nFlightCount);
	m_pItinerantFlight->SetACType(m_csACType);

	m_pItinerantFlight->SetOrig(m_csOrig);
	if (m_bArrTime)
	{
		m_pItinerantFlight->SetArrStartTime(m_strArrStartTime);
		m_pItinerantFlight->SetArrEndTime(m_strArrEndTime);
	}
	if (m_bArrDist)
	{
		m_pItinerantFlight->SetArrProbTypes(m_emArrProbTypes);
		m_pItinerantFlight->SetArrDistName(m_strArrDistName);
		m_pItinerantFlight->SetArrDistScreenPrint(m_strArrDistScreenPrint);
		m_pItinerantFlight->SetArrPrintDist(m_strArrDistPrint);
	}
	if (m_bEntry)
	{
		m_pItinerantFlight->SetEntryFlag(m_nEntryWayStandFlag);
		m_pItinerantFlight->SetEntryID(m_nEntryID);
	}

	m_pItinerantFlight->SetDest(m_csDest);
	if (m_bDepTime)
	{
		m_pItinerantFlight->SetDepStartTime(m_strDepStartTime);
		m_pItinerantFlight->SetDepEndTime(m_strDepEndTime);
	}

	if (m_bDepDist)
	{
		m_pItinerantFlight->SetDepProbTypes(m_emDepProbType);
		m_pItinerantFlight->SetDepDistName(m_strDepDistName);
		m_pItinerantFlight->SetDepDistScreenPrint(m_strDepDistScreenPrint);
		m_pItinerantFlight->SetDepPrintDist(m_strDepDistPrint);
	}

	if (m_bExit)
	{
		m_pItinerantFlight->SetExitFlag(m_nExitWayStandFlag);
		m_pItinerantFlight->SetExitID(m_nExitID);
	}

	if(m_btnEnRoute.GetCheck())
	{
		m_pItinerantFlight->SetEnRoute(TRUE);
		ElapsedTime DepTime;
		m_pItinerantFlight->SetDepStartTime(DepTime);
		m_pItinerantFlight->SetDepEndTime(DepTime);
		m_pItinerantFlight->SetDepDistName(_T(""));
		m_pItinerantFlight->SetDepDistScreenPrint(_T(""));	
		m_pItinerantFlight->SetDepProbTypes(CONSTANT);
		m_pItinerantFlight->SetDepPrintDist(_T("Constant:0.00"));
	}
	else
	{
		m_pItinerantFlight->SetEnRoute(FALSE);
	}
	OnOK();
}

void CDlgItinerantTrafficDefine::OnBnClickedCheckEnroute()
{
	// TODO: Add your control notification handler code here
	if(m_btnEnRoute.GetCheck())
	{
	//	m_pItinerantFlight->SetEnRoute(TRUE);
		m_btnArr.EnableWindow(FALSE);
		m_btnDep.EnableWindow(FALSE);
		m_btnArr.SetCheck(TRUE);
		m_btnDep.SetCheck(TRUE);
		InitArrControl(FALSE);
		InitDepControl(FALSE);

		m_ArrComBoDistribution.EnableWindow(TRUE);
		m_ArrTimeEdit.EnableWindow(TRUE);
		m_wndEditEntry.EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ENTRY)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ARRTIMEWINDOW)->EnableWindow(TRUE);

		m_DepComBoDistribution.EnableWindow(FALSE);
		m_wndEditExit.EnableWindow(TRUE);
		m_DepTimtEdit.EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DEPTIMEWINDOW)->EnableWindow(FALSE);
	}
	else
	{
	//	m_pItinerantFlight->SetEnRoute(FALSE);
		m_btnArr.EnableWindow(TRUE);
		m_btnDep.EnableWindow(TRUE);
		m_btnArr.SetCheck(FALSE);
		m_btnDep.SetCheck(FALSE);

		m_ArrComBoDistribution.EnableWindow(FALSE);
		m_comboOrig.EnableWindow(FALSE);
		m_ArrTimeEdit.EnableWindow(FALSE);
		m_wndEditEntry.EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ENTRY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ARRTIMEWINDOW)->EnableWindow(FALSE);

		m_DepComBoDistribution.EnableWindow(FALSE);
		m_wndEditExit.EnableWindow(FALSE);
		m_DepTimtEdit.EnableWindow(FALSE);
		m_comboDest.EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DEPTIMEWINDOW)->EnableWindow(FALSE);

		m_ArrComBoDistribution.SetCurSel(-1);
		m_comboOrig.SetCurSel(-1);
		m_ArrTimeEdit.SetWindowText(_T(""));
		m_wndEditEntry.SetWindowText(_T(""));

		m_DepComBoDistribution.SetCurSel(-1);
		m_comboDest.SetCurSel(-1);
		m_DepTimtEdit.SetWindowText(_T(""));
		m_wndEditExit.SetWindowText(_T(""));
	}
}

void CDlgItinerantTrafficDefine::OnBnClickedCheckArr()
{
	// TODO: Add your control notification handler code here
	if (m_btnArr.GetCheck())
	{
		InitArrControl(TRUE);
	}
	else
	{
		InitArrControl(FALSE);
	}
}

void CDlgItinerantTrafficDefine::OnBnClickedCheckDep()
{
	// TODO: Add your control notification handler code here
	if (m_btnDep.GetCheck())
	{
		InitDepControl(TRUE);
	}
	else
	{
		InitDepControl(FALSE);
	}
}

void CDlgItinerantTrafficDefine::InitArrControl(BOOL bEnable)
{
	if (bEnable)
	{
		m_ArrComBoDistribution.EnableWindow(TRUE);
		m_wndEditEntry.EnableWindow(TRUE);
		m_ArrTimeEdit.EnableWindow(TRUE);
		m_comboOrig.EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ENTRY)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ARRTIMEWINDOW)->EnableWindow(TRUE);
	}
	else
	{
		m_ArrComBoDistribution.EnableWindow(FALSE);
		m_comboOrig.EnableWindow(FALSE);
		m_ArrTimeEdit.EnableWindow(FALSE);
		m_wndEditEntry.EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ENTRY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ARRTIMEWINDOW)->EnableWindow(FALSE);
	
		m_ArrComBoDistribution.SetCurSel(-1);
		m_comboOrig.SetCurSel(-1);
		m_ArrTimeEdit.SetWindowText(_T(""));
		m_wndEditEntry.SetWindowText(_T(""));	
	}
}

void CDlgItinerantTrafficDefine::InitDepControl(BOOL bEnable)
{
	if (bEnable)
	{
		m_DepComBoDistribution.EnableWindow(TRUE);
		m_wndEditExit.EnableWindow(TRUE);
		m_DepTimtEdit.EnableWindow(TRUE);
		m_comboDest.EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DEPTIMEWINDOW)->EnableWindow(TRUE);
	}
	else
	{
		m_DepComBoDistribution.EnableWindow(FALSE);
		m_wndEditExit.EnableWindow(FALSE);
		m_DepTimtEdit.EnableWindow(FALSE);
		m_comboDest.EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DEPTIMEWINDOW)->EnableWindow(FALSE);

		m_DepComBoDistribution.SetCurSel(-1);
		m_comboDest.SetCurSel(-1);
		m_DepTimtEdit.SetWindowText(_T(""));
		m_wndEditExit.SetWindowText(_T(""));
	}
}