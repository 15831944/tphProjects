#include "StdAfx.h"
#include ".\hold3d.h"
#include "..\InputAirside\AirHold.h"
#include "..\InputAirside\ALTObjectDisplayProp.h"

#include "..\InputAirside\AirWayPoint.h"
#include ".\Airside3d.h"
#include "WayPoint3D.h"
#include "Airspace3D.h"
#include "Airport3D.h"
#include "3DView.h"


CHold3D::CHold3D( int id ): ALTObject3D(id)
{
	m_pObj = new AirHold;
//	m_pDisplayProp = new AirHoldDisplayProp;
	m_pAirside = NULL;
}
CHold3D::~CHold3D(void) 
{
}

void CHold3D::DrawOGL( C3DView * m_pView )
{
	AirHoldDisplayProp * pDisplay = (AirHoldDisplayProp*)GetDisplayProp();//.get();
	
	if(!pDisplay->m_dpShape.bOn) return;
	
	if(IsSelected()) glLineWidth(3.0f);

	ARCColor4 cColor = GetDisplayProp()->m_dpShape.cColor;
	cColor[ALPHA]= 255/4;
	glColor4ubv(cColor);

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	RenderShape(m_pView);	
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	//RenderShadow(m_pView);


	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-2.0f,-2.0f);	
	RenderLines(m_pView);	
	glDisable(GL_POLYGON_OFFSET_LINE);
	glEnable(GL_LIGHTING);

	if(IsSelected()) glLineWidth(1.0f);
}

void CHold3D::DrawSelect( C3DView * pView )
{
	RenderShape(pView);
}

void CHold3D::RenderShape( C3DView * pView )
{
	
	DoSync();

	if(m_RoundPath.getCount()<1)return;

	CAirside3D* pAirside = pView->GetAirside3D();
	double dAirportAlt = pAirside->GetActiveAirport()->m_altAirport.getElevation();


	double minAlt = GetHold()->getMinAltitude() - dAirportAlt;
	double maxAlt = GetHold()->getMaxAltitude() - dAirportAlt;
	glBegin(GL_QUAD_STRIP);
	for(int i=0;i<m_RoundPath.getCount();++i){
		glVertex3d(m_RoundPath[i].getX(),m_RoundPath[i].getY(),minAlt);
		glVertex3d(m_RoundPath[i].getX(),m_RoundPath[i].getY(),maxAlt);
	}
	glVertex3d(m_RoundPath[0].getX(),m_RoundPath[0].getY(),minAlt);
	glVertex3d(m_RoundPath[0].getX(),m_RoundPath[0].getY(),maxAlt);
	glEnd();

	glBegin(GL_POLYGON);
	for(int i=0;i<m_RoundPath.getCount();++i){
		glVertex3d(m_RoundPath[i].getX(),m_RoundPath[i].getY(),minAlt);		
	}
	glEnd();

	glBegin(GL_POLYGON);
	for(int i=0;i<m_RoundPath.getCount();++i){
		glVertex3d(m_RoundPath[i].getX(),m_RoundPath[i].getY(),maxAlt);		
	}
	glEnd();
	
}

void CHold3D::RenderShadow( C3DView * pView )
{
	glDisable(GL_LIGHTING);

	static const ARCColor4 shadowcolor(128,128,128,196);
	glColor4ubv(shadowcolor);
	glBegin(GL_POLYGON);
	for(int i=0;i<m_RoundPath.getCount();++i){
		glVertex3d(m_RoundPath[i].getX(),m_RoundPath[i].getY(),0);		
	}
	glEnd();
	glEnable(GL_LIGHTING);
}

ARCPoint3 CHold3D::GetLocation( void ) const
{
	int nPtCnt = m_RoundPath.getCount();
	if(!nPtCnt)
	{
		return ARCPoint3(0,0,0);
	}
	ARCPoint3 location =  ( m_RoundPath.getPoint(0) + m_RoundPath.getPoint( int(nPtCnt * 0.5) ) ) * 0.5; 
	location[VZ] = ( GetHold()->getMaxAltitude() + GetHold()->getMinAltitude() ) * 0.5;
	return location;
}

void CHold3D::Update( CAirside3D* pAirside )
{
	ALTObject3D::Update();
	m_pAirside = pAirside;
}
void CHold3D::DoSync()
{
	int WayptID = GetHold()->GetWatpoint();
	
	if(m_pAirside && m_pAirside->GetAirspace() )
	{
		m_pWayPoint = m_pAirside->GetAirspace()->GetObject3D(WayptID);

		if( m_pWayPoint.get() )
		{
			CWayPoint3D * pWayPt3D = (CWayPoint3D*) m_pWayPoint.get();
			ARCPoint3 pt = pWayPt3D->GetLocation();	
			CAirport3D* pAirport3D  = pWayPt3D->m_pAirside->GetActiveAirport();
			ASSERT(pAirport3D);	
			Point fixPt = Point(pt[VX],pt[VY],pt[VZ]);
			m_RoundPath = GetHold()->GetPath(fixPt,pAirport3D->m_altAirport.GetMagnectVariation().GetRealVariation());				
		}
	}
	SyncDateType::DoSync();
}

AirHold * CHold3D::GetHold()const
{
	return (AirHold*)m_pObj.get();
}

//AirHoldDisplayProp * CHold3D::GetDisplayProp()
//{
//	return (AirHoldDisplayProp*)m_pDisplayProp.get();
//}

void CHold3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
}

void CHold3D::RenderLines( C3DView * pView )
{
	CAirside3D* pAirside = pView->GetAirside3D();
	double dAirportAlt = pAirside->GetActiveAirport()->m_altAirport.getElevation();

	double minAlt =GetHold()->getMinAltitude() - dAirportAlt ;
	double maxAlt = GetHold()->getMaxAltitude() - dAirportAlt;

	glBegin(GL_LINE_LOOP);
	for(int i=0;i<m_RoundPath.getCount();++i){
		glVertex3d(m_RoundPath[i].getX(),m_RoundPath[i].getY(),minAlt);		
	}
	glEnd();

	glBegin(GL_LINE_LOOP);
	for(int i=0;i<m_RoundPath.getCount();++i){
		glVertex3d(m_RoundPath[i].getX(),m_RoundPath[i].getY(),maxAlt);		
	}
	glEnd();
}

void CHold3D::FlushChange()
{
	try
	{
		GetHold()->UpdateObject(m_nID);
	}
	catch (CADOException& e)
	{
		CString strErr = e.ErrorMessage(); 
	}	
}

void CHold3D::OnRotate( DistanceUnit dx )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
}