#pragma once
#include "InputAirside/contour.h"
#include "3DSceneNode.h"
class Contour;

class CRenderCounter3D :
	public C3DSceneNode
{
public:
	SCENE_NODE(CRenderCounter3D,C3DSceneNode)
	void Setup3D(ALTObject* );
protected:
	virtual void UpdateSelected(bool b);
	virtual void Update3D(ALTObject* pBaseObj );

	virtual ALTObject* GetBaseObject() const;

protected:
	void Build(ALTObject* pBaseObject, Ogre::ManualObject* pObj);
	void DrawContour(Ogre::ManualObject* pObj,CString matName, Contour *pContour);
	void GenAllTriBlock(Contour* m_pFather,const DistanceUnit& dAptAlt);
	double min, max;
};


