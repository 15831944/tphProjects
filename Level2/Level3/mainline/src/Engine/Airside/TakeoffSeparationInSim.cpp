#include "StdAfx.h"
#include ".\takeoffseparationinsim.h"
#include "AircraftClassificationManager.h"
#include "../../InputAirside/TakeoffClearanceCriteria.h"
#include "AirsideFlightInSim.h"
#include "RunwayInSim.h"
#include "FlightPerformanceManager.h"
#include "../../Common/ARCUnit.h"
#include "../../Common/EngineDiagnoseEx.h"
#include "../../Common/ARCExceptionEx.h"
#include "../../InputAirside/RunwaySeparationRelation.h"
#include "FlightPerformancesInSim.h"

CTakeoffSeparationInSim::CTakeoffSeparationInSim(void)
{
	m_pAircraftClassificationManager = NULL;
	m_pTakeoffClearanceCriteria = NULL;
}

CTakeoffSeparationInSim::~CTakeoffSeparationInSim(void)
{
	if (NULL != m_pTakeoffClearanceCriteria)
	{
		delete m_pTakeoffClearanceCriteria;
		m_pTakeoffClearanceCriteria = NULL;
	}
}

void CTakeoffSeparationInSim::Init(AircraftClassificationManager* pAircraftClassificationManager, int nProjID,const ALTObjectList& vRunways)
{
	m_pAircraftClassificationManager = pAircraftClassificationManager;

	if (NULL == m_pTakeoffClearanceCriteria)
	{
		m_pTakeoffClearanceCriteria = new CTakeoffClearanceCriteria(nProjID, vRunways);
	}
	m_pTakeoffClearanceCriteria->ReadData();
}

//takeoff clearance separation
//bIsTimeOrDis=TRUE, estSepTime is valid, bIsTimeOrDis=FALSE, dSepDistance is valid
//		\\    
//       \\landing flight
//        \\                              front flight
//==================================================================>//runway
//         ||
//		   ||take off flight
//		   ||
//
//calculate the separation between take off flight and front flight
void CTakeoffSeparationInSim::GetTakeoffClearToPositionSeparationTimeBehindLanding(AirsideFlightInSim* pTakeoffFlight, AirsideFlightInSim* pFrontFlight,ElapsedTime& estSepTime)
{
	BOOL bIsTimeOrDis = false;
	double dSepDistance = 0.0;

	ASSERT(NULL != pTakeoffFlight);
	ASSERT(NULL != pFrontFlight);

	LogicRunwayInSim* pRunwayInSim = pTakeoffFlight->GetAndAssignTakeoffRunway();
	ASSERT(NULL != pRunwayInSim);
	

	int nRwyID = pRunwayInSim->GetRunwayInSim()->GetRunwayID();
	RUNWAY_MARK rwyMark = pRunwayInSim->getLogicRunwayType();

	if (pFrontFlight->GetLandingRunway() != pTakeoffFlight->GetAndAssignTakeoffRunway() && pFrontFlight->GetLandingRunway() != pTakeoffFlight->GetAndAssignTakeoffRunway()->GetOtherPort())
	{
		estSepTime = 0L;
		return;
	}
	//takeoff behind landing
	bIsTimeOrDis = m_pTakeoffClearanceCriteria->IsTimeOrDistance(nRwyID, rwyMark, AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED);

	//time
	if (bIsTimeOrDis)
	{
		estSepTime = m_pTakeoffClearanceCriteria->GetTakeoffSeparationTime(nRwyID, rwyMark, AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED);
		dSepDistance = 0;
	}
	//distance
	else
	{
		dSepDistance = m_pTakeoffClearanceCriteria->GetTakeoffSeparationDistance(nRwyID, rwyMark, AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED);
		dSepDistance = ARCUnit::ConvertLength(dSepDistance,ARCUnit::FEET,ARCUnit::CM);
		double dLeadSpeed = (pFrontFlight->GetPerformance()->getLandingSpeed(pFrontFlight) + pFrontFlight->GetPerformance()->getExitSpeed(pFrontFlight))/2;
		estSepTime = ElapsedTime(dSepDistance/dLeadSpeed);
	}
	
}

//throw AirsideACTypeMatchError exception
void CTakeoffSeparationInSim::GetTakeoffSeparationTime(AirsideFlightInSim* pFlight,AirsideFlightInSim* pFrontFlight,const AirsideMobileElementMode& frontFlightMode,
																			 double& minTime)
{
	//ASSERT(NULL != pTrailFlight);
	//ASSERT(NULL != pLeadFlight);

	minTime = 60;
	double minDistance = 1;

	if(!pFlight || !pFrontFlight )
	{
		minTime = 0;
		minDistance = 0;
		return;
	}

	LogicRunwayInSim* pTrailFlightRunwayInSim = pFlight->GetAndAssignTakeoffRunway();
	ASSERT(NULL != pTrailFlightRunwayInSim);

	//get lead flight state, takeoff or landing
	AIRCRAFTOPERATIONTYPE aircraftOperationType = AIRCRAFTOPERATION_TYPE_NONE;
	LogicRunwayInSim* pLeadFlightRunwayInSim = NULL;
	//landing
	if (frontFlightMode == OnLanding)
	{
		pLeadFlightRunwayInSim = pFrontFlight->GetLandingRunway();
		aircraftOperationType = AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED;
	}
	//takeoff
	else if (frontFlightMode == OnTakeoff )
	{
		pLeadFlightRunwayInSim = pFrontFlight->GetAndAssignTakeoffRunway();
		aircraftOperationType = AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF;
	}
	else
	{
		TRACE("exception: void CTakeoffSeparationInSim::GetTakeoffSeparationTime");
		return;
	}

	int nTrailRwyID = pTrailFlightRunwayInSim->GetRunwayInSim()->GetRunwayID();
	RUNWAY_MARK trailRwyMark = pTrailFlightRunwayInSim->getLogicRunwayType();

	int nLeadRwyID = pLeadFlightRunwayInSim->GetRunwayInSim()->GetRunwayID();
	RUNWAY_MARK leadRwyMark = pLeadFlightRunwayInSim->getLogicRunwayType();

	CTakeoffClaaranceRunwayNode* pTakeoffClearanceRunwayNode = m_pTakeoffClearanceCriteria->GetTakeoffClearanceRunwayNode(nTrailRwyID, trailRwyMark, nLeadRwyID, leadRwyMark, aircraftOperationType);
	//set default value
	if (pTakeoffClearanceRunwayNode != NULL )
	{
		AircraftClassificationItem* leadFlightItem = NULL;
		AircraftClassificationItem* trailFlightItem = NULL;
		try
		{
			CString strTrailACType;
			pFlight->getACType(strTrailACType.GetBuffer(1024));
			strTrailACType.ReleaseBuffer();

			//trail flight item
			if(pTakeoffClearanceRunwayNode->m_emClassType == ApproachSpeedBased)
			{
				double dSpeedInKnots = pFlight->GetPerformance()->getAvgApproachSpeedInKnots(pFlight);
				double dSpeedInMpS =  ARCUnit::ConvertVelocity(dSpeedInKnots,ARCUnit::KNOT,ARCUnit::MpS);
				trailFlightItem = m_pAircraftClassificationManager->GetApproachSpeedClass(dSpeedInMpS);
			}
			else
				trailFlightItem = m_pAircraftClassificationManager->GetAircraftClass(strTrailACType, pTakeoffClearanceRunwayNode->m_emClassType);

			CString strLeadACType;
			pFrontFlight->getACType(strLeadACType.GetBuffer(1024));
			strLeadACType.ReleaseBuffer();

			//lead flight item
			if(pTakeoffClearanceRunwayNode->m_emClassType == ApproachSpeedBased)
			{
				double dSpeedInKnots = pFrontFlight->GetPerformance()->getAvgApproachSpeedInKnots(pFrontFlight);
				double dSpeedInMpS =  ARCUnit::ConvertVelocity(dSpeedInKnots,ARCUnit::KNOT,ARCUnit::MpS);
				leadFlightItem = m_pAircraftClassificationManager->GetApproachSpeedClass(dSpeedInMpS);
			}
			else
				leadFlightItem = m_pAircraftClassificationManager->GetAircraftClass(strLeadACType, pTakeoffClearanceRunwayNode->m_emClassType);

		}
		catch(AirsideACTypeMatchError* pError)
		{

			AirsideDiagnose *pDiagnose = (AirsideDiagnose *)pError->GetDiagnose();
			CString strErrorMsg = pDiagnose->GetDetails();
			CString strDetails;
			CString strFlightID;
			pFlight->getFullID(strFlightID.GetBuffer(1024));

			CString strFrontID;
			pFrontFlight->getFullID(strFrontID.GetBuffer(1024));

			strDetails.Format(_T("Can't claculate takeoff speration between flight %s and %s, Detail:%s"),strFlightID,strFrontID,strErrorMsg);
			pDiagnose->SetDetails(strDetails);

			throw pError;
		}


		CTakeoffClaaranceSeparationItem* pTakeoffClearanceSeparationItem = NULL;
		if(trailFlightItem != NULL && leadFlightItem != NULL)
			pTakeoffClearanceSeparationItem = pTakeoffClearanceRunwayNode->GetItem(trailFlightItem, leadFlightItem);

		//set default value
		if (pTakeoffClearanceSeparationItem != NULL )
		{
			minTime = pTakeoffClearanceSeparationItem->m_dMinTime;
			//if(pTakeoffClearanceSeparationItem->m_pTimeDistribution)
			//	minTime  += pTakeoffClearanceSeparationItem->m_pTimeDistribution->getRandomValue();
			
			minDistance = pTakeoffClearanceSeparationItem->m_dMinDistance;
			//if (pTakeoffClearanceSeparationItem->m_pDisDistribution)
			//	minDistance += pTakeoffClearanceSeparationItem->m_pDisDistribution->getRandomValue();

			double dLeadSpeed = pFrontFlight->GetPerformance()->getClimboutSpeed(pFlight);
			double dTempminDistance = ARCUnit::ConvertLength(minDistance,ARCUnit::NM,ARCUnit::CM);
			double dLeadTime = dTempminDistance/dLeadSpeed;
			if (dLeadTime >minTime)
				minTime = dLeadTime;
		}
	}

	//distance to time

	if (pTrailFlightRunwayInSim == pLeadFlightRunwayInSim || pTrailFlightRunwayInSim == pLeadFlightRunwayInSim->GetOtherPort())
	{
		if(aircraftOperationType == AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED)
		{
			double dLeadSpeed =(pFrontFlight->GetPerformance()->getLandingSpeed(pFrontFlight)+ pFlight->GetPerformance()->getClimboutSpeed(pFlight))/2.0;
			minDistance = ARCUnit::ConvertLength(minDistance,ARCUnit::NM,ARCUnit::CM);
			double dLeadTime = minDistance/dLeadSpeed;
			ASSERT(dLeadTime <= 120);
			//double dLeadTime = 0;
			//if (dLeadTime >minTime)
			//	minTime = dLeadTime;
		}
		else
		{
			double dLeadSpeed = pFrontFlight->GetPerformance()->getClimboutSpeed(pFlight);
			minDistance = ARCUnit::ConvertLength(minDistance,ARCUnit::NM,ARCUnit::CM);
			double dLeadTime = minDistance/dLeadSpeed;
			if (dLeadTime >minTime)
				minTime = dLeadTime;
		}
		
	}

	//ElapsedTime dClearTime ;
	//GetTakeoffClearToPositionSeparationTime(pFlight,pFrontFlight,frontFlightMode,dClearTime);

	//if (dClearTime > ElapsedTime(minTime))
	//{
	//	minTime = dClearTime;
	//}

}
void CTakeoffSeparationInSim::GetRelatedRunwayNotConcern(LogicRunwayInSim *pCurPort,
															  std::vector<int > vConcernRunway,
															  std::vector<std::pair<int,RUNWAY_MARK> >& vNotConcernLandRunway,
															  std::vector<std::pair<int,RUNWAY_MARK> >& vNotConcernTakeOffRunway)
{
	int nRwyID = pCurPort->GetRunwayInSim()->GetRunwayID();
	RUNWAY_MARK rwyMark = pCurPort->getLogicRunwayType();
	//landing behind land
	//if (aircraftOperationType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED)
	{	
		m_pTakeoffClearanceCriteria->GetRelatedLogicRunway(nRwyID, rwyMark, vConcernRunway,vNotConcernLandRunway,AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED);

	}
	//landing behind takeoff
	//else if (aircraftOperationType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF)

	{
		m_pTakeoffClearanceCriteria->GetRelatedLogicRunway(nRwyID, rwyMark,vConcernRunway,vNotConcernTakeOffRunway,AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF);
	}
}

//		\\    
//       \\landing flight
//        \\                              front flight
//==================================================================>//runway
//         ||
//		   ||take off flight
//		   ||
//
//calculate the separation between take off flight and landing flight
void CTakeoffSeparationInSim::GetLandingSeparationTimeOnSameRunway( AirsideFlightInSim* pTakeoffFlight, AirsideFlightInSim* pLandingFlight,const AirsideMobileElementMode& frontFlightMode, ElapsedTime& estSepTime )
{
	BOOL bIsTimeOrDis = false;
	double dSepDistance = 0.0;

	ASSERT(NULL != pTakeoffFlight);
	ASSERT(NULL != pLandingFlight);

	LogicRunwayInSim* pRunwayInSim = pTakeoffFlight->GetAndAssignTakeoffRunway();
	ASSERT(NULL != pRunwayInSim);

	int nRwyID = pRunwayInSim->GetRunwayInSim()->GetRunwayID();
	RUNWAY_MARK rwyMark = pRunwayInSim->getLogicRunwayType();
	//OccupancyInstance landingFlightOcy = pRunwayInSim->GetOccupyInstance(pLandingFlight);
	//if (landingFlightOcy.IsValid())
	{
	//	if (landingFlightOcy.IsExitTimeValid())
		{
			if (frontFlightMode == OnTakeoff)
			{
				//takeoff behind takeoff
				bIsTimeOrDis = m_pTakeoffClearanceCriteria->IsLandingTimeOrDistance(nRwyID, rwyMark, AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF);

				//time
				if (bIsTimeOrDis)
				{
					estSepTime = m_pTakeoffClearanceCriteria->GetLandingSeparationTime(nRwyID, rwyMark, AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF);
				}
				//distance
				else
				{
					dSepDistance = m_pTakeoffClearanceCriteria->GetLandingSeparationDistance(nRwyID, rwyMark, AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF);
					dSepDistance = ARCUnit::ConvertLength(dSepDistance,ARCUnit::FEET,ARCUnit::CM);
					double dLeadSpeed = pLandingFlight->GetPerformance()->getLandingSpeed(pLandingFlight);
					estSepTime = ElapsedTime(dSepDistance/dLeadSpeed);
				}
			}
			if (frontFlightMode == OnLanding)
			{
				//takeoff behind landing
				bIsTimeOrDis = m_pTakeoffClearanceCriteria->IsLandingTimeOrDistance(nRwyID, rwyMark, AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED);

				//time
				if (bIsTimeOrDis)
				{
					estSepTime = m_pTakeoffClearanceCriteria->GetLandingSeparationTime(nRwyID, rwyMark, AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED);
					dSepDistance = 0;
				}
				//distance
				else
				{
					dSepDistance = m_pTakeoffClearanceCriteria->GetLandingSeparationDistance(nRwyID, rwyMark, AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED);
					dSepDistance = ARCUnit::ConvertLength(dSepDistance,ARCUnit::NM,ARCUnit::CM);
					double dLeadSpeed = pLandingFlight->GetPerformance()->getLandingSpeed(pLandingFlight) ;
					estSepTime = ElapsedTime(dSepDistance/dLeadSpeed);
				}
			}
		}
	//	else
	//	{
	//		//ASSERT(FALSE);
	//	}
	}
	//else
	//{
	//	//ASSERT(FALSE);
	//}
}

void CTakeoffSeparationInSim::GetRelatedRunwayListByAcOpType( const std::vector<LogicRunwayInSim*>& vRunways, AIRCRAFTOPERATIONTYPE AcOpType,std::vector<std::pair<LogicRunwayInSim*,LogicRunwayInSim*> >& vRelatedRunways )
{
	if (m_pTakeoffClearanceCriteria == NULL)
		return;

	size_t nRwyCount = vRunways.size();

	if (AcOpType == AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED)
	{
		CTakeoffClaaranceTakeOffBehindLandedList* pTakeoffBehindLandedList = m_pTakeoffClearanceCriteria->GetTakeoffBehindLandingList();
		size_t nSize = pTakeoffBehindLandedList->m_vrNode.size() ;
		for (size_t i =0; i < nSize; i++)
		{
			CTakeoffClaaranceRunwayNode* pRwyNode = pTakeoffBehindLandedList->m_vrNode.at(i);

			if (pRwyNode->m_emRIT == RIT_NONE || pRwyNode->m_emRRT == RRT_INTERSECT || pRwyNode->m_emRRT == RRT_SAMERUNWAY)
				continue;

			int nRwy1 = pRwyNode->m_nFirstRunwayID;
			int nRwy2 = pRwyNode->m_nSecondRunwayID;

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
		size_t nSize = m_pTakeoffClearanceCriteria->GetTakeoffBehindTakeOffList()->m_vrNode.size();
		for (size_t i =0; i < nSize; i++)
		{
			CTakeoffClaaranceRunwayNode* pRwyNode = m_pTakeoffClearanceCriteria->GetTakeoffBehindTakeOffList()->m_vrNode.at(i);

			if (pRwyNode->m_emRIT == RIT_NONE || pRwyNode->m_emRRT == RRT_INTERSECT || pRwyNode->m_emRRT == RRT_SAMERUNWAY)
				continue;

			int nRwy1 = pRwyNode->m_nFirstRunwayID;
			int nRwy2 = pRwyNode->m_nSecondRunwayID;

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

CRunwaySeparationRelation* CTakeoffSeparationInSim::GetTakeSepRwyRelation()
{
	return &m_pTakeoffClearanceCriteria->m_runwaySeparationRelation;
}

void CTakeoffSeparationInSim::GetTakeoffClearToPositionSeparationTimeBehindTakeoff( AirsideFlightInSim* pTakeoffFlight, AirsideFlightInSim* pFrontFlight, ElapsedTime& estSepTime )
{
	
	double dSepDistance = 0.0;

	ASSERT(NULL != pTakeoffFlight);
	ASSERT(NULL != pFrontFlight);

	LogicRunwayInSim* pRunwayInSim = pTakeoffFlight->GetAndAssignTakeoffRunway();
	ASSERT(NULL != pRunwayInSim);
	
	int nRwyID = pRunwayInSim->GetRunwayInSim()->GetRunwayID();
	RUNWAY_MARK rwyMark = pRunwayInSim->getLogicRunwayType();
	
	//front flight on other runway
	if (pFrontFlight->GetAndAssignTakeoffRunway() != pTakeoffFlight->GetAndAssignTakeoffRunway() && pFrontFlight->GetAndAssignTakeoffRunway() != pTakeoffFlight->GetAndAssignTakeoffRunway()->GetOtherPort())
	{
		estSepTime = 0L;
		return;
	}

	//takeoff behind takeoff
	BOOL bIsTimeOrDis = m_pTakeoffClearanceCriteria->IsTimeOrDistance(nRwyID, rwyMark, AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF);

		//time
	if (bIsTimeOrDis){
		estSepTime = m_pTakeoffClearanceCriteria->GetTakeoffSeparationTime(nRwyID, rwyMark, AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF);
	}
	//distance
	else
	{
		DistanceUnit takeoffPosFront = pFrontFlight->GetTakeoffPosition()->GetEnterRunwayPos();
		DistanceUnit takeoffPos = pTakeoffFlight->GetTakeoffPosition()->GetEnterRunwayPos();

		dSepDistance = m_pTakeoffClearanceCriteria->GetTakeoffSeparationDistance(nRwyID, rwyMark, AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF);
		dSepDistance = ARCUnit::ConvertLength(dSepDistance,ARCUnit::FEET,ARCUnit::CM);
		dSepDistance += (takeoffPos-takeoffPosFront);

		double dRollSpeed = pFrontFlight->GetLiftoffSpeed()*0.5;
		double dTakeOffRollDist =  pFrontFlight->GetTakeoffRoll();
		//ASSUME THE flight always accelerate in takeoff
		double dt = sqrt(dSepDistance * dTakeOffRollDist)/dRollSpeed;
		estSepTime = ElapsedTime(dt);

		//if(dTakeOffRollDist > dSepDistance)
		//	estSepTime = ElapsedTime(dSepDistance/dRollSpeed);
		//else
		//{
		//	//roll time + lift off time
		//	estSepTime = ElapsedTime(dTakeOffRollDist/dRollSpeed) + ElapsedTime((dSepDistance - dTakeOffRollDist)/pFrontFlight->GetLiftoffSpeed());
		//}
	}
			
}

void CTakeoffSeparationInSim::GetTakeoffClearToPositionSeparationTime( AirsideFlightInSim* pTakeoffFlight, AirsideFlightInSim* pFrontFlight,const AirsideMobileElementMode& frontFlightMode, ElapsedTime& estSepTime )
{
	if(frontFlightMode == OnLanding){
		return GetTakeoffClearToPositionSeparationTimeBehindLanding(pTakeoffFlight,pFrontFlight,estSepTime);
	}
	if(frontFlightMode == OnTakeoff){
		return GetTakeoffClearToPositionSeparationTimeBehindTakeoff(pTakeoffFlight,pFrontFlight,estSepTime);
	}
	//ASSERT(FALSE);
}
