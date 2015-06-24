// DlgActivityDensity.cpp : implementation file
//

#include "stdafx.h"
#include "DlgActivityDensity.h"
#include "TermPlan.h"
#include "MainFrm.h"
#include "3DView.h"
#include "TermPlanDoc.h"
#include "Floor2.h"
#include <CommonData/Fallback.h>
#include "../common/UnitsManager.h"
#include "common\FloatUtils.h"
#include "common\WinMsg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void AdjustColorBoxRect(CRect& r)
{
	r.DeflateRect(CSize(0,1));
	r.right = r.CenterPoint().x;
	r.left++;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgActivityDensity dialog


CDlgActivityDensity::CDlgActivityDensity(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgActivityDensity::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgActivityDensity)
	m_dCPx = 0.0;
	m_dCPy = 0.0;
	m_dSizeX = 0.0;
	m_dLOSint = 0.0;
	m_nLOSlevels = 0;
	m_dLOSmin = 0.0;
	m_dSizeY = 0.0;
	m_nSubdivsX = 0;
	m_nSubdivsY = 0;
	m_sUnits = _T("");
	m_sUnits2 = _T("");
	m_sUnits3 = _T("");
	m_sUnits4 = _T("");
	m_sUnits5 = _T("");
	m_sUnits6 = _T("");
	//}}AFX_DATA_INIT
}


void CDlgActivityDensity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgActivityDensity)
	DDX_Control(pDX, IDC_CMB_FLOOR, m_cmbFloor);
	DDX_Control(pDX, IDC_LIST_LOSCOLORS, m_lstColors);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Text(pDX, IDC_EDIT_CPTX, m_dCPx);
	DDX_Text(pDX, IDC_EDIT_CPTY, m_dCPy);
	DDX_Text(pDX, IDC_EDIT_SIZEX, m_dSizeX);
	DDX_Text(pDX, IDC_EDIT_LOSINT, m_dLOSint);
	DDX_Text(pDX, IDC_EDIT_LOSLVL, m_nLOSlevels);
	DDX_Text(pDX, IDC_EDIT_LOSMIN, m_dLOSmin);
	DDX_Text(pDX, IDC_EDIT_SIZEY, m_dSizeY);
	DDX_Text(pDX, IDC_EDIT_SUBDIVSX, m_nSubdivsX);
	DDX_Text(pDX, IDC_EDIT_SUBDIVSY, m_nSubdivsY);
	DDX_Text(pDX, IDC_STATIC_UNITS, m_sUnits);
	DDX_Text(pDX, IDC_STATIC_UNITS2, m_sUnits2);
	DDX_Text(pDX, IDC_STATIC_UNITS3, m_sUnits3);
	DDX_Text(pDX, IDC_STATIC_UNITS4, m_sUnits4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgActivityDensity, CDialog)
	//{{AFX_MSG_MAP(CDlgActivityDensity)
	ON_EN_KILLFOCUS(IDC_EDIT_LOSLVL, OnKillfocusEditLOSlvl)
	ON_CBN_SELCHANGE(IDC_CMB_FLOOR, OnSelchangeCmbFloor)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_LOSCOLORS, OnDblclkListLoscolors)
	ON_EN_KILLFOCUS(IDC_EDIT_LOSMIN, OnKillfocusEditLosmin)
	ON_EN_KILLFOCUS(IDC_EDIT_LOSINT, OnKillfocusEditLosint)
	ON_BN_CLICKED(IDC_BUTTON_PFM, OnButtonPfm)
	//}}AFX_MSG_MAP
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgActivityDensity message handlers

void CDlgActivityDensity::OnOK() 
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	CActivityDensityParams* pADParams = &(pDoc->m_adParams);

	//update adparams
	UpdateData(TRUE);
	pADParams->SetAOICenter(UNITSMANAGER->UnConvertLength(m_dCPx),UNITSMANAGER->UnConvertLength(m_dCPy));
	pADParams->SetAOISize(UNITSMANAGER->UnConvertLength(m_dSizeX),UNITSMANAGER->UnConvertLength(m_dSizeY));
	pADParams->SetAOISubdivs(m_nSubdivsX,m_nSubdivsY);

	pADParams->saveDataSet(pDoc->m_ProjInfo.path, true);
	CDialog::OnOK();
}

BOOL CDlgActivityDensity::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Create columns
	CRect lstRect;
	m_lstColors.GetClientRect(&lstRect);
	m_lstColors.InsertColumn( 0, "LOS range", LVCFMT_LEFT, 150 );
	m_lstColors.InsertColumn( 1, "Color", LVCFMT_LEFT, lstRect.Width()-150 );

	// Load data.
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	CActivityDensityParams* pADParams = &(pDoc->m_adParams);

	//load floors
	int nFloors = pDoc->GetCurModeFloor().m_vFloors.size();
	for(int i=0; i<nFloors; i++) {
		m_cmbFloor.AddString(pDoc->GetCurModeFloor().m_vFloors[i]->FloorName());
	}

	m_cmbFloor.SetCurSel(pADParams->GetAOIFloor());

	//
	m_dCPx = RoundDouble(UNITSMANAGER->ConvertLength(pADParams->GetAOICenter()[VX]),2); m_dCPy = RoundDouble(UNITSMANAGER->ConvertLength(pADParams->GetAOICenter()[VY]),2);
	m_dSizeX = RoundDouble(UNITSMANAGER->ConvertLength(pADParams->GetAOISize()[VX]),2); m_dSizeY = RoundDouble(UNITSMANAGER->ConvertLength(pADParams->GetAOISize()[VY]),2);
	m_nSubdivsX = static_cast<int>(pADParams->GetAOISubdivs()[VX]); 
	m_nSubdivsY = static_cast<int>(pADParams->GetAOISubdivs()[VY]);
	m_dLOSmin = pADParams->GetLOSMinimum();
	m_dLOSint = pADParams->GetLOSInterval();
	m_nLOSlevels = pADParams->GetLOSLevels();
	m_sUnits = m_sUnits2 = m_sUnits3 = m_sUnits4 = m_sUnits5 = m_sUnits6 = UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits());
	
	RebuildLOSColorsList();
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgActivityDensity::RebuildLOSColorsList()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	CActivityDensityParams* pADParams = &(pDoc->m_adParams);

	m_lstColors.FreeItemMemory();
	m_lstColors.DeleteAllItems();

	for(int i=0; i<m_nLOSlevels; i++) {
		_LOSinfo* pLI = new _LOSinfo();
		pLI->sTag = pADParams->GetLOSLevelDescription(i);
		pLI->cColor = pADParams->GetLOSColor(i);

		LVITEM lvi;
		lvi.mask = LVIF_PARAM;
		lvi.iSubItem = 0;
		lvi.iItem = i;
		lvi.lParam = (LPARAM) pLI;
		m_lstColors.InsertItem(&lvi);
	}
}

void CDlgActivityDensity::OnKillfocusEditLOSlvl() 
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	CActivityDensityParams* pADParams = &(pDoc->m_adParams);
	UpdateData(TRUE);
	if(m_nLOSlevels != pADParams->GetLOSLevels()) {
		//# of levels has changed
		pADParams->SetLOSLevels(m_nLOSlevels);
		RebuildLOSColorsList();
	}
	
}

void CDlgActivityDensity::OnSelchangeCmbFloor() 
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	CActivityDensityParams* pADParams = &(pDoc->m_adParams);
	int n = m_cmbFloor.GetCurSel();
	if(n != -1)
		pADParams->SetAOIFloor(n);
	
}

void CDlgActivityDensity::OnDblclkListLoscolors(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	CActivityDensityParams* pADParams = &(pDoc->m_adParams);
	CPoint ptMsg = GetMessagePos();
	m_lstColors.ScreenToClient( &ptMsg );
	NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
	int nItem = pNMLW->iItem;
	if(nItem != -1) {
		_LOSinfo* li = (_LOSinfo*) m_lstColors.GetItemData(nItem);
		if(pNMLW->iSubItem == 1) {// color box column
			CRect rcColor;
			m_lstColors.GetSubItemRect(nItem,1,LVIR_LABEL,rcColor);
			AdjustColorBoxRect(rcColor);
			if(rcColor.PtInRect(ptMsg)) {
				CColorDialog dlg(li->cColor, CC_ANYCOLOR | CC_FULLOPEN, this);
				if(dlg.DoModal() == IDOK) {
					li->cColor = dlg.GetColor();
					pADParams->SetLOSColor(nItem,li->cColor);
				}
			}
		}
		m_lstColors.Invalidate();
	}
	
	*pResult = 0;
}

void CDlgActivityDensity::OnKillfocusEditLosmin() 
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	CActivityDensityParams* pADParams = &(pDoc->m_adParams);
	UpdateData(TRUE);
	if(m_dLOSmin != pADParams->GetLOSMinimum()) {
		//# of levels has changed
		pADParams->SetLOSMinimum(m_dLOSmin);
		RebuildLOSColorsList();
	}
}

void CDlgActivityDensity::OnKillfocusEditLosint() 
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	CActivityDensityParams* pADParams = &(pDoc->m_adParams);
	UpdateData(TRUE);
	if(m_dLOSint != pADParams->GetLOSInterval()) {
		//# of levels has changed
		pADParams->SetLOSInterval(m_dLOSint);
		RebuildLOSColorsList();
	}
}

void CDlgActivityDensity::OnCancel() 
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	CActivityDensityParams* pADParams = &(pDoc->m_adParams);
	AfxGetApp()->BeginWaitCursor();
	pADParams->loadInputs( pDoc->m_ProjInfo.path, &(pDoc->GetTerminal()) );
	AfxGetApp()->EndWaitCursor();
	
	CDialog::OnCancel();
}


CLOSColorsListCtrl::CLOSColorsListCtrl()
{
}

CLOSColorsListCtrl::~CLOSColorsListCtrl()
{
}

BEGIN_MESSAGE_MAP(CLOSColorsListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CLayerListCtrl)
	//ON_NOTIFY_REFLECT_EX(NM_DBLCLK, OnClick)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CLOSColorsListCtrl::DrawItem(LPDRAWITEMSTRUCT lpdis)
{
	COLORREF clrTextSave, clrBkSave;
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
	CRect rcColor;
	this->GetSubItemRect(nItem,1,LVIR_LABEL,rcColor);
	AdjustColorBoxRect(rcColor);
    rcAllLabels.left = rcLabel.left;
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
	_LOSinfo* pLI = (_LOSinfo*) lvi.lParam;

	//draw layer name
    pDC->DrawText(pLI->sTag,rcLabel, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
	//draw color box
	pDC->FillSolidRect(rcColor,pLI->cColor);
	
	if (lvi.state & LVIS_FOCUSED) 
        pDC->DrawFocusRect(rcAllLabels); 

    if (bSelected) {
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

/*
void CLOSColorsListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint ptMsg = GetMessagePos();
	this->ScreenToClient( &ptMsg );
	NMLISTVIEW* pNMLW = (NMLISTVIEW*) pNMHDR;
	int nItem = pNMLW->iItem;
	if(nItem != -1) {
		_LOSinfo* li = (_LOSinfo*) this->GetItemData(nItem);
		if(pNMLW->iSubItem == 1) {// color box column
			CRect rcColor;
			this->GetSubItemRect(nItem,1,LVIR_LABEL,rcColor);
			AdjustColorBoxRect(rcColor);
			if(rcColor.PtInRect(ptMsg)) {
				CColorDialog dlg(li->cColor, CC_ANYCOLOR | CC_FULLOPEN, this);
				if(dlg.DoModal() == IDOK) {
					li->cColor = dlg.GetColor();
				}
			}
		}
		Invalidate();
	}
	
	*pResult = TRUE; //let parent also handle
}
*/

void CLOSColorsListCtrl::OnDestroy()
{
	CListCtrl::OnDestroy();
	
	FreeItemMemory();
}

void CLOSColorsListCtrl::FreeItemMemory()
{
	int count = GetItemCount();
	for(int i=0; i<count; i++) {
		delete ((_LOSinfo*)GetItemData(i));
	}
}

void CDlgActivityDensity::OnButtonPfm() 
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	C3DView* p3DView = pDoc->Get3DView();
	if( p3DView == NULL )
	{
		pDoc->GetMainFrame()->CreateOrActive3DView();
		p3DView = pDoc->Get3DView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}

	HideDialog( pDoc->GetMainFrame() );

	
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();
	enum FallbackReason enumReason = PICK_ONERECT;

	if( !pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}	
}

LRESULT CDlgActivityDensity::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	CWnd* wnd = GetParent();
	while (!wnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		wnd->ShowWindow(SW_SHOW);
		wnd->EnableWindow(false);
		wnd = wnd->GetParent();
	}
	wnd->EnableWindow(false);
	ShowWindow(SW_SHOW);	
	EnableWindow();
    CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);

	if(pData->size() <= 1)
		return FALSE;
	
	m_dCPx = RoundDouble(UNITSMANAGER->ConvertLength(pData->at(0)[VX]),2);
	m_dCPy = RoundDouble(UNITSMANAGER->ConvertLength(pData->at(0)[VY]),2);
	m_dSizeX = RoundDouble(UNITSMANAGER->ConvertLength(fabs((pData->at(1)[VX]-pData->at(0)[VX])*2.0)),2);
	m_dSizeY = RoundDouble(UNITSMANAGER->ConvertLength(fabs((pData->at(1)[VY]-pData->at(0)[VY])*2.0)),2);
	
	UpdateData(FALSE);
		
	pDoc->UpdateAllViews(NULL);
	
	return TRUE;
}

void CDlgActivityDensity::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
}

void CDlgActivityDensity::ShowDialog(CWnd* parentWnd)
{
	while (!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		parentWnd->ShowWindow(SW_SHOW);
		parentWnd->EnableWindow(FALSE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(FALSE);
	ShowWindow(SW_SHOW);	
	EnableWindow();
}

int CDlgActivityDensity::DoFakeModal()
{
	if( CDialog::Create(CDlgActivityDensity::IDD, m_pParentWnd) ) 
	{
		CenterWindow();
		ShowWindow(SW_SHOW);
		GetParent()->EnableWindow(FALSE);
		EnableWindow();
		int nReturn = RunModalLoop();
		DestroyWindow();
		return nReturn;
	}
	else
		return IDCANCEL;
}
