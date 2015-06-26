#pragma once
#include "RenderScene3DNode.h"
#include "CommonData/3DViewCommon.h"
#include "Common\Point2008.h"


//////////////////////////////////////////////////////////////////////////
class LandsideExternalNode;
class LandsideExternalNode3D : public CLayoutObject3DNode
{
public:
	SCENENODE_CONVERSION_DEF(LandsideExternalNode3D, CLayoutObject3DNode)
	PROTOTYPE_DECLARE(LandsideExternalNode3D)

	virtual void OnUpdate3D();

protected:
	void SetQueryData(LandsideExternalNode* pLot);
	LandsideExternalNode* GetExtNode();
};
