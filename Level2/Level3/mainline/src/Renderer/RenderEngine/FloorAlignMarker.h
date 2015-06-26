#pragma once
#include "3DScene.h"
//display other scene camera coordinate

//this is the scene will draw a coordinate in the viewport
class RENDERENGINE_API CFloorAlignMarker : public C3DNodeObject
{
public:
	CFloorAlignMarker(){ mpNode  = NULL;}
	CFloorAlignMarker(Ogre::SceneNode* pNode){ mpNode = pNode; }

	bool Load(Ogre::SceneManager* pManager);
};
