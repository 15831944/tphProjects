#include "StdAfx.h"
#include ".\rendermeetingpoint3d.h"
#include "RenderAirport3D.h"

#include "RenderAirport3D.h"
#include <InputAirside/Taxiway.h>
#include <InputAirside/ALTAirport.h>
#include "shapebuilder.h"
#include "Common/IARCportLayoutEditContext.h"
#include "InputAirside/InputAirside.h"
using namespace Ogre;

Taxiway* CRenderMeetingPoint3D::GetTaxiway(CMeetingPoint* pMeetingPoint) const
{
	int nTaxiID = pMeetingPoint->GetTaxiwayID();
	return  GetRoot().GetModel()->OnQueryInputAirside()->GetTaxiwayByID(nTaxiID);
	//return m_pAirport3D->GetALTAirport()->GetTaxiwayByID(nTaxiID);
}

bool CRenderMeetingPoint3D::GetLocation(CMeetingPoint* pMeetingPoint, ARCPoint3& pos) const
{
	Taxiway* pTaxiway = GetTaxiway(pMeetingPoint);
	if(pTaxiway)
	{
		const CPath2008& path = pTaxiway->GetPath();
		DistanceUnit distInPath = pMeetingPoint->GetTaxiwayDivision();
		CPoint2008 pt = path.GetIndexPoint((float)distInPath);
		pos = pt;
		return true;
	}
	return false;
}

void CRenderMeetingPoint3D::Build(CMeetingPoint* pMeetingPoint, ManualObject* pObj)
{
	pObj->clear();

	ARCVector3 center;
	if (GetLocation(pMeetingPoint, center))
	{
		DistanceUnit dRadius = pMeetingPoint->GetRadius();
		ProcessorBuilder::DrawColorCircle(pObj, ColourValue(1.0, 0.2, 0.1, 1.0), center, dRadius, dRadius - 100, 0.1);

// 		glEnable(GL_CULL_FACE);
// 		glCullFace(GL_BACK);
		ARCPoint3 markPos; double dRotAngle;double dscale;
		pMeetingPoint->getMarkingPos(GetTaxiway(pMeetingPoint)->GetPath(), markPos, dRotAngle, dscale);
// 
// 		TEXTMANAGER3D->DrawOutlineText( pMeetingPoint->GetMarking().c_str(),markPos,(float)dRotAngle,(float)dscale); 	
// 		glEnable(GL_LIGHTING);
// 		glDisable(GL_CULL_FACE);
	}
}


//IMPLEMENT_AIRSIDE3DOBJECT_SIMPLE_UPDATE(CRenderMeetingPoint3D, CMeetingPoint);

void CRenderMeetingPoint3D::Update3D( ALTObjectDisplayProp* pDispObj )
{

}

void CRenderMeetingPoint3D::Update3D( ALTObject* pBaseObj )
{
	ManualObject* obj = _GetOrCreateManualObject(GetName());
	Build((CMeetingPoint*)pBaseObj,obj);
	AddObject(obj);
}

