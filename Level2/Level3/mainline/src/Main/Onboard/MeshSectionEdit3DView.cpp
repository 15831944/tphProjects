#include "StdAfx.h"
#include "../resource.h"
#include ".\meshsectionedit3Dview.h"

#include <InputOnBoard/CInputOnboard.h>
#include <InputOnboard/ComponentModel.h>
#include "DlgModifyThickness.h"
#include "OnboardViewMsg.h"
#include "DlgSectionProp.h"
#include <InputOnboard/ComponentEditContext.h>
#include "MeshEditFrame.h"
#include "..\DlgSectionPointProp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
static int nCurPtId  = -1;


IMPLEMENT_DYNCREATE(CMeshSectionEditView, CMeshEdit3DView)


BEGIN_MESSAGE_MAP(CMeshSectionEditView, CMeshEdit3DView)	
	//ON_WM_MOUSEMOVE()
	//ON_WM_LBUTTONDOWN()
	//ON_WM_LBUTTONUP()	
	//ON_WM_SIZE()
	//ON_WM_RBUTTONDOWN()	

	ON_COMMAND(ID_ONBOARDSTATIONPOINTEDIT_THICKNESS,OnModifyThickness)
	ON_COMMAND(ID_ONBOARDSTATIONPOINTEDIT_DELETE,OnDelStationPoint)
	ON_COMMAND(ID_ONBOARDSTATION_ADDSTATIONPOINT, OnAddStationPoint)
	ON_COMMAND(ID_ONBOARDSTATION_STATIONPROPRERTIES, OnStationProp)
	ON_COMMAND(ID_LINETYPE_CUBIC, OnLinetypeCubic)
	ON_UPDATE_COMMAND_UI(ID_LINETYPE_CUBIC, OnUpdateLinetypeCubic)
	ON_COMMAND(ID_LINETYPE_STRAIGHT, OnLinetypeStraight)
	ON_UPDATE_COMMAND_UI(ID_LINETYPE_STRAIGHT, OnUpdateLinetypeStraight)
END_MESSAGE_MAP()





CMeshSectionEditView::CMeshSectionEditView(void)
{
}

CMeshSectionEditView::~CMeshSectionEditView(void)
{
}

#include <renderer/RenderEngine/RenderEngine.h>
#include <common/TmpFileManager.h>

void CMeshSectionEditView::SetupViewport()
{
	CComponentMeshModel* pModel = GetEditContext()->GetCurrentComponent();
	
	//set up scene add view port main
	CSimpleMeshEditScene* pMeshScene = GetMeshEditScene();
	if(pMeshScene)
	{		
		if(!pMeshScene->IsValid())
		{
			pMeshScene->Setup(GetEditContext());			
		}
		pMeshScene->AddListener(this);	
		CCameraNode camera = pMeshScene->GetCamera( mViewCameraName );	
		Ogre::Camera* pcam = camera.mpCamera;
		camera.Load(pModel->mSectionCam);

		int idx = m_rdc.AddViewport( pcam,true );
		ViewPort vp = m_rdc.GetViewport(idx);
		vp.SetClear(RGB(164,164,164));	
		vp.SetVisibleFlag(YZVisibleFlag| NormalVisibleFlag);
	}

}
//

void CMeshSectionEditView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint )
{	
	if(lHint == VM_ONBOARD_COMPONET_SCENE_CHANGE)
	{
		Invalidate(FALSE);
	}
	if(lHint == VM_ONBOARD_COMPONENT_GRID_CHANGE)
	{
		Invalidate(FALSE);
	}
	if(lHint == VM_ONBOARD_COMPONENT_SAVE_MODEL)
	{
		CComponentMeshModel* pModel = GetEditContext()->GetCurrentComponent();
		CCameraNode camera = GetMeshEditScene()->GetCamera( mViewCameraName );	
		camera.Save(pModel->mSectionCam);
	}
}
//
//
//void CMeshSectionEditView::OnRButtonDown( UINT nFlags, CPoint point )
//{
//	
//	
//
//	C3DBaseView::OnRButtonDown(nFlags, point);
//}







void CMeshSectionEditView::OnAddStationPoint()
{
	//if( GetComponetEditContext() && GetComponetEditContext()->GetCurrentStation() )
	//{
	//	CComponentSection* pStation = GetComponetEditContext()->GetCurrentStation();
	//	Ogre::Ray mouseRay = m_lastRBtnDownRay;
	//	Wm4::Ray3d ray;
	//	ray.Origin = Wm4::Vector3d(mouseRay.getOrigin().x, mouseRay.getOrigin().y, mouseRay.getOrigin().z);
	//	ray.Direction = Wm4::Vector3d(mouseRay.getDirection().x, mouseRay.getDirection().y, mouseRay.getDirection().z);

	//	DistanceUnit dXPos = pStation->getPos();
	//	Wm4::Plane3d plane( Wm4::Vector3d::UNIT_X, Wm4::Vector3d(0,0,0)  );

	//	Wm4::IntrRay3Plane3d intersect(ray,plane);
	//	if( intersect.Find() )
	//	{
	//		Wm4::Vector3d vIntr = ray.Origin + ray.Direction * intersect.GetRayT() ;

	//		Real dY = vIntr.Y();
	//		Real dZ = vIntr.Z() - GetCurrentComponent()->getHeight();

	//		ARCVector2 newPointPos =  ARCVector2(dY,dZ);

	//		int nPtid = pStation->AddPoint(newPointPos);			

	//		m_sectionScene.OnChangeSection(GetCurrentComponent(), GetComponetEditContext()->GetCurrentStationIndex());
	//		
	//		GetDocument()->UpdateAllViews(this, VM_ONBOARD_COMPONENT_ADD_STATION_POINT,(CObject*)nPtid);
	//		
	//		Invalidate(FALSE);

	//	}
	//			
	//}
}

void CMeshSectionEditView::OnDelStationPoint()
{
	/*if(m_pSelectContrlPoint && m_pSelectContrlPoint->m_type == CModelControlPoint::SECTION_POINT )
	{
		CComponentSection* pStation = GetComponetEditContext()->GetCurrentStation();
		ASSERT(pStation);
		if(pStation)
		{	
			int nPtId = m_pSelectContrlPoint->m_nControlPtrIndex;

			if(pStation->getPointCount() <=1)
			{
				AfxMessageBox("Each Station  must have one point at least!");
				return;
			}
			else
			{
				pStation->RemovePoint(nPtId);			
				m_sectionScene.OnChangeSection(GetCurrentComponent(), GetComponetEditContext()->GetCurrentStationIndex());
				GetDocument()->UpdateAllViews(this, VM_ONBOARD_COMPONENT_DEL_STATION_POINT, (CObject*)nPtId );
				Invalidate(FALSE);
			}
		}
	}*/
}


void CMeshSectionEditView::OnModifyThickness()
{
	/*if(m_pSelectContrlPoint && m_pSelectContrlPoint->m_type == CModelControlPoint::SECTION_POINT )
	{
		CDlgModifyThickness dlg;
		nCurPtId = m_pSelectContrlPoint->m_nControlPtrIndex;
		CComponentSection* pSection = GetComponetEditContext()->GetCurrentStation();
		dlg.setThickness( pSection->getThickness(nCurPtId) );
		
		if(dlg.DoModal() ==IDOK)
		{			
			pSection->setThickness(nCurPtId,dlg.getThickness() );
			m_sectionScene.OnChangeSection(GetCurrentComponent(), GetComponetEditContext()->GetCurrentStationIndex());

			GetDocument()->UpdateAllViews(this, VM_ONBOARD_COMPONET_MODIFY_THICKNESS, (CObject*)nCurPtId);
			Invalidate(FALSE);
		}
	}*/
}



void CMeshSectionEditView::OnStationProp()
{
	//CComponentSection* pSection = GetComponetEditContext()->GetCurrentStation();
	//if(!pSection)
	//	return;

	//CDlgSectionProp dlgProp(pSection,this);
	//if(dlgProp.DoModal() == IDOK )
	//{
	//	if(dlgProp.mbSectionChange)
	//	{
	//		//m_sectionScene.OnChangeSection( GetCurrentComponent(), GetComponetEditContext()->GetCurrentStationIndex() );
	//		GetDocument()->UpdateAllViews(NULL, VM_ONBOARD_COMPONENT_MODIFY_STATION, NULL);
	//	}
	//	if(dlgProp.mbMapChange)
	//	{
	//		//m_sectionScene.OnChangeSectionMap( GetCurrentComponent(), GetComponetEditContext()->GetCurrentStationIndex() );
	//		GetDocument()->UpdateAllViews(NULL, VM_ONBOARD_COMPONENT_MODIFY_STATIONMAP, NULL);
	//	}
	//	else
	//	{
	//		//m_sectionScene.OnChangeMapTransform( GetCurrentComponent(), GetComponetEditContext()->GetCurrentStationIndex() );
	//		GetDocument()->UpdateAllViews(NULL, VM_ONBOARD_SECTION_MAP_TRANSFROM_CHANGE, NULL);
	//		return ;
	//	}
	//	
	//}
}
void CMeshSectionEditView::OnLinetypeCubic()
{
	// TODO: Add your command handler code here
	CComponentMeshSection* pSection = NULL;//GetComponetEditContext()->GetCurrentStation();
	if(pSection)
	{
		/*bool bChange = pSection->getEdgeType(nCurPtId)!=Cubic_Spline;
		pSection->setEdgeType(nCurPtId, Cubic_Spline);
		if(bChange)
		{
			GetDocument()->UpdateAllViews(NULL, VM_ONBOARD_COMPONET_MODIFY_LINETYPE, NULL);
			Invalidate(FALSE);
		}*/
	}
}

void CMeshSectionEditView::OnUpdateLinetypeCubic(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CComponentMeshSection* pSection = NULL;//GetComponetEditContext()->GetCurrentStation();
	if(pSection)
	{
		/*bool bCubic = (pSection->getEdgeType(nCurPtId) == Cubic_Spline);
		pCmdUI->SetCheck(bCubic?1:0);*/
	}
}

void CMeshSectionEditView::OnLinetypeStraight()
{
	// TODO: Add your command handler code here
	CComponentMeshSection* pSection = NULL;//GetComponetEditContext()->GetCurrentStation();
	if(pSection)
	{
		/*bool bChange = pSection->getEdgeType(nCurPtId)!=Straight_Line;
		pSection->setEdgeType(nCurPtId, Straight_Line);
		if(bChange)
		{
			GetDocument()->UpdateAllViews(NULL, VM_ONBOARD_COMPONET_MODIFY_LINETYPE, NULL);
			Invalidate(FALSE);
		}*/
	}
}

void CMeshSectionEditView::OnUpdateLinetypeStraight(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CComponentMeshSection* pSection = NULL;//GetComponetEditContext()->GetCurrentStation();
	if(pSection)
	{
		/*bool bStraight = (pSection->getEdgeType(nCurPtId) == Straight_Line);
		pCmdUI->SetCheck(bStraight?1:0);*/
	}
}


C3DSceneBase* CMeshSectionEditView::GetScene( Ogre::SceneManager* pScene )
{
	CSimpleMeshEditScene* pMeshScene = GetMeshEditScene();
	if( pMeshScene && pScene == pMeshScene->getSceneManager() )
		return pMeshScene;	
	return NULL;
}

void CMeshSectionEditView::OnDBClikcOnSectionPoint( CComponentMeshSection* pSection,int pIdx, const CPoint& pt )
{
	ARCVector2 vPos = pSection->GetCtrlPoint(pIdx);
	CDlgSectionPointProp dlg(vPos[VX],vPos[VY],this);
	MoveSectionPointPosCommmand* pnewCmd = new MoveSectionPointPosCommmand(pSection,pIdx);
	pnewCmd->BeginOp();
	if(dlg.DoModal()==IDOK)
	{
		vPos[VX] = dlg.mdY;
		vPos[VY] = dlg.mdZ;	
		pSection->SetCtrlPoint(pIdx,vPos);
		pnewCmd->EndOp();
		GetMeshEditScene()->OnUpdateOperation(pnewCmd);
		Invalidate(FALSE);
	}
}