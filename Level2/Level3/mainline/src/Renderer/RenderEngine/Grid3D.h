#pragma once
#include "3DNodeObject.h"
class CGrid;
class C3DSceneBase;

class RENDERENGINE_API CGrid3D  : public C3DNodeObject
{
public:
	enum emExtent{ XYExtent, YZExtent, XZExtent };

	CGrid3D(){}
	CGrid3D(Ogre::SceneNode* pNode):C3DNodeObject(pNode){  }
	void Update(const CGrid& griddata,emExtent extent=XYExtent, bool bShowZ=false);
	
	static void Build( const CGrid& griddata, Ogre::ManualObject* pGridObj,CGrid3D::emExtent extType, bool bShowZ /*= false*/ );

	static void Build( const CGrid& griddata, Ogre::ManualObject* pGridObj,BOOL bActive);
};

