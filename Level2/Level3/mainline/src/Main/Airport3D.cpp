#include "StdAfx.h"
#include ".\airport3d.h"
#include "./Runway3D.h"
#include "./../InputAirside/Runway.h"
#include "./Taxiway3D.h"
#include "./Gate3D.h"
#include "./DeicePad3D.h"
#include ".\3DView.h"
#include ".\..\InputAirside\Taxiway.h"
#include ".\..\InputAirside\stand.h"
#include ".\Gate3D.h"
#include ".\..\InputAirside\DeicePad.h"
#include ".\DeicePad3d.h"
#include "./Stretch3D.h"
#include "TrafficLight3D.h"
#include "TollGate3D.h"
#include "StopSign3D.h"
#include "YieldSign3D.h"
#include ".\AirsideGround.h"
#include "./RoadIntersection3D.h"
#include "./TermPlanDoc.h"
#include "VehiclePoolParking3D.h"
#include <CommonData/3DTextManager.h>
#include "ChildFrm.h"
#include "SelectableSettings.h"
#include "FilletTaxiway3D.h"
#include "..\Inputs\AirsideInput.h"
#include "..\Inputs\AirportInfo.h"
#include "./Heliport3D.h"
#include "./CircleStretch3D.h"
#include "./StartPosition3D.h"
#include "./MeetingPoint3D.h"
#include "AirsidePaxBusParkSpot3D.h"

CAirport3D::CAirport3D(int _Id) : m_nID(_Id) , m_vLevelList(_Id)
{
	Init();
}

CAirport3D::~CAirport3D(void)
{
}

void CAirport3D::Init()
{
	std::vector<int> objectIDs;
	// get runway Object list
	ALTAirport::GetRunwaysIDs(m_nID,objectIDs);

	for(size_t i=0;i<objectIDs.size();++i){
		CRunway3D * pRunway3D = new CRunway3D(objectIDs[i]);
		m_vRunways.push_back(pRunway3D);
		pRunway3D->Update();
	}

	//get heliport
	objectIDs.clear();
	//get heliport object list
	ALTAirport::GetHeliportsIDs(m_nID, objectIDs);

	for (size_t i=0; i<objectIDs.size();i++)
	{
		CHeliport3D* pHeliport3D = new CHeliport3D(objectIDs[i]);
		m_vHeliport.push_back(pHeliport3D);
		pHeliport3D->Update();
	}
	
	//get taxiways
	objectIDs.clear();
	ALTAirport::GetTaxiwaysIDs(m_nID,objectIDs);

	for(size_t i=0;i<objectIDs.size();++i){

		CTaxiway3D  * pTaxi3D = new CTaxiway3D(objectIDs[i]);
		m_vTaxways.push_back(pTaxi3D);
		pTaxi3D->Update();
	}

	//get stands
	objectIDs.clear();
	ALTAirport::GetStandsIDs(m_nID,objectIDs);

	for(size_t i=0;i<objectIDs.size();++i){
		CStand3D * pGate3D = new CStand3D(objectIDs[i]);
		m_vStands.push_back(pGate3D);
		pGate3D->Update();
	}

	// get deice pads
	objectIDs.clear();
	ALTAirport::GetDeicePadsIDs(m_nID,objectIDs);

	for(size_t i=0;i<objectIDs.size();++i){
		CDeicePad3D * pDeicePad = new CDeicePad3D(objectIDs[i]);
		m_vDeicePad.push_back(pDeicePad);
		pDeicePad->Update();
	}
	
	// get Stretch
	objectIDs.clear();
	ALTAirport::GetStretchsIDs(m_nID,objectIDs);
	for(size_t i=0;i<objectIDs.size();++i)
	{
		CStretch3D * pStretch = new CStretch3D(objectIDs[i]);
		m_vStretchs.push_back(pStretch);
		pStretch->Update();
	}
	//get road intersection
	objectIDs.clear();
	ALTAirport::GetRoadIntersectionsIDs(m_nID,objectIDs);
	for(size_t i=0;i<objectIDs.size();++i)
	{
		CRoadIntersection3D * pRoadIntersection = new CRoadIntersection3D(objectIDs[i]);
		m_vRoadIntersections.push_back(pRoadIntersection);
		pRoadIntersection->Update();
	}

	//get vehicle pool parking 
	objectIDs.clear();
	ALTAirport::GetVehiclePoolParkingIDs(m_nID,objectIDs);
	for(size_t i=0;i<objectIDs.size();++i)
	{
		CVehiclePoolParking3D * pVehiclePoolParking = new CVehiclePoolParking3D(objectIDs[i]);
		m_vVehiclePoolParking.push_back(pVehiclePoolParking);
		pVehiclePoolParking->Update();
	}

	//get Traffic Light
	objectIDs.clear();
	ALTAirport::GetTrafficLightsIDs(m_nID,objectIDs);
	for(size_t i=0;i<objectIDs.size();++i)
	{
		CTrafficLight3D * pTrafficLight = new CTrafficLight3D(objectIDs[i]);
		m_vrTrafficLights.push_back(pTrafficLight);
		pTrafficLight->Update();
	}

	//get TollGate
	objectIDs.clear();
	ALTAirport::GetTollGatesIDs(m_nID,objectIDs);
	for(size_t i=0;i<objectIDs.size();++i)
	{
		CTollGate3D * pTrafficLight = new CTollGate3D(objectIDs[i]);
		m_vrTollGate.push_back(pTrafficLight);
		pTrafficLight->Update();
	}

	//get StopSign
	objectIDs.clear();
	ALTAirport::GetStopSignsIDs(m_nID,objectIDs);
	for(size_t i=0;i<objectIDs.size();++i)
	{
		CStopSign3D * pStopSign = new CStopSign3D(objectIDs[i]);
		m_vrStopSign.push_back(pStopSign);
		pStopSign->Update();
	}

	//get YieldSign
	objectIDs.clear();
	ALTAirport::GetYieldSignsIDs(m_nID,objectIDs);
	for(size_t i=0;i<objectIDs.size();++i)
	{
		CYieldSign3D * pYieldSign = new CYieldSign3D(objectIDs[i]);
		m_vrYieldSign.push_back(pYieldSign);
		pYieldSign->Update();
	}

	//get CircleStretch
	objectIDs.clear();
	ALTAirport::GetCircleStretchsIDs(m_nID,objectIDs);
    for (size_t i=0; i<objectIDs.size(); ++i)
    {
		CCircleStretch3D * pCircleStretch = new CCircleStretch3D(objectIDs[i]);
		m_vCircleStretchs.push_back(pCircleStretch);
		pCircleStretch->Update();
    }

	//get StartPosition
	objectIDs.clear();
	ALTAirport::GetStartPositionIDs(m_nID,objectIDs);

	for(size_t i=0;i<objectIDs.size();++i){

		CStartPosition3D  * pStP3D = new CStartPosition3D(objectIDs[i]);
		m_vStartPositions.push_back(pStP3D);
		pStP3D->Update();
	}

	//get MeetingPoints
	objectIDs.clear();
	ALTAirport::GetMeetingPointIDs(m_nID,objectIDs);
	for(size_t i=0;i<objectIDs.size();++i)
	{
		CMeetingPoint3D  * pMeetingPoint3D = new CMeetingPoint3D(objectIDs[i]);
		m_vMeetingPoints.push_back(pMeetingPoint3D);
		pMeetingPoint3D->Update();
	}

	//get pax bus parking spot
	objectIDs.clear();
	ALTAirport::GetPaxBusParkPosIDs(m_nID,objectIDs);
	for(size_t i=0;i<objectIDs.size();++i)
	{
		CAirsidePaxBusParkSpot3D  * pPos = new CAirsidePaxBusParkSpot3D(objectIDs[i]);
		m_vPaxBusParkSpots.push_back(pPos);
		pPos->Update();
	}

	//get bag cart parking spot
	objectIDs.clear();
	ALTAirport::GetBagCartParkPosIDs(m_nID, objectIDs);
	for(size_t i=0;i<objectIDs.size();++i)
	{
		CAirsideBagCartParkPos3D  * pPos = new CAirsideBagCartParkPos3D(objectIDs[i]);
		m_vBagCartParkSpots.push_back(pPos);
		pPos->Update();
	}


	m_altAirport.ReadAirport(m_nID);
	ReadGroundInfo();


	ALTObject3DList vObject3DList;
	GetObject3DList(vObject3DList);

	m_vAirportNodes.Init(m_nID, vObject3DList.GetObjectList() );
	m_vFilletTaxiways.Init(m_nID, m_vAirportNodes);
	m_vIntersectedStretch.InitIntersectedStrechInAirport(m_nID, m_vAirportNodes);

	//
	for(int i=0;i<(int)m_vTaxways.size();i++)
	{
		CTaxiway3D * pTaxiway3D = (CTaxiway3D*)m_vTaxways.at(i).get();
		pTaxiway3D->UpdateIntersectionNodes(m_vAirportNodes);
	}
	UpdateObjectsRelations();	
}

void CAirport3D::DrawOGL( C3DView * pView,bool bWithlevles )
{	
	
	//draw floors
	glDisable(GL_LIGHTING);

	if(bWithlevles)
	{
		
		glDisable(GL_LIGHTING);
		for(int i=0;i< m_vLevelList.GetCount();i++)
		{
			m_vLevelList.GetLevel(i).DrawOGL(pView,0);
		}
	}
	glEnable(GL_LIGHTING);	


	RenderToDepth(pView);


///////////////////////////////
	glEnable(GL_TEXTURE_2D);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(-4.0,-4.0);
	glDepthMask(GL_FALSE);	
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
//////////////////////////////
	//render runways
	{

		for(int i=0;i<(int)m_vRunways.size();++i){
			CRunway3D* pRunway = (CRunway3D*)m_vRunways.at(i).get();
			pRunway->RenderBaseWithSideLine(pView);
		}	

		for(int i=0; i<(int)m_vHeliport.size();++i)
		{
			CHeliport3D* pHeliport = (CHeliport3D*)m_vHeliport.at(i).get();
			pHeliport->RenderBase(pView);
		}
		
		{	
			
			//render taxiway surface first
			for(size_t i=0;i<m_vTaxways.size();++i){
				CTaxiway3D * pTaxi = (CTaxiway3D*)m_vTaxways[i].get();
				pTaxi->RenderSurface(pView);
			}
			m_vFilletTaxiways.RenderSurface(pView); 
			//render stretch 
		}

		
		for(int i=0;i<(int)m_vRunways.size();++i )
		{
			CRunway3D * pRunway = (CRunway3D*)m_vRunways.at(i).get();
			pRunway->RenderBase(pView);
		}


		//stretch
        if (pView->GetDocument()->m_bIsOpenStencil)
        {
			glEnable(GL_STENCIL_TEST);
			glStencilFunc(GL_EQUAL,0x1,0x1);
			glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
			GLfloat mat_specular[]={1.0,1.0,1.0,1.0};
			GLfloat mat_shininess[] = {50.0};
			glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
			glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
			glPushMatrix();

			for(int i=0;i <(int)m_vStretchs.size();++i)
			{
				CStretch3D * pStretch = (CStretch3D*)m_vStretchs.at(i).get();
				pStretch->RenderBaseWithSideLine(pView);
			}


			for (int i=0;i<(int)m_vStretchs.size();i++)
			{
				CStretch3D * pStretch = (CStretch3D*)m_vStretchs.at(i).get();
				pStretch->RenderBase(pView);
			}
			m_vIntersectedStretch.DrawOGL(pView);

			for(int i=0;i <(int)m_vCircleStretchs.size();++i)
			{
				CCircleStretch3D * pCirStretch = (CCircleStretch3D*)m_vCircleStretchs.at(i).get();
				pCirStretch->RenderBaseWithSideLine(pView);
			}


			for (int i=0;i<(int)m_vCircleStretchs.size();i++)
			{
				CCircleStretch3D * pCirStretch = (CCircleStretch3D*)m_vCircleStretchs.at(i).get();
				pCirStretch->RenderBase(pView);
			}

			glPopMatrix();	

			glStencilFunc(GL_NOTEQUAL,0x1,0x1);
			glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
			glPushMatrix();
			for(int i=0;i <(int)m_vStretchs.size();++i)
			{
				CStretch3D * pStretch = (CStretch3D*)m_vStretchs.at(i).get();
				pStretch->RenderBaseWithSideLine(pView);
			}


			for (int i=0;i<(int)m_vStretchs.size();i++)
			{
				CStretch3D * pStretch = (CStretch3D*)m_vStretchs.at(i).get();
				pStretch->RenderBase(pView);
			}

			for(int i=0;i <(int)m_vCircleStretchs.size();++i)
			{
				CCircleStretch3D * pCirStretch = (CCircleStretch3D*)m_vCircleStretchs.at(i).get();
				pCirStretch->RenderBaseWithSideLine(pView);
			}


			for (int i=0;i<(int)m_vCircleStretchs.size();i++)
			{
				CCircleStretch3D * pCirStretch = (CCircleStretch3D*)m_vCircleStretchs.at(i).get();
				pCirStretch->RenderBase(pView);
			}
			m_vIntersectedStretch.DrawOGL(pView);

			glPopMatrix();	
			glDisable(GL_STENCIL_TEST);
        }
		else
		{
			GLfloat mat_specular[]={1.0,1.0,1.0,1.0};
			GLfloat mat_shininess[] = {50.0};
			glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
			glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
			glPushMatrix();

			for(int i=0;i <(int)m_vStretchs.size();++i)
			{
				CStretch3D * pStretch = (CStretch3D*)m_vStretchs.at(i).get();
				pStretch->RenderBaseWithSideLine(pView);
			}


			for (int i=0;i<(int)m_vStretchs.size();i++)
			{
				CStretch3D * pStretch = (CStretch3D*)m_vStretchs.at(i).get();
				pStretch->RenderBase(pView);
			}

			for(int i=0;i <(int)m_vCircleStretchs.size();++i)
			{
				CCircleStretch3D * pCirStretch = (CCircleStretch3D*)m_vCircleStretchs.at(i).get();
				pCirStretch->RenderBaseWithSideLine(pView);
			}


			for (int i=0;i<(int)m_vCircleStretchs.size();i++)
			{
				CCircleStretch3D * pCirStretch = (CCircleStretch3D*)m_vCircleStretchs.at(i).get();
				pCirStretch->RenderBase(pView);
			}

			m_vIntersectedStretch.DrawOGL(pView);

			glPopMatrix();	
		}

		//glPushMatrix();
		//GLfloat mat_specular[]={1.0,1.0,1.0,1.0};
		//GLfloat mat_shininess[] = {50.0};
		//glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
		//glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);

        //glPopMatrix();

		for (size_t i = 0;i < m_vVehiclePoolParking.size();i++) {
			CVehiclePoolParking3D * pVehiclePoolParking = (CVehiclePoolParking3D *)m_vVehiclePoolParking[i].get();
			pVehiclePoolParking->DrawOGL(pView);
		}

		for(int i=0;i<(int)m_vRunways.size();++i )
		{
			CRunway3D * pRunway = (CRunway3D*)m_vRunways.at(i).get();
			pRunway->RenderMarkings(pView);
		}


		for(int i =0;i <(int) m_vRoadIntersections.size(); ++i )
		{
			CRoadIntersection3D  *pRoadIntersect = (CRoadIntersection3D*)m_vRoadIntersections.at(i).get();
			pRoadIntersect->DrawOGL(pView);
		}

		for(int i=0;i<(int)m_vStretchs.size();++i )
		{
			CStretch3D * pStretch = (CStretch3D*)m_vStretchs.at(i).get();
			pStretch->RenderMarkings(pView);
		}

		for(int i=0;i<(int)m_vCircleStretchs.size();++i )
		{
			CCircleStretch3D * pCircleStretch = (CCircleStretch3D*)m_vCircleStretchs.at(i).get();
			pCircleStretch->RenderMarkings(pView);
		}

	}
	
	glDisable(GL_TEXTURE_2D);	

	for(int i=0;i<(int)m_vRunways.size();++i )
	{
		CRunway3D * pRunway = (CRunway3D*)m_vRunways.at(i).get();
		pRunway->RenderEditPoint(pView);
	}


	//for(size_t i=0;i<m_vStretchs.size();++i){
	//	CStretch3D * pStretch = ( CStretch3D *) m_vStretchs[i].get();
	//	pStretch->RenderEditPoint(pView);
	//}

	//for(int i=0;i<(int)m_vStretchs.size();++i )
	//{
	//	CStretch3D * pStretch = (CStretch3D*)m_vStretchs.at(i).get();
	//	pStretch->RenderMarkings(pView);
	//}

	//for(int i=0;i<(int)m_vCircleStretchs.size();++i )
	//{
	//	CCircleStretch3D * pCircleStretch = (CCircleStretch3D*)m_vCircleStretchs.at(i).get();
	//	pCircleStretch->RenderMarkings(pView);
	//}

	for(size_t i=0; i<m_vHeliport.size(); ++i)
	{
		CHeliport3D * pHeliport = (CHeliport3D *)m_vHeliport[i].get();
		pHeliport->RenderEditPoint(pView);
	}

	for (size_t i = 0;i < m_vVehiclePoolParking.size();i++) {
		CVehiclePoolParking3D * pVehiclePoolParking = (CVehiclePoolParking3D *)m_vVehiclePoolParking[i].get();
		pVehiclePoolParking->RenderEditPoint(pView);
	}

	for(size_t i=0;i<m_vrTrafficLights.size();++i){
		CTrafficLight3D * pTrafficLight  = (CTrafficLight3D * )m_vrTrafficLights[i].get();
		pTrafficLight->RenderEditPoint(pView);
	}	

	for(size_t i=0;i<m_vrTollGate.size();++i){
		CTollGate3D * pTollGate  = (CTollGate3D * )m_vrTollGate[i].get();
		pTollGate->RenderEditPoint(pView);
	}
	
	for(size_t i=0;i<m_vrStopSign.size();++i){
		CStopSign3D * pStopSign  = (CStopSign3D * )m_vrStopSign[i].get();
		pStopSign->RenderEditPoint(pView);
	}
	
	for(size_t i=0;i<m_vrYieldSign.size();++i){
		CYieldSign3D * pYieldSign  = (CYieldSign3D * )m_vrYieldSign[i].get();
		pYieldSign->RenderEditPoint(pView);
	}

	for(size_t i=0;i<m_vTaxways.size();++i){
		CTaxiway3D * pTaxi  = (CTaxiway3D * )m_vTaxways[i].get();
		pTaxi->RenderMarkings(pView);
	}
	
	m_vFilletTaxiways.RenderLine(pView,false);
	
	for(size_t i= 0 ;i<m_vDeicePad.size();i++){
		CDeicePad3D * pDeice = ( CDeicePad3D *) m_vDeicePad[i].get();
		pDeice->RenderSurface(pView);
	}	

	if(pView->GetDocument()->m_bShowAirsideNodes)
	{
		RenderNodes(pView);
	}

	glDepthMask(GL_TRUE);

	for(size_t i=0;i<m_vStands.size();++i){
		m_vStands[i]->DrawOGL(pView);
	}

	for(size_t i=0;i<m_vDeicePad.size();++i){
		CDeicePad3D * pDeice = ( CDeicePad3D *) m_vDeicePad[i].get();
		pDeice->DrawOGL(pView);
	}


	for(size_t i=0;i<m_vrTrafficLights.size();++i){
		CTrafficLight3D * pTrafficLight = ( CTrafficLight3D *) m_vrTrafficLights[i].get();
		pTrafficLight->DrawOGL(pView);
	}

	for(size_t i=0;i<m_vrTollGate.size();++i){
		CTollGate3D * pTollGate = ( CTollGate3D *) m_vrTollGate[i].get();
		pTollGate->DrawOGL(pView);
	}

	for(size_t i=0;i<m_vrStopSign.size();++i){
		CStopSign3D * pStopSign = ( CStopSign3D *) m_vrStopSign[i].get();
		pStopSign->DrawOGL(pView);
	}

	for(size_t i=0;i<m_vrYieldSign.size();++i){
		CYieldSign3D * pYieldSign = ( CYieldSign3D *) m_vrYieldSign[i].get();
		pYieldSign->DrawOGL(pView);
	}

	// draw StartPosition(s)
	for (int i=0; i<(int)m_vStartPositions.size(); ++i)
	{
		CStartPosition3D * pStartPosition3D = (CStartPosition3D*)m_vStartPositions.at(i).get();
		pStartPosition3D->DrawOGL(pView);
	}

	// draw MeetingPoint(s)
	for (int i=0; i<(int)m_vMeetingPoints.size(); ++i)
	{
		CMeetingPoint3D * pMeetingPoint3D = (CMeetingPoint3D*)m_vMeetingPoints.at(i).get();
		pMeetingPoint3D->DrawOGL(pView);
	}

	//draw paxbus park pos 
	for (int i=0; i<(int)m_vPaxBusParkSpots.size(); ++i)
	{
		CAirsidePaxBusParkSpot3D * pSpot3D = (CAirsidePaxBusParkSpot3D*)m_vPaxBusParkSpots.at(i).get();
		pSpot3D->DrawOGL(pView);
	}
	//draw bag cart  park pos
	for (int i=0; i<(int)m_vBagCartParkSpots.size(); ++i)
	{
		CAirsideBagCartParkPos3D * pSpot3D = (CAirsideBagCartParkPos3D*)m_vBagCartParkSpots.at(i).get();
		pSpot3D->DrawOGL(pView);
	}

	if(pView->GetDocument()->GetCurrentMode() == EnvMode_AirSide && pView->GetDocument()->m_bHideARP == FALSE)
	{     
		glPushMatrix();
		ARCVector3 m_location = m_altAirport.getRefPoint();
		m_location *= SCALE_FACTOR;
		glTranslated(m_location[VX],m_location[VY],m_location[VZ]);
		{	//draw reference point			
			GLUquadric* pQuadObj = gluNewQuadric();		
			glPushMatrix();
			glTranslated(0.0, 0.0, -1500.0);
			glColor3ub(255, 0, 0);
			gluCylinder(pQuadObj, 50.0, 50.0, 3000.0, 16, 5);
			glPopMatrix();
			glColor3f(0.2f,1.0f,0.3f);
			gluDisk(pQuadObj, 50.0, 750.0, 16, 1);		
			gluDeleteQuadric(pQuadObj);
		}		
		glPopMatrix();	//
	}
	glDisable(GL_POLYGON_OFFSET_FILL);		
	
}

ALTObject3D* CAirport3D::GetObject3D( int id ) 
{
	// search runway list
	for(size_t i=0;i<m_vRunways.size();++i){
		if( m_vRunways[i]->GetID() == id ) return m_vRunways[i].get();
 	}

	for (size_t i=0; i<m_vHeliport.size(); ++i)
	{
		if(m_vHeliport[i]->GetID() == id) return m_vHeliport[i].get();
	}

	for(size_t i=0;i<m_vTaxways.size();++i){
		if( m_vTaxways[i]->GetID() == id ) return m_vTaxways[i].get();
	}
	for(size_t i=0;i<m_vStands.size();++i){
		if( m_vStands[i]->GetID() == id ) return m_vStands[i].get();
	}
	for(size_t i=0;i<m_vDeicePad.size();++i){
		if( m_vDeicePad[i]->GetID() == id ) return m_vDeicePad[i].get();
	}

	for(int i=0;i< (int)m_vStretchs.size();++i)
	{
		if( m_vStretchs[i]->GetID() == id ) 
			return m_vStretchs[i].get();
	}

	for(int i=0;i< (int)m_vCircleStretchs.size();++i)
	{
		if( m_vCircleStretchs[i]->GetID() == id ) 
			return m_vCircleStretchs[i].get();
	}
	for(int i=0;i< (int)m_vRoadIntersections.size();++i)
	{
		if( m_vRoadIntersections[i]->GetID() == id ) 
			return m_vRoadIntersections[i].get();
	}
	for (int i = 0;i < (int)m_vVehiclePoolParking.size();++i)
	{
		if(m_vVehiclePoolParking[i]->GetID() == id)
			return m_vVehiclePoolParking[i].get();
	}
	for (int i = 0;i < (int)m_vrTrafficLights.size();++i)
	{
		if(m_vrTrafficLights[i]->GetID() == id)
			return m_vrTrafficLights[i].get();
	}
	for (int i = 0;i < (int)m_vrTollGate.size();++i)
	{
		if(m_vrTollGate[i]->GetID() == id)
			return m_vrTollGate[i].get();
	}
	for (int i = 0;i < (int)m_vrStopSign.size();++i)
	{
		if(m_vrStopSign[i]->GetID() == id)
			return m_vrStopSign[i].get();
	}
	for (int i = 0;i < (int)m_vrYieldSign.size();++i)
	{
		if(m_vrYieldSign[i]->GetID() == id)
			return m_vrYieldSign[i].get();
	}
	for (int i = 0;i < (int)m_vStartPositions.size();++i)
	{
		if(m_vStartPositions[i]->GetID() == id)
			return m_vStartPositions[i].get();
	}
	for (int i = 0;i < (int)m_vMeetingPoints.size();++i)
	{
		if(m_vMeetingPoints[i]->GetID() == id)
			return m_vMeetingPoints[i].get();
	}

	for (int i = 0;i < (int)m_vPaxBusParkSpots.size();++i)
	{
		if(m_vPaxBusParkSpots[i]->GetID() == id)
			return m_vPaxBusParkSpots[i].get();
	}

	for (int i = 0;i < (int)m_vBagCartParkSpots.size();++i)
	{
		if(m_vBagCartParkSpots[i]->GetID() == id)
			return m_vBagCartParkSpots[i].get();
	}

	return NULL;
}

void CAirport3D::DrawSelectOGL( C3DView * pView,  SelectableSettings& selsetting)
{
	
	if( selsetting.m_ALTobjectSelectableMap[ALT_DEICEBAY] )
	{
		for(int i=m_vDeicePad.size()-1;i>=0;--i){
			glLoadName( pView->GetSelectionMap().NewSelectable(m_vDeicePad[i].get()) );
			m_vDeicePad[i]->DrawSelect(pView);
		}
	}

	if( selsetting.m_ALTobjectSelectableMap[ALT_STAND] )
	{
		for(int i=m_vStands.size()-1;i>=0;--i){
			glLoadName( pView->GetSelectionMap().NewSelectable(m_vStands[i].get()) );
			m_vStands[i]->DrawSelect(pView);
		}
	}
	
	if( selsetting.m_ALTobjectSelectableMap[ALT_RUNWAY] )
	{
		for(int i=m_vRunways.size()-1;i>=0; --i){
			glLoadName( pView->GetSelectionMap().NewSelectable(m_vRunways[i].get()) );
			m_vRunways[i]->DrawSelect(pView);
		}
	}
	
	if( selsetting.m_ALTobjectSelectableMap[ALT_HELIPORT] )
	{
		for(int i=m_vHeliport.size()-1;i>=0; --i){
			glLoadName( pView->GetSelectionMap().NewSelectable(m_vHeliport[i].get()) );
			m_vHeliport[i]->DrawSelect(pView);
		}
	}
	
	if(selsetting.m_ALTobjectSelectableMap[ALT_HELIPORT])
	{
		for (int i=0; i<(int)m_vStartPositions.size(); ++i)
		{
			glLoadName(pView->GetSelectionMap().NewSelectable(m_vStartPositions[i].get()) );
			m_vStartPositions[i]->DrawSelect(pView);
		}
	}

	if( selsetting.m_ALTobjectSelectableMap[ALT_TAXIWAY] )
	{
		for(int i=m_vTaxways.size()-1;i>=0;--i){		
			glLoadName(pView->GetSelectionMap().NewSelectable(m_vTaxways[i].get()) );
			m_vTaxways[i]->DrawSelect(pView);
		}	
	}

	m_vFilletTaxiways.DrawSelect(pView);

	// draw stretch 
	if( selsetting.m_ALTobjectSelectableMap[ALT_STRETCH] )
	{
		for(int i = m_vStretchs.size()-1; i>=0; --i)
		{
			glLoadName(pView->GetSelectionMap().NewSelectable(m_vStretchs[i].get()) );
			m_vStretchs[i]->DrawSelect(pView);
		}
	}

	if( selsetting.m_ALTobjectSelectableMap[ALT_CIRCLESTRETCH] )
	{
		for(int i = m_vCircleStretchs.size()-1; i>=0; --i)
		{
			glLoadName(pView->GetSelectionMap().NewSelectable(m_vCircleStretchs[i].get()) );
			m_vCircleStretchs[i]->DrawSelect(pView);
		}
	}
//	m_vIntersectedStretch.DrawSelect(pView);
	// draw Road Intersection
	if( selsetting.m_ALTobjectSelectableMap[ALT_INTERSECTIONS] )
	{
		for(int i = m_vRoadIntersections.size()-1; i>=0; --i)
		{
			glLoadName(pView->GetSelectionMap().NewSelectable(m_vRoadIntersections[i].get()) );
			m_vRoadIntersections[i]->DrawSelect(pView);
		}
	}

	//draw vehicle pool parking.
	if( selsetting.m_ALTobjectSelectableMap[ALT_VEHICLEPOOLPARKING] )
	{
		for (int i = m_vVehiclePoolParking.size() - 1;i >= 0; --i )
		{
			glLoadName(pView->GetSelectionMap().NewSelectable(m_vVehiclePoolParking[i].get()) );
			m_vVehiclePoolParking[i]->DrawSelect(pView);
		}
	}

	//draw Traffic Light
	if( selsetting.m_ALTobjectSelectableMap[ALT_TRAFFICLIGHT] )
	{
		for (int i = m_vrTrafficLights.size() - 1;i >= 0; --i )
		{
			glLoadName(pView->GetSelectionMap().NewSelectable(m_vrTrafficLights[i].get()) );
			m_vrTrafficLights[i]->DrawSelect(pView);
		}
	}

	//draw Toll Gate
	if( selsetting.m_ALTobjectSelectableMap[ALT_TOLLGATE] )
	{
		for (int i = m_vrTollGate.size() - 1;i >= 0; --i )
		{
			glLoadName(pView->GetSelectionMap().NewSelectable(m_vrTollGate[i].get()) );
			m_vrTollGate[i]->DrawSelect(pView);
		}
	}
	
	//draw StopSign
	if( selsetting.m_ALTobjectSelectableMap[ALT_STOPSIGN] )
	{
		for (int i = m_vrStopSign.size() - 1;i >= 0; --i )
		{
			glLoadName(pView->GetSelectionMap().NewSelectable(m_vrStopSign[i].get()) );
			m_vrStopSign[i]->DrawSelect(pView);
		}
	}

	//draw YieldSign
	if( selsetting.m_ALTobjectSelectableMap[ALT_YIELDSIGN] )
	{
		for (int i = m_vrYieldSign.size() - 1;i >= 0; --i )
		{
			glLoadName(pView->GetSelectionMap().NewSelectable(m_vrYieldSign[i].get()) );
			m_vrYieldSign[i]->DrawSelect(pView);
		}
	}

	if( selsetting.m_ALTobjectSelectableMap[ALT_APAXBUSSPOT] )
	{
		for (int i = m_vPaxBusParkSpots.size() - 1;i >= 0; --i )
		{
			glLoadName(pView->GetSelectionMap().NewSelectable(m_vPaxBusParkSpots[i].get()) );
			m_vPaxBusParkSpots[i]->DrawSelect(pView);
		}
	}

	if( selsetting.m_ALTobjectSelectableMap[ALT_ABAGCARTSPOT] )
	{
		for (int i = m_vBagCartParkSpots.size() - 1;i >= 0; --i )
		{
			glLoadName(pView->GetSelectionMap().NewSelectable(m_vBagCartParkSpots[i].get()) );
			m_vBagCartParkSpots[i]->DrawSelect(pView);
		}
	}

}

bool CAirport3D::RemoveObject( int id )
{
	//find from the Runway
	for(ALTObject3DList::iterator itr = m_vRunways.begin(); itr!=m_vRunways.end();++itr){
		if ( (*itr)->GetID() == id ) { m_vRunways.erase(itr); return true; }
	}
	for(ALTObject3DList::iterator itr = m_vHeliport.begin(); itr!=m_vHeliport.end(); ++itr)
	{
		if( (*itr)->GetID() == id )  { m_vHeliport.erase(itr);return true; }
	}
	//find from the deicepad
	for(ALTObject3DList::iterator itr = m_vDeicePad.begin(); itr!=m_vDeicePad.end();++itr){
		if ( (*itr)->GetID() == id ) { m_vDeicePad.erase(itr); return true; }
	}
	//find from the stand
	for(ALTObject3DList::iterator itr = m_vStands.begin(); itr!=m_vStands.end();++itr){
		if ( (*itr)->GetID() == id ) { m_vStands.erase(itr); return true; }
	}
	//find from the taxiway
	for(ALTObject3DList::iterator itr = m_vTaxways.begin(); itr!=m_vTaxways.end();++itr){
		if ( (*itr)->GetID() == id ) { m_vTaxways.erase(itr); return true; }
	}
	//find from the stretch
	for(ALTObject3DList::iterator itr= m_vStretchs.begin();itr!= m_vStretchs.end();++itr)
	{
		if( (*itr)->GetID() == id ){ m_vStretchs.erase(itr); return true; }
	}
	//find from the circlestretch
	for(ALTObject3DList::iterator itr= m_vCircleStretchs.begin();itr!= m_vCircleStretchs.end();++itr)
	{
		if( (*itr)->GetID() == id ){ m_vCircleStretchs.erase(itr); return true; }
	}
	//find from the road_intersection
	for(ALTObject3DList::iterator itr= m_vRoadIntersections.begin();itr!= m_vRoadIntersections.end();++itr)
	{
		if( (*itr)->GetID() == id ){ m_vRoadIntersections.erase(itr); return true; }
	}

	//find from VehiclePoolParking
	for(ALTObject3DList::iterator itr= m_vVehiclePoolParking.begin();itr!= m_vVehiclePoolParking.end();++itr)
	{
		if( (*itr)->GetID() == id ){ m_vVehiclePoolParking.erase(itr); return true; }
	}

	//find from Traffic light
	for(ALTObject3DList::iterator itr= m_vrTrafficLights.begin();itr!= m_vrTrafficLights.end();++itr)
	{
		if( (*itr)->GetID() == id ){ m_vrTrafficLights.erase(itr); return true; }
	}

	//find from TollGate
	for(ALTObject3DList::iterator itr= m_vrTollGate.begin();itr!= m_vrTollGate.end();++itr)
	{
		if( (*itr)->GetID() == id ){ m_vrTollGate.erase(itr); return true; }
	}

	//find from StopSign
	for(ALTObject3DList::iterator itr= m_vrStopSign.begin();itr!= m_vrStopSign.end();++itr)
	{
		if( (*itr)->GetID() == id ){ m_vrStopSign.erase(itr); return true; }
	}
	
	//find from YieldSign
	for(ALTObject3DList::iterator itr= m_vrYieldSign.begin();itr!= m_vrYieldSign.end();++itr)
	{
		if( (*itr)->GetID() == id ){ m_vrYieldSign.erase(itr); return true; }
	}

	//find from YieldSign
	for(ALTObject3DList::iterator itr= m_vPaxBusParkSpots.begin();itr!= m_vPaxBusParkSpots.end();++itr)
	{
		if( (*itr)->GetID() == id ){ m_vPaxBusParkSpots.erase(itr); return true; }
	}

	for(ALTObject3DList::iterator itr= m_vBagCartParkSpots.begin();itr!= m_vBagCartParkSpots.end();++itr)
	{
		if( (*itr)->GetID() == id ){ m_vBagCartParkSpots.erase(itr); return true; }
	}
	return false;
}


void CAirport3D::UpdateAddorRemoveObjects()
{
		
	std::vector<int> objectIDs;
	ALTObject3DList m_removedObjIDs;
	ALTObject3DList m_addObjects;

	bool bNeedDoAutoSnap = false;

	//update Rw
	ALTAirport::GetRunwaysIDs(m_nID,objectIDs);
	ALTObject3DList newRwList;
	//remove 
	for(size_t i=0;i<objectIDs.size();++i){
		ALTObject3DList::iterator theItr;
		if(  m_vRunways.end() == (theItr=find_if( m_vRunways.begin(),m_vRunways.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
			CRunway3D * pRunway3D = new CRunway3D(objectIDs[i]);
			newRwList.push_back(pRunway3D);
			pRunway3D->Update();
			m_addObjects.push_back(pRunway3D);
		}else {
			newRwList.push_back(*theItr);
			m_vRunways.erase(theItr);
		}
	}
	m_removedObjIDs.insert(m_removedObjIDs.end(),m_vRunways.begin(),m_vRunways.end());
	m_vRunways = newRwList;


	//update taxiways
	objectIDs.clear();
	ALTAirport::GetTaxiwaysIDs(m_nID,objectIDs);

	ALTObject3DList newTxList;
	for(size_t i=0;i<objectIDs.size();++i){
		ALTObject3DList::iterator theItr;
		if(  m_vTaxways.end() == (theItr=find_if( m_vTaxways.begin(),m_vTaxways.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
			CTaxiway3D * pTaxi3D = new CTaxiway3D(objectIDs[i]);
			newTxList.push_back(pTaxi3D);
			pTaxi3D->Update();	
			m_addObjects.push_back(pTaxi3D);
		}else {
			newTxList.push_back(*theItr);
			m_vTaxways.erase(theItr);
		}
	}
	m_removedObjIDs.insert(m_removedObjIDs.end(),m_vTaxways.begin(),m_vTaxways.end());
	m_vTaxways = newTxList;	


	//get stands
	objectIDs.clear();
	ALTAirport::GetStandsIDs(m_nID,objectIDs);

	ALTObject3DList newStandList;
	for(size_t i=0;i<objectIDs.size();++i){
		ALTObject3DList::iterator theItr;
		if(  m_vStands.end() == (theItr=find_if( m_vStands.begin(),m_vStands.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
			CStand3D * pStand3D = new CStand3D(objectIDs[i]);
			newStandList.push_back(pStand3D);
			pStand3D->Update();		
			m_addObjects.push_back(pStand3D);
		}else {
			newStandList.push_back(*theItr);
			m_vStands.erase(theItr);
		}
	}
	m_removedObjIDs.insert(m_removedObjIDs.end(),m_vStands.begin(),m_vStands.end());
	m_vStands = newStandList;

	// get deice pads
	objectIDs.clear();
	ALTAirport::GetDeicePadsIDs(m_nID,objectIDs);

	ALTObject3DList newDeiceList;
	for(size_t i=0;i<objectIDs.size();++i){
		ALTObject3DList::iterator theItr;
		if(  m_vDeicePad.end() == (theItr=find_if( m_vDeicePad.begin(),m_vDeicePad.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
			CDeicePad3D * pDeice3D = new CDeicePad3D(objectIDs[i]);
			newDeiceList.push_back(pDeice3D);
			pDeice3D->Update();
			m_addObjects.push_back(pDeice3D);
		}else {
			newDeiceList.push_back(*theItr);
			m_vDeicePad.erase(theItr);
		}
	}
	m_removedObjIDs.insert(m_removedObjIDs.end(),m_vDeicePad.begin(),m_vDeicePad.end());
	m_vDeicePad = newDeiceList;


	// get stretch
	objectIDs.clear();
	ALTAirport::GetStretchsIDs(m_nID,objectIDs);

	ALTObject3DList newStretchList;
	for(size_t i=0;i<objectIDs.size();++i){
		ALTObject3DList::iterator theItr;
		if(  m_vStretchs.end() == (theItr=find_if( m_vStretchs.begin(),m_vStretchs.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
			CStretch3D * pStretch = new CStretch3D(objectIDs[i]);
			newStretchList.push_back(pStretch);
			pStretch->Update();
			m_addObjects.push_back(pStretch);
		}else {
			newStretchList.push_back(*theItr);
			m_vStretchs.erase(theItr);
		}
	}
	m_removedObjIDs.insert(m_removedObjIDs.end(),m_vStretchs.begin(),m_vStretchs.end());
	m_vStretchs = newStretchList;

	//get CircleStretch
	objectIDs.clear();
	ALTAirport::GetCircleStretchsIDs(m_nID,objectIDs);
	ALTObject3DList newCsList;
	//remove 
	for(size_t i=0;i<objectIDs.size();++i){
		ALTObject3DList::iterator theItr;
		if(  m_vCircleStretchs.end() == (theItr=find_if( m_vCircleStretchs.begin(),m_vCircleStretchs.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
			CCircleStretch3D * pCircleStretch3D = new CCircleStretch3D(objectIDs[i]);
			newCsList.push_back(pCircleStretch3D);
			pCircleStretch3D->Update();
			m_addObjects.push_back(pCircleStretch3D);
		}else {
			newCsList.push_back(*theItr);
			m_vCircleStretchs.erase(theItr);
		}
	}
	m_removedObjIDs.insert(m_removedObjIDs.end(),m_vCircleStretchs.begin(),m_vCircleStretchs.end());
	m_vCircleStretchs = newCsList;

	if(m_addObjects.size())
	{
		ReflectChangeOf( m_addObjects );
	}
	else if(m_removedObjIDs.size())
	{
		ReflectRemoveOf(m_removedObjIDs);
	}

	//get heliport
	objectIDs.clear();
	ALTAirport::GetHeliportsIDs(m_nID,objectIDs);
	ALTObject3DList newHeliportList;
	for(size_t i=0; i<objectIDs.size(); ++i)
	{
		ALTObject3DList::iterator theItr;
		if(m_vHeliport.end() == (theItr=find_if(m_vHeliport.begin(),m_vHeliport.end(),ALTObject3DIDIs(objectIDs[i]))))
		{
			CHeliport3D* pHeliport = new CHeliport3D(objectIDs[i]);
            newHeliportList.push_back(pHeliport);
			pHeliport->Update();
		}
		else
		{
			newHeliportList.push_back(*theItr);
			m_vHeliport.erase(theItr);
		}
	}
	m_vHeliport = newHeliportList;

	//get intersection
	objectIDs.clear();
	ALTAirport::GetRoadIntersectionsIDs(m_nID,objectIDs);

	ALTObject3DList newIntersectionList;
	for(size_t i=0;i<objectIDs.size();++i){
		ALTObject3DList::iterator theItr;
		if(  m_vRoadIntersections.end() == (theItr=find_if( m_vRoadIntersections.begin(),m_vRoadIntersections.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
			CRoadIntersection3D * pIntersection = new CRoadIntersection3D(objectIDs[i]);
			newIntersectionList.push_back(pIntersection);
			pIntersection->Update();			
		}else {
			newIntersectionList.push_back(*theItr);
			m_vRoadIntersections.erase(theItr);
		}
	}
	m_vRoadIntersections = newIntersectionList;

	// get vehicle pool parking 
	objectIDs.clear();
	ALTAirport::GetVehiclePoolParkingIDs(m_nID,objectIDs);

	ALTObject3DList newVehiclePoolParkingList;
	for(size_t i=0;i<objectIDs.size();++i){
		ALTObject3DList::iterator theItr;
		if(  m_vVehiclePoolParking.end() == (theItr=find_if( m_vVehiclePoolParking.begin(),m_vVehiclePoolParking.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
			CVehiclePoolParking3D * pVehiclePoolParkingList = new CVehiclePoolParking3D(objectIDs[i]);
			newVehiclePoolParkingList.push_back(pVehiclePoolParkingList);
			pVehiclePoolParkingList->Update();			
		}else {
			newVehiclePoolParkingList.push_back(*theItr);
			m_vVehiclePoolParking.erase(theItr);
		}
	}
	m_vVehiclePoolParking = newVehiclePoolParkingList;
	
	
	//get traffic light
	objectIDs.clear();
	ALTAirport::GetTrafficLightsIDs(m_nID,objectIDs);

	ALTObject3DList newTrafficLightList;
	for(size_t i=0;i<objectIDs.size();++i){
		ALTObject3DList::iterator theItr;
		if(  m_vrTrafficLights.end() == (theItr=find_if( m_vrTrafficLights.begin(),m_vrTrafficLights.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
			CTrafficLight3D * pTrafficLight = new CTrafficLight3D(objectIDs[i]);
			newTrafficLightList.push_back(pTrafficLight);
			pTrafficLight->Update();			
		}else {
			newTrafficLightList.push_back(*theItr);
			m_vrTrafficLights.erase(theItr);
		}
	}
	m_vrTrafficLights = newTrafficLightList;

	//get TollGate
	objectIDs.clear();
	ALTAirport::GetTollGatesIDs(m_nID,objectIDs);

	ALTObject3DList newTollGate;
	for(size_t i=0;i<objectIDs.size();++i){
		ALTObject3DList::iterator theItr;
		if(  m_vrTollGate.end() == (theItr=find_if( m_vrTollGate.begin(),m_vrTollGate.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
			CTollGate3D * pTollGate = new CTollGate3D(objectIDs[i]);
			newTollGate.push_back(pTollGate);
			pTollGate->Update();			
		}else {
			newTollGate.push_back(*theItr);
			m_vrTollGate.erase(theItr);
		}
	}
	m_vrTollGate = newTollGate;


	//get StopSign
	objectIDs.clear();
	ALTAirport::GetStopSignsIDs(m_nID,objectIDs);

	ALTObject3DList newStopSign;
	for(size_t i=0;i<objectIDs.size();++i){
		ALTObject3DList::iterator theItr;
		if(  m_vrStopSign.end() == (theItr=find_if( m_vrStopSign.begin(),m_vrStopSign.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
			CStopSign3D * pStopSign = new CStopSign3D(objectIDs[i]);
			newStopSign.push_back(pStopSign);
			pStopSign->Update();			
		}else {
			newStopSign.push_back(*theItr);
			m_vrStopSign.erase(theItr);
		}
	}
	m_vrStopSign = newStopSign;

	//get YieldSign
	objectIDs.clear();
	ALTAirport::GetYieldSignsIDs(m_nID,objectIDs);

	ALTObject3DList newYieldSign;
	for(size_t i=0;i<objectIDs.size();++i){
		ALTObject3DList::iterator theItr;
		if(  m_vrYieldSign.end() == (theItr=find_if( m_vrYieldSign.begin(),m_vrYieldSign.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
			CYieldSign3D * pYieldSign = new CYieldSign3D(objectIDs[i]);
			newYieldSign.push_back(pYieldSign);
			pYieldSign->Update();			
		}else {
			newYieldSign.push_back(*theItr);
			m_vrYieldSign.erase(theItr);
		}
	}
	m_vrYieldSign = newYieldSign;


	{
		//update StartPosition
		objectIDs.clear();
		ALTAirport::GetStartPositionIDs(m_nID,objectIDs);

		ALTObject3DList newStPList;
		for(size_t i=0;i<objectIDs.size();++i){
			ALTObject3DList::iterator theItr;
			if(  m_vStartPositions.end() == (theItr=find_if( m_vStartPositions.begin(),m_vStartPositions.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
				CStartPosition3D * pStP3D = new CStartPosition3D(objectIDs[i]);
				newStPList.push_back(pStP3D);
				pStP3D->Update();
			}else {
				newStPList.push_back(*theItr);
				m_vStartPositions.erase(theItr);
			}
		}

		m_vStartPositions = newStPList;	
	}

	{
		//update MeetingPoint
		objectIDs.clear();
		ALTAirport::GetMeetingPointIDs(m_nID,objectIDs);

		ALTObject3DList newMeetingPointList;
		for(size_t i=0;i<objectIDs.size();++i){
			ALTObject3DList::iterator theItr;
			if(  m_vMeetingPoints.end() == (theItr=find_if( m_vMeetingPoints.begin(),m_vMeetingPoints.end(), ALTObject3DIDIs(objectIDs[i]) ) ) ){
				CMeetingPoint3D * pStP3D = new CMeetingPoint3D(objectIDs[i]);
				newMeetingPointList.push_back(pStP3D);
				pStP3D->Update();
			}else {
				newMeetingPointList.push_back(*theItr);
				m_vMeetingPoints.erase(theItr);
			}
		}

		m_vMeetingPoints = newMeetingPointList;	
	}

	{
		//update airside 
		//update MeetingPoint
		objectIDs.clear();
		ALTAirport::GetPaxBusParkPosIDs(m_nID,objectIDs);

		ALTObject3DList newPaxBusPosList;
		for(size_t i=0;i<objectIDs.size();++i)
		{
			ALTObject3DList::iterator theItr= find_if( m_vPaxBusParkSpots.begin(),m_vPaxBusParkSpots.end(), ALTObject3DIDIs(objectIDs[i]) ) ; 
			if(  m_vPaxBusParkSpots.end() == theItr )
			{
				CAirsidePaxBusParkSpot3D * pStP3D = new CAirsidePaxBusParkSpot3D(objectIDs[i]);
				newPaxBusPosList.push_back(pStP3D);
				pStP3D->Update();
			}
			else
			{
				newPaxBusPosList.push_back(*theItr);
			}
		}

		m_vPaxBusParkSpots = newPaxBusPosList;	
	}

	{
		//update airside 
		//update MeetingPoint
		objectIDs.clear();
		ALTAirport::GetBagCartParkPosIDs(m_nID,objectIDs);

		ALTObject3DList newbagCartlist;
		for(size_t i=0;i<objectIDs.size();++i)
		{
			ALTObject3DList::iterator theItr=find_if( m_vBagCartParkSpots.begin(),m_vBagCartParkSpots.end(), ALTObject3DIDIs(objectIDs[i]) );
			if(  m_vBagCartParkSpots.end() == theItr )
			{
				CAirsideBagCartParkPos3D * pStP3D = new CAirsideBagCartParkPos3D(objectIDs[i]);
				newbagCartlist.push_back(pStP3D);
				pStP3D->Update();
			}
			else
			{
				newbagCartlist.push_back(*theItr);
			}
		}
		m_vBagCartParkSpots = newbagCartlist;	
	}

	
}

void CAirport3D::RenderToDepth( C3DView *pView )
{
	//render to depth buffer
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f,4.0f);	
		
	for(size_t i=0;i<m_vTaxways.size();++i){
		CTaxiway3D * pTaxi = (CTaxiway3D*)m_vTaxways[i].get();
		pTaxi->RenderSurface(pView);
	}
	m_vFilletTaxiways.RenderSurface(pView);

	for(size_t i=0;i<m_vRunways.size();++i){
		CRunway3D * pRunway3D = (CRunway3D*)m_vRunways.at(i).get();
		pRunway3D->RenderBase(pView);
	}

	for(size_t i=0; i<m_vHeliport.size(); ++i)
	{
		CHeliport3D* pHeliport3D = (CHeliport3D*)m_vHeliport.at(i).get();
		pHeliport3D->RenderBase(pView);
	}

	for(int i =0;i <(int) m_vStretchs.size(); ++i )
	{
		CStretch3D  *pStretch = (CStretch3D*)m_vStretchs.at(i).get();
		pStretch->RenderBaseWithSideLine(pView);
	}

	for (int i=0; i<(int)m_vCircleStretchs.size(); ++i)
	{
		CCircleStretch3D * pCircleStretch = (CCircleStretch3D*)m_vCircleStretchs.at(i).get();
		pCircleStretch->RenderBaseWithSideLine(pView);
	}

	m_vIntersectedStretch.DrawOGL(pView);

	for(size_t i= 0 ;i<m_vDeicePad.size();i++){
		CDeicePad3D * pDeice = ( CDeicePad3D *) m_vDeicePad[i].get();
		pDeice->RenderSurface(pView);
	}

	for (size_t i = 0;i < m_vVehiclePoolParking.size();i++) {
		CVehiclePoolParking3D * pVehiclePoolParking = (CVehiclePoolParking3D *)m_vVehiclePoolParking[i].get();
		pVehiclePoolParking->DrawOGL(pView);
	}


	glDisable(GL_POLYGON_OFFSET_FILL);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
}




void CAirport3D::AutoSnap( ALTObject3DList vObj3Ds )
{	
	for(int i=0;i<(int)vObj3Ds.size();i++)
	{
		ALTObject3D * pObj3D = vObj3Ds[i].get();
		if(pObj3D->GetObjectType() == ALT_STAND)
		{
			CStand3D * pStand = (CStand3D* )pObj3D;
			pStand->SnapTo(m_vTaxways);		
		}
		else if(pObj3D->GetObjectType() == ALT_TAXIWAY )
		{
			CTaxiway3D* pTaxiway = (CTaxiway3D*) pObj3D;
			ALTObject3DList rwTaxiList;
			rwTaxiList.insert(rwTaxiList.end(), m_vTaxways.begin(),m_vTaxways.end());
			rwTaxiList.insert(rwTaxiList.end(), m_vRunways.begin(), m_vRunways.end());
			pTaxiway->SnapTo(rwTaxiList);
			//pTaxiway->SnapTo(m_vTaxways);	
			//pTaxiway->SnapTo(m_vRunways);
		}
		else if(pObj3D->GetObjectType() == ALT_DEICEBAY)
		{
			CDeicePad3D * pDeice = (CDeicePad3D* )pObj3D;
			pDeice->SnapTo(m_vTaxways);		
		}
		else if (pObj3D->GetObjectType() == ALT_STRETCH)
		{
			CStretch3D * pStretch = (CStretch3D*)pObj3D;
			pStretch->SnapTo(m_vStretchs);
			pStretch->SnapTo(m_vCircleStretchs);
		}
	}
}

void CAirport3D::ReadGroundInfo()
{
	m_vLevelList.ReadFullLevelList();
	for(int i=0;i<m_vLevelList.GetCount();i++)
	{
		CFloor2& cFloor = m_vLevelList.GetLevel(i);
		if(!cFloor.IsCADLoaded()) cFloor.LoadCAD();
		if(cFloor.IsCADLoaded() && !cFloor.IsCADLayersLoaded())
			cFloor.LoadCADLayers();
		if((cFloor.IsCADLoaded() && cFloor.IsCADLayersLoaded() && !cFloor.IsCADProcessed()) )
		{
			cFloor.ProcessCAD();
		}
	}
}

void CAirport3D::Update()
{
	m_altAirport.ReadAirport(m_nID);
}

void CAirport3D::GetObject3DList( ALTObject3DList& objList ) const
{
	objList.insert(objList.end(),m_vRunways.begin(),m_vRunways.end());
	objList.insert(objList.end(),m_vTaxways.begin(),m_vTaxways.end());
	objList.insert(objList.end(),m_vStands.begin(),m_vStands.end());
	objList.insert(objList.end(),m_vDeicePad.begin(),m_vDeicePad.end() );
	objList.insert(objList.end(),m_vStretchs.begin(),m_vStretchs.end() );	
	objList.insert(objList.end(),m_vRoadIntersections.begin(),m_vRoadIntersections.end() );	
	objList.insert(objList.end(),m_vVehiclePoolParking.begin(),m_vVehiclePoolParking.end());
	objList.insert(objList.end(),m_vrTrafficLights.begin(),m_vrTrafficLights.end());
	objList.insert(objList.end(),m_vrTollGate.begin(),m_vrTollGate.end());
	objList.insert(objList.end(),m_vrStopSign.begin(),m_vrStopSign.end());
	objList.insert(objList.end(),m_vrYieldSign.begin(),m_vrYieldSign.end());
	objList.insert(objList.end(),m_vHeliport.begin(),m_vHeliport.end());
	objList.insert(objList.end(),m_vCircleStretchs.begin(),m_vCircleStretchs.end() );	
	objList.insert(objList.end(),m_vPaxBusParkSpots.begin(),m_vPaxBusParkSpots.end());
	objList.insert(objList.end(),m_vBagCartParkSpots.begin(),m_vBagCartParkSpots.end());
}


ALTObject3D* CAirport3D::AddObject( ALTObject* pObj )
{
	ALTObject3D * pNewObj3D = NULL;
	switch(pObj->GetType())
	{
	case ALT_RUNWAY:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vRunways.push_back(pNewObj3D);
		break;
	case ALT_HELIPORT:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vHeliport.push_back(pNewObj3D);
		break;
	case ALT_TAXIWAY:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vTaxways.push_back(pNewObj3D);
		break;
	case ALT_STAND:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vStands.push_back(pNewObj3D);
		break;
	case ALT_DEICEBAY:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vDeicePad.push_back(pNewObj3D);
		break;
	case ALT_STRETCH:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vStretchs.push_back(pNewObj3D);
		break;
	case ALT_INTERSECTIONS : 
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vRoadIntersections.push_back(pNewObj3D);
		break;
	case ALT_VEHICLEPOOLPARKING : 
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vVehiclePoolParking.push_back(pNewObj3D);
		break;
	case ALT_TRAFFICLIGHT : 
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vrTrafficLights.push_back(pNewObj3D);
		break;
	case ALT_TOLLGATE : 
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vrTollGate.push_back(pNewObj3D);
		break;
	case ALT_STOPSIGN : 
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vrStopSign.push_back(pNewObj3D);
		break;
	case ALT_YIELDSIGN : 
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vrYieldSign.push_back(pNewObj3D);
		break;
	case ALT_LANEADAPTER:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		//m_vrLaneAdapter.push_back(pNewObj3D);
		break;
	case ALT_CIRCLESTRETCH:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vCircleStretchs.push_back(pNewObj3D);
		break;
	case ALT_STARTPOSITION:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vStartPositions.push_back(pNewObj3D);
		break;
	case ALT_MEETINGPOINT:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vMeetingPoints.push_back(pNewObj3D);
		break;
	case ALT_APAXBUSSPOT:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vPaxBusParkSpots.push_back(pNewObj3D);
		break;
	case ALT_ABAGCARTSPOT:
		pNewObj3D = ALTObject3D::NewALTObject3D(pObj);
		m_vBagCartParkSpots.push_back(pNewObj3D);
		break;
	default:
		ASSERT(false);
	}
	return pNewObj3D;
}

void UpdateTaxiwayHoldPositions(const ALTAirport& airport, ALTObject3DList runway3Ds, ALTObject3DList taxiway3Ds ,const FilletTaxiwaysInAirport& vFillets, IntersectionNodesInAirport& vAirportNodes , const std::vector<int>& vChangeNodesIdx  )//update HoldPosition
{
	for(int i=0;i< (int)taxiway3Ds.size();i++)
	{
		ALTObject3D* pObj3D = taxiway3Ds.at(i).get();
		if(pObj3D && pObj3D->GetObjectType() == ALT_TAXIWAY)
		{
			CTaxiway3D* pTaxiway3D = (CTaxiway3D*)pObj3D;
			bool bTaxiwayInChangeNode = false;
			for(int j=0;j<(int)vChangeNodesIdx.size();j++)
			{
				IntersectionNode& theNode = vAirportNodes.NodeAt(vChangeNodesIdx[j]);
				if( theNode.HasObject(pTaxiway3D->GetID()) )
				{
					bTaxiwayInChangeNode = true;
					break;
				}
			}

			if( bTaxiwayInChangeNode )
			{
				pTaxiway3D->UpdateHoldPositions( airport, runway3Ds,vFillets, vAirportNodes);
			}
		}
	}
}

void UpdateRunwayExits(ALTObject3DList runway3Ds,IntersectionNodesInAirport& vAirportNodes , const std::vector<int>& vChangeNodesIdx )
{
	for(int i=0;i< (int)runway3Ds.size();i++)
	{
		ALTObject3D* pObj3D = runway3Ds.at(i).get();
		if(pObj3D && pObj3D->GetObjectType() == ALT_RUNWAY)
		{

			CRunway3D * pRunway3D = (CRunway3D*)pObj3D;
			bool bRunwayInChangeNode = false;
			for(int j=0;j<(int)vChangeNodesIdx.size();j++)
			{
				IntersectionNode& theNode = vAirportNodes.NodeAt(vChangeNodesIdx[j]);
				if( theNode.HasObject(pRunway3D->GetID()) )
				{
					bRunwayInChangeNode = true;
					break;
				}
			}
			if(bRunwayInChangeNode)
			{
				pRunway3D->GetRunway()->UpdateRunwayExits(vAirportNodes);
			}
		}
	}
}

bool CAirport3D::ReflectChangeOf( ALTObject3DList obj3DList )
{	

	const int nChangeSteps = 10;
	CProgressBar bar( "Reflecting Changes...", 100, nChangeSteps, TRUE );

	
	if( obj3DList.HasType(ALT_RUNWAY) || obj3DList.HasType(ALT_TAXIWAY)) 
	{
		AutoSnap( m_vTaxways );
		AutoSnap( m_vStands );
	}
	else{
		AutoSnap(obj3DList);
	}

	if (obj3DList.HasType(ALT_STRETCH))
	{
		AutoSnap(m_vStretchs);
	}

	bar.StepIt();
	bar.SetText("Updating IntesectionNodes...");	

	
	ALTObject3DList ALLobj3DList;
	GetObject3DList(ALLobj3DList);

	std::vector<int> vChangeNodesIdx =	m_vAirportNodes.ReflectChangeOf( obj3DList.GetObjectList(), ALLobj3DList.GetObjectList() );
	
	bar.StepIt();
	bar.SetText("Updating Fillet Taxiways...");

	
	m_vFilletTaxiways.UpdateFillets( m_vAirportNodes,  vChangeNodesIdx  );

	bar.StepIt();
	bar.SetText( "Updating Road Intersections...");

	m_vIntersectedStretch.UpdateIntersectedStretchInAirport(m_vAirportNodes,vChangeNodesIdx);


	bar.StepIt();
	bar.SetText( "Updating Hold Positions..." );

	for(int i=0;i< (int)m_vTaxways.size();i++)
	{
		ALTObject3D* pObj3D = m_vTaxways.at(i).get();
		if(pObj3D && pObj3D->GetObjectType() == ALT_TAXIWAY)
		{
			CTaxiway3D* pTaxiway3D = (CTaxiway3D*)pObj3D;			
			pTaxiway3D->UpdateHoldPositions(m_altAirport, m_vRunways, m_vFilletTaxiways.m_vFilletTaxiways, m_vAirportNodes);

		}
	}
	bar.StepIt();
	bar.SetText( "Updating Runway Exits...");

	try
	{
		UpdateRunwayExits(m_vRunways, m_vAirportNodes, vChangeNodesIdx);	
	}
	catch(CException* e)
	{
		delete e;
	}
	catch(...)
	{
		ASSERT(false);
	}



	//update Runway exit	

	UpdateObjectsRelations();	

	return true;
}

void CAirport3D::UpdateObjectsRelations()
{

	for(int i=0;i< (int)m_vTaxways.size();i++)
	{
		CTaxiway3D * ptaxiway3D = (CTaxiway3D*)m_vTaxways.at(i).get();
		ptaxiway3D->SetRelatFillets(m_vFilletTaxiways);
	}

	for (int i=0; i<(int)m_vStretchs.size();i++)
	{
		CStretch3D * pStretch3D = (CStretch3D*)m_vStretchs.at(i).get();
		pStretch3D->SetRelatFillets(m_vIntersectedStretch);
	}
	
	
}


void CAirport3D::RenderNodeNames(C3DView  *pView)
{	
	glColor3ub(20,20,220);
	for(int i=0;i< m_vAirportNodes.GetCount();i++)
	{
		const IntersectionNode& node = m_vAirportNodes.NodeAt(i);
		{
			CPoint2008 pos = node.GetPosition();
			CString nodename = node.GetName();
//#ifdef _DEBUG		
			nodename.Format("(%d)%s",node.GetID(),_T(node.GetName()));
//#endif
			TEXTMANAGER3D->DrawBitmapText(nodename, pos );
		}
	}		
}

void CAirport3D::RenderNodes( C3DView *pView )
{
	glColor3ub(255,255,255);
	for(int i =0 ;i < m_vAirportNodes.GetCount();i++)
	{
		const IntersectionNode& node = m_vAirportNodes.NodeAt(i);
		CPoint2008 pos = node.GetPosition();
		glPushMatrix();
		glTranslated(pos.getX(),pos.getY(),pos.getZ());
		glScaled(500,500,0);
		glCallList(pView->GetParentFrame()->DiskDLID());
		glPopMatrix();
	}
}

void CAirport3D::UpdateObject( int objID )
{
	ALTObject3D * pObj3D = GetObject3D(objID);
	ASSERT(pObj3D);
	if(pObj3D)
		pObj3D->Update();
}

void CAirport3D::ReflectRemoveOf( ALTObject3DList vObj3Ds )
{
	const int nChangeSteps = 10;
	CProgressBar bar( "Reflecting Remove...", 100, nChangeSteps, TRUE );

	bar.StepIt();
	bar.SetText("Updating IntesectionNodes...");	
	std::vector<int> vChangeNodesIdx  = m_vAirportNodes.ReflectRemoveOf( vObj3Ds.GetObjectList() );

	bar.StepIt();
	bar.SetText("Updating Fillet Taxiways...");

	m_vFilletTaxiways.UpdateFillets(  m_vAirportNodes, vChangeNodesIdx );

	bar.StepIt();
	bar.SetText( "Updating Road Intersections...");

	m_vIntersectedStretch.UpdateIntersectedStretchInAirport(m_vAirportNodes, vChangeNodesIdx);

	bar.StepIt();
	bar.SetText( "Updating Hold Positions..." );
	
	for(int i=0;i< (int)m_vTaxways.size();i++)
	{
		ALTObject3D* pObj3D = m_vTaxways.at(i).get();
		if(pObj3D && pObj3D->GetObjectType() == ALT_TAXIWAY)
		{
			CTaxiway3D* pTaxiway3D = (CTaxiway3D*)pObj3D;			
			pTaxiway3D->UpdateHoldPositions(m_altAirport,m_vRunways, m_vFilletTaxiways.m_vFilletTaxiways, m_vAirportNodes);
			
		}
	}

	UpdateObjectsRelations();
}


void CAirport3D::RenderStenciles()
{
	glClear(GL_STENCIL_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	glStencilFunc(GL_ALWAYS,0x1,0x1);
	glStencilOp(GL_REPLACE,GL_REPLACE,GL_REPLACE);
//	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	for (int i=0;i<(int)m_vStretchs.size();i++)
	{
		CStretch3D * pStretch3D = (CStretch3D*)m_vStretchs.at(i).get();
		pStretch3D->RenderOneStencil();
	}

	m_vIntersectedStretch.RenderStencil();

//	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);

}

void CAirport3D::RenderHoldLineNames( C3DView* pView )
{
	glColor3ub(20,20,220);
	for(size_t idxTaxiway=0;idxTaxiway<m_vTaxways.size();idxTaxiway++)
	{
		CTaxiway3D* pTaxiway3D = (CTaxiway3D*)m_vTaxways.at(idxTaxiway).get();
		pTaxiway3D->RenderHoldName(pView);
		
	}
}