#include "StdAfx.h"
#include ".\approachseparationinsim.h"
#include "RunwayInSim.h"
#include "AirsideFlightInSim.h"
#include "../../InputAirside/ApproachSeparationCirteria.h"
#include "AircraftClassificationManager.h"
#include "FlightPerformanceManager.h"
#include "../../Common/ARCUnit.h"
#include "../../Common/EngineDiagnoseEx.h"
#include "../../Common/ARCExceptionEx.h"
#include "AirspaceResource.h"
#include "../../Common/Point2008.h"
#include "ConflictConcernIntersectionInAirspace.h"
#include "AirspaceResource.h"
#include "../../Common/Point2008.h"
#include "ConflictConcernIntersectionInAirspace.h"
#include "../../InputAirside/RunwaySeparationRelation.h"
#include "AirsideResourceManager.h"
CApproachSeparationInSim::CApproachSeparationInSim(void)
{
	m_pApproachSeparation = NULL;
	m_pAircraftClassificationManager = NULL;
}

CApproachSeparationInSim::~CApproachSeparationInSim(void)
{
	if (NULL != m_pApproachSeparation)
	{
		delete m_pApproachSeparation;
		m_pApproachSeparation = NULL;
	}
}

void CApproachSeparationInSim::Init(AircraftClassificationManager* pAircraftClassificationManager, int nProjID, const ALTObjectList& vRunways)
{
	ASSERT(NULL != pAircraftClassificationManager);
	m_pAircraftClassificationManager = pAircraftClassificationManager;

	if (NULL == m_pApproachSeparation)
	{
		m_pApproachSeparation = new CApproachSeparationCriteria(nProjID,vRunways);

	}
	m_pApproachSeparation->ReadData();
}

SEPSTD CApproachSeparationInSim::GetApproachSeparationStandard(AirsideFlightInSim* pTrailFlight, AirsideFlightInSim* pLeadFlight)
{
	ASSERT(NULL != pTrailFlight);
	ASSERT(NULL != pLeadFlight);
	if(!pTrailFlight || !pLeadFlight )
	{
		return SEPSTD_NONE;
	}

	AirsideFlightState trailFlightState = pTrailFlight->GetCurState();
	AirsideMobileElementMode trailFlightCurMode = trailFlightState.m_fltMode;
	LogicRunwayInSim* pTrailFlightRunwayInSim = pTrailFlight->GetLandingRunway();

	//if the trail flight isn't approach flight
	if (trailFlightCurMode > OnExitRunway)
	{
		return SEPSTD_NONE;
		ASSERT(FALSE);
	}

	AirsideFlightState leadFlightState = pLeadFlight->GetCurState();
	AirsideMobileElementMode leadFlightCurMode = leadFlightState.m_fltMode;
	LogicRunwayInSim* pLeadFlightRunwayInSim = NULL;

	AIRCRAFTOPERATIONTYPE aircraftOperationType = AIRCRAFTOPERATION_TYPE_NONE;
	//landing
	if (leadFlightCurMode <= OnExitRunway)
	{
		pLeadFlightRunwayInSim = pLeadFlight->GetLandingRunway();
		aircraftOperationType = AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED;
	}
	//takeoff
	else if (OnTaxiToRunway <= leadFlightCurMode )
	{
		pLeadFlightRunwayInSim = pLeadFlight->GetAndAssignTakeoffRunway();
		aircraftOperationType = AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF;
	}
	else
	{
		return SEPSTD_NONE;
		ASSERT(FALSE);
	}

	CApproachTypeList* pApproachTypeList = NULL;

	//landing behind land
	if (aircraftOperationType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED)
	{
		pApproachTypeList = m_pApproachSeparation->GetLindingBehindLandedList();
	}
	//landing behind takeoff
	else if (aircraftOperationType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF)
	{
		pApproachTypeList = m_pApproachSeparation->GetLindingBehindTakeOffList();
	}
	//false
	else
	{
		ASSERT(FALSE);
		return SEPSTD_NONE;
	}
	ASSERT(NULL != pApproachTypeList);
	if(!pApproachTypeList || !pTrailFlightRunwayInSim || !pLeadFlightRunwayInSim)
		return SEPSTD_NONE;

	int nTrailRwyID = pTrailFlightRunwayInSim->GetRunwayInSim()->GetRunwayID();
	RUNWAY_MARK trailRwyMark = pTrailFlightRunwayInSim->getLogicRunwayType();

	int nLeadRwyID = pLeadFlightRunwayInSim->GetRunwayInSim()->GetRunwayID();
	RUNWAY_MARK leadRwyMark = pLeadFlightRunwayInSim->getLogicRunwayType();


	CApproachRunwayNode* pApproachRunwayNode = pApproachTypeList->GetNode(nTrailRwyID, trailRwyMark, nLeadRwyID, leadRwyMark);

	//set default value
	if (NULL == pApproachRunwayNode)
	{
		return SEPSTD_NONE;
	}
	else
	{
		return pApproachRunwayNode->m_emSepStd;
	}

	return SEPSTD_NONE;
}

BOOL CApproachSeparationInSim::IsTakeTheLAHSO(AirsideFlightInSim* pTrailFlight, AirsideFlightInSim* pLeadFlight)
{
	ASSERT(NULL != pTrailFlight);
	ASSERT(NULL != pLeadFlight);

	if(!pTrailFlight || !pLeadFlight)
		return FALSE;

	AirsideFlightState trailFlightState = pTrailFlight->GetCurState();
	AirsideMobileElementMode trailFlightCurMode = trailFlightState.m_fltMode;
	LogicRunwayInSim* pTrailFlightRunwayInSim = pTrailFlight->GetLandingRunway();

	//if the trail flight isn't approach flight
	if (trailFlightCurMode > OnExitRunway)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	AirsideFlightState leadFlightState = pLeadFlight->GetCurState();
	AirsideMobileElementMode leadFlightCurMode = leadFlightState.m_fltMode;
	LogicRunwayInSim* pLeadFlightRunwayInSim = NULL;

	AIRCRAFTOPERATIONTYPE aircraftOperationType = AIRCRAFTOPERATION_TYPE_NONE;
	//landing
	if (leadFlightCurMode <= OnExitRunway)
	{
		pLeadFlightRunwayInSim = pLeadFlight->GetLandingRunway();
		aircraftOperationType = AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED;
	}
	//takeoff
	else if (OnTaxiToRunway <= leadFlightCurMode )
	{
		pLeadFlightRunwayInSim = pLeadFlight->GetAndAssignTakeoffRunway();
		aircraftOperationType = AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF;
	}
	else
	{
		ASSERT(FALSE);
		return FALSE;
	}


	CApproachTypeList* pApproachTypeList = NULL;

	//landing behind land
	if (aircraftOperationType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED)
	{
		pApproachTypeList = m_pApproachSeparation->GetLindingBehindLandedList();
	}
	//landing behind takeoff
	else if (aircraftOperationType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF)
	{
		pApproachTypeList = m_pApproachSeparation->GetLindingBehindTakeOffList();
	}
	//false
	else
	{
		ASSERT(FALSE);
		return FALSE;
	}
	ASSERT(NULL != pApproachTypeList);
	
	if(!pApproachTypeList || !pTrailFlightRunwayInSim || !pLeadFlightRunwayInSim)
		return FALSE;

	int nTrailRwyID = pTrailFlightRunwayInSim->GetRunwayInSim()->GetRunwayID();
	RUNWAY_MARK trailRwyMark = pTrailFlightRunwayInSim->getLogicRunwayType();

	int nLeadRwyID = pLeadFlightRunwayInSim->GetRunwayInSim()->GetRunwayID();
	RUNWAY_MARK leadRwyMark = pLeadFlightRunwayInSim->getLogicRunwayType();

	CApproachRunwayNode* pApproachRunwayNode = pApproachTypeList->GetNode(nTrailRwyID, trailRwyMark, nLeadRwyID, leadRwyMark);

	//set default value
	if (NULL == pApproachRunwayNode)
	{
		return FALSE;
	}
	else
	{
		return pApproachRunwayNode->m_bLAHSO;
	}

	return FALSE;
}

BOOL CApproachSeparationInSim::IsTakeTheLAHSO(LogicRunwayInSim* pLandRunway, LogicRunwayInSim* pLandOrTakeoffRunway, AIRCRAFTOPERATIONTYPE aircraftOperationType)
{
	ASSERT(NULL != pLandRunway);
	ASSERT(NULL != pLandOrTakeoffRunway);

	CApproachTypeList* pApproachTypeList = NULL;

	//landing behind land
	if (aircraftOperationType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED)
	{
		pApproachTypeList = m_pApproachSeparation->GetLindingBehindLandedList();
	}
	//landing behind takeoff
	else if (aircraftOperationType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF)
	{
		pApproachTypeList = m_pApproachSeparation->GetLindingBehindTakeOffList();
	}
	//false
	else
	{
		ASSERT(FALSE);
		return FALSE;
	}
	ASSERT(NULL != pApproachTypeList);
	if(!pApproachTypeList || !pLandRunway || !pLandOrTakeoffRunway)
		return FALSE;

	int nTrailRwyID = pLandRunway->GetRunwayInSim()->GetRunwayID();
	RUNWAY_MARK trailRwyMark = pLandRunway->getLogicRunwayType();

	int nLeadRwyID = pLandOrTakeoffRunway->GetRunwayInSim()->GetRunwayID();
	RUNWAY_MARK leadRwyMark = pLandOrTakeoffRunway->getLogicRunwayType();

	CApproachRunwayNode* pApproachRunwayNode = pApproachTypeList->GetNode(nTrailRwyID, trailRwyMark, nLeadRwyID, leadRwyMark);

	//set default value
	if (NULL == pApproachRunwayNode)
	{
		return FALSE;
	}
	else
	{
		return pApproachRunwayNode->m_bLAHSO;
	}

	return FALSE;
}

//Called the GetAircraftClass() function but not catch the exception, throw AirsideDiagnose exception
void CApproachSeparationInSim::GetApproachSeparationTime(AirsideFlightInSim* pFlight,
														AirsideFlightInSim* pFrontFlight,const AirsideMobileElementMode& frontFlightMode, 
														double& dMinTime)
{
	double dMinDistance = 4;
	dMinTime = 90;

	ASSERT(NULL != pFlight);
	ASSERT(NULL != pFrontFlight);

	if (pFrontFlight == NULL || pFlight == NULL)
	{
		dMinTime = 0;
		return;
	}

	AIRCRAFTOPERATIONTYPE aircraftOperationType = AIRCRAFTOPERATION_TYPE_NONE;
	//if the trail flight isn't approach flight
	if (frontFlightMode == OnLanding)
	{

		aircraftOperationType = AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED;
	}
	else if(frontFlightMode == OnTakeoff)
	{
		aircraftOperationType = AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF;
	}
	else
	{
		TRACE("exception,  void CApproachSeparationInSim::GetApproachSeparationTime 1");
		return;

	}

	CApproachTypeList* pApproachTypeList = NULL;

	LogicRunwayInSim* pFrontFlightRunwayInSim = NULL;

	//landing behind land
	if (aircraftOperationType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED)
	{
		pApproachTypeList = m_pApproachSeparation->GetLindingBehindLandedList();

		pFrontFlightRunwayInSim = pFrontFlight->GetLandingRunway();
	}
	//landing behind takeoff
	else if (aircraftOperationType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF)
	{
		pApproachTypeList = m_pApproachSeparation->GetLindingBehindTakeOffList();

		pFrontFlightRunwayInSim = pFrontFlight->GetAndAssignTakeoffRunway();
	}
	else	//error
	{
		TRACE("exception,  void CApproachSeparationInSim::GetApproachSeparationTime 2");
		return;
	}

	LogicRunwayInSim* pFlightRunwayInSim = pFlight->GetLandingRunway();

	ASSERT(NULL != pApproachTypeList);
	if(!pApproachTypeList)
		return;

	int nTrailRwyID = pFlightRunwayInSim->GetRunwayInSim()->GetRunwayID();
	RUNWAY_MARK trailRwyMark = pFlightRunwayInSim->getLogicRunwayType();

	int nLeadRwyID = pFrontFlightRunwayInSim->GetRunwayInSim()->GetRunwayID();
	RUNWAY_MARK leadRwyMark = pFrontFlightRunwayInSim->getLogicRunwayType();

	CApproachRunwayNode* pApproachRunwayNode = pApproachTypeList->GetNode(nTrailRwyID, trailRwyMark, nLeadRwyID, leadRwyMark);



	//set default value
	if ( pApproachRunwayNode)
	{
		//trail flight item
		CString strTrailACType;
		pFlight->getACType(strTrailACType.GetBuffer(1024));
		strTrailACType.ReleaseBuffer();

		AircraftClassificationItem* trailFlightItem = NULL;
		AircraftClassificationItem* leadFlightItem  = NULL; 
		try
		{
			if( pApproachRunwayNode->m_emClassType == ApproachSpeedBased)
			{
				double dSpeedInKnots = pFlight->GetPerformance()->getAvgApproachSpeedInKnots(pFlight);
				double dSpeedInMpS =  ARCUnit::ConvertVelocity(dSpeedInKnots,ARCUnit::KNOT,ARCUnit::MpS);
				trailFlightItem = m_pAircraftClassificationManager->GetApproachSpeedClass(dSpeedInMpS);
			}
			else
				trailFlightItem = m_pAircraftClassificationManager->GetAircraftClass(strTrailACType, pApproachRunwayNode->m_emClassType);
			
			ASSERT(NULL != trailFlightItem);

			//lead flight item
			CString strLeadACType;
			pFrontFlight->getACType(strLeadACType.GetBuffer(1024));
			strLeadACType.ReleaseBuffer();	
			if(pApproachRunwayNode->m_emClassType == ApproachSpeedBased)
			{
				double dSpeedInKnots = pFrontFlight->GetPerformance()->getAvgApproachSpeedInKnots(pFrontFlight);
				double dSpeedInMpS =  ARCUnit::ConvertVelocity(dSpeedInKnots,ARCUnit::KNOT,ARCUnit::MpS);
				leadFlightItem = m_pAircraftClassificationManager->GetApproachSpeedClass(dSpeedInMpS);
			}
			else
				leadFlightItem = m_pAircraftClassificationManager->GetAircraftClass(strLeadACType, pApproachRunwayNode->m_emClassType);
			
			ASSERT(NULL != leadFlightItem);
		}
		catch (AirsideACTypeMatchError* pError)
		{
			AirsideDiagnose *pDialognose = (AirsideDiagnose *)pError->GetDiagnose();
			CString strErrorMsg = pDialognose->GetDetails();
			CString strDetails;
			CString strFlightID;
			pFlight->getFullID(strFlightID.GetBuffer(1024));

			CString strFrontID;
			pFrontFlight->getFullID(strFrontID.GetBuffer(1024));

			strDetails.Format(_T("Can't calculate approach separation between flight %s and %s, Detail:%s"),strFlightID,strFrontID,strErrorMsg);
			pDialognose->SetDetails(strDetails);

			throw pError;
		}


		CApproachSeparationItem* pApproachSeparationItem = NULL;
		if (leadFlightItem != NULL&& trailFlightItem != NULL)
			pApproachSeparationItem =  pApproachRunwayNode->GetItem(trailFlightItem, leadFlightItem);
		


		//set default value
		if (pApproachSeparationItem)
		{
			dMinDistance = pApproachSeparationItem->m_dMinDistance ;
			//if(pApproachSeparationItem->m_pDisDistribution)
			//	dMinDistance += pApproachSeparationItem->m_pDisDistribution->getRandomValue();
			
			dMinTime = pApproachSeparationItem->m_dMinTime;
			//if (pApproachSeparationItem->m_pTimeDistribution)
			//	dMinTime += pApproachSeparationItem->m_pTimeDistribution->getRandomValue();
		}
	}
	
	//convert distance separation to time
	if (aircraftOperationType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED)
	{	
		//double dTrailSpeed = pFlight->GetPerformance()->getApproachSpeedHorizen(pFlight);
		dMinDistance = ARCUnit::ConvertLength(dMinDistance,ARCUnit::NM,ARCUnit::CM);

		FlightRouteInSim* pFlightRoute = NULL;
		if (pFlight->IsCircuitFlight())
		{
			AirsideCircuitFlightInSim* pCirFlight = (AirsideCircuitFlightInSim*)pFlight;
			if (pCirFlight->IntensionOn())
			{
				pFlightRoute = pFlight->GetLandingCircuit();
			}
			else
			{
				pFlightRoute = pFlight->GetSTAR();
			}
		}
		else
		{
			pFlightRoute = pFlight->GetSTAR();
		}

		AirspaceIntrailSeparationSpeed sepSpd((AirsideResource*)pFrontFlightRunwayInSim,pFlightRoute);
		double dSpeed = sepSpd.GetSparationSpeed(pFlight,dMinDistance);
		double dTimeSepDis = 0.0;
		if (dSpeed > (std::numeric_limits<double>::min)())
		{
			dTimeSepDis = dMinDistance / dSpeed;
		}

		if(dMinTime < dTimeSepDis)
			dMinTime = dTimeSepDis;
		
	}
	else
	{
		double dTrailSpeed = (pFrontFlight->GetPerformance()->getLiftoffSpeed(pFrontFlight) + pFlight->GetPerformance()->getLandingSpeed(pFlight))/2.0;
		dMinDistance = ARCUnit::ConvertLength(dMinDistance,ARCUnit::NM,ARCUnit::CM);		
		if (pFrontFlight->GetAndAssignTakeoffRunway() == pFlightRunwayInSim)
		{
			dTrailSpeed = pFlight->GetPerformance()->getLandingSpeed(pFlight); 		
			dMinDistance+= pFlight->GetTouchDownDistAtRuwnay(pFlightRunwayInSim);//pFlight->GetPerformance()->getDistThresToTouch(pFlight);
		}
		double dTimeSepDis = dMinDistance/dTrailSpeed;

		if(dMinTime < dTimeSepDis)
			dMinTime = dTimeSepDis;
		dMinTime += pFrontFlight->GetTakeoffPositionTime();
	}


}

void CApproachSeparationInSim::GetRelatedRunwayNotConcern(LogicRunwayInSim *pCurPort,
									std::vector<int > vConcernRunway,
									std::vector<std::pair<int,RUNWAY_MARK> >& vNotConcernLandRunway,
									std::vector<std::pair<int,RUNWAY_MARK> >& vNotConcernTakeOffRunway)
{

	int nRwyID = pCurPort->GetRunwayInSim()->GetRunwayID();
	RUNWAY_MARK rwyMark = pCurPort->getLogicRunwayType();
	//landing behind land
	//if (aircraftOperationType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED)
	{	
		CApproachTypeList* pApproachTypeList = NULL;
		pApproachTypeList = m_pApproachSeparation->GetLindingBehindLandedList();



		pApproachTypeList->GetRelatedLogicRunway(nRwyID, rwyMark, vConcernRunway,vNotConcernLandRunway);

	}
	//landing behind takeoff
	//else if (aircraftOperationType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF)

	{
		CApproachTypeList* pApproachTypeList = NULL;
		pApproachTypeList = m_pApproachSeparation->GetLindingBehindTakeOffList();
		pApproachTypeList->GetRelatedLogicRunway(nRwyID, rwyMark, vConcernRunway,vNotConcernTakeOffRunway);

	}
}

double CApproachSeparationInSim::GetTimeAccordSeparationDist(AirsideFlightInSim* pFlight, AirsideFlightInSim* pFrontFlight, double dMinDist)
{
	LogicRunwayInSim* pLogicRunway = pFlight->GetLandingRunway();
	FlightRouteInSim* pStar = pFrontFlight->GetSTAR();

	AirRouteSegInSim* pFinalSeg = (AirRouteSegInSim*)pStar->GetItem(pStar->GetItemCount()-2);

	if (pFinalSeg)
	{
		double dAppSped = pFinalSeg->GetFirstConcernIntersection()->getInputPoint()->GetOccupyInstance(pFrontFlight).GetSpeed();
		return dMinDist/dAppSped;		
	}

	return 0;
}

void CApproachSeparationInSim::GetRelatedRunwayListByACOpType( const std::vector<LogicRunwayInSim*>& vRunways, AIRCRAFTOPERATIONTYPE AcOpType, std::vector<std::pair<LogicRunwayInSim*,LogicRunwayInSim*> >& vRelatedRunways  )
{
	if (m_pApproachSeparation == NULL)
		return;

	size_t nRwyCount = vRunways.size();

	if (AcOpType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED)
	{
		size_t nSize = m_pApproachSeparation->GetLindingBehindLandedList()->m_vrNode.size();
		for (size_t i =0; i < nSize; i++)
		{
			CApproachRunwayNode* pRwyNode = m_pApproachSeparation->GetLindingBehindLandedList()->m_vrNode.at(i);

			int nRwy1 = pRwyNode->m_nFirstRunwayID;
			int nRwy2 = pRwyNode->m_nSecondRunwayID;

			if (nRwy1 == nRwy2)
				continue;

			if (pRwyNode->m_emART == RRT_SAMERUNWAY)
				continue;

			RUNWAY_MARK eRwyMark1 = pRwyNode->m_emFirstRunwayMark;
			RUNWAY_MARK eRwyMark2 = pRwyNode->m_emSecondRunwayMark;

			LogicRunwayInSim* pRwy1 = NULL;
			LogicRunwayInSim* pRwy2 = NULL;

			for(size_t j =0; j < nRwyCount; j++)
			{
				LogicRunwayInSim* pRwy = vRunways.at(j);
				if (pRwy->GetRunwayInSim()->GetRunwayID() == nRwy1 && pRwy->getLogicRunwayType() == eRwyMark1)
					pRwy1 = pRwy;

				if (pRwy->GetRunwayInSim()->GetRunwayID() == nRwy2 && pRwy->getLogicRunwayType() == eRwyMark2)
					pRwy2 = pRwy;

				if (pRwy1 != NULL && pRwy2 != NULL)
					break;
			}

			if (pRwy1 != NULL && pRwy2 != NULL)
				vRelatedRunways.push_back(std::make_pair(pRwy1, pRwy2));

		}

	}
	else
	{
		size_t nSize = m_pApproachSeparation->GetLindingBehindTakeOffList()->m_vrNode.size();
		for (size_t i =0; i < nSize; i++)
		{
			CApproachRunwayNode* pRwyNode = m_pApproachSeparation->GetLindingBehindTakeOffList()->m_vrNode.at(i);

			if (pRwyNode->m_emAIT == RIT_NONE || pRwyNode->m_emART == RRT_INTERSECT || pRwyNode->m_emART == RRT_SAMERUNWAY)
				continue;

			int nRwy1 = pRwyNode->m_nFirstRunwayID;
			int nRwy2 = pRwyNode->m_nSecondRunwayID;

			if (nRwy1 == nRwy2)
				continue;

			RUNWAY_MARK eRwyMark1 = pRwyNode->m_emFirstRunwayMark;
			RUNWAY_MARK eRwyMark2 = pRwyNode->m_emSecondRunwayMark;

			LogicRunwayInSim* pRwy1 = NULL;
			LogicRunwayInSim* pRwy2 = NULL;

			for(size_t j =0; j < nRwyCount; j++)
			{
				LogicRunwayInSim* pRwy = vRunways.at(j);
				if (pRwy->GetRunwayInSim()->GetRunwayID() == nRwy1 && pRwy->getLogicRunwayType() == eRwyMark1)
					pRwy1 = pRwy;

				if (pRwy->GetRunwayInSim()->GetRunwayID() == nRwy2 && pRwy->getLogicRunwayType() == eRwyMark2)
					pRwy2 = pRwy;

				if (pRwy1 != NULL && pRwy2 != NULL)
					break;
			}

			if (pRwy1 != NULL && pRwy2 != NULL)
				vRelatedRunways.push_back(std::make_pair(pRwy1, pRwy2));

		}
	}



}

CRunwaySeparationRelation* CApproachSeparationInSim::GetApproachSepRwyRelation()
{
	return &m_pApproachSeparation->m_runwaySeparationRelation;
}
