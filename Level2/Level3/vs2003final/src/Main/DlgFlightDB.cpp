// DlgFlightDB.cpp : implementation file
//

#include "stdafx.h"
#include "../MFCExControl/SortableHeaderCtrl.h"
#include "PrintableListCtrl.h"
#include "TermPlan.h"
#include "engine\terminal.h"
#include "inputs\flight.h"
#include "..\common\FlightManager.h"
#include "common\airportdatabase.h"
#include "DlgFlightCondition.h"
#include "DlgFlightDB.h"
#include ".\dlgflightdb.h"
#include "../Common/AirportDatabaseList.h"
#include "../Database/ARCportADODatabase.h"
#include "DlgSaveWarningWithCheckBox.h"
#include "ImportFlightGroupsDlg.h"
// CDlgFlightDB dialog
static const char* columnLabel[] = {	
	"Airline", 
		"Arr ID", 
		"Origin",
		"Arr Stopover",
		"Day",
		"Time",
		"Dep ID",
		"Dep Stopover",
		"Destination",
		"Day",
		"Time",
		"Ac Type",
		"Stand Gate",
		"Gate Occupancy(Min)",
		"Arr Gate",
		"Dep Gate",
		"BagDevice"
};
IMPLEMENT_DYNAMIC(CDlgFlightDB, CDialog)
CDlgFlightDB::CDlgFlightDB(int nProjID,Terminal* _pTerm,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFlightDB::IDD, pParent)
{
	m_bDragging				= FALSE;
	m_pDragImage			= NULL;
	m_nItemDragSrc			= -1;
	m_nItemDragDes			= -1;
	m_ptLastMouse			=CPoint(-1,-1);
	m_nRClickItem           = -1;
	m_nProjID				= nProjID;

	if( _pTerm )
	{
		m_pFlightMan = _pTerm->m_pAirportDB->getFlightMan();
		m_pTerm = _pTerm;
	}
	m_IsEdit = FALSE ;
	m_loadOpr = FALSE ;
}

CDlgFlightDB::~CDlgFlightDB()
{
	for(size_t i = 0; i < m_vFilter.size(); i++)
	{
		delete m_vFilter[i];
	}
	m_vFilter.clear();
}

void CDlgFlightDB::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FLIGHT,m_listCtrlSchdList);
	DDX_Control(pDX,IDC_LIST_FLIGHT_DATA,m_wndListCtrl);
	DDX_Control(pDX,IDC_LIST_GROUPS,m_wndListBox);
	DDX_Control(pDX, IDC_BUTTON_LOAD, m_Load);
}


BEGIN_MESSAGE_MAP(CDlgFlightDB, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT,OnBnClickedImport)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT,OnBnClickedExport)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_FLIGHT, OnBegindragLstFlight)
	ON_LBN_SELCHANGE(IDC_LIST_GROUPS, OnLbnSelchangeListGroups)
	ON_COMMAND(ID_TOOLBAR_ADD,OnToolbarAdd)
	ON_COMMAND(ID_TOOLBAR_EDIT,OnToolbarEdit)
	ON_COMMAND(ID_TOOLBAR_DEL,OnToolbarDel)
	ON_COMMAND(ID_AIRPORT_SAVEAS, OnSaveAsTemplate)
	ON_COMMAND(ID_AIRPORT_LOAD, OnLoadFromTemplate)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FLIGHT_DATA, OnLvnItemchangedListFlightData)
END_MESSAGE_MAP()

void CDlgFlightDB::LoadFromTemplateDatabase()
{
	CAirportDatabase* projectDB = m_pTerm->m_pAirportDB ;
	CAirportDatabase* DefaultDB = AIRPORTDBLIST->getAirportDBByName(projectDB->getName());
	if(DefaultDB == NULL)
	{
		CString msg ;
		msg.Format("%s AirportDB is not Exist in AirportDB template.",projectDB->getName()) ;
		MessageBox(msg,"Warning",MB_OK) ;
		return ;
	}
	//m_pFlightMan->ReadDataFromDB(DefaultDB->GetID(),DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;
	m_pFlightMan->loadDatabase(DefaultDB);
	CString msg ;
	msg.Format("Load %s from %s AirportDB template DataBase successfully.",DefaultDB->getName(),DefaultDB->getName()) ;
	MessageBox(msg,NULL,MB_OK) ;

	m_loadOpr = TRUE ;
	m_IsEdit = FALSE ;

	LoadFlight(NULL);
	PopulateGroupsList();
	m_wndListBox.SetCurSel(0);
	PopulateFlightList(0);
}
// CDlgFlightDB message handlers
BOOL CDlgFlightDB::OnInitDialog()
{
	CDialog::OnInitDialog();
	OnInitToolbar();
	// TODO: Add extra initialization here
	AfxGetApp()->BeginWaitCursor();	
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
    OnInitToolbar();
	// init list ctrl
	DWORD dwStyle = m_listCtrlSchdList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listCtrlSchdList.SetExtendedStyle( dwStyle );
	int DefaultColumnWidth[] = { 75, 75, 75, 100, 75, 75, 75, 100, 75, 75, 75, 75, 80,75,80,80,80};
	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER,
		LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_LEFT,LVCFMT_CENTER,LVCFMT_LEFT,LVCFMT_LEFT,LVCFMT_LEFT};

	m_listCtrlSchdList.SubClassHeadCtrl();
	for( int i=0; i<17; i++ )
	{
		m_listCtrlSchdList.InsertColumn( i, columnLabel[i], nFormat[i], DefaultColumnWidth[i] );
		m_listCtrlSchdList.SetDataType( i, CSortAndPrintListCtrl::ITEM_STRING );
		if(i==1 || i==6 ) m_listCtrlSchdList.SetDataType(i,CSortAndPrintListCtrl::ITEM_INT);
		if(i==4 || i==5 || i==9 || i==10 || i==15) m_listCtrlSchdList.SetDataType(i,CSortAndPrintListCtrl::ITEM_TIME);
	}

	LoadFlight(NULL);
	CRect listrect;
	m_wndListCtrl.GetClientRect(&listrect);
	dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );
	m_wndListCtrl.SubClassHeadCtrl();
	m_wndListCtrl.GetClientRect(&listrect);
	m_wndListCtrl.InsertColumn(0, "Airline", LVCFMT_LEFT, listrect.Width()/3);
	m_wndListCtrl.InsertColumn(1, "ID", LVCFMT_LEFT, listrect.Width()/3);
	m_wndListCtrl.InsertColumn(2, "Day",LVCFMT_LEFT, listrect.Width()/3);
//	m_wndListCtrl.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	m_wndListCtrl.SetDataType(0,CSortAndPrintListCtrl::ITEM_STRING);
	m_wndListCtrl.SetDataType(1,CSortAndPrintListCtrl::ITEM_INT);
	m_wndListCtrl.SetDataType(2,CSortAndPrintListCtrl::ITEM_INT);

	PopulateGroupsList();
	if (m_wndListBox.GetCount()>0)
	{
		m_wndListBox.SetCurSel(0);
		PopulateFlightList(0);
	}
	//	Init Cool button
	m_Load.SetOperation(0);
	m_Load.SetWindowText(_T("Database Template"));
	m_Load.SetOpenButton(FALSE);
	m_Load.SetType(CCoolBtn::TY_FLIGHTGROUP);

 	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
 	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgFlightDB::AppendOneFlightIntoListCtrl( const Flight* _pFlight ,int _nIndex)
{

	int nIndex = m_listCtrlSchdList.GetItemCount();
	char str[128];
	_pFlight->getAirline( str );
	//	m_listCtrlSchdList.InsertItem( nIndex, str );
	SetFlightInListCtrl( nIndex, *_pFlight );
	return nIndex;
}

void CDlgFlightDB::SetFlightInListCtrl( int _nIndex, const Flight& _flight )
{
	char str[128];
	CString strProcID;

	COleDateTime outTime;

	if( _flight.isArriving())
	{
		FlightGroup::CFlightGroupFilter* pFliter = new FlightGroup::CFlightGroupFilter();
		CString szFliter = _T("");
		_flight.getAirline( str );
		szFliter.Format(_T("%s-"),str);
		m_listCtrlSchdList.InsertItem( _nIndex, str );
		_flight.getArrID( str );
		szFliter += str;
		
		m_listCtrlSchdList.SetItemText( _nIndex, 1, str );

		_flight.getOrigin( str );
		m_listCtrlSchdList.SetItemText( _nIndex, 2, str );

		_flight.getArrStopover( str );
		m_listCtrlSchdList.SetItemText( _nIndex, 3, str );

		CString strDay;
		ElapsedTime etDayTime = _flight.getArrTime();
		strDay.Format("%d", etDayTime.GetDay());
		m_listCtrlSchdList.SetItemText( _nIndex, 4, strDay );
		szFliter += _T("-") + strDay;
		pFliter->SetFilter(szFliter);
		outTime.SetTime(etDayTime.GetHour(), etDayTime.GetMinute(), etDayTime.GetSecond());
		m_listCtrlSchdList.SetItemText( _nIndex, 5, outTime.Format( _T("%X") ) );

		m_listCtrlSchdList.SetItemText( _nIndex, 6, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 7, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 8, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 9, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 10, " " );

		_flight.getACType( str );
		m_listCtrlSchdList.SetItemText( _nIndex, 11, str );

		strProcID = _flight.getArrStand().GetIDString();
		if( strProcID == "All Processors" )
			strProcID = " ";
		m_listCtrlSchdList.SetItemText( _nIndex, 12, strProcID );

		ElapsedTime time;
		if( _flight.isTurnaround())
			time = _flight.getDepTime() - _flight.getArrTime();
		else
			time = _flight.getServiceTime();
		time.printTime( str, 0 );
		m_listCtrlSchdList.SetItemText( _nIndex, 13, str );

		strProcID = _flight.getArrGate().GetIDString();
		if( strProcID == "All Processors" )
			strProcID = " ";
		m_listCtrlSchdList.SetItemText( _nIndex, 14,strProcID );

		strProcID = _flight.getDepGate().GetIDString();
		if( strProcID == "All Processors" )
			strProcID = " ";
		m_listCtrlSchdList.SetItemText( _nIndex, 15, strProcID );
		strProcID = _flight.getBaggageDevice().GetIDString();
		if(strProcID =="All Processors")
			strProcID =" ";
		m_listCtrlSchdList.SetItemText(_nIndex,16,strProcID);
		m_listCtrlSchdList.SetItemData(_nIndex,(DWORD_PTR)pFliter);
		m_vFilter.push_back(pFliter);
		_nIndex++;
	}

	if( _flight.isDeparting() )
	{
		FlightGroup::CFlightGroupFilter* pFliter = new FlightGroup::CFlightGroupFilter();
		_flight.getAirline( str );
		CString szFliter = _T("");
		szFliter.Format(_T("%s-"),str);
		m_listCtrlSchdList.InsertItem( _nIndex, str );
		m_listCtrlSchdList.SetItemText( _nIndex, 1, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 2, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 3, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 4, " " );
		m_listCtrlSchdList.SetItemText( _nIndex, 5, " " );

		_flight.getDepID( str );
		szFliter += str;
		
		m_listCtrlSchdList.SetItemText( _nIndex, 6, str );
		_flight.getDepStopover( str );
		m_listCtrlSchdList.SetItemText( _nIndex, 7, str );
		_flight.getDestination( str );
		m_listCtrlSchdList.SetItemText( _nIndex, 8, str );

		CString strDay;
		ElapsedTime etDayTime = _flight.getDepTime();
		strDay.Format("%d", etDayTime.GetDay());
		m_listCtrlSchdList.SetItemText( _nIndex, 9, strDay );
		szFliter += _T("-") + strDay;
		pFliter->SetFilter(szFliter);
		outTime.SetTime(etDayTime.GetHour(), etDayTime.GetMinute(), etDayTime.GetSecond());
		m_listCtrlSchdList.SetItemText( _nIndex, 10, outTime.Format( _T("%X") ) );

		_flight.getACType( str );
		m_listCtrlSchdList.SetItemText( _nIndex, 11, str );

		strProcID = _flight.getDepStand().GetIDString();
		if( strProcID == "All Processors" )
			strProcID = " ";
		m_listCtrlSchdList.SetItemText( _nIndex, 12, strProcID );

		ElapsedTime time;
		if( _flight.isTurnaround())
			time = _flight.getDepTime() - _flight.getArrTime();
		else
			time = _flight.getServiceTime();
		time.printTime( str, 0 );
		m_listCtrlSchdList.SetItemText( _nIndex, 13, str );

		strProcID = _flight.getArrGate().GetIDString();
		if( strProcID == "All Processors" )
			strProcID = " ";
		m_listCtrlSchdList.SetItemText( _nIndex, 14,strProcID );

		strProcID = _flight.getDepGate().GetIDString();
		if( strProcID == "All Processors" )
			strProcID = " ";
		m_listCtrlSchdList.SetItemText( _nIndex, 15, strProcID );
		strProcID = _flight.getBaggageDevice().GetIDString();
		if(strProcID =="All Processors")
			strProcID =" ";
		m_listCtrlSchdList.SetItemText(_nIndex,16,strProcID);
		m_listCtrlSchdList.SetItemData(_nIndex,(DWORD_PTR)pFliter);
		m_vFilter.push_back(pFliter);
	}
}

void CDlgFlightDB::LoadFlight(Flight* _pSelectFlight/*=NULL*/)
{
	m_listCtrlSchdList.DeleteAllItems();
	Flight *pFlight;
	int count = m_pTerm->flightSchedule->getCount();
	int iSelect = -1;
	for (int i = 0; i < count; i++)
	{
		pFlight = m_pTerm->flightSchedule->getItem (i);
		if( pFlight == _pSelectFlight )
		{
			iSelect= AppendOneFlightIntoListCtrl( pFlight, i );
		}
		else
		{
			AppendOneFlightIntoListCtrl( pFlight, i );
		}
	}
	SetFlightSelected( iSelect );
}

void CDlgFlightDB::SetFlightSelected(int _nIndex)
{
	for( int i=m_listCtrlSchdList.GetItemCount()-1; i>-1; i-- )
	{
		if( i == _nIndex )
			m_listCtrlSchdList.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
		else
			m_listCtrlSchdList.SetItemState( i, 0, LVIS_SELECTED );
	}
}

void CDlgFlightDB::OnLoadFromTemplate()
{
	m_Load.SetOperation(0);
	m_Load.SetWindowText(_T("Load from Template"));
	LoadFromTemplateDatabase();
}

void CDlgFlightDB::OnSaveAsTemplate()
{
	m_Load.SetOperation(1);
	m_Load.SetWindowText(_T("Save as Template"));

	CAirportDatabase* DefaultDB = AIRPORTDBLIST->getAirportDBByName(m_pTerm->m_pAirportDB->getName());
	if(DefaultDB == NULL)
	{
		CString strError(_T(""));
		strError.Format(_T("%s AirportDB is not Exist in AirportDB template."),m_pTerm->m_pAirportDB->getName());
		MessageBox(strError,"Warning",MB_OK);
		return ;
	}

	m_pTerm->m_pAirportDB->saveAsTemplateDatabase(DefaultDB);
}

void CDlgFlightDB::OnClickMultiBtn()
{
	int type = m_Load.GetOperation() ;
	if (type == 0)
		OnLoadFromTemplate();
	if(type == 1)
		OnSaveAsTemplate();
}

void CDlgFlightDB::PopulateGroupsList()
{
	FLIGHTGROUPLIST	*pvSList = m_pFlightMan->GetFlightGroups();
	if(!pvSList)
		return; //bail if no list

	m_wndListBox.ResetContent();
	int nCurItem = 0;
	for (FLIGHTGROUPLIST::const_iterator iter = pvSList->begin();
		iter != pvSList->end();
		iter++)
	{
		FlightGroup* pGroups= *iter;
		m_wndListBox.SetItemData(m_wndListBox.InsertString(nCurItem, pGroups->getGroupName()), (DWORD)pGroups);
		nCurItem ++;
	}
	if (m_wndListBox.GetCount()>0)
	{
		m_wndListBox.SetCurSel(0);
		PopulateFlightList(0);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,TRUE);
	}
}
void CDlgFlightDB::OnLbnSelchangeListGroups()
{
	// TODO: Add your control notification handler code here
	int nCurIndex = m_wndListBox.GetCurSel();
	m_wndListCtrl.DeleteAllItems();
	FlightGroup* pGroups = reinterpret_cast<FlightGroup*>(m_wndListBox.GetItemData(nCurIndex));
	PopulateFlightList(nCurIndex);
	if (m_wndListBox.GetCount() == 0 || nCurIndex == -1)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
	}
	else
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
	}
}

int CDlgFlightDB::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_FLIGHTMANAGER_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

void CDlgFlightDB::OnInitToolbar()
{
	CRect rc;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_wndToolBar.MoveWindow(&rc,true);
	m_wndToolBar.ShowWindow( SW_SHOW );
}


 void CDlgFlightDB::AddDraggedItemToList()
 {
 	int nSectorItem = m_wndListBox.GetCurSel();
	if (nSectorItem == LB_ERR)
	{
		return;
	}
 	FlightGroup* pGroup = (FlightGroup*) m_wndListBox.GetItemData(nSectorItem);
	ASSERT(pGroup);
  	POSITION pos = m_listCtrlSchdList.GetFirstSelectedItemPosition();
  	while(pos)
  	{
		int nAirportItem = m_listCtrlSchdList.GetNextSelectedItem(pos);
 		FlightGroup::CFlightGroupFilter* pFilter = (FlightGroup::CFlightGroupFilter*)(m_listCtrlSchdList.GetItemData(nAirportItem));
 		ASSERT(pFilter);
 		FlightGroup::CFlightGroupFilter* pGroupFilter = new FlightGroup::CFlightGroupFilter(*pFilter);
  		CString sFlightName;
 		for (size_t i = 0; i < pGroup->GetFilterCount();i++)
 		{
 			if (*(pGroup->GetFilterByIndex(i)) == *pGroupFilter)
 			{
 				::AfxMessageBox(_T("The Flight is exist..!"));
 				return;
 			}
 		}
  		pGroup->AddFilter(pGroupFilter);
  	}
    m_IsEdit = TRUE ;
 	PopulateFlightList( nSectorItem );
 }

void CDlgFlightDB::PopulateFlightList(int _nItem)
{
	int nItem = _nItem;
	m_wndListBox.SetCurSel(_nItem);
	ASSERT( nItem == _nItem );
	m_wndListCtrl.DeleteAllItems();

	if (m_wndListBox.GetCount() != 0)
	{
		FlightGroup* pGroup = reinterpret_cast<FlightGroup*>(m_wndListBox.GetItemData(nItem));
		ASSERT(pGroup != NULL);

		char str[128];
		CString strDay = _T("");
		ElapsedTime etDayTime;
		int nIndex = 0;
		if (m_pTerm->flightSchedule->getCount() != 0)
		{
			for (int i = 0; i < m_pTerm->flightSchedule->getCount();i++)
			{
				Flight* pFlight = m_pTerm->flightSchedule->getItem(i);

				if (pFlight->isArriving())
				{
					if (pFlight->isBelongToGroup(pGroup,true))
					{
						pFlight->getAirline(str);
						m_wndListCtrl.InsertItem(nIndex,str);

						pFlight->getArrID(str);
						m_wndListCtrl.SetItemText(nIndex,1,str);

						etDayTime = pFlight->getArrTime();
						strDay.Format("%d", etDayTime.GetDay());
						m_wndListCtrl.SetItemText(nIndex,2,strDay);

						nIndex++;
					}
				}
				if (pFlight->isDeparting())
				{
					if (pFlight->isBelongToGroup(pGroup,false))
					{
						pFlight->getAirline(str);
						m_wndListCtrl.InsertItem(nIndex,str);

						pFlight->getDepID(str);
						m_wndListCtrl.SetItemText(nIndex,1,str);

						etDayTime = pFlight->getDepTime();
						strDay.Format("%d", etDayTime.GetDay());
						m_wndListCtrl.SetItemText(nIndex,2,strDay);
						nIndex++;
					}
				}
			}
		}
	}
}

BOOL CDlgFlightDB::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CDlgFlightDB::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	HWND hwndT=::GetWindow(m_hWnd, GW_CHILD);
	CRect rectCW;
	CRgn rgnCW;
	rgnCW.CreateRectRgn(0,0,0,0);
	while (hwndT != NULL)
	{
		CWnd* pWnd=CWnd::FromHandle(hwndT)  ;
		if(pWnd->IsKindOf(RUNTIME_CLASS(CListCtrl))
			|| pWnd->IsKindOf(RUNTIME_CLASS(CListBox)))
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
;
	CDialog::OnPaint();
}

void CDlgFlightDB::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType,cx,cy);

	if (m_listCtrlSchdList.GetSafeHwnd() == NULL)
		return;

	int nMargin=7;
	int nMargin2=nMargin*2;
	CRect rc, rc2;
	CRect btnrc;
	GetDlgItem(IDC_STATIC_FLIGHT)->SetWindowPos(NULL,nMargin,15,cx-nMargin2,20,NULL);
	GetDlgItem(IDC_STATIC_FLIGHT)->GetWindowRect(&rc);
	ScreenToClient(&rc);

	int nHeightTop=cy/3;
	m_listCtrlSchdList.SetWindowPos( NULL,rc.left,rc.bottom,rc.Width(),nHeightTop,NULL);
	m_listCtrlSchdList.GetWindowRect(&rc);
	ScreenToClient(&rc);

	GetDlgItem(IDC_STATIC_GROUP)->GetClientRect(btnrc);
	int nHeightWnd=btnrc.Height();

	GetDlgItem(IDC_STATIC_GROUP)->SetWindowPos(NULL,rc.left,rc.bottom,rc.Width(),nHeightWnd,NULL);
	GetDlgItem(IDC_STATIC_GROUP)->GetWindowRect(&rc);
	ScreenToClient(&rc);

	m_wndToolBar.SetWindowPos(NULL,rc.left,rc.bottom+1,rc.Width(),26,NULL);
	m_wndToolBar.GetWindowRect(&rc);
	ScreenToClient(&rc);

	m_wndListBox.SetWindowPos(NULL, rc.left, rc.bottom, rc.Width()/4, cy-38-nMargin-rc.bottom, NULL);
	m_wndListBox.GetWindowRect( &rc2 );
	ScreenToClient(&rc2);

	m_wndListCtrl.SetWindowPos(NULL, rc2.right+5, rc2.top, 3*(rc.Width()/4)-5, cy-38-nMargin-rc.bottom, NULL);
	m_wndListCtrl.GetWindowRect( &rc );
	ScreenToClient( &rc );


	GetDlgItem(IDC_STATIC_GROUP)->GetClientRect(btnrc);

	m_Load.GetWindowRect(&rc2) ;
	ScreenToClient(&rc2);
	
	
	GetDlgItem(IDOK)->GetWindowRect( &btnrc );
	GetDlgItem(IDOK)->MoveWindow( rc.right-btnrc.Width() ,cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	GetDlgItem(IDOK)->GetWindowRect(&btnrc);
	ScreenToClient(&btnrc);

	GetDlgItem(IDC_BUTTON_EXPORT)->MoveWindow(btnrc.left - btnrc.Width() - 5,cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height());
	GetDlgItem(IDC_BUTTON_IMPORT)->MoveWindow(btnrc.left - 2*btnrc.Width() - 10,cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height());
	m_Load.MoveWindow(btnrc.right - 3*btnrc.Width() - rc2.Width() - 18,cy-10-rc2.Height(), rc2.Width(), rc2.Height(), TRUE) ;
}

void CDlgFlightDB::OnToolbarAdd()
{
	FlightGroup* pGroup = new FlightGroup;
	pGroup->setGroupName("NewFlightGroup");
	CDlgFlightCondition dlg(m_pTerm,pGroup);
	if (dlg.DoModal() == IDOK)
	{	
		if (m_pFlightMan->CheckFlightGroup(pGroup) == FALSE)
		{
			pGroup->setGroupName(pGroup->getGroupName().MakeUpper());
			m_pFlightMan->AddFlightGroup(pGroup);
			PopulateGroupsList();
			PopulateFlightList(m_wndListBox.GetCount()-1);
		}
		else
		{
			CString strTemp = pGroup->getGroupName();
			CString sz;
			int nCount = strTemp.GetLength();
			int nIndex = 1;
			while (m_pFlightMan->CheckFlightGroup(pGroup) == TRUE)
			{
				strTemp = strTemp.Left(nCount);
	
				sz.Format("%s%d",strTemp,nIndex);
				sz.MakeUpper();
				pGroup->setGroupName(sz);
				nIndex++;
			}
			m_pFlightMan->AddFlightGroup(pGroup);
			PopulateGroupsList();
			PopulateFlightList(m_wndListBox.GetCount()-1);
		}
		 m_IsEdit = TRUE ;
	}
}

void CDlgFlightDB::OnToolbarDel()
{
	int nCurIndex = m_wndListBox.GetCurSel();
	if (nCurIndex == LB_ERR)
	{
		return;
	}
	FlightGroup* pGroup = (FlightGroup*)m_wndListBox.GetItemData(nCurIndex);
	if(AfxMessageBox(_T("Are you sure?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		m_pFlightMan->DelItem(pGroup);
		m_wndListBox.DeleteString(nCurIndex);
		 m_IsEdit = TRUE ;
		PopulateFlightList(0);
	}
	if (m_wndListBox.GetCount() == 0 || nCurIndex == -1)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
	}
	else
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
	}
}

void CDlgFlightDB::OnToolbarEdit()
{
	int nCurIndex = m_wndListBox.GetCurSel();
	if (nCurIndex == LB_ERR)
	{
		return;
	}
	FlightGroup* pGroup = (FlightGroup*)m_wndListBox.GetItemData(m_wndListBox.GetCurSel());
	CDlgFlightCondition dlg(m_pTerm,pGroup);
	CString strGroupName = _T("");
	int nCurCount = m_wndListBox.GetCurSel();
	if(IDOK == dlg.DoModal())
	{
		if (m_pFlightMan->CheckFlightGroup(pGroup) == TRUE)
		{
			 m_IsEdit = TRUE ;
			pGroup->setGroupName(pGroup->getGroupName().MakeUpper());
			m_wndListBox.GetText(nCurCount,strGroupName);
			if (strGroupName.CompareNoCase(pGroup->getGroupName())==0)
			{
				PopulateFlightList(m_wndListBox.GetCurSel());
				return;
			}
			else
			{
			    PopulateGroupsList();
				m_wndListBox.SetCurSel(nCurCount);
				PopulateFlightList(nCurCount);
			}
		}
	}
	
	if (m_wndListBox.GetCount() == 0 || nCurIndex == -1)
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,FALSE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
	}
	else
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
	}
}

void CDlgFlightDB::OnBnClickedOk()
{
	if(m_IsEdit)
	{
		try
		{
			CDatabaseADOConnetion::BeginTransaction(m_pTerm->m_pAirportDB->GetAirportConnection()) ;
			if(m_loadOpr)
			{
				m_pFlightMan->deleteDatabase(m_pTerm->m_pAirportDB) ;
				m_pFlightMan->ResetAllID() ;
				m_pFlightMan->saveDatabase(m_pTerm->m_pAirportDB) ;
			}else
				m_pFlightMan->saveDatabase(m_pTerm->m_pAirportDB);
			CDatabaseADOConnetion::CommitTransaction(m_pTerm->m_pAirportDB->GetAirportConnection()) ;
		}
		catch (CADOException& e)
		{
			e.ErrorMessage() ;
			CDatabaseADOConnetion::RollBackTransation(m_pTerm->m_pAirportDB->GetAirportConnection()) ;
		}
	}
	else if(m_loadOpr)
	{
		try
		{
			CDatabaseADOConnetion::BeginTransaction(m_pTerm->m_pAirportDB->GetAirportConnection()) ;
			m_pFlightMan->deleteDatabase(m_pTerm->m_pAirportDB);
			m_pFlightMan->ResetAllID() ;
			m_pFlightMan->saveDatabase(m_pTerm->m_pAirportDB);
			CDatabaseADOConnetion::CommitTransaction(m_pTerm->m_pAirportDB->GetAirportConnection()) ;
		}
		catch (CADOException& e)
		{
			e.ErrorMessage() ;
			CDatabaseADOConnetion::RollBackTransation(m_pTerm->m_pAirportDB->GetAirportConnection()) ;
		}
	}
	OnOK();
}

 void CDlgFlightDB::OnBegindragLstFlight(NMHDR* pNMHDR, LRESULT* pResult)
 {
 	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
 
 	m_nItemDragSrc = pNMListView->iItem;
 	m_nItemDragDes = -1;
 
 	if(m_pDragImage)
 		delete m_pDragImage;
 
 	CPoint ptTemp(0,0);
 	m_pDragImage = m_listCtrlSchdList.CreateDragImage(m_nItemDragSrc, &ptTemp);
 	if( !m_pDragImage )
 		return;
 
 	m_bDragging = true;
 	m_pDragImage->BeginDrag ( 0, CPoint(8,8) );
 	CPoint  pt = pNMListView->ptAction;
 	ClientToScreen( &pt );
 	m_pDragImage->DragEnter( GetDesktopWindow(), pt );  
 
 	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
 	SetCapture();
 
 	*pResult = 0;
 }

 void CDlgFlightDB::OnMouseMove(UINT nFlags, CPoint point)
 {
 	if( m_bDragging && m_ptLastMouse!=point )
 	{
 		CPoint pt (point);
 		ClientToScreen (&pt);
 
 		// move the drag image
 		VERIFY(m_pDragImage->DragMove(pt));
 		// unlock window updates
 		m_pDragImage->DragShowNolock(FALSE);
 		//////////////////////////////////////////////////////
 		CRect rcSM;
 		m_wndListCtrl.GetWindowRect( &rcSM );
 		if( rcSM.PtInRect( pt ) )	
 		{
 			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
 			CPoint ptTest( point );
 			ClientToScreen( &ptTest );
 			m_wndListBox.ScreenToClient(&ptTest);
 
 			// TRACE("x = %d, y = %d \r\n", ptTest.x, ptTest.y);
 
 			//destination item is the currently selected sector
 			m_nItemDragDes = m_wndListBox.GetCurSel();
 		}
 		else
 		{
 			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
 		}
 		m_pDragImage->DragShowNolock (TRUE);
 		m_wndListCtrl.Invalidate(FALSE);
 		m_ptLastMouse=point;
 	}
 
 	CDialog::OnMouseMove(nFlags, point);
 }
 
 void CDlgFlightDB::OnLButtonUp(UINT nFlags, CPoint point)
 {
 	if( m_bDragging )
 	{
 		CPoint pt (point);
 		ClientToScreen (&pt);
 
 		CRect rcSM;
 		m_wndListCtrl.GetWindowRect( &rcSM );
 		if( (m_nItemDragDes != -1) && rcSM.PtInRect( pt ) && m_wndListCtrl.IsWindowEnabled() )	
 		{
 			AddDraggedItemToList();
 		}
 
 		m_bDragging = false;
 		ReleaseCapture();
 		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
 		// end dragging
 		m_pDragImage->DragLeave( GetDesktopWindow() );
 		m_pDragImage->EndDrag();
 		delete m_pDragImage;
 		m_pDragImage = NULL;
 	}	
 	CDialog::OnLButtonUp(nFlags, point);
 }

 void CDlgFlightDB::OnBnClickedExport()
 {
	CFileDialog filedlg( FALSE,"frp", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"FlightGroup File (*.frp)|*.frp;*.fpr|All type (*.*)|*.*|", NULL );
	if(filedlg.DoModal()!=IDOK)
		return;

	CString csFileName=filedlg.GetPathName();

	// file title, header
	ArctermFile file;
	file.openFile(csFileName,WRITE);
	file.writeField("ARCTerm Flight Groups File");
	file.writeLine();
	file.writeField("Flight Groups");
	file.writeField("Airline");
	file.writeField("ID");
	file.writeField("Day");
	file.writeLine();

	char str[128];
	memset(str,0,128*sizeof(char));
	ElapsedTime etDayTime;
	CString strDay(_T(""));
	BOOL bFind = FALSE;
	//flight groups content
	for (int i = 0; i < (int)m_pFlightMan->GetFlightGroups()->size(); i++)
	{
		bFind = FALSE;
		FlightGroup* pFlightGroup = m_pFlightMan->getGroupItem(i);
	//	for (int j = 0; j < (int)pFlightGroup->GetFilterCount(); j++)
		{
			for (int nIdx = 0; nIdx < m_pTerm->flightSchedule->getCount();nIdx++)
			{
				Flight* pFlight = m_pTerm->flightSchedule->getItem(nIdx);
				if (pFlight->isArriving() && pFlight->isBelongToGroup(pFlightGroup,true))
				{
					file.writeField(pFlightGroup->getGroupName());
					pFlight->getAirline(str);
					file.writeField(str);
					file.writeField(pFlight->getArrID().GetValue());
					etDayTime = pFlight->getArrTime();
					strDay.Format("%d", etDayTime.GetDay());
					file.writeField(strDay);
					file.writeLine();
					bFind = TRUE;
				}

				if (pFlight->isDeparting() && pFlight->isBelongToGroup(pFlightGroup,false))
				{
					file.writeField(pFlightGroup->getGroupName());
					pFlight->getAirline(str);
					file.writeField(str);
					file.writeField(pFlight->getDepID().GetValue());
					etDayTime = pFlight->getDepTime();
					strDay.Format("%d", etDayTime.GetDay());
					file.writeField(strDay);
					file.writeLine();
					bFind = TRUE;
				}
			}
		}
		if (!bFind)
		{
			file.writeField(pFlightGroup->getGroupName());
			file.writeLine();
		}
	}
	file.endFile();
 }

 void CDlgFlightDB::OnBnClickedImport()
 {
	 CImportFlightGroupsDlg dlg(m_pTerm);
	 if(dlg.DoModal() == IDOK)
	 {
		 PopulateGroupsList();
		 m_IsEdit=true;
	 }
	
 }
 void CDlgFlightDB::OnLvnItemchangedListFlightData(NMHDR *pNMHDR, LRESULT *pResult)
 {
	 LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	 // TODO: Add your control notification handler code here
	 *pResult = 0;
 }
