#include "StdAfx.h"
#include ".\camera.h"

#include "../../Common/ARCVector.h"
#include <common/util.h>
#include <common\Camera.h>
#include "OgreConvert.h"
#include "3DNodeObject.h"
#include ".\3Dscene.h"

using namespace Ogre;
using namespace std;


void CCameraNode::Set3D( double fovy )
{	
	if(!IsValid())
	{
		ASSERT(FALSE);
		return;
	}

	mpCamera->setProjectionType(PT_PERSPECTIVE);
	mpCamera->setFOVy( Degree(fovy) );
	mpCamera->setAutoAspectRatio(true);
}

void CCameraNode::Set2D( double worldWidth )
{
	if(!IsValid())
	{
		ASSERT(FALSE);
		return;
	}


	mpCamera->setProjectionType(PT_ORTHOGRAPHIC);
	mpCamera->setAutoAspectRatio(true);
	mpCamera->setOrthoWindowWidth(worldWidth);
}

void CCameraNode::Set2D( double worldWidth,double worldHeight )
{	
	if(!IsValid())
	{
		ASSERT(FALSE);
		return;
	}
	mpCamera->setAutoAspectRatio(false);
	mpCamera->setProjectionType(PT_ORTHOGRAPHIC);
	mpCamera->setOrthoWindow(worldWidth,worldHeight);
}

void CCameraNode::LookAt( const ARCVector3& vcamPos, const ARCVector3& vlookPos, const ARCVector3& vUpdir )
{
	if(!IsValid())
	{
		ASSERT(FALSE);
		return;
	}


	//ASSERT(mpCamera->getParentSceneNode());
	SceneNode* pNode= mpCamera->getParentSceneNode();

	ARCVector3 vDir = vlookPos - vcamPos;
	vDir.Normalize();



	pNode->setPosition(OGREVECTOR(vcamPos));
	pNode->setOrientation(Quaternion::IDENTITY);
	
	Vector3 vCamDir = mpCamera->getRealDirection();
	//fix up dir
	Vector3 destDir = OGREVECTOR(vDir);
	Vector3 destUp = OGREVECTOR(vUpdir);
	Vector3 destRight  = destDir.crossProduct(destUp);
	destUp = destRight.crossProduct(destDir);

	Quaternion quat = vCamDir.getRotationTo(destDir);
	pNode->rotate(quat,Node::TS_PARENT);
	//Vector3 retDir = mpCamera->getRealDirection();

	Vector3 vCamUp = mpCamera->getRealUp();
	quat = vCamUp.getRotationTo(destUp);	
	pNode->rotate(quat,Node::TS_PARENT);
	//Vector3 retUp = mpCamera->getRealUp();

}

void CCameraNode::Pan( const CPoint& mouseDiff,int vpHeight, const Ogre::Plane& plan )
{
	if(!IsValid())
	{
		ASSERT(FALSE);
		return;
	}


	Camera* pcamera = mpCamera;
	//ASSERT(pcamera && pcamera->getParentSceneNode());
	//if(pcamera && pcamera->getParentSceneNode() )
	{
		if(pcamera->getProjectionType()==PT_PERSPECTIVE)
		{
			Vector3 vRight = pcamera->getRealRight();
			Vector3 vUp = pcamera->getRealUp();				
			SceneNode* pNode= pcamera->getParentSceneNode();
			double dFoucesDist = GetFocusDist(plan);
			double dRat = Math::Tan(pcamera->getFOVy()*0.5)*dFoucesDist*2.0/vpHeight;
			pNode->translate( -mouseDiff.x * vRight * dRat  + mouseDiff.y * vUp * dRat,Node::TS_PARENT );	

		}
		else
		{
			double dWidth = pcamera->getOrthoWindowWidth();	
			double dHeight = pcamera->getOrthoWindowHeight();

			Vector3 vRight = pcamera->getRealRight();
			Vector3 vUp = pcamera->getRealUp();

			SceneNode* pNode= pcamera->getParentSceneNode();			
			pNode->translate( -dHeight /vpHeight* mouseDiff.x * vRight + dHeight/vpHeight * mouseDiff.y * vUp,Node::TS_PARENT );	
		}

		UpdateNearFar();
	}
}

void CCameraNode::Tumble( const CPoint& mouseDiff, const Ogre::Plane& plan )
{
	if(!IsValid())
	{
		ASSERT(FALSE);
		return;
	}


	Camera* pcamera = mpCamera;
	//ASSERT(pcamera && pcamera->getParentSceneNode() );*/
	{		
		if(pcamera->getProjectionType()==PT_PERSPECTIVE)
		{
			double dFoucuseDist = GetFocusDist(plan);

			SceneNode* pNode= pcamera->getParentSceneNode();
			pNode->translate(pcamera->getRealDirection() * dFoucuseDist,Node::TS_PARENT);	
			Quaternion qRight;
			qRight.FromAngleAxis( Ogre::Radian(-mouseDiff.y/100.0),  pcamera->getRealRight() );
			pNode->rotate( qRight ,Node::TS_PARENT);

			Quaternion qUp;
			qUp.FromAngleAxis( Ogre::Radian(-mouseDiff.x/100.0), Vector3::UNIT_Z );
			pNode->rotate( qUp ,Node::TS_PARENT);

			pNode->translate(-pcamera->getRealDirection() * dFoucuseDist ,Node::TS_PARENT);
		}
		else
		{
			SceneNode* pNode= pcamera->getParentSceneNode();
			Quaternion qUp;
			qUp.FromAngleAxis( Ogre::Radian(-mouseDiff.x/100.0), Vector3::UNIT_Z);
			pNode->rotate( qUp ,Node::TS_PARENT);
		}

		UpdateNearFar();
	}
}

void CCameraNode::ZoomInOut( const CPoint& mouseDiff, const Ogre::Plane& plan )
{
	if(!IsValid())
	{
		ASSERT(FALSE);
		return;
	}


	Camera* pcamera = mpCamera;
	{
		SceneNode* pNode = pcamera->getParentSceneNode();
		double ZoomFactor = 0.003;

		if(pcamera->getProjectionType()==PT_PERSPECTIVE)
		{
			double dFoucuseDist = GetFocusDist(plan);
			dFoucuseDist = MAX(10,dFoucuseDist);
			double dOffset = dFoucuseDist * mouseDiff.x * ZoomFactor ;
			pNode->translate( dOffset  * pcamera->getRealDirection(),Node::TS_PARENT );
		}
		else
		{		
		
			double dWidth = pcamera->getOrthoWindowWidth();			
			dWidth = std::max(1.0,dWidth*(1.0 - ZoomFactor*mouseDiff.x) );
			pcamera->setOrthoWindowWidth(dWidth);
		}
		UpdateNearFar();
	}
}



void CCameraNode::UpdateNearFar()
{
	OgreUtil::UpdateCameraNearFar(mpCamera);
}

void CCameraNode::SetNearFar( double dNear, double dFar )
{
	if(!IsValid())
	{
		ASSERT(FALSE);
		return;
	}


	mpCamera->setNearClipDistance(dNear);
	mpCamera->setFarClipDistance(dFar);
}

double CCameraNode::GetFocusDist( const Ogre::Plane& plan )
{
	double dFoucesDist = Math::POS_INFINITY;
	Ray mouseRay(mpCamera->getRealPosition(),mpCamera->getRealDirection());
	std::pair<bool, Real> vRestl1 = mouseRay.intersects(plan);
	if(vRestl1.first)
	{
		dFoucesDist = vRestl1.second;
	}

	dFoucesDist = MAX(mpCamera->getNearClipDistance(), dFoucesDist);
	dFoucesDist = MIN(mpCamera->getFarClipDistance(),dFoucesDist);			
	return dFoucesDist;
}

void CCameraNode::Load( const CCameraData& camera )
{
	if(!IsValid())
	{
		ASSERT(FALSE);
		return;
	}

	LookAt(camera.m_vLocation,camera.m_vLook+camera.m_vLocation,camera.m_vUp);
	if(camera.m_eProjType==CCameraData::parallel)
	{
		Set2D(camera.dWorldWidth);
	}
	else
	{
		Set3D( camera.dFovy );
	}
}

void CCameraNode::Save( CCameraData& camera )
{
	if(!IsValid())
	{
		ASSERT(FALSE);
		return;
	}


	Vector3 pos = mpCamera->getRealPosition();
	Vector3 dir = mpCamera->getRealDirection();
	Vector3 updir = mpCamera->getRealUp();
	camera.m_vLocation = ARCVECTOR(pos);
	camera.m_vLook = ARCVECTOR(dir);
	camera.m_vUp = ARCVECTOR(updir);
	camera.dFovy = mpCamera->getFOVy().valueDegrees();
	camera.dWorldWidth = mpCamera->getOrthoWindowWidth();
	camera.m_eProjType = (mpCamera->getProjectionType()==PT_PERSPECTIVE)?CCameraData::perspective:CCameraData::parallel;
}



CCameraNode::CCameraNode( Ogre::Camera* pcam )
:mpCamera(pcam)
,C3DSceneNode(pcam->getParentSceneNode())
{
}


bool CCameraNode::MoveCamera( UINT nFlags, const CPoint& ptDiff, C3DSceneBase* pScene )
{
	if(!IsValid())
		return false;

	const Plane& plane = pScene->getFocusPlan();
	if(nFlags & MK_LBUTTON)
	{ 		
		int vpHeight = mpCamera->getViewport()->getActualHeight(); 
		Pan( ptDiff, vpHeight,plane );
		return true;
	}
	else if(nFlags & MK_MBUTTON){
		Tumble(ptDiff, plane);
		return true;
	}
	else if(nFlags & MK_RBUTTON){ 
		ZoomInOut(ptDiff, plane);
		return true;
	}
	return false;
}
