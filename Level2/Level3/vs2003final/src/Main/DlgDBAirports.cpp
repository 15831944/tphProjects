// DlgDBAirports.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgDBAirports.h"
#include "DlgAirportProperties.h"
#include "..\common\AirportsManager.h"
#include "FltSchedDefineDlg.h"
#include "engine\terminal.h"
#include "common\airportdatabase.h"
#include "assert.h"
#include ".\dlgdbairports.h"
#include "../Common/AirportDatabaseList.h"
#include "../Database/ARCportADODatabase.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "DlgSaveWarningWithCheckBox.h"
static int GetSelectedItem( const CListCtrl& lst )
{
	POSITION pos = lst.GetFirstSelectedItemPosition();
	if (pos == NULL)
		return -1;

	return lst.GetNextSelectedItem(pos);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgDBAirports dialog


CDlgDBAirports::CDlgDBAirports(BOOL bMenu,Terminal* _pTerm, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDBAirports::IDD, pParent)
	, m_bDragging(FALSE)
	, m_pDragImage(NULL)
	, m_nItemDragSrc(-1)
	, m_nItemDragDes(-1)
	, m_ptLastMouse(-1, -1)
	, m_bMenu(bMenu)
	, m_CommitToDefault(FALSE)
	, m_Load_OPer(FALSE)
	, m_IsEdit(FALSE)
	, m_pTerm(_pTerm)
{
	if( m_pTerm  )
	{
		m_pCurAirportDatabase = m_pTerm->m_pAirportDB;
		ASSERT(m_pCurAirportDatabase);
		m_pAirportMan = m_pCurAirportDatabase->getAirportMan();
		ASSERT(m_pAirportMan);
	}
	else
	{
		// m_bMenu should not be FALSE when _pTerm is NULL
		ASSERT(TRUE == m_bMenu);
		// here should initialize m_pCurAirportDatabase and m_pAcMan
		m_pCurAirportDatabase = NULL;
		m_pAirportMan = NULL;
	}
}

CDlgDBAirports::~CDlgDBAirports()
{
	if (m_pDragImage)
	{
		delete m_pDragImage;
		m_pDragImage = NULL;
	}
}


void CDlgDBAirports::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDBAirports)
	DDX_Control(pDX, IDC_STATIC_LABLE, m_cstLable);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_LIST_AIRPORTS, m_lstAirports);
	DDX_Control(pDX, IDC_LIST_SECTORS, m_lstSectors);
	DDX_Control(pDX, IDC_LIST_SECTORMEMBERS, m_lstSectorMembers);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BUTTON_LOADDEFAULT_AIRPORT, m_button_Load);
	DDX_Control(pDX, IDCANCEL, m_ButCancel);
	DDX_Control(pDX, IDC_CHECK_FLIGHTSCHEDULE, m_btnCheck);
	//DDX_Control(pDX, IDC_STATIC_LOAD, m_StaticLoad);
}


BEGIN_MESSAGE_MAP(CDlgDBAirports, CDialog)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CTX_NEWAIRPORT, OnCtxNewAirport)
	ON_COMMAND(ID_CTX_EDITAIRPORT, OnCtxEditAirport)
	ON_COMMAND(ID_CTX_NEWSECTOR, OnCtxNewSector)
	ON_COMMAND(ID_CTX_ADDAIRPORTS, OnCtxAddAirports)
	ON_COMMAND(ID_CTX_DELETESECTOR, OnCtxDeleteSector)
	ON_COMMAND(ID_CTX_DELETEAIRPORT, OnCtxDeleteAirport)
	ON_COMMAND(ID_AIRPORT_SAVEAS, OnSaveAsTemplate)
	ON_COMMAND(ID_AIRPORT_LOAD, OnLoadFromTemplate)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_AIRPORTS, OnClickLstAirports)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_LBN_SELCHANGE(IDC_LIST_SECTORS, OnLbnSelchangeListSectors)
	ON_BN_CLICKED(IDC_BUTTON_LOADDEFAULT_AIRPORT, OnBnClickedButtonLoaddefaultAirport)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_AIRPORTS, OnBegindragLstAirports)
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_CHECK_FLIGHTSCHEDULE, OnBnClickedCheckFlightSchedule)
END_MESSAGE_MAP()


void CDlgDBAirports::OnBnClickedCheckFlightSchedule()
{
	m_bMenu = m_btnCheck.GetCheck()?FALSE:TRUE;
	PopulateAirportList();
	if (m_lstSectors.GetCount() > 0)
	{
		m_lstSectors.SetCurSel(0);
		OnLbnSelchangeListSectors();
	}
}

void CDlgDBAirports::OnPaint()
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

BOOL CDlgDBAirports::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CDlgDBAirports::OnBegindragLstAirports(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	m_nItemDragSrc = pNMListView->iItem;
	m_nItemDragDes = -1;

	if(m_pDragImage)
		delete m_pDragImage;

	CPoint ptTemp(0,0);
	m_pDragImage = m_lstAirports.CreateDragImage(m_nItemDragSrc, &ptTemp);
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

void CDlgDBAirports::OnMouseMove(UINT nFlags, CPoint point) 
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
		m_lstSectorMembers.GetWindowRect( &rcSM );
		if( rcSM.PtInRect( pt ) )	
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			CPoint ptTest( point );
			ClientToScreen( &ptTest );
			m_lstSectors.ScreenToClient(&ptTest);

			// TRACE("x = %d, y = %d \r\n", ptTest.x, ptTest.y);

			//destination item is the currently selected sector
			m_nItemDragDes = m_lstSectors.GetCurSel();
		}
		else
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		}
		m_pDragImage->DragShowNolock (TRUE);
		m_lstAirports.Invalidate(FALSE);
		m_ptLastMouse=point;
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CDlgDBAirports::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( m_bDragging )
	{
		CPoint pt (point);
		ClientToScreen (&pt);

		CRect rcSM;
		m_lstSectorMembers.GetWindowRect( &rcSM );
		if( (m_nItemDragDes != -1) && rcSM.PtInRect( pt ) && m_lstSectorMembers.IsWindowEnabled() )	
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


// Get an airport sector...
bool CDlgDBAirports::GetSector(const char *pszAirport, char *pszSector, 
                               unsigned short usSize) const
{
	// Variables...
    CSECTORLIST    *pvSList         = NULL;
    CSector        *pSector         = NULL;
    CAIRPORTLIST    AList;
    unsigned int    unSectorIndex   = 0;
    unsigned int    unAirportIndex  = 0;

    // Get the sector list...
	pvSList = m_pAirportMan->GetSectorList();

    // Check each sector...
	for(unSectorIndex = 0; unSectorIndex < pvSList->size(); unSectorIndex++) 
    {
		// Fetch a sector...
        pSector = (*pvSList)[unSectorIndex];

        // Get the airport list...
		pSector->GetAirportList(AList);
		
        // Scan sector for airport...
        for(unAirportIndex = 0; unAirportIndex < AList.size(); unAirportIndex++)
        {
            // Located...
            if(AList.at(unAirportIndex)->m_sIATACode == pszAirport)
            {
                // Return sector name...
                strncpy(pszSector, pSector->m_sName, usSize);
                return true;
            }
        }

        // Cleanup...
		AList.clear();
	}

    // Not found...
    return false;
}


void CDlgDBAirports::PopulateSectorList()
{
    CSECTORLIST	*pvSList = m_pAirportMan->GetSectorList();
	if(!pvSList)
		return; //bail if no list

	m_lstSectors.ResetContent();
	int nCurItem = 0;
	for (CSECTORLIST::const_iterator iter = pvSList->begin();
		iter != pvSList->end();
		iter++)
	{
		CSector* pSector = *iter;
		m_lstSectors.SetItemData(m_lstSectors.InsertString(nCurItem, pSector->m_sName), (DWORD)pSector);
		nCurItem ++;
	}
}
void CDlgDBAirports::AddAirportItem(CAirport* pAirport)
{
	int insertItem = m_lstAirports.GetItemCount() ;
	m_lstAirports.InsertItem(insertItem,pAirport->m_sIATACode);
	m_lstAirports.SetItemText(insertItem,1,pAirport->m_sICAOCode);
	m_lstAirports.SetItemText(insertItem,2,pAirport->m_sLongName);
	m_lstAirports.SetItemText(insertItem,3,pAirport->m_sARPCoordinates);
	m_lstAirports.SetItemText(insertItem,4,pAirport->m_sCity);
	m_lstAirports.SetItemText(insertItem,5,pAirport->m_sCountry);
	m_lstAirports.SetItemText(insertItem,6,pAirport->m_sCountryCode);
	m_lstAirports.SetItemText(insertItem,7,pAirport->m_sElevation);
	m_lstAirports.SetItemText(insertItem,8,pAirport->m_sNumberOfRunways);
	m_lstAirports.SetItemText(insertItem,9,pAirport->m_sMaximumRunwayLength);
	m_lstAirports.SetItemText(insertItem,10,pAirport->m_sAlternateAirportICAOCode);
	m_lstAirports.SetItemText(insertItem,11,_T(""));
	// Add to list control...
	m_lstAirports.SetItemData(insertItem,(DWORD)pAirport);
	if (pAirport->IsEdit())
	{
		m_lstAirports.SetItemColor(insertItem,::GetSysColor(COLOR_WINDOWFRAME),RGB(217,217,255));
	}
	pAirport->Update();
	m_IsEdit = TRUE ;
}
void CDlgDBAirports::EditAirportItem(int selItem ,CAirport* pAirport)
{
	m_lstAirports.SetItemText(selItem,0,pAirport->m_sIATACode);
	m_lstAirports.SetItemText(selItem,1,pAirport->m_sICAOCode);
	m_lstAirports.SetItemText(selItem,2,pAirport->m_sLongName);
	m_lstAirports.SetItemText(selItem,3,pAirport->m_sARPCoordinates);
	m_lstAirports.SetItemText(selItem,4,pAirport->m_sCity);
	m_lstAirports.SetItemText(selItem,5,pAirport->m_sCountry);
	m_lstAirports.SetItemText(selItem,6,pAirport->m_sCountryCode);
	m_lstAirports.SetItemText(selItem,7,pAirport->m_sElevation);
	m_lstAirports.SetItemText(selItem,8,pAirport->m_sNumberOfRunways);
	m_lstAirports.SetItemText(selItem,9,pAirport->m_sMaximumRunwayLength);
	m_lstAirports.SetItemText(selItem,10,pAirport->m_sAlternateAirportICAOCode);
	m_lstAirports.SetItemText(selItem,11,_T(""));
	// Add to list control...
	m_lstAirports.SetItemData(selItem,(DWORD)pAirport);
	if (pAirport->IsEdit())
	{
		m_lstAirports.SetItemColor(selItem,::GetSysColor(COLOR_WINDOWFRAME),RGB(217,217,255));
	}
	pAirport->Update();
	m_IsEdit = TRUE ;
}
// Populate airport list control...
void CDlgDBAirports::PopulateAirportList()
{
	m_lstAirports.DeleteAllItems();
	// Variables...
    CAIRPORTLIST   *pvList          = NULL;
    unsigned int    unIndex         = 0;
	CAirport       *pAirport        = NULL;
	int k = 0;
	char strOrigin[128];
	char strDest[128];
	// Get the airport list...
    pvList = m_pAirportMan->GetAirportList();

	CString strIATA,strICAO,strName,strARP,strCity,strCountry,strContryCode,strEle,strNum,strMax,strAlt,strSec;
	// Add each airport...
	if (m_bMenu)
	{
		for(unIndex = 0; unIndex < pvList->size(); unIndex++)
		{
			// Fetch an airport...
			pAirport = (*pvList)[unIndex];

			// Initialize...
			m_lstAirports.InsertItem(unIndex,pAirport->m_sIATACode);
			m_lstAirports.SetItemText(unIndex,1,pAirport->m_sICAOCode);
			m_lstAirports.SetItemText(unIndex,2,pAirport->m_sLongName);
			m_lstAirports.SetItemText(unIndex,3,pAirport->m_sARPCoordinates);
			m_lstAirports.SetItemText(unIndex,4,pAirport->m_sCity);
			m_lstAirports.SetItemText(unIndex,5,pAirport->m_sCountry);
			m_lstAirports.SetItemText(unIndex,6,pAirport->m_sCountryCode);
			m_lstAirports.SetItemText(unIndex,7,pAirport->m_sElevation);
			m_lstAirports.SetItemText(unIndex,8,pAirport->m_sNumberOfRunways);
			m_lstAirports.SetItemText(unIndex,9,pAirport->m_sMaximumRunwayLength);
			m_lstAirports.SetItemText(unIndex,10,pAirport->m_sAlternateAirportICAOCode);
			m_lstAirports.SetItemText(unIndex,11,_T(""));
			// Add to list control...
			m_lstAirports.SetItemData(unIndex,(DWORD)pAirport);
			if (pAirport->IsEdit())
			{
				m_lstAirports.SetItemColor(unIndex,::GetSysColor(COLOR_WINDOWFRAME),RGB(235,235,235));
			}
		}
	}
	else
	{
		for(unIndex = 0; unIndex < pvList->size(); unIndex++)
		{
			// Fetch an airport...
			pAirport = (*pvList)[unIndex];
			for (int j=0;j<m_pTerm->flightSchedule->getCount();j++)
			{
				// Initialize...
				Flight* pFlight = m_pTerm->flightSchedule->getItem(j);
				pFlight->getOrigin(strOrigin);
				pFlight->getDestination(strDest);
				if (strcmp(pAirport->m_sIATACode,strOrigin)==0||strcmp(pAirport->m_sIATACode,strDest)==0)
				{
					m_lstAirports.InsertItem(k,pAirport->m_sIATACode);
					m_lstAirports.SetItemText(k,1,pAirport->m_sICAOCode);
					m_lstAirports.SetItemText(k,2,pAirport->m_sLongName);
					m_lstAirports.SetItemText(k,3,pAirport->m_sARPCoordinates);
					m_lstAirports.SetItemText(k,4,pAirport->m_sCity);
					m_lstAirports.SetItemText(k,5,pAirport->m_sCountry);
					m_lstAirports.SetItemText(k,6,pAirport->m_sCountryCode);
					m_lstAirports.SetItemText(k,7,pAirport->m_sElevation);
					m_lstAirports.SetItemText(k,8,pAirport->m_sNumberOfRunways);
					m_lstAirports.SetItemText(k,9,pAirport->m_sMaximumRunwayLength);
					m_lstAirports.SetItemText(k,10,pAirport->m_sAlternateAirportICAOCode);
					m_lstAirports.SetItemText(k,11,_T(""));
		
					m_lstAirports.SetItemData(k,(DWORD)pAirport);
					if (pAirport->IsEdit())
					{
						m_lstAirports.SetItemColor(k,::GetSysColor(COLOR_WINDOWFRAME),RGB(235,235,235));
					}
					k++;
					break;

				}
			}
		}
	}
}

void CDlgDBAirports::PopulateSectorMemberList(int _nItem)
{
	int nItem = m_lstSectors.GetCurSel();
	ASSERT( nItem == _nItem );
	m_lstSectorMembers.DeleteAllItems();
	if (nItem < 0) return;

	CSector* pSector = reinterpret_cast<CSector*>(m_lstSectors.GetItemData(nItem));
	ASSERT(pSector != NULL);

	CAIRPORTLIST AList;
	pSector->GetAirportList(AList);
	char strOrigin[128];
	char strDest[128];
	int m = 0;
	CAirport *pAirport = NULL;
	CString strIATA,strICAO,strName;
	if (m_bMenu)
	{
		for (UINT k = 0; k < AList.size(); k++)
		{
			pAirport = AList[k];
			m_lstSectorMembers.InsertItem(k,pAirport->m_sIATACode);
			m_lstSectorMembers.SetItemText(k,1,pAirport->m_sICAOCode);
			m_lstSectorMembers.SetItemText(k,2,pAirport->m_sLongName);
			m_lstSectorMembers.SetItemData(k,(DWORD)AList[k]);
		}
	}
	else
	{

		for (UINT k = 0; k < AList.size(); k++)
		{
			pAirport = (AList)[k];
			for (int j=0;j<m_pTerm->flightSchedule->getCount();j++)
			{
				// Initialize...
				Flight* pFlight = m_pTerm->flightSchedule->getItem(j);
				pFlight->getOrigin(strOrigin);
				pFlight->getDestination(strDest);
				if (strcmp(pAirport->m_sIATACode,strOrigin)==0||strcmp(pAirport->m_sIATACode,strDest)==0)
				{
					m_lstSectorMembers.InsertItem(m,pAirport->m_sIATACode);
					m_lstSectorMembers.SetItemText(m,1,pAirport->m_sICAOCode);
					m_lstSectorMembers.SetItemText(m,2,pAirport->m_sLongName);
					m_lstSectorMembers.SetItemData(m,(DWORD)AList[m]);
					m++;
					break;
				}
			}
		}
	}
}

BOOL CDlgDBAirports::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_wndToolBarTop.GetToolBarCtrl().SetCmdID(0,ID_CTX_NEWAIRPORT);
	m_wndToolBarTop.GetToolBarCtrl().SetCmdID(1,ID_CTX_DELETEAIRPORT);
	m_wndToolBarTop.GetToolBarCtrl().SetCmdID(2,ID_CTX_EDITAIRPORT);
	m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_DELETEAIRPORT,FALSE);
	m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_EDITAIRPORT,FALSE);
	m_wndToolBarBottom.GetToolBarCtrl().SetCmdID(0,ID_CTX_NEWSECTOR);
	m_wndToolBarBottom.GetToolBarCtrl().SetCmdID(1,ID_CTX_DELETESECTOR);
	m_wndToolBarBottom.GetToolBarCtrl().SetCmdID(2,ID_CTX_ADDAIRPORTS);
	m_wndToolBarBottom.GetToolBarCtrl().EnableButton(ID_CTX_DELETESECTOR,FALSE);
	m_wndToolBarBottom.GetToolBarCtrl().EnableButton(ID_CTX_ADDAIRPORTS,FALSE);

	DWORD dwStyle = m_lstAirports.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_lstAirports.SetExtendedStyle( dwStyle );
	CRect listrect;
	m_lstAirports.GetClientRect(&listrect);
	m_lstAirports.InsertColumn(0, "IATA Code", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirports.InsertColumn(1, "ICAO Code", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirports.InsertColumn(2, "Airport Name", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirports.InsertColumn(3, "ARP coordinates", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirports.InsertColumn(4, "City", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirports.InsertColumn(5, "Country", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirports.InsertColumn(6, "Country code", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirports.InsertColumn(7, "Elevation", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirports.InsertColumn(8, "Number of runways", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirports.InsertColumn(9, "Maximum runway length", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirports.InsertColumn(10, "Alternate airport ICAO code", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirports.InsertColumn(11, "Sectors", LVCFMT_LEFT, listrect.Width()/5);

	m_lstAirports.SetDataType(0,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirports.SetDataType(1,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirports.SetDataType(2,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirports.SetDataType(3,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirports.SetDataType(4,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirports.SetDataType(5,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirports.SetDataType(6,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirports.SetDataType(7,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirports.SetDataType(8,CSortAndPrintListCtrl::ITEM_INT);
	m_lstAirports.SetDataType(9,CSortAndPrintListCtrl::ITEM_INT);
	m_lstAirports.SetDataType(10,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirports.SetDataType(11,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirports.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	m_lstAirports.SubClassHeadCtrl();
	//now populate the list
	PopulateAirportList();

	dwStyle = m_lstSectorMembers.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_lstSectorMembers.SetExtendedStyle( dwStyle );
	m_lstSectorMembers.GetClientRect(&listrect);
	m_lstSectorMembers.InsertColumn(0, "IATA Code", LVCFMT_LEFT, listrect.Width()/4);
	m_lstSectorMembers.InsertColumn(1, "ICAO Code", LVCFMT_LEFT, listrect.Width()/4);
	m_lstSectorMembers.InsertColumn(2, "Airport Name", LVCFMT_LEFT, listrect.Width()/2);
	m_lstSectorMembers.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

	m_lstSectorMembers.SetDataType(0,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstSectorMembers.SetDataType(1,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstSectorMembers.SetDataType(2,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstSectorMembers.SubClassHeadCtrl();
	PopulateSectorList();

	m_btnCheck.SetCheck(!m_bMenu);
	if (m_lstSectors.GetCount() > 0)
	{
		m_lstSectors.SetCurSel(0);
		OnLbnSelchangeListSectors();
	}
	
	m_button_Load.SetOperation(0);
	m_button_Load.SetWindowText(_T("Database Template"));
	m_button_Load.SetOpenButton(FALSE);
	m_button_Load.SetType(CCoolBtn::TY_AIRPORT);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgDBAirports::FreeItemMemory()
{
}

void CDlgDBAirports::OnDestroy() 
{
	FreeItemMemory();
	CDialog::OnDestroy();	
}

void CDlgDBAirports::OnCtxNewAirport() 
{
	int nCount = m_pAirportMan->GetAirportList()->size();
	CString sName;
	sName.Format("New Airport %d", nCount);
	CAirport* pA = new CAirport();
	pA->m_sLongName  = sName;
	CDlgAirportProperties dlg(pA);
	if(dlg.DoModal() == IDOK) {
		//check to make sure this airport doesn't already exist
		CAIRPORTFILTER apFilter;
		apFilter.sIATACode = pA->m_sIATACode;
		CAIRPORTLIST vListAP;
		m_pAirportMan->GetAirportsByFilter(apFilter, vListAP);
		if(vListAP.size() == 0) {
			m_pAirportMan->GetAirportList()->push_back(pA);
			AddAirportItem(pA) ;
			return;
		}
		else {
			CString msg;
			msg.Format("An airport with IATA code %s (%s) already exists.\nOperation cancelled.", vListAP[0]->m_sIATACode, vListAP[0]->m_sLongName);
			AfxMessageBox(msg);
		}
	}
	delete pA;
}

void CDlgDBAirports::OnCtxEditAirport() 
{
 	int sel = GetSelectedItem( m_lstAirports );
 
 	if(sel != -1) {
 		CAirport* pAirport = (CAirport*) m_lstAirports.GetItemData(sel);
		if(pAirport == NULL)
			return ;
		CString code = pAirport->m_sIATACode ;
 		CDlgAirportProperties dlg(pAirport);
 		if(dlg.DoModal() == IDOK) {
			CAIRPORTFILTER apFilter;
			apFilter.sIATACode = pAirport->m_sIATACode;
			CAIRPORTLIST vListAP;
			m_pAirportMan->GetAirportsByFilter(apFilter, vListAP);
			if(vListAP.size() <= 1) {
				if (pAirport->IsEdit())
				{
					pAirport->IsEdit(FALSE);
					pAirport->SetID(-1);
					if (pAirport->m_sIATACode != code)
					{
						CARCProjectDatabase* pAirportDatabase = (CARCProjectDatabase*)m_pCurAirportDatabase;
						CAirport* pArcAirport = pAirportDatabase->GetArcDatabaseAirport(code);
						if (pArcAirport)
						{
							CAirport* pData = new CAirport(*pArcAirport);
							pData->IsEdit(TRUE);
							m_pAirportMan->GetAirportList()->push_back(pData);
						}
					}
				}
				EditAirportItem(sel,pAirport) ;
				return;
			}
			else {
				CString msg;
				msg.Format("An airport with IATA code %s (%s) already exists.\nOperation cancelled.", vListAP[0]->m_sIATACode, vListAP[0]->m_sLongName);
				AfxMessageBox(msg);
				pAirport->m_sIATACode = code ;
			}
			PopulateAirportList();
 		}
 	}
}

void CDlgDBAirports::OnCtxNewSector() 
{
	int nCount = m_pAirportMan->GetSectorList()->size();
	CString sName;
	sName.Format("New Sector %d", nCount);
	CSector* pSector = new CSector( m_pAirportMan );
	pSector->m_sName = sName;
	CDlgSectorProperties dlg( m_pAirportMan, pSector);
	if(dlg.DoModal() == IDOK)
	{
		int nCurItem = m_lstSectors.InsertString(m_lstSectors.GetCount(), pSector->m_sName);
		m_lstSectors.SetItemData(nCurItem, reinterpret_cast<LPARAM>(pSector));

		m_pAirportMan->GetSectorList()->push_back(pSector);
		m_lstSectors.SetCurSel(m_lstSectors.GetCount() - 1);
		OnLbnSelchangeListSectors();
		m_IsEdit = TRUE ;
	}
	else 
	{
		delete pSector;
	}
}

void CDlgDBAirports::OnCtxAddAirports() //i.e. edit sector members - so "edit criteria"
{
	int nItem = m_lstSectors.GetCurSel();
	if (nItem == -1)
		return;

    CSector* pSector = reinterpret_cast<CSector*>(m_lstSectors.GetItemData(nItem));
	CDlgSectorProperties dlg( m_pAirportMan, pSector);
	if(dlg.DoModal() == IDOK)
	{
		PopulateSectorList();
		m_lstSectors.SetCurSel(nItem);
		PopulateSectorMemberList(nItem);
		m_IsEdit = TRUE ;
	}
}

void CDlgDBAirports::OnOK() 
{
	CAirportDatabase* projectDB = m_pTerm->m_pAirportDB ;
	CAirportDatabase* DefaultDB = AIRPORTDBLIST->getAirportDBByName(projectDB->getName());

   if(m_IsEdit)
   {
	   //save local data 
		try
		{
			CDatabaseADOConnetion::BeginTransaction(m_pCurAirportDatabase->GetAirportConnection()) ;
			if(m_Load_OPer)
			{
				projectDB->getAirportMan()->ReSetAllID() ;
				m_pCurAirportDatabase->getAirportMan()->deleteDatabase(m_pCurAirportDatabase) ;
				m_pCurAirportDatabase->getAirportMan()->ReSetAllID() ;
				m_pCurAirportDatabase->getAirportMan()->saveDatabase(m_pCurAirportDatabase) ;
			}else
			{
				m_pAirportMan->saveDatabase(m_pCurAirportDatabase);
			}
			CDatabaseADOConnetion::CommitTransaction(m_pCurAirportDatabase->GetAirportConnection()) ;
		}
		catch (CADOException& e)
		{
			e.ErrorMessage() ;
			CDatabaseADOConnetion::RollBackTransation(m_pCurAirportDatabase->GetAirportConnection()) ;
		}
	}
	if(m_IsEdit == FALSE && m_Load_OPer)
	{
		try
		{
			CDatabaseADOConnetion::BeginTransaction(m_pCurAirportDatabase->GetAirportConnection()) ;
				m_pCurAirportDatabase->getAirportMan()->deleteDatabase(m_pCurAirportDatabase) ;
				m_pCurAirportDatabase->getAirportMan()->ReSetAllID() ;
				m_pCurAirportDatabase->getAirportMan()->saveDatabase(m_pCurAirportDatabase) ;
			CDatabaseADOConnetion::CommitTransaction(m_pCurAirportDatabase->GetAirportConnection()) ;
		}
		catch (CADOException& e)
		{
			e.ErrorMessage() ;
			CDatabaseADOConnetion::RollBackTransation(m_pCurAirportDatabase->GetAirportConnection()) ;
		}
	}
	CDialog::OnOK();
}

void CDlgDBAirports::OnCtxDeleteSector() 
{
	//int nItem = GetSelectedItem( m_lstSectors );
	int nItem = m_lstSectors.GetCurSel();

	if(nItem != -1 && AfxMessageBox(_T("Are you sure?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		CSector* pSector = reinterpret_cast<CSector*>(m_lstSectors.GetItemData(nItem));
		m_pAirportMan->RemoveSector(pSector) ;
		m_lstSectors.DeleteString(nItem);
	
		m_lstSectors.SetCurSel(0);
		OnLbnSelchangeListSectors();
		m_IsEdit = TRUE ;
	}
}

void CDlgDBAirports::OnCtxDeleteAirport() 
{
 	if(AfxMessageBox(_T("Are you sure?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
 	{
		POSITION pos = m_lstAirports.GetFirstSelectedItemPosition();
		int sel = -1;
		if(!pos) return;
 	
 		while(pos)
 		{
			sel = m_lstAirports.GetNextSelectedItem(pos);
			if(sel != -1) 
			{
				CAirport* pAirport = (CAirport*) m_lstAirports.GetItemData(sel);
				CARCProjectDatabase* pAirportDatabase = (CARCProjectDatabase*)m_pCurAirportDatabase;
				CAirport* pArcAirport = pAirportDatabase->GetArcDatabaseAirport(pAirport->m_sIATACode);
				if (pArcAirport)
				{
					CAirport* pData = new CAirport(*pArcAirport);
					pData->IsEdit(TRUE);
					m_pAirportMan->GetAirportList()->push_back(pData);
				}
				m_pAirportMan->RemveAirport(pAirport) ;
			}
 		}
		m_IsEdit = TRUE ;
 		PopulateAirportList();
 	}
}

void CDlgDBAirports::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	int nMargin=7;
	int nMargin2=nMargin*2;
	CRect rc, rc2;
	CRect btnrc;
	m_wndToolBarTop.SetWindowPos(NULL,nMargin,26,cx-nMargin2,26,NULL);
	m_wndToolBarTop.GetWindowRect( &rc );
	ScreenToClient( &rc );
	
	int nHeightTop=cy/3;
	m_lstAirports.SetWindowPos( NULL,rc.left,rc.bottom,rc.Width(),nHeightTop,NULL);
	m_lstAirports.GetWindowRect( &rc );
	ScreenToClient( &rc );
	
	m_wndSplitter.SetWindowPos(NULL,rc.left,rc.bottom,rc.Width(),nMargin,SWP_NOACTIVATE);
	m_wndSplitter.SetRange(50,cy-100);
	m_wndSplitter.GetWindowRect( &rc );
	ScreenToClient( &rc );
	
	m_cstLable.GetClientRect(btnrc);
	int nHeightWnd=btnrc.Height();
	
	m_cstLable.SetWindowPos(NULL,rc.left,rc.bottom,rc.Width(),nHeightWnd,NULL);
	m_cstLable.GetWindowRect( &rc );
	ScreenToClient( &rc );
	
	m_wndToolBarBottom.SetWindowPos(NULL,rc.left,rc.bottom+1,rc.Width(),26,NULL);
	m_wndToolBarBottom.GetWindowRect( &rc );
	ScreenToClient( &rc );

	m_lstSectors.SetWindowPos(NULL, rc.left, rc.bottom, rc.Width()/4, cy-38-nMargin-rc.bottom, NULL);
	m_lstSectors.GetWindowRect( &rc2 );
	ScreenToClient( &rc2 );

	m_lstSectorMembers.SetWindowPos(NULL, rc2.right+5, rc2.top, 3*(rc.Width()/4)-5, cy-38-nMargin-rc.bottom, NULL);
	m_lstSectorMembers.GetWindowRect( &rc );
	ScreenToClient( &rc );
	
	m_ButCancel.GetWindowRect( &btnrc );
	m_ButCancel.MoveWindow( rc.right-btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	int width = btnrc.Width() ;
	m_btnOk.GetWindowRect( &btnrc );
	m_btnOk.MoveWindow( rc.right-btnrc.Width()-25-width,cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	
	m_wndToolBarTop.GetWindowRect( &rc );
	ScreenToClient( &rc );
	m_button_Load.GetWindowRect( &btnrc );
	m_button_Load.MoveWindow( rc.right-2*width-45-btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );

	m_btnCheck.GetWindowRect(&btnrc);
	ScreenToClient(&btnrc);
	m_btnCheck.MoveWindow(rc2.left,cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height());
}

int CDlgDBAirports::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndSplitter.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, IDC_STATIC_SPLITTER);
	if (!m_wndToolBarTop.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBarTop.LoadToolBar(IDR_BAGDEV_GATE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	if (!m_wndToolBarBottom.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBarBottom.LoadToolBar(IDR_BAGDEV_GATE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	

	return 0;
}

LRESULT CDlgDBAirports::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_NOTIFY)
	{
		if (wParam == IDC_STATIC_SPLITTER )
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			DoResize(pHdr->delta);
		}
	}	
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CDlgDBAirports::DoResize(int nDelta)
{
	//Resize Top Wnd
	CSplitterControl::ChangeHeight(&m_lstAirports,nDelta,CW_TOPALIGN);
	//Resize BOttom Wnd
	CSplitterControl::ChangeHeight(&m_lstSectors,-nDelta,CW_BOTTOMALIGN);
	CSplitterControl::ChangeHeight(&m_lstSectorMembers,-nDelta,CW_BOTTOMALIGN);
	CRect rect;
	m_cstLable.GetWindowRect(rect);
	ScreenToClient(rect);
	m_cstLable.SetWindowPos(NULL,rect.left,rect.top+nDelta,0,0,SWP_NOSIZE);
	
	m_wndToolBarBottom.GetWindowRect(rect);
	ScreenToClient(rect);
	m_wndToolBarBottom.SetWindowPos(NULL,rect.left,rect.top+nDelta,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
	
	Invalidate();
}


void CDlgDBAirports::OnClose() 
{
	CDialog::OnClose();
}

void CDlgDBAirports::OnClickMultiBtn()
{
	int type = m_button_Load.GetOperation() ;
	if (type == 0)
		OnLoadFromTemplate();
	if(type == 1)
		OnSaveAsTemplate();
}


void CDlgDBAirports::OnLoadFromTemplate()
{
	m_button_Load.SetOperation(0);
	m_button_Load.SetWindowText(_T("Load from Template"));
	OnBnClickedButtonLoaddefaultAirport();
}

void CDlgDBAirports::OnSaveAsTemplate()
{
	m_button_Load.SetOperation(1);
	m_button_Load.SetWindowText(_T("Save As Template"));
	CAirportDatabase* DefaultDB = AIRPORTDBLIST->getAirportDBByName(m_pCurAirportDatabase->getName());
	if(DefaultDB == NULL)
	{
		CString strError(_T(""));
		strError.Format(_T("%s AirportDB is not Exist in AirportDB template."),m_pCurAirportDatabase->getName());
		MessageBox(strError,"Warning",MB_OK);
		return ;
	}

	m_pCurAirportDatabase->saveAsTemplateDatabase(DefaultDB);
}

void CDlgDBAirports::AddDraggedItemToList()
{
	int nSectorItem = m_lstSectors.GetCurSel();
 	CSector* pSubairline = (CSector*) m_lstSectors.GetItemData(nSectorItem);
 	CString sName;
 	m_lstSectors.GetText(nSectorItem, sName);
	POSITION pos = m_lstAirports.GetFirstSelectedItemPosition();
	while(pos)
 	{
		int nAirportItem = m_lstAirports.GetNextSelectedItem(pos);
 		CAirport* pAirport = reinterpret_cast<CAirport*>(m_lstAirports.GetItemData(nAirportItem));
 		CAIRPORTFILTER af;
 		af.sIATACode = pAirport->m_sIATACode;
 		m_pAirportMan->AddSector(sName, af);
 	}
	m_IsEdit = TRUE ;
 	PopulateSectorMemberList( nSectorItem );
}

void CDlgDBAirports::OnClickLstAirports(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nItem=pNMListView->iItem;
	if(nItem!=-1)
	{
		CAirport* pAirport = (CAirport*) m_lstAirports.GetItemData(nItem);
		if (pAirport && pAirport->IsEdit())
		{
			m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_DELETEAIRPORT,FALSE);
			m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_EDITAIRPORT,TRUE);
		}
		else
		{
			m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_DELETEAIRPORT,TRUE);
			m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_EDITAIRPORT,TRUE);
		}
	}
	else
	{
		m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_DELETEAIRPORT,FALSE);
		m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_EDITAIRPORT,FALSE);
	}
	*pResult = 0;
}

void CDlgDBAirports::setAirportMan( CAirportsManager* _pAirportMan )
{
	assert( _pAirportMan );
	m_pAirportMan = _pAirportMan;
}

void CDlgDBAirports::OnLbnSelchangeListSectors()
{
	int nCurSector = m_lstSectors.GetCurSel();
	PopulateSectorMemberList(nCurSector);
	if (nCurSector == LB_ERR)
	{
		m_wndToolBarBottom.GetToolBarCtrl().EnableButton(ID_CTX_DELETESECTOR, FALSE);
		m_wndToolBarBottom.GetToolBarCtrl().EnableButton(ID_CTX_ADDAIRPORTS, FALSE);
		return;
	}
	m_wndToolBarBottom.GetToolBarCtrl().EnableButton(ID_CTX_DELETESECTOR, TRUE);
	m_wndToolBarBottom.GetToolBarCtrl().EnableButton(ID_CTX_ADDAIRPORTS, TRUE);
}


void CDlgDBAirports::OnBnClickedButtonLoaddefaultAirport()
{
	// TODO: Add your control notification handler code here
	CAirportDatabase* DefaultDB = AIRPORTDBLIST->getAirportDBByName(m_pCurAirportDatabase->getName());
	if(DefaultDB == NULL)
	{
		CString msg ;
		msg.Format("%s AirportDB is not exist in AirportDB template.",m_pCurAirportDatabase->getName()) ;
		MessageBox(msg,"Warning",MB_OK) ;
		return ;
	}
    m_pCurAirportDatabase->loadAirportFromDatabase(DefaultDB) ;
	CString msg ;
	msg.Format("Load %s's Airport from %s AirportDB template  successfully.",DefaultDB->getName(),DefaultDB->getName()) ;
	MessageBox(msg,NULL,MB_OK) ;
    m_Load_OPer = TRUE ;
    m_IsEdit = FALSE ;
	PopulateAirportList();
	PopulateSectorList();
	if (m_lstSectors.GetCount() > 0)
	{
		m_lstSectors.SetCurSel(0);
		OnLbnSelchangeListSectors();
	}
}

void CDlgDBAirports::OnCancel()
{

	 if(m_IsEdit || m_Load_OPer)
		  m_pCurAirportDatabase->loadAirportFromDatabase(m_pCurAirportDatabase) ;
	CDialog::OnCancel() ;
}
CDlgDBAirportsForDefault::CDlgDBAirportsForDefault(BOOL bMenu,Terminal* _pTerm, CAirportDatabase* _theAirportDB, CWnd* pParent /* = NULL */)
	: CDlgDBAirports(bMenu,_pTerm,pParent)
{
	// retrieve the pointer
	m_pCurAirportDatabase = _pTerm?(_pTerm->m_pAirportDB):_theAirportDB;
	ASSERT(m_pCurAirportDatabase);

	CAirportDatabase* pDefaultDB = AIRPORTDBLIST->getAirportDBByName( m_pCurAirportDatabase->getName());
	ASSERT(pDefaultDB);
	m_pAirportMan = pDefaultDB->getAirportMan();
	ASSERT(m_pAirportMan);

	if(!pDefaultDB->hadLoadDatabase())
		pDefaultDB->LoadDataFromDB(DATABASESOURCE_TYPE_ACCESS_GLOBALDB);
}

void CDlgDBAirportsForDefault::OnOK()
{
	try
	{
		if (m_pAirportMan)
		{
			CDatabaseADOConnetion::BeginTransaction(m_pCurAirportDatabase->GetAirportConnection()) ;
			m_pAirportMan->saveDatabase(m_pCurAirportDatabase) ;
			CDatabaseADOConnetion::CommitTransaction(m_pCurAirportDatabase->GetAirportConnection()) ;
		}
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		CDatabaseADOConnetion::RollBackTransation(m_pCurAirportDatabase->GetAirportConnection()) ;
	}
	CDialog::OnOK() ;
}
void CDlgDBAirportsForDefault::OnCancel()
{
	if(m_pAirportMan)
	{
		m_pAirportMan->loadDatabase(m_pCurAirportDatabase) ;
	}
	CDialog::OnCancel();
}
CDlgDBAirportsForDefault::~CDlgDBAirportsForDefault()
{

}
BOOL CDlgDBAirportsForDefault::OnInitDialog()
{
	CDlgDBAirports::OnInitDialog();
// 	m_wndToolBarTop.ShowWindow(SW_HIDE);
// 	m_wndToolBarBottom.ShowWindow(SW_HIDE) ;
	m_button_Load.ShowWindow(SW_HIDE) ;
	m_btnCheck.ShowWindow(SW_HIDE);
//	m_StaticLoad.ShowWindow(SW_HIDE);
	SetWindowText("Global Airports");
	return TRUE ;
}
