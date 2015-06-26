#include "StdAfx.h"
#include ".\landsidestretchsegment3d.h"
#include ".\landside\LandsideStretchSegment.h"
#include "RenderFloor3D.h"
#include "Render3DScene.h"
#include "Landside/LandsideQueryData.h"
#include ".\SceneNodeQueryData.h"
#include "Tesselator.h"
#include "Landside/ILandsideEditContext.h"
#include "Landside/LandsideEditCommand.h"
#include "LandsideCurbside3D.h"
#include "LandsideLinkStretchArea.h"

//get a sub part of the stretch(has the same shape, curve, line... as the selected part of stretch)
//void GetStretchSegPartPipe(const CPath2008& center, double dwidth, double distFrom, double distTo, double dwidthFrom, double dwidthTo , ARCPipe& out, double dZupper = 5 )
//{
//	out.m_centerPath.clear();
//	out.m_sidePath1.clear();
//	out.m_sidePath2.clear();
//
//	ARCPoint3 upLeve(0,0,dZupper);
//	double dMaxLength = center.GetTotalLength();
//	distFrom = MIN(dMaxLength,distFrom); distFrom = MAX(0,distFrom);
//	distTo = MIN(dMaxLength,distTo); distTo = MAX(0,distTo);
//	CPath2008 subCenter = center.GetSubPath(distFrom,distTo);
//
//	out = ARCPipe(subCenter,dwidth);
//
//	for(int i=0;i<subCenter.getCount();i++)
//	{
//		ARCPoint3 ptL = out.m_sidePath1[i];
//		ARCPoint3 ptR = out.m_sidePath2[i];
//
//		double dRatFrom = dwidthFrom/dwidth;
//		double dRatTo = dwidthTo/dwidth;
//		dRatFrom = MIN(1.0,dRatFrom);
//		dRatFrom = MAX(0,dRatFrom);
//
//		dRatTo = MIN(1.0,dRatTo);
//		dRatTo = MAX(0,dRatTo);
//
//
//		ARCPoint3 ptLeftout = ptL * (1-dRatFrom) + ptR * dRatFrom; 
//		ARCPoint3 ptRightout = ptL * (1-dRatTo) + ptR * dRatTo;
//
//		out.m_sidePath1[i] = (ptLeftout) + upLeve;
//		out.m_sidePath2[i] = (ptRightout) + upLeve;		
//	}
//
//}
//
PROTYPE_INSTANCE (LandsideStretchSegment3D)

void LandsideStretchSegment3D::OnUpdate3D()
{
	LandsideStretchSegment* pStretchSegment = getStretchSegment();	

	ILayoutObjectDisplay::DspProp& dspShape = pStretchSegment->getDisplayProp().GetProp(ILayoutObjectDisplay::_Shape);


	//build the area on the stretch
	if(dspShape.bOn)
	{
		//build  the area picked on the stretch
		CString sStretchArea = GetIDName() + "StretchAreaSeg";
		LandsideLinkStretchArea3D stretchAreaNode = OgreUtil::createOrRetrieveSceneNode(sStretchArea, GetScene());
		stretchAreaNode.SetQueryData(pStretchSegment);
		stretchAreaNode.Detach();


		CPoint2008 center;
		if( stretchAreaNode.UpdateDraw(center) )
		{
			stretchAreaNode.AttachTo(*this);
			SetPosition(ARCVector3(center));	
		}		
	}
}

void LandsideStretchSegment3D::OnUpdateEditMode( BOOL b )
{
	/*LandsideStretchSegment* pStretchSegment = getStretchSegment();	
	LandsideStretch * pStretch = pStretchSegment->getStrech();
	if(pStretch)
	{
		CPath2008& drawpath = pStretch->getControlPath().getBuildPath();
		double dWidth = pStretch->getLaneWidth()*pStretch->getLaneNum();
		double dLaneWidth = pStretch->getLaneWidth();		
		ARCPipe partPipe;
		GetStretchSegPartPipe(drawpath,dWidth,pStretchSegment->getDistFrom(),pStretchSegment->getDistTo(),pStretchSegment->GetLaneFrom()*dLaneWidth,(pStretchSegment->GetLaneTo()+1)*dLaneWidth,partPipe);
	}*/
}

void LandsideStretchSegment3D::StartDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideStretchSegment* pStretchSegment = getStretchSegment();	
	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pStretchSegment, plandsideCtx->getInput());	
	plandsideCtx->StartEditOp(pModCmd);
	SetInDragging(TRUE);

	OnDrag(lastMouseInfo,curMouseInfo);
}

void LandsideStretchSegment3D::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideStretchSegment* pCurb = getStretchSegment();	
	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;
}

void LandsideStretchSegment3D::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->NotifyViewCurentOperation();
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}

LandsideStretchSegment* LandsideStretchSegment3D::getStretchSegment()
{
	return (LandsideStretchSegment*)getLayOutObject();
}