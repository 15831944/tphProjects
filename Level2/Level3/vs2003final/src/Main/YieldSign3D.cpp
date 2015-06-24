#include "StdAfx.h"
#include "../InputAirside/YieldSign.h"
#include "../InputAirside/ALTObjectDisplayProp.h"
#include ".\YieldSign3D.h"
#include "../Common/ARCPipe.h"
#include ".\3DView.h"
#include "Floors.h"
#include "TermPlanDoc.h"

CYieldSign3D::CYieldSign3D(int nID):ALTObject3D(nID)
{
	m_pObj = new YieldSign();
	//m_pDisplayProp = new YieldSignDisplayProp();
	m_pModel = NULL;
	m_pModel2D = NULL;
}

CYieldSign3D::~CYieldSign3D(void)
{
}

void CYieldSign3D::DrawOGL( C3DView * pView )
{
	if(IsNeedSync()){
		DoSync();
	}
    glDisable(GL_POLYGON_OFFSET_FILL);
	const CPath2008& path = GetYieldSign()->GetPath();
	if(path.getCount()<1)return;

	ARCVector3 position = path.getPoint(0);

	CFloors & floors = pView->GetDocument()->GetFloorByMode(EnvMode_AirSide);
	position[VZ] = floors.getVisibleAltitude( position[VZ] );


	glPushMatrix();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glTranslated(position[VX],position[VY],position[VZ]);	
	glRotated(GetYieldSign()->GetRotation(), 0.0, 0.0, 1.0);
	//glScaled(m_pProc2->GetScale()[VX], m_pProc2->GetScale()[VY], m_pProc2->GetScale()[VZ]);

	if(!m_pModel)
		m_pModel =  pView->getModelRes().getModel(C3DModelResource::YieldSignModel);
	if(!m_pModel2D)
		m_pModel2D = pView->getModelRes().getModel(C3DModelResource::YieldSign2DModel);

	if(pView->GetCamera()->GetProjectionType() == C3DCamera::perspective )
	{	
		if(m_pModel)
			m_pModel->Render(pView);
	}
	else 
	{
		if(m_pModel2D)
			m_pModel2D->Render(pView);
	}

	glDisable(GL_CULL_FACE);

	glPopMatrix();

}


ARCPoint3 CYieldSign3D::GetLocation( void ) const
{
	YieldSign * pYieldSign = (YieldSign*)m_pObj.get();
	const CPath2008& path = pYieldSign->GetPath();
	if(path.getCount() < 1)return (ARCPoint3(0,0,0));
	return path.getPoint(0);
}

YieldSign * CYieldSign3D::GetYieldSign()
{
	return (YieldSign*) GetObject();
}


void CYieldSign3D::DrawSelect( C3DView * pView )
{
	glLoadName( pView->GetSelectionMap().NewSelectable( this));
	DrawOGL(pView);
}

void CYieldSign3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	CPath2008& path = const_cast<CPath2008&>(GetYieldSign()->GetPath());
	path.DoOffset(dx,dy,dz);
}

void CYieldSign3D::RenderEditPoint( C3DView * pView )
{
	glDisable(GL_TEXTURE_2D);
	if(!IsInEdit()) return;
	if(IsNeedSync()){
		DoSync();
	}	
	for(ALTObjectControlPoint2008List::iterator itr = m_vControlPoints.begin();itr!=m_vControlPoints.end();itr++){
		glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
		(*itr)->DrawSelect(pView);		 
	}
}

void CYieldSign3D::DoSync()
{
	m_vControlPoints.clear();
	int nPtCnt = GetYieldSign()->GetPath().getCount();

	CPath2008& path = const_cast<CPath2008&>(GetYieldSign()->GetPath());
	for(int i=0;i<nPtCnt;++i)
	{
		m_vControlPoints.push_back( new CALTObjectPathControlPoint2008(this,path,i) );
	}

	ALTObject3D::DoSync();	
}

void CYieldSign3D::OnRotate( DistanceUnit dx )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	ARCVector3 center = GetLocation();

	CPath2008& path = const_cast<CPath2008&>(GetYieldSign()->GetPath());
	path.DoOffset(-center[VX],-center[VY],-center[VZ]);
	path.Rotate(dx);
	path.DoOffset(center[VX],center[VY],center[VZ]);

}