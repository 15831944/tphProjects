#include "StdAfx.h"
#include ".\renderholdposition3d.h"

#include <InputAirside/HoldPosition.h>
#include <Common/ARCPath.h>

#include "ShapeBuilder.h"
#include "SelectionManager.h"
#include "CustomizedRenderQueue.h"

using namespace Ogre;

//
//CRenderHoldPosition3D::CRenderHoldPosition3D( Ogre::SceneNode* pNode, CRender3DScene* p3DScnene, ALTObject* pObject, int id )
//	: C3DSceneNodeObject(pNode, p3DScnene)
//	, m_pObject(pObject)
//	, m_nid(id)
//{
//
//}

ARCPoint3 CRenderHoldPosition3D::GetLocation() const
{
	return ( m_line.GetPoint1() + m_line.GetPoint2() ) * 0.5;
}

void CRenderHoldPosition3D::FlushChange()
{
	GetHoldPosition().UpdateData();
}

void CRenderHoldPosition3D::Update()
{
	UpdateLine();

	const HoldPosition& hold = GetHoldPosition();

	ManualObject* pObj = _GetOrCreateManualObject(GetName());//OgreUtil::createOrRetrieveManualObject(mpNode->getName().c_str(),pScene);
	pObj->setRenderQueueGroup(RenderObject_Mark);

	ARCVector3 pt1 = m_line.GetPoint1();
	ARCVector3 pt2 = m_line.GetPoint2();
	static const double dHoldWidth = 300.0;
	ARCVector3 ptOffset = (pt1 - pt2).PerpendicularLCopy().Normalize()*(dHoldWidth/2.0);
	ARCVector3 ptOrigin = (pt1 + pt2)/2.0;
	pt1 = ptOrigin + ptOffset;
	pt2 = ptOrigin - ptOffset;

	ARCPath3 drawPath;
	if(hold.IsFirst()) 	
	{
		drawPath.push_back(pt1);
		drawPath.push_back(pt2);
	}
	else
	{
		drawPath.push_back(pt2);
		drawPath.push_back(pt1);
	}
	ColourValue clrYellow(1.0, 1.0, 0.0, 1.0);
	ProcessorBuilder::DrawTextureRepeatedPath(pObj, _T("Airside/HoldPosition"), clrYellow, drawPath,
		m_line.GetLineLength(), 0.0, m_line.GetLineLength()/250.0/*One clip per 2.5m*/);

	//pObj->setVisibilityFlags(NormalVisibleFlag);
// 	REG_SELECT(pObj);
	AddObject(pObj);//OgreUtil::AttachMovableObject(pObj,mpNode);

}