#include "StdAfx.h"
#include ".\landsideportal3d.h"
#include "Landside\LandsidePortal.h"
#include "Renderer\RenderEngine\SceneNodeQueryData.h"
#include "./ShapeBuilder.h"
#include "MaterialDef.h"
#include "OgreConvert.h"
#include "SceneState.h"
#include "Landside/ILandsideEditContext.h"
#include "Render3DScene.h"
#include "Landside/LandsideEditCommand.h"
#include "landside/InputLandside.h"
using namespace Ogre;

#define AREA_PORTTAL_MAT _T("GridLine")

PROTYPE_INSTANCE(LandsidePortal3D)
void LandsidePortal3D::Update3D(CLandsidePortal *pPortal)
{
	if(SceneNodeQueryData* pdata = InitQueryData())
	{
		pdata->SetData("PortalPointer",(int)pPortal);
	}

	OnUpdate3D();
}
void LandsidePortal3D::OnUpdate3D()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	CLandsidePortal *pPortal=GetPortal();
	if (!pPortal)
	{
		return;
	}

	CString strPortalObj=_T("Portal-")+pPortal->name;
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(strPortalObj,GetScene());	

	CPath2008 path = pPortal->m_path;
	plandsideCtx->ConvertToVisualPath(path);
	const std::vector<CPoint2008>& orgPoints = path.points;
	pObj->clear();
	if (orgPoints.size())
	{
		pObj->begin( AREA_PORTTAL_MAT, RenderOperation::OT_LINE_STRIP);
		pObj->colour(OgreConvert::GetColor(pPortal->color));
		for (std::vector<CPoint2008>::const_iterator ite = orgPoints.begin();ite != orgPoints.end();ite++)
		{
			pObj->position(ite->x, ite->y, ite->z);
		}
		//pObj->position(orgPoints.front().x, orgPoints.front().y, 0.0f);
		pObj->end();
	}

	pObj->setVisibilityFlags(NormalVisibleFlag);
	AddObject(pObj,2);
}

CLandsidePortal *LandsidePortal3D::GetPortal()
{
	if(SceneNodeQueryData* pdata = getQueryData())
	{
		CLandsidePortal* pObj = NULL;
		if(pdata->GetData("PortalPointer",(int&)pObj))
		{
			return pObj;
		}
	}
	return NULL;
}

void LandsidePortal3D::StartDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo)
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	CLandsidePortal* pPortal = GetPortal();		
	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(static_cast<CLandsidePortal*>(pPortal), plandsideCtx->getInput());
	pModCmd->m_strName = _T("Portal-")+pPortal->name;
	plandsideCtx->StartEditOp(pModCmd);
	SetInDragging(TRUE);
	OnDrag(lastMouseInfo,curMouseInfo);
}

void LandsidePortal3D::OnDrag(const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	CLandsidePortal* pPortal = GetPortal();	
	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;

	for (int i = 0; i < (int)pPortal->points.size(); i++)
	{
		pPortal->points.at(i).DoOffset(mousediff.x, mousediff.y);
	}
	pPortal->m_path.clear();
	pPortal->m_path.init((int)pPortal->points.size());	
	for (int i=0;i<(int)pPortal->points.size();i++)
	{				
		pPortal->m_path[i].setPoint((DistanceUnit)pPortal->points.at(i)[VX],(DistanceUnit)pPortal->points.at(i)[VY],(DistanceUnit)pPortal->floor);
	}		
	plandsideCtx->NotifyViewCurentOperation();
}

void LandsidePortal3D::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->DoneCurEditOp();
	plandsideCtx->getInput()->GetPortals().SaveData(-1);
	SetInDragging(FALSE);
}