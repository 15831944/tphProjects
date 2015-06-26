#pragma once
#include "3DSceneNode.h"
#include <CommonData/AreaPortalBase.h>


class CNamedPointList;
class CNamedPointList;
class CRenderNamedPointList3D : public C3DSceneNode
{
	SCENE_NODE(CRenderNamedPointList3D,C3DSceneNode)
public:
	//virtual void UpdateSelected(bool b); // show bounding box by default, or overrided
	virtual void Setup3D(CNamedPointList* pBaseObj, int idx);
protected:
	void Update3D(CNamedPointList* pData);
};

class CAreaBase;
class CRenderArea3D : public CRenderNamedPointList3D
{
	SCENE_NODE(CRenderArea3D,CRenderNamedPointList3D)

};


class CPortalBase;
class  CRenderPortal3D : public CRenderNamedPointList3D
{
	SCENE_NODE(CRenderPortal3D,CRenderNamedPointList3D)
};

