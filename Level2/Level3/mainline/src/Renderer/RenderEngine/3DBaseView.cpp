#include "StdAfx.h"
#include ".\3dbaseview.h"
#include "3DScene.h"
#include "SceneState.h"
#include <common\Guid.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(C3DBaseView, IRender3DView)

// 

BEGIN_MESSAGE_MAP(C3DBaseView, IRender3DView)	
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()	
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()




void C3DBaseView::OnDraw( CDC* pDC )
{	
	if(m_bBusy)
		return;

	m_bBusy = TRUE;

	//try
	{
		PrepareDraw();
		//TRACE("Update View");
		m_rdc.Render();	
	}
	//catch (...)
	{
	}

	CalculateFrameRate();
	m_bBusy = FALSE;
}



void C3DBaseView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);	
	// TODO: Add your message handler code here	
	if(m_rdc.IsValid())
	{
		//MakeSureSetuped();
		m_rdc.OnSize();
	}	
	Invalidate(FALSE);

}


void C3DBaseView::OnDestroy()
{
	UnSetupViewport();
	m_rdc.Destory();
	
	CView::OnDestroy();
}

void C3DBaseView::OnInitialUpdate()
{
	CView::OnInitialUpdate();	
	m_nAnimationTimer = SetTimer(m_nAnimationTimer, m_nAnimationInterval, NULL);
	MakeSureSetuped();	
	m_rdc.Render();
}

BOOL C3DBaseView::OnEraseBkgnd( CDC* pDC )
{
	if(m_rdc.IsValid())
		return TRUE;
	else
		return CView::OnEraseBkgnd(pDC);
}


bool C3DBaseView::MoveCamera( UINT nFlags, const CPoint& ptTo,int vpIdx )
{	
	ViewPort vp =  m_rdc.GetViewport(vpIdx);
	C3DSceneBase* pScene = GetScene(vp.getScene());
	if(pScene)
	{
		CCameraNode camera = vp.getCamera();	
		return camera.MoveCamera(nFlags,ptTo-m_lastMousePt,pScene);		
	}
	return false;	
}

void C3DBaseView::OnMouseMove( UINT nFlags, CPoint point )
{	

	if( ::GetAsyncKeyState(VK_MENU) < 0 )//ALT pressed // move camera
	{
		if( MoveCamera(nFlags,point) )
		{
			if(!IsAnimation())
				Invalidate(FALSE);
		}
	}
	else
	{		
		
		ViewPort vp = m_rdc.GetActiveViewport();		
		{
			C3DSceneBase* pScene = GetScene(vp.getScene());
			if(pScene)
			{
				MouseSceneInfo info;
				info.SetWndInfo(nFlags,point,this);
				MovableObject* pObj = NULL;//pScene->IsSelectObject()?vp.PickObject(point):NULL;
				info.SetSceneInfo(vp.GetWorldMouseRay(point),pScene->getFocusPlan(),pObj);
				pScene->OnMouseMove(info);
			}
		}      
	}	
	m_lastMousePt = point;	
	CView::OnMouseMove(nFlags,point);
	//Invalidate(FALSE);
}


void C3DBaseView::OnLButtonDblClk(UINT nFlags, CPoint point)
{	

	if( ::GetAsyncKeyState(VK_MENU) < 0 )//ALT pressed // move camera
	{	
	}
	else
	{		
		// TODO: Add your message handler code here and/or call default
		for(int i=0;i<m_rdc.GetViewportCount();i++)
		{
			ViewPort vp = m_rdc.GetViewport(i);
			if( vp.IsPointIn(point) )
			{
				C3DSceneBase* pScene = GetScene(vp.getScene());
				if(pScene)
				{
					MouseSceneInfo info;
					info.SetWndInfo(nFlags,point,this);
					MovableObject* pObj = pScene->IsSelectObject()?vp.PickObject(point):NULL;		
					info.SetSceneInfo(vp.GetWorldMouseRay(point),pScene->getFocusPlan(),pObj);					
					pScene->OnLButtonDblClk( info );
				}
			}
		}		
		Invalidate(FALSE);
	}	
	CView::OnLButtonDblClk(nFlags, point);
}

void C3DBaseView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( ::GetAsyncKeyState(VK_MENU) < 0 )//ALT pressed // move camera
	{

	}
	else
	{
		for(int i=0;i<m_rdc.GetViewportCount();i++)
		{
			ViewPort vp = m_rdc.GetViewport(i);
			if( vp.IsPointIn(point) )
			{
				C3DSceneBase* pScene = GetScene(vp.getScene());
				if(pScene)
				{
					MouseSceneInfo info;
					info.SetWndInfo(nFlags,point,this);
					MovableObject* pObj = pScene->IsSelectObject()?vp.PickObject(point):NULL;		
					info.SetSceneInfo(vp.GetWorldMouseRay(point),pScene->getFocusPlan(),pObj);					
					pScene->OnLButtonDown( info );
				}
					
			}
		}
	}
	CView::OnLButtonDown(nFlags, point);
}

void C3DBaseView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( ::GetAsyncKeyState(VK_MENU) < 0 )//ALT pressed // move camera
	{

	}
	else
	{
		BOOL NRefresh = TRUE;
		// TODO: Add your message handler code here and/or call default
		for(int i=0;i<m_rdc.GetViewportCount();i++)
		{
			ViewPort vp = m_rdc.GetViewport(i);
			C3DSceneBase* pScene = GetScene(vp.getScene());
			if(pScene)
			{
				MouseSceneInfo info;
				info.SetWndInfo(nFlags,point,this);
				MovableObject* pObj = pScene->IsSelectObject()?vp.PickObject(point):NULL;		
				info.SetSceneInfo(vp.GetWorldMouseRay(point),pScene->getFocusPlan(),pObj);					
				pScene->OnLButtonUp( info );
			}
		}
		Invalidate(NRefresh);
	}
	
	CView::OnLButtonUp(nFlags, point);
}

void C3DBaseView::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	if( ::GetAsyncKeyState(VK_MENU) < 0 )//ALT pressed // move camera
	{

	}
	else
	{
		BOOL NRefresh =TRUE;
		// TODO: Add your message handler code here and/or call default
		for(int i=0;i<m_rdc.GetViewportCount();i++)
		{
			ViewPort vp = m_rdc.GetViewport(i);
			C3DSceneBase* pScene = GetScene(vp.getScene());
			if(pScene)
			{
				MouseSceneInfo info;
				info.SetWndInfo(nFlags,point,this);
				MovableObject* pObj = pScene->IsSelectObject()?vp.PickObject(point):NULL;		
				info.SetSceneInfo(vp.GetWorldMouseRay(point),pScene->getFocusPlan(),pObj);					
				pScene->OnMButtonDblClk(info);
			}
		}
		Invalidate(NRefresh);
	}
	
	CView::OnMButtonDblClk(nFlags, point);
}

void C3DBaseView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( ::GetAsyncKeyState(VK_MENU) < 0 )//ALT pressed // move camera
	{		
	}
	else
	{
		BOOL NRefresh =TRUE; 
		for(int i=0;i<m_rdc.GetViewportCount();i++)
		{
			ViewPort vp = m_rdc.GetViewport(i);
			C3DSceneBase* pScene = GetScene(vp.getScene());
			if(pScene)
			{
				MouseSceneInfo info;
				info.SetWndInfo(nFlags,point,this);
				MovableObject* pObj = pScene->IsSelectObject()?vp.PickObject(point):NULL;		
				info.SetSceneInfo(vp.GetWorldMouseRay(point),pScene->getFocusPlan(),pObj);					
				pScene->OnMButtonDown( info );
			}
		}		
		Invalidate(NRefresh);
	}
	CView::OnMButtonDown(nFlags, point);
}

void C3DBaseView::OnMButtonUp(UINT nFlags, CPoint point)
{
	if( ::GetAsyncKeyState(VK_MENU) < 0 )//ALT pressed // move camera
	{		
		
	}	
	else
	{
		BOOL NRefresh =TRUE;
		for(int i=0;i<m_rdc.GetViewportCount();i++)
		{
			ViewPort vp = m_rdc.GetViewport(i);
			C3DSceneBase* pScene = GetScene(vp.getScene());
			if(pScene)
			{
				MouseSceneInfo info;
				info.SetWndInfo(nFlags,point,this);
				MovableObject* pObj = pScene->IsSelectObject()?vp.PickObject(point):NULL;		
				info.SetSceneInfo(vp.GetWorldMouseRay(point),pScene->getFocusPlan(),pObj);					
				pScene->OnMButtonUp( info );
			}
		}
		Invalidate(NRefresh);
	}
	// TODO: Add your message handler code here and/or call default	
	CView::OnMButtonUp(nFlags, point);
}

BOOL C3DBaseView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call defaultfor(int i=0;i<m_rdc.GetViewportCount();i++)
	/*for(int i=0;i<m_rdc.GetViewportCount();i++)
	{
		ViewPort vp = m_rdc.GetViewport(i);
		if( vp.IsPointIn(pt) )
		{
			C3DSceneBase* pScene = GetScene(vp.GetCamera()->getSceneManager());
			if(pScene)
				pScene->OnMouseWheel( MouseSceneInfo(nFlags,pt,vp.GetWorldMouseRay(pt),vp.PickObject(pt) ), zDelta);
		}
	}*/
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void C3DBaseView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	if(::GetAsyncKeyState(VK_MENU) < 0)
	{

	}
	else
	{
		//BOOL NRefresh =TRUE;
		//// TODO: Add your message handler code here and/or call default
		//for(int i=0;i<m_rdc.GetViewportCount();i++)
		//{
		//	ViewPort vp = m_rdc.GetViewport(i);
		//	C3DSceneBase* pScene = GetScene(vp.getScene());
		//	if(pScene)
		//	{
		//		MouseSceneInfo info;
		//		info.SetWndInfo(nFlags,point,this);
		//		MovableObject* pObj = pScene->IsSelectObject()?vp.PickObject(point):NULL;		
		//		info.SetSceneInfo(vp.GetWorldMouseRay(point),pScene->getFocusPlan(),pObj);					
		//		pScene->OnRButtonDblClk( MouseSceneInfo(nFlags,point,vp.GetWorldMouseRay(point),NULL,pScene->getFocusPlan() ) );
		//	}
		//}
		//Invalidate(NRefresh);
	}
	
	CView::OnRButtonDblClk(nFlags, point);
}

void C3DBaseView::OnRButtonDown(UINT nFlags, CPoint point)
{
	ClearContextMenuInfo();
	// TODO: Add your message handler code here and/or call default
	if( ::GetAsyncKeyState(VK_MENU) < 0 )//ALT pressed // move camera
	{		
	}
	else
	{
		BOOL NRefresh =TRUE;
		for(int i=0;i<m_rdc.GetViewportCount();i++)
		{
			ViewPort vp = m_rdc.GetViewport(i);
			C3DSceneBase* pScene = GetScene(vp.getScene());
			if(pScene)
			{
				MouseSceneInfo info;
				info.SetWndInfo(nFlags,point,this);
				MovableObject* pObj = pScene->IsSelectObject()?vp.PickObject(point):NULL;		
				info.SetSceneInfo(vp.GetWorldMouseRay(point),pScene->getFocusPlan(),pObj);					
				pScene->OnRButtonDown(info);				
			}
		}
	}
	CView::OnRButtonDown(nFlags, point);
}

void C3DBaseView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( ::GetAsyncKeyState(VK_MENU) < 0 )//ALT pressed // move camera
	{		
	}
	else
	{
		BOOL NRefresh =TRUE;
		for(int i=0;i<m_rdc.GetViewportCount();i++)
		{
			ViewPort vp = m_rdc.GetViewport(i);
			C3DSceneBase* pScene = GetScene(vp.getScene());
			if(pScene)
			{
				MouseSceneInfo info;
				info.SetWndInfo(nFlags,point,this);
				MovableObject* pObj = pScene->IsSelectObject()?vp.PickObject(point):NULL;		
				info.SetSceneInfo(vp.GetWorldMouseRay(point),pScene->getFocusPlan(),pObj);					
				pScene->OnRButtonUp(info);
			}
		}
		Invalidate(NRefresh);
	}
	CView::OnRButtonUp(nFlags, point);
}





bool C3DBaseView::MakeSureSetuped()
{
	if(!m_rdc.IsValid())
	{
		if(m_rdc.Create(this->GetSafeHwnd()))
		{		
			SetupViewport();
			return true;
		}	
	}
	return m_rdc.IsValid();
}

C3DBaseView::C3DBaseView()
{
	CGuid guid; CGuid::Create(guid);
	mViewCameraName = guid;
	m_bBusy= FALSE;
	m_nAnimationTimer = 1;
	m_nAnimationInterval = 250;
}


bool C3DBaseWnd::GetMouseWorldPos( const CPoint& pt, ARCVector3& wdPos ,int vpIndx /*= 0*/ )
{
	ViewPort vp = m_rdc.GetViewport(0);
	C3DSceneBase* pScene = GetScene(vp.getCamera()->getSceneManager());
	Ogre::Ray mouseRay = vp.GetWorldMouseRay(pt);
	if(pScene){
		Ogre::Vector3 vec;
        bool b =  OgreUtil::GetRayIntersectPos(mouseRay,pScene->getFocusPlan(),vec);
		wdPos = ARCVECTOR(vec);
		return b;
	}
	return false;
}

void C3DBaseView::CalculateFrameRate()
{
	if(m_rdc.m_Window)
	{
		m_dFrameRate =	m_rdc.m_Window->getLastFPS();	
	}
}

void C3DBaseView::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == m_nAnimationTimer)
	{
		if (IsAnimation() && !m_bBusy)
		{
			Invalidate(FALSE);	
			return;
		}
	}		
	CView::OnTimer(nIDEvent);
}

void C3DBaseView::SetAnimInterval( UINT newTime )
{
	if(m_nAnimationInterval!=newTime)
	{
		m_nAnimationInterval = newTime;
		KillTimer(m_nAnimationTimer);
		m_nAnimationTimer = SetTimer(m_nAnimationTimer, m_nAnimationInterval, NULL);
	}
}