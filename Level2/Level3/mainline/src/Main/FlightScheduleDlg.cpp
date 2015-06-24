// FlightScheduleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "FlightScheduleDlg.h"
#include "inputs\flight.h"
#include "engine\terminal.h"
#include "inputs\schedule.h"
#include "fltscheddefinedlg.h"
#include "TermPlanDoc.h"
#include "inputs\paxflow.h"
#include "ImportFlightFromFileDlg.h"
#include "FlightScheduleAddNewDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern const CString c_setting_regsectionstring = "setting";
const CString c_fltschd_sorting_entry = "Flight Schedule Sorting String";
const char* columnLabel[] = {	
	"Index",
	"Airline", 
		"Arr ID", 
		"Origin",
		"Arr Stopover",
		//Modify by adam 2007-04-24
		//"Arr Date",
		"Day",
		//"Arr Time",
		"Time",
		"Arr Loadfactor(%)",
		//End Modify by adam 2007-04-24
		"Dep ID",
		"Dep Stopover",
		"Destination",
		//Modify by adam 2007-04-24
		//"Dep Date",
		"Day",
		//"Dep Time",
		"Time",
		"Dep Loadfactor(%)",
		//End Modify by adam 2007-04-24
		"Ac Type",
		"Capacity",
		"Arr Stand",
		"Dep Stand",
		"Intermediate Stand",
		"Stay Time(HH:MM)",
		"Arr Gate",
		"Dep Gate",
		"BagDevice",
		"ADA",
		"ATA",
		"ADD",
		"ATD",
		"D TOW OFF",
		"T TOW OFF",
		"D EX INT STAND",
		"T EX INT STAND",
};

/////////////////////////////////////////////////////////////////////////////
// CFlightScheduleDlg dialog


CFlightScheduleDlg::CFlightScheduleDlg(CWnd* pParent)
: CDialog(CFlightScheduleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFlightScheduleDlg)
	//}}AFX_DATA_INIT
	m_pParent=pParent;
	m_iPreSortByColumnIdx = -1;
}


void CFlightScheduleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFlightScheduleDlg)
	DDX_Control(pDX, IDC_BUTTON_MODIFY_SCHDULE, m_btModifySchedule);
	DDX_Control(pDX, IDOK, m_butOK);
	DDX_Control(pDX, IDCANCEL, m_butCancel);
	DDX_Control(pDX, IDC_STATIC_FRAME, m_staticFrame);
	DDX_Control(pDX, IDC_BUTTON_SAVEAS, m_butSaveAs);
	DDX_Control(pDX, IDC_BUTTON_LOADFROM, m_butLoadFrom);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_LIST_SCHEDULELIST, m_listCtrlSchdList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFlightScheduleDlg, CDialog)
	//{{AFX_MSG_MAP(CFlightScheduleDlg)
	ON_MESSAGE(SORT_INDEX_COMPLETE_MSG,OnColumnclickListSchedulelist)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SCHEDULELIST, OnDblclkListSchedulelist)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SCHEDULELIST, OnClickListSchedulelist)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_SAVEAS, OnButtonSaveas)
	ON_BN_CLICKED(IDC_BUTTON_LOADFROM, OnButtonLoadfrom)
	ON_BN_CLICKED(IDC_BUTTON_PRINT, OnButtonPrint)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_COMMAND(ID_BUTTON_SPLIT, OnButtonSplit)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_SCHDULE, OnButtonModifySchdule)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlightScheduleDlg message handlers

BOOL CFlightScheduleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	AfxGetApp()->BeginWaitCursor();
	// Initialize the toolbar
	m_ToolBar.ShowWindow(SW_SHOW);
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	
	// init list ctrl
	
	InitGridTitle();
	LoadFlight();

	InitToolbar();

	//Init FlowConvetor,use to update Detail Gate Flow( only temp)
	InitFlowConvetor();
	/////////////////////////////////////////////////
	AfxGetApp()->EndWaitCursor();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFlightScheduleDlg::InitGridTitle()
{
	DWORD dwStyle = m_listCtrlSchdList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listCtrlSchdList.SetExtendedStyle( dwStyle );

	int DefaultColumnWidth[] = {30, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 80, 80, 80, 75, 80, 80, 80
	,75,75,75,75,75,75,75,75};
	int nFormat[] = {LVCFMT_CENTER,	LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER,
		LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER,  LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, 
		LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT,
		LVCFMT_LEFT,LVCFMT_LEFT,LVCFMT_LEFT,LVCFMT_LEFT,LVCFMT_LEFT,LVCFMT_LEFT,LVCFMT_LEFT,LVCFMT_LEFT};

	// column
	m_listCtrlSchdList.SubClassHeadCtrl();
	for( int i=0; i<31; i++ )
	{
		m_listCtrlSchdList.InsertColumn( i, columnLabel[i], nFormat[i], DefaultColumnWidth[i] );
		m_listCtrlSchdList.SetDataType( i, CSortAndPrintListCtrl::ITEM_STRING );
		if(i== 0 || i==2 || i==7 ||i==8 || i==13 || i == 15) 
			m_listCtrlSchdList.SetDataType(i,CSortAndPrintListCtrl::ITEM_INT);

		if(i==5 || i==6 || i==11 || i==12 || i == 19 || i > 22)
			m_listCtrlSchdList.SetDataType(i,CSortAndPrintListCtrl::ITEM_TIME);
	}
}

LRESULT CFlightScheduleDlg::OnColumnclickListSchedulelist(WPARAM wParam, LPARAM lParam) 
{
	int nSubItem = (int)lParam;
	if (nSubItem != 0)
	{
		Flight *pFlight;
		int count = GetInputTerminal()->flightSchedule->getCount();
		for (int i = 0; i < count; i++)
		{
			pFlight = GetInputTerminal()->flightSchedule->getItem (i);

			CString strIndex;
			strIndex.Format(_T("%d"),i+1);
			m_listCtrlSchdList.SetItemText( i, 0, strIndex );
		}
	}

	return 0;
}

void CFlightScheduleDlg::LoadFlight(Flight* _pSelectFlight/*=NULL*/)
{
	m_listCtrlSchdList.DeleteAllItems();
	Flight *pFlight;
	int count = GetInputTerminal()->flightSchedule->getCount();
	int iSelect = -1;
	for (int i = 0; i < count; i++)
	{
		pFlight = GetInputTerminal()->flightSchedule->getItem (i);
		if( pFlight == _pSelectFlight )
		{
			iSelect= AppendOneFlightIntoListCtrl( pFlight, i );
		}
		else
		{
			AppendOneFlightIntoListCtrl( pFlight, i );
		}
	}
}

void CFlightScheduleDlg::OnButtonAdd() 
{
	// TODO: Add your control notification handler code here
	CFltSchedDefineDlg dlg(	GetInputTerminal() );
	if( dlg.DoModal() == IDCANCEL )
		return;
	
	Flight* pFlight = new Flight( (Terminal*)GetInputTerminal() );
	dlg.GetFlight( *pFlight );
	
	// add into database.
    GetInputTerminal()->flightSchedule->addItem( pFlight );
	
	// find index in the database.
	int nDBIdx = GetInputTerminal()->flightSchedule->findItem( pFlight );
	
	if (dlg.m_bStartDateChanged == TRUE)
		LoadFlight(pFlight);
	else
	{
		int nIdx = AppendOneFlightIntoListCtrl( pFlight, nDBIdx );
		SetFlightSelected( nIdx );
	}

	//&
	
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_DEL );	
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_EDIT );
	if( pFlight->isTurnaround() )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_SPLIT );
	}
	else
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_SPLIT ,FALSE);
	}
	
	
	m_btnSave.EnableWindow();
}

void CFlightScheduleDlg::OnButtonEdit() 
{
	// TODO: Add your control notification handler code here
	int nIndex = GetSelIndex();
	if( nIndex == -1 )
		return;

	Flight* pFlight = (Flight*)m_listCtrlSchdList.GetItemData( nIndex );
	CFltSchedDefineDlg dlg( *pFlight, GetInputTerminal() );
	if( dlg.DoModal() == IDCANCEL )
		return;

	dlg.GetFlight( *pFlight );	
	SetFlightInListCtrl( nIndex, *pFlight );

	// add into database.
	Flight* pFlightTemp = new Flight( (Terminal*)GetInputTerminal() );
	*pFlightTemp = *pFlight;
	int nFltIndex = GetInputTerminal()->flightSchedule->findItem( pFlight );
	GetInputTerminal()->flightSchedule->deleteItem(nFltIndex);
	GetInputTerminal()->flightSchedule->addItem( pFlightTemp );

	if (dlg.m_bStartDateChanged  == TRUE)
		LoadFlight(pFlightTemp);
	else
		m_listCtrlSchdList.SetItemData( nIndex, (unsigned long)pFlightTemp );


	m_btnSave.EnableWindow();
}

void CFlightScheduleDlg::OnButtonDel() 
{
	// TODO: Add your control notification handler code here
	std::vector<int> vecIndices;
	int nCount = GetSelIndices(vecIndices);

	for (int i=0;i<nCount;i++)
	{
		Flight* pFlight = (Flight*)m_listCtrlSchdList.GetItemData( vecIndices[i] );
		int nFltIndex = GetInputTerminal()->flightSchedule->findItem( pFlight );
		GetInputTerminal()->flightSchedule->deleteItem(nFltIndex);
	}

	LoadFlight();
	//SetFlightSelected( -1 );
	if( m_iPreSortByColumnIdx >= 0 )
	{
		/*CWaitCursor	wCursor;
		m_ctlHeaderCtrl.SortColumn( m_iPreSortByColumnIdx, SINGLE_COLUMN_SORT );
		m_ctlHeaderCtrl.SortColumn( m_iPreSortByColumnIdx, SINGLE_COLUMN_SORT );
		m_ctlHeaderCtrl.SaveSortList();*/
	}	

	//
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_DEL ,FALSE);	
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_EDIT ,FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_SPLIT ,FALSE);

	m_btnSave.EnableWindow();
}

void CFlightScheduleDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	// copy the flight data to the GetInputTerminal()->flightSchedule
	GetInputTerminal()->flightSchedule->updateScheduleEndDay();
	GetInputTerminal()->flightSchedule->saveDataSet(GetProjPath(), true);	

	//Update Detail Gate Flow
	UpdateFlow();

	//CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	//pDoc->ReloadInputData();

	////////////////////////////////////////////////////////////////////////////////////////
	m_btnSave.EnableWindow( false );
}

int CFlightScheduleDlg::GetSelIndex()
{
	for( int i=m_listCtrlSchdList.GetItemCount()-1; i>-1; i-- )
	{
		if( ( m_listCtrlSchdList.GetItemState(i,LVIS_SELECTED) & LVIS_SELECTED ) == LVIS_SELECTED )
		{
			return i;
		}
	}

	return -1;
}

int CFlightScheduleDlg::GetSelIndices(std::vector<int>& vecIndices)
{
	vecIndices.clear();
	for( int i=m_listCtrlSchdList.GetItemCount()-1; i>-1; i-- )
	{
		if( ( m_listCtrlSchdList.GetItemState(i,LVIS_SELECTED) & LVIS_SELECTED ) == LVIS_SELECTED )
		{
			vecIndices.push_back(i);
		}
	}

	return (int)vecIndices.size();
}

int CFlightScheduleDlg::AppendOneFlightIntoListCtrl( const Flight* _pFlight ,int _nIndex)
{
	
	int nIndex = m_listCtrlSchdList.GetItemCount();
	CString strIndex;
	strIndex.Format(_T("%d"),_nIndex + 1);
	m_listCtrlSchdList.InsertItem( nIndex, strIndex );
	SetFlightInListCtrl( nIndex, *_pFlight );
	m_listCtrlSchdList.SetItemData( nIndex, (unsigned long)_pFlight );
	return nIndex;
}

void CFlightScheduleDlg::SetFlightSelected(int _nIndex)
{
	for( int i=m_listCtrlSchdList.GetItemCount()-1; i>-1; i-- )
	{
		if( i == _nIndex )
			m_listCtrlSchdList.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
		else
			m_listCtrlSchdList.SetItemState( i, 0, LVIS_SELECTED );
	}
}

void CFlightScheduleDlg::OnOK() 
{
	// TODO: Add extra validation here
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnOK();
		return;
	}
	
	AfxGetApp()->BeginWaitCursor();
	OnButtonSave();
	AfxGetApp()->EndWaitCursor();

	CDialog::OnOK();
}


void CFlightScheduleDlg::SetFlightInListCtrl( int _nIndex, const Flight& _flight )
{
	char str[128];
	CString strProcID;
	//Delete by adam 2007-04-24
	//CStartDate sDate = GetInputTerminal()->flightSchedule->GetStartDate();
	//COleDateTime outDate;
	//End Delete by adam 2007-04-24

	COleDateTime outTime;
	//bool bAbsDate;
	//int nOutDateIdx;

	CString strIndex;
	strIndex.Format(_T("%d"),_nIndex + 1);
	m_listCtrlSchdList.SetItemText(_nIndex, 0 ,strIndex);
	_flight.getAirline( str );
	m_listCtrlSchdList.SetItemText( _nIndex, 1, str );
	if( _flight.isArriving() )
	{
		_flight.getArrID( str );
		m_listCtrlSchdList.SetItemText( _nIndex, 2, str );

		_flight.getOrigin( str );
		m_listCtrlSchdList.SetItemText( _nIndex, 3, str );

		_flight.getArrStopover( str );
		m_listCtrlSchdList.SetItemText( _nIndex, 4, str );

		//Modify by adam 2007-04-24
		// arr date
		//sDate.GetDateTime(  _flight.getArrTime(), bAbsDate, outDate, nOutDateIdx, outTime );
		//if( bAbsDate )
		//	m_listCtrlSchdList.SetItemText( _nIndex, 4, outDate.Format( _T("%x")) );
		CString strDay;
		ElapsedTime etDayTime = _flight.getArrTime();
		strDay.Format("%d", etDayTime.GetDay());
		m_listCtrlSchdList.SetItemText( _nIndex, 5, strDay );
		//End Modify by adam 2007-04-24

		outTime.SetTime(etDayTime.GetHour(), etDayTime.GetMinute(), etDayTime.GetSecond());
		//(_flight.getArrTime()).printTime( str, 0 );
		m_listCtrlSchdList.SetItemText( _nIndex, 6, outTime.Format( _T("%X") ) );

		CString strLoad = "";
		double dValue = _flight.getArrLFInput();
		if (dValue >0)
			strLoad.Format("%.2f", dValue*100);
		m_listCtrlSchdList.SetItemText( _nIndex, 7, strLoad );		

		//ata
		{
			if (_flight.GetAtaTime() >= 0)
			{
				CString strAtaDay;
				ElapsedTime eAtaTime;
				eAtaTime.setPrecisely(_flight.GetAtaTime());

				//Day
				strAtaDay.Format("%d", eAtaTime.GetDay());
				m_listCtrlSchdList.SetItemText( _nIndex, 23, strAtaDay );

				//Time
				outTime.SetTime(eAtaTime.GetHour(), eAtaTime.GetMinute(), eAtaTime.GetSecond());
				m_listCtrlSchdList.SetItemText( _nIndex, 24, outTime.Format( _T("%X") ) );
			}
			else
			{
				m_listCtrlSchdList.SetItemText( _nIndex, 23, " " );
				m_listCtrlSchdList.SetItemText( _nIndex, 24, " " );
			}
		}
	}
	else
	{
		m_listCtrlSchdList.SetItemText( _nIndex, 2, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 3, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 4, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 5, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 6, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 7, " " );

		m_listCtrlSchdList.SetItemText( _nIndex, 23, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 24, " " );
	}

	if( _flight.isDeparting() )
	{
		_flight.getDepID( str );
		m_listCtrlSchdList.SetItemText( _nIndex, 8, str );
		_flight.getDepStopover( str );
		m_listCtrlSchdList.SetItemText( _nIndex, 9, str );
		_flight.getDestination( str );
		m_listCtrlSchdList.SetItemText( _nIndex, 10, str );

		//Modify by adam 2007-04-24
		// dep date
		//sDate.GetDateTime(  _flight.getDepTime(), bAbsDate, outDate, nOutDateIdx, outTime );
		//if( bAbsDate )
		//	m_listCtrlSchdList.SetItemText( _nIndex, 9, outDate.Format( _T("%x")));
		CString strDay;
		ElapsedTime etDayTime = _flight.getDepTime();
		strDay.Format("%d", etDayTime.GetDay());
		m_listCtrlSchdList.SetItemText( _nIndex, 11, strDay );

		outTime.SetTime(etDayTime.GetHour(), etDayTime.GetMinute(), etDayTime.GetSecond());
		//(_flight.getDepTime()).printTime( str, 0 );
		m_listCtrlSchdList.SetItemText( _nIndex, 12, outTime.Format( _T("%X") ) );
		//Modify by adam 2007-04-24

		CString strLoad = "";
		double dValue = _flight.getDepLFInput();
		if (dValue >0)
			strLoad.Format("%.2f", dValue*100);
		m_listCtrlSchdList.SetItemText( _nIndex, 13, strLoad );		

		//atd
		{
			if (_flight.GetAtdTime() >= 0)
			{
				CString strAtdDay;
				ElapsedTime eAtdTime;
				eAtdTime.setPrecisely(_flight.GetAtdTime());

				//Day
				strAtdDay.Format("%d", eAtdTime.GetDay());
				m_listCtrlSchdList.SetItemText( _nIndex, 25, strAtdDay );

				//Time
				outTime.SetTime(eAtdTime.GetHour(), eAtdTime.GetMinute(), eAtdTime.GetSecond());
				m_listCtrlSchdList.SetItemText( _nIndex, 26, outTime.Format( _T("%X") ) );
			}
			else
			{
				m_listCtrlSchdList.SetItemText( _nIndex, 25, " " );
				m_listCtrlSchdList.SetItemText( _nIndex, 26, " " );
			}
		}	
	}
	else
	{
		m_listCtrlSchdList.SetItemText( _nIndex, 8, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 9, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 10, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 11, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 12, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 13, " " );

		m_listCtrlSchdList.SetItemText( _nIndex, 25, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 26, " " );
	}

	_flight.getACType( str );
	m_listCtrlSchdList.SetItemText( _nIndex, 14, str );

	CString strCap = "";
	if (_flight.getCapacityInput() >0)
		strCap.Format("%d", _flight.getCapacityInput());
	m_listCtrlSchdList.SetItemText( _nIndex, 15, strCap );	

	strProcID = _flight.getArrStand().GetIDString();
	if( strProcID == "All Processors" )
		strProcID = " ";
	m_listCtrlSchdList.SetItemText( _nIndex, 16, strProcID );
	if (strProcID ==" " && _flight.isArriving())
	{
		ASSERT(FALSE);
	}

	strProcID = _flight.getDepStand().GetIDString();
	if( strProcID == "All Processors" )
		strProcID = " ";
	m_listCtrlSchdList.SetItemText( _nIndex, 17, strProcID );

	strProcID = _flight.getIntermediateStand().GetIDString();
	if( strProcID == "All Processors" )
		strProcID = " ";
	m_listCtrlSchdList.SetItemText( _nIndex, 18, strProcID );

	ElapsedTime time;
	if( _flight.isTurnaround())
		time = _flight.getDepTime() - _flight.getArrTime();
	else
		time = _flight.getServiceTime();

	time.printTime( str, 0 );
	m_listCtrlSchdList.SetItemText( _nIndex, 19, str );

	strProcID = _flight.getArrGate().GetIDString();
	if( strProcID == "All Processors" )
		strProcID = " ";
	m_listCtrlSchdList.SetItemText( _nIndex, 20,strProcID );

	strProcID = _flight.getDepGate().GetIDString();
	if( strProcID == "All Processors" )
		strProcID = " ";
	m_listCtrlSchdList.SetItemText( _nIndex, 21, strProcID );
	strProcID = _flight.getBaggageDevice().GetIDString();
	if(strProcID =="All Processors")
		strProcID =" ";
	m_listCtrlSchdList.SetItemText(_nIndex,22,strProcID);

	//tow off
	{
		if (_flight.GetTowoffTime() >= 0)
		{
			CString strTowoffDay;
			ElapsedTime eTowoffTime;
			eTowoffTime.setPrecisely(_flight.GetTowoffTime());

			//Day
			strTowoffDay.Format("%d", eTowoffTime.GetDay());
			m_listCtrlSchdList.SetItemText( _nIndex, 27, strTowoffDay );

			//Time
			outTime.SetTime(eTowoffTime.GetHour(), eTowoffTime.GetMinute(), eTowoffTime.GetSecond());
			m_listCtrlSchdList.SetItemText( _nIndex, 28, outTime.Format( _T("%X") ) );
		}
		else
		{
			m_listCtrlSchdList.SetItemText( _nIndex, 27, " " );
			m_listCtrlSchdList.SetItemText( _nIndex, 28, " " );
		}
	}
	
	//ex int stand
	{
		if (_flight.GetExIntStandTime() >= 0)
		{
			CString strExIntStandDay;
			ElapsedTime eExIntStandTime;
			eExIntStandTime.setPrecisely(_flight.GetExIntStandTime());

			//Day
			strExIntStandDay.Format("%d", eExIntStandTime.GetDay());
			m_listCtrlSchdList.SetItemText( _nIndex, 29, strExIntStandDay );

			//Time
			outTime.SetTime(eExIntStandTime.GetHour(), eExIntStandTime.GetMinute(), eExIntStandTime.GetSecond());
			m_listCtrlSchdList.SetItemText( _nIndex, 30, outTime.Format( _T("%X") ) );
		}
		else
		{
			m_listCtrlSchdList.SetItemText( _nIndex, 29, " " );
			m_listCtrlSchdList.SetItemText( _nIndex, 30, " " );
		}
		
	}
	
}

void CFlightScheduleDlg::OnDblclkListSchedulelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint Pt = GetMessagePos();
	m_listCtrlSchdList.ScreenToClient(&Pt);
	UINT uFlags;
	int nHitItem = m_listCtrlSchdList.HitTest(Pt, &uFlags);
	if( nHitItem > -1 )
	{
		OnButtonEdit();
	}

	*pResult = 0;
}

void CFlightScheduleDlg::OnClickListSchedulelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint Pt = GetMessagePos();
	m_listCtrlSchdList.ScreenToClient(&Pt);
	UINT uFlags;
	int nHitItem = m_listCtrlSchdList.HitTest(Pt, &uFlags);
	if( nHitItem > -1 )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_DEL );	
		m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_EDIT );

		int nIndex = GetSelIndex();
		if( nIndex == -1 )
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_SPLIT ,FALSE);
			return;
		}

		Flight* pFlight = (Flight*)m_listCtrlSchdList.GetItemData( nIndex );
		if( pFlight->isTurnaround() )
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_SPLIT );
		}
		else
		{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_SPLIT ,FALSE);
		}
	}
	else
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_DEL ,FALSE);	
		m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_EDIT ,FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_SPLIT ,FALSE);
	}
	*pResult = 0;
}

InputTerminal* CFlightScheduleDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}

CString CFlightScheduleDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}

void CFlightScheduleDlg::OnButtonSaveas() 
{
	CFileDialog filedlg( FALSE,"skd", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"FlightSchedule File (*.skd)|*.skd;*.SKD|All type (*.*)|*.*|", NULL );
	if(filedlg.DoModal()!=IDOK)
		return;
	
	CString csFileName=filedlg.GetPathName();
	
	// unassign
	// call saveDataSetToOtherFile()
	GetInputTerminal()->flightSchedule->saveDataSetToOtherFile( csFileName );
	
}

void CFlightScheduleDlg::OnButtonLoadfrom() 
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	
	CImportFlightFromFileDlg dlg(GetInputTerminal(),GetProjPath(),pDoc->GetProjectID(),m_pParent);
	dlg.SetFlightScheduleDlg(TRUE);
	if( dlg.DoModal() == IDOK )
	{
		LoadFlight();
		m_btnSave.EnableWindow( TRUE );
	}
}

void CFlightScheduleDlg::OnButtonPrint() 
{
	// TODO: Add your control notification handler code here
	//Print();
	m_listCtrlSchdList.PrintList( "Flight Schedule", "Flight Schedule" );
}

void CFlightScheduleDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
		{
		CDialog::OnCancel();
		return;
	}
	
	AfxGetApp()->BeginWaitCursor();
	try
	{
		GetInputTerminal()->flightSchedule->loadDataSet( GetProjPath() );
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;			
	}
	AfxGetApp()->EndWaitCursor();
	CDialog::OnCancel();
}


int CFlightScheduleDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_FLIGHTSCHEDULEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}


void CFlightScheduleDlg::InitToolbar()
{
	CRect rc;
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	/*	CRect rcToolbar;
	m_wndToolBar.GetWindowRect( &rcToolbar );
	//	ScreenToClient(&rcToolbar);
	CRect ret;
	ret.left=rc.left+rc.Width()-rcToolbar.Width();
	ret.bottom = rc.bottom;
	ret.top = rc.top;
	ret.right =rc.right;
	ClientToScreen(&ret);*/
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_DEL ,FALSE);	
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_EDIT ,FALSE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_SPLIT ,FALSE);
	
	
	
}

BOOL CFlightScheduleDlg::InitFlowConvetor()
{
	m_paxFlowConvertor.SetInputTerm( GetInputTerminal() );
	m_allFlightFlow.ClearAll();
	
	return TRUE;
}

void CFlightScheduleDlg::UpdateFlow()
{
	//refresh the m_allFlightFlow
	CWaitCursor m_waitCursor;
	m_allFlightFlow.ClearAll();
	if(!m_paxFlowConvertor.BuildSpecFlightFlowFromGateAssign( m_allFlightFlow ))
		return;
	//m_allFlightFlow.PrintAllStructure();

	if( m_allFlightFlow.IfAllFlowValid( true ))
	{
		m_paxFlowConvertor.ToOldFlightFlow( m_allFlightFlow );
		GetInputTerminal()->m_pSpecificFlightPaxFlowDB->saveDataSet( GetProjPath() ,true );
	}
}

void CFlightScheduleDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if( m_butOK.m_hWnd == NULL )
		return;
	
	CRect rc;
	m_butCancel.GetWindowRect( &rc );

	m_butCancel.MoveWindow( cx - 10 - rc.Width(), cy-10-rc.Height(),rc.Width(),rc.Height());
	m_butOK.MoveWindow( cx - 10 - 2*rc.Width() - 15,cy-10-rc.Height(),rc.Width(),rc.Height() );
	m_btnSave.MoveWindow( cx - 10 - 3*rc.Width() - 2*15,cy-10-rc.Height(),rc.Width(),rc.Height() );
	m_butSaveAs.MoveWindow( cx - 10 - 4*rc.Width() - 3*15,cy-10-rc.Height(),rc.Width(),rc.Height() );

	CRect rcModified;
	m_btModifySchedule.GetWindowRect( rcModified );

	m_btModifySchedule.MoveWindow( cx - 10 - 5*rc.Width() - 5*15,cy-10-rc.Height(), rcModified.Width(),rcModified.Height() );
	
	m_butLoadFrom.SetWindowPos(NULL,10,cy-10-rc.Height(),0,0,SWP_NOSIZE);

	m_staticFrame.MoveWindow( 10,4,cx-20,cy-10-rc.Height()-16 );
	m_ToolBar.MoveWindow( 11,10,cx-21,26 );
	m_listCtrlSchdList.MoveWindow(10,36,cx-20,cy-10-rc.Height()-20-26);
	
	InvalidateRect(NULL);
}

void CFlightScheduleDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default

	lpMMI->ptMinTrackSize.x = 600;
	lpMMI->ptMinTrackSize.y = 400;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

BOOL CFlightScheduleDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;
}

void CFlightScheduleDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	HWND hwndT=::GetWindow(m_hWnd, GW_CHILD);
	CRect rectCW;
	CRgn rgnCW;
	rgnCW.CreateRectRgn(0,0,0,0);
	while (hwndT != NULL)
	{
		CWnd* pWnd=CWnd::FromHandle(hwndT)  ;
		if( pWnd->IsKindOf(RUNTIME_CLASS(CListCtrl)) )
		{
			if(!pWnd->IsWindowVisible())
			{
				hwndT=::GetWindow(hwndT,GW_HWNDNEXT);
				continue;
			}
			pWnd->GetWindowRect(rectCW);
			ScreenToClient(rectCW);
			CRgn rgnTemp;
			rgnTemp.CreateRectRgnIndirect(rectCW);
			rgnCW.CombineRgn(&rgnCW,&rgnTemp,RGN_OR);
		}
		hwndT=::GetWindow(hwndT,GW_HWNDNEXT);
		
	}
	CRect rect;
	GetClientRect(&rect);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);
	CRgn rgnDraw;
	rgnDraw.CreateRectRgn(0,0,0,0);
	rgnDraw.CombineRgn(&rgn,&rgnCW,RGN_DIFF);
	CBrush br(GetSysColor(COLOR_BTNFACE));
	dc.FillRgn(&rgnDraw,&br);
	CRect rectRight=rect;
	rectRight.left=rectRight.right-10;
	dc.FillRect(rectRight,&br);
	rectRight=rect;
	rectRight.top=rect.bottom-44;
	dc.FillRect(rectRight,&br);
}

void CFlightScheduleDlg::OnButtonSplit() 
{
	int nIndex = GetSelIndex();
	if( nIndex == -1 )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_SPLIT ,FALSE);
		return;
	}

	Flight* pFlight = (Flight*)m_listCtrlSchdList.GetItemData( nIndex );
	if( pFlight->isTurnaround() )
	{
		Flight* pDepFlight = new Flight( (Terminal*)GetInputTerminal() );

		char sAirline[AIRLINE_LEN];
		pFlight->getAirline( sAirline );

		pDepFlight->setAirline( sAirline );

		pDepFlight->setDepID( pFlight->getDepID() );
		pDepFlight->setDepTime( pFlight->getDepTime() );
		pDepFlight->setDepLFInput( pFlight->getDepLFInput() );

		char sDes[AIRPORT_LEN];
		pFlight->getDepStopover( sDes );
		pDepFlight->setStopoverAirport( sDes );

		pFlight->getDestination( sDes );
		pDepFlight->setAirport( sDes );

		char sAcType[AC_TYPE_LEN];
		pFlight->getACType( sAcType );
		pDepFlight->setACType( sAcType );
		pDepFlight->setCapacityInput( pFlight->getCapacityInput() );

		pDepFlight->setServiceTime( ElapsedTime(30*60l) );// 30 min is default
		pFlight->setServiceTime( ElapsedTime(30*60l) );// 30 min is default

		pDepFlight->setDepGateIndex( pFlight->getDepGateIndex() );
		pDepFlight->setDepStandID( pFlight->getDepStandID() );
		pDepFlight->setDepGate( pFlight->getDepGate() );
		pDepFlight->setDepStand( pFlight->getDepStand() );

		ProcessorID gate;
		gate.SetStrDict( GetInputTerminal()->inStrDict );
		pDepFlight->setArrGate( gate );
		pFlight->setDepGate( gate );	

		ALTObjectID stand;
		pFlight->setDepStand(stand);
		pDepFlight->setArrStand(stand);
		pFlight->setIntermediateStand(stand);

		pFlight->setDepID( "" );
		ElapsedTime gateTime;
		gateTime.set( 0l );
		pFlight->setDepTime( gateTime );
		pFlight->setDepGateIndex(-1);
		pFlight->setDepStandID(-1);


		// add into database.
		GetInputTerminal()->flightSchedule->addItem( pDepFlight );

		LoadFlight( pFlight );	

		if( m_iPreSortByColumnIdx >= 0 )
		{
			/*CWaitCursor	wCursor;
			m_ctlHeaderCtrl.SortColumn( m_iPreSortByColumnIdx, SINGLE_COLUMN_SORT );
			m_ctlHeaderCtrl.SortColumn( m_iPreSortByColumnIdx, SINGLE_COLUMN_SORT );
			m_ctlHeaderCtrl.SaveSortList();*/
		}	
		m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_DEL ,FALSE);	
		m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_EDIT ,FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_SPLIT ,FALSE);
		m_btnSave.EnableWindow();

	}


	m_ToolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_SPLIT ,FALSE);

}

void CFlightScheduleDlg::OnButtonModifySchdule() 
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	
	CFlightScheduleAddNewDlg dlg((InputTerminal*)&pDoc->GetTerminal(), pDoc );
	if( dlg.DoModal() == IDOK )
	{
		LoadFlight();
	}
	
}
