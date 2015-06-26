#include "StdAfx.h"
#include ".\landsidedecisionpoint3d.h"
#include "landside\LandsideControlDevice.h"
#include "landside/LandsideEditCommand.h"
#include "landside/LandsideQueryData.h"
#include "Landside/LandsideStretch.h"
#include "SceneNodeQueryData.h"
#include "landside/ILandsideEditContext.h"
#include "Render3DScene.h"
#include "SceneState.h"
#include "Common/ARCPipe.h"
#include "Landside/SmallFuncAndMicro.h"

PROTYPE_INSTANCE(LandsideDecisionPoint3D)

void LandsideDecisionPoint3D::OnUpdate3D()
{
	UpdateShape(IsInEditMode());
}

void LandsideDecisionPoint3D::OnUpdateEditMode( BOOL b )
{
	UpdateShape(b);
}

void LandsideDecisionPoint3D::StartDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;
	LandsideDecisionPoint* pLinkStretchObject = (LandsideDecisionPoint*)getLinkStretchObject();
	if (pLinkStretchObject == NULL)
		return;


	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pLinkStretchObject, plandsideCtx->getInput());	
	plandsideCtx->StartEditOp(pModCmd);
	SetInDragging(TRUE);

	OnDrag(lastMouseInfo,curMouseInfo);
}

void LandsideDecisionPoint3D::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideDecisionPoint* pLinkStretchObject = getLinkStretchObject();	
	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;

	pLinkStretchObject->DoOffset(ARCVector3(mousediff.x, mousediff.y, mousediff.z));

	plandsideCtx->NotifyViewCurentOperation();
}

void LandsideDecisionPoint3D::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->NotifyViewCurentOperation();
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}

LandsideDecisionPoint* LandsideDecisionPoint3D::getLinkStretchObject()
{
	LandsideDecisionPoint* pLinkStretchObject = NULL;

	if(SceneNodeQueryData* pData = getQueryData())
	{
		pData->GetData(KeyAltObjectPointer,(int&)pLinkStretchObject);
	}
	return pLinkStretchObject;
}

void LandsideDecisionPoint3D::UpdateShape( BOOL bEdit )
{
	RemoveChild();
	LandsideDecisionPoint* pPoint = getLinkStretchObject();
	LandsideStretch* pStretch = pPoint->getStretch();
	if(!pPoint || !pStretch)	return;

	ILandsideEditContext* pctx = GetLandsideEditContext();
	if(!pctx)return;

	ILayoutObjectDisplay::DspProp& dspShape = pPoint->getDisplayProp().GetProp(ILayoutObjectDisplay::_Shape);

	BuildSidePath();
	CPath2008 leftPath = m_leftPath;
	CPath2008 rightPath = m_rightPath;
	pctx->ConvertToVisualPath(leftPath);
	pctx->ConvertToVisualPath(rightPath);
	CPoint2008 ptCenter = leftPath[0]+leftPath[1] + rightPath[0] + rightPath[1];
	ptCenter = ptCenter * 0.25;

	SetPosition(ARCVector3(ptCenter));

	RemoveAllObject();
	//render	
	if(dspShape.bOn)
	{
		//update shape
		ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetIDName(), GetScene());
		pObj->clear();
		
		MaterialPtr matPtr = OgreUtil::createOrRetrieveColorMaterial(dspShape.cColor);	
		pObj->begin(matPtr->getName(),RenderOperation::OT_TRIANGLE_STRIP);

		leftPath.DoOffset(-ptCenter.x, -ptCenter.y, -ptCenter.z + 10);
		rightPath.DoOffset(-ptCenter.x, -ptCenter.y, -ptCenter.z + 10);

		double dMatWidth = pStretch->getLaneNum()*pStretch->getLaneWidth()/64;
		double dMatLen = 5.0 / 256;
		Vector3 prepos1 = OGREVECTOR(leftPath[0]);
		Vector3 prepos2 = OGREVECTOR(leftPath[1]);

		Vector3 pos1 = OGREVECTOR(rightPath[0]);
		Vector3 pos2 = OGREVECTOR(rightPath[1]);
		Vector3 norm = (pos2-pos1).crossProduct(pos1-prepos1).normalisedCopy();

		Vector3 dir=OGREVECTOR(m_centerPath[1]-m_centerPath[0]);

		pObj->position(prepos1);pObj->textureCoord(dMatWidth,0);pObj->normal(norm);
		pObj->position(prepos2);pObj->textureCoord(0,0);

		pObj->position(pos1);pObj->textureCoord(dMatWidth,dMatLen);
		pObj->position(pos2);pObj->textureCoord(0,dMatLen);

		pObj->end();

		AddObject(pObj, 2);

	}
}

void LandsideDecisionPoint3D::BuildSidePath()
{
	LandsideDecisionPoint* pPoint = getLinkStretchObject();
	LandsideStretch* pStretch = pPoint->getStretch();
	if(pStretch == NULL)
		return;

	double dPosOnStretch = pPoint->GetPosOnStretch();
	double dWidth = 30.0;//default
	CPath2008 subPath = pStretch->getControlPath().getBuildPath().GetSubPath(dPosOnStretch, dPosOnStretch+dWidth);
	ARCPipe pipe(subPath, pStretch->getLaneNum()*pStretch->getLaneWidth());

	m_leftPath.init(2);m_rightPath.init(2);
	int count = (int)pipe.m_centerPath.size();
	ARCVECTOR2POINT(m_leftPath[0], pipe.m_sidePath1[0]);
	ARCVECTOR2POINT(m_leftPath[1], pipe.m_sidePath2[0]);
	ARCVECTOR2POINT(m_rightPath[0], pipe.m_sidePath1[count-1]);
	ARCVECTOR2POINT(m_rightPath[1], pipe.m_sidePath2[count-1]);

	m_centerPath.init(2);
	CPoint2008 startPos = (m_leftPath[0] + m_rightPath[0]);
	CPoint2008 endPos = (m_leftPath[1] + m_rightPath[1]);
	m_centerPath[0].init(startPos.getX()/2,startPos.getY()/2,startPos.getZ()/2);
	m_centerPath[1].init(endPos.getX()/2,endPos.getY()/2,endPos.getZ()/2);
}
