#include "StdAfx.h"
#include ".\sector3d.h"
#include "..\InputAirside\AirSector.h"
#include "..\InputAirside\ALTObjectDisplayProp.h"
#include "3DView.h"
#include "Airside3D.h"
#include "Airport3D.h"

CAirSector3D::CAirSector3D(int id) : ALTObject3D(id)
{
	//m_pDisplayProp = new AirSectorDisplayProp;
	m_pObj = new AirSector;
}
CAirSector3D::~CAirSector3D(void)
{
}

void CAirSector3D::DrawOGL( C3DView * m_pView )
{
	AirSectorDisplayProp * pDisplay = (AirSectorDisplayProp * )GetDisplayProp();//.get();

	if(!pDisplay->m_dpShape.bOn)return;
	
	if(IsSelected()) glLineWidth(3.0f);

	ARCColor4 cColor(255,0,0,0);
	if(GetDisplayProp())
	cColor = GetDisplayProp()->m_dpShape.cColor;
	cColor[ALPHA]= 255/4.0;
	glColor4f(cColor[RED]/255.0,cColor[GREEN]/255.0,cColor[BLUE]/255.0,cColor[ALPHA]/255.0);

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	
	Render(m_pView);	
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	//RenderShadow(m_pView);
	
	
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.0f,-1.0f);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		Render(m_pView);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);		
	glDisable(GL_POLYGON_OFFSET_LINE);

	glEnable(GL_LIGHTING);
	
	if(IsSelected()) glLineWidth(1.0f);
}

void CAirSector3D::Render( C3DView * pView )
{
	AirSector * pObj = (AirSector*)m_pObj.get();

	CPath2008 path = pObj->GetPath();

	CAirside3D* pAirside = pView->GetAirside3D();
	double dAirportAlt = pAirside->GetActiveAirport()->m_altAirport.getElevation();

	if(path.getCount()<1)return;
	double minAlt = pObj->GetLowAltitude() - dAirportAlt;
	double maxAlt = pObj->GetHighAltitude() - dAirportAlt;
	
	
	glBegin(GL_QUAD_STRIP);
	for(int i=0;i<path.getCount();++i){
		glVertex3d(path[i].getX(),path[i].getY(),minAlt);
		glVertex3d(path[i].getX(),path[i].getY(),maxAlt);
	}
	glVertex3d(path[0].getX(),path[0].getY(),minAlt);
	glVertex3d(path[0].getX(),path[0].getY(),maxAlt);
	glEnd();

	glBegin(GL_POLYGON);
	for(int i=0;i<path.getCount();++i){
		glVertex3d(path[i].getX(),path[i].getY(),minAlt);		
	}
	glEnd();

	glBegin(GL_POLYGON);
	for(int i=0;i<path.getCount();++i){
		glVertex3d(path[i].getX(),path[i].getY(),maxAlt);		
	}
	glEnd();
	glDepthMask(TRUE);
	

	
	/*glBegin(GL_LINE_LOOP);
	for(int i=0;i<path.getCount();++i){
		glVertex3d(path[i].getX(),path[i].getY(),minAlt);		
	}
	glEnd();

	glBegin(GL_LINE_LOOP);
	for(int i=0;i<path.getCount();++i){
		glVertex3d(path[i].getX(),path[i].getY(),maxAlt);		
	}
	glEnd();

	glBegin(GL_LINES);
	for(int i=0;i<path.getCount();++i){
		glVertex3d(path[i].getX(),path[i].getY(),maxAlt);	
		glVertex3d(path[i].getX(),path[i].getY(),minAlt);
	}
	glEnd();*/

	
}

void CAirSector3D::RenderShadow( C3DView * pView )
{
	glDisable(GL_LIGHTING);
	AirSector * pObj = (AirSector*)m_pObj.get();

	CPath2008 path = pObj->GetPath();
	
	static const ARCColor4 shadowcolor(128,128,128,196);
	glColor4ubv(shadowcolor);
	glBegin(GL_POLYGON);
	for(int i=0;i<path.getCount();++i){
		glVertex3d(path[i].getX(),path[i].getY(),0);		
	}
	glEnd();
	glEnable(GL_LIGHTING);
}

void CAirSector3D::DrawSelect( C3DView * pView )
{
	Render(pView);
}

ARCPoint3 CAirSector3D::GetLocation( void ) const
{
	const CPath2008& path = GetSector()->GetPath();
	ARCPoint3 location(0,0,0);
	for(int i =0 ;i<path.getCount();i++){
		location += path.getPoint(i);
	}
	location /= path.getCount();
	return location;
}

void CAirSector3D::OnMove( DistanceUnit dx, DistanceUnit dy,DistanceUnit dz /* = 0 */ )
{
	if (m_pObj->GetLocked())
	{
		return;
	}
	GetSector()->m_path.DoOffset(dx,dy,dz);
}

AirSector * CAirSector3D::GetSector()const
{
	return (AirSector*)m_pObj.get();
}

void CAirSector3D::FlushChange()
{
	try
	{
		GetSector()->UpdatePath();
	}
	catch (CADOException& e)
	{
		CString strErr = e.ErrorMessage(); 
	}	
}

void CAirSector3D::Update()
{
	try
	{
		GetSector()->ReadObject(m_nID);
	}
	catch (CADOException& e)
	{
		CString strErr = e.ErrorMessage(); 
	}
	SyncDateType::Update();
}