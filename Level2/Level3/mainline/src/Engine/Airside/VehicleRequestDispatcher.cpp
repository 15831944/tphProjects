#include "StdAfx.h"
#include ".\vehiclerequestdispatcher.h"
#include "SimulationErrorShow.h"
#include "VehiclePoolResourceManager.h"
#include "VehiclePoolsManagerInSim.h"
#include "VehicleServiceRequest.h"
#include "VehiclePoolInSim.h"
#include "FlightServiceReqirementInSim.h"
#include "AirsideFlightInSim.h"
#include "common/EngineDiagnoseEx.h"
#include "../SimulationDiagnoseDelivery.h"
#include "InputAirside/ServicingRequirement.h"
#include "PaxBusServiceRequest.h"
#include "PaxBusParkingResourceManager.h"
#include ".\DeiceVehicleServiceRequest.h"
#include "FollowMeCarConnectionInSim.h"
#include "AirsideFollowMeCarInSim.h"
#include "StandInSim.h"
#include "../../InputAirside/FollowMeConnectionData.h"
#include "TaxiRouteInSim.h"

VehicleRequestDispatcher::VehicleRequestDispatcher(CAirportDatabase* pAirportDB)
:m_bVehicleService(true)
,m_pPoolResManager(NULL)
,m_pPoolsDeployment(NULL)
,m_pFlightServiceRequirement(NULL)
,m_pPaxParkingManager(NULL)
{
	m_pFollowMeCarConnection = new FollowMeCarConnectionInSim(pAirportDB);
}

VehicleRequestDispatcher::~VehicleRequestDispatcher(void)
{
	m_pFlightServiceRequirement = NULL;

	m_vUnhandledList.clear();
	m_pPoolsDeployment = NULL;
	m_pPoolResManager = NULL;

	delete m_pFollowMeCarConnection;
	m_pFollowMeCarConnection = NULL;
}

void VehicleRequestDispatcher::AddServiceRequest(AirsideFlightInSim* pFlight)
{
	std::vector<VehicleServiceRequest*> vRequests;
	m_pFlightServiceRequirement->GetFlightGeneralVehicleServicingRequirement(pFlight,vRequests);
	for(int i=0;i<(int)vRequests.size();i++)
	{
		VehicleServiceRequest* pRequest = vRequests.at(i);
		if( IsRequestCanBeHandled(pRequest) )
			AddServiceRequest(pRequest);
		else
		{
			CString strVehicleType = m_pPoolsDeployment->GetVehicleTypeByID(pRequest->GetServiceVehicleTypeID());
			CString strWarn;
			strWarn.Format("%s:There is no definition for the vehicle in vehicle pool deployment,the flight cannot be serviced",strVehicleType);
			CString strError = _T("DEFINITION ERROR");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);		
		}
	}
}

void VehicleRequestDispatcher::AddTowingServiceRequest(TowTruckServiceRequest* pRequest)
{
	m_vTowTruckServiceRequest.push_back(pRequest);
}

bool VehicleRequestDispatcher::AddServiceRequest( VehicleServiceRequest* pServiceReq )
{	
	m_vUnhandledList.insert(pServiceReq);
	pServiceReq->GetServiceFlight()->AddVehicleServiceRequest(pServiceReq);
	return true;	
}


void VehicleRequestDispatcher::AddPaxBusServiceRequest(AirsideFlightInSim* pFlight,bool bArrival)
{

	//m_pFlightServiceRequirement->GetFlightGeneralVehicleServicingRequirement(pFlight,m_vServiceRequestList);

	//std::vector<CVehicleSpecificationItem * > vVehicleTypeSpec;
	std::vector<CServicingRequirement* > vServiceRequirement;
	m_pFlightServiceRequirement->GetAvailableVehicleTypeByBaseType(pFlight,VehicleType_PaxTruck,vServiceRequirement);
	if (!vServiceRequirement.empty())
	{
		CPaxBusServiceRequest* paxBusRequest = new CPaxBusServiceRequest;
		paxBusRequest->SetServiceFlight(pFlight);
		paxBusRequest->SetArrival(bArrival);
		//paxBusRequest->SetVehicleBaseType(VehicleType_PaxTruck);
		int nPaxCount  =  0 ;
		if(bArrival)
			nPaxCount = pFlight->GetFlightInput()->getArrLoad();
		else
			nPaxCount = pFlight->GetFlightInput()->getDepLoad() ;
		paxBusRequest->SetPaxCount(nPaxCount);
		m_vPaxBusServiceRequest.push_back(paxBusRequest);

		pFlight->AddVehicleServiceRequest(paxBusRequest);
	}

	//std::sort(m_vPaxBusServiceRequest.begin(),m_vPaxBusServiceRequest.end());
}

void VehicleRequestDispatcher::TowTruckServiceRequestDispatch()
{
	int nReqCount = m_vTowTruckServiceRequest.size();

	for (int i = 0; i <nReqCount; i++)
	{
		TowTruckServiceRequest* pRequest = m_vTowTruckServiceRequest.at(i);
		std::vector<int> vIDs;
		vIDs = m_pFlightServiceRequirement->GetVehicleTypeIDByBaseType(VehicleType_TowTruck);
		AirsideFlightInSim* pFlight = pRequest->GetServiceFlight();

		if (vIDs.empty())
		{
			CString strVehicleType = "Tow Truck";
			CString strWarn;
			strWarn.Format("There is no definition for the type of tow truck in Vehicle Specification for the flight Push&Tow operation ");
			CString strError = _T("DEFINITION ERROR");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);	
			DeleteRequest(pRequest);
			nReqCount--;
			continue;
		}

		int nCount = vIDs.size();
		bool bExistTowTruckPool = false;
		for (int i = 0; i < nCount; i++)
		{
			int nVehicleID = vIDs.at(i);
			std::vector<int> vPoolIDs;
			vPoolIDs.clear();
			m_pPoolsDeployment->GetTowTruckServicePool(pFlight,nVehicleID,vPoolIDs);
			if (vPoolIDs.empty())
				continue;

			bExistTowTruckPool = true;
			pRequest->SetServiceVehicleTypeID(nVehicleID);
			if (IsTowingRequestHandledByPools(pRequest,vPoolIDs))
			{
				pRequest->SetProceed(true);
				DeleteRequest(pRequest);
				nReqCount--;
				break;
			}

		}
		if (!bExistTowTruckPool)
		{
			//CString strWarn = _T("There is no definition for the type of tow truck in Vehicle Pool Deployment,the flight cannot be serviced");
			//CString strError = _T("DEFINITION ERROR");
			//AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);	
			DeleteRequest(pRequest);
			nReqCount--;
			
		}
	}
}

bool VehicleRequestDispatcher::AvailablePaxServiceFlight(AirsideFlightInSim* pFlight,char mode)
{
	std::vector<int> vIDs;
	if (!m_pFlightServiceRequirement)
	{
		return false;
	}
	vIDs = m_pFlightServiceRequirement->GetVehicleTypeIDByBaseType(VehicleType_PaxTruck);

	if (vIDs.empty())
		return false;

	int nCount = vIDs.size();
	bool bExistTowTruckPool = false;
	for (int i = 0; i < nCount; i++)
	{
		int nVehicleID = vIDs.at(i);
		std::vector<int> vPoolIDs;
		vPoolIDs.clear();
		m_pPoolsDeployment->GetPaxServicePool(pFlight,nVehicleID,mode,vPoolIDs);
		if (vPoolIDs.empty())
			continue;

		bExistTowTruckPool = true;
	}

	return bExistTowTruckPool;
}

bool VehicleRequestDispatcher::HasVehicleServiceFlight(AirsideFlightInSim* pFlight, enumVehicleBaseType vehicleType)
{
	std::vector<int> vIDs;
	vIDs = m_pFlightServiceRequirement->GetVehicleTypeIDByBaseType(vehicleType);

	if (vIDs.empty())
		return false;

	int nCount = vIDs.size();
	bool bExistTowTruckPool = false;
	for (int i = 0; i < nCount; i++)
	{
		int nVehicleID = vIDs.at(i);
		std::vector<int> vPoolIDs;
		vPoolIDs.clear();
		m_pPoolsDeployment->GetServicePool(pFlight,nVehicleID,vPoolIDs);
		if (vPoolIDs.empty())
			continue;

		return true;
	}
	return false;
}

bool VehicleRequestDispatcher::HasTowTruckServiceFlight( AirsideFlightInSim* pFlight )
{
	std::vector<int> vIDs;
	vIDs = m_pFlightServiceRequirement->GetVehicleTypeIDByBaseType(VehicleType_TowTruck);

	if (vIDs.empty())
		return false;

	int nCount = vIDs.size();
	bool bExistTowTruckPool = false;
	for (int i = 0; i < nCount; i++)
	{
		int nVehicleID = vIDs.at(i);
		std::vector<int> vPoolIDs;
		vPoolIDs.clear();
		m_pPoolsDeployment->GetTowTruckServicePool(pFlight,nVehicleID,vPoolIDs);
		if (vPoolIDs.empty())
			continue;

		bExistTowTruckPool = true;
		break;
	}

	return bExistTowTruckPool;
}

bool VehicleRequestDispatcher::IsTowingRequestHandledByPools(TowTruckServiceRequest* pRequest, std::vector<int>& vPoolIDs)
{
	int nPoolCount = vPoolIDs.size();
	for (int j = 0; j < nPoolCount; j++)
	{
		VehiclePoolInSim* pPool =  m_pPoolResManager->GetVehiclePool(vPoolIDs.at(j));
		if (pPool)
		{
			if (pPool->HandleServiceRequest(pRequest))
				return true;

			pPool->CallVehicleReturnPool();

		}
	}
	return false;
}

void VehicleRequestDispatcher::DeleteRequest(VehicleServiceRequest* pRequest)
{
	if (pRequest->GetType() == VehicleType_TowTruck)
	{
		std::vector<TowTruckServiceRequest*>::iterator iter = std::find(m_vTowTruckServiceRequest.begin(), m_vTowTruckServiceRequest.end(),pRequest);
		if (iter != m_vTowTruckServiceRequest.end())
		{
			m_vTowTruckServiceRequest.erase(iter);
		}
		delete pRequest;
		pRequest = NULL;
	}
	else
	{
		VehicleServiceRequestList::iterator itrFind = std::find(m_vUnhandledList.begin(),m_vUnhandledList.end(),pRequest);
		if(itrFind!=m_vUnhandledList.end())
		{
			delete pRequest;
			m_vUnhandledList.erase(itrFind);

		}		
	}
}

bool VehicleRequestDispatcher::UnDispatchTowTruckForFlight(AirsideFlightInSim* pFlight)
{
	int nUnAssignedCount = m_vTowTruckServiceRequest.size();
	for (int i = 0; i < nUnAssignedCount; i++)
	{
		if (m_vTowTruckServiceRequest[i]->GetServiceFlight() == pFlight)
			return true;
	}

	return false;
}

void VehicleRequestDispatcher::PaxBusServiceRequestDispatch()
{
	//get pax bus service count
	if (m_vPaxBusServiceRequest.empty())
		return;

	//get the pool 
	for (int i =0; i < int(m_vPaxBusServiceRequest.size()); i++)
	{
		CPaxBusServiceRequest* request = (CPaxBusServiceRequest *)m_vPaxBusServiceRequest[i];
		AirsideFlightInSim* pFlight = request->GetServiceFlight();

		if (pFlight->GetMode() == OnTerminate)
		{
			continue;
		}

		//enumVehicleBaseType vehicleBaseType = request->GetVehiclebaseType();
	
		//get available vehicle Type
		std::vector<CVehicleSpecificationItem * > vVehicleTypeSpec;
		 std::vector<CServicingRequirement* > vServiceRequirement;
		m_pFlightServiceRequirement->GetAvailableVehicleTypeByBaseType(pFlight,VehicleType_PaxTruck,/*vVehicleTypeSpec,*/vServiceRequirement);
		
		//have vehicle service request
		size_t nAvailableVehicleTypeCount = vServiceRequirement.size();
		if(nAvailableVehicleTypeCount == 0)
		{
			//drawing warning
			request->ServicedPaxCount(request->GetAllPaxCount());
			request->SetProceed(true);
			m_vPaxBusServiceRequest.erase(m_vPaxBusServiceRequest.begin() + i);
			i=i-1;
			continue;
		}
		
		//check the flight has parking place
		{
			CPaxBusParkingInSim *pPaxBusparkingInSim = NULL;
			

			if (request->IsArrival())
			{	
				if(request->GetServiceFlight() && request->GetServiceFlight()->GetFlightInput())
				{
					CString strDepGate = request->GetServiceFlight()->getArrGateInSim();
					pPaxBusparkingInSim = m_pPaxParkingManager->GetBestMatch(strDepGate,arrgate);
					
					if(pPaxBusparkingInSim==NULL){
						CString strError;
						strError.Format(_T("PaxBus Can not find Parking place for Arr Gate(%s), can not start Service"),strDepGate );
						AirsideSimErrorShown::SimWarning(pFlight,strError, _T("Define Error") );
					}
				}
			}
			else
			{	
				if(request->GetServiceFlight() && request->GetServiceFlight()->GetFlightInput())
				{
					CString strDepGate = pFlight->getDepGateInSim();
					pPaxBusparkingInSim = m_pPaxParkingManager->GetBestMatch(strDepGate,depgate);
					if(pPaxBusparkingInSim==NULL){
						CString strError;
						strError.Format(_T("PaxBus Can not find Parking place for Dep Gate(%s), can not start Service"),strDepGate );
						AirsideSimErrorShown::SimWarning(pFlight,strError, _T("Define Error") );
					}
				}
			}
			
			//delete this request, it is invalid
			if (pPaxBusparkingInSim == NULL)
			{
				
				request->ServicedPaxCount(request->GetLeftPaxCount());
				request->SetProceed(true);
				m_vPaxBusServiceRequest.erase(m_vPaxBusServiceRequest.begin() + i);
				i=i-1;
				if (i < 0)
					i = 0;
				continue;
			}

		}

		for (size_t nType = 0; nType < nAvailableVehicleTypeCount; ++ nType)
		{
			
			//CVehicleSpecificationItem *pTypeSpecItem = vVehicleTypeSpec[nType];
			CServicingRequirement* pRequirement = vServiceRequirement.at(nType);

			int nVehicleTypeID = pRequirement->GetServicingRequirementNameID();
			std::vector<int> vServicePoolList;
			vServicePoolList.clear();

			request->SetServiceVehicleTypeID(nVehicleTypeID);
			request->SetServiceTime(ProbabilityDistribution::CopyProbDistribution(vServiceRequirement[nType]->GetServiceTimeDistribution()));

			m_pPoolsDeployment->GetServicePool(pFlight, nVehicleTypeID, vServicePoolList);
			
			bool bFinishedDispatch = false;
			for (int j =0; j < int(vServicePoolList.size()); j++)
			{
				VehiclePoolInSim* pPool = m_pPoolResManager->GetVehiclePool(vServicePoolList[j]);
				if (pPool)
				{
					if (pPool->HandlePaxBusServiceRequest(request))
					{
						if(request->GetLeftPaxCount() == 0)
						{
							request->SetProceed(true);
							bFinishedDispatch = true;
							break;
						}

					}
					else
					{
						pPool->CallVehicleReturnPool();
					}
				}
			}
			if(bFinishedDispatch)
			{
				m_vPaxBusServiceRequest.erase(m_vPaxBusServiceRequest.begin() + i);
				i=i-1;		
				if (i < 0)
					i = 0;
				break;
			}
		}
	}
	
}
void VehicleRequestDispatcher::ServiceRequestDispatch()
{
	if (m_vUnhandledList.empty())
	{
		for (int i =0; i < m_pPoolResManager->GetVehiclePoolCount(); i++)
		{
			m_pPoolResManager->GetVehiclePoolByIdx(i)->CallVehicleReturnPool();
		}
		return ;
	}

	
	ElapsedTime currentTime;
	if (!m_vUnhandledList.empty())
	{
		VehicleServiceRequest* request = *m_vUnhandledList.rbegin();
		AirsideFlightInSim* pLastFlight = request->GetServiceFlight();
		currentTime = pLastFlight->GetTime();
	}	


	for(VehicleServiceRequestList::iterator itr = m_vUnhandledList.begin();itr!=m_vUnhandledList.end();itr++)
	{
		VehicleServiceRequest* pRequest = *itr;
		AirsideFlightInSim* pFlight = pRequest->GetServiceFlight();
		if(pFlight->GetMode() == OnTerminate )
		{
			continue;
		}
		int nVehicleTypeID = pRequest->GetServiceVehicleTypeID();
		

		/*std::vector<int> vServicePoolList;		
		m_pPoolsDeployment->GetServicePool(pFlight, nVehicleTypeID, vServicePoolList);*/

		int j;
		for (j =0; j < int(m_pPoolResManager->GetVehiclePoolCount()); j++)
		{
			VehiclePoolInSim* pPool =  m_pPoolResManager->GetVehiclePoolByIdx(j);
			if (pPool)
			{
				if (pPool->HandleServiceRequest(pRequest))
				{
					pRequest->SetProceed(true);
					m_vUnhandledList.erase(itr);
					break;
				}
				else
				{
					pPool->CallVehicleReturnPool();
				}
			}
		}
		if(j != m_pPoolResManager->GetVehiclePoolCount() )
			break;
	}

	//call all vehicle turn back
	for(int i =0 ; i < m_pPoolResManager->GetVehiclePoolCount(); i++)
	{
		VehiclePoolInSim * pPool = m_pPoolResManager->GetVehiclePoolByIdx(i);
		pPool->CallVehicleReturnPool();
	}
}

void VehicleRequestDispatcher::SetPaxBusParkingResourceManager( CPaxBusParkingResourceManager *pPaxParkingResourceManager )
{
	m_pPaxParkingManager = pPaxParkingResourceManager;
}

ElapsedTime VehicleRequestDispatcher::getVehicleAvaibleTime( AirsideFlightInSim* pFlight, AirsideResource* pDestRes, enumVehicleBaseType vehicleType )
{
	return ElapsedTime::Max();
}

bool VehicleRequestDispatcher::IsRequestCanBeHandled( VehicleServiceRequest* pServiceReq )
{
	AirsideFlightInSim* pFlight = pServiceReq->GetServiceFlight();
	int nVehicleTypeID = pServiceReq->GetServiceVehicleTypeID();

	std::vector<int> vServicePoolList;	
	m_pPoolsDeployment->GetServicePool(pFlight, nVehicleTypeID, vServicePoolList);

	if (vServicePoolList.empty())
		return false;

	return true;
}


void VehicleRequestDispatcher::AddDeiceServiceRequest( AirsideFlightInSim* pFlight,const FlightDeiceRequirment& deiceRequirment )
{
	if(deiceRequirment.bNeedDeice())
	{
		std::vector<int> vVehicles = m_pFlightServiceRequirement->GetVehicleTypeIDByBaseType(VehicleType_DeicingTruck); //get base type vehicle
		for(int i=0;i<(int)vVehicles.size();i++)
		{
			int nVehicleID = vVehicles.at(i);
			std::vector<int> vServicePoolList;
			m_pPoolsDeployment->GetServicePool(pFlight,nVehicleID,vServicePoolList);
			if(!vServicePoolList.empty())//there is deice truck in the pool  can handle the request
			{
				DeiceVehicleServiceRequest* pRequest = new DeiceVehicleServiceRequest(pFlight,deiceRequirment, pFlight->GetDepTime());
				pRequest->SetServiceVehicleTypeID(nVehicleID);
				AddServiceRequest(pRequest);
				return;
			}
		}		
	}
}

AirsideFollowMeCarInSim* VehicleRequestDispatcher::AssignFollowMeCarForFlight( AirsideFlightInSim* pFlight, StandInSim* pDest )
{
	CFollowMeConnectionStand* pStandItem = m_pFollowMeCarConnection->GetAvailableFollowMeCarConnectionData(pFlight,pDest);
	if (pStandItem == NULL)
		return NULL;

	std::vector<ALTObjectID> vObjNames;
	int nCount = pStandItem->m_MeetPointData.size();
	for (int i =0; i <nCount; i++)
	{
		CString strName = pStandItem->m_MeetPointData.at(i);

		if (strName.CompareNoCase("All") ==0)
		{
			ALTObjectID altObject;
			vObjNames.clear();
			vObjNames.push_back(altObject);
			break;
		}
		
		ALTObjectID altobject(strName);
		vObjNames.push_back(altobject);
	}

	AirsideMeetingPointInSim* pMeetingPoint = NULL;
	if (pFlight->GetTemporaryParking() && pFlight->GetTemporaryParking()->GetParkingType() == TempParkingInSim::TempStand)
	{
		TaxiRouteInSim* pRouteInSim = pFlight->GetRouteToTempParking();
		if (pRouteInSim)
		{
			pMeetingPoint = pFlight->GetRouteToTempParking()->GetFirstMeetingPointInRoute(vObjNames);
		}
	}
	else
	{
		TaxiRouteInSim* pRouteInSim = pFlight->GetRouteToStand();
		if (pRouteInSim)
		{
			pMeetingPoint = pFlight->GetRouteToStand()->GetFirstMeetingPointInRoute(vObjNames);
		}
	}


	if (pMeetingPoint == NULL)
		return NULL;

	std::vector<int> vIDs;
	vIDs = m_pFlightServiceRequirement->GetVehicleTypeIDByBaseType(VehicleType_FollowMeCar);
	nCount = vIDs.size();

	if (nCount == 0)
	{
		CString strVehicleType = "FollowMe Car";
		CString strWarn;
		strWarn.Format("There is no definition for follow-me car in Vehicle pool deployment(GSE)!");
		CString strError = _T("DEFINITION ERROR");
		AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);	
		return NULL;
	}


	for (int i = 0; i < nCount; i++)
	{
		int nVehicleID = vIDs.at(i);
		std::vector<int> vServicePoolList;
		m_pPoolsDeployment->GetFollowMeCarServicePool(pMeetingPoint,nVehicleID,vServicePoolList);	

		int nPoolCount = vServicePoolList.size();
		for (int j =0; j < nPoolCount; j++)
		{
			VehiclePoolInSim* pPool = m_pPoolResManager->GetVehiclePool(vServicePoolList.at(j));
			if (pPool)
			{
				AirsideFollowMeCarInSim* pCar = pPool->getAvailableFollowMeCar();
				if (pCar)
				{
					pCar->SetMeetingPoint(pMeetingPoint);
					pCar->SetAbandonData(pStandItem->GetAbandonmentPoint());
					pCar->SetServiceFlight(pFlight, 0.0);
					return pCar;
				}
			}
		}
	}

	return NULL;
}

void VehicleRequestDispatcher::WakeupAllPaxBusCompleteService(AirsideFlightInSim* pFlight)
{
	for (size_t i = 0; i < m_vPaxBusServiceRequest.size(); i++)
	{
		m_vPaxBusServiceRequest[i]->WakeupAllPaxBusCompleteService(pFlight);
	}
}
