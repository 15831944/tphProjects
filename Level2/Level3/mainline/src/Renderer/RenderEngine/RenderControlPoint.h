#pragma once
#include "3DNodeObject.h"

class RENDERENGINE_API RenderControlPoint  : public AutoDeleteNodeObject
{
public:
	RenderControlPoint(Ogre::SceneNode* pNode):AutoDeleteNodeObject(pNode) {}

	virtual void OnMove(const ARCVector3& newPos);

};
