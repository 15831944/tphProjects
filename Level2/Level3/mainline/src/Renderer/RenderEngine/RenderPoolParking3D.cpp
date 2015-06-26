#include "StdAfx.h"
#include ".\renderpoolparking3d.h"

#include "RenderAirport3D.h"
#include "OgreConvert.h"
#include "ShapeBuilder.h"
#include <InputAirside/ALTObjectDisplayProp.h>
#include <Common/ARCPath.h>

using namespace Ogre;

static void Build(VehiclePoolParking* pVehiclePoolParking, ManualObject* pObj)
{
	pObj->clear();

	const CPath2008& paths = pVehiclePoolParking->GetPath();
	if (paths.getCount()<1)
		return;

	/*VehiclePoolParkingDisplayProp dispProp;
	dispProp.ReadData(pVehiclePoolParking->getID());*/
	ALTObjectDisplayProp* pDispProp = pVehiclePoolParking->getDisplayProp();//&dispProp;

	Path drawPath(paths);
	if( pDispProp->m_dpShape.bOn )
	{
		Ogre::ColourValue clr = OgreConvert::GetColor(pDispProp->m_dpShape.cColor);
		ProcessorBuilder::DrawArea(pObj, &drawPath, _T("Runway/RunwayBase"), clr);
	}

}

//IMPLEMENT_AIRSIDE3DOBJECT_SIMPLE_UPDATE(CRenderPoolParking3D, VehiclePoolParking);

void CRenderPoolParking3D::Update3D( ALTObjectDisplayProp* pDispObj )
{

}

void CRenderPoolParking3D::Update3D( ALTObject* pBaseObj )
{
	ManualObject* pObj = _GetOrCreateManualObject(GetName());
	Build((VehiclePoolParking*)pBaseObj,pObj);
	AddObject(pObj);
}

