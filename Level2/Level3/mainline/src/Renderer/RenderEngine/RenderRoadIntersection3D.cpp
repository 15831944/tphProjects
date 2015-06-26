#include "StdAfx.h"
#include ".\renderroadintersection3d.h"

#include "RenderAirport3D.h"

using namespace Ogre;

static void Build(Intersections* pCircleStretch, ManualObject* pObj)
{
	pObj->clear();


}



//IMPLEMENT_AIRSIDE3DOBJECT_SIMPLE_UPDATE(CRenderRoadIntersection3D, Intersections);

void CRenderRoadIntersection3D::Update3D( ALTObjectDisplayProp* pDispObj )
{

}

void CRenderRoadIntersection3D::Update3D( ALTObject* pBaseObj )
{
	ManualObject* pObj = _GetOrCreateManualObject(GetName());
	Build((Intersections*)pBaseObj,pObj);
	AddObject(pObj);
}
