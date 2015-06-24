#include "StdAfx.h"
#include ".\airroute3d.h"
#include "..\InputAirside\AirRoute.h"
#include "..\InputAirside\ALTObjectDisplayProp.h"

#include "..\InputAirside\AirRoute.h"
#include "..\InputAirside\AirWayPoint.h"

#include "..\InputAirside\Runway.h"
#include ".\Airside3D.h"
#include ".\WayPoint3D.h"
#include "3dview.h"
#include "ChildFrm.h"

#include <Common/ShapeGenerator.h>
#include "Airport3D.h"
#include "Airspace3D.h"

//#include <Common/ARCConsole.h>

CAirRoute3D::CAirRoute3D( int id):ALTObject3D(id)
{
	m_pObj = new CAirRoute(id);
	//m_pDisplayProp = new AirRouteDisplayProp;
	
}
CAirRoute3D::~CAirRoute3D(void)
{
}

void CAirRoute3D::DrawOGL( C3DView * m_pView )
{
	
	if(IsSelected()){
		glLineWidth(3.0f);
	}
	if(IsNeedSync()){
		DoSync();
	}

	// bOn = true;
	if(GetDisplayProp()->m_dpShape.bOn){
		ARCColor4 cColor = GetDisplayProp()->m_dpShape.cColor;
		cColor[ALPHA]= 255/4;
		glColor4ubv(cColor);
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		RenderShape(m_pView);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);	
// render lines
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.0f,-1.0f);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		RenderShape(m_pView);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);		
		glDisable(GL_POLYGON_OFFSET_LINE);

		glEnable(GL_LIGHTING);
			
	}
	
	if(IsSelected()){
		glLineWidth(1.0f);
	}
}

void CAirRoute3D::DrawSelect( C3DView * pView )
{
	RenderShape(pView);
}

void CAirRoute3D::RenderShape( C3DView * pview )
{
	
	ARCPath3 m_routePath;
	std::vector<ARCPath3>m_detachPath;
	std::vector<double > m_vlowLimits;
	std::vector<double > m_vHighLimits;
	std::vector<BOOL>  m_bIdentify;
    //BOOL bIsHeading = FALSE;
	m_routePath.clear();
	
	GetAirRoute()->CalWaypointExtentPoint();

	if( m_Runway.get() )
	{
		Runway* pRw = (Runway*)m_Runway.get()->GetObject();	
		
		CAirRoute::RouteType rtType = GetAirRoute()->getRouteType() ;
		if( rtType == CAirRoute::SID || rtType == CAirRoute::MIS_APPROACH )
		{
			if(m_iRunwayside)
				m_routePath.push_back(  pRw->GetPath().getPoint(0) );
			else
				m_routePath.push_back(	pRw->GetPath().getPoint(1) );
			m_vlowLimits.push_back(0);
			m_vHighLimits.push_back(0);
			m_bIdentify.push_back(TRUE);
		}
	}

	std::vector<ARWaypoint *> m_tempARW = GetAirRoute()->getARWaypoint();
	CAirport3D * pAirport3D = pview->GetParentFrame()->GetAirside3D()->GetActiveAirport();
	double dAirpotAlt = pAirport3D->m_altAirport.getElevation();
	for(int i=0 ;i< (int)m_vWayPointList.size();++i)
	{
		AirWayPoint * waypt = NULL;
		ARWaypoint *pARWayPoint = NULL;	

		waypt = (AirWayPoint*)m_vWayPointList.at(i)->GetObject();
		pARWayPoint = m_tempARW[i];
	
		if(!waypt)continue;
		if(pARWayPoint->getDepartType() != ARWaypoint::Heading)
		{
			m_routePath.push_back( waypt->GetLocation(pAirport3D->m_altAirport) );	
			if ((pARWayPoint->getMinHeight() +1.0) < ARCMath::EPSILON)
				m_vlowLimits.push_back(waypt->GetLowLimit() - dAirpotAlt);
			else
				m_vlowLimits.push_back( pARWayPoint->getMinHeight() - dAirpotAlt);

			if ((pARWayPoint->getMaxHeight() +1.0) < ARCMath::EPSILON)
				m_vHighLimits.push_back(waypt->GetHighLimit() - dAirpotAlt);
			else
				m_vHighLimits.push_back(pARWayPoint->getMaxHeight()- dAirpotAlt);	
			m_bIdentify.push_back(TRUE);
		}
		else
		{
			m_routePath.push_back( waypt->GetLocation(pAirport3D->m_altAirport) );
			if ((pARWayPoint->getMinHeight() +1.0) < ARCMath::EPSILON)
				m_vlowLimits.push_back(waypt->GetLowLimit() - dAirpotAlt);
			else
				m_vlowLimits.push_back( pARWayPoint->getMinHeight() - dAirpotAlt);

			if ((pARWayPoint->getMaxHeight() +1.0) < ARCMath::EPSILON)
				m_vHighLimits.push_back(waypt->GetHighLimit() - dAirpotAlt);
			else
				m_vHighLimits.push_back(pARWayPoint->getMaxHeight()- dAirpotAlt);	
			m_bIdentify.push_back(TRUE);

			m_routePath.push_back(pARWayPoint->getExtentPoint());
			if ((pARWayPoint->getMinHeight() +1.0) < ARCMath::EPSILON)
				m_vlowLimits.push_back(waypt->GetLowLimit() - dAirpotAlt);
			else
				m_vlowLimits.push_back( pARWayPoint->getMinHeight() - dAirpotAlt);

			if ((pARWayPoint->getMaxHeight() +1.0) < ARCMath::EPSILON)
				m_vHighLimits.push_back(waypt->GetHighLimit() - dAirpotAlt);
			else
				m_vHighLimits.push_back(pARWayPoint->getMaxHeight()- dAirpotAlt);	
			m_bIdentify.push_back(FALSE);

			//bIsHeading = TRUE;
		}

			
	}



	// get runway end
	if( m_Runway.get() )
	{
		Runway* pRw = (Runway*)m_Runway.get()->GetObject();		
		if(GetAirRoute()->getRouteType() == CAirRoute::STAR)
		{
			if(m_iRunwayside)
				m_routePath.push_back( pRw->GetLandThreshold2Point() );
			else
				m_routePath.push_back( pRw->GetLandThreshold1Point() );
			m_vlowLimits.push_back(0);
			m_vHighLimits.push_back(0);
			m_bIdentify.push_back(TRUE);
		}        
	}
	
	int tempCount = 1;
	int routeCount = m_routePath.size();
	if(routeCount < 1) 
		return;

	int indexCount=m_bIdentify.size();
	for(int index=0; index<indexCount; index++)
	{
		if(m_bIdentify[index]==FALSE)
			tempCount++;
	}

	std::vector<ShapeGenerator::WidthPipePath>widPath;
	//ShapeGenerator::GenWidthPipePath(m_routePath,3200,widPath);

    m_detachPath.resize(tempCount);
	widPath.resize(tempCount);
    int routeAdd=0, borderAdd=0;
	int limitAdd=0;
	for(int i=0; i<tempCount; i++)
	{
		do 
		{
			m_detachPath[i].push_back(m_routePath[routeAdd]);
			if( (routeAdd+1) == routeCount )
				break;
			routeAdd++;
			
		}while(m_bIdentify[routeAdd-1] !=FALSE && routeAdd<routeCount );

		ShapeGenerator::GenWidthPipePath(m_detachPath[i],3200,widPath[i]);

		int nPtCnt = widPath[i].leftpath.size();

		for(int index=0;index<nPtCnt-1;index++)
		{
			glBegin(GL_QUAD_STRIP);

			glVertex3d(widPath[i].leftpath[index][VX],widPath[i].leftpath[index][VY], m_vlowLimits[limitAdd]);	
			glVertex3d(widPath[i].leftpath[index+1][VX],widPath[i].leftpath[index+1][VY], m_vlowLimits[limitAdd+1]);
			glVertex3d(widPath[i].rightpath[index][VX],widPath[i].rightpath[index][VY],m_vlowLimits[limitAdd]);	
			glVertex3d(widPath[i].rightpath[index+1][VX],widPath[i].rightpath[index+1][VY],m_vlowLimits[limitAdd+1]);
			glVertex3d(widPath[i].rightpath[index][VX],widPath[i].rightpath[index][VY],m_vHighLimits[limitAdd]);	
			glVertex3d(widPath[i].rightpath[index+1][VX],widPath[i].rightpath[index+1][VY],m_vHighLimits[limitAdd+1]);
			glVertex3d(widPath[i].leftpath[index][VX],widPath[i].leftpath[index][VY],m_vHighLimits[limitAdd]);
			glVertex3d(widPath[i].leftpath[index+1][VX],widPath[i].leftpath[index+1][VY],m_vHighLimits[limitAdd+1]);
			glVertex3d(widPath[i].leftpath[index][VX],widPath[i].leftpath[index][VY], m_vlowLimits[limitAdd]);
			glVertex3d(widPath[i].leftpath[index+1][VX],widPath[i].leftpath[index+1][VY], m_vlowLimits[limitAdd+1]);

			glEnd();
			limitAdd++;
		}
		if(limitAdd<static_cast<int>(m_vlowLimits.size())-1)
		{
			limitAdd++;
		}
	}
	//ShapeGenerator::WidthPipePath widPath;
	//ShapeGenerator::GenWidthPipePath(m_routePath,3200,widPath);
	//int nPtCnt = widPath.leftpath.size();
	//for(int i=0; i<nPtCnt -1; i++){

	//	if( m_bIdentify[i] == FALSE )
	//		continue;

	//	glBegin(GL_QUAD_STRIP);
	//	
	//	glVertex3d(widPath.leftpath[i][VX],widPath.leftpath[i][VY], m_vlowLimits[i]);	
	//	glVertex3d(widPath.leftpath[i+1][VX],widPath.leftpath[i+1][VY], m_vlowLimits[i+1]);

	//	glVertex3d(widPath.rightpath[i][VX],widPath.rightpath[i][VY],m_vlowLimits[i]);	
	//	glVertex3d(widPath.rightpath[i+1][VX],widPath.rightpath[i+1][VY],m_vlowLimits[i+1]);

	//	glVertex3d(widPath.rightpath[i][VX],widPath.rightpath[i][VY],m_vHighLimits[i]);	
	//	glVertex3d(widPath.rightpath[i+1][VX],widPath.rightpath[i+1][VY],m_vHighLimits[i+1]);

	//	glVertex3d(widPath.leftpath[i][VX],widPath.leftpath[i][VY],m_vHighLimits[i]);
	//	glVertex3d(widPath.leftpath[i+1][VX],widPath.leftpath[i+1][VY],m_vHighLimits[i+1]);

	//	glVertex3d(widPath.leftpath[i][VX],widPath.leftpath[i][VY], m_vlowLimits[i]);	
	//	glVertex3d(widPath.leftpath[i+1][VX],widPath.leftpath[i+1][VY], m_vlowLimits[i+1]);

	//	glEnd();
		
	/*}*/
}
	



ARCPoint3 CAirRoute3D::GetLocation( void ) const
{
	int wayptCnt = m_vWayPointList.size();

	int nMid = (wayptCnt-1) /2;
	int nMidNext = min(nMid +1,wayptCnt-1);

	if(nMid < wayptCnt && nMidNext < wayptCnt )
	{	
		CWayPoint3D * pWayPoint3D1 = (CWayPoint3D*)m_vWayPointList.at(nMid).get();
		CWayPoint3D * pWayPoint3D2 = (CWayPoint3D*)m_vWayPointList.at(nMidNext).get();
		ARCPoint3 loc = (pWayPoint3D1->GetLocation() + pWayPoint3D2->GetLocation())*0.5;
		AirWayPoint* pWP1 =  pWayPoint3D1->GetAirWayPoint();
		AirWayPoint* pWP2 =  pWayPoint3D2->GetAirWayPoint();
		loc.z = (pWP1->GetHighLimit() + pWP2->GetHighLimit())*0.5;
		return loc;
	}
	
	return ARCPoint3(0,0,0);
}


CAirRoute * CAirRoute3D::GetAirRoute() const
{
	return (CAirRoute*)m_pObj.get();
}

//AirRouteDisplayProp  * CAirRoute3D::GetDisplayProp() const
//{
//	return (AirRouteDisplayProp*)m_pDisplayProp.get();
//}

void CAirRoute3D::Update(CAirside3D* pAirside)
{
	try
	{		
		GetAirRoute()->ReadData(m_nID);
		GetDisplayProp()->ReadData(m_nID);
		m_pAirside = pAirside;
	}
	catch (CADOException& e)
	{
		CString strErr = e.ErrorMessage(); 
	}
	SyncDateType::Update();
}

void CAirRoute3D::DoSync()
{
	m_vWayPointList.clear();
	
	for(int i=0 ;i< (int)GetAirRoute()->getARWaypoint().size();++i){
		int wayptID = GetAirRoute()->getARWaypoint().at(i)->getWaypoint().getID();
		ALTObject3D * pObj3D =  m_pAirside->GetAirspace()->GetObject3D(wayptID);
		if(pObj3D) m_vWayPointList.push_back(pObj3D);
	}
	// get runway 
	if(GetAirRoute()->getRouteType() == CAirRoute::STAR || GetAirRoute()->getRouteType() == CAirRoute::SID || GetAirRoute()->getRouteType() == CAirRoute::MIS_APPROACH)
	{
		int rwID = GetAirRoute()->getLogicRunwayByIdx(0).first;
		//int AirportID = GetAirRoute()->getRunwayObj().getAptID();
        m_iRunwayside = GetAirRoute()->getLogicRunwayByIdx(0).second;

		ALTObject3D * pRw3d = m_pAirside->GetActiveAirport()->GetObject3D(rwID);

		if(pRw3d)
		{
			ASSERT(pRw3d->GetObjectType() == ALT_RUNWAY);
			if(pRw3d->GetObjectType() == ALT_RUNWAY)
				m_Runway = pRw3d;
		}
	}
	else
		m_Runway = NULL;


	SyncDateType::DoSync();

}

CString CAirRoute3D::GetDisplayName()const
{
	return GetAirRoute()->getName();
}
