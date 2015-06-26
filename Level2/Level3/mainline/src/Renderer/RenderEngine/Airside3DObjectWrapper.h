#pragma once


template <class DerivedClass, class DataType>
class CAirside3DObjectWrapper : CAirside3DObject
{
	CAirside3DObjectWrapper(Ogre::SceneNode* pNode, CRender3DScene* p3DScene, const CGuid& guid)
		: CAirside3DObject(pNode, p3DScene, guid)
	{

	}

	typedef boost::shared_ptr<DerivedClass> SharedPtr;
	typedef std::vector<SharedPtr> List;


	DataType* GetObject() const
	{
		return NULL; //???
	}
	virtual ALTObject* GetBaseObject() const // return NULL if not maintaining a CBaseObject
	{
		return GetObject();
	}

};


// #include "RenderAirport3D.h"

template <class DerivedClass, class DataType>
class CAirport3DObjectWrapper : public CAirport3DObject
{
public:
	CAirport3DObjectWrapper(Ogre::SceneNode* pNode, CRender3DScene* p3DScene, CRenderAirside3D* pAirport3D, const CGuid& guid)
		: CAirport3DObject(pNode, p3DScene, pAirport3D, guid)
	{

	}

	typedef boost::shared_ptr<DerivedClass> SharedPtr;
	typedef std::vector<SharedPtr> List;


	DataType* GetObject() const
	{
		return m_pAirport3D->QueryDataObject<DataType>(GetGuid());
	}
	virtual ALTObject* GetBaseObject() const // return NULL if not maintaining a CBaseObject
	{
		return (ALTObject*)GetObject();
	}

};


#define DECLARE_AIRSIDE3DOBJECT_SIMPLE_CTOR(ClassName, DataType)					\
	ClassName(Ogre::SceneNode* pNode, CRender3DScene* p3DScene, const CGuid& guid)	\
	: CAirport3DObjectWrapper<ClassName, DataType>(pNode, p3DScene, guid) {}																												\


#define DECLARE_AIRPORT3DOBJECT_SIMPLE_CTOR(ClassName, DataType)													\
	ClassName(Ogre::SceneNode* pNode, CRender3DScene* p3DScene, CRenderAirside3D* pAirport3D, const CGuid& guid)	\
	: CAirport3DObjectWrapper<ClassName, DataType>(pNode, p3DScene, pAirport3D, guid) {}																												\


#define IMPLEMENT_AIRSIDE3DOBJECT_SIMPLE_UPDATE(ClassName, DataType)										\
	void ClassName::Update3D(ALTObject* pBaseObj, int nUpdateCode/* = Terminal3DUpdateAll*/)			\
	{																									\
		DataType* pDataObject = (DataType*)pBaseObj;													\
		SceneManager* pScene = GetScene();																\
		ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(mpNode->getName().c_str(),pScene);	\
		Build(pDataObject, pObj);																		\
		pObj->setVisibilityFlags(NormalVisibleFlag);													\
		AddObject(pObj,true);																			\
		UpdateSelected(GetSelected());																	\
	}
