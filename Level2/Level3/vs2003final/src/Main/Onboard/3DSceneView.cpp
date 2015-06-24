#include "StdAfx.h"
#include "3DSceneView.h"
#include "../TermPlanDoc.h"
#include ".\3dsceneview.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 
IMPLEMENT_DYNCREATE(C3DSceneView, CView)

BEGIN_MESSAGE_MAP(C3DSceneView, CView)	
	ON_WM_ERASEBKGND()	
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_SIZE()
END_MESSAGE_MAP()



//
BOOL C3DSceneView::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;	
}

void C3DSceneView::OnDraw( CDC* pDC )
{	
	if(!m_sceneWnd.IsCreated())
	{
		m_sceneWnd.OnCreate(GetSafeHwnd());
	}

	if(!m_bViewportSetuped) //make sure scene setup already
	{
		SetupViewport();
		m_bViewportSetuped = true;
	}
	
	PreOnDraw();
	m_sceneWnd.OnDraw();

	CView::OnDraw(pDC);
}


C3DSceneView::~C3DSceneView()
{

}

C3DSceneView::C3DSceneView()
{
	m_bViewportSetuped = FALSE;
}



int C3DSceneView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	if(!m_sceneWnd.IsCreated())
	{
		m_sceneWnd.OnCreate(GetSafeHwnd());
	}
	SetupScene();

	// TODO:  Add your specialized creation code here
	return 0;
}

void C3DSceneView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_mousePtDown = point;
	//set current view port
	m_sceneWnd.setActiveViewport( m_sceneWnd.getPointViewport(point) );

	m_sceneWnd.getPtMouseRay(point, m_lastLBtnDownRay);

	CView::OnLButtonDown(nFlags, point);
}

void C3DSceneView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_mousePtDown = point;
	//set current view port
	m_sceneWnd.setActiveViewport( m_sceneWnd.getPointViewport(point) );

	m_sceneWnd.getPtMouseRay(point,m_lastRBtnDownRay);

	CView::OnRButtonDown(nFlags, point);
}

void C3DSceneView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_mousePtDown = point;
	//set current view port
	m_sceneWnd.setActiveViewport( m_sceneWnd.getPointViewport(point) );

	m_sceneWnd.getPtMouseRay(point, m_lastMBtnDownRay);

	CView::OnMButtonDown(nFlags, point);
}


InputOnboard* C3DSceneView::GetInputOnboard() const
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	if(pDoc)
		return pDoc->GetInputOnboard();
	return NULL;
}
void C3DSceneView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);	
	// TODO: Add your message handler code here	
	m_sceneWnd.OnSize();
}

bool C3DSceneView::IsViewSetuped() const
{
	return  m_bViewportSetuped;
}

void C3DSceneView::SetupViewport()
{
	ASSERT(FALSE);
}

void C3DSceneView::SetupScene()
{
	ASSERT(FALSE);
}

void C3DSceneView::PreOnDraw()
{
	
}
//
//void C3DSceneView::SetupScene()
//{
//	//need to inherit this function
//	ASSERT(FALSE);
//}