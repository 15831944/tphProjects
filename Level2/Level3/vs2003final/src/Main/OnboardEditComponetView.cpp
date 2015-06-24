#include "StdAfx.h"
#include ".\onboardeditcomponetview.h"
#include <Ogre.h>

IMPLEMENT_DYNCREATE(~COnBoardEditComponentView, CView)

~COnBoardEditComponentView::~COnBoardEditComponentView(void)
{
	m_bRenderSetuped = false;
}

~COnBoardEditComponentView::~~COnBoardEditComponentView(void)
{
}
BEGIN_MESSAGE_MAP(~COnBoardEditComponentView, CView)
	ON_WM_SIZE()	
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void ~COnBoardEditComponentView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType,cx,cy);
	// TODO: Add your message handler code here and/or call default

	m_SceneWindow.OnSize();
	Invalidate(FALSE);
}



void ~COnBoardEditComponentView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint mouseDiff = point - m_mouseLast;
	m_mouseLast = point;

	CView::OnLButtonDown(nFlags, point);
}

void ~COnBoardEditComponentView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint mouseDiff = point - m_mouseLast;
	m_mouseLast = point;

	CView::OnMButtonDown(nFlags, point);
}

void ~COnBoardEditComponentView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint mouseDiff = point - m_mouseLast;
	m_mouseLast = point;

	CView::OnRButtonDown(nFlags, point);
}

void ~COnBoardEditComponentView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint mouseDiff = point - m_mouseLast;
	m_mouseLast = point;

	if(nFlags & MK_LBUTTON)
	{
		ScreenToClient(&point);
		Ogre::Viewport *vp = m_SceneWindow.getPointViewport(point);
		if(vp && vp->getCamera())
		{
			Ogre::Camera* pcam = vp->getCamera();
			m_SceneWindow.PanCamera(pcam,mouseDiff);
			Invalidate(FALSE);
		}

	}
	if(nFlags & MK_RBUTTON)
	{
		ScreenToClient(&point);
		Ogre::Viewport *vp = m_SceneWindow.getPointViewport(point);
		if(vp && vp->getCamera())
		{
			Ogre::Camera* pcam = vp->getCamera();
			m_SceneWindow.ZoomInOutCamera(pcam,mouseDiff);
			Invalidate(FALSE);
		}
	}
	if(nFlags & MK_MBUTTON)
	{
		ScreenToClient(&point);
		Ogre::Viewport *vp = m_SceneWindow.getPointViewport(point);
		if(vp && vp->getCamera())
		{
			Ogre::Camera* pcam = vp->getCamera();
			m_SceneWindow.TumbleCamera(pcam,mouseDiff);
			Invalidate(FALSE);
		}
	}	
	CView::OnMouseMove(nFlags, point);
}

BOOL ~COnBoardEditComponentView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}

void ~COnBoardEditComponentView::OnDraw( CDC* pDC )
{
	CDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if(!m_bRenderSetuped)
		SetupRenderer();


	// TODO: add draw code for native data here	
	m_SceneWindow.OnDraw();
}

void ~COnBoardEditComponentView::SetupRenderer()
{			
	m_SceneWindow.OnCreate( m_hWnd );
	
	m_SceneWindow.m_theScene.Init();


	Ogre::Viewport* pPort1 = m_SceneWindow.AddViewport(m_SceneWindow.m_theScene.GetMainCamera());
	pPort1->setDimensions(0,0,1,1);
	pPort1->setBackgroundColour(Ogre::ColourValue(0,0,0,0));



	m_SceneWindow.m_theScene.SetupScene();

	m_bRenderSetuped = true;
}