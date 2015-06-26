#pragma once

#include <vector>
#include <CommonData/Processor2Base.h>
#include "3DSceneNode.h"

class Path;
class CProcessor2Base;

class C3DProcessorNode : public C3DSceneNode
{
public:
	SCENE_NODE(C3DProcessorNode,C3DSceneNode)
	virtual void Setup3D(CProcessor2Base* pProc, int idx);
private:
	
};
