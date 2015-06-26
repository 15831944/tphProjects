#include "StdAfx.h"
#include ".\simplemesh3d.h"
#include "3DScene.h"
#include "../../InputOnBoard\ComponentSection.h"
#include "../../InputOnBoard/ComponentModel.h"
#include "SimpleMeshEditScene.h"
#include <common/Guid.h>
#include "RenderEngine.h"
#include "OgreConvert.h"
#include <boost/tuple/tuple.hpp>
#include ".\MaterialDef.h"
#include "ogreutil.h"

using namespace Ogre;

//
void CMeshSection3D::Update(CComponentMeshSection* pSection)
{
	int nLod = pSection->getCreator()->GetLOD();
	CString Linename = mpNode->getName().c_str();
	ManualObject* m_pSectionLine = OgreUtil::createOrRetrieveManualObject(Linename,GetScene());

	m_pSectionLine->clear();
	{
		m_pSectionLine->begin( SECTIONMAT ,RenderOperation::OT_LINE_STRIP);
		m_pSectionLine->colour( ColourValue::Blue );
		//update content		
		for(int ptrIndex=0;ptrIndex<=nLod;++ptrIndex)
		{
			ARCVector2 pos = pSection->getPoint( (Real)ptrIndex/(Real)nLod );
			m_pSectionLine->position( 0, pos.x , pos.y );				
		}	
		m_pSectionLine->end();
	}
	//m
	{
		m_pSectionLine->begin(SECTIONMAT ,RenderOperation::OT_LINE_STRIP);
		m_pSectionLine->colour( ColourValue::Blue );
	
		for(int ptrIndex=0;ptrIndex<=nLod;++ptrIndex)
		{
			double dMidd = (Real)ptrIndex/(Real)nLod;
			ARCVector2 pos = pSection->getInnerPoint( dMidd );			
			m_pSectionLine->position( 0, pos.x , pos.y );				
		}		
		m_pSectionLine->end();
	}
	OgreUtil::AttachMovableObject(m_pSectionLine,mpNode);
	ARCVector2 vOffset = pSection->getOffset();
	SetPosition(ARCVector3( pSection->getPos(),vOffset[VX],vOffset[VY]) );
}



#include <common/ARCStringConvert.h>
CString CMeshSection3D::GetNodeName( CComponentMeshSection* pSection )
{
	return CString("Section") + ARCStringConvert::toString((int)pSection);
}

void CMeshSection3D::UpdatePos(CComponentMeshSection* pSection)
{
	ARCVector2 vOffset = pSection->getOffset();
	SetPosition(ARCVector3( pSection->getPos(),vOffset[VX],vOffset[VY]) );
}
//////////////////////////////////////////////////////////////////////////

int AddSegmentFace(const SegmentData& dataOut, ManualObject* pObj, int VertexOffset ,bool bBackFace )
{	
	int nRow = dataOut.getRow(); int nCol = dataOut.getCol();
	for(int j=0;j<nRow;j++)
	{
		for(int k=0;k<nCol;k++)
		{
			const SegmentData::Vertex& vertex = dataOut[j][k];
			pObj->position(vertex.pos[VX],vertex.pos[VY],vertex.pos[VZ]);
			pObj->normal(vertex.dir[VX],vertex.dir[VY],vertex.dir[VZ]);
		}
	}		
	for(int j=0;j<nRow-1;j++)
	{
		for(int k=0;k<nCol;k++)
		{
			int nBegin = k + j*nCol + VertexOffset;
			if(k==nCol-1)
			{
				bBackFace?pObj->quad(nBegin+nCol,nBegin,nBegin+1-nCol,nBegin+1):pObj->quad(nBegin,nBegin+nCol,nBegin+1,nBegin+1-nCol);
			}
			else{
				bBackFace?pObj->quad(nBegin+nCol,nBegin,nBegin+1,nBegin+nCol+1):pObj->quad(nBegin,nBegin+nCol,nBegin+nCol+1,nBegin+1);
			}
		}
	}
	return nRow * nCol;
}
//add thickness face front
int AddFrontFace(const SegmentData& dataOut,const SegmentData& dataIn, ManualObject* pObj, int VertexOffset )
{
	int nRow = dataOut.getRow(); int nCol = dataOut.getCol();
	if( nRow && nCol )
	{
		for(int k=0;k<nCol;k++)
		{
			const SegmentData::Vertex& vertexIn = dataIn[0][k];
			pObj->position( OGREVECTOR(vertexIn.pos) );
			pObj->normal(1,0,0);
		}
		for(int k=0;k<nCol;k++)
		{
			const SegmentData::Vertex& vertexOut = dataOut[0][k];
			pObj->position( OGREVECTOR(vertexOut.pos) );
			pObj->normal(1,0,0);
		}
		for(int k=0;k<nCol;k++)
		{
			int nBegin = k + VertexOffset;
			if(k==nCol-1)
			{
				pObj->quad(nBegin,nBegin+nCol,nBegin+1,nBegin+1-nCol);
			}
			else{
				pObj->quad(nBegin,nBegin+nCol,nBegin+nCol+1,nBegin+1);
			}
		}
	}
	return 2*nCol;
}
//add thickness face back
int AddBackFace(const SegmentData& dataOut,const SegmentData& dataIn, ManualObject* pObj, int VertexOffset)
{
	int nRow = dataOut.getRow(); int nCol = dataOut.getCol();
	if( nRow && nCol )
	{
		for(int k=0;k<nCol;k++)
		{
			const SegmentData::Vertex& vertexIn = dataIn[nRow-1][k];
			pObj->position( OGREVECTOR(vertexIn.pos) );
			pObj->normal(-1,0,0);
		}
		for(int k=0;k<nCol;k++)
		{
			const SegmentData::Vertex& vertexOut = dataOut[nRow-1][k];
			pObj->position( OGREVECTOR(vertexOut.pos) );
			pObj->normal(-1,0,0);
		}
		for(int k=0;k<nCol;k++)
		{
			int nBegin = k + VertexOffset;
			if(k==nCol-1)
			{
				pObj->quad(nBegin+nCol,nBegin,nBegin+1-nCol,nBegin+1);
			}
			else{
				pObj->quad(nBegin+nCol,nBegin,nBegin+1,nBegin+nCol+1);
			}
		}
	}
	return 2*nCol;
}

void CSimpleMesh3D::Update(const CString& strMat)
{
	MovableObject* pAttachObj = NULL;
	CComponentMeshModel* pModeData = m_pModeData;

	CString objName = GetIDName();	
	{
		ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(objName, GetScene());
		pAttachObj = pObj;
		pObj->clear();

		//pObj->estimateIndexCount(600);
		//pObj->estimateVertexCount(200);
		//pObj->setDynamic(true);

		//build 
		pModeData->Update();
		
		const std::vector<SegmentData>& OutSegmentDataList = pModeData->mvSegmentData;
		const std::vector<SegmentData>& innerSegmetDataList = pModeData->mvSegmentDataInner;
		int nSegmentCount = (int)OutSegmentDataList.size();
		for(int i=0;i<nSegmentCount;i++)
		{
			const SegmentData& dataOut = OutSegmentDataList[i];
			const SegmentData& dataIn = innerSegmetDataList[i];
			pObj->begin(strMat.GetString(),RenderOperation::OT_TRIANGLE_LIST);
			int vertexAdd = 0;

			//add front face
			if(i==0 && pModeData->IsEnableThickness())
			{
				vertexAdd += AddFrontFace(dataOut,dataIn,pObj,vertexAdd);
			}
			vertexAdd += AddSegmentFace(dataOut,pObj,vertexAdd,false);
			if(pModeData->IsEnableThickness())
			{
				const SegmentData& dataInner = innerSegmetDataList[i];
				vertexAdd+= AddSegmentFace(dataInner,pObj,vertexAdd,true);
			}
			//add back face
			if(i==nSegmentCount-1 && pModeData->IsEnableThickness())
			{
				vertexAdd+= AddBackFace(dataOut,dataIn,pObj,vertexAdd);
			}
			pObj->end();	
		}
	}
	OgreUtil::AttachMovableObject(pAttachObj,mpNode);

}


void CSimpleMesh3D::OnMouseMove( const Ogre::Ray& mouseRay,Ogre::MovableObject* pObj )
{		
	//ManualObject* pManulObj = OgreUtil::createOrRetrieveManualObject(mpNode->getName().c_str(), GetScene());
	/*if(HasMovableObj(pObj,true))
	{
		OgreUtil::SetObjectMaterial(pManulObj,MESH_MAT_B);
	}
	else
	{
		OgreUtil::SetObjectMaterial(pManulObj,MESHMAT);
	}*/
}


void CSimpleMesh3D::SaveMesh()
{	
	CComponentMeshModel* pModeData = m_pModeData;
	CString meshName = pModeData->GetMeshFileName();
	CString meshFilePath = pModeData->msTmpWorkDir + pModeData->GetMeshFileName();	
	
	if(!pModeData->mbExternalMesh)
	{
		try
		{
			ManualObject* pManulObj = OgreUtil::createOrRetrieveManualObject(mpNode->getName().c_str(), GetScene());			
			MeshPtr pmeshp = pManulObj->convertToMesh(meshName.GetString());

			Ogre::MeshSerializer meshSer;
			meshSer.exportMesh(pmeshp.getPointer(), meshFilePath.GetString() );
			Ogre::MeshManager::getSingleton().remove(pmeshp->getHandle());
		}
		catch (Ogre::Exception&)
		{
		}	
	}
	else
	{
		/*try
		{
			MeshPtr pmeshp = MeshManager::getSingleton().getByName(pModeData->msMeshFileName.GetString() );
			MeshSerializer meshSer;
			meshSer.exportMesh(pmeshp.getPointer(), meshFilePath.GetString() );
		}*/
		/*catch (Ogre::Exception&)
		{
			
		}*/
	}


	
	
}

void CSimpleMesh3D::SetMaterial( const CString& str )
{
	if(!m_pModeData)return;
	if(!mpNode)return;

	if(m_pModeData->mbExternalMesh) //use external mesh
	{
		Entity* pEnt = OgreUtil::createOrRetrieveEntity(GetIDName(), m_pModeData->GetMeshFileName() ,GetScene());
		pEnt->setMaterialName(str.GetString());		
	}
	else
	{
		C3DNodeObject::SetMaterial(str);
	}

}

void CSimpleMesh3D::SaveThumbnail()
{
	SnapShot(m_pModeData->msTmpWorkDir);
}

CSimpleMesh3D::CSimpleMesh3D( CComponentMeshModel* pModeData,Ogre::SceneNode* pNode ) :C3DNodeObject(pNode)
{
	m_pModeData = pModeData;
}
CSectionPos3DController::~CSectionPos3DController()
{
	mSection3D.Detach();
	mSectionPos.Detach();	
}

void CSectionPos3DController::Update( C3DSceneBase& scene )
{
	CString secName =  CMeshSection3D::GetNodeName(mpSection);
	mSection3D = scene.GetSceneNode( secName );
	scene.AddSceneNodeTo(secName,MeshEditCenterLineName);
	mSection3D.Update(mpSection);
	mSection3D.SetVisibleFlag(NormalVisibleFlag);

	CString ctrlPtName = secName + _T("ctrlpoint");
	C3DNodeObject mCtrlPoint3D = scene.GetSceneNode( ctrlPtName );
	Entity* pEnt = OgreUtil::createOrRetrieveEntity(ctrlPtName,SphereMesh,mCtrlPoint3D.GetScene() );
	mCtrlPoint3D.AddObject(pEnt);
	mCtrlPoint3D.SetVisibleFlag(NormalVisibleFlag);	
	mCtrlPoint3D.EnableSelect(true);
	scene.EnableAutoScale(ctrlPtName,15);
	
	mSectionPos = scene.AddSceneNode(secName + _T("posNode"));
	mSectionPos.SetPosition( ARCVector3(mpSection->getPos(),0,0) );
	
	mCtrlPoint3D.AttachTo(mSectionPos);
	

}

void CSectionPos3DController::OnMouseMove( const Ogre::Ray& rayFrom, const Ogre::Ray& rayTo )
{
	//fix the move in  
	Vector3 focusPlandir  = (rayTo.getDirection().crossProduct(Vector3::UNIT_X).crossProduct(Vector3::UNIT_X) );
	if(focusPlandir.isZeroLength()){ focusPlandir = rayTo.getDirection(); }
	focusPlandir.normalise();
	Plane focusPlan(focusPlandir,Vector3::ZERO);
	bool bIntersect = false;
	Real dDist = 0;
	boost::tie(bIntersect,dDist) = rayTo.intersects(focusPlan);
	if(bIntersect)
	{
		Vector3 pos = rayTo.getPoint(dDist);
		mpSection->setPos(pos.x);
	}


}

void CSectionPos3DController::UpdatePos()
{
	mSection3D.UpdatePos(mpSection);
	mSectionPos.SetPosition(ARCVector3(mpSection->getPos(),0,0));
}

void CSectionPos3DController::UpdateShape()
{
	mSection3D.Update(mpSection);
}

void CSection3DControllerList::Update( CComponentMeshModel* pModel, C3DSceneBase& scene)
{
	DataList newList;
	for(int i=0;i<pModel->GetSectionCount();i++)
	{
		DataItem item  = FindItem(pModel->GetSection(i));
		if(item.get())
		{
			newList.push_back(item);
		}
		else
		{
			CSectionPos3DController* pnewData = new CSectionPos3DController(pModel->GetSection(i));
			newList.push_back( DataItem(pnewData) );
			pnewData->Update(scene);
		}
	}
	mvSectionlist = newList;
}

CSection3DControllerList::DataItem CSection3DControllerList::FindItem( CComponentMeshSection* pSection )
{
	for(int i=0;i<(int)mvSectionlist.size();i++)
	{
		DataItem& item = mvSectionlist[i];
		if(item.get() && item->mpSection == pSection )
		{
			return item;
		}		
	}
	return DataItem((CSectionPos3DController*)NULL);
}

void CSection3DControllerList::RemoveItem( CComponentMeshSection* pSection )
{
	for(DataIterator itr = mvSectionlist.begin();itr!=mvSectionlist.end();itr++)
	{
		DataItem& item = *itr;
		if(item->mpSection == pSection)
		{
			mvSectionlist.erase(itr);
			return;
		}
	}
}

CSectionPos3DController* CSection3DControllerList::IsMouseOn( MovableObject* pObj )
{
	for(DataIterator itr = mvSectionlist.begin();itr!=mvSectionlist.end();itr++)
	{
		DataItem& item = *itr;
		if(item->mSectionPos.HasMovableObj(pObj))
			return item.get();
	}
	return NULL;
}

void CSection3DControllerList::UpdateAllShape()
{
	for(DataIterator itr = mvSectionlist.begin();itr!=mvSectionlist.end();itr++)
	{
		DataItem& item = *itr;
		item->UpdateShape();			
	}
}

void CSection3DControllerList::ActiveItem( CComponentMeshSection* pSection )
{
	for(int i=0;i<(int)mvSectionlist.size();i++)
	{
		DataItem& item = mvSectionlist[i];
		if(item->mpSection == pSection)
		{
			item->mSectionPos.SetMaterial(STR_MESHNODE_ACTIVE_COLOR);
			item->mSection3D.SetMaterial(STR_ACTIVE_LINE,false);
		}
		else
		{
			item->mSectionPos.SetMaterial(STR_MESHNODE_INACTIVE_COLOR);
			item->mSection3D.SetMaterial(STR_INACTIVE_LINE,false);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CSectionDetailController::Update( C3DSceneBase& scene )
{	
	//CString centerNodeName  = _T("SectionCenterNode");
	//mSectionCenterCtrl = scene.GetSceneNode(centerNodeName);
	//mSectionCenterCtrl.AttachTo(*this);	
	//Entity* pEnt = OgreUtil::createOrRetrieveEntity(centerNodeName,SphereMesh,GetScene() );
	//mSectionCenterCtrl.AddObject(pEnt);
	//mSectionCenterCtrl.SetVisibleFlag(NormalVisibleFlag);
	//mSectionCenterCtrl.EnableSelect(true);
	//scene.EnableAutoScale(centerNodeName,15);

	//update control points
	mctrlPtsList.clear();
	int nCtrlPtCnt = mpSection->GetCtrlPtCount();
	for(int i=0;i<nCtrlPtCnt;i++)
	{
		CString sName;sName.Format(_T("ControlPoint%d"),i); 
		AutoDetachNodeObject* pNewNode = new AutoDetachNodeObject(scene.GetSceneNode(sName)); 
		Entity* pEnt = OgreUtil::createOrRetrieveEntity(sName,SphereMesh,GetScene() );		
		pNewNode->AddObject(pEnt);
		pNewNode->SetVisibleFlag(NormalVisibleFlag);		
		pNewNode->AttachTo(*this);
		pNewNode->EnableSelect(true);
		pEnt->setMaterialName(STR_MESHNODE_ACTIVE_COLOR);

		ARCVector2 vPos = mpSection->GetCtrlPoint(i);
		pNewNode->SetPosition( ARCVector3(0,vPos[VX],vPos[VY]) );
		mctrlPtsList.push_back( AutoDetachNodeObject::SharedPtr(pNewNode) );
		scene.EnableAutoScale(sName,15);

	}
	//update thickness control points
	//mThicknessCtrlList.clear();
	//for(int i=0;i<nCtrlPtCnt;i++)
	//{
	//	CString sName;sName.Format(_T("ThicknessControlPoint%d"),i); 
	//	AutoDetachNodeObject* pNewNode =new AutoDetachNodeObject(scene.GetSceneNode(sName)); 
	//	Entity* pEnt = OgreUtil::createOrRetrieveEntity(sName,SphereMesh,GetScene() );		
	//	pNewNode->AddObject(pEnt);
	//	pNewNode->SetVisibleFlag(NormalVisibleFlag);	
	//	pNewNode->AttachTo(*this);
	//	ARCVector2 vPos =  mpSection->getInnerPoint( (double)i/nCtrlPtCnt );//mpSection->getCtrlPoint(i);
	//	pNewNode->SetPosition( ARCVector3(0,vPos[VX],vPos[VY]) );
	//	pNewNode->EnableSelect(true);

	//	mThicknessCtrlList.push_back( AutoDetachNodeObject::SharedPtr(pNewNode) );
	//	scene.EnableAutoScale(sName,15);
	//} 

}

void CSectionDetailController::Update()
{
	//update control points
	int nCtrlPtCnt = (int)mctrlPtsList.size();
	for(int i=0;i<nCtrlPtCnt;i++)
	{		
		{
			ARCVector2 vPos = mpSection->GetCtrlPoint(i);	
			AutoDetachNodeObject::SharedPtr& item = mctrlPtsList[i];
			item->SetPosition( ARCVector3(0,vPos[VX],vPos[VY]) );
		}
		{
			//ARCVector2 vPos =  mpSection->getInnerPoint( (double)i/nCtrlPtCnt );
			//AutoDetachNodeObject::SharedPtr& item = mThicknessCtrlList[i];
			//item->SetPosition( ARCVector3(0,vPos[VX],vPos[VY]) );
		}
	}

}
CSectionDetailController::CSectionDetailController( CComponentMeshSection* pSection, Ogre::SceneNode* pNode) :C3DNodeObject(pNode)
{
	mpSection = pSection;
}

bool CSectionDetailController::IsCenterCtrl( Ogre::MovableObject* pObj )
{
    return mSectionCenterCtrl.HasMovableObj(pObj);
}

bool CSectionDetailController::IsSectionCtrl( Ogre::MovableObject* pObj, int& idx )
{
	for(int i=0;i<(int)mctrlPtsList.size();i++)
	{
		AutoDetachNodeObject::SharedPtr& item = mctrlPtsList[i];
		if(item->HasMovableObj(pObj))
		{
			idx = i;
			return true;
		}
	}
	return false;
}

bool CSectionDetailController::IsThickNessCtrl( Ogre::MovableObject* pObj,int& idx )
{
	//for(int i=0;i<(int)mThicknessCtrlList.size();i++)
	//{
	//	AutoDetachNodeObject::SharedPtr& item = mThicknessCtrlList[i];
	//	if(item->HasMovableObj(pObj))
	//	{
	//		idx = i;
	//		return true;
	//	}
	//}
	return false;
}

void CSectionDetailController::OnMouseMoveCenter( const Ogre::Ray& rayFrom, const Ogre::Ray& rayTo )
{
	Plane movePlan(Vector3::UNIT_X, Vector3(mpSection->getPos(),0,0) );
	bool b1,b2;
	Real d1,d2;
	boost::tie(b1,d1) = rayFrom.intersects(movePlan);
	boost::tie(b2,d2) = rayTo.intersects(movePlan);
	if(b1 && b2)
	{
		Vector3 offset = -rayFrom.getPoint(d1) + rayTo.getPoint(d2);
		if(mpSection)
		{
			mpSection->mCenter += ARCVector2(offset.y,offset.z);
		}
	}
}

void CSectionDetailController::OnMouseMoveCtrlPt( int idx,const Ogre::Ray& rayFrom, const Ogre::Ray& rayTo )
{
	Plane movePlan(Vector3::UNIT_X, Vector3(mpSection->getPos(),0,0) );
	bool b1,b2;
	double d1,d2;
	boost::tie(b1,d1) = rayFrom.intersects(movePlan);
	boost::tie(b2,d2) = rayTo.intersects(movePlan);
	if(b1 && b2)
	{
		Vector3 offset = -rayFrom.getPoint(d1) + rayTo.getPoint(d2);
		if(mpSection)
		{
			mpSection->SetCtrlPoint(idx,mpSection->GetCtrlPoint(idx)+ARCVector2(offset.y,offset.z));
		}
	}
}

void CSectionDetailController::OnMouseMoveThickness( int idx, const Ogre::Ray& rayFrom, const Ogre::Ray& rayTo )
{
	Plane movePlan(Vector3::UNIT_X, Vector3(mpSection->getPos(),0,0) );
	bool b1,b2;
	Real d1,d2;
	boost::tie(b1,d1) = rayFrom.intersects(movePlan);
	boost::tie(b2,d2) = rayTo.intersects(movePlan);
	if(b1 && b2)
	{
		Vector3 offset = -rayFrom.getPoint(d1) + rayTo.getPoint(d2);
		if(mpSection)
		{
			ARCVector2 voffset = ARCVector2(offset.y,offset.z);
			double dSign = mpSection->getDir( double(idx)/mpSection->GetCtrlPtCount() )*voffset>=0?1:-1;
			mpSection->SetThickness(idx,mpSection->GetThickness(idx)+dSign*voffset.Length());
		}
	}
}


//////////////////////////////////////////////////////////////////////////
void CCenterLine3D::Update( CComponentMeshModel* pMeshModel,C3DSceneBase& scene )
{
	ASSERT(pMeshModel);
	if(!mpNode)
		return;

	//build line
	{
		Real dFrom = -5000;
		Real dTo = 5000;
		if(pMeshModel)
		{
			dFrom = -pMeshModel->mGrid.dSizeX;
			dTo = -dFrom;
		}
		CString lineName = mpNode->getName().c_str();
		lineName += _T("lineX");
		ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(lineName,GetScene());
		pObj->clear();	
		pObj->begin( STR_CENTER_LINE_MAT ,RenderOperation::OT_LINE_STRIP);
		pObj->colour(Ogre::ColourValue::Blue);
		pObj->position(dFrom,0,0);
		pObj->position(dTo,0,0);
		pObj->end();
		AddObject(pObj);
		pObj->setVisibilityFlags(PERSP_VisibleFlag | XZVisibleFlag);

	}

	///build other line 
	{
		Real dFrom = -5000;
		Real dTo = 5000;
		if(pMeshModel)
		{
			dFrom = -pMeshModel->mGrid.dSizeY;
			dTo = -dFrom;
		}
		CString lineName = mpNode->getName().c_str();
		lineName += _T("lineY");
		ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(lineName,GetScene());
		pObj->clear();	
		pObj->begin( STR_CENTER_LINE_MAT ,RenderOperation::OT_LINE_STRIP);
		pObj->colour(Ogre::ColourValue::Blue);
		pObj->position(0,dFrom,0);
		pObj->position(0,dTo,0);
		pObj->end();
		AddObject(pObj);
		pObj->setVisibilityFlags(YZVisibleFlag);
	}

	{
		Real dTo = 5000;
		if(pMeshModel)
		{			
			dTo = pMeshModel->mGrid.dSizeX;
		}
		Entity* pEnt = OgreUtil::createOrRetrieveEntity(mCtrlPt.GetIDName(),SphereMesh,GetScene() );
		mCtrlPt.AddObject(pEnt);
		mCtrlPt.AttachTo(*this);
		mCtrlPt.EnableSelect(true);
		scene.EnableAutoScale(mCtrlPt.GetIDName(),25);
		mCtrlPt.SetVisibleFlag(PERSP_VisibleFlag | XZVisibleFlag);
		mCtrlPt.SetPosition( ARCVector3(dTo,0,0) );
	}


	double dHeight = 5000;
	if(pMeshModel)dHeight = pMeshModel->getHeight();
	SetPosition(ARCVector3(0,0,dHeight));

}

void CCenterLine3D::OnMove(const Ogre::Ray& rayFrom, const Ogre::Ray& rayTo  )
{
	Vector3 focusPlandir  = (rayTo.getDirection().crossProduct(Vector3::UNIT_Z).crossProduct(Vector3::UNIT_Z) );
	if(focusPlandir.isZeroLength()){ focusPlandir = rayTo.getDirection(); }
	focusPlandir.normalise();
	Plane focusPlan( focusPlandir, OGREVECTOR(mCtrlPt.GetPosition()) );
	bool bIntersect = false;
	double dDist = 0;
	boost::tie(bIntersect,dDist) = rayTo.intersects(focusPlan);
	if(bIntersect)
	{
		Vector3 pos = rayTo.getPoint(dDist);
		mpNode->setPosition(0,0,pos.z);
	}	
}





CCenterLine3D::CCenterLine3D( Ogre::SceneNode* pNode ):C3DNodeObject(pNode)
{
	CString controlPt =  mpNode->getName().c_str();
	controlPt+= _T("ControlPoint");
	mCtrlPt = OgreUtil::createOrRetrieveSceneNode(controlPt,GetScene());		
}

void CCenterLine3D::UpdatePos(CComponentMeshModel* pMeshModel)
{
	if(!mpNode)return;
	double z = pMeshModel->getHeight();
	mpNode->setPosition(0,0,z);
}