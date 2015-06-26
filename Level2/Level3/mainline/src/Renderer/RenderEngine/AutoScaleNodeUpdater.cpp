#include "StdAfx.h"
#include "AutoScaleNodeUpdater.h"
#include "SelectionManager.h"

void AutoScaleNodeUpdater::preFindVisibleObjects( SceneManager* source, SceneManager::IlluminationRenderStage irs, Viewport* v )
{
	if(v == SelectionManager::getSingleton().mSelectionBuffer.getTextureViewport() )
		return;
	if(v->getCamera() == mpShadowCam)
		return;

	int vWidth = v->getActualWidth();

	std::map<Ogre::String,double>::iterator itr= mvNodeList.begin();
	for(;itr!=mvNodeList.end();++itr)
	{
		const String& sNode = itr->first;
		double dWidth = itr->second;
		SceneNode* pNode= source->getSceneNode(sNode);
		if(pNode && pNode->isInSceneGraph() )
		{
			Vector3 vSize;
			if( getNodeProjSize(pNode,v->getCamera(),vSize) )
			{
				Vector3 vResutlScale = pNode->getScale() * (dWidth/(vSize.y*v->getActualHeight()) );
				pNode->setScale(vResutlScale);
			}
		}
	}
}

bool AutoScaleNodeUpdater::getNodeProjSize( SceneNode* pNode , Camera* pCam, Vector3& proj )
{
	Vector3 eyeSpacePos = pCam->getViewMatrix() * pNode->_getDerivedPosition();
	// z < 0 means in front of cam
	if (eyeSpacePos.z < 0) 
	{			
		const AxisAlignedBox& bbox = pNode->_getWorldAABB();
		double dLen = bbox.getSize().x;

		Vector3 spheresize(dLen, dLen, eyeSpacePos.z);
		proj = pCam->getProjectionMatrix() * spheresize;
		return true;
	}
	return false;
}