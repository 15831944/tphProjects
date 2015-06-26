#include "StdAfx.h"
#include ".\3dnodeobject.h"
#include "OgreConvert.h"
#include <common\ZipInfo.h>
#include <common\FileInDB.h>
#include "SelectionManager.h"
#include "camera.h"
#include "./ARCRenderTargetListener.h"
#include "RotationSeries.h"
#include ".\SceneNodeToMesh.h"
#include ".\modelsnapshot.h"
#include <common/ARCVector4.h>
#include <common/ARCStringConvert.h>
using namespace Ogre;

bool C3DNodeObject::HasMovableObj( Ogre::MovableObject* pObj,bool cascade /*=true*/ ) const
{	
	if(!mpNode)return false;

	for(int i=0;i<mpNode->numAttachedObjects();i++)
	{
		if(mpNode->getAttachedObject(i) == pObj)
			return true;
	}

	if(cascade)
	{		
		Node::ChildNodeIterator itr = mpNode->getChildIterator();
		while(itr.hasMoreElements())
		{
			Node* pChildNode = itr.getNext();			
			if (GetScene()->hasSceneNode(pChildNode->getName()))
			{
				SceneNode* pchildSceneNode = GetScene()->getSceneNode(pChildNode->getName());
				if(pchildSceneNode == pChildNode)
				{
					bool b = C3DNodeObject(pchildSceneNode).HasMovableObj(pObj,cascade);
					if(b)
						return true;
				}
			}
		}
	}
	return false;

}
void C3DNodeObject::SetScale( const ARCVector3& scale )
{
	ASSERT(mpNode);
	if(mpNode)
		mpNode->setScale( OGREVECTOR(scale) );
}


void GetRotateQuat( const ARCVector3& eularAngle, EularRotType rotType, Ogre::Quaternion& quat)
{
	Ogre::Matrix3 rotateMat;
	switch(rotType)
	{
	case ER_XYZ:
		rotateMat.FromEulerAnglesXYZ(Degree(eularAngle[VX]),Degree(eularAngle[VY]),Degree(eularAngle[VZ]));
		break;
	case ER_XZY:
		rotateMat.FromEulerAnglesXZY(Degree(eularAngle[VX]),Degree(eularAngle[VZ]),Degree(eularAngle[VY]));
		break;
	case ER_YXZ:
		rotateMat.FromEulerAnglesYXZ(Degree(eularAngle[VY]),Degree(eularAngle[VX]),Degree(eularAngle[VZ]));
		break;
	case ER_YZX:
		rotateMat.FromEulerAnglesYZX(Degree(eularAngle[VY]),Degree(eularAngle[VZ]),Degree(eularAngle[VX]));
		break;
	case ER_ZXY:
		rotateMat.FromEulerAnglesZXY(Degree(eularAngle[VZ]),Degree(eularAngle[VZ]),Degree(eularAngle[VY]));
		break;
	case ER_ZYX:
		rotateMat.FromEulerAnglesZYX(Degree(eularAngle[VZ]),Degree(eularAngle[VY]),Degree(eularAngle[VX]));
		break;
	}		
	quat.FromRotationMatrix(rotateMat);
}

void C3DNodeObject::SetRotation( const ARCVector3& eularAngle, EularRotType rotType)
{
	ASSERT(mpNode);
	if( mpNode )
	{
		Quaternion quat;
		GetRotateQuat(eularAngle,rotType,quat);	
		mpNode->setOrientation(quat);
	}
}

void C3DNodeObject::SetRotation( const ARCVector4& quat )
{
	Quaternion q(quat.w,quat.x,quat.y,quat.z);
	if(mpNode)
		mpNode->setOrientation(q);
}

//void C3DNodeObject::DoRotation( const RotationSeries& rots ) const
//{
//	if (mpNode)
//	{
//		rots.ApplyRotation(mpNode);
//	}
//}


void C3DNodeObject::SetPosition( const ARCVector3& pos )
{
	ASSERT(mpNode);
	if(mpNode)
		mpNode->setPosition( OGREVECTOR(pos) );
}

//void C3DNodeObject::Translate(Ogre::Real x,Ogre::Real y,Ogre::Real z, Ogre::Node::TransformSpace relativeTo /* = Ogre::Node::TS_PARENT */)
//{
//  ASSERT(mpNode);
//  if (mpNode)
//	mpNode->translate(x, y, z, relativeTo);
//}
//
void C3DNodeObject::Translate(ARCVector3& d)
{
	ASSERT(mpNode);
	if (mpNode)
		mpNode->translate( OGREVECTOR(d) );
}

Ogre::SceneManager* C3DNodeObject::GetScene()const
{
	ASSERT(mpNode);
	if(mpNode)
		return mpNode->getCreator();
	return NULL;
}

void C3DNodeObject::RemoveVisibleFlag( uint32 visibleFlag,bool cascade )
{
	SceneNode::ObjectIterator itr =	mpNode->getAttachedObjectIterator();
	while (itr.hasMoreElements())
	{
		MovableObject* pobj = itr.getNext();
		pobj->removeVisibilityFlags(visibleFlag);
	}
	if(cascade)
	{		
		Node::ChildNodeIterator itr = mpNode->getChildIterator();
		while(itr.hasMoreElements())
		{
			Node* pChildNode = itr.getNext();			
			if (GetScene()->hasSceneNode(pChildNode->getName()))
			{
				SceneNode* pchildSceneNode = GetScene()->getSceneNode(pChildNode->getName());
				if(pchildSceneNode == pChildNode){
					C3DNodeObject(pchildSceneNode).RemoveVisibleFlag(visibleFlag,cascade);	
				}
			}
		}
	}
}

void C3DNodeObject::AddVisibleFlag( uint32 visibleFlag,bool cascade )
{
	SceneNode::ObjectIterator itr =	mpNode->getAttachedObjectIterator();
	while (itr.hasMoreElements())
	{
		MovableObject* pobj = itr.getNext();
		pobj->addVisibilityFlags(visibleFlag);
	}
	if(cascade)
	{
		Node::ChildNodeIterator itr = mpNode->getChildIterator();
		while(itr.hasMoreElements())
		{
			Node* pChildNode = itr.getNext();			
			if (GetScene()->hasSceneNode(pChildNode->getName()))
			{
				SceneNode* pchildSceneNode = GetScene()->getSceneNode(pChildNode->getName());
				if(pchildSceneNode == pChildNode){
					C3DNodeObject(pchildSceneNode).AddVisibleFlag(visibleFlag,cascade);
				}
			}
		}
	}
}

void C3DNodeObject::Clear()
{
	if(mpNode)
	{
		mpNode->detachAllObjects();
		mpNode->removeAllChildren();
	}
}

CString C3DNodeObject::GetName() const
{
	if(mpNode)
	{
		Any any= mpNode->getUserAny();
		try
		{
			CString str = any_cast<CString>(any);
			return str;
		}		
		catch (...)
		{
		}
		
	}
	return GetIDName();
}

void C3DNodeObject::SetName( CString sName )
{
	if(mpNode)
	{
		mpNode->setUserAny(Any(sName));
	}
}

C3DNodeObject::NodeList C3DNodeObject::GetChild() const
{
	NodeList ret;
	if(mpNode)
	{
	
		SceneNode::ChildNodeIterator itr = mpNode->getChildIterator();
		while(itr.hasMoreElements())
		{
			Node* pNode = itr.getNext();
			if(mpNode->getCreator()->hasSceneNode(pNode->getName()))
			{
				ret.push_back((SceneNode*)pNode);
			}		
			
		}
	}	
	return ret;
}

C3DNodeObject C3DNodeObject::GetParent() const
{
	return mpNode->getParentSceneNode();
}

ARCVector3 C3DNodeObject::GetScale() const
{
	if(mpNode)
		return ARCVECTOR(mpNode->getScale());
	return ARCVector3(1,1,1);
}

ARCVector3 C3DNodeObject::GetRotation() const
{
	if(mpNode)
	{
		Matrix3 rMat;
		mpNode->getOrientation().ToRotationMatrix(rMat);
		Radian x,y,z;
		rMat.ToEulerAnglesXYZ(x,y,z);
		return ARCVector3(x.valueDegrees(),y.valueDegrees(),z.valueDegrees());
	}
	return ARCVector3(0,0,0);
}

ARCVector3 C3DNodeObject::GetPosition() const
{
	if(mpNode)
	{
		return ARCVECTOR(mpNode->getPosition());
	}
	return ARCVector3(0,0,0);
}

void C3DNodeObject::SetVisibleFlag( Ogre::uint32 visibleFlag,bool cascade )
{
	SceneNode::ObjectIterator itr =	mpNode->getAttachedObjectIterator();
	while (itr.hasMoreElements())
	{
		MovableObject* pobj = itr.getNext();
		pobj->setVisibilityFlags(visibleFlag);
	}
	if(cascade)
	{
		Node::ChildNodeIterator itr = mpNode->getChildIterator();
		while(itr.hasMoreElements())
		{
			Node* pChildNode = itr.getNext();			
			if (GetScene()->hasSceneNode(pChildNode->getName()))
			{
				SceneNode* pchildSceneNode = GetScene()->getSceneNode(pChildNode->getName());
				if(pchildSceneNode == pChildNode){
					C3DNodeObject(pchildSceneNode).SetVisibleFlag(visibleFlag,cascade);
				}
			}
		}
	}
}
void C3DNodeObject::AttachTo( C3DNodeObject& otherNode )
{
	if(mpNode && otherNode.mpNode)
		OgreUtil::AttachNodeTo(mpNode,otherNode.mpNode);
}

void C3DNodeObject::Detach()
{
	if (mpNode)
	{
		OgreUtil::RemoveSceneNode(mpNode);
	}
}

//--------------------------START----------------------------------
// For GUI only
void C3DNodeObject::SetVisible( bool bVisible )
{
	if(mpNode){
		mpNode->setVisible(bVisible,true);
	}

}
bool C3DNodeObject::GetVisible() const
{
	if(mpNode)
	{
		SceneNode::ObjectIterator itr =	mpNode->getAttachedObjectIterator();
		while (itr.hasMoreElements())
		{
			MovableObject* pobj = itr.getNext();
			if(pobj->getVisible())
				return true;
		}
		//if(cascade)
		{
			Node::ChildNodeIterator itr = mpNode->getChildIterator();
			while(itr.hasMoreElements())
			{
				Node* pChildNode = itr.getNext();			
				if (GetScene()->hasSceneNode(pChildNode->getName()))
				{
					SceneNode* pchildSceneNode = GetScene()->getSceneNode(pChildNode->getName());
					if(pchildSceneNode == pChildNode){
						bool b = C3DNodeObject(pchildSceneNode).GetVisible();
						if(b)
							return b;
					}
				}
			}
		}
	}
	return false;
}

bool C3DNodeObject::IsGroup() const
{
	if (mpNode)
	{
		return !mpNode->getAttachedObjectIterator().hasMoreElements();
	}
	return false;
}

C3DNodeObject C3DNodeObject::CreateNewChild()
{
	if(mpNode)
	{
		return mpNode->createChildSceneNode();
	}
	return C3DNodeObject(NULL);
}
//--------------------------END----------------------------------

//
//void C3DNodeObject::SaveData( const CString& zipFilePath )
//{
//	//export all node child node to file 
//	//create temp folder first
//	CString strFolder;
//	::GetTempPath(256,strFolder.GetBuffer(256));
//	//save mesh to a file
//	StringList fileList;
//	ExportToTempFolder(strFolder,fileList);
//
//	CZipInfo zipfile;
//	zipfile.AddFiles(zipFilePath.GetString(), fileList);
//}
//
//void C3DNodeObject::ExportToTempFolder( const CString& strFolder, StringList& fileList )
//{
//	TiXmlDocument xmlDoc;
//	CString strPathName = strFolder+STR_MODEL_FILE;
//	xmlDoc.SaveFile(strPathName);
//	fileList.push_back(strPathName.GetString());
//
//}


void C3DNodeObject::AddObject( Ogre::MovableObject* pObj,int bSelectable , bool bVisble/*= false*/ )
{
	if(mpNode && pObj)
	{
		if(bVisble)
			pObj->addVisibilityFlags(NormalVisibleFlag);
		else 
			pObj->removeVisibilityFlags(NormalVisibleFlag);

		OgreUtil::AttachMovableObject(pObj,mpNode);
		if(bSelectable)
			REG_SELECT(pObj,bSelectable);
	}
}

CString C3DNodeObject::GetIDName() const
{
	if(mpNode)
	{
		return mpNode->getName().c_str();
	}
	return _T("");
}

void C3DNodeObject::EnableSelect( int bOrder )
{
	SceneNode::ObjectIterator itr =	mpNode->getAttachedObjectIterator();
	while (itr.hasMoreElements())
	{
		MovableObject* pobj = itr.getNext();
		if(bOrder)
			REG_SELECT(pobj,bOrder);
		else 
			UNREG_SELECT(pobj);		
	}
	//cascade
	{
		Node::ChildNodeIterator itr = mpNode->getChildIterator();
		while(itr.hasMoreElements())
		{
			Node* pChildNode = itr.getNext();			
			if (GetScene()->hasSceneNode(pChildNode->getName()))
			{
				SceneNode* pchildSceneNode = GetScene()->getSceneNode(pChildNode->getName());
				if(pchildSceneNode == pChildNode)
				{
					C3DNodeObject(pchildSceneNode).EnableSelect(bOrder);
				}
			}
		}
	}	
}

void C3DNodeObject::SetIdentTransform() const
{
	if(mpNode){
		mpNode->setPosition(0,0,0);
		mpNode->setScale(1,1,1);
		mpNode->setOrientation(Quaternion::IDENTITY);
	}
}



void C3DNodeObject::SetMaterial( const CString& str, bool bcascad /*= true*/ )
{
	SceneNode::ObjectIterator itr =	mpNode->getAttachedObjectIterator();
	while (itr.hasMoreElements())
	{
		MovableObject* pobj = itr.getNext();
		OgreUtil::SetMaterial(pobj,str);
	}
	//cascade
	if(bcascad){
		Node::ChildNodeIterator itr = mpNode->getChildIterator();
		while(itr.hasMoreElements())
		{
			Node* pChildNode = itr.getNext();			
			if (GetScene()->hasSceneNode(pChildNode->getName()))
			{
				SceneNode* pchildSceneNode = GetScene()->getSceneNode(pChildNode->getName());
				if(pchildSceneNode == pChildNode)
				{
					C3DNodeObject(pchildSceneNode).SetMaterial(str,bcascad);
				}
			}
		}
	}	
}
void C3DNodeObject::SetShowBound( bool b )
{
	if(mpNode){
		mpNode->showBoundingBox(b);
	}
}

bool C3DNodeObject::GetShowBound() const
{
	if(mpNode){
		return mpNode->getShowBoundingBox();
	}
	return false;
}


bool C3DNodeObject::ExportToMesh( CString meshPrefix, CString folder )
{
	SceneNodeToMesh convertor;
	convertor.Init(meshPrefix);
	convertor.VisitNode(mpNode);
	convertor.Save(folder);
	return true;
}


static void DestoryNodeObjects( SceneNode* pNode )
{
	if(!pNode)
		return;
	SceneManager* pScene = pNode->getCreator();
	if(!pScene)
		return;

	SceneNode::ObjectIterator itr =	pNode->getAttachedObjectIterator();
	while (itr.hasMoreElements())
	{
		MovableObject* pobj = itr.getNext();
		pScene->destroyMovableObject(pobj);
	}	
	pNode->detachAllObjects();
	pScene->destroySceneNode(pNode);
}

void C3DNodeObject::Destory()
{
	if(mpNode)
	{
		Node::ChildNodeIterator itr = mpNode->getChildIterator();
		while(itr.hasMoreElements())
		{
			Node* pChildNode = itr.getNext();			
			if (GetScene()->hasSceneNode(pChildNode->getName()))
			{
				SceneNode* pchildSceneNode = GetScene()->getSceneNode(pChildNode->getName());
				if(pchildSceneNode == pChildNode){
					DestoryNodeObjects(pchildSceneNode);
				}
			}
		}
		DestoryNodeObjects(mpNode);
		mpNode = NULL; //###
	}
}

//

AutoDeleteNodeObject::~AutoDeleteNodeObject()
{
	Destory();
	mpNode =NULL;
}



//update snapshot to model snapshot buffer
void C3DNodeObject::SnapShot(CString tmpFolder)
{
	CModelSnapShot snapshot;
	snapshot.DoNode(*this, tmpFolder);
}


void C3DNodeObject::RemoveChild()
{
	if(mpNode)
		mpNode->removeAllChildren();
}

void C3DNodeObject::SetAny( const Ogre::Any& any )
{
	if(mpNode)
		mpNode->setUserAny(any);
}



const Ogre::Any& C3DNodeObject::GetAny() const
{
	if(mpNode)
		return mpNode->getUserAny();
	const static Any sAny;
	return sAny;
}

ARCVector3 C3DNodeObject::ConvertPositionLocalToWorld( const ARCVector3& pos )
{
	if(mpNode)
		return ARCVECTOR(mpNode->convertLocalToWorldPosition(OGREVECTOR(pos)));
	return ARCVector3(0.0, 0.0, 0.0);
}

ARCVector3 C3DNodeObject::ConvertPositionWorldToLocal( const ARCVector3& pos )
{
	if(mpNode)
		return ARCVECTOR(mpNode->convertWorldToLocalPosition(OGREVECTOR(pos)));
	return ARCVector3(0.0, 0.0, 0.0);
}

C3DNodeObject C3DNodeObject::CreateOrRetrieveChild( int idx )
{
	if(mpNode)
	{		
		SceneNode* pNode =  OgreUtil::createOrRetrieveSceneNode(GetIDName()+ARCStringConvert::toString(idx), GetScene());
		OgreUtil::AttachNodeTo(pNode,mpNode);
		return pNode;
	}
	return C3DNodeObject(NULL);
}

ARCVector3 C3DNodeObject::GetBounds() const
{
	if(mpNode)
	{
		const AxisAlignedBox& bb =  mpNode->_getWorldAABB();
		return ARCVECTOR(bb.getMaximum()-bb.getMinimum());
	}
	return ARCVector3(0,0,0);
}

void C3DNodeObject::RemoveAllObject()
{
	if(mpNode)
		mpNode->detachAllObjects();
}

void C3DNodeObject::SetDirection( const ARCVector3& vHeadDir, const ARCVector3& vUpDir )
{	
	if(mpNode)
	{
		static Vector3 vOrignHead = Vector3::UNIT_X;
		static Vector3 vOrignUp = Vector3::UNIT_Z;

		Vector3 vHead = OGREVECTOR(vHeadDir).normalisedCopy();
		Vector3 vUp = OGREVECTOR(vUpDir).normalisedCopy();		
		Vector3 vRealUp = vHead.crossProduct(vUp.crossProduct(vHead));

		if(vRealUp == Vector3::ZERO)
			vRealUp = vHead.perpendicular();

		Vector3 vLeft = vRealUp.crossProduct(vHead);

		Quaternion q;
		q.FromAxes(vHead, vLeft,vRealUp);
		mpNode->setOrientation(q);
	}
}

void C3DNodeObject::Rotate( const ARCVector3& eularAngle, EularRotType rotType /*= ER_XYZ*/ )
{
	Quaternion quat;
	GetRotateQuat(eularAngle,rotType,quat);
	mpNode->rotate(quat,Node::TS_PARENT);

}
void C3DNodeObject::RotateLocal( const ARCVector3& eularAngle, EularRotType rotType /*= ER_XYZ*/ )
{
	Quaternion quat;
	GetRotateQuat(eularAngle,rotType,quat);
	mpNode->rotate(quat,Node::TS_LOCAL);

}

C3DNodeObject C3DNodeObject::GetRoot() const
{
	SceneNode* pNode = NULL;
	if(SceneManager *pScene = GetScene())
	{
		pNode=pScene->getRootSceneNode();
	}
	return pNode;
}

bool C3DNodeObject::AttachToRoot()
{
	if(C3DNodeObject node = GetRoot())
	{
		AttachTo(node);
		return true;
	}
	return false;
}

C3DNodeObject C3DNodeObject::CreateChild( const CString& sName )
{
	if(mpNode)
	{
		return mpNode->createChildSceneNode(sName.GetString());
	}
	return C3DNodeObject(NULL);
}


