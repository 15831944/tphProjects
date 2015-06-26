#pragma once

#include "Terminal3DObject.h"
#include "Terminal3DList.h"

#include <InputAirside/HoldShortLines.h>
#include <InputAirside/IntersectionNode.h>

namespace Ogre
{
	class SceneNode;
}

class CRender3DScene;

class CRenderTaxiInterruptLine3D : public CAirside3DObject
{
//public:
	typedef boost::shared_ptr<CRenderTaxiInterruptLine3D> SharedPtr;
	typedef std::vector<SharedPtr>                List;

	CRenderTaxiInterruptLine3D(Ogre::SceneNode* pNode, CRender3DScene* p3DScnene, CTaxiInterruptLine* pLine, size_t nIndex);

	void MoveLocation(DistanceUnit dx, DistanceUnit dy, DistanceUnit dz=0);
	void FlushChangeOf();

	void Update3D(CTaxiInterruptLine* pLine, int nUpdateCode = Terminal3DUpdateAll);
protected:
	void Build(Ogre::ManualObject* pObj);

	IntersectionNode m_FrontIntersectionNode;
	IntersectionNode m_BackIntersectionNode;
	CString m_strName;
	int m_nItem;
	double m_dDistanceToInterNode;
};

//class CRenderTaxiInterruptLine3DList : public CAirside3DList<CRenderTaxiInterruptLine3DList, CTaxiInterruptLine, CRenderTaxiInterruptLine3D>
//{
//public:
//	CRenderTaxiInterruptLine3D* CreateNew3D(CTaxiInterruptLine* pData, size_t nIndex) const;
//
//};

