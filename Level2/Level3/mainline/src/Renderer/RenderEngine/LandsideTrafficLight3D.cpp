
#include "stdafx.h"
 #include <common/Path2008.h>
#include "CommonShapes.h"
#include ".\SceneNodeQueryData.h"


#include "LandsideTrafficLight3D.h"

#define MESH_TRAFFICLIGHT "light.mesh"
#define MESH_TRAFFICLIGHT_PLAN "light_plan.mesh"
#define MESH_TRAFFICLIGHT_LEFT_CROSS "left_cross_light.mesh"
#define MESH_TRAFFICLIGHT_LEFT_CROSS_PLAN "left_cross_light_plan.mesh"
#define MESH_TRAFFICLIGHT_RIGHT_CROSS "right_cross_light.mesh"
#define MESH_TRAFFICLIGHT_RIGHT_CROSS_PLAN "right_cross_light_plan.mesh"

#define MAT_LANDSIDE_TRAFFICLIGHT_RED "trafficlight/red"
#define MAT_LANDSIDE_TRAFFICLIGHT_GREEN "trafficlight/green"
#define MAT_LANDSIDE_TRAFFICLIGHT_YELLOW "trafficlight/_Gold_"
#define MAT_LANDSIDE_TRAFFICLIGHT_GRAY "trafficlight/_Gainsboro_"


// template<class DataType>
void LandsideTrafficLight3D::Update3D()
{
	SceneNodeQueryData* pqdata=NULL;
	if(pqdata = getQueryData())
	{
		CPoint2008 pos,dir;
		int nStretchID,nLaneIndex, nGroupSize;
		double nLaneWidth;
		CString sName,sMesh;

		pqdata->GetData("PosX",pos.x);
		pqdata->GetData("PosY",pos.y);
		pqdata->GetData("PosZ",pos.z);
		pqdata->GetData("DirX",dir.x);
		pqdata->GetData("DirY",dir.y);
		pqdata->GetData("DirZ",dir.z);
		pqdata->GetData("KeyStretchID",nStretchID);
		pqdata->GetData("KeyLaneIndex",nLaneIndex);
		pqdata->GetData("KeyGroupSize",nGroupSize);
		pqdata->GetData("KeyLaneWidth",nLaneWidth);
		pqdata->GetData("LightEntityName",sName);
		pqdata->GetData("KeyMesh",sMesh);

		Ogre::Entity* light = OgreUtil::createOrRetrieveEntity(sName, sMesh, GetScene());
		if(!light) 
			return;

		SetPosition(pos);
		SetDirection(dir, ARCVector3(0,0,1));
		AddObject(light);
	}
}
void LandsideTrafficLight3D::BuildPole(ManualObject* pObj,double poleLeftLength,double poleRightLength)
{
	const String& sMat ="TrafficLight/Pole";
	pObj->begin(sMat);

	double height=250.0;

	Vector3 norm;

	pObj->position(poleLeftLength,0,height);//0
	norm=Vector3(1,-1,-1);
	pObj->normal(norm);

	pObj->position(-poleRightLength,0,height);//1
	norm=Vector3(-1,-1,-1);
	pObj->normal(norm);

	pObj->position(-poleRightLength,0,height+20);//2
	norm=Vector3(-1,-1,1);
	pObj->normal(norm);

	pObj->position(poleLeftLength,0,height+20);//3
	norm=Vector3(1,-1,1);
	pObj->normal(norm);

	pObj->position(poleLeftLength,20,height);//4
	norm=Vector3(1,1,-1);
	pObj->normal(norm);

	pObj->position(-poleRightLength,20,height);//5
	norm=Vector3(-1,1,-1);
	pObj->normal(norm);

	pObj->position(-poleRightLength,20,height+20);//6
	norm=Vector3(-1,1,1);
	pObj->normal(norm);

	pObj->position(poleLeftLength,20,height+20);//7
	norm=Vector3(1,1,1);
	pObj->normal(norm);



	pObj->quad(0,3,2,1);
	pObj->quad(4,5,6,7);
	pObj->quad(3,7,6,2);
	pObj->quad(0,1,5,4);
	pObj->quad(0,4,7,3);
	pObj->quad(1,2,6,5);
	pObj->end();
}
//template<class DataType>
void LandsideTrafficLight3D::SetRed(Ogre::Entity* light, bool bEnable)
{
	if(bEnable) 
		light->getSubEntity(0)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_LANDSIDE_TRAFFICLIGHT_RED));
	else
		light->getSubEntity(0)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_LANDSIDE_TRAFFICLIGHT_GRAY));
}

//template<class DataType>
void LandsideTrafficLight3D::SetYellow(Ogre::Entity* light, bool bEnable)
{
	if(bEnable) 
		light->getSubEntity(1)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_LANDSIDE_TRAFFICLIGHT_YELLOW));
	else
		light->getSubEntity(1)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_LANDSIDE_TRAFFICLIGHT_GRAY));
}

//template<class DataType>
void LandsideTrafficLight3D::SetGreen(Ogre::Entity* light, bool bEnable)
{
	if(bEnable) 
		light->getSubEntity(2)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_LANDSIDE_TRAFFICLIGHT_GREEN));
	else
		light->getSubEntity(2)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_LANDSIDE_TRAFFICLIGHT_GRAY));
}

void LandsideTrafficLight3D::SetCrossRed(Ogre::Entity* light)
{ 
	light->getSubEntity(3)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_LANDSIDE_TRAFFICLIGHT_RED));
}
// 
//template<class DataType>
void LandsideTrafficLight3D::SetCrossGreen(Ogre::Entity* light)
{
	light->getSubEntity(3)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_LANDSIDE_TRAFFICLIGHT_GREEN));
}
//template<class DataType>
void LandsideTrafficLight3D::SetCrossYellow(Ogre::Entity* light)
{
	light->getSubEntity(3)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_LANDSIDE_TRAFFICLIGHT_YELLOW));
}

//template<class DataType>
void LandsideTrafficLight3D::SetRed(bool isCrossOver)
{
	SceneNodeQueryData* pqdata=getQueryData();
	if(!pqdata)
		return;

	CString sName,sMesh;
	int nGroupSize;
	pqdata->GetData("KeyGroupSize",nGroupSize);
	pqdata->GetData("LightEntityName",sName);
	pqdata->GetData("KeyMesh",sMesh);
	if (sName==_T(""))
		return;

	Ogre::Entity* light = NULL;
	light = OgreUtil::createOrRetrieveEntity(sName, sMesh, GetScene());
	if(!light) 
		return;
	switch(nGroupSize)
	{
	case 1:
		SetRed(light, true);
		SetGreen(light, false);
		SetYellow(light, false);
		break;
	case 2:
		if (isCrossOver == false)
		{
			SetRed(light, true);
			SetGreen(light, false);
			SetYellow(light, false);
		}
		else
			SetCrossRed(light);
		break;
	default:
		break;
	}
}

//template<class DataType>
void LandsideTrafficLight3D::SetGreen(bool isCrossOver)
{
	SceneNodeQueryData* pqdata=getQueryData();
	if(!pqdata)
		return;

	CString sName,sMesh;
	int nGroupSize;
	pqdata->GetData("KeyGroupSize",nGroupSize);
	pqdata->GetData("LightEntityName",sName);
	pqdata->GetData("KeyMesh",sMesh);
	if (sName==_T(""))
		return;

	Ogre::Entity* light = NULL;
	light = OgreUtil::createOrRetrieveEntity(sName, sMesh, GetScene());
	if(!light) 
		return;
	switch(nGroupSize)
	{
	case 1:
		SetRed(light, false);
		SetGreen(light, true);
		SetYellow(light, false);
		break;
	case 2:
		if (isCrossOver == false)
		{
			SetRed(light, false);
			SetGreen(light, true);
			SetYellow(light, false);
		}
		else
			SetCrossGreen(light);
		break;
	default:
		break;
	}
}
//template<class DataType>
void LandsideTrafficLight3D::SetYellow(bool isCrossOver)
{
	SceneNodeQueryData* pqdata=getQueryData();
	if(!pqdata)
		return;

	CString sName,sMesh;
	int nGroupSize;
	pqdata->GetData("KeyGroupSize",nGroupSize);
	pqdata->GetData("LightEntityName",sName);
	pqdata->GetData("KeyMesh",sMesh);
	if (sName==_T(""))
		return;

	Ogre::Entity* light = NULL;
	light = OgreUtil::createOrRetrieveEntity(sName, sMesh, GetScene());
	if(!light) 
		return;
	switch(nGroupSize)
	{
	case 1:
		SetRed(light, false);
		SetGreen(light, false);
		SetYellow(light, true);
		break;
	case 2:
		if (isCrossOver == false)
		{
			SetRed(light, false);
			SetGreen(light, false);
			SetYellow(light, true);
		}
		else
			SetCrossYellow(light);
		break;
	default:
		break;
	}
}

void LandsideTrafficLight3D::SetLocationData(CPoint2008 pos,ARCVector3 dir,int nStretchID,/*int nLaneIndex,*/int nGroupSize,double nLaneWidth,CString sName,CString sMesh)
{
	if(SceneNodeQueryData* pqdata = InitQueryData())
	{
		pqdata->SetData("PosX",pos.x);
		pqdata->SetData("PosY",pos.y);
		pqdata->SetData("PosZ",pos.z);

		pqdata->SetData("DirX",dir.x);
		pqdata->SetData("DirY",dir.y);
		pqdata->SetData("DirZ",dir.z);

		pqdata->SetData("KeyStretchID",nStretchID);
		//pqdata->SetData("KeyLaneIndex",nLaneIndex);
		pqdata->SetData("KeyGroupSize",nGroupSize);
		pqdata->SetData("KeyLaneWidth",nLaneWidth);

		pqdata->SetData("LightEntityName",sName);
		pqdata->SetData("KeyMesh",sMesh);
	}
}