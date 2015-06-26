#pragma once

#include "./renderdevice.h"


class OgreOverlaySwitcher;
class RENDERENGINE_API CSceneOverlayDisplay
{
public:
	CSceneOverlayDisplay(){ mpOverlaySwitch = NULL;}
	virtual ~CSceneOverlayDisplay();

	void RegShowInViewport( const ViewPort& vp);
	void UnRegShowInViewport( const ViewPort& vp);

	virtual void PreDrawTo(Ogre::Viewport* ){} //override to prepare drawing
	virtual void EndDrawTo(Ogre::Viewport* ){}

	static Ogre::Vector3 _getScreenCoordinates(const Ogre::Vector3& position,Ogre::Viewport*);
protected:
	OgreOverlaySwitcher* mpOverlaySwitch;
};




