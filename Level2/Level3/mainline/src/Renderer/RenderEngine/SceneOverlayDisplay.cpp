#include "StdAfx.h"
#include ".\sceneoverlaydisplay.h"


using namespace Ogre;


class OgreOverlaySwitcher :public Ogre::RenderTargetListener
{
public:
	OgreOverlaySwitcher(CSceneOverlayDisplay* pdsp){ pParent = pdsp; }

	void preViewportUpdate(const RenderTargetViewportEvent& evt)
	{		
		if( m_showViewports.find(evt.source)== m_showViewports.end())
		{
			return;
		}
		pParent->PreDrawTo(evt.source);		
	}
	void postViewportUpdate(const RenderTargetViewportEvent& evt)
	{		
		if(  m_showViewports.find(evt.source)== m_showViewports.end() )
		{
			return;
		}
		pParent->EndDrawTo(evt.source);
	}	

	bool AddViewport(Viewport*pvp){ return m_showViewports.insert(pvp).second; }
	void RemoveViewport(Viewport* pvp){ m_showViewports.erase(pvp); }

protected:
	CSceneOverlayDisplay* pParent;
	std::set<Viewport*> m_showViewports; //the viewport show 
};

CSceneOverlayDisplay::~CSceneOverlayDisplay()
{
	delete mpOverlaySwitch; mpOverlaySwitch = NULL;
}





Ogre::Vector3 CSceneOverlayDisplay::_getScreenCoordinates( const Ogre::Vector3& position,Ogre::Viewport* pTarget)
{	
	const Ogre::Camera* mpCam = pTarget->getCamera();
	Vector3 hcsPosition = mpCam->getProjectionMatrix() * (mpCam->getViewMatrix() * position) ;
	return hcsPosition + Vector3(1,-1,1);
	
}

void CSceneOverlayDisplay::RegShowInViewport( const ViewPort& vp )
{
	
	ASSERT(vp.IsValid());
	if(!vp.IsValid())
		return;

	if(!mpOverlaySwitch) 
		mpOverlaySwitch = new OgreOverlaySwitcher(this);

	if( mpOverlaySwitch->AddViewport(vp.mvp) )
		vp.mvp->getTarget()->addListener(mpOverlaySwitch);
	
}

