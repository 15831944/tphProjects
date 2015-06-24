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

VehiclePoolInSim::VehiclePoolInSim(VehiclePoolParking * pVehiclePool)
{
	m_pPoolInput = pVehiclePool;
	m_pRequestDispatcher = NULL;
	m_pPoolsDeployment = NULL;
	m_vVehicleList.clear();
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

CPoint2008 VehiclePoolInSim::GetParkingPos()
{
	CPoint2008 pos;

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

bool VehiclePoolInSim::IsPathOverlapPool( const CPath2008& path )
{
	const CPoint2008* pointList = m_pPoolInput->GetPath().getPointList();
	int nCount = m_pPoolInput->GetPath().getCount();
	CPollygon2008 polygen;
	polygen.init(nCount, pointList);

	const CPoint2008* pointList2 = path.getPointList();
	nCount = path.getCount();
	CPollygon2008 polygen2;
	polygen2.init(nCount,pointList2);

	return polygen.IfOverlapWithOtherPolygon(polygen2);
}

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