#pragma once
#include "3DScene.h"
//display other scene camera coordinate

//this is the scene will draw a coordinate in the viewport
class RENDERENGINE_API CCoordinateDisplayScene : public C3DSceneBase
{
public:
	CCoordinateDisplayScene(void);
	~CCoordinateDisplayScene(void);

	virtual void Setup();
	void UpdateCamera(Ogre::Camera* pWatchCam);
	Ogre::Camera* GetLocalCamera();
	
};
