#include "StdAfx.h"
#include ".\finalapproachcontroller.h"
#include "../../InputAirside/GoAroundCriteriaDataProcess.h"
#include "AirspaceResource.h"
#include "AirsideFlightInSim.h"
#include "AirspaceResourceManager.h"
#include "AirsideFlightInSim.h"
#include "AirportResourceManager.h"
#include "AirspaceResourceManager.h"
#include "../../InputAirside/SideStepSpecification.h"
#include "../../InputAirside/CtrlInterventionSpecDataList.h"
#include "FlightPerformanceManager.h"


CFinalApproachController::CFinalApproachController(void)
{
	m_pCtrlInterventionSpec = NULL;
}

CFinalApproachController::~CFinalApproachController(void)
{
	if (NULL!=m_pGoAroundCriteriaDataProcess)
	{
		delete m_pGoAroundCriteriaDataProcess;
		m_pGoAroundCriteriaDataProcess = NULL;
	}
	delete m_pCtrlInterventionSpec;
}

bool CFinalApproachController::FindClearanceInConcern( AirsideFlightInSim * pFlight,ClearanceItem& lastItem, Clearance& newClearance )
{
	//ASSERT(pFlight);

	////AirWayPointInSim *pWayPoint = (AirWayPointInSim*)pFlight->GetResource();
	//AirRoutePointInSim* pWayPoint = ((CFinalApproachSegInSim*)lastItem.GetResource())->GetWayPoint1();
	//ASSERT(pWayPoint);

	//LogicRunwayInSim *pRunwayPortInSim = pFlight->GetLandingRunway();
	//ASSERT(pRunwayPortInSim);

	////side step intervention
	//
	//if(IsNeedToSideStep(pFlight,lastItem))
	//{
	//	LogicRunwayInSim * pSideStepRunway = m_SideStepIntervention.GetSideStepRunway(pFlight,lastItem);	
	//	if(pSideStepRunway)
	//	{
	//		pFlight->SetLandingRunway(pSideStepRunway);			
	//		double landspd = pFlight->GetPerformance()->getLandingSpeed(pFlight);
	//		//landing item
	//		DistanceUnit touchDist = pFlight->GetPerformance()->getDistThresToTouch(pFlight);
	//		ClearanceItem landingItem(pSideStepRunway,OnLanding,touchDist);		
	//		landingItem.SetSpeed(landspd);
	//		ElapsedTime dT = pFlight->GetTimeBetween(lastItem,landingItem);
	//		ElapsedTime nextTime = lastItem.GetTime() + dT;		
	//		landingItem.SetTime(nextTime);
	//		lastItem = landingItem;
	//		newClearance.AddItem(lastItem);
	//		return true;		
	//	}
	//}
	//
	////go around 
	//CFinalApproachSegInSim *pFinalApproachSegInSim = m_pAirspaceRes->GetFinalApproachRouteSeg(pWayPoint, pRunwayPortInSim);
	//ASSERT(pFinalApproachSegInSim);

	//return pFinalApproachSegInSim->FindClearanceInConcern(pFlight, lastItem, newClearance, m_pGoAroundCriteriaDataProcess);
	return false;
	
}

void CFinalApproachController::Init( int nPrjID, AirportResourceManager * pAirportRes, AirRouteNetworkInSim* pAirspaceRes )
{
	m_pAirportRes = pAirportRes;
	m_pAirspaceRes = pAirspaceRes;

	m_pGoAroundCriteriaDataProcess = new CGoAroundCriteriaDataProcess(nPrjID);
	m_pGoAroundCriteriaDataProcess->ReadData(nPrjID);

	m_pCtrlInterventionSpec = new CCtrlInterventionSpecDataList;
	m_pCtrlInterventionSpec->ReadData(nPrjID);
	m_SideStepIntervention.Init(nPrjID,pAirportRes);
}

//CFinalApproachSegInSim* CFinalApproachController::GetFinalApproachSeg( AirsideFlightInSim * pFlight, ClearanceItem& lastItem )
//{
//	AirsideResource * plastRes = lastItem.GetResource();
//	if(plastRes && plastRes->GetType() == AirsideResource::ResType_WayPoint)
//	{
//		AirRoutePointInSim * lastWayPoint = (AirRoutePointInSim*) plastRes;
//		LogicRunwayInSim * pLandRunway = pFlight->GetLandingRunway();
//		return m_pAirspaceRes->GetFinalApproachRouteSeg(lastWayPoint,pLandRunway);
//	}
//
//	return NULL;
//}


bool CFinalApproachController::IsNeedToSideStep( AirsideFlightInSim * pFlight, ClearanceItem& lastItem ) const
{
	LogicRunwayInSim * pLandRunway = pFlight->GetLandingRunway();
	if(pLandRunway->GetLastInResourceFlight())
	{
		return m_pCtrlInterventionSpec->IsSideStepPreGoAround(pLandRunway->GetRunwayInSim()->GetRunwayInput()->getID(),pLandRunway->getLogicRunwayType());
	}
	return false;
}
