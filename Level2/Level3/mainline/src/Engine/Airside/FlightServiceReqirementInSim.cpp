#include "StdAfx.h"
#include ".\flightservicereqirementinsim.h"
#include "AirsideFlightInSim.h"
#include "InputAirside/FlightServicingRequirementList.h"
#include "InputAirside/FlightServicingRequirement.h"
#include "InputAirside/ServicingRequirementList.h"
#include "InputAirside/ServicingRequirement.h"
#include "common/AirportDatabase.h"
#include "FlightServiceInformation.h"
#include "../../InputAirside/VehicleSpecifications.h"


FlightServiceReqirementInSim::FlightServiceReqirementInSim(void)
{
	m_pFlightServicingRequirementList = new CFlightServicingRequirementList;
	m_pVehicleSpecifications = NULL;
}

FlightServiceReqirementInSim::~FlightServiceReqirementInSim(void)
{
	if (NULL != m_pVehicleSpecifications)
	{
		delete m_pVehicleSpecifications;
		m_pVehicleSpecifications = NULL;
	}
}

void FlightServiceReqirementInSim::Init(int nPrjID, CAirportDatabase* pDatabase)
{
	m_pFlightServicingRequirementList->SetAirportDatabase(pDatabase);

	m_pVehicleSpecifications = new CVehicleSpecifications();
	m_pVehicleSpecifications->ReadData(nPrjID);

	m_pFlightServicingRequirementList->ReadData(nPrjID, m_pVehicleSpecifications);
}

bool FlightServiceReqirementInSim::GetFlightGeneralVehicleServicingRequirement(AirsideFlightInSim* pFlight, std::vector<VehicleServiceRequest*>& requestList)
{
	CFlightServicingRequirement* pFlightRequirement = NULL;
	int nCount = m_pFlightServicingRequirementList->GetElementCount();
	for (int i =0; i < nCount; i++)
	{
		FlightConstraint fltCnst = m_pFlightServicingRequirementList->GetItem(i)->GetFltType();
		if (pFlight->fits(fltCnst))
		{
			pFlightRequirement = m_pFlightServicingRequirementList->GetItem(i);
			break;
		}
	}
	if (pFlightRequirement)
	{
		CServicingRequirement* pRequirement = NULL;
		nCount = pFlightRequirement->GetElementCount();
		for (int i = 0; i < nCount; i++)
		{
			pRequirement = pFlightRequirement->GetItem(i);
			if (pRequirement)
			{
				int nVehicleTypeID = pRequirement->GetServicingRequirementNameID();
				CVehicleSpecificationItem *pVehicleSpecItem = m_pVehicleSpecifications->GetVehicleItemByID(nVehicleTypeID);
				if (pVehicleSpecItem->getBaseType() != VehicleType_General)
				{
					continue;
				}

				CFlightServiceInformation* pInfo =pFlight->GetServicingInformation(); 
				int nServicePointCount = pInfo->GetFltServicePointCount(pRequirement->GetServicingRequirementNameID());
				int nServiceCount = 0;
				int nSpareCount = 0;
				if (pRequirement->GetNum() > 0)
				{
					nServiceCount = nServicePointCount / pRequirement->GetNum();
					nSpareCount = nServicePointCount % pRequirement->GetNum();
				}
				for ( int j = 0; j < pRequirement->GetNum(); j++)
				{
					GeneralVehicleServiceRequest* pRequest = new GeneralVehicleServiceRequest;
					pRequest->SetServiceVehicleTypeID(pRequirement->GetServicingRequirementNameID());
					pRequest->SetServiceFlight(pFlight);
					if (pRequirement->GetServiceTimeDistribution())
						pRequest->SetServiceTime(ProbabilityDistribution::CopyProbDistribution(pRequirement->GetServiceTimeDistribution()));
					if(pRequirement->GetSubServiceTimeDistribution())
						pRequest->SetSubServiceTime(ProbabilityDistribution::CopyProbDistribution(pRequirement->GetSubServiceTimeDistribution()));

					if (nSpareCount > 0)
					{
						pRequest->SetServiceCount(nServiceCount +1);
						nSpareCount--;
					}
					else
						pRequest->SetServiceCount(nServiceCount);

					requestList.push_back(pRequest);
				}
			}
		}
	}

	return true;

}

BOOL FlightServiceReqirementInSim::HavePaxTruckForFlight(AirsideFlightInSim* pFlight)
{
   //std::vector<CVehicleSpecificationItem *> list_vehicle ;
   std::vector<CServicingRequirement* > list_servicerequest ;
   GetAvailableVehicleTypeByBaseType(pFlight,VehicleType_PaxTruck,/*list_vehicle,*/list_servicerequest) ;
   if(list_servicerequest.size() == 0)
	   return FALSE ;
   else
	   return TRUE ;
}
bool FlightServiceReqirementInSim::GetAvailableVehicleTypeByBaseType(AirsideFlightInSim* pFlight,enumVehicleBaseType baseType,
																	/* std::vector<CVehicleSpecificationItem *>& vVehicleTypeSpec,*/
																	 std::vector<CServicingRequirement* >& vServiceRequirement)
{
	//vVehicleTypeSpec.clear();
	CFlightServicingRequirement* pFlightRequirement = NULL;
	int nCount = m_pFlightServicingRequirementList->GetElementCount();
	for (int i =0; i < nCount; i++)
	{
		FlightConstraint fltCnst = m_pFlightServicingRequirementList->GetItem(i)->GetFltType();
		if (pFlight->fits(fltCnst))
		{
			pFlightRequirement = m_pFlightServicingRequirementList->GetItem(i);
			break;
		}
	}
	if (pFlightRequirement)
	{
		CServicingRequirement* pRequirement = NULL;
		nCount = pFlightRequirement->GetElementCount();
		for (int i = 0; i < nCount; i++)
		{
			pRequirement = pFlightRequirement->GetItem(i);
			if (pRequirement && pRequirement->GetNum() > 0 )
			{

				int nVehicleTypeID = pRequirement->GetServicingRequirementNameID();
				CVehicleSpecificationItem *pVehicleSpecItem = m_pVehicleSpecifications->GetVehicleItemByID(nVehicleTypeID);
				if (pVehicleSpecItem->getBaseType() == baseType)
				{
					//vVehicleTypeSpec.push_back(pVehicleSpecItem);
					vServiceRequirement.push_back(pRequirement);
				}
			}
		}
	}

	return true;
}

std::vector<int> FlightServiceReqirementInSim::GetVehicleTypeIDByBaseType( enumVehicleBaseType baseType )
{
	std::vector<int> vRets;
	m_pVehicleSpecifications->GetVehicleIDByBaseType(baseType,vRets);
	return vRets;
}
