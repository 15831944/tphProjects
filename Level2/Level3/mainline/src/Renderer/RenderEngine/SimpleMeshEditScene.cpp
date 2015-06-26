#include "StdAfx.h"
#include ".\simplemesheditscene.h"
#include <common\Grid.h>
#include "../../InputOnBoard/ComponentModel.h"
#include "ogreutil.h"
//#include "SceneNodeCoordinate.h"
#include "SimpleMesh3D.h"
#include "SceneState.h"
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>
#include "../../InputOnBoard/ComponetEditCommands.h"
#include "../../InputOnBoard/ComponentEditContext.h"
#include ".\RenderEngine.h"
#include <common/ListenerCommon.h>

#include "MaterialDef.h"

#define GridXYName _T("GridXY")
#define GridXZName _T("GridXZ")
#define GridYZName _T("GridYZ")
#define CADMapName _T("CADMap")
#define PICK_LINE _T("PickLine")

#define ExternalMeshName _T("ExternalMesh")
#define EditModelName _T("EditModel")

using namespace Ogre;
using namespace std;

void CSimpleMeshEditScene::Setup(CComponentEditContext* pEdCtx)
{
	mpEditContext = pEdCtx;
	if(!MakeSureInited())
	{
		ASSERT(FALSE);
		return;
	}	

	CComponentMeshModel* pModel = pEdCtx->GetCurrentComponent();
	ResourceFileArchive::AddDirectory(pModel->msTmpWorkDir,(CString)pModel->mguid);	
	//set up lighting
	//mpScene->setAmbientLight(ColourValue(0.2,0.2,0.2));
	
	//set up camera	
	{		
		mGridXY = AddSceneNode(GridXYName);
		mGridXZ = AddSceneNode(GridXZName);
		mGridYZ = AddSceneNode(GridYZName);
	}

	mPickLine = GetSceneNode(PICK_LINE);
	mPickLine.AttachTo(GetRoot());

	//add model to scene
	if(pModel->mbExternalMesh)
	{
		mExternalMesh = AddSceneNode(ExternalMeshName);
		Entity* pEnt = OgreUtil::createOrRetrieveEntity(mExternalMesh.GetIDName(), pModel->GetMeshFileName(),getSceneManager());
		ASSERT(pEnt);
		mExternalMesh.AddObject(pEnt);
	}
	else
	{
		//
		mCenerLine = CCenterLine3D(AddSceneNode(MeshEditCenterLineName) );		
		mCenerLine.Update(pModel,*this);
		//
		mLoadMap = GetSceneNode(CADMapName);		
		//
		mEditMesh = CSimpleMesh3D(pModel,GetSceneNode(EditModelName) );	
		mEditMesh.Update(GetCurMeshMat());	
		mEditMesh.AttachTo(mCenerLine);
		mSection3DList.Update(pModel,*this);
	}
	OnUpdateGrid(pModel);
}
	

void CSimpleMeshEditScene::OnModelChange( CComponentMeshModel* pModel )
{
	Clear();
	//change 
	Setup(mpEditContext);
}

CSimpleMeshEditScene::CSimpleMeshEditScene()
{
	mpEditContext = NULL;
}




BOOL CSimpleMeshEditScene::OnMouseMove( const MouseSceneInfo& mouseInfo )
{
	do 
	{

		if(mMouseState == _gettwopoints || mMouseState == _getmanypoints || mMouseState == _distancemeasure)
		{
			ARCPath3 vPath = m_vMousePosList;
			vPath.push_back( ARCVECTOR(mouseInfo.mWorldPos) );
			mPickLine.Update(vPath);	
			break;
		}

		CComponentEditOperation* pCurEditOP = mpEditContext->GetCurEditOperation();
		EditOperation mCurOp = OP_null;
		if(pCurEditOP){ mCurOp = pCurEditOP->getEditOp(); }

		mpEditContext->InitCurEditOnce();
		////
		if(mCurOp == OP_moveModelHeight)
		{		
			mCenerLine.OnMove(mState->m_LastMoseMoveInfo.mSceneRay,mouseInfo.mSceneRay);
			mpEditContext->OnEditOpertaion();
			break;
		}
		//
		if(mCurOp == OP_moveSectionXPos)
		{
			CSectionPos3DController::SharedPtr pCtrl = mSection3DList.FindItem(mpEditContext->GetActiveSection());
			pCtrl->OnMouseMove(mState->m_LastMoseMoveInfo.mSceneRay, mouseInfo.mSceneRay);
			pCtrl->UpdatePos();
			mEditMesh.Update(GetCurMeshMat());
			mpEditContext->OnEditOpertaion();
			break;
		}

		if(mCurOp == OP_moveSectionCenter)
		{
			mSetionDetailCtrl.OnMouseMoveCenter(mState->m_LastMoseMoveInfo.mSceneRay, mouseInfo.mSceneRay);
			CSectionPos3DController::SharedPtr pCtrl = mSection3DList.FindItem(mpEditContext->GetActiveSection());
			pCtrl->UpdatePos();    
			mEditMesh.Update(GetCurMeshMat());
			mpEditContext->OnEditOpertaion();
			break;
		}

		if(mCurOp == OP_moveSectionCtrlPoint)
		{
			int nCurPt = mpEditContext->GetActiveCtrlPtIdx();
			mSetionDetailCtrl.OnMouseMoveCtrlPt(nCurPt,mState->m_LastMoseMoveInfo.mSceneRay, mouseInfo.mSceneRay);
			mSetionDetailCtrl.Update();
			CSectionPos3DController::SharedPtr pCtrl = mSection3DList.FindItem(mpEditContext->GetActiveSection());
			pCtrl->UpdateShape();
			mEditMesh.Update(GetCurMeshMat());
			mpEditContext->OnEditOpertaion();
			break;
		}
		if(mCurOp == OP_moveSectionThickPoint)
		{
			int nCurPt = mpEditContext->GetActiveCtrlPtIdx();
			mSetionDetailCtrl.OnMouseMoveThickness(nCurPt,mState->m_LastMoseMoveInfo.mSceneRay, mouseInfo.mSceneRay);
			mSetionDetailCtrl.Update();
			CSectionPos3DController::SharedPtr pCtrl = mSection3DList.FindItem(mpEditContext->GetActiveSection());
			pCtrl->UpdateShape();
			mEditMesh.Update(GetCurMeshMat());
			mpEditContext->OnEditOpertaion();
			break;
		}

	}while (false);	
	return C3DSceneBase::OnMouseMove( mouseInfo );
}

BOOL CSimpleMeshEditScene::OnLButtonDown( const MouseSceneInfo& mouseInfo)
{		
	do //for can not just return save as switch case  
	{
		MovableObject* pSelObj = mouseInfo.mOnObject;;
		//down on centerline control point		
		if( CSectionPos3DController*  pController = mSection3DList.IsMouseOn(pSelObj) )
		{
			if (mpEditContext->SetActiveSection(pController->mpSection))
			{
				ActiveSection(pController->mpSection);
			}
			mpEditContext->StartNewEditOp(OP_moveSectionXPos);
			break;
		}

		if( mSetionDetailCtrl.IsCenterCtrl(pSelObj) )
		{
			mpEditContext->SetActiveSection(mSetionDetailCtrl.mpSection);
			mpEditContext->StartNewEditOp(OP_moveSectionCenter);
			break;
		}

		int idx;
		if( mSetionDetailCtrl.IsSectionCtrl(pSelObj,idx) )
		{
			mpEditContext->SetActiveCtrlPt(idx);
			mpEditContext->StartNewEditOp(OP_moveSectionCtrlPoint);
			break;
		}

		if(mSetionDetailCtrl.IsThickNessCtrl(pSelObj,idx) )
		{
			mpEditContext->SetActiveSection(mSetionDetailCtrl.mpSection);
			mpEditContext->SetActiveCtrlPt(idx);
			mpEditContext->StartNewEditOp(OP_moveSectionThickPoint);
			break;
		}		

		if( mCenerLine.HasMovableObj(pSelObj) )
		{
			mpEditContext->StartNewEditOp(OP_moveModelHeight);
			break;
		}


		
	}
	while (false);

	return C3DSceneBase::OnLButtonDown(mouseInfo);
}

BOOL CSimpleMeshEditScene::OnLButtonUp( const MouseSceneInfo& mouseInfo )
{

	if(mMouseState == _getonepoint || mMouseState == _gettwopoints || mMouseState == _getmanypoints || mMouseState == _distancemeasure )
	{
		m_vMousePosList.push_back( ARCVECTOR(mouseInfo.mWorldPos) );
		mPickLine.Update(m_vMousePosList);			
	}		
	else
	{
		mpEditContext->DoneCurEdit();	
		
	}	
	return C3DSceneBase::OnLButtonUp(mouseInfo);
}


BOOL CSimpleMeshEditScene::OnRButtonDown( const MouseSceneInfo& mouseInfo )
{
	do 
	{
		if(mMouseState == _getonepoint|| mMouseState == _gettwopoints || mMouseState == _getmanypoints || mMouseState == _distancemeasure )
		{
			break;
		}
		MovableObject* pSelObj = mouseInfo.mOnObject;
		//right button on section point
		if( CSectionPos3DController*  pController = mSection3DList.IsMouseOn(pSelObj) )
		{
			if(mpEditContext->SetActiveSection(pController->mpSection))
                ActiveSection(pController->mpSection);
			DEAL_LISTENER_HANDLER( OnRBtnDownSection(pController->mpSection,mouseInfo.mpt) );
			break;
		}
		//check what kind of object is right click		
		if( mCenerLine.HasMovableObj(pSelObj,false) )
		{
			break;
		}
		
		//down on nothing
		//get word 
		CGrid& grid = mpEditContext->GetCurrentComponent()->mGrid;
		Vector3 vMin(-grid.dSizeX*0.5f,-grid.dSizeY*0.5f,0);
		ARCVector3 vIntersect = GetWorldIntersect(vMin,mouseInfo.mSceneRay);
		DEAL_LISTENER_HANDLER( OnRBtnDownScene(mouseInfo.mpt,vIntersect) );
        //check if click on the 
		
	} while (false);

	return C3DSceneBase::OnRButtonDown(mouseInfo);
}




void CSimpleMeshEditScene::SaveModel()
{
	if(mEditMesh)
	{
		mEditMesh.SaveThumbnail();	
		mEditMesh.SaveMesh();
	}
	else
	{
		mExternalMesh.SnapShot(mpEditContext->GetCurrentComponent()->msTmpWorkDir);
	}
}



#define STR_ACTIVE_SECTION _T("ActiveSection")

void CSimpleMeshEditScene::ActiveSection( CComponentMeshSection* pSection )
{
	
	CSection3DControllerList::DataItem section3d = mSection3DList.FindItem(pSection);
	if(section3d.get())
	{
		mSetionDetailCtrl = CSectionDetailController(pSection,GetSceneNode(STR_ACTIVE_SECTION) ); 
		mSetionDetailCtrl.Update(*this);
		mSetionDetailCtrl.AttachTo(section3d->mSection3D);
		UpdateCADMap(pSection);
	}	
	mSection3DList.ActiveItem(pSection);
}


void CSimpleMeshEditScene::OnUpdateOperation( CComponentEditOperation* pOP)
{
	if(!pOP)
		return ;

	EditOperation emOp = pOP->getEditOp();
	if(emOp == OP_moveModelHeight
		|| emOp == OP_editLODHeight)
	{
		mCenerLine.UpdatePos(mpEditContext->GetCurrentComponent());
	}
	if(emOp == OP_moveSectionXPos
		|| emOp ==OP_moveSectionCenter )
	{
		CComponentMeshSection* pSection = pOP->ToSectionEditOp()->GetSection();
        CSectionPos3DController::SharedPtr pCtrl = mSection3DList.FindItem(pSection);		
		if(pCtrl.get())
		{
			pCtrl->UpdatePos();
			mEditMesh.Update(GetCurMeshMat());
		}
	}

	if(emOp == OP_addSection || emOp == OP_delSection)
	{
		mSection3DList.Update(mpEditContext->GetCurrentComponent(),*this);		
		CComponentMeshSection* pSection = pOP->ToSectionEditOp()->GetSection();
		UpdateCADMap(pSection);
		mEditMesh.Update(GetCurMeshMat());
	}
	

	if(emOp == OP_moveSectionCtrlPoint || OP_moveSectionThickPoint == emOp
		|| OP_addSectionPoint == emOp || OP_delSectionPoint == emOp)
	{
		CComponentMeshSection* pSection = pOP->ToSectionEditOp()->GetSection();
		CSectionPos3DController::SharedPtr pCtrl = mSection3DList.FindItem(pSection);			
		pCtrl->UpdateShape();
		mEditMesh.Update(GetCurMeshMat());
		//update active section
		CSection3DControllerList::DataItem section3d = mSection3DList.FindItem(pSection);
		if(section3d.get())
		{
			mSetionDetailCtrl = CSectionDetailController(pSection,GetSceneNode(STR_ACTIVE_SECTION) ); 
			mSetionDetailCtrl.Update(*this);
		}	
	}

	if(emOp == OP_editSectionProp)
	{
		CComponentMeshSection* pSection = pOP->ToSectionEditOp()->GetSection();
		CSectionPos3DController::SharedPtr pCtrl = mSection3DList.FindItem(pSection);			
		pCtrl->UpdateShape();
		pCtrl->UpdatePos();
		mEditMesh.Update(GetCurMeshMat());
		UpdateCADMap(pSection);
	}

	if(emOp == OP_editLODHeight)
	{
		mEditMesh.Update(GetCurMeshMat());
		mSection3DList.UpdateAllShape();
	}
	
}



void CSimpleMeshEditScene::UpdateCADMap( CComponentMeshSection* pSection )
{
	mLoadMap.Detach();
	CSectionPos3DController::SharedPtr pCtrl = mSection3DList.FindItem(pSection);
	if(pCtrl.get())
	{
		mLoadMap.AttachTo(pCtrl->mSectionPos);
		mLoadMap.Update(pSection->m_cadFileInfo);
		mLoadMap.UpdateTransform(pSection->m_cadFileInfo.mCADInfo);
		mLoadMap.SetVisibleFlag(YZVisibleFlag);
	}
}

#define MAT_SOLID _T("MeshSolid")
#define MAT_SOLID_WIRE _T("MeshSolidWire")
#define MAT_TRANS _T("MeshTransparent")
#define MAT_TRANS_WIRE _T("MeshTransparentWire")
#define MAT_WIRE _T("MeshWireframe")
#define MAT_HID_WIRE _T("MeshHiddenWireframe")

void CSimpleMeshEditScene::UpdateMeshViewType()
{
	mEditMesh.SetMaterial(GetCurMeshMat());	
}

CString CSimpleMeshEditScene::GetCurMeshMat() const
{
	if(mpEditContext)
	{
		emMeshViewType vt = mpEditContext->GetMeshViewType();
		switch(vt)
		{
		case MV_Solid :
			return MAT_SOLID;
		case MV_Solid_Wireframe :
			return MAT_SOLID_WIRE;
		case MV_Transparent :
			return MAT_TRANS;		
		case MV_Wireframe:
			return MAT_WIRE;		
		case MV_Transparent_Wireframe:
			return MAT_TRANS_WIRE;	
		case MV_Hidden_Wire:
			return MAT_HID_WIRE;
		default:
			break;
		}
	}
	return MAT_SOLID;
}



ARCVector3 CSimpleMeshEditScene::GetWorldIntersect( const Ogre::Vector3& vMin, const Ogre::Ray& ray ) const
{
	Ogre::Plane xz( Vector3::UNIT_Y,Vector3(0,vMin.y,0) );
	Ogre::Plane xy(Vector3::UNIT_Z,Vector3(0,0,vMin.z) );
	Ogre::Plane yz(Vector3::UNIT_X,Vector3(vMin.x,0,0) );
	typedef std::pair<bool,Real> IRET;
	IRET retxy = ray.intersects(xy);
	IRET retxz = ray.intersects(xz);
	IRET retyz = ray.intersects(yz);
	
	IRET ret = retxy;	
	if(retxz.first){
		if(ret.first&&retxz.second<ret.second){ ret = retxz; }
		if(!ret.first){ ret= retxz; }
	}
	if(retyz.first){
		if(ret.first&&retyz.second<ret.second){ ret = retxy; }
		if(!ret.first){ ret = retyz; }
	}
	ASSERT(ret.first);
	Real dLen = ret.second;
	return ARCVECTOR( ray.getPoint(dLen) );
	
}



ARCVector3 CSimpleMeshEditScene::GetMousePosXY() const
{
	MouseSceneInfo& minfo = mState->m_LastMoseMoveInfo;
	bool b; Real d;
	boost::tie(b,d)  = minfo.mSceneRay.intersects(Plane(Vector3::UNIT_Z,Vector3::ZERO));
	if(b){
		return ARCVECTOR(minfo.mSceneRay.getPoint(d));
	}
	return ARCVector3(0,0,0);
}

ARCVector3 CSimpleMeshEditScene::GetMousePosXZ() const
{
	MouseSceneInfo& minfo = mState->m_LastMoseMoveInfo;
	bool b; Real d;
	boost::tie(b,d)  = minfo.mSceneRay.intersects(Plane(Vector3::UNIT_Y,Vector3::ZERO));
	if(b){
		return ARCVECTOR(minfo.mSceneRay.getPoint(d));
	}
	return ARCVector3(0,0,0);
}

ARCVector3 CSimpleMeshEditScene::GetMousePosYZ() const
{
	MouseSceneInfo& minfo = mState->m_LastMoseMoveInfo;
	bool b; Real d;
	boost::tie(b,d)  = minfo.mSceneRay.intersects(Plane(Vector3::UNIT_X,Vector3::ZERO));
	if(b){
		return ARCVECTOR(minfo.mSceneRay.getPoint(d));
	}
	return ARCVector3(0,0,0);
}

void CSimpleMeshEditScene::UpdateGrid( CComponentMeshModel* pModel )
{
	mGridXY.Update(pModel->mGrid,CGrid3D::XYExtent,true);
	mGridXY.SetVisibleFlag(PERSP_VisibleFlag);

	mGridXZ.Update(pModel->mGrid,CGrid3D::XZExtent,false);
	mGridXZ.SetVisibleFlag(XZVisibleFlag);

	mGridYZ.Update(pModel->mGrid,CGrid3D::YZExtent,false);
	mGridYZ.SetVisibleFlag(YZVisibleFlag);
}


void CSimpleMeshEditScene::OnUpdateGrid( CComponentMeshModel* pMeshModel )
{
	if(pMeshModel->mbExternalMesh)
	{
		mCenerLine.Update(pMeshModel,*this);
	}
	UpdateGrid(pMeshModel);
}


void CSimpleMeshEditScene::StartDistanceMeasure()
{
	ResetMouseState();
	mMouseState = _distancemeasure;	
}

void CSimpleMeshEditScene::EndDistanceMeasure()
{
	DEAL_LISTENER_HANDLER( OnFinishDistanceMesure(m_vMousePosList.GetLength()) );
	ResetMouseState();
	mPickLine.Update(CLines3D::VertexList());
}

void CSimpleMeshEditScene::ResetMouseState()
{
	m_vMousePosList.resize(0);
	mMouseState = _default;	
}

BOOL CSimpleMeshEditScene::OnRButtonUp( const MouseSceneInfo& mouseInfo )
{
	BOOL norefresh = TRUE;	
	if(mMouseState == _getonepoint|| mMouseState == _gettwopoints || mMouseState == _getmanypoints || mMouseState == _distancemeasure )
	{
		if(mouseInfo.bInPlane)
		{
			m_vMousePosList.push_back( ARCVECTOR(mouseInfo.mWorldPos) );
		}	
		if(mMouseState == _distancemeasure){ EndDistanceMeasure(); }
		
	}
	return C3DSceneBase::OnRButtonUp(mouseInfo);
}

BOOL CSimpleMeshEditScene::OnLButtonDblClk( const MouseSceneInfo& mouseInfo )
{
	if(mMouseState == _getonepoint|| mMouseState == _gettwopoints || mMouseState == _getmanypoints || mMouseState == _distancemeasure )
	{
		return TRUE;
	}
	MovableObject* pSelObj = mouseInfo.mOnObject;
	//right button on section point
	for(int i=0;i<(int)mSetionDetailCtrl.mctrlPtsList.size();i++)
	{
		AutoDetachNodeObject::SharedPtr& ptr = mSetionDetailCtrl.mctrlPtsList[i];
		if(ptr->HasMovableObj(pSelObj))
		{
			DEAL_LISTENER_HANDLER(OnDBClikcOnSectionPoint(mSetionDetailCtrl.mpSection,i,mouseInfo.mpt));
		}
	}
	return FALSE;

}