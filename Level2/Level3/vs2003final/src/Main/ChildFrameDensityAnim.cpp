// ChildFrameDensityAnim.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"

#include "DensityGLView.h"

#include "ChildFrameDensityAnim.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static UINT indicators[] =
{
	ID_INDICATOR_DENSITYPARAMS,
	ID_INDICATOR_ANIMTIME,
};


/////////////////////////////////////////////////////////////////////////////
// CChildFrameDensityAnim

IMPLEMENT_DYNCREATE(CChildFrameDensityAnim, CMDIChildWnd)

CChildFrameDensityAnim::CChildFrameDensityAnim()
{
}

CChildFrameDensityAnim::~CChildFrameDensityAnim()
{
}

BEGIN_MESSAGE_MAP(CChildFrameDensityAnim, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrameDensityAnim)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrameDensityAnim::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrameDensityAnim::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrameDensityAnim message handlers

BOOL CChildFrameDensityAnim::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CMDIChildWnd::PreCreateWindow(cs);
}

BOOL CChildFrameDensityAnim::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CMDIChildWnd::OnCreateClient(lpcs, pContext);
}

int CChildFrameDensityAnim::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	CRect rect;
	m_wndStatusBar.GetWindowRect(&rect);
	m_wndStatusBar.SetPaneInfo(0, ID_INDICATOR_DENSITYPARAMS, SBPS_STRETCH, 0);
	
	return 0;
}

CDensityGLView* CChildFrameDensityAnim::GetDensityAnimView()
{
	CDocument* pDoc = GetActiveDocument();
	if(pDoc == NULL)
		return NULL;
	CView* pView;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		pView = pDoc->GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CDensityGLView)))
			return (CDensityGLView*) pView;
	}
	return NULL;
}

void CChildFrameDensityAnim::OnUpdateFrameMenu (BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt)
{
    CMDIChildWnd::OnUpdateFrameMenu (bActivate, pActivateWnd, hMenuAlt);
}
