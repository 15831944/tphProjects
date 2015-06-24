#include "StdAfx.h"
#include "../InputAirside/TollGate.h"
#include "../InputAirside/ALTObjectDisplayProp.h"
#include ".\TollGate3D.h"
#include "../Common/ARCPipe.h"
#include ".\3DView.h"
#include "Floors.h"
#include "TermPlanDoc.h"

CTollGate3D::CTollGate3D(int nID):ALTObject3D(nID)
{
	m_pObj = new TollGate();
	//m_pDisplayProp = new TollGateDisplayProp();
	m_pModel = NULL;
	m_pModel2D = NULL;
}

CTollGate3D::~CTollGate3D(void)
{
}

void CTollGate3D::DrawOGL( C3DView * pView )
{
	if(IsNeedSync()){
		DoSync();
	}
	//glDisable(GL_POLYGON_OFFSET_FILL);
	const CPath2008& path = GetTollGate()->GetPath();
	if(path.getCount()<1)return;

	ARCVector3 position = path.getPoint(0);

	CFloors & floors = pView->GetDocument()->GetFloorByMode(EnvMode_AirSide);
	position[VZ] = floors.getVisibleAltitude( position[VZ] );


	glPushMatrix();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glTranslated(position[VX],position[VY],position[VZ]);	
	glRotated(GetTollGate()->GetRotation(), 0.0, 0.0, 1.0);
	//glScaled(m_pProc2->GetScale()[VX], m_pProc2->GetScale()[VY], m_pProc2->GetScale()[VZ]);

	if(!m_pModel)
		m_pModel =  pView->getModelRes().getModel(C3DModelResource::TollGateModel);
	if(!m_pModel2D)
		m_pModel2D = pView->getModelRes().getModel(C3DModelResource::TollGate2DModel);

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


ARCPoint3 CTollGate3D::GetLocation( void ) const
{
	TollGate * pTrafficLight = (TollGate*)m_pObj.get();
	const CPath2008& path = pTrafficLight->GetPath();
	if(path.getCount() < 1)return (ARCPoint3(0,0,0));
	return path.getPoint(0);
}

TollGate * CTollGate3D::GetTollGate()
{
	return (TollGate*) GetObject();
}


void CTollGate3D::DrawSelect( C3DView * pView )
{
	glLoadName( pView->GetSelectionMap().NewSelectable( this));
	DrawOGL(pView);
}

void CTollGate3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	CPath2008& path = const_cast<CPath2008&>(GetTollGate()->GetPath());
	path.DoOffset(dx,dy,dz);
}

void CTollGate3D::RenderEditPoint( C3DView * pView )
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

void CTollGate3D::DoSync()
{
	m_vControlPoints.clear();
	int nPtCnt = GetTollGate()->GetPath().getCount();

	CPath2008& path = const_cast<CPath2008&>(GetTollGate()->GetPath());
	for(int i=0;i<nPtCnt;++i)
	{
		m_vControlPoints.push_back( new CALTObjectPathControlPoint2008(this,path,i) );
	}

	ALTObject3D::DoSync();	
}

void CTollGate3D::OnRotate( DistanceUnit dx )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	ARCVector3 center = GetLocation();

	CPath2008& path = const_cast<CPath2008&>(GetTollGate()->GetPath());
	path.DoOffset(-center[VX],-center[VY],-center[VZ]);
	path.Rotate(dx);
	path.DoOffset(center[VX],center[VY],center[VZ]);

}