#include "StdAfx.h"
#include ".\renderyieldsign3d.h"


using namespace Ogre;

void CRenderYieldSign3D::Build(YieldSign* pYieldSign, ManualObject* pObj)
{
	pObj->clear();

	const CPath2008& path = pYieldSign->GetPath();
	if (path.getCount()<1)
		return;

	ARCVector3 position = path.getPoint(0);
// 	CFloors & floors = pView->GetDocument()->GetFloorByMode(EnvMode_AirSide);
// 	position[VZ] = floors.getVisibleAltitude( position[VZ] );

	Entity * pEnt =  _GetOrCreateEntity(GetName(),_T("yieldsign.mesh"));//OgreUtil::createOrRetrieveEntity(GetName() + _T("-Entity"), _T("yieldsign.mesh"), getScene() );
	if (pEnt)
	{
		AddObject(pEnt,true);

// 		TollGateDisplayProp dispProp;
// 		dispProp.ReadData(pYieldSign->getID());
// 		TollGateDisplayProp* pDispProp = &dispProp;
// 		ColourValue clr = OgreConvert::GetColor(pDispProp->m_dpShape.cColor);

// 		MaterialPtr matPtr = OgreUtil::createOrRetrieveColorMaterial(pdp.color[PDP_DISP_SHAPE]);
// 		if(!matPtr.isNull())
// 		{
// 			pEnt->setMaterial(matPtr);
// 		}
	}

	SetPosition(position);
	SetRotation(ARCVector3(0.0, 0.0, pYieldSign->GetRotation()));
}

//IMPLEMENT_AIRSIDE3DOBJECT_SIMPLE_UPDATE(CRenderYieldSign3D, YieldSign);

void CRenderYieldSign3D::Update3D( ALTObjectDisplayProp* pDispObj/*= Terminal3DUpdateAll*/ )
{

}

void CRenderYieldSign3D::Update3D( ALTObject* pBaseObj )
{
	ManualObject* obj = _GetOrCreateManualObject(GetName());
	Build((YieldSign*)pBaseObj,obj);
	AddObject(obj);
}
