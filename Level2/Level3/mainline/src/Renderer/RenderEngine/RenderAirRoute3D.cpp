#include "StdAfx.h"
#include ".\renderairroute3d.h"
#include "SelectionManager.h"
#include "Render3DScene.h"
#include "OgreConvert.h"
#include "InputAirside/AirRoute.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside/Runway.h"
#include "common/ARCPath.h"
#include "Common\ARCPipe.h"
#include "InputAirside\ALTObjectDisplayProp.h"
#include "materialdef.h"
#include "Common/IARCportLayoutEditContext.h"
#include "InputAirside/ALTAirportLayout.h"
//CRenderAirRoute3D::CRenderAirRoute3D( Ogre::SceneNode* pNode, CRender3DScene* p3DScene, const CGuid& guid )
//:CAirside3DObject(pNode,p3DScene,guid )
//{
//
//}
//CRenderAirRoute3D::~CRenderAirRoute3D(void)
//{
//}
void CRenderAirRoute3D::UpdateSelected( bool b )
{

}
using namespace Ogre;
void CRenderAirRoute3D::Update3D( ALTObject* pBaseObj )
{
	if(pBaseObj == NULL || pBaseObj->GetType()  != ALT_AIRROUTE)
		return;

	//SceneManager* pScene = GetScene();
	Ogre::ManualObject* pObj = _GetOrCreateManualObject(GetName());//OgreUtil::createOrRetrieveManualObject(GetIDName(),pScene);
	Ogre::ManualObject *pLineObj = _GetOrCreateManualObject(GetName()+_T("Line"));
	Build(pBaseObj, pObj,pLineObj);
	AddObject(pObj,true);
	AddObject(pLineObj);

	//UpdateSelected(GetSelected());
}

//void CRenderAirWayPoint3D::Update3D( ALTObject* pBaseObj, int nUpdateCode /*= Terminal3DUpdateAll*/ )
//{
//
//}

#define AIRROUTE_MAT VERTEXCOLOR_TRANSPARENT
void CRenderAirRoute3D::Build( ALTObject* pBaseObj, Ogre::ManualObject* pObj, Ogre::ManualObject* pLineObj )
{
	if(pBaseObj == NULL || pBaseObj->GetType()  != ALT_AIRROUTE)
		return;
	
	CAirRoute *pAirRoute = (CAirRoute *)pBaseObj;
	if(pAirRoute == NULL)
		return;

	InputAirside *pAirside =  GetRoot().GetModel()->OnQueryInputAirside();


	ALTObjectList m_vWayPointList;
	Runway * pRunway = NULL;
	int m_iRunwayside;

	for(int i=0 ;i< (int)pAirRoute->getARWaypoint().size();++i)
	{
		int wayptID = pAirRoute->getARWaypoint().at(i)->getWaypoint().getID();

		ALTObject * pObj =  pAirside->GetAirspace()->GetObjectByID(ALT_WAYPOINT, wayptID);
		if(pObj) 
			m_vWayPointList.push_back(pObj);

	}
	// get runway 
	if(pAirRoute->getRouteType() == CAirRoute::STAR ||
		pAirRoute->getRouteType() == CAirRoute::SID ||
		pAirRoute->getRouteType() == CAirRoute::MIS_APPROACH)
	{
		if(pAirRoute->GetConnectRunwayCount())
		{
			int rwID = pAirRoute->getLogicRunwayByIdx(0).first;
			m_iRunwayside = pAirRoute->getLogicRunwayByIdx(0).second;

			pRunway = pAirside->GetActiveAirport()->GetRunwayByID(rwID);
		}

	}


	ARCPath3 m_routePath;
	std::vector<ARCPath3>m_detachPath;
	std::vector<double > m_vlowLimits;
	std::vector<double > m_vHighLimits;
	std::vector<BOOL>  m_bIdentify;
	//BOOL bIsHeading = FALSE;
	m_routePath.clear();

	pAirRoute->CalWaypointExtentPoint();


	if( pRunway )
	{
		CAirRoute::RouteType rtType = pAirRoute->getRouteType() ;
		if( rtType == CAirRoute::SID || rtType == CAirRoute::MIS_APPROACH )
		{
			if(m_iRunwayside)
				m_routePath.push_back(  pRunway->GetPath().getPoint(0) );
			else
				m_routePath.push_back(	pRunway->GetPath().getPoint(1) );
			m_vlowLimits.push_back(0);
			m_vHighLimits.push_back(0);
			m_bIdentify.push_back(TRUE);
		}
	}

	std::vector<ARWaypoint *> m_tempARW = pAirRoute->getARWaypoint();

	ALTAirport  * pAirport = pAirside->GetActiveAirport();
	double dAirpotAlt = pAirport->getElevation();

	for(int i=0 ;i< (int)m_vWayPointList.size();++i)
	{
		AirWayPoint * waypt = NULL;
		ARWaypoint *pARWayPoint = NULL;	
	
		waypt = (AirWayPoint*)m_vWayPointList.at(i).get();
		pARWayPoint = m_tempARW[i];
		
		if(!waypt)
			continue;

		if(pARWayPoint->getDepartType() != ARWaypoint::Heading)
		{
			m_routePath.push_back( waypt->GetLocation(*pAirport) );
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
			m_routePath.push_back( waypt->GetLocation(*pAirport) );
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
	if( pRunway )
	{			
		if(pAirRoute->getRouteType() == CAirRoute::STAR)
		{
			if(m_iRunwayside)
				m_routePath.push_back( pRunway->GetLandThreshold2Point() );
			else
				m_routePath.push_back( pRunway->GetLandThreshold1Point() );
			m_vlowLimits.push_back(0);
			m_vHighLimits.push_back(0);
			m_bIdentify.push_back(TRUE);
		}        
	}

	int tempCount = 1;
	int routeCount = static_cast<int>(m_routePath.size());
	if(routeCount < 1) 
		return;

	int indexCount = static_cast<int>(m_bIdentify.size());
	for(int index=0; index<indexCount; index++)
	{
		if(m_bIdentify[index]==FALSE)
			tempCount++;
	}


	Ogre::ColourValue color = OgreConvert::GetColor(ARCColor3(255, 0,0));
	ALTObjectDisplayProp *pDisplay = pAirRoute->getDisplayProp();
	ASSERT(pDisplay != NULL);
	if(pDisplay)
		color = OgreConvert::GetColor(pDisplay->m_dpShape.cColor);
	color.a = 0.2f;


	m_detachPath.resize(tempCount);
	int routeAdd=0, borderAdd=0;
	int limitAdd=0;

	pObj->clear();
	pLineObj->clear();
	pLineObj->begin(VERTEXCOLOR_LIGTHOFF,RenderOperation::OT_LINE_LIST);
	pObj->begin(AIRROUTE_MAT,RenderOperation::OT_TRIANGLE_LIST);
	int nVertexAdd = 0;
	for(int i=0; i<tempCount; i++)
	{
		do 
		{
			m_detachPath[i].push_back(m_routePath[routeAdd]);
			if( (routeAdd+1) == routeCount )
				break;
			routeAdd++;
		}while(m_bIdentify[routeAdd-1] !=FALSE && routeAdd<routeCount );
		ARCPipe arcPipe(m_detachPath[i],3200);
		int nPtCnt = static_cast<int>(arcPipe.m_sidePath1.size());

		for(int index=0;index<nPtCnt-1;index++)
		{
			pObj->position(arcPipe.m_sidePath1[index][VX],arcPipe.m_sidePath1[index][VY], m_vlowLimits[limitAdd]);	
			pObj->colour(color);
			pObj->position(arcPipe.m_sidePath1[index+1][VX],arcPipe.m_sidePath1[index+1][VY], m_vlowLimits[limitAdd+1]);

			pObj->position(arcPipe.m_sidePath2[index][VX],arcPipe.m_sidePath2[index][VY],m_vlowLimits[limitAdd]);	
			pObj->position(arcPipe.m_sidePath2[index+1][VX],arcPipe.m_sidePath2[index+1][VY],m_vlowLimits[limitAdd+1]);

			pObj->position(arcPipe.m_sidePath2[index][VX],arcPipe.m_sidePath2[index][VY],m_vHighLimits[limitAdd]);	
			pObj->position(arcPipe.m_sidePath2[index+1][VX],arcPipe.m_sidePath2[index+1][VY],m_vHighLimits[limitAdd+1]);

			pObj->position(arcPipe.m_sidePath1[index][VX],arcPipe.m_sidePath1[index][VY],m_vHighLimits[limitAdd]);
			pObj->position(arcPipe.m_sidePath1[index+1][VX],arcPipe.m_sidePath1[index+1][VY],m_vHighLimits[limitAdd+1]);

			//line object
			pLineObj->position(arcPipe.m_sidePath1[index][VX],arcPipe.m_sidePath1[index][VY], m_vlowLimits[limitAdd]);	
			pLineObj->colour(color);
			pLineObj->position(arcPipe.m_sidePath1[index+1][VX],arcPipe.m_sidePath1[index+1][VY], m_vlowLimits[limitAdd+1]);

			pLineObj->position(arcPipe.m_sidePath2[index][VX],arcPipe.m_sidePath2[index][VY],m_vlowLimits[limitAdd]);	
			pLineObj->position(arcPipe.m_sidePath2[index+1][VX],arcPipe.m_sidePath2[index+1][VY],m_vlowLimits[limitAdd+1]);

			pLineObj->position(arcPipe.m_sidePath2[index][VX],arcPipe.m_sidePath2[index][VY],m_vHighLimits[limitAdd]);	
			pLineObj->position(arcPipe.m_sidePath2[index+1][VX],arcPipe.m_sidePath2[index+1][VY],m_vHighLimits[limitAdd+1]);

			pLineObj->position(arcPipe.m_sidePath1[index][VX],arcPipe.m_sidePath1[index][VY],m_vHighLimits[limitAdd]);
			pLineObj->position(arcPipe.m_sidePath1[index+1][VX],arcPipe.m_sidePath1[index+1][VY],m_vHighLimits[limitAdd+1]);

			//pObj->position(arcPipe.m_sidePath1[index][VX],arcPipe.m_sidePath1[index][VY], m_vlowLimits[limitAdd]);	
			//pObj->position(arcPipe.m_sidePath1[index+1][VX],arcPipe.m_sidePath1[index+1][VY], m_vlowLimits[limitAdd+1]);

			pObj->quad(nVertexAdd,nVertexAdd+1,nVertexAdd+3,nVertexAdd+2);
			pObj->quad(nVertexAdd+2,nVertexAdd+3,nVertexAdd+5,nVertexAdd+4);
			pObj->quad(nVertexAdd+4,nVertexAdd+5,nVertexAdd+7,nVertexAdd+6);
			pObj->quad(nVertexAdd+6,nVertexAdd+7,nVertexAdd+1,nVertexAdd);

			nVertexAdd += 8;
			limitAdd++;
		}
		if(limitAdd<static_cast<int>(m_vlowLimits.size())-1)
		{
			limitAdd++;
		}
	}	
	pObj->end();
	pLineObj->end();

}

void CRenderAirRoute3D::Setup3D( ALTObject* pBaseObj )
{
	Update3D(pBaseObj);
}
