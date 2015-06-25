#include "stdafx.h"
#include "Resource.h"
#include "DlgFlightRelativeServiceLocations.h"
#include "../InputAirside/FlightTypeDetailsManager.h"
#include "../InputAirside/FlightTypeDetailsItem.h"
#include "../Common/AirportDatabase.h"

#include "../InputAirside/VehicleSpecifications.h"
#include ".\dlgflightrelativeservicelocations.h"

#include "DragDropImpl.h"
#include "../Database//DBElementCollection_Impl.h"

//-------------------------------------------------------------------------------------------------------
//CFltRelativeIconListCtrl
#include <algorithm>
IMPLEMENT_DYNAMIC(CFltRelativeIconListCtrl, CListCtrl)

CFltRelativeIconListCtrl::CFltRelativeIconListCtrl()
{
	//icons map 
	m_mapElementIcon.insert( std::make_pair(_T("Fuel truck"), LoadIcon(IDI_ICON_FUELING) ) );
	m_mapElementIcon.insert( std::make_pair(_T("Catering truck"), LoadIcon(IDI_ICON_CATERINGTRUCK) ) );
	m_mapElementIcon.insert( std::make_pair(_T("Baggage tug"), LoadIcon(IDI_ICON_BAGGAGETUGS) ) );
	m_mapElementIcon.insert( std::make_pair(_T("Baggage cart"), LoadIcon(IDI_ICON_BAGGAGECARS) ) );
	m_mapElementIcon.insert( std::make_pair(_T("Maintenance truck"), LoadIcon(IDI_ICON_MAINTENANCE) ) );
	m_mapElementIcon.insert( std::make_pair(_T("Lift"), LoadIcon(IDI_ICON_LIFTS) ) );
	m_mapElementIcon.insert( std::make_pair(_T("Cargo tug"), LoadIcon(IDI_ICON_CARGOTUGS) ) );
	m_mapElementIcon.insert( std::make_pair(_T("Cargo ULD cart"), LoadIcon(IDI_ICON_CARGOULDCARTS) ) );
	m_mapElementIcon.insert( std::make_pair(_T("Conveyor"), LoadIcon(IDI_ICON_CONVEYORS) ) );
	m_mapElementIcon.insert( std::make_pair(_T("Stairs"), LoadIcon(IDI_ICON_STAIRS) ) );
	m_mapElementIcon.insert( std::make_pair(_T("Staff car"), LoadIcon(IDI_ICON_STAFFCAR) ) );
	m_mapElementIcon.insert( std::make_pair(_T("Crew bus"), LoadIcon(IDI_ICON_CREWBUS) ) );
	m_mapElementIcon.insert( std::make_pair(_T("Tow truck"), LoadIcon(IDI_ICON_TOWTRUCK) ) );
	m_mapElementIcon.insert( std::make_pair(_T("Deicing truck"), LoadIcon(IDI_ICON_DEICINGTRUCK) ) );
	m_mapElementIcon.insert( std::make_pair(_T("Pax bus A"), LoadIcon(IDI_ICON_PAXBUSA) ) );
	m_mapElementIcon.insert( std::make_pair(_T("Pax bus B"), LoadIcon(IDI_ICON_PAXBUSB) ) );
	m_mapElementIcon.insert( std::make_pair(_T("Pax bus C"), LoadIcon(IDI_ICON_PAXBUSC) ) );

	m_hDefaultIcon = LoadIcon(IDI_ICON_OTHERS);//
	DWORD n = ::GetLastError();//IDI_ICON_OTHERS
}

CFltRelativeIconListCtrl::~CFltRelativeIconListCtrl()
{
	for(std::map<CString, HICON>::iterator iter = m_mapElementIcon.begin();
		iter != m_mapElementIcon.end(); iter++)
	{
		if(iter->second)
			DestroyIcon(iter->second);
	}
	if(m_hDefaultIcon)
		DestroyIcon(m_hDefaultIcon);

}

BEGIN_MESSAGE_MAP(CFltRelativeIconListCtrl, CListCtrl)
END_MESSAGE_MAP()


HICON CFltRelativeIconListCtrl::LoadIcon(UINT uIDRes)
{

	HMODULE hCurModule = NULL;
#ifdef _DEBUG
	hCurModule = GetModuleHandle(_T("AirsideGUI_D.dll"));
#else
	hCurModule = GetModuleHandle(_T("AirsideGUI.dll"));
#endif
	ASSERT(hCurModule != NULL);

	return (HICON) LoadImage(hCurModule, MAKEINTRESOURCE(uIDRes), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
}

HICON CFltRelativeIconListCtrl::GetIcon(CString strName)
{
	std::map<CString, HICON>::iterator iter = m_mapElementIcon.begin();
	for(; iter!= m_mapElementIcon.end(); iter++)
	{
		if(iter->first.CompareNoCase(strName) == 0)
			return iter->second;
	}

	return m_hDefaultIcon;
}

void CFltRelativeIconListCtrl::DrawItem(LPDRAWITEMSTRUCT lpdis)
{
	//COLORREF clrTextSave, clrBkSave;
	CRect rcItem(lpdis->rcItem);
	CDC* pDC = CDC::FromHandle(lpdis->hDC);
	int nItem = lpdis->itemID;
	LVITEM lvi; 
	lvi.mask = LVIF_PARAM | LVIF_STATE; 
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;
	this->GetItem(&lvi); 

	BOOL bFocus = (GetFocus() == this);
	BOOL bSelected = (lvi.state & LVIS_SELECTED) || (lvi.state & LVIS_DROPHILITED);

	CRect rcAllLabels; 
	this->GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS); 
	CRect rcLabel; 
	this->GetItemRect(nItem, rcLabel, LVIR_LABEL);
	CRect rcSymbol;
	this->GetSubItemRect(nItem,1,LVIR_LABEL,rcSymbol);
	CRect rcNumber;
	this->GetSubItemRect(nItem,2,LVIR_LABEL,rcNumber);

	//AdjustColorBoxRect(rcColor);
	rcAllLabels.left = rcLabel.left;
	COLORREF clrTextSave, clrBkSave;
	if(bSelected) { 
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT))); 
	} 
	else {
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_WINDOW)));
	}
	CString strElemName = GetItemText(nItem, 0);
	CString strNumber = GetItemText(nItem,2);

	pDC->DrawText(strElemName,rcLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	
	rcNumber.left += 20;
	pDC->DrawText(strNumber,rcNumber, DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);

	HICON hDrawIcon = GetIcon(strElemName);

	ASSERT(hDrawIcon != NULL);

	::DrawIconEx(lpdis->hDC, rcSymbol.left+ 17, rcSymbol.top, hDrawIcon, rcSymbol.Height(), 
		rcSymbol.Height()-2, 0, NULL, DI_NORMAL);

	//pDC->FillSolidRect(rcColor,RGB(111,111,111));

	if (lvi.state & LVIS_FOCUSED) 
		pDC->DrawFocusRect(rcAllLabels); 

	if (bSelected) {
		pDC->SetTextColor(RGB(100,145,111));
		pDC->SetBkColor(RGB(234,11,9));
	}

}

//-------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDlgFlightRelativeServiceLocations, CDialog)
CDlgFlightRelativeServiceLocations::CDlgFlightRelativeServiceLocations(int nProjID, PSelectFlightType pSelectFlightType,
																	   CAirportDatabase* pAirportDB, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFlightRelativeServiceLocations::IDD, pParent)
	,m_nProjID(nProjID)
	,m_pAirportDB(pAirportDB)
	,m_pSelectFlightType(pSelectFlightType)
	,m_nExtent(50)
	,m_nMinorInterval(2)
	,m_nRingRoadOffset(5)
	,m_bInit(FALSE)

{
	m_DragDropFormat = CF_TEXT;

	m_pFlightTypeDetailsMgr = new FlightTypeDetailsManager(m_pAirportDB, nProjID);
	m_pVehicleSpecifications = new CVehicleSpecifications;

}

CDlgFlightRelativeServiceLocations::~CDlgFlightRelativeServiceLocations()
{
	delete m_pFlightTypeDetailsMgr;
	delete m_pVehicleSpecifications;
}

void CDlgFlightRelativeServiceLocations::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FLTTYPE, m_wndListBox);
	DDX_Control(pDX, IDC_LIST_RELATIVE, m_wndListCtrl);
	DDX_Control(pDX, IDC_SLIDER_EXTX, m_wndExtXSlider);
	DDX_Control(pDX, IDC_SLIDER_EXTY, m_wndExtYSlider);
	DDX_Text(pDX, IDC_EDIT1, m_nExtent);
	DDX_Text(pDX, IDC_EDIT2, m_nMinorInterval);
	DDX_Text(pDX, IDC_EDIT_RINGROADOFFSET, m_nRingRoadOffset);
}


BEGIN_MESSAGE_MAP(CDlgFlightRelativeServiceLocations, CDialog)
	ON_WM_CREATE()

	ON_COMMAND(ID_FLTREL_ADD, OnNewFltType)
	ON_COMMAND(ID_FLTREL_DEL, OnDelFltType)
	
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEditExtent)
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeEditMinorInterval)
	ON_EN_CHANGE(IDC_EDIT_RINGROADOFFSET, OnEnChangeEditRingRoadOffset)
	ON_WM_HSCROLL()

	ON_LBN_SELCHANGE(IDC_LIST_FLTTYPE, OnSelchangeListFltType)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	//ON_BN_CLICKED(IDC_BUTTON_PICK, OnPickRingRoad)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_RELATIVE, OnLvnBegindragListRelative)
	ON_BN_CLICKED(IDCSAVE2, OnBnClickedSave)
END_MESSAGE_MAP()


int CDlgFlightRelativeServiceLocations::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndFltToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_wndFltToolbar.LoadToolBar(IDR_TOOLBAR_FLTREL))
	{
		return -1;
	}

	return 0;
}

void CDlgFlightRelativeServiceLocations::UpdateUIState()
{
	int nSel = m_wndListBox.GetCurSel();
	BOOL bEnable = (nSel != LB_ERR);

	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_FLTREL_DEL, bEnable);
	m_wndListCtrl.EnableWindow(bEnable);

	//GetDlgItem(IDC_BUTTON_PICK)->EnableWindow(bEnable);
}

BOOL CDlgFlightRelativeServiceLocations::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rectFltToolbar;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rectFltToolbar);
	ScreenToClient(&rectFltToolbar);
	m_wndFltToolbar.MoveWindow(&rectFltToolbar, true);

	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_FLTREL_ADD, TRUE);
	m_wndFltToolbar.GetToolBarCtrl().EnableButton(ID_FLTREL_DEL, TRUE);

	InitListCtrl();
	
	m_wndExtXSlider.SetRange(0, 10);
	m_wndExtYSlider.SetRange(0, 10);
	m_wndExtXSlider.SetPos(5);
	m_wndExtYSlider.SetPos(5);

	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN1))->SetRange(30,500);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN2))->SetRange(1,50);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN3))->SetRange(1,10);

	m_spFlight2D = GetDlgItem(IDC_FLIGHT2D1)->GetControlUnknown();
	m_spGridPtr = m_spFlight2D;
	m_spFltDimensions = m_spFlight2D;
	m_spServiceLocations = m_spFlight2D;
  

	size_t nVehicleCount = m_pVehicleSpecifications->GetElementCount();

	CString strValue;
	for (size_t i = 0; i < nVehicleCount; i++)
	{
		CVehicleSpecificationItem* pItem = m_pVehicleSpecifications->GetItem(i);
		CComBSTR bstr(pItem->getName());
		m_spServiceLocations->AddElementInfo(pItem->GetID(), bstr.m_str);
	}

	//m_spServiceLocations->SetRingRoadOffset(m_nRingRoadOffset);


	m_pFlightTypeDetailsMgr->ReadData();
	InitListBox();
	UpdateUIState();

	UpdateData(FALSE);
	m_bInit = TRUE;

	return TRUE;
}

void CDlgFlightRelativeServiceLocations::InitListBox()
{
	for (int i=0; i<m_pFlightTypeDetailsMgr->GetCount(); i++)
	{
		FlightTypeDetailsItem* pItem = m_pFlightTypeDetailsMgr->GetItem(i);

		FlightConstraint fltType = pItem->GetFltType();
		CString strFltType;
		fltType.screenPrint(strFltType);
		int nSel = m_wndListBox.AddString(strFltType);
		m_wndListBox.SetItemData(nSel, (DWORD_PTR)pItem);			
	}

	if(m_pFlightTypeDetailsMgr->GetCount()>0)
	{
		m_wndListBox.SetCurSel(0);
		OnSelchangeListFltType();
	}
}

void CDlgFlightRelativeServiceLocations::InitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle(dwStyle);

	m_wndListCtrl.InsertColumn(0, "Name", LVCFMT_LEFT, 120);
	m_wndListCtrl.InsertColumn(1, "Symbol", LVCFMT_CENTER, 50);
	m_wndListCtrl.InsertColumn(2, "Count", LVCFMT_LEFT, 50);
	
	m_pVehicleSpecifications->ReadData(m_nProjID);

	size_t nVehicleCount = m_pVehicleSpecifications->GetElementCount();

	CString strValue;
	for (size_t i = 0; i < nVehicleCount; i++)
	{
		CVehicleSpecificationItem* pItem = m_pVehicleSpecifications->GetItem(i);
		m_wndListCtrl.InsertItem(i, pItem->getName());
		m_wndListCtrl.SetItemData(i, (DWORD_PTR)pItem->GetID());
	}

}

void CDlgFlightRelativeServiceLocations::OnNewFltType()
{
	if (m_pSelectFlightType == NULL)
		return;
	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	if(m_pFlightTypeDetailsMgr->FlightConstraintHasDefine(fltType))
	{
		AfxMessageBox(_T("current Flight type has been defined!"));
		return;
	}

	FlightTypeDetailsItem* pNewItem = new FlightTypeDetailsItem(m_pAirportDB, m_nProjID);
	pNewItem->SetFltType(fltType);

	m_pFlightTypeDetailsMgr->AddItem(pNewItem);

	CString strFltType;
	fltType.screenPrint(strFltType);

	int nSel = m_wndListBox.AddString(strFltType);
	m_wndListBox.SetCurSel(nSel);
	m_wndListBox.SetItemData(nSel, (DWORD_PTR)pNewItem);

	OnSelchangeListFltType();
}

void CDlgFlightRelativeServiceLocations::OnDelFltType()
{
	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;

	FlightTypeDetailsItem* pItem = (FlightTypeDetailsItem*)m_wndListBox.GetItemData(nSel);
	m_pFlightTypeDetailsMgr->DeleteItem(pItem);

	m_wndListBox.DeleteString(nSel);

	if(m_wndListBox.GetSelCount())
	{
		m_wndListBox.SetCurSel(0);
	}
	OnSelchangeListFltType();
}

void CDlgFlightRelativeServiceLocations::OnSelchangeListFltType()
{
	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
	{
		UpdateUIState();
		return;
	}

	FlightTypeDetailsItem* pItem = (FlightTypeDetailsItem*)m_wndListBox.GetItemData(nSel);

	m_nExtent = (int)pItem->GetDimensions().GetValue(0);
	m_nMinorInterval = (int)pItem->GetDimensions().GetValue(1);

	
	ResetListCtrl(pItem);
	ResetARCFlightControl(pItem);

	m_wndExtXSlider.SetPos(5);
	m_wndExtYSlider.SetPos(5);

	m_nRingRoadOffset = pItem->GetRingRoad().GetOffset();

	UpdateData(FALSE);
	UpdateUIState();
}

void CDlgFlightRelativeServiceLocations::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(IDC_SLIDER_EXTX == pScrollBar->GetDlgCtrlID())
	{
		int nPos = m_wndExtXSlider.GetPos();
		if(nPos <=5)
		{
			int n = -(5-nPos);
			m_spFlight2D->DoOffset(VARIANT_TRUE, -(5-nPos));

		}
		else
			m_spFlight2D->DoOffset(VARIANT_TRUE, nPos - 5);

	}
	else if(IDC_SLIDER_EXTY == pScrollBar->GetDlgCtrlID())
	{
		int nPos = m_wndExtYSlider.GetPos();
		if(nPos <=5)
			m_spFlight2D->DoOffset(VARIANT_FALSE, -(5-nPos));
		else
			m_spFlight2D->DoOffset(VARIANT_FALSE, nPos - 5);
	}
}

void CDlgFlightRelativeServiceLocations::OnEnChangeEditExtent()
{
	if(!m_bInit)
		return;

	UpdateData();

	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
	{
		UpdateUIState();
		return;
	}

	FlightTypeDetailsItem* pItem = (FlightTypeDetailsItem*)m_wndListBox.GetItemData(nSel);

	FlightTypeDimensions& dim = pItem->GetDimensions();

	dim.SetValue(0, m_nExtent);
	dim.SetValue(1, m_nMinorInterval);

	ResetARCFlightControl(pItem);

}

void CDlgFlightRelativeServiceLocations::OnEnChangeEditMinorInterval()
{
	OnEnChangeEditExtent();
}

void CDlgFlightRelativeServiceLocations::OnEnChangeEditRingRoadOffset()
{
	if(!m_bInit)
		return;
	UpdateData();

	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
	{
		UpdateUIState();
		return;
	}

	FlightTypeDetailsItem* pItem = (FlightTypeDetailsItem*)m_wndListBox.GetItemData(nSel);
	pItem->GetRingRoad().SetOffset( m_nRingRoadOffset );

	//update Ring road path
	m_spServiceLocations->SetRingRoadOffset(m_nRingRoadOffset);
	m_spFlight2D->Refresh();
}

void CDlgFlightRelativeServiceLocations::ResetListCtrl(FlightTypeDetailsItem* pItem)
{
	FlightTypeServiceLoactions& svrLocations = pItem->GetRelativeLocations();

	CString strCount;
	size_t nVehicleCount = m_pVehicleSpecifications->GetElementCount();
	for (size_t i = 0; i < nVehicleCount; i++)
	{
		int nTypeID = m_pVehicleSpecifications->GetItem(i)->GetID();
		strCount.Format("%d", svrLocations.GetElementCount(nTypeID));
		m_wndListCtrl.SetItemText(i, 2, strCount);
	}

	GetDlgItem(IDCSAVE2)->EnableWindow(TRUE);
}

void CDlgFlightRelativeServiceLocations::ResetSliderCtrl()
{

}

void CDlgFlightRelativeServiceLocations::ResetARCFlightControl(FlightTypeDetailsItem* pItem)
{
	FlightTypeDimensions& dim = pItem->GetDimensions();	

	long lVal = (long)dim.GetGridExtent();
	m_spGridPtr->SetExtent(lVal);
	lVal = (long)dim.GetGridSpacing();
	m_spGridPtr->SetMinorInterval(lVal);

	m_spFltDimensions->MaxWingSpan			=	dim.GetMaxWingSpan();
	m_spFltDimensions->MaxWingThickness		=	dim.GetMaxWingThickness();
	m_spFltDimensions->MinWingThickness		=	dim.GetMinWingThickness();
	m_spFltDimensions->WingSweep			=	dim.GetWingSweep();
	m_spFltDimensions->MaxFuselageLength	=	dim.GetMaxFuselageLength();
	m_spFltDimensions->MaxFuselageWidth		=	dim.GetMaxFuselageWidth();
	m_spFltDimensions->WingRootPosition		=	dim.GetWingRootPosition();
	m_spFltDimensions->TailPlaneSpan		=	dim.GetTailPlaneSpan();
	m_spFltDimensions->TailPlaneThickness	=	dim.GetTailPlaneThickness();
	m_spFltDimensions->TailPlaneSweep		=	dim.GetTailPlaneSweep();
	m_spFltDimensions->TailPlanePosition	=	dim.GetTailPlanePosition();


	m_spServiceLocations->ResetServiceLocations();

	FlightTypeServiceLoactions& svrLocations = pItem->GetRelativeLocations();
	FlightTypeServiceLoactions::ElementList& ElementSvrList = svrLocations.GetElementList();

	for (size_t i=0; i<ElementSvrList.size(); i++)
	{
		FlightTypeRelativeElement item = ElementSvrList.at(i);
		m_spServiceLocations->AddRelativeElement(item.GetTypeID(), item.GetServiceLocation().getX(), item.GetServiceLocation().getY());

	}

	CPath2008 ringRoadPath = pItem->GetRingRoad().GetPath();
	for (int i=0; i<ringRoadPath.getCount(); i++)
	{
		m_spServiceLocations->AddRingRoadPathPoint( (i == ringRoadPath.getCount()-1) ,
			ringRoadPath.getPoint(i).getX(), ringRoadPath.getPoint(i).getY());
	}

	m_spFlight2D->Refresh();
}

//void CDlgFlightRelativeServiceLocations::OnPickRingRoad()
//{
//	int nSel = m_wndListBox.GetCurSel();
//	if (nSel == LB_ERR)
//		return;
//
//	FlightTypeDetailsItem* pItem = (FlightTypeDetailsItem*)m_wndListBox.GetItemData(nSel);
//	RingRoad& ringRoad = pItem->GetRingRoad();
//	ringRoad.ResetPath();
//
//	m_spServiceLocations->EnableRingRoadDefine();
//}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
void CDlgFlightRelativeServiceLocations::OnBnClickedOk()
{
	if(!IsAllElementHasServiceLocation())
	{
		AfxMessageBox(_T("All flight type's relative element must has one service location at least!"));
	    return;
	}
	try
	{
		CADODatabase::BeginTransaction();
		m_pFlightTypeDetailsMgr->SaveData();
		CADODatabase::CommitTransaction();

	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
	}
	OnOK();
}


void CDlgFlightRelativeServiceLocations::OnLvnBegindragListRelative(NMHDR *pNMHDR, LRESULT *pResult)
{
	using namespace ElementDragSource;

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	int nItem = pNMLV->iItem;

	CString strType;
	int nType = (int)m_wndListCtrl.GetItemData(nItem);
	size_t nVehicleCount = m_pVehicleSpecifications->GetElementCount();
	for (size_t i = 0; i < nVehicleCount; i++)
	{
		if(nType == m_pVehicleSpecifications->GetItem(i)->GetID())
		{
			strType = m_pVehicleSpecifications->GetItem(i)->getName();
			break;
		}
	}

	USES_CONVERSION;
	CComBSTR bstr = strType;
	CIDropSource* pdsrc = new CIDropSource;
	if(pdsrc == NULL)
		return ;
	pdsrc->AddRef();

	CIDataObject* pdobj = new CIDataObject(pdsrc);
	if(pdobj == NULL)
		return ;
	pdobj->AddRef();

	FORMATETC fmtetc = {0};
	fmtetc.cfFormat = CF_TEXT;
	fmtetc.dwAspect = DVASPECT_CONTENT;
	fmtetc.lindex = -1;
	fmtetc.tymed = TYMED_HGLOBAL;

	STGMEDIUM medium = {0};
	medium.tymed = TYMED_HGLOBAL;
	TCHAR* str = OLE2T(bstr.m_str);

	medium.hGlobal = GlobalAlloc(GMEM_MOVEABLE, strlen(str)+1); //for NULL
	TCHAR* pMem = (TCHAR*)GlobalLock(medium.hGlobal);
	strcpy(pMem,str);
	GlobalUnlock(medium.hGlobal);
	pdobj->SetData(&fmtetc,&medium,TRUE);

	HRESULT hr;
	IStream* pStream;

	CreateStreamOnHGlobal(NULL, TRUE, &pStream);
	DWORD dwWritten;
	pStream->Write(str, strlen(str)+1, &dwWritten);
	LARGE_INTEGER dlibMove; dlibMove.QuadPart = 0;
	pStream->Seek(dlibMove,STREAM_SEEK_SET,NULL);
	medium.pstm = pStream;
	fmtetc.tymed = TYMED_ISTREAM;
	medium.tymed = TYMED_ISTREAM;
	pdobj->SetData(&fmtetc,&medium,TRUE);
	/////////////////////////////////////////

	CDragSourceHelper dragSrcHelper;
	dragSrcHelper.InitializeFromWindow(m_hWnd, pNMLV->ptAction, pdobj);

	DWORD dwEffect;
	hr = ::DoDragDrop(pdobj, pdsrc, DROPEFFECT_COPY, &dwEffect);
	pdsrc->Release();
	pdobj->Release();

	*pResult = 0;
}

BEGIN_EVENTSINK_MAP(CDlgFlightRelativeServiceLocations, CDialog)
	ON_EVENT(CDlgFlightRelativeServiceLocations, IDC_FLIGHT2D1, 1, FireElementAdd, VTS_I4 VTS_R8 VTS_R8)
	ON_EVENT(CDlgFlightRelativeServiceLocations, IDC_FLIGHT2D1, 2, FireElementDel, VTS_I4 VTS_R8 VTS_R8)
	ON_EVENT(CDlgFlightRelativeServiceLocations, IDC_FLIGHT2D1, 3, FireRingRoadPointAdd, VTS_BOOL VTS_R8 VTS_R8)
	ON_EVENT(CDlgFlightRelativeServiceLocations, IDC_FLIGHT2D1, 4, FireResetAllElement, VTS_NONE)
	ON_EVENT(CDlgFlightRelativeServiceLocations, IDC_FLIGHT2D1, 5, FireElementMove, VTS_R8 VTS_R8 VTS_R8 VTS_R8)
END_EVENTSINK_MAP()


void CDlgFlightRelativeServiceLocations::FireElementAdd(long nTypeID, double x, double y)
{
	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;

	FlightTypeDetailsItem* pItem = (FlightTypeDetailsItem*)m_wndListBox.GetItemData(nSel);

	FlightTypeRelativeElement item;
	CPoint2008 svrPt(x, y, 0);
	item.SetFlightTypeDetailsItemID(pItem->GetID());
	item.SetTypeID(nTypeID);
	item.SetServiceLocation(svrPt);

	pItem->GetRelativeLocations().AddItem(item);

	ResetListCtrl(pItem);
}

void CDlgFlightRelativeServiceLocations::FireElementDel(long nTypeID, double x, double y)
{
	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;

	FlightTypeDetailsItem* pItem = (FlightTypeDetailsItem*)m_wndListBox.GetItemData(nSel);

	CPoint2008 svrPt(x, y, 0);
	pItem->GetRelativeLocations().DeleteItem(svrPt);

	ResetListCtrl(pItem);

}

void CDlgFlightRelativeServiceLocations::FireResetAllElement()
{
	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;

	FlightTypeDetailsItem* pItem = (FlightTypeDetailsItem*)m_wndListBox.GetItemData(nSel);
	pItem->GetRelativeLocations().ResetAllElements();

	ResetListCtrl(pItem);
}

void CDlgFlightRelativeServiceLocations::FireRingRoadPointAdd(VARIANT_BOOL bLastPt, double x, double y)
{

	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;

	FlightTypeDetailsItem* pItem = (FlightTypeDetailsItem*)m_wndListBox.GetItemData(nSel);
	RingRoad& ringRoad = pItem->GetRingRoad();

	if(bLastPt)
	{
		if(m_vectPathPoint.size() < 3)
			AfxMessageBox("Need more positions");

		CPoint2008 ptList[1024];
		int nCount = m_vectPathPoint.size();
		for(size_t i=0; i<m_vectPathPoint.size(); i++)
		{
			ptList[i].setX(m_vectPathPoint.at(i).getX());
			ptList[i].setY(m_vectPathPoint.at(i).getY());
			ptList[i].setZ(0.0);
		}
		ringRoad.InitServiceLocation(nCount, ptList);

		m_vectPathPoint.clear();
	}
	else
	{
		CPoint2008 pt(x, y, 0.0);
		m_vectPathPoint.push_back(pt);
	}

	GetDlgItem(IDCSAVE2)->EnableWindow(TRUE);
}

BOOL CDlgFlightRelativeServiceLocations::IsAllElementHasServiceLocation()
{
	for(int i=0; i<m_pFlightTypeDetailsMgr->GetCount(); i++)
	{
		FlightTypeDetailsItem* pItem = m_pFlightTypeDetailsMgr->GetItem(i);

		size_t nVehicleCount = m_pVehicleSpecifications->GetElementCount();
		for (size_t i = 0; i < nVehicleCount; i++)
		{
			int nTypeID = m_pVehicleSpecifications->GetItem(i)->GetID();
			if(pItem->GetRelativeLocations().GetElementCount(nTypeID) == 0)
				return FALSE;
		}
	}

	return TRUE;

}

void CDlgFlightRelativeServiceLocations::FireElementMove(double x1, double y1, double x2, double y2)
{
	CPoint2008 oldPoint(x1, y1, 0);
	CPoint2008 newPoint(x2, y2,0);
	int nSel = m_wndListBox.GetCurSel();
	if (nSel == LB_ERR)
		return;

	FlightTypeDetailsItem* pItem = (FlightTypeDetailsItem*)m_wndListBox.GetItemData(nSel);
	pItem->GetRelativeLocations().UpdateItem(oldPoint, newPoint);

	GetDlgItem(IDCSAVE2)->EnableWindow(TRUE);


}
void CDlgFlightRelativeServiceLocations::OnBnClickedSave()
{
	if(!IsAllElementHasServiceLocation())
	{
		AfxMessageBox(_T("All flight type's relative element must has one service location at least!"));
		return;
	}

	m_pFlightTypeDetailsMgr->SaveData();
	GetDlgItem(IDCSAVE2)->EnableWindow(FALSE);
}
