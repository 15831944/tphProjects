#include "stdafx.h"
#include "landside/Landside.h"
#include "LandsideBusStation3D.h"
#include "Landside/ILandsideEditContext.h"
#include "Landside/InputLandside.h"
#include "Landside/LandsideQueryData.h"
#include "./SceneNodeQueryData.h"
#include "Render3DScene.h"
#include "./ShapeBuilder.h"
#include "common/ARCStringConvert.h"
#include "MaterialDef.h"
#include "CommonShapes.h"
#include "LandsideLinkStretchArea.h"

#define MAT_LANDSIDE_BUS_STATION_WAITING "BusStation/WaitingPath"
#define MAT_LANDSIDE_BUS_STATION "BusStation/Face"
#define MAT_LANDSIDE_BUS_STATION_PARKING "BusStation/ParkingArea"
#define MESH_LANDSIDE_BUS_STATION_CYLINDER "cylinder01.mesh"
#define MAT_LANDSIDE_BUS_STATION_BANK "BusStation/Pan"
#define MESH_LANDSIDE_BUS_STATION_BOX "box01.mesh"

//////////////////////////////////////////////////////////////////////////

static void BuildBusStation(CPath2008& path, CPoint2008& center, LandsideBusStation3D* station)
{
	path.DoOffset(-center.x, -center.y, 0.0);
	ARCPipe pipe(path, 200);
	Ogre::ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(station->GetIDName(), station->GetScene());
	pObj->clear();
	ProcessorBuilder::DrawColorPath(pObj, pipe, MAT_LANDSIDE_BUS_STATION_WAITING, ColourValue(0.10,0.20,0.10));
	station->AddObject(pObj, 2);

	//draw cylinder
	CString sName;
	ARCVector3 adir(path[1]-path[0]);
	Ogre::Vector3 dir = OGREVECTOR(adir);
	ARCVector3 pdir(dir.y, -dir.x, dir.z);
	pdir.Normalize();

	sName.Format("%s-left_cylinder", station->GetIDName());
	Ogre::Entity* leftcylinder = OgreUtil::createOrRetrieveEntity(sName, MESH_LANDSIDE_BUS_STATION_CYLINDER, station->GetScene());
	leftcylinder->setMaterial(OgreUtil::createOrRetrieveColorMaterial(ARCColor4(147,175,157,190)));
	C3DNodeObject leftNode = OgreUtil::createOrRetrieveSceneNode(sName, station->GetScene());	
	ARCVector3 scaleCylinder(3, 3, 10);
	leftNode.SetScale(scaleCylinder);
	leftNode.SetPosition(pipe.m_centerPath[0]+adir/4);
	leftNode.AddObject(leftcylinder);
	leftNode.AttachTo(*station);

	sName.Format("%s-right_cylinder", station->GetIDName());
	Ogre::Entity* rightcylinder = OgreUtil::createOrRetrieveEntity(sName, MESH_LANDSIDE_BUS_STATION_CYLINDER, station->GetScene());
	rightcylinder->setMaterial(OgreUtil::createOrRetrieveColorMaterial(ARCColor4(147,191,157,190)));
	C3DNodeObject rightNode = OgreUtil::createOrRetrieveSceneNode(sName, station->GetScene());	
	rightNode.SetScale(scaleCylinder);
	rightNode.SetPosition(pipe.m_centerPath[1]-adir/4);
	rightNode.AddObject(rightcylinder);
	rightNode.AttachTo(*station);

	sName.Format("%s-box", station->GetIDName());
	Ogre::Entity* box = OgreUtil::createOrRetrieveEntity(sName, MESH_LANDSIDE_BUS_STATION_BOX, station->GetScene());
	box->setMaterial(OgreUtil::createOrRetrieveColorMaterial(ARCColor4(67,65,129, 190)));
	C3DNodeObject boxNode = OgreUtil::createOrRetrieveSceneNode(sName, station->GetScene());	

	Ogre::Vector3 boxSize = box->getBoundingBox().getSize();
	ARCVector3 scaleBox((40+dir.length())/boxSize.x, 40/boxSize.y, 200/boxSize.z);
	boxNode.SetScale(scaleBox);
	boxNode.SetPosition((pipe.m_centerPath[1]+pipe.m_centerPath[0])/2 + ARCVector3(0, 0, 200) + pdir*20);
	boxNode.SetDirection(adir,ARCVector3(0,0,1));
	boxNode.AddObject(box);
	boxNode.AttachTo(*station);
	
	//draw pan
	//pObj->begin(MAT_LANDSIDE_BUS_STATION_BANK, Ogre::RenderOperation::OT_TRIANGLE_LIST);
	//	Ogre::Vector3 dir = OGREVECTOR(path[1]-path[0]);
	//	Ogre::Vector3 pDir(-dir.y, dir.x, dir.z);
	//	pDir.normalise();
	//	Ogre::Vector3 origin = OGREVECTOR(path[0]) - (pDir*25);
	//	pDir *= 50;
	//	Ogre::Vector3 bottom(Ogre::Vector3::UNIT_Z*150+origin);
	//	Ogre::Vector3 top(Ogre::Vector3::UNIT_Z*300+bottom);

	//	pObj->position(bottom);
	//	pObj->position(top);
	//	pObj->position(top+dir);	
	//	pObj->position(bottom+dir);	

	//	pObj->position(bottom+pDir);	
	//	pObj->position(top+pDir);		
	//	pObj->position(top+pDir+dir);	
	//	pObj->position(bottom+pDir+dir);

	//	pObj->index(0);pObj->index(3);pObj->index(1);
	//	pObj->index(3);pObj->index(2);pObj->index(1);
	//	pObj->index(4);pObj->index(5);pObj->index(7);
	//	pObj->index(7);pObj->index(5);pObj->index(6);
	//	pObj->index(1);pObj->index(2);pObj->index(5);
	//	pObj->index(2);pObj->index(6);pObj->index(5);
	//	pObj->index(0);pObj->index(4);pObj->index(3);
	//	pObj->index(3);pObj->index(4);pObj->index(7);
	//	pObj->index(0);pObj->index(1);pObj->index(4);
	//	pObj->index(5);pObj->index(4);pObj->index(1);
	//	pObj->index(3);pObj->index(7);pObj->index(2);
	//	pObj->index(7);pObj->index(6);pObj->index(2);

	//pObj->end();
}
//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(LandsideBusStation3D)

class LandsideStretch;
void LandsideBusStation3D::OnUpdate3D()
{
	LandsideBusStation* pBusStation = getBusStation();

	CPath2008 path = pBusStation->getPath();
	if(path.getCount() < 2) return;

	CPoint2008 ptCenter;

	CString sStretchArea;
	sStretchArea.Format("%s-%s", GetIDName(),"BusParkingArea");
	LandsideLinkStretchArea3D stretchAreaNode = OgreUtil::createOrRetrieveSceneNode(sStretchArea, GetScene());
	stretchAreaNode.Detach();
	stretchAreaNode.SetQueryData(pBusStation);
	if(	!stretchAreaNode.UpdateDraw(ptCenter) )
		return;

	stretchAreaNode.SetQueryData(pBusStation);
	stretchAreaNode.AttachTo(*this);

	SetPosition(ptCenter);
	BuildBusStation(path, ptCenter, this);
	UpdateWaitingPath(pBusStation, ptCenter);
}

void LandsideBusStation3D::UpdateWaitingPath(LandsideFacilityLayoutObject* pBusStation, CPoint2008& center)
{
	CString sName;
	sName.Format("%s-%s", GetIDName(), "WaitingPath");
	Ogre::ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(sName, GetScene());
	pObj->clear();

	CPath2008& waitingPath = pBusStation->GetPaxWaitQueue()->getWaittingPath();
	int count =  waitingPath.getCount();
	if(count < 2) return;

	CPath2008 relatedPath;
	relatedPath.init(count);
	for (int i = 0; i < count; i++)
	{
		relatedPath[i] = waitingPath[i] - center;
		relatedPath[i].setZ(0.0);
	}

	//ARCPipe pipe(relatedPath, 100);
	//ProcessorBuilder::DrawColorPath(pObj, pipe, MAT_LANDSIDE_BUS_STATION_WAITING, ColourValue(0.23,0.34,0.19));
	Path m_Path(relatedPath);
	ProcessorBuilder::DrawPath(pObj, &m_Path, ColourValue(0.23,0.34,0.19));
	ProcessorBuilder::DrawTailArrow(pObj, &m_Path, ColourValue(0.23,0.34,0.19));
	AddObject(pObj, 2);

	LandsideBusStationWaitingPath3D  waitingPath3D = OgreUtil::createOrRetrieveSceneNode(sName, GetScene());
	waitingPath3D.Update3D(pBusStation, center, IsInEditMode());
	waitingPath3D.AttachTo(*this);
}

//void LandsideBusStation3D::UpdateParkingArea(LandsideBusStation* pBusStation, CPoint2008& center)
//{
//	CString sName;
//	sName.Format("%s-%s", GetIDName(),"Landside-BusStation-ParkingArea");
//
//	Ogre::ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(sName, GetScene());
//	pObj->clear();
//
//	ARCPipe out;
//	LandsideLinkStretchArea3D::GetPartStretch(pBusStation, center, out);
//	ProcessorBuilder::DrawColorPath(pObj,out,MAT_LANDSIDE_BUS_STATION_PARKING,ColourValue(0.25,0.16,0.09));
//	AddObject(pObj, 2);
//
//	LandsideBusStationParkingArea3D  parkingArea3D = OgreUtil::createOrRetrieveSceneNode(sName, GetScene());
//	parkingArea3D.Update3D(pBusStation, center, out, IsInEditMode());
//	parkingArea3D.AttachTo(*this);
//}

void LandsideBusStation3D::OnUpdateEditMode(BOOL b)
{
	SetEditMode(b);
	OnUpdate3D();
}

void LandsideBusStation3D::StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo)
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideBusStation* pBusStation = getBusStation();	
	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(static_cast<LandsideFacilityLayoutObject*>(pBusStation), plandsideCtx->getInput());	
	plandsideCtx->StartEditOp(pModCmd);
	SetInDragging(TRUE);

	OnDrag(lastMouseInfo,curMouseInfo);
}

void LandsideBusStation3D::OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideBusStation* pBusStation = getBusStation();	
	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;

	//pBusStation->DoOffset(ARCVector3(mousediff.x, mousediff.y, mousediff.z));
	MoveAlongStretch(mousediff);

	plandsideCtx->NotifyViewCurentOperation();
}

void LandsideBusStation3D::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->NotifyViewCurentOperation();
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}

void LandsideBusStation3D::MoveAlongStretch(Vector3 MouseMove)
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	LandsideBusStation* pBusStation = getBusStation();

	CPoint2008* pointList = pBusStation->m_PickArea.getPointList();
	CPoint2008* pointList2 = pBusStation->getPath().getPointList();
	int pathCount = pBusStation->m_PickArea.getCount();
	int pathCount2 = pBusStation->getPath().getCount();

	LandsideStretch* pStretch = pBusStation->getStrech();
	CPath2008& drawpath = pStretch->getControlPath().getBuildPath();
	int count = drawpath.getCount();
	ASSERT(count > 0);
	double len = 0;
	int k = 0;
	for ( ; k < (count - 1); k++)
	{
		if (len > pBusStation->m_dPosInStretch)
			break;
		len += drawpath.getPoint(k).distance3D( drawpath.getPoint(k+1) );
	}

	CPoint2008 BeforePoint = drawpath.GetDistPoint(pBusStation->m_dPosInStretch);
	//calculate the displacement in direction of the stretch
	ARCVector3 v(drawpath[k] - drawpath[k-1]);
	v.Normalize();
	int d = MouseMove.dotProduct( OGREVECTOR(v) );
	pBusStation->m_dPosInStretch+=d;
	len = 0;
	int i = 0;
	for ( ; i < (count - 1); i++)
	{
		if (len > pBusStation->m_dPosInStretch)
			break;
		len += drawpath.getPoint(i).distance3D( drawpath.getPoint(i+1) );
	}

	//border condition
	if (pBusStation->m_dPosInStretch < 0)
		pBusStation->m_dPosInStretch = 0;
	int pathLen = drawpath.GetTotalLength();
	if (pBusStation->m_dPosInStretch + pBusStation->m_dLength> pathLen)
		pBusStation->m_dPosInStretch = pathLen - pBusStation->m_dLength;

	CPoint2008 AfterPoint = drawpath.GetDistPoint(pBusStation->m_dPosInStretch);

	if (pBusStation->m_dPosInStretch!=0&&pBusStation->m_dPosInStretch!=pathLen-pBusStation->m_dLength)
	{
		int j;
		if (d>0)
		{
			if (k!=i)
			{
				ARCVector3 v1,v2,v3;
				double CosAngle,SinAngle;
				CPoint2008 dCenter;
				v1 = drawpath[k] - drawpath[k-1];
				v2 = drawpath[i] - drawpath[k];
				v1.Normalize();
				v2.Normalize();
				CosAngle = v1.dot(v2);
				v3 = v1.cross(v2);
				if (v3.z>0)
					SinAngle = sin(acos(CosAngle));
				else
					SinAngle = -sin(acos(CosAngle));

				dCenter = drawpath.GetIndexPoint(k);
				for (j = 0 ; j < pathCount; j++)
				{
					CPoint2008 NewPoint;
					NewPoint.x = pointList[j].x-dCenter.x;	
					NewPoint.y = pointList[j].y-dCenter.y;	
					NewPoint.z = pointList[j].z-dCenter.z;

					CPoint2008 Pt;
					Pt.x = NewPoint.x*CosAngle - NewPoint.y*SinAngle;
					Pt.y = NewPoint.x*SinAngle + NewPoint.y*CosAngle;
					Pt.z = NewPoint.z;

					pointList[j].x = Pt.x+dCenter.x;	
					pointList[j].y = Pt.y+dCenter.y;	
					pointList[j].z = Pt.z+dCenter.z; 
				}
				for (j = 0 ; j < pathCount2; j++)
				{
					CPoint2008 NewPoint;
					NewPoint.x = pointList2[j].x-dCenter.x;	
					NewPoint.y = pointList2[j].y-dCenter.y;	
					NewPoint.z = pointList2[j].z-dCenter.z;

					CPoint2008 Pt;
					Pt.x = NewPoint.x*CosAngle - NewPoint.y*SinAngle;
					Pt.y = NewPoint.x*SinAngle + NewPoint.y*CosAngle;
					Pt.z = NewPoint.z;

					pointList2[j].x = Pt.x+dCenter.x;	
					pointList2[j].y = Pt.y+dCenter.y;	
					pointList2[j].z = Pt.z+dCenter.z; 
				}
			} 
			else
			{
				for ( j = 0 ; j < pathCount; j++)
				{
					pointList[j].x += AfterPoint.x - BeforePoint.x;	
					pointList[j].y += AfterPoint.y - BeforePoint.y;	
					pointList[j].z += AfterPoint.z - BeforePoint.z;
				}
				for ( j = 0 ; j < pathCount2; j++)
				{
					pointList2[j].x += AfterPoint.x - BeforePoint.x;	
					pointList2[j].y += AfterPoint.y - BeforePoint.y;	
					pointList2[j].z += AfterPoint.z - BeforePoint.z;
				}
			}
		} 
		else
		{
			if (k!=i)
			{
				ARCVector3 v1,v2,v3;
				double CosAngle,SinAngle;
				CPoint2008 dCenter;
				v1 = drawpath[i] - drawpath[i-1];
				v2 = drawpath[k] - drawpath[i];
				v1.Normalize();
				v2.Normalize();
				CosAngle = v1.dot(v2);
				v3 = v1.cross(v2);
				if (v3.z<0)
					SinAngle = sin(acos(CosAngle));
				else
					SinAngle = -sin(acos(CosAngle));

				dCenter = drawpath.GetIndexPoint(i);
				for (j = 0 ; j < pathCount; j++)
				{
					CPoint2008 NewPoint;
					NewPoint.x = pointList[j].x-dCenter.x;	
					NewPoint.y = pointList[j].y-dCenter.y;	
					NewPoint.z = pointList[j].z-dCenter.z;

					CPoint2008 Pt;
					Pt.x = NewPoint.x*CosAngle - NewPoint.y*SinAngle;
					Pt.y = NewPoint.x*SinAngle + NewPoint.y*CosAngle;
					Pt.z = NewPoint.z;

					pointList[j].x = Pt.x+dCenter.x;	
					pointList[j].y = Pt.y+dCenter.y;	
					pointList[j].z = Pt.z+dCenter.z; 
				}
				for (j = 0 ; j < pathCount2; j++)
				{
					CPoint2008 NewPoint;
					NewPoint.x = pointList2[j].x-dCenter.x;	
					NewPoint.y = pointList2[j].y-dCenter.y;	
					NewPoint.z = pointList2[j].z-dCenter.z;

					CPoint2008 Pt;
					Pt.x = NewPoint.x*CosAngle - NewPoint.y*SinAngle;
					Pt.y = NewPoint.x*SinAngle + NewPoint.y*CosAngle;
					Pt.z = NewPoint.z;

					pointList2[j].x = Pt.x+dCenter.x;	
					pointList2[j].y = Pt.y+dCenter.y;	
					pointList2[j].z = Pt.z+dCenter.z; 
				}
			} 
			else
			{
				for ( j = 0 ; j < pathCount; j++)
				{
					pointList[j].x += AfterPoint.x - BeforePoint.x;	
					pointList[j].y += AfterPoint.y - BeforePoint.y;	
					pointList[j].z += AfterPoint.z - BeforePoint.z;
				}
				for ( j = 0 ; j < pathCount2; j++)
				{
					pointList2[j].x += AfterPoint.x - BeforePoint.x;	
					pointList2[j].y += AfterPoint.y - BeforePoint.y;	
					pointList2[j].z += AfterPoint.z - BeforePoint.z;
				}
			}
		}
	}

	plandsideCtx->NotifyViewCurentOperation();
}

LandsideBusStation* LandsideBusStation3D::getBusStation()
{
	return (LandsideBusStation*)getLayOutObject();
}
//////////////////////////////////////////////////////////////////////////

void LandsideObject3DDraggableSubItem::OnUpdateEditMode(BOOL b)
{
	SetEditMode(b);
	OnUpdate3D();
}

void LandsideObject3DDraggableSubItem::StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo)
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideFacilityLayoutObject* pBusStation = getLayoutObject();	
	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(static_cast<LandsideFacilityLayoutObject*>(pBusStation), plandsideCtx->getInput());	
	plandsideCtx->StartEditOp(pModCmd);
	SetInDragging(TRUE);

	OnDrag(lastMouseInfo,curMouseInfo);
}

void LandsideObject3DDraggableSubItem::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->NotifyViewCurentOperation();
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}

LandsideFacilityLayoutObject* LandsideObject3DDraggableSubItem::getLayoutObject()
{
	if(SceneNodeQueryData *pData = getQueryData())
	{
		LandsideFacilityLayoutObject* pBusStation  = NULL;
		pData->GetData(KeyAltObjectPointer,(int&)pBusStation);
		return pBusStation;
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(LandsideBusStationWaitingPath3D)
void LandsideBusStationWaitingPath3D::SetQueryData(LandsideFacilityLayoutObject* pBusStation)
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer, (int)pBusStation);	
		pData->SetData(KeyLandsideItemType, getType());
	}
}

emLandsideItemType LandsideBusStationWaitingPath3D::getType()
{
	return _BusStationWaitingPath;
}

void LandsideBusStationWaitingPath3D::OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideFacilityLayoutObject* pBusStation = getLayoutObject();
	ILandsidePaxWaitQueue* pPaxWaitQ  = pBusStation->GetPaxWaitQueue();
	ASSERT(pPaxWaitQ);
	if(!pPaxWaitQ)return;

	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;
	pPaxWaitQ->WaitingPathDoOffset(ARCVector3(mousediff.x, mousediff.y, mousediff.z));

	plandsideCtx->NotifyViewCurentOperation();
}

int LandsideBusStationWaitingPath3D::GetPathIndex()
{
	int ipath = -1;
	if(SceneNodeQueryData *pData = getQueryData())
	{
		pData->GetData(KeyControlPathIndex, ipath);
	}
	return ipath;
}



void LandsideBusStationWaitingPath3D::Update3D(LandsideFacilityLayoutObject* pBusStation, CPoint2008& center, int bEdit)
{
	SetQueryData(pBusStation);

	RemoveChild();

	CString sName;
	sName.Format("%s-%s", GetIDName(), "Landside-BusStation-WaitingPath-CtrlPath");
	Ogre::ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(sName, GetScene());
	pObj->clear();

	if(!bEdit) return;

	CPath2008& waitingPath = pBusStation->GetPaxWaitQueue()->getWaittingPath();
	int count =  waitingPath.getCount();
	if(count < 2) return;

	CPoint2008 relatedPoint;
	pObj->begin(VERTEXCOLOR_LIGTHOFF, Ogre::RenderOperation::OT_LINE_STRIP);
		for (int i = 0; i < count; i++)
		{
			relatedPoint = waitingPath[i]-center;
			relatedPoint.setZ(0.0);
			pObj->position(relatedPoint.x, relatedPoint.y, relatedPoint.z);
			pObj->colour(Ogre::ColourValue::Red);

			LandsideBusStationWaitingPoint3D point3D = GetControlPoint3D(i);
			point3D.Update3D(pBusStation, i);
			point3D.SetPosition(relatedPoint);
			point3D.AttachTo(*this);
		}
	pObj->end();
	
	AddObject(pObj,2);
}

LandsideBusStationWaitingPoint3D LandsideBusStationWaitingPath3D::GetControlPoint3D(int idx)
{
	CString sPointName;
	sPointName.Format("%s-%d", GetIDName(), idx);
	return OgreUtil::createOrRetrieveSceneNode(sPointName.GetString(), GetScene());
}

void LandsideBusStationWaitingPath3D::OnRButtonDown(const MouseSceneInfo& mouseInfo)
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyRClickWorldPos, ARCStringConvert::toString(ARCVector3(mouseInfo.mWorldPos.x, mouseInfo.mWorldPos.y, mouseInfo.mWorldPos.z)));
	}
}
//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(LandsideBusStationWaitingPoint3D)

void LandsideBusStationWaitingPoint3D::SetQueryData(LandsideFacilityLayoutObject* pBusStation, int ipoint)
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyAltObjectPointer, (int)pBusStation);	
		pData->SetData(KeyLandsideItemType, getType());
		pData->SetData(KeyControlPointIndex, ipoint);
	}
}

emLandsideItemType LandsideBusStationWaitingPoint3D::getType()
{
	return _BusStationWaitingPoint;
}

int LandsideBusStationWaitingPoint3D::GetPointIndex()
{
	int ipoint = -1;
	if(SceneNodeQueryData *pData = getQueryData())
	{
		pData->GetData(KeyControlPointIndex,ipoint);
	}
	return ipoint;
}

void LandsideBusStationWaitingPoint3D::OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideFacilityLayoutObject* pBusStation = getLayoutObject();	
	ILandsidePaxWaitQueue* pPaxWaitQ  = pBusStation->GetPaxWaitQueue();

	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;
	int ipoint = GetPointIndex();
	pPaxWaitQ->DragPointOnWaitingPath(ARCVector3(mousediff.x, mousediff.y, mousediff.z), ipoint);

	plandsideCtx->NotifyViewCurentOperation();
}

void LandsideBusStationWaitingPoint3D::Update3D(LandsideFacilityLayoutObject* pBusStation,int ipoint)
{
	SetQueryData(pBusStation, ipoint);

	CString sPointName;
	sPointName.Format("%s-%d", GetIDName(), ipoint);
	Ogre::Entity* pEnt = OgreUtil::createOrRetrieveEntity(sPointName, SphereMesh, GetScene());
	pEnt->setMaterial(OgreUtil::createOrRetrieveColorMaterial(ARCColor3(255,191,0)));
	AddObject(pEnt, 2);
}
//////////////////////////////////////////////////////////////////////////
//PROTYPE_INSTANCE(LandsideBusStationParkingArea3D)
//
//emLandsideItemType LandsideBusStationParkingArea3D::getType()
//{
//	return _BusStationParkingArea;
//}
//
//void LandsideBusStationParkingArea3D::SetQueryData(LandsideBusStation* pBusStation)
//{
//	if(SceneNodeQueryData* pData = InitQueryData())
//	{
//		pData->SetData(KeyAltObjectPointer, (int)pBusStation);	
//		pData->SetData(KeyLandsideItemType, getType());
//	}
//}
//
//void LandsideBusStationParkingArea3D::OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
//{
//	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
//	if(!plandsideCtx)
//		return;	
//	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
//		return;
//
//	LandsideBusStation* pBusStation = getLayoutObject();	
//	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;
//
//
//	plandsideCtx->NotifyViewCurentOperation();
//}
//
//LandsideBusStationParkingAreaPoint3D LandsideBusStationParkingArea3D::GetControlPoint3D(int idx)
//{
//	CString sPointName;
//	sPointName.Format("%s-%d", GetIDName(), idx);
//	return OgreUtil::createOrRetrieveSceneNode(sPointName.GetString(), GetScene());
//}
//
//void LandsideBusStationParkingArea3D::Update3D(LandsideBusStation* pBusStation, CPoint2008& center, const ARCPipe& pipe, int bEdit)
//{
//	typedef LandsideBusStationParkingAreaPoint3D::LSBSPAPointType PointType;
//
//	RemoveChild();
//
//	if (!bEdit) return;
//	
//	for (int i = 0; i < 4; i++)
//	{
//		LandsideBusStationParkingAreaPoint3D point3D = GetControlPoint3D(i);
//		PointType type = static_cast<PointType>(i);
//		point3D.Update3D(pBusStation, type);
//		point3D.AttachTo(*this);
//
//		switch (type)
//		{
//		case LandsideBusStationParkingAreaPoint3D::LSBSPAPointType::BOTTOM:
//			{
//				point3D.SetPosition((pipe.m_sidePath1[0]+pipe.m_sidePath2[0])/2);
//			}
//			break;
//		case LandsideBusStationParkingAreaPoint3D::LSBSPAPointType::TOP:
//			{
//				int count = (int)pipe.m_centerPath.size();
//				point3D.SetPosition((pipe.m_sidePath1[count-1]+pipe.m_sidePath2[count-1])/2);
//			}
//			break;
//		case LandsideBusStationParkingAreaPoint3D::LSBSPAPointType::LEFT:
//			{
//				CPath2008 leftPath;
//				int count = (int)pipe.m_centerPath.size();
//				leftPath.init(count);
//				for (int i = 0; i < count; i++)
//				{
//					leftPath[i].x = pipe.m_sidePath1[i].x;
//					leftPath[i].y = pipe.m_sidePath1[i].y;
//					leftPath[i].z = pipe.m_sidePath1[i].z;
//				}
//				ARCVector3 pos(leftPath.GetDistPoint(leftPath.GetTotalLength()/2));
//				point3D.SetPosition(pos);
//			}
//			break;
//		case LandsideBusStationParkingAreaPoint3D::LSBSPAPointType::RIGHT:
//			{
//				CPath2008 rightPath;
//				int count = (int)pipe.m_centerPath.size();
//				rightPath.init(count);
//				for (int i = 0; i < count; i++)
//				{
//					rightPath[i].x = pipe.m_sidePath2[i].x;
//					rightPath[i].y = pipe.m_sidePath2[i].y;
//					rightPath[i].z = pipe.m_sidePath2[i].z;
//				}
//				ARCVector3 pos(rightPath.GetDistPoint(rightPath.GetTotalLength()/2));
//				point3D.SetPosition(pos);
//			}
//			break;
//		}
//	}
//}
//
////////////////////////////////////////////////////////////////////////////
//PROTYPE_INSTANCE(LandsideBusStationParkingAreaPoint3D)
//
//emLandsideItemType LandsideBusStationParkingAreaPoint3D::getType()
//{
//	return _BusStationParkingAreaPoint;
//}
//
//void LandsideBusStationParkingAreaPoint3D::SetQueryData(LandsideBusStation* pBusStation, LSBSPAPointType type)
//{
//	if (SceneNodeQueryData* PData = InitQueryData())
//	{
//		PData->SetData(KeyAltObjectPointer, (int)pBusStation);
//		PData->SetData(KeyControlPointIndex, static_cast<int>(type));
//	}
//}
//
//LandsideBusStationParkingAreaPoint3D::LSBSPAPointType
//LandsideBusStationParkingAreaPoint3D::GetPointType()
//{
//	int type = -1;
//	if(SceneNodeQueryData *pData = getQueryData())
//	{
//		pData->GetData(KeyControlPointIndex,type);
//	}
//	return static_cast<LSBSPAPointType>(type);
//}
//
//void LandsideBusStationParkingAreaPoint3D::StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo)
//{
//	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
//	if(!plandsideCtx)
//		return;	
//
//	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
//		return;
//
//	LandsideBusStation* pBusStation = getLayoutObject();	
//	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(static_cast<LandsideLayoutObject*>(pBusStation), plandsideCtx->getInput());	
//	plandsideCtx->StartEditOp(pModCmd);
//	SetInDragging(TRUE);
//	m_lastCrossPoint = OGREVECTOR(GetPosition());
//	OnDrag(lastMouseInfo,curMouseInfo);
//}
//
//void LandsideBusStationParkingAreaPoint3D::OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
//{
//	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
//	if(!plandsideCtx)
//		return;	
//	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
//		return;
//
//	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;
//
//	LandsideLayoutObject* pBusStation = getLayoutObject();	
//	LandsideStretch* pStretch = pBusStation->getStrech();
//
//	CPath2008& path = pStretch->getControlPath().getBuildPath();
//
//	LSBSPAPointType type = GetPointType();
//	switch (type)
//	{
//	case BOTTOM:
//		{
//			int i = pBusStation->getPointIndexOnStretch(pBusStation->GetPosInStretch());
//			ARCVector3 dir(path[i+1]-path[i]);
//			dir.Normalize();
//			double dis = mousediff.dotProduct(OGREVECTOR(dir));
//			pBusStation->PosOnStretchDoOffset(dis, static_cast<int>(BOTTOM));
//		}
//		break;
//	case TOP:
//		{
//			int i = pBusStation->getPointIndexOnStretch(pBusStation->GetPosInStretch()+pBusStation->GetLength());
//			ARCVector3 dir(path[i+1]-path[i]);
//			dir.Normalize();
//			double dis = mousediff.dotProduct(OGREVECTOR(dir));
//			pBusStation->PosOnStretchDoOffset(dis, static_cast<int>(TOP));
//		}
//		break;
//	case LEFT:
//		{
//			Ogre::Vector3 v = curMouseInfo.mWorldPos - m_lastCrossPoint;
//			int count = path.getCount();
//			int midIndex = count / 2;
//			ARCVector3 dir(path[midIndex] - path[midIndex-1]);
//			ARCVector3 pdir(dir.y, -dir.x, dir.z);
//			pdir.Normalize();
//			int dis = v.dotProduct(OGREVECTOR(pdir));
//			if (dis > pStretch->getLaneWidth())
//				dis = 1;
//			else if( dis < -pStretch->getLaneWidth())
//				dis = -1;
//			else 
//				dis = 0;
//			if(dis != 0)
//				m_lastCrossPoint = curMouseInfo.mWorldPos;
//			
//			pBusStation->LaneFromDoOffset(dis);
//		}
//		break;
//	case RIGHT:
//		{
//			Ogre::Vector3 v = curMouseInfo.mWorldPos - m_lastCrossPoint;
//			int count = path.getCount();
//			int midIndex = count / 2;
//			ARCVector3 dir(path[midIndex] - path[midIndex-1]);
//			ARCVector3 pdir(dir.y, -dir.x, dir.z);
//			pdir.Normalize();
//			int dis = v.dotProduct(OGREVECTOR(pdir));
//			if (dis > pStretch->getLaneWidth())
//				dis = 1;
//			else if( dis < -pStretch->getLaneWidth())
//				dis = -1;
//			else 
//				dis = 0;
//			if(dis != 0)
//				m_lastCrossPoint = curMouseInfo.mWorldPos;
//
//			pBusStation->LaneToDoOffset(dis);
//		}
//		break;
//	}
//
//	plandsideCtx->NotifyViewCurentOperation();
//}
//
//void LandsideBusStationParkingAreaPoint3D::Update3D(LandsideBusStation* pBusStation,LSBSPAPointType type)
//{
//	SetQueryData(pBusStation, type);
//
//	CString sPointName;
//	sPointName.Format("%s-%d", GetIDName(), (int)type);
//	Ogre::Entity* pEnt = OgreUtil::createOrRetrieveEntity(sPointName, SphereMesh, GetScene());
//	pEnt->setMaterial(OgreUtil::createOrRetrieveColorMaterial(ARCColor3(255,191,0)));
//	AddObject(pEnt, 2);
//
//}
