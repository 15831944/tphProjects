#include "stdafx.h"
#include "LandsideTaxiPool3D.h"
#include "Tesselator.h"
#include "landside/ILandsideEditContext.h"
#include "./ShapeBuilder.h"
#include "CommonShapes.h"
#include "LandsideStretch3D.h"

#include "./Landside/LandsideEditCommand.h"
#include "ShapeBuilder.h"
#include "common/ARCPath.h"
#include "landside/LandsideQueryData.h"
#include ".\SceneNodeQueryData.h"
#include "LandIntersection3D.h"
#include "MaterialDef.h"
#include "SceneState.h"
#include "Render3DScene.h"
#include <common/ARCStringConvert.h>
#include "CommonShapes.h"
#include <common/Ray.h>

#define STRETCH_BASE_MAT "TaxiPool/RoadBase"
#define STRETCH_MARKLINE_MAT "Stretch/MarkLine"
// #define STRETCH_SIDEWALK_MAT "Stretch/SideWalk"
#define STRETCH_BASE_MAT_SEL "TaxiPool/RoadBaseTransparent"
#define PARK_AREA_MAT "Parkinglot/Area"
#define PARK_AREA_MAT_EDIT "parkingLot/AreaEdit"
#define FLOORMAT_OUTLINE _T("BaseFloorOutLine")

PROTYPE_INSTANCE(LandsideTaxiPool3D)
PROTYPE_INSTANCE(TaxiPoolArea3DNode)
PROTYPE_INSTANCE(TaxiPoolAreaControlPath3D)
PROTYPE_INSTANCE(TaxiPoolAreaControlPoint3D)

void BuildTaxiPoolPipe(LandsideTaxiPool* pPool,ManualObject* pObj,const CPath2008& path,BOOL InEditMode)
{
	static const double sc_dCenterLineTexBias = 0.1;

	double dLaneWidht = pPool->getLaneWidth();
	double dLaneNum = pPool->getLaneNum();

	double basewidth = dLaneNum*dLaneWidht;

	//draw base
	pObj->clear();
	ARCPipe pipe(path,basewidth);
	CommonShapes cShape3D(pObj);
	cShape3D.msMat = InEditMode?STRETCH_BASE_MAT_SEL:STRETCH_BASE_MAT;

	cShape3D.BuildTexturePipe2D(pipe,0.5 , 0.5  + dLaneNum);
	//draw side

	//draw mark
	const static double dmarkOffset = pPool->getLaneWidth()*0.5 + 200;
	const static double dmarklength = pPool->getLaneWidth();
	for(int i=0;i<pPool->getLaneNum();i++)
	{
		//LandsideLane& pLane = pPool->getLane(i);ii
		ARCPath3 lanepath;
		double dLanPos = (i+0.5)*pPool->getLaneWidth();
		LandsideStretch3D::GetPipeMidPath(pipe,lanepath,dLanPos);
		CString sMat = "Stretch/MarkC";
		if (true)
		{
			float idxPos = lanepath.GetDistIndex(lanepath.GetLength() - dmarkOffset);
			ARCVector3 vPos = lanepath.GetIndexPos(idxPos);
			ARCVector3 vDir = lanepath.GetIndexDir(idxPos);
			CommonShapes shapes(pObj);shapes.msMat =sMat;
			shapes.DrawSquare(vPos,vDir,pPool->getLaneWidth(),dmarklength);
		}
		else
		{
			float idxPos = lanepath.GetDistIndex(dmarkOffset);
			ARCVector3 vPos = lanepath.GetIndexPos(idxPos);
			ARCVector3 vDir = -lanepath.GetIndexDir(idxPos);
			CommonShapes shapes(pObj);shapes.msMat =sMat;
			shapes.DrawSquare(vPos,vDir,pPool->getLaneWidth(),dmarklength);
		}			
	}
	
}

void BuildTaxiPoolArea(ManualObject* pObj,const CPath2008& path,BOOL InEditMode)
{
	pObj->clear();
	CTesselator visTess;
	{
		ARCVectorListList Outline;
		ARCVectorList vlist= CTesselator::GetAntiClockVectorList(path);
		Outline.push_back(vlist);
		visTess.MakeOutLine(Outline,Vector3::UNIT_Z);
	}

	CTesselator allMergeoutline;
	CTesselator allMergeMesh;
	CTesselator allMergeMeshBack;
	{
		ARCVectorListList visOutline;
		for(int i=0;i<visTess.mCurMesh.getCount();i++)
		{
			visOutline.push_back(visTess.mCurMesh.getSection(i)->m_vPoints );
		}
		
		allMergeoutline.MakeOutLine(visOutline,Vector3::UNIT_Z);
		allMergeMesh.MakeMeshPositive(visOutline,Vector3::UNIT_Z);
		allMergeMeshBack.MakeMeshNegative(visOutline,-Vector3::UNIT_Z);
	}
	double dThick =  50;
	ColourValue regionColor(ARCColor3::GREY_VISIBLEREGION_FLOAT,ARCColor3::GREY_VISIBLEREGION_FLOAT,ARCColor3::GREY_VISIBLEREGION_FLOAT);
	
	allMergeMesh.BuildMeshToObject(pObj,InEditMode?PARK_AREA_MAT_EDIT:PARK_AREA_MAT,Vector3::UNIT_Z,regionColor,0); //top

	if(!InEditMode)
	{
		allMergeMeshBack.BuildMeshToObject(pObj,PARK_AREA_MAT,-Vector3::UNIT_Z,regionColor, -dThick); //bottom
		allMergeoutline.BuildSideToObject(pObj,PARK_AREA_MAT,regionColor,-dThick);
	}

	float c = 0.25;
	allMergeoutline.BuildOutlineToObject(pObj,FLOORMAT_OUTLINE,ColourValue(c,c,c), -dThick);
}

void ConvertPortInfoToVisual(StretchPortList& portlistEntry, ILandsideEditContext* pCtx, const CPoint2008& ptCenter)
{
	for(size_t i=0;i<portlistEntry.size();i++){
		StretchSideInfo& info = portlistEntry[i];
		pCtx->ConvertToVisualPos(info.mCenter);
		pCtx->ConvertToVisualPos(info.mLeft);
		pCtx->ConvertToVisualPos(info.mRight);
		info.mCenter= info.mCenter - ptCenter;
		info.mLeft= info.mLeft - ptCenter;
		info.mRight = info.mRight - ptCenter;
	}
}

void LandsideTaxiPool3D::OnUpdate3D()
{
	RemoveChild();
	ILayoutObject* pObject = getLayOutObject();
	if(pObject->GetType()!= ALT_LTAXIPOOL)
		return;
	LandsideTaxiPool* pTaxiPool = (LandsideTaxiPool*)pObject;
	
	CPoint2008 centerPt;
	if(!pTaxiPool->getCenter(centerPt))
		return;
	ILandsideEditContext* pCtx = GetLandsideEditContext();
	pCtx->ConvertToVisualPos(centerPt);

	//draw pipe
	CPath2008 path = pTaxiPool->getControlPath().getBuildPath();
	pCtx->ConvertToVisualPath(path);
	path.DoOffset(-centerPt.x,-centerPt.y,-centerPt.z);
	CString sPipe = GetIDName()+_T("Pipe");
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(sPipe,GetScene());
	BuildTaxiPoolPipe(pTaxiPool,pObj,path,IsInEditMode());

	//draw entry
	StretchPortList portlistEntry;
	if(_GetEntrySideList(portlistEntry))
	{
		ConvertPortInfoToVisual(portlistEntry,pCtx,centerPt);
		LandIntersection3D::DrawLinkNodes(portlistEntry,pObj,false,IsInEditMode());
	}

	//draw exit
	StretchPortList portlistExit;
	if(_GetExitSideList(portlistExit))
	{
		ConvertPortInfoToVisual(portlistExit,pCtx,centerPt);
		LandIntersection3D::DrawLinkNodes(portlistExit,pObj,false,IsInEditMode());
	}
	
	if(pTaxiPool->IsNewObject())
		AddObject(pObj,2);
	else
		AddObject(pObj,1);
	SetPosition(centerPt);

	//draw control path
	LandsideObjectControlPath3D ctrlpath = GetControlPath3D();
	if(IsInEditMode())
	{		
		ctrlpath.Update3D(pTaxiPool,pCtx,centerPt,path);
		ctrlpath.AttachTo(*this);
	}
	else 
		ctrlpath.Detach();

	//draw area
	if(pTaxiPool->getAreaPath().getCount()>0)
	{
		CString sTaxiPoolArea = GetIDName()+_T("AreaNode");
		TaxiPoolArea3DNode AreaNode = OgreUtil::createOrRetrieveSceneNode(sTaxiPoolArea,GetScene());
		AreaNode.SetQueryData(pTaxiPool);
		AreaNode.OnUpdate3D();
		AreaNode.AttachTo(*this);
		//AreaNode.SetScale(ARCVector3(100,100,100));
		//AreaNode.SetPosition(centerPt);
	}
}

LandsideTaxiPool* LandsideTaxiPool3D::getTaxiPool()
{
	if(SceneNodeQueryData *pData = getQueryData())
	{
		LandsideTaxiPool* pTaxiPool  = NULL;
		pData->GetData(KeyAltObjectPointer,(int&)pTaxiPool);
		return pTaxiPool;
	}
	return NULL;
	//return (LandsideTaxiPool*)getLayOutObject();
}

void LandsideTaxiPool3D::SetQueryData(LandsideTaxiPool* pTaxiPool)
{
	if (SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer,(int)pTaxiPool);
		pData->SetData(KeyLandsideItemType,_LayoutObject);
	}
}

LandsideObjectControlPath3D LandsideTaxiPool3D::GetControlPath3D()
{
	CString sControlPath = GetIDName() + "ControlPathNode";
	LandsideObjectControlPath3D path3d = OgreUtil::createOrRetrieveSceneNode(sControlPath,GetScene());
	path3d.Init(getTaxiPool());
	return path3d;
}

bool LandsideTaxiPool3D::_GetEntrySideList( StretchPortList& spanMap )
{
	LandsideTaxiPool* pPool = getTaxiPool();
	StretchSideInfo sideInof;
	if(!pPool->GetEntryStretch().GetInfo(sideInof))
		return false;
	
	StretchSideInfo poolside;
	if(!pPool->GetHeadInfo(poolside))
		return false;;
	
	spanMap.push_back(sideInof);
	spanMap.push_back(poolside);
	return true;
}

bool LandsideTaxiPool3D::_GetExitSideList( StretchPortList& spanMap )
{
	LandsideTaxiPool* pPool = getTaxiPool();
	StretchSideInfo sideInof;
	if(!pPool->GetExitStretch().GetInfo(sideInof))
		return false;

	StretchSideInfo poolside;
	if(!pPool->GetTrailInfo(poolside))
		return false;;

	spanMap.push_back(sideInof);
	spanMap.push_back(poolside);
	return true;
}

LandsideTaxiPool* LandsideTaxiPool3DItem::getPool()const
{
	if (SceneNodeQueryData* pData = getQueryData())
	{
		LandsideTaxiPool* pTaxiPool = NULL;
		pData->GetData(KeyAltObjectPointer,(int&)pTaxiPool);
		return pTaxiPool;
	}
	return NULL;
}

emLandsideItemType TaxiPoolArea3DNode::getType()const
{
	return _TaxiPoolArea;
}

void TaxiPoolArea3DNode::OnUpdate3D()
{
	LandsideTaxiPool* pTaxiPool = NULL;
	if (SceneNodeQueryData* pData = getQueryData())
	{
		if (!pData->GetData(KeyAltObjectPointer,(int&)pTaxiPool))
			return;

		if(!pTaxiPool)
			return;
		
		//CRenderScene3DNode node = GetParent().GetParent();
		//CPoint2008 ptC = CPoint2008(node.GetPosition());
		CPoint2008 ptC;
		pTaxiPool->getCenter(ptC);
		Build3D(pTaxiPool,ptC);
	}
}

void TaxiPoolArea3DNode::OnUpdateEditMode(BOOL b)
{
	SetEditMode(b);
	OnUpdate3D();
}

void TaxiPoolArea3DNode::OnLButtonDblClk( const MouseSceneInfo& mouseinfo )
{
	FlipEditMode();
}

void TaxiPoolArea3DNode::SetQueryData(LandsideTaxiPool* pTaxiPool)
{
	if (SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer,(int)pTaxiPool);
		pData->SetData(KeyLandsideItemType,_LayoutObject);
		pData->SetData(KeyTaxiPoolItemType,_TaxiPoolArea);
	}
}

void TaxiPoolArea3DNode::StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo)
{
	if (!IsInEditMode() || !IsDraggable())
	{
		return;
	}
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)	return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	SetInDragging(TRUE);
	if(LandsideTaxiPool* pTaxiPool = getPool())
	{		
		LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pTaxiPool,plandsideCtx->getInput());	
		plandsideCtx->StartEditOp(pModCmd);
		OnDrag(lastMouseInfo,curMouseInfo);
	}
}

void TaxiPoolArea3DNode::OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	if(LandsideTaxiPool* pTaxiPool = getPool())
	{
		Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;
		ControlPath& contolpath =pTaxiPool->getAreaPath();
		CPath2008& path = contolpath.getPath();
		path.DoOffset(mousediff.x,mousediff.y,0);
		pTaxiPool->getAreaPath().setPath(path);
		plandsideCtx->NotifyViewCurentOperation();
	}
}

void TaxiPoolArea3DNode::EndDrag()
{
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)return;	
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}

bool TaxiPoolArea3DNode::IsDraggable()const
{
	if(LandsideTaxiPool* pTaxiPool = getPool())
	{
		return !pTaxiPool->GetLocked();
	}
	return false;
}

void TaxiPoolArea3DNode::Build3D(LandsideTaxiPool* pTaxiPool,CPoint2008& ptCenter)
{
	ILandsideEditContext* pCtx = GetLandsideEditContext();
	if(!pCtx || !pTaxiPool)
		return;

	pCtx->ConvertToVisualPos(ptCenter);
	CPath2008& AreaPath = pTaxiPool->getAreaPath().getPath();
	pCtx->ConvertToVisualPath(AreaPath);
	AreaPath.DoOffset(-ptCenter.x,-ptCenter.y,-ptCenter.z-10);

	CString sTaxiPoolArea = GetIDName()+_T("Area");
	ManualObject* pAreaObj = OgreUtil::createOrRetrieveManualObject(sTaxiPoolArea,GetScene());
	BuildTaxiPoolArea(pAreaObj,AreaPath,IsInEditMode());
	if(pTaxiPool->IsNewObject())
		AddObject(pAreaObj,2);
	else
		AddObject(pAreaObj,1);
	//SetPosition(ptCenter);

	RemoveChild();
	if(IsInEditMode())
	{
		CString sAreaPath = GetIDName()+_T("AreaPathNode");
		TaxiPoolAreaControlPath3D ctrlpath3d = OgreUtil::createOrRetrieveSceneNode(sAreaPath,GetScene());
		ctrlpath3d.Update3D(pTaxiPool,AreaPath,ptCenter);
		ctrlpath3d.AttachTo(*this);
	}
}

CRenderScene3DNode TaxiPoolControlPath3D::GetControlPoint3D(int idx)
{
	CString sPt;
	sPt.Format("%sAreaPointNode%d",GetIDName(),idx);
	return OgreUtil::createOrRetrieveSceneNode(sPt,GetScene());
}

CPath2008& TaxiPoolControlPath3D::getPath(LandsideTaxiPool* pTaxiPool)
{
	if(SceneNodeQueryData *pData = getQueryData())
	{
		CPath2008 *path;
		pData->GetData(KeyTaxiPoolCtrlPath,(int&)path);
		return *path;
	}
	static CPath2008 path;
	return path;
}

void TaxiPoolControlPath3D::Update3D(LandsideTaxiPool* pTaxiPool, CPath2008& path, CPoint2008& ptCenter)
{
	//set data
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer, (int)pTaxiPool);
		pData->SetData(KeyLandsideItemType,getType());
		pData->SetData(KeyTaxiPoolCtrlPath,(int)&path);
	}
	ILandsideEditContext* pCtx = GetLandsideEditContext();
	if(!pCtx || !pTaxiPool)
		return;
	//draw pipe
	if(!ptCenter)
		return;
	pCtx->ConvertToVisualPos(ptCenter);
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());	

	pObj->clear();
	/*CPath2008 AreaPath = pTaxiPool->getAreaPath().getBuildPath();
	pCtx->ConvertToVisualPath(AreaPath);
	AreaPath.DoOffset(-ptCenter.x,-ptCenter.y,-ptCenter.z);*/

	CShapeBuilder::DrawPath(pObj,VERTEXCOLOR_LIGTHOFF,path,ARCColor3(255,0,0));
	AddObject(pObj,2);
	//update control points
	RemoveChild();
	UpdatePoints(pTaxiPool, path);
}

emLandsideItemType TaxiPoolAreaControlPath3D::getType()const
{
	return _TaxiPoolAreaPath;
}

void TaxiPoolAreaControlPath3D::UpdatePoints(LandsideTaxiPool* pTaxiPool, CPath2008& path)
{
	for(int i=0;i<path.getCount();i++)
	{		
		TaxiPoolAreaControlPoint3D ctrlPt = GetControlPoint3D(i);		
		ctrlPt.Update3D(pTaxiPool,i,path);
		ctrlPt.SetPosition(path[i]);
		ctrlPt.AttachTo(*this);
	}
}

void TaxiPoolAreaControlPath3D::OnRButtonDown( const MouseSceneInfo& mouseInfo )
{
	const Ray& sceneray = mouseInfo.mSceneRay;
	Point Origin = Point(sceneray.getOrigin().x,sceneray.getOrigin().y,sceneray.getOrigin().z );
	Point Direction = Point(sceneray.getDirection().x,sceneray.getDirection().y,sceneray.getDirection().z);
	ARCRay ray(Origin,Direction);
	if (SceneNodeQueryData* pData = InitQueryData())
	{
		const CPath2008& path2008 = getPath(getPool());
		Path path = path2008;
		DistanceRay3Path3 dsitraypath(ray,path);
		dsitraypath.GetSquared(0);
		double dPos =  dsitraypath.m_fPathParameter;	
		CPoint2008 worldpos = path2008.GetIndexPoint(dPos);	
		pData->SetData(KeyRClickWorldPos, ARCStringConvert::toString(ARCVector3(worldpos.x, worldpos.y, worldpos.z)));
	}
}

void TaxiPoolAreaControlPath3D::Update3D(LandsideTaxiPool* pTaxiPool,CPath2008& path, CPoint2008& ptCenter)
{
	if (SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer, (int)pTaxiPool);
		pData->SetData(KeyLandsideItemType,getType());
		pData->SetData(KeyTaxiPoolCtrlPath,(int)&path);
	}

	ILandsideEditContext* pCtx = GetLandsideEditContext();
	CString sAreaPath = GetIDName() + _T("AreaPath");
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(sAreaPath,GetScene());	
	pObj->clear();
	//CPath2008 path = getPath(pTaxiPool);
	/*pCtx->ConvertToVisualPath(path);
	path.DoOffset(-ptCenter.x,-ptCenter.y,0);*/
	//CShapeBuilder::DrawPath(pObj,VERTEXCOLOR_LIGTHOFF,path,ARCColor3(255,0,0));
	pObj->begin(VERTEXCOLOR_LIGTHOFF, Ogre::RenderOperation::OT_LINE_STRIP);
	for (int i = 0; i < path.getCount(); i++)
	{
		pObj->position(path[i].x, path[i].y, path[i].z);
		pObj->colour(Ogre::ColourValue(1.0, 0, 0));
	}
	pObj->position(path[0].x, path[0].y, path[0].z);
	pObj->colour(Ogre::ColourValue(1.0, 0, 0));
	pObj->end();
	AddObject(pObj,2);

	//update control points
	RemoveChild();
	UpdatePoints(pTaxiPool, path);
}

void TaxiPoolControlPoint3D::StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo)
{
	if (!IsDraggable())
	{
		return;
	}
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)	return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	SetInDragging(TRUE);


	if(LandsideTaxiPool* pTaxiPool = getPool())
	{		
		LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pTaxiPool,plandsideCtx->getInput());	
		plandsideCtx->StartEditOp(pModCmd);
		OnDrag(lastMouseInfo,curMouseInfo);
	}
}

void TaxiPoolControlPoint3D::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;


	if(LandsideTaxiPool* pTaxiPool = getPool())
	{		
		int nPtIdx = getPointIndex();
		//CPath2008& path = getPath(pTaxiPool);

		Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;
		CPath2008& path = pTaxiPool->getAreaPath().getPath();
		path[nPtIdx] = path[nPtIdx]  + CPoint2008(mousediff.x,mousediff.y,0);
		pTaxiPool->getAreaPath().setPath(path);

		CRenderScene3DNode node = GetParent().GetParent();
		node.OnUpdate3D();
		plandsideCtx->UpdateDrawing();	
	}
}

void TaxiPoolControlPoint3D::EndDrag()
{
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)
		return;	
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}

bool TaxiPoolControlPoint3D::IsDraggable()const
{
	if(LandsideTaxiPool* pTaxiPool = getPool())
	{
		return !pTaxiPool->GetLocked();
	}
	return false;
}

int TaxiPoolControlPoint3D::getPointIndex()const
{
	int ptidx = 0;
	if(SceneNodeQueryData*pData = getQueryData())
	{
		pData->GetData(KeyTaxiPoolCtrlPointIndex,ptidx);
	}
	return ptidx;
}

CPath2008& TaxiPoolControlPoint3D::getPath(LandsideTaxiPool* pTaxiPool)
{
	if(SceneNodeQueryData *pData = getQueryData())
	{
		CPath2008 *path;
		pData->GetData(KeyTaxiPoolCtrlPath,(int&)path);
		return *path;
	}
	static CPath2008 path;
	return path;
}

void TaxiPoolControlPoint3D::Update3D(LandsideTaxiPool* pTaxiPool,int iPtIdx,CPath2008& path)
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer, (int)pTaxiPool);
		pData->SetData(KeyLandsideItemType,getType());
		pData->SetData(KeyTaxiPoolCtrlPointIndex,iPtIdx);
		pData->SetData(KeyTaxiPoolCtrlPath,(int)&path);
	}
	CString sPt;
	sPt.Format(_T("%sAreaPoint%d"),GetIDName(),iPtIdx);
	Entity* pEnt =  OgreUtil::createOrRetrieveEntity(sPt,SphereMesh,GetScene());
	ARCColor3 color = ARCColor3(255,0,0);	
	pEnt->setMaterial(OgreUtil::createOrRetrieveColorMaterial(color));
	AddObject(pEnt,2);
}

emLandsideItemType TaxiPoolAreaControlPoint3D::getType()const
{
	return _TaxiPoolAreaPoint;
}