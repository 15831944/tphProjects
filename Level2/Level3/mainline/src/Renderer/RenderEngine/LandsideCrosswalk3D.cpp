#include "StdAfx.h"
#include "landside/Landside.h"
#include "LandsideCrossWalk3D.h"
#include "Landside/ILandsideEditContext.h"
#include "Landside/InputLandside.h"
#include "Landside/LandsideQueryData.h"
#include "./SceneNodeQueryData.h"
#include "Render3DScene.h"
#include "./ShapeBuilder.h"
//#include <Common/ARCConsole.h>

#define MAT_CROSSWALK_ZEBRA "Crosswalk/Zebra/Face"
#define MAT_CROSSWALK_INTERSECTION "Crosswalk/Intersection/Face"
#define MESH_LANDSIDE_CROSSLIGHT_Intersection "CrossTrafficLight-Intersection.mesh"
#define MESH_LANDSIDE_CROSSLIGHT_Left_Pelican "CrossTrafficLight-Left_Pelican.mesh"
#define MESH_LANDSIDE_CROSSLIGHT_Right_Pelican "CrossTrafficLight-Right_Pelican.mesh"

#define MAT_PAXLIGHT_INTER_GREEN "Material_CrossWalk_PaxLight_Green"
#define MAT_PAXLIGHT_INTER_RED "1321514774_3_Material2"

// #define MAT_VLIGHT_PELICAN_GREEN "1321511518_10_Material3"
// #define MAT_VLIGHT_PELICAN_GREEN_GRAY "Material_CrossWalk_PaxLight_Green_Gray"
// #define MAT_VLIGHT_PELICAN_RED "1321511518_8_Material5"
// #define MAT_VLIGHT_PELICAN_RED_GRAY "Material_CrossWalk_PaxLight_Red_Gray"
// #define MAT_VLIGHT_PELICAN_YELLOW "1321511518_1_Material7"
// #define MAT_VLIGHT_PELICAN_YELLOW_GRAY "Material_CrossWalk_PaxLight_Yellow_Gray"
// #define MAT_BUTTONLIGHT_PELICAN_RED "1321511518_5_Material1"
// #define MAT_BUTTONLIGHT_PELICAN_GREEN "Material_CrossWalk_ButtonLight_Green"
// #define MAT_PAXLIGHT_PELICAN_GREEN "Material_CrossWalk_PaxLight_Green"
// #define MAT_PAXLIGHT_PELICAN_RED "1321511518_9_Material2"

// #define MAT_PAXLIGHT_INTER_GREEN "CrossTrafficLight/green"
// #define MAT_PAXLIGHT_INTER_RED "CrossTrafficLight/red"
#define MAT_VLIGHT_PELICAN_GREEN "CrossTrafficLight/green"
#define MAT_VLIGHT_PELICAN_GREEN_GRAY "CrossTrafficLight/green_gray"
#define MAT_VLIGHT_PELICAN_RED "CrossTrafficLight/red"
#define MAT_VLIGHT_PELICAN_RED_GRAY "CrossTrafficLight/red_gray"
#define MAT_VLIGHT_PELICAN_YELLOW "CrossTrafficLight/_Gold_"
#define MAT_VLIGHT_PELICAN_YELLOW_GRAY "CrossTrafficLight/_Gold_gray"
#define MAT_BUTTONLIGHT_PELICAN_GREEN "CrossTrafficLight/green"
#define MAT_BUTTONLIGHT_PELICAN_RED "CrossTrafficLight/red"
#define MAT_PAXLIGHT_PELICAN_GREEN "CrossTrafficLight/green"
#define MAT_PAXLIGHT_PELICAN_GREEN_GRAY "CrossTrafficLight/green_gray"
#define MAT_PAXLIGHT_PELICAN_RED "CrossTrafficLight/red"
#define MAT_PAXLIGHT_PELICAN_RED_GRAY "CrossTrafficLight/red_gray"

PROTYPE_INSTANCE(LandsideCrosswalk3D)

void LandsideCrosswalk3D::OnUpdate3D()
{
	RemoveChild();
	LandsideCrosswalk* pCrosswalk = getCrosswalk();
	LandsideStretch* pStretch = pCrosswalk->getStretch();
	if(!pCrosswalk || !pStretch)	return;
	ILandsideEditContext* pctx = GetLandsideEditContext();
	if(!pctx)return;

	ILayoutObjectDisplay::DspProp& dspShape = pCrosswalk->getDisplayProp().GetProp(ILayoutObjectDisplay::_Shape);

	pCrosswalk->BuildSidePath();
	CPath2008 leftPath = pCrosswalk->getLeftPath();
	CPath2008 rightPath = pCrosswalk->getRightPath();
	pctx->ConvertToVisualPath(leftPath);
	pctx->ConvertToVisualPath(rightPath);
	CPoint2008 ptCenter = leftPath[0]+leftPath[1] + rightPath[0] + rightPath[1];
	ptCenter = ptCenter * 0.25;

	SetPosition(ARCVector3(ptCenter));

	RemoveAllObject();
	//render	
	if(dspShape.bOn)
	{
		//update shape
		ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(), GetScene());
		pObj->clear();
		
		/*if (getCrosswalk()->getCrossType()==Cross_Intersection)
		{
			pObj->begin(MAT_CROSSWALK_INTERSECTION,RenderOperation::OT_TRIANGLE_STRIP);
		}else */
		{
			pObj->begin(MAT_CROSSWALK_ZEBRA,RenderOperation::OT_TRIANGLE_STRIP);
		}
		
		
		leftPath.DoOffset(-ptCenter.x, -ptCenter.y, -ptCenter.z);
		rightPath.DoOffset(-ptCenter.x, -ptCenter.y, -ptCenter.z);

		double dMatWidth = pStretch->getLaneNum()*pStretch->getLaneWidth()/64;
		double dMatLen = pCrosswalk->getWidth() / 256;
		Vector3 prepos1 = OGREVECTOR(leftPath[0]);
		Vector3 prepos2 = OGREVECTOR(leftPath[1]);

		Vector3 pos1 = OGREVECTOR(rightPath[0]);
		Vector3 pos2 = OGREVECTOR(rightPath[1]);
		Vector3 norm = (pos2-pos1).crossProduct(pos1-prepos1).normalisedCopy();

		Vector3 dir=OGREVECTOR(pCrosswalk->getCenterPath()[1]-pCrosswalk->getCenterPath()[0]);

		pObj->position(prepos1);pObj->textureCoord(dMatWidth,0);pObj->normal(norm);
		pObj->position(prepos2);pObj->textureCoord(0,0);

		pObj->position(pos1);pObj->textureCoord(dMatWidth,dMatLen);
		pObj->position(pos2);pObj->textureCoord(0,dMatLen);

		pObj->end();

		CrossType crossType=getCrosswalk()->getCrossType();
		if (crossType==Cross_Pelican || crossType==Cross_Intersection)
		{
			CString sName,sMeshName;
			if (crossType==Cross_Pelican)
			{
				bool bLeftDrive = pctx->isLeftDrive();
				if(bLeftDrive == true)
					sMeshName=MESH_LANDSIDE_CROSSLIGHT_Left_Pelican;
				else
					sMeshName=MESH_LANDSIDE_CROSSLIGHT_Right_Pelican;
			}else
			{
				sMeshName=MESH_LANDSIDE_CROSSLIGHT_Intersection;
			}

			sName.Format("%s-CrossLight1", GetIDName());
			Ogre::Entity* light1 = OgreUtil::createOrRetrieveEntity(sName, sMeshName, GetScene());
			if(!light1) 
				return;
			SceneNode *lightNode1=GetScene()->createSceneNode();
			OgreUtil::AttachMovableObject(light1,lightNode1);
			Vector3 lightPos1=pos1;
			lightNode1->setPosition(lightPos1);

			lightNode1->setDirection(dir,Ogre::Node::TS_LOCAL,Vector3::UNIT_Y);
			GetSceneNode()->addChild(lightNode1);


			sName.Format("%s-CrossLight2", GetIDName());
			Ogre::Entity* light2 = OgreUtil::createOrRetrieveEntity(sName, sMeshName, GetScene());
			if(!light2) 
				return;
			SceneNode *lightNode2=GetScene()->createSceneNode();
			OgreUtil::AttachMovableObject(light2,lightNode2);
			Vector3 lightPos2=prepos2;
			lightNode2->setPosition(lightPos2);

			lightNode2->setDirection(dir,Ogre::Node::TS_LOCAL,Vector3::NEGATIVE_UNIT_Y);
			GetSceneNode()->addChild(lightNode2);

		}
		AddObject(pObj, 2);

	}
	
}

void LandsideCrosswalk3D::OnUpdateEditMode(BOOL b)
{

}

void LandsideCrosswalk3D::StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo)
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideCrosswalk* pCrosswalk = getCrosswalk();	
	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(static_cast<LandsideFacilityLayoutObject*>(pCrosswalk), plandsideCtx->getInput());	
	plandsideCtx->StartEditOp(pModCmd);
	SetInDragging(TRUE);

	OnDrag(lastMouseInfo,curMouseInfo);
}

void LandsideCrosswalk3D::OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideCrosswalk* pCrosswalk = getCrosswalk();	
	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;

	pCrosswalk->DoOffset(ARCVector3(mousediff.x, mousediff.y, mousediff.z));

	plandsideCtx->NotifyViewCurentOperation();
}

void LandsideCrosswalk3D::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->NotifyViewCurentOperation();
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);

}



LandsideCrosswalk* LandsideCrosswalk3D::getCrosswalk()
{
	return (LandsideCrosswalk*)getLayOutObject();
}
void LandsideCrosswalk3D::SetLight(Cross_LightState eState)
{
	LandsideCrosswalk* pCrosswalk = getCrosswalk();

	if(!pCrosswalk) 
		return;
	CrossType crossType=getCrosswalk()->getCrossType();

	if (crossType==Cross_Pelican || crossType==Cross_Intersection)
	{
		CString sName,sMeshName;
		if (crossType==Cross_Pelican)
		{
			ILandsideEditContext* pCtx = GetLandsideEditContext();
			if(!pCtx)
				return;
			bool bLeftDrive = pCtx->isLeftDrive();
			if(bLeftDrive == true)
				sMeshName=MESH_LANDSIDE_CROSSLIGHT_Left_Pelican;
			else
				sMeshName=MESH_LANDSIDE_CROSSLIGHT_Right_Pelican;
		}else
		{
			sMeshName=MESH_LANDSIDE_CROSSLIGHT_Intersection;
		}

		sName.Format("%s-CrossLight1", GetIDName());
		Ogre::Entity* light1 = OgreUtil::createOrRetrieveEntity(sName, sMeshName, GetScene());
		if(!light1) 
			return;

		sName.Format("%s-CrossLight2", GetIDName());
		Ogre::Entity* light2 = OgreUtil::createOrRetrieveEntity(sName, sMeshName, GetScene());
		if(!light2) 
			return;

		switch (eState)
		{
		case CROSSLIGHT_GREEN:
			{
				SetGreen(light1);
				SetGreen(light2);
				break;
			}
		case CROSSLIGHT_RED:
			{
				SetRed(light1);
				SetRed(light2);
				break;
			}
		case CROSSLIGHT_BUFFER:
			{
				SetYellow(light1);
				SetYellow(light2);
				break;
			}
		default:
			{
				break;
			}
		}
	}
}
void LandsideCrosswalk3D::SetRed(Ogre::Entity *light)
{
	CrossType crossType=getCrosswalk()->getCrossType();
	if (crossType==Cross_Pelican)
	{
		//VLIGHT
		light->getSubEntity(0)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_RED_GRAY));
		light->getSubEntity(1)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_YELLOW_GRAY));
		light->getSubEntity(2)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_GREEN));
		//PAXLIGHT
		light->getSubEntity(3)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_RED));
		light->getSubEntity(4)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_GREEN_GRAY));
		//BUTTONLIGHT
		light->getSubEntity(5)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_RED));
//		for (int i=0;i<(int)light->getNumSubEntities();i++)
//		{
//			SubEntity *subEntity=light->getSubEntity(i);
// 			//VLIGHT
// 			if (subEntity->getMaterialName()==MAT_VLIGHT_PELICAN_RED)
// 			{
// 				subEntity->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_RED_GRAY));
// 			}
// 			else if (subEntity->getMaterialName()==MAT_VLIGHT_PELICAN_YELLOW)
// 			{
// 				subEntity->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_YELLOW_GRAY));
// 			}
// 			else if (subEntity->getMaterialName()==MAT_VLIGHT_PELICAN_GREEN_GRAY)
// 			{
// 				subEntity->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_GREEN));
// 			}//PAXLIGHT
// 			else if(subEntity->getMaterialName()==MAT_PAXLIGHT_PELICAN_RED_GRAY)
// 			{
// 				subEntity->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_PAXLIGHT_PELICAN_RED));
// 			}
// 			else if(subEntity->getMaterialName()==MAT_PAXLIGHT_PELICAN_GREEN)
// 			{
// 				subEntity->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_PAXLIGHT_PELICAN_GREEN_GRAY));
// 			}
// 			//BUTTONLIGHT
// 			else if (subEntity->getMaterialName()==MAT_BUTTONLIGHT_PELICAN_GREEN)
// 			{
// 				subEntity->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_BUTTONLIGHT_PELICAN_RED));
// 			}
//		}

	}
	else if (crossType==Cross_Intersection)
	{
		for (int i=0;i<(int)light->getNumSubEntities();i++)
		{
			SubEntity *subEntity=light->getSubEntity(i);
			if(subEntity->getMaterialName()==MAT_PAXLIGHT_INTER_GREEN)
			{
				subEntity->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_PAXLIGHT_INTER_RED));
				break;
			}
		}
	}
}
void LandsideCrosswalk3D::SetGreen(Ogre::Entity *light)
{
	CrossType crossType=getCrosswalk()->getCrossType();
	if (crossType==Cross_Pelican)
	{
// 		for (int i=0;i<(int)light->getNumSubEntities();i++)
// 		{
// 			SubEntity *subEntity=light->getSubEntity(i);
// 			//VLIGHT
// 			if (subEntity->getMaterialName()==MAT_VLIGHT_PELICAN_RED_GRAY)
// 			{
// 				subEntity->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_RED));
// 			}
// 			else if (subEntity->getMaterialName()==MAT_VLIGHT_PELICAN_YELLOW)
// 			{
// 				subEntity->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_YELLOW_GRAY));
// 			}
// 			else if (subEntity->getMaterialName()==MAT_VLIGHT_PELICAN_GREEN)
// 			{
// 				subEntity->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_GREEN_GRAY));
// 			}
// 			//PAXLIGHT
// 			else if(subEntity->getMaterialName()==MAT_PAXLIGHT_PELICAN_RED)
// 			{
// 				subEntity->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_PAXLIGHT_PELICAN_RED_GRAY));
// 			}
// 			else if(subEntity->getMaterialName()==MAT_PAXLIGHT_PELICAN_GREEN_GRAY)
// 			{
// 				subEntity->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_PAXLIGHT_PELICAN_GREEN));
// 			}
// 			//BUTTONLIGHT
// 			else if (subEntity->getMaterialName()==MAT_BUTTONLIGHT_PELICAN_RED)
// 			{
// 				subEntity->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_BUTTONLIGHT_PELICAN_GREEN));
// 			}
// 		}
		//VLIGHT
		light->getSubEntity(0)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_RED));
		light->getSubEntity(1)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_YELLOW_GRAY));
		light->getSubEntity(2)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_GREEN_GRAY));
		//PAXLIGHT
		light->getSubEntity(3)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_RED_GRAY));
		light->getSubEntity(4)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_GREEN));
		//BUTTONLIGHT
		light->getSubEntity(5)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_GREEN));
	}
	else if (crossType==Cross_Intersection)
	{
		for (int i=0;i<(int)light->getNumSubEntities();i++)
		{
			SubEntity *subEntity=light->getSubEntity(i);
			if(subEntity->getMaterialName()==MAT_PAXLIGHT_INTER_RED)
			{
				subEntity->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_PAXLIGHT_INTER_GREEN));
				break;
			}
		}
	}
}
void LandsideCrosswalk3D::SetYellow(Ogre::Entity *light)
{
	CrossType crossType=getCrosswalk()->getCrossType();
	if (crossType==Cross_Pelican)
	{
// 		for (int i=0;i<(int)light->getNumSubEntities();i++)
// 		{
// 			SubEntity *subEntity=light->getSubEntity(i);
// 			if (subEntity->getMaterialName()==MAT_VLIGHT_PELICAN_RED)
// 			{
// 				subEntity->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_RED_GRAY));
// 			}
// 			else if (subEntity->getMaterialName()==MAT_VLIGHT_PELICAN_YELLOW_GRAY)
// 			{
// 				subEntity->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_YELLOW));
// 			}
// 			else if (subEntity->getMaterialName()==MAT_VLIGHT_PELICAN_GREEN)
// 			{
// 				subEntity->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_GREEN_GRAY));
// 			}
// 		}
		//VLIGHT
		light->getSubEntity(0)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_RED_GRAY));
		light->getSubEntity(1)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_YELLOW));
		light->getSubEntity(2)->setMaterial(OgreUtil::createOrRetrieveMaterial(MAT_VLIGHT_PELICAN_GREEN_GRAY));
	}
}