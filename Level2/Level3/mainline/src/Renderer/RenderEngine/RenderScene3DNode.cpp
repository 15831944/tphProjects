#include "StdAfx.h"
#include "./SceneNodeQueryData.h"
#include ".\renderscene3dnode.h"
#include "Landside/ILandsideEditContext.h"
#include <landside/LandsideQueryData.h>
#include <CommonData/3DViewCommon.h>
#include "Render3DScene.h"
#include "..\..\common\IARCportLayoutEditContext.h"

using namespace Ogre;


//////////////////////////////////////////////////////////////////////////
const static CString nameRenderScene = "RenderScene";
CRender3DScene* CRenderSceneRootNode::getRenderScene()
{
	CRender3DScene* const * pPtr  = any_cast<CRender3DScene*>(&GetAny());
	if(pPtr)
	{
		return *pPtr;
	}	
	return NULL;
}

void CRenderSceneRootNode::setRenderScene( CRender3DScene* pQuerier )
{
	SetAny(Any(pQuerier));
}
//////////////////////////////////////////////////////////////////////////
PROTYPE_INSTANCE(CRenderScene3DNode)

SceneNodeQueryData* CRenderScene3DNode::getQueryData()const
{
	return const_cast<SceneNodeQueryData*>(any_cast<SceneNodeQueryData>(&GetAny()));
}


SceneNodeQueryData* CRenderScene3DNode::InitQueryData( bool bForceInit /*= false*/ )
{
	SceneNodeQueryData* pData = getQueryData();
	if(!pData || bForceInit )
	{
		SceneNodeQueryData qdata;
		qdata.mpNodeDynamic = getProType();
		mpNode->setUserAny(Any(qdata));		
		return getQueryData();
	}	
	return pData;
}

CRender3DScene* CRenderScene3DNode::getRenderScene()const
{
	if(!GetScene())
		return NULL;

	CRenderSceneRootNode rootNode = GetScene()->getRootSceneNode();
	if(rootNode)
	{
		return rootNode.getRenderScene();
	}
	ASSERT(FALSE); //please set root render scene in setup
	return NULL;
}

CRenderScene3DNode* CRenderScene3DNode::DynamicCast()const
{
	if(SceneNodeQueryData* pData = getQueryData() )
	{
		if(CRenderScene3DNode* pDynamic = pData->mpNodeDynamic)
		{
			*pDynamic = *this;
			return pDynamic;
		}
	}
	ProType = *this;
	return &ProType;
}

#define KeyIsInDragging "IsInDragging"
BOOL CRenderScene3DNode::IsInDragging() const
{
	BOOL b = FALSE;
	if(SceneNodeQueryData* pdata = getQueryData())
	{
		pdata->GetData(KeyIsInDragging,b);
	}
	return b;
}

void CRenderScene3DNode::SetInDragging( BOOL b )
{
	if(SceneNodeQueryData* pdata = InitQueryData())
	{
		pdata->SetData(KeyIsInDragging,b);
	}
}

void CRenderScene3DNode::SetEditMode( BOOL b )
{
	if(SceneNodeQueryData* pdata = InitQueryData() )
	{
		pdata->SetData(KeyIsInEditMode,b);
	}
}	



BOOL CRenderScene3DNode::IsInEditMode() const
{
	BOOL b =FALSE;
	if(SceneNodeQueryData* pdata = getQueryData())
	{
		pdata->GetData(KeyIsInEditMode,b);
	}
	return b;
}

void CRenderScene3DNode::FlipEditMode()
{
	BOOL bInEdit = IsInEditMode();
	DynamicCast()->OnUpdateEditMode(!bInEdit);
	SetEditMode(!bInEdit);
}

//SceneNodeType CRenderScene3DNode::GetSceneNodeType()
//{
//	if(SceneNodeQueryData* pData = getQueryData())
//	{
//		return pData->eNodeType;
//	}
//	return SceneNode_None;
//}
#define KeyIsInSelected "IsInSelect"
BOOL CRenderScene3DNode::IsSelected() const
{
	BOOL b = FALSE;
	if(SceneNodeQueryData* pData = getQueryData())
	{
		pData->GetData(KeyIsInSelected,b);
	}
	return b;
}

void CRenderScene3DNode::Select( BOOL b )
{
	if(IsSelected() == b)
		return;
	
	DynamicCast()->OnUpdateSelect(b);
	SetSelected(b);
}

void CRenderScene3DNode::SetSelected( BOOL b )
{
	if(SceneNodeQueryData* pdata = InitQueryData())
	{
		pdata->SetData(KeyIsInSelected,b);
	}
}

IRenderSceneMouseState*  CRenderScene3DNode::GetBePickMode() const
{
	IRenderSceneMouseState* pState= NULL;
	if(SceneNodeQueryData* pData = getQueryData())
	{
		pData->GetData(KeyInPickMode,(int&)pState);
	}
	return pState;
}

void CRenderScene3DNode::ChangeToPicMode( IRenderSceneMouseState* pState )
{
	if(!IsValid())
		return;

	if(GetBePickMode()==pState)
		return;

	DynamicCast()->OnUpdateBePickMode(pState);
	SetBePickMode(pState);
}

void CRenderScene3DNode::SetBePickMode(IRenderSceneMouseState* pState)
{
	if(SceneNodeQueryData* pData = InitQueryData())
	{
		pData->SetData(KeyInPickMode,(int)pState);
	}	
}

void CRenderScene3DNode::OnUpdateBePickMode( IRenderSceneMouseState* pState )
{

}

void CRenderScene3DNode::OnUpdateSelect( BOOL b )
{

}


void CRenderScene3DNode::OnRButtonDown( const MouseSceneInfo& mouseInfo )
{
	if( !getDynamicProtype() ||  IsRealType() )
		return;	
	DynamicCast()->OnRButtonDown(mouseInfo);
}

void CRenderScene3DNode::OnLButtonDblClk( const MouseSceneInfo& mouseinfo )
{
	if( !getDynamicProtype() ||  IsRealType() )
		return;	
	DynamicCast()->OnLButtonDblClk(mouseinfo);
}
void CRenderScene3DNode::OnMouseMove( const MouseSceneInfo& mouseinfo )
{
	if( !getDynamicProtype() ||  IsRealType() )
		return;	
	DynamicCast()->OnMouseMove(mouseinfo);
}

void CRenderScene3DNode::StartDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	if( !getDynamicProtype() ||  IsRealType() )
		return;	
	DynamicCast()->StartDrag(lastMouseInfo,curMouseInfo);
}

void CRenderScene3DNode::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	if( !getDynamicProtype() ||  IsRealType() )
		return;	
	DynamicCast()->OnDrag(lastMouseInfo,curMouseInfo);
}

void CRenderScene3DNode::EndDrag()
{
	if( !getDynamicProtype() ||  IsRealType() )
		return;	

	DynamicCast()->EndDrag();
}

bool CRenderScene3DNode::IsDraggable()const
{
	if( !getDynamicProtype() ||  IsRealType() )
		return false;	
	
	return DynamicCast()->IsDraggable();
}

bool CRenderScene3DNode::IsRealType() const
{
	CRenderScene3DNode* pDyNode = getDynamicProtype();
	CRenderScene3DNode* pReNode = getProType();
	return (getDynamicProtype()==getProType());
}

bool CRenderScene3DNode::IsValid() const
{
	if(!C3DNodeObject::IsValid())
		return false;
	return getDynamicProtype()!=NULL;
}

CRenderScene3DNode* CRenderScene3DNode::getDynamicProtype()const
{
	if(SceneNodeQueryData* pData = getQueryData() )
	{
		return pData->mpNodeDynamic;			
	}
	return false;
}

CRenderScene3DNode CRenderScene3DNode::CreateGetNameNode( CString sName ) const
{
	SceneNode* pNode = NULL;
	if(GetScene())
	{
		return OgreUtil::createOrRetrieveSceneNode(sName,GetScene());
	}
	return pNode;
}

void CRenderScene3DNode::OnUpdate3D()
{
	if( !getDynamicProtype() ||  IsRealType() )
		return;	
	DynamicCast()->OnUpdate3D();
}

void CRenderScene3DNode::OnUpdateEditMode( BOOL b )
{
	if( !getDynamicProtype() ||  IsRealType() )
		return;	
	DynamicCast()->OnUpdateEditMode(b);
}

IEnvModeEditContext* CRenderScene3DNode::GetEditContext( EnvironmentMode mode )const
{
	if(CRender3DScene* pScene = getRenderScene()){
		return pScene->mpEditor->GetContext(mode);	
	}
	return NULL;
}

ILandsideEditContext* CRenderScene3DNode::GetLandsideEditContext() const
{
	IEnvModeEditContext* pctx = GetEditContext(EnvMode_LandSide);
	if(pctx){
		return pctx->toLandside();
	}
	return NULL;
}

IAirsideEditContext* CRenderScene3DNode::GetAirsideEditContext() const
{
	IEnvModeEditContext* pctx = GetEditContext(EnvMode_AirSide);
	if(pctx){
		return pctx->toAirside();
	}
	return NULL;
}

ITerminalEditContext* CRenderScene3DNode::GetTerminalEditContext() const
{
	IEnvModeEditContext* pctx = GetEditContext(EnvMode_Terminal);
	if(pctx){
		return pctx->toTerminal();
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
#include "Mouse3DState.h"
#include "SceneState.h"
#include "landside/LandsideEditCommand.h"

PROTYPE_INSTANCE(CLayoutObject3DNode)

LandsideFacilityLayoutObject* CLayoutObject3DNode::getLayOutObject()const
{	
	if(SceneNodeQueryData* pdata = getQueryData())
	{
		LandsideFacilityLayoutObject* pObj = NULL;
		if(pdata->GetData(KeyAltObjectPointer,(int&)pObj))
		{
			return pObj;
		}
	}
	return NULL;
}

void CLayoutObject3DNode::SetQueryData(LandsideFacilityLayoutObject* pObj)
{
	if(SceneNodeQueryData* pdata = InitQueryData())
	{
		pdata->SetData(KeyAltObjectPointer,(int)pObj);	
		pdata->SetData(KeyLandsideItemType,_LayoutObject);			
	}
}

void CLayoutObject3DNode::Setup3D( LandsideFacilityLayoutObject *pObj )
{
	SetQueryData(pObj);
	DynamicCast()->OnUpdate3D();	
}

void CLayoutObject3DNode::OnLButtonDblClk( const MouseSceneInfo& mouseinfo )
{
	FlipEditMode();
}

bool CLayoutObject3DNode::IsLocked() const
{
	if(LandsideFacilityLayoutObject* pobj= getLayOutObject())
	{
		return pobj->GetLocked();
	}
	return true;
}

bool CLayoutObject3DNode::IsDraggable() const
{
	return !IsLocked();
}


void CLayoutObject3DNode::StartDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{	
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	LandsideFacilityLayoutObject*pStretch = getLayOutObject();
	if(!pStretch)return;

	LandsideModifyLayoutObjectCmd* pModCmd = new LandsideModifyLayoutObjectCmd(pStretch,plandsideCtx->getInput());	
	plandsideCtx->StartEditOp(pModCmd);
	SetInDragging(TRUE);

	OnDrag(lastMouseInfo,curMouseInfo);
}

void CLayoutObject3DNode::OnDrag( const MouseSceneInfo& lastMouseInfo, const MouseSceneInfo& curMouseInfo )
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	

	if(!lastMouseInfo.bInPlane || !curMouseInfo.bInPlane)
		return;

	Vector3 mousediff = curMouseInfo.mWorldPos- lastMouseInfo.mWorldPos;
	if(LandsideFacilityLayoutObject*pStretch = getLayOutObject())
	{
		pStretch->DoOffset(mousediff.x, mousediff.y,0);
		OnUpdate3D();
		plandsideCtx->UpdateDrawing();
	}

	/*CPoint2008 pt;
	if(pStretch->getCenter(pt)){
		plandsideCtx->ConvertToVisualPos(pt);
		SetPosition(pt);
		plandsideCtx->UpdateDrawing();
	}*/

}

void CLayoutObject3DNode::EndDrag()
{
	ILandsideEditContext* plandsideCtx = getRenderScene()->GetEditContext()->toLandside();
	if(!plandsideCtx)
		return;	
	plandsideCtx->NotifyViewCurentOperation();
	plandsideCtx->DoneCurEditOp();
	SetInDragging(FALSE);
}

void CLayoutObject3DNode::OnUpdateEditMode( BOOL b )
{
	SetEditMode(b);
	OnUpdate3D();
}
