#include "StdAfx.h"
#include <common/ray.h>
#include "landside/Landside.h"
#include "LandsideWalkway3D.h"
#include "Landside/ILandsideEditContext.h"
#include "Landside/InputLandside.h"
#include "Landside/LandsideQueryData.h"
#include "./SceneNodeQueryData.h"
#include "Render3DScene.h"
#include "./ShapeBuilder.h"
#include <common/ARCStringConvert.h>
#include "MaterialDef.h"


#define MAT_WALKWAY "Walkway/Face"



//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(LandsideWalkway3D)

//void buildSideFromCenterPath(CPath2008& center, std::vector<double>& widthList, CPath2008& left, CPath2008& right, CPoint2008& centerPoint)
//{
//	if(center.getCount() < 2) return;
//
//	ARCVector3 dir1, dir2, ldir, rdir, offset;
//	double vcos, vsin;
//
//	int count = center.getCount();
//	left.init(count); right.init(count);
//	//first point
//	POINT2ARCVECTOR(dir1, center[1]-center[0]);
//	dir1.Normalize();
//	LEFTPERPENDICULAR(ldir, dir1);
//	ldir.Normalize();
//
//	POINTADDVECTOR(left[0], center[0] - centerPoint, ldir*(widthList[0] / 2));
//	POINTADDVECTOR(right[0], center[0] - centerPoint, -ldir*(widthList[0] / 2));
//
//	dir2 = dir1;
//	for (int i = 1; i < count - 1; i++)
//	{
//		dir1 = dir2;
//		POINT2ARCVECTOR(dir2, center[i+1] - center[i]);
//		dir2.Normalize();
//
//		vcos = (-dir1).dot(dir2);
//		double vsin_half = sqrt((1-vcos)/2);
//		if (vsin_half < ARCMath::EPSILON) vsin_half = ARCMath::EPSILON;
//		
//		vsin = ((-dir1)^dir2).z;
//		if (vsin > 0)
//			vsin_half = -vsin_half;
//		
//		double d = widthList[i] / 2;
//		double len = d / vsin_half;
//		offset = - dir1 + dir2;
//		offset.Normalize();
//		POINTADDVECTOR(left[i], center[i] - centerPoint, offset*len);
//		POINTADDVECTOR(right[i], center[i] - centerPoint, -offset*len);
//	}
//	//last point
//	LEFTPERPENDICULAR(ldir, dir2);
//	ldir.Normalize();
//	POINTADDVECTOR(left[count-1], center[count-1] - centerPoint, ldir*(widthList[count-1] / 2));
//	POINTADDVECTOR(right[count-1], center[count-1]- centerPoint, -ldir*(widthList[count-1] / 2));
//}

void LandsideWalkway3D::OnUpdate3D()
{
	LandsideWalkway* pWalkway = getWalkway();
	CPath2008& drawPath = pWalkway->getPath();
	std::vector<double>& widthList = pWalkway->getWidthList();
	ILayoutObjectDisplay::DspProp& dspShape = pWalkway->getDisplayProp().GetProp(ILayoutObjectDisplay::_Shape);
	ILandsideEditContext* pctx = GetLandsideEditContext();
	
	CPoint2008 centerPoint;
	if(!pWalkway->getCenter(centerPoint) )
		return;

	pWalkway->BuildSidePath(CPoint2008());

	//build face
	//buildSideFromCenterPath(drawPath, widthList, pWalkway->m_leftPath, pWalkway->m_rightPath, centerPoint);
	pctx->ConvertToVisualPos(centerPoint);
	CPath2008 centerPath = pWalkway->getPath();
	CPath2008 leftPath = pWalkway->m_leftPath;
	CPath2008 rightPath = pWalkway->m_rightPath;
	pctx->ConvertToVisualPath(leftPath);
	pctx->ConvertToVisualPath(rightPath);
	pctx->ConvertToVisualPath(centerPath);
	centerPath.DoOffset(-centerPoint);
	leftPath.DoOffset(-centerPoint);
	rightPath.DoOffset(-centerPoint);
	
	SetPosition(centerPoint);
	

	Ogre::ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(), GetScene());
	pObj->clear();
	OgrePainter painter;
	painter.msmat = MAT_WALKWAY;
	painter.UseColor(OGRECOLOR(dspShape.cColor,0.3f));
	CShapeBuilder::DrawPipe(pObj,leftPath,rightPath,painter);
	AddObject(pObj, 2);
	
	RemoveChild();
	//build rib path
	//UpdateRibPath3D(pWalkway);

	//build control path
	WalkwayControlPath3D ctrlPath3D = GetControlPath3D();
	if (IsInEditMode())
	{
		ctrlPath3D.Update3D(pWalkway, centerPath);
		ctrlPath3D.AttachTo(*this);
	}
	else
		ctrlPath3D.Detach();
	//UpdateControlPath3D(pWalkway, centerPoint);	

}


LandsideWalkway* LandsideWalkway3D::getWalkway()
{
	return (LandsideWalkway*)getLayOutObject();
}

void LandsideWalkway3D::StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo)
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideWalkway* pWalkway = getWalkway();	
	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(static_cast<LandsideFacilityLayoutObject*>(pWalkway), plandsideCtx->getInput());	
	plandsideCtx->StartEditOp(pModCmd);
	SetInDragging(TRUE);

	OnDrag(lastMouseInfo,curMouseInfo);
}

void LandsideWalkway3D::OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideWalkway* pWalkway = getWalkway();	
	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;

	pWalkway->DoOffset(ARCVector3(mousediff.x, mousediff.y, mousediff.z));

	plandsideCtx->NotifyViewCurentOperation();
}

void LandsideWalkway3D::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->NotifyViewCurentOperation();
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);

}



WalkwayRibPath3D LandsideWalkway3D::GetRibPath3D(int idx)
{
	CString sPathName;
	sPathName.Format("%s-%s-%d", GetIDName(),"RibPath",idx);
	return OgreUtil::createOrRetrieveSceneNode(sPathName.GetString(), GetScene());
}

void LandsideWalkway3D::UpdateRibPath3D(LandsideWalkway* pWalkway)
{
	//int count = pWalkway->getPath().getCount();
	//for (int i = 0; i < count; i++)
	//{
	//	WalkwayRibPath3D  rib3D = GetRibPath3D(i);
	//	if (IsInEditMode())
	//	{
	//		rib3D.Update3D(pWalkway, i);
	//		rib3D.AttachTo(*this);
	//	}
	//	else
	//		rib3D.Detach();
	//}
}

WalkwayControlPath3D LandsideWalkway3D::GetControlPath3D()
{
	CString sName;
	sName.Format("%s-%s", GetIDName(), "WalkwayControlPath");
	return OgreUtil::createOrRetrieveSceneNode(sName, GetScene());
}

void LandsideWalkway3D::UpdateControlPath3D(LandsideWalkway* pWalkway, CPoint2008& center)
{
		
}
//////////////////////////////////////////////////////////////////////////
void Walkway3DItem::SetQueryData( LandsideWalkway* pWalkway, int ipath = -1, int ipoint = -1)
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer, (int)pWalkway);	
		pData->SetData(KeyLandsideItemType, getType());
		if (ipath != -1)
			pData->SetData(KeyControlPathIndex, ipath);
		if(ipoint != -1)
			pData->SetData(KeyControlPointIndex, ipoint);
	}
}

LandsideWalkway* Walkway3DItem::getWalkway()
{
	if(SceneNodeQueryData *pData = getQueryData())
	{
		LandsideWalkway* pWalkway  = NULL;
		pData->GetData(KeyAltObjectPointer,(int&)pWalkway);
		return pWalkway;
	}
	return NULL;
}

void Walkway3DItem::OnUpdateEditMode(BOOL b)
{
	SetEditMode(b);
	OnUpdate3D();
}

void Walkway3DItem::StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo)
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideWalkway* pWalkway = getWalkway();	
	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(static_cast<LandsideFacilityLayoutObject*>(pWalkway), plandsideCtx->getInput());	
	plandsideCtx->StartEditOp(pModCmd);
	SetInDragging(TRUE);

	OnDrag(lastMouseInfo,curMouseInfo);
}

void Walkway3DItem::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->NotifyViewCurentOperation();
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}

bool Walkway3DItem::IsDraggable()const
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(WalkwayRibPath3D)

emLandsideItemType WalkwayRibPath3D::getType()
{
	return _WalkwayRibPath;
}

void WalkwayRibPath3D::Update3D(LandsideWalkway* pWalkway, int idx,const CPoint2008& leftPt,const CPoint2008& rightPt)
{
	SetQueryData(pWalkway, idx);

	CString sName;
	sName.Format("%s-%d", GetIDName(), idx);
	Ogre::ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(sName, GetScene());
	pObj->clear();

	CPath2008 tempPath;
	tempPath.init(2);
	tempPath[0] = pWalkway->m_leftPath[idx];
	tempPath[1] = pWalkway->m_rightPath[idx];
	//CShapeBuilder::DrawPath(pObj,VERTEXCOLOR_LIGTHOFF,tempPath,ARCColor3(255,0,0));
	Ogre::ColourValue c(1.0, 0, 0);
	pObj->begin(VERTEXCOLOR_LIGTHOFF, Ogre::RenderOperation::OT_LINE_STRIP);
	for (int i = 0; i < tempPath.getCount(); i++)
	{
		pObj->position(OGREVECTOR(tempPath[i]));
		pObj->colour(c);
	}
	pObj->end();
	AddObject(pObj, 2);

	RemoveChild();
	for (int i = 0; i < 2; i++)
	{
		WalkwayRibPoint3D ribPoint3D = GetRibPoint3D(idx, i);
		ribPoint3D.Update3D(pWalkway, idx, i);
		ribPoint3D.SetPosition(tempPath[i]);
		ribPoint3D.AttachTo(*this);
	}

}

WalkwayRibPoint3D WalkwayRibPath3D::GetRibPoint3D(int ipath, int ipoint)
{
	CString sPointName;
	sPointName.Format("%s-%d-%d", GetIDName(), ipath, ipoint);
	return OgreUtil::createOrRetrieveSceneNode(sPointName.GetString(), GetScene());
}

int WalkwayRibPath3D::GetRibPathIndex()
{
	if(SceneNodeQueryData *pData = getQueryData())
	{
		int ipath = -1;
		pData->GetData(KeyControlPathIndex,(int&)ipath);
		return ipath;
	}
	return NULL;
}

void WalkwayRibPath3D::OnRButtonDown(const MouseSceneInfo& mouseInfo)
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{

		pData->SetData(KeyRClickWorldPos, ARCStringConvert::toString(ARCVector3(mouseInfo.mWorldPos.x, mouseInfo.mWorldPos.y, mouseInfo.mWorldPos.z)));
	}
}

void WalkwayRibPath3D::OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideWalkway* pWalkway = getWalkway();	
	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;

	int ipath = GetRibPathIndex();
	pWalkway->MoveRibPathByPath(ARCVector3(mousediff.x, mousediff.y, mousediff.z), ipath);

	plandsideCtx->NotifyViewCurentOperation();
}

//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(WalkwayRibPoint3D)

emLandsideItemType WalkwayRibPoint3D::getType()
{
	return _WalkwayRibControlPoint;
}

int WalkwayRibPoint3D::GetPathAndPointIndex(int& ipath, int&ipoint)
{
	if(SceneNodeQueryData *pData = getQueryData())
	{
		pData->GetData(KeyControlPointIndex,ipoint);
		pData->GetData(KeyControlPathIndex, ipath);
	}
	return -1;
}

void WalkwayRibPoint3D::Update3D(LandsideWalkway* pWalkway, int ipath, int ipoint)
{
	SetQueryData(pWalkway, ipath, ipoint);
	
	CString sPointName;
	sPointName.Format("%s-%d-%d", GetIDName(), ipath, ipoint);
	Ogre::Entity* pEnt = OgreUtil::createOrRetrieveEntity(sPointName, SphereMesh, GetScene());
	pEnt->setMaterial(OgreUtil::createOrRetrieveColorMaterial(ARCColor3(255,191,0)));
	AddObject(pEnt, 2);
}

void WalkwayRibPoint3D::OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideWalkway* pWalkway = getWalkway();	
	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;

	int ipath = -1;
	int ipoint = -1;
	GetPathAndPointIndex(ipath, ipoint);
	ASSERT(ipath != -1 && ipoint != -1);
	pWalkway->MoveRibPathByPoint(ARCVector3(mousediff.x, mousediff.y, mousediff.z), ipath, ipoint);

	plandsideCtx->NotifyViewCurentOperation();
}

//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(WalkwayControlPath3D)

emLandsideItemType WalkwayControlPath3D::getType()
{
	return _WalkwayControlPath;
}

WalkwayControlPoint3D WalkwayControlPath3D::GetControlPoint(int ipoint)
{
	CString sName;
	sName.Format("%s-%d", GetIDName(), ipoint);
	return OgreUtil::createOrRetrieveSceneNode(sName, GetScene());
}

void WalkwayControlPath3D::Update3D(LandsideWalkway* pWalkway, const CPath2008& path)
{
	SetQueryData(pWalkway);

	Ogre::ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(), GetScene());
	pObj->clear();

	CShapeBuilder::DrawPath(pObj,VERTEXCOLOR_LIGTHOFF,path,ARCColor3(255,0,0));
	AddObject(pObj, 2);

	RemoveChild();
	for (int i = 0; i < path.getCount(); i++)
	{
		CPoint2008 relatePos= path[i];
		WalkwayControlPoint3D ctrlPoint3D = GetControlPoint(i);
		ctrlPoint3D.Update3D(pWalkway,i);
		ctrlPoint3D.SetPosition(relatePos);
		ctrlPoint3D.AttachTo(*this);
	}
}

void WalkwayControlPath3D::OnRButtonDown(const MouseSceneInfo& mouseInfo)
{
	LandsideWalkway* pWalkway = getWalkway();
	if(!pWalkway)return ;

	const Ray& sceneray = mouseInfo.mSceneRay;
	Point Origin = Point(sceneray.getOrigin().x,sceneray.getOrigin().y,sceneray.getOrigin().z );
	Point Direction = Point(sceneray.getDirection().x,sceneray.getDirection().y,sceneray.getDirection().z);
	ARCRay ray(Origin,Direction);

	CPath2008& drawPath = pWalkway->getPath();
	Path path = drawPath;
	DistanceRay3Path3 dsitraypath(ray,path);
	dsitraypath.GetSquared(0);
	double dPos =  dsitraypath.m_fPathParameter;	
	CPoint2008 pt = drawPath.GetIndexPoint(dPos);

	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyPosIndexInStretchBuildPath,dPos);
		pData->SetData(KeyRClickWorldPos, ARCStringConvert::toString(pt) );
	}
}


//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(WalkwayControlPoint3D)

emLandsideItemType WalkwayControlPoint3D::getType()
{
	return _WalkwayControlPoint;
}

void WalkwayControlPoint3D::SetQueryData(LandsideWalkway* pWalkway, int ipoint)
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer, (int)pWalkway);	
		pData->SetData(KeyLandsideItemType, getType());
		pData->SetData(KeyControlPointIndex, ipoint);
	}
}

void WalkwayControlPoint3D::Update3D(LandsideWalkway* pWalkway, int ipoint)
{
	SetQueryData(pWalkway,ipoint);

	CString sPointName;
	sPointName.Format("%s-%d", GetIDName(), ipoint);
	Ogre::Entity* pEnt = OgreUtil::createOrRetrieveEntity(sPointName, SphereMesh, GetScene());
	pEnt->setMaterial(OgreUtil::createOrRetrieveColorMaterial(ARCColor3(255,191,0)));
	AddObject(pEnt, 2);
}

int WalkwayControlPoint3D::GetPointIndex()
{
	if(SceneNodeQueryData *pData = getQueryData())
	{
		int ipoint = -1;
		pData->GetData(KeyControlPointIndex,(int&)ipoint);
		return ipoint;
	}
	return NULL;
}

void WalkwayControlPoint3D::OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideWalkway* pWalkway = getWalkway();	
	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;

	int ipoint = GetPointIndex();
	pWalkway->MoveRibPathByPath(ARCVector3(mousediff.x, mousediff.y, mousediff.z), ipoint);

	plandsideCtx->NotifyViewCurentOperation();
}









