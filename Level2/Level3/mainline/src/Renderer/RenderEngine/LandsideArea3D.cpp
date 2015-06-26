#include "StdAfx.h"
#include "LandsideArea3D.h"
#include "Renderer\RenderEngine\SceneNodeQueryData.h"
#include "Render3DScene.h"
#include "Landside/ILandsideEditContext.h"
#include "Landside/LandsideEditCommand.h"
#include "SceneState.h"
#include "Landside\landsideArea.h"
#include "OgreConvert.h"
#include "Landside/InputLandside.h"
#define AREA_PORTTAL_MAT _T("GridLine")

PROTYPE_INSTANCE(LandsideArea3D)
void LandsideArea3D::Update3D( CLandsideArea *pLandsideArea )
{
	if(SceneNodeQueryData* pdata = InitQueryData())
	{
		pdata->SetData("AreaPointer",(int)pLandsideArea);
	}

	OnUpdate3D();
}

void LandsideArea3D::OnUpdate3D()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	CLandsideArea *pArea=GetArea();
	if (!pArea)
	{
		return;
	}

	CString strAreaObj=_T("Area-")+pArea->name;
	ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(strAreaObj,GetScene());	
	
	CPath2008 path = pArea->m_path;
	plandsideCtx->ConvertToVisualPath(path);
	const std::vector<CPoint2008>& orgPoints = path.points;
	pObj->clear();
	if (orgPoints.size())
	{
		pObj->begin( AREA_PORTTAL_MAT, RenderOperation::OT_LINE_STRIP);
		pObj->colour(OgreConvert::GetColor(pArea->color));
		for (std::vector<CPoint2008>::const_iterator ite = orgPoints.begin();ite != orgPoints.end();ite++)
		{
			pObj->position(ite->x, ite->y, ite->z);
		}
		pObj->position(orgPoints[0].x, orgPoints[0].y, orgPoints[0].z);
		pObj->colour(OgreConvert::GetColor(pArea->color));
		pObj->end();
	}

	pObj->setVisibilityFlags(NormalVisibleFlag);
	AddObject(pObj,1);
}

CLandsideArea * LandsideArea3D::GetArea()
{
	if(SceneNodeQueryData* pdata = getQueryData())
	{
		CLandsideArea* pObj = NULL;
		if(pdata->GetData("AreaPointer",(int&)pObj))
		{
			return pObj;
		}
	}
	return NULL;
}

void LandsideArea3D::StartDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	CLandsideArea* pLandsideArea = GetArea();		
	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pLandsideArea, plandsideCtx->getInput());
	//pModCmd->m_strName = _T("Area-")+pLandsideArea->name;
	plandsideCtx->StartEditOp(pModCmd);
	SetInDragging(TRUE);
	OnDrag(lastMouseInfo,curMouseInfo);
}

void LandsideArea3D::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	CLandsideArea* pLandsideArea = GetArea();	
	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;

	for (int i = 0; i < (int)pLandsideArea->points.size(); i++)
	{
		pLandsideArea->points.at(i).DoOffset(mousediff.x, mousediff.y);
	}
	pLandsideArea->m_path.clear();
	pLandsideArea->m_path.init((int)pLandsideArea->points.size());	
	for (int i=0;i<(int)pLandsideArea->points.size();i++)
	{				
		pLandsideArea->m_path[i].setPoint((DistanceUnit)pLandsideArea->points.at(i)[VX],(DistanceUnit)pLandsideArea->points.at(i)[VY],(DistanceUnit)pLandsideArea->floor);
	}		
	plandsideCtx->NotifyViewCurentOperation();
}

void LandsideArea3D::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->DoneCurEditOp();
	plandsideCtx->getInput()->GetLandsideAreas().SaveData(-1);
	SetInDragging(FALSE);
}
