#include "StdAfx.h"
#include ".\rendertollgate3d.h"

#include "RenderAirport3D.h"

using namespace Ogre;

void CRenderTollGate3D::Build(TollGate* pTollGate)
{
	//pObj->clear();

	const CPath2008& path = pTollGate->GetPath();
	if (path.getCount()<1)
		return;

	ARCVector3 position = path.getPoint(0);
// 	CFloors & floors = pView->GetDocument()->GetFloorByMode(EnvMode_AirSide);
// 	position[VZ] = floors.getVisibleAltitude( position[VZ] );

	Entity * pEnt =  _GetOrCreateEntity(GetName(),_T("Tollgate.mesh"));//OgreUtil::createOrRetrieveEntity(GetIDName() + _T("-Entity"), , GetScene());
	if (pEnt)
	{
		AddObject(pEnt,true);
	}

	SetPosition(position);
	SetRotation(ARCVector3(0.0, 0.0, pTollGate->GetRotation()));
}

//IMPLEMENT_AIRSIDE3DOBJECT_SIMPLE_UPDATE(CRenderTollGate3D, TollGate);

void CRenderTollGate3D::Update3D( ALTObjectDisplayProp* pDispObj )
{

}

void CRenderTollGate3D::Update3D( ALTObject* pBaseObj )
{
	//ManualObject* pObj = _GetOrCreateManualObject(GetName());
	Build((TollGate*)pBaseObj);
	//AddObject(pObj);
}
