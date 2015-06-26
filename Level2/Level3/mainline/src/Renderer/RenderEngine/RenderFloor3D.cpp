#include "StdAfx.h"
#include "RenderFloor3D.h"
#include "OgreConvert.h"

#include <functional>
#include <algorithm>
#include ".\CustomizedRenderQueue.h"
#include "RenderEngine.h"
#include "MaterialDef.h"
using namespace Ogre;

#define PICTUREMAT_MAT "picturemap_mat"

//material name == texture name 
CString CRenderFloor3D::createTextureMaterialOnce(const CString& tex)
{
	if(tex.IsEmpty())
		return PICTUREMAT_MAT;

	if( TextureManager::getSingleton().resourceExists(tex.GetString()) )
	{
		TexturePtr texture = TextureManager::getSingleton().getByName(tex.GetString());
		if( MaterialManager::getSingleton().resourceExists(tex.GetString()) )
			return tex;
	}

	//load texture and create material
	CString strDir = tex.Left(tex.ReverseFind('\\'));
	ResourceFileArchive::AddDirectory(strDir,"temp");
	Image img;
	img.load(tex.GetString(),"temp");
	m_picSize.x = img.getWidth();
	m_picSize.y = img.getHeight();
	ResourceFileArchive::RemoveLocation(strDir,"temp");

	//try to load img 	
	while(true){
		try
		{
			size_t scaledwidth = m_picSize.x;
			size_t scaledheight = m_picSize.y;
			scaledwidth =  MIN(2048, scaledwidth);
			scaledheight =  MIN(2048, scaledheight);
			if(scaledwidth!= m_picSize.x || scaledheight != m_picSize.y ){
				img.resize(scaledwidth,scaledheight);
			}
			
			TextureManager::getSingleton().loadImage(tex.GetString(),ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,img,
				TEX_TYPE_2D,1);
			break;
		}
		catch(Ogre::Exception& e )
		{			
			size_t width = img.getWidth();
			size_t height = img.getHeight();
			if(width<2 || height <2)
				break;
			else
			{
				img.resize(width/2,height/2);
				continue;
			}
		}
	}	

	MaterialPtr mat = OgreUtil::cloneMaterial(tex,PICTUREMAT_MAT);
	if(!mat.isNull()){
		if(!mat->getNumTechniques()){ mat->createTechnique(); }
		Technique* pTech = mat->getTechnique(0);
		if(!pTech->getNumPasses()){ pTech->createPass(); }
		Pass* pPass = pTech->getPass(0);
		if(!pPass->getNumTextureUnitStates()){ pPass->createTextureUnitState(); }
		TextureUnitState* pTex = pPass->getTextureUnitState(0);
		pTex->setTextureName(tex.GetString());
		return tex;
	}
	return PICTUREMAT_MAT;
}

void CRenderFloor3D::UpdatePicture( CRenderFloor* floorData,C3DNodeObject& parentnode )
{
	//create texture y
	try
	{
		CString picNodeName;
		picNodeName.Format("%sPic",floorData->GetUID() );
		C3DNodeObject nodeObj = OgreUtil::createOrRetrieveSceneNode(picNodeName.GetString(),parentnode.GetScene());
		if(!floorData->m_picInfo.bShow){
			nodeObj.Detach();
			return;
		}

		ARCVector2 fixv1 = ARCVector2(floorData->m_picInfo.refLine.getPoint(0));
		fixv1[VY] = -fixv1[VY];
		ARCVector2 fixv2 = ARCVector2(floorData->m_picInfo.refLine.getPoint(1));
		fixv2[VY] = -fixv2[VY];

		ARCVector2 vDir = fixv2 - fixv1;

		if(vDir.Magnitude() < ARCMath::EPSILON )
		{
			return;
		}
	
		//Vector2 picSize(0,0);
		CString matName = createTextureMaterialOnce( floorData->m_picInfo.sFileName);
		//matName ="";



		ARCVector2 vNorth (0,1);
		double dAngle = vDir.GetAngleOfTwoVector(vNorth);

		double dRotate = -floorData->m_picInfo.dRotation +dAngle;
		double dScale = floorData->m_picInfo.dScale /floorData->m_picInfo.refLine.GetTotalLength();
		ARCVector2 vPicOffset = (fixv1 + fixv2) * 0.5;


		Vector3 offset = Vector3(-vPicOffset[VX],-vPicOffset[VY],0);
		ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(picNodeName,parentnode.GetScene());
		pObj->clear();
		pObj->begin(matName.GetString());

		pObj->position(offset);pObj->textureCoord(0,0);pObj->normal(0,0,1);
		pObj->position(offset+Vector3(0,-m_picSize.y,0) );pObj->textureCoord(0,1);
		pObj->position(offset+Vector3(m_picSize.x,-m_picSize.y,0));pObj->textureCoord(1,1);
		pObj->position(offset+Vector3(m_picSize.x,0,0) );pObj->textureCoord(1,0);	

		pObj->quad(0,1,2,3);
		pObj->end();
		pObj->setRenderQueueGroupAndPriority(RenderFloors_Solid,2);

		nodeObj.AddObject(pObj);
		nodeObj.SetScale(ARCVector3(dScale,dScale,1));
		nodeObj.SetRotation(ARCVector3(dRotate,0,0),ER_ZXY);
		nodeObj.SetPosition(ARCVector3(floorData->m_picInfo.vOffset,0));
		nodeObj.AttachTo(parentnode);
		
	}	
	catch (Ogre::Exception& e )
	{
		const char* text = e.what();
	}

}


CRenderFloor3D::~CRenderFloor3D()
{
	Destory();
}

void CRenderFloor3D::Update(CRenderFloor* floorData,C3DNodeObject& parent)
{
	// retrieve the level info
	mpDeck = floorData;
	CBaseFloor3D::Update(parent);
	//update picture
	UpdatePicture(floorData,*this);
	
}

void CRenderFloor3D::UpdateAltitude( double dAltitude )
{
	ARCVector3 pos = GetPosition();
	pos.z = dAltitude;
	SetPosition(pos);
}
class NodeAttachFunctor
{
public:
	NodeAttachFunctor(C3DNodeObject obj)
		: m_obj(obj)
	{

	}
	void operator()(C3DNodeObject objChild)
	{
		objChild.AttachTo(m_obj);
	}
private:
	C3DNodeObject m_obj;
};

void CRenderFloor3DList::Update( CRenderFloorList& floorData, BOOL inEditMode )
{
	RenderFloor3DVector newdata;
	// update
	for (CRenderFloorList::iterator ite = floorData.begin();ite!=floorData.end(); ite++)
	{
		CRenderFloor* pFloor = *ite;
		CRenderFloor3D::SharedPtr ptr = GetFloorByData(pFloor);
		if(!ptr.get()){			
			Ogre::SceneNode* pNode = OgreUtil::createOrRetrieveSceneNode(pFloor->GetUID(),GetScene());
			ptr = CRenderFloor3D::SharedPtr( new CRenderFloor3D( pNode)  ); 
		}
		ptr->Update(*ite, *this);
		ptr->ShowGrid(inEditMode);
		newdata.push_back(ptr);
	}
	m_vecFloors = newdata;
}

void CRenderFloor3DList::UpdateFloorAltitude( CRenderFloor* pFloorData )
{
	CRenderFloor3D::SharedPtr ptr = GetFloorByData(pFloorData);
	if(ptr){
		ptr->UpdateAltitude(pFloorData->Altitude());
	}

}

void CRenderFloor3DList::UpdateFloor( CRenderFloor* pFloorData, BOOL inEditMode )
{
	CRenderFloor3D::SharedPtr ptr = GetFloorByData(pFloorData);
	if(ptr){
		ptr->Update(pFloorData,*this);
		ptr->ShowGrid(inEditMode);
	}

	
}



CRenderFloor3D::SharedPtr CRenderFloor3DList::GetFloorByData( CRenderFloor* pFloorData )
{
	for(RenderFloor3DVector::iterator itr = m_vecFloors.begin();itr!=m_vecFloors.end();++itr){
		CRenderFloor3D::SharedPtr& ptr= *itr;
		if (ptr->GetName() == pFloorData->GetUID() )
		{
			return ptr;
		}
	}
	return CRenderFloor3D::SharedPtr(); 
}

CRenderFloor3D::SharedPtr CRenderFloor3DList::GetFloorByLevel( int level )
{
	for(RenderFloor3DVector::iterator itr = m_vecFloors.begin();itr!=m_vecFloors.end();++itr){
		CRenderFloor3D::SharedPtr& ptr= *itr;
		CRenderFloor* pfloor =  (CRenderFloor*)(ptr->getKey());
		if ( pfloor && pfloor->Level() == level   )
		{
			return ptr;
		}
	}
	return CRenderFloor3D::SharedPtr(); 
}
