// LayoutBar.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "TermPlanDoc.h"
#include "MainFrm.h"
#include "LayoutBar.h"
  
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLayoutBar

CLayoutBar::CLayoutBar()
{   
}

CLayoutBar::~CLayoutBar()
{
}


BEGIN_MESSAGE_MAP(CLayoutBar, CToolBar24X24)
	//{{AFX_MSG_MAP(CLayoutBar)
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_COMBO_ACTIVEFLOOR,OnSelChangeActiveFloorComboBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayoutBar message handlers

int CLayoutBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBar24X24::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitImageList(IDB_TOOLBAR_LAYOUT_NORMAL,IDB_TOOLBAR_LAYOUT_HOT,IDB_TOOLBAR_LAYOUT_DISABLE,RGB(255,0,255));
	AppendImageList(IDB_TOOLBAR_INFO_NORMAL,IDB_TOOLBAR_INFO_HOT,IDB_TOOLBAR_INFO_DISABLED,RGB(255,0,255));
	AppendImageList(IDB_TOOLBAR_FLOOR_NORMAL,IDB_TOOLBAR_FLOOR_HOT,IDB_TOOLBAR_FLOOR_DISABLED,RGB(255,0,255));
	AppendImageList(IDB_TOOLBAR_HIDEPROCTAGS_NORMAL, IDB_TOOLBAR_HIDEPROCTAGS_HOT, IDB_TOOLBAR_HIDEPROCTAGS_DISABLED, RGB(255,0,255));
	AppendImageList(IDB_TOOLBAR_MAIN_NORMAL,IDB_TOOLBAR_MAIN_HOT,IDB_TOOLBAR_MAIN_DISABLE,RGB(255,0,255));
	AppendImageList(IDB_TOOLBAR_DIRECTION, IDB_TOOLBAR_DIRECTION, IDB_TOOLBAR_DIRECTION,RGB(255,0,255));
	SetImageList();
	
	TBBUTTON tbb;
	CToolBar24X24::InitTBBUTTON(tbb) ;

	tbb.iBitmap = 12;
	tbb.idCommand = ID_EDIT_UNDO;
	AddButton(&tbb);

	tbb.iBitmap = 13;
	tbb.idCommand = ID_EDIT_REDO;
	AddButton(&tbb);
	
	tbb.iBitmap = 0;
	tbb.idCommand = ID_LAYOUTLOCK;
	AddButton(&tbb);
	
	tbb.iBitmap = 1;
	tbb.idCommand = ID_DISTANCEMEASURE;
	AddButton(&tbb);
	
	tbb.iBitmap = 2;
	tbb.idCommand = ID_SHOWSHAPESTOOLBAR;
	AddButton(&tbb);

	tbb.iBitmap = 3;
	tbb.idCommand = ID_VIEW_SHARPENVIEW;
	AddButton(&tbb);

	tbb.iBitmap = 4;
	tbb.idCommand = ID_LAYOUT_INFO;
	AddButton(&tbb);

	tbb.iBitmap = 5;
	tbb.idCommand = ID_LAYOUT_FLOORADJUST;
	AddButton(&tbb);

	m_nCountNotButton=3; //not sure why needed, if remove this, get button behind combo box
	AddTextComboBox(" ", &m_cbActiveFloor, CSize(110,200), IDC_COMBO_ACTIVEFLOOR, CBS_DROPDOWNLIST);

	for(int j=0;j<3;j++)
	{
		tbb.iBitmap = -1;
		tbb.idCommand = TEMP_ID_FOR_NOT_BUTTON+m_nCountNotButton;;
		tbb.fsState &= ~TBSTATE_ENABLED;
		AddButton(&tbb,0,TRUE);
	}

	tbb.iBitmap = 6;
	tbb.idCommand = ID_LAYOUT_HIDEPROCTAGS;
	tbb.fsState |= TBSTATE_ENABLED;
	AddButton(&tbb);

	//tbb.iBitmap = 14;
	//tbb.idCommand = ID_LAYOUT_HIDETAXIWAYDIR;
	//tbb.iString = ID_LAYOUT_HIDETAXIWAYDIR;
	//AddButton(&tbb);
	
	SetWindowText (_T ("Layout"));

	EnableDocking(CBRS_ALIGN_ANY);
	return 0;
}

void CLayoutBar::OnSelChangeActiveFloorComboBox()
{

	int nSeled=m_cbActiveFloor.GetCurSel();
	//DWORD nItemData=m_cbActiveFloor.GetItemData(nSeled);

	CMainFrame* pMF=(CMainFrame*)AfxGetMainWnd();
	CView* pView=pMF->GetActiveFrame()->GetActiveView();
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pView->GetDocument();
	ASSERT(pDoc);
	int iFloorCount =pDoc->GetCurModeFloor().m_vFloors.size();

	//pDoc->ActivateFloor(nItemData);
	pDoc->ActivateFloor(iFloorCount -1 -nSeled);
	pDoc->GetCurModeFloor().saveDataSet(pDoc->m_ProjInfo.path, true);

	pDoc->UpdateViewSharpTextures();
	pDoc->UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) (CTVNode*) pDoc->FloorsNode());
	/*
	CView* pView=pMF->GetActiveFrame()->GetActiveView();

	if(pView->IsKindOf(RUNTIME_CLASS(C3DView)))
	{
		C3DView* p3DView=(C3DView*)pView;
		p3DView->m_nPaxSeledID=nItemData;
	}
	*/	
}