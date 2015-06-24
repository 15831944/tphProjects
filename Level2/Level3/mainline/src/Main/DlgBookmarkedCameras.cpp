// DlgBookmarkedCameras.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "TermPlanDoc.h"
#include "3DCamera.h"
#include "DlgBookmarkedCameras.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBookmarkedCameras dialog


CDlgBookmarkedCameras::CDlgBookmarkedCameras(CTermPlanDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBookmarkedCameras::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBookmarkedCameras)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	VERIFY(m_pDoc = pDoc);
}


void CDlgBookmarkedCameras::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBookmarkedCameras)
	DDX_Control(pDX, IDC_STATIC_TOOLBAR, m_staticToolbar);
	DDX_Control(pDX, IDC_LIST_BMCAMERAS, m_lstBMCameras);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBookmarkedCameras, CDialog)
	//{{AFX_MSG_MAP(CDlgBookmarkedCameras)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_BMCAMERAS, OnGetDispInfoListBMCameras)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_BMCAMERAS, OnEndLabelEditListBMCameras)
	ON_NOTIFY(NM_CLICK, IDC_LIST_BMCAMERAS, OnClickListBMCameras)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttonDEL)
	ON_COMMAND(ID_TOOLBARBUTTONEDIT, OnToolbarbuttonEDIT)
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonADD)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LIST_BMCAMERAS, OnBeginLabelEditListBMCameras)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBookmarkedCameras message handlers

BOOL CDlgBookmarkedCameras::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_lstBMCameras.InsertColumn(0,_T("Name"),LVCFMT_LEFT,150);
	m_lstBMCameras.InsertColumn(1,_T("Type"),LVCFMT_LEFT,50);
	m_lstBMCameras.InsertColumn(2,_T("Split?"),LVCFMT_LEFT,50);
	LoadBMCameraData();
	InitToolBar();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgBookmarkedCameras::LoadBMCameraData()
{
	/*
	C3DCameraList* pBMCamList = &(m_pDoc->m_cameras.m_vBookmarkedCameras);
	int nBMCamCount = pBMCamList->size();
	for(int i=0; i<nBMCamCount; i++) {
		m_lstBMCameras.InsertItem(LVIF_TEXT|LVIF_PARAM, i, LPSTR_TEXTCALLBACK, 0, 0, 0, (LPARAM)pBMCamList->at(i));
	}
	*/
	std::vector<CViewDesc>& vSavedViews = m_pDoc->GetCurModeCameras().m_savedViews;
	int nCount = vSavedViews.size();
	for(int i=0; i<nCount; i++) {
		CViewDesc* pVD = &(vSavedViews[i]);
		m_lstBMCameras.InsertItem(LVIF_TEXT/*|LVIF_PARAM*/, i, LPSTR_TEXTCALLBACK, 0, 0, 0, NULL/*(LPARAM)pVD*/);
	}

}

void CDlgBookmarkedCameras::OnGetDispInfoListBMCameras(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if(pDispInfo->item.mask & LVIF_TEXT) {
		//CViewDesc* pVD = (CViewDesc*)pDispInfo->item.lParam;
		CViewDesc* pVD = &(m_pDoc->GetCurModeCameras().m_savedViews[pDispInfo->item.iItem]);
		switch(pDispInfo->item.iSubItem) {
		case 0:
			lstrcpy(pDispInfo->item.pszText, pVD->name);
			break;
		case 1:
			ASSERT(pVD->panecameras.size() > 0);
			lstrcpy(pDispInfo->item.pszText, pVD->panecameras[0]->GetProjectionType()==C3DCamera::perspective?_T("3D"):_T("2D"));
			break;
		case 2:
			lstrcpy(pDispInfo->item.pszText, (pVD->paneinfo.nNumRows > 1 || pVD->paneinfo.nNumCols > 1)?_T("Yes"):_T("No"));
			break;
		default:
			ASSERT(0);
			break;
		}
	}
	
	
	*pResult = 0;
}

int CDlgBookmarkedCameras::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CDlgBookmarkedCameras::InitToolBar()
{
	CRect rc;
	m_staticToolbar.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_toolBar.MoveWindow(&rc,true);
	m_toolBar.ShowWindow( SW_SHOW );
	if (m_pDoc->Get3DViewParent() == NULL)
	{
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,false);
	}
	else
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,true);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,false);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,false);
}

void CDlgBookmarkedCameras::OnEndLabelEditListBMCameras(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,true);

	if(!pDispInfo->item.pszText)
		return;
	int nIdx = pDispInfo->item.iItem;
	
	//CViewDesc* pVD = (CViewDesc*) m_lstBMCameras.GetItemData(nIdx);
	CViewDesc* pVD = &(m_pDoc->GetCurModeCameras().m_savedViews[nIdx]);
	pVD->name = pDispInfo->item.pszText;

	*pResult =1;
}

void CDlgBookmarkedCameras::OnClickListBMCameras(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_lstBMCameras.GetSelectedCount() > 0) {
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,true);
	}
	else {
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,false);
	}

	if(m_lstBMCameras.GetSelectedCount() == 1) {
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,true);
	}
	else {
		m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,false);
	}
	
	*pResult = 0;
}

void CDlgBookmarkedCameras::DeleteListItem(int _idx)
{
	/*
	ASSERT(_idx < m_lstBMCameras.GetItemCount());
	ASSERT(_idx < m_pDoc->m_cameras.m_vBookmarkedCameras.size());

	m_lstBMCameras.DeleteItem(_idx);
	delete m_pDoc->m_cameras.m_vBookmarkedCameras[_idx];
	m_pDoc->m_cameras.m_vBookmarkedCameras.erase(m_pDoc->m_cameras.m_vBookmarkedCameras.begin()+_idx);
	// TRACE("Delete Item %d\n", _idx);
	*/
	ASSERT(_idx < m_lstBMCameras.GetItemCount());
	ASSERT(_idx < static_cast<int>(m_pDoc->GetCurModeCameras().m_savedViews.size()));

//	if(_idx < 2)
//		return; //cannot delete "Default 3D View" and "Defualt 2D View"

	CViewDesc* pVD = &(m_pDoc->GetCurModeCameras().m_savedViews[_idx]);
	m_lstBMCameras.DeleteItem(_idx);
	//delete all cameras owned by this item
	int nCamCount = pVD->panecameras.size();
	for(int i=0; i<nCamCount; i++) {
		delete (pVD->panecameras[i]);
		pVD->panecameras[i] = NULL;
	}
	//now erase this saved view
	m_pDoc->GetCurModeCameras().m_savedViews.erase(m_pDoc->GetCurModeCameras().m_savedViews.begin()+_idx);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,false);
	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,false);
}

void CDlgBookmarkedCameras::OnToolbarbuttonDEL()
{
	ASSERT(m_lstBMCameras.GetSelectedCount() > 0);
	POSITION pos = m_lstBMCameras.GetFirstSelectedItemPosition();
	std::vector<int> vSelIdx;
	while(pos != NULL) {
		vSelIdx.push_back(m_lstBMCameras.GetNextSelectedItem(pos));
	}
	for(std::vector<int>::reverse_iterator rit=vSelIdx.rbegin();
		rit!=vSelIdx.rend();
		rit++) {
		DeleteListItem(*rit);
	}
}

void CDlgBookmarkedCameras::OnToolbarbuttonEDIT()
{
	ASSERT(m_lstBMCameras.GetSelectedCount() == 1);
	POSITION pos = m_lstBMCameras.GetFirstSelectedItemPosition();

	m_lstBMCameras.EditLabel(m_lstBMCameras.GetNextSelectedItem(pos));
}

void CDlgBookmarkedCameras::OnToolbarbuttonADD()
{
	m_pDoc->SaveNamedView(_T("       "));
	int idx = m_pDoc->GetCurModeCameras().m_savedViews.size()-1;
	ASSERT(idx == m_lstBMCameras.GetItemCount());
	m_lstBMCameras.InsertItem(LVIF_TEXT, idx, LPSTR_TEXTCALLBACK, 0, 0, 0, NULL);
	m_lstBMCameras.SetItemState(idx, LVIS_SELECTED, LVIS_SELECTED);
	m_lstBMCameras.SetFocus();
	m_lstBMCameras.EditLabel(idx);
}

void CDlgBookmarkedCameras::OnBeginLabelEditListBMCameras(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	m_toolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,false);
	
	*pResult = 0;
}
