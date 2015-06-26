#include "StdAfx.h"
#include ".\arcrendertargetlistener.h"

using namespace Ogre;
void Ogre::ARCRenderTargetListener::preViewportUpdate( const RenderTargetViewportEvent& evt )
{
	//update camera light position
	Camera *pCamera = evt.source->getCamera();
	OgreUtil::UpdateCameraNearFar(pCamera);
	Light* plit = OgreUtil::createOrRetrieveLight(StrCameraLight,pCamera->getSceneManager() );
	//SceneNode* plitNode = OgreUtil::createOrRetrieveSceneNode(StrCameraLight,pCamera->getSceneManager());
	plit->setVisible(true);
	//plitNode->setPosition(-1,0,2);
	//OgreUtil::AttachMovableObject(plit,plitNode);//->attachObject(plit);
	
	SceneNode* pNode = pCamera->getParentSceneNode();
	
	//plit->setPosition( pNode->_getFullTransform()*Vector3(-1,0,0) );
	OgreUtil::AttachMovableObject(plit,pNode);
	//OgreUtil::AttachNodeTo(plitNode,pNode);

	//plitNode->needUpdate(true);
	
}

ARCRenderTargetListener* Ogre::ARCRenderTargetListener::GetInstance()
{
	static ARCRenderTargetListener inst;
	return &inst;
}
void Ogre::ARCRenderTargetListener::postViewportUpdate( const RenderTargetViewportEvent& evt )
{
	
	
}