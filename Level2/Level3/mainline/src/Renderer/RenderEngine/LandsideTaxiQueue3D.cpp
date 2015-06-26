#include "stdafx.h"
#include "LandsideTaxiQueue3D.h"
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

PROTYPE_INSTANCE(LandsideTaxiQueue3D)

#define MAT_LANDSIDE_BUS_STATION_WAITING "BusStation/WaitingPath"
#define MAT_LANDSIDE_BUS_STATION "BusStation/Face"
#define MAT_LANDSIDE_BUS_STATION_PARKING "BusStation/ParkingArea"
#define MESH_LANDSIDE_BUS_STATION_CYLINDER "cylinder01.mesh"
#define MAT_LANDSIDE_BUS_STATION_BANK "BusStation/Pan"
#define MESH_LANDSIDE_BUS_STATION_BOX "box01.mesh"


void LandsideTaxiQueue3D::_buildTaxiQueue(CPath2008& path, CPoint2008& center)
{
	path.DoOffset(-center.x, -center.y, 0.0);
	ARCPipe pipe(path, 200);
	Ogre::ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(), GetScene());
	pObj->clear();
	ProcessorBuilder::DrawColorPath(pObj, pipe, MAT_LANDSIDE_BUS_STATION_WAITING, ColourValue(0.10,0.20,0.10));
	AddObject(pObj, 2);

	//draw cylinder
	CString sName;
	ARCVector3 adir(path[1]-path[0]);
	Ogre::Vector3 dir = OGREVECTOR(adir);
	ARCVector3 pdir(dir.y, -dir.x, dir.z);
	pdir.Normalize();

	sName.Format("%s-left_cylinder", GetIDName());
	Ogre::Entity* leftcylinder = OgreUtil::createOrRetrieveEntity(sName, MESH_LANDSIDE_BUS_STATION_CYLINDER, GetScene());
	leftcylinder->setMaterial(OgreUtil::createOrRetrieveColorMaterial(ARCColor4(147,175,157,190)));
	C3DNodeObject leftNode = OgreUtil::createOrRetrieveSceneNode(sName, GetScene());	
	ARCVector3 scaleCylinder(3, 3, 10);
	leftNode.SetScale(scaleCylinder);
	leftNode.SetPosition(pipe.m_centerPath[0]+adir/4);
	leftNode.AddObject(leftcylinder);
	leftNode.AttachTo(*this);

	sName.Format("%s-right_cylinder", GetIDName());
	Ogre::Entity* rightcylinder = OgreUtil::createOrRetrieveEntity(sName, MESH_LANDSIDE_BUS_STATION_CYLINDER, GetScene());
	rightcylinder->setMaterial(OgreUtil::createOrRetrieveColorMaterial(ARCColor4(147,191,127,190)));
	C3DNodeObject rightNode = OgreUtil::createOrRetrieveSceneNode(sName, GetScene());	
	rightNode.SetScale(scaleCylinder);
	rightNode.SetPosition(pipe.m_centerPath[1]-adir/4);
	rightNode.AddObject(rightcylinder);
	rightNode.AttachTo(*this);

	sName.Format("%s-box", GetIDName());
	Ogre::Entity* box = OgreUtil::createOrRetrieveEntity(sName, MESH_LANDSIDE_BUS_STATION_BOX, GetScene());
	box->setMaterial(OgreUtil::createOrRetrieveColorMaterial(ARCColor4(255,201,14, 190)));
	C3DNodeObject boxNode = OgreUtil::createOrRetrieveSceneNode(sName, GetScene());	

	Ogre::Vector3 boxSize = box->getBoundingBox().getSize();
	ARCVector3 scaleBox((40+dir.length())/boxSize.x, 40/boxSize.y, 200/boxSize.z);
	boxNode.SetScale(scaleBox);
	boxNode.SetPosition((pipe.m_centerPath[1]+pipe.m_centerPath[0])/2 + ARCVector3(0, 0, 200) + pdir*20);
	boxNode.SetDirection(adir,ARCVector3(0,0,1));
	boxNode.AddObject(box);
	boxNode.AttachTo(*this);
}

void LandsideTaxiQueue3D::OnUpdate3D()
{
	LandsideTaxiQueue* pQ = getTaxiQueue();

	CPath2008 path = pQ->getPath();
	if(path.getCount() < 2) return;

	CPoint2008 ptCenter;

	//area on stretch
	CString sStretchArea;
	sStretchArea.Format("%s-%s", GetIDName(),"parkarea");
	LandsideLinkStretchArea3D stretchAreaNode = OgreUtil::createOrRetrieveSceneNode(sStretchArea, GetScene());
	stretchAreaNode.Detach();
	stretchAreaNode.SetQueryData(pQ);
	if(	!stretchAreaNode.UpdateDraw(ptCenter) )
		return;

	stretchAreaNode.SetQueryData(pQ);
	stretchAreaNode.AttachTo(*this);

	SetPosition(ptCenter);

	_buildTaxiQueue(path, ptCenter);
	UpdateWaitingPath(pQ, ptCenter);
}

void LandsideTaxiQueue3D::UpdateWaitingPath( LandsideTaxiQueue* pBusStation, CPoint2008& center )
{
	CString sName;
	sName.Format("%s-%s", GetIDName(), "WaitingPath");
	Ogre::ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(sName, GetScene());
	pObj->clear();

	CPath2008& waitingPath = pBusStation->getWaittingPath();
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
	waitingPath3D.Update3D( pBusStation , center, IsInEditMode());
	waitingPath3D.AttachTo(*this);
}

void LandsideTaxiQueue3D::StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo)
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideTaxiQueue* pTaxiQueue = getTaxiQueue();	
	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(static_cast<LandsideFacilityLayoutObject*>(pTaxiQueue), plandsideCtx->getInput());	
	plandsideCtx->StartEditOp(pModCmd);
	SetInDragging(TRUE);

	OnDrag(lastMouseInfo,curMouseInfo);
}

void LandsideTaxiQueue3D::OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideTaxiQueue* pTaxiQueue = getTaxiQueue();	
	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;

	//pBusStation->DoOffset(ARCVector3(mousediff.x, mousediff.y, mousediff.z));
	MoveAlongStretch(mousediff);

	plandsideCtx->NotifyViewCurentOperation();
}

void LandsideTaxiQueue3D::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->NotifyViewCurentOperation();
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}

void LandsideTaxiQueue3D::MoveAlongStretch(Vector3 MouseMove)
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	LandsideTaxiQueue* pTaxiQueue = getTaxiQueue();	

	CPoint2008* pointList = pTaxiQueue->m_PickArea.getPointList();
	CPoint2008* pointList2 = pTaxiQueue->getPath().getPointList();
	int pathCount = pTaxiQueue->m_PickArea.getCount();
	int pathCount2 = pTaxiQueue->getPath().getCount();

	LandsideStretch* pStretch = pTaxiQueue->getStrech();
	CPath2008& drawpath = pStretch->getControlPath().getBuildPath();
	int count = drawpath.getCount();
	ASSERT(count > 0);
	double len = 0;
	int k = 0;
	for ( ; k < (count - 1); k++)
	{
		if (len > pTaxiQueue->m_dPosInStretch)
			break;
		len += drawpath.getPoint(k).distance3D( drawpath.getPoint(k+1) );
	}

	CPoint2008 BeforePoint = drawpath.GetDistPoint(pTaxiQueue->m_dPosInStretch);
	//calculate the displacement in direction of the stretch
	ARCVector3 v(drawpath[k] - drawpath[k-1]);
	v.Normalize();
	int d = MouseMove.dotProduct( OGREVECTOR(v) );
	pTaxiQueue->m_dPosInStretch+=d;
	len = 0;
	int i = 0;
	for ( ; i < (count - 1); i++)
	{
		if (len > pTaxiQueue->m_dPosInStretch)
			break;
		len += drawpath.getPoint(i).distance3D( drawpath.getPoint(i+1) );
	}

	//border condition
	if (pTaxiQueue->m_dPosInStretch < 0)
		pTaxiQueue->m_dPosInStretch = 0;
	int pathLen = drawpath.GetTotalLength();
	if (pTaxiQueue->m_dPosInStretch + pTaxiQueue->m_dLength> pathLen)
		pTaxiQueue->m_dPosInStretch = pathLen - pTaxiQueue->m_dLength;

	CPoint2008 AfterPoint = drawpath.GetDistPoint(pTaxiQueue->m_dPosInStretch);

	if (pTaxiQueue->m_dPosInStretch!=0&&pTaxiQueue->m_dPosInStretch!=pathLen-pTaxiQueue->m_dLength)
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

LandsideTaxiQueue* LandsideTaxiQueue3D::getTaxiQueue()
{
	return (LandsideTaxiQueue*)getLayOutObject();
}
