#pragma once
#include "3DNodeObject.h"

#include <Common/ARCColor.h>

class RENDERENGINE_API CLines3D  : public C3DNodeObject
{

public:
	SCENENODE_CONVERSION_DEF(CLines3D,C3DNodeObject)

	enum emExtent{ XYExtent, YZExtent, XZExtent };
	typedef std::vector<ARCVector3> VertexList;

	void Update(const VertexList& vertexList, ARCColor3 lineColor = ARCColor3::Black, float fAlpha = 1.0f);
};

