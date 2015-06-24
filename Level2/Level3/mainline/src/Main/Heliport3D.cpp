#include "StdAfx.h"
#include ".\heliport3d.h"
#include "../InputAirside/Heliport.h"
#include "..\InputAirside\ALTObjectDisplayProp.h"
#include "../Common/ARCPipe.h"
#include ".\3DView.h"
#include "glrender/glTextureResource.h"
#define PI  3.1415926

CHeliport3D::CHeliport3D(int id):ALTObject3D(id)
{
	m_pObj = new Heliport;
	//m_pDisplayProp = new HeliportDisplayProp;
	m_bInEdit = false;
}

CHeliport3D::~CHeliport3D(void)
{
}

void CHeliport3D::DrawOGL(C3DView * pView)
{
	if(IsNeedSync()){
		DoSync();
	}

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	RenderBase(pView);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-4.0,-4.0);

	glDepthMask(GL_FALSE);	
	RenderBase(pView);	
	glDepthMask(GL_TRUE);
	glDisable(GL_POLYGON_OFFSET_FILL);

	glDepthMask(GL_FALSE);
	glColor3ub(255,255,255);
	RenderEditPoint(pView);
	glDepthMask(GL_TRUE);
}

void CHeliport3D::DrawSelect(C3DView * pView)
{
	if(IsNeedSync())
	{
		DoSync();
	}

	DrawOGL(pView);

	if(IsInEdit())
	{
		RenderEditPoint(pView,true);
	}
}

Heliport* CHeliport3D::GetHeliport()const
{
	return (Heliport*)m_pObj.get();
}

void CHeliport3D::RenderEditPoint(C3DView* pView, bool bSelectMode /* = false */)
{
	glDisable(GL_TEXTURE_2D);
	if(!IsInEdit()) return;
	for(ALTObjectControlPoint2008List::iterator itr = m_vControlPoints.begin();itr!=m_vControlPoints.end();itr++){
		if(bSelectMode)
			glLoadName( pView->GetSelectionMap().NewSelectable( (*itr).get() ) );
		(*itr)->DrawSelect(pView);	
	}

}

void CHeliport3D::RenderBase(C3DView * pView)
{
	Heliport * pHeliport = GetHeliport();

	if(!pHeliport->GetPath().getCount())
		return ;

	ARCPipe pipepath (pHeliport->GetPath(),pHeliport->GetRadius()/2);
	int npippathcnt  = (int)pipepath.m_centerPath.size();

	CTexture * pTexture = pView->getTextureResource()->getTexture("RunwayBase");
	if(pTexture)
		pTexture->Apply();


	glBegin(GL_QUADS);
	ARCVector3 prePointL = pipepath.m_sidePath1[0];
	ARCVector3 prePointR  = pipepath.m_sidePath2[0];
	glColor3ub(255,255,255);
	glNormal3i(0,0,1);
	double dTexCoordLeft = 0;
	double dTexCoordRight  =0 ;

	for(int i=1;i<npippathcnt;i ++ )
	{
		glTexCoord2d(0,dTexCoordLeft);
		glVertex3dv(prePointL);
		glTexCoord2d(1,dTexCoordRight);
		glVertex3dv(prePointR);			

		DistanceUnit dLen = prePointL.DistanceTo(pipepath.m_sidePath1[i]);	
		dTexCoordLeft += dLen * 0.002;
		dLen = prePointR.DistanceTo(pipepath.m_sidePath2[i]);
		dTexCoordRight += dLen * 0.002;		

		glTexCoord2d(1,dTexCoordRight);
		glVertex3dv(pipepath.m_sidePath2[i]);
		glTexCoord2d(0,dTexCoordLeft);
		glVertex3dv(pipepath.m_sidePath1[i]);


		std::swap(dTexCoordLeft, dTexCoordRight);
		prePointL = pipepath.m_sidePath1[i];
		prePointR = pipepath.m_sidePath2[i];
	}
	glEnd();

	ARCVector3 m_vViewAngle;
	ARCVector3 m_vViewStrafe;

	m_vViewAngle = pipepath.m_centerPath[npippathcnt-1]-pipepath.m_centerPath[npippathcnt-2];

	float angle = (float)(180*acos(m_vViewAngle.Normalize().dot(ARCVector3(1.0,0.0,0.0)))/PI);
    if(m_vViewAngle[VY]<0)
		angle =360-angle;

	m_vPosition = pipepath.m_centerPath[npippathcnt-1];
	glPushMatrix();

	glTranslatef((GLfloat)m_vPosition[VX],(GLfloat)m_vPosition[VY],(GLfloat)m_vPosition[VZ]);
	glRotatef(angle, 0.0,0.0,1.0);	


	ARCVector3 m_TempX = ARCVector3(pHeliport->GetRadius(),0.0,0.0);
	ARCVector3 m_TempY = ARCVector3(0.0,pHeliport->GetRadius(),0.0);

	CTexture * pTextureOther = pView->getTextureResource()->getTexture("Heliport");
	if(pTextureOther)
		pTextureOther->Apply();
   
	glNormal3f(0.0,0.0,1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0,0.0);
	glVertex3dv( m_TempX + m_TempY);
	glTexCoord2f(0.0,1.0);
	glVertex3dv(-m_TempX + m_TempY);
	glTexCoord2f(1.0,1.0);
	glVertex3dv(-m_TempX - m_TempY);
	glTexCoord2f(1.0,0.0);
	glVertex3dv( m_TempX - m_TempY);
	glEnd();

	glPopMatrix();
}

void CHeliport3D::RotateVector(float angle, float x, float y, float z, ARCVector3& m_vView)
{
	ARCVector3 vNewView;
	ARCVector3 vView = m_vView;
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	vNewView[VX]  = (cosTheta+(1-cosTheta)*x*x)*vView[VX];
	vNewView[VX] += ((1-cosTheta)*x*y-z*sinTheta)*vView[VY];
	vNewView[VX] += ((1-cosTheta)*x*z+y*sinTheta)*vView[VZ];

	vNewView[VY]  = ((1-cosTheta)*x*y+z*sinTheta)*vView[VX];
	vNewView[VY] += (cosTheta+(1-cosTheta)*y*y)*vView[VY];
	vNewView[VY] += ((1-cosTheta)*y*z-x*sinTheta)*vView[VZ];

	vNewView[VZ]  = ((1-cosTheta)*x*z-y*sinTheta)*vView[VX];
	vNewView[VZ] += ((1-cosTheta)*y*z+x*sinTheta)*vView[VY];
	vNewView[VZ] += (cosTheta+(1-cosTheta)*z*z)*vView[VZ];

	m_vView = vNewView;

}

void CHeliport3D::FlushChange()
{
	try
	{
		GetHeliport()->UpdatePath();
	}
	catch (CADOException& e)
	{
		CString strErr = e.ErrorMessage(); 
	}	
}

ARCPoint3 CHeliport3D::GetLocation( void ) const
{
	Heliport * pHeliport = (Heliport*)m_pObj.get();
	const CPath2008& path = pHeliport->GetPath();

	int nMid = int( (path.getCount()-1) * 0.5);

	if(nMid<path.getCount()-1)
		return (path.getPoint(nMid) + path.getPoint(nMid+1) )*0.5;

	return ARCPoint3(0,0,0);

}



void CHeliport3D::OnMove(DistanceUnit dx, DistanceUnit dy,DistanceUnit dz )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	GetHeliport()->m_path.DoOffset(dx,dy,dz);
}

void CHeliport3D::OnRotate( DistanceUnit dx )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	ARCVector3 center = GetLocation();

	GetHeliport()->getPath().DoOffset(-center[VX],-center[VY],-center[VZ]);
	GetHeliport()->getPath().Rotate(dx);
	GetHeliport()->getPath().DoOffset(center[VX],center[VY],center[VZ]);
}


void CHeliport3D::DoSync()
{
	m_vControlPoints.clear();
	int nPtCnt = GetHeliport()->GetPath().getCount();

	for(int i=0;i<nPtCnt;++i)
	{
		m_vControlPoints.push_back( new CALTObjectPathControlPoint2008(this,GetHeliport()->getPath(),i) );
	}
	SyncDateType::DoSync();
}
