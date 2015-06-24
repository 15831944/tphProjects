// DlgDBAirline.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgDBAirline.h"
#include "termplan.h"
#include "DlgAirlineProperties.h"
#include "..\common\AirlineManager.h"
#include "FltSchedDefineDlg.h"
#include "engine\terminal.h"
#include "common\airportdatabase.h"
#include ".\dlgdbairline.h"
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
// CDlgDBAirline dialog


CDlgDBAirline::CDlgDBAirline(BOOL bMenu,Terminal* _pTerm, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDBAirline::IDD, pParent)
	, m_bDragging(FALSE)
	, m_pDragImage(NULL)
	, m_nItemDragSrc(-1)
	, m_nItemDragDes(-1)
	, m_ptLastMouse(-1, -1)
	, m_bMenu(bMenu)
	, m_commit_Operation(FALSE)
	, m_Load_Operation(FALSE)
	, m_IsEdit(FALSE)
	, m_pTerm(_pTerm)
{
	if( m_pTerm  )
	{
		m_pCurAirportDatabase = m_pTerm->m_pAirportDB;
		ASSERT(m_pCurAirportDatabase);
		m_pAirlinesMan = m_pCurAirportDatabase->getAirlineMan();
		ASSERT(m_pAirlinesMan);
	}
	else
	{
		// m_bMenu should not be FALSE when _pTerm is NULL
		ASSERT(TRUE == m_bMenu);
		// here should initialize m_pCurAirportDatabase and m_pAcMan
		m_pCurAirportDatabase = NULL;
		m_pAirlinesMan = NULL;
	}
}

CDlgDBAirline::~CDlgDBAirline()
{
	if (m_pDragImage)
	{
		delete m_pDragImage;
		m_pDragImage = NULL;
	}
}
void CDlgDBAirline::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDBAirline)
	DDX_Control(pDX, IDC_STATIC_LABLE, m_cstLable);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_LST_AIRLINES, m_lstAirlines);
	DDX_Control(pDX, IDC_LIST_SUBAIRLINES, m_lstSubAirlines);
	DDX_Control(pDX, IDC_LIST_SALMEMBERS, m_lstSALMembers);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BUTTON_LOADDEFAULT_AIRLINE, m_button_Load);
	DDX_Control(pDX, IDCANCEL, m_buttonCancel);
	DDX_Control(pDX, IDC_CHECK_FLIGHTSCHEDULE, m_btnCheck);
	//DDX_Control(pDX, IDC_STATIC_LOAD, m_StaticLoad);
}


BEGIN_MESSAGE_MAP(CDlgDBAirline, CDialog)
	//{{AFX_MSG_MAP(CDlgDBAirline)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SUBAIRLINES, OnItemChangedSALList)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CTX_NEWAIRLINE, OnCtxNewAirline)
	ON_COMMAND(ID_CTX_EDITAIRLINE, OnCtxEditAirline)
	ON_COMMAND(ID_CTX_NEWSUBAIRLINE, OnCtxNewSubairline)
	ON_COMMAND(ID_CTX_ADDAIRLINES, OnCtxAddAirlines)
	ON_COMMAND(ID_CTX_DELETESUBAIRLINE, OnCtxDeleteSubairline)
	ON_COMMAND(ID_CTX_DELETEAIRLINE, OnCtxDeleteAirline)
	ON_COMMAND(ID_AIRPORT_SAVEAS, OnSaveAsTemplate)
	ON_COMMAND(ID_AIRPORT_LOAD, OnLoadFromTemplate)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
//	ON_NOTIFY(NM_CLICK, IDC_LST_AIRLINES, OnClickLstAirlines)
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_LBN_SELCHANGE(IDC_LIST_SUBAIRLINES, OnLbnSelchangeListSubairlines)
	ON_BN_CLICKED(IDC_BUTTON_LOADDEFAULT_AIRLINE, OnBnClickedButtonLoaddefaultAirline)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LST_AIRLINES, OnBegindragLstAirlines)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY(NM_CLICK, IDC_LST_AIRLINES, OnClickLstAirlines)
	ON_BN_CLICKED(IDC_CHECK_FLIGHTSCHEDULE, OnBnClickedCheckFlightSchedule)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDBAirline message handlers


void CDlgDBAirline::OnBnClickedCheckFlightSchedule()
{
	m_bMenu =m_btnCheck.GetCheck()?FALSE:TRUE;
	PopulateAirlineList();
	if (m_lstSubAirlines.GetCount() > 0)
	{
		m_lstSubAirlines.SetCurSel(0);
		OnLbnSelchangeListSubairlines();
	}

}

void CDlgDBAirline::OnPaint()
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

BOOL CDlgDBAirline::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CDlgDBAirline::PopulateSubairlineList()
{
	CSUBAIRLINELIST* pvSList = m_pAirlinesMan->GetSubairlineList();
	if(!pvSList)
		return; //bail if no list

	m_lstSubAirlines.ResetContent();
	int nCurItem = 0;
	for(CSUBAIRLINELIST::const_iterator iter = pvSList->begin();
		iter != pvSList->end(); iter++)
	{
		CSubairline* pSubairline = *iter;
		m_lstSubAirlines.SetItemData(m_lstSubAirlines.InsertString(nCurItem, pSubairline->m_sName), (DWORD)pSubairline);
		nCurItem ++;
	}
}

void CDlgDBAirline::PopulateSubairlineMembersList(int _nItem)
{
	int nItem = m_lstSubAirlines.GetCurSel();
	ASSERT( nItem == _nItem );
	m_lstSALMembers.DeleteAllItems();
	if (nItem < 0) return;

	CSubairline* pSubairline = reinterpret_cast<CSubairline*>(m_lstSubAirlines.GetItemData(nItem));
	CAIRLINELIST AList;
	char str[128];
	pSubairline->GetAirlineList(AList);
	int m = 0;
	if (m_bMenu)
	{
		for(int k=0; k<static_cast<int>(AList.size()); k++) 
		{
			m_lstSALMembers.InsertItem(k,AList[k]->m_sAirlineIATACode);
			m_lstSALMembers.SetItemText(k,1,AList[k]->m_sICAOCode);
			m_lstSALMembers.SetItemText(k,2,AList[k]->m_sLongName);
			m_lstSALMembers.SetItemData(k,(DWORD)AList[k]);
		}
	}
	else
	{
		for(int i=0; i<static_cast<int>(AList.size()); i++)
		{
			CAirline* pAirline = (AList)[i];
			if (m_pTerm)
			{
				for (int j=0;j<m_pTerm->flightSchedule->getCount();j++)
				{
					Flight* pFlight = m_pTerm->flightSchedule->getItem(j);
					pFlight->getAirline(str);
					if (strcmp(str,pAirline->m_sAirlineIATACode) == 0)
					{
						m_lstSALMembers.InsertItem(m,pAirline->m_sAirlineIATACode);
						m_lstSALMembers.SetItemText(m,1,pAirline->m_sICAOCode);
						m_lstSALMembers.SetItemText(m,2,pAirline->m_sLongName);
						m_lstSALMembers.SetItemData(m,(DWORD)pAirline);
						m++;
						break;
					}
				}
			}
		}
	}
}
void CDlgDBAirline::AddAirlineItem( CAirline* pAirline)
{
	int insertItem = m_lstAirlines.GetItemCount() ;
	m_lstAirlines.InsertItem(insertItem,pAirline->m_sAirlineIATACode);
	m_lstAirlines.SetItemText(insertItem,1,pAirline->m_sICAOCode);
	m_lstAirlines.SetItemText(insertItem,2,pAirline->m_sLongName);
	m_lstAirlines.SetItemText(insertItem,3,pAirline->m_sOtherCodes);
	m_lstAirlines.SetItemText(insertItem,4,pAirline->m_sStreetAddress);
	m_lstAirlines.SetItemText(insertItem,5,pAirline->m_sCity);
	m_lstAirlines.SetItemText(insertItem,6,pAirline->m_sCountry);
	m_lstAirlines.SetItemText(insertItem,7,pAirline->m_sHQZipCode);
	m_lstAirlines.SetItemText(insertItem,8,pAirline->m_sContinent);
	m_lstAirlines.SetItemText(insertItem,9,pAirline->m_sTelephone);
	m_lstAirlines.SetItemText(insertItem,10,pAirline->m_sFax);
	m_lstAirlines.SetItemText(insertItem,11,pAirline->m_sEMail);
	m_lstAirlines.SetItemText(insertItem,12,pAirline->m_sDirector);
	m_lstAirlines.SetItemText(insertItem,13,pAirline->m_sFleet);
	m_lstAirlines.SetItemText(insertItem,14,"soiled");
	m_lstAirlines.SetItemData(insertItem,(DWORD)pAirline);

	if (pAirline->IsEdit())
	{
		m_lstAirlines.SetItemColor(insertItem,::GetSysColor(COLOR_WINDOWFRAME),RGB(217,217,255));
	}
	pAirline->Update();
	m_IsEdit = TRUE ;
}
void CDlgDBAirline::EditAirlineItem(int selItem , CAirline* pAirline)
{
	m_lstAirlines.SetItemText(selItem,0,pAirline->m_sAirlineIATACode);
	m_lstAirlines.SetItemText(selItem,1,pAirline->m_sICAOCode);
	m_lstAirlines.SetItemText(selItem,2,pAirline->m_sLongName);
	m_lstAirlines.SetItemText(selItem,3,pAirline->m_sOtherCodes);
	m_lstAirlines.SetItemText(selItem,4,pAirline->m_sStreetAddress);
	m_lstAirlines.SetItemText(selItem,5,pAirline->m_sCity);
	m_lstAirlines.SetItemText(selItem,6,pAirline->m_sCountry);
	m_lstAirlines.SetItemText(selItem,7,pAirline->m_sHQZipCode);
	m_lstAirlines.SetItemText(selItem,8,pAirline->m_sContinent);
	m_lstAirlines.SetItemText(selItem,9,pAirline->m_sTelephone);
	m_lstAirlines.SetItemText(selItem,10,pAirline->m_sFax);
	m_lstAirlines.SetItemText(selItem,11,pAirline->m_sEMail);
	m_lstAirlines.SetItemText(selItem,12,pAirline->m_sDirector);
	m_lstAirlines.SetItemText(selItem,13,pAirline->m_sFleet);
	m_lstAirlines.SetItemText(selItem,14,"soiled");
	m_lstAirlines.SetItemData(selItem,(DWORD)pAirline);

	if (pAirline->IsEdit())
	{
		m_lstAirlines.SetItemColor(selItem,::GetSysColor(COLOR_WINDOWFRAME),RGB(217,217,255));
	}
	pAirline->Update();
	m_IsEdit = TRUE ;
}
void CDlgDBAirline::PopulateAirlineList()
{
	m_lstAirlines.DeleteAllItems() ;

	CString strIATA,strICAO,strName,strOther,strStreet,strCity,strCountry,strHQ,strContinent,strTel,strFax,strEmail,
		strDirector,strFleet,strGroup;
	CAIRLINELIST* pvList;
	char str[128];
	int k = 0;
	pvList = m_pAirlinesMan->GetAirlineList();
	if (m_bMenu)
	{
		for(int i=0; i<static_cast<int>(pvList->size()); i++)
		{
			CAirline* pAirline = (*pvList)[i];
			m_lstAirlines.InsertItem(i,pAirline->m_sAirlineIATACode);
			m_lstAirlines.SetItemText(i,1,pAirline->m_sICAOCode);
			m_lstAirlines.SetItemText(i,2,pAirline->m_sLongName);
			m_lstAirlines.SetItemText(i,3,pAirline->m_sOtherCodes);
			m_lstAirlines.SetItemText(i,4,pAirline->m_sStreetAddress);
			m_lstAirlines.SetItemText(i,5,pAirline->m_sCity);
			m_lstAirlines.SetItemText(i,6,pAirline->m_sCountry);
			m_lstAirlines.SetItemText(i,7,pAirline->m_sHQZipCode);
			m_lstAirlines.SetItemText(i,8,pAirline->m_sContinent);
			m_lstAirlines.SetItemText(i,9,pAirline->m_sTelephone);
			m_lstAirlines.SetItemText(i,10,pAirline->m_sFax);
			m_lstAirlines.SetItemText(i,11,pAirline->m_sEMail);
			m_lstAirlines.SetItemText(i,12,pAirline->m_sDirector);
			m_lstAirlines.SetItemText(i,13,pAirline->m_sFleet);
			m_lstAirlines.SetItemText(i,14,"soiled");
			m_lstAirlines.SetItemData(i,(DWORD)pAirline);
			if (pAirline->IsEdit())
			{
				m_lstAirlines.SetItemColor(i,::GetSysColor(COLOR_WINDOWFRAME),RGB(235,235,235));
			}
		}
	}
	else
	{
		for(int i=0; i<static_cast<int>(pvList->size()); i++)
		{
			CAirline* pAirline = (*pvList)[i];
			if (m_pTerm)
			{
				for (int j=0;j<m_pTerm->flightSchedule->getCount();j++)
				{
					Flight* pFlight = m_pTerm->flightSchedule->getItem(j);
					pFlight->getAirline(str);
					if (strcmp(str,pAirline->m_sAirlineIATACode) == 0)
					{
						m_lstAirlines.InsertItem(k,pAirline->m_sAirlineIATACode);
						m_lstAirlines.SetItemText(k,1,pAirline->m_sICAOCode);
						m_lstAirlines.SetItemText(k,2,pAirline->m_sLongName);
						m_lstAirlines.SetItemText(k,3,pAirline->m_sOtherCodes);
						m_lstAirlines.SetItemText(k,4,pAirline->m_sStreetAddress);
						m_lstAirlines.SetItemText(k,5,pAirline->m_sCity);
						m_lstAirlines.SetItemText(k,6,pAirline->m_sCountry);
						m_lstAirlines.SetItemText(k,7,pAirline->m_sHQZipCode);
						m_lstAirlines.SetItemText(k,8,pAirline->m_sContinent);
						m_lstAirlines.SetItemText(k,9,pAirline->m_sTelephone);
						m_lstAirlines.SetItemText(k,10,pAirline->m_sFax);
						m_lstAirlines.SetItemText(k,11,pAirline->m_sEMail);
						m_lstAirlines.SetItemText(k,12,pAirline->m_sDirector);
						m_lstAirlines.SetItemText(k,13,pAirline->m_sFleet);
						m_lstAirlines.SetItemText(k,14,"soiled");
						m_lstAirlines.SetItemData(k,(DWORD)pAirline);
						if (pAirline->IsEdit())
						{
							m_lstAirlines.SetItemColor(k,::GetSysColor(COLOR_WINDOWFRAME),RGB(235,235,235));
						}
						k++;
						break;
					}
				}
			}
		}
	}
}



BOOL CDlgDBAirline::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_wndToolBarTop.GetToolBarCtrl().SetCmdID(0,ID_CTX_NEWAIRLINE);
	m_wndToolBarTop.GetToolBarCtrl().SetCmdID(1,ID_CTX_DELETEAIRLINE);
	m_wndToolBarTop.GetToolBarCtrl().SetCmdID(2,ID_CTX_EDITAIRLINE);
	m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_DELETEAIRLINE,FALSE);
	m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_EDITAIRLINE,FALSE);
	m_wndToolBarBottom.GetToolBarCtrl().SetCmdID(0,ID_CTX_NEWSUBAIRLINE);
	m_wndToolBarBottom.GetToolBarCtrl().SetCmdID(1,ID_CTX_DELETESUBAIRLINE);
	m_wndToolBarBottom.GetToolBarCtrl().SetCmdID(2,ID_CTX_ADDAIRLINES);
	m_wndToolBarBottom.GetToolBarCtrl().EnableButton(ID_CTX_ADDAIRLINES,FALSE);

	DWORD dwStyle = m_lstAirlines.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_lstAirlines.SetExtendedStyle( dwStyle );
	CRect listrect;
	m_lstAirlines.GetClientRect(&listrect);
	m_lstAirlines.InsertColumn(0, "IATA Code", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirlines.InsertColumn(1, "ICAO Code", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirlines.InsertColumn(2, "Airline Name", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirlines.InsertColumn(3, "Other Codes", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirlines.InsertColumn(4, "Street Address", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirlines.InsertColumn(5, "City", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirlines.InsertColumn(6, "Country", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirlines.InsertColumn(7, "HQ Zip Code", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirlines.InsertColumn(8, "Continent", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirlines.InsertColumn(9, "Telephone", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirlines.InsertColumn(10, "Fax", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirlines.InsertColumn(11, "E-Mail", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirlines.InsertColumn(12, "Director", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirlines.InsertColumn(13, "Fleet", LVCFMT_LEFT, listrect.Width()/5);
	m_lstAirlines.InsertColumn(14, "Group", LVCFMT_LEFT, listrect.Width()/5);

	m_lstAirlines.SetDataType(0,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirlines.SetDataType(1,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirlines.SetDataType(2,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirlines.SetDataType(3,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirlines.SetDataType(4,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirlines.SetDataType(5,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirlines.SetDataType(6,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirlines.SetDataType(7,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirlines.SetDataType(8,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirlines.SetDataType(9,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirlines.SetDataType(10,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirlines.SetDataType(11,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirlines.SetDataType(12,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirlines.SetDataType(13,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstAirlines.SetDataType(14,CSortAndPrintListCtrl::ITEM_STRING);

	m_lstAirlines.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	m_lstAirlines.SubClassHeadCtrl();
	//now populate the list
	PopulateAirlineList();

	m_lstSALMembers.GetClientRect(&listrect);
	dwStyle = m_lstSALMembers.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_lstSALMembers.SetExtendedStyle( dwStyle );
	m_lstSALMembers.GetClientRect(&listrect);
	m_lstSALMembers.InsertColumn(0, "IATA Code", LVCFMT_LEFT, listrect.Width()/4);
	m_lstSALMembers.InsertColumn(1, "ICAO Code", LVCFMT_LEFT, listrect.Width()/4);
	m_lstSALMembers.InsertColumn(2, "Airline Name", LVCFMT_LEFT, listrect.Width()/2);
	m_lstSALMembers.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

	m_lstSALMembers.SetDataType(0,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstSALMembers.SetDataType(1,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstSALMembers.SetDataType(2,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstSALMembers.SubClassHeadCtrl();

	PopulateSubairlineList();
	if (m_lstSubAirlines.GetCount() > 0)
	{
		m_lstSubAirlines.SetCurSel(0);
		OnLbnSelchangeListSubairlines();
	}

	//	Init Cool button
	m_button_Load.SetOperation(0);
	m_button_Load.SetWindowText(_T("Database Template"));
	m_button_Load.SetOpenButton(FALSE);
	m_button_Load.SetType(CCoolBtn::TY_AIRLINE);

	m_btnCheck.SetCheck(!m_bMenu);


	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	/*CString msg ;
	msg.Format(_T("Load airline from %s template"),m_pCurAirportDatabase->getName()) ;
	m_StaticLoad.SetWindowText(msg) ;*/

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CDlgDBAirline message handlers

void CDlgDBAirline::FreeItemMemory()
{
}

void CDlgDBAirline::OnDestroy() 
{
	FreeItemMemory();
	CDialog::OnDestroy();	
}

void CDlgDBAirline::OnCtxNewAirline() 
{
	int nCount = m_pAirlinesMan->GetAirlineList()->size();
	CString sName;
	sName.Format("New Airline %d", nCount);
	CAirline* pA = new CAirline();
	pA->m_sLongName  = sName;
	CDlgAirlineProperties dlg(pA);
	if(dlg.DoModal() == IDOK) {
		//check to make sure this airline doesn't already exist
		CAIRLINEFILTER apFilter;
		apFilter.code = pA->m_sAirlineIATACode;
		CAIRLINELIST vListAP;
		m_pAirlinesMan->GetAirlinesByFilter(apFilter, vListAP);
		if(vListAP.size() == 0) {
			m_pAirlinesMan->GetAirlineList()->push_back(pA);
			AddAirlineItem(pA) ;
			return;
		}
		else {
			CString msg;
			msg.Format("An airline with IATA code %s (%s) already exists.\nOperation cancelled.", vListAP[0]->m_sAirlineIATACode, vListAP[0]->m_sLongName);
			AfxMessageBox(msg);
		}
	}
	delete pA;
}

void CDlgDBAirline::OnCtxEditAirline() 
{
	int sel = GetSelectedItem( m_lstAirlines );
 	if(sel != -1) {
 		CAirline* pAirline = (CAirline*) m_lstAirlines.GetItemData(sel);
		if(pAirline == NULL)
			return ;
		CString code = pAirline->m_sAirlineIATACode ;
 		CDlgAirlineProperties dlg(pAirline);
 		if(dlg.DoModal() == IDOK) {
			CAIRLINEFILTER apFilter;
			apFilter.code = pAirline->m_sAirlineIATACode;
			CAIRLINELIST vListAP;
			m_pAirlinesMan->GetAirlinesByFilter(apFilter, vListAP);
			if(vListAP.size() <= 1) 
			{
			   if (pAirline->IsEdit())
			   {
				   pAirline->IsEdit(FALSE);
				   pAirline->SetID(-1);
				   if (pAirline->m_sAirlineIATACode != code)
				   {
					   CARCProjectDatabase* pAirportDatabase = (CARCProjectDatabase*)m_pCurAirportDatabase;
					   CAirline* pArcAirline = pAirportDatabase->GetArcDatabaseAirline(code);
					   if (pArcAirline)
					   {
						   CAirline* pData = new CAirline(*pArcAirline);
						   pData->IsEdit(TRUE);
						   m_pAirlinesMan->GetAirlineList()->push_back(pData);
					   }
				   }
			   }
			   EditAirlineItem(sel,pAirline) ;
			}else
			{
				CString msg;
				msg.Format("An airline with IATA code %s (%s) already exists.\nOperation cancelled.", vListAP[0]->m_sAirlineIATACode, vListAP[0]->m_sLongName);
				AfxMessageBox(msg);
				pAirline->m_sAirlineIATACode = code ;
			}
			PopulateAirlineList();
 		}
 	}
}

void CDlgDBAirline::OnCtxNewSubairline() 
{
	int nCount = m_pAirlinesMan->GetSubairlineList()->size();
	CString sName;
	sName.Format("New Subairline %d", nCount);
	CSubairline* pSubairline = new CSubairline( m_pAirlinesMan );
	pSubairline->m_sName = sName;
	CDlgSubairlineProperties dlg( m_pAirlinesMan, pSubairline);
	if(dlg.DoModal() == IDOK)
	{
		int nCurItem = m_lstSubAirlines.InsertString(m_lstSubAirlines.GetCount(), pSubairline->m_sName);
		m_lstSubAirlines.SetItemData(nCurItem, reinterpret_cast<LPARAM>(pSubairline));

		m_pAirlinesMan->GetSubairlineList()->push_back(pSubairline);
		m_lstSubAirlines.SetCurSel(m_lstSubAirlines.GetCount() - 1);
		m_IsEdit = TRUE ;
		OnLbnSelchangeListSubairlines();
	}
	else
	{
		delete pSubairline;
	}
}

void CDlgDBAirline::OnCtxAddAirlines() //i.e. edit subairline members - so "edit criteria"
{
	int nItem = m_lstSubAirlines.GetCurSel();
	if (nItem == -1)
		return;

	CSubairline* pSubairline = reinterpret_cast<CSubairline*>(m_lstSubAirlines.GetItemData(nItem));
	CDlgSubairlineProperties dlg(m_pAirlinesMan, pSubairline);
	if(dlg.DoModal() == IDOK)
	{
		PopulateSubairlineList();
		m_lstSubAirlines.SetCurSel(nItem);
		PopulateSubairlineMembersList(nItem);
		m_IsEdit = TRUE ;
	}
}

void CDlgDBAirline::OnOK() 
{
	CAirportDatabase* DefaultDB = AIRPORTDBLIST->getAirportDBByName(m_pCurAirportDatabase->getName());
	if(m_IsEdit)
	{
		//write local data 
		try
		{
			CDatabaseADOConnetion::BeginTransaction(m_pCurAirportDatabase->GetAirportConnection()) ;
			if(m_Load_Operation)
			{
				m_pCurAirportDatabase->getAirlineMan()->deleteDatabase(m_pCurAirportDatabase) ;
				m_pCurAirportDatabase->getAirlineMan()->ReSetAllID() ;
				m_pCurAirportDatabase->getAirlineMan()->saveDatabase(m_pCurAirportDatabase) ;
			}
			else
			{

				m_pCurAirportDatabase->getAirlineMan()->saveDatabase(m_pCurAirportDatabase) ;
			}
			CDatabaseADOConnetion::CommitTransaction(m_pCurAirportDatabase->GetAirportConnection()) ;
		}
		catch (CADOException& e)
		{
			CString ErrorMsg ;
			ErrorMsg.Format(_T("Save Data Error ,ErrorMsg:%s") ,e.ErrorMessage()) ;
			CDatabaseADOConnetion::RollBackTransation(m_pCurAirportDatabase->GetAirportConnection()) ;
			MessageBox(ErrorMsg,_T("Error"),MB_OK) ;
		}	
	}
	if(m_IsEdit == FALSE && m_Load_Operation)
	{
		try
		{
			CDatabaseADOConnetion::BeginTransaction(m_pCurAirportDatabase->GetAirportConnection()) ;
			m_pCurAirportDatabase->getAirlineMan()->deleteDatabase(m_pCurAirportDatabase) ;
			m_pCurAirportDatabase->getAirlineMan()->ReSetAllID() ;
			m_pCurAirportDatabase->getAirlineMan()->saveDatabase(m_pCurAirportDatabase) ;
			CDatabaseADOConnetion::CommitTransaction(m_pCurAirportDatabase->GetAirportConnection()) ;
		}
		catch (CADOException& e)
		{
			CString ErrorMsg ;
			ErrorMsg.Format(_T("Save Data Error ,ErrorMsg:%s") ,e.ErrorMessage()) ;
			CDatabaseADOConnetion::RollBackTransation(m_pCurAirportDatabase->GetAirportConnection()) ;
			MessageBox(ErrorMsg,_T("Error"),MB_OK) ;
		}
	}
	CDialog::OnOK();
}

void CDlgDBAirline::OnCtxDeleteSubairline() 
{
	int nItem = m_lstSubAirlines.GetCurSel();

	if(nItem != -1 && AfxMessageBox(_T("Are you sure?"), MB_YESNO | MB_ICONQUESTION) == IDYES) {

		CSubairline* pSubairline = reinterpret_cast<CSubairline*>(m_lstSubAirlines.GetItemData(nItem));
		CSUBAIRLINELIST* pAllSubairlines = m_pAirlinesMan->GetSubairlineList();
		m_pAirlinesMan->RemoveSubAirline(pSubairline) ;

		m_lstSubAirlines.DeleteString(nItem);
		
		m_lstSubAirlines.SetCurSel(0);
		OnLbnSelchangeListSubairlines();
		m_IsEdit = TRUE ;
	}
}

void CDlgDBAirline::OnCtxDeleteAirline() 
{
 	if(AfxMessageBox(_T("Are you sure?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
 	{
 		POSITION pos = m_lstAirlines.GetFirstSelectedItemPosition();
		if(!pos) return;
 		
 		int sel = -1;
 		while (pos)
 		{	
 			sel = m_lstAirlines.GetNextSelectedItem(pos);
			if(sel != -1) 
			{
				CAirline* pAirline = (CAirline*) m_lstAirlines.GetItemData(sel);
				CARCProjectDatabase* pAirportDatabase = (CARCProjectDatabase*)m_pCurAirportDatabase;
				CAirline* pArcAirline = pAirportDatabase->GetArcDatabaseAirline(pAirline->m_sAirlineIATACode);
				if (pArcAirline)
				{
					CAirline* pData = new CAirline(*pArcAirline);
					pData->IsEdit(TRUE);
					m_pAirlinesMan->GetAirlineList()->push_back(pData);
				}
				m_pAirlinesMan->RemoveAirline(pAirline) ;
			}
 		}
		m_IsEdit = TRUE ;
 		PopulateAirlineList();
 	}
}

void CDlgDBAirline::OnBegindragLstAirlines(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	m_nItemDragSrc = pNMListView->iItem;
	m_nItemDragDes = -1;

	if(m_pDragImage)
		delete m_pDragImage;

	CPoint ptTemp(0,0);
	m_pDragImage = m_lstAirlines.CreateDragImage( m_nItemDragSrc, &ptTemp );
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

void CDlgDBAirline::OnMouseMove(UINT nFlags, CPoint point) 
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
		CRect rcSALM;
		m_lstSALMembers.GetWindowRect( &rcSALM );
		if( rcSALM.PtInRect( pt ) )	
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			CPoint ptTest( point );
			ClientToScreen( &ptTest );
			m_lstSubAirlines.ScreenToClient(&ptTest);

			// TRACE("x = %d, y = %d \r\n", ptTest.x, ptTest.y);

			//destination item is the currently selected subairline
			m_nItemDragDes = m_lstSubAirlines.GetCurSel();
		}
		else
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		}
		m_pDragImage->DragShowNolock (TRUE);
		m_lstAirlines.Invalidate(FALSE);
		m_ptLastMouse=point;
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CDlgDBAirline::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( m_bDragging )
	{
		CPoint pt (point);
		ClientToScreen (&pt);

		CRect rcSALM;
		m_lstSALMembers.GetWindowRect( &rcSALM );
		if( (m_nItemDragDes != -1) && rcSALM.PtInRect( pt ) && m_lstSALMembers.IsWindowEnabled() )	
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

void CDlgDBAirline::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	int nMargin=7;
	int nMargin2=nMargin*2;
	CRect rc, rc2;
	CRect btnrc;
	m_wndToolBarTop.SetWindowPos(NULL,nMargin,26,cx-nMargin2,26,NULL);
	m_wndToolBarTop.GetWindowRect(&rc);
	ScreenToClient(&rc);
	
	int nHeightTop=cy/3;
	m_lstAirlines.SetWindowPos( NULL,rc.left,rc.bottom,rc.Width(),nHeightTop,NULL);
	m_lstAirlines.GetWindowRect(&rc);
	ScreenToClient(&rc);
	
	m_wndSplitter.SetWindowPos(NULL,rc.left,rc.bottom,rc.Width(),nMargin,SWP_NOACTIVATE);
	m_wndSplitter.SetRange(50,cy-100);
	m_wndSplitter.GetWindowRect(&rc);
	ScreenToClient(&rc);
	
	m_cstLable.GetClientRect(btnrc);
	int nHeightWnd=btnrc.Height();
	
	m_cstLable.SetWindowPos(NULL,rc.left,rc.bottom,rc.Width(),nHeightWnd,NULL);
	m_cstLable.GetWindowRect(&rc);
	ScreenToClient(&rc);
	
	m_wndToolBarBottom.SetWindowPos(NULL,rc.left,rc.bottom+1,rc.Width(),26,NULL);
	m_wndToolBarBottom.GetWindowRect(&rc);
	ScreenToClient(&rc);
	
	m_lstSubAirlines.SetWindowPos(NULL, rc.left, rc.bottom, rc.Width()/4, cy-38-nMargin-rc.bottom, NULL);
	m_lstSubAirlines.GetWindowRect( &rc2 );
	ScreenToClient(&rc2);

	m_lstSALMembers.SetWindowPos(NULL, rc2.right+5, rc2.top, 3*(rc.Width()/4)-5, cy-38-nMargin-rc.bottom, NULL);
	m_lstSALMembers.GetWindowRect( &rc );
	ScreenToClient( &rc );

	m_buttonCancel.GetWindowRect( &btnrc );
	m_buttonCancel.MoveWindow( rc.right-btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
       int width = btnrc.Width() ;
	m_btnOk.GetWindowRect( &btnrc );
	m_btnOk.MoveWindow( rc.right-btnrc.Width()-25 -width ,cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );

	m_wndToolBarTop.GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_button_Load.GetWindowRect( &btnrc );
	m_button_Load.MoveWindow( rc.right-2*width-45 -btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );

	m_btnCheck.GetWindowRect(&btnrc);
	ScreenToClient(&btnrc);
	m_btnCheck.MoveWindow(rc2.left,cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height());
}

int CDlgDBAirline::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

LRESULT CDlgDBAirline::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
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

void CDlgDBAirline::DoResize(int nDelta)
{
	//Resize Top Wnd
	CSplitterControl::ChangeHeight(&m_lstAirlines,nDelta,CW_TOPALIGN);
	//Resize BOttom Wnd
	CSplitterControl::ChangeHeight(&m_lstSubAirlines,-nDelta,CW_BOTTOMALIGN);
	CSplitterControl::ChangeHeight(&m_lstSALMembers,-nDelta,CW_BOTTOMALIGN);
	CRect rect;
	m_cstLable.GetWindowRect(rect);
	ScreenToClient(rect);
	m_cstLable.SetWindowPos(NULL,rect.left,rect.top+nDelta,0,0,SWP_NOSIZE);
	
	m_wndToolBarBottom.GetWindowRect(rect);
	ScreenToClient(rect);
	m_wndToolBarBottom.SetWindowPos(NULL,rect.left,rect.top+nDelta,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
	
	Invalidate();
}

void CDlgDBAirline::OnClose() 
{
	CDialog::OnClose();
}

void CDlgDBAirline::AddDraggedItemToList()
{
 	int nSALItem = m_nItemDragDes;
 	CSubairline* pSubairline = (CSubairline*) m_lstSubAirlines.GetItemData(nSALItem);
 	CString sName;
 	m_lstSubAirlines.GetText(nSALItem, sName);
 	POSITION pos = m_lstAirlines.GetFirstSelectedItemPosition();
 	while(pos)
 	{
		int nAirlineItem = m_lstAirlines.GetNextSelectedItem(pos);
 		CAIRLINEFILTER af;
 		af.code = m_lstAirlines.GetItemText(nAirlineItem, 0);
 		m_pAirlinesMan->AddSubairline(sName, af);
 	}
	m_IsEdit = TRUE ;
	PopulateSubairlineMembersList( nSALItem );
}

void CDlgDBAirline::OnClickLstAirlines(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nItem=pNMListView->iItem;
	if(nItem!=-1)
	{
		CAirline* pAirline = (CAirline*) m_lstAirlines.GetItemData(nItem);
		if (pAirline && pAirline->IsEdit())
		{
			m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_DELETEAIRLINE,FALSE);
			m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_EDITAIRLINE,TRUE);
		}
		else
		{
			m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_DELETEAIRLINE,TRUE);
			m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_EDITAIRLINE,TRUE);
		}
	}
	else
	{
		m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_DELETEAIRLINE,FALSE);
		m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_EDITAIRLINE,FALSE);
	}
	*pResult = 0;
}


void CDlgDBAirline::setAirlinesMan( CAirlinesManager* _pAirlinesMan )
{
	ASSERT( _pAirlinesMan );
	m_pAirlinesMan = _pAirlinesMan;
}

void CDlgDBAirline::OnItemChangedSALList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;
	
	if((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
		return;

	if(pnmv->uOldState & LVIS_SELECTED) {
		if(!(pnmv->uNewState & LVIS_SELECTED)) {
		}
	}
	else if(pnmv->uNewState & LVIS_SELECTED) {
	
		PopulateSubairlineMembersList(pnmv->iItem);
	}
	else
		ASSERT(0);
}
void CDlgDBAirline::OnLbnSelchangeListSubairlines()
{
	int nCurSector = m_lstSubAirlines.GetCurSel();
	PopulateSubairlineMembersList(nCurSector);
	if (nCurSector == LB_ERR)
		return;
	m_wndToolBarBottom.GetToolBarCtrl().EnableButton(ID_CTX_DELETESECTOR);
	m_wndToolBarBottom.GetToolBarCtrl().EnableButton(ID_CTX_ADDAIRLINES);
}

void CDlgDBAirline::OnSaveAsTemplate()
{
	m_button_Load.SetOperation(1);
	m_button_Load.SetWindowText(_T("Save as Template"));

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

void CDlgDBAirline::OnLoadFromTemplate()
{
	m_button_Load.SetOperation(0);
	m_button_Load.SetWindowText(_T("Load from Template"));
	OnBnClickedButtonLoaddefaultAirline();
}

void CDlgDBAirline::OnClickMultiBtn()
{
	int type = m_button_Load.GetOperation() ;
	if (type == 0)
		OnLoadFromTemplate();
	if(type == 1)
		OnSaveAsTemplate();
}


void CDlgDBAirline::OnBnClickedButtonLoaddefaultAirline()
{
	// TODO: Add your control notification handler code here
	CAirportDatabase* DefaultDB = AIRPORTDBLIST->getAirportDBByName(m_pCurAirportDatabase->getName());
	if(DefaultDB == NULL)
	{
		CString msg ;
		msg.Format("%s AirportDB is not Exist in AirportDB template.",m_pCurAirportDatabase->getName()) ;
		MessageBox(msg,"Warning",MB_OK) ;
		return ;
	}
	m_pCurAirportDatabase->loadAirlineFromDatabase(DefaultDB);
	CString msg ;
	msg.Format("Load %s from %s AirportDB template DataBase successfully.",DefaultDB->getName(),DefaultDB->getName()) ;
	MessageBox(msg,NULL,MB_OK) ;

	m_Load_Operation = TRUE ;
    m_IsEdit = FALSE ;

		PopulateAirlineList();
		PopulateSubairlineList();
		if (m_lstSubAirlines.GetCount() > 0)
		{
			m_lstSubAirlines.SetCurSel(0);
			OnLbnSelchangeListSubairlines();
		}
}


void CDlgDBAirline::OnCancel()
{
	// TODO: Add your control notification handler code here
  if(m_IsEdit || m_Load_Operation)
	  m_pTerm->m_pAirportDB->loadAirlineFromDatabase(m_pCurAirportDatabase) ;

  CDialog::OnCancel() ;
}

CDlgDBAirlineForDefault::CDlgDBAirlineForDefault(BOOL bMenu,Terminal* _pTerm, CAirportDatabase* _theAirportDB,CWnd* pParent /* = NULL */)
	: CDlgDBAirline(bMenu,_pTerm,pParent)
{
	// retrieve the pointer
	m_pCurAirportDatabase = _pTerm?(_pTerm->m_pAirportDB):_theAirportDB;
	ASSERT(m_pCurAirportDatabase);

	CAirportDatabase* pDefaultDB = AIRPORTDBLIST->getAirportDBByName( m_pCurAirportDatabase->getName());
	ASSERT(pDefaultDB);
	m_pAirlinesMan = pDefaultDB->getAirlineMan();
	ASSERT(m_pAirlinesMan);

	if(!pDefaultDB->hadLoadDatabase())
		pDefaultDB->loadDatabase();
}
CDlgDBAirlineForDefault::~CDlgDBAirlineForDefault()
{

}
void CDlgDBAirlineForDefault::OnOK()
{
	try
	{
		CDatabaseADOConnetion::BeginTransaction(m_pCurAirportDatabase->GetAirportConnection()) ;
		if(m_pAirlinesMan&&m_pCurAirportDatabase)
		{
			m_pAirlinesMan->saveDatabase(m_pCurAirportDatabase) ;
		}
		CDatabaseADOConnetion::CommitTransaction(m_pCurAirportDatabase->GetAirportConnection()) ;
	}
	catch (CADOException& e)
	{
		e.ErrorMessage() ;
		CDatabaseADOConnetion::RollBackTransation(m_pCurAirportDatabase->GetAirportConnection()) ;
	}
 
	CDialog::OnOK() ;
}
void CDlgDBAirlineForDefault::OnCancel()
{
	if(m_pAirlinesMan&&m_pCurAirportDatabase)
		m_pAirlinesMan->loadDatabase(m_pCurAirportDatabase) ;
	CDialog::OnCancel() ;
}
BOOL CDlgDBAirlineForDefault::OnInitDialog()
{
	CDlgDBAirline::OnInitDialog() ;
// 	m_wndToolBarTop.ShowWindow(SW_HIDE);
// 	m_wndToolBarBottom.ShowWindow(SW_HIDE) ;
	m_button_Load.ShowWindow(SW_HIDE);
	m_btnCheck.ShowWindow(SW_HIDE);
//	m_StaticLoad.ShowWindow(SW_HIDE);
	SetWindowText("Global Airline");
	return TRUE ;
}
