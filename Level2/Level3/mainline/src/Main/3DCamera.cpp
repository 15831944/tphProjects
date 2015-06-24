// 3DCamera.cpp: implementation of the C3DCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "3DCamera.h"

#include "common\ARCMatrix4x4.h"
#include "common\TERMFILE.H"
#include "common\point.h"

#include <math.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define CDGAIN	50.0


void C3DCamera::Interpolate(const C3DCamera& c1, const C3DCamera& c2, double t, C3DCamera& cres)
{
	cres.m_vLocation = (1.0-t)*c2.m_vLocation + t*c1.m_vLocation;

	cres.m_vLook = (1.0-t)*c2.m_vLook + t*c1.m_vLook;

	cres.m_vUp = (1.0-t)*c2.m_vUp + t*c1.m_vUp;

	cres.m_dFocusDistance = (1.0 -t) * c2.m_dFocusDistance + t * c1.m_dFocusDistance;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


C3DCamera::C3DCamera()	
{
	m_sName =_T("default");
	InitDefault(perspective);
}

C3DCamera::C3DCamera(PROJECTIONTYPE eType)
{
	if(eType == perspective)
		m_sName = (eType == perspective)?_T(" 3D "):_T(" 2D ");
	InitDefault(eType);
}

C3DCamera::C3DCamera(LPCTSTR lpszName,PROJECTIONTYPE eType)	
{
	m_sName = (lpszName);
	InitDefault(eType);
}

C3DCamera::C3DCamera(const C3DCamera& cam)	
{
	m_vLocation=(cam.m_vLocation);
	m_vLook=cam.m_vLook;
	m_vUp=(cam.m_vUp);
	m_sName=(cam.m_sName);
	m_iZoom=(cam.m_iZoom);
	m_dFarPlane=(cam.m_dFarPlane);
	m_dNearPlane=(cam.m_dNearPlane);
	m_eProjType=(cam.m_eProjType);
	m_dFocusDistance=(cam.m_dFocusDistance);
	m_vWorldC=(cam.m_vWorldC);
	m_dWorldR=(cam.m_dWorldR);
	m_bdirty=true;
}

C3DCamera::C3DCamera( const CCameraData& camd ):CCameraData(camd)
{
	m_bdirty = true;
}

void C3DCamera::InitDefault(PROJECTIONTYPE eType)
{
	m_eProjType = eType;
	switch(eType)
	{
	case perspective:
		m_vLocation[VX] = 0.0;
		m_vLocation[VY] = -16000.0;
		m_vLocation[VZ] = 8000.0;

		m_vLook = -m_vLocation;

		m_vLook.Normalize();

		m_vUp = ARCVector3(1.0, 0.0, 0.0) ^ m_vLook;

		m_vUp.Normalize();

		m_dFocusDistance = m_vLocation.Magnitude();
		break;
	case parallel:
		m_vLocation[VX] = 0.0;
		m_vLocation[VY] = 0.0;
		m_vLocation[VZ] = 8000.0;
		m_vLook = -m_vLocation;
		m_vLook.Normalize();
		m_vUp = ARCVector3(1.0, 0.0, 0.0) ^ m_vLook;
		m_vUp.Normalize();
		m_dFocusDistance = m_vLocation[VZ];
		break;
	}
	
	m_iZoom = 100;				// 100%

	m_dFarPlane =  50000.0;
	m_dNearPlane = 10.0;

	m_vWorldC = ARCVector3(0.0, 0.0, 10000.0);
	m_dWorldR = 20000.0;

	m_bdirty=true;
}

C3DCamera::~C3DCamera()
{
}

C3DCamera& C3DCamera::operator =(const C3DCamera& _rhs)
{
	if(this == &_rhs)
		return *this;
	
	m_vLocation = _rhs.m_vLocation;
	m_vLook = _rhs.m_vLook;
	m_vUp = _rhs.m_vUp;
	m_sName = _rhs.m_sName;
	m_iZoom = _rhs.m_iZoom;
	m_dFarPlane = _rhs.m_dFarPlane;
	m_dNearPlane = _rhs.m_dNearPlane;
	m_eProjType = _rhs.m_eProjType;
	m_dFocusDistance = _rhs.m_dFocusDistance;
	m_vWorldC = _rhs.m_vWorldC;
	m_dWorldR = _rhs.m_dWorldR;

	m_bdirty=true;
	return *this;
}

void C3DCamera::ApplyGLProjection(double dAspectRatio) const
{
	switch(m_eProjType)
	{
	case perspective:
		{
			//ptC should be the point between the center of the first floor and the center of the last floor
			//r should be equal to sqrt(2*gridextents^2) or sqrt(2*floorsep^2) whichever is larger
			ARCVector3 ptD = m_vWorldC - m_vLocation;
			if(ptD.Magnitude() < m_dWorldR) {
				m_dNearPlane =100;
				m_dFarPlane = 2*m_dWorldR;
			}
			else {
				m_dNearPlane = ptD.Magnitude() - m_dWorldR;
				m_dFarPlane = m_dNearPlane + 2*m_dWorldR;
			}
			//m_dNearPlane = 10.0;
			//m_dFarPlane = 200000.0;
 			gluPerspective(NORMAL_ZOOM_FOV * (100.0 / m_iZoom), dAspectRatio,m_dNearPlane, m_dFarPlane);
			break;
		}
	case parallel:
		{
			
			m_dFarPlane = m_dFocusDistance*4;
			m_dNearPlane = m_dFocusDistance - m_dWorldR;
			
			if(m_dNearPlane < 100)
				m_dNearPlane = 100;
			
			//m_dFarPlane = m_dFocusDistance  + 1000.0;
			//m_dNearPlane = m_dFocusDistance - 5000.0;
			/*ARCVector3 ptD = m_vWorldC - m_vLocation;
			m_dNearPlane = ptD.Magnitude() - m_dWorldR;
			m_dFarPlane = m_dNearPlane + 2*m_dWorldR;*/

			//double dZoomFact = m_vLocation[VZ] / 50.0;
			double dZoomFact = m_dFocusDistance / 50.0;
			glOrtho(-40.0 * dZoomFact * dAspectRatio, 40.0 * dZoomFact * dAspectRatio, -40.0 * dZoomFact, 40.0 * dZoomFact, 0, m_dFarPlane);
			break;
		}
	}
}

void C3DCamera::ApplyCompassGLProjection() const
{
	gluPerspective(7.0, 1.0, 1, 200);
}

void C3DCamera::ApplyGLView() const
{
		gluLookAt(m_vLocation[VX], m_vLocation[VY], m_vLocation[VZ],
				  m_vLocation[VX] + m_vLook[VX]*1000, m_vLocation[VY] + m_vLook[VY]*1000, m_vLocation[VZ] + m_vLook[VZ]*1000,
				  m_vUp[VX], m_vUp[VY], m_vUp[VZ]);
}

void C3DCamera::ApplyCompassGLView() const
{
	ARCVector3 v(m_vLook * 25);

	gluLookAt(-v[VX], -v[VY], -v[VZ],
			  0.0, 0.0, 0.0,
			  m_vUp[VX], m_vUp[VY], m_vUp[VZ]);
}

void C3DCamera::Tumble(const ARCVector2& _mm)
{
	if(m_eProjType == perspective) {
	
		//for x mouse movement, rotate location vector around up vector
		//for y mouse movement, rotate location vector around right vector
		double dZAngle = 0.3*_mm[VX];
		double dRAngle = 0.3*_mm[VY];
		
		//correct for being upside down
		if(m_vUp[VZ] > 0)
			dZAngle = -dZAngle;

		ARCVector3 zAxis(0.0, 0.0, 1.0);	// z axis
		ARCVector3 vFixPt(m_vLocation+m_dFocusDistance*m_vLook);
		ARCMatrix4x4 Mt,Mrz,Mrr;
		ARCMatrix4x4::GetTranslate3D(-vFixPt, Mt);
		m_vLocation = Mt * m_vLocation;
		ARCMatrix4x4::GetRotate3D(VZ, dZAngle, Mrz);
		ARCMatrix4x4::GetRotate3D(m_vLook^m_vUp, dRAngle, Mrr);
		m_vLook = Mrz * (Mrr * m_vLook);
		m_vUp = Mrz * (Mrr * m_vUp);
		m_vLocation = Mrz * (Mrr * m_vLocation);
		ARCMatrix4x4::GetTranslate3D(vFixPt, Mt);
		m_vLocation = Mt * m_vLocation;
	}
	else {
		double dAngle = 0.3*_mm[VX];
		ARCVector3 zAxis(0.0,0.0,1.0);
		ARCMatrix4x4 Mr;
		ARCMatrix4x4::GetRotate3D(VZ,dAngle,Mr);
		m_vUp = Mr * m_vUp;
	}

	m_bdirty=true;
}

void C3DCamera::Pan(const ARCVector2& _mm)
{
	//move in +ve or -ve direction along plane defined by up vector and right vector
	ARCVector3 vRight(m_vLook ^ m_vUp);
	vRight.Normalize();
	// TRACE("Up mag: %f\n", m_vUp.Magnitude());
	double dAdjFact = m_dFocusDistance/500.0;
	if( abs(dAdjFact) < 0.5 && dAdjFact < 0)
	{
		dAdjFact = -0.5;
	}
	else if (dAdjFact < 0.5 && dAdjFact >= 0)
	{
		dAdjFact = 0.5;
	}
	m_vLocation = m_vLocation - dAdjFact*(_mm[VX]*vRight + _mm[VY]*m_vUp);

	m_bdirty=true;
}

void C3DCamera::Zoom(DistanceUnit _mm)
{
	//move in +ve or -ve direction along look vector
	
	//// TRACE("Look mag: %f\n", m_vLook.Magnitude());
	//// TRACE("m_dFocusDistance = %f\n", m_dFocusDistance);
	
	double dAdjFact = abs(m_dFocusDistance/500.0);
	dAdjFact = max(1.0,dAdjFact);

	m_vLocation = m_vLocation + dAdjFact*_mm*m_vLook;

	if (m_eProjType == parallel && m_vLocation[VZ] < 0)
	{
		m_vLocation[VZ] = abs(m_vLocation[VZ]);
	}
	m_bdirty=true;
}

BOOL C3DCamera::WriteCamera(ArctermFile* pFile) const
{
	if(!m_sName.IsEmpty())
		pFile->writeField(m_sName);
	else
		pFile->writeField(_T("unamed view"));
	pFile->writeInt((short) m_eProjType);
	pFile->writeFloat(static_cast<float>(m_vLocation[VX])); pFile->writeFloat(static_cast<float>(m_vLocation[VY])); pFile->writeFloat(static_cast<float>(m_vLocation[VZ]));
	pFile->writeFloat(static_cast<float>(m_vLook[VX])); pFile->writeFloat(static_cast<float>(m_vLook[VY])); pFile->writeFloat(static_cast<float>(m_vLook[VZ]));
	pFile->writeFloat(static_cast<float>(m_vUp[VX])); pFile->writeFloat(static_cast<float>(m_vUp[VY])); pFile->writeFloat(static_cast<float>(m_vUp[VZ]));
	pFile->writeInt(m_iZoom);
	pFile->writeFloat(static_cast<float>(m_dFocusDistance));
	pFile->writeFloat(static_cast<float>(m_dNearPlane));
	pFile->writeFloat(static_cast<float>(m_dFarPlane));
	pFile->writeFloat(static_cast<float>(m_vWorldC[VX])); pFile->writeFloat(static_cast<float>(m_vWorldC[VY])); pFile->writeFloat(static_cast<float>(m_vWorldC[VZ]));
	pFile->writeFloat(static_cast<float>(m_dWorldR));
	return TRUE;
}

BOOL C3DCamera::ReadCamera(ArctermFile* pFile)
{
	short sn;
	char buf[256];
	pFile->getField(buf,255);
	m_sName = buf;
	pFile->getInteger(sn);
	m_eProjType = (PROJECTIONTYPE) sn;
	pFile->getFloat(m_vLocation[VX]); pFile->getFloat(m_vLocation[VY]); pFile->getFloat(m_vLocation[VZ]);
	pFile->getFloat(m_vLook[VX]); pFile->getFloat(m_vLook[VY]); pFile->getFloat(m_vLook[VZ]);
	pFile->getFloat(m_vUp[VX]); pFile->getFloat(m_vUp[VY]); pFile->getFloat(m_vUp[VZ]);
	pFile->getInteger(m_iZoom);
	pFile->getFloat(m_dFocusDistance);
	pFile->getFloat(m_dNearPlane);
	pFile->getFloat(m_dFarPlane);
	pFile->getFloat(m_vWorldC[VX]); pFile->getFloat(m_vWorldC[VY]); pFile->getFloat(m_vWorldC[VZ]);
	pFile->getFloat(m_dWorldR);
	return TRUE;
	m_bdirty=true;
}



void C3DCamera::AutoAdjustCameraFocusDistance(double dFocusPlaneAlt)
{
	if(m_vLook[VZ] == 0.0)
		return;
	if(m_eProjType == parallel) {
		dFocusPlaneAlt = 0.0;
	}
	double res = m_vLook.Magnitude()*(dFocusPlaneAlt-m_vLocation[VZ])/m_vLook[VZ];
	//// TRACE("focus distance: %.2f\n", res);
	/*if(res < 0.0 || res > 1000000.0) {
		return;
	}*/
	m_dFocusDistance = res;
	m_bdirty=true;
}

void C3DCamera::Make2D()
{
	if(parallel==m_eProjType)
		return; //already 2D
	m_eProjType = parallel;
	m_sName = _T(" 2D ");

	double alpha = -m_vLocation[VZ]/m_vLook[VZ];
	m_vLocation[VX] = m_vLocation[VX]+alpha*m_vLook[VX];
	m_vLocation[VY] = m_vLocation[VY]+alpha*m_vLook[VY];

	m_vLook = ARCVector3(0.0,0.0,-m_vLocation[VZ]);
	m_vLook.Normalize();

	m_vUp = (m_vLook^m_vUp)^m_vLook;
	m_vUp.Normalize();

	//// TRACE("Mag(Up): %.2f Mag(Look): %.2f\n", m_vUp.Magnitude(), m_vLook.Magnitude());
	m_bdirty=true;
}

void C3DCamera::Make3D()
{
	if(perspective==m_eProjType)
		return; //already 3D
	m_eProjType = perspective;
	m_sName = _T(" 3D ");

	m_bdirty=true;
}

void C3DCamera::SetCamera(const ARCVector3& _eye, const ARCVector3& _look, const ARCVector3& _up)
{
	m_vLocation = _eye;
	m_vLook = _look;
	m_vLook.Normalize();
	ARCVector3 r = _up ^ m_vLook;
	r.Normalize();
	m_vUp = m_vLook ^ r;
	m_vUp.Normalize();
	m_dFocusDistance = m_vLocation.Magnitude();

	m_bdirty=true;
}
//
void C3DCamera::ExtractFrustum(void ) {
	if(!m_bdirty)return;
	double proj[16]; // For Grabbing The PROJECTION Matrix
	double modl[16]; // For Grabbing The MODELVIEW Matrix
	double clip[16]; // Result Of Concatenating PROJECTION and MODELVIEW
	double t; // Temporary Work Variable

	glGetDoublev( GL_PROJECTION_MATRIX, proj ); // Grab The Current PROJECTION Matrix
	glGetDoublev( GL_MODELVIEW_MATRIX, modl ); // Grab The Current MODELVIEW Matrix

	// Concatenate (Multiply) The Two Matricies
	clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];


	// Extract the RIGHT clipping plane
	frustum[0][0] = clip[ 3] - clip[ 0];
	frustum[0][1] = clip[ 7] - clip[ 4];
	frustum[0][2] = clip[11] - clip[ 8];
	frustum[0][3] = clip[15] - clip[12];

	//// Normalize it
	t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
	frustum[0][0] /= t;
	frustum[0][1] /= t;
	frustum[0][2] /= t;
	frustum[0][3] /= t;


	// Extract the LEFT clipping plane
	frustum[1][0] = clip[ 3] + clip[ 0];
	frustum[1][1] = clip[ 7] + clip[ 4];
	frustum[1][2] = clip[11] + clip[ 8];
	frustum[1][3] = clip[15] + clip[12];

	//// Normalize it
	t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
	frustum[1][0] /= t;
	frustum[1][1] /= t;
	frustum[1][2] /= t;
	frustum[1][3] /= t;


	// Extract the BOTTOM clipping plane
	frustum[2][0] = clip[ 3] + clip[ 1];
	frustum[2][1] = clip[ 7] + clip[ 5];
	frustum[2][2] = clip[11] + clip[ 9];
	frustum[2][3] = clip[15] + clip[13];

	//// Normalize it
	t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
	frustum[2][0] /= t;
	frustum[2][1] /= t;
	frustum[2][2] /= t;
	frustum[2][3] /= t;


	// Extract the TOP clipping plane
	frustum[3][0] = clip[ 3] - clip[ 1];
	frustum[3][1] = clip[ 7] - clip[ 5];
	frustum[3][2] = clip[11] - clip[ 9];
	frustum[3][3] = clip[15] - clip[13];

	//// Normalize it
	t =  sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
	frustum[3][0] /= t;
	frustum[3][1] /= t;
	frustum[3][2] /= t;
	frustum[3][3] /= t;


	// Extract the FAR clipping plane
	frustum[4][0] = clip[ 3] - clip[ 2];
	frustum[4][1] = clip[ 7] - clip[ 6];
	frustum[4][2] = clip[11] - clip[10];
	frustum[4][3] = clip[15] - clip[14];

	//// Normalize it
	//t =  sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
	frustum[4][0] /= t;
	frustum[4][1] /= t;
	frustum[4][2] /= t;
	frustum[4][3] /= t;


	// Extract the NEAR clipping plane.  This is last on purpose (see pointinfrustum() for reason)
	frustum[5][0] = clip[ 3] + clip[ 2];
	frustum[5][1] = clip[ 7] + clip[ 6];
	frustum[5][2] = clip[11] + clip[10];
	frustum[5][3] = clip[15] + clip[14];

	//// Normalize it
	t =  sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
	frustum[5][0] /= t;
	frustum[5][1] /= t;
	frustum[5][2] /= t;
	frustum[5][3] /= t;

	m_bdirty=false;
}
bool C3DCamera::PointInCamera(DistanceUnit x,DistanceUnit y,DistanceUnit z ){
	if(m_bdirty)ExtractFrustum();
	for(int p = 0; p < 6; p++ )
	{
		if( frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <=0.0  )
		{
			return false;
		}
	}

	return true;
}
bool C3DCamera::SphereInCamera(const Point & centerpt, double radius){
	if(m_bdirty)ExtractFrustum();
	for(int p = 0; p < 6; p++ )
	{
		if( frustum[p][0] * centerpt.getX() + frustum[p][1] * centerpt.getY() + frustum[p][2] *centerpt.getZ() + frustum[p][3] <= -radius )
			return false;
	}

	return true;
}
bool C3DCamera::CubeInCamera(const Point & p1,const Point & p2){
	if(m_bdirty)ExtractFrustum();
	int i ;
	ARCVector3 vertex[8];
	vertex[0][0]=p1.getX(); vertex[0][1]=p1.getY(); vertex[0][2]=p1.getZ();
	vertex[1][0]=p1.getX(); vertex[1][1]=p1.getY(); vertex[1][2]=p2.getZ();
	vertex[2][0]=p1.getX(); vertex[2][1]=p2.getY(); vertex[2][2]=p1.getZ();
	vertex[3][0]=p1.getX(); vertex[3][1]=p2.getY(); vertex[3][2]=p2.getZ();
	vertex[4][0]=p2.getX(); vertex[4][1]=p1.getY(); vertex[4][2]=p1.getZ();
	vertex[5][0]=p2.getX(); vertex[5][1]=p1.getY(); vertex[5][2]=p2.getZ();
	vertex[6][0]=p2.getX(); vertex[6][1]=p2.getY(); vertex[6][2]=p1.getZ();
	vertex[7][0]=p2.getX(); vertex[7][1]=p2.getY(); vertex[7][2]=p2.getZ();
	for( i = 0 ; i < 8 ; i++)
	{
		if(PointInCamera(vertex[i][VX] , vertex[i][VY] , vertex[i][VZ]))
			return true;
	}
	return false;
}

void C3DCamera::Update()
{
	m_bdirty = true;
}