// FlightDialog.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "FlightDialog.h"
#include "TermPlanDoc.h"
#include "StopOverAirportDialog.h"

#include "..\inputs\in_term.h"
#include "..\inputs\schedule.h"
#include "..\common\AirlineManager.h"
#include "..\common\AirportsManager.h"
#include "..\common\ACTypesManager.h"
#include "../Common/FlightManager.h"
#include "../Common/AirportDatabase.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const int SELECTED_TEXT_LEN = 1024;


CFlightDialog::CFlightDialog(CWnd* pParent, bool bShowThroughout)
 : CDialog(CFlightDialog::IDD, pParent)
 , m_bShowThroughoutMode(bShowThroughout)
{
}

void CFlightDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFlightDialog)
	DDX_Control(pDX, IDC_TREE_AIRLINE_FLT, m_AirlineTree);
	DDX_Control(pDX, IDC_LIST_AIRLINE_GROUP,m_listAirlineGroup);
	DDX_Control(pDX, IDC_LIST_SECTOR_FLT, m_Sector);
	DDX_Control(pDX, IDC_LIST_CATEGORY_FLT, m_Category);
	DDX_Control(pDX, IDC_LIST_ACTYPE_FLT, m_ACType);
	DDX_Control(pDX, IDC_LIST_AIRPORT_FLT, m_Airport);
	DDX_Text(pDX, IDC_EDIT_FLIGHT, m_strFlight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFlightDialog, CDialog)
	//{{AFX_MSG_MAP(CFlightDialog)
	ON_BN_CLICKED(IDC_RADIO_ALLFLIGHTS_FLT, OnRadioAllflightsFlt)
	ON_BN_CLICKED(IDC_RADIO_ARRIVING_FLT, OnRadioArrivingFlt)
	ON_BN_CLICKED(IDC_RADIO_DEPARTING_FLT, OnRadioDepartingFlt)
	ON_BN_CLICKED(IDC_RADIO_THROUGHOUT_FLT, OnRadioThroughoutFlt)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_AIRLINE_FLT, OnSelchangedTreeAirlineFlt)
	ON_LBN_SELCHANGE(IDC_LIST_AIRLINE_GROUP, OnSelchangeListAirlineGroup)
	ON_LBN_SELCHANGE(IDC_LIST_AIRPORT_FLT, OnSelchangeListAirportFlt)
	ON_LBN_SELCHANGE(IDC_LIST_ACTYPE_FLT, OnSelchangeListActypeFlt)
	ON_LBN_SELCHANGE(IDC_LIST_SECTOR_FLT, OnSelchangeListSectorFlt)
	ON_LBN_SELCHANGE(IDC_LIST_CATEGORY_FLT, OnSelchangeListCategoryFlt)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_STOPOVERAIRPORT, OnClickStopOverAirport)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlightDialog message handlers

BOOL CFlightDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	AfxGetApp()->BeginWaitCursor();
	ResetSeletion();
	AfxGetApp()->EndWaitCursor();
	//m_FlightSelection.SetInputTerminal( GetInputTerminal() );
	m_FlightSelection.SetAirportDB(GetInputTerminal()->m_pAirportDB);

	CButton* pButton = (CButton*) GetDlgItem(IDC_RADIO_THROUGHOUT_FLT);
	if (pButton == NULL)
		return TRUE;
	if (!m_bShowThroughoutMode)
		pButton->ShowWindow(SW_HIDE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFlightDialog::OnRadioAllflightsFlt() 
{
	if ( m_FlightSelection.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ALL )
		return;

	CButton * pAll = (CButton *) GetDlgItem(IDC_RADIO_ALLFLIGHTS_FLT);
	if (pAll == NULL)
		return;
	
	pAll->SetCheck(1);
	m_FlightSelection.SetFltConstraintMode( ENUM_FLTCNSTR_MODE_ALL );

	ResetFlightID();
}

void CFlightDialog::OnRadioArrivingFlt() 
{
	if (m_FlightSelection.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ARR )
		return;

	CButton * pArriving = (CButton *) GetDlgItem(IDC_RADIO_ARRIVING_FLT);
	if (pArriving == NULL)
		return;
	
	pArriving->SetCheck(1);
	m_FlightSelection.SetFltConstraintMode( ENUM_FLTCNSTR_MODE_ARR );

	ResetFlightID();
}

void CFlightDialog::OnRadioDepartingFlt() 
{
	if (m_FlightSelection.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_DEP )
		return;

	CButton * pDeparting = (CButton *) GetDlgItem(IDC_RADIO_DEPARTING_FLT);
	if (pDeparting == NULL)
		return;
	
	pDeparting->SetCheck(1);
	m_FlightSelection.SetFltConstraintMode( ENUM_FLTCNSTR_MODE_DEP );

	ResetFlightID();
}

void CFlightDialog::OnRadioThroughoutFlt()
{
	if (m_FlightSelection.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_THU )
		return;

	CButton* pButton = (CButton*) GetDlgItem(IDC_RADIO_THROUGHOUT_FLT);
	if (pButton == NULL)
		return;

	pButton->SetCheck(1);
	m_FlightSelection.SetFltConstraintMode(ENUM_FLTCNSTR_MODE_THU);

	ResetFlightID();
}

void CFlightDialog::OnSelchangedTreeAirlineFlt(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hItem = NULL, hParentItem = NULL;

	hItem = pNMTreeView->itemNew.hItem;

	CString strAlirline, strFlightID;
	if (hItem == NULL)
	{
		strAlirline.Empty();
		strFlightID.Empty();
	}
	else
	{
		hParentItem = m_AirlineTree.GetParentItem(hItem);
		if (hParentItem == NULL)
		{
			strAlirline = m_AirlineTree.GetItemText(hItem);   // Get Airline
			strFlightID.Empty();
		}
		else
		{
			strAlirline = m_AirlineTree.GetItemText(hParentItem);   // Get Airline
			strFlightID = m_AirlineTree.GetItemText(hItem);         // Get Flight ID
		}
	}
	
	m_FlightSelection.setAirline(strAlirline);

	CFlightID fltID;
	fltID = strFlightID;

	m_FlightSelection.setFlightID( fltID );	

	ResetFlightID();
	
	*pResult = 0;
}

void CFlightDialog::OnSelchangeListAirportFlt() 
{
	int nIndex = m_Airport.GetCurSel();
	if (nIndex < 0)
		return;

	CString strSel;
	m_Airport.GetText(nIndex, strSel);

	CString strTemp;
	m_FlightSelection.getAirport(strTemp.GetBuffer(512));
	if (strTemp == strSel)
	{
		m_Airport.SetSel(-1);
		strSel = "";
	}

	m_FlightSelection.setAirport(strSel);
	
	ResetFlightID();

}

void CFlightDialog::OnSelchangeListActypeFlt() 
{
	int nIndex = m_ACType.GetCurSel();
	if (nIndex < 0)
		return;
	
	CString strSel;
	m_ACType.GetText(nIndex, strSel);
	
	CString strTemp;
	m_FlightSelection.getACType(strTemp.GetBuffer(512));
	if (strTemp == strSel)
	{
		m_ACType.SetSel(-1);
		strSel = "";
	}
	
	m_FlightSelection.setACType(strSel);
	
	ResetFlightID();
}

void CFlightDialog::OnSelchangeListSectorFlt() 
{
	int nIndex = m_Sector.GetCurSel();
	if (nIndex < 0)
		return;

	CString strSel;
	m_Sector.GetText(nIndex, strSel);

	CString strTemp;
	m_FlightSelection.getSector(strTemp.GetBuffer(0));
	if (strTemp == strSel)
	{
		m_Sector.SetSel(-1);
		strSel = "";
	}


	//m_FlightSelection.SetInputTerminal( GetInputTerminal() );
	m_FlightSelection.SetAirportDB(GetInputTerminal()->m_pAirportDB);

	m_FlightSelection.setSector(strSel);
	
	ResetFlightID();	
}

void CFlightDialog::OnSelchangeListCategoryFlt() 
{
	int nIndex = m_Category.GetCurSel();
	if (nIndex < 0)
		return;

	CString strSel;
	m_Category.GetText(nIndex, strSel);

	char szTemp[256] = {0};
	m_FlightSelection.getCategory(szTemp);
	CString strTemp(szTemp);
	if (strTemp == strSel)
	{
		m_Category.SetSel(-1);
		strSel = "";
	}

	//m_FlightSelection.SetInputTerminal( GetInputTerminal() );
	m_FlightSelection.SetAirportDB(GetInputTerminal()->m_pAirportDB);
	m_FlightSelection.setCategory(strSel);
	
	ResetFlightID();
}

void CFlightDialog::OnSelchangeListAirlineGroup()
{
	CSUBAIRLINELIST* pvSList;
	pvSList = _g_GetActiveAirlineMan( GetInputTerminal()->m_pAirportDB )->GetSubairlineList();
	int nIndex = m_listAirlineGroup.GetCurSel();
	if (nIndex < 0)
		return;

	CString strSel;
	m_listAirlineGroup.GetText(nIndex, strSel);

	CString strTemp;
	m_FlightSelection.getAirlineGroup(strTemp.GetBuffer(255));
	if (strTemp == strSel)
	{
		m_listAirlineGroup.SetSel(-1);
		strSel = "";
	}
	//m_FlightSelection.SetInputTerminal( GetInputTerminal() );
	m_FlightSelection.getFlightGroup(strTemp.GetBuffer(255));
	if (strTemp == strSel)
	{
		m_listAirlineGroup.SetSel(-1);
		strSel = "";
	}


	m_FlightSelection.SetAirportDB(GetInputTerminal()->m_pAirportDB);

	if ((int)pvSList->size() > nIndex)
	{
		m_FlightSelection.setAirlineGroup(strSel);
	}
	else
	{
		m_FlightSelection.setFlightGroup(strSel);
	}
	ResetFlightID();		
}

void CFlightDialog::Setup(const FlightConstraint &FlightSelection,int Instrinsic)
{
	m_FlightSelection    = FlightSelection;
    m_Intrinsic = Instrinsic;
	// Set to default mode (not 'A' or 'D').
	m_FlightSelection.SetFltConstraintMode( ENUM_FLTCNSTR_MODE_ALL );
	
	// Reset all the selection.
	ResetSeletion();
}

void CFlightDialog::ResetSeletion()
{
	if (!::IsWindow(GetSafeHwnd()))
		return;

	ResetFlightID();

	int i = 0, j = 0;

	// 
	// Radio controls
	//
	
	FlightConstraint sel;
	sel = m_FlightSelection;	

	CButton* pBtn = NULL;
	if ( sel.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ALL )
	{
		pBtn = (CButton *) GetDlgItem(IDC_RADIO_ALLFLIGHTS_FLT);
		if (pBtn != NULL)
			pBtn->SetCheck(1);
	}
    else if (sel.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ARR )
	{
		pBtn = (CButton *) GetDlgItem(IDC_RADIO_ARRIVING_FLT);
		if (pBtn != NULL)
			pBtn->SetCheck(1);
	}
    else if (sel.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_DEP )
	{
		pBtn = (CButton *) GetDlgItem(IDC_RADIO_DEPARTING_FLT);
		if (pBtn != NULL)
			pBtn->SetCheck(1);
	}
	else if (sel.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_THU )
	{
		pBtn = (CButton *) GetDlgItem(IDC_RADIO_THROUGHOUT_FLT);
		if (pBtn != NULL)
			pBtn->SetCheck(1);
	}
	
	//
	// Flight stuff
	//

	StringList strDict, strDict2;
	TVITEM ti;
	TVINSERTSTRUCT tis;
	HTREEITEM hItem;
	
	// Airlines 
	CString strTemp;
	CAIRLINELIST* pvAirlineList;
	pvAirlineList = _g_GetActiveAirlineMan( GetInputTerminal()->m_pAirportDB )->GetAirlineList();
	for(i=0; i<static_cast<int>(pvAirlineList->size()); i++) 
	{
		CAirline* pAirline = (*pvAirlineList)[i];
		strTemp = pAirline->m_sAirlineIATACode;
		strTemp.TrimLeft(); strTemp.TrimRight();
		if (strTemp.IsEmpty())
			continue;

		ti.mask       = TCIF_TEXT;		
		ti.cChildren  = 1;
		ti.pszText    = (LPSTR) strTemp.GetBuffer(0);
		ti.cchTextMax = 256;

		tis.hParent   = TVI_ROOT;
		tis.hInsertAfter = TVI_SORT;
		tis.item      = ti;
		
		hItem = m_AirlineTree.InsertItem(&tis);
		if (hItem == NULL)
			continue;

		sel.setAirline(strTemp);
		GetInputTerminal()->flightSchedule->getFlightList(strDict2, sel);
		for (j = 0; j < strDict2.getCount(); j++)
		{
			strTemp = strDict2.getString(j);
			strTemp.TrimLeft(); strTemp.TrimRight();
			if (strTemp.IsEmpty())
				continue;
			
			ti.pszText    = (LPSTR) strTemp.GetBuffer(0);
			
			tis.hParent   = hItem;
			tis.item      = ti;
			
			m_AirlineTree.InsertItem(&tis);
		}
		
		m_AirlineTree.Expand(hItem, TVE_EXPAND);
	}
	sel.setAirline("");

	//Airline Group
	CSUBAIRLINELIST* pvSList;
	pvSList = _g_GetActiveAirlineMan( GetInputTerminal()->m_pAirportDB )->GetSubairlineList();
	for(i=0; i<static_cast<int>(pvSList->size()); i++)
	{
		CSubairline* pSubairline = (*pvSList)[i];
		m_listAirlineGroup.InsertString(-1,pSubairline->m_sName );
	}

	FLIGHTGROUPLIST* pGroups;
	pGroups = _g_GetActiveFlightMan( GetInputTerminal()->m_pAirportDB)->GetFlightGroups();
	for (i=0;i<static_cast<int>(pGroups->size());i++)
	{
		FlightGroup* pGroup = (*pGroups)[i];
		m_listAirlineGroup.AddString(pGroup->getGroupName());
	}

	// Airlines 
	/*
	CString strTemp;
	GetInputTerminal()->flightSchedule->getCarrierList (strDict, sel);
	for (i = 0; i < strDict.getCount(); i++)	
	{
		strTemp = strDict.getString(i);
		strTemp.TrimLeft(); strTemp.TrimRight();
		if (strTemp.IsEmpty())
			continue;

		ti.mask       = TCIF_TEXT;		
		ti.cChildren  = 1;
		ti.pszText    = (LPSTR) strTemp.GetBuffer(0);
		ti.cchTextMax = 256;

		tis.hParent   = TVI_ROOT;
		tis.hInsertAfter = TVI_SORT;
		tis.item      = ti;
		
		hItem = m_AirlineTree.InsertItem(&tis);
		if (hItem == NULL)
			continue;

		sel.setAirline(strTemp);
		GetInputTerminal()->flightSchedule->getFlightList(strDict2, sel);
		for (j = 0; j < strDict2.getCount(); j++)
		{
			strTemp = strDict2.getString(j);
			strTemp.TrimLeft(); strTemp.TrimRight();
			if (strTemp.IsEmpty())
				continue;
			
			ti.pszText    = (LPSTR) strTemp.GetBuffer(0);
			
			tis.hParent   = hItem;
			tis.item      = ti;
			
			m_AirlineTree.InsertItem(&tis);
		}
		
		m_AirlineTree.Expand(hItem, TVE_EXPAND);
	}
	sel.setAirline("");
	*/
	// Airport
	CAIRPORTLIST* pvAirportList;
	pvAirportList = _g_GetActiveAirportMan( GetInputTerminal()->m_pAirportDB )->GetAirportList();
	for( i=0; i<static_cast<int>(pvAirportList->size()); i++) 
	{
		CAirport* pAirport = (*pvAirportList)[i];
		m_Airport.AddString( pAirport->m_sIATACode );
	}
	
	

/*	sel.setMode('A');
	GetInputTerminal()->flightSchedule->getAirportList (strDict, sel);
	for (i = 0; i < strDict.getCount(); i++)
	{
		m_Airport.AddString(strDict.getString (i));
	}
	sel.setMode('D');
	GetInputTerminal()->flightSchedule->getAirportList (strDict, sel);
	for (i = 0; i < strDict.getCount(); i++)
	{
		m_Airport.AddString(strDict.getString (i));
	}	
*/
	// AC Type
		CACTYPELIST* pvACTList;
	pvACTList = _g_GetActiveACMan( GetInputTerminal()->m_pAirportDB )->GetACTypeList();
	for( i=0; i<static_cast<int>(pvACTList->size()); i++) 
	{
		CACType* pACT = (*pvACTList)[i];
		m_ACType.AddString( pACT->GetIATACode() );
	}

/*	sel.setMode('A');
	GetInputTerminal()->flightSchedule->getACTypeList (strDict, sel);
	for (i = 0; i < strDict.getCount(); i++)
	{
		m_ACType.AddString(strDict.getString(i));
	}
	sel.setMode('D');
	GetInputTerminal()->flightSchedule->getACTypeList (strDict, sel);
	for (i = 0; i < strDict.getCount(); i++)
	{
		m_ACType.AddString(strDict.getString(i));
	}	
*/
	// Sector
	CSECTORLIST* pvSector;
	pvSector = _g_GetActiveAirportMan( GetInputTerminal()->m_pAirportDB )->GetSectorList();
		for( i=0; i<static_cast<int>(pvSector->size()); i++) 
	{
		CSector* pSector = (*pvSector)[i];
		m_Sector.AddString( pSector->m_sName );
	}

	/*
	sel.initDefault();
	GetInputTerminal()->flightSchedule->getSectors (strDict, sel);
	for (i = 0; i < strDict.getCount(); i++)
	{
		m_Sector.AddString(strDict.getString (i));
	}
*/	
	// Category 
	CACCATEGORYLIST* pvACCategory;
	pvACCategory = _g_GetActiveACMan( GetInputTerminal()->m_pAirportDB )->GetACCategoryList();
	for( i=0; i<static_cast<int>(pvACCategory->size()); i++) 
	{
		CACCategory* pACCategory = (*pvACCategory)[i];
		m_Category.AddString( pACCategory->m_sName );
	}

	/*
	sel.initDefault();
	GetInputTerminal()->flightSchedule->getCategories (strDict, sel);
	for (i = 0; i < strDict.getCount(); i++)
	{
		m_Category.AddString(strDict.getString (i));
	}*/

	if(m_Intrinsic==4) 
	{
		pBtn->SetCheck(0);
		OnRadioArrivingFlt();
	}
}

void CFlightDialog::ResetFlightID()
{
	TCHAR szFlight[SELECTED_TEXT_LEN];

	m_FlightSelection.screenPrint(szFlight);
	
	CEdit* pEdit = (CEdit *) GetDlgItem(IDC_EDIT_FLIGHT);
	if (pEdit != NULL)
	{
		pEdit->SetWindowText(szFlight);
	}
}

InputTerminal* CFlightDialog::GetInputTerminal()
{
	if(m_pParentWnd != NULL)
	{
		CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
		return (InputTerminal*)&pDoc->GetTerminal();
	}
	
	else
	{
		
		POSITION ps = theApp.m_pDocManager->GetFirstDocTemplatePosition();
		POSITION ps2 = theApp.m_pDocManager->GetNextDocTemplate(ps)->GetFirstDocPosition();
		CTermPlanDoc *pDoc = (CTermPlanDoc*)(theApp.m_pDocManager->GetNextDocTemplate(ps)->GetNextDoc(ps2));
		return (InputTerminal*)&pDoc->GetTerminal();
	}
}

void CFlightDialog::OnClickStopOverAirport()
{
	CStopOverAirportDialog dlg(GetInputTerminal(), this);
	if (IDOK == dlg.DoModal())
	{
		m_FlightSelection.setStopoverAirport(dlg.GetSelectedAirport());
		m_FlightSelection.setStopoverSector(dlg.GetSelectedSector());
		ResetFlightID();
	}
}
