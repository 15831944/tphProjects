#include "StdAfx.h"
#include ".\landsidecurbside3d.h"
#include <Common/ARCPipe.h>
#include <common/ARCStringConvert.h>
#include "landside/Landside.h"
#include "Landside/LandsideCurbSide.h"
#include "Landside/LandsideStretchSegment.h"
#include "MaterialDef.h"
#include "./ShapeBuilder.h"
#include "RenderFloor3D.h"
#include "Render3DScene.h"
#include "Landside/LandsideQueryData.h"
#include ".\SceneNodeQueryData.h"
#include <Common/Pollygon2008.h>
#include "Tesselator.h"
#include "Common/ARCPipe.h"
#include "Landside/LandsideLinkStretchObject.h"
#include "LandsideLinkStretchArea.h"
#include <string.h>
#include "LandsideParkingLot3D.h"

#define CURBSIDE_MAT "Landside/CurbsideFace"


PROTYPE_INSTANCE (LandsideCurbside3D)


class CurbsideParkingSpace3D : public CRenderScene3DNode
{
public:
	SCENENODE_CONVERSION_DEF(CurbsideParkingSpace3D, CRenderScene3DNode)
	PROTOTYPE_DECLARE(CurbsideParkingSpace3D);

	void OnLButtonDblClk( const MouseSceneInfo& mouseinfo )
	{
		if(IsInEditMode())
		{
			LandsideCurbSide* pCurb = getCurb();
			if(pCurb)
				pCurb->SaveObject(-1);
		}
		FlipEditMode();
		Update3D();		
	}

	void Update3D()
	{
		LandsideCurbSide* pCurb = getCurb();
		int idx = getSpaceIndex();
		if(pCurb && idx>=0)
		{
			ARCVector3 pCenter = GetParent().GetPosition();
			Update3D(pCurb, idx, CPoint2008(pCenter.x,pCenter.y,pCenter.z));
		}
	}

	void Update3D(LandsideCurbSide* pCurb, int idx , const CPoint2008&  ptCenter)
	{
		RemoveChild();

		SetQueryData(pCurb,idx);
		ManualObject* pObj = OgreUtil::createOrRetrieveManualObject( GetIDName() ,GetScene());
		ManualObject* pselObj = OgreUtil::createOrRetrieveManualObject( GetIDName() +"select", GetScene());

		ParkingSpace& space = pCurb->m_outStretchParkingspace.getSpace(idx);
		ParkingLotParkingSpace3DNode::RenderSpace(space, idx, pObj,pselObj, ptCenter);
		AddObject(pObj,0,true);
		AddObject(pselObj,2,false);

		if(IsInEditMode())
		{
			CString sName = GetIDName() + _T("CtrlPath");
			ControlPath3D ctrlpath3d = OgreUtil::createOrRetrieveSceneNode(sName,GetScene());
			ctrlpath3d.Update3D(&space.m_ctrlPath,ptCenter);
			ctrlpath3d.AttachTo(*this);
		}
	}

	LandsideCurbSide* getCurb()
	{
		if(SceneNodeQueryData *pData = getQueryData())
		{
			LandsideCurbSide* pCurb  = NULL;
			pData->GetData(KeyAltObjectPointer,(int&)pCurb);
			return pCurb;
		}
		return NULL;
	}
	int getSpaceIndex()
	{
		if(SceneNodeQueryData *pData = getQueryData())
		{
			int idx = -1;
			pData->GetData(KeyControlPathIndex,idx);
			return idx;
		}
		return -1;
	}

	void SetQueryData(LandsideCurbSide*pCurb, int spaceIdx)
	{
		if(SceneNodeQueryData* pData = InitQueryData())
		{
			pData->SetData(KeyAltObjectPointer,(int)pCurb);
			pData->SetData(KeyControlPathIndex,spaceIdx);
		}
	}
};




void ControlPoint3D::Update3D(const CPath2008* path, int idx )
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{		
		pData->SetData(KeyControlPathIndex, (int)path);
		pData->SetData(KeyControlPointIndex, idx);	
	}

	Entity* pEnt =  OgreUtil::createOrRetrieveEntity(GetIDName(),SphereMesh,GetScene());
	ARCColor3 color = ARCColor3(255,0,0);	
	pEnt->setMaterial(OgreUtil::createOrRetrieveColorMaterial(color));
	AddObject(pEnt,2);
}

void ControlPoint3D::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	CPath2008* path = getPath();
	int idx = getIndex();
	if(!path)
		return;
	if(idx<0 && idx>= path->getCount() )
		return;
	ILandsideEditContext* plandsideCtx = GetLandsideEditContext();
	if(!plandsideCtx)
		return;	

	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;
	{
		(*path)[idx] = (*path)[idx]  + CPoint2008(mousediff.x,mousediff.y,0);
		ControlPath3D node = GetParent();
		CurbsideParkingSpace3D space3D = node.GetParent();		
		space3D.Update3D();

		plandsideCtx->UpdateDrawing();
	}
}

CPath2008* ControlPoint3D::getPath()
{
	CPath2008* path = NULL;
	if(SceneNodeQueryData* pData = getQueryData())
	{		
		pData->GetData(KeyControlPathIndex, (int&)path);
	}
	return path;
}

int ControlPoint3D::getIndex()
{
	int idx = -1;
	if(SceneNodeQueryData* pData = getQueryData())
	{		
		pData->GetData(KeyControlPointIndex, idx);
	}
	return idx;
}

void ControlPoint3D::EndDrag()
{
	SetInDragging(FALSE);
}

void ControlPath3D::Update3D( const CPath2008* path, const CPoint2008& ptCenter )
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{		
		pData->SetData(KeyControlPathIndex, (int)path);
	}


	CPath2008 drawpath = *path;

	drawpath.DoOffset(-ptCenter.x,-ptCenter.y,-ptCenter.z);

	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());	

	pObj->clear();

	CShapeBuilder::DrawPath(pObj,VERTEXCOLOR_LIGTHOFF,drawpath,ARCColor3(255,0,0));
	AddObject(pObj,2);
	//update control points
	RemoveChild();

	//update points
	for(int i=0;i<drawpath.getCount();i++)
	{	
		CString sPt;
		sPt.Format(_T("%s%d"),GetIDName(),i);
		ControlPoint3D ctrlPt3D = OgreUtil::createOrRetrieveSceneNode(sPt,GetScene());
		ctrlPt3D.Update3D(path,i);		
		ctrlPt3D.SetPosition(drawpath.getPoint(i));
		ctrlPt3D.AttachTo(*this);
	}
}

CPath2008* ControlPath3D::getPath()
{
	CPath2008* path= NULL;
	if(SceneNodeQueryData* pData = getQueryData())
	{		
		pData->GetData(KeyControlPathIndex, (int&)path);
	}
	return path;
}



PROTYPE_INSTANCE(CurbsideParkingSpace3D);
PROTYPE_INSTANCE(ControlPoint3D);
/////////////////////////////////////////////////////////////////////////
void LandsideCurbside3D::OnUpdate3D()
{
	 LandsideCurbSide* pCurb = getCurbside();	
	SetQueryData(pCurb);
	RemoveChild();

	LandsideStretch* pStretch = pCurb->getStrech();
	ILayoutObjectDisplay::DspProp& dspShape = pCurb->getDisplayProp().GetProp(ILayoutObjectDisplay::_Shape);

	
	//build the area on the stretch
	if(pStretch &&  dspShape.bOn)
	{
		CPoint2008 ptCenter;
		
		
		//build  the area picked on the stretch

		{
			CString sStretchArea = GetIDName() + "StretchArea";		

			LandsideLinkStretchArea3D stretchAreaNode = OgreUtil::createOrRetrieveSceneNode(sStretchArea, GetScene());
			stretchAreaNode.Detach();
			stretchAreaNode.SetQueryData(pCurb);			

			if(	!stretchAreaNode.UpdateDraw(ptCenter) )
				return;
		
			stretchAreaNode.SetQueryData(pCurb);			
			stretchAreaNode.AttachTo(*this);
			SetPosition(ARCVector3(ptCenter));
		}
	
		//build the DecisionLineNode
		{
			CString sDecisionLineNode  = GetIDName() + "DecisionLineNode";
			LandsideLinkStretchDecisionLine3D pDecisionLineNode = OgreUtil::createOrRetrieveSceneNode(sDecisionLineNode,GetScene());	
			pDecisionLineNode.Detach();
			pDecisionLineNode.SetQueryData(pCurb);
			if(	pDecisionLineNode.OnUpdate3D(ptCenter) )
			{
				pDecisionLineNode.SetQueryData(pCurb);
				pDecisionLineNode.AttachTo(*this);
			}			
		}
		//build the area beside the stretch
		if(pCurb->m_PickArea.getCount() > 2 ) 
		{			
			CString sPickArea = GetIDName() + "PickArea";
			LandsideLinkStretchPickArea3D pickAreaNode = OgreUtil::createOrRetrieveSceneNode(sPickArea, GetScene());
			pickAreaNode.Detach();

			pickAreaNode.SetQueryData(pCurb);
			pickAreaNode.OnUpdate3D(ptCenter);	
			pickAreaNode.AttachTo(*this);	
		}
		//build the parking spaces
		if(true)
		{
			for(int i=0;i<pCurb->m_outStretchParkingspace.getCount();i++)
			{
				//render parking space
				CString sName;
				sName.Format("%s-space(%d)",GetIDName(),i);
				CurbsideParkingSpace3D space3DNode=OgreUtil::createOrRetrieveSceneNode(sName,GetScene());
				space3DNode.Update3D(pCurb,i, ptCenter);
				space3DNode.AttachTo(*this);
			}

		}

	}

}
//////////////////////////////////////////////////////////////////////////


void LandsideCurbside3D::StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo)
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideCurbSide* pCurb = getCurbside();	
	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pCurb, plandsideCtx->getInput());	
	plandsideCtx->StartEditOp(pModCmd);
	SetInDragging(TRUE);

	OnDrag(lastMouseInfo,curMouseInfo);
}

class CLayoutObject3DNode;
class LandsideLinkStretchArea3D;
class LandsideLinkStretchPickArea3D;
void LandsideCurbside3D::OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo)
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideCurbSide* pCurb = getCurbside();	
	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;
	MoveAlongStretch(mousediff);

	
}

void LandsideCurbside3D::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->NotifyViewCurentOperation();
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}

void LandsideCurbside3D::MoveAlongStretch(const Vector3& MouseMove)
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	LandsideCurbSide* pCurb = getCurbside();

	CPoint2008* pointList = pCurb->m_PickArea.getPointList();
	int pathCount = pCurb->m_PickArea.getCount();

	LandsideStretch* pStretch = pCurb->getStrech();
	CPath2008& drawpath = pStretch->getControlPath().getBuildPath();
	int count = drawpath.getCount();
	ASSERT(count > 0);
	double len = 0;
	int k = 0;
	for ( ; k < (count - 1); k++)
	{
		if (len > pCurb->m_dPosInStretch)
			break;
		len += drawpath.getPoint(k).distance3D( drawpath.getPoint(k+1) );
	}

	CPoint2008 BeforePoint = drawpath.GetDistPoint(pCurb->m_dPosInStretch);
	//calculate the displacement in direction of the stretch
	ARCVector3 v(drawpath[k] - drawpath[k-1]);
	v.Normalize();
	int d = MouseMove.dotProduct( OGREVECTOR(v) );
	//double dDistance = pCurb->m_dPosInStretch;
	pCurb->m_dPosInStretch+=d;
	len = 0;
	int i = 0;
	for ( ; i < (count - 1); i++)
	{
		if (len > pCurb->m_dPosInStretch)
			break;
		len += drawpath.getPoint(i).distance3D( drawpath.getPoint(i+1) );
	}

	//border condition
	if (pCurb->m_dPosInStretch < 0)
		pCurb->m_dPosInStretch = 0;
	int pathLen = drawpath.GetTotalLength();
	if (pCurb->m_dPosInStretch + pCurb->m_dLength> pathLen)
		pCurb->m_dPosInStretch = pathLen - pCurb->m_dLength;

	//dDistance = pCurb->m_dPosInStretch - dDistance;
	//pCurb->m_DecisionLine += dDistance;

	CPoint2008 AfterPoint = drawpath.GetDistPoint(pCurb->m_dPosInStretch);

	if (pCurb->m_dPosInStretch!=0&&pCurb->m_dPosInStretch!=pathLen-pCurb->m_dLength)
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
			} 
			else
			{
				for ( j = 0 ; j < pathCount; j++)
				{
					pointList[j].x += AfterPoint.x - BeforePoint.x;	
					pointList[j].y += AfterPoint.y - BeforePoint.y;	
					pointList[j].z += AfterPoint.z - BeforePoint.z;
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
			} 
			else
			{
				for ( j = 0 ; j < pathCount; j++)
				{
					pointList[j].x += AfterPoint.x - BeforePoint.x;	
					pointList[j].y += AfterPoint.y - BeforePoint.y;	
					pointList[j].z += AfterPoint.z - BeforePoint.z;
				}
			}
		}
	}

	plandsideCtx->NotifyViewCurentOperation();
}

LandsideCurbSide* LandsideCurbside3D::getCurbside()
{
	return (LandsideCurbSide*)getLayOutObject();
}


//////////////////////////////////////////////////////////////////////////