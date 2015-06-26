#pragma once
#include "3DSceneNode.h"

class ILandsideEditContext;
class C3DLandsideNode : public C3DSceneNode
{
	SCENE_NODE(C3DLandsideNode,C3DSceneNode)

public:
	void UpdateAll(IARCportLayoutEditContext* pEditContext);
	void RemoveFloor(const CString& floorId);
	void UpdateFloorAltitude(const CString& floorId, double d);
	
};