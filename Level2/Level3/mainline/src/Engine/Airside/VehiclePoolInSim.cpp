#include "StdAfx.h"
#include ".\vehiclepoolinsim.h"
#include "Common/pollygon2008.h"
#include "AirsideVehicleInSim.h"
#include "VehicleServiceRequest.h"
#include "VehiclePoolsManagerInSim.h"
#include "InputAirside/VehiclePoolsAndDeployment.h"
#include "VehicleRequestDispatcher.h"
#include "AirsideFlightInSim.h"
#include "StandInSim.h"
#include "AirsidePaxBusInSim.h"
#include "PaxBusServiceRequest.h"
#include ".\DeiceVehicleServiceRequest.h"
#include ".\DeiceVehicleInSim.h"
#include "TowTruckServiceRequest.h"
#include "AirsideFollowMeCarInSim.h"
#include "../core/SAgent.h"
#include "../BaggageTrainServiceRequest.h"
#include "../AirsideBaggageTrainInSim.h"
#include "Common/ARCPipe.h"
#include "../BoostPathFinder.h"

#define FOR(var, varf, vart)  for(int var=varf; var<vart;++var)

typedef std::vector<int> IndexList;

VehiclePoolInSim::VehiclePoolInSim(VehiclePoolParking * pVehiclePool)
{
	m_pPoolInput = pVehiclePool;
	m_pRequestDispatcher = NULL;
	m_pPoolsDeployment = NULL;
	m_vVehicleList.clear();
	mpPathFinder = NULL;
	/*Build(pVehiclePool);*/

	//init spots
	for(int i=0;i<pVehiclePool->m_parkspaces.getCount();i++)
	{
		VehiclePoolParkSpaceInSim* pSpace = new VehiclePoolParkSpaceInSim(pVehiclePool->m_parkspaces.getSpace(i));
		m_vParkSpaces.push_back(pSpace);
	}
	//init drive pipe
	for(int i=0;i<pVehiclePool->m_dirvepipes.getCount();i++)
	{
		VehiclePoolPipeInSim* pPipe = new VehiclePoolPipeInSim(pVehiclePool->m_dirvepipes.getPipe(i),true);
		m_vpipes.push_back(pPipe);
	}
}


CPoint2008 VehiclePoolInSim::GetDistancePoint(double dist) const
{
	CPoint2008 _point;


	return _point;
}

CString VehiclePoolInSim::PrintResource() const
{
	return m_pPoolInput->GetObjNameString();
}

CPoint2008 VehiclePoolInSim::GetRandPoint()
{
	const CPoint2008* pointList = m_pPoolInput->GetPath().getPointList();
	int nCount = m_pPoolInput->GetPath().getCount();
	CPollygon2008 polygen;
	polygen.init(nCount, pointList);

	CPoint2008 pos = polygen.getRandPoint();

	return pos;
}



void VehiclePoolInSim::AddVehicleInPool(AirsideVehicleInSim* pVehicle)
{
	m_vVehicleList.push_back(pVehicle);
}

bool VehiclePoolInSim::HandleGeneralRequest(GeneralVehicleServiceRequest* pRequest)
{
	AirsideFlightInSim* pFlight = pRequest->GetServiceFlight();

	int vehicleTypeID = pRequest->GetServiceVehicleTypeID();
	CVehicleServiceTimeRange* pTimeRange = m_pPoolsDeployment->GetVehicleServicePoolDeploy(vehicleTypeID,m_pPoolInput->getID(),pFlight);

	//ASSERT(pTimeRange);
	if (pTimeRange == NULL)
		return false;

	VehicleRegimenType RegimenType = pTimeRange->GetRegimenType();
	double vStuffPercent = 100.0/pTimeRange->GetAircraftServiceNum();
	ElapsedTime tLeaveTime = pTimeRange->GetLeaveTime();
	VehicleLeaveType LeaveType = pTimeRange->GetLeaveType();

	double MinDist = -1.0;
	int nServiceIdx = -1;


	int nVehicleCount = m_vVehicleList.size();
	for (int i =0; i < nVehicleCount; i++)
	{
		AirsideVehicleInSim* pVehicle = m_vVehicleList.at(i);
		if ( pVehicle->GetVehicleTypeID() == vehicleTypeID && pVehicle->IsAvailable())
		{
			if ( pVehicle->GetStuffCount() < vStuffPercent)
				pVehicle->ReturnVehiclePool(m_pPoolsDeployment->GetTurnAroundTimeDistribution(vehicleTypeID,m_pPoolInput->getID()));
			else
			{
				if (pVehicle->GetResource()->GetType()== AirsideResource::ResType_VehiclePool)
				{
					if (LeaveType == BeforeAircraftArrival)
					{
						ElapsedTime tTime = pRequest->GetServiceFlight()->GetEstimateStandTime() - tLeaveTime;
						if (tTime < SAgent::curTime() )
							tTime = SAgent::curTime();
						pVehicle->SetLeavePoolTime(tTime);
					}
					else if (LeaveType == BeforeAircraftDeparts)
					{
						ElapsedTime tTime = pRequest->GetServiceFlight()->GetDepTime() - tLeaveTime;
						if (tTime < SAgent::curTime() )
							tTime = SAgent::curTime();
						m_vVehicleList[i]->SetLeavePoolTime(tTime);
					}
					else
						pVehicle->SetLeavePoolTime(pRequest->GetServiceFlight()->GetEstimateStandTime() + tLeaveTime);

					//LeavePool(pVehicle);
					pVehicle->SetServiceFlight(pFlight, vStuffPercent);
					pRequest->AddServiceVehicle(pVehicle);
					pVehicle->SetServicePointCount(pRequest->GetServiceCount());
					if (pRequest->GetServiceTime())
						pVehicle->SetServiceTimeDistribution(pRequest->GetServiceTime());
					return true;
					
				}
				else
				{
					switch(RegimenType)
					{
						case ServiceCloset:
							{
								CPoint2008 pos = pVehicle->GetPosition();
								double dist = pos.distance(pFlight->GetOperationParkingStand()->GetStandInput()->GetServicePoint());
								if ( MinDist < 0.0)
								{
									MinDist = dist;
									nServiceIdx = i;
								}
								else
								{
									if (MinDist > dist)
									{
										MinDist = dist;
										nServiceIdx = i;
									}
								}
							}
							break;
						case ServiceNextDep:
							{
								if (pFlight->IsDeparture()||( pFlight->IsTransfer() && pFlight->GetMode() >= OnHeldAtStand ))
								{
									pVehicle->SetServiceFlight(pFlight,vStuffPercent);
									pRequest->AddServiceVehicle(pVehicle);
									pVehicle->SetServicePointCount(pRequest->GetServiceCount());
									if (pRequest->GetServiceTime())
										pVehicle->SetServiceTimeDistribution(pRequest->GetServiceTime());

									return true;
								}
							}
							break;
						case ServiceRandom:
							{
								pVehicle->SetServiceFlight(pFlight,vStuffPercent);
								pVehicle->SetServicePointCount(pRequest->GetServiceCount());
								pRequest->AddServiceVehicle(pVehicle);
								if (pRequest->GetServiceTime())
									pVehicle->SetServiceTimeDistribution(pRequest->GetServiceTime());
								return true;
							}
							break;
						case ServiceFIFO:
							{
								pVehicle->SetServiceFlight(pFlight,vStuffPercent);
								pRequest->AddServiceVehicle(pVehicle);
								pVehicle->SetServicePointCount(pRequest->GetServiceCount());
								if (pRequest->GetServiceTime())
									pVehicle->SetServiceTimeDistribution(pRequest->GetServiceTime());
								return true;
							}

							break;
						default:
							break;
					}
					if ( nServiceIdx >=0 && nServiceIdx != i)		//closet service type
					{
						pVehicle->ReturnVehiclePool(m_pPoolsDeployment->GetTurnAroundTimeDistribution(vehicleTypeID,m_pPoolInput->getID()));
					}
					if (nServiceIdx < 0)		//other service type
					{
						if(pVehicle->GetResource()->GetType()== AirsideResource::ResType_FlightServiceRingRoute)
						{
							pVehicle->SetServiceFlight(pFlight, vStuffPercent);
							pRequest->AddServiceVehicle(pVehicle);
							pVehicle->SetServicePointCount(pRequest->GetServiceCount());
							if (pRequest->GetServiceTime())
								pVehicle->SetServiceTimeDistribution(pRequest->GetServiceTime());
							return true;
						}
						else
						{
							pVehicle->ReturnVehiclePool(m_pPoolsDeployment->GetTurnAroundTimeDistribution(vehicleTypeID,m_pPoolInput->getID()));
						}
					}

				}
			}
		}
	}

	if (nServiceIdx >=0)
	{
		m_vVehicleList[nServiceIdx]->SetServiceFlight(pFlight, vStuffPercent);
		pRequest->AddServiceVehicle(m_vVehicleList[nServiceIdx]);
		m_vVehicleList[nServiceIdx]->SetServicePointCount(pRequest->GetServiceCount());
		if (pRequest->GetServiceTime())
			m_vVehicleList[nServiceIdx]->SetServiceTimeDistribution(pRequest->GetServiceTime());

		return true;
	}

	return false;
	
}

bool VehiclePoolInSim::HandleTowingServiceRequest(TowTruckServiceRequest* pRequest)
{
	AirsideFlightInSim* pFlight = pRequest->GetServiceFlight();

	int vehicleTypeID = pRequest->GetServiceVehicleTypeID();

	CVehicleTypePools* pPools = m_pPoolsDeployment->GetVehicleServicePool(vehicleTypeID);

	if (pPools)
	{
		CVehiclePool* pPool = NULL;
		for (int i =0; i < pPools->GetVehiclePoolCount();i++)
		{
			if (pPools->GetVehiclePoolItem(i))
				if (pPools->GetVehiclePoolItem(i)->GetParkingLotID() == m_pPoolInput->getID())
				{
					pPool = pPools->GetVehiclePoolItem(i);
					break;
				}	
		}
		if (pPool)
		{
			ALTObjectID objectID;
			if(pFlight->GetOperationParkingStand())
				objectID = pFlight->GetOperationParkingStand()->GetStandInput()->GetObjectName();
			else
				objectID = pFlight->GetFlightInput()->getStand();

			for (int i =0; i < pPool->GetServiceStandFamilyCount(); i++)
			{
				if ( objectID.idFits(pPool->GetServiceStandFamilyItem(i)->GetStandALTObjectID()) )
				{
					CVehicleServiceStandFamily* pStandFamily = pPool->GetServiceStandFamilyItem(i);

					int nVehicleCount = m_vVehicleList.size();
					for (int i =0; i < nVehicleCount; i++)
					{
						AirsideVehicleInSim* pVehicle = m_vVehicleList.at(i);
						if ( pVehicle->GetVehicleBaseType() != VehicleType_TowTruck )
							continue;

						AirsideTowTruckInSim* pTowTruck = dynamic_cast<AirsideTowTruckInSim*>(pVehicle);
						if (pTowTruck && pTowTruck->IsAvailable())
						{
							if (pTowTruck->GetResource()->GetType()== AirsideResource::ResType_VehiclePool)
							{				
								ElapsedTime tTime = pRequest->m_tApplyTime - pStandFamily->GetLeaveTime();
								pTowTruck->SetLeavePoolTime(tTime);

								//LeavePool(pTowTruck);
								pTowTruck->SetServiceFlight(pFlight, 0);
								pTowTruck->SetReturnRoute(pRequest->m_ReturnRoute);
								pTowTruck->SetReturnToStretch(pRequest->m_vSegments);
								pTowTruck->SetServiceType(pRequest->m_ServiceType);
								pTowTruck->SetReleasePoint(pRequest->m_pReleasePoint);
								return true;

							}
							else
							{								
								pTowTruck->SetServiceFlight(pFlight, 0);
								pTowTruck->SetReturnRoute(pRequest->m_ReturnRoute);
								pTowTruck->SetReturnToStretch(pRequest->m_vSegments);
								pTowTruck->SetServiceType(pRequest->m_ServiceType);
								pTowTruck->SetReleasePoint(pRequest->m_pReleasePoint);
								return true;					
								
							}
						}

					}
					break;
				}
			}
		}
	}
	
	////ASSERT(pTimeRange);
	//if (pTimeRange == NULL)
	//	return false;

	//VehicleRegimenType RegimenType = pTimeRange->GetRegimenType();
	////ElapsedTime tLeaveTime = pTimeRange->GetLeaveTime();
	//VehicleLeaveType LeaveType = pTimeRange->GetLeaveType();

	//double MinDist = -1.0;
	//int nServiceIdx = -1;

	//int nVehicleCount = m_vVehicleList.size();
	//for (int i =0; i < nVehicleCount; i++)
	//{
	//	AirsideVehicleInSim* pVehicle = m_vVehicleList.at(i);
	//	if ( pVehicle->GetVehicleBaseType() != VehicleType_TowTruck )
	//		continue;
	//	
	//	AirsideTowTruckInSim* pTowTruck = dynamic_cast<AirsideTowTruckInSim*>(pVehicle);
	//	if (pTowTruck && pTowTruck->IsAvailable())
	//	{
	//		if (pTowTruck->GetResource()->GetType()== AirsideResource::ResType_VehiclePool)
	//		{				
	//			ElapsedTime tTime = pRequest->m_tApplyTime - pTimeRange->GetLeaveTime();
	//			pTowTruck->SetLeavePoolTime(pRequest->m_tApplyTime);
	//			
	//			pTowTruck->SetServiceFlight(pFlight, 0);
	//			pTowTruck->SetReturnRoute(pRequest->m_ReturnRoute);
	//			pTowTruck->SetServiceType(pRequest->m_ServiceType);
	//			pTowTruck->SetReleasePoint(pRequest->m_pReleasePoint);
	//			return true;

	//		}
	//		else
	//		{
				//switch(RegimenType)
				//{
				//case ServiceCloset:
				//	{
				//		CPoint2008 pos = pVehicle->GetPosition();
				//		double dist = pos.distance(pFlight->GetPosition());
				//		if ( MinDist < 0.0)
				//		{
				//			MinDist = dist;
				//			nServiceIdx = i;
				//		}
				//		else
				//		{
				//			if (MinDist > dist)
				//			{
				//				MinDist = dist;
				//				nServiceIdx = i;
				//			}
				//		}
				//	}
				//	break;
				//case ServiceNextDep:
					//{
					//	pTowTruck->SetServiceFlight(pFlight, 0);
					//	pTowTruck->SetReturnRoute(pRequest->m_ReturnRoute);
					//	pTowTruck->SetServiceType(pRequest->m_ServiceType);
					//	pTowTruck->SetReleasePoint(pRequest->m_pReleasePoint);
					//	return true;					
					//}
				//	break;
				//case ServiceRandom:
				//	{
				//		pTowTruck->SetServiceFlight(pFlight, 0);
				//		pTowTruck->SetReturnRoute(pRequest->m_ReturnRoute);
				//		pTowTruck->SetServiceType(pRequest->m_ServiceType);
				//		pTowTruck->SetReleasePoint(pRequest->m_pReleasePoint);
				//		return true;
				//	}
				//	break;
				//case ServiceFIFO:
				//	{
				//		pTowTruck->SetServiceFlight(pFlight, 0);
				//		pTowTruck->SetReturnRoute(pRequest->m_ReturnRoute);
				//		pTowTruck->SetServiceType(pRequest->m_ServiceType);
				//		pTowTruck->SetReleasePoint(pRequest->m_pReleasePoint);
				//		return true;
				//	}

				//	break;
				//default:
				//	break;
				//}
				//if ( nServiceIdx >=0 && nServiceIdx != i)		//closet service type
				//{
				//	pVehicle->ReturnVehiclePool(m_pPoolsDeployment->GetTurnAroundTimeDistribution(vehicleTypeID,m_pPoolInput->getID()));
				//}
				//if (nServiceIdx < 0)		//other service type
				//{
				//	pVehicle->ReturnVehiclePool(m_pPoolsDeployment->GetTurnAroundTimeDistribution(vehicleTypeID,m_pPoolInput->getID()));
				//}
	//		}
	//	}
	//	
	//}

	//if (nServiceIdx >=0)
	//{
	//	m_vVehicleList[nServiceIdx]->SetServiceFlight(pFlight, 0);
	//	((AirsideTowTruckInSim*)m_vVehicleList[nServiceIdx])->SetReturnRoute(pRequest->m_ReturnRoute);
	//	((AirsideTowTruckInSim*)m_vVehicleList[nServiceIdx])->SetServiceType(pRequest->m_ServiceType);
	//	((AirsideTowTruckInSim*)m_vVehicleList[nServiceIdx])->SetReleasePoint(pRequest->m_pReleasePoint);
	//	return true;
	//}

	return false;

}

bool VehiclePoolInSim::HandlePaxBusServiceRequest(CPaxBusServiceRequest* pRequest)
{
	CPaxBusServiceRequest& request = *((CPaxBusServiceRequest *)pRequest);
	AirsideFlightInSim* pFlight = request.GetServiceFlight();
	int vehicleTypeID = request.GetServiceVehicleTypeID();
	CVehicleServiceTimeRange* pTimeRange = m_pPoolsDeployment->GetVehicleServicePoolDeploy(vehicleTypeID,m_pPoolInput->getID(),pFlight);

	if(pTimeRange == NULL)
		return FALSE ;

	VehicleRegimenType RegimenType = pTimeRange->GetRegimenType();
	double vStuffPercent = 100.0/pTimeRange->GetAircraftServiceNum();
	ElapsedTime tLeaveTime = pTimeRange->GetLeaveTime();
	VehicleLeaveType LeaveType = pTimeRange->GetLeaveType();

	double MinDist = -1.0;
	int nServiceIdx = -1;

	for (int i =0; i < int(m_vVehicleList.size()); i++)
	{
		if ( m_vVehicleList[i]->GetVehicleTypeID() == vehicleTypeID && m_vVehicleList[i]->IsAvailable())
		{
			CAirsidePaxBusInSim *pPaxBus = (CAirsidePaxBusInSim *)m_vVehicleList[i];
			int nVehicleCapacity = pPaxBus->GetVehicleCapacity();
			
			if ( m_vVehicleList[i]->GetStuffCount() < vStuffPercent)
				m_vVehicleList[i]->ReturnVehiclePool(m_pPoolsDeployment->GetTurnAroundTimeDistribution(vehicleTypeID,m_pPoolInput->getID()));
			else
			{
				if (m_vVehicleList[i]->GetResource()->GetType()== AirsideResource::ResType_VehiclePool)
				{
					if (LeaveType == BeforeAircraftArrival)
					{
						ElapsedTime tTime = request.GetServiceFlight()->GetEstimateStandTime() - tLeaveTime;
						if (tTime < SAgent::curTime() )
							tTime = SAgent::curTime();
						m_vVehicleList[i]->SetLeavePoolTime(tTime);
					}
					else if (LeaveType == BeforeAircraftDeparts)
					{
						ElapsedTime tTime = request.GetServiceFlight()->GetDepTime() - tLeaveTime;
						if (tTime < SAgent::curTime() )
							tTime = SAgent::curTime();
						m_vVehicleList[i]->SetLeavePoolTime(tTime);
					}
					else
						m_vVehicleList[i]->SetLeavePoolTime(request.GetServiceFlight()->GetEstimateStandTime() + tLeaveTime);

					//LeavePool(pPaxBus);
					pPaxBus->SetServiceFlight(pFlight, vStuffPercent,request.IsArrival());
					m_vVehicleList[i]->SetServicePointCount(request.GetServiceCount());
					if (request.GetServiceTime())
						m_vVehicleList[i]->SetServiceTimeDistribution(request.GetServiceTime());
					
					request.AddServiceVehicle(pPaxBus);
					if(request.GetLeftPaxCount() - nVehicleCapacity > 0)
					{
						pPaxBus->SetLoadPaxCount(nVehicleCapacity);
						request.ServicedPaxCount(nVehicleCapacity);
						continue;
					}
					else
					{
						pPaxBus->SetLoadPaxCount(request.GetLeftPaxCount());
						request.ServicedPaxCount(request.GetLeftPaxCount());
                        return true;
					}

				}
				else
				{
					switch(RegimenType)
					{
					case ServiceCloset:
						{
							CPoint2008 pos = m_vVehicleList[i]->GetPosition();
							double dist = pos.distance(pFlight->GetOperationParkingStand()->GetStandInput()->GetServicePoint());
							if ( MinDist < 0.0)
							{
								MinDist = dist;
								nServiceIdx = i;
							}
							else
							{
								if (MinDist > dist)
								{
									MinDist = dist;
									nServiceIdx = i;
								}
							}
						}
						break;
					case ServiceNextDep:
						{
							if (pFlight->IsDeparture()||( pFlight->IsTransfer() && pFlight->GetMode() >= OnHeldAtStand ))
							{
								pPaxBus->SetServiceFlight(pFlight,vStuffPercent,request.IsArrival());
								m_vVehicleList[i]->SetServicePointCount(request.GetServiceCount());
								if (request.GetServiceTime())
									m_vVehicleList[i]->SetServiceTimeDistribution(request.GetServiceTime());

								request.AddServiceVehicle(pPaxBus);
								if(request.GetLeftPaxCount() - nVehicleCapacity > 0)
								{
									pPaxBus->SetLoadPaxCount(nVehicleCapacity);
									request.ServicedPaxCount(nVehicleCapacity);
									continue;
								}
								else
								{
									pPaxBus->SetLoadPaxCount(request.GetLeftPaxCount());
									request.ServicedPaxCount(request.GetLeftPaxCount());
									return true;
								}
							}
						}
						break;
					case ServiceRandom:
						{
							pPaxBus->SetServiceFlight(pFlight,vStuffPercent,request.IsArrival());
							m_vVehicleList[i]->SetServicePointCount(request.GetServiceCount());
							if (request.GetServiceTime())
								m_vVehicleList[i]->SetServiceTimeDistribution(request.GetServiceTime());

							request.AddServiceVehicle(pPaxBus);
							if(request.GetLeftPaxCount() - nVehicleCapacity > 0)
							{
								pPaxBus->SetLoadPaxCount(nVehicleCapacity);
								request.ServicedPaxCount(nVehicleCapacity);
								continue;
							}
							else
							{
								pPaxBus->SetLoadPaxCount(request.GetLeftPaxCount());
								request.ServicedPaxCount(request.GetLeftPaxCount());
								return true;
							}
						}
						break;
					case ServiceFIFO:
						{
							pPaxBus->SetServiceFlight(pFlight,vStuffPercent,request.IsArrival());
							m_vVehicleList[i]->SetServicePointCount(request.GetServiceCount());
							if (request.GetServiceTime())
								m_vVehicleList[i]->SetServiceTimeDistribution(request.GetServiceTime());
							
							request.AddServiceVehicle(pPaxBus);
							if(request.GetLeftPaxCount() - nVehicleCapacity > 0)
							{
								pPaxBus->SetLoadPaxCount(nVehicleCapacity);
								request.ServicedPaxCount(nVehicleCapacity);
								continue;
							}
							else
							{
								pPaxBus->SetLoadPaxCount(request.GetLeftPaxCount());
								request.ServicedPaxCount(request.GetLeftPaxCount());
								return true;
							}
						}

						break;
					default:
						break;
					}
					if ( nServiceIdx >=0 && nServiceIdx != i)		//closet service type
					{
						m_vVehicleList[i]->ReturnVehiclePool(m_pPoolsDeployment->GetTurnAroundTimeDistribution(vehicleTypeID,m_pPoolInput->getID()));
					}
					if (nServiceIdx < 0)		//other service type
					{
						m_vVehicleList[i]->ReturnVehiclePool(m_pPoolsDeployment->GetTurnAroundTimeDistribution(vehicleTypeID,m_pPoolInput->getID()));
					}

				}
			}
		}
	}

	if (nServiceIdx >=0)
	{
		CAirsidePaxBusInSim *pPaxBus = (CAirsidePaxBusInSim *)m_vVehicleList[nServiceIdx];
		int nVehicleCapacity = pPaxBus->GetVehicleCapacity();

		pPaxBus->SetServiceFlight(pFlight,vStuffPercent,request.IsArrival());
		request.AddServiceVehicle(pPaxBus);


		m_vVehicleList[nServiceIdx]->SetServicePointCount(request.GetServiceCount());
		if (request.GetServiceTime())
			m_vVehicleList[nServiceIdx]->SetServiceTimeDistribution(request.GetServiceTime());
		if(request.GetLeftPaxCount() - nVehicleCapacity > 0)
		{
			pPaxBus->SetLoadPaxCount(nVehicleCapacity);
			request.ServicedPaxCount(nVehicleCapacity);
		}
		else
		{
			pPaxBus->SetLoadPaxCount(request.GetLeftPaxCount());
			request.ServicedPaxCount(request.GetLeftPaxCount());
		
		}
		return true;
	}

	return false;
}
void VehiclePoolInSim::GetNextServiceRequest()
{
	m_pRequestDispatcher->ServiceRequestDispatch();
	m_pRequestDispatcher->PaxBusServiceRequestDispatch();
	m_pRequestDispatcher->TowTruckServiceRequestDispatch();
}

void VehiclePoolInSim::CallVehicleReturnPool()
{
	int nCount = int(m_vVehicleList.size());
	for ( int i =0; i < nCount; i++ )
	{
		AirsideVehicleInSim* pVehicle = m_vVehicleList.at(i);
		if (pVehicle->IsAvailable() && pVehicle->GetResource()->GetType() != ResType_VehiclePool && pVehicle->GetMode() != OnBackPool )
		{
			pVehicle->ReturnVehiclePool(NULL);
		}
	}
}
bool VehiclePoolInSim::IsPoolCanServiceVehicle(int vehicleType)
{
	for (int i =0; i < int(m_vVehicleList.size()); i++)
	{
		if ( m_vVehicleList[i]->GetVehicleTypeID() == vehicleType)
		{
			return true;
		}
	}

	return false;
}

bool VehiclePoolInSim::HandleServiceRequest( VehicleServiceRequest* request )
{
	switch(request->GetType())
	{
	case VehicleType_General:
		return HandleGeneralRequest((GeneralVehicleServiceRequest*)request);
	case VehicleType_PaxTruck:
		return HandlePaxBusServiceRequest( (CPaxBusServiceRequest*)request );
	case VehicleType_DeicingTruck:
		return HandleDeiceServiceRequest( (DeiceVehicleServiceRequest*)request );
	case VehicleType_TowTruck:
		return HandleTowingServiceRequest((TowTruckServiceRequest*)request);
	default:
		return false;
	}
}


bool VehiclePoolInSim::HandleDeiceServiceRequest( DeiceVehicleServiceRequest *pRequest )
{
	
	AirsideFlightInSim* pFlight = pRequest->GetServiceFlight();
	int vehicleTypeID = pRequest->GetServiceVehicleTypeID();
	CVehicleServiceTimeRange* pTimeRange = m_pPoolsDeployment->GetVehicleServicePoolDeploy(vehicleTypeID,m_pPoolInput->getID(),pFlight);
	if(!pFlight->GetDeiceDecision().m_deicePlace)
		return false;

	if(pTimeRange == NULL)
		return FALSE ;
	
	for (int i =0; i < int(m_vVehicleList.size()); i++)
	{
		AirsideVehicleInSim* pVehicle = m_vVehicleList.at(i);
		if(pVehicle->GetVehicleBaseType() == VehicleType_DeicingTruck && pVehicle->IsAvailable() )
		{
			DeiceVehicleInSim* pDeiceVehicle = (DeiceVehicleInSim*)pVehicle;
			if(!pRequest->isRequestHandled()  )
			{
				//ASSERT(pTimeRange);
				if( pDeiceVehicle->getFluidAvaiable()>0)
				{
					pDeiceVehicle->SetLeavePoolTime(pRequest->getRequestTime() - pTimeRange->GetLeaveTime() );
					//LeavePool(pDeiceVehicle);
					pDeiceVehicle->SetServicePointCount(1);
					pRequest->AssignVehicle( pDeiceVehicle );
					pDeiceVehicle->SetServiceFlight(pFlight,1);
					continue;
				}
			}
			else
			{
				break;
			}
		}
	}	
	return pRequest->isRequestHandled();
}

#include "../../Results/AirsideFlightEventLog.h"
void VehiclePoolInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = m_pPoolInput->getID();
	resDesc.resType = GetType();
	resDesc.strRes =  PrintResource().GetString();
}
//
//bool VehiclePoolInSim::IsPathOverlapPool( const CPath2008& path )
//{
//	const CPoint2008* pointList = m_pPoolInput->GetPath().getPointList();
//	int nCount = m_pPoolInput->GetPath().getCount();
//	CPollygon2008 polygen;
//	polygen.init(nCount, pointList);
//
//	const CPoint2008* pointList2 = path.getPointList();
//	nCount = path.getCount();
//	CPollygon2008 polygen2;
//	polygen2.init(nCount,pointList2);
//
//	return polygen.IfOverlapWithOtherPolygon(polygen2);
//}

CPoint2008 VehiclePoolInSim::GetCenterPoint()
{
	double davgX = 0; 
	double davgY = 0;

	CPath2008 path = m_pPoolInput->GetPath();
	int nCount = path.getCount();
	for (int i = 0; i <nCount; i++)
	{
		davgX += path[i].getX();
		davgY += path[i].getY();
	}

	CPoint2008 pos(davgX/nCount, davgY/nCount,0);

	return pos;
}

AirsideFollowMeCarInSim* VehiclePoolInSim::getAvailableFollowMeCar()
{
	int nCount = int(m_vVehicleList.size());
	for (int i =0; i < nCount; i++)
	{
		AirsideVehicleInSim* pVehicle = m_vVehicleList.at(i);
		if(pVehicle->GetVehicleBaseType() == VehicleType_FollowMeCar && pVehicle->IsAvailable() )
			return (AirsideFollowMeCarInSim*)pVehicle;	
	}

	return NULL;
}

bool VehiclePoolInSim::HandleBaggageTrainServiceRequest( BaggageTrainServiceRequest* pRequest )
{
	//if request doesn't have checkbaggage, it's unnecessary to assign a baggage train 
	if (pRequest->GetBaggageLeft() == 0)
	{
		pRequest->setUnserviceBaggageCount(0);
		return true;
	}
	
	BaggageTrainServiceRequest& request = *pRequest;
	AirsideFlightInSim* pFlight = request.GetServiceFlight();
	int vehicleTypeID = request.GetServiceVehicleTypeID();
	CVehicleServiceTimeRange* pTimeRange = m_pPoolsDeployment->GetVehicleServicePoolDeploy(vehicleTypeID,m_pPoolInput->getID(),pFlight);

	if(pTimeRange == NULL)
		return false ;

	VehicleRegimenType RegimenType = pTimeRange->GetRegimenType();
	double vStuffPercent = 100.0/pTimeRange->GetAircraftServiceNum();
	ElapsedTime tLeaveTime = pTimeRange->GetLeaveTime();
	VehicleLeaveType LeaveType = pTimeRange->GetLeaveType();

	double MinDist = -1.0;
	int nServiceIdx = -1;

	for (int i =0; i < int(m_vVehicleList.size()); i++)
	{
		AirsideVehicleInSim* pVehicle = m_vVehicleList[i];

		if ( pVehicle->GetVehicleTypeID() == vehicleTypeID && pVehicle->IsAvailable() )
		{
			AirsideBaggageTrainInSim *pBaggageTrain = (AirsideBaggageTrainInSim *)pVehicle;
			int nVehicleCapacity = pBaggageTrain->GetVehicleCapacity();

			if ( pVehicle->GetStuffCount() < vStuffPercent)
			{
				pVehicle->ReturnVehiclePool(m_pPoolsDeployment->GetTurnAroundTimeDistribution(vehicleTypeID,m_pPoolInput->getID()));
			}
			else
			{
				if (pVehicle->GetResource()->GetType()== AirsideResource::ResType_VehiclePool)
				{
					if (LeaveType == BeforeAircraftArrival)
					{
						ElapsedTime tTime = request.GetServiceFlight()->GetEstimateStandTime() - tLeaveTime;
						if (tTime < SAgent::curTime() )
							tTime = SAgent::curTime();
						pVehicle->SetLeavePoolTime(tTime);
					}
					else if (LeaveType == BeforeAircraftDeparts)
					{
						ElapsedTime tTime = request.GetServiceFlight()->GetDepTime() - tLeaveTime;
						if (tTime < SAgent::curTime() )
							tTime = SAgent::curTime();
						pVehicle->SetLeavePoolTime(tTime);
					}
					else
						pVehicle->SetLeavePoolTime(request.GetServiceFlight()->GetEstimateStandTime() + tLeaveTime);

					//LeavePool(pBaggageTrain);
					pBaggageTrain->SetServiceFlight(pFlight, vStuffPercent,request);
					pVehicle->SetServicePointCount(request.GetServiceCount());
					if (request.GetServiceTime())
						pVehicle->SetServiceTimeDistribution(request.GetServiceTime());
					if (request.GetSubServiceTime())
						pVehicle->SetSubServiceTimeDistribution(request.GetSubServiceTime());

					request.AddServiceVehicle(pBaggageTrain);
					if(request.GetBaggageLeft() - nVehicleCapacity > 0)
					{
						pBaggageTrain->SetServiceCount(nVehicleCapacity);
						request.setUnserviceBaggageCount(request.GetBaggageLeft() -nVehicleCapacity);
						continue;
					}
					else
					{
						pBaggageTrain->SetServiceCount(request.GetBaggageLeft());
						request.setUnserviceBaggageCount(0);
						return true;
					}

				}
				else
				{
					switch(RegimenType)
					{
					case ServiceCloset:
						{
							CPoint2008 pos = pVehicle->GetPosition();
							double dist = pos.distance(pFlight->GetOperationParkingStand()->GetStandInput()->GetServicePoint());
							if ( MinDist < 0.0)
							{
								MinDist = dist;
								nServiceIdx = i;
							}
							else
							{
								if (MinDist > dist)
								{
									MinDist = dist;
									nServiceIdx = i;
								}
							}
						}
						break;
					case ServiceNextDep:
						{
							if (pFlight->IsDeparture()||( pFlight->IsTransfer() && pFlight->GetMode() >= OnHeldAtStand ))
							{
								pBaggageTrain->SetServiceFlight(pFlight,vStuffPercent,request);
								m_vVehicleList[i]->SetServicePointCount(request.GetServiceCount());
								if (request.GetServiceTime())
									m_vVehicleList[i]->SetServiceTimeDistribution(request.GetServiceTime());
								if (request.GetSubServiceTime())
									m_vVehicleList[i]->SetSubServiceTimeDistribution(request.GetSubServiceTime());

								request.AddServiceVehicle(pBaggageTrain);
								if(request.GetBaggageLeft() - nVehicleCapacity > 0)
								{
									pBaggageTrain->SetServiceCount(nVehicleCapacity);
									request.setUnserviceBaggageCount(request.GetBaggageLeft() -nVehicleCapacity);
									continue;
								}
								else
								{
									pBaggageTrain->SetServiceCount(request.GetBaggageLeft());
									request.setUnserviceBaggageCount(0);
									return true;
								}
							}
						}
						break;
					case ServiceRandom:
						{
							pBaggageTrain->SetServiceFlight(pFlight,vStuffPercent,request);
							m_vVehicleList[i]->SetServicePointCount(request.GetServiceCount());
							if (request.GetServiceTime())
								m_vVehicleList[i]->SetServiceTimeDistribution(request.GetServiceTime());
							if (request.GetSubServiceTime())
								m_vVehicleList[i]->SetSubServiceTimeDistribution(request.GetSubServiceTime());

							request.AddServiceVehicle(pBaggageTrain);
							if(request.GetBaggageLeft() - nVehicleCapacity > 0)
							{
								pBaggageTrain->SetServiceCount(nVehicleCapacity);
								request.setUnserviceBaggageCount(request.GetBaggageLeft() -nVehicleCapacity);
								continue;
							}
							else
							{
								pBaggageTrain->SetServiceCount(request.GetBaggageLeft());
								request.setUnserviceBaggageCount(0);
								return true;
							}
						}
						break;
					case ServiceFIFO:
						{
							pBaggageTrain->SetServiceFlight(pFlight,vStuffPercent,request);
							m_vVehicleList[i]->SetServicePointCount(request.GetServiceCount());
							if (request.GetServiceTime())
								m_vVehicleList[i]->SetServiceTimeDistribution(request.GetServiceTime());
							if (request.GetSubServiceTime())
								m_vVehicleList[i]->SetSubServiceTimeDistribution(request.GetSubServiceTime());

							request.AddServiceVehicle(pBaggageTrain);
							if(request.GetBaggageLeft() - nVehicleCapacity > 0)
							{
								pBaggageTrain->SetServiceCount(nVehicleCapacity);
								request.setUnserviceBaggageCount(request.GetBaggageLeft() -nVehicleCapacity);
								continue;
							}
							else
							{
								pBaggageTrain->SetServiceCount(request.GetBaggageLeft());
								request.setUnserviceBaggageCount(0);
								return true;
							}
						}

						break;
					default:
						break;
					}
					if ( nServiceIdx >=0 && nServiceIdx != i)		//closet service type
					{
						m_vVehicleList[i]->ReturnVehiclePool(m_pPoolsDeployment->GetTurnAroundTimeDistribution(vehicleTypeID,m_pPoolInput->getID()));
					}
					if (nServiceIdx < 0)		//other service type
					{
						m_vVehicleList[i]->ReturnVehiclePool(m_pPoolsDeployment->GetTurnAroundTimeDistribution(vehicleTypeID,m_pPoolInput->getID()));
					}

				}
			}
		}
	}

	if (nServiceIdx >=0)
	{
		AirsideBaggageTrainInSim *pBaggageTrain = (AirsideBaggageTrainInSim *)m_vVehicleList[nServiceIdx];
		int nVehicleCapacity = pBaggageTrain->GetVehicleCapacity();

		pBaggageTrain->SetServiceFlight(pFlight,vStuffPercent,request);
		request.AddServiceVehicle(pBaggageTrain);


		m_vVehicleList[nServiceIdx]->SetServicePointCount(request.GetServiceCount());
		if (request.GetServiceTime())
			m_vVehicleList[nServiceIdx]->SetServiceTimeDistribution(request.GetServiceTime());
		if (request.GetSubServiceTime())
			m_vVehicleList[nServiceIdx]->SetSubServiceTimeDistribution(request.GetSubServiceTime());

		if(request.GetBaggageLeft() - nVehicleCapacity > 0)
		{
			pBaggageTrain->SetServiceCount(nVehicleCapacity);
			request.setUnserviceBaggageCount(request.GetBaggageLeft() -nVehicleCapacity);

		}
		else
		{
			pBaggageTrain->SetServiceCount(request.GetBaggageLeft());
			request.setUnserviceBaggageCount(0);
		}
		return true;
	}


	return false;
}


void VehiclePoolInSim::Build( /*VehiclePoolParking* pPool */)
{
	//add entry exit 
	FOR(i,0,(int)m_vEntris.size())
	{
		VehiclePoolEntry* entry = m_vEntris.at(i);
		ParkingPoolNode node(entry,0, entry->m_pPoolLane,entry->m_distInPoolLane);
		AddNode(node);
	}
	FOR(i,0, (int)m_vExits.size())
	{
		VehiclePoolExit* pexit = m_vExits.at(i);
		ParkingPoolNode node(pexit->m_pPoolLane, pexit->m_distInPoolLane, pexit,0);
		AddNode(node);
	}

	//link parking spot to drive pipe
	for(size_t i=0;i<m_vParkSpaces.size();i++)
	{
		VehiclePoolParkSpaceInSim* pSpace = m_vParkSpaces[i];
		for(int j=0;j<pSpace->getSpotCount();j++)
		{
			VehiclePoolParkSpotInSim* pSpot = pSpace->getSpot(j);
			ARCPoint3 entryPos = pSpot->m_entryPos;
			ARCPoint3 exitPos = pSpot->m_exitPos; 
			
			DistanceUnit distInlane = 0;

			VehiclePoolPipeInSim* EntryPipe = getClosestPipe(entryPos,distInlane);		
			if(EntryPipe)
			{
				for(int iLaneIdx = 0 ;iLaneIdx<EntryPipe->getLaneCount();iLaneIdx++)
				{
					VehiclePoolLaneInSim* pEntryLane = EntryPipe->getLane(iLaneIdx);
					distInlane = pEntryLane->getPath().GetPointDist(CPoint2008(entryPos.x,entryPos.y,entryPos.z));
					ParkingPoolNode nodeEntry(pEntryLane, distInlane , pSpot, 0);
					AddNode(nodeEntry);
				}
			}

			VehiclePoolPipeInSim* ExitPipe = getClosestPipe(exitPos,distInlane);		
			if(ExitPipe)
			{
				for(int iLaneIdx = 0 ;iLaneIdx<ExitPipe->getLaneCount();iLaneIdx++)
				{
					VehiclePoolLaneInSim* pExitLane = ExitPipe->getLane(iLaneIdx);
					distInlane = pExitLane->getPath().GetPointDist(CPoint2008(exitPos.x,exitPos.y,exitPos.z));
					ParkingPoolNode nodeExit(pSpot, 0, pExitLane, distInlane);
					AddNode(nodeExit);
				}
			}	
		}
	}

	//init pipe with pipe
	int nPipeCount = m_vpipes.size();
	FOR(i, 0 , nPipeCount)
	{
		VehiclePoolPipeInSim* pipe1 = m_vpipes[i];
		FOR(j, i+1, nPipeCount)
		{
			VehiclePoolPipeInSim* pipe2 = m_vpipes[j];

			IntersectPathPath2D inters;
			int nCount = inters.Intersects(pipe1->getPath(),pipe2->getPath(),pipe2->getWidth());
			for(int k=0;k<nCount;k++)
			{
				double i1= inters.m_vIntersectPtIndexInPath1[k];
				double d1= pipe1->getPath().GetIndexDist((float)i1);
				double i2 = inters.m_vIntersectPtIndexInPath2[k];
				double d2 = pipe2->getPath().GetIndexDist((float)i2);
				ARCVector3 pos1 = pipe1->getPath().GetIndexPoint((float)i1);
				ARCVector3 pos2 = pipe2->getPath().GetIndexPoint((float)i2);
				if( abs(pos1.z - pos2.z) > ARCMath::EPSILON)
				{
					continue;
				}

				FOR(l1idx, 0, pipe1->getLaneCount())
				{ 
					VehiclePoolLaneInSim * pLane1 = pipe1->getLane(l1idx);
					double distInLane1 = pLane1->getPath().GetPointDist(CPoint2008(pos1.x,pos1.y,pos1.z));
					FOR(l2idx, 0, pipe2->getLaneCount())
					{
						VehiclePoolLaneInSim* pLane2 = pipe2->getLane(l2idx);
						double distInLane2 = pLane2->getPath().GetPointDist(CPoint2008(pos2.x,pos2.y,pos2.z));

						ParkingPoolNode node1(pLane1,distInLane1,pLane2,distInLane2);
						AddNode(node1);

						ParkingPoolNode node2(pLane2,distInLane2,pLane1,distInLane1);
						AddNode(node2);
					}
				}
			}
		}
	}

	//init graph
	cpputil::autoPtrReset(mpPathFinder, new CBoostDijPathFinder());	
	mpPathFinder->mGraph.Init(m_vNodes.size());

	typedef cpputil::nosort_map< VehiclePoolItemInSim*,IndexList > ResourceNodeMap;
	ResourceNodeMap EntryNodesMap;
	ResourceNodeMap ExitNodesMap;

	FOR(i, 0, (int)m_vNodes.size())
	{
		const ParkingPoolNode& node = m_vNodes[i];
		if(VehiclePoolItemInSim* pLane = node.m_to)
		{
			IndexList&fromNodes = EntryNodesMap[pLane];
			fromNodes.push_back(i);
		}
		if(VehiclePoolItemInSim* pLane = node.m_from)
		{
			IndexList& toNodes = ExitNodesMap[pLane];
			toNodes.push_back(i);
		}
	}
	//
	for(ResourceNodeMap::const_iterator itr=EntryNodesMap.begin();itr!=EntryNodesMap.end(); itr++)
	{
		VehiclePoolItemInSim* plane = itr->first;
		const IndexList& entryNodes = itr->second;
		const IndexList& exitNodes = ExitNodesMap[plane];
		FOR(iEntry , 0, (int)entryNodes.size())
		{
			const ParkingPoolNode& nodeEntry = m_vNodes[entryNodes[iEntry]];
			FOR(iExit, 0, (int)exitNodes.size() )
			{
				const ParkingPoolNode& nodeExit = m_vNodes[exitNodes[iExit]];
				if(nodeEntry.m_toDist< nodeExit.m_fromDist)
				{
					DistanceUnit dist = nodeExit.m_fromDist -  nodeEntry.m_toDist;
					mpPathFinder->mGraph.AddEdge(entryNodes[iEntry], exitNodes[iExit], dist);
				}
			}
		}
	}


}

VehiclePoolInSim::~VehiclePoolInSim()
{
	cpputil::deletePtrVector(m_vParkSpaces);
	cpputil::deletePtrVector(m_vpipes);
	cpputil::autoPtrReset(mpPathFinder);
}

VehiclePoolPipeInSim* VehiclePoolInSim::getClosestPipe( const ARCPoint3& cpt, DistanceUnit& distInLane ) const
{
	if(m_vpipes.empty())
		return NULL;

	VehiclePoolPipeInSim* pClostLane = NULL;

	CPoint2008 pt = CPoint2008(cpt.x,cpt.y,cpt.z);
	DistanceUnit minDisTo;

	for(size_t i=0;i<m_vpipes.size();i++)
	{
		VehiclePoolPipeInSim* pPipe = m_vpipes.at(i);
		DistanceUnit thedistInLane = pPipe->getPath().GetPointDist(pt);
		CPoint2008 posInLane = pPipe->getPath().GetDistPoint(thedistInLane);
		if( abs( posInLane.z - pt.z )> ARCMath::EPSILON)
			continue;

		DistanceUnit distTo = posInLane.distance3D(pt);
		if(!pClostLane)
		{
			minDisTo = distTo;
			pClostLane = pPipe;
			distInLane = thedistInLane;
		}
		else if(distTo < minDisTo)
		{
			minDisTo = distTo;
			pClostLane = pPipe;
			distInLane = thedistInLane;
		}

	}
	return pClostLane;
}

void VehiclePoolInSim::AddNode( const ParkingPoolNode& node )
{
	m_vNodes.push_back(node);
	m_vNodes.back().m_idx = m_vNodes.size()-1;
}


CPath2008 VehiclePoolInSim::ParkingToSpot( AirsideVehicleInSim* pV, VehicleRouteNode* pNode)
{
	VehiclePoolEntry* pEntry = NULL;
	FOR(i, 0, (int)m_vEntris.size())
	{
		VehiclePoolEntry* ientry = m_vEntris.at(i);
		if(ientry == pNode)
		{
			pEntry = ientry;	
		}
	}

	VehiclePoolParkSpotInSim* pSpot  = getFreeSpot(pV);
	if(pEntry && pSpot)
	{
		
		PoolNodeList path;
		if(FindPath(pEntry, pSpot, path))
		{
			CPath2008 retPath;
			retPath.push_back(pV->GetPosition());
			FOR(i,1,(int)path.size())
			{
				ParkingPoolNode& p1 = path[i-1];
				ParkingPoolNode& p2 = path[i];
				if(p1.m_to && p1.m_to == p2.m_from )
				{
					if(VehiclePoolLaneInSim* plane = p1.m_to->toLane())
					{
						CPath2008 subpath = plane->getPath().GetSubPath(p1.m_toDist, p2.m_fromDist);
						retPath.Append(subpath);
					}
				}
			}
			retPath.push_back(pSpot->m_pos+pSpot->m_dir);
			retPath.push_back(pSpot->m_pos);
			return retPath;
		}
		else
		{
			CPath2008 path;
			path.push_back(pV->GetPosition());
			path.push_back(pSpot->m_pos+pSpot->m_dir);
			path.push_back(pSpot->m_pos);
			return path;
		}
	}
	
	CPath2008 path;
	path.push_back(pV->GetPosition());
	path.push_back(GetRandPoint());
	return path;
	
}

bool VehiclePoolInSim::FindPath( VehiclePoolItemInSim* itemF, VehiclePoolItemInSim* itemT, PoolNodeList& path )
{
	IndexList fromNodes;
	IndexList toNodes;

	FOR(i,0,(int)m_vNodes.size())
	{
		const ParkingPoolNode& pNode = m_vNodes[i];
		if(itemF &&  pNode.m_from == itemF )
		{
			fromNodes.push_back(i);
		}
		if( itemT && pNode.m_to == itemT )
		{
			toNodes.push_back(i);
		}
	}

	FOR(i,0 , (int)fromNodes.size())
	{
		FOR(j,0,(int)toNodes.size())
		{
			int idxF = fromNodes[i];
			int idxT = toNodes[j];
			std::vector<myvertex_descriptor> retpath;double pathLen;
			if( mpPathFinder->FindPath(idxF,idxT,retpath,pathLen) )
			{
				//build path
				for(size_t k = 0;k<retpath.size();++k)
				{
					int idx1 = retpath[k];
					//int idx2 = retpath[k+1];
					const ParkingPoolNode& node1 = m_vNodes[idx1];
					//const ParkingPoolNode& node2 = m_vNodes[idx2];

					//ASSERT(node1.m_pTo == node2.m_pFrom);
					path.push_back(node1);
							
				}				
				//path.Update();
				return true;
			}
		}
	}

	return false;
}

VehiclePoolParkSpotInSim* VehiclePoolInSim::getFreeSpot(AirsideVehicleInSim* pV)
{
	/*FOR(i, 0, (int)m_vParkSpaces.size())
	{
		VehiclePoolParkSpaceInSim* space  = m_vParkSpaces.at(i);
		FOR(j,0,space->getSpotCount())
		{
			VehiclePoolParkSpotInSim* spot = space->getSpot(j);
			if(spot->m_pOccupyVehicle==NULL)
				return spot;
		}
	}*/
	std::vector<VehiclePoolParkSpotInSim*> availableParkingSpotVector;
	for (int i = 0; i < (int)m_vParkSpaces.size(); i++)
	{
		VehiclePoolParkSpaceInSim* space  = m_vParkSpaces.at(i);
		for(int j = 0; j < space->getSpotCount(); j++)
		{
			VehiclePoolParkSpotInSim* spot = space->getSpot(j);
			if(spot->m_pOccupyVehicle==NULL)
				availableParkingSpotVector.push_back(spot);
		}
	}

	if (availableParkingSpotVector.empty())
	{
		return NULL;
	}

	int nCount = (int)availableParkingSpotVector.size();
	int nRand = random(nCount);
	VehiclePoolParkSpotInSim* pSpot = availableParkingSpotVector[nRand];
	pSpot->m_pOccupyVehicle = pV;
	return pSpot;
}


CPath2008 VehiclePoolInSim::LeavePool( AirsideVehicleInSim* pV )
{
	VehiclePoolParkSpotInSim* pSpot = NULL;
	FOR(i, 0, (int)m_vParkSpaces.size())
	{
		VehiclePoolParkSpaceInSim* space  = m_vParkSpaces.at(i);
		FOR(j,0,space->getSpotCount())
		{
			VehiclePoolParkSpotInSim *spot = space->getSpot(j);
			if(spot->m_pOccupyVehicle==pV)
			{
				spot->m_pOccupyVehicle = NULL;
				pSpot = spot;
			}
		}
	}

	VehiclePoolExit* pExit = NULL;
	int nCount = (int)m_vExits.size();
	if(nCount>0)
	{
		int nRand = random(nCount);
		pExit = m_vExits.at(nRand);
	}
	
	if(pSpot && pExit)
	{

		PoolNodeList path;
		if(FindPath(pSpot, pExit, path))
		{
			CPath2008 retPath;
			//retPath.push_back(pV->GetPosition());
			FOR(i,1,(int)path.size())
			{
				ParkingPoolNode& p1 = path[i-1];
				ParkingPoolNode& p2 = path[i];
				if(p1.m_to && p1.m_to == p2.m_from )
				{
					if(VehiclePoolLaneInSim* plane = p1.m_to->toLane())
					{
						CPath2008 subpath = plane->getPath().GetSubPath(p1.m_toDist, p2.m_fromDist);
						retPath.Append(subpath);
					}
				}
			}
			//retPath.push_back(pExit->GetPosition());
			return retPath;
		}
		else
		{
			CPath2008 path;
			path.push_back(pV->GetPosition());
			path.push_back(pExit->GetPosition());
			return path;
		}
	}

	CPath2008 path;
	path.push_back(pV->GetPosition());
	return path;
}

void VehiclePoolInSim::BirthOnPool( AirsideVehicleInSim* pV, CPoint2008& pos ,CPoint2008& dir )
{
	VehiclePoolParkSpotInSim* spot = getFreeSpot(pV);
	if(spot)
	{
		pos = spot->m_pos;
		dir = spot->m_dir;
		spot->m_pOccupyVehicle = pV;
		return;
	}
	else
	{
		pos = GetRandPoint();
		dir = ARCVector3::UNIT_X;
	}
}

//parking space
VehiclePoolParkSpaceInSim::VehiclePoolParkSpaceInSim( const ParkingSpace& space )
{
	int i=0;
	CPoint2008 pos;
	ARCVector3 dir;
	CPoint2008 entrypos;
	CPoint2008 exitPos;

	while(space.GetSpotDirPos(i,pos,dir,entrypos,exitPos))
	{
		//pos.setZ(dHeight);
		//entrypos.setZ(dHeight);

		VehiclePoolParkSpotInSim* pSpot = new VehiclePoolParkSpotInSim();
		pSpot->m_pos = pos;
		pSpot->m_dir = dir;
		pSpot->m_entryPos = entrypos;
		pSpot->m_exitPos = exitPos;
		pSpot->bPushback = (space.m_opType==ParkingSpace::_backup);
		m_vSpots.push_back(pSpot);
		++i;
	}
}

VehiclePoolPipeInSim::VehiclePoolPipeInSim( const ParkingDrivePipe& dpipe,bool bLeftDrive )
{
	m_path = dpipe.m_ctrlPath;
	m_bLeftDrive = bLeftDrive;
	m_Dir = (dirtype)dpipe.m_nType;
	ARCPipe pipe;
	pipe.Init(m_path,dpipe.m_width);
	for(int i=0;i<dpipe.m_nLaneNum;i++)
	{
		bool bOtherDir = false;
		if(m_Dir == _bidirectional )
		{
			if(m_bLeftDrive)
				bOtherDir = (i*2>= dpipe.m_nLaneNum);
			else
				bOtherDir = (i*2 < dpipe.m_nLaneNum);
		}

		double drat = (i+.5)/dpipe.m_nLaneNum;
		VehiclePoolLaneInSim* plane = new VehiclePoolLaneInSim();
		for(int k=0;k<m_path.getCount();k++)
		{
			plane->m_path.push_back( CPoint2008(pipe.m_sidePath1[k] *(1-drat) + pipe.m_sidePath2[k]*drat) );
		}
		//reverse path
		if(bOtherDir)
			plane->m_path.invertList();

		m_vLanes.push_back(plane);
	}
}
