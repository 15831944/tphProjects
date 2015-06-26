#include "StdAfx.h"
#include ".\landsidelinkstretcharea.h"
#include "Landside/LandsideLinkStretchObject.h"
#include ".\SceneNodeQueryData.h"
#include "Landside/LandsideQueryData.h"
#include "Render3DScene.h"
#include "landside/Landside.h"
#include "MaterialDef.h"
#include "./ShapeBuilder.h"
#include "Landside/LandsideCurbSide.h"
#include "CommonShapes.h"

#define LINKSTRETCH_MAT "Landside/CurbsideFace"
//#define DecisionLinePath "Stretch/MarkC"
#define DecisionLinePath "LeftArrow"
PROTYPE_INSTANCE(LandsideLinkStretchArea3D)

//get a sub part of the stretch(has the same shape, curve, line... as the selected part of stretch)
void GetPartPipe(const CPath2008& center, double dwidth, double dIndexFrom, double dIndexTo, double dwidthFrom, double dwidthTo , ARCPipe& out, double dZupper/* = 5*/ )
{
	out.m_centerPath.clear();
	out.m_sidePath1.clear();
	out.m_sidePath2.clear();

	ARCPoint3 upLeve(0,0,dZupper);
	double dMaxIndex = center.getCount();
	dIndexFrom = MIN(dMaxIndex,dIndexFrom); dIndexFrom = MAX(0,dIndexFrom);
	dIndexTo = MIN(dMaxIndex,dIndexTo); dIndexTo = MAX(0,dIndexTo);
	CPath2008 subCenter = center.GetSubPathIndex(dIndexFrom,dIndexTo);

	out = ARCPipe(subCenter,dwidth);

	for(int i=0;i<subCenter.getCount();i++)
	{
		ARCPoint3 ptL = out.m_sidePath1[i];
		ARCPoint3 ptR = out.m_sidePath2[i];

		double dRatFrom = dwidthFrom/dwidth;
		double dRatTo = dwidthTo/dwidth;
		dRatFrom = MIN(1.0,dRatFrom);
		dRatFrom = MAX(0,dRatFrom);

		dRatTo = MIN(1.0,dRatTo);
		dRatTo = MAX(0,dRatTo);


		ARCPoint3 ptLeftout = ptL * (1-dRatFrom) + ptR * dRatFrom; 
		ARCPoint3 ptRightout = ptL * (1-dRatTo) + ptR * dRatTo;

		out.m_sidePath1[i] = (ptLeftout) + upLeve;
		out.m_sidePath2[i] = (ptRightout) + upLeve;		
	}

}


LandsideLinkStretchObject* LandsideLinkStretchArea3D::getLinkStretchObject()
{
	LandsideLinkStretchObject* pLinkStretchObject = NULL;

	if(SceneNodeQueryData* pData = getQueryData())
	{
		pData->GetData(KeyAltObjectPointer,(int&)pLinkStretchObject);
	}
	return pLinkStretchObject;
}

void LandsideLinkStretchArea3D::SetQueryData( LandsideLinkStretchObject* pLinkStretchObject )
{
	if(SceneNodeQueryData* pData =InitQueryData())
	{
		pData->SetData(KeyLandsideItemType,_LayoutObject);
		pData->SetData(KeyAltObjectPointer,(int)pLinkStretchObject);
	}
}

void LandsideLinkStretchArea3D::OnUpdate3D()
{
	CPoint2008 ptcenter;
	UpdateDraw(ptcenter);
}

void LandsideLinkStretchArea3D::StartDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideLinkStretchObject* pLinkStretchObject = getLinkStretchObject();
	if (pLinkStretchObject == NULL)
		return;


	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pLinkStretchObject, plandsideCtx->getInput());	
	plandsideCtx->StartEditOp(pModCmd);
	SetInDragging(TRUE);

	OnDrag(lastMouseInfo,curMouseInfo);
}


void LandsideLinkStretchArea3D::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{	
	CRenderScene3DNode(this->GetParent()).OnDrag(lastMouseInfo,curMouseInfo);
	//plandsideCtx->NotifyViewCurentOperation();
}

void LandsideLinkStretchArea3D::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->NotifyViewCurentOperation();
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}

bool LandsideLinkStretchArea3D::IsDraggable() const
{
	return true;
}

void LandsideLinkStretchArea3D::OnUpdateEditMode( BOOL b )
{
	SetEditMode(b);	
	OnUpdate3D();
}

LandsideLinkStretchControlPoint3D LandsideLinkStretchArea3D::GetControlPoint3D( int idx )
{
	CString spt;
	spt.Format(_T("%s%d"), GetIDName(), idx);
	return OgreUtil::createOrRetrieveSceneNode(spt.GetString(), GetScene());
}


void LandsideLinkStretchArea3D::BuildControlPoints(const ARCPipe& m_pipe)
{
	LandsideLinkStretchObject* pLinkStretchObject = getLinkStretchObject();
	if (pLinkStretchObject == NULL)
		return;

	LandsideStretch* pStretch = pLinkStretchObject->getStrech();


	double len = 0, segLen = 0;
	ARCVector3 dir, top, bottom;
	//get the middle len of the path to set the top control point.
	double totalLen = m_pipe.m_sidePath1.GetLength();
	int count = (int)m_pipe.m_sidePath1.size();
	ASSERT(count > 1);
	for (int i = 0; i< count - 1; i++)
	{
		dir = m_pipe.m_sidePath1[i+1] - m_pipe.m_sidePath1[i];
		segLen = dir.Length();
		len += segLen;
		if ( len > (totalLen /2) ) {
			top = m_pipe.m_sidePath1[i+1] - dir * ( (len - totalLen / 2) /segLen );
			break;
		}
	}
	//get the middle len of the path to set the bottom control point.
	len = 0;
	totalLen = m_pipe.m_sidePath2.GetLength();
	count = (int)m_pipe.m_sidePath2.size();
	ASSERT(count > 1);
	for (int i = 0; i< count - 1; i++)
	{
		dir = m_pipe.m_sidePath2[i+1] - m_pipe.m_sidePath2[i];
		segLen = dir.Length();
		len += segLen;
		if ( len > (totalLen /2) ) {
			bottom = m_pipe.m_sidePath2[i+1] - dir * ( (len - totalLen / 2) /segLen );
			break;
		}
	}

	//the index of control points  
	//		3
	//	0		1
	//		2

	//set control point on the left side of the pipe
	LandsideLinkStretchControlPoint3D nodePoint3D = GetControlPoint3D(0);
	nodePoint3D.SetQueryData( getLinkStretchObject(), 0, LandsideLinkStretchControlPoint3D::CP_STRETCH);
	nodePoint3D.SetPosition( (m_pipe.m_sidePath1[0] + m_pipe.m_sidePath2[0]) / 2);
	nodePoint3D.Update3D();
	nodePoint3D.AttachTo(*this);
	//nodePoint3D.SetScale(ARCVector3((i+1), (i+1), (i+1)) );

	//set control point on the right side of the pipe
	nodePoint3D = GetControlPoint3D(1);
	nodePoint3D.SetQueryData( getLinkStretchObject(), 1, LandsideLinkStretchControlPoint3D::CP_STRETCH);
	nodePoint3D.SetPosition( (m_pipe.m_sidePath1[count-1] + m_pipe.m_sidePath2[count-1]) / 2 );
	nodePoint3D.Update3D();
	nodePoint3D.AttachTo(*this);
	//nodePoint3D.SetScale(ARCVector3((i+1), (i+1), (i+1)) );

	//set control point on the bottom side of the pipe
	nodePoint3D = GetControlPoint3D(2);
	nodePoint3D.SetQueryData( getLinkStretchObject(), 2, LandsideLinkStretchControlPoint3D::CP_STRETCH);
	nodePoint3D.SetPosition( bottom );
	nodePoint3D.Update3D();
	nodePoint3D.AttachTo(*this);
	//nodePoint3D.SetScale(ARCVector3((i+1), (i+1), (i+1)) );

	//set control point on the top side of the pipe
	nodePoint3D = GetControlPoint3D(3);
	nodePoint3D.SetQueryData( getLinkStretchObject(), 3, LandsideLinkStretchControlPoint3D::CP_STRETCH);
	nodePoint3D.SetPosition( top );
	nodePoint3D.Update3D();
	nodePoint3D.AttachTo(*this);
	//nodePoint3D.SetScale(ARCVector3((i+1), (i+1), (i+1)) );

	for (int i = 0; i < 4; i++)
	{
		nodePoint3D = GetControlPoint3D(i);
		//if not in the edit mode, hiding
		if ( !IsInEditMode())  
			nodePoint3D.SetVisible(false);
		else
			nodePoint3D.SetVisible(true);
	}
}

bool LandsideLinkStretchArea3D::UpdateDraw( CPoint2008& outcenter )
{
	RemoveAllObject();

	LandsideLinkStretchObject* pLinkStretchObject = getLinkStretchObject();
	if (pLinkStretchObject == NULL)
		return false;
	ILandsideEditContext* pCtx = GetLandsideEditContext();
	if(!pCtx)
		return false;

	LandsideStretch* pStretch = pLinkStretchObject->getStrech();
	if ( !pStretch)
		return false;

	ILayoutObjectDisplay::DspProp& dspShape = pLinkStretchObject->getDisplayProp().GetProp(ILayoutObjectDisplay::_Shape);



	CPath2008 drawpath = pStretch->getControlPath().getBuildPath();
	double dIndexFrom = drawpath.GetDistIndex(pLinkStretchObject->getDistFrom());
	double dIndexTo = drawpath.GetDistIndex(pLinkStretchObject->getDistTo());
	pCtx->ConvertToVisualPath(drawpath);
	outcenter = drawpath.GetIndexPoint(dIndexFrom);// m_pipe.m_centerPath.GetDistPos(m_pipe.m_centerPath.GetLength()*.5);

	

	if(pLinkStretchObject->m_bUseInStretchParking && dspShape.bOn )
	{
		double dWidth = pStretch->getLaneWidth()*pStretch->getLaneNum();
		double dLaneWidth = pStretch->getLaneWidth();		

		ARCPipe m_pipe;
		GetPartPipe(drawpath,dWidth,dIndexFrom,dIndexTo,(pLinkStretchObject->m_nLaneFrom-1)*dLaneWidth,pLinkStretchObject->m_nLaneTo*dLaneWidth,m_pipe,5);
		//out
		int pipecount = (int)m_pipe.m_centerPath.size();
		ARCVector3 offset(outcenter);
		for (int i = 0; i < pipecount; i++)
		{
			m_pipe.m_centerPath[i] = m_pipe.m_centerPath[i] - offset;
			m_pipe.m_sidePath1[i] = m_pipe.m_sidePath1[i] - offset;
			m_pipe.m_sidePath2[i] = m_pipe.m_sidePath2[i] - offset;
		}

		ARCPath3 cirlePath= m_pipe.m_sidePath1;
		cirlePath.Reverse();
		cirlePath.Append(m_pipe.m_sidePath2);

		ManualObject* p3DObj = OgreUtil::createOrRetrieveManualObject(GetIDName(), GetScene());
		p3DObj->clear();
		ProcessorBuilder::DrawTexturePath(p3DObj,LINKSTRETCH_MAT,OGRECOLOR(dspShape.cColor),cirlePath,50,0,true);
		AddObject(p3DObj, 2);

		BuildControlPoints(m_pipe);
	}
	else
	{
		ManualObject* p3DObj = OgreUtil::createOrRetrieveManualObject(GetIDName(), GetScene());
		p3DObj->clear();
		p3DObj->begin(LINKSTRETCH_MAT,RenderOperation::OT_TRIANGLE_FAN);
		p3DObj->position(-20,-20,10);
		p3DObj->colour(OGRECOLOR(dspShape.cColor));
		p3DObj->position(20,-20,10);
		p3DObj->position(20,20,10);
		p3DObj->position(-20,20,10);	
		p3DObj->end();
		AddObject(p3DObj, 2);
	}
	return true;

}


void LandsideLinkStretchArea3D::GetPartStretch( LandsideLinkStretchObject* pBusStation, const CPoint2008& center, ARCPipe& out )
{
	LandsideStretch* pStretch = pBusStation->getStrech();

	CPath2008& centerPath = pStretch->getControlPath().getBuildPath();
	double pos = pBusStation->GetPosInStretch();
	double length = pBusStation->GetLength();
	CPath2008 drawPath = centerPath.GetSubPath(pos, pos+length);

	for (int i = 0; i < drawPath.getCount(); i++)
		drawPath[i] += -center;

	double laneWidth = pStretch->getLaneWidth();
	int laneNum = pStretch->getLaneNum();
	double stretchWdith = laneWidth*laneNum;
	out = ARCPipe(drawPath, stretchWdith);

	double rateFrom = pBusStation->GetLaneFrom()/(double)laneNum;
	double rateTo = pBusStation->GetLaneTo()/(double)laneNum;

	for (int i = 0; i < drawPath.getCount(); i++)
	{
		ARCPoint3 dir = out.m_sidePath2[i] - out.m_sidePath1[i];
		out.m_sidePath1[i] += dir*rateFrom;
		out.m_sidePath2[i] = out.m_sidePath1[i] + dir*(rateTo - rateFrom);
	}
}



PROTYPE_INSTANCE(LandsideLinkStretchDecisionLine3D)
LandsideCurbSide* LandsideLinkStretchDecisionLine3D::getLandsideCurbSideObject()
{
	LandsideCurbSide* pLandsideCurbSideObject = NULL;

	if(SceneNodeQueryData* pData = getQueryData())
	{
		pData->GetData(KeyAltObjectPointer,(int&)pLandsideCurbSideObject);
	}
	return pLandsideCurbSideObject;
}

void LandsideLinkStretchDecisionLine3D::SetQueryData(LandsideCurbSide* pLandsideCurbSideOject)
{
	if(SceneNodeQueryData* pData =InitQueryData())
	{
		pData->SetData(KeyLandsideItemType,_LayoutObject);
		pData->SetData(KeyAltObjectPointer,(int)pLandsideCurbSideOject);
	}
}

bool LandsideLinkStretchDecisionLine3D::OnUpdate3D(CPoint2008& ptCenter)
{
	RemoveAllObject();
	
	LandsideCurbSide* pLandsideCurbSideObject = getLandsideCurbSideObject();
	if (pLandsideCurbSideObject == NULL)
		return false;
	ILandsideEditContext* pCtx = GetLandsideEditContext();
	if(!pCtx)return false;

	LandsideStretch* pStretch = pLandsideCurbSideObject->getStrech();
	if ( !pStretch)
		return false;

	ILayoutObjectDisplay::DspProp& dspShape = pLandsideCurbSideObject->getDisplayProp().GetProp(ILayoutObjectDisplay::_Shape);
	Side side = _Left;
	int iAtLane = -1;
	if(! pLandsideCurbSideObject->getDecisionLinePos(iAtLane,side))
		return false;

	CPath2008 drawpath = pStretch->getControlPath().getBuildPath();

	double dIndexFrom = drawpath.GetDistIndex(pLandsideCurbSideObject->m_dPosInStretch);//pLandsideCurbSideObject->m_DecisionLine);
	double dIndexTo = drawpath.GetDistIndex(pLandsideCurbSideObject->m_dPosInStretch+pStretch->getLaneWidth());

	pCtx->ConvertToVisualPath(drawpath);

	double dWidth = pStretch->getLaneWidth()*pStretch->getLaneNum();
	double dLaneWidth = pStretch->getLaneWidth();		

	
	
	CPath2008 midPath;
	ARCPipe pipe(drawpath,dWidth);
	pipe.getMidPath( (iAtLane-0.5)*dLaneWidth , midPath);
	CPoint2008  atPos = CPoint2008(0,0,5) + ( midPath.GetIndexPoint(dIndexFrom) - ptCenter );
	ARCVector3 dir = midPath.GetIndexDir(dIndexFrom);	

	//out
	if(dspShape.bOn)
	{
		ManualObject* p3DObj = OgreUtil::createOrRetrieveManualObject(GetIDName(), GetScene());
		p3DObj->clear();
		///////////////////////////////
		CommonShapes shapes(p3DObj);
		shapes.msMat = DecisionLinePath;
		if(_Left==side)
			shapes.DrawSquare(atPos,dir,dLaneWidth,dLaneWidth);	
		else
			shapes.DrawSquareFlip(atPos,dir,dLaneWidth,dLaneWidth);
		///////////////////////////////
		AddObject(p3DObj, 2);
	}
	return true;
}

PROTYPE_INSTANCE(LandsideLinkStretchControlPoint3D)
#define KeyCurbCtrlPtType "cubctrlpttype"

void LandsideLinkStretchControlPoint3D::Update3D()
{
	Entity* pEnt =  OgreUtil::createOrRetrieveEntity(GetIDName(),SphereMesh,GetScene());

	ARCColor3 color = ARCColor3(255,191,0);
	//if(IsStraightPoint())
	//	color = ARCColor3(98,250,191);	
	pEnt->setMaterial(OgreUtil::createOrRetrieveColorMaterial(color));
	AddObject(pEnt,2);
}

void LandsideLinkStretchControlPoint3D::StartDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideLinkStretchObject* pLinkStretch = GetLandsideLinkStretchObject();

	SetInDragging(TRUE);

	if(pLinkStretch)
	{		
		LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pLinkStretch,plandsideCtx->getInput());	
		plandsideCtx->StartEditOp(pModCmd);
		m_lastCrossPoint = OGREVECTOR(GetPosition());
		OnDrag(lastMouseInfo,curMouseInfo);
	}
}

void LandsideLinkStretchControlPoint3D::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	LandsideLinkStretchObject* pLinkStretch = GetLandsideLinkStretchObject();
	if ( !pLinkStretch )
		return;

	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if( !plandsideCtx )
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;

	Type t = GetControlPointType();
	switch (t)
	{
	case CP_PICKAREA://point on the side of the stretch
		{
			int idx = GetControlPointIndex();
			CPoint2008* pointList = pLinkStretch->m_PickArea.getPointList();
			pointList[idx].x += mousediff.x;	pointList[idx].y += mousediff.y;	pointList[idx].z+= mousediff.z;
		}
		break;
	case CP_STRETCH://point on the stretch
		{
			LandsideStretch* pStretch = pLinkStretch->getStrech();
			int idx = GetControlPointIndex();

			//calculate the position of the area on the stretch when dragging the point
			CPath2008& drawpath = pStretch->getControlPath().getBuildPath();
			int pathLen = drawpath.GetTotalLength();
			int count = drawpath.getCount();
			int len = 0, i = 0;
			for ( ; i < (count - 1); i++)
			{
				if (len > pLinkStretch->m_dPosInStretch)
					break;
				len += drawpath.getPoint(i).distance3D( drawpath.getPoint(i+1) );
			}

			//calculate the displacement of the mouse in direction of  the stretch(v) 
			
			ARCVector3 v = drawpath.GetIndexDir(i);//(drawpath[i] - drawpath[i-1]);
			v.Normalize();
			int d = mousediff.dotProduct( OGREVECTOR(v) );	

			//calculate the displacement of the mouse in direction be perpendicular to the stretch
			ARCVector3 p(v.y, -v.x, v.z);  //degree(v) - 90
			p.Normalize();
			Vector3 dRay = curMouseInfo.mWorldPos - m_lastCrossPoint;
			int dp = dRay.dotProduct(OGREVECTOR(p));

			//now determine the increase of number of lane  accord to the sign 
			//		  stretch  ^ 
			//		|		   |           |  
			//		|		   |           |
			//		|		   |     dp    |
			//		|		   |  -------> |
			//		|		   |           |
			//		|		   |           |
			//  from|--------------------->|to
			//	 top|--------------------->|bottom 

			if( dp > pStretch->getLaneWidth())
				dp = 1;
			else if( dp < -pStretch->getLaneWidth())
				dp = -1;
			else
				dp = 0;

			if( dp != 0)
				m_lastCrossPoint = curMouseInfo.mWorldPos; 

			double lanNum = pStretch->getLaneNum();


			if (idx == 0)  // left point
			{
				pLinkStretch->m_dPosInStretch += d;

				if (pLinkStretch->m_dPosInStretch < 0)
					pLinkStretch->m_dPosInStretch = 0;
				else
					pLinkStretch->m_dLength -= d;

				if (pLinkStretch->m_dLength < 500)
					pLinkStretch->m_dLength = 500;
				if (pLinkStretch->m_dPosInStretch + pLinkStretch->m_dLength > pathLen)
					pLinkStretch->m_dPosInStretch = pathLen - pLinkStretch->m_dLength;
				if (pLinkStretch->m_dPosInStretch < 0)
					pLinkStretch->m_dPosInStretch = 0;
			}
			if (idx == 1) // right point
			{
				pLinkStretch->m_dLength += d;
				if (pLinkStretch->m_dLength < 500) {
					pLinkStretch->m_dLength = 500;
					pLinkStretch->m_dPosInStretch += d;
				}
				if (pLinkStretch->m_dPosInStretch < 0)
					pLinkStretch->m_dPosInStretch = 0;
				if (pLinkStretch->m_dPosInStretch + pLinkStretch->m_dLength > pathLen)
					pLinkStretch->m_dLength = pathLen - pLinkStretch->m_dPosInStretch;
				if (pLinkStretch->m_dLength < 500)
					pLinkStretch->m_dLength = 500;
			}
			if (idx == 2) //bottom point
			{
				pLinkStretch->m_nLaneTo += dp;
				if( pLinkStretch->m_nLaneTo > lanNum)
					pLinkStretch->m_nLaneTo = lanNum;
				if(pLinkStretch->m_nLaneTo <= pLinkStretch->m_nLaneFrom )
					pLinkStretch->m_nLaneTo = pLinkStretch->m_nLaneFrom;
			}
			if (idx == 3) //top point
			{
				pLinkStretch->m_nLaneFrom += dp;
				if(pLinkStretch->m_nLaneFrom < 1)
					pLinkStretch->m_nLaneFrom = 1;
				if (pLinkStretch->m_nLaneFrom >= pLinkStretch->m_nLaneTo)
					pLinkStretch->m_nLaneFrom = pLinkStretch->m_nLaneTo;
			}
		}
		break;

	default:
		ASSERT(FALSE);
		break;
	}

	plandsideCtx->NotifyViewCurentOperation();
}

void LandsideLinkStretchControlPoint3D::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}

void LandsideLinkStretchControlPoint3D::SetQueryData( LandsideLinkStretchObject* pLinkStretchObject, int idx, Type t /*= CP_DEFAULT*/ )
{
	if(SceneNodeQueryData* pData = InitQueryData()) 
	{
		pData->SetData(KeyAltObjectPointer,(int)pLinkStretchObject);
		pData->SetData(KeyControlPointIndex, idx);
		pData->SetData(KeyCurbCtrlPtType,t);	
		pData->SetData(KeyLandsideItemType,_CurbSideControlPoint);
	}
}

int LandsideLinkStretchControlPoint3D::GetControlPointIndex()
{
	int PtIdx = -1;
	if(SceneNodeQueryData* pData = getQueryData())
	{
		pData->GetData(KeyControlPointIndex,PtIdx);
	}
	return PtIdx;
}

LandsideLinkStretchControlPoint3D::Type LandsideLinkStretchControlPoint3D::GetControlPointType()
{
	int t = CP_DEFAULT;
	if (SceneNodeQueryData* pData = getQueryData())
	{
		pData->GetData(KeyCurbCtrlPtType, t);
	}
	return static_cast<LandsideLinkStretchControlPoint3D::Type>( t );
}

LandsideLinkStretchObject* LandsideLinkStretchControlPoint3D::GetLandsideLinkStretchObject() const
{
	LandsideLinkStretchObject* pLinkStretch  = NULL;
	if(SceneNodeQueryData* pDatat = getQueryData())
	{
		pDatat->GetData(KeyAltObjectPointer,(int&)pLinkStretch);
	}
	return pLinkStretch;
}


PROTYPE_INSTANCE(LandsideLinkStretchControlPath3D)
void LandsideLinkStretchControlPath3D::Update3D( LandsideLinkStretchObject* pLinkStretch,const CPoint2008& center )
{
	SetQueryData(pLinkStretch);

	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());
	pObj->clear();


	CPath2008 tempPath = pLinkStretch->m_PickArea;
	tempPath.insertPointAfterAt(pLinkStretch->m_PickArea[0], tempPath.getCount()-1);
	for (int i = 0; i < tempPath.getCount(); i++)
	{
		tempPath[i] = tempPath[i] - center;
		tempPath[i].z = 0;
	}

	CShapeBuilder::DrawPath(pObj,VERTEXCOLOR_LIGTHOFF,tempPath,ARCColor3(255,0,0));
	AddObject(pObj, 2);

	RemoveChild();
	for (int i = 0; i < tempPath.getCount()-1; i++)
	{
		LandsideLinkStretchControlPoint3D nodePoint3D = GetControlPoint3D(i);
		nodePoint3D.SetQueryData( GetLandsideLinkStretchObject(), i, LandsideLinkStretchControlPoint3D::CP_PICKAREA);
		nodePoint3D.SetPosition( tempPath.getPoint(i)  );
		nodePoint3D.Update3D();
		nodePoint3D.AttachTo(*this);
	}
}

LandsideLinkStretchObject* LandsideLinkStretchControlPath3D::GetLandsideLinkStretchObject()
{
	LandsideLinkStretchObject* pLinkStretch = NULL;
	if(SceneNodeQueryData* pData = getQueryData())
	{
		pData->GetData(KeyAltObjectPointer,(int&)pLinkStretch);
	}
	return pLinkStretch;
}

void LandsideLinkStretchControlPath3D::SetQueryData( LandsideLinkStretchObject*pLinkStretch )
{
	if(SceneNodeQueryData* pData =InitQueryData())
	{
		pData->SetData(KeyLandsideItemType,_CurbSideControlPath);
		pData->SetData(KeyAltObjectPointer,(int)pLinkStretch);
	}
}

LandsideLinkStretchControlPoint3D LandsideLinkStretchControlPath3D::GetControlPoint3D( int idx )
{
	CString sPointName;
	sPointName.Format(_T("%s%d"), GetIDName(), idx);
	return OgreUtil::createOrRetrieveSceneNode(sPointName.GetString(), GetScene());
}

int LandsideLinkStretchControlPath3D::AddControlPoint( Vector3 position )
{
	return -1;
}

int LandsideLinkStretchControlPath3D::RemoveControlPoint( Vector3 position )
{
	return -1;
}

void LandsideLinkStretchControlPath3D::OnRButtonDown( const MouseSceneInfo& mouseInfo )
{
	LandsideLinkStretchObject* pLinkStretch = GetLandsideLinkStretchObject();	

	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData( KeyRClickWorldPos, ARCVECTOR(mouseInfo.mWorldPos) );
	}
}

PROTYPE_INSTANCE(LandsideLinkStretchPickArea3D)
void LandsideLinkStretchPickArea3D::OnUpdate3D( const CPoint2008& ptCenter )
{
	LandsideLinkStretchObject* pLinkStretch = getLinkStretchObject();


	//the display property(eg. colour) was store in the LandsideCurbside::mDspProp
	ILayoutObjectDisplay::DspProp& dspShape = pLinkStretch->getDisplayProp().GetProp(ILayoutObjectDisplay::_Shape);

	ManualObject* p3DObj = OgreUtil::createOrRetrieveManualObject(GetIDName(),GetScene());		
	p3DObj->clear();

	//double hight = GetStretchHight();
	CPoint2008* pointlist = pLinkStretch->m_PickArea.getPointList();
	CPath2008 child_space_path(pLinkStretch->m_PickArea);
	CPoint2008* child_space_points = child_space_path.getPointList();

	for (int i = 0; i < pLinkStretch->m_PickArea.getCount(); i++)
	{
		child_space_points[i] = pointlist[i] - ptCenter; 
		child_space_points[i].z = 0;
	}


	CShapeBuilder::DrawArea( p3DObj,LINKSTRETCH_MAT, child_space_path,ARCColor3(dspShape.cColor) );

	UpdateControlPath3D(pLinkStretch,ptCenter);

	AddObject(p3DObj,2);	
}

void LandsideLinkStretchPickArea3D::SetQueryData( LandsideLinkStretchObject* pLinkStretchObject )
{
	if(SceneNodeQueryData* pData =InitQueryData())
	{
		pData->SetData(KeyLandsideItemType,_LayoutObject);
		pData->SetData(KeyAltObjectPointer,(int)pLinkStretchObject);
	}
}

LandsideLinkStretchObject* LandsideLinkStretchPickArea3D::getLinkStretchObject()
{
	LandsideLinkStretchObject* pLinkStretchObject = NULL;

	if(SceneNodeQueryData* pData = getQueryData())
	{
		pData->GetData(KeyAltObjectPointer,(int&)pLinkStretchObject);
	}
	return pLinkStretchObject;
}

void LandsideLinkStretchPickArea3D::StartDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideLinkStretchObject* pLinkStretch = getLinkStretchObject();


	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pLinkStretch, plandsideCtx->getInput());	
	plandsideCtx->StartEditOp(pModCmd);
	SetInDragging(TRUE);

	OnDrag(lastMouseInfo,curMouseInfo);
}

void LandsideLinkStretchPickArea3D::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	CRenderScene3DNode(this->GetParent()).OnDrag(lastMouseInfo,curMouseInfo);
	//plandsideCtx->NotifyViewCurentOperation();
}

void LandsideLinkStretchPickArea3D::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->NotifyViewCurentOperation();
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}

bool LandsideLinkStretchPickArea3D::IsDraggable() const
{
	return true;
}

void LandsideLinkStretchPickArea3D::OnUpdateEditMode( BOOL b )
{
	SetEditMode(b);	
	CPoint2008 center = CPoint2008(GetParent().GetPosition());
	OnUpdate3D(center);
}

LandsideLinkStretchControlPath3D LandsideLinkStretchPickArea3D::GetControlPath3D()
{
	CString sPathName = GetIDName() + "ControlPath";
	return OgreUtil::createOrRetrieveSceneNode(sPathName.GetString(), GetScene());
}

void LandsideLinkStretchPickArea3D::UpdateControlPath3D( LandsideLinkStretchObject* pLinkStretchObject, const CPoint2008& center )
{
	LandsideLinkStretchControlPath3D controlPath = GetControlPath3D();

	if(IsInEditMode())
	{		
		controlPath.Update3D(pLinkStretchObject,center);
		controlPath.AttachTo(*this);
	}
	else 
		controlPath.Detach();

}
