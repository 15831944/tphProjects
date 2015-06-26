// DlgDBAircraftTypes.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"

#include "..\common\ACTypesManager.h"
#include "ACCatPropertiesDlg.h"
#include "DlgACTypeProperties.h"
#include "DlgDBAircraftTypes.h"
#include "FltSchedDefineDlg.h"
#include "engine\terminal.h"
#include "common\airportdatabase.h"
#include "assert.h"
#include <vector>
#include ".\dlgdbaircrafttypes.h"
#include "../Common/AirportDatabaseList.h"
#include "../Common/AcTypeAlias.h"
#include "../Common/SuperTypeRelationData.h"
#include "DlgSaveWarningWithCheckBox.h"
#include "../Database/ARCportADODatabase.h"
#include "AircraftPropertySheet.h"
#include "DlgActypeRelationShip.h"
#include <common/ARCUnit.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CDlgDBAircraftTypes dialog


CDlgDBAircraftTypes::CDlgDBAircraftTypes(BOOL bMenu,Terminal* _pTerm, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDBAircraftTypes::IDD, pParent), m_pAcMan( NULL )
	, m_bDragging(FALSE)
	, m_pDragImage(NULL)
	, m_nItemDragSrc(-1)
	, m_hItemDragDes(NULL)
	, m_ptLastMouse(-1, -1)
	, m_bMenu(bMenu)
	, m_CommitOperation(FALSE)
	, m_LoadDefaultOperation(FALSE)
	, m_IsEdited(FALSE)
	, m_pTerm(_pTerm)
{
	// implement our logic of getting m_pCurAirportDatabase and m_pAcMan
	if( m_pTerm  )
	{
		m_pCurAirportDatabase = m_pTerm->m_pAirportDB;
		ASSERT(m_pCurAirportDatabase);
		m_pAcMan = m_pCurAirportDatabase->getAcMan();
		ASSERT(m_pAcMan);
	}
	else
	{
		// m_bMenu should not be FALSE when _pTerm is NULL
		ASSERT(TRUE == m_bMenu);
		// here should initialize m_pCurAirportDatabase and m_pAcMan
		m_pCurAirportDatabase = NULL;
		m_pAcMan = NULL;
	}
}
CDlgDBAircraftTypes::~CDlgDBAircraftTypes()
{
	if (m_pDragImage)
	{
		delete m_pDragImage;
		m_pDragImage = NULL;
	}
}

void CDlgDBAircraftTypes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_LABLE, m_cstLable);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_LIST_ACTYPES, m_lstACTypes);
	DDX_Control(pDX, IDC_LIST_ACCATS, m_lstACCats);
	DDX_Control(pDX, IDC_LIST_CATEGORY, m_lstCategory);
	DDX_Control(pDX, IDC_STATIC_CATEGORY, m_lblCategory);
	DDX_Control(pDX, IDC_STATIC_MEMBER, m_lblMember);
	DDX_Control(pDX, IDC_BUTTON_LOADDEFAULT, m_button_LoadDefault);
	DDX_Control(pDX, IDCANCEL, m_buttonCancel);
	DDX_Control(pDX, IDC_CHECK_FLIGHTSHCEUDLE, m_btnCheck);
	//DDX_Control(pDX, IDC_STATIC_LOAD, m_Static_load);
}


BEGIN_MESSAGE_MAP(CDlgDBAircraftTypes, CDialog)
	ON_COMMAND(ID_CTX_NEWACCAT, OnNewACCat)
	ON_COMMAND(ID_CTX_NEWACTYPE, OnNewACType)
	ON_COMMAND(ID_CTX_EDITACTYPE, OnEditACType)
	ON_COMMAND(ID_CTX_EDITACCAT, OnEditACCat)
	ON_COMMAND(ID_CTX_DELETEACTYPE, OnDeleteACType)
	ON_COMMAND(ID_CTX_DELETEACCAT, OnDeleteACCat)
	ON_COMMAND(ID_AIRPORT_SAVEAS, OnSaveAsTemplate)
	ON_COMMAND(ID_AIRPORT_LOAD, OnLoadFromTemplate)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CLOSE()
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_ACTYPES, OnBegindragListActypes)
	ON_WM_CREATE()
	ON_LBN_SELCHANGE(IDC_LIST_CATEGORY, OnLbnSelchangeListCategory)
	ON_BN_CLICKED(IDC_BUTTON_LOADDEFAULT, OnBnClickedButtonLoaddefault)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ACTYPES, OnLvnItemchangedListActypes)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ACTYPES, OnClickListActypes)
	ON_BN_CLICKED(IDC_CHECK_FLIGHTSHCEUDLE, OnBnClickedCheckFlightSchedule)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDBAircraftTypes message handlers
//void CDlgDBAircraftTypes::OnRelationship()
//{
//	CDlgActypeRelationShip dlg(m_pAcMan,this);
//	dlg.DoModal();
//}

void CDlgDBAircraftTypes::OnBnClickedCheckFlightSchedule()
{
	m_bMenu =m_btnCheck.GetCheck()?FALSE:TRUE;
	PopulateACTypeList();
	if (m_lstCategory.GetCount() > 0)
	{
		m_lstCategory.SetCurSel(0);
		OnLbnSelchangeListCategory();
	}

}

void CDlgDBAircraftTypes::OnPaint()
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

BOOL CDlgDBAircraftTypes::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CDlgDBAircraftTypes::OnBegindragListActypes(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_nItemDragSrc = pNMListView->iItem;
	m_hItemDragDes = NULL;

	if (m_pDragImage)
	{
		delete m_pDragImage;
		m_pDragImage = NULL;
	}

	CPoint ptTemp(0,0);
	m_pDragImage = m_lstACTypes.CreateDragImage( m_nItemDragSrc,&ptTemp);
	if (!m_pDragImage)
		return;

	m_bDragging = true;
	m_pDragImage->BeginDrag ( 0,CPoint(8,8) );
	CPoint  pt = pNMListView->ptAction;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter ( GetDesktopWindow (),pt );  

	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	SetCapture();

	*pResult = 0;
}

void CDlgDBAircraftTypes::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_bDragging&&m_ptLastMouse!=point )
	{
		CPoint pt (point);
		ClientToScreen (&pt);

		// move the drag image
		VERIFY (m_pDragImage->DragMove (pt));
		// unlock window updates
		m_pDragImage->DragShowNolock (FALSE);
		//////////////////////////////////////////////////////
		CRect rectACCatsList;
		m_lstACCats.GetWindowRect(&rectACCatsList);
		int nItem = m_lstCategory.GetCurSel();
		if (rectACCatsList.PtInRect(pt) && nItem != LB_ERR)
		{
			CACCategory* pACC = m_pAcMan->getCatItem(nItem);
			if (!pACC->m_bHasScript)
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			else
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		}
		else
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		}

		m_pDragImage->DragShowNolock (TRUE);
		m_lstACTypes.Invalidate(FALSE);
		m_ptLastMouse=point;
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CDlgDBAircraftTypes::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bDragging)
	{
		CPoint pt (point);
		ClientToScreen (&pt);

		CRect rectACCatsList;
		m_lstACCats.GetWindowRect(&rectACCatsList);
		if (rectACCatsList.PtInRect(pt) && m_lstCategory.GetCount() > 0)
		{
			AddDraggedItem();
		}

		m_bDragging = false;
		ReleaseCapture();
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		// end dragging
		m_pDragImage->DragLeave (GetDesktopWindow ());
		m_pDragImage->EndDrag ();
		delete m_pDragImage;
		m_pDragImage = NULL;
	}

	CDialog::OnLButtonUp(nFlags, point);
}

int CDlgDBAircraftTypes::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

void CDlgDBAircraftTypes::InitColumnName()
{
	CString colnames;
	colnames.LoadString(IDS_ATD_COLNAMES);
	std::vector<CString> vNameSplit;
	//split the colnames into parts seperated by commas
	int first = -1, next = -1;
	while(TRUE) 
	{
		next = colnames.Find(',', first+1);
		if(next != -1)
			m_vNameSplit.push_back(colnames.Mid(first+1,next-first-1));
		else 
		{
			m_vNameSplit.push_back(colnames.Mid(first+1));
			break;
		}
		first = next;
	}
}

void CDlgDBAircraftTypes::InitACTypeListHeader()
{
	DWORD dwStyle = m_lstACTypes.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_lstACTypes.SetExtendedStyle( dwStyle );
	CRect listrect;
	m_lstACTypes.GetClientRect(&listrect);
	int nWidth = 50;//listrect.Width()/m_vNameSplit.size();
	for(int i=0; i<static_cast<int>(m_vNameSplit.size()); i++) 
	{
		int nListAlignFormat;
		if (i == 0 || i == 1 || i == 2)
		{
			nListAlignFormat = LVCFMT_LEFT;
			nWidth = 70;
			if(i == 2)
				nWidth = 90;
		}
		else
		{
			nListAlignFormat = LVCFMT_RIGHT;
			nWidth = 50;
		}
		m_lstACTypes.InsertColumn(i, m_vNameSplit[i], nListAlignFormat, nWidth);
	}

	m_lstACTypes.SetDataType(0,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstACTypes.SetDataType(1,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstACTypes.SetDataType(2,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstACTypes.SetDataType(3,CSortAndPrintListCtrl::ITEM_STRING);

	m_lstACTypes.SetDataType(4,CSortAndPrintListCtrl::ITEM_DOUBLE);
	m_lstACTypes.SetDataType(5,CSortAndPrintListCtrl::ITEM_DOUBLE);
	m_lstACTypes.SetDataType(6,CSortAndPrintListCtrl::ITEM_DOUBLE);
	m_lstACTypes.SetDataType(7,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstACTypes.SetDataType(8,CSortAndPrintListCtrl::ITEM_DOUBLE);
	m_lstACTypes.SetDataType(9,CSortAndPrintListCtrl::ITEM_DOUBLE);
	m_lstACTypes.SetDataType(10,CSortAndPrintListCtrl::ITEM_DOUBLE);
	m_lstACTypes.SetDataType(11,CSortAndPrintListCtrl::ITEM_DOUBLE);
	m_lstACTypes.SetDataType(12,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstACTypes.SetDataType(13,CSortAndPrintListCtrl::ITEM_INT);
	m_lstACTypes.SetDataType(14,CSortAndPrintListCtrl::ITEM_DOUBLE);
	m_lstACTypes.SetDataType(15,CSortAndPrintListCtrl::ITEM_DOUBLE);

	m_lstACTypes.InsertColumn(m_vNameSplit.size(), _T("Categories"),LVCFMT_LEFT, 100);
	m_lstACTypes.SetDataType(16,CSortAndPrintListCtrl::ITEM_STRING );
	m_lstACTypes.SubClassHeadCtrl();
}

void CDlgDBAircraftTypes::InitACatListHeader()
{
	DWORD dwStyle = m_lstACCats.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_lstACCats.SetExtendedStyle( dwStyle );
	CRect listrect;
	m_lstACCats.GetClientRect(&listrect);
	int nWidth = 50;//listrect.Width()/m_vNameSplit.size();
	for(int i=0; i<static_cast<int>(m_vNameSplit.size()); i++) 
	{
		int nListAlignFormat;
		if (i == 0 || i == 1 || i == 2)
		{
			nListAlignFormat = LVCFMT_LEFT;
			nWidth = 70;
			if(i == 2)
				nWidth = 90;
		}
		else
		{
			nListAlignFormat = LVCFMT_RIGHT;
			nWidth = 50;
		}
		m_lstACCats.InsertColumn(i, m_vNameSplit[i], nListAlignFormat, nWidth);
	}

	m_lstACCats.SetDataType(0,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstACCats.SetDataType(1,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstACCats.SetDataType(2,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstACCats.SetDataType(3,CSortAndPrintListCtrl::ITEM_STRING);

	m_lstACCats.SetDataType(4,CSortAndPrintListCtrl::ITEM_DOUBLE);
	m_lstACCats.SetDataType(5,CSortAndPrintListCtrl::ITEM_DOUBLE);
	m_lstACCats.SetDataType(6,CSortAndPrintListCtrl::ITEM_DOUBLE);
	m_lstACCats.SetDataType(7,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstACCats.SetDataType(8,CSortAndPrintListCtrl::ITEM_DOUBLE);
	m_lstACCats.SetDataType(9,CSortAndPrintListCtrl::ITEM_DOUBLE);
	m_lstACCats.SetDataType(10,CSortAndPrintListCtrl::ITEM_DOUBLE);
	m_lstACCats.SetDataType(11,CSortAndPrintListCtrl::ITEM_DOUBLE);
	m_lstACCats.SetDataType(12,CSortAndPrintListCtrl::ITEM_STRING);
	m_lstACCats.SetDataType(13,CSortAndPrintListCtrl::ITEM_INT);
	m_lstACCats.SetDataType(14,CSortAndPrintListCtrl::ITEM_DOUBLE);
	m_lstACCats.SetDataType(15,CSortAndPrintListCtrl::ITEM_DOUBLE);

	m_lstACCats.SubClassHeadCtrl();
}

BOOL CDlgDBAircraftTypes::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_wndToolBarTop.GetToolBarCtrl().SetCmdID(0,ID_CTX_NEWACTYPE);
	m_wndToolBarTop.GetToolBarCtrl().SetCmdID(1,ID_CTX_DELETEACTYPE);
	m_wndToolBarTop.GetToolBarCtrl().SetCmdID(2,ID_CTX_EDITACTYPE);
	
	m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_DELETEACTYPE,FALSE);
	m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_EDITACTYPE,FALSE);
	m_wndToolBarBottom.GetToolBarCtrl().SetCmdID(0,ID_CTX_NEWACCAT);
	m_wndToolBarBottom.GetToolBarCtrl().SetCmdID(1,ID_CTX_DELETEACCAT);
	m_wndToolBarBottom.GetToolBarCtrl().SetCmdID(2,ID_CTX_EDITACCAT);

	InitColumnName();
	InitACTypeListHeader();
	//now populate the list
	PopulateACTypeList();

	InitACatListHeader();
	PopulateACCatList();
	m_lstCategory.SetCurSel(0);
	OnLbnSelchangeListCategory();
	m_btnCheck.SetCheck(!m_bMenu);
	
	//	Init Cool button
	m_button_LoadDefault.SetOperation(0);
	m_button_LoadDefault.SetWindowText(_T("Database Template"));
	m_button_LoadDefault.SetOpenButton(FALSE);
	m_button_LoadDefault.SetType(CCoolBtn::TY_ACTYPE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}
void CDlgDBAircraftTypes::AddActypeItem(CACType* pACT)
{
	CString strHeight, strLength, strSpan, strMZFW, strOEW, strMTOW, strMLW, strCapacity, strCategories, strOtherCodes,strMinTurn;
	strHeight.Format(_T("%.2f"), pACT->m_fHeight);
	strLength.Format(_T("%.2f"), pACT->m_fLen);
	strSpan.Format(_T("%.2f"), pACT->m_fSpan);
	strMZFW.Format(_T("%.2f"), ARCUnit::KgsToLBS(pACT->m_fMZFW) );
	strOEW.Format(_T("%.2f"), ARCUnit::KgsToLBS(pACT->m_fOEW) );
	strMTOW.Format(_T("%.2f"), ARCUnit::KgsToLBS(pACT->m_fMTOW) );
	strMLW.Format(_T("%.2f"), ARCUnit::KgsToLBS(pACT->m_fMLW) );
	strCapacity.Format(_T("%d"), pACT->m_iCapacity);
	strMinTurn.Format(_T("%.2f"),pACT->m_fMinTurnRad);
	strOtherCodes = pACT->m_sOtherCodes;

	for (CACCATEGORYLIST::const_iterator iterCAT = m_pAcMan->GetACCategoryList()->begin();
		iterCAT != m_pAcMan->GetACCategoryList()->end(); iterCAT++)
	{
		CACTYPELIST ACTypeList;
		(*iterCAT)->GetACTypeList(ACTypeList);
		for (CACTYPELIST::const_iterator iterType = ACTypeList.begin();
			iterType != ACTypeList.end(); iterType++)
		{
			if (*(*iterType) == *pACT)
			{
				char szTemp[255];
				(*iterCAT)->getCatName(szTemp);
				if (!strCategories.IsEmpty())
					strCategories += _T(", ");
				strCategories += szTemp;
			}
		}
	}

	CString cabinwidth  ;
	cabinwidth.Format(_T("%0.2f") ,pACT->m_dCabinWidth ) ;
	int insertItem = m_lstACTypes.GetItemCount() ;
	m_lstACTypes.InsertItem(insertItem, pACT->GetIATACode());
	m_lstACTypes.SetItemText(insertItem, 1,pACT->m_sCompany);
	m_lstACTypes.SetItemText(insertItem, 2, pACT->m_sDescription);
	m_lstACTypes.SetItemText(insertItem, 3, pACT->m_sICAOGroup);
	m_lstACTypes.SetItemText(insertItem, 4, strHeight);
	m_lstACTypes.SetItemText(insertItem, 5, strLength);
	m_lstACTypes.SetItemText(insertItem, 6, strSpan);
	m_lstACTypes.SetItemText(insertItem, 7, pACT->m_sWakeVortexCategory);
	m_lstACTypes.SetItemText(insertItem, 8, strMZFW);
	m_lstACTypes.SetItemText(insertItem, 9, strOEW);
	m_lstACTypes.SetItemText(insertItem, 10, strMTOW);
	m_lstACTypes.SetItemText(insertItem, 11, strMLW);
	m_lstACTypes.SetItemText(insertItem, 12, pACT->m_sWeightCategory);
	m_lstACTypes.SetItemText(insertItem, 13, strCapacity);
	m_lstACTypes.SetItemText(insertItem, 14, strMinTurn);
	m_lstACTypes.SetItemText(insertItem , 15, cabinwidth);
	m_lstACTypes.SetItemData(insertItem , (LPARAM) pACT) ;
	if (pACT->IsEdit())
	{
		m_lstACTypes.SetItemColor(insertItem,::GetSysColor(COLOR_WINDOWFRAME),RGB(235,235,235));
	}

	pACT->Update();
}

void CDlgDBAircraftTypes::EditActypeItem(int selItem,CACType* pACT)
{
	CString strHeight, strLength, strSpan, strMZFW, strOEW, strMTOW, strMLW, strCapacity, strCategories, strOtherCodes,strMinTurn;
	strHeight.Format(_T("%.2f"), pACT->m_fHeight);
	strLength.Format(_T("%.2f"), pACT->m_fLen);
	strSpan.Format(_T("%.2f"), pACT->m_fSpan);
	strMZFW.Format(_T("%.2f"), ARCUnit::KgsToLBS(pACT->m_fMZFW) );
	strOEW.Format(_T("%.2f"),ARCUnit::KgsToLBS( pACT->m_fOEW) );
	strMTOW.Format(_T("%.2f"), ARCUnit::KgsToLBS(pACT->m_fMTOW) );
	strMLW.Format(_T("%.2f"), ARCUnit::KgsToLBS(pACT->m_fMLW) );
	strCapacity.Format(_T("%d"), pACT->m_iCapacity);
	strMinTurn.Format(_T("%.2f"),pACT->m_fMinTurnRad);
	strOtherCodes = pACT->m_sOtherCodes;

	CString cabinwidth  ;
	cabinwidth.Format(_T("%0.2f") ,pACT->m_dCabinWidth ) ;
	m_lstACTypes.SetItemText(selItem, 0, pACT->GetIATACode());
	m_lstACTypes.SetItemText(selItem, 1,pACT->m_sCompany);
	m_lstACTypes.SetItemText(selItem, 2, pACT->m_sDescription);
	m_lstACTypes.SetItemText(selItem, 3, pACT->m_sICAOGroup);
	m_lstACTypes.SetItemText(selItem, 4, strHeight);
	m_lstACTypes.SetItemText(selItem, 5, strLength);
	m_lstACTypes.SetItemText(selItem, 6, strSpan);
	m_lstACTypes.SetItemText(selItem, 7, pACT->m_sWakeVortexCategory);
	m_lstACTypes.SetItemText(selItem, 8, strMZFW);
	m_lstACTypes.SetItemText(selItem, 9, strOEW);
	m_lstACTypes.SetItemText(selItem, 10, strMTOW);
	m_lstACTypes.SetItemText(selItem, 11, strMLW);
	m_lstACTypes.SetItemText(selItem, 12, pACT->m_sWeightCategory);
	m_lstACTypes.SetItemText(selItem, 13, strCapacity);
	m_lstACTypes.SetItemText(selItem, 14, strMinTurn);
	m_lstACTypes.SetItemText(selItem , 15, cabinwidth);
	m_lstACTypes.SetItemData(selItem, (LPARAM) pACT);
	if (pACT->IsEdit())
	{
		m_lstACTypes.SetItemColor(selItem,::GetSysColor(COLOR_WINDOWFRAME),RGB(235,235,235));
	}
	pACT->Update();
}

void CDlgDBAircraftTypes::PopulateACTypeList()
{
	if(m_pAcMan == NULL)
		return ;
	m_lstACTypes.DeleteAllItems();
	CACTYPELIST* pvList;
	pvList = m_pAcMan->GetACTypeList();
	char str[128];
	int k = 0;
	int ndx = 0 ;
	if (m_bMenu)
	{
		for(int i=0; i<static_cast<int>(pvList->size()); i++)
		{
			CACType* pACT = (*pvList)[i];
			AddActypeItem(pACT) ;
		}
	}
	else
	{
		for(int i=0; i<static_cast<int>(pvList->size()); i++)
		{
			CACType* pACT = (*pvList)[i];
			for (int j=0;j<m_pTerm->flightSchedule->getCount();j++)
			{
				Flight* pFlight = m_pTerm->flightSchedule->getItem(j);
				pFlight->getACType(str);
				if (strcmp(str,pACT->GetIATACode())==0)
				{
					AddActypeItem(pACT) ;
					break;
				}
			}
		}
	}
}

void CDlgDBAircraftTypes::PopulateACCatList()
{
	if(m_pAcMan == NULL)
		return ;
	CACCATEGORYLIST* pvACCList;
	pvACCList = m_pAcMan->GetACCategoryList();

	m_lstCategory.ResetContent();
	int nCurItem = 0;
	for (CACCATEGORYLIST::const_iterator iter = pvACCList->begin();
		 iter != pvACCList->end(); iter ++)
	{
		CACCategory* pACC = *iter;
		m_lstCategory.SetItemData(m_lstCategory.InsertString(nCurItem, pACC->m_sName), (DWORD)pACC);
		nCurItem ++;
	}
}

void CDlgDBAircraftTypes::FreeItemMemory()
{
}

void CDlgDBAircraftTypes::OnNewACCat() 
{
	int nCount = m_pAcMan->GetACCategoryList()->size();
	CString sName;
	sName.Format("New AC Category %d", nCount);
	CACCategory* pACC = new CACCategory(m_pAcMan);
	pACC->m_sName = sName;
	CDlgACCatProperties dlg(m_pAcMan, pACC);
	if(dlg.DoModal() == IDOK)
	{
		m_pAcMan->GetACCategoryList()->push_back(pACC);
		int nCurItem = m_lstCategory.InsertString(m_lstCategory.GetCount(), pACC->m_sName);
		m_lstCategory.SetItemData(nCurItem, (DWORD)pACC);
		m_lstCategory.SetCurSel(nCurItem);
		OnLbnSelchangeListCategory();
		m_IsEdited = TRUE ;
	}
	else
	{
		delete pACC;
	}
}

void CDlgDBAircraftTypes::OnNewACType() 
{
	int nCount = m_pAcMan->GetACTypeList()->size();
	CString sName;
	sName.Format("New Aircraft %d", nCount);
	CACType* pACT = new CACType();
	pACT->m_sDescription = sName;
	
	CAircraftPropertySheet dlg(_T("Aircraft property"),this);
	dlg.setACType(pACT);
	if(dlg.DoModal() == IDOK) {
		//check to make sure this aircraft doesn't already exist

		std::vector<CString> vListACT;
		vListACT.push_back(pACT->GetIATACode()) ;
		if( m_pAcMan->FilterActypeByStrings(vListACT) == NULL) {
			m_pAcMan->GetACTypeList()->push_back(pACT);
			AddActypeItem(pACT) ;
			m_IsEdited = TRUE ;
			//check it is exist in relationship 
			if(m_pAcMan->GetActypeRelationShip() == NULL)
				return ;
		   CAcTypeAlias* AliasName =	m_pAcMan->GetActypeRelationShip()->FindAcAliasNode(pACT->GetIATACode()) ;
           if(AliasName != NULL)
		   {
			   if(AliasName->GetActypeInstance() == NULL)
				   AliasName->SetActypeInstance(pACT) ;
			   else
			   {
					AliasName->RemoveAliasName(AliasName->GetActypeInstance()->GetIATACode()) ;
                    AliasName->SetActypeInstance(pACT) ;
			   }
			
		   }
			return;
		}
		else {
			CString msg;
			msg.Format("An aircraft with name %s  already exists.\nOperation cancelled.", pACT->GetIATACode());
			AfxMessageBox(msg);
		}
	}
	delete pACT;
}

void CDlgDBAircraftTypes::OnEditACType() 
{
	POSITION pos = m_lstACTypes.GetFirstSelectedItemPosition();
	int sel = -1;
	if(pos)
		sel = m_lstACTypes.GetNextSelectedItem(pos);
	else
		return;

	if (sel != -1)
	{
		CACType* pACT = (CACType*) m_lstACTypes.GetItemData(sel);
		CString Actypename = pACT->GetIATACode() ;
		CAircraftPropertySheet dlg(_T("Aircraft property"),this);
		dlg.setACType(pACT);
		if (dlg.DoModal() == IDOK)
		{
				std::vector<CString> vListACT;
				vListACT.push_back(pACT->GetIATACode()) ;
				CACType* FindActype =	m_pAcMan->FilterActypeByStrings(vListACT) ;

				if(FindActype == NULL || FindActype == pACT) {
					//edit arc data
					if (pACT->IsEdit())
					{
						pACT->IsEdit(FALSE);
						pACT->SetID(-1);
						pACT->ResetDoorID();
						if (Actypename != pACT->GetIATACode())
						{	
							CARCProjectDatabase* pAirportDatabase = (CARCProjectDatabase*)m_pCurAirportDatabase;
							CACType* pArcActype = pAirportDatabase->GetArcDatabaseActype(Actypename);
							if (pArcActype)
							{
								CACType* pData = new CACType(*pArcActype);
								pData->IsEdit(TRUE);
								m_pAcMan->GetACTypeList()->push_back(pData);
							}
						}
					}

					EditActypeItem(sel,pACT) ;
					m_IsEdited = TRUE ;
					//check it is exist in relationship 
					if(m_pAcMan->GetActypeRelationShip() == NULL)
						return ;
					CAcTypeAlias* AliasName =	m_pAcMan->GetActypeRelationShip()->FindAcAliasNode(pACT->GetIATACode()) ;
					if(AliasName != NULL)
					{
						if(AliasName->GetActypeInstance() == NULL)
							AliasName->SetActypeInstance(pACT) ;
						else
						{
							AliasName->RemoveAliasName(AliasName->GetActypeInstance()->GetIATACode()) ;
							AliasName->SetActypeInstance(pACT) ;
						}

					}
				}else
				{
					CString msg;
					msg.Format("An aircraft with name %s  already exists.\nOperation cancelled.", FindActype->GetIATACode());
					AfxMessageBox(msg);
                    pACT->setIATACode(Actypename) ;
				}
            /* To do: Entry is not updated in UI. Fix. */
		}
	}
	
}

void CDlgDBAircraftTypes::OnEditACCat() 
{
	int iCurSel = m_lstCategory.GetCurSel();
	if (iCurSel == LB_ERR)
		return;

	CACCategory* pACC = (CACCategory*)m_lstCategory.GetItemData(iCurSel);
	CDlgACCatProperties dlg(m_pAcMan, pACC);
	if (dlg.DoModal() == IDOK)
	{
		PopulateACCatList();
		//PopulateACTypeList();
		m_lstCategory.SetCurSel(iCurSel);
		OnLbnSelchangeListCategory();
		m_IsEdited = TRUE ;
	}
}

void CDlgDBAircraftTypes::OnDeleteACType() 
{
	if(AfxMessageBox(_T("Are you sure?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		POSITION pos = m_lstACTypes.GetFirstSelectedItemPosition();
		int sel = -1;
		if(!pos) return;
		while (pos)
		{
			sel = m_lstACTypes.GetNextSelectedItem(pos);
			if(sel != -1)
			{
				CACType* pACT = (CACType*) m_lstACTypes.GetItemData(sel);
				CARCProjectDatabase* pAirportDatabase = (CARCProjectDatabase*)m_pCurAirportDatabase;
				CACType* pArcActype = pAirportDatabase->GetArcDatabaseActype(pACT->GetIATACode());
				if (pArcActype)
				{
					CACType* pData = new CACType(*pArcActype);
					pData->IsEdit(TRUE);
					m_pAcMan->GetACTypeList()->push_back(pData);
				}
				m_pAcMan->RemoveActype(pACT);

				if(m_pAcMan->GetActypeRelationShip() != NULL)
				{
					CAcTypeAlias* AliasName =	m_pAcMan->GetActypeRelationShip()->FindAcAliasNode(pACT->GetIATACode()) ;
					if(AliasName != NULL)
					{
						if(AliasName->GetActypeInstance() == pACT)
						{
							AliasName->SetActypeInstance(NULL) ;
						}
					}
				}
			}
		}
		m_IsEdited = TRUE ;
		PopulateACTypeList();
	}
	
}

void CDlgDBAircraftTypes::OnDeleteACCat() 
{
	int iCurSel = m_lstCategory.GetCurSel();
	if (iCurSel == LB_ERR)
		return;

	if(AfxMessageBox(_T("Are you sure?"), MB_YESNO|MB_ICONQUESTION) == IDYES)
	{
		//CACCategory* pACC = m_pAcMan->getCatItem(iCurSel);
		CACCategory* pACC = (CACCategory*)m_lstCategory.GetItemData(iCurSel);
        m_pAcMan->RemoveCategories(pACC) ;
		//CACCATEGORYLIST* pAllACC = m_pAcMan->GetACCategoryList();
		//for(int i = 0; i < static_cast<int>(pAllACC->size()); i++)
		//{
		//	CACCategory* pTemACC = pAllACC->at(i);
		//	if(pTemACC == pACC)
		//	{
		//		pAllACC->erase(pAllACC->begin() + i);
		//		delete pACC;
		//		//m_tabCtrl.DeleteItem(i);
		//		break;
		//	}
		//}
		PopulateACCatList();
		
		m_lstCategory.SetCurSel(0);
		OnLbnSelchangeListCategory();
		m_IsEdited = TRUE ;
	}
}

void CDlgDBAircraftTypes::OnOK() 
{
	CAirportDatabase* DefaultDB = AIRPORTDBLIST->getAirportDBByName(m_pCurAirportDatabase->getName());
	if(m_IsEdited)
	{
		// write data to local firstly
		try
		{
			CDatabaseADOConnetion::BeginTransaction(m_pCurAirportDatabase->GetAirportConnection());
			if(m_LoadDefaultOperation)
			{
				m_pCurAirportDatabase->getAcMan()->deleteDatabase(m_pCurAirportDatabase);
				m_pCurAirportDatabase->getAcMan()->ReSetAllID();
				m_pCurAirportDatabase->getAcMan()->saveDatabase(m_pCurAirportDatabase);
			}
			else
			{
				m_pCurAirportDatabase->getAcMan()->saveDatabase(m_pCurAirportDatabase);
			}
			m_pCurAirportDatabase->getAcMan()->ClacCategoryACTypeList();
			CDatabaseADOConnetion::CommitTransaction(m_pCurAirportDatabase->GetAirportConnection());
		}
		catch (CADOException& e)
		{
			e.ErrorMessage() ;
			CDatabaseADOConnetion::RollBackTransation(m_pCurAirportDatabase->GetAirportConnection());
		}
	}
	if(m_IsEdited == FALSE && m_LoadDefaultOperation)
	{
		try
		{
			CDatabaseADOConnetion::BeginTransaction(m_pCurAirportDatabase->GetAirportConnection());
			m_pCurAirportDatabase->getAcMan()->deleteDatabase(m_pCurAirportDatabase);
			m_pCurAirportDatabase->getAcMan()->ReSetAllID();
			m_pCurAirportDatabase->getAcMan()->saveDatabase(m_pCurAirportDatabase);
			CDatabaseADOConnetion::CommitTransaction(m_pCurAirportDatabase->GetAirportConnection());
		}
		catch (CADOException& e)
		{
			e.ErrorMessage() ;
			CDatabaseADOConnetion::RollBackTransation(m_pCurAirportDatabase->GetAirportConnection());
		}
	}
	CDialog::OnOK();
}

void CDlgDBAircraftTypes::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	int nMargin=7;
	int nMargin2=nMargin*2;
	CRect rc;
	CRect btnrc;
	m_wndToolBarTop.SetWindowPos(NULL,nMargin,26,cx-nMargin2,26,NULL);
	m_wndToolBarTop.GetWindowRect(rc);
	ScreenToClient(rc);

	int nHeightTop=cy/3;
	m_lstACTypes.SetWindowPos( NULL,rc.left,rc.bottom,rc.Width(),nHeightTop,NULL);
	m_lstACTypes.GetWindowRect(rc);
	ScreenToClient(rc);

	m_wndSplitter.SetWindowPos(NULL,rc.left,rc.bottom,rc.Width(),nMargin,SWP_NOACTIVATE);
	m_wndSplitter.SetRange(60,cy-100);
	m_wndSplitter.GetWindowRect(rc);
	ScreenToClient(rc);
	
	m_cstLable.GetClientRect(btnrc);
	int nHeightWnd=btnrc.Height();

	m_cstLable.SetWindowPos(NULL,rc.left,rc.bottom,rc.Width(),nHeightWnd,NULL);
	m_cstLable.GetWindowRect(rc);
	ScreenToClient(rc);

	m_wndToolBarBottom.SetWindowPos(NULL,rc.left,rc.bottom+1,rc.Width(),26,NULL);
	m_wndToolBarBottom.GetWindowRect(rc);
	ScreenToClient(rc);

	m_lblCategory.SetWindowPos(NULL,rc.left,rc.bottom ,150, 20,NULL);
	m_lblMember.SetWindowPos(NULL,rc.left + 150,rc.bottom ,rc.Width() - 150, 20,NULL);

	m_lstCategory.SetWindowPos(NULL,rc.left,rc.bottom + 20,145,cy-38-nMargin-rc.bottom - 20,NULL);
	m_lstACCats.SetWindowPos(NULL,rc.left + 150,rc.bottom + 20,rc.Width() - 150,cy-38-nMargin-rc.bottom - 20,NULL);
	m_lstACCats.GetWindowRect(&rc);
	ScreenToClient( &rc );
	m_buttonCancel.GetWindowRect( &btnrc );
	m_buttonCancel.MoveWindow( rc.right-btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
    int width = btnrc.Width() ;
	m_btnOk.GetWindowRect( &btnrc );
    m_btnOk.MoveWindow( rc.right-btnrc.Width()-25-width,cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );

	m_wndToolBarTop.GetWindowRect(rc);
	ScreenToClient(rc);
	m_button_LoadDefault.GetWindowRect(&btnrc);
    m_button_LoadDefault.MoveWindow( rc.right- btnrc.Width()-45-2*width,cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );

	m_btnCheck.GetWindowRect(&btnrc);
	ScreenToClient(&btnrc);
	m_btnCheck.MoveWindow(rc.left,cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height());
}

LRESULT CDlgDBAircraftTypes::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
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

void CDlgDBAircraftTypes::DoResize(int nDelta)
{
	CSplitterControl::ChangeHeight(&m_lstACTypes,nDelta,CW_TOPALIGN);
	CSplitterControl::ChangeHeight(&m_lstCategory,-nDelta,CW_BOTTOMALIGN);
	CSplitterControl::ChangeHeight(&m_lstACCats,-nDelta,CW_BOTTOMALIGN);
	CRect rect;
	m_cstLable.GetWindowRect(rect);
	ScreenToClient(rect);
	m_cstLable.SetWindowPos(NULL,rect.left,rect.top+nDelta,0,0,SWP_NOSIZE);

	CRect caterect;
	m_lblCategory.GetWindowRect(caterect);
	ScreenToClient(caterect);
	m_lblCategory.SetWindowPos(NULL,caterect.left,caterect.top+nDelta,0,0,SWP_NOSIZE);

	CRect memrect;
	m_lblMember.GetWindowRect(memrect);
	ScreenToClient(memrect);
	m_lblMember.SetWindowPos(NULL,memrect.left,memrect.top+nDelta,0,0,SWP_NOSIZE);

	m_wndToolBarBottom.GetWindowRect(rect);
	ScreenToClient(rect);
	m_wndToolBarBottom.SetWindowPos(NULL,rect.left,rect.top+nDelta,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);

// 	ScreenToClient(rect);
// 	CRect rectTabCtrlItem;
// 	int iTabHeaderHeight = 0;
// 	m_lstACCats.SetWindowPos(NULL, rect.left + 8, rect.top + iTabHeaderHeight + 8, 
// 							 rect.Width() - 16, rect.Height() - iTabHeaderHeight - 16, NULL);
	
	Invalidate();
	
}

void CDlgDBAircraftTypes::AddDraggedItem()
{
	int nCatItem = m_lstCategory.GetCurSel();
	if (nCatItem == LB_ERR)
		return;

	POSITION pos = m_lstACTypes.GetFirstSelectedItemPosition();
	while (pos)
 	{
		int nItem = m_lstACTypes.GetNextSelectedItem(pos);
 		CACCategory* pACC = m_pAcMan->getCatItem(nCatItem);
 		if (!pACC->m_bHasScript)
		{
			CACType* Pactype = (CACType*)m_lstACTypes.GetItemData(nItem) ;
			if(Pactype)
 				pACC->m_vDirectACTypes.push_back(Pactype->GetIATACode());
		}
 	}
    m_IsEdited = TRUE ;
	//PopulateACCatList();
	OnLbnSelchangeListCategory();
}

void CDlgDBAircraftTypes::OnClickListActypes(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nItem=pNMListView->iItem;
	if(nItem!=-1)
	{
		CACType* pACtype = (CACType*) m_lstACTypes.GetItemData(nItem);
		if (pACtype && pACtype->IsEdit())
		{
			m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_DELETEACTYPE,FALSE);
			m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_EDITACTYPE,TRUE);
		}
		else
		{
			m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_DELETEACTYPE,TRUE);
			m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_EDITACTYPE,TRUE);
		}
		
	}
	else
	{
		m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_DELETEACTYPE,FALSE);
		m_wndToolBarTop.GetToolBarCtrl().EnableButton(ID_CTX_EDITACTYPE,FALSE);
	}
	*pResult = 0;
}

void CDlgDBAircraftTypes::setAcManager( CACTypesManager* _pAcMan )
{
	assert( _pAcMan );
	m_pAcMan = _pAcMan;
}

void CDlgDBAircraftTypes::OnColumnclickAcTypes(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
// 	int nTestIndex = pNMListView->iSubItem;
// 	if( nTestIndex >= 0 )
// 	{
// 		CWaitCursor	wCursor;
// 		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
// 			m_HeaderCtrlACTypes.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
// 		else
// 			m_HeaderCtrlACTypes.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
// 		m_HeaderCtrlACTypes.SaveSortList();
// 	}	
	*pResult = 0;
}

void CDlgDBAircraftTypes::OnColumnclickAcCats(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
// 	int nTestIndex = pNMListView->iSubItem;
// 	if( nTestIndex >= 0 )
// 	{
// 		CWaitCursor	wCursor;
// 		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
// 			m_HeaderCtrlACCats.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
// 		else
// 			m_HeaderCtrlACCats.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
// 		m_HeaderCtrlACCats.SaveSortList();
// 	}	
	*pResult = 0;
}

void CDlgDBAircraftTypes::OnLbnSelchangeListCategory()
{
	m_lstACCats.DeleteAllItems();

	int iCurSel = m_lstCategory.GetCurSel();
	if (iCurSel == LB_ERR)
		return;

	char str[128];
	int k = 0;
	CACCategory* pACCatrgory = m_pAcMan->getCatItem(iCurSel);
	pACCatrgory->ClacACTypeList();
	CACTYPELIST ACTList;
	pACCatrgory->GetACTypeList(ACTList);
	if (m_bMenu)
	{
		for (int i = 0; i < static_cast<int>(ACTList.size()); i++)
		{
			CACType* pACT = ACTList[i];
			CString strHeight, strLength, strSpan, strMZFW, strOEW, strMTOW, strMLW, strCapacity, strOtherCodes,strMinTurn,strCabinWidth;

			strHeight.Format(_T("%.2f"), pACT->m_fHeight);
			strLength.Format(_T("%.2f"), pACT->m_fLen);
			strSpan.Format(_T("%.2f"), pACT->m_fSpan);
			strMZFW.Format(_T("%.2f"), ARCUnit::KgsToLBS(pACT->m_fMZFW) );
			strOEW.Format(_T("%.2f"), ARCUnit::KgsToLBS(pACT->m_fOEW) );
			strMTOW.Format(_T("%.2f"), ARCUnit::KgsToLBS(pACT->m_fMTOW) );
			strMLW.Format(_T("%.2f"), ARCUnit::KgsToLBS(pACT->m_fMLW) );
			strCapacity.Format(_T("%d"), pACT->m_iCapacity);
			strMinTurn.Format(_T("%.2f"),pACT->m_fMinTurnRad);
			strCabinWidth.Format(_T("%.2f"),pACT->m_dCabinWidth);
			strOtherCodes = pACT->m_sOtherCodes;


			m_lstACCats.InsertItem(i, pACT->GetIATACode());
			m_lstACCats.SetItemText(i, 1,pACT->m_sCompany);
			m_lstACCats.SetItemText(i, 2, pACT->m_sDescription);
			m_lstACCats.SetItemText(i, 3, pACT->m_sICAOGroup);
			m_lstACCats.SetItemText(i, 4, strHeight);
			m_lstACCats.SetItemText(i, 5, strLength);
			m_lstACCats.SetItemText(i, 6, strSpan);
			m_lstACCats.SetItemText(i, 7, pACT->m_sWakeVortexCategory);
			m_lstACCats.SetItemText(i, 8, strMZFW);
			m_lstACCats.SetItemText(i, 9, strOEW);
			m_lstACCats.SetItemText(i, 10, strMTOW);
			m_lstACCats.SetItemText(i, 11, strMLW);
			m_lstACCats.SetItemText(i, 12, pACT->m_sWeightCategory);
			m_lstACCats.SetItemText(i, 13, strCapacity);
			m_lstACCats.SetItemText(i, 14, strMinTurn);
			m_lstACCats.SetItemText(i, 15, strCabinWidth);

		}
	}
	else
	{
		for(int i=0; i<static_cast<int>(ACTList.size()); i++)
		{
			CACType* pACT = (ACTList)[i];
			for (int j=0;j<m_pTerm->flightSchedule->getCount();j++)
			{
				BOOL bSame = FALSE;
				Flight* pFlight = m_pTerm->flightSchedule->getItem(j);
				pFlight->getACType(str);
				if (strcmp(str,pACT->GetIATACode())==0)
				{
					CString strHeight, strLength, strSpan, strMZFW, strOEW, strMTOW, strMLW, strCapacity, strCategories, strOtherCodes,strCabinWidth,strMinTurn;
					strHeight.Format(_T("%.2f"), pACT->m_fHeight);
					strLength.Format(_T("%.2f"), pACT->m_fLen);
					strSpan.Format(_T("%.2f"), pACT->m_fSpan);
					strMZFW.Format(_T("%.2f"), ARCUnit::KgsToLBS(pACT->m_fMZFW) );
					strOEW.Format(_T("%.2f"), ARCUnit::KgsToLBS(pACT->m_fOEW) );
					strMTOW.Format(_T("%.2f"), ARCUnit::KgsToLBS(pACT->m_fMTOW) );
					strMLW.Format(_T("%.2f"), ARCUnit::KgsToLBS(pACT->m_fMLW) );
					strCapacity.Format(_T("%d"), pACT->m_iCapacity);
					strMinTurn.Format(_T("%.2f"),pACT->m_fMinTurnRad);
					strCabinWidth.Format(_T("%.2f"),pACT->m_dCabinWidth);
					strOtherCodes = pACT->m_sOtherCodes;
					//CStringArray strArray;
				
					int ndx= 0 ;
					ndx =  m_lstACCats.InsertItem(ndx, pACT->GetIATACode());
					m_lstACCats.SetItemText(ndx, 1,pACT->m_sCompany);
					m_lstACCats.SetItemText(ndx, 2, pACT->m_sDescription);
					m_lstACCats.SetItemText(ndx, 3, pACT->m_sICAOGroup);
					m_lstACCats.SetItemText(ndx, 4, strHeight);
					m_lstACCats.SetItemText(ndx, 5, strLength);
					m_lstACCats.SetItemText(ndx, 6, strSpan);
					m_lstACCats.SetItemText(ndx, 7, pACT->m_sWakeVortexCategory);
					m_lstACCats.SetItemText(ndx, 8, strMZFW);
					m_lstACCats.SetItemText(ndx, 9, strOEW);
					m_lstACCats.SetItemText(ndx, 10, strMTOW);
					m_lstACCats.SetItemText(ndx, 11, strMLW);
					m_lstACCats.SetItemText(ndx, 12, pACT->m_sWeightCategory);
					m_lstACCats.SetItemText(ndx, 13, strCapacity);
					m_lstACCats.SetItemText(ndx, 14, strMinTurn);
					m_lstACCats.SetItemText(ndx, 15, strCabinWidth);

					if(!m_lstACCats.SetItemData(ndx, (LPARAM) pACT))
						MessageBox("SetItemData failed.");
					break;
				}
			}
		}
	}
}

void CDlgDBAircraftTypes::OnBnClickedButtonLoaddefault()
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

   m_pCurAirportDatabase->loadAircraftFromDatabase(DefaultDB) ;
   CString msg ;
    msg.Format("Load %s from %s AirportDB template successfully.",DefaultDB->getName(),DefaultDB->getName()) ;
    MessageBox(msg,NULL,MB_OK) ;
	m_IsEdited = FALSE ;
	m_LoadDefaultOperation = TRUE ;
	PopulateACTypeList();
	PopulateACCatList() ;
}

void CDlgDBAircraftTypes::OnLoadFromTemplate()
{
	m_button_LoadDefault.SetOperation(0);
	m_button_LoadDefault.SetWindowText(_T("Load from Template"));
	OnBnClickedButtonLoaddefault();
}

void CDlgDBAircraftTypes::OnSaveAsTemplate()
{
	m_button_LoadDefault.SetOperation(1);
	m_button_LoadDefault.SetWindowText(_T("Save as Template"));

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

void CDlgDBAircraftTypes::OnClickMultiBtn()
{
	int type = m_button_LoadDefault.GetOperation() ;
	if (type == 0)
		OnLoadFromTemplate();
	if(type == 1)
		OnSaveAsTemplate();
}

void CDlgDBAircraftTypes::OnCancel()
{
	if(m_IsEdited || m_LoadDefaultOperation)
		m_pCurAirportDatabase->loadAircraftFromDatabase(m_pCurAirportDatabase);	
	CDialog::OnCancel() ;
}

void CDlgDBAircraftTypes::OnLvnItemchangedListActypes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


//////////////////////////////////////////////////////////////////////////
//
// class CDlgDBAircraftTypesForDefault
//
CDlgDBAircraftTypesForDefault::CDlgDBAircraftTypesForDefault(BOOL bMenu,Terminal* _pTerm,CAirportDatabase* _theAirportDB, CWnd* pParent /* = NULL */)
	: CDlgDBAircraftTypes(bMenu,_pTerm,pParent)
{
	// retrieve the pointer
	m_pCurAirportDatabase = _pTerm?(_pTerm->m_pAirportDB):_theAirportDB;
	ASSERT(m_pCurAirportDatabase);

	CAirportDatabase* pDefaultDB = AIRPORTDBLIST->getAirportDBByName( m_pCurAirportDatabase->getName());
	ASSERT(pDefaultDB);
	m_pAcMan = pDefaultDB->getAcMan();
	ASSERT(m_pAcMan);

	if(!pDefaultDB->hadLoadDatabase())
		pDefaultDB->loadDatabase();
}
CDlgDBAircraftTypesForDefault::~CDlgDBAircraftTypesForDefault()
{

}


BOOL CDlgDBAircraftTypesForDefault::OnInitDialog()
{
	if(m_pAcMan == NULL || m_pCurAirportDatabase == NULL)
		return FALSE ;

	CDlgDBAircraftTypes::OnInitDialog();
	m_wndToolBarTop.GetToolBarCtrl().ShowWindow(SW_HIDE) ;
	m_wndToolBarBottom.GetToolBarCtrl().ShowWindow(SW_HIDE) ;
	m_button_LoadDefault.ShowWindow(SW_HIDE) ;
	m_btnCheck.ShowWindow(SW_HIDE);
//	m_Static_load.ShowWindow(SW_HIDE);
	SetWindowText("Global Aircraft Types") ;
	return TRUE ;
}


void CDlgDBAircraftTypesForDefault::OnOK()
{
	try
	{
		if (m_pAcMan)
		{
			CDatabaseADOConnetion::BeginTransaction(m_pCurAirportDatabase->GetAirportConnection()) ;
			m_pAcMan->saveDatabase(m_pCurAirportDatabase) ;
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


void CDlgDBAircraftTypesForDefault::OnCancel()
{
	if(m_pAcMan)
	{
		m_pAcMan->loadDatabase(m_pCurAirportDatabase);
	}
	CDialog::OnCancel() ;
}