#include "StdAfx.h"
#include ".\airsidepaxbusparkspot3d.h"
#include "..\InputAirside\AirsidePaxBusParkSpot.h"
#include "ShapeRenderer.h"
#include "InputAirside\AirsideBagCartParkSpot.h"

ARCPoint3 CAirsideParkingPos3D::GetLocation( void ) const
{
	const AirsideParkSpot* pSpot = getSpot();
	return pSpot->GetServicePoint();
}



void CAirsideParkingPos3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*= 0*/ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	getSpot()->DoOffset(dx,dy,dz);
}

void CAirsideParkingPos3D::OnRotate( DistanceUnit dx )
{
	if(m_pObj->GetLocked())
	{
		return;
	}
	getSpot()->OnRotate(dx, GetLocation() );
}

CAirsidePaxBusParkSpot3D::CAirsidePaxBusParkSpot3D( int id )
:CAirsideParkingPos3D(id)
{
	m_pObj = new AirsidePaxBusParkSpot();
}



void CAirsideParkingPos3D::DrawOGL( C3DView * m_pView )
{
	if(IsSelected()){
		glLineWidth(3.0f);
	}
	if(IsNeedSync()){
		DoSync();
	}
	glEnable(GL_LIGHTING);
	AirsideParkSpot * spot = (AirsideParkSpot*) m_pObj.get();
	ParkSpotDisplayProp * pDisplay = (ParkSpotDisplayProp*)GetDisplayProp();//.get();

	CPoint2008 pt = spot->GetServicePoint();

	//draw shape
	if( pDisplay->m_dpShape.bOn ) 
	{		
		glColor4ubv(pDisplay->m_dpShape.cColor);

		glPushMatrix();
		glTranslated(pt.getX(),pt.getY(),pt.getZ());	
		glBegin(GL_TRIANGLE_FAN);
			glNormal3d(0,0,1);
			glVertex2d(100,100);
			glVertex2d(-100,100);
			glVertex2d(-100,-100);
			glVertex2d(100,-100);
		glEnd();
		
		glPopMatrix();		
	}
	glDisable(GL_LIGHTING);
	//draw lead in line
	if( pDisplay->m_dpInConstr.bOn )
	{
		glColor4ubv(pDisplay->m_dpInConstr.cColor);		
		ARCGLRender::DrawArrowPath(spot->getLeadInLine(),68);			
			
	}
	if( pDisplay->m_dpOutConstr.bOn)
	{
		glColor4ubv(pDisplay->m_dpOutConstr.cColor);		
		ARCGLRender::DrawArrowPath(spot->getLeadOutLine(),68);
	}

	
	glEnable(GL_LIGHTING);
	glDepthMask(GL_FALSE);



	if(IsSelected())
	{
		glLineWidth(1.0f);
	}
}

void CAirsideParkingPos3D::DrawSelect( C3DView * pView )
{
	DrawOGL(pView);
}

CAirsideBagCartParkPos3D::CAirsideBagCartParkPos3D( int id )
:CAirsideParkingPos3D(id)
{
	m_pObj = new AirsideBagCartParkSpot();
}
