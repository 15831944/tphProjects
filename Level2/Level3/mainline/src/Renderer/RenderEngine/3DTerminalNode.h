#pragma once
#include "3DSceneNode.h"
#include "Common/RenderFloor.h"
class ITerminalEditContext;


class C3DTerminalNode : public C3DSceneNode
{
	SCENE_NODE(C3DTerminalNode,C3DSceneNode)

public:
	void UpdateTerminalProcssor(IARCportLayoutEditContext* pEditContext);

	void UpdateTerminalStructureBase(IARCportLayoutEditContext* pEditContext);
	void UpdateAll(IARCportLayoutEditContext* pEditContext);
	void RemoveFloor(const CString& floorId);
	void UpdateFloorAltitude(const CString& floorId, double d);	
protected:
	template <class DataType,class _3DType>
	void SetupObjectList(const CRenderFloorList& flist, IARCportLayoutEditContext* pEditContext );


	void UpdateRailway(IARCportLayoutEditContext* pEditContext);
};