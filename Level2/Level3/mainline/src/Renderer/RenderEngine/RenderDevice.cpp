#include "StdAfx.h"
#include ".\renderdevice.h"
#include "RenderEngine.h"
#include "SelectionManager.h"
#include "3DScene.h"
#include "OgreConvert.h"
#include "Camera.h"
#include "./ARCRenderTargetListener.h"
using namespace Ogre;

RenderWnd::RenderWnd(void)
{
	m_Window = NULL;
	m_nActiveVP = 0;
}

RenderWnd::~RenderWnd(void)
{
	Destory();		
}

bool RenderWnd::Create( HWND hwnd )
{		
	m_Window = RenderEngine::getSingleton().CreateRenderWindow(hwnd);
	if(m_Window){ m_Window->addListener(ARCRenderTargetListener::GetInstance()); }
	ASSERT(m_Window);
	return m_Window!=NULL;
}

void RenderWnd::Destory()
{
	if(m_Window)
	{
		RenderEngine::getSingleton().DestroyRenderWindow(m_Window);
		m_Window  =NULL;
	}
}



void RenderWnd::OnSize()
{	
	if(m_Window)
	{	
		m_Window->windowMovedOrResized();
		for(FixSizeVPList::iterator itr = mFixSizsVps.begin();itr!=mFixSizsVps.end();itr++)
		{
			SetViewportDim(itr->first, itr->second.first, itr->second.second, true);		
		}	
	}	
}

void RenderWnd::Render()
{
	if(m_Window)
	{
		try
		{
			//m_Window->setActive(true);
			Ogre::Root::getSingleton().renderOneFrame();
			//m_Window->update();
		}
		catch (Ogre::Exception&e )
		{
			TRACE(e.what());
		}
		//Root::getSingleton().renderOneFrame();
	}
	
}



int RenderWnd::AddViewport( Ogre::Camera* pcam , bool bShowOverlay /*= false*/ )
{
	if(m_Window)
	{
		int nIdx = m_Window->getNumViewports();
		Viewport* pvp = m_Window->addViewport(pcam,m_Window->getNumViewports());
		pvp->setVisibilityMask(NormalVisibleFlag);
		pvp->setOverlaysEnabled(bShowOverlay);
		return nIdx;
	}
	return -1;
}


void RenderWnd::SetViewportDim(int vpIdx ,const CRect& rect,const VPAlignPos& algpos ,bool bFix)
{
	if(!IsValid())
		return;	

	//get alg pos
	double wndWidth = m_Window->getWidth();
	double wndheight = m_Window->getHeight();
	int RealLeft;
	int RealTop;
	if(algpos.x == VPA_Left){ RealLeft = rect.left; }
	if(algpos.x == VPA_Center){ RealLeft = rect.left+ (wndWidth-rect.Width())/2; }
	if(algpos.x == VPA_Right){ RealLeft = wndWidth - rect.Width() - rect.left; }

	if(algpos.y == VPA_Top){ RealTop = rect.top; }
	if(algpos.y == VPA_Center){ RealTop = rect.top+ (wndheight-rect.Height())/2; }
	if(algpos.y == VPA_Bottom){ RealTop = wndheight - rect.Height() - rect.top; }

	Ogre::RealRect realrect(RealLeft/wndWidth,RealTop/wndheight,(RealLeft+rect.Width())/wndWidth,(RealTop+rect.Height())/wndheight );
	Viewport* mvp = m_Window->getViewport(vpIdx);
	mvp->setDimensions( realrect.left,realrect.top, realrect.width(),realrect.height() );
	if(bFix){
		RegFixSizeViewport(vpIdx,rect,algpos);
	}
	else
		UnRegFixSizeViewport(vpIdx);
}

void ViewPort::SetClear( DWORD color, bool bClearColor /*= true*/, bool bClearDepth /*= true*/,bool bStencile )
{
	ASSERT(IsValid());
	mvp->setBackgroundColour(OgreConvert::GetColor(color));
	int signf = 0;
	signf |= (bClearColor?FBT_COLOUR:0);
	signf |= (bClearDepth?FBT_DEPTH:0);
	signf |= (bStencile?FBT_STENCIL:0);
	mvp->setClearEveryFrame(bClearColor||bClearDepth||bStencile,signf);
}

Ogre::MovableObject* ViewPort::PickObject( const CPoint& winPt )
{
	return SelectionManager::getSingleton().SelectionPick( winPt.x,winPt.y,mvp );
}




bool ViewPort::GetWorldPosition( const CPoint& winPt, const Ogre::Plane& worldPlan, Ogre::Vector3& oWorldPos )
{
	Ogre::Ray mouseRay = GetWorldMouseRay(winPt);
	std::pair<bool,Real> result =	mouseRay.intersects(worldPlan);
	if(result.first)
	{
		oWorldPos = mouseRay.getPoint(result.second);
			return true;
	}	
	return false;
}


Ogre::Ray ViewPort::GetWorldMouseRay( const CPoint& winPt) const
{
	ASSERT(IsValid());		
	if(IsValid())
	{
		Camera* pcam = mvp->getCamera();
		Vector2 vpPos = GetRealPos(winPt);
		return pcam->getCameraToViewportRay(vpPos.x,vpPos.y);
	}
	return Ray(Vector3::ZERO,-Vector3::UNIT_Z);
}

bool ViewPort::IsPointIn( const CPoint& pt ) const
{
	int left = mvp->getActualLeft();
	int top = mvp->getActualTop();
	int width = mvp->getActualWidth();
	int height = mvp->getActualHeight();
	int offsetx = pt.x - left;
	int offsety = pt.y - top;
    
	return offsetx>=0 && offsetx<=width && offsety>=0 && offsety<=height;
}

Ogre::Vector2 ViewPort::GetRealPos( const CPoint& winPt ) const
{
	int left = mvp->getActualLeft();
	int top = mvp->getActualTop();
	int width = mvp->getActualWidth();
	int height = mvp->getActualHeight();
	double offsetx = winPt.x - left;
	double offsety = winPt.y - top;
	return Vector2(offsetx/width,offsety/height);
}

Ogre::Camera* ViewPort::getCamera() const
{
	if(mvp)
		return mvp->getCamera();
	return NULL;
}

int ViewPort::GetHeight() const
{
	return mvp->getActualHeight();
}

void ViewPort::SetVisibleFlag( const Ogre::uint32& flag )
{
	if(mvp)
		mvp->setVisibilityMask(flag);
}

Ogre::SceneManager* ViewPort::getScene() const
{
	if(!mvp)
		return NULL;

	if(mvp->getCamera())
		return mvp->getCamera()->getSceneManager();
	return NULL;
}

void ViewPort::addVisibilityFlags( const Ogre::uint32& flags )
{
	if(!mvp)return;

	uint32 mVisibilityFlags = mvp->getVisibilityMask();
	mVisibilityFlags |= flags;
	mvp->setVisibilityMask(mVisibilityFlags);
}

void ViewPort::removeVisibilityFlags( const Ogre::uint32& flags )
{
	if(!mvp)return;

	uint32 mVisibilityFlags = mvp->getVisibilityMask();
	mVisibilityFlags &= ~flags;
	mvp->setVisibilityMask(mVisibilityFlags);
}

void RenderWnd::RegFixSizeViewport( int vpIdx, const CRect& rect,const VPAlignPos& alg )
{
	mFixSizsVps[vpIdx].first = rect;
	mFixSizsVps[vpIdx].second = alg;
}

void RenderWnd::UnRegFixSizeViewport( int vpIdx)
{
	mFixSizsVps.erase(vpIdx);
}

int RenderWnd::GetViewportCount() const
{
	if(m_Window)
		return m_Window->getNumViewports();
	return 0;
}

Ogre::Vector2 RenderWnd::GetRealPos( const CPoint& winPos ) const
{
	ASSERT(m_Window);
	return Vector2(winPos.x/m_Window->getWidth(), winPos.y/m_Window->getHeight() );
}

ViewPort RenderWnd::GetViewport( int idx ) const
{
	if(m_Window->getNumViewports()>idx)
	{
		return ViewPort( m_Window->getViewport(idx));
	}
	return ViewPort(NULL);
}
