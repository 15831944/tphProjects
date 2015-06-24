// DlgVisibleRegions.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgVisibleRegions.h"
//#include "TermPlanDoc.h"
#include <CommonData/Fallback.h>
#include "common\WinMsg.h"
#include "MainFrm.h"
#include "3DView.h"
#include "MFCExControl\SmartEdit.h"
#include "../common/UnitsManager.h"
#include "../InputOnBoard/Deck.h"
#include "Onboard/AircraftLayout3DView.h"
#include "OnBoard/OnboardViewMsg.h"
#include "onboard/AircraftLayOutFrame.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//copies VR from floor to _pVisRegions
static void CopyVisRegions(CBaseFloor* _pFloor, std::vector<CVisibleRegion*>* _pVisRegions)
{
	int nCount = _pFloor->GetVisibleRegionVectorPtr()->size();
	_pVisRegions->reserve(nCount);
	for(int i=0; i<nCount; i++) {
		CVisibleRegion* pNewRegion = new CVisibleRegion(_pFloor->GetVisibleRegionVectorPtr()->at(i)->sName);
		pNewRegion->m_ID = _pFloor->GetVisibleRegionVectorPtr()->at(i)->m_ID ;
		int nPtCount = _pFloor->GetVisibleRegionVectorPtr()->at(i)->polygon.getCount();
		pNewRegion->polygon.init(nPtCount, _pFloor->GetVisibleRegionVectorPtr()->at(i)->polygon.getPointList());
		_pVisRegions->push_back(pNewRegion);
	}
	
}

//copies VR from _pVisRegions to floor
static void StoreVisRegions(CBaseFloor* _pFloor, std::vector<CVisibleRegion*>* _pVisRegions)
{
	//1st clear the VR from floor
	_pFloor->DeleteAllVisibleRegions();
	size_t nCount = _pVisRegions->size();
	_pFloor->GetVisibleRegionVectorPtr()->reserve(nCount);
	for(size_t i=0; i<nCount; i++) {
		CVisibleRegion* pNewRegion = new CVisibleRegion(_pVisRegions->at(i)->sName);
			pNewRegion->m_ID = _pVisRegions->at(i)->m_ID ;
		int nPtCount = _pVisRegions->at(i)->polygon.getCount();
		pNewRegion->polygon.init(nPtCount,_pVisRegions->at(i)->polygon.getPointList());
		_pFloor->GetVisibleRegionVectorPtr()->push_back(pNewRegion);
	}
}
//for Invisible regions Ben
static void CopyInVisRegions(CBaseFloor* _pFloor, std::vector<CVisibleRegion*>* _pVisRegions)
{
	int nCount = _pFloor->GetInVisibleRegionVectorPtr()->size();
	_pVisRegions->reserve(nCount);
	for(int i=0; i<nCount; i++) {
		CVisibleRegion* pNewRegion = new CVisibleRegion(_pFloor->GetInVisibleRegionVectorPtr()->at(i)->sName);
		pNewRegion->m_ID = _pFloor->GetInVisibleRegionVectorPtr()->at(i)->m_ID ;
		int nPtCount = _pFloor->GetInVisibleRegionVectorPtr()->at(i)->polygon.getCount();
		pNewRegion->polygon.init(nPtCount, _pFloor->GetInVisibleRegionVectorPtr()->at(i)->polygon.getPointList());
		_pVisRegions->push_back(pNewRegion);
	}

}
//copies inVR from _pVisRegions to floor
static void StoreInVisRegions(CBaseFloor* _pFloor, std::vector<CVisibleRegion*>* _pVisRegions)
{
	//1st clear the VR from floor
	_pFloor->DeleteAllInVisibleRegions();	

	size_t nCount = _pVisRegions->size();
	_pFloor->GetInVisibleRegionVectorPtr()->reserve(nCount);
	for(size_t i=0; i<nCount; i++) {
		CVisibleRegion* pNewRegion = new CVisibleRegion(_pVisRegions->at(i)->sName);
		pNewRegion->m_ID = _pVisRegions->at(i)->m_ID ;
		int nPtCount = _pVisRegions->at(i)->polygon.getCount();
		pNewRegion->polygon.init(nPtCount,_pVisRegions->at(i)->polygon.getPointList());
		_pFloor->GetInVisibleRegionVectorPtr()->push_back(pNewRegion);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgVisibleRegions dialog


CDlgVisibleRegions::CDlgVisibleRegions(CBaseFloor* pFloor,BOOL bIsVR, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgVisibleRegions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgVisibleRegions)
	m_bEnableVR = FALSE;
	//}}AFX_DATA_INIT
	m_bIsVR=bIsVR;
	//m_pSmartEdit = NULL;

	VERIFY(m_pFloor = pFloor);
	//VERIFY(m_pDoc = pDoc);
	
	if(m_bIsVR){
		CopyVisRegions(m_pFloor, &m_VisibleRegions);
	}else{
		CopyInVisRegions(m_pFloor, &m_VisibleRegions);
	}
	
}

void CDlgVisibleRegions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgVisibleRegions)
	DDX_Control(pDX, IDC_STATIC_TOOLBAR, m_staticToolbar);
	DDX_Control(pDX, IDC_TREE_VISREGIONS, m_treeVisRegions);
	DDX_Check(pDX, IDC_CHECK_ENABLEVR, m_bEnableVR);
	//}}AFX_DATA_MAP	
}


BEGIN_MESSAGE_MAP(CDlgVisibleRegions, CDialog)
	//{{AFX_MSG_MAP(CDlgVisibleRegions)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_COMMAND(ID_PICKUP_FORM_MAP, OnPickFromMap)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_VISREGIONS, OnSelchangedVisRegions)
	ON_BN_CLICKED(IDC_CHECK_ENABLEVR, OnCheckEnableVR)
	//}}AFX_MSG_MAP
	ON_MESSAGE(TP_ONBOARD_DATABACK, OnTempFallbackFinished)
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
	ON_MESSAGE(WM_END_EDIT,OnEndEdit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgVisibleRegions message handlers

BOOL CDlgVisibleRegions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_bIsVR)
		SetWindowText(_T("Visible Region"));
		else
		SetWindowText(_T("InVisible Region"));
	
	CWaitCursor wc;
	InitToolBar();

	LoadRegionData();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgVisibleRegions::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	return 0;
}

void CDlgVisibleRegions::InitToolBar()
{
	CRect rc;
	m_staticToolbar.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_toolBar.MoveWindow(&rc,true);
	m_toolBar.ShowWindow( SW_SHOW );

	m_toolBar.GetToolBarCtrl().DeleteButton(2);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,true);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,false);
	//m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,false);

}

void CDlgVisibleRegions::DeleteRegionTree()
{
	m_treeVisRegions.DeleteAllItems();
}

void CDlgVisibleRegions::LoadRegionData()
{
	char buf[256];
	m_bEnableVR = m_pFloor->UseVisibleRegions();
	std::vector<CVisibleRegion*>* vVR= &m_VisibleRegions;	
	 
	TVINSERTSTRUCT tis;
	TVITEM ti;
			
	for(int i=0; i<static_cast<int>(vVR->size()); i++) {
		CVisibleRegion* vr = vVR->at(i);
		int nPtCount = vr->polygon.getCount();
		tis.hParent = TVI_ROOT;
		tis.hInsertAfter = TVI_SORT;
		ti.mask       = TVIF_TEXT | TVIF_CHILDREN | TVIF_PARAM;
		ti.pszText    = (LPSTR) vr->sName.GetBuffer(0);
		ti.lParam = (LPARAM) vr;
		if(nPtCount > 0)
			ti.cChildren  = 1;
		else
			ti.cChildren = 0;
		tis.item = ti;
		HTREEITEM hItem = m_treeVisRegions.InsertItem(&tis);
		HTREEITEM hLastItem = NULL;
		for(int j=0; j<nPtCount; j++) {
			double x = UNITSMANAGER->ConvertLength(vr->polygon.getPoint(j).getX());
			double y = UNITSMANAGER->ConvertLength(vr->polygon.getPoint(j).getY());
			sprintf(buf, "x = %.2f %s; y = %.2f %s",x,UNITSMANAGER->GetLengthUnitString(),y,UNITSMANAGER->GetLengthUnitString());
			tis.hParent = hItem;
			tis.hInsertAfter = hLastItem;
			ti.cChildren  = 0;
			ti.mask       = TVIF_TEXT | TVIF_CHILDREN;
			ti.pszText    = (LPSTR) buf;
			tis.item = ti;
			hLastItem = m_treeVisRegions.InsertItem(&tis);
		}
	}
}



HTREEITEM CDlgVisibleRegions::InsertItem(CVisibleRegion* pVR)
{
	char buf[256];
	TVINSERTSTRUCT tis;
	TVITEM ti;
	int nPtCount = pVR->polygon.getCount();
	tis.hParent = TVI_ROOT;
	tis.hInsertAfter = TVI_SORT;
	ti.mask       = TVIF_TEXT | TVIF_CHILDREN | TVIF_PARAM;
	ti.pszText    = (LPSTR) pVR->sName.GetBuffer(0);
	ti.lParam = (LPARAM) pVR;
	if(nPtCount > 0)
		ti.cChildren  = 1;
	else
		ti.cChildren = 0;
	tis.item = ti;
	HTREEITEM hItem = m_treeVisRegions.InsertItem(&tis);
	HTREEITEM hLastItem = NULL;
	for(int i=0; i<nPtCount; i++) {
		sprintf(buf, "x = %.2f; y = %.2f",pVR->polygon.getPoint(i).getX(),pVR->polygon.getPoint(i).getY());
		tis.hParent = hItem;
		tis.hInsertAfter = hLastItem;
		ti.cChildren  = 0;
		ti.mask       = TVIF_TEXT | TVIF_CHILDREN;
		ti.pszText    = (LPSTR) buf;
		tis.item = ti;
		hLastItem = m_treeVisRegions.InsertItem(&tis);
	}
	return hItem;
}

void CDlgVisibleRegions::OnToolbarbuttonadd() 
{
	std::vector<CVisibleRegion*>* vVR= &m_VisibleRegions;	
	m_treeVisRegions.EnsureVisible(m_treeVisRegions.GetRootItem());
	CVisibleRegion* vr = new CVisibleRegion("");
	vVR->push_back(vr);
	HTREEITEM hItem = InsertItem(vr);
	EditItem(hItem);
}

void CDlgVisibleRegions::OnToolbarbuttondel()
{
	HTREEITEM hSelItem = m_treeVisRegions.GetSelectedItem();
	if(hSelItem == NULL)
		return;
	std::vector<CVisibleRegion*>* vVR= &m_VisibleRegions;	

	CVisibleRegion* pVR = (CVisibleRegion*) m_treeVisRegions.GetItemData(hSelItem);
	std::vector<CVisibleRegion*>::iterator it;
	for(it=vVR->begin(); it!=vVR->end(); it++) {
		if(*it == pVR) {
			delete pVR;
			vVR->erase(it);
			break;
		}
	}
	m_treeVisRegions.DeleteItem(hSelItem);
	//DeleteRegionTree();
	//LoadRegionData();
}

void CDlgVisibleRegions::HideDialog(CWnd *parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();

}

void CDlgVisibleRegions::ShowDialog(CWnd *parentWnd)
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

LRESULT CDlgVisibleRegions::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	CWnd* wnd = GetParent();
	ShowDialog(wnd);

	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);
	int nDataSize = pData->size();

	if(nDataSize>2)
	{
		CVisibleRegion* vr = (CVisibleRegion*) m_treeVisRegions.GetItemData(m_hRClickItem);
		
		vr->polygon.clear();
		//create a temp point list
		Point* pPList = new Point[nDataSize];
		for(int i=0; i<nDataSize; i++) {
			pPList[i].setPoint(pData->at(i)[VX], pData->at(i)[VY], 0.0);
		}

		vr->polygon.init(nDataSize, pPList);
		delete [] pPList;

		DeleteRegionTree();
		LoadRegionData();
		//GetDocument()->UpdateAllViews(NULL);
		return TRUE;
		
	}
	else
	{
		MessageBox("A visible region requires at least 3 points");
		return FALSE;
	}
}



void CDlgVisibleRegions::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CRect mRect;
	m_treeVisRegions.GetWindowRect( &mRect );
	if( !mRect.PtInRect( point ))
		return;

	m_treeVisRegions.SetFocus();
	CPoint pt = point;
	m_treeVisRegions.ScreenToClient( &pt );
	UINT iRet;
	m_hRClickItem	= m_treeVisRegions.HitTest( pt, &iRet );
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
	{
		m_hRClickItem = NULL;
		return;
	}

	CMenu menuPop;

	menuPop.CreatePopupMenu();
	menuPop.AppendMenu(MF_ENABLED,ID_PICKUP_FORM_MAP,"Pick Region from View");
	menuPop.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this );
}

void CDlgVisibleRegions::EditItem(HTREEITEM hItem)
{
	CRect rectItem;
	CRect rectTREE;
	m_hEditItem = hItem;
	m_treeVisRegions.GetWindowRect( &rectTREE );
	ScreenToClient( &rectTREE );
	m_treeVisRegions.GetItemRect(hItem, &rectItem, FALSE );
	
	rectItem.OffsetRect( rectTREE.left+2, rectTREE.top+1 );
	
	rectItem.right += 1;
	rectItem.bottom += 4;
	
	CString csItemText(m_treeVisRegions.GetItemText(hItem));

/*
	if( m_pSmartEdit ) {
		delete  m_pSmartEdit;
		m_pSmartEdit = NULL;
	}
	m_pSmartEdit = new CSmartEdit( csItemText );
	DWORD dwStyle = ES_LEFT | WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL |ES_UPPERCASE ;
	m_pSmartEdit->Create( dwStyle, rectItem, this, IDC_IPEDIT );
*/
	CSmartEdit* pSmartEdit = new CSmartEdit( csItemText );
	DWORD dwStyle = ES_LEFT | WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL |ES_UPPERCASE ;
	pSmartEdit->Create( dwStyle, rectItem, this, IDC_IPEDIT );

	m_toolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD,FALSE );
	m_toolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );
}

LONG CDlgVisibleRegions::OnEndEdit( WPARAM p_wParam, LPARAM p_lParam )
{
	if( p_lParam != IDC_IPEDIT )
		return 0;

	if(m_hEditItem == NULL)
		return 0;
	
	CString csStr( (LPTSTR)p_wParam );
	if( csStr.IsEmpty() )
	{
		MessageBox( "You have entered a blank name. Please try again.", "Error", MB_OK|MB_ICONWARNING );
		m_treeVisRegions.DeleteItem(m_hEditItem);
		m_hEditItem = NULL;
	}
	else
	{
		// can not have space and "'"
		csStr.TrimLeft();
		csStr.TrimRight();
		csStr.Remove(_T(','));
		csStr.Replace( _T(' '), _T('_') );
		csStr.MakeUpper();

		CVisibleRegion* vr = (CVisibleRegion*) m_treeVisRegions.GetItemData(m_hEditItem);
		ASSERT(vr!=NULL);
		vr->sName = csStr;
		m_treeVisRegions.SetItemText(m_hEditItem,csStr);
	}

	UpdateToolbar(); //to update toolbar

/*
	if( m_pSmartEdit )
	{
		delete  m_pSmartEdit;
		m_pSmartEdit = NULL;
	}
*/
	return 0;
}

void CDlgVisibleRegions::OnSelchangedVisRegions(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	UpdateToolbar();

	*pResult = 0;
}

void CDlgVisibleRegions::UpdateToolbar()
{
	HTREEITEM hSelItem = m_treeVisRegions.GetSelectedItem();

	//enable add
	m_toolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD,TRUE );

	if(hSelItem == NULL) {
		//no selection
		//disable delete
		m_toolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );
	}
	else {
		//enable delete
		m_toolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,TRUE );
	}
}
void CDlgVisibleRegions::SaveData()
{
	UpdateData(TRUE);
	//make sure each VR has at least 3 points
	int nVRCount = m_VisibleRegions.size();
	for(int i=0; i<nVRCount; i++) {
		int nPtCount = m_VisibleRegions[i]->polygon.getCount();
		if(nPtCount < 3) {
			//warn user and abort ok
			CString s;
			s.Format("The visible region \"%s\" contains only %d points.\nPlease select at least 3 points for this visible region.",
				m_VisibleRegions[i]->sName, nPtCount);
			AfxMessageBox(s);
			return;
		}
	}

	if(m_bIsVR){
		StoreVisRegions(m_pFloor, &m_VisibleRegions);
	}else{
		StoreInVisRegions(m_pFloor, &m_VisibleRegions);
	}
	m_pFloor->UseVisibleRegions(m_bEnableVR);

	/*
	//tesselate polygon to set of convex polygons
	nVRCount = m_pFloor->GetVisibleRegionVectorPtr()->size(); 
	for(i=0; i<nVRCount; i++) {
	CFloor2::CVisibleRegion* pVR = m_pFloor->GetVisibleRegionVectorPtr()->at(i);
	if(pVR->polygon.getCount() > 3) {
	pVR->polygon.DividAsConvexPollygons(pVR->vTessPoly);
	}
	}
	*/
	//mark floor visible regions changed
	m_pFloor->InvalidateVisibleRegions();
}
void CDlgVisibleRegions::OnOK() 
{
	SaveData() ;
	CDialog::OnOK();
}

void CDlgVisibleRegions::OnCheckEnableVR() 
{
}
CDlgDeckVisibleRegions::CDlgDeckVisibleRegions(CBaseFloor* pFloor,BOOL bIsVR, CTermPlanDoc* pDoc, CWnd* pParent /* = NULL */)
:CDlgVisibleRegions(pFloor,bIsVR,pParent)
{
	mpDoc = pDoc;
}
void CDlgDeckVisibleRegions::OnOK()
{
   SaveData() ;
  SaveDataToDB() ;
   CDialog::OnOK();
}
void CDlgDeckVisibleRegions::SaveDataToDB()
{
	try
	{
		CDeck* pDeck = (CDeck*)m_pFloor;
		CADODatabase::BeginTransaction() ;
		pDeck->SaveDeckProperies() ;
		if(m_bIsVR)
			pDeck->WriteVisibleRegion() ;
		else
			pDeck->WriteInVisibleRegion() ;
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
}
void CDlgDeckVisibleRegions::OnPickFromMap()
{
#ifndef TEST_CODE
	CTermPlanDoc* pDoc = mpDoc;
	ASSERT(pDoc != NULL);

	CWaitCursor wc;
	CAircraftLayOutFrame* layoutFrame = NULL ;
	layoutFrame = (CAircraftLayOutFrame*)pDoc->GetMainFrame()->CreateOrActivateFrame(theApp.m_pAircraftLayoutTermplate, RUNTIME_CLASS(CAircraftLayout3DView));
	wc.Restore();
	
	ASSERT(layoutFrame->m_LayoutView);
	HideDialog(  pDoc->GetMainFrame() );
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();
	enum FallbackReason enumReason;
	enumReason = PICK_MANYPOINTS;

	if( !layoutFrame->m_LayoutView->SendMessage( TP_ONBOARDLAYOUT_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( pDoc->GetMainFrame() );
		return;
	}
#endif
}
BEGIN_MESSAGE_MAP(CDlgDeckVisibleRegions, CDlgVisibleRegions)
	//{{AFX_MSG_MAP(CDlgVisibleRegions)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_COMMAND(ID_PICKUP_FORM_MAP, OnPickFromMap)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_VISREGIONS, OnSelchangedVisRegions)
	ON_BN_CLICKED(IDC_CHECK_ENABLEVR, OnCheckEnableVR)
	//}}AFX_MSG_MAP
	ON_MESSAGE(TP_ONBOARD_DATABACK, OnTempFallbackFinished)
	ON_MESSAGE(WM_END_EDIT,OnEndEdit)
END_MESSAGE_MAP()


CDlgFloorVisibleRegions::CDlgFloorVisibleRegions( CBaseFloor* pFloor,BOOL bIsVR,CTermPlanDoc* pDoc, CWnd* pParent /*= NULL*/ )
: CDlgVisibleRegions(pFloor,bIsVR,pParent)
, mpDoc(pDoc)
{
	ASSERT(mpDoc);
}

void CDlgFloorVisibleRegions::OnPickFromMap()
{
#ifndef TEST_CODE
	HideDialog(  mpDoc->GetMainFrame() );
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();
	enum FallbackReason enumReason;
	enumReason = PICK_MANYPOINTS;
	if( !mpDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog(  mpDoc->GetMainFrame() );
		return;
	}
#endif	

}
