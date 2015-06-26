#pragma once

#include <common/ARCVector.h>
#include <common/ARCColor.h>
#include <common/Point2008.h>

namespace Ogre
{
	class ManualObjectExt;
	class BillboardObject;
	class OgreUtil
	{
	public:
		static void SetObjectMaterial(Ogre::ManualObject* pObj,const Ogre::String& matName);
		static Ogre::Camera* createOrRetrieveCamera(const CString& strName,Ogre::SceneManager* pScene);
		static Ogre::SceneNode* createOrRetrieveSceneNode(const CString& strNodeName,Ogre::SceneManager* pScene);
		static Ogre::ManualObject* createOrRetrieveManualObject(const CString& objName,Ogre::SceneManager* pScene);
		static Ogre::Light* createOrRetrieveLight(const CString& lightname,Ogre::SceneManager* pScene);
		static Ogre::Entity* createOrRetrieveEntity(const CString& entName, const CString& meshName,Ogre::SceneManager* pScene);
		static Ogre::ManualObjectExt* createOrRetrieveManualObjectExt(const CString& coordName,Ogre::SceneManager* pScene);
		static Ogre::MaterialPtr createOrRetrieveMaterial(const CString& matName);
		static Ogre::MaterialPtr createOrRetrieveMaterial2(const CString& matName, bool& bCreated);
		
		static Ogre::MaterialPtr createOrRetrieveColorMaterial(COLORREF clr);
		static Ogre::MaterialPtr createOrRetrieveColormaterialNoCull(COLORREF clr);
		static Ogre::MaterialPtr createOrRetrieveColorMaterialNoLight(COLORREF clr);

		static const Ogre::String& createOrRetrieveTextureMaterial(const CString& texFileName);


		static 
		enum ColorTemplateType
		{
			DefaultColor = -1,

			ClrMat_TaxiwayCenterLine = 0,

			ColorTemplateCount,
		};
		static Ogre::MaterialPtr createOrRetrieveColorMaterialFromTemplate(COLORREF clr, ColorTemplateType ctt);

		static Ogre::BillboardObject* createOrRetriveBillBoardObject(const CString& objName,Ogre::SceneManager* pScene);
		static Ogre::Overlay* createOrRetriveOverlay(const CString& strName);		

		static void AttachNodeTo(Ogre::SceneNode* pChild, Ogre::SceneNode* pParent );
		static void AttachMovableObject(Ogre::MovableObject* pObj, Ogre::SceneNode* pNode);
		static void DetachMovableObject(Ogre::MovableObject* pObj);
		static void RemoveSceneNode(Ogre::SceneNode* pNodeChild);

		static void SetMaterial(Ogre::MovableObject *pobj,const CString &matName);

		static void SetCustomParameter(Ogre::MovableObject *pobj, size_t idx, const Ogre::Vector4& val);

		static Ogre::MaterialPtr cloneMaterial( const CString& newName, const CString& srcMat );

		static bool GetRayIntersectPos(const Ogre::Ray& ray,const Ogre::Plane& plane, Ogre::Vector3& pos);

		static void UpdateCameraNearFar(Ogre::Camera* pCam);



		static void SetEntityColor(Entity* pEnt , const MaterialPtr& matPtr,const String& replacematName);
		
		static void ReplaceEntityColor(Entity* pEnt , const ARCColor4& c, const String& replaceMat);

	};
}


inline Ogre::Vector3 OGREVECTOR(const ARCVector3& vec)
{
	return Ogre::Vector3(static_cast<Ogre::Real>(vec[VX]), static_cast<Ogre::Real>(vec[VY]),static_cast<Ogre::Real>(vec[VZ]) );
}
inline Ogre::Vector3 OGREVECTOR(const CPoint2008& pos)
{
	return Ogre::Vector3(static_cast<Ogre::Real>(pos.x), static_cast<Ogre::Real>(pos.y), static_cast<Ogre::Real>(pos.z));
}


inline ARCVector3 ARCVECTOR(const Ogre::Vector3& vec){
	return ARCVector3(vec.x,vec.y,vec.z);
}


inline Ogre::ColourValue OGRECOLOR(const ARCColor3& arcColor, float a = 1.0f)
{
	return Ogre::ColourValue(arcColor[RED]/255.0f, arcColor[GREEN]/255.0f,arcColor[BLUE]/255.0f,a  );
}

inline Ogre::ColourValue OGRECOLOR(const ARCColor4& arcColor)
{
	return Ogre::ColourValue(arcColor[RED]/255.0f, arcColor[GREEN]/255.0f, arcColor[BLUE]/255.0f, arcColor[ALPHA]/255.0f);
}

inline Ogre::ColourValue OGRECOLOR(const ARCColor4& arcColor, float a )
{
	return Ogre::ColourValue(arcColor[RED]/255.0f, arcColor[GREEN]/255.0f,arcColor[BLUE]/255.0f,a  );
}
