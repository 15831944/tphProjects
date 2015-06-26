#include "StdAfx.h"
#include ".\nonpaxrelativeposdefscene.h"
#include <common/Grid.h>
#include <common/util.h>
#include <Inputs/NonPaxRelativePosSpec.h>
#include <common/Guid.h>
#include <common/ARCStringConvert.h>

using namespace Ogre;
const static ARCVector3 mCenter = ARCVector3(0,0,85);

void CNonPaxRelativePosDefScene::SetupCamera()
{
	//mpTopCamera = CreateCamera();
	//mpLeftCamera = CreateCamera();
	//mpPerspectCamera = CreateCamera();
	//mpFrontCamera = CreateCamera();
	mCamera3D = CreateCamera();

	//mpPerpCamNode = getSceneManager()->createSceneNode();
	//mpPerpCamNode->attachObject(mpPerspectCamera);
	//mpPerpCamNode->attachObject(mpTopCamera);
	//mpPerpCamNode->attachObject(mpLeftCamera);
	//mpPerpCamNode->attachObject(mpFrontCamera);
	//mpPerpCamNode->setPosition(mCenter);
	//
	RestoreCameraToDefault(camPerspect);
	RestoreCameraToDefault(camLeft);
	RestoreCameraToDefault(camFront);
	RestoreCameraToDefault(camTop);
	
	SetCamera(camPerspect);

}

void CNonPaxRelativePosDefScene::SetupScene()
{
	if(!MakeSureInited())
		return ;



	//add extra setup code
	//1. set up grid
	//m_pSceneManager->setsha
	//getSceneManager()->setShadowTechnique(SHADOWTYPE_NONE);
	//	m_pSceneManager->setShadowColour( SceneConfig::VisibleRegionColor );
	//getSceneManager()->setAmbientLight(ColourValue::White * 0.7);

	//setup light
	//light 1
	/*Vector3 dir(0,0,-1);
	dir.normalise();
	Light* l = getSceneManager()->createLight("mylight1");
	l->setDiffuseColour( ColourValue::White * .7 );
	l->setType(Light::LT_DIRECTIONAL);	
	l->setDirection(dir);
	l->setCastShadows(true);*/


	//scene grid
	CGrid griddata;
	//griddata.InitDefault();
	//griddata.dMaxX = 2*SCALE_FACTOR;
	//griddata.dMinX =-2 *SCALE_FACTOR;
	//griddata.dMaxY = 2*SCALE_FACTOR;
	//griddata.dMinY = -2*SCALE_FACTOR;
	griddata.dSizeX = 2*SCALE_FACTOR;
	griddata.dSizeY = 2*SCALE_FACTOR;
	griddata.dLinesEvery = 0.1*SCALE_FACTOR;

	m_sceneGrid = AddSceneNode("MainGrid");	
	m_sceneGrid.Update(griddata);
	//animation pax
	m_pAnimPax = CAnimaPax3D(0,AddSceneNode("Pax1"));
	//m_pAnimPax->AttachTo(m_pSceneManager->getRootSceneNode());
	m_pAnimPax.SetShape(_T("Casual Man 1"));
	//m_pAnimPax.SetRotation(ARCVector3(0,0,90));
	//m_pAnimPax->SetqueryFlag(UnSelectable);
	////m
	m_pAnimPax.SetAnimationData(PaxAnimationData(PaxWalking));
	//m_pAnimPax->SetRotation(90);
	//
	Ogre::Entity* pCoord = OgreUtil::createOrRetrieveEntity("Coord","coord.mesh",getSceneManager());
	ASSERT(pCoord);
	GetRoot().AddObject(pCoord);

	//mCenter = Vector3(0,0,85);
	SetupCamera();	
	//C3DScene::SetupScene();
}
void CNonPaxShape3D::FlushChangeToData()
{
	if(!mpNode)
		return;

	const Vector3& newPos = mpNode->getPosition();
	
	//Rotate(ARCVector3(0,0,-90));
	Ogre::Quaternion quat = mpNode->getOrientation();	
	//Rotate(ARCVector3(0,0,90));

	if(mpData)
	{
		mpData->m_rotate = ARCVector4(quat.x,quat.y,quat.z,quat.w);
		mpData->m_relatePos = CPoint2008(newPos.x,newPos.y,newPos.z);
	}
}

CNonPaxRelativePosDefScene::CNonPaxRelativePosDefScene()
{
	///m_pAnimPax = NULL;
	//mpTopCamera = mpLeftCamera = mpPerspectCamera = mpFrontCamera = NULL;

	//init shape name map
	m_shapeNameMap[1] = _T("Casual Man 3");
	m_shapeNameMap[2] = _T("Check Bag");
	m_shapeNameMap[3] = _T("Hand Bag");
	m_shapeNameMap[8] = _T("Laptop Bag");
	m_shapeNameMap[9] = _T("Laptop");
	m_shapeNameMap[10] = _T("Rollaboard Open");
	m_shapeNameMap[11] = _T("Rollaboard Closed");
	m_shapeNameMap[12] = _T("Coat");
	m_shapeNameMap[13] = _T("Shoes");
	m_shapeNameMap[14] = _T("Keys");
	m_shapeNameMap[15] = _T("X-Ray Tray");
	m_shapeNameMap[16] = _T("Backpack");
	m_shapeNameMap[17] = _T("Rollaboard Open");
	//init camera date

}


void CNonPaxRelativePosDefScene::SetCamera( emCameraType camType )
{
	switch(camType)
	{
	case camTop:
		mCamera3D.Load(mpTopCamera);
		break;
	case camLeft:
		mCamera3D.Load(mpLeftCamera);
		break;
	case camFront:
		mCamera3D.Load(mpFrontCamera);
		break;
	case camPerspect:
		mCamera3D.Load(mPerspectCamera);
		break;
	default:
		NULL;
	}
	m_curCamType =camType;
}

void CNonPaxRelativePosDefScene::RestoreCameraToDefault( emCameraType camType )
{

	if(camType == camTop )
	{
		mpTopCamera.m_vLocation = mCenter + ARCVector3(0,0,3*SCALE_FACTOR);
		mpTopCamera.m_eProjType = CCameraData::parallel;
		mpTopCamera.m_vUp = ARCVector3(1,0,0);
		mpTopCamera.m_vLook = ARCVector3(0,0,-1);
		mpTopCamera.dWorldWidth = 3*SCALE_FACTOR;		
	}
	if(camType == camLeft )
	{
		mpLeftCamera.m_vLocation = mCenter - ARCVector3(0,3*SCALE_FACTOR,0);
		mpLeftCamera.m_eProjType = CCameraData::parallel;
		mpLeftCamera.m_vUp = ARCVector3(0,0,1);
		mpLeftCamera.m_vLook = ARCVector3(0,1,0);
		mpLeftCamera.dWorldWidth = 3*SCALE_FACTOR;			
	}
	if(camType == camPerspect )
	{
		mPerspectCamera.m_vLocation = mCenter+ ARCVector3(3*SCALE_FACTOR,0,2*SCALE_FACTOR);
		mPerspectCamera.m_eProjType = CCameraData::perspective;
		mPerspectCamera.m_vUp = ARCVector3(0,0,1);
		mPerspectCamera.m_vLook = -ARCVector3(3,0,2);
		mPerspectCamera.dFovy = 30;				
	}
	if(camType == camFront )
	{
		mpFrontCamera.m_vLocation = mCenter + ARCVector3(3*SCALE_FACTOR,0,0);
		mpFrontCamera.m_eProjType = CCameraData::parallel;
		mpFrontCamera.m_vUp = ARCVector3(0,0,1);
		mpFrontCamera.m_vLook = -ARCVector3(1,0,0);
		mpFrontCamera.dWorldWidth = 3*SCALE_FACTOR;		
	}

	if(!mCamera3D)
		return;
}

inline static double clampInRage(double inval, const double& dmin, const double& dmax)
{	
	return std::min(dmax,std::max(dmin,inval));
}

//void CNonPaxRelativePosDefScene::ZoomCam( double percent )
//{
//	if(GetActiveCamera())
//	{
//		Vector3 vecpos  = GetActiveCamera()->getPosition();
//		Vector3 vecDir = GetActiveCamera()->getDirection();
//		double dLen = vecpos.length();
//
//		double dOffset = dLen * percent;
//		if(GetActiveCamera() == mpPerspectCamera)
//		{
//			double dnextlen = dLen+dLen*percent;
//			dnextlen = clampInRage(dnextlen,1*SCALE_FACTOR,8*SCALE_FACTOR);
//			dOffset = dnextlen-dLen;
//		}	
//		vecpos -= vecDir * dOffset;
//
//		if(GetActiveCamera() == mpLeftCamera)
//		{
//			vecpos.y = clampInRage(vecpos.y,-6*SCALE_FACTOR,-1*SCALE_FACTOR);
//		}
//		if(GetActiveCamera() == mpFrontCamera)
//		{
//			vecpos.x = clampInRage(vecpos.x,1*SCALE_FACTOR,6*SCALE_FACTOR);
//		}
//		if(GetActiveCamera() == mpTopCamera)
//		{
//			vecpos.z = clampInRage(vecpos.z,1*SCALE_FACTOR,6*SCALE_FACTOR);
//		}
//
//		GetActiveCamera()->setPosition(vecpos);
//
//		dLen = GetActiveCamera()->getPosition().length();
//		if(GetActiveCamera()!=mpPerspectCamera)
//		{
//			GetActiveCamera()->setOrthoWindowWidth( dLen );
//		}
//
//	}
//
//}
//
//void CNonPaxRelativePosDefScene::RotateCam( double dAnlge,double yawAngle )
//{
//	if(GetActiveCamera() == mpPerspectCamera && mpPerpCamNode)
//	{
//		mpPerpCamNode->yaw(Radian(yawAngle));
//		mpPerpCamNode->rotate(Vector3::UNIT_Z,-Radian(dAnlge),Node::TS_WORLD);
//		/*	Vector3 camPos = mpPerspectCamera->getPosition();
//		camPos.distance(mCenter);
//		Vector3 camDir = camPos - mCenter;
//
//
//		Vector3 newCamPos = mCenter+camDir2;
//		mpPerspectCamera->setPosition(newCamPos);
//		mpPerspectCamera->setDirection(mCenter-newCamPos);*/
//
//
//		/*	Vector3 camDir = mpPerspectCamera->getPosition() - mCenter;
//
//		mpPerspectCamera->moveRelative(-camDir);
//		Quaternion quat;
//		quat.FromAngleAxis(Radian(dAnlge),Vector3::UNIT_X);
//		mpPerspectCamera->rotate( quat );
//		mpPerspectCamera->moveRelative(-dLen*Vector3::UNIT_Z);*/
//	}
//
//
//}
//
Ogre::Plane CNonPaxRelativePosDefScene::getFocusPlan() const
{


	//if(getc() /*&& GetActiveCamera()!= mpPerspectCamera*/)
	//{
	//	Vector3 dir = -GetActiveCamera()->getDerivedDirection();
	//	dir.normalise();
	//	if(GetActiveCamera() != mpPerspectCamera)
	//	{
	//		
	//	}
	//	else if( abs(dir.z) < 0.3 )
	//	{
	//		return Ogre::Plane(dir,pt);
	//	}
	//}
	Vector3 dir = mCamera3D.mpCamera->getDerivedDirection();
	if(abs(dir.z)<0.3)
	{
		return Ogre::Plane(dir,Vector3::ZERO);
	}	
	return Ogre::Plane(Vector3::UNIT_Z,Vector3::ZERO);
}

void CNonPaxRelativePosDefScene::AddUpdate( CNonPaxRelativePos* pPosData )
{
	CNonPaxShape3D p3DShape = Get3DShape(pPosData);
	if(!p3DShape){ //update data
		p3DShape= CNonPaxShape3D( pPosData,AddSceneNode(ARCStringConvert::toString((int)pPosData)) );	
		mvNonPaxlist.push_back(p3DShape);
		//p3DShape->AttachTo(m_pSceneManager->getRootSceneNode());
		CString strShape =  GetShapeName(pPosData->m_nTypeIdx);
		p3DShape.SetShape( strShape );
		//p3DShape->SetqueryFlag(ModelEntity);
	}
	p3DShape.UpdateDisplay();
}

CString CNonPaxRelativePosDefScene::GetShapeName(  int nTypeIdx ) const
{
	std::map<int,CString>::const_iterator itr =  m_shapeNameMap.find(nTypeIdx);
	if(itr!=m_shapeNameMap.end())
	{
		return itr->second;
	}
	return _T("");
}



void CNonPaxRelativePosDefScene::RemoveShape3D( CNonPaxShape3D p3d )
{
	std::vector<CNonPaxShape3D>::iterator itr = std::find(mvNonPaxlist.begin(),mvNonPaxlist.end(),p3d);
	if(itr!=mvNonPaxlist.end())
	{
		mvNonPaxlist.erase(itr);		
	}
	p3d.Detach();
}



CNonPaxShape3D CNonPaxRelativePosDefScene::Get3DShape( CNonPaxRelativePos* pData ) const
{
	for(size_t i=0;i<mvNonPaxlist.size();i++)
		if(mvNonPaxlist[i].mpData == pData)
			return mvNonPaxlist[i];
	return CNonPaxShape3D();
}

Ogre::Camera* CNonPaxRelativePosDefScene::CreateCamera()
{
	return OgreUtil::createOrRetrieveCamera( CGuid::Create(), getSceneManager() );
}

CNonPaxShape3D CNonPaxRelativePosDefScene::GetSelectNode( Ogre::MovableObject* pObj ) const
{
	for(size_t i=0;i<mvNonPaxlist.size();i++)
	{
		const CNonPaxShape3D& pShape = mvNonPaxlist[i];
		if(pShape.HasMovableObj(pObj))
			return pShape;
	
	}
	return CNonPaxShape3D();
}
void CNonPaxShape3D::UpdateDisplay()
{
	if(!mpData)
		return ;

	CPoint2008 pt = mpData->m_relatePos;
	SetPosition(ARCVector3(pt.getX(),pt.getY(),pt.getZ()));
	SetRotation(mpData->m_rotate);
	//Rotate(ARCVector3(0,0,90));

	if(mpData->m_nTypeIdx == 1 )
	{
		SetAnimationData(PaxAnimationData(PaxWalking));
		//Ogre::Quaternion quat; quat.FromAngleAxis(Degree(90),Vector3::UNIT_Z);		
		SetRotation(mpData->m_rotate);
		//Rotate(ARCVector3(0,0,90));
	}
}

void CNonPaxShape3D::DoRotate( double dAngle, const ARCVector3& axis )
{
	if(mpData)
	{
		Quaternion quat;
		quat.FromAngleAxis(Radian(dAngle),OGREVECTOR(axis) );
		Quaternion orignq(mpData->m_rotate.w,mpData->m_rotate.x,mpData->m_rotate.y,mpData->m_rotate.z);
		Quaternion destq = quat* orignq;
		mpData->m_rotate = ARCVector4(destq.x,destq.y,destq.z,destq.w);
	}
	UpdateDisplay();
}

void CNonPaxShape3D::Move( const ARCVector3& offset )
{
	if(mpData)
	{
		mpData->m_relatePos =  mpData->m_relatePos  + CPoint2008(offset.x,offset.y,offset.z);
	}
	UpdateDisplay();
}

CNonPaxShape3D::CNonPaxShape3D( CNonPaxRelativePos* pPosData,  Ogre::SceneNode* pNode ) :CAnimaPax3D(0,pNode)
{
	mpData = pPosData;
}
