#include "StdAfx.h"
#include ".\renderstopsign3d.h"

#include "RenderAirport3D.h"

using namespace Ogre;

void CRenderStopSign3D::Build(StopSign* pStopSign, ManualObject* pObj)
{
	pObj->clear();

	const CPath2008& path = pStopSign->GetPath();
	if (path.getCount()<1)
		return;

	ARCVector3 position = path.getPoint(0);
// 	CFloors & floors = pView->GetDocument()->GetFloorByMode(EnvMode_AirSide);
// 	position[VZ] = floors.getVisibleAltitude( position[VZ] );

	Entity * pEnt = _GetOrCreateEntity( GetName(), _T("stopsign.mesh") );// OgreUtil::createOrRetrieveEntity(GetIDName() + _T("-Entity"), , GetScene());
	if (pEnt)
	{
		AddObject(pEnt,true);

// 		TollGateDisplayProp dispProp;
// 		dispProp.ReadData(pStopSign->getID());
// 		TollGateDisplayProp* pDispProp = &dispProp;
// 		ColourValue clr = OgreConvert::GetColor(pDispProp->m_dpShape.cColor);

// 		MaterialPtr matPtr = OgreUtil::createOrRetrieveColorMaterial(pdp.color[PDP_DISP_SHAPE]);
// 		if(!matPtr.isNull())
// 		{
// 			pEnt->setMaterial(matPtr);
// 		}
	}

	SetPosition(position);
	SetRotation(ARCVector3(0.0, 0.0, pStopSign->GetRotation()));
}

//IMPLEMENT_AIRSIDE3DOBJECT_SIMPLE_UPDATE(CRenderStopSign3D, StopSign);

void CRenderStopSign3D::Update3D( ALTObjectDisplayProp* pDispObj )
{

}

void CRenderStopSign3D::Update3D( ALTObject* pBaseObj )
{
	ManualObject* obj = _GetOrCreateManualObject(GetName());
	Build((StopSign*)pBaseObj,obj);
	AddObject(obj);
}
