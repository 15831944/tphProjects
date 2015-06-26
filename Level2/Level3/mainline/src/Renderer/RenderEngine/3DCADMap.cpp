#include "StdAfx.h"
#include ".\3dcadmap.h"
#include "Camera.h"
#include "RenderEngine.h"
#include <OpenDWGToolKit/CADFileLayer.h>
#define CADMapTarget "CADMapBackGround"

using namespace Ogre;
CString C3DCADMapLayerList::GenMapTexture( const CCADFileDisplayInfo& displyinfo,CString textTurename )
{
	const CCADFileContent::SharePtr&pFlieLayers  = displyinfo.mLoadedLayers;
	return GenMapTexture(pFlieLayers.get(),textTurename);
}

CString C3DCADMapLayerList::GenMapTexture( CCADFileContent* pFlieLayers,CString textTurename )
{
	if( pFlieLayers->ProcessCAD() )
	{
		SceneManager* pScene  = RenderEngine::getSingleton().CreateSceneManager();

		const ARCVector2 vMin = pFlieLayers->m_vMapExtentsMin;
		const ARCVector2 vMax = pFlieLayers->m_vMapExtentsMax;

		ARCVector2 vCenter = ARCVector2(0,0);//(vMax + vMin)*0.5;
		ARCVector2 vExt = vMax - vMin;
		{
			C3DCADMapLayerList m3dlayers =  pScene->getRootSceneNode()->createChildSceneNode();		
			m3dlayers.Update(pFlieLayers);

			TexturePtr texture = TextureManager::getSingleton().getByName(textTurename.GetString());
			if(texture.isNull())
				texture = TextureManager::getSingleton().createManual( textTurename.GetString(), 
				ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 
				1024, 1024, 0, PF_R8G8B8A8, TU_RENDERTARGET );
			RenderTarget *rttTex = texture->getBuffer()->getRenderTarget();	

			CCameraNode camera = OgreUtil::createOrRetrieveCamera(_T("ZCamera"), pScene);
			camera.LookAt(ARCVector3(vCenter[VX],vCenter[VY],-100),ARCVector3(vCenter[VX],vCenter[VY],0),ARCVector3(0,-1,0) );
			camera.Set2D(vExt[VX],vExt[VY]);
			camera.SetNearFar(1,200);	
			Viewport *v = rttTex->addViewport( camera.mpCamera );
			v->setClearEveryFrame( true );
			v->setBackgroundColour(ColourValue(0,0,0,0));
			rttTex->update();	
			//rttTex->writeContentsToFile(_T("c:\\debug.bmp"));
			rttTex->removeAllViewports();	
			//texture->reload();
			
			/*TexturePtr outTex = TextureManager::getSingleton().getByName(textTurename.GetString());
			if(outTex.isNull())
			{
				outTex = TextureManager::getSingleton().createManual( textTurename.GetString(), 
					ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 
					1024, 1024,0, texture->getFormat(),texture->getUsage() );
			}
			texture->copyToTexture(outTex);	*/

		}
		RenderEngine::getSingleton().DestroySceneManager(pScene);
	}
	return textTurename;
}

void C3DCADMapLayerList::UpdateTransform( const CCADInfo& cadInfo )
{	
	if(mpNode)
	{
		mpNode->setPosition( 0,cadInfo.vOffset[VX], cadInfo.vOffset[VY] );
		Ogre::Quaternion quat;
		quat.FromAxes(Vector3::UNIT_Y, Vector3::UNIT_Z, Vector3::UNIT_X);
		Ogre::Quaternion quat2;
		quat2.FromAngleAxis( Radian(cadInfo.dRotation), Vector3::UNIT_Z);
		mpNode->setOrientation(quat*quat2);
		mpNode->setScale(cadInfo.dScale * Vector3::UNIT_SCALE);
	}
	
}

void C3DCADMapLayerList::Update( const CCADFileDisplayInfo& displyinfo )
{
	CCADFileContent::SharePtr pFlieLayers = displyinfo.mLoadedLayers;

	if(!pFlieLayers.get())
		return ;

	if( !pFlieLayers->ProcessCAD() )
		return;

	mLayerlist.clear();
	

	for(int i=0;i<pFlieLayers->GetLayerCount();i++)
	{
		CCADLayer* player = pFlieLayers->GetLayer(i);
		int displayinfoidx = displyinfo.GetLayerInfoIdx(player->sName);		
		{
			C3DCADMapLayer* pLayer3D = new C3DCADMapLayer( player,mpNode->createChildSceneNode() );
			bool bVisible = true;
			ARCColor3 destColor;
		
			if(displayinfoidx>=0)
			{
				const CCADLayerInfo& layInfo = displyinfo.LayerAt(displayinfoidx);
				destColor = layInfo.cColor;
				bVisible = bVisible && layInfo.bIsOn;
			}
			else					
			{				
				destColor = player->cColor;
				bVisible = bVisible && player->bIsOn;
			}
			
			pLayer3D->UpdateLayer(destColor);
			pLayer3D->SetVisible(bVisible);
			//pLayer3D->UpdateMaterial();			
			mLayerlist.push_back( C3DCADMapLayer::SharedPtr(pLayer3D) );				
		}
	}
}

void C3DCADMapLayerList::Update( CCADFileContent* pFlieLayers )
{		
	if( !pFlieLayers->ProcessCAD() )
		return;

	mLayerlist.clear();	

	for(int i=0;i<pFlieLayers->GetLayerCount();i++)
	{
		CCADLayer* player = pFlieLayers->GetLayer(i);			
		if(player->bIsOn){
			C3DCADMapLayer* pLayer3D = new C3DCADMapLayer( player,mpNode->createChildSceneNode() );
			ARCColor3 destColor = player->cColor;
			BOOL bVisible = player->bIsOn;
			pLayer3D->UpdateLayer(destColor);				
			mLayerlist.push_back( C3DCADMapLayer::SharedPtr(pLayer3D) );				
		}
	}
}



//bool C3DCADMapLayerList::GenStaticGeometry(const CCADFileDisplayInfo& displyinfo,Ogre::StaticGeometry* geo )
//{
//	SceneManager* pScene  = RenderEngine::getSingleton().CreateSceneManager();
//	C3DCADMapLayerList m3dlayers =  pScene->getRootSceneNode()->createChildSceneNode();		
//	bool bsucess  = (m3dlayers.UpdateToGeometry(displyinfo,geo));
//	RenderEngine::getSingleton().DestroySceneManager(pScene);
//	return bsucess;
//}
//

#define LINE_MAT _T("GridLine")
// #define MAX_VERTEX_COUNT 10000

bool C3DCADMapLayerList::UpdateToGeometry( CCADFileContent* pFlieLayers ,Ogre::ManualObject* geo )
{
	//CCADFileContent::SharePtr pFlieLayers = displyinfo.mLoadedLayers;

	if(!pFlieLayers)
		return false;

	if( !pFlieLayers->ProcessCAD() )
		return false;

	//draw lines to list first
// 	geo->estimateVertexCount(MAX_VERTEX_COUNT);
	geo->begin(LINE_MAT,RenderOperation::OT_LINE_LIST);
	for(int i=0;i<pFlieLayers->GetLayerCount();i++)
	{
		CCADLayer* player = pFlieLayers->GetLayer(i);
		BOOL bVisible =  player->bIsOn;
		const ARCColor3& destColor = player->bIsMonochrome?player->cMonochromeColor:player->cColor;		
		if(bVisible)
		{
			C3DCADMapLayer::UpdateLayerToStaticGeo(player,destColor,geo,true);
		}	
	}	
	geo->end();

	///draw other shapes
	for(int i=0;i<pFlieLayers->GetLayerCount();i++)
	{
		CCADLayer* player = pFlieLayers->GetLayer(i);
		BOOL bVisible = player->bIsOn;
		const ARCColor3& destColor = player->bIsMonochrome?player->cMonochromeColor:player->cColor;		
		if(bVisible)
		{
			C3DCADMapLayer::UpdateLayerToStaticGeo(player,destColor,geo,false);
		}	
	}	
	
	return true;
}
//
void C3DCADMapLayer::UpdateLayer(const ARCColor3& color)
{	
	Ogre::ManualObject* p3DObj = OgreUtil::createOrRetrieveManualObject( GetIDName(), GetScene());
	p3DObj->clear();
	for(size_t i=0; i<m_player->m_vEntities.size(); i++) 
	{
		m_player->m_vEntities[i]->DrawToObject(p3DObj, Ogre::Matrix4::IDENTITY,OGRECOLOR(color) );
	}
	AddObject(p3DObj);
}

void C3DCADMapLayer::UpdateLayerToStaticGeo(CCADLayer* _player, const ARCColor3& color,Ogre::ManualObject* p3DObj, bool bLineList )
{	
	for(size_t i=0; i<_player->m_vEntities.size(); i++) 
	{
		DwgEntities::DwgEntity* pEnt = _player->m_vEntities[i];
		
		if(bLineList  ){
			pEnt->DrawToLineList(p3DObj,Ogre::Matrix4::IDENTITY,OGRECOLOR(color) );
		}
		else{
			pEnt->DrawToObject(p3DObj,Ogre::Matrix4::IDENTITY,OGRECOLOR(color));
		}
	}	
}
//
//void C3DCADMapLayer::UpdateMaterial( const CCADFileDisplayInfo& displyinfo )
//{
//	CCADLayer* player = getLayer();
//	bool bVisible = displyinfo.bVisible==TRUE;
//
//	if(player )
//	{		
//		CString matName = GetIDName() +_T("mat");
//		Ogre::MaterialPtr m_pMaterial = MaterialManager::getSingleton().getByName(matName.GetString());
//		Ogre::ManualObject* p3DObj = OgreUtil::createOrRetrieveManualObject( GetIDName(), GetScene());
//		
//		if(m_pMaterial.isNull())
//		{
//			m_pMaterial = OgreUtil::cloneMaterial(matName , _T("") );
//		}
//
//		if(!m_pMaterial.isNull() )
//		{
//			if(displyinfo.IsMonochrome())
//			{
//				m_pMaterial->setAmbient( OGRECOLOR(displyinfo.m_cMonochromeColor) );
//				m_pMaterial->setDiffuse( OGRECOLOR(displyinfo.m_cMonochromeColor) );
//			}
//			else
//			{
//				int displayinfoidx = displyinfo.GetLayerInfoIdx(player->sName);
//				if(displayinfoidx>=0)
//				{
//					const CCADLayerInfo& layInfo = displyinfo.LayerAt(displayinfoidx);
//					m_pMaterial->setAmbient( OGRECOLOR(layInfo.cColor) );
//					m_pMaterial->setDiffuse( OGRECOLOR(layInfo.cColor) );
//					bVisible = bVisible && layInfo.bIsOn;
//				}
//				else					
//				{
//					m_pMaterial->setAmbient( OGRECOLOR(player->cColor) );
//					m_pMaterial->setDiffuse( OGRECOLOR(player->cColor) );
//				}
//			}	
//			
//			for(int i=0 ;i<(int)p3DObj->getNumSections();++i)
//			{
//				p3DObj->setMaterialName(i,m_pMaterial->getName());
//			}
//		}	
//		p3DObj->setVisible(bVisible);
//	}	
//}
//
//void C3DCADMapLayer::UpdateMaterial()
//{
//	CCADLayer* player = getLayer();
//
//	bool bVisible = true;
//	if(player )
//	{		
//		CString matName = GetIDName() +_T("mat");
//		Ogre::MaterialPtr m_pMaterial = MaterialManager::getSingleton().getByName(matName.GetString());
//		Ogre::ManualObject* p3DObj = OgreUtil::createOrRetrieveManualObject( GetIDName(), GetScene());
//
//		if(m_pMaterial.isNull())
//		{
//			m_pMaterial = OgreUtil::cloneMaterial(matName , _T("") );
//		}
//
//		if(!m_pMaterial.isNull() )
//		{
//			m_pMaterial->setAmbient( OGRECOLOR(player->cColor) );
//			m_pMaterial->setDiffuse( OGRECOLOR(player->cColor) );
//
//			for(int i=0 ;i<(int)p3DObj->getNumSections();++i)
//			{
//				p3DObj->setMaterialName(i,m_pMaterial->getName());
//			}
//		}	
//		p3DObj->setVisible(bVisible);
//	}	
//}
//
//
//
CString C3DCADMapNonSharp::updateRTTMaterial( const CCADFileDisplayInfo& displyinfo, CString matName )
{
	CString texName = C3DCADMapLayerList::GenMapTexture(displyinfo,matName);

	//create debug material
	MaterialPtr pMat = MaterialManager::getSingleton().createOrRetrieve(texName.GetString(),ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME).first;
	
	Pass* pas =  pMat->getTechnique(0)->getPass(0);
	TextureUnitState* pTexStat = NULL;
	pas->setCullingMode(CULL_NONE);
	pas->setSceneBlending(SBT_TRANSPARENT_ALPHA);
	pas->setLightingEnabled(false);
	if(pas->getNumTextureUnitStates())
	{
		pTexStat = pas->getTextureUnitState(0);			
	}
	else
	{
		pTexStat = pas->createTextureUnitState();
	}	
	pTexStat->setTextureName(texName.GetString());

	return texName;
}

CString C3DCADMapNonSharp::updateRTTMaterial( CCADFileContent* pFlieLayers, CString matName )
{
	CString texName = C3DCADMapLayerList::GenMapTexture(pFlieLayers,matName);

	//create debug material
	MaterialPtr pMat = MaterialManager::getSingleton().createOrRetrieve(texName.GetString(),ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME).first;

	Pass* pas =  pMat->getTechnique(0)->getPass(0);
	TextureUnitState* pTexStat = NULL;
	//pas->setCullingMode(CULL_NONE);
	//pas->setSceneBlending(SBT_TRANSPARENT_ALPHA);
	pas->setLightingEnabled(false);
	if(pas->getNumTextureUnitStates())
	{
		pTexStat = pas->getTextureUnitState(0);			
	}
	else
	{
		pTexStat = pas->createTextureUnitState();
	}	
	//pTexStat->setAlphaOperation(LBX_SOURCE1);
	pTexStat->setColourOperation(LBO_ALPHA_BLEND);
	pTexStat->setTextureName(texName.GetString());
	pTexStat->setNumMipmaps(5);

	return texName;
}
void C3DCADMapNonSharp::Update( const CCADFileDisplayInfo& displyinfo )
{
	const CCADFileContent::SharePtr& pFlieLayers = displyinfo.mLoadedLayers;
	if( pFlieLayers.get() && 
		pFlieLayers->ProcessCAD() )
	{
		CString matName = updateRTTMaterial(displyinfo, GetIDName());

		const ARCVector2 vMin = pFlieLayers->m_vMapExtentsMin;
		const ARCVector2 vMax = pFlieLayers->m_vMapExtentsMax;	

		Ogre::ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());
		ASSERT(pObj);
		pObj->clear();
		pObj->begin(matName.GetString(),RenderOperation::OT_TRIANGLE_FAN);
		pObj->position(vMin[VX],vMin[VY],0);
		pObj->normal(0,0,1);
		pObj->textureCoord(0,0);
		
		pObj->position(vMax[VX],vMin[VY],0);
		pObj->normal(0,0,1);
		pObj->textureCoord(1,0);

		pObj->position(vMax[VX],vMax[VY],0);
		pObj->normal(0,0,1);
		pObj->textureCoord(1,1);

		pObj->position(vMin[VX],vMax[VY],0);
		pObj->normal(0,0,1);
		pObj->textureCoord(0,1);
		pObj->end();

		AddObject(pObj);

	}

}

void C3DCADMapNonSharp::UpdateTransform( const CCADInfo& cadInfo )
{
	if(mpNode)
	{
		mpNode->setPosition( 0,cadInfo.vOffset[VX], cadInfo.vOffset[VY] );
		Ogre::Quaternion quat;
		quat.FromAxes(Vector3::UNIT_Y, Vector3::UNIT_Z, Vector3::UNIT_X);
		Ogre::Quaternion quat2;
		quat2.FromAngleAxis( Radian(cadInfo.dRotation), Vector3::UNIT_Z);
		mpNode->setOrientation(quat*quat2);
		mpNode->setScale(cadInfo.dScale * Vector3::UNIT_SCALE);
	}
}