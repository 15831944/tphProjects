#include "StdAfx.h"
#include "FlightPlanInSim.h"
#include "AirsideResourceManager.h"
//#include "../../InputAirside/FlightPlan.h"
#include "../../Common/ARCUnit.h"
#include "AirspaceResource.h"
#include "AirRoutePointInSim.h"
#include "AirsideFlightInSim.h"
#include "../../Common/AirportDatabase.h"
#include "AirspaceResource.h"
#include "SimulationErrorShow.h"
#include "FlightPerformanceManager.h"
#include "FlightPlanGenericRouteInSim.h"
#include "FlightPlanSpecificRouteInSim.h"
#include "../../Common/AirportsManager.h"
#include "AirTrafficController.h"
#include "FlightPerformancesInSim.h"

void FlightPlanInSim::Init(int nPrjID,AirsideResourceManager* pResManager,CAirportDatabase* pAirportDB)
{
	m_pFlightPlansInput = new ns_FlightPlan::FlightPlans(nPrjID,pAirportDB);
	m_pFlightPlansInput->Read();
	m_pResManager = pResManager;
	m_pAirportDB = pAirportDB;
}
FlightPlanInSim::~FlightPlanInSim()
{
	delete m_pFlightPlansInput;
	m_pFlightPlansInput = NULL;
}

bool FlightPlanInSim::GetTakeoffCircuitFlightPlan(AirsideFlightInSim* pFlight, FlightRouteInSim*& pRoute)
{
	using namespace ns_FlightPlan;

	FlightPlanBase *pBasePlan = GetBestFitFlightPlan(pFlight, FlightPlanBase::Circuit );
	if(pBasePlan == NULL)//have no available flight plan setting
	{
		return false;
	}
	if(pBasePlan->GetProcfileType() == FlightPlanBase::Generic_Profile)
	{
		FlightPlanGeneric *pfltPlan = (FlightPlanGeneric *)pBasePlan;
		pRoute = new FlightPlanGenericRouteInSim(pfltPlan);

		CAirRoute *pTakeoffCircuitRoute = GetBestAirRoute(pFlight, CAirRoute::CIRCUIT, pFlight->GetAndAssignTakeoffRunway());

		if(pTakeoffCircuitRoute == NULL)//have no available SID
			return false;

		//update take off runway
		LogicRunwayInSim * pTakeOffRunway = pFlight->GetAndAssignTakeoffRunway();
		int nRwyID = -1;
		int nRwyMark = 0;
		if (pTakeOffRunway)
		{
			nRwyID = pTakeOffRunway->GetRunwayInSim()->GetRunwayInput()->getID();
			nRwyMark = pTakeOffRunway->getLogicRunwayType();
		}

		if(pTakeOffRunway == NULL ||
			!pTakeoffCircuitRoute->IsConnectRunway(nRwyID, nRwyMark))		//if no sid connected to runway,change another connected runway
		{
			int nRwy = pTakeoffCircuitRoute->GetConnectRunwayCount();
			for (int m =0; m < nRwy; m++)
			{
				LogicRunway_Pair logicrunway = pTakeoffCircuitRoute->getLogicRunwayByIdx(m);

				if (pFlight->GetAirTrafficController()->GetRunwayAssignmentStrategy()->IsDepartureRunway(pFlight,logicrunway.first,logicrunway.second))
				{
					pTakeOffRunway = m_pResManager->GetAirportResource()->getRunwayResource()->GetLogicRunway(logicrunway.first,(RUNWAY_MARK)logicrunway.second);
					if(pTakeOffRunway == NULL)
						continue;

					pFlight->SetTakeoffRunway(pTakeOffRunway);
					break;
				}
			}
		}
		ASSERT(pTakeOffRunway != NULL);
		if(pTakeOffRunway == NULL)
			return false;

		//get the SID from air route
		FlightPlanPropertiesInSim& planProperties = pRoute->GetPlanProperties();
		double ClimbSpeed = pFlight->GetPerformance()->getClimboutSpeed(pFlight) ;

		int nID = pTakeoffCircuitRoute->getID();
		pRoute->SetName(pTakeoffCircuitRoute->getName());
		AirRouteInSim* pRouteInSim = m_pResManager->GetAirspaceResource()->GetAirLaneInSimByID(nID);

		pRoute->SetAirRouteInSim(pRouteInSim);
		int nCount = pRouteInSim->GetRoutePointCount();
		for (int i = 0; i< nCount; i++)					// init route point property
		{
			AirRoutePointInSim * pPoint = pRouteInSim->GetRoutePointByIdx(i);

			FlightPlanPropertyInSim property(pPoint);
			property.SetAlt((pPoint->GetAltHigh() + pPoint->GetAltLow())/2.0) ;
			property.SetSpeed(ClimbSpeed) ;
			property.SetMode(OnClimbout);

			planProperties.push_back(property);
		}

		pRoute->AddItem(pTakeOffRunway);//runway
		nCount = pRouteInSim->GetRouteSegCount();			//init air route segments
		for (int j =0; j < nCount - 1 ; j++)
		{
			AirsideResource* pSeg = pRouteInSim->GetSeg(j);
			if (pSeg->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
				break;
						
			pRoute->AddItem(pSeg);
		}
		return true;

	}
	else if(pBasePlan->GetProcfileType() == FlightPlanBase::Specific_Profile)
	{
		pRoute = new FlightPlanSpecificRouteInSim;
		FlightPlanPropertiesInSim& planProperties = pRoute->GetPlanProperties();

		FlightPlanSpecific *pfltPlan = (FlightPlanSpecific *)pBasePlan;

		AirRoutePointInSim* pFirstIntersection = NULL;
		AirRoutePointInSim* pSecondIntersection = NULL;

		int nRouteCount = (int)pfltPlan->GetAirRouteCount();
		for(int i = 0;i< nRouteCount; ++i)
		{
			ns_FlightPlan::AirRouteProperty* pRouteProperty =pfltPlan->GetItem(i);
			int nID = pRouteProperty->GetAirRoute().getID();
			AirRouteInSim* pRouteInSim = m_pResManager->GetAirspaceResource()->GetAirLaneInSimByID(nID);

			if (pRouteProperty->GetAirRoute().getRouteType() == CAirRoute::CIRCUIT)
			{
				if (i == 0)				//Add Runway into air route resource list
				{
					int nRwyID = pRouteProperty->GetAirRoute().getLogicRunwayByIdx(0).first;
					RunwayInSim* pRunway = m_pResManager->GetAirportResource()->getRunwayResource()->GetRunwayByID(nRwyID);
					int nIdx = pRouteProperty->GetAirRoute().getLogicRunwayByIdx(0).second;
					LogicRunwayInSim* pLogicRunway = NULL;

					if (nIdx == 0)
						pLogicRunway = pRunway->GetLogicRunway1();
					else
						pLogicRunway = pRunway->GetLogicRunway2();

					pRoute->AddItem(pLogicRunway);
					pFlight->SetTakeoffRunway(pLogicRunway);
					pRoute->SetAirRouteInSim(pRouteInSim);
				}

				if (pSecondIntersection)
					pFirstIntersection = pSecondIntersection;

				int nPointCount = (int)pRouteProperty->GetWayPointCount();
				for(int j =0 ; j < nPointCount; ++j)
				{
					ns_FlightPlan::WayPointProperty * pWayPointProp =  pRouteProperty->GetItem(j);
					int nID = pWayPointProp->GetWayPoint().getWaypoint().getID();
					AirWayPointInSim * pWayPtSim = m_pResManager->GetAirspaceResource()->GetAirRoutePointList().GetAirWaypointInSimByID(nID);

					if ( !pWayPtSim)
						continue;

					if (i == 0 && j == 0)
						pFirstIntersection = pWayPtSim;

					if (j == nPointCount -1)
						pSecondIntersection = pWayPtSim;

					FlightPlanPropertyInSim property(pWayPtSim);
					property.SetAlt(ARCUnit::ConvertLength(pWayPointProp->GetAltitude(),ARCUnit::FEET,ARCUnit::CM)) ;
					double dSpeed = ARCUnit::ConvertVelocity(pWayPointProp->GetSpeed(),ARCUnit::KNOT,ARCUnit::CMpS);

					bool bSpeedFit = true;
					CString strError;
					switch(pWayPointProp->GetPhase())
					{
					case ns_FlightPlan::WayPointProperty::Phase_climbout:
						{
							property.SetMode(OnClimbout) ;

							if (dSpeed > pFlight->GetMaxSpeed(OnClimbout) || dSpeed < pFlight->GetMinSpeed(OnClimbout))
							{
								bSpeedFit = false;
								strError.Format("The flight plan speed exceed the flight performance speed on climb out phase!");
								dSpeed = pFlight->GetPerformance()->getClimboutSpeed(pFlight);
							}
							break;
						}

					case ns_FlightPlan::WayPointProperty::Phase_cruise:
						{
							property.SetMode(OnCruiseDep) ;

							if (dSpeed > pFlight->GetMaxSpeed(OnCruiseDep) || dSpeed < pFlight->GetMinSpeed(OnCruiseDep))
							{
								bSpeedFit = false;
								strError.Format("The flight plan speed exceed the flight performance speed on cruise phase!");
								dSpeed = (pFlight->GetMaxSpeed(OnCruiseDep) + pFlight->GetMinSpeed(OnCruiseDep))/2.0;
							}
							break;
						}
					default: 
						{
							property.SetMode(OnCruiseDep) ;

							if (dSpeed > pFlight->GetMaxSpeed(OnCruiseDep) || dSpeed < pFlight->GetMinSpeed(OnCruiseDep))
							{
								bSpeedFit = false;
								strError.Format("The flight plan speed exceed the flight performance speed on cruise phase!");
								dSpeed = (pFlight->GetMaxSpeed(OnCruiseDep) + pFlight->GetMinSpeed(OnCruiseDep))/2.0;
							}
							break;
						}
					}

					if (bSpeedFit == false)
					{
						CString strWarn = _T("DEFINITION ERROR");
						AirsideSimErrorShown::SimWarning(pFlight,strError,strWarn);
					}

					property.SetSpeed(dSpeed) ;
					planProperties.push_back(property);
				}
				AirsideResourceList segList = pRouteInSim->GetRouteSegmentList(pFirstIntersection,pSecondIntersection);

				int nSegCount = int(segList.size());
				for (int k =0; k< nSegCount - 1; k++)
				{
					if (segList[k]->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
						break;
					pRoute->AddItem(segList[k]);
				}
			}

		}
		return true;
	}


	return false;
}

bool FlightPlanInSim::GetLandingCircuitFlightPlan( AirsideFlightInSim* pFlight, FlightRouteInSim*& pRoute )
{
	using namespace ns_FlightPlan;

	FlightPlanBase *pBasePlan = GetBestFitFlightPlan(pFlight, FlightPlanBase::Circuit );
	if(pBasePlan == NULL)//have no available flight plan setting
	{
		return false;
	}
	if(pBasePlan->GetProcfileType() == FlightPlanBase::Generic_Profile)
	{

		FlightPlanGeneric *pfltPlan = (FlightPlanGeneric *)pBasePlan;
		pRoute = new FlightPlanGenericRouteInSim(pfltPlan);
		CAirRoute *pStarRoute = GetBestAirRoute(pFlight, CAirRoute::CIRCUIT, pFlight->GetLandingRunway());

		if(pStarRoute == NULL)//have no available STAR
			return false;

		//update landing runway
		LogicRunwayInSim * pLandingRunway = pFlight->GetLandingRunway();
		int nRwyID = -1;
		int nRwyMark = 0;
		if (pLandingRunway)
		{
			nRwyID = pLandingRunway->GetRunwayInSim()->GetRunwayInput()->getID();
			nRwyMark = pLandingRunway->getLogicRunwayType();
		}

		//set landing runway if no runway assigned
		if(pLandingRunway == NULL ||//no landing runway assigned
			!pStarRoute->IsConnectRunway(nRwyID, nRwyMark) //runway is not connected
			)
		{
			//update landing runway

			int nRwy = pStarRoute->GetConnectRunwayCount();
			for (int m =0; m < nRwy; m++)
			{
				LogicRunway_Pair logicrunway = pStarRoute->getLogicRunwayByIdx(m);

				if (pFlight->GetAirTrafficController()->GetRunwayAssignmentStrategy()->IsArrivalRunway(pFlight,logicrunway.first,logicrunway.second))
				{
					pLandingRunway = m_pResManager->GetAirportResource()->getRunwayResource()->GetLogicRunway(logicrunway.first,(RUNWAY_MARK)logicrunway.second);
					if(pLandingRunway == NULL)
						continue;

					pFlight->SetLandingRunway(pLandingRunway);
					break;
				}
			}
		}
		ASSERT(pLandingRunway != NULL);
		if(pLandingRunway == NULL)
			return false;

		//get the STAR from air route
		//get the AirRoute

		FlightPlanPropertiesInSim& planProperties = pRoute->GetPlanProperties();
		int nID = pStarRoute->getID();
		pRoute->SetName(pStarRoute->getName());
		AirRouteInSim* pRouteInSim = m_pResManager->GetAirspaceResource()->GetAirLaneInSimByID(nID);
		pRoute->SetAirRouteInSim(pRouteInSim);

		//OnTerminal ->...->OnApproach
		int nCount = pRouteInSim->GetRoutePointCount();			//init route point property
		for (int i =0; i < nCount; i++)
		{
			AirRoutePointInSim * pPoint = pRouteInSim->GetRoutePointByIdx(i);
			FlightPlanPropertyInSim property(pPoint);
			property.SetAlt((pPoint->GetAltHigh() + pPoint->GetAltLow())/2.0) ;
			property.SetArrival();
			if (i == nCount -1)//the last item is approach, 
			{
				property.SetSpeed((pFlight->GetPerformance()->getMinApproachSpeed(pFlight)+ pFlight->GetPerformance()->getMaxApproachSpeed(pFlight))/2.0);
				property.SetMode(OnApproach);
			}
			else
			{
				property.SetSpeed((pFlight->GetPerformance()->getMaxTerminalSpeed(pFlight)+ pFlight->GetPerformance()->getMinTerminalSpeed(pFlight))/2.0);
				property.SetMode(OnTerminal);
			}
			planProperties.push_back(property);
		}

		
		nCount = pRouteInSim->GetRouteSegCount();			//init air route segments

		int nHeadingSeg = nCount - 1;
		for (int nSeg = 0; nSeg < nCount; nSeg++)
		{
			AirsideResource* pSeg = pRouteInSim->GetSeg(nSeg);
			if (pSeg->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
			{
				nHeadingSeg = nSeg;
				break;
			}

		}

		for (int j =nHeadingSeg; j < nCount ; j++)
		{
			AirsideResource* pSeg = pRouteInSim->GetSeg(j);
			pRoute->AddItem(pSeg);
		}
		pRoute->AddItem(pLandingRunway);

		return true;
	}
	else if(pBasePlan->GetProcfileType() == FlightPlanBase::Specific_Profile)
	{
		pRoute = new FlightPlanSpecificRouteInSim;

		FlightPlanPropertiesInSim& planProperties = pRoute->GetPlanProperties();

		FlightPlanSpecific *pfltPlan = (FlightPlanSpecific *)pBasePlan;

		AirRoutePointInSim* pFirstIntersection = NULL;
		AirRoutePointInSim* pSecondIntersection = NULL;

		int nRouteCount = (int)pfltPlan->GetAirRouteCount();
		ns_FlightPlan::AirRouteProperty* pRouteProperty = NULL;
		AirRouteInSim* pRouteInSim = NULL;

		for(int i = 0;i< nRouteCount; ++i)
		{
			pRouteProperty =pfltPlan->GetItem(i);

			if (pRouteProperty->GetAirRoute().getRouteType() == CAirRoute::EN_ROUTE)
			{
				if (pRouteProperty->GetWayPointCount() == 0)
					continue;

				ns_FlightPlan::WayPointProperty * pWayPointProp =  pRouteProperty->GetItem(int(pRouteProperty->GetWayPointCount())-1);
				int nID = pWayPointProp->GetWayPoint().getWaypoint().getID() ;
				pFirstIntersection = m_pResManager->GetAirspaceResource()->GetAirRoutePointList().GetAirWaypointInSimByID(nID);
				continue;
			}

			int nID = pRouteProperty->GetAirRoute().getID();
			pRouteInSim = m_pResManager->GetAirspaceResource()->GetAirLaneInSimByID(nID);

			if (pSecondIntersection)
				pFirstIntersection = pSecondIntersection;

			int nPointCount = (int)pRouteProperty->GetWayPointCount();

			for(int j = 0 ; j < nPointCount; ++j)
			{
				ns_FlightPlan::WayPointProperty * pWayPointProp =  pRouteProperty->GetItem(j);
				int nID = pWayPointProp->GetWayPoint().getWaypoint().getID() ;
				AirWayPointInSim * pWayPtSim = m_pResManager->GetAirspaceResource()->GetAirRoutePointList().GetAirWaypointInSimByID(nID);

				if ( !pWayPtSim)
					continue;

				if (pFirstIntersection == NULL && i == 0)
				{
					pFirstIntersection = pWayPtSim;
				}
				if (j == nPointCount -1)
					pSecondIntersection = pWayPtSim;

				FlightPlanPropertyInSim property(pWayPtSim);
				property.SetAlt(ARCUnit::ConvertLength(pWayPointProp->GetAltitude(),ARCUnit::FEET,ARCUnit::CM)) ;
				double dSpeed = ARCUnit::ConvertVelocity(pWayPointProp->GetSpeed(),ARCUnit::KNOT,ARCUnit::CMpS);

				bool bSpeedFit = true;
				CString strError;
				switch(pWayPointProp->GetPhase())
				{
				case ns_FlightPlan::WayPointProperty::Phase_cruise :
					{
						property.SetMode(OnCruiseArr) ;

						if (dSpeed > pFlight->GetMaxSpeed(OnCruiseArr) || dSpeed < pFlight->GetMinSpeed(OnCruiseArr))
						{
							bSpeedFit = false;
							strError.Format("The flight plan speed exceed the flight performance speed on cruise phase!");
							dSpeed = (pFlight->GetMaxSpeed(OnCruiseArr) + pFlight->GetMinSpeed(OnCruiseArr))/2.0;
						}
						break;
					}

				case ns_FlightPlan::WayPointProperty::Phase_approach :
					{
						property.SetMode(OnApproach) ;

						if (dSpeed > pFlight->GetMaxSpeed(OnApproach) || dSpeed < pFlight->GetMinSpeed(OnApproach))
						{
							bSpeedFit = false;
							strError.Format("The flight plan speed exceed the flight performance speed on approach phase!");
							dSpeed = (pFlight->GetMaxSpeed(OnApproach) + pFlight->GetMinSpeed(OnApproach))/2.0;
						}
						break;
					}

				case ns_FlightPlan::WayPointProperty::Phase_Terminal:
					{
						property.SetMode(OnTerminal);

						if (dSpeed > pFlight->GetMaxSpeed(OnTerminal) || dSpeed < pFlight->GetMinSpeed(OnTerminal))
						{
							bSpeedFit = false;
							strError.Format("The flight plan speed exceed the flight performance speed on terminal phase!");
							dSpeed = (pFlight->GetMaxSpeed(OnTerminal) + pFlight->GetMinSpeed(OnTerminal))/2.0;
						}
						break;
					}

				case ns_FlightPlan::WayPointProperty::Phase_Landing:
					{
						property.SetMode(OnApproach);

						if (dSpeed > pFlight->GetMaxSpeed(OnApproach) || dSpeed < pFlight->GetMinSpeed(OnApproach))
						{
							bSpeedFit = false;
							strError.Format("The flight plan speed exceed the flight performance speed when landing!");
							//dSpeed = pFlight->GetPerformance()->getTouchDownSpeed(pFlight);
							dSpeed = (pFlight->GetMaxSpeed(OnApproach) + pFlight->GetMinSpeed(OnApproach))/2.0;
						}
						break;
					}

				default :
					{
						property.SetMode(OnCruiseArr);

						if (dSpeed > pFlight->GetMaxSpeed(OnCruiseArr) || dSpeed < pFlight->GetMinSpeed(OnCruiseArr))
						{
							bSpeedFit = false;
							strError.Format("The flight plan speed exceed the flight performance speed when landing!");
							//dSpeed = pFlight->GetPerformance()->getTouchDownSpeed(pFlight);
							dSpeed = (pFlight->GetMaxSpeed(OnCruiseArr) + pFlight->GetMinSpeed(OnCruiseArr))/2.0;
						}
						break;
					}
					break;
				}

				if (bSpeedFit == false)
				{
					CString strWarn = _T("DEFINITION ERROR");
					AirsideSimErrorShown::SimWarning(pFlight,strError,strWarn);
				}
				property.SetSpeed(dSpeed) ;
				property.SetArrival();
				planProperties.push_back(property);
			}
			AirsideResourceList segList = pRouteInSim->GetRouteSegmentList(pFirstIntersection,pSecondIntersection);

			int nSegCount = int(segList.size());
			int nHeadingSeg = nSegCount - 1;
			for (int nSeg = 0; nSeg < nSegCount; nSeg++)
			{
				AirsideResource* pSeg = pRouteInSim->GetSeg(nSeg);
				if (pSeg->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
				{
					nHeadingSeg = nSeg;
					break;
				}

			}
			for (int k =nHeadingSeg; k< nSegCount; k++)
				pRoute->AddItem(segList[k]);

		}
		if (pRouteProperty->GetAirRoute().getRouteType() == CAirRoute::CIRCUIT)	//Add Runway into air route resource list
		{
			int nRwyID = pRouteProperty->GetAirRoute().getLogicRunwayByIdx(0).first;
			RunwayInSim* pRunway = m_pResManager->GetAirportResource()->getRunwayResource()->GetRunwayByID(nRwyID);
			int nIdx = pRouteProperty->GetAirRoute().getLogicRunwayByIdx(0).second;
			LogicRunwayInSim* pLogicRunway = NULL;
			if (nIdx == 0)
				pLogicRunway = pRunway->GetLogicRunway1();
			else
				pLogicRunway = pRunway->GetLogicRunway2();

			pRoute->AddItem(pLogicRunway);
			pRoute->SetAirRouteInSim(pRouteInSim);
			pFlight->SetLandingRunway(pLogicRunway);
		}
		return true;
	}

	return false;
}

bool FlightPlanInSim::GetLandingEnrouteFlightPlan(AirsideFlightInSim* pFlight,FlightRouteInSim*& pRoute)
{
	using namespace ns_FlightPlan;
	
	FlightPlanBase *pBasePlan = GetBestFitFlightPlan(pFlight, FlightPlanBase::enroute );
	if(pBasePlan == NULL)//have no available flight plan setting
	{
		return false;
	}
	if(pBasePlan->GetProcfileType() == FlightPlanBase::Generic_Profile)
	{	
		FlightPlanGeneric *pfltPlan = (FlightPlanGeneric *)pBasePlan;
		pRoute = new FlightPlanGenericRouteInSim(pfltPlan);

		//get air route base on the direction of airport

		//no action
	
	}
	else if(pBasePlan->GetProcfileType() == FlightPlanBase::Specific_Profile)
	{
		pRoute = new FlightPlanSpecificRouteInSim;

		FlightPlanPropertiesInSim& planProperties = pRoute->GetPlanProperties();
		FlightPlanSpecific *pfltPlan = (FlightPlanSpecific *)pBasePlan;


		AirRoutePointInSim* pFirstIntersection = NULL;
		AirRoutePointInSim* pSecondIntersection = NULL;

		int nRouteCount = (int)pfltPlan->GetAirRouteCount();
		ns_FlightPlan::AirRouteProperty* pRouteProperty = NULL;
		for(int i = 0;i< nRouteCount; ++i)
		{
			pRouteProperty = pfltPlan->GetItem(i);
			if (pRouteProperty->GetAirRoute().getRouteType() == CAirRoute::EN_ROUTE)
			{
				int nID = pRouteProperty->GetAirRoute().getID();
				AirRouteInSim* pRouteInSim = m_pResManager->GetAirspaceResource()->GetAirLaneInSimByID(nID);

				if (pSecondIntersection)
					pFirstIntersection = pSecondIntersection;

				int nPointCount = (int)pRouteProperty->GetWayPointCount();

				for(int j =0 ; j < nPointCount; ++j)
				{
					ns_FlightPlan::WayPointProperty * pWayPointProp =  pRouteProperty->GetItem(j);
					int nID = pWayPointProp->GetWayPoint().getWaypoint().getID() ;
					AirWayPointInSim * pWayPtSim = m_pResManager->GetAirspaceResource()->GetAirRoutePointList().GetAirWaypointInSimByID(nID);

					if ( !pWayPtSim)
						continue;
					if (i == 0 && j == 0)
						pFirstIntersection = pWayPtSim;

					if (j == nPointCount -1)
						pSecondIntersection = pWayPtSim;

					FlightPlanPropertyInSim property(pWayPtSim);
					property.SetAlt(ARCUnit::ConvertLength(pWayPointProp->GetAltitude(),ARCUnit::FEET,ARCUnit::CM)) ;
					property.SetSpeed(ARCUnit::ConvertVelocity(pWayPointProp->GetSpeed(),ARCUnit::KNOT,ARCUnit::CMpS)) ;
					property.SetMode(OnCruiseArr);
					property.SetArrival();
					planProperties.push_back(property);

				}

				AirsideResourceList segList = pRouteInSim->GetRouteSegmentList(pFirstIntersection,pSecondIntersection);
				
				int nSegCount = int(segList.size());
				for (int k =0; k< nSegCount; k++)
				{
					pRoute->AddItem(segList[k]);
				}
			}

		}
		return true;
	}


	return false;
}

bool FlightPlanInSim::GetTakeoffEnrouteFlightPlan(AirsideFlightInSim* pFlight,FlightRouteInSim*& pRoute)
{

	using namespace ns_FlightPlan;

	FlightPlanBase *pBasePlan = GetBestFitFlightPlan(pFlight, FlightPlanBase::takeoff );
	if(pBasePlan == NULL)//have no available flight plan setting
	{
		return false;
	}
	if(pBasePlan->GetProcfileType() == FlightPlanBase::Generic_Profile)
	{
		FlightPlanGeneric *pfltPlan = (FlightPlanGeneric *)pBasePlan;
		pRoute = new FlightPlanGenericRouteInSim(pfltPlan);

		//no action


	}
	else if(pBasePlan->GetProcfileType() == FlightPlanBase::Specific_Profile)
	{
		pRoute = new FlightPlanSpecificRouteInSim;

		FlightPlanPropertiesInSim& planProperties = pRoute->GetPlanProperties();

		FlightPlanSpecific *pfltPlan = (FlightPlanSpecific *)pBasePlan;
		
		AirRoutePointInSim* pFirstIntersection = NULL;
		AirRoutePointInSim* pSecondIntersection = NULL;

		int nRouteCount = (int)pfltPlan->GetAirRouteCount();
		for(int i = 0;i< nRouteCount; ++i)
		{
			ns_FlightPlan::AirRouteProperty* pRouteProperty =pfltPlan->GetItem(i);

			if (pRouteProperty->GetAirRoute().getRouteType() == CAirRoute::SID)
			{
				if (pRouteProperty->GetWayPointCount() == 0)
					continue;

				ns_FlightPlan::WayPointProperty * pWayPointProp =  pRouteProperty->GetItem(int(pRouteProperty->GetWayPointCount())-1);
				int nID = pWayPointProp->GetWayPoint().getWaypoint().getID() ;
				pFirstIntersection = m_pResManager->GetAirspaceResource()->GetAirRoutePointList().GetAirWaypointInSimByID(nID);
				continue;
			}

			int nID = pRouteProperty->GetAirRoute().getID();
			AirRouteInSim* pRouteInSim = m_pResManager->GetAirspaceResource()->GetAirLaneInSimByID(nID);

			if (pSecondIntersection)
				pFirstIntersection = pSecondIntersection;

			int nPointCount = (int)pRouteProperty->GetWayPointCount();
			for(int j =0 ; j < nPointCount; ++j)
			{
				ns_FlightPlan::WayPointProperty * pWayPointProp =  pRouteProperty->GetItem(j);
				int nID = pWayPointProp->GetWayPoint().getWaypoint().getID() ;
				AirWayPointInSim * pWayPtSim = m_pResManager->GetAirspaceResource()->GetAirRoutePointList().GetAirWaypointInSimByID(nID);

				if ( !pWayPtSim)
					continue;

				if (j == nPointCount -1)
					pSecondIntersection = pWayPtSim;

				FlightPlanPropertyInSim property(pWayPtSim);
				property.SetAlt(ARCUnit::ConvertLength(pWayPointProp->GetAltitude(),ARCUnit::FEET,ARCUnit::CM)) ;
				property.SetSpeed(ARCUnit::ConvertVelocity(pWayPointProp->GetSpeed(),ARCUnit::KNOT,ARCUnit::CMpS)) ;
				property.SetMode(OnCruiseDep);
				planProperties.push_back(property);

			}

			AirsideResourceList segList = pRouteInSim->GetRouteSegmentList(pFirstIntersection,pSecondIntersection);

			int nSegCount = int(segList.size());
			for (int k =0; k< nSegCount; k++)
				pRoute->AddItem(segList[k]);
		}
		return true;
	}

	return false;
}

bool FlightPlanInSim::GetSTARFlightPlan(AirsideFlightInSim* pFlight,FlightRouteInSim*& pRoute)
{

	using namespace ns_FlightPlan;

	FlightPlanBase *pBasePlan = GetBestFitFlightPlan(pFlight, FlightPlanBase::landing );
	if(pBasePlan == NULL)//have no available flight plan setting
	{
		return false;
	}
	if(pBasePlan->GetProcfileType() == FlightPlanBase::Generic_Profile)
	{

		FlightPlanGeneric *pfltPlan = (FlightPlanGeneric *)pBasePlan;
		pRoute = new FlightPlanGenericRouteInSim(pfltPlan);
		CAirRoute *pStarRoute = GetBestAirRoute(pFlight, CAirRoute::STAR, pFlight->GetLandingRunway());
		
		if(pStarRoute == NULL)//have no available STAR
			return false;

//update landing runway
		LogicRunwayInSim * pLandingRunway = pFlight->GetLandingRunway();
		int nRwyID = -1;
		int nRwyMark = 0;
		if (pLandingRunway)
		{
			nRwyID = pLandingRunway->GetRunwayInSim()->GetRunwayInput()->getID();
			nRwyMark = pLandingRunway->getLogicRunwayType();
		}

		//set landing runway if no runway assigned
		if(pLandingRunway == NULL ||//no landing runway assigned
			!pStarRoute->IsConnectRunway(nRwyID, nRwyMark) //runway is not connected
			)
		{
			//update landing runway

			int nRwy = pStarRoute->GetConnectRunwayCount();
			for (int m =0; m < nRwy; m++)
			{
				LogicRunway_Pair logicrunway = pStarRoute->getLogicRunwayByIdx(m);

				if (pFlight->GetAirTrafficController()->GetRunwayAssignmentStrategy()->IsArrivalRunway(pFlight,logicrunway.first,logicrunway.second))
				{
					pLandingRunway = m_pResManager->GetAirportResource()->getRunwayResource()->GetLogicRunway(logicrunway.first,(RUNWAY_MARK)logicrunway.second);
					if(pLandingRunway == NULL)
						continue;

					pFlight->SetLandingRunway(pLandingRunway);
					break;
				}
			}
		}
		ASSERT(pLandingRunway != NULL);
		if(pLandingRunway == NULL)
			return false;

//get the STAR from air route
		//get the AirRoute

		FlightPlanPropertiesInSim& planProperties = pRoute->GetPlanProperties();
		int nID = pStarRoute->getID();
		pRoute->SetName(pStarRoute->getName());
		AirRouteInSim* pRouteInSim = m_pResManager->GetAirspaceResource()->GetAirLaneInSimByID(nID);
		pRoute->SetAirRouteInSim(pRouteInSim);

		//OnTerminal ->...->OnApproach
		int nCount = pRouteInSim->GetRoutePointCount();			//init route point property
		for (int i =0; i < nCount; i++)
		{
			AirRoutePointInSim * pPoint = pRouteInSim->GetRoutePointByIdx(i);
			FlightPlanPropertyInSim property(pPoint);
			property.SetAlt((pPoint->GetAltHigh() + pPoint->GetAltLow())/2.0) ;
			property.SetArrival();
			if (i == nCount -1)//the last item is approach, 
			{
				property.SetSpeed((pFlight->GetPerformance()->getMinApproachSpeed(pFlight)+ pFlight->GetPerformance()->getMaxApproachSpeed(pFlight))/2.0);
				property.SetMode(OnApproach);
			}
			else
			{
				property.SetSpeed((pFlight->GetPerformance()->getMaxTerminalSpeed(pFlight)+ pFlight->GetPerformance()->getMinTerminalSpeed(pFlight))/2.0);
				property.SetMode(OnTerminal);
			}
			planProperties.push_back(property);
		}

		nCount = pRouteInSim->GetRouteSegCount();			//init air route segments
		for (int j =0; j < nCount ; j++)
		{
			AirsideResource* pSeg = pRouteInSim->GetSeg(j);
			pRoute->AddItem(pSeg);
		}
		pRoute->AddItem(pLandingRunway);

		return true;
	}
	else if(pBasePlan->GetProcfileType() == FlightPlanBase::Specific_Profile)
	{
		pRoute = new FlightPlanSpecificRouteInSim;

		FlightPlanPropertiesInSim& planProperties = pRoute->GetPlanProperties();

		FlightPlanSpecific *pfltPlan = (FlightPlanSpecific *)pBasePlan;

		AirRoutePointInSim* pFirstIntersection = NULL;
		AirRoutePointInSim* pSecondIntersection = NULL;

		int nRouteCount = (int)pfltPlan->GetAirRouteCount();
		ns_FlightPlan::AirRouteProperty* pRouteProperty = NULL;
		AirRouteInSim* pRouteInSim = NULL;

		for(int i = 0;i< nRouteCount; ++i)
		{
			pRouteProperty =pfltPlan->GetItem(i);

			if (pRouteProperty->GetAirRoute().getRouteType() == CAirRoute::EN_ROUTE)
			{
				if (pRouteProperty->GetWayPointCount() == 0)
					continue;

				ns_FlightPlan::WayPointProperty * pWayPointProp =  pRouteProperty->GetItem(int(pRouteProperty->GetWayPointCount())-1);
				int nID = pWayPointProp->GetWayPoint().getWaypoint().getID() ;
				pFirstIntersection = m_pResManager->GetAirspaceResource()->GetAirRoutePointList().GetAirWaypointInSimByID(nID);
				continue;
			}

			int nID = pRouteProperty->GetAirRoute().getID();
			pRouteInSim = m_pResManager->GetAirspaceResource()->GetAirLaneInSimByID(nID);

			if (pSecondIntersection)
				pFirstIntersection = pSecondIntersection;

			int nPointCount = (int)pRouteProperty->GetWayPointCount();

			for(int j = 0 ; j < nPointCount; ++j)
			{
				ns_FlightPlan::WayPointProperty * pWayPointProp =  pRouteProperty->GetItem(j);
				int nID = pWayPointProp->GetWayPoint().getWaypoint().getID() ;
				AirWayPointInSim * pWayPtSim = m_pResManager->GetAirspaceResource()->GetAirRoutePointList().GetAirWaypointInSimByID(nID);

				if ( !pWayPtSim)
					continue;

				if (pFirstIntersection == NULL && i == 0)
				{
					pFirstIntersection = pWayPtSim;
				}
				if (j == nPointCount -1)
					pSecondIntersection = pWayPtSim;

				FlightPlanPropertyInSim property(pWayPtSim);
				property.SetAlt(ARCUnit::ConvertLength(pWayPointProp->GetAltitude(),ARCUnit::FEET,ARCUnit::CM)) ;
				double dSpeed = ARCUnit::ConvertVelocity(pWayPointProp->GetSpeed(),ARCUnit::KNOT,ARCUnit::CMpS);

				bool bSpeedFit = true;
				CString strError;
				switch(pWayPointProp->GetPhase())
				{
					case ns_FlightPlan::WayPointProperty::Phase_cruise :
					{
						property.SetMode(OnCruiseArr) ;

						if (dSpeed > pFlight->GetMaxSpeed(OnCruiseArr) || dSpeed < pFlight->GetMinSpeed(OnCruiseArr))
						{
							bSpeedFit = false;
							strError.Format("The flight plan speed exceed the flight performance speed on cruise phase!");
							dSpeed = (pFlight->GetMaxSpeed(OnCruiseArr) + pFlight->GetMinSpeed(OnCruiseArr))/2.0;
						}
						break;
					}

					case ns_FlightPlan::WayPointProperty::Phase_approach :
					{
						property.SetMode(OnApproach) ;

						if (dSpeed > pFlight->GetMaxSpeed(OnApproach) || dSpeed < pFlight->GetMinSpeed(OnApproach))
						{
							bSpeedFit = false;
							strError.Format("The flight plan speed exceed the flight performance speed on approach phase!");
							dSpeed = (pFlight->GetMaxSpeed(OnApproach) + pFlight->GetMinSpeed(OnApproach))/2.0;
						}
						break;
					}

					case ns_FlightPlan::WayPointProperty::Phase_Terminal:
					{
						property.SetMode(OnTerminal);

						if (dSpeed > pFlight->GetMaxSpeed(OnTerminal) || dSpeed < pFlight->GetMinSpeed(OnTerminal))
						{
							bSpeedFit = false;
							strError.Format("The flight plan speed exceed the flight performance speed on terminal phase!");
							dSpeed = (pFlight->GetMaxSpeed(OnTerminal) + pFlight->GetMinSpeed(OnTerminal))/2.0;
						}
						break;
					}

					case ns_FlightPlan::WayPointProperty::Phase_Landing:
					{
						property.SetMode(OnApproach);

						if (dSpeed > pFlight->GetMaxSpeed(OnApproach) || dSpeed < pFlight->GetMinSpeed(OnApproach))
						{
							bSpeedFit = false;
							strError.Format("The flight plan speed exceed the flight performance speed when landing!");
							//dSpeed = pFlight->GetPerformance()->getTouchDownSpeed(pFlight);
							dSpeed = (pFlight->GetMaxSpeed(OnApproach) + pFlight->GetMinSpeed(OnApproach))/2.0;
						}
						break;
					}

					default :
						break;
				}

				if (bSpeedFit == false)
				{
					CString strWarn = _T("DEFINITION ERROR");
					AirsideSimErrorShown::SimWarning(pFlight,strError,strWarn);
				}
				property.SetSpeed(dSpeed) ;
				property.SetArrival();
				planProperties.push_back(property);
			}
			AirsideResourceList segList = pRouteInSim->GetRouteSegmentList(pFirstIntersection,pSecondIntersection);

			int nSegCount = int(segList.size());
			for (int k =0; k< nSegCount; k++)
				pRoute->AddItem(segList[k]);

		}
		if (pRouteProperty->GetAirRoute().getRouteType() == CAirRoute::STAR)	//Add Runway into air route resource list
		{
			int nRwyID = pRouteProperty->GetAirRoute().getLogicRunwayByIdx(0).first;
			RunwayInSim* pRunway = m_pResManager->GetAirportResource()->getRunwayResource()->GetRunwayByID(nRwyID);
			int nIdx = pRouteProperty->GetAirRoute().getLogicRunwayByIdx(0).second;
			LogicRunwayInSim* pLogicRunway = NULL;
			if (nIdx == 0)
				pLogicRunway = pRunway->GetLogicRunway1();
			else
				pLogicRunway = pRunway->GetLogicRunway2();

			pRoute->AddItem(pLogicRunway);
			pRoute->SetAirRouteInSim(pRouteInSim);
			pFlight->SetLandingRunway(pLogicRunway);
		}
		return true;
	}

	return false;
}

bool FlightPlanInSim::GetSIDFlightPlan(AirsideFlightInSim* pFlight,FlightRouteInSim*& pRoute)
{

	using namespace ns_FlightPlan;

	FlightPlanBase *pBasePlan = GetBestFitFlightPlan(pFlight, FlightPlanBase::takeoff );
	if(pBasePlan == NULL)//have no available flight plan setting
	{
		return false;
	}
	if(pBasePlan->GetProcfileType() == FlightPlanBase::Generic_Profile)
	{
		FlightPlanGeneric *pfltPlan = (FlightPlanGeneric *)pBasePlan;
		pRoute = new FlightPlanGenericRouteInSim(pfltPlan);

		CAirRoute *pSIDRoute = GetBestAirRoute(pFlight, CAirRoute::SID, pFlight->GetAndAssignTakeoffRunway());
		
		if(pSIDRoute == NULL)//have no available SID
			return false;

//update take off runway
		LogicRunwayInSim * pTakeOffRunway = pFlight->GetAndAssignTakeoffRunway();
		int nRwyID = -1;
		int nRwyMark = 0;
		if (pTakeOffRunway)
		{
			nRwyID = pTakeOffRunway->GetRunwayInSim()->GetRunwayInput()->getID();
			nRwyMark = pTakeOffRunway->getLogicRunwayType();
		}

		if(pTakeOffRunway == NULL ||
			!pSIDRoute->IsConnectRunway(nRwyID, nRwyMark))		//if no sid connected to runway,change another connected runway
		{
			int nRwy = pSIDRoute->GetConnectRunwayCount();
			for (int m =0; m < nRwy; m++)
			{
				LogicRunway_Pair logicrunway = pSIDRoute->getLogicRunwayByIdx(m);

				if (pFlight->GetAirTrafficController()->GetRunwayAssignmentStrategy()->IsDepartureRunway(pFlight,logicrunway.first,logicrunway.second))
				{
					pTakeOffRunway = m_pResManager->GetAirportResource()->getRunwayResource()->GetLogicRunway(logicrunway.first,(RUNWAY_MARK)logicrunway.second);
					if(pTakeOffRunway == NULL)
						continue;

					pFlight->SetTakeoffRunway(pTakeOffRunway);
					break;
				}
			}
		}
		ASSERT(pTakeOffRunway != NULL);
		if(pTakeOffRunway == NULL)
			return false;

//get the SID from air route
		FlightPlanPropertiesInSim& planProperties = pRoute->GetPlanProperties();
		double ClimbSpeed = pFlight->GetPerformance()->getClimboutSpeed(pFlight) ;

		int nID = pSIDRoute->getID();
		pRoute->SetName(pSIDRoute->getName());
		AirRouteInSim* pRouteInSim = m_pResManager->GetAirspaceResource()->GetAirLaneInSimByID(nID);

		pRoute->SetAirRouteInSim(pRouteInSim);
		int nCount = pRouteInSim->GetRoutePointCount();
		for (int i = 0; i< nCount; i++)					// init route point property
		{
			AirRoutePointInSim * pPoint = pRouteInSim->GetRoutePointByIdx(i);

			FlightPlanPropertyInSim property(pPoint);
			property.SetAlt((pPoint->GetAltHigh() + pPoint->GetAltLow())/2.0) ;
			property.SetSpeed(ClimbSpeed) ;
			property.SetMode(OnClimbout);

			planProperties.push_back(property);
		}

		pRoute->AddItem(pTakeOffRunway);//runway
		nCount = pRouteInSim->GetRouteSegCount();			//init air route segments
		for (int j =0; j < nCount ; j++)
		{
			AirsideResource* pSeg = pRouteInSim->GetSeg(j);
			pRoute->AddItem(pSeg);
		}
		return true;

	}
	else if(pBasePlan->GetProcfileType() == FlightPlanBase::Specific_Profile)
	{
		pRoute = new FlightPlanSpecificRouteInSim;
		FlightPlanPropertiesInSim& planProperties = pRoute->GetPlanProperties();

		FlightPlanSpecific *pfltPlan = (FlightPlanSpecific *)pBasePlan;
	
		AirRoutePointInSim* pFirstIntersection = NULL;
		AirRoutePointInSim* pSecondIntersection = NULL;

		int nRouteCount = (int)pfltPlan->GetAirRouteCount();
		for(int i = 0;i< nRouteCount; ++i)
		{
			ns_FlightPlan::AirRouteProperty* pRouteProperty =pfltPlan->GetItem(i);
			int nID = pRouteProperty->GetAirRoute().getID();
			AirRouteInSim* pRouteInSim = m_pResManager->GetAirspaceResource()->GetAirLaneInSimByID(nID);

			if (pRouteProperty->GetAirRoute().getRouteType() == CAirRoute::SID)
			{
				if (i == 0)				//Add Runway into air route resource list
				{
					int nRwyID = pRouteProperty->GetAirRoute().getLogicRunwayByIdx(0).first;
					RunwayInSim* pRunway = m_pResManager->GetAirportResource()->getRunwayResource()->GetRunwayByID(nRwyID);
					int nIdx = pRouteProperty->GetAirRoute().getLogicRunwayByIdx(0).second;
					LogicRunwayInSim* pLogicRunway = NULL;

					if (nIdx == 0)
						pLogicRunway = pRunway->GetLogicRunway1();
					else
						pLogicRunway = pRunway->GetLogicRunway2();

					pRoute->AddItem(pLogicRunway);
					pFlight->SetTakeoffRunway(pLogicRunway);
					pRoute->SetAirRouteInSim(pRouteInSim);
				}

				if (pSecondIntersection)
					pFirstIntersection = pSecondIntersection;

				int nPointCount = (int)pRouteProperty->GetWayPointCount();
				for(int j =0 ; j < nPointCount; ++j)
				{
					ns_FlightPlan::WayPointProperty * pWayPointProp =  pRouteProperty->GetItem(j);
					int nID = pWayPointProp->GetWayPoint().getWaypoint().getID();
					AirWayPointInSim * pWayPtSim = m_pResManager->GetAirspaceResource()->GetAirRoutePointList().GetAirWaypointInSimByID(nID);

					if ( !pWayPtSim)
						continue;

					if (i == 0 && j == 0)
						pFirstIntersection = pWayPtSim;

					if (j == nPointCount -1)
						pSecondIntersection = pWayPtSim;

					FlightPlanPropertyInSim property(pWayPtSim);
					property.SetAlt(ARCUnit::ConvertLength(pWayPointProp->GetAltitude(),ARCUnit::FEET,ARCUnit::CM)) ;
					double dSpeed = ARCUnit::ConvertVelocity(pWayPointProp->GetSpeed(),ARCUnit::KNOT,ARCUnit::CMpS);

					bool bSpeedFit = true;
					CString strError;
					switch(pWayPointProp->GetPhase())
					{
						case ns_FlightPlan::WayPointProperty::Phase_climbout:
						{
							property.SetMode(OnClimbout) ;

							if (dSpeed > pFlight->GetMaxSpeed(OnClimbout) || dSpeed < pFlight->GetMinSpeed(OnClimbout))
							{
								bSpeedFit = false;
								strError.Format("The flight plan speed exceed the flight performance speed on climb out phase!");
								dSpeed = pFlight->GetPerformance()->getClimboutSpeed(pFlight);
							}
							break;
						}

						case ns_FlightPlan::WayPointProperty::Phase_cruise:
						{
							property.SetMode(OnCruiseDep) ;

							if (dSpeed > pFlight->GetMaxSpeed(OnCruiseDep) || dSpeed < pFlight->GetMinSpeed(OnCruiseDep))
							{
								bSpeedFit = false;
								strError.Format("The flight plan speed exceed the flight performance speed on cruise phase!");
								dSpeed = (pFlight->GetMaxSpeed(OnCruiseDep) + pFlight->GetMinSpeed(OnCruiseDep))/2.0;
							}
							break;
						}
					}

					if (bSpeedFit == false)
					{
						CString strWarn = _T("DEFINITION ERROR");
						AirsideSimErrorShown::SimWarning(pFlight,strError,strWarn);
					}

					property.SetSpeed(dSpeed) ;
					planProperties.push_back(property);
				}
				AirsideResourceList segList = pRouteInSim->GetRouteSegmentList(pFirstIntersection,pSecondIntersection);

				int nSegCount = int(segList.size());
				for (int k =0; k< nSegCount; k++)
					pRoute->AddItem(segList[k]);
			}

		}
		return true;
	}

	
	return false;
}

//the data in the m_pFlightPlansInput has been sorted
//so, return the first fit one 
//if have no available item ,return NULL
ns_FlightPlan::FlightPlanBase * FlightPlanInSim::GetBestFitFlightPlan( AirsideFlightInSim *pFlight, ns_FlightPlan::FlightPlanBase::OperationType opType ) const
{
	using namespace ns_FlightPlan;

	FlightPlanBase *pBestPlan = NULL;

	int nPlanCount = (int)m_pFlightPlansInput->GetCount();
	for (int nPlan = 0; nPlan < nPlanCount; ++nPlan)
	{
		 FlightPlanBase *pFltPlan = m_pFlightPlansInput->GetFlightPlan(nPlan);
		 ASSERT(pFltPlan != NULL);

		 if(pFltPlan == NULL)
			 continue;

		 if(pFltPlan->GetOperationType() != opType)
			 continue;

		 FlightConstraint fltCnst;		
		 ASSERT(m_pAirportDB);
		 fltCnst.SetAirportDB(m_pAirportDB);
		 fltCnst.setConstraintWithVersion(pFltPlan->GetFlightType());

		 if( pFlight->fits(fltCnst) )
		 {
			 pBestPlan = pFltPlan;
			 break;
		 }

	}

	return pBestPlan;
}


CAirRoute * FlightPlanInSim::GetBestAirRoute( AirsideFlightInSim* pFlight, CAirRoute::RouteType enumType, LogicRunwayInSim *pLogicRunway ) const
{
	CAirRoute* pBestRoute = NULL;
	int nRwyID = -1;
	int nRwyMark = 0;
	if (pLogicRunway != NULL)
	{
		nRwyID = pLogicRunway->GetRunwayInSim()->GetRunwayInput()->getID();
		nRwyMark = pLogicRunway->getLogicRunwayType();
	}
	
	

	CAirportCode ARP = pFlight->GetFlightInput()->getOrigin();
	CLatitude lat;
	CLongitude lont;
	_g_GetActiveAirportMan(m_pAirportDB)->GetAirportCoordinates(ARP.GetValue(),lat,lont);

	AirRouteList& _RouteList = m_pResManager->GetAirspaceResource()->GetAirRouteList(); 


	std::vector<CAirRoute*> vAirRoutes;
	int nSize = _RouteList.GetAirRouteCount();
	for (int i = 0; i < nSize; i ++)
	{
		CAirRoute* pAirRoute = _RouteList.GetAirRoute(i);
		if (pAirRoute->getRouteType() != enumType)
			continue;

		if (nRwyID < 0)//without runway assign, get arbitrary one
		{
			vAirRoutes.push_back(pAirRoute);
		}
		else//if with runway assign, assign a star connected with the runway
		{
			int nRwy = pAirRoute->GetConnectRunwayCount();
			for (int j = 0; j < nRwy; j++)
			{
				if (pAirRoute->IsConnectRunway(nRwyID, nRwyMark))
					vAirRoutes.push_back(pAirRoute);
			}
		}

	}

	//According to direction to find one air route
	int nAvailableCount = vAirRoutes.size();
	double dMinDirCos = -1;
	for (int nAvail = 0; nAvail < nAvailableCount; nAvail++)
	{
		CAirRoute* pAirRoute = vAirRoutes.at(nAvail);
		double dValue = pAirRoute->GetAngleCosValueWithARPCoordinateBaseOnRefPoint(lat,lont);
		if (dValue >= dMinDirCos)
		{
			pBestRoute = pAirRoute;
			dMinDirCos = dValue;
		}
	}

	return pBestRoute;
}


























