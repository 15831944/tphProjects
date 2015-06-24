#include "StdAfx.h"
#include ".\sid_starassignmentinsim.h"
#include "AirsideFlightInSim.h"
#include "../../InputAirside/FlightSIDAssignment.h"
#include "../../InputAirside/FlightStarAssignment.h"
#include "./Common/AirportDatabase.h"
#include "./InputAirside/AirRoute.h"
#include "AirspaceResourceManager.h"
#include "RunwayInSim.h"
#include "AirspaceResource.h"
#include "Common/ARCUnit.h"
#include "FlightPerformanceManager.h"
#include "AirsideItinerantFlightInSim.h"
#include "FlightPlanInSim.h"
#include "AirTrafficController.h"
#include "AirsideResourceManager.h"
#include "SimulationErrorShow.h"
#include "../../Common/AirportsManager.h"
#include "../../Common/latlong.h"

#include "FlightPlanSpecificRouteInSim.h"

CSID_STARAssignmentInSim::CSID_STARAssignmentInSim(void)
:m_pFlightSIDAssignmentList(NULL)
,m_pFlightSTARAssignmentList(NULL)
{

}

CSID_STARAssignmentInSim::~CSID_STARAssignmentInSim(void)
{
	if (m_pFlightSIDAssignmentList != NULL)
	{
		delete m_pFlightSIDAssignmentList;
		m_pFlightSIDAssignmentList = NULL;
	}
	if (m_pFlightSTARAssignmentList != NULL)
	{
		delete m_pFlightSTARAssignmentList;
		m_pFlightSTARAssignmentList = NULL;
	}

}

void CSID_STARAssignmentInSim::Init(int nPrjID, CAirportDatabase* pAirportDatabase)
{
	m_pAirportDB = pAirportDatabase;
	m_pFlightSIDAssignmentList = new FlightSIDAssignmentList;
	m_pFlightSIDAssignmentList->SetAirportDatabase(pAirportDatabase);
	m_pFlightSIDAssignmentList->ReadData(nPrjID);
	m_pFlightSTARAssignmentList = new FlightStarAssignmentList;
	m_pFlightSTARAssignmentList->SetAirportDatabase(pAirportDatabase);
	m_pFlightSTARAssignmentList->ReadData(nPrjID);

}

bool CSID_STARAssignmentInSim::GetSIDRoute(AirsideFlightInSim* pFlight,AirsideResourceManager* pResManger,FlightRouteInSim*& pnewRoute)
{
	if (pFlight== NULL || pResManger ==NULL )
		return false;
	//flight plan defined
	bool bInit = pFlight->GetAirTrafficController()->GetFlightPlan()->GetSIDFlightPlan(pFlight,pnewRoute);
	if (bInit)
		return true;

	//uses SID assignment
	delete pnewRoute;
	pnewRoute =new FlightPlanSpecificRouteInSim;
	FlightPlanPropertiesInSim& planProperties = pnewRoute->GetPlanProperties();	

	LogicRunwayInSim* pLogicRunway = pFlight->GetAndAssignTakeoffRunway();
	AirRouteList& _RouteList = pResManger->GetAirspaceResource()->GetAirRouteList(); 

	CAirRoute* pSIDRoute = NULL;
	int nRwyID = -1;
	int nRwyMark = 0;
	if (pLogicRunway)
	{
		nRwyID = pLogicRunway->GetRunwayInSim()->GetRunwayInput()->getID();
		nRwyMark = pLogicRunway->getLogicRunwayType();
	}

	FlightSIDAssignment* pSIDAssignment = NULL;
	std::vector< SIDPercentage*> vSIDRoute;
	int nCount = m_pFlightSIDAssignmentList->GetSIDCount();
	for (int i = 0; i < nCount; i++)
	{
		pSIDAssignment = m_pFlightSIDAssignmentList->GetSIDItem(i);
		FlightConstraint fltCnst = pSIDAssignment->GetFlightConstraint();
		if (pFlight->fits(fltCnst))
		{
			ElapsedTime tStart = pSIDAssignment->GetStartTime() ;
			ElapsedTime tEnd = pSIDAssignment->GetEndTime() ;
			if (pFlight->GetTime()  >= tStart && pFlight->GetTime() <= tEnd)
			{
				int nSID = pSIDAssignment->GetSIDCount();
				if (pLogicRunway != NULL)	//assigned takeoff runway
				{
					for (int j = 0; j < nSID; j++ )
					{
						SIDPercentage* pSID = pSIDAssignment->GetSIDItem(j);
						vSIDRoute.push_back(pSID);						
					}
				}
				else			//not assigned takeoff runway
				{
					double dRandPercent = double(rand()%100);
					double dPercent = 0.0;
					for ( int j = 0; j< nSID; j++ )
					{
						SIDPercentage* pSID = pSIDAssignment->GetSIDItem(j);
						if (dRandPercent>= dPercent && dRandPercent < dPercent + pSID->GetPercent())
						{
							vSIDRoute.push_back(pSID);
							break;
						}
						dPercent += pSID->GetPercent();
					}
				}			
			}
			break;
		}
	}

	if (!vSIDRoute.empty())
	{	
		if (pFlight->IsItinerantFlight())//filter the sid route for itinerant flight
		{
			AirsideItinerantFlightInSim *pItiFlight = reinterpret_cast<AirsideItinerantFlightInSim *>(pFlight);
			if (pItiFlight)
			{
				for (size_t nSID = 0; nSID < vSIDRoute.size(); ++ nSID)
				{	
					SIDPercentage* pSID = vSIDRoute.at(nSID);
					CAirRoute* pRoute = _RouteList.GetAirRouteByID(pSID->GetSIDID());

					if (pSID== NULL || pRoute == NULL)
						continue;

					if(pRoute->IsWaypointExists(pItiFlight->GetExitWaypointID()))
					{
						pSIDRoute = pRoute;

						int nRunwayID = pSIDRoute->getLogicRunwayByIdx(0).first ;
						int nRunwayMark = pSIDRoute->getLogicRunwayByIdx(0).second;
						pLogicRunway = pResManger->GetAirportResource()->getRunwayResource()->GetLogicRunway(nRunwayID,(RUNWAY_MARK)nRunwayMark);
						pFlight->SetTakeoffRunway(pLogicRunway);
						break;
					}
				}
			}
		}
		else		// for schedule flight
		{
			int nSize = (int)vSIDRoute.size();
			double dRandPercent = double(rand()%100);
			double dPercent = 0.0;

			for (int n =0; n < nSize; n++)
			{
				SIDPercentage* pSID = vSIDRoute.at(n);
				CAirRoute* pRoute = _RouteList.GetAirRouteByID(pSID->GetSIDID());
				if (pRoute)
				{
					if (nRwyID < 0 || (nRwyID >=0 && pRoute->IsConnectRunway(nRwyID, nRwyMark)))	// no assigned takeoff runway or the SID connected the runway
					{
						dPercent += pSID->GetPercent();
						pSIDRoute = pRoute;
					}
					if (dPercent - dRandPercent >= 0)
						break;

				}

			}
		}

		if (pSIDRoute == NULL && pResManger->GetAirspaceResource()->HasSID())	// error assignment
		{
			CString strDetals = _T("SID Assignment Conflict With Takeoff Runway Assignment!");
			CString strError = _T("DEFINE ERROR");
			AirsideSimErrorShown::SimWarning(pFlight,strDetals,strError);

			vSIDRoute.clear();
		}
		
	}

	if (vSIDRoute.empty())	//no available sid list
	{
		CAirportCode ARP = pFlight->GetFlightInput()->getDestination();
		CLatitude lat;
		CLongitude lont;
		_g_GetActiveAirportMan(m_pAirportDB)->GetAirportCoordinates(ARP.GetValue(),lat,lont);
		
		int nSize = _RouteList.GetAirRouteCount();
		std::vector<CAirRoute*> vAirRoutes;
		for (int i = 0; i < nSize; i ++)
		{
			CAirRoute* pAirRoute = _RouteList.GetAirRoute(i);
			if (pAirRoute->getRouteType() != CAirRoute::SID)
				continue;

			if (nRwyID < 0)		//without runway assign, get arbitrary one
			{
				vAirRoutes.push_back(pAirRoute);
			}
			else			//if with runway assign, assign a sid connected with the runway
			{
				int nRwy = pAirRoute->GetConnectRunwayCount();
				for (int j = 0; j < nRwy; j++)
				{
					if (pAirRoute->IsConnectRunway(nRwyID, nRwyMark) )
						vAirRoutes.push_back(pAirRoute);
				}
			}

		}
		
		//According to direction to find sid
		int nSIDCount = vAirRoutes.size();
		double dMinDirCos = -1;
		for (int i = 0; i < nSIDCount; i++)
		{
			CAirRoute* pAirRoute = vAirRoutes.at(i);
			double dValue = pAirRoute->GetAngleCosValueWithARPCoordinateBaseOnRefPoint(lat,lont);
			if (dValue >= dMinDirCos)
			{
				pSIDRoute = pAirRoute;
				dMinDirCos = dValue;
			}
		}
	}

	if(pSIDRoute && !pSIDRoute->IsConnectRunway(nRwyID, nRwyMark))		//if no sid connected to runway,change another connected runway
	{
		int nRwy = pSIDRoute->GetConnectRunwayCount();
		for (int m =0; m < nRwy; m++)
		{
			LogicRunway_Pair logicrunway = pSIDRoute->getLogicRunwayByIdx(m);

			if (pFlight->GetAirTrafficController()->GetRunwayAssignmentStrategy()->IsDepartureRunway(pFlight,logicrunway.first,logicrunway.second))
			{
				pLogicRunway = pResManger->GetAirportResource()->getRunwayResource()->GetLogicRunway(logicrunway.first,(RUNWAY_MARK)logicrunway.second);
				pFlight->SetTakeoffRunway(pLogicRunway);
				break;
			}
		}
	}

	if (pSIDRoute)
	{
		double ClimbSpeed = pFlight->GetPerformance()->getClimboutSpeed(pFlight) ;

		int nID = pSIDRoute->getID();
		pnewRoute->SetName(pSIDRoute->getName());
		AirRouteInSim* pRouteInSim = pResManger->GetAirspaceResource()->GetAirLaneInSimByID(nID);

		pnewRoute->SetAirRouteInSim(pRouteInSim);
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

		if (pLogicRunway)
			pnewRoute->AddItem(pLogicRunway);
		else
		{
			int nRunwayID = pSIDRoute->getLogicRunwayByIdx(0).first ;
			int nRunwayMark = pSIDRoute->getLogicRunwayByIdx(0).second;
			LogicRunwayInSim* pRunway = pResManger->GetAirportResource()->getRunwayResource()->GetLogicRunway(nRunwayID,(RUNWAY_MARK)nRunwayMark);
			if(pRunway)
			{
				pFlight->SetTakeoffRunway(pRunway);
				pnewRoute->AddItem(pRunway);
			}

		}
		nCount = pRouteInSim->GetRouteSegCount();			//init air route segments
		for (int j =0; j < nCount ; j++)
		{
			AirsideResource* pSeg = pRouteInSim->GetSeg(j);
			pnewRoute->AddItem(pSeg);
		}
		return true;

	}

	if (pLogicRunway)
		pnewRoute->AddItem(pLogicRunway);
	else
	{
		RunwayResourceManager* pResRunway = pResManger->GetAirportResource()->getRunwayResource();
		if (pResRunway->GetRunwayCount() <= 0)
			return false;

		LogicRunwayInSim* pRunway = pResRunway->GetRunwayByIndex(0)->GetLogicRunway1();
		pFlight->SetTakeoffRunway(pRunway);
		pnewRoute->AddItem(pRunway);
	}
	return false;
}

bool CSID_STARAssignmentInSim::GetSTARRoute(AirsideFlightInSim* pFlight,AirsideResourceManager* pResManger,FlightRouteInSim*& pnewRoute)
{
	if (pFlight== NULL || pResManger ==NULL)
		return false;

	bool bInit = pFlight->GetAirTrafficController()->GetFlightPlan()->GetSTARFlightPlan(pFlight,pnewRoute);
	if (bInit)
		return true;
	
	delete pnewRoute;
	pnewRoute = new FlightPlanSpecificRouteInSim;

	FlightPlanPropertiesInSim& planProperties = pnewRoute->GetPlanProperties();
	LogicRunwayInSim* pLogicRunway = pFlight->GetLandingRunway();

	FlightStarAssignment* pStarAssignment = NULL;
	std::vector< StarPercentage*> vStarRoute;
	int nCount = m_pFlightSTARAssignmentList->GetSTARCount();
	for (int i = 0; i < nCount; i++)
	{
		pStarAssignment = m_pFlightSTARAssignmentList->GetStarItem(i);
		FlightConstraint fltCnst = pStarAssignment->GetFlightConstraint();
		if (pFlight->fits(fltCnst))
		{
			ElapsedTime tStart = pStarAssignment->GetStartTime() ;
			ElapsedTime tEnd = pStarAssignment->GetEndTime() ;
			if (pFlight->GetTime()  >= tStart && pFlight->GetTime() <= tEnd)
			{
				int nStar = pStarAssignment->GetSTARCount();
				if (pLogicRunway != NULL)	//assigned landing runway
				{
					for (int j = 0; j < nStar; j++ )
					{
						StarPercentage* pStar = pStarAssignment->GetStarItem(j);
						vStarRoute.push_back(pStar);						
					}
				}
				else			//not assigned takeoff runway
				{
					double dRandPercent = double(rand()%100);
					double dPercent = 0.0;
					for ( int j = 0; j< nStar; j++ )
					{
						StarPercentage* pStar = pStarAssignment->GetStarItem(j);
						if (dRandPercent>= dPercent && dRandPercent < dPercent + pStar->GetPercent())
						{
							vStarRoute.push_back(pStar);
							break;
						}
						dPercent += pStar->GetPercent();
					}
				}			
			}
			break;
		}
	}

	CAirRoute* pStarRoute = NULL;
	int nRwyID = -1;
	int nRwyMark = 0;
	if (pLogicRunway)
	{
		nRwyID = pLogicRunway->GetRunwayInSim()->GetRunwayInput()->getID();
		nRwyMark = pLogicRunway->getLogicRunwayType();
	}

	CAirportCode ARP = pFlight->GetFlightInput()->getOrigin();
	CLatitude lat;
	CLongitude lont;
	_g_GetActiveAirportMan(m_pAirportDB)->GetAirportCoordinates(ARP.GetValue(),lat,lont);

	AirRouteList& _RouteList = pResManger->GetAirspaceResource()->GetAirRouteList(); 
	if (vStarRoute.empty())	//not assign sid
	{
		
		{
			std::vector<CAirRoute*> vAirRoutes;
			int nSize = _RouteList.GetAirRouteCount();
			for (int i = 0; i < nSize; i ++)
			{
				CAirRoute* pAirRoute = _RouteList.GetAirRoute(i);
				if (pAirRoute->getRouteType() != CAirRoute::STAR)
					continue;

				if (nRwyID < 0)		//without runway assign, get arbitrary one
				{
					vAirRoutes.push_back(pAirRoute);
				}
				else			//if with runway assign, assign a star connected with the runway
				{
					int nRwy = pAirRoute->GetConnectRunwayCount();
					for (int j = 0; j < nRwy; j++)
					{
						if (pAirRoute->IsConnectRunway(nRwyID, nRwyMark))
							vAirRoutes.push_back(pAirRoute);
					}
				}

			}

			//According to direction to find sid
			int nSTARCount = vAirRoutes.size();
			double dMinDirCos = -1;
			for (int i = 0; i < nSTARCount; i++)
			{
				CAirRoute* pAirRoute = vAirRoutes.at(i);
				double dValue = pAirRoute->GetAngleCosValueWithARPCoordinateBaseOnRefPoint(lat,lont);
				if (dValue >= dMinDirCos)
				{
					pStarRoute = pAirRoute;
					dMinDirCos = dValue;
				}
			}
		}
	}
	else
	{
		int nSize = (int)vStarRoute.size();
		//if (nSize == 1)
		//{
		//	CAirRoute* pRoute = _RouteList.GetAirRouteByID(vStarRoute.at(0)->GetStarID());
		//	ASSERT(pRoute);		//error

		//	if (pRoute && (nRwyID < 0 || (nRwyID >=0 && pRoute->IsConnectRunway(nRwyID, nRwyMark))))
		//		pStarRoute = pRoute;		
		//}
		//else
		{
			double dRandPercent = double(rand()%100);
			double dPercent = 0.0;

			for (int n =0; n < nSize; n++)
			{
				StarPercentage* pStar = vStarRoute.at(n);
				CAirRoute* pRoute = _RouteList.GetAirRouteByID(pStar->GetStarID());
				if (pRoute)
				{					
					if (nRwyID < 0 || (nRwyID >=0 && pRoute->IsConnectRunway(nRwyID, nRwyMark) && pStar->GetPercent()>0) )
					{
						pStarRoute = pRoute;
					}		
					dPercent += pStar->GetPercent();
					if (dPercent>dRandPercent && pStarRoute!=NULL)
						break;
									
				}

			}
		}
	}
	
	if (pFlight->IsItinerantFlight())//filter the star route
	{
		AirsideItinerantFlightInSim *pItiFlight = reinterpret_cast<AirsideItinerantFlightInSim *>(pFlight);
		if (pItiFlight)
		{
			for (size_t nStar = 0; nStar< vStarRoute.size(); ++ nStar)
			{	
				StarPercentage* pStar = vStarRoute.at(nStar);
				CAirRoute* pRoute = _RouteList.GetAirRouteByID(pStar->GetStarID());

				if (pStar== NULL || pRoute == NULL)
					continue;

				if(pRoute->IsWaypointExists(pItiFlight->GetExitWaypointID()))
				{
					pStarRoute = pRoute;

					int nRunwayID = pStarRoute->getLogicRunwayByIdx(0).first ;
					int nRunwayMark = pStarRoute->getLogicRunwayByIdx(0).second;
					pLogicRunway = pResManger->GetAirportResource()->getRunwayResource()->GetLogicRunway(nRunwayID,(RUNWAY_MARK)nRunwayMark);
					pFlight->SetLandingRunway(pLogicRunway);

					break;
				}
			}
		}
	}

	if (pStarRoute == NULL && pResManger->GetAirspaceResource()->HasSTAR())	// error assignment
	{
		CString strDetals = _T("STAR Assignment Conflict With Landing Runway Assignment!");
		CString strError = _T("DEFINE ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strDetals,strError);
	}

	if(pStarRoute == NULL && !vStarRoute.empty())		//if no star connected to runway,change another connected runway
	{
		for (size_t j =0; j < vStarRoute.size(); j++)
		{
			CAirRoute* pRoute = _RouteList.GetAirRouteByID(vStarRoute.at(j)->GetStarID());
			int nRwy = pRoute->GetConnectRunwayCount();
			for (int m =0; m < nRwy; m++)
			{
				LogicRunway_Pair Logicrunway = pRoute->getLogicRunwayByIdx(m);

				if (pFlight->GetAirTrafficController()->GetRunwayAssignmentStrategy()->IsArrivalRunway(pFlight,Logicrunway.first,Logicrunway.second))
				{
					pStarRoute = pRoute;
					pLogicRunway = pResManger->GetAirportResource()->getRunwayResource()->GetLogicRunway(Logicrunway.first,(RUNWAY_MARK)Logicrunway.second);
					pFlight->SetLandingRunway(pLogicRunway);
					break;
				}
			}
		}
	}

	if(pStarRoute && !pStarRoute->IsConnectRunway(nRwyID, nRwyMark))		//if star not connected to runway,change another connected runway
	{
		int nRwy = pStarRoute->GetConnectRunwayCount();
		for (int m =0; m < nRwy; m++)
		{
			LogicRunway_Pair logicrunway = pStarRoute->getLogicRunwayByIdx(m);

			if (pFlight->GetAirTrafficController()->GetRunwayAssignmentStrategy()->IsArrivalRunway(pFlight,logicrunway.first,logicrunway.second))
			{
				pLogicRunway = pResManger->GetAirportResource()->getRunwayResource()->GetLogicRunway(logicrunway.first,(RUNWAY_MARK)logicrunway.second);
				pFlight->SetLandingRunway(pLogicRunway);
				break;
			}
		}
	}

	if (pStarRoute )
	{
		//pSTAR->CalWaypointExtentPoint();

		int nID = pStarRoute->getID();
		pnewRoute->SetName(pStarRoute->getName());
		AirRouteInSim* pRouteInSim = pResManger->GetAirspaceResource()->GetAirLaneInSimByID(nID);
		pnewRoute->SetAirRouteInSim(pRouteInSim);

		int nCount = pRouteInSim->GetRoutePointCount();			//init route point property
		for (int i =0; i < nCount; i++)
		{
			AirRoutePointInSim * pPoint = pRouteInSim->GetRoutePointByIdx(i);
			FlightPlanPropertyInSim property(pPoint);
			property.SetAlt((pPoint->GetAltHigh() + pPoint->GetAltLow())/2.0) ;
			property.SetArrival();
			if (i == nCount -1)
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
			pnewRoute->AddItem(pSeg);
		}

		if (pLogicRunway)
			pnewRoute->AddItem(pLogicRunway);
		else
		{
			int nRunwayID = pStarRoute->getLogicRunwayByIdx(0).first ;
			int nRunwayMark = pStarRoute->getLogicRunwayByIdx(0).second;
			LogicRunwayInSim* pRunway = pResManger->GetAirportResource()->getRunwayResource()->GetLogicRunway(nRunwayID,(RUNWAY_MARK)nRunwayMark);
			if (pRunway)
			{
				pFlight->SetLandingRunway(pRunway);
				pnewRoute->AddItem(pRunway);
			}

		}

		return true;

	}

	if (pLogicRunway)
		pnewRoute->AddItem(pLogicRunway);
	else
	{
		RunwayResourceManager* pResRunway = pResManger->GetAirportResource()->getRunwayResource();
		if (pResRunway->GetRunwayCount() <= 0)
			return false;

		LogicRunwayInSim* pRunway = pResRunway->GetRunwayByIndex(0)->GetLogicRunway1();
		pFlight->SetLandingRunway(pRunway);
		pnewRoute->AddItem(pRunway);
	}


	return false;

}

bool CSID_STARAssignmentInSim::GetLandingCircuitRoute( AirsideFlightInSim* pFlight, AirsideResourceManager* pResManger,FlightRouteInSim*& pnewRoute )
{
	if (pFlight== NULL || pResManger ==NULL)
		return false;

	bool bInit = pFlight->GetAirTrafficController()->GetFlightPlan()->GetLandingCircuitFlightPlan(pFlight,pnewRoute);
	if (bInit)
		return true;

	delete pnewRoute;
	pnewRoute = new FlightPlanSpecificRouteInSim;

	FlightPlanPropertiesInSim& planProperties = pnewRoute->GetPlanProperties();
	LogicRunwayInSim* pLogicRunway = pFlight->GetLandingRunway();

	CAirRoute* pStarRoute = NULL;
	int nRwyID = -1;
	int nRwyMark = 0;
	if (pLogicRunway)
	{
		nRwyID = pLogicRunway->GetRunwayInSim()->GetRunwayInput()->getID();
		nRwyMark = pLogicRunway->getLogicRunwayType();
	}

	CAirportCode ARP = pFlight->GetFlightInput()->getOrigin();
	CLatitude lat;
	CLongitude lont;
	_g_GetActiveAirportMan(m_pAirportDB)->GetAirportCoordinates(ARP.GetValue(),lat,lont);

	AirRouteList& _RouteList = pResManger->GetAirspaceResource()->GetAirRouteList(); 
	
	std::vector<CAirRoute*> vAirRoutes;
	int nSize = _RouteList.GetAirRouteCount();
	for (int i = 0; i < nSize; i ++)
	{
		CAirRoute* pAirRoute = _RouteList.GetAirRoute(i);
		if (pAirRoute->getRouteType() != CAirRoute::CIRCUIT)
			continue;

		if (nRwyID < 0)		//without runway assign, get arbitrary one
		{
			vAirRoutes.push_back(pAirRoute);
		}
		else			//if with runway assign, assign a star connected with the runway
		{
			int nRwy = pAirRoute->GetConnectRunwayCount();
			for (int j = 0; j < nRwy; j++)
			{
				if (pAirRoute->IsConnectRunway(nRwyID, nRwyMark))
					vAirRoutes.push_back(pAirRoute);
			}
		}

	}

	//According to direction to find sid
	int nSTARCount = vAirRoutes.size();
	double dMinDirCos = -1;
	for (int i = 0; i < nSTARCount; i++)
	{
		CAirRoute* pAirRoute = vAirRoutes.at(i);
		double dValue = pAirRoute->GetAngleCosValueWithARPCoordinateBaseOnRefPoint(lat,lont);
		if (dValue >= dMinDirCos)
		{
			pStarRoute = pAirRoute;
			dMinDirCos = dValue;
		}
	}
	
	if(pStarRoute && !pStarRoute->IsConnectRunway(nRwyID, nRwyMark))		//if star not connected to runway,change another connected runway
	{
		int nRwy = pStarRoute->GetConnectRunwayCount();
		for (int m =0; m < nRwy; m++)
		{
			LogicRunway_Pair logicrunway = pStarRoute->getLogicRunwayByIdx(m);

			if (pFlight->GetAirTrafficController()->GetRunwayAssignmentStrategy()->IsArrivalRunway(pFlight,logicrunway.first,logicrunway.second))
			{
				pLogicRunway = pResManger->GetAirportResource()->getRunwayResource()->GetLogicRunway(logicrunway.first,(RUNWAY_MARK)logicrunway.second);
				pFlight->SetLandingRunway(pLogicRunway);
				break;
			}
		}
	}

	if (pStarRoute )
	{
		int nID = pStarRoute->getID();
		pnewRoute->SetName(pStarRoute->getName());
		AirRouteInSim* pRouteInSim = pResManger->GetAirspaceResource()->GetAirLaneInSimByID(nID);
		pnewRoute->SetAirRouteInSim(pRouteInSim);

		int nCount = pRouteInSim->GetRoutePointCount();			//init route point property
		for (int i =0; i < nCount; i++)
		{
			AirRoutePointInSim * pPoint = pRouteInSim->GetRoutePointByIdx(i);
			FlightPlanPropertyInSim property(pPoint);
			property.SetAlt((pPoint->GetAltHigh() + pPoint->GetAltLow())/2.0) ;
			property.SetArrival();
			if (i == nCount -1)
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
		//find heading segment
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
			pnewRoute->AddItem(pSeg);
		}

		if (pLogicRunway)
			pnewRoute->AddItem(pLogicRunway);
		else
		{
			int nRunwayID = pStarRoute->getLogicRunwayByIdx(0).first ;
			int nRunwayMark = pStarRoute->getLogicRunwayByIdx(0).second;
			LogicRunwayInSim* pRunway = pResManger->GetAirportResource()->getRunwayResource()->GetLogicRunway(nRunwayID,(RUNWAY_MARK)nRunwayMark);
			if (pRunway)
			{
				pFlight->SetLandingRunway(pRunway);
				pnewRoute->AddItem(pRunway);
			}

		}

		return true;

	}

	if (pLogicRunway)
		pnewRoute->AddItem(pLogicRunway);
	else
	{
		RunwayResourceManager* pResRunway = pResManger->GetAirportResource()->getRunwayResource();
		if (pResRunway->GetRunwayCount() <= 0)
			return false;

		LogicRunwayInSim* pRunway = pResRunway->GetRunwayByIndex(0)->GetLogicRunway1();
		pFlight->SetLandingRunway(pRunway);
		pnewRoute->AddItem(pRunway);
	}


	return false;
}

bool CSID_STARAssignmentInSim::GetTakeoffCircuitRoute( AirsideFlightInSim* pFlight, AirsideResourceManager* pResManger,FlightRouteInSim*& pnewRoute )
{
	if (pFlight== NULL || pResManger ==NULL )
		return false;
	//flight plan defined
	bool bInit = pFlight->GetAirTrafficController()->GetFlightPlan()->GetTakeoffCircuitFlightPlan(pFlight,pnewRoute);
	if (bInit)
		return true;

	//uses SID assignment
	delete pnewRoute;
	pnewRoute =new FlightPlanSpecificRouteInSim;
	FlightPlanPropertiesInSim& planProperties = pnewRoute->GetPlanProperties();	

	LogicRunwayInSim* pLogicRunway = pFlight->GetAndAssignTakeoffRunway();
	AirRouteList& _RouteList = pResManger->GetAirspaceResource()->GetAirRouteList(); 

	CAirRoute* pCircuitRoute = NULL;
	int nRwyID = -1;
	int nRwyMark = 0;
	if (pLogicRunway)
	{
		nRwyID = pLogicRunway->GetRunwayInSim()->GetRunwayInput()->getID();
		nRwyMark = pLogicRunway->getLogicRunwayType();
	}

	std::vector< SIDPercentage*> vSIDRoute;


	CAirportCode ARP = pFlight->GetFlightInput()->getDestination();
	CLatitude lat;
	CLongitude lont;
	_g_GetActiveAirportMan(m_pAirportDB)->GetAirportCoordinates(ARP.GetValue(),lat,lont);

	int nSize = _RouteList.GetAirRouteCount();
	std::vector<CAirRoute*> vAirRoutes;
	for (int i = 0; i < nSize; i ++)
	{
		CAirRoute* pAirRoute = _RouteList.GetAirRoute(i);
		if (pAirRoute->getRouteType() != CAirRoute::CIRCUIT)
			continue;

		if (nRwyID < 0)		//without runway assign, get arbitrary one
		{
			vAirRoutes.push_back(pAirRoute);
		}
		else			//if with runway assign, assign a sid connected with the runway
		{
			int nRwy = pAirRoute->GetConnectRunwayCount();
			for (int j = 0; j < nRwy; j++)
			{
				if (pAirRoute->IsConnectRunway(nRwyID, nRwyMark) )
					vAirRoutes.push_back(pAirRoute);
			}
		}

	}

	//According to direction to find sid
	int nSIDCount = vAirRoutes.size();
	double dMinDirCos = -1;
	for (int i = 0; i < nSIDCount; i++)
	{
		CAirRoute* pAirRoute = vAirRoutes.at(i);
		double dValue = pAirRoute->GetAngleCosValueWithARPCoordinateBaseOnRefPoint(lat,lont);
		if (dValue >= dMinDirCos)
		{
			pCircuitRoute = pAirRoute;
			dMinDirCos = dValue;
		}
	}
	

	if(pCircuitRoute && !pCircuitRoute->IsConnectRunway(nRwyID, nRwyMark))		//if no sid connected to runway,change another connected runway
	{
		int nRwy = pCircuitRoute->GetConnectRunwayCount();
		for (int m =0; m < nRwy; m++)
		{
			LogicRunway_Pair logicrunway = pCircuitRoute->getLogicRunwayByIdx(m);

			if (pFlight->GetAirTrafficController()->GetRunwayAssignmentStrategy()->IsDepartureRunway(pFlight,logicrunway.first,logicrunway.second))
			{
				pLogicRunway = pResManger->GetAirportResource()->getRunwayResource()->GetLogicRunway(logicrunway.first,(RUNWAY_MARK)logicrunway.second);
				pFlight->SetTakeoffRunway(pLogicRunway);
				break;
			}
		}
	}

	if (pCircuitRoute)
	{
		double ClimbSpeed = pFlight->GetPerformance()->getClimboutSpeed(pFlight) ;

		int nID = pCircuitRoute->getID();
		pnewRoute->SetName(pCircuitRoute->getName());
		AirRouteInSim* pRouteInSim = pResManger->GetAirspaceResource()->GetAirLaneInSimByID(nID);

		pnewRoute->SetAirRouteInSim(pRouteInSim);
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

		if (pLogicRunway)
			pnewRoute->AddItem(pLogicRunway);
		else
		{
			int nRunwayID = pCircuitRoute->getLogicRunwayByIdx(0).first ;
			int nRunwayMark = pCircuitRoute->getLogicRunwayByIdx(0).second;
			LogicRunwayInSim* pRunway = pResManger->GetAirportResource()->getRunwayResource()->GetLogicRunway(nRunwayID,(RUNWAY_MARK)nRunwayMark);
			if(pRunway)
			{
				pFlight->SetTakeoffRunway(pRunway);
				pnewRoute->AddItem(pRunway);
			}

		}
		nCount = pRouteInSim->GetRouteSegCount();			//init air route segments
		for (int j =0; j < nCount - 1 ; j++)
		{
			AirsideResource* pSeg = pRouteInSim->GetSeg(j);
			if (pSeg->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
				break;
			pnewRoute->AddItem(pSeg);
		}
		return true;

	}

	if (pLogicRunway)
		pnewRoute->AddItem(pLogicRunway);
	else
	{
		RunwayResourceManager* pResRunway = pResManger->GetAirportResource()->getRunwayResource();
		if (pResRunway->GetRunwayCount() <= 0)
			return false;

		LogicRunwayInSim* pRunway = pResRunway->GetRunwayByIndex(0)->GetLogicRunway1();
		pFlight->SetTakeoffRunway(pRunway);
		pnewRoute->AddItem(pRunway);
	}
	return false;
}

