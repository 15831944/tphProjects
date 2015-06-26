#include "stdafx.h"
#include "3DFloorNode.h"
#include "Common/BaseFloor.h"
#include "OgreUtil.h"
#include "Grid3D.h"
#include "3DCADMap.h"
#include ".\Tesselator.h"
#include ".\CustomizedRenderQueue.h"
#include "Common\RenderFloor.h"
#include "RenderEngine.h"
#include "FloorSurfaceRender.h"

using namespace  Ogre;
#define FLOORMAT _T("BaseFloorMat")
#define FLOORMAT_THICKSIDE _T("BaseFloorThickSide")
#define FLOORMAT_OUTLINE _T("BaseFloorOutLine")
#define PICTUREMAT_MAT "picturemap_mat"

namespace
{
	void DrawRectTopBottomToObject(Ogre::ManualObject* pobj,Vector3& v1, Vector3&v2, Vector3&v3, Vector3&v4,double dThick=0,const String& topMat = FLOORMAT)
	{
		float c = ARCColor3::GREY_VISIBLEREGION_FLOAT;
		Vector3 offset = -dThick*Vector3::UNIT_Z;
		//top
		pobj->begin(topMat,RenderOperation::OT_TRIANGLE_FAN);
		pobj->position(v1);pobj->textureCoord(0,0);	
		pobj->colour( c,c,c);	
		pobj->normal(Vector3::UNIT_Z);
		pobj->position(v2);pobj->textureCoord(1,0);	
		pobj->position(v3);pobj->textureCoord(1,1);	
		pobj->position(v4);pobj->textureCoord(0,1);	
		pobj->end();
		////bottom
		pobj->begin(FLOORMAT,RenderOperation::OT_TRIANGLE_FAN);
		pobj->position(v4+offset);
		pobj->colour( c,c,c);
		pobj->normal(-Vector3::UNIT_Z);
		pobj->position(v3+offset);
		pobj->position(v2+offset);
		pobj->position(v1+offset);
		pobj->end();
	};
	void DrawRectSideToObject(Ogre::ManualObject* pobj, const VectorList &list, double dThick)
	{
		if(dThick ==0)return;

		float c = ARCColor3::GREY_VISIBLEREGION_FLOAT;
		Vector3 offset = -dThick*Vector3::UNIT_Z;
		pobj->begin(FLOORMAT_THICKSIDE,RenderOperation::OT_TRIANGLE_STRIP);
		for(int i=0;i<(int)list.size();i++)
		{
			Vector3 v1 = list.at(i);
			Vector3 v2 = list.at( (i+1)%list.size());
			pobj->position(v1);
			pobj->colour(c,c,c);
			Vector3 vnormal = (v2-v1).crossProduct(Vector3::UNIT_Z);
			vnormal.normalise();
			pobj->normal(vnormal);
			pobj->position(v1+offset);
			pobj->position(v2);
			pobj->position(v2+offset);
		}
		pobj->end();
	}


	void DrawRectOutLineToObject(Ogre::ManualObject* pobj, const VectorList &list, double dThick)
	{
		float c = ARCColor3::GREYVALUE_VISREGIONOUTLINE_FLOAT;
		Vector3 offset = -dThick*Vector3::UNIT_Z;
		pobj->begin(FLOORMAT_OUTLINE,RenderOperation::OT_LINE_STRIP);
		for(int i=0;i<(int)list.size();i++)
		{
			const Vector3& v1 = list[i];
			pobj->position(v1);
			pobj->colour(c,c,c);
			if(i==(int)list.size()-1)
				pobj->position(list[0]);
		}	
		pobj->end();

		if(dThick<ARCMath::EPSILON)
			return;

		pobj->begin(FLOORMAT_OUTLINE,RenderOperation::OT_LINE_STRIP);
		for(int i=0;i<(int)list.size();i++)
		{
			Vector3 v1 = list[i];
			pobj->position(v1+offset);
			pobj->colour(c,c,c);
			if(i==(int)list.size()-1)
				pobj->position( list[0]+offset );
		}	
		pobj->end();

		pobj->begin(FLOORMAT_OUTLINE,RenderOperation::OT_LINE_LIST);
		for(int i=0;i<(int)list.size();i++)
		{
			Vector3 v1 = list[i];
			pobj->position(v1);
			pobj->colour(c,c,c);
			pobj->position( v1+offset);	
		}	
		pobj->end();

	}

}


void C3DFloorNode::UpdateAll( CBaseFloor* pFloor , bool bInMode )
{
	if(!pFloor->IsVisible())
	{
		RemoveFromParent();
		return;
	}

	UpdateAltitude(pFloor->Altitude());

	bool bShowPic = UpdatePicture((CRenderFloor*)pFloor);
	
	//show 
	UpdateFloorSolid(pFloor);
	//
	UpdateGrid(pFloor, bInMode);

}

void C3DFloorNode::UpdateAltitude( double dAlt )
{
	SetPosition(0,0,dAlt);
}



void C3DFloorNode::UpdateGrid( CBaseFloor* pFloor,bool bInMode)
{
	ManualObject* gridObj = _getGridObject();
	RemoveObject(gridObj);
	if(!pFloor->IsGridVisible() || !bInMode ) { return ;}

	CGrid3D::Build( pFloor->m_Grid, gridObj, pFloor->IsActive() );
	AddObject(gridObj);
}

Ogre::ManualObject* C3DFloorNode::_getGridObject()
{
	CString gridName = GetName() + "-Grid";
	ManualObject* obj = OgreUtil::createOrRetrieveManualObject(gridName, getScene());
	return obj;
}



Ogre::ManualObject* C3DFloorNode::_getFloorObject()
{
	CString gridName = GetName() + "-Top";
	ManualObject* obj = OgreUtil::createOrRetrieveManualObject(gridName, getScene());
	return obj;
}


void C3DFloorNode::_UpdateGridRegion(  CBaseFloor* pFloor )
{	


	ManualObject* pobj = _getFloorObject();
	//use grid size
	CGrid* pGridData = pFloor->GetGrid();
	double dHfSizeX = pGridData->dSizeX;
	double dHfSizeY = pGridData->dSizeY;
	//top

	//
	Vector3 v1(-dHfSizeX*Vector3::UNIT_X-dHfSizeY*Vector3::UNIT_Y);
	Vector3 v2(dHfSizeX*Vector3::UNIT_X-dHfSizeY*Vector3::UNIT_Y);
	Vector3 v3(dHfSizeX*Vector3::UNIT_X+dHfSizeY*Vector3::UNIT_Y);
	Vector3 v4(-dHfSizeX*Vector3::UNIT_X+dHfSizeY*Vector3::UNIT_Y);
	VectorList vlist; vlist.reserve(4); 
	vlist.push_back(v1);vlist.push_back(v2);vlist.push_back(v3);vlist.push_back(v4);


	double dThick = pFloor->Thickness();
	DrawRectTopBottomToObject(pobj,v1,v2,v3,v4, dThick );
	DrawRectSideToObject(pobj,vlist,dThick);	
	DrawRectOutLineToObject(pobj,vlist,dThick);	
}


void C3DFloorNode::_UpdateVisibleRegion(  CBaseFloor* pFloor, bool bShowMap,const CString& matname )
{
	//use tesslator
	CTesselator visTess;
	{
		ARCVectorListList visOutline;
		for(int i=0;i<(int)pFloor->m_vVisibleRegions.size();i++)
		{
			Pollygon& poly = pFloor->m_vVisibleRegions[i]->polygon;
			ARCVectorList vlist= CTesselator::GetAntiClockVectorList(poly);			
			visOutline.push_back(vlist);		
		}
		visTess.MakeOutLine(visOutline,Vector3::UNIT_Z);
	}
	//in vistess
	CTesselator invisTess;
	{
		ARCVectorListList visOutline;
		for(int i=0;i<(int)pFloor->m_vInVisibleRegions.size();i++)
		{
			Pollygon& poly = pFloor->m_vInVisibleRegions[i]->polygon;
			ARCVectorList vlist=CTesselator::GetAntiClockVectorList(poly);			
			//std::reverse(vlist.begin(),vlist.end());			
			visOutline.push_back(vlist);		
		}
		invisTess.MakeOutLine(visOutline,Vector3::UNIT_Z);
	}
	//all Merge tess
	CTesselator allMergeoutline;
	CTesselator allMergeMesh;
	CTesselator allMergeMeshBack;
	{
		ARCVectorListList visOutline;
		for(int i=0;i<visTess.mCurMesh.getCount();i++)
		{
			visOutline.push_back(visTess.mCurMesh.getSection(i)->m_vPoints );
		}
		for(int i=0;i<invisTess.mCurMesh.getCount();i++)
		{
			ARCVectorList vlist = invisTess.mCurMesh.getSection(i)->m_vPoints;
			std::reverse(vlist.begin(),vlist.end());
			visOutline.push_back( vlist );
		}
		allMergeoutline.MakeOutLine(visOutline,Vector3::UNIT_Z);
		allMergeMesh.MakeMeshPositive(visOutline,Vector3::UNIT_Z);
		allMergeMeshBack.MakeMeshNegative(visOutline,-Vector3::UNIT_Z);
	}
	//build obj
	Matrix4 transmat = Matrix4::IDENTITY;
	if(bShowMap)
	{
		ARCVector2 vSize =  pFloor->m_vMapExtentsMax - pFloor->m_vMapExtentsMin;
		double dSizeX = vSize[VX]*pFloor->MapScale();
		double dSizeY = vSize[VY]*pFloor->MapScale();
		Vector3 offset(-pFloor->m_vOffset[VX]/dSizeX+0.5,-pFloor->m_vOffset[VY]/dSizeY+0.5,0);
		Vector3 scale(1.0/dSizeX,1.0/dSizeY,1.0);
		Quaternion quat;quat.FromAngleAxis(Degree(pFloor->MapRotation()),Vector3::UNIT_Z);
		transmat.makeTransform(offset,scale,quat);
	}

	ManualObject* pobj = _getFloorObject();
	pobj->clear();

	float c = ARCColor3::GREY_VISIBLEREGION_FLOAT;
	allMergeMesh.BuildMeshToObject(pobj,bShowMap?matname.GetString():FLOORMAT,Vector3::UNIT_Z,ColourValue(c,c,c),0,transmat); //top
	
	double dThick = 0;
	if(pFloor->IsOpaque())
	{		
		dThick = pFloor->Thickness();
		allMergeMeshBack.BuildMeshToObject(pobj,FLOORMAT,-Vector3::UNIT_Z,ColourValue(c,c,c), -dThick); //bottom
		allMergeoutline.BuildSideToObject(pobj,FLOORMAT_THICKSIDE,ColourValue(c,c,c),-dThick);
	}
	c = ARCColor3::GREYVALUE_VISREGIONOUTLINE_FLOAT;
	allMergeoutline.BuildOutlineToObject(pobj,FLOORMAT_OUTLINE,ColourValue(c,c,c), -dThick);
}

void C3DFloorNode::_UpdateMapRegion(  CBaseFloor* pFloor,const CString& matname )
{
	ManualObject* pobj = _getFloorObject();
	pobj->clear();
	//map size
	ARCVector2 vSize =  pFloor->m_vMapExtentsMax - pFloor->m_vMapExtentsMin;
	double dHfSizeX = vSize[VX]*0.5;
	double dHfSizeY = vSize[VY]*0.5;
	//
	Vector3 v1(-dHfSizeX*Vector3::UNIT_X-dHfSizeY*Vector3::UNIT_Y);
	Vector3 v2(dHfSizeX*Vector3::UNIT_X-dHfSizeY*Vector3::UNIT_Y);
	Vector3 v3(dHfSizeX*Vector3::UNIT_X+dHfSizeY*Vector3::UNIT_Y);
	Vector3 v4(-dHfSizeX*Vector3::UNIT_X+dHfSizeY*Vector3::UNIT_Y);

	double dScale = pFloor->m_CadFile.dScale;
	double dRotate = pFloor->m_CadFile.dRotation;
	ARCVector2 voffset = pFloor->m_vOffset;
	Quaternion quat;
	quat.FromAngleAxis(Degree(dRotate),Vector3::UNIT_Z);
	Matrix4 tranf;
	tranf.makeTransform(Vector3(voffset[VX],voffset[VY],0),Vector3(dScale,dScale,1),quat);	
	v1 = tranf*v1; v2  =tranf*v2; v3 = tranf*v3; v4= tranf*v4;

	VectorList vlist; vlist.reserve(4); 
	vlist.push_back(v1);vlist.push_back(v2);vlist.push_back(v3);vlist.push_back(v4);
	double dThick = pFloor->IsOpaque()?pFloor->Thickness():0;

	DrawRectTopBottomToObject(pobj,v1,v2,v3,v4, dThick, matname.GetString());

	DrawRectSideToObject(pobj,vlist,dThick);				

	DrawRectOutLineToObject(pobj,vlist,dThick);
}

void C3DFloorNode::UpdateFloorSolid( CBaseFloor* pFloor )
{
	CString sCadMat = FLOORMAT ;
	bool bShowCad = _updateCadMapMaterial(pFloor,sCadMat);
	ManualObject* pobj = _getFloorObject();
	pobj->clear();

	RemoveObject(pobj);

	if(pFloor->UseVisibleRegions())
	{
		_UpdateVisibleRegion(pFloor,bShowCad,sCadMat);
	}
	else if(bShowCad)
	{
		_UpdateMapRegion(pFloor,sCadMat);
	}
	else if(pFloor->IsOpaque())
	{
		_UpdateGridRegion(pFloor);
	}
	else{
		return;
	}

	pobj->setRenderQueueGroup(RenderFloors_Solid);
	if(MaterialManager::getSingleton().resourceExists(sCadMat.GetString()))
	{
		MaterialPtr pSolidMat = MaterialManager::getSingleton().getByName(sCadMat.GetString());
		pSolidMat->getTechnique(0)->getPass(0)->getUserObjectBindings().
			setUserAny(FloorSurfaceRender::StencilValue,Any(FloorSurfaceRender::getIntance()->GetStencilValue()));
	}
	int nNumSec = pobj->getNumSections();
	for(int i=0;i<nNumSec;++i)
		pobj->getSection(i)->setMaterialName(sCadMat.GetString());
	
	AddObject(pobj);

	_UpdateSharpModeNode(pFloor,sCadMat,FloorSurfaceRender::getIntance()->GetStencilValue());

}




bool C3DFloorNode::UpdatePicture( CRenderFloor* floorData )
{
	//create texture y
	try
	{
		C3DSceneNode picNode = GetSceneNode( GetName()  + _T("Pic"), true );
		picNode.RemoveFromParent();

		if(!floorData->m_picInfo.bShow){	return false; 	}
	
		//create texture 
		CString matName;
		if( !_createPicuterTextureMat(floorData->m_picInfo.sFileName, matName) )
		{
			return false;
		}
	
		ARCVector2 fixv1 = ARCVector2(floorData->m_picInfo.refLine.getPoint(0));
		fixv1[VY] = -fixv1[VY];
		ARCVector2 fixv2 = ARCVector2(floorData->m_picInfo.refLine.getPoint(1));
		fixv2[VY] = -fixv2[VY];

		ARCVector2 vDir = fixv2 - fixv1;		
		double d  = fixv2.y - fixv1.y;
		
		ARCVector2 vNorth (0,1);
		double dAngle = vDir.GetAngleOfTwoVector(vNorth);

		double dRotate = -floorData->m_picInfo.dRotation +dAngle;
		double dScale = floorData->m_picInfo.dScale /floorData->m_picInfo.refLine.GetTotalLength();
		ARCVector2 vPicOffset = (fixv1 + fixv2) * 0.5;


		Vector3 offset = Vector3(-vPicOffset[VX],-vPicOffset[VY],0);
		ManualObject* pObj = _GetOrCreateManualObject(picNode.GetName());//::createOrRetrieveManualObject(picNodeName,parentnode.GetScene());
		pObj->clear();
		pObj->begin(matName.GetString());

		pObj->position(offset);pObj->textureCoord(0,0);pObj->normal(0,0,1);
		pObj->position(offset+Vector3(0,-m_picSize.y,0) );pObj->textureCoord(0,1);
		pObj->position(offset+Vector3(m_picSize.x,-m_picSize.y,0));pObj->textureCoord(1,1);
		pObj->position(offset+Vector3(m_picSize.x,0,0) );pObj->textureCoord(1,0);	

		pObj->quad(0,1,2,3);
		pObj->end();
		pObj->setRenderQueueGroupAndPriority(RenderFloors_Solid,2);

		picNode.AddObject(pObj);
		picNode.SetScale(ARCVector3(dScale,dScale,1));
		picNode.SetRotation(ARCVector3(0,0,dRotate),ER_XYZ);
		picNode.SetPosition(ARCVector3(floorData->m_picInfo.vOffset,0));
		
		AddChild(picNode);
	}	
	catch (Ogre::Exception& e )
	{
		const char* text = e.what();
		return false;
	}
	return true;
}

bool C3DFloorNode::_createPicuterTextureMat( const CString& tex, CString& matName )
{
	if(tex.IsEmpty())
		return false;

	if( TextureManager::getSingleton().resourceExists(tex.GetString()) )
	{
		TexturePtr texture = TextureManager::getSingleton().getByName(tex.GetString());
		if( MaterialManager::getSingleton().resourceExists(tex.GetString()) )
		{
			matName = tex;
			return true;
		}
	}

	//load texture and create material
	CString strDir = tex.Left(tex.ReverseFind('\\'));
	ResourceFileArchive::AddDirectory(strDir,"temp");
	Image img;
	try
	{
		img.load(tex.GetString(),"temp");
		m_picSize.x = img.getWidth();
		m_picSize.y = img.getHeight();
	}
	catch (Ogre::Exception&)
	{
		ResourceFileArchive::RemoveLocation(strDir,"temp");
		return false;
	}
	ResourceFileArchive::RemoveLocation(strDir,"temp");

	Ogre::ushort scaledwidth = ARCMath::FloorTo2n(m_picSize.x);
	Ogre::ushort scaledheight = ARCMath::FloorTo2n(m_picSize.y);
	scaledwidth =  MIN(2048, scaledwidth);
	scaledheight =  MIN(2048, scaledheight);
	//try to load img 	
	while(true){
		try
		{
			if(scaledwidth!= m_picSize.x || scaledheight != m_picSize.y ){
				img.resize(scaledwidth,scaledheight);
			}

			TextureManager::getSingleton().loadImage(tex.GetString(),ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,img,
				TEX_TYPE_2D,0);
			break;
		}
		catch(Ogre::Exception& )
		{			
			Ogre::ushort width = (Ogre::ushort)img.getWidth();
			Ogre::ushort height = (Ogre::ushort)img.getHeight();
			if(width<2 && height <2)
			{
				return false;
			}
			else
			{
				scaledwidth/=2;
				scaledheight/=2;
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
		matName = tex;
		return true;
	}
	return false;
}

bool C3DFloorNode::_updateCadMapMaterial( CBaseFloor* pFloor,CString& matName )
{

	if(!pFloor->IsShowMap())
		return false;

	if(!pFloor->IsMapValid())
		return false;

	if(!pFloor->IsCADLoaded())
	{
		if(!pFloor->LoadCAD())
			return false;
		if(!pFloor->LoadCADLayers())
			return false;		
	}
//  	matName = FLOORMAT;
//  	return true;

	matName = GetName() + "cadMap";
	if( matName != C3DCADMapLayerList::GenMapTexture(pFloor,matName) )
		return false;

	if( !MaterialManager::getSingleton().resourceExists(matName.GetString()) )//create material once
	{
		FloorSurfaceRender::getIntance()->IncreaseStencilValue();
		MaterialPtr matPtrfloor = MaterialManager::getSingleton().getByName(FLOORMAT);
		ASSERT(!matPtrfloor.isNull());
		MaterialPtr mMapMatPtr = matPtrfloor->clone(matName.GetString());
		Technique* pTech = mMapMatPtr->getTechnique(0);
		Pass* pPass = pTech->getPass(0);
		TextureUnitState* pTexture=NULL;
		if(pPass->getNumTextureUnitStates())
		{
			pTexture= pPass->getTextureUnitState(0);
		}
		else
		{
			pTexture = pPass->createTextureUnitState();
		}
		pTexture->setTextureName(matName.GetString());	
		pTexture->setColourOperation(LBO_ALPHA_BLEND);
		pTexture->setTextureAddressingMode(TextureUnitState::TAM_BORDER);
		pTexture->setTextureBorderColour(ColourValue(0,0,0,0));
	}	
	return true;
}

C3DSceneNode C3DFloorNode::_getSharpModeNode()
{
	return GetSceneNode(GetName()+"CadSHARP",true);
}

//class FloorLister : public MovableObject::Listener
//{
//public:
//	virtual bool objectRendering(const MovableObject*, const Camera*) { 
//		//ASSERT(false);	
//		return true; 
//	}
//
//
//};

bool C3DFloorNode::_UpdateSharpModeNode(CBaseFloor* pFloor,CString MatName,int StencilValue)
{
	C3DSceneNode node =  _getSharpModeNode();
	node.DetachAllObjects();
	
	if(!pFloor->IsShowMap())
		return false;

	if(!pFloor->IsMapValid())
		return false;

	if(!pFloor->IsCADLoaded())
	{
		if(!pFloor->LoadCAD())
			return false;
		if(!pFloor->LoadCADLayers())
			return false;		
	}

	ManualObject* pobj = node._GetOrCreateManualObject(node.GetName());
	pobj->clear();
	C3DCADMapLayerList::UpdateToGeometry(pFloor,pobj);
	node.AddObject(pobj);
	pobj->setRenderQueueGroup(RenderFloors_SurfaceMap);
	//pobj->setListener(new FloorLister());
	if(MaterialManager::getSingleton().resourceExists(MatName.GetString()))
	{
		MaterialPtr pCadMat = MaterialManager::getSingleton().getByName(MatName.GetString());
		pCadMat->getTechnique(0)->getPass(0)->getUserObjectBindings().
			setUserAny(FloorSurfaceRender::StencilValue,Any(StencilValue));
	}
	int nNumSec = pobj->getNumSections();
	for(int i=0;i<nNumSec;++i)
		pobj->getSection(i)->setMaterialName(MatName.GetString());

	CCADInfo & cadInfo = pFloor->m_CadFile;
	node.SetPosition(ARCVector3(pFloor->m_vOffset,0));
	node.SetRotation(ARCVector3(0,0,cadInfo.dRotation),ER_XYZ);
	node.SetScale(ARCVector3(cadInfo.dScale,cadInfo.dScale,0));
	//
	

	this->AddChild(node);	
	return true;
}

void C3DFloorNode::UpdateShowSharp( bool b )
{
	if(b){
		C3DSceneNode node =  _getSharpModeNode();
		node.RemoveFromParent();
	}
	else{
		ManualObject* pobj = _getFloorObject();
	}
}



