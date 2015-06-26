#include "StdAfx.h"
#include ".\renderstartposition3d.h"

#include "RenderAirport3D.h"
#include <InputAirside/Taxiway.h>
#include <InputAirside/ALTAirport.h>
#include "Common/IARCportLayoutEditContext.h"
#include "InputAirside/InputAirside.h"
#include "ShapeBuilder.h"
#include "CommonData/3DTextManager.h"
#include "ManualObjectUtil.h"
#include "CustomizedRenderQueue.h"
using namespace Ogre;

Taxiway* CRenderStartPosition3D::GetTaxiway(CStartPosition* pStartPosition) const
{
	int nTaxiID = pStartPosition->GetTaxiwayID();
	return  GetRoot().GetModel()->OnQueryInputAirside()->GetTaxiwayByID(nTaxiID);
	//return m_pAirport3D->GetALTAirport()->GetTaxiwayByID(nTaxiID);
}

bool CRenderStartPosition3D::GetLocation(CStartPosition* pStartPosition, ARCPoint3& pos) const
{
	int nTaxiID = pStartPosition->GetTaxiwayID();
	Taxiway* pTaxiway = GetTaxiway(pStartPosition);
	if(pTaxiway)
	{
		const CPath2008& path = pTaxiway->GetPath();
		DistanceUnit distInPath = pStartPosition->GetTaxiwayDivision();
		CPoint2008 pt = path.GetIndexPoint((float)distInPath);
		pos = pt;
		return true;
	}
	return false;
}

void CRenderStartPosition3D::Build(CStartPosition* pStartPosition, ManualObject* pObj)
{
	pObj->clear();
	pObj->setRenderQueueGroup(RenderObject_Mark);

	ARCVector3 center;
	if (GetLocation(pStartPosition, center))
	{
		DistanceUnit dRadius = pStartPosition->GetRadius();
		ProcessorBuilder::DrawColorCircle(pObj, ColourValue(1.0, 1.0, 1.0, 1.0), center, dRadius, dRadius - 100, 10);

		ARCPoint3 markPos; double dRotAngle;double dscale;
		pStartPosition->getMarkingPos(GetTaxiway(pStartPosition)->GetPath(), markPos, dRotAngle, dscale);
		
		ManualObjectUtil drawobj(pObj);
		drawobj.beginText(dRadius*2);
		drawobj.setColor(ARCColor4(255,255,255,255));
		drawobj.setIdentTransform();
		drawobj.rotate(ARCVector3::UNIT_Z,dRotAngle);
		drawobj.translate(center);
		drawobj.drawCenterAligmentText(pStartPosition->GetMarking().c_str());
		drawobj.end();

// 		glEnable(GL_CULL_FACE);
// 		glCullFace(GL_BACK);
//		TEXTMANAGER3D->DrawOutlineText( pStartPosition->GetMarking().c_str(),markPos,(float)dRotAngle,(float)dscale); 	
// 		glEnable(GL_LIGHTING);
// 		glDisable(GL_CULL_FACE);
	}
}

//IMPLEMENT_AIRSIDE3DOBJECT_SIMPLE_UPDATE(CRenderStartPosition3D, CStartPosition);

void CRenderStartPosition3D::Update3D( ALTObjectDisplayProp* pDispObj)
{

}

void CRenderStartPosition3D::Update3D( ALTObject* pBaseObj )
{
	ManualObject* obj = _GetOrCreateManualObject(GetName());
	Build((CStartPosition*)pBaseObj,obj);
	AddObject(obj);
}
