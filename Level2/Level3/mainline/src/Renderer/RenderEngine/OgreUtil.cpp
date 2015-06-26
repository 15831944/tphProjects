#include "StdAfx.h"
#include "ManualObjectExt.h"
#include ".\ogreutil.h"
#include "../3DTools/3DTools.h"
#include <common/util.h>
#include <boost\tuple\tuple.hpp>
#include "OgreBillboardObject.h"
#include "OgreConvert.h"

using namespace Ogre;

Ogre::Camera* OgreUtil::createOrRetrieveCamera( const CString& strName, Ogre::SceneManager* pScene)
{
	String strCam = strName.GetString();
	Camera* pCam = NULL;
	if( pScene->hasCamera(strCam) )
	{
		pCam = pScene->getCamera(strCam); 
	}
	else
	{
		pCam = pScene->createCamera(strCam);
		pCam->setAutoAspectRatio(true);
	}
	SceneNode* pCamNode = createOrRetrieveSceneNode(strName,pScene);
	AttachMovableObject(pCam,pCamNode);
	//AttachNodeTo(pCamNode,pScene->getRootSceneNode());
	return pCam;
}

Ogre::SceneNode* OgreUtil::createOrRetrieveSceneNode( const CString& strNodeName , Ogre::SceneManager* pScene)
{	
	String nodeName = strNodeName.GetString();
	SceneNode* pNode = NULL;
	if(pScene->hasSceneNode(nodeName))
	{
		pNode = pScene->getSceneNode(nodeName);
	}
	else
	{
		pNode = pScene->createSceneNode(nodeName);
	}
	return pNode;
}


Ogre::ManualObjectExt* OgreUtil::createOrRetrieveManualObjectExt( const CString& txtName,Ogre::SceneManager* pScene )
{
	String nodeName = txtName.GetString();
	if(pScene->hasMovableObject(nodeName,ManualObjectExtFactory::FACTORY_TYPE_NAME))
	{
		return (ManualObjectExt*)pScene->getMovableObject(nodeName,ManualObjectExtFactory::FACTORY_TYPE_NAME);
	}
	return (ManualObjectExt*)pScene->createMovableObject(nodeName,ManualObjectExtFactory::FACTORY_TYPE_NAME);
}


void OgreUtil::RemoveSceneNode( Ogre::SceneNode* pNode )
{
	if(!pNode)return;
	SceneNode* pParent = pNode->getParentSceneNode();
	if(pParent)
	{
		pParent->removeChild(pNode);
		pParent->_updateBounds();
	}
}

Ogre::ManualObject* OgreUtil::createOrRetrieveManualObject( const CString& objName,SceneManager* pScene )
{
	if(!pScene)return NULL;

	if(pScene->hasManualObject(objName.GetString()))
	{
		return pScene->getManualObject(objName.GetString());
	}
	ManualObject* pobj =  pScene->createManualObject(objName.GetString());
	pobj->setVisibilityFlags(NormalVisibleFlag);
	return pobj;
}

//#include "ManualObjectExt.h"
//Ogre::ManualObject* OgreUtil::createOrRetrieveManualObject( const CString& objName,SceneManager* pScene )
//{
//	if(!pScene)return NULL;
//
//	if(pScene->hasMovableObject(objName.GetString(), ManualObjectExtFactory::FACTORY_TYPE_NAME))
//	{
//		return (ManualObject*)pScene->getMovableObject(objName.GetString(), ManualObjectExtFactory::FACTORY_TYPE_NAME );
//	}
//	ManualObject* pobj =  (ManualObject*)pScene->createMovableObject(objName.GetString(),ManualObjectExtFactory::FACTORY_TYPE_NAME);
//	pobj->setVisibilityFlags(NormalVisibleFlag);
//	return pobj;
//}


Ogre::Light* OgreUtil::createOrRetrieveLight( const CString& str , Ogre::SceneManager* pScene)
{
	Light* plight = NULL;
	String lightName = str.GetString();
	if(pScene->hasLight(lightName))
	{
		plight = pScene->getLight(lightName);
	}
	else
	{
		plight = pScene->createLight(lightName);
	}		
	return plight;
}


void OgreUtil::AttachMovableObject( Ogre::MovableObject* pObj, Ogre::SceneNode* pNode )
{
	ASSERT(pObj && pNode);
	if(!pObj || !pNode)
		return;

	if(pObj->getParentSceneNode()==pNode)
		return ;
	if(pObj->getParentSceneNode())
		pObj->getParentSceneNode()->detachObject(pObj);
	pNode->attachObject(pObj);
}

Ogre::Entity* OgreUtil::createOrRetrieveEntity( const CString& entName, const CString& meshName,SceneManager* pScene )
{
	Entity* pEnt = NULL;
	if(pScene->hasEntity(entName.GetString()) )
	{
		pEnt  = pScene->getEntity(entName.GetString());
		if( pEnt->getMesh()->getName() == String(meshName.GetString()) )
			return pEnt;
		pScene->destroyEntity(pEnt);
		pEnt = NULL;
	}
	try
	{	
		pEnt = pScene->createEntity(entName.GetString(),meshName.GetString());
		pEnt->setVisibilityFlags(NormalVisibleFlag);
	}	
	catch (Exception& e)
	{
		e;
	}
	return pEnt;	
}


void OgreUtil::DetachMovableObject( Ogre::MovableObject* pObj )
{
	SceneNode* pNode = pObj->getParentSceneNode();
	if(pNode)
		pNode->detachObject(pObj);
}

void OgreUtil::AttachNodeTo( Ogre::SceneNode* pNodeChild,Ogre::SceneNode* pParent )
{
	if(!pNodeChild || !pParent)
		return;
	if(pNodeChild == pParent)
		return;
	if(pNodeChild->getParent()==pParent)
		return;

	if(pNodeChild->getParent())
		pNodeChild->getParent()->removeChild(pNodeChild);	
	pParent->addChild(pNodeChild);	
}

void OgreUtil::SetObjectMaterial( Ogre::ManualObject* pObj,const Ogre::String& matName )
{
	for(int i=0;i<(int)pObj->getNumSections();i++)
	{
		pObj->getSection(i)->setMaterialName(matName);
	}
}

Ogre::MaterialPtr OgreUtil::cloneMaterial( const CString& newName, const CString& srcMat )
{
	Ogre::MaterialPtr pMat;

	if(  Ogre::MaterialManager::getSingleton().resourceExists(newName.GetString()) )
	{
		Ogre::MaterialManager::getSingleton().remove(newName.GetString());
	}

	if( Ogre::MaterialManager::getSingleton().resourceExists(srcMat.GetString()) )
	{
		Ogre::ResourcePtr res = Ogre::MaterialManager::getSingleton().getByName(srcMat.GetString());
		Ogre::Material* pcloneMat = (Ogre::Material*)res.getPointer();
		pMat = pcloneMat->clone(newName.GetString());
	}
	else
	{		
		Ogre::MaterialPtr defaultMat = Ogre::MaterialManager::getSingleton().getDefaultSettings();
		pMat = defaultMat->clone(newName.GetString());
	}
	return pMat;
}

void OgreUtil::SetMaterial( Ogre::MovableObject *pobj,const CString &matName )
{
	if(!pobj)
		return;

	if(pobj->getMovableType() == EntityFactory::FACTORY_TYPE_NAME)
	{
		Entity* pEnt = (Entity*)pobj;
		pEnt->setMaterialName(matName.GetString());
	}
	if(pobj->getMovableType() == ManualObjectFactory::FACTORY_TYPE_NAME)
	{
		ManualObject* pManual = (ManualObject*)pobj;
		for(int i=0;i<(int)pManual->getNumSections();i++)
		{
			pManual->setMaterialName(i,matName.GetString());
		}
	}
}



void SetCustomParameterManual( Ogre::ManualObject *pobj, size_t idx, const Ogre::Vector4& val )
{
	for(int i=0;i<(int)pobj->getNumSections();i++)
	{
		pobj->getSection(i)->setCustomParameter(idx,val);
	}
}

void SetCustomParameterEntity( Ogre::Entity *pobj, size_t idx, const Ogre::Vector4& val )
{
	for(int i=0;i<(int)pobj->getNumSubEntities();i++)
	{
		pobj->getSubEntity(i)->setCustomParameter(idx,val);
	}
}

void Ogre::OgreUtil::SetCustomParameter( Ogre::MovableObject *pobj, size_t idx, const Ogre::Vector4& val )
{
	if(pobj->getMovableType() == ManualObjectFactory::FACTORY_TYPE_NAME )
	{
		SetCustomParameterManual( (Ogre::ManualObject*)pobj,idx,val);
	}
	if(pobj->getMovableType()==EntityFactory::FACTORY_TYPE_NAME )
	{
		SetCustomParameterEntity( (Ogre::Entity*)pobj,idx,val);
	}
}

//bool Ogre::OgreUtil::Import3DSToMeshFile( CString s3dsfile, CString meshPrefix, CString dir )
//{
//	C3DTools::C3DS2Mesh toMesh(meshPrefix,)
//	if( C3DTools::Convert3DsToMesh(s3dsfile.GetString(),meshPrefix.GetString(), 1.0f, dir.GetString() ) )
//	{		
//		return true;
//	}
//	return false;
//}
//
bool Ogre::OgreUtil::GetRayIntersectPos( const Ogre::Ray& ray,const Ogre::Plane& plane, Ogre::Vector3& pos )
{
	bool bRet; Real dist;
	boost::tie(bRet,dist) = ray.intersects(plane);
	if(bRet){
		pos = ray.getPoint(dist);
	}
	return bRet;
}

Ogre::MaterialPtr Ogre::OgreUtil::createOrRetrieveMaterial( const CString& matName )
{
	bool bCreated = false;
	return createOrRetrieveMaterial2(matName, bCreated);
}

Ogre::MaterialPtr Ogre::OgreUtil::createOrRetrieveMaterial2( const CString& matName, bool& bCreated )
{
   const String& matGroup=ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;	
	 ResourceManager::ResourceCreateOrRetrieveResult ret = MaterialManager::getSingleton().createOrRetrieve(matName.GetString(), matGroup);
	bCreated = ret.second;
	ret.first->load();
	return ret.first;
}

#define PureColorMaterialPrefix _T("PureClrMat")
Ogre::MaterialPtr Ogre::OgreUtil::createOrRetrieveColorMaterial( COLORREF clr)
{	
	CString outMat;
	outMat.Format(_T("%s%08x"), PureColorMaterialPrefix, clr);
	bool bCreated = false;
	MaterialPtr matPtr = OgreUtil::createOrRetrieveMaterial2(outMat, bCreated);
	if(!matPtr.isNull() && bCreated)
	{
		if(!matPtr->getNumTechniques())
			matPtr->createTechnique();
		Technique* pTech = matPtr->getTechnique(0);
		if(!pTech->getNumPasses()){
			pTech->createPass();
		}
		Pass * pPass = pTech->getPass(0);
		Ogre::ColourValue ogreColor = OgreConvert::GetColor(clr);
		pPass->setAmbient( ogreColor );
		pPass->setDiffuse( ogreColor );
		//pPass->setCullingMode(CULL_NONE);
		matPtr->load();
	}
	return matPtr;
}

static const char* s_colorTemplateName[Ogre::OgreUtil::ColorTemplateCount] =
{
	"Airside/TaxiwayCenterLine",
};

static const char* s_colorNameFormat[Ogre::OgreUtil::ColorTemplateCount] =
{
	"TaxiLnMat%08x",
};

Ogre::MaterialPtr Ogre::OgreUtil::createOrRetrieveColorMaterialFromTemplate( COLORREF clr, ColorTemplateType ctt )
{
	if (DefaultColor == ctt)
		return createOrRetrieveColorMaterial(clr);

	CString outMat;
	outMat.Format(s_colorNameFormat[ctt], clr);
	bool bCreated = false;
	MaterialPtr matPtr = createOrRetrieveMaterial2(outMat, bCreated);
	if (!matPtr.isNull() && bCreated)
	{
		MaterialPtr matTemplatePtr = createOrRetrieveMaterial(s_colorTemplateName[ctt]);
		if(!matTemplatePtr.isNull())
		{
			matTemplatePtr->copyDetailsTo(matPtr); // copy
			if(!matPtr->getNumTechniques())
				matPtr->createTechnique();

			Technique* pTech = matPtr->getTechnique(0);
			if(!pTech->getNumPasses())
				pTech->createPass();

			Pass * pPass = pTech->getPass(0);
			Ogre::ColourValue ogreColor = OgreConvert::GetColor(clr);
			pPass->setAmbient( ogreColor );
			pPass->setDiffuse( ogreColor );
			//pPass->setCullingMode(CULL_NONE);
			matPtr->load();
		}
	}
	return matPtr;
}

#define PureColorMatNoCullPrefix _T("PureClrNoCull")
Ogre::MaterialPtr Ogre::OgreUtil::createOrRetrieveColormaterialNoCull( COLORREF clr )
{
	CString outMat;
	outMat.Format(_T("%s%08x"), PureColorMatNoCullPrefix, clr);
	bool bCreated = false;
	MaterialPtr matPtr = OgreUtil::createOrRetrieveMaterial2(outMat, bCreated);
	if(!matPtr.isNull() && bCreated)
	{
		if(!matPtr->getNumTechniques())
			matPtr->createTechnique();
		Technique* pTech = matPtr->getTechnique(0);
		if(!pTech->getNumPasses()){
			pTech->createPass();
		}
		Pass * pPass = pTech->getPass(0);
		Ogre::ColourValue ogreColor = OgreConvert::GetColor(clr);
		pPass->setAmbient( ogreColor );
		pPass->setDiffuse( ogreColor );
		pPass->setCullingMode(CULL_NONE);
		matPtr->load();
	}
	return matPtr;
}
#define PureColorMatNoLightPrefix _T("PureClrNoLight")
Ogre::MaterialPtr Ogre::OgreUtil::createOrRetrieveColorMaterialNoLight( COLORREF clr)
{
	CString outMat;
	outMat.Format(_T("%s%08x"), PureColorMatNoLightPrefix, clr);
	bool bCreated = false;
	MaterialPtr matPtr = OgreUtil::createOrRetrieveMaterial2(outMat, bCreated);
	if(!matPtr.isNull() && bCreated)
	{
		if(!matPtr->getNumTechniques())
			matPtr->createTechnique();
		Technique* pTech = matPtr->getTechnique(0);
		if(!pTech->getNumPasses()){
			pTech->createPass();
		}
		Pass * pPass = pTech->getPass(0);
		Ogre::ColourValue ogreColor = OgreConvert::GetColor(clr);	
		pPass->setLightingEnabled(false);
		pPass->setSelfIllumination( ogreColor );
		//pPass->setCullingMode(CULL_NONE);
		matPtr->load();
	}
	return matPtr;
}


void Ogre::OgreUtil::UpdateCameraNearFar( Ogre::Camera* pCam )
{
	if(!pCam)
		return;

	Ogre::SceneManager* pscene = pCam->getSceneManager();
	Ogre::SceneNode * pRootNode = pscene->getRootSceneNode();
	AxisAlignedBox sceneBBox = pRootNode->_getWorldAABB();
	Ogre::Plane cameraPlane(pCam->getRealDirection(),pCam->getRealPosition() );

	Real nearDist = 100.0f;
	Real farDist = 200.0f;

	if( sceneBBox.isFinite())
	{
		const Ogre::Vector3* vCorners = sceneBBox.getAllCorners();
		nearDist = farDist =  cameraPlane.getDistance(vCorners[0]) ;
		for(int i=1;i<8;i++)
		{
			Ogre::Real dist = cameraPlane.getDistance(vCorners[i]);
			if(dist < nearDist){ nearDist = dist; }
			if(dist > farDist){ farDist = dist; }
		}
	}
	if(nearDist <=0 && pCam->getProjectionType()==PT_ORTHOGRAPHIC)
	{
		Vector3 camdir = pCam->getRealDirection();
		SceneNode* pNode = pCam->getParentSceneNode();
		pNode->translate( (nearDist-1)*camdir,Node::TS_PARENT );			
		nearDist = 1;
	}

	//
	//nearDist -= 100.0;
	nearDist = MAX(nearDist-1,100);
	pCam->setNearClipDistance( nearDist );		

	farDist = MAX(farDist+1,nearDist+1);	
	pCam->setFarClipDistance( farDist );

	//TRACE("%.2f %.2f \n",nearDist,farDist);
	
}

Ogre::BillboardObject* Ogre::OgreUtil::createOrRetriveBillBoardObject( const CString& objName,Ogre::SceneManager* pScene )
{
	String nodeName = objName.GetString();
	const String& typeName = BillboardObjectFactory::FACTORY_TYPE_NAME;
	if(pScene->hasMovableObject(nodeName,typeName) )
	{
		return (BillboardObject*)pScene->getMovableObject(nodeName,typeName);
	}
	return (BillboardObject*)pScene->createMovableObject(nodeName,typeName);
}

Ogre::Overlay* Ogre::OgreUtil::createOrRetriveOverlay( const CString& strName )
{
	String sName = strName.GetString();
	Ogre::Overlay* pOvlay = OverlayManager::getSingleton().getByName(sName);
	if(!pOvlay){
		pOvlay = OverlayManager::getSingleton().create(sName);
	}
	return pOvlay;
}

void Ogre::OgreUtil::SetEntityColor( Entity* pEnt , const MaterialPtr& matPtr,const String& replacematName )
{
	unsigned int nEntCount = pEnt->getNumSubEntities();
	if (nEntCount)
	{
		for (unsigned int i=0;i<nEntCount;i++)
		{					
			SubEntity* pSubEnt = pEnt->getSubEntity(i);					
			String matEnt = pSubEnt->getMaterialName();
			if ( pSubEnt->getMaterialName()== replacematName)
			{
				pSubEnt->setMaterial(matPtr);
			}
		}
	}
	//set lod
	for(size_t i=0;i<pEnt->getNumManualLodLevels();i++)
	{
		SetEntityColor(pEnt->getManualLodLevel(i),matPtr,replacematName);
	}
}

void Ogre::OgreUtil::ReplaceEntityColor( Entity* pEnt , const ARCColor4& c, const String& replaceMat )
{
	MaterialPtr matPtr = OgreUtil::createOrRetrieveColorMaterial(c);
	SetEntityColor(pEnt,matPtr,replaceMat);
}

const Ogre::String& Ogre::OgreUtil::createOrRetrieveTextureMaterial( const CString& texFileName )
{
	CString outMat =  CString(_T("texMat\\") ) + texFileName; 
	bool bCreated = false;
	MaterialPtr matPtr = createOrRetrieveMaterial2(outMat, bCreated);
	if (!matPtr.isNull() && bCreated)
	{			
		Technique* pTech = NULL;
		
		if(!matPtr->getNumTechniques())
		{	pTech = matPtr->createTechnique();
		}
		else
		{	pTech = matPtr->getTechnique(0);
		}

		Pass * pPass =NULL;
		if(!pTech->getNumPasses())
		{
			pPass = pTech->createPass();
		}
		else
            pPass = pTech->getPass(0);
		pPass->setCullingMode(CULL_NONE);
		pPass->setManualCullingMode(MANUAL_CULL_NONE);
		pPass->setDepthBias(-0.1,-0.1);

		TextureUnitState* texUnit = NULL;
		if(pPass->getNumTextureUnitStates())
		{
			texUnit = pPass->getTextureUnitState(0);
			texUnit->setTextureName(texFileName.GetString());
		}
		else
			texUnit = pPass->createTextureUnitState(texFileName.GetString());
		texUnit->setTextureScale(2000,2000);

		
		matPtr->load();
		
	}
	return matPtr->getName();
}


