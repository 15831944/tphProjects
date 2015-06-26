#include "StdAfx.h"
#include ".\renderairsector3d.h"
#include "InputAirside\AirSector.h"
#include "SelectionManager.h"
#include "Render3DScene.h"
#include "OgreConvert.h"
#include "InputAirside/ALTObjectDisplayProp.h"
#include "MaterialDef.h"
#include "./Tesselator.h"
#include <common/Pollygon2008.h>

using namespace Ogre;

//CRenderAirSector3D::CRenderAirSector3D( Ogre::SceneNode* pNode, CRender3DScene* p3DScene, const CGuid& guid )
//:CAirside3DObject(pNode,p3DScene,guid )
//{
//}
//
//CRenderAirSector3D::~CRenderAirSector3D(void)
//{
//}
void CRenderAirSector3D::UpdateSelected( bool b )
{

}

void CRenderAirSector3D::Update3D( ALTObject* pBaseObj )
{
	if(pBaseObj == NULL || pBaseObj->GetType()  != ALT_SECTOR)
		return;

	//SceneManager* pScene = GetScene();
	Ogre::ManualObject* pObj = _GetOrCreateManualObject(GetName());// OgreUtil::createOrRetrieveManualObject( GetIDName() ,pScene);
	Build(pBaseObj, pObj, CPoint2008(0,0,0));	
	AddObject(pObj,true);
	//UpdateSelected(GetSelected());

}

//void CRenderAirSector3D::Update3D( ALTObject* pBaseObj, int nUpdateCode /*= Terminal3DUpdateAll*/ )
//{
//
//}



//#define AIRSECTOR_MAT _T("WallShape/WallShapeMaterial") // 
void CRenderAirSector3D::Build( ALTObject* pBaseObject, Ogre::ManualObject* pObj, const CPoint2008& offset  )
{
	AirSector * pSector = (AirSector*)pBaseObject;

	const CPath2008& path = pSector->GetPath();
	CPollygon2008 poly;
	poly.init(path.getCount(),path.getPointList());
	poly.DoOffset(offset.x,offset.y,offset.z);
	ARCVectorList vlist = CTesselator::GetAntiClockVectorList(poly);
	ARCVectorListList vlislist(&vlist,&vlist+1);

	Ogre::ColourValue color = OgreConvert::GetColor(ARCColor3(255, 0,0));
	ALTObjectDisplayProp *pDisplay = pSector->getDisplayProp(true);
	ASSERT(pDisplay != NULL);
	if(pDisplay)
	{	
		color = OgreConvert::GetColor(pDisplay->m_dpShape.cColor);
		color[ALPHA]/=4.0 ;
	}

	/*
	CTesselator tesslatorFront;
	tesslatorFront.MakeMeshPositive( vlislist,Vector3::UNIT_Z);
	tesslatorFront.BuildMeshToObject(pObj,VERTEXCOLOR_TRANSPARENT, Vector3::UNIT_Z,color);


	CTesselator tesslatorBack;
	tesslatorBack.MakeMeshNegative( vlislist,Vector3::NEGATIVE_UNIT_Z);
	tesslatorBack.BuildMeshToObject(pObj,VERTEXCOLOR_TRANSPARENT,Vector3::NEGATIVE_UNIT_Z,color);*/



	//CAirside3D* pAirside = pView->GetAirside3D();
	//double dAirportAlt = pAirside->GetActiveAirport()->m_altAirport.getElevation();
	double dAirportAlt = 0;
	double minAlt = pSector->GetLowAltitude() - dAirportAlt;
	double maxAlt = pSector->GetHighAltitude() - dAirportAlt;

	
	

	//pObj->clear();

	//pObj->begin(VERTEXCOLOR_TRANSPARENT,RenderOperation::OT_TRIANGLE_STRIP);
	//for(int i=0;i < servicePath.getCount() -1;++i)
	//{
	//	pObj->position(servicePath[i].getX(),servicePath[i].getY(),maxAlt);
	//	pObj->colour(color);
	//	pObj->position(servicePath[i].getX(),servicePath[i].getY(),minAlt);

	//	//pObj->position(servicePath[i].getX(),servicePath[i].getY(),minAlt);
	//	//pObj->position(servicePath[i+1].getX(),servicePath[i+1].getY(),minAlt);

	//	//pObj->end();


	//	//pObj->begin(AIRSECTOR_MAT,RenderOperation::OT_TRIANGLE_LIST);
	//	//pObj->position(servicePath[i+1].getX(),servicePath[i+1].getY(),minAlt);
	//	//pObj->colour(color);
	//	//pObj->position(servicePath[i+1].getX(),servicePath[i+1].getY(),maxAlt);
	//	//pObj->position(servicePath[i].getX(),servicePath[i].getY(),maxAlt);
	//}
	//pObj->end();
	
	pObj->begin(VERTEXCOLOR_TRANSPARENT,RenderOperation::OT_TRIANGLE_STRIP);
	for(int i=0;i<path.getCount();++i){
		pObj->position(path[i].getX(),path[i].getY(),maxAlt);
		pObj->colour(color);
		pObj->position(path[i].getX(),path[i].getY(),minAlt);
		pObj->colour(color);
	}
	pObj->position(path[0].getX(),path[0].getY(),maxAlt);
	pObj->colour(color);
	pObj->position(path[0].getX(),path[0].getY(),minAlt);
	pObj->colour(color);
	pObj->end();

	pObj->begin(VERTEXCOLOR_TRANSPARENT,RenderOperation::OT_TRIANGLE_FAN);
	for(int i=path.getCount()-1;i>=0;--i)
	{	
		pObj->position(path[i].getX(),path[i].getY(),minAlt);	
		pObj->colour(color);
	}
	pObj->end();

	pObj->begin(VERTEXCOLOR_TRANSPARENT,RenderOperation::OT_TRIANGLE_FAN);
	for(int i=0;i<path.getCount();++i)
	{
		pObj->position(path[i].getX(),path[i].getY(),maxAlt);	
		pObj->colour(color);
	}
	pObj->end();

	pObj->begin(VERTEXCOLOR_TRANSPARENT,RenderOperation::OT_LINE_LIST);
	for(int i=0;i<path.getCount();++i){
		pObj->position(path[i].getX(),path[i].getY(),minAlt);
		pObj->colour(color);
		pObj->position(path[i].getX(),path[i].getY(),maxAlt);
		pObj->colour(color);
	}
	pObj->position(path[0].getX(),path[0].getY(),minAlt);
	pObj->colour(color);
	pObj->position(path[0].getX(),path[0].getY(),maxAlt);
	pObj->colour(color);
	pObj->end();

	pObj->begin(VERTEXCOLOR_TRANSPARENT,RenderOperation::OT_LINE_STRIP);
	for(int i=0;i<path.getCount();++i)
	{
		pObj->position(path[i].getX(),path[i].getY(),minAlt);	
		pObj->colour(color);
	}
	pObj->position(path[0].getX(),path[0].getY(),minAlt);	
	pObj->colour(color);
	pObj->end();

	pObj->begin(VERTEXCOLOR_TRANSPARENT,RenderOperation::OT_LINE_STRIP);
	for(int i=0;i<path.getCount();++i)
	{	
		pObj->position(path[i].getX(),path[i].getY(),maxAlt);	
		pObj->colour(color);
	}
	pObj->position(path[0].getX(),path[0].getY(),maxAlt);	
	pObj->colour(color);
	pObj->end();

}

void CRenderAirSector3D::Setup3D( ALTObject* pBaseObj )
{
	Update3D(pBaseObj);
}
