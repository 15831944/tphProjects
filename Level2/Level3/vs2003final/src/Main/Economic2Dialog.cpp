// Economic2Dialog.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "Economic2Dialog.h"

#include "passengertypedialog.h"
#include "flightdialog.h"
#include "termplandoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ITEMDATA_STARTIDX 100

/////////////////////////////////////////////////////////////////////////////
// CEconomic2Dialog dialog


CEconomic2Dialog::CEconomic2Dialog(enum ECONOMIC2TYPE _enumType, CWnd* pParent /*=NULL*/)
	: CDialog(CEconomic2Dialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEconomic2Dialog)
	//}}AFX_DATA_INIT

	m_enumType = _enumType;
	m_arColName.RemoveAll();
	m_strTitle.Empty();

	m_nListPreIndex = -1;
}


void CEconomic2Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEconomic2Dialog)
	DDX_Control(pDX, IDC_STATIC_ECONOMIC2TOOLBAR, m_listtoolbarcontenter);
	DDX_Control(pDX, IDC_LIST_ECONOMIC2, m_List);
	DDX_Control(pDX, IDC_BTN_ECONOMIC2_SAVE, m_btnSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEconomic2Dialog, CDialog)
	//{{AFX_MSG_MAP(CEconomic2Dialog)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_ECONOMIC2, OnEndlabeleditListEconomic2)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ECONOMIC2, OnItemchangedListEconomic2)
	ON_BN_CLICKED(IDC_BTN_ECONOMIC2_SAVE, OnBtnEconomic2Save)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPeoplemoverNew)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEconomic2Dialog message handlers

int CEconomic2Dialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_ListToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ListToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	

	// Init list ctrl
	InitListCtrl();
	
	return 0;
}

BOOL CEconomic2Dialog::OnInitDialog() 
{
	m_toolTips.InitTooltips( &m_List );
	
	CDialog::OnInitDialog();
	
	AfxGetApp()->BeginWaitCursor();

	SetWindowText(m_strTitle);

	// Init tool bar
	InitToolbar();

	// Init list ctrl
	InitListCtrl();
	
	ReloadDatabase();

	m_btnSave.EnableWindow( FALSE );
	
	AfxGetApp()->EndWaitCursor();	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEconomic2Dialog::ReloadDatabase()
{
	m_List.DeleteAllItems();

	CEconomicManager* pEconMan = GetEconomicManager();
	
	switch( m_enumType )
	{
	case PASSENGER:
		{
			
			std::vector<PassengerCostDataItem>& vData = pEconMan->m_pPassengerCost->GetDataList();
			int nCount = vData.size();
			for( int i=0; i<nCount; i++ )
			{
				//char szBuf[256];
				CString szBuf;
				vData[i].m_pPaxType->screenPrint( szBuf );
				int nItem = m_List.InsertItem( i, szBuf.GetBuffer(0) );
				CString csVal;
				csVal.Format( "%.2f", vData[i].m_dFixedPerPax );
				m_List.SetItemText( nItem, 1, csVal );
				csVal.Format( "%.2f", vData[i].m_dVarPerHour );
				m_List.SetItemText( nItem, 2, csVal );				
				csVal.Format( "%.2f", vData[i].m_dVarPerProc );
				m_List.SetItemText( nItem, 3, csVal );
				csVal.Format( "%.2f", vData[i].m_dVarPerCart );
				m_List.SetItemText( nItem, 4, csVal );
				csVal.Format( "%.2f", vData[i].m_dVarPerBag );
				m_List.SetItemText( nItem, 5, csVal );

				m_List.SetItemData( nItem, i + ITEMDATA_STARTIDX );
			}


		}
		break;
	case AIRCRAFT:
		{
			std::vector<AircraftCostDataItem>& vData = pEconMan->m_pAircraftCost->GetDataList();
			int nCount = vData.size();
			for( int i=0; i<nCount; i++ )
			{
				//char szBuf[256];
				CString szBuf;
				vData[i].m_pFltType->screenPrint( szBuf );
				int nItem = m_List.InsertItem( i, szBuf.GetBuffer(0) );
				CString csVal;
				csVal.Format( "%.2f", vData[i].m_dServicePerAC );
				m_List.SetItemText( nItem, 1, csVal );
				csVal.Format( "%.2f", vData[i].m_dServicePerHour );
				m_List.SetItemText( nItem, 2, csVal );

				m_List.SetItemData( nItem, i + ITEMDATA_STARTIDX );
			}
		}

		break;
	case PASSENGER_AIRPORT_FEES:
		{
			std::vector<PaxAirportFeeDataItem>& vData = pEconMan->m_pPaxAirportFeeRevenue->GetDataList();
			int nCount = vData.size();
			for( int i=0; i<nCount; i++ )
			{
				CString szBuf;
				vData[i].m_pPaxType->screenPrint( szBuf );
				int nItem = m_List.InsertItem( i, szBuf );
				CString csVal;
				csVal.Format( "%.2f", vData[i].m_dFeePerPax );
				m_List.SetItemText( nItem, 1, csVal );
				csVal.Format( "%.2f", vData[i].m_dFeePerHour );
				m_List.SetItemText( nItem, 2, csVal );

				m_List.SetItemData( nItem, i + ITEMDATA_STARTIDX );
			}
		}
		break;
	case LANDING_FEES:
		{
			std::vector<LandingFeeRevDataItem>& vData = pEconMan->m_pLandingFeesRevenue->GetDataList();
			int nCount = vData.size();
			for( int i=0; i<nCount; i++ )
			{
					CString szBuf;
				vData[i].m_pFltType->screenPrint( szBuf );
				int nItem = m_List.InsertItem( i, szBuf );
				CString csVal;
				csVal.Format( "%.2f", vData[i].m_dFeePerLanding );
				m_List.SetItemText( nItem, 1, csVal );
				csVal.Format( "%.2f", vData[i].m_dFeePer1000MLW );
				m_List.SetItemText( nItem, 2, csVal );

				m_List.SetItemData( nItem, i + ITEMDATA_STARTIDX );
			}
		}
		break;
	case WAITING_COST_FACTORS:
		{
			std::vector<WaitingCostFactorDataItem>& vData = pEconMan->m_pWaitingCostFactors->GetDataList();
			int nCount = vData.size();
			for( int i=0; i<nCount; i++ )
			{
					CString szBuf;
				vData[i].m_pPaxType->screenPrint( szBuf );
				int nItem = m_List.InsertItem( i, szBuf );
				CString csVal;
				csVal.Format( "%.2f", vData[i].m_dCostPerHour );
				m_List.SetItemText( nItem, 1, csVal );
				m_List.SetItemData( nItem, i + ITEMDATA_STARTIDX );
			}
		}
		break;
	}
}


void CEconomic2Dialog::OnEndlabeleditListEconomic2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	
	*pResult = 0;

	int nIdx = pDispInfo->item.iItem;

	int nItemData = m_List.GetItemData( nIdx );

	if( nItemData == 0 )
		return;

	nItemData -= ITEMDATA_STARTIDX;
		
	// modify the exist data.
	if( pDispInfo->item.iSubItem == 0 )
		return;

	CEconomicManager* pEconMan = GetEconomicManager();

	CString csStr = m_List.GetItemText( nIdx, pDispInfo->item.iSubItem );
	csStr.TrimLeft(); 
	csStr.TrimRight();
	double dVal = atof( csStr );

	
	switch( m_enumType )
	{
	case PASSENGER:		
		{
			std::vector<PassengerCostDataItem>& vData = pEconMan->m_pPassengerCost->GetDataList();
			switch( pDispInfo->item.iSubItem )
			{
			case 1:
				vData[nItemData].m_dFixedPerPax = dVal;
				break;
			case 2:
				vData[nItemData].m_dVarPerHour = dVal;
				break;
			case 3:
				vData[nItemData].m_dVarPerProc = dVal;
				break;
			case 4:
				vData[nItemData].m_dVarPerCart = dVal;
				break;
			case 5:
				vData[nItemData].m_dVarPerBag = dVal;
				break;
			}
			break;
		}
	case AIRCRAFT:
		{
			std::vector<AircraftCostDataItem>& vData = pEconMan->m_pAircraftCost->GetDataList();
			switch( pDispInfo->item.iSubItem )
			{
			case 1:
				vData[nItemData].m_dServicePerAC = dVal;
				break;
			case 2:
				vData[nItemData].m_dServicePerHour = dVal;
				break;
			}
			break;
		}

	case PASSENGER_AIRPORT_FEES:
		{
			std::vector<PaxAirportFeeDataItem>& vData = pEconMan->m_pPaxAirportFeeRevenue->GetDataList();
			switch( pDispInfo->item.iSubItem )
			{
			case 1:
				vData[nItemData].m_dFeePerPax = dVal;
				break;
			case 2:
				vData[nItemData].m_dFeePerHour = dVal;
				break;
			}
			break;
		}

	case LANDING_FEES:
		{
			std::vector<LandingFeeRevDataItem>& vData = pEconMan->m_pLandingFeesRevenue->GetDataList();
			switch( pDispInfo->item.iSubItem )
			{
			case 1:
				vData[nItemData].m_dFeePerLanding = dVal;
				break;
			case 2:
				vData[nItemData].m_dFeePer1000MLW = dVal;
				break;
			}
			break;
		}

	case WAITING_COST_FACTORS:
		{
			std::vector<WaitingCostFactorDataItem>& vData = pEconMan->m_pWaitingCostFactors->GetDataList();
			switch( pDispInfo->item.iSubItem )
			{
			case 1:
				vData[nItemData].m_dCostPerHour = dVal;
				break;
			}
			break;
		}

	}
			
	m_btnSave.EnableWindow();

}

void CEconomic2Dialog::OnItemchangedListEconomic2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	if (GetSelectedItem() >= 0)
		m_ListToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,TRUE);
	
	*pResult = 0;
}

void CEconomic2Dialog::OnBtnEconomic2Save() 
{
	CEconomicManager* pEconMan = GetEconomicManager();
	CWaitCursor wc;
	
	switch( m_enumType )
	{
	case PASSENGER:
		pEconMan->m_pPassengerCost->saveDataSet( GetProjPath(), true );
		break;

	case AIRCRAFT:
		pEconMan->m_pAircraftCost->saveDataSet( GetProjPath(), true );
		break;

	case PASSENGER_AIRPORT_FEES:
		pEconMan->m_pPaxAirportFeeRevenue->saveDataSet( GetProjPath(), true );
		break;

	case LANDING_FEES:
		pEconMan->m_pLandingFeesRevenue->saveDataSet( GetProjPath(), true );
		break;

	case WAITING_COST_FACTORS:
		pEconMan->m_pWaitingCostFactors->saveDataSet( GetProjPath(), true );
		break;
	}
	m_btnSave.EnableWindow(FALSE);	
}

void CEconomic2Dialog::OnPeoplemoverNew() 
{
	BOOL bDirty = FALSE;
	CString strTemp;
	
//	TCHAR szName[256];
//	ZeroMemory(&szName, sizeof(szName));
CString szName;
	CEconomicManager* pEconMan = GetEconomicManager();
	int nIdx = -1;
	switch( m_enumType )
	{
	case PASSENGER:
	case PASSENGER_AIRPORT_FEES:
	case WAITING_COST_FACTORS:
		{
			CPassengerTypeDialog dlg( m_pParentWnd );
			if (dlg.DoModal() == IDOK)
			{
				//PassengerConstraint newPaxCon = dlg.GetPaxSelection();
				CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();
				mobileConstr.screenPrint(szName);
				//char szRaw[512]; matt
				char szRaw[2560];
				mobileConstr.WriteSyntaxStringWithVersion( szRaw );

				// check the if duplicated.
				if( m_enumType == PASSENGER )
				{
					std::vector<PassengerCostDataItem>& vData = pEconMan->m_pPassengerCost->GetDataList();

					// check duplicated.
					int nCount = vData.size();
					for( int i=0; i<nCount; i++ )
					{
						if( mobileConstr.isEqual( vData[i].m_pPaxType ) )
							break;
					}
					if( i == nCount )
					{
						PassengerCostDataItem item;
						item.m_pPaxType = new CMobileElemConstraint(GetInputTerminal());
						item.m_pPaxType->SetInputTerminal( GetInputTerminal() );
						item.m_pPaxType->setConstraintWithVersion( szRaw );
						item.m_dFixedPerPax = 0.0;
						item.m_dVarPerHour = 0.0;
						item.m_dVarPerProc = 0.0;
						item.m_dVarPerCart = 0.0;
						item.m_dVarPerBag = 0.0;

						vData.push_back( item );
						nIdx = vData.size() - 1;
						bDirty = TRUE;
					}

				}
				else if( m_enumType == PASSENGER_AIRPORT_FEES )
				{
					std::vector<PaxAirportFeeDataItem>& vData = pEconMan->m_pPaxAirportFeeRevenue->GetDataList();
					// check duplicated.
					int nCount = vData.size();
					for( int i=0; i<nCount; i++ )
					{
						if( mobileConstr.isEqual( vData[i].m_pPaxType ) )
							break;
					}
					if( i == nCount )
					{
						PaxAirportFeeDataItem item;
						item.m_pPaxType = new CMobileElemConstraint(GetInputTerminal());
						item.m_pPaxType->SetInputTerminal( GetInputTerminal() );
						item.m_pPaxType->setConstraintWithVersion( szRaw );
						item.m_dFeePerPax = 0.0;
						item.m_dFeePerHour = 0.0;

						vData.push_back( item );																				
						nIdx = vData.size() - 1;
						bDirty = TRUE;
					}
				}
				else
				{
					std::vector<WaitingCostFactorDataItem>& vData = pEconMan->m_pWaitingCostFactors->GetDataList();
					int nCount = vData.size();
					for( int i=0; i<nCount; i++ )
					{
						if( mobileConstr.isEqual( vData[i].m_pPaxType ) )
							break;
					}
					if( i == nCount )
					{
						WaitingCostFactorDataItem item;
						item.m_pPaxType = new CMobileElemConstraint(GetInputTerminal());
						item.m_pPaxType->SetInputTerminal( GetInputTerminal() );
						item.m_pPaxType->setConstraintWithVersion( szRaw );
						item.m_dCostPerHour = 0.0;

						vData.push_back( item );																				
						nIdx = vData.size() - 1;
						bDirty = TRUE;
					}
				}
				
			}
			break;
		}
	case AIRCRAFT:		
	case LANDING_FEES:
		{
			CFlightDialog dlg( m_pParentWnd );
			if( dlg.DoModal() == IDOK )
			{
				FlightConstraint newFltCon = dlg.GetFlightSelection();
				newFltCon.screenPrint(szName);
				//char szRaw[512]; matt
				char szRaw[2560];
				newFltCon.WriteSyntaxStringWithVersion( szRaw );

				if( m_enumType == AIRCRAFT )
				{
					std::vector<AircraftCostDataItem>& vData = pEconMan->m_pAircraftCost->GetDataList();

					int nCount = vData.size();
					for( int i=0; i<nCount; i++ )
					{
						if( newFltCon.isEqual( vData[i].m_pFltType ) )
							break;
					}
					if( i == nCount )
					{
						AircraftCostDataItem item;
						item.m_pFltType = new FlightConstraint;
						//item.m_pFltType->SetInputTerminal( GetInputTerminal() );
						item.m_pFltType->SetAirportDB(GetInputTerminal()->m_pAirportDB);
						item.m_pFltType->setConstraintWithVersion( szRaw );
						item.m_dServicePerAC = 0.0;
						item.m_dServicePerHour = 0.0;
						vData.push_back( item );
						nIdx = vData.size() - 1;
						bDirty = TRUE;
					}
				}
				else
				{
					std::vector<LandingFeeRevDataItem>& vData = pEconMan->m_pLandingFeesRevenue->GetDataList();

					int nCount = vData.size();
					for( int i=0; i<nCount; i++ )
					{
						if( newFltCon.isEqual( vData[i].m_pFltType ) )
							break;
					}
					if( i == nCount )
					{
						
						LandingFeeRevDataItem item;
						item.m_pFltType = new FlightConstraint;
						//item.m_pFltType->SetInputTerminal( GetInputTerminal() );
						item.m_pFltType->SetAirportDB(GetInputTerminal()->m_pAirportDB);
						item.m_pFltType->setConstraintWithVersion( szRaw );
						item.m_dFeePerLanding = 0.0;
						item.m_dFeePer1000MLW = 0.0;
						vData.push_back( item );

						nIdx = vData.size() - 1;
						bDirty = TRUE;
					}
				}
				
			}
			break;
		}
	default:
		ASSERT(FALSE);   // Can not go here
		break;
	}

	if(!bDirty ||szName=="") //_tcscmp(szName, "") == NULL)
		return;	

	CHeaderCtrl* pHeader = m_List.GetHeaderCtrl();
	int nColumnCount = pHeader->GetItemCount();
	int nCount = m_List.GetItemCount();

	BOOL bEmpty = TRUE;
	CString str;
	
	str = m_List.GetItemText(nCount-1, 0);
	str.TrimLeft(); str.TrimRight();
	if (!str.IsEmpty() || nCount <= 0)
		m_List.InsertItem(max(nCount, 0), "");
	
	int nItem = m_List.GetItemCount() - 1;
	ASSERT(nItem >= 0);
	if (nItem < 0) return;
	m_List.SetItemText(nItem, 0, szName);
	for (int i = 1; i < nColumnCount; i++)
	{
		str = m_List.GetItemText(nItem, i);
		str.TrimLeft(); str.TrimRight();
		if (!str.IsEmpty())
			continue;
		
		m_List.SetItemText(nItem, i, "0");
	}

	m_List.SetItemData( nItem, nIdx + ITEMDATA_STARTIDX );
	
	m_btnSave.EnableWindow();
}

void CEconomic2Dialog::OnPeoplemoverDelete() 
{
	int nSelIdx = GetSelectedItem();
	if( nSelIdx == -1 )
		return;

	int nDBIdx = m_List.GetItemData( nSelIdx ) - ITEMDATA_STARTIDX;

	CEconomicManager* pEconMan = GetEconomicManager();
	
	switch( m_enumType )
	{
	case PASSENGER:		
		{
			std::vector<PassengerCostDataItem>& vData = pEconMan->m_pPassengerCost->GetDataList();
			vData.erase( vData.begin() + nDBIdx );
			break;
		}
	case AIRCRAFT:
		{
			std::vector<AircraftCostDataItem>& vData = pEconMan->m_pAircraftCost->GetDataList();
			vData.erase( vData.begin() + nDBIdx );
			break;
		}

	case PASSENGER_AIRPORT_FEES:
		{
			std::vector<PaxAirportFeeDataItem>& vData = pEconMan->m_pPaxAirportFeeRevenue->GetDataList();
			vData.erase( vData.begin() + nDBIdx );
			break;
		}

	case LANDING_FEES:
		{
			std::vector<LandingFeeRevDataItem>& vData = pEconMan->m_pLandingFeesRevenue->GetDataList();
			vData.erase( vData.begin() + nDBIdx );
			break;
		}

	case WAITING_COST_FACTORS:
		{
			std::vector<WaitingCostFactorDataItem>& vData = pEconMan->m_pWaitingCostFactors->GetDataList();
			vData.erase( vData.begin() + nDBIdx );
			break;
		}

	}

	ReloadDatabase();
	m_btnSave.EnableWindow();
	m_ListToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,FALSE);
}

void CEconomic2Dialog::InitListCtrl()
{
	if (!::IsWindow(m_List.GetSafeHwnd()))
		return;

	if (m_arColName.GetSize() <= 0)
		return;
	
	// set list ctrl	
	DWORD dwStyle = m_List.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_List.SetExtendedStyle( dwStyle );

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;	

	CRect rcList;
	m_List.GetClientRect(&rcList);
	int w1 = 200;
	int w = ( rcList.Width() - w1 ) / ( m_arColName.GetSize() - 1 );
	int nFormat[] = { LVCFMT_CENTER | LVCFMT_NOEDIT, LVCFMT_CENTER | LVCFMT_NUMBER };

	for( int i = 0; i < m_arColName.GetSize(); i++ )
	{
		CStringList strList;
		strList.AddTail( CString("OPEN") );
		strList.AddTail( CString("CLOSE") );
		lvc.csList = &strList;
		lvc.pszText = (LPSTR) ((LPCTSTR) m_arColName.GetAt(i));
		lvc.cx = (i== 0 ? w1:w);
		lvc.fmt = (i == 0 ? nFormat[0] : nFormat[1]);
		m_List.InsertColumn( i, &lvc );
	}
}

void CEconomic2Dialog::InitToolbar()
{
	CRect rc;
	m_listtoolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ListToolBar.MoveWindow(&rc);
	m_ListToolBar.ShowWindow(SW_SHOW);
	m_listtoolbarcontenter.ShowWindow(SW_HIDE);
	m_ListToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW ,TRUE);	
	m_ListToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,FALSE);	
	m_ListToolBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE );
}

void CEconomic2Dialog::SetTitle(LPCTSTR szTitle)
{
	m_strTitle = szTitle;

	if (!::IsWindow(GetSafeHwnd()))
		return;

	SetWindowText(m_strTitle);
}

void CEconomic2Dialog::SetColNameArray(const CStringArray& arColName)
{
	m_arColName.RemoveAll();
	
	for (int i = 0; i < arColName.GetSize(); i++)
		m_arColName.Add(arColName.GetAt(i));
}

int CEconomic2Dialog::GetSelectedItem()
{
	int nCount = m_List.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_List.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
		{
			return i;
		}
	}
	return -1;
}

void CEconomic2Dialog::OnOK() 
{
	if (m_btnSave.IsWindowEnabled())
		OnBtnEconomic2Save();
	CDialog::OnOK();
}

void CEconomic2Dialog::OnCancel() 
{
	if (m_btnSave.IsWindowEnabled())
	{
		CEconomicManager* pEconMan = GetEconomicManager();
		try
		{		
			switch( m_enumType )
			{
			case PASSENGER:
				pEconMan->m_pPassengerCost->loadDataSet( GetProjPath() );
				break;

			case AIRCRAFT:
				pEconMan->m_pAircraftCost->loadDataSet( GetProjPath() );
				break;

			case PASSENGER_AIRPORT_FEES:
				pEconMan->m_pPaxAirportFeeRevenue->loadDataSet( GetProjPath() );
				break;

			case LANDING_FEES:
				pEconMan->m_pLandingFeesRevenue->loadDataSet( GetProjPath() );
				break;

			case WAITING_COST_FACTORS:
				pEconMan->m_pWaitingCostFactors->loadDataSet( GetProjPath() );
				break;
			}
		}
		catch( FileVersionTooNewError* _pError )
		{
			char szBuf[128];
			_pError->getMessage( szBuf );
			MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
			delete _pError;			
		}

	}
	
	CDialog::OnCancel();
}


CEconomicManager* CEconomic2Dialog::GetEconomicManager()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (CEconomicManager*)&pDoc->GetEconomicManager();
}


CString CEconomic2Dialog::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}

InputTerminal* CEconomic2Dialog::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}

BOOL CEconomic2Dialog::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_toolTips.RelayEvent( pMsg );
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CEconomic2Dialog::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int nIndex = m_List.HitTest( point );

	if( nIndex!=-1 )
	{
		if(m_nListPreIndex != nIndex)
		{
			int iItemData = m_List.GetItemData( nIndex ) - ITEMDATA_STARTIDX;
			ShowTips( iItemData );
			m_nListPreIndex = nIndex;
		}
	}
	else
	{
		m_nListPreIndex = -1;	
		m_toolTips.DelTool( &m_List );
	}
	
	CDialog::OnMouseMove(nFlags, point);
}

void CEconomic2Dialog::ShowTips(int iItemData)
{
	CString strTips;
	Constraint* pCon = NULL;
	CEconomicManager* pEconMan = GetEconomicManager();
	
	switch( m_enumType )
	{
	case PASSENGER:
		{
			std::vector<PassengerCostDataItem>& vData = pEconMan->m_pPassengerCost->GetDataList();
			pCon = vData[ iItemData ].m_pPaxType;
			break;
		}

	case PASSENGER_AIRPORT_FEES:
		{
			std::vector<PaxAirportFeeDataItem>& vData = pEconMan->m_pPaxAirportFeeRevenue->GetDataList();
			pCon = vData[ iItemData ].m_pPaxType;
			break;
		}

	case WAITING_COST_FACTORS:
		{
			std::vector<WaitingCostFactorDataItem>& vData = pEconMan->m_pWaitingCostFactors->GetDataList();
			pCon = vData[ iItemData ].m_pPaxType;
			break;
		}

	case AIRCRAFT:
		{
			std::vector<AircraftCostDataItem>& vData = pEconMan->m_pAircraftCost->GetDataList();
			pCon = vData[ iItemData ].m_pFltType;
			break;
		}

	case LANDING_FEES:
		{
			std::vector<LandingFeeRevDataItem>& vData = pEconMan->m_pLandingFeesRevenue->GetDataList();		
			pCon = vData[ iItemData ].m_pFltType;
			break;
		}
	}

	if( pCon )
	{
		pCon->screenTips( strTips );
		m_toolTips.ShowTips( strTips );
	}
}
