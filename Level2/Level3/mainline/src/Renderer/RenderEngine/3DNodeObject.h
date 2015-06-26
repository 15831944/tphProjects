#pragma once
#include "AfxHeader.h"
class ARCVector3;
class ARCVector4;

#include <vector>
#include <boost/shared_ptr.hpp>
#include "RenderEngineAPI.h"

class TiXmlElement;
class RotationSeries;
enum EularRotType
{
	ER_XYZ,
	ER_XZY,
	ER_YXZ,
	ER_YZX,
	ER_ZXY,
	ER_ZYX,
};

class RENDERENGINE_API C3DNodeObject 
{
public:
	typedef boost::shared_ptr<C3DNodeObject> SharedPtr;

	C3DNodeObject(){ mpNode  = NULL;}
	C3DNodeObject(Ogre::SceneNode* pNode){ mpNode = pNode; }
	C3DNodeObject& operator=(Ogre::SceneNode* pNode){ mpNode = pNode; return *this; }

	operator bool () const { return IsValid(); }
	virtual bool IsValid()const{  return NULL!=mpNode; }

	bool HasMovableObj(Ogre::MovableObject* pObj,bool cascade = true)const;
	
	//remove all child Node
	void RemoveChild();
	
	//detach all object
	void RemoveAllObject();

	bool HoldSameSceneNode(const C3DNodeObject& otherNode) const { return mpNode == otherNode.mpNode; }

	//relate to parent transform functions
	void SetScale(const ARCVector3& scale);

	
	void SetRotation(const ARCVector3& eularAngle, EularRotType rotType = ER_XYZ); // refer to setOrientation for better choice
	void SetRotation(const ARCVector4& quat);	
	void Rotate(const ARCVector3& eularAngle, EularRotType rotType = ER_XYZ);
	void RotateLocal(const ARCVector3& eularAngle, EularRotType rotType = ER_XYZ);

	void SetPosition(const ARCVector3& pos);

	void Translate(ARCVector3& d);

	//void Translate(Ogre::Real x,Ogre::Real y,Ogre::Real z, Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_PARENT);

	//void DoRotation(const RotationSeries& rots) const;

	void SetDirection(const ARCVector3& vHeadDir, const ARCVector3& vUpDir );


	
	//relate to parent
	ARCVector3 GetScale()const;
	ARCVector3 GetRotation()const;
	ARCVector3 GetPosition()const;

	//bound
	ARCVector3 GetBounds()const;

	// position conversion
	ARCVector3 ConvertPositionLocalToWorld(const ARCVector3& pos);
	ARCVector3 ConvertPositionWorldToLocal(const ARCVector3& pos);

	//set transform is Id
	void SetIdentTransform()const;

	// not the scene node name
	// caution: conflict with SetAny and GetAny
	CString GetName() const;
	void SetName(CString sName);

	// caution: conflict with GetName and SetName
	void SetAny(const Ogre::Any& any);
	const Ogre::Any& GetAny()const;	

	//the node unique name
	CString GetIDName()const;

	typedef std::vector<C3DNodeObject> NodeList;	
	NodeList GetChild()const;
	C3DNodeObject GetParent() const;

	//remove visible flag for object attached in the node
	void RemoveVisibleFlag(Ogre::uint32 visibleFlag,bool cascade= true);
	//add visible flag for object attached in the node
	void AddVisibleFlag( Ogre::uint32 visibleFlag,bool cascade= true);
	//set visibleFlag
	void SetVisibleFlag( Ogre::uint32 visibleFlag,bool cascade = true );

	//-----------------------START-------------------------------------
	// For GUI Only
	// only set this object node visible or not
	void SetVisible(bool bVisible);
	// get the visibility of only this object node
	bool GetVisible() const;
	// decide whether the node is an group
	bool IsGroup() const;
	// add a sub empty node
	C3DNodeObject CreateNewChild();
	//add  a sub node with name
	C3DNodeObject CreateChild(const CString& sName);
	//add a child which name is name+idx,
	C3DNodeObject CreateOrRetrieveChild(int idx);
	//-------------------------END-----------------------------------

	void SetShowBound(bool b);
	bool GetShowBound() const;

	//detach all child
	void Clear();

	bool AttachToRoot();
	void AttachTo(C3DNodeObject& otherNode);
	void Detach();
	C3DNodeObject GetRoot()const;
	//not exportable	
	Ogre::SceneManager* GetScene()const;
	Ogre::SceneNode* GetSceneNode()const{ return mpNode; }

	//0 : no select, 1 select, >2 first select, decrepted , do not use 
	void AddObject(Ogre::MovableObject* pObj,int bSelectable = 0,bool bVisble = true);

	//call this after all object added and updated
	void EnableSelect(int bOrder);

	void SetMaterial(const CString& str, bool bcascad = true);
	//export 
	bool ExportToMesh(CString meshPrefix, CString folder);
	void SnapShot(CString tmpFolder);

	void Destory();
protected:
	Ogre::SceneNode* mpNode;
};


class AutoDeleteNodeObject : public C3DNodeObject
{
public:
	AutoDeleteNodeObject(){}
	AutoDeleteNodeObject(Ogre::SceneNode* pNode):C3DNodeObject(pNode){}
	virtual ~AutoDeleteNodeObject();

};


class AutoDetachNodeObject : public C3DNodeObject
{
public:
	typedef boost::shared_ptr<AutoDetachNodeObject> SharedPtr;
	AutoDetachNodeObject(){};
	AutoDetachNodeObject(Ogre::SceneNode* pNode):C3DNodeObject(pNode){}
	virtual ~AutoDetachNodeObject(){ Detach(); }
};

#define SCENENODE_CONVERSION_DEF(TCLASS,PCLASS) TCLASS(){}\
	TCLASS(const C3DNodeObject& obj):PCLASS(obj){}\
	TCLASS(Ogre::SceneNode* pNode):PCLASS(pNode){}
	

