#include "StdAfx.h"
#include ".\scenenodecoordinate.h"
#include "ManualObjectExt.h"
#include "3DScene.h"
#include "ogreutil.h"
using namespace Ogre;
using namespace std;
Ogre::ManualObject* CSceneNodeCoordinate::GetCoordinateElm( emCoordElm elm )
{
	ASSERT(mpNode);
	CString coordName = mpNode->getName().c_str();
	CString strTemp = coordName;
	switch(elm)
	{
	case CE_XAxis :
		strTemp = coordName + _T("X");
		break; 
	case CE_YAxis :
		strTemp = coordName + _T("Y");
		break;
	case CE_ZAxis :
		strTemp = coordName + _T("Z");
		break;
	case CE_XYPlan :
		strTemp = coordName + _T("XY");
		break;
	case CE_YZPlan :
		strTemp = coordName + _T("YZ");
		break;
	case CE_XZPlan :
		strTemp = coordName + _T("XZ");
		break;
	case CE_XRotate :
		strTemp = coordName + _T("XRot");
		break;
	case CE_YRotate :
		strTemp = coordName + _T("YRot");
		break;
	case CE_ZRotate :
		strTemp = coordName + _T("ZRot");
		break;
	default:
		return NULL;
	}
	Ogre::ManualObject* pObj =  OgreUtil::createOrRetrieveManualObject( strTemp,GetScene() );
	pObj->setUserAny( Any((int)elm) );
	pObj->setRenderQueueGroup(RENDER_QUEUE_9);
	pObj->setVisibilityFlags(0);
	pObj->addVisibilityFlags(NormalVisibleFlag);
	pObj->setCastShadows(false);

	return pObj;
}

void CSceneNodeCoordinate::InitOnce()
{
	if(!mpNode->getAttachedObjectIterator().hasMoreElements()) //can not find any drawing stuff , build them
	{
		
		{
			ManualObject* pObj = GetCoordinateElm(CE_XAxis);
			BuildXCoord(pObj);
			OgreUtil::AttachMovableObject(pObj,mpNode);
		}
		{
			ManualObject* pObj =GetCoordinateElm(CE_YAxis);
			BuildYCoord(pObj);
			OgreUtil::AttachMovableObject(pObj,mpNode);

		}
		{
			ManualObject* pObj =GetCoordinateElm(CE_ZAxis);
			BuildZCoord(pObj);
			OgreUtil::AttachMovableObject(pObj,mpNode);

		}
		{
			ManualObject* pObj = GetCoordinateElm(CE_XYPlan);
			BuildXYPlan(pObj);
			OgreUtil::AttachMovableObject(pObj,mpNode);
		}
		{
			ManualObject* pObj = GetCoordinateElm(CE_YZPlan);
			BuildYZPlan(pObj);
			OgreUtil::AttachMovableObject(pObj,mpNode);
		}
		{
			ManualObject* pObj = GetCoordinateElm(CE_XZPlan);
			BuildXZPlan(pObj);
			OgreUtil::AttachMovableObject(pObj,mpNode);
		}
		{
			ManualObject* pObj = GetCoordinateElm(CE_XRotate);
			BuildXRotate(pObj);
			OgreUtil::AttachMovableObject(pObj,mpNode);
		}
		{
			ManualObject* pObj = GetCoordinateElm(CE_YRotate);
			BuildYRotate(pObj);
			OgreUtil::AttachMovableObject(pObj,mpNode);
		}
		{
			ManualObject* pObj = GetCoordinateElm(CE_ZRotate);
			BuildZRotate(pObj);
			OgreUtil::AttachMovableObject(pObj,mpNode);
		}

	}
}


#define ArrowSize 0.1f
#define TextPos  3.5f
#define TWidth  0.1f
#define THeight  0.2f
#define AxisMat "CoordAxis"
#define PlanMat "CoordPlan"
#define RotatMat "CoordRotate"
#define MouseOnMat "CoordMouseOnMat"
#define CirclePtCnt 64
#define CircleRad 2.0f
#define CircleRadIn 1.6f
#define RenderApla 64/255.0f

void CSceneNodeCoordinate::BuildXCoord( Ogre::ManualObject* pObj )
{
	pObj->clear();
	pObj->begin(AxisMat,RenderOperation::OT_LINE_LIST);
	pObj->colour(ColourValue::Red);
	pObj->position(0,0,0);
	pObj->position(2,0,0);
	pObj->end();

	pObj->begin(AxisMat,RenderOperation::OT_TRIANGLE_FAN);
	pObj->colour(ColourValue::Red);
	pObj->position(3,0,0);
	pObj->position(2,-ArrowSize,-ArrowSize);
	pObj->position(2,ArrowSize,-ArrowSize);
	pObj->position(2,ArrowSize,ArrowSize);
	pObj->position(2,-ArrowSize,ArrowSize);
	pObj->position(2,-ArrowSize,-ArrowSize);
	pObj->end();




	pObj->begin(AxisMat,RenderOperation::OT_TRIANGLE_FAN);
	pObj->colour(ColourValue::Red);
	pObj->position(2,-ArrowSize,-ArrowSize);
	pObj->position(2,-ArrowSize,ArrowSize);
	pObj->position(2,ArrowSize,ArrowSize);
	pObj->position(2,ArrowSize,-ArrowSize);
	pObj->end();


	pObj->begin(AxisMat,RenderOperation::OT_LINE_LIST);
	pObj->colour(ColourValue::Red);
	pObj->position(TextPos-THeight,TWidth,0);
	pObj->position(TextPos+THeight,-TWidth,0);
	pObj->position(TextPos-THeight,-TWidth,0);
	pObj->position(TextPos+THeight,TWidth,0);
	pObj->end();

}

void CSceneNodeCoordinate::BuildYCoord( Ogre::ManualObject* pObj )
{
	pObj->clear();
	pObj->begin(AxisMat,RenderOperation::OT_LINE_LIST);
	pObj->colour(ColourValue::Green);
	pObj->position(0,0,0);
	pObj->position(0,2,0);
	pObj->end();

	pObj->begin(AxisMat,RenderOperation::OT_TRIANGLE_FAN);
	pObj->colour(ColourValue::Green);
	pObj->position(0,3,0);
	pObj->position(-ArrowSize,2,-ArrowSize);
	pObj->position(-ArrowSize,2,ArrowSize);
	pObj->position(ArrowSize,2,ArrowSize);
	pObj->position(ArrowSize,2,-ArrowSize);
	pObj->position(-ArrowSize,2,-ArrowSize);
	pObj->end();




	pObj->begin(AxisMat,RenderOperation::OT_TRIANGLE_FAN);
	pObj->colour(ColourValue::Green);
	pObj->position(-ArrowSize,2,-ArrowSize);
	pObj->position(ArrowSize,2,-ArrowSize);
	pObj->position(ArrowSize,2,ArrowSize);
	pObj->position(-ArrowSize,2,ArrowSize);
	pObj->end();


	pObj->begin(AxisMat,RenderOperation::OT_LINE_LIST);
	pObj->colour(ColourValue::Green);
	pObj->position(-TWidth ,TextPos+THeight,0);
	pObj->position(0,TextPos,0);
	pObj->position(0,TextPos,0);
	pObj->position(TWidth,TextPos+THeight,0);	
	pObj->position(0,TextPos,0);
	pObj->position(0,TextPos-THeight,0);
	pObj->end();

}

void CSceneNodeCoordinate::BuildZCoord( Ogre::ManualObject* pObj )
{
	pObj->clear();
	pObj->begin(AxisMat,RenderOperation::OT_LINE_LIST);
	pObj->colour(ColourValue::Blue);
	pObj->position(0,0,0);
	pObj->position(0,0,2);
	pObj->end();

	pObj->begin(AxisMat,RenderOperation::OT_TRIANGLE_FAN);
	pObj->colour(ColourValue::Blue);
	pObj->position(0,0,3);
	pObj->position(-ArrowSize,-ArrowSize,2);
	pObj->position(ArrowSize,-ArrowSize,2);
	pObj->position(ArrowSize,ArrowSize,2);
	pObj->position(-ArrowSize,ArrowSize,2);
	pObj->position(-ArrowSize,-ArrowSize,2);
	pObj->end();




	pObj->begin(AxisMat,RenderOperation::OT_TRIANGLE_FAN);
	pObj->colour(ColourValue::Blue);
	pObj->position(-ArrowSize,-ArrowSize,2);
	pObj->position(-ArrowSize,ArrowSize,2);
	pObj->position(ArrowSize,ArrowSize,2);
	pObj->position(ArrowSize,-ArrowSize,2);
	pObj->end();


	pObj->begin(AxisMat,RenderOperation::OT_LINE_STRIP);
	pObj->colour(ColourValue::Blue);
	pObj->position(0,-TWidth,THeight+TextPos);
	pObj->position(0,TWidth,THeight+TextPos);
	pObj->position(0,-TWidth,-THeight+TextPos);
	pObj->position(0,TWidth,-THeight+TextPos);
	pObj->end();

}

void CSceneNodeCoordinate::BuildXYPlan( Ogre::ManualObject* pObj )
{
	pObj->clear();
	pObj->begin(PlanMat,RenderOperation::OT_TRIANGLE_FAN);
	pObj->colour(1,1,0,RenderApla);
	pObj->position(0,0,0);
	pObj->position(1,0,0);
	pObj->position(1,1,0);
	pObj->position(0,1,0);
	pObj->end();

	pObj->begin(PlanMat,RenderOperation::OT_LINE_STRIP);
	pObj->colour(1,1,0);
	pObj->position(1,0,0);
	pObj->position(1,1,0);
	pObj->position(0,1,0);
	pObj->end();


}

void CSceneNodeCoordinate::BuildYZPlan( Ogre::ManualObject* pObj )
{
	pObj->clear();
	pObj->begin(PlanMat,RenderOperation::OT_TRIANGLE_FAN);
	pObj->colour(0,1,1,RenderApla);
	pObj->position(0,0,0);
	pObj->position(0,1,0);
	pObj->position(0,1,1);
	pObj->position(0,0,1);
	pObj->end();

	pObj->begin(PlanMat,RenderOperation::OT_LINE_STRIP);
	pObj->colour(0,1,1);
	pObj->position(0,1,0);
	pObj->position(0,1,1);
	pObj->position(0,0,1);
	pObj->end();
}

void CSceneNodeCoordinate::BuildXZPlan( Ogre::ManualObject* pObj )
{
	pObj->clear();
	pObj->begin(PlanMat,RenderOperation::OT_TRIANGLE_FAN);
	pObj->colour(1,0,1,RenderApla);
	pObj->position(0,0,0);
	pObj->position(0,0,1);
	pObj->position(1,0,1);
	pObj->position(1,0,0);
	pObj->end();

	pObj->begin(PlanMat,RenderOperation::OT_LINE_STRIP);
	pObj->colour(1,0,1);
	pObj->position(0,0,1);
	pObj->position(1,0,1);
	pObj->position(1,0,0);
	pObj->end();
}
#define COS(x) Math::Cos(x)
#define SIN(x) Math::Sin(x)

void CSceneNodeCoordinate::BuildXRotate( Ogre::ManualObject* pObj )
{
	pObj->clear();
	pObj->begin(RotatMat,RenderOperation::OT_LINE_STRIP);
	pObj->colour(1,0,0);

	pObj->position(0,CircleRad,0);
	for(int i=1;i<CirclePtCnt;i++)
	{
		Degree deg(360.0*i/CirclePtCnt);
		pObj->position(0,CircleRad*COS(deg),CircleRad*SIN(deg) );
	}
	pObj->position(0,CircleRad,0);
	pObj->end();
}

void CSceneNodeCoordinate::BuildYRotate( Ogre::ManualObject* pObj )
{
	pObj->clear();
	pObj->begin(RotatMat,RenderOperation::OT_LINE_STRIP);
	pObj->colour(0,1,0);

	pObj->position(0,0,CircleRad);
	for(int i=1;i<CirclePtCnt;i++)
	{
		Degree deg(360.0*i/CirclePtCnt);		
		pObj->position( CircleRad*SIN(deg), 0, CircleRad*COS(deg) );
	}
	pObj->position(0,0,CircleRad);
	pObj->end();
}

void CSceneNodeCoordinate::BuildZRotate( Ogre::ManualObject* pObj )
{
	pObj->clear();
	pObj->begin(RotatMat,RenderOperation::OT_TRIANGLE_LIST);
	pObj->colour(0,0,1,RenderApla);

	pObj->position(CircleRadIn,0,0);
	pObj->position(CircleRad,0,0);
	for(int i=1;i<CirclePtCnt;i++)
	{
		Degree deg(360.0*i/CirclePtCnt);			
		pObj->position( CircleRadIn*COS(deg),CircleRadIn*SIN(deg),0 );
		pObj->position( CircleRad*COS(deg),CircleRad*SIN(deg),0 );
	}
	pObj->position(CircleRadIn,0,0);
	pObj->position(CircleRad,0,0);
	for(int i=0;i<CirclePtCnt;i++)
	{
		pObj->quad(i*2,i*2+1,i*2+3,i*2+2);
	}
	pObj->end();
}
CSceneNodeCoordinate::CSceneNodeCoordinate( Ogre::SceneNode* pNode ) : C3DNodeObject(pNode)
{
	InitOnce();
	mpDragObj = NULL;
	mpManipulateNode = NULL;
	mpLastMouseOnObj = NULL;
	mHintTextName = pNode->getName() + _T("HintText");
}

void CSceneNodeCoordinate::AddListener( EventListener* pListen )
{
	mvListers.push_back( ListenerPtr(pListen) );
}

void CSceneNodeCoordinate::OnDrag(const Ogre::Ray& mouseRayFrom, const Ogre::Ray& mouseRayTo)
{
	//drag options move rotate or scale	
	int elm = any_cast<int>(mpDragObj->getUserAny());
	
	Vector3 posForm,posTo;
	getRayIntersectPos(mouseRayFrom,(emCoordElm)elm,posForm);
	getRayIntersectPos(mouseRayTo,(emCoordElm)elm,posTo);	
	switch (elm)
	{
	case CE_XAxis:
	case CE_YAxis:
	case CE_ZAxis:
	case CE_XYPlan:
	case CE_YZPlan:
	case CE_XZPlan:
		{
			GetManipulateNode()->translate(posTo-posForm);
			mpNode->setPosition(GetManipulateNode()->_getDerivedPosition());
			break;
		}
	case CE_XRotate:
	case CE_YRotate:
	case CE_ZRotate:
		{	
			Vector3 vPos = GetManipulateNode()->_getDerivedPosition();
			Vector3 vF = (posForm - vPos).normalisedCopy();
			Vector3 vT = (posTo - vPos).normalisedCopy();
			GetManipulateNode()->rotate(vF.getRotationTo(vT),Node::TS_PARENT);
			break;
		}
	}
	//
	for(int i=0;i<(int)mvListers.size();i++)
	{
		mvListers[i]->OnTransform(GetManipulateNode());
	}
}

void CSceneNodeCoordinate::EndDrag(const Ogre::Ray& mouseRay)
{
	mpDragObj = NULL;
	for(int i=0;i<(int)mvListers.size();i++)
	{
		mvListers[i]->EndTransform(GetManipulateNode());
	}
}


bool CSceneNodeCoordinate::getRayIntersectPos(const Ogre::Ray& ray, emCoordElm em,Ogre::Vector3& outPos )
{
	Vector3 NodePos = mpNode->_getDerivedPosition();	
	Vector3 vXDir =  mpNode->_getDerivedOrientation() * Vector3::UNIT_X;
	Vector3 vYDir = mpNode->_getDerivedOrientation() * Vector3::UNIT_Y;
	Vector3 vZDir = mpNode->_getDerivedOrientation() * Vector3::UNIT_Z;
	
	Plane xyPlane(vZDir,NodePos);
	Plane yzPlane(vXDir,NodePos);
	Plane xzPlane(vYDir,NodePos);
	Ray xRay(NodePos, vXDir);
	Ray yRay(NodePos, vYDir);
	Ray zRay(NodePos, vZDir);
	
	std::pair<bool,Real> ret;

	if(em==CE_XAxis){
		double AbsCos1 = Math::Abs(ray.getDirection().dotProduct(xzPlane.normal) );
		double AbsCos2 = Math::Abs(ray.getDirection().dotProduct(xyPlane.normal) );
		
		const Vector3& dir = vXDir;
		const Ray& vRay = xRay;	

		ret = ray.intersects(AbsCos1>AbsCos2?xzPlane:xyPlane);	
		Vector3 pos = ray.getPoint(ret.second);

		Plane posPlan = Plane(dir,pos);
		ret = vRay.intersects(posPlan);
		outPos = vRay.getPoint(ret.second);
	}
	if(em == CE_YAxis)
	{
		double AbsCos1 = Math::Abs(ray.getDirection().dotProduct(yzPlane.normal) );
		double AbsCos2 = Math::Abs(ray.getDirection().dotProduct(xyPlane.normal) );
		 		
		const Vector3& dir = vYDir;
		const Ray& vRay = yRay;		

		ret = ray.intersects(AbsCos1>AbsCos2?yzPlane:xyPlane);
		Vector3 pos = ray.getPoint(ret.second);
		Plane posPlan = Plane(dir,pos);
		ret = vRay.intersects(posPlan);
		outPos = vRay.getPoint(ret.second);
	}
	if(em == CE_ZAxis)
	{
		double AbsCos1 = Math::Abs(ray.getDirection().dotProduct(yzPlane.normal) );
		double AbsCos2 = Math::Abs(ray.getDirection().dotProduct(xzPlane.normal) );

		const Vector3& dir = vZDir;
		const Ray& vRay = zRay;		

		ret = ray.intersects(AbsCos1>AbsCos2?yzPlane:xzPlane);
		Vector3 pos = ray.getPoint(ret.second);

		Plane posPlan = Plane(dir,pos);
		ret = vRay.intersects(posPlan);
		outPos = vRay.getPoint(ret.second);
	}
	
	if(em == CE_XYPlan || em == CE_ZRotate )
	{		
		ret = ray.intersects(xyPlane);
		outPos = ray.getPoint(ret.second);
	}
	if(em == CE_YZPlan || em == CE_XRotate )
	{
		ret =  ray.intersects(yzPlane);
		outPos = ray.getPoint(ret.second);
	}
	if(em == CE_XZPlan || em == CE_YRotate )
	{
		ret = ray.intersects(xzPlane);
		outPos = ray.getPoint(ret.second);
	}

	return false;	
}

void CSceneNodeCoordinate::StartDrag( const Ogre::Ray& startRay, Ogre::MovableObject* pObj )
{
	mStartRay = startRay;
	mpDragObj = pObj;

	int elm = any_cast<int>(mpDragObj->getUserAny());		
	getRayIntersectPos(startRay,(emCoordElm)elm, mStartDragPos);
	mLastDragPos = mStartDragPos;

	for(int i=0;i<(int)mvListers.size();i++)
	{
		mvListers[i]->BeginTransform(GetManipulateNode());
	}
}


void CSceneNodeCoordinate::SetManipulateNode( Ogre::SceneNode* pNode )
{
	if(mpManipulateNode && mpManipulateNode!=pNode)
	{
		//C3DNodeObject(mpManipulateNode).AddVisibleFlag(SelectableFlag,true);
	}
	mpManipulateNode = pNode;
	if(mpManipulateNode)
	{
		mpNode->setPosition(pNode->_getDerivedPosition());	
		//C3DNodeObject(mpManipulateNode).RemoveVisibleFlag(SelectableFlag,true);
		OgreUtil::AttachNodeTo(mpNode,GetScene()->getRootSceneNode());
	}
	else
	{		
		OgreUtil::RemoveSceneNode(mpNode);
	}

}

static void RestoreObjMat(Ogre::MovableObject* pObj)
{
	emCoordElm em = (emCoordElm)any_cast<int>(pObj->getUserAny());
	if(em == CE_ZAxis || em==CE_XAxis || em ==CE_YAxis)
	{
		OgreUtil::SetObjectMaterial((ManualObject*)pObj,AxisMat);
	}
	if(em ==CE_XYPlan || em == CE_YZPlan || em == CE_XYPlan)
	{
		OgreUtil::SetObjectMaterial((ManualObject*)pObj,PlanMat);
	}
	OgreUtil::SetObjectMaterial((ManualObject*)pObj,RotatMat);

}


void CSceneNodeCoordinate::OnMousemove( const Ogre::Ray& mouseRay,Ogre::MovableObject* pObj )
{
	if(mpLastMouseOnObj && mpLastMouseOnObj!=pObj) //restore the mat
	{
		RestoreObjMat(mpLastMouseOnObj);
		mpLastMouseOnObj = NULL;
	}
	
	if(HasMovableObj(pObj,true))
	{
		OgreUtil::SetObjectMaterial((ManualObject*)pObj,MouseOnMat);
		mpLastMouseOnObj = pObj;
	}

	
	
}
//
//void CSceneNodeCoordinate::UpdateHintText()
//{
//	ManualText* pText = OgreUtil::createOrRetrieveManualText(mHintTextName.c_str(),GetScene());
//	pText->clear();
//	pText->begin(_T("Arial"),ManualText::TT_Texture,"Font");
//	pText->end();
//	OgreUtil::AttachMovableObject(pText,mpNode);
//}