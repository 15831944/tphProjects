#pragma once
#include "AfxHeader.h"


class IARCportLayoutEditContext;
class ISceneNodeImpl;
class ISceneNodeImplPool;
class ARCVector3;
class C3DRootSceneNode;
//the representative of the ogre scene  node, with some function in arc port
namespace Ogre
{
	class SceneNode;
	class Any;
	class SceneManager;
}

class C3DSceneNode 
{
public:
	enum EularRotType
	{
		ER_XYZ,
		ER_XZY,
		ER_YXZ,
		ER_YZX,
		ER_ZXY,
		ER_ZYX,
	};


	C3DSceneNode():m_pNode(NULL){}
	C3DSceneNode(Ogre::SceneNode*pNode):m_pNode(pNode){}
	operator bool () const { return m_pNode!=NULL; }
	//
	CString GetName()const; 

	const Ogre::Any& GetUserAny(const CString& s)const;
	void SetUserAny(const Ogre::Any&a, const CString&s );
	

	//remove all child Node
	void RemoveAllChildNodes();
	//detach all movable object in ogre
	void DetachAllObjects();
	//get the scene root node
	C3DRootSceneNode GetRoot()const;
	//add child node
	void AddChild(C3DSceneNode& node);
		//
	void RemoveFromParent();

	void RemoveChild(const CString& id);

	void AddToRootNode();

	//
	C3DSceneNode GetParent()const;

	bool IsRoot()const;

	//0 : no select, 1 select, >2 first select, decrepted , do not use 
	void AddObject(Ogre::MovableObject* pObj,int bSelectable = 0,bool bVisble = true);
	
	//detach object
	void RemoveObject(Ogre::MovableObject* pObj);
	//

	void SetVisible(bool b);


	//transform
	void SetRotation( const ARCVector3& eularAngle, EularRotType rotType = ER_XYZ);
	void SetScale( const ARCVector3& scale );
	//set position
	void SetPosition(double x, double y, double z );
	void SetPosition(const ARCVector3& v);
	ARCVector3 GetPosition()const;
	ARCVector3 GetWorldPosition()const;

	//for implement
	ISceneNodeImpl* GetImpl();
	//always set new Impl will delete it 
	void SetImpl(ISceneNodeImpl* );
	//
	//get or retrieve child
	C3DSceneNode GetCreateChildNode(const CString& nodeName);
	C3DSceneNode GetSceneNode(const CString& nodeName, bool bCreate = false);
	Ogre::ManualObject* _GetOrCreateManualObject(const CString& name);
	Ogre::Entity* _GetOrCreateEntity(const CString& name, const CString& meshName);
	Ogre::SceneManager* getScene()const;

protected:
	Ogre::SceneNode* m_pNode;
	static void _GetRotateQuat( const ARCVector3& eularAngle, EularRotType rotType, Ogre::Quaternion& quat);
};

//
#define SCENE_NODE(TCLASS,PCLASS) public:\
	TCLASS(){}\
	TCLASS(const C3DSceneNode& obj):PCLASS(obj){}\
	TCLASS(Ogre::SceneNode* pNode):PCLASS(pNode){}



//root node 
class CRender3DSceneTags;
class C3DRootSceneNode : public C3DSceneNode
{
	SCENE_NODE(C3DRootSceneNode,C3DSceneNode)
public:	
	operator bool () const { return IsRoot(); }
	//set implement pool
	void SetImplPool(ISceneNodeImplPool* pPool);
	ISceneNodeImplPool* GetImplPool();
	//set the model
	IARCportLayoutEditContext* GetModel()const;
	void SetModel(IARCportLayoutEditContext* pModel);
	//set scene tags managerCRender3DSceneTags
	void SetTagManager(CRender3DSceneTags* pTag);
	CRender3DSceneTags* GetTagManager();	
};


//reactor of the scene node . update node, on click
class ISceneNodeImpl
{
public:
	virtual bool Update3D(C3DSceneNode& node){ return false; }
};


class ISceneNodeImplPool
{
public:
	~ISceneNodeImplPool(){ Clear(); }
	void Add(ISceneNodeImpl* p){ m_dataList.push_back(p); }
	void Remove(ISceneNodeImpl* p);
	void Clear();
protected:
	std::vector<ISceneNodeImpl*> m_dataList;
};



