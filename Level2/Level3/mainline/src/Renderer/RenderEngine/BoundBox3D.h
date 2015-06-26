#pragma once
#include "3DNodeObject.h"

class RENDERENGINE_API CBoundBox3D  : public C3DNodeObject
{
public:
	CBoundBox3D(){}
	CBoundBox3D(Ogre::SceneNode* pNode):C3DNodeObject(pNode){}
	void Update() const;

	typedef std::vector<ARCVector3> CrossSection; // cross section polygon
	typedef std::vector<CrossSection> Column; // 

	Column m_column;
};
