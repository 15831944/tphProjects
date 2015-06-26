#include "StdAfx.h"
#include ".\LandsideControlDevice3D.h"
#include "landside\LandsideControlDevice.h"
#include "CommonShapes.h"
#include <common/ARCPipe.h>
#include "MaterialDef.h"
#include "LandsideStretch3D.h"
#include "Common/BizierCurve.h"
#include "Landside/LandsideStretch.h"
#include "SceneNodeQueryData.h"
#include "landside/ILandsideEditContext.h"
#include "Render3DScene.h"
#include "SceneState.h"
#include "landside/LandsideEditCommand.h"
#include "landside/LandsideQueryData.h"
#include "Landside/LandsideControlDevice.h"
#include "ManualObjectUtil.h"

using namespace Ogre;
#define STRETCH_BASE_MAT_SEL "Stretch/RoadBaseTransparent"
#define STRETCH_BASE_MAT "Stretch/RoadBase"

PROTYPE_INSTANCE(LandsideControlDevice3D)
void LandsideControlDevice3D::OnUpdate3D()
{	

	UpdateShape(IsInEditMode());
}


void LandsideControlDevice3D::OnUpdateEditMode( BOOL b )
{	
	UpdateShape(b);
}



void LandsideControlDevice3D::StartDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;
	LandsideControlDevice* pLinkStretchObject = (LandsideControlDevice*)getLinkStretchObject();
	if (pLinkStretchObject == NULL)
		return;


	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pLinkStretchObject, plandsideCtx->getInput());	
	plandsideCtx->StartEditOp(pModCmd);
	SetInDragging(TRUE);

	OnDrag(lastMouseInfo,curMouseInfo);
}

void LandsideControlDevice3D::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->NotifyViewCurentOperation();
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}

void LandsideControlDevice3D::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;
	LandsideControlDevice* pLinkStretchObject = (LandsideControlDevice*)getLinkStretchObject();
	if (pLinkStretchObject == NULL)
		return;

	LandsideStretch* pStretch = pLinkStretchObject->getStretch();

	if (pStretch == NULL)
		return;
	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;

	CPath2008& drawpath = pStretch->getControlPath().getBuildPath();
	int count = drawpath.getCount();
	ASSERT(count > 0);
	int len = 0, i = 1;
	for ( ; i < (count - 1); i++)
	{
		if (len > pLinkStretchObject->GetPosOnStretch())
			break;
		len += drawpath.getPoint(i).distance3D( drawpath.getPoint(i+1) );
	}

	//calculate the displacement in direction of the stretch
	ARCVector3 v(drawpath[i] - drawpath[i-1]);
	v.Normalize();
	int d = mousediff.dotProduct( OGREVECTOR(v) );	
	int Dis = pLinkStretchObject->GetPosOnStretch();;
	Dis +=  d;
	pLinkStretchObject->setPosOnStretch((double)Dis);
	//border condition
	if (pLinkStretchObject->GetPosOnStretch() < 0)
		pLinkStretchObject->setPosOnStretch(0);
	plandsideCtx->NotifyViewCurentOperation();	
}
#define  _TRAFFICLIGHT_MODEL _T("Trafficlight.mesh")
#define  _STOPSIGN_MODEL _T("stopsign.mesh")
#define  _YIELDSIGN_MODEL _T("yield.mesh")
#define	 _TOLLGATE_MODEL _T("toll_station.mesh")
#define  _SPEEDSING_MODEL _T("speedsign.mesh")

CString getModel(ALTOBJECT_TYPE t)
{
	if(t == ALT_LYIELDSIGN)
		return _YIELDSIGN_MODEL;
	if(t == ALT_LSTOPSIGN)
		return _STOPSIGN_MODEL;
	if(t == ALT_LTRAFFICLIGHT)
		return _TRAFFICLIGHT_MODEL;
	if(t == ALT_LTOLLGATE)
		return _TOLLGATE_MODEL;
	if(t == ALT_LSPEEDSIGN)
		return _SPEEDSING_MODEL;

	return _STOPSIGN_MODEL;
}

void LandsideControlDevice3D::UpdateShape( BOOL bEdit )
{
	ILayoutObject* pObj = getLayOutObject();
	if(!pObj)
		return;
	LandsideControlDevice* pDevice =(LandsideControlDevice*)pObj;
	ILandsideEditContext* pCtx = GetLandsideEditContext();


	CPoint2008 pos; ARCVector3 dir;
	if( pDevice->GetPosition(pos) && pDevice->GetDirection(dir) ){
		pCtx->ConvertToVisualPos(pos);

		SetPosition( ARCVector3(pos.x,pos.y,pos.z) );
		SetDirection( ARCVector3(dir.x,dir.y,dir.z), ARCVector3(0,0,1) );
	}	
	
	CString sEntity;
	sEntity.Format(_T("ControlDevice-%d"),pObj);
	
	ALTOBJECT_TYPE ntype = pObj->GetType();
	if (ntype == ALT_LSPEEDSIGN)
	{
		ManualObject* pObj = OgreUtil::createOrRetrieveManualObject(GetName()+"number",GetScene());
		pObj->clear();
		ManualObjectUtil drawobj(pObj);
		drawobj.beginText(30);
		drawobj.setColor(ARCColor4(0,0,0,255));
		dir.Normalize();
		drawobj.rotate(ARCVector3::UNIT_Z,180);
		drawobj.rotate(ARCVector3::UNIT_X,-90);
		drawobj.scale(ARCVector3(1,1,1));
		drawobj.setTranslate(ARCVector3(0,15,300));	
		LandsideSpeedSign* pSpeedSign = pDevice->toSpeedSign();
		double dSpeed = pSpeedSign->m_dSpeedLimit;
		CString sSpeed;
		sSpeed.Format("%.0lf",dSpeed);    
		drawobj.drawCenterAligmentText(sSpeed);
		drawobj.end();
		if(pDevice->IsNewObject())
			AddObject(pObj,2);
		else
			AddObject(pObj,1);
	}

	{
		Entity* pEnt = OgreUtil::createOrRetrieveEntity(sEntity,getModel(ntype), GetScene() );
		if(pDevice->IsNewObject())
			AddObject(pEnt,2);
		else
			AddObject(pEnt,1);
	}	
	
}

LandsideControlDevice* LandsideControlDevice3D::getLinkStretchObject()
{
	LandsideControlDevice* pLinkStretchObject = NULL;

	if(SceneNodeQueryData* pData = getQueryData())
	{
		pData->GetData(KeyAltObjectPointer,(int&)pLinkStretchObject);
	}
	return pLinkStretchObject;
}
