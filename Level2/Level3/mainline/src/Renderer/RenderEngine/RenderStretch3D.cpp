#include "StdAfx.h"
#include ".\renderstretch3d.h"

#include "RenderAirport3D.h"
#include <InputAirside/ALTObjectDisplayProp.h>
#include <Common/ARCPath.h>
#include "ShapeBuilder.h"
using namespace Ogre;

static const double sc_dStretchThickness = 100.0;
static const double sc_dCenterLineTexBias = 0.1;

static void Build(Stretch* pStretch, ManualObject* pObj)
{
	pObj->clear();

	const CPath2008& path2008 = pStretch->GetPath();
	int nCount = path2008.getCount();
	if(nCount < 2)
		return;

	/*StretchDisplayProp dispProp;
	dispProp.ReadData(pStretch->getID());*/
	ALTObjectDisplayProp* pDispProp = pStretch->getDisplayProp();

	ARCPath3 drawPath;
	for(int i=0;i<nCount;i++)
	{
		drawPath.push_back(path2008.getPoint(i));
	}

	Ogre::ColourValue clrSide(1.0, 1.0, 1.0, 1.0);
	if (pDispProp->m_dpShape.bOn)
	{
		double dWidth = pStretch->GetLaneNumber()*pStretch->GetLaneWidth();
		ProcessorBuilder::DrawTexturePath(pObj, _T("Airside/roadsideline"), clrSide, drawPath, dWidth*1.1, 0.0);
		ProcessorBuilder::DrawTextureRepeatedPath(pObj, _T("Airside/StretchLine"), clrSide, drawPath, dWidth,
			0.5 + sc_dCenterLineTexBias, 0.5 - sc_dCenterLineTexBias + pStretch->GetLaneNumber());
	}

}

//IMPLEMENT_AIRSIDE3DOBJECT_SIMPLE_UPDATE(CRenderStretch3D, Stretch);

void CRenderStretch3D::Update3D( ALTObjectDisplayProp* pDispObj /*= Terminal3DUpdateAll*/ )
{

}

void CRenderStretch3D::Update3D( ALTObject* pBaseObj )
{
	ManualObject* pObj = _GetOrCreateManualObject(GetName());
	Build((Stretch*)pBaseObj,pObj);
	AddObject(pObj);
}
