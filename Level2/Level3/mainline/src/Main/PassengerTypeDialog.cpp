// PassengerTypeDialog.cpp : implementation file
//
#include "stdafx.h"
#include "termplan.h"
#include "mainfrm.h"
#include "TermPlanDoc.h"
#include "CompareReportDoc.h"
#include "DlgSelMultiPaxType.h"
#include "PassengerTypeDialog.h"
#include "MoblieElemDlg.h"
#include "FlightDialog.h"
#include "StopOverAirportDialog.h"

#include "../inputs/in_term.h"
#include "../inputs/paxflow.h"
#include "../inputs/typelist.h"
#include "../inputs/probab.h"
#include "../inputs/typelist.h"
#include "../inputs/schedule.h"
#include "../Inputs/TrayHost.h"
#include "../inputs/MobileElemTypeStrDB.h"
#include "../common/AirlineManager.h"
#include "../common/AirportsManager.h"
#include "../common/ACTypesManager.h"
#include "../Common/FlightManager.h"
#include "../common/WinMsg.h"
#include "../Common/AirportDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPassengerTypeDialog dialog


CPassengerTypeDialog::CPassengerTypeDialog(CWnd* pParent,BOOL _bOnlyPax, BOOL bNoDefault)
	: CDialog(CPassengerTypeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPassengerTypeDialog)
	m_strPaxType = _T("");
	m_bAllBranchesInType = FALSE;
	//}}AFX_DATA_INIT
	bOnlyPax = _bOnlyPax ? true : false;
	m_bShowFlightID = TRUE;
	m_bIsNewItem = false;

	m_bNewVersion = FALSE;
	m_pTerminal = NULL;
	m_bDisableDefault = bNoDefault;
}


void CPassengerTypeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPassengerTypeDialog)
	DDX_Control(pDX, IDC_CHECK_ALLTYPE, m_CheckAllType);
	DDX_Control(pDX, IDC_LIST_PASSENGER, m_listPassenger);
	DDX_Control(pDX, IDC_TREE_PAXTYPE, m_treePaxType);
	DDX_Control(pDX, IDC_LIST_AIRLINE_GROUP, m_listAirlineGroup);
	DDX_Control(pDX, IDC_BUT_EDIT_NONPAX, m_butEdit);
	DDX_Control(pDX, IDC_COMBO_NOPAX, m_comboMobileElem);
	DDX_Control(pDX, IDC_TREE_AIRLINE, m_AirlineTree);
	DDX_Control(pDX, IDC_TREE_PASSENGERTYPE, m_PaxTypeTree);
	DDX_Control(pDX, IDC_LIST_SECTOR_PXTD, m_Sector);
	DDX_Control(pDX, IDC_LIST_CATEGORY_PXTD, m_Category);
	DDX_Control(pDX, IDC_LIST_AIRPORT_PXTD, m_Airport);
	DDX_Control(pDX, IDC_LIST_ACTYPE_PXTD, m_ACType);
	DDX_Text(pDX, IDC_EDIT_PASSENGERTYPE, m_strPaxType);
	DDX_Control(pDX, IDC_BUTTON_OTHERFLIGHT, m_butOtherFlight);
	DDX_Check(pDX, IDC_CHECK_ALLTYPE, m_bAllBranchesInType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPassengerTypeDialog, CDialog)
	//{{AFX_MSG_MAP(CPassengerTypeDialog)
	ON_LBN_SELCHANGE(IDC_LIST_ACTYPE_PXTD, OnSelchangeListActypePxtd)
	ON_LBN_SELCHANGE(IDC_LIST_AIRPORT_PXTD, OnSelchangeListAirportPxtd)
	ON_LBN_SELCHANGE(IDC_LIST_CATEGORY_PXTD, OnSelchangeListCategoryPxtd)
	ON_LBN_SELCHANGE(IDC_LIST_SECTOR_PXTD, OnSelchangeListSectorPxtd)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_AIRLINE, OnSelchangedTreeAirline)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PASSENGERTYPE, OnSelchangedTreePassengertype)
	ON_NOTIFY(TVN_ITEMEXPANDING,IDC_TREE_PASSENGERTYPE, OnTvnItemexpanding)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnRadioAll)
	ON_BN_CLICKED(IDC_RADIO_ARRIVING, OnRadioArriving)
	ON_BN_CLICKED(IDC_RADIO_DEPARTING, OnRadioDeparting)
	ON_BN_CLICKED(IDCCANCEL, OnCcancel)
	ON_MESSAGE(WM_UNSELECTABLELISTBOX_SELCHANGE,OnUnselectableListBoxSelchange)
	ON_MESSAGE(WM_UNSELECTABLELTREE_SELCHANGE,OnUnselectableTreeSelchange)
	ON_BN_CLICKED(IDC_BUT_EDIT_NONPAX, OnButEditNonpax)
	ON_BN_CLICKED(IDC_RADIO_PAX, OnRadioPax)
	ON_BN_CLICKED(IDC_RADIO_NONPAX, OnRadioNonpax)
	ON_CBN_SELCHANGE(IDC_COMBO_NOPAX, OnSelchangeComboNopax)
	ON_LBN_SELCHANGE(IDC_LIST_AIRLINE_GROUP, OnSelchangeListAirlineGroup)
	ON_BN_CLICKED(IDC_BUTTON_OTHERFLIGHT, OnButtonOtherflight)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PASSENGER, OnItemchangedListPassenger)
	ON_BN_CLICKED(IDC_RADIO_ALLTYPE, OnRadioAlltype)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PASSENGER, OnDblclkListPassenger)
	ON_BN_CLICKED(IDC_CHECK_ALLTYPE, OnCheckAlltype)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnAdd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnDelete)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_PASSENGER, OnEndlabeleditList)
	
	ON_BN_CLICKED(IDC_STOPOVERAIRPORT, OnClickStopOverAirport)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPassengerTypeDialog message handlers

BOOL CPassengerTypeDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//m_PaxSelection.SetInputTerminal( GetInputTerminal() );
	InputTerminal* pTerm = GetInputTerminal();
	if(pTerm == NULL)
		return TRUE;
	m_MobileElem.SetInputTerminal( GetInputTerminal() );
	InitRadio();
	
	AfxGetApp()->BeginWaitCursor();
	ResetSeletion();
    AfxGetApp()->EndWaitCursor();
	

	if( bOnlyPax )
	{
		GetDlgItem(IDC_RADIO_NONPAX)->EnableWindow( FALSE );
		GetDlgItem(IDC_RADIO_ALLTYPE)->EnableWindow( FALSE );
	}
	CRect rectWnd;
	m_listPassenger.GetWindowRect(rectWnd);
	ScreenToClient(rectWnd);
	int nLeft=rectWnd.left;
	int nHeight=26;
	int nTop=rectWnd.top-nHeight;
	int nWidth=rectWnd.Width();
	m_wndToolBar.SetWindowPos(NULL,nLeft,nTop,nWidth,nHeight,SWP_NOACTIVATE);
	m_wndToolBar.GetToolBarCtrl().HideButton(ID_TOOLBARBUTTONEDIT,TRUE);
	EnableToolbarButtonByRules();

	DWORD dwStyle = m_listPassenger.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_listPassenger.SetExtendedStyle( dwStyle );

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT |LVCF_FMT;
	
	CStringList listStr;
	// add all non pax type
	for( int i=1; i<=NOPAX_COUNT; i++ )
	{
		listStr.AddTail( GetInputTerminal()->m_pMobElemTypeStrDB->GetString(i) );
	}

	lvc.csList=&listStr;
//	lvc.cx=rectWnd.Width()/3;
	int widthSlot = (rectWnd.Width() - 10)/4;
	
	lvc.fmt=LVCFMT_DROPDOWN;
	lvc.pszText="Non Pax Type";
	lvc.cx=widthSlot*2;
	m_listPassenger.InsertColumn(0,&lvc);	

	listStr.RemoveAll();
	listStr.AddTail("<");
	listStr.AddTail("<=");
	listStr.AddTail("==");
	listStr.AddTail(">=");
	listStr.AddTail(">");
	listStr.AddTail("!=");
	lvc.pszText="Operator";
	lvc.cx=widthSlot;
	m_listPassenger.InsertColumn(1,&lvc);
	
	lvc.fmt=LVCFMT_NUMBER;
	lvc.cx-=5;
	lvc.pszText="Value";
	lvc.cx=widthSlot;
	m_listPassenger.InsertColumn(2,&lvc);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPassengerTypeDialog::DeleteAllChildren(HTREEITEM hItem)
{
	HTREEITEM hChild = m_PaxTypeTree.GetChildItem(hItem);
	while(hChild)
	{
		HTREEITEM hNext = m_PaxTypeTree.GetNextSiblingItem(hChild);
		std::map<HTREEITEM,int>::iterator iter = m_mapLevelItem.find(hChild);
		if (iter != m_mapLevelItem.end())
		{
			m_mapLevelItem.erase(iter);
		}
		m_PaxTypeTree.DeleteItem(hChild);
		hChild = hNext;
	} 
}

void CPassengerTypeDialog::OnTvnItemexpanding( NMHDR *pNMHDR, LRESULT *pResult )
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	*pResult = 0;

	std::map<HTREEITEM,int>::iterator iter = m_mapLevelItem.find(hItem);
	if (iter == m_mapLevelItem.end())
		return;
	int nIdx = (*iter).second;
	int nBranch = 0;
	nIdx++;
	if (nIdx == LB_ERR || nIdx >= LEVELS)
		return;
	if(pNMTreeView->action == TVE_EXPAND)
	{
		DeleteAllChildren(hItem);
		// add the real children (only if they themselves have children
		InsertTreeItem(hItem,nIdx);
	}
	else 
	{
		DeleteAllChildren(hItem);
		nBranch = GetInputTerminal()->inTypeList->getBranchesAtLevel( nIdx );
		// add a dummy child if the node has children
		if(nBranch != 0)
			m_PaxTypeTree.InsertItem("error",hItem);
	}
}

void CPassengerTypeDialog::InsertTreeItem( HTREEITEM _hItem, int _nLevel )
{
	if( _hItem == NULL || _nLevel < 0 || _nLevel >= LEVELS )
		return;
	
	PassengerTypeList* pTypeList = GetInputTerminal()->inTypeList;
	int nCount = pTypeList->getBranchesAtLevel( _nLevel );
	for( int i=0; i<nCount; i++ )
	{
		CString strTemp = GetInputTerminal()->inTypeList->getTypeName( i, _nLevel );
		strTemp.TrimLeft(); strTemp.TrimRight();
		if (strTemp.IsEmpty())
			continue;
		
		HTREEITEM hItem = m_PaxTypeTree.InsertItem( strTemp, _hItem );
		m_PaxTypeTree.SetItemData( hItem, i );
		m_mapLevelItem.insert(std::make_pair(hItem,_nLevel));
		if (_nLevel >= LEVELS - 1)
			continue;

		int nChildCount = GetInputTerminal()->inTypeList->getBranchesAtLevel( _nLevel + 1 );
		// add a dummy child if the node has children
		if(nChildCount != 0)
			m_PaxTypeTree.InsertItem("error",hItem);
	}
	return;
}


void CPassengerTypeDialog::Setup(const CMobileElemConstraint &MobileSelection)
{
	m_MobileElem   = MobileSelection;

	// Set to default mode (not 'A' or 'D').
	m_MobileElem.SetFltConstraintMode( ENUM_FLTCNSTR_MODE_ALL );
	
	// Reset all the selection.
	ResetSeletion();
}

void CPassengerTypeDialog::ResetSeletion()
{
	if (!::IsWindow(GetSafeHwnd()))
		return;

	ResetPaxType();

	int i = 0, j = 0;

	//
	// Passenger Type Tree
	//

    char *type_labels[] = { "ALL TYPES", "ARRIVAL", "DEPARTURE",
		"TRANSIT", "TRANSFER" };

	for (i = 0; i < 5; i++)
	{
		HTREEITEM hItem = m_PaxTypeTree.InsertItem( type_labels[i] );
		m_PaxTypeTree.SetItemData( hItem, i );

		InsertTreeItem( hItem, 0 );    // Insert all the branches of every level.

		m_PaxTypeTree.Expand( hItem, TVE_EXPAND );   // Expand it.
	}


	int iIntrinsic = m_MobileElem.getIntrinsic();
	int iTypeSel = 0;
	for (i = 0; i < LEVELS; i++)
	{
		iTypeSel = m_MobileElem.getUserType (i);
	}

	// 
	// Radio controls
	//
	
	FlightConstraint sel;
	sel = m_MobileElem;	

	CButton* pBtn = NULL;
	if ( sel.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ALL )
	{
		pBtn = (CButton *) GetDlgItem(IDC_RADIO_ALL);
		if (pBtn != NULL)
			pBtn->SetCheck(1);
	}
    else if (sel.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ARR )
	{
		pBtn = (CButton *) GetDlgItem(IDC_RADIO_ARRIVING);
		if (pBtn != NULL)
			pBtn->SetCheck(1);
	}
    else if (sel.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_DEP )
	{
		pBtn = (CButton *) GetDlgItem(IDC_RADIO_DEPARTING);
		if (pBtn != NULL)
			pBtn->SetCheck(1);
	}
	
	//
	// Flight stuff
	//

	StringList strDict, strDict2;

	
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

		TVITEM ti;
		ti.mask       = TCIF_TEXT;		
		ti.cChildren  = 1;
		ti.pszText    = (LPSTR) strTemp.GetBuffer(0);
		ti.cchTextMax = 256;

		TVINSERTSTRUCT tis;
		tis.hParent   = TVI_ROOT;
		tis.hInsertAfter = TVI_SORT;
		tis.item      = ti;
		
		HTREEITEM hItem = m_AirlineTree.InsertItem(&tis);
		if (hItem == NULL)
			continue;

		sel.setAirline(strTemp);
		GetInputTerminal()->flightSchedule->getFlightList(strDict2, sel);
		if( m_bShowFlightID )			
		{
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
	pvACTList = _g_GetActiveACMan( GetInputTerminal()->m_pAirportDB)->GetACTypeList();
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
	// add by bird, to reload nonpax count
	ReloadNonpaxCount();
}

void CPassengerTypeDialog::OnSelchangeListActypePxtd() 
{
	int nIndex = m_ACType.GetCurSel();
	if (nIndex < 0)
	{
		m_MobileElem.setACType( "" );
		ResetPaxType();
	}
	else
	{
		CString strSel;
		m_ACType.GetText(nIndex, strSel);		
		m_MobileElem.setACType(strSel);		
		ResetPaxType();
	}		
}

void CPassengerTypeDialog::OnSelchangeListAirportPxtd() 
{
	int nIndex = m_Airport.GetCurSel();
	if (nIndex < 0)
	{
		m_MobileElem.setAirport( "" );
		ResetPaxType();		
	}
	else
	{
		CString strSel;
		m_Airport.GetText(nIndex, strSel);

		m_MobileElem.setAirport(strSel);		
		ResetPaxType();
	}
}

void CPassengerTypeDialog::OnSelchangeListAirlineGroup() 
{
	CSUBAIRLINELIST* pvSList;
	pvSList = _g_GetActiveAirlineMan( GetInputTerminal()->m_pAirportDB )->GetSubairlineList();
	int nIndex = m_listAirlineGroup.GetCurSel();
	if (nIndex < 0)
	{		
		m_MobileElem.setAirlineGroup( "" );
		m_MobileElem.setFlightGroup("");
		ResetPaxType();
	}
	else
	{
		
		CString strSel;
		m_listAirlineGroup.GetText(nIndex, strSel);

		if ((int)pvSList->size() > nIndex)
		{
			m_MobileElem.setAirlineGroup(strSel);
		}
		else
		{
			m_MobileElem.setFlightGroup(strSel);
		}
		ResetPaxType();
	}	
}


void CPassengerTypeDialog::OnSelchangeListCategoryPxtd() 
{
	int nIndex = m_Category.GetCurSel();
	if (nIndex < 0)
	{
		m_MobileElem.setCategory( "" );
		ResetPaxType();		
	}
	else
	{
		CString strSel;
		m_Category.GetText(nIndex, strSel);

		m_MobileElem.setCategory(strSel);
		
		ResetPaxType();	
	}	
}

void CPassengerTypeDialog::OnSelchangeListSectorPxtd() 
{
	int nIndex = m_Sector.GetCurSel();
	if (nIndex < 0)
	{
		m_MobileElem.setSector( "" );
		ResetPaxType();
	}
	else
	{
		CString strSel;
		m_Sector.GetText(nIndex, strSel);

		m_MobileElem.setSector(strSel);
		
		ResetPaxType();
	}
}



void CPassengerTypeDialog::OnSelchangedTreeAirline(NMHDR* pNMHDR, LRESULT* pResult) 
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

	m_MobileElem.setAirline(strAlirline);

	CFlightID fltID;
	fltID = strFlightID;
	if (!strFlightID.IsEmpty())
	{
		if (fltID == m_MobileElem.getFlightID())
		{
			fltID = "";
		}	
	}

	m_MobileElem.setFlightID( fltID );
	ResetPaxType();
	*pResult = 0;
}

void CPassengerTypeDialog::OnSelchangedTreePassengertype(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	// get level, 
	HTREEITEM hSelItem = pNMTreeView->itemNew.hItem;
	
	if( !m_bAllBranchesInType)
	{
		SelchangedTreePassengertype(hSelItem);
		//contrl flight radio button
		ControlFlightRadio();
		/////////////////////////////////
		// show/or hide the other flight button
		m_butOtherFlight.ShowWindow( m_MobileElem.getIntrinsic()==4 ? SW_SHOW : SW_HIDE );
		ResetPaxType();
		*pResult = 0;
	}
	else
	{
		int nLevel = -1;
	    HTREEITEM hParentItem = m_PaxTypeTree.GetParentItem( hSelItem );
	    while( hParentItem )
		{
			nLevel++;
			hParentItem = m_PaxTypeTree.GetParentItem( hParentItem );
		}
		
		int nUserType[MAX_PAX_TYPES];
		for( int i=0; i<MAX_PAX_TYPES; i++ )
			nUserType[i] = 0;
		
		HTREEITEM hItem = hSelItem;
		while( hItem )
		{
			// get branch
			int nBranchIdx = m_PaxTypeTree.GetItemData( hItem );
			if( nLevel == -1 )
				m_MobileElem.setIntrinsicType( nBranchIdx );
			
			nLevel--;
			hItem = m_PaxTypeTree.GetParentItem( hItem );
		}


		for(unsigned int i = 0;i< m_dMultiSel.m_Sel.size();i++)
		{   
			int j= m_dMultiSel.m_Sel[i];
		    int SelLevel = m_dMultiSel.m_TypeStrings[j].nLevel;
		    nUserType[SelLevel]=GetInputTerminal()->inTypeList->getTypeIndex( m_dMultiSel.m_TypeStrings[j].nIdx, SelLevel );

		}

		m_MobileElem.setUserType( nUserType );   
			

		//contrl flight radio button
		ControlFlightRadio();
		/////////////////////////////////
		// show/or hide the other flight button
		m_butOtherFlight.ShowWindow( m_MobileElem.getIntrinsic()==4 ? SW_SHOW : SW_HIDE );
		ResetPaxType();
		*pResult = 0;

	}
}

void CPassengerTypeDialog::OnRadioAll() 
{
	if (m_MobileElem.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ALL )
		return;

	CButton * pAll = (CButton *) GetDlgItem(IDC_RADIO_ALL);
	if (pAll == NULL)
		return;
	
	pAll->SetCheck(1);
	m_MobileElem.SetFltConstraintMode( ENUM_FLTCNSTR_MODE_ALL );
	
	ResetPaxType();
}

void CPassengerTypeDialog::OnRadioArriving() 
{
	if (m_MobileElem.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_ARR )
		return;

	CButton * pArriving = (CButton *) GetDlgItem(IDC_RADIO_ARRIVING);
	if (pArriving == NULL)
		return;
	
	pArriving->SetCheck(1);
	m_MobileElem.SetFltConstraintMode( ENUM_FLTCNSTR_MODE_ARR );

	ResetPaxType();
}

void CPassengerTypeDialog::OnRadioDeparting() 
{
	if (m_MobileElem.GetFltConstraintMode() == ENUM_FLTCNSTR_MODE_DEP )
		return;

	CButton * pDeparting = (CButton *) GetDlgItem(IDC_RADIO_DEPARTING);
	if (pDeparting == NULL)
		return;
	
	pDeparting->SetCheck(1);
	m_MobileElem.SetFltConstraintMode( ENUM_FLTCNSTR_MODE_DEP );

	ResetPaxType();
}



InputTerminal* CPassengerTypeDialog::GetInputTerminal()
{	
	if (!m_bNewVersion)
	{
		CTermPlanDoc* pTermPlanDoc = (CTermPlanDoc*)(((CMainFrame*)AfxGetApp()->GetMainWnd())->GetActiveFrame()->GetActiveDocument());
		ASSERT_KINDOF(CTermPlanDoc, pTermPlanDoc);
		return (InputTerminal*)&pTermPlanDoc->GetTerminal();
	}
	else
	{
		return m_pTerminal;
	}
}



void CPassengerTypeDialog::OnCcancel() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

LRESULT CPassengerTypeDialog::OnUnselectableListBoxSelchange( WPARAM wParam, LPARAM lParam )
{		
	OnSelchangeListActypePxtd();
	OnSelchangeListAirportPxtd();
	OnSelchangeListCategoryPxtd();
	OnSelchangeListSectorPxtd();
	OnSelchangeListAirlineGroup();
	return TRUE;
}	

LRESULT CPassengerTypeDialog::OnUnselectableTreeSelchange( WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}
void CPassengerTypeDialog::OnButEditNonpax() 
{
	// TODO: Add your control notification handler code here
	CMoblieElemDlg dlg(m_pParentWnd);

	dlg.DoModal();

	LoadCombo();
}

void CPassengerTypeDialog::LoadCombo()
{
	m_comboMobileElem.ResetContent();
	int nCount	= GetInputTerminal()->m_pMobElemTypeStrDB->GetCount();
	
	for(int i=1; i<nCount; i++)
	{
		m_comboMobileElem.InsertString(-1,GetInputTerminal()->m_pMobElemTypeStrDB->GetString( i ));
	}

	for (int i = 0; i < GetInputTerminal()->GetTrayHostList()->GetItemCount(); i++)
	{
		TrayHost* pTrayHost = GetInputTerminal()->GetTrayHostList()->GetItem(i);
		m_comboMobileElem.InsertString(-1,pTrayHost->GetHostName());
	}

	m_comboMobileElem.SetCurSel( 0 );

	CString strTmp;
	m_comboMobileElem.GetWindowText( strTmp );
	if (GetInputTerminal()->m_pMobElemTypeStrDB->GetIndex( strTmp ) == -1)
	{
		m_MobileElem.SetTypeIndex( GetInputTerminal()->GetTrayHostList()->FindItem( strTmp ) + ALLPAX_COUNT);
	}
	else
	{
		m_MobileElem.SetTypeIndex( GetInputTerminal()->m_pMobElemTypeStrDB->GetIndex( strTmp ) );
	}
}

void CPassengerTypeDialog::InitRadio()
{

	m_nConstraintType = 0;

	LoadCombo();

	if(m_bDisableDefault)
	{
		CButton* pButPax = (CButton*)GetDlgItem( IDC_RADIO_PAX );
		pButPax->SetCheck(1);

		CButton* pBut = (CButton*)GetDlgItem( IDC_RADIO_ALLTYPE );
		pBut->EnableWindow(FALSE);	
		
		m_MobileElem.SetMobileElementType(enum_MobileElementType_PAX);
		m_MobileElem.SetTypeIndex( 0 );
		
		m_nConstraintType = 0;
	}
	else if(bOnlyPax)//mobile element dialog
	{
		CButton* pButPax = (CButton*)GetDlgItem( IDC_RADIO_PAX );
		pButPax->SetCheck(1);
		m_MobileElem.SetMobileElementType(enum_MobileElementType_PAX);
		m_MobileElem.SetTypeIndex( 0 );
		m_nConstraintType = 0;
	}
	else
	{	
		CButton* pButAll = (CButton*)GetDlgItem( IDC_RADIO_ALLTYPE );
		pButAll->SetCheck(1);

		m_nConstraintType = -1;

		m_MobileElem.SetMobileElementType(enum_MobileElementType_ALL);
		m_MobileElem.SetTypeIndex( 0 );
	}



	//m_MobileElem.SetMobileElementType(enum_MobileElementType_PAX);
	//m_MobileElem.SetTypeIndex( 0 );
	//pBut = (CButton*)GetDlgItem( IDC_RADIO_NONPAX );
	//pBut->EnableWindow( FALSE );
	m_comboMobileElem.EnableWindow( FALSE );
	GetDlgItem( IDC_BUT_EDIT_NONPAX )->EnableWindow( FALSE );
}

void CPassengerTypeDialog::OnRadioPax() 
{
	m_listPassenger.EnableWindow(TRUE);
	m_wndToolBar.EnableWindow(TRUE);
	// TODO: Add your control notification handler code here
	if( m_nConstraintType == 0)
		return;

	CButton* pBut = (CButton*)GetDlgItem( IDC_RADIO_PAX );
	pBut->SetCheck( 1 );
	m_nConstraintType = 0;
	
	m_MobileElem.SetMobileElementType(enum_MobileElementType_PAX);
	m_MobileElem.SetTypeIndex( 0 );	//TypeIndex=0

	m_comboMobileElem.EnableWindow( FALSE );
	GetDlgItem( IDC_BUT_EDIT_NONPAX )->EnableWindow( FALSE );
	ResetPaxType();
}

void CPassengerTypeDialog::OnRadioNonpax() 
{
	// TODO: Add your control notification handler code here
	m_listPassenger.EnableWindow(FALSE);
	m_wndToolBar.EnableWindow(FALSE);

	//only when paxtype is Passenger, the NonPaxCount is validate
	m_MobileElem.getNonpaxCount().init();
	ReloadNonpaxCount();
	
	if( m_nConstraintType == 1)
		return;

	CButton* pBut = (CButton*)GetDlgItem( IDC_RADIO_NONPAX );
	pBut->SetCheck( 1 );
	m_nConstraintType = 1;

	m_MobileElem.SetMobileElementType(enum_MobileElementType_NONPAX);

	CString strTmp;
	m_comboMobileElem.GetWindowText( strTmp );
	if (GetInputTerminal()->m_pMobElemTypeStrDB->GetIndex( strTmp ) == -1)
	{
		m_MobileElem.SetTypeIndex(GetInputTerminal()->GetTrayHostList()->FindItem(strTmp) + ALLPAX_COUNT);
	}
	else
	{
		m_MobileElem.SetTypeIndex( GetInputTerminal()->m_pMobElemTypeStrDB->GetIndex( strTmp ) );
	}
	
	m_comboMobileElem.EnableWindow( TRUE );
	GetDlgItem( IDC_BUT_EDIT_NONPAX )->EnableWindow( TRUE );

	ResetPaxType();
	
}

void CPassengerTypeDialog::OnSelchangeComboNopax() 
{
	// TODO: Add your control notification handler code here
	CString strTmp;
	m_comboMobileElem.GetWindowText( strTmp );
	if (GetInputTerminal()->m_pMobElemTypeStrDB->GetIndex( strTmp ) == -1)
	{
		m_MobileElem.SetTypeIndex(GetInputTerminal()->GetTrayHostList()->FindItem(strTmp) + ALLPAX_COUNT);
	}
	else
	{
		m_MobileElem.SetTypeIndex( GetInputTerminal()->m_pMobElemTypeStrDB->GetIndex( strTmp ) );
	}

	ResetPaxType();

}


void CPassengerTypeDialog::ResetPaxType()
{
	//Show By Edit
	//TCHAR szPax[SELECTED_TEXT_LEN];
    CString szPax;
	m_MobileElem.screenPrint(szPax,0,SELECTED_TEXT_LEN);
	
	CEdit* pEdit = (CEdit *) GetDlgItem(IDC_EDIT_PASSENGERTYPE);
	if (pEdit != NULL)
	{
		pEdit->SetWindowText(szPax.GetBuffer(0));
	}
}

//change Flight Radio state,when pax type changed
//no exception
void CPassengerTypeDialog::ControlFlightRadio()
{
	
	CButton* pButAll	= (CButton*)GetDlgItem(IDC_RADIO_ALL);
	CButton* pButArr	= (CButton*)GetDlgItem(IDC_RADIO_ARRIVING);
	CButton* pButDep	= (CButton*)GetDlgItem(IDC_RADIO_DEPARTING);
	
	switch(m_MobileElem.getIntrinsic())
	{
	case 0:	//default: all radio enable
		pButAll->EnableWindow(TRUE);
		pButArr->EnableWindow(TRUE);
		pButDep->EnableWindow(TRUE);
		break;
	
	case 1: //arriver: only arrive radio enble,and checked
		pButAll->EnableWindow(FALSE);
		pButArr->EnableWindow(TRUE);
		pButDep->EnableWindow(FALSE);

		pButAll->SetCheck(0);
		pButArr->SetCheck(1);
		pButDep->SetCheck(0);
		m_MobileElem.SetFltConstraintMode( ENUM_FLTCNSTR_MODE_ARR );
		break;

	case 2: //dep	:  only dep radio enble,and checked
	
		pButAll->EnableWindow(FALSE);
		pButArr->EnableWindow(FALSE);
		pButDep->EnableWindow(TRUE);

		pButAll->SetCheck(0);
		pButArr->SetCheck(0);
		pButDep->SetCheck(1);
		m_MobileElem.SetFltConstraintMode( ENUM_FLTCNSTR_MODE_DEP );
		break;

	case 3: //transit: all radio enable
	case 4: //translate: all radio enable
	default://: all radio enable
		pButAll->EnableWindow(TRUE);
		pButArr->EnableWindow(TRUE);
		pButDep->EnableWindow(TRUE);

		
		pButAll->SetCheck(1);
		pButArr->SetCheck(0);
		pButDep->SetCheck(0);
		m_MobileElem.SetFltConstraintMode( ENUM_FLTCNSTR_MODE_ALL );
		break;
	}
	return;
}

void CPassengerTypeDialog::OnButtonOtherflight() 
{
	// TODO: Add your control notification handler code here
	CFlightDialog dlg( m_pParentWnd );
	dlg.Setup( m_MobileElem.getOtherFlightConstraint(),m_MobileElem.getIntrinsic() );
	if( dlg.DoModal() )
	{
		FlightConstraint otherFlightCon = dlg.GetFlightSelection();
		m_MobileElem.setOtherFlightConstraint( otherFlightCon );
	}
	
	ResetPaxType();
}

int CPassengerTypeDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}		
	return 0;
}

void CPassengerTypeDialog::OnRadioAlltype() 
{
	if(m_nConstraintType == -1)
		return;

	m_nConstraintType = -1;
	m_listPassenger.EnableWindow(FALSE);
	m_wndToolBar.EnableWindow(FALSE);

	m_MobileElem.SetMobileElementType(enum_MobileElementType_ALL);
	//only when paxtype is Passenger, the NonPaxCount is validate
	m_MobileElem.getNonpaxCount().init();
	ReloadNonpaxCount();

	ResetPaxType();
}

//////////////////////////////////////////////////////////////////////////
void CPassengerTypeDialog::OnAdd()
{
	m_bIsNewItem = true;
	m_listPassenger.EditNew();
	int nLastItem=m_listPassenger.GetItemCount()-1;
	m_listPassenger.SetItemText(nLastItem,1,">=");
	m_listPassenger.SetItemText(nLastItem,2,"0");
	EnableToolbarButtonByRules();
}

void CPassengerTypeDialog::OnDelete()
{
	int iIndex = m_listPassenger.GetCurSel();
	if( iIndex == -1 )
		return;

	int iItemData = m_listPassenger.GetItemData( iIndex );
	m_MobileElem.getNonpaxCount().removeItem( iItemData );
	ReloadNonpaxCount();
	ResetPaxType();
	EnableToolbarButtonByRules();
}

void CPassengerTypeDialog::OnItemchangedListPassenger(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int selectedNum=m_listPassenger.GetSelectedCount();
	EnableToolbarButtonByRules();
	*pResult = 0;
}



void CPassengerTypeDialog::OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pLVDispInfo = (LV_DISPINFO*)pNMHDR;
	int nCol=pLVDispInfo->item.iSubItem;
	int nRow=m_listPassenger.GetCurSel();
	CString strText=pLVDispInfo->item.pszText;

	//////////////////////////////////////////////////////////////////////////
	if( m_bIsNewItem )	// is new add item
	{
		assert( nCol == 0);
		AddNopaxCountItem(strText);		
	}
	else
	{
		ModifyNopaxCountItem( nRow, nCol, strText );
	}
	m_bIsNewItem = false;
	*pResult=0;
	
	ResetPaxType();

	EnableToolbarButtonByRules();
}

//////////////////////////////////////////////////////////////////////////
int CPassengerTypeDialog::ReloadNonpaxCount()
{
	// clear all content
	m_listPassenger.DeleteAllItems();

	CString strSubItem;
	for( int i=0; i< m_MobileElem.getNonpaxCount().getCount(); i++ )
	{
		NONPAXCOUNTITEM item = m_MobileElem.getNonpaxCount().getItem(i);
		strSubItem = GetInputTerminal()->m_pMobElemTypeStrDB->GetString( item.iNonPaxType );
		int iIndex = m_listPassenger.InsertItem( m_listPassenger.GetItemCount(),strSubItem );
		strSubItem = CNonPaxCount::getStringByOperator( item.iOperator );
		m_listPassenger.SetItemText( iIndex, 1, strSubItem );
		strSubItem.Format("%d", item.iValue );
		m_listPassenger.SetItemText( iIndex, 2, strSubItem );

		// set item data
		m_listPassenger.SetItemData( iIndex, i);
	}

	return m_MobileElem.getNonpaxCount().getCount();
}

void CPassengerTypeDialog::AddNopaxCountItem(const CString &_strNonPax)
{
	NONPAXCOUNTITEM item;
	item.iNonPaxType = GetInputTerminal()->m_pMobElemTypeStrDB->GetIndex( _strNonPax );
	assert( item.iNonPaxType!= -1 );
	item.iOperator = more_equal;
	item.iValue = 0;
	
	m_MobileElem.getNonpaxCount().addItem(item);

	ReloadNonpaxCount();
}

void CPassengerTypeDialog::ModifyNopaxCountItem(int _row, int _col, const CString &_strItem)
{
	int iItemData = m_listPassenger.GetItemData( _row );
	NONPAXCOUNTITEM item = m_MobileElem.getNonpaxCount().getItem( iItemData );

	switch( _col )
	{
	case 0:
		item.iNonPaxType = GetInputTerminal()->m_pMobElemTypeStrDB->GetIndex( _strItem );
		assert( item.iNonPaxType!= -1 );
		break;

	case 1:
		item.iOperator = CNonPaxCount::getOperatorByString( _strItem );
		assert( item.iOperator != other_operator );
		break;

	case 2:
		item.iValue = atoi(_strItem);
		//assert( item.iValue>=0);
		break;

	default:
		assert(0);
	}
	m_MobileElem.getNonpaxCount().modifyItem( iItemData, item );
	
	ReloadNonpaxCount();
}



void CPassengerTypeDialog::OnDblclkListPassenger(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	EnableToolbarButtonByRules();
	*pResult = 0;
}


void CPassengerTypeDialog::OnCheckAlltype() 
{

	HTREEITEM hSelItem = m_PaxTypeTree.GetSelectedItem();
	//if (hSelItem == NULL) return;
	if (hSelItem == NULL)
	{
		m_MobileElem.setIntrinsicType( 0);
		int nUserType[MAX_PAX_TYPES];
		for( int i=0; i<MAX_PAX_TYPES; i++ )
			nUserType[i] = 0;	
		m_MobileElem.setUserType( nUserType );

	}

	SelchangedTreePassengertype(hSelItem);	
	/*// show/or hide the other flight button
	m_butOtherFlight.ShowWindow( m_MobileElem.getIntrinsic()==4 ? SW_SHOW : SW_HIDE );	
	ResetPaxType();*/
	UpdateData(TRUE);
	if(m_bAllBranchesInType)
	{
		m_dMultiSel.SetPointers(GetInputTerminal()->inTypeList,&m_MobileElem);	
		if(m_dMultiSel.DoModal() == IDOK)
		{
			int nUserType[MAX_PAX_TYPES];
			for( int i=0; i<MAX_PAX_TYPES; i++ )
				nUserType[i] = 0;	

			for(unsigned int i = 0;i< m_dMultiSel.m_Sel.size();i++)
			{  
				int j= m_dMultiSel.m_Sel[i];
				int SelLevel = m_dMultiSel.m_TypeStrings[j].nLevel;
				nUserType[SelLevel]=GetInputTerminal()->inTypeList->getTypeIndex( m_dMultiSel.m_TypeStrings[j].nIdx, SelLevel );
			}
			
         m_MobileElem.setUserType( nUserType );   
		}
   
		m_butOtherFlight.ShowWindow( m_MobileElem.getIntrinsic()==4 ? SW_SHOW : SW_HIDE );	
	    ResetPaxType();

	}
	else
	{
		m_butOtherFlight.ShowWindow( m_MobileElem.getIntrinsic()==4 ? SW_SHOW : SW_HIDE );	
	    ResetPaxType();

	}

}

void CPassengerTypeDialog::SelchangedTreePassengertype(HTREEITEM& hSelItem)
{
	int nLevel = -1;
	HTREEITEM hParentItem = m_PaxTypeTree.GetParentItem( hSelItem );
	while( hParentItem )
	{
		nLevel++;
		hParentItem = m_PaxTypeTree.GetParentItem( hParentItem );
	}
	
	int nUserType[MAX_PAX_TYPES];
	for( int i=0; i<MAX_PAX_TYPES; i++ )
		nUserType[i] = 0;	
	
	UpdateData(TRUE) ;	
	
	//only select child node when contrl key press down.
	/*int nTest=::GetKeyState(VK_CONTROL) & 0xFF00;
	if (nTest) 
	{
		m_CheckAllType.SetCheck( TRUE ) ;
	}
	
	if(m_bAllBranchesInType)
	{
		if(nLevel>=0)
		{
			for(int n=0;n<nLevel;n++)    nUserType[n]=0;
			
			nUserType[nLevel]=GetInputTerminal()->inTypeList->getTypeIndex( m_PaxTypeTree.GetItemData( hSelItem ), nLevel );
		}
		
		HTREEITEM hItem = hSelItem;
		int nBranch= m_PaxTypeTree.GetItemData( hItem );
		while(hItem)
		{
			nBranch=m_PaxTypeTree.GetItemData( hItem );
			hItem=m_PaxTypeTree.GetParentItem(hItem);
		}
		m_MobileElem.setIntrinsicType( nBranch );
	}
	else*/
	//{
		HTREEITEM hItem = hSelItem;
		while( hItem )
		{
			// get branch
			int nBranchIdx = m_PaxTypeTree.GetItemData( hItem );
			if( nLevel == -1 )
				m_MobileElem.setIntrinsicType( nBranchIdx );
			else
			{
				int nStrIdx = GetInputTerminal()->inTypeList->getTypeIndex( nBranchIdx, nLevel );
				nUserType[nLevel] = nStrIdx;
			}
			nLevel--;
			hItem = m_PaxTypeTree.GetParentItem( hItem );
		}
	//}
	m_MobileElem.setUserType( nUserType );
}

void CPassengerTypeDialog::OnClickStopOverAirport()
{
	CStopOverAirportDialog dlg(GetInputTerminal(), this);
	if (IDOK == dlg.DoModal())
	{
		m_MobileElem.setStopoverAirport(dlg.GetSelectedAirport());
		m_MobileElem.setStopoverSector(dlg.GetSelectedSector());
		ResetPaxType();
	}
}
void CPassengerTypeDialog::EnableToolbarButtonByRules()
{
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD, TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL, 
		0 != m_MobileElem.getNonpaxCount().getCount()
		&& -1 != m_listPassenger.GetCurSel()
		);
}
