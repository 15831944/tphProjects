#include "stdafx.h"
#include "3DMovableObject.h"
#include "3DSceneNode.h"
#include "SelectionManager.h"

using namespace Ogre;



CString C3DSceneNode::GetName() const
{
	if(m_pNode)
		return m_pNode->getName().c_str();
	return "";
}

void C3DSceneNode::DetachAllObjects()
{
	if(m_pNode){
		m_pNode->detachAllObjects();
	}
}

void C3DSceneNode::RemoveAllChildNodes()
{
	if(m_pNode){
		m_pNode->removeAllChildren();
	}
}

C3DRootSceneNode C3DSceneNode::GetRoot() const
{
	if(m_pNode){
		if(SceneManager* ps = m_pNode->getCreator()){
			return ps->getRootSceneNode();
		}
	}
	return NULL;
}

void C3DSceneNode::AddChild( C3DSceneNode& node )
{
	if(m_pNode && node.m_pNode){
		OgreUtil::AttachNodeTo(node.m_pNode,m_pNode);
	}
}

void C3DSceneNode::AddObject( Ogre::MovableObject* pObj,int bSelectable /*= 0*/,bool bVisble /*= true*/ )
{
	if(m_pNode && pObj)
	{
		if(bVisble)
			pObj->addVisibilityFlags(NormalVisibleFlag);
		else 
			pObj->removeVisibilityFlags(NormalVisibleFlag);

		OgreUtil::AttachMovableObject(pObj,m_pNode);
		if(bSelectable)
			REG_SELECT(pObj,bSelectable);
	}
}

const static CString sKeyImpl = _T("Impl");
ISceneNodeImpl* C3DSceneNode::GetImpl() 
{
	if(m_pNode)
	{	
		const Any& any = GetUserAny(sKeyImpl);//.getUserAny(sImpl);
		ISceneNodeImpl* const * ret = any_cast<ISceneNodeImpl*>(&any);
		if(ret){ return *ret; }		
	}
	return NULL;
}


void C3DSceneNode::SetPosition( double x, double y, double z )
{
	if(m_pNode){
		m_pNode->setPosition(x,y,z);
	}
}

void C3DSceneNode::SetPosition( const ARCVector3& v )
{
	if(m_pNode){
		m_pNode->setPosition(v.x,v.y,v.z);
	}
}

Ogre::SceneManager* C3DSceneNode::getScene() const
{
	if(m_pNode){
		return m_pNode->getCreator();
	}
	return NULL;
}

C3DSceneNode C3DSceneNode::GetCreateChildNode( const CString& nodeName )
{
	C3DSceneNode node = OgreUtil::createOrRetrieveSceneNode(nodeName,getScene());
	AddChild(node);
	return node;
}

void C3DSceneNode::RemoveObject( Ogre::MovableObject* pObj )
{
	if(m_pNode)
		m_pNode->detachObject(pObj);
}



const Ogre::Any& C3DSceneNode::GetUserAny( const CString& s )const
{
	return m_pNode->getUserObjectBindings().getUserAny(s.GetString());
}

void C3DSceneNode::SetUserAny( const Ogre::Any&a, const CString&s )
{
	ASSERT(m_pNode);
	if(m_pNode)
		m_pNode->getUserObjectBindings().setUserAny(s.GetString(),a);
}

Ogre::ManualObject* C3DSceneNode::_GetOrCreateManualObject( const CString& name )
{
	return OgreUtil::createOrRetrieveManualObject(name,getScene());
}

void C3DSceneNode::SetVisible( bool b )
{
	if(m_pNode){
		m_pNode->setVisible(b);
	}
}

void C3DSceneNode::RemoveFromParent()
{
	OgreUtil::RemoveSceneNode(m_pNode);
}

void C3DSceneNode::RemoveChild( const CString& id )
{
	if( C3DSceneNode node = GetSceneNode(id) )
	{
		node.RemoveFromParent();
	}
}

void C3DSceneNode::_GetRotateQuat( const ARCVector3& eularAngle, EularRotType rotType, Ogre::Quaternion& quat )
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
		rotateMat.FromEulerAnglesZXY(Degree(eularAngle[VZ]),Degree(eularAngle[VX]),Degree(eularAngle[VY]));
		break;
	case ER_ZYX:
		rotateMat.FromEulerAnglesZYX(Degree(eularAngle[VZ]),Degree(eularAngle[VY]),Degree(eularAngle[VX]));
		break;
	}		
	quat.FromRotationMatrix(rotateMat);
}

void C3DSceneNode::SetRotation( const ARCVector3& eularAngle, EularRotType rotType )
{
	ASSERT(m_pNode);
	if( m_pNode )
	{
		Quaternion quat;
		_GetRotateQuat(eularAngle,rotType,quat);	
		m_pNode->setOrientation(quat);
	}
}

void C3DSceneNode::SetScale( const ARCVector3& scale )
{
	ASSERT(m_pNode);
	if(m_pNode)
		m_pNode->setScale( OGREVECTOR(scale) );
}

const static CString sKeyModel = "Model";
const static CString sKeyTagMan = "TagManager";

IARCportLayoutEditContext* C3DRootSceneNode::GetModel()const
{
	if(*this)
	{
		const Any& any = GetUserAny(sKeyModel);
		IARCportLayoutEditContext*const * ret =  any_cast<IARCportLayoutEditContext*>(&any);
		if(ret){ return *ret; } 
	}
	return NULL;
}	

void C3DRootSceneNode::SetModel( IARCportLayoutEditContext* pModel )
{
	if(*this){	
		SetUserAny(Any(pModel),sKeyModel);
	}	
}

void C3DRootSceneNode::SetTagManager( CRender3DSceneTags* pTag )
{
	if(*this){	
		SetUserAny(Any(pTag),sKeyTagMan);
	}	
}


void C3DSceneNode::SetImpl( ISceneNodeImpl* p)
{
	if(C3DRootSceneNode root = GetRoot() )
	{
		ISceneNodeImplPool* pPool = root.GetImplPool();
		ASSERT(pPool);
		if(!pPool)return;

		if(*this){
			//remove old
			ISceneNodeImpl* pOld = GetImpl();
			if(pOld){  pPool->Remove(pOld); }		
			//set new
			SetUserAny(Any(p),sKeyImpl);
		}
	}
	
}


const static CString sKeyImplPool = _T("ImplPool"); 
void C3DRootSceneNode::SetImplPool( ISceneNodeImplPool* pPool )
{
	if(*this)
	{
		SetUserAny(Any(pPool),sKeyImplPool);
	}
}

ISceneNodeImplPool* C3DRootSceneNode::GetImplPool()
{
	if(*this)
	{
		const Any& any = GetUserAny(sKeyImplPool);
		ISceneNodeImplPool*const * ret =  any_cast<ISceneNodeImplPool*>(&any);
		if(ret){ return *ret; } 
	}
	return NULL;
}

CRender3DSceneTags* C3DRootSceneNode::GetTagManager()
{
	if(*this)
	{
		const Any& any = GetUserAny(sKeyTagMan);
		CRender3DSceneTags*const * ret =  any_cast<CRender3DSceneTags*>(&any);
		if(ret){ return *ret; } 
	}
	return NULL;
}


C3DSceneNode C3DSceneNode::GetParent() const
{
	if(m_pNode)
		return m_pNode->getParentSceneNode();
	return NULL;
}

Ogre::Entity* C3DSceneNode::_GetOrCreateEntity( const CString& name, const CString& meshName )
{
	return OgreUtil::createOrRetrieveEntity(name,meshName, getScene());
}

ARCVector3 C3DSceneNode::GetPosition() const
{
	if(m_pNode){
		return ARCVECTOR(m_pNode->getPosition());
	}
	return ARCVector3::ZERO;
}


bool C3DSceneNode::IsRoot() const
{
	if(C3DSceneNode rootNode = GetRoot() ){
		return rootNode == *this; 
	}
	return false;
}

C3DSceneNode C3DSceneNode::GetSceneNode( const CString& _nodeName, bool bCreate /*= false*/ )
{
	String nodeName = _nodeName.GetString();
	if(SceneManager* pScen = getScene() )
	{
		if( pScen->hasSceneNode(nodeName) )
		{
			return pScen->getSceneNode(nodeName);
		}
		else if(bCreate)
		{
			return pScen->createSceneNode(nodeName);
		}
	}
	return NULL;
}

void C3DSceneNode::AddToRootNode()
{
	C3DSceneNode root = GetRoot();
	root.AddChild(*this);
}

ARCVector3 C3DSceneNode::GetWorldPosition() const
{
	if(m_pNode)
	{
		return ARCVECTOR(m_pNode->_getDerivedPosition());
	}
	return ARCVector3::ZERO; 
}




void ISceneNodeImplPool::Clear()
{
	std::vector<ISceneNodeImpl*>::const_iterator itr = m_dataList.begin();
	for(;itr!=m_dataList.end();++itr){
		delete *itr;
	}
	m_dataList.clear();
}

void ISceneNodeImplPool::Remove( ISceneNodeImpl* p )
{
	std::vector<ISceneNodeImpl*>::iterator itr = std::find(m_dataList.begin(),m_dataList.end(),p);
	if(itr!=m_dataList.end()){
		m_dataList.erase(itr);
		delete p;
	}
	ASSERT(false);
}
