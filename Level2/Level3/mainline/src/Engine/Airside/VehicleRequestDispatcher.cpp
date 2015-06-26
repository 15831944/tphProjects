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
#include "../BaggageTrainServiceRequest.h"
#include "../SimpleConveyor.h"
#include "../Loader.h"
#include "Inputs/MobileElemConstraint.h"
#include "../BagCartsParkingSpotInSim.h"
#include "..\BagCartsParkingSpotResourceManager.h"
#include "..\Pusher.h"
#include "../TERMINAL.H"


VehicleRequestDispatcher::VehicleRequestDispatcher(CAirportDatabase* pAirportDB, ProcessorList* pProcList)
:m_bVehicleService(true)
,m_pPoolResManager(NULL)
,m_pPoolsDeployment(NULL)
,m_pFlightServiceRequirement(NULL)
,m_pPaxParkingManager(NULL)
,m_pTermProcList(pProcList)
,m_pBagCartsSpotResourceManager(NULL)
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
	if(!IsVehicleService())
		return false;
	
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


bool VehicleRequestDispatcher::HasBaggageTrainServiceFlight( AirsideFlightInSim* pFlight,const ElapsedTime& eTime )
{
	std::vector<int> vIDs;
	vIDs = m_pFlightServiceRequirement->GetVehicleTypeIDByBaseType(VehicleType_BaggageTug);

	if (vIDs.empty())
		return false;

	int nCount = vIDs.size();
	bool bExistTowTruckPool = false;
	for (int i = 0; i < nCount; i++)
	{
		int nVehicleID = vIDs.at(i);
		std::vector<int> vPoolIDs;
		vPoolIDs.clear();
		m_pPoolsDeployment->GetBaggageServicePool(pFlight,nVehicleID,vPoolIDs,eTime);
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
		if(m_vUnhandledList.remove(pRequest))
		{
			delete pRequest;
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
			AirsidePaxBusParkSpotInSim *pPaxBusparkingInSim = NULL;
			

			if (request->IsArrival())
			{	
				if(request->GetServiceFlight() && request->GetServiceFlight()->GetFlightInput())
				{
					CString strArrGate = request->GetServiceFlight()->getArrGateInSim();
					pPaxBusparkingInSim = m_pPaxParkingManager->GetBestMatch(strArrGate);
					
					if(pPaxBusparkingInSim)
					{
						pFlight->SetArrivalPaxBusParking(pPaxBusparkingInSim);
					}
					else{
						CString strError;
						strError.Format(_T("PaxBus Can not find Parking place for Arr Gate(%s), can not start Service"),strArrGate );
						AirsideSimErrorShown::SimWarning(pFlight,strError, _T("Define Error") );
					}
				}
			}
			else
			{	
				if(request->GetServiceFlight() && request->GetServiceFlight()->GetFlightInput())
				{
					CString strDepGate = pFlight->getDepGateInSim();
					pPaxBusparkingInSim = m_pPaxParkingManager->GetBestMatch(strDepGate);
					if(pPaxBusparkingInSim)
					{
						pFlight->SetDepPaxBusParking(pPaxBusparkingInSim);
					}
					else{
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
	if (m_vUnhandledList.isEmpty())
	{
		for (int i =0; i < m_pPoolResManager->GetVehiclePoolCount(); i++)
		{
			m_pPoolResManager->GetVehiclePoolByIdx(i)->CallVehicleReturnPool();
		}
		return ;
	}

	
	/*ElapsedTime currentTime;
	if (!m_vUnhandledList.isEmpty())
	{
	VehicleServiceRequest* request = *m_vUnhandledList.rbegin();
	AirsideFlightInSim* pLastFlight = request->GetServiceFlight();
	currentTime = pLastFlight->GetTime();
	}	
	*/

	for(int  i = 0; i<m_vUnhandledList.getCount(); ++i)
	{
		VehicleServiceRequest* pRequest = m_vUnhandledList.getItem(i);
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
					m_vUnhandledList.remove(pRequest);
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
	{
		if (vObjNames.empty() == false)
		{
			CString strWarn;
			strWarn.Format("Meeting point is not in the inbound route!") ;
			CString strError = _T("DEFINE ERROR");
			AirsideSimErrorShown::SimWarning(pFlight,strWarn,strError);	
		}
		return NULL;
	}

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

void VehicleRequestDispatcher::AddBaggageTrainServiceRequest( AirsideFlightInSim* pFlight,FlightOperation enumOperation )
{
	//std::vector<CVehicleSpecificationItem * > vVehicleTypeSpec;
	std::vector<CServicingRequirement* > vServiceRequirement;
	m_pFlightServiceRequirement->GetAvailableVehicleTypeByBaseType(pFlight,VehicleType_BaggageTug,vServiceRequirement);
	if (!vServiceRequirement.empty())
	{
		BaggageTrainServiceRequest* bagTrainRequest = new BaggageTrainServiceRequest;
		bagTrainRequest->SetServiceFlight(pFlight);
		bagTrainRequest->setFltOperation(enumOperation);
		//paxBusRequest->SetVehicleBaseType(VehicleType_PaxTruck);
		int nPaxCount  =  0 ;
		if(enumOperation == ARRIVAL_OPERATION)
		{
			nPaxCount = pFlight->GetFlightInput()->getArrBagCount();
		}
		else
		{
			nPaxCount = pFlight->GetFlightInput()->getDepBagCount();
		}
		bagTrainRequest->setBaggageCount(nPaxCount);
		bagTrainRequest->setUnserviceBaggageCount(nPaxCount);
		m_vBagTrainServiceRequest.push_back(bagTrainRequest);

		pFlight->AddVehicleServiceRequest(bagTrainRequest);
	}

}

void VehicleRequestDispatcher::BaggageTrainServiceRequestDispatch()
{
	//get pax bus service count
	if (m_vBagTrainServiceRequest.empty())
		return;

	//copy for erase issue
	std::vector<BaggageTrainServiceRequest*> vBagTrainServiceRequest = m_vBagTrainServiceRequest;
	
	for (int nRequst = 0; nRequst < (int)vBagTrainServiceRequest.size(); ++ nRequst)
	{
		BaggageTrainServiceRequest *pBagTrainRequest = vBagTrainServiceRequest.at(nRequst);
		if(pBagTrainRequest == NULL)
			return;

		AirsideFlightInSim* pFlight = pBagTrainRequest->GetServiceFlight();

		if (pFlight->GetMode() == OnTerminate)
		{
			continue;
		}


		//get available vehicle Type
		std::vector<CVehicleSpecificationItem * > vVehicleTypeSpec;
		 std::vector<CServicingRequirement* > vServiceRequirement;
		m_pFlightServiceRequirement->GetAvailableVehicleTypeByBaseType(pFlight,VehicleType_BaggageTug,/*vVehicleTypeSpec,*/vServiceRequirement);
		
		//have vehicle service request
		size_t nAvailableVehicleTypeCount = vServiceRequirement.size();
		if(nAvailableVehicleTypeCount == 0)
		{
			//drawing warning
			pBagTrainRequest->SetServiceCount(0);
			pBagTrainRequest->SetProceed(true);
			RemoveBaggageServiceRequest(pBagTrainRequest);
			continue;
		}


		
		FlightOperation fltOperation = pBagTrainRequest->getFltOperation();
		if(fltOperation == ARRIVAL_OPERATION)
		{
			ProcessorID procID = pFlight->GetFlightInput()->getBaggageDevice();
			//if arrival, find loader
			std::vector<Processor *> vAvailableLoader;
			std::vector<CBagCartsParkingSpotInSim *> vAvailbeParkingSpot;
			//then linked parking position
			//if parking position cannot be found, give error message
			//and baggage reclaim
			std::vector<BaseProcessor *> vConveyProcessor;
			m_pTermProcList->GetProcessorsByType(vConveyProcessor, LineProc);
			
			int nConveyCount = static_cast<int>(vConveyProcessor.size());
			for(int nConvey = 0; nConvey < nConveyCount; ++ nConvey)
			{
			//	CSimpleConveyor *pConveyProc = (CSimpleConveyor *)vConveyProcessor.at(nConvey);
				Processor* pLineProc = (Processor*)vConveyProcessor.at(nConvey);
				if(pLineProc == NULL)
					continue;
			//	if(pLineProc->GetSubConveyorType() == LOADER)
				{
// 					CLoader *pLoadProc = (CLoader *)pConveyProc;
// 
// 					if(pLoadProc == NULL)
// 						continue;

					CMobileElemConstraint paxCons;
					Flight* pFlightInput = pFlight->GetFlightInput();
					ASSERT(pFlightInput);
					FlightConstraint fltCons = pFlightInput->getType('A');
					paxCons.SetAirportDB(pFlightInput->GetTerminal()->m_pAirportDB);
					paxCons.MergeFlightConstraint(&fltCons);
					paxCons.setIntrinsicType(1);
					paxCons.SetTypeIndex(2);
				
					ALTObjectID altTermProcID;
					altTermProcID.FromString(pLineProc->getID()->GetIDString());

					ALTObjectID altReclaimID;
					altReclaimID.FromString(procID.GetIDString());

					if(pLineProc->CanLeadToReclaim(paxCons, procID) &&	pLineProc->canServe(paxCons))
					{
					

						//check which load linked with parking Spot
						/*ALTObjectID altTermProcID;
						altTermProcID.FromString(pLineProc->getID()->GetIDString());*/
						CBagCartsParkingSpotInSim *pLinkedSpot = m_pBagCartsSpotResourceManager->GetSpotLinked(altTermProcID);
						if(pLinkedSpot == NULL)
							continue;

						vAvailableLoader.push_back(pLineProc);
						vAvailbeParkingSpot.push_back(pLinkedSpot);
					}
				}
			}
			//have no arrival baggage service settings
			//ignore this request
			if(vAvailableLoader.size() == 0)
			{
				pBagTrainRequest->SetProceed(true);
				continue;
			}

			pBagTrainRequest->SetTermProcList(vAvailableLoader);
			pBagTrainRequest->SetPakringSpotList(vAvailbeParkingSpot);

			for (size_t nType = 0; nType < nAvailableVehicleTypeCount; ++ nType)
			{

				//CVehicleSpecificationItem *pTypeSpecItem = vVehicleTypeSpec[nType];
				CServicingRequirement* pRequirement = vServiceRequirement.at(nType);

				int nVehicleTypeID = pRequirement->GetServicingRequirementNameID();
				std::vector<int> vServicePoolList;
				vServicePoolList.clear();

				pBagTrainRequest->SetServiceVehicleTypeID(nVehicleTypeID);
				pBagTrainRequest->SetServiceTime(ProbabilityDistribution::CopyProbDistribution(vServiceRequirement[nType]->GetServiceTimeDistribution()));
				pBagTrainRequest->SetSubServiceTime(ProbabilityDistribution::CopyProbDistribution(vServiceRequirement[nType]->GetSubServiceTimeDistribution()));


				m_pPoolsDeployment->GetServicePool(pFlight, nVehicleTypeID, vServicePoolList);

				bool bFinishedDispatch = false;
				for (int j =0; j < int(vServicePoolList.size()); j++)
				{
					VehiclePoolInSim* pPool = m_pPoolResManager->GetVehiclePool(vServicePoolList[j]);
					if (pPool)
					{
						if (pPool->HandleBaggageTrainServiceRequest(pBagTrainRequest))
						{
							if(pBagTrainRequest->GetBaggageLeft() == 0)
							{
								pBagTrainRequest->SetProceed(true);
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
					//remove from list
					RemoveBaggageServiceRequest(pBagTrainRequest);
					break;
				}
			}


		}
		else if (fltOperation == DEPARTURE_OPERATION)
		{
			//if departure, find pusher
			//roster

			std::vector<BaseProcessor *> vConveyProcessor;
			m_pTermProcList->GetProcessorsByType(vConveyProcessor, ConveyorProc);

			std::vector<Processor *> vAvailablePusher;
			std::vector<CBagCartsParkingSpotInSim *> vAvailbeParkingSpot;
			int nConveyCount = static_cast<int>(vConveyProcessor.size());
			for(int nConvey = 0; nConvey < nConveyCount; ++ nConvey)
			{
				Conveyor *pConveyProc = (Conveyor *)vConveyProcessor.at(nConvey);
				if(pConveyProc == NULL)
					continue;
				if(pConveyProc->GetSubConveyorType() == PUSHER)
				{
					Pusher *pPusherProc = (Pusher *)pConveyProc->GetPerformer();

					if(pPusherProc == NULL)
						continue;

					CMobileElemConstraint paxCons;
					Flight* pFlightInput = pFlight->GetFlightInput();
					ASSERT(pFlightInput);
					FlightConstraint fltCons = pFlightInput->getType('D');
					paxCons.SetAirportDB(pFlightInput->GetTerminal()->m_pAirportDB);
					paxCons.setIntrinsicType(2);
					paxCons.SetTypeIndex(2);
					paxCons.MergeFlightConstraint(&fltCons);


					if(pPusherProc->canServe(paxCons) || pPusherProc->HaveBagOfFlight(pFlight->GetFlightInput()->getFlightIndex()))
					{
						//check which load linked with parking Spot
						ALTObjectID altTermProcID;
						altTermProcID.FromString(pConveyProc->getID()->GetIDString());
						CBagCartsParkingSpotInSim *pLinkedSpot = m_pBagCartsSpotResourceManager->GetSpotLinked(altTermProcID);
						if(pLinkedSpot == NULL)
							continue;

						vAvailablePusher.push_back(pConveyProc);
						vAvailbeParkingSpot.push_back(pLinkedSpot);
					}
				}
			}

			if(vAvailablePusher.size() == 0)
			{
				pBagTrainRequest->SetProceed(true);
				continue;
			}
			
			pBagTrainRequest->SetTermProcList(vAvailablePusher);
			pBagTrainRequest->SetPakringSpotList(vAvailbeParkingSpot);
			for (size_t nType = 0; nType < nAvailableVehicleTypeCount; ++ nType)
			{

				//CVehicleSpecificationItem *pTypeSpecItem = vVehicleTypeSpec[nType];
				CServicingRequirement* pRequirement = vServiceRequirement.at(nType);

				int nVehicleTypeID = pRequirement->GetServicingRequirementNameID();
				std::vector<int> vServicePoolList;
				vServicePoolList.clear();

				pBagTrainRequest->SetServiceVehicleTypeID(nVehicleTypeID);
				pBagTrainRequest->SetServiceTime(ProbabilityDistribution::CopyProbDistribution(vServiceRequirement[nType]->GetServiceTimeDistribution()));
				pBagTrainRequest->SetSubServiceTime(ProbabilityDistribution::CopyProbDistribution(vServiceRequirement[nType]->GetSubServiceTimeDistribution()));

				m_pPoolsDeployment->GetServicePool(pFlight, nVehicleTypeID, vServicePoolList);

				bool bFinishedDispatch = false;
				for (int j =0; j < int(vServicePoolList.size()); j++)
				{
					VehiclePoolInSim* pPool = m_pPoolResManager->GetVehiclePool(vServicePoolList[j]);
					if (pPool)
					{
						if (pPool->HandleBaggageTrainServiceRequest(pBagTrainRequest))
						{
							if(pBagTrainRequest->GetBaggageLeft() == 0)
							{
								pBagTrainRequest->SetProceed(true);
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
					RemoveBaggageServiceRequest(pBagTrainRequest);
					break;
				}
			}
		}
	}
}

void VehicleRequestDispatcher::SetBagCartsParkingSpotResourceManager( CBagCartsParkingSpotResourceManager *pBagCartsSpotResourceManager )
{
	m_pBagCartsSpotResourceManager = pBagCartsSpotResourceManager;
}

void VehicleRequestDispatcher::RemoveBaggageServiceRequest( BaggageTrainServiceRequest*req )
{
	std::vector<BaggageTrainServiceRequest*>::iterator itr = std::find(m_vBagTrainServiceRequest.begin(),m_vBagTrainServiceRequest.end(),req);
	if(itr!=m_vBagTrainServiceRequest.end())
		m_vBagTrainServiceRequest.erase(itr);
}

void VehicleServiceRequestList::insert( VehicleServiceRequest* rq )
{
	datalist.add(rq);
	std::sort(datalist.begin(),datalist.end(), VehicleServiceRequest::OrderLess );
}
