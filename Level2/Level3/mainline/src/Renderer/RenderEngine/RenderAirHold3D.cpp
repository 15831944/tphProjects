#include "StdAfx.h"
#include ".\RenderAirHold3D.h"
#include "SelectionManager.h"
#include "Render3DScene.h"
#include "OgreConvert.h"
#include "InputAirside\AirHold.h"
#include "InputAirside\AirWayPoint.h"
#include "InputAirside\InputAirside.h"
#include "InputAirside\ALTAirport.h"
#include "RenderControlPoint.h"
#include "Boundbox3D.h"
#include "InputAirside/ALTObjectDisplayProp.h"
#include "InputAirside/InputAirside.h"
#include "Common/IARCportLayoutEditContext.h"
#include "InputAirside\ALTAirportLayout.h"

#include "materialdef.h"

using namespace Ogre;

//CRenderAirHold3D::CRenderAirHold3D(Ogre::SceneNode* pNode, CRender3DScene* p3DScene, const CGuid& guid )
//:CAirside3DObject(pNode,p3DScene,guid )
//{
//}
//
//CRenderAirHold3D::~CRenderAirHold3D(void)
//{
//}
void CRenderAirHold3D::UpdateSelected( bool b )
{

}

void CRenderAirHold3D::Update3D( ALTObject* pBaseObj )
{
	if(pBaseObj == NULL || pBaseObj->GetType()  != ALT_HOLD)
		return;

	//SceneManager* pScene = GetScene();
	Ogre::ManualObject* pObj = _GetOrCreateManualObject(GetName());// OgreUtil::createOrRetrieveManualObject(mpNode->getName().c_str(),pScene);
	Build(pBaseObj, pObj);
	AddObject(pObj,true);
	//UpdateSelected(GetSelected());

}

//void CRenderAirWayPoint3D::Update3D( ALTObject* pBaseObj, int nUpdateCode /*= Terminal3DUpdateAll*/ )
//{
//
//}

//ALTObject* CRenderAirHold3D::GetBaseObject() const
//{
//	return NULL;
//}

#define AIRHOLD_MAT VERTEXCOLOR_TRANSPARENT

void CRenderAirHold3D::Build( ALTObject* pBaseObject, Ogre::ManualObject* pObj )
{
	if(pBaseObject == NULL || pBaseObject->GetType()  != ALT_HOLD)
		return;
	AirHold *pHold = (AirHold *)pBaseObject;
	if(pHold == NULL)
		return;
	
	Path roundPath;
	Point fixPt;
	int WayptID = pHold->GetWatpoint();
	//SceneManager* pScene = GetScene();
	InputAirside *pAirside =  GetRoot().GetModel()->OnQueryInputAirside();
	if(pAirside && pAirside->GetAirspace() )
	{
		ALTObject *pBaseWayPoint = pAirside->GetAirspace()->GetObjectByID(ALT_WAYPOINT,WayptID);

		AirWayPoint *pWayPoint = (AirWayPoint *)pBaseWayPoint;

		if( pWayPoint)
		{	
			ALTAirportLayout *pAirport  = pAirside->GetActiveAirport();
			ARCPoint3 pt = pWayPoint->GetLocation(*pAirport);	

			fixPt = Point(pt[VX],pt[VY],pt[VZ]);

			ASSERT(pAirport);
			if(pAirport)
				roundPath = pHold->GetPath(fixPt,pAirport->GetMagnectVariation().GetRealVariation());				
		}
	}
	if(roundPath.getCount()== 0 || pAirside == NULL)
		return;



	double dAirportAlt = pAirside->GetActiveAirport()->getElevation();
	double minAlt = pHold->getMinAltitude() - dAirportAlt;
	double maxAlt =pHold->getMaxAltitude() - dAirportAlt;

	Ogre::ColourValue color;
	ALTObjectDisplayProp *pDisplay = pHold->getDisplayProp();
	ASSERT(pDisplay != NULL);
	if(pDisplay)
		color = OgreConvert::GetColor(pDisplay->m_dpShape.cColor);
	color.a = 0.2f;


	pObj->begin(AIRHOLD_MAT,RenderOperation::OT_TRIANGLE_STRIP);
	for(int i=0;i<roundPath.getCount();++i)
	{
		pObj->position(roundPath[i].getX(),roundPath[i].getY(),minAlt);
		pObj->colour(color);
		pObj->position(roundPath[i].getX(),roundPath[i].getY(),maxAlt);
	}
	pObj->position(roundPath[0].getX(),roundPath[0].getY(),minAlt);
	pObj->position(roundPath[0].getX(),roundPath[0].getY(),maxAlt);
	pObj->end();


	pObj->begin(AIRHOLD_MAT,RenderOperation::OT_TRIANGLE_FAN);
	for(int i=0;i<roundPath.getCount();++i)
	{
		pObj->position(roundPath[i].getX(),roundPath[i].getY(),minAlt);		
		pObj->colour(color);
	}
	pObj->end();

	//draw line
	pObj->begin(VERTEXCOLOR_LIGTHOFF,RenderOperation::OT_LINE_STRIP);
	for(int i=0;i<roundPath.getCount();++i)
	{
		pObj->position(roundPath[i].getX(),roundPath[i].getY(),minAlt);		
		pObj->colour(color);
	}
	pObj->end();


	pObj->begin(AIRHOLD_MAT,RenderOperation::OT_TRIANGLE_FAN);
	for(int i=0;i<roundPath.getCount();++i)
	{
		pObj->position(roundPath[i].getX(),roundPath[i].getY(),maxAlt);
		pObj->colour(color);
	}
	pObj->end();

	//draw line
	pObj->begin(VERTEXCOLOR_LIGTHOFF,RenderOperation::OT_LINE_STRIP);
	for(int i=0;i<roundPath.getCount();++i)
	{
		pObj->position(roundPath[i].getX(),roundPath[i].getY(),maxAlt);
		pObj->colour(color);
	}
	pObj->end();

}

void CRenderAirHold3D::Setup3D( ALTObject* pBaseObj )
{
	Update3D(pBaseObj);
}


















