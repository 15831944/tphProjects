#include "StdAfx.h"
#include ".\vehiclepoolsmanagerinsim.h"
#include "VehiclePoolInSim.h"
#include "InputAirside/VehiclePoolsAndDeployment.h"
#include "Common/AirportDatabase.h"
#include "AirsideFlightInSim.h"
#include "StandInSim.h"
#include "SimulationErrorShow.h"
#include "VehiclePoolResourceManager.h"
#include "TowTruckServiceRequest.h"
#include "AirsideMeetingPointInSim.h"
#include "../../InputAirside/FollowMeCarServiceMeetingPoints.h"
#include "../../InputAirside/MeetingPoint.h"

VehiclePoolsManagerInSim::VehiclePoolsManagerInSim(void)
{
}

VehiclePoolsManagerInSim::~VehiclePoolsManagerInSim(void)
{
}

int VehiclePoolsManagerInSim::GetVehicleTypeCount()
{
	return m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsCount();
}

int VehiclePoolsManagerInSim::GetVehicleTypeIDByIdx(int idx)
{
	return m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(idx)->GetVehicleTypeID();
}

void VehiclePoolsManagerInSim::Init(int nPrjID,CAirportDatabase* pDatabase)
{
	m_pVehiclePoolsAndDeployment = new CVehiclePoolsAndDeployment();
	m_pVehiclePoolsAndDeployment->SetAirportDB(pDatabase);
	m_pVehiclePoolsAndDeployment->ReadData(nPrjID);
}

int VehiclePoolsManagerInSim::GetVehicleQuantityInPool(int nVehicleTypeID, int nParkingLotID)
{
	CVehicleTypePools* pPools = NULL;

	for (int i =0; i< m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsCount(); i++ )
	{
		if ( m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i))	
			if ( m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i)->GetVehicleTypeID() == nVehicleTypeID)
			{
				pPools = m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i);
				break;
			}
	}

	if (pPools)
	{
		for (int i =0; i < pPools->GetVehiclePoolCount();i++)
		{
			if (pPools->GetVehiclePoolItem(i))
				if (pPools->GetVehiclePoolItem(i)->GetParkingLotID() == nParkingLotID)
				{
					return pPools->GetVehiclePoolItem(i)->GetVehicleQuantity();
				}	
		}
	}

	return 0;

}

ProbabilityDistribution* VehiclePoolsManagerInSim::GetTurnAroundTimeDistribution(int nVehicleTypeID, int nParkingLotID)
{
	CVehicleTypePools* pPools = NULL;

	for (int i =0; i< m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsCount(); i++ )
	{
		if ( m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i))	
			if ( m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i)->GetVehicleTypeID() == nVehicleTypeID)
			{
				pPools = m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i);
				break;
			}
	}

	if (pPools)
	{
		for (int i =0; i < pPools->GetVehiclePoolCount();i++)
		{
			if (pPools->GetVehiclePoolItem(i))
				if (pPools->GetVehiclePoolItem(i)->GetParkingLotID() == nParkingLotID)
				{
					return pPools->GetVehiclePoolItem(i)->GetProbDistribution();	
				}
		}
	}

	return NULL;
}

CString VehiclePoolsManagerInSim::GetVehicleTypeByID(int nID)
{
	CString strVehilceType = "";
	for (int i = 0; i < m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsCount(); i++)
	{
		if ( m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i))
		{
			if ( m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i)->GetVehicleTypeID() == nID)
			{
				strVehilceType = m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i)->GetVehicleTypeName();
				break;
			}
		}

	}

	return strVehilceType;
}

void VehiclePoolsManagerInSim::GetPaxServicePool(AirsideFlightInSim* pFlight,int nVehicleTypeID,char mode, std::vector<int>& poolIDList)
{
	CVehicleTypePools* pPools = m_pVehiclePoolsAndDeployment->GetVehicleTypePools(nVehicleTypeID);

	if (pPools)
	{
		ALTObjectID objectID;
		if (mode == 'A')
		{
			if(pFlight->GetArrParkingStand())
				objectID = pFlight->GetArrParkingStand()->GetStandInput()->GetObjectName();
			else
				objectID = pFlight->GetFlightInput()->getArrStand();
		}
		else
		{
			if(pFlight->GetDepParkingStand())
				objectID = pFlight->GetDepParkingStand()->GetStandInput()->GetObjectName();
			else
				objectID = pFlight->GetFlightInput()->getDepStand();
		}
		


		for (int i = 0; i < pPools->GetVehiclePoolCount(); i++)
		{
			CVehiclePool* pVehiclePool = pPools->GetVehiclePoolItem(i);
			std::vector<CVehicleServiceStandFamily*> standVector;
			pVehiclePool->GetServiceFitStand(objectID,standVector);

			for (size_t j = 0; j < standVector.size(); j++)
			{
				CVehicleServiceStandFamily* pStandFamily = standVector[j];
				if(pStandFamily->Fit(pFlight->GetLogEntry().GetAirsideDesc(),mode,pFlight->GetTime()))
				{
					int nID = pVehiclePool->GetParkingLotID();
					poolIDList.push_back(nID);
					break;
				}	
			}
		}
	}
}

void VehiclePoolsManagerInSim::GetBaggageServicePool( AirsideFlightInSim* pFlight,int nVehicleTypeID,std::vector<int>& poolIDList,const ElapsedTime& eTime )
{
	CVehicleTypePools* pPools = m_pVehiclePoolsAndDeployment->GetVehicleTypePools(nVehicleTypeID);

	if (pPools)
	{
		char mode = pFlight->IsArrivingOperation()?'A':'D';

		ALTObjectID objectID;
		if(pFlight->GetOperationParkingStand())
			objectID = pFlight->GetOperationParkingStand()->GetStandInput()->GetObjectName();
		else
			objectID = pFlight->GetFlightInput()->getStand();


		for (int i = 0; i < pPools->GetVehiclePoolCount(); i++)
		{
			CVehiclePool* pVehiclePool = pPools->GetVehiclePoolItem(i);
			std::vector<CVehicleServiceStandFamily*> standVector;
			pVehiclePool->GetServiceFitStand(objectID,standVector);

			for (size_t j = 0; j < standVector.size(); j++)
			{
				CVehicleServiceStandFamily* pStandFamily = standVector[j];
				if(pStandFamily->Fit(pFlight->GetLogEntry().GetAirsideDesc(),mode,eTime))
				{
					int nID = pVehiclePool->GetParkingLotID();
					poolIDList.push_back(nID);
					break;
				}	
			}
		}
	}
}

void VehiclePoolsManagerInSim::GetServicePool(AirsideFlightInSim* pFlight,int nVehicleTypeID, std::vector<int>& poolIDList)
{
	CVehicleTypePools* pPools = m_pVehiclePoolsAndDeployment->GetVehicleTypePools(nVehicleTypeID);

	if (pPools)
	{
		char mode = pFlight->IsArrivingOperation()?'A':'D';

		ALTObjectID objectID;
		if(pFlight->GetOperationParkingStand())
		 	objectID = pFlight->GetOperationParkingStand()->GetStandInput()->GetObjectName();
		else
		 	objectID = pFlight->GetFlightInput()->getStand();


		for (int i = 0; i < pPools->GetVehiclePoolCount(); i++)
		{
			CVehiclePool* pVehiclePool = pPools->GetVehiclePoolItem(i);
			std::vector<CVehicleServiceStandFamily*> standVector;
			pVehiclePool->GetServiceFitStand(objectID,standVector);

			for (size_t j = 0; j < standVector.size(); j++)
			{
				CVehicleServiceStandFamily* pStandFamily = standVector[j];
				if(pStandFamily->Fit(pFlight->GetLogEntry().GetAirsideDesc(),mode,pFlight->GetTime()))
				{
					int nID = pVehiclePool->GetParkingLotID();
					poolIDList.push_back(nID);
					break;
				}	
			}
		}
	}
}

void VehiclePoolsManagerInSim::GetFollowMeCarServicePool( AirsideMeetingPointInSim* pMeetingPoint, int nVehicleTypeID, std::vector<int>& vPoolIDs )
{
	CVehicleTypePools* pPools = NULL;

	for (int i =0; i< m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsCount(); i++ )
	{
		if ( m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i))	
			if ( m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i)->GetVehicleTypeID() == nVehicleTypeID)
			{
				pPools = m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i);
				break;
			}
	}

	if (pPools)
	{	
		ALTObjectID MeetingPointName = pMeetingPoint->GetMeetingPointInput()->GetObjectName();
		int nCount = pPools->GetVehiclePoolCount();
		for (int i =0; i < nCount;i++)
		{
			CVehiclePool* pServicePool = pPools->GetVehiclePoolItem(i);
			if (pServicePool)
			{
				FollowMeCarServiceMeetingPoints* pMeetingPoints = pServicePool->GetFollowMeCarServiceMeetingPoints();
				
				int nPointCounts = pMeetingPoints->GetServiceMeetingPointCount();
				for (int j =0; j < nPointCounts; j++)
				{
					if (MeetingPointName.idFits(pMeetingPoints->GetServiceMeetingPoint(j)) == FALSE)
						continue;
										
					int nID = pServicePool->GetParkingLotID();
					vPoolIDs.push_back(nID);				
					
				}
			}
		}
	}
}

CVehicleServiceTimeRange* VehiclePoolsManagerInSim::GetVehicleServicePoolDeploy(int nVehicleTypeID, int nParkingLotID, AirsideFlightInSim* pFlight)
{
	CVehicleTypePools* pPools = NULL;
	CVehicleServiceTimeRange* pTimeRange = NULL;

	for (int i =0; i< m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsCount(); i++ )
	{
		if ( m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i))	
			if ( m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i)->GetVehicleTypeID() == nVehicleTypeID)
			{
				pPools = m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i);
				break;
			}
	}

	if (pPools)
	{
		CVehiclePool* pPool = NULL;
		for (int i =0; i < pPools->GetVehiclePoolCount();i++)
		{
			if (pPools->GetVehiclePoolItem(i))
			{
				if (pPools->GetVehiclePoolItem(i)->GetParkingLotID() == nParkingLotID)
				{
					pPool = pPools->GetVehiclePoolItem(i);
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
								CVehicleServiceFlightType* pServiceFlightType = NULL;

								for (int j =0; j < pStandFamily->GetServiceFlightTypeCount(); j++)
								{
									if (pStandFamily->GetServiceFlightTypeItem(j))
									{
										FlightConstraint fltCnst = pStandFamily->GetServiceFlightTypeItem(j)->GetFltType();
										if(pFlight->fits(fltCnst))
										{
											pServiceFlightType = pStandFamily->GetServiceFlightTypeItem(j);
											break;
										}
									}
								}

								ElapsedTime tTime = pFlight->GetTime();

								if(pServiceFlightType)
								{
									for ( int j =0; j < pServiceFlightType->GetServiceTimeRangeCount(); j++)
									{
										if (pServiceFlightType->GetServiceTimeRangeItem(j))
										{
											pTimeRange = pServiceFlightType->GetServiceTimeRangeItem(j);
											if ( pTimeRange->GetStartTime() <= tTime && tTime <=pTimeRange->GetEndTime() )
											{
												return pTimeRange;
											}
										}
									}
								}

							}
						}
					}
				}
			}
		}
	}

	return pTimeRange;
}

void VehiclePoolsManagerInSim::GetTowTruckServicePool( AirsideFlightInSim* pFlight,int nVehicleTypeID,std::vector<int>& poolIDList )
{
	CVehicleTypePools* pPools = NULL;

	for (int i =0; i< m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsCount(); i++ )
	{
		if ( m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i))	
			if ( m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i)->GetVehicleTypeID() == nVehicleTypeID)
			{
				pPools = m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i);
				break;
			}
	}

	if (pPools)
	{	
		ALTObjectID objectID;
		if(pFlight->GetOperationParkingStand())
			objectID = pFlight->GetOperationParkingStand()->GetStandInput()->GetObjectName();
		else
			objectID = pFlight->GetFlightInput()->getStand();

		for (int i =0; i < pPools->GetVehiclePoolCount();i++)
		{
			if (pPools->GetVehiclePoolItem(i))
			{
				CVehicleServiceStandFamily* pStandFamily = NULL;

				for (int j =0; j < pPools->GetVehiclePoolItem(i)->GetServiceStandFamilyCount(); j++)
				{
					if ( objectID.idFits(pPools->GetVehiclePoolItem(i)->GetServiceStandFamilyItem(j)->GetStandALTObjectID() ) )
					{
						pStandFamily = pPools->GetVehiclePoolItem(i)->GetServiceStandFamilyItem(j);

						int nID = pPools->GetVehiclePoolItem(i)->GetParkingLotID();
						poolIDList.push_back(nID);
						break;
					}
				}
			}

		}
	}

}

CVehicleTypePools* VehiclePoolsManagerInSim::GetVehicleServicePool( int nVehicleTypeID )
{
	for (int i =0; i< m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsCount(); i++ )
	{
		if ( m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i))
		{
			if ( m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i)->GetVehicleTypeID() == nVehicleTypeID)
				return m_pVehiclePoolsAndDeployment->GetVehicleTypePoolsItem(i);
		}
	}

	return NULL;
}
