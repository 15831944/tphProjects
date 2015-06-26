#include "StdAfx.h"
#include "OgreConvert.h"
#include "InputAirside/AirWayPoint.h"
#include "InputAirside/ALTObjectDisplayProp.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside/ALTAirport.h"
#include ".\MaterialDef.h"
#include "RenderAirWayPoint3D.h"
#include "common\IARCportLayoutEditContext.h"
#include "InputAirside\ALTAirportLayout.h"
using namespace Ogre;


void CRenderAirWayPoint3D::UpdateSelected( bool b )
{

}

void CRenderAirWayPoint3D::Update3D( ALTObject* pBaseObj )
{
	if(pBaseObj == NULL || pBaseObj->GetType() != ALT_WAYPOINT)
		return;

	AirWayPoint *pWayPointInput = (AirWayPoint *)pBaseObj;
	if(pWayPointInput == NULL)
		return;


	CPoint2008 ServPoint = GetLocation(pWayPointInput) ;


	Ogre::ManualObject* pObj = _GetOrCreateManualObject(GetName() );//OgreUtil::createOrRetrieveManualObject( GetIDName(),pScene);
	
	Build(pWayPointInput, pObj);


	AddObject(pObj,true);

	SetPosition(ServPoint);

	//UpdateSelected(GetSelected());


}


void DrawCylinder(Ogre::ManualObject* pObj, Real baseRadius, Real topRadius, Real baseheight, Real topheight, int slices, const ColourValue& color )
{	
	Vector3 firstPtL = Vector3(baseRadius,0, baseheight);
	Vector3 firstPtH = Vector3(topRadius,0,topheight);
	Vector3 firstDir = Vector3::UNIT_Y;
	Vector3 firstNorm = (firstPtL-firstPtH).crossProduct(firstDir).normalisedCopy();

	pObj->begin(VERTEXCOLOR_LIGHTON,RenderOperation::OT_TRIANGLE_STRIP);
	pObj->position(firstPtH);pObj->colour(color);pObj->normal(firstNorm);
	pObj->position(firstPtL);
	
	for(int i=1;i<slices;i++)
	{
		Degree deg(360.0f*i/slices);
		Vector3 ptL = Vector3(baseRadius*Math::Cos(deg),baseRadius*Math::Sin(deg),baseheight);
		Vector3 ptH = Vector3(topRadius*Math::Cos(deg),topRadius*Math::Sin(deg),topheight);
		Vector3 dir = Vector3(-Math::Sin(deg),Math::Cos(deg),0);
		Vector3 norm = (ptL-ptH).crossProduct(dir).normalisedCopy();

		pObj->position(ptH);pObj->normal(norm);
		pObj->position(ptL);
	}

	pObj->position(firstPtH);pObj->normal(firstNorm);
	pObj->position(firstPtL);
	pObj->end();
}
void DrawDisk(ManualObject* pObj, Real Radius, Real height, int slices,const ColourValue& color )
{
	pObj->begin(VERTEXCOLOR_LIGHTON,RenderOperation::OT_TRIANGLE_FAN);

	pObj->position(0,0,height);pObj->normal(Vector3::UNIT_Z);pObj->colour(color);

	Vector3 firstPt(Radius,0,height); 
	pObj->position(firstPt);
	for(int i=1;i<slices;i++)
	{
		Degree deg(360.0f*i/slices);
		Vector3 pt(Radius*Math::Cos(deg),Radius*Math::Sin(deg),height);
		pObj->position(pt);		
	}
	pObj->position(firstPt);
	pObj->end();
}



void CRenderAirWayPoint3D::Build( ALTObject* pBaseObject, Ogre::ManualObject* pObj )
{
	if(pBaseObject == NULL || pBaseObject->GetType() != ALT_WAYPOINT)
		return;

	AirWayPoint *pWayPointInput = (AirWayPoint *)pBaseObject;
	if(pWayPointInput == NULL)
		return;


	DistanceUnit lowalt = pWayPointInput->GetLowLimit();
	DistanceUnit highalt = pWayPointInput->GetHighLimit();

	const double FixHight=100000.0;
	const double FixTopRad=8000*0.3048;

	double diskrlow=lowalt*FixTopRad/FixHight; 
	double  diskrhigh=highalt*FixTopRad/FixHight;	

	//int nCyclePointCount = 10;

	Ogre::ColourValue color;
	ALTObjectDisplayProp *pDisplay = pWayPointInput->getDisplayProp();
	ASSERT(pDisplay != NULL);
	if(pDisplay)
		color = OgreConvert::GetColor(pDisplay->m_dpShape.cColor);

	pObj->clear();

	const static int slicscount = 24; 
	DrawCylinder(pObj,0,diskrlow,0,lowalt,slicscount,ColourValue::White);
	DrawCylinder(pObj,diskrlow,diskrhigh,lowalt,highalt,slicscount,color);
	DrawDisk(pObj,diskrhigh,highalt,slicscount,ColourValue::White);	

}


CPoint2008 CRenderAirWayPoint3D::GetLocation( AirWayPoint *pWayPointInput ) const
{
	InputAirside *pAirside =   GetRoot().GetModel()->OnQueryInputAirside();
	ALTAirportLayout *pAirport = pAirside->GetActiveAirport();
	if(pAirport)
        return pWayPointInput->GetLocation(*pAirport) -  ARCVector3(0,0,pAirport->getElevation() );
	
	return pWayPointInput->GetServicePoint();
}

void CRenderAirWayPoint3D::Setup3D( AirWayPoint *pWayPointInput )
{
	Update3D(pWayPointInput);
}










