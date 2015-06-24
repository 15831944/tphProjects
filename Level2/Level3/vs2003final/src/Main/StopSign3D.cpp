#include "StdAfx.h"
#include "../InputAirside/StopSign.h"
#include "../InputAirside/ALTObjectDisplayProp.h"
#include ".\StopSign3D.h"
#include "../Common/ARCPipe.h"
#include ".\3DView.h"
#include "Floors.h"
#include "TermPlanDoc.h"


CStopSign3D::CStopSign3D(int nID):ALTObject3D(nID)
{
	m_pObj = new StopSign();
	//m_pDisplayProp = new StopSignDisplayProp();
	m_pModel = NULL;
	m_pModel2D = NULL;
}

CStopSign3D::~CStopSign3D(void)
{
}

void CStopSign3D::DrawOGL( C3DView * pView )
{
	if(IsNeedSync()){
		DoSync();
	}
    glDisable(GL_POLYGON_OFFSET_FILL);
	const CPath2008& path = GetStopSign()->GetPath();
	if(path.getCount()<1)return;

	ARCVector3 position = path.getPoint(0);

	CFloors & floors = pView->GetDocument()->GetFloorByMode(EnvMode_AirSide);
	position[VZ] = floors.getVisibleAltitude( position[VZ] );


	glPushMatrix();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glTranslated(position[VX],position[VY],position[VZ]);	
	glRotated(GetStopSign()->GetRotation(), 0.0, 0.0, 1.0);
	//glScaled(m_pProc2->GetScale()[VX], m_pProc2->GetScale()[VY], m_pProc2->GetScale()[VZ]);

	if(!m_pModel)
		m_pModel =  pView->getModelRes().getModel(C3DModelResource::StopSignModel);
	if(!m_pModel2D)
		m_pModel2D = pView->getModelRes().getModel(C3DModelResource::StopSign2DModel);

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


ARCPoint3 CStopSign3D::GetLocation( void ) const
{
	StopSign * pStopSign = (StopSign*)m_pObj.get();
	const CPath2008& path = pStopSign->GetPath();
	if(path.getCount() < 1)return (ARCPoint3(0,0,0));
	return path.getPoint(0);
}

StopSign * CStopSign3D::GetStopSign()
{
	return (StopSign*) GetObject();
}


void CStopSign3D::DrawSelect( C3DView * pView )
{
	glLoadName( pView->GetSelectionMap().NewSelectable( this));
	DrawOGL(pView);
}

void CStopSign3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	CPath2008& path = const_cast<CPath2008&>(GetStopSign()->GetPath());
	path.DoOffset(dx,dy,dz);
}

void CStopSign3D::RenderEditPoint( C3DView * pView )
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

void CStopSign3D::DoSync()
{
	m_vControlPoints.clear();
	int nPtCnt = GetStopSign()->GetPath().getCount();

	CPath2008& path = const_cast<CPath2008&>(GetStopSign()->GetPath());
	for(int i=0;i<nPtCnt;++i)
	{
		m_vControlPoints.push_back( new CALTObjectPathControlPoint2008(this,path,i) );
	}

	ALTObject3D::DoSync();	
}

void CStopSign3D::OnRotate( DistanceUnit dx )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	ARCVector3 center = GetLocation();

	CPath2008& path = const_cast<CPath2008&>(GetStopSign()->GetPath());
	path.DoOffset(-center[VX],-center[VY],-center[VZ]);
	path.Rotate(dx);
	path.DoOffset(center[VX],center[VY],center[VZ]);

}