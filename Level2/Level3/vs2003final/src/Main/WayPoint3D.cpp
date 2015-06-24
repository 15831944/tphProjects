#include "StdAfx.h"
#include ".\waypoint3d.h"

#include "..\InputAirside\AirWayPoint.h"

#include "..\InputAirside\ALTObjectDisplayProp.h"


#include "Airside3D.h"
#include "Airspace3D.h"
#include "Airport3D.h"

CWayPoint3D::CWayPoint3D( int id ) : ALTObject3D(id)
{
	m_pObj = new AirWayPoint;
	//m_pDisplayProp = new AirWayPointDisplayProp;	
	m_pAirside = NULL;
}
CWayPoint3D::~CWayPoint3D(void)
{
}

void CWayPoint3D::DrawOGL( C3DView * m_pView )
{	
	AirWayPointDisplayProp * pDisplay = (AirWayPointDisplayProp*)GetDisplayProp();
	glEnable(GL_LIGHTING);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	if(pDisplay->m_dpShape.bOn){
		glColor3ubv(pDisplay->m_dpShape.cColor);
		RenderShape(m_pView);
	}
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
}

void CWayPoint3D::DrawSelect( C3DView * pView )
{
	RenderShape(pView);
}

void CWayPoint3D::RenderShape( C3DView * pView )
{	
	
	AirWayPoint * pObj = (AirWayPoint * )m_pObj.get();
	

	DistanceUnit lowalt = pObj->GetLowLimit();
	DistanceUnit highalt = pObj->GetHighLimit();

	const double FixHight=100000.0;
	const double FixTopRad=8000*0.3048;

	double diskrlow=lowalt*FixTopRad/FixHight; 
	double  diskrhigh=highalt*FixTopRad/FixHight;
	
	
	GLUquadric* pQuadObj = gluNewQuadric();
	
	ARCVector3 ServPoint = GetLocation() ;

	glPushMatrix();
	glTranslated(ServPoint[VX],ServPoint[VY],ServPoint[VZ]);
	
	glPushMatrix();
	glTranslated(0,0,lowalt);
	gluCylinder(pQuadObj,diskrlow,diskrhigh,highalt-lowalt,36,12);
	glPopMatrix();


	glColor3ubv(ARCColor3::White);
	gluCylinder(pQuadObj, 0.0, diskrlow,lowalt, 36, 12);

	glPushMatrix();		
	glTranslated(0.0, 0.0, highalt);
	gluDisk(pQuadObj, 0.0, diskrhigh, 36, 8);								
	glPopMatrix();

	glPopMatrix();
	
	gluDeleteQuadric(pQuadObj);
	

}

ARCPoint3 CWayPoint3D::GetLocation( void ) const
{
	AirWayPoint * pObj = (AirWayPoint*)m_pObj.get(); 
	if(m_pAirside)
	{
		//link other way point
		CWayPoint3D  *pOtherWayPt  = NULL;
		//	
		if(GetAirWayPoint()->GetFlag() == 1)
		{		
			pOtherWayPt = (CWayPoint3D*) m_pAirside->GetAirspace()->GetObject3D( GetAirWayPoint()->GetOtherWayPointID() );
			if(pOtherWayPt)
				GetAirWayPoint()->SetOtherWayPoint(pOtherWayPt->GetAirWayPoint());
		}
		if(GetAirWayPoint()->GetFlag() == 2)
		{
			pOtherWayPt = (CWayPoint3D*)m_pAirside->GetAirspace()->GetObject3D( GetAirWayPoint()->GetOtherWayPoint1ID() );
			if(pOtherWayPt)
				GetAirWayPoint()->SetOtherWayPoint1(pOtherWayPt->GetAirWayPoint());
			pOtherWayPt = (CWayPoint3D*)m_pAirside->GetAirspace()->GetObject3D( GetAirWayPoint()->GetOtherWayPoint2ID() );
			if(pOtherWayPt)
				GetAirWayPoint()->SetOtherWayPoint2(pOtherWayPt->GetAirWayPoint());
		}
		
		CAirport3D  *pRefAirport = m_pAirside->GetActiveAirport();
		if(pRefAirport)
		{
			return pObj->GetLocation(pRefAirport->m_altAirport) - ARCPoint3(0,0,pRefAirport->m_altAirport.getElevation() );
		}
	}
	
	{
		//ASSERT(false);
		return pObj->GetServicePoint();
	}
}

void CWayPoint3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /*=0*/ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	GetAirWayPoint()->m_serviceLocation.DoOffset(dx,dy,dz);
}

AirWayPoint * CWayPoint3D::GetAirWayPoint()const
{
	return (AirWayPoint*)m_pObj.get();
}

//AirWayPointDisplayProp * CWayPoint3D::GetDisplayProp()
//{
//	return (AirWayPointDisplayProp*)m_pDisplayProp.get();
//}

void CWayPoint3D::Update( CAirside3D* pAirside )
{
	ALTObject3D::Update();
	m_pAirside = pAirside;
	ASSERT(pAirside);
}