#pragma once
#include "3DNodeObject.h"
#include "Terminal3DListBase.h"

#include <Common/Guid.h>

class CBaseObject;
class CRender3DScene;
class CObjectDisplay;


class ALTObject;
class ALTObjectDisplayProp;

class ALTAirspace;

class RENDERENGINE_API C3DSceneNodeObject : public AutoDetachNodeObject
{
public:
	enum EnumObjectType
	{
		ObjectType_Terminal= 0,
		ObjectType_Airside, // airside
		ObjectType_Airport, // airport related, subset of ObjectType_Airside

		//
		ObjectType_Count

	};
	typedef boost::shared_ptr<C3DSceneNodeObject> SharedPtr;

	C3DSceneNodeObject(Ogre::SceneNode* pNode, CRender3DScene* p3DScene)
		: AutoDetachNodeObject(pNode)
		, m_p3DScene(p3DScene)
		, m_bSelected(false)
	{

	}
	~C3DSceneNodeObject()
	{
	}
public:

	virtual EnumObjectType GetObjectType() const = 0;

	virtual void UpdateSelected(bool b); // show bounding box by default, or overrided

	bool GetSelected() const { return m_bSelected; }

protected:
	void SetSelected(bool val) { m_bSelected = val; }

protected:
	CRender3DScene* m_p3DScene;

	bool m_bSelected; // decides whether the object has been selected

};



class RENDERENGINE_API CModeBase3DObject : public C3DSceneNodeObject
{
public:
	typedef boost::shared_ptr<CModeBase3DObject> SharedPtr;

	CModeBase3DObject(Ogre::SceneNode* pNode, CRender3DScene* p3DScene, const CGuid& guid)
		: C3DSceneNodeObject(pNode, p3DScene)
		, m_guid(guid)
	{

	}
	~CModeBase3DObject()
	{
	}
public:
	const CGuid& GetGuid() const { return m_guid; }


protected:
	const CGuid m_guid;

};


class RENDERENGINE_API CTerminal3DObject  : public CModeBase3DObject
{
public:
	typedef boost::shared_ptr<CTerminal3DObject> SharedPtr;

	CTerminal3DObject(Ogre::SceneNode* pNode, CRender3DScene* p3DScene, const CGuid& guid)
		: CModeBase3DObject(pNode,p3DScene,guid)
	{}
	~CTerminal3DObject()
	{}


public:
	virtual void Update3D(CBaseObject* pBaseObj, int nUpdateCode = Terminal3DUpdateAll);
	virtual void Update3D(CObjectDisplay* pDispObj, int nUpdateCode = Terminal3DUpdateAll);
	virtual CBaseObject* GetBaseObject() const; // return NULL if not maintaining a CBaseObject
	virtual CObjectDisplay* GetDisplayObject() const; // return NULL if not maintaining a CObjectDisplay

	virtual EnumObjectType GetObjectType() const {return ObjectType_Terminal;}
};

class RENDERENGINE_API CAirside3DObject : public CModeBase3DObject
{
public:
	CAirside3DObject(Ogre::SceneNode* pNode, CRender3DScene* p3DScene, const CGuid& guid )
		: CModeBase3DObject(pNode,p3DScene,guid)
	{
	}
	~CAirside3DObject()
	{

	}

public:
	virtual void Update3D(ALTObject* pBaseObj, int nUpdateCode = Terminal3DUpdateAll);
	virtual void Update3D(ALTObjectDisplayProp* pDispObj, int nUpdateCode = Terminal3DUpdateAll);
	virtual ALTObject* GetBaseObject() const; // return NULL if not maintaining a CBaseObject
	virtual ALTObjectDisplayProp* GetDisplayProp() const; // return NULL if not maintaining a CObjectDisplay

	virtual CModeBase3DObject::EnumObjectType GetObjectType() const {return ObjectType_Airside;}

};

class CRenderAirside3D;

class RENDERENGINE_API CAirport3DObject : public CAirside3DObject
{
public:
	CAirport3DObject(Ogre::SceneNode* pNode, CRender3DScene* p3DScene, CRenderAirside3D* pAirport3D, const CGuid& guid )
		: CAirside3DObject(pNode,p3DScene,guid)
		, m_pAirport3D(pAirport3D)
	{
	}

	virtual CModeBase3DObject::EnumObjectType GetObjectType() const {return ObjectType_Airport;}

protected:
	CRenderAirside3D* m_pAirport3D;

};

