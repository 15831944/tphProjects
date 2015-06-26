#include "StdAfx.h"
#include ".\BaseFloor3D.h"


#include <inputonboard/Deck.h>
#include "OgreUtil.h"
#include "3DCADMap.h"
#include ".\Tesselator.h"
#include ".\CustomizedRenderQueue.h"

#define FLOORMAT _T("BaseFloorMat")
#define FLOORMAT_THICKSIDE _T("BaseFloorThickSide")
#define FLOORMAT_OUTLINE _T("BaseFloorOutLine")

using namespace Ogre;
void CBaseFloor3DList::Update( const std::vector<CBaseFloor*>& vFlrList, C3DNodeObject& parentNode )
{
	
	{
		NodeList newList;
		for(int i=0;i<(int)vFlrList.size();i++)
		{
			CBaseFloor* pDeck = vFlrList.at(i);
			NodeTypePtr item = FindItem(pDeck);
			if(item.get())
			{
				item->Update(parentNode);
				newList.push_back(item);
			}
			else
			{
				C3DNodeObject newObj = parentNode.CreateNewChild();
				NodeTypePtr newItem( new CBaseFloor3D(newObj.GetSceneNode(),pDeck) );
				newItem->Update(parentNode);
				newList.push_back(newItem);
			}
		}
		mvNodeList = newList;
	}
}






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

void UpdateGridRegion(Ogre::ManualObject* pobj, CBaseFloor* pFloor)
{
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



void UpdateVisibleRegion(Ogre::ManualObject* pobj, CBaseFloor* pFloor,const String& topMat)
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
			std::reverse(vlist.begin(),vlist.end());			
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
	if(pFloor->IsMapValid() )
	{
		ARCVector2 vSize =  pFloor->m_vMapExtentsMax - pFloor->m_vMapExtentsMin;
		double dSizeX = vSize[VX]*pFloor->MapScale();
		double dSizeY = vSize[VY]*pFloor->MapScale();
		Vector3 offset(-pFloor->m_vOffset[VX]/dSizeX+0.5,-pFloor->m_vOffset[VY]/dSizeY+0.5,0);
		Vector3 scale(1.0/dSizeX,1.0/dSizeY,1.0);
		Quaternion quat;quat.FromAngleAxis(Degree(pFloor->MapRotation()),Vector3::UNIT_Z);
		transmat.makeTransform(offset,scale,quat);
	}
	double dThick = pFloor->Thickness();
	float c = ARCColor3::GREY_VISIBLEREGION_FLOAT;
	allMergeMesh.BuildMeshToObject(pobj,topMat,Vector3::UNIT_Z,ColourValue(c,c,c),0,transmat); //top
	allMergeMeshBack.BuildMeshToObject(pobj,FLOORMAT,-Vector3::UNIT_Z,ColourValue(c,c,c), -dThick); //bottom
	
	allMergeoutline.BuildSideToObject(pobj,FLOORMAT_THICKSIDE,ColourValue(c,c,c),-dThick);
	
	c = ARCColor3::GREYVALUE_VISREGIONOUTLINE_FLOAT;
	allMergeoutline.BuildOutlineToObject(pobj,FLOORMAT_OUTLINE,ColourValue(c,c,c), -dThick);

}
void UpdateMapRegion(Ogre::ManualObject* pobj, CBaseFloor* pFloor, const String& matName)
{
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

	DrawRectTopBottomToObject(pobj,v1,v2,v3,v4, dThick,matName);
	
	DrawRectSideToObject(pobj,vlist,dThick);				
	
	DrawRectOutLineToObject(pobj,vlist,dThick);		

	
}
//update deck region
void CBaseFloor3D::UpdateDeckRegion(bool bShowMap)
{
	BOOL bVisible = mpDeck->IsVisible() && mpDeck->IsOpaque();
	CString strID;
	strID.Format("Floor %d",(int)mpDeck);
	Ogre::ManualObject* pobj = OgreUtil::createOrRetrieveManualObject(strID,GetScene());
	pobj->clear();
	if(bVisible)
	{
		String topmat = FLOORMAT;
		if( mpDeck->IsShowMap() && bShowMap )
		{			
			topmat = mMapMat.GetString();
		}

		if(mpDeck->UseVisibleRegions())
		{
			UpdateVisibleRegion(pobj,mpDeck,topmat);
		}
		else if(mpDeck->IsMapValid())
		{
			UpdateMapRegion(pobj,mpDeck,topmat);
		}
		else
		{
			UpdateGridRegion(pobj,mpDeck);
		}
		pobj->setRenderQueueGroup(RenderFloors_Solid);
		AddObject(pobj);
	}	
	else
	{
		OgreUtil::DetachMovableObject(pobj);
	}
}

void CBaseFloor3D::UpdatePos()
{
	if(mpDeck)
	{		
		{
			SetIdentTransform();
			SetPosition(ARCVector3(0,0,mpDeck->Altitude()));
		}
	}
}

void CBaseFloor3D::Update(C3DNodeObject& parentNode)
{	
	if(mpDeck->IsVisible())
	{
		UpdateMaterial();
		UpdateDeckRegion();
		UpdatePos();
		UpdateGrid();
		UpdateMarkers();	
		AttachTo(parentNode);
	}
	else
		Detach();
}

//void CBaseFloor3D::Update( CBaseFloor* pDeck )
//{
//	mpDeck = pDeck;
//	Update();
//}

void CBaseFloor3D::UpdateMaterial()
{
	if(!mpDeck->IsCADLoaded())
	{
		if(mpDeck->LoadCAD())
			mpDeck->LoadCADLayers();
	}

	mMapMat = FLOORMAT;
	if (mpDeck->IsShowMap() && mpDeck->IsMapValid())
	{		
		CString strMat;
		strMat.Format( _T("FloorCadMat%d"),(int)mpDeck);
		mMapMat = strMat;

		TexturePtr texture = TextureManager::getSingleton().getByName(strMat.GetString());		
		if (!mpDeck->IsTextureValid() || texture.isNull())
		{
			strMat = C3DCADMapLayerList::GenMapTexture(mpDeck,strMat);
			ASSERT(mMapMat == strMat); // strMat should not be changed in C3DCADMapLayerList::GenMapTexture
			//update texture
			mpDeck->m_bIsTexValid = true;
	
			if( !MaterialManager::getSingleton().resourceExists(strMat.GetString()) )//create material once
			{
				MaterialPtr matPtrfloor = MaterialManager::getSingleton().getByName(FLOORMAT);
				ASSERT(!matPtrfloor.isNull());
				MaterialPtr mMapMatPtr = matPtrfloor->clone(strMat.GetString());
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
				pTexture->setTextureName(strMat.GetString());	
				pTexture->setColourOperation(LBO_ALPHA_BLEND);
				pTexture->setTextureAddressingMode(TextureUnitState::TAM_BORDER);
				pTexture->setTextureBorderColour(ColourValue(0,0,0,0));
			}			
		}
	}	
	
}

void CBaseFloor3D::UpdateMarkers()
{
	// 
	SceneManager* pScene = GetScene();

	// draw CBaseFloor
	if (!m_alignMarker1)
	{
		m_alignMarker1 = CreateNewChild().GetSceneNode();
		m_alignMarker1.Load(pScene);
	}
	if (!m_alignMarker2)
	{
		m_alignMarker2 = CreateNewChild().GetSceneNode();
		m_alignMarker2.Load(pScene);
	}
	if (mpDeck->UseAlignLine())
	{
		m_alignMarker1.SetPosition(ARCVector3(mpDeck->GetAlignLine().getBeginPoint(), 0.0));
		m_alignMarker1.AttachTo(*this);
		m_alignMarker2.SetPosition(ARCVector3(mpDeck->GetAlignLine().getEndPoint(), 0.0));
		m_alignMarker2.AttachTo(*this);
	}
	else if (mpDeck->UseMarker())
	{
		m_alignMarker1.SetPosition(ARCVector3(mpDeck->GetMarkerLocation(), 0.0));
		m_alignMarker1.AttachTo(*this);
		m_alignMarker2.Detach();
	}
	else
	{
		m_alignMarker1.Detach();
		m_alignMarker2.Detach();
	}
}

void CBaseFloor3D::UpdateGrid()
{
	BOOL bVisible = mpDeck->IsGridVisible();
	if(bVisible)
	{
		if (!mGrid)
		{
			mGrid = CGrid3D(CreateNewChild().GetSceneNode());
		}
		mGrid.Update(*mpDeck->GetGrid());
		mGrid.AttachTo(*this);		
	}	
	else
	{
		mGrid.Detach();
	}
	
}

void CBaseFloor3D::ShowGrid( BOOL b )
{
	if(b)
	{
		mGrid.AttachTo(*this);
	}
	else
	{
		mGrid.Detach();
	}
}
