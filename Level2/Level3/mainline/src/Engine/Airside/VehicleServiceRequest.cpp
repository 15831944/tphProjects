#include "StdAfx.h"
#include ".\vehicleservicerequest.h"
#include "AirsideFlightInSim.h"
#include "AirsideVehicleInSim.h"

VehicleServiceRequest::VehicleServiceRequest(void)
{
	m_pServiceFlight = NULL;
	m_nServiceVehicleTypeID = 0;
	m_nServiceCount = 0;   //
	m_pServiceTimeDistribution = NULL;
	m_pSubServiceTimeDistribution = NULL;
	m_bProceed = false;
}

VehicleServiceRequest::~VehicleServiceRequest(void)
{
}


void VehicleServiceRequest::AddServiceVehicle(AirsideVehicleInSim * pVehicle)
{
	m_vServiceVehicle.push_back(pVehicle);
}
void VehicleServiceRequest::RemoveServiceVehicle(AirsideVehicleInSim *pVehicle)
{
	std::vector<AirsideVehicleInSim *>::iterator iter = std::find(m_vServiceVehicle.begin(),m_vServiceVehicle.end(),pVehicle);
	if(iter!=m_vServiceVehicle.end())
		m_vServiceVehicle.erase(iter);	
}

bool VehicleServiceRequest::bHasVehicle( AirsideVehicleInSim* pVehicle ) const
{
	std::vector<AirsideVehicleInSim *>::const_iterator iter = std::find(m_vServiceVehicle.begin(),m_vServiceVehicle.end(),pVehicle);	
	return	iter!=m_vServiceVehicle.end();
}

ElapsedTime VehicleServiceRequest::getRequestTime()
{
	return m_pServiceFlight->GetEstimateStandTime();
}

bool VehicleServiceRequest::IsCompleteService()
{
	if(m_vServiceVehicle.empty() && IsProceed())
		return true;
	return false;
}

void VehicleServiceRequest::WakeupAllPaxBusCompleteService(AirsideFlightInSim* pFlight)
{
	if (m_pServiceFlight != pFlight)
		return;
	
	if (IsCompleteService())
		return;
	std::vector<AirsideVehicleInSim *> vVehicles = m_vServiceVehicle;

	for (size_t i = 0; i < vVehicles.size(); i++)
	{
		AirsideVehicleInSim* pVehicle = vVehicles[i];
		/*if(pVehicle->GetResource() && pVehicle->GetResource()->GetType() == AirsideResource::ResType_PaxBusParking)
		{
			pVehicle->SetTime(pFlight->GetTime());
			pVehicle->GetNextCommand();
		}*/
	/*	if(pVehicle->GetResource() && pVehicle->GetResource()->GetType() == AirsideResource::ResType_VehicleStretchLane)
		{
			pVehicle->SetTime(pFlight->GetTime());
			pVehicle->CancelCurService(pFlight->GetTime());
		}*/
		
	}
}

void VehicleServiceRequest::SetProceed( bool bProceed )
{
	m_bProceed = bProceed;
}

bool VehicleServiceRequest::IsProceed() const
{
	return m_bProceed;
}
//////////////////////////////////////////////////////////////////////////
GeneralVehicleServiceRequest::GeneralVehicleServiceRequest()
{
}



bool VehicleServiceRequest::OrderLess( VehicleServiceRequest* q1, VehicleServiceRequest* q2 )
{
	if(q1 && q2)
		return q1->getRequestTime() < q2->getRequestTime();
	return false;
}
