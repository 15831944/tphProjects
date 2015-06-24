#include "StdAfx.h"
#include "DeiceVehicleServiceRequest.h"


DeiceVehicleServiceRequest::DeiceVehicleServiceRequest( AirsideFlightInSim* pFlight,const FlightDeiceRequirment& deiceRequir,const ElapsedTime& requestT )
{
	SetServiceFlight(pFlight);
	mDeiceRequir = deiceRequir;		
	m_requestTime = requestT;
	m_dAntiFliudUnHandled = mDeiceRequir.m_antiicefluidReq;
	m_dDeiceFluidUnHandled = mDeiceRequir.m_deicefluidReq;
	m_nDeiceCount= deiceRequir.m_deiceVehicleNum;
	m_nAntiIceCount = deiceRequir.m_antiiceVehicleNum;
}

bool DeiceVehicleServiceRequest::isRequestHandled() const
{
	return m_nDeiceCount+ m_nAntiIceCount <=0;//m_dAntiFliudUnHandled + m_dDeiceFluidUnHandled <= 0;
}

void DeiceVehicleServiceRequest::AssignVehicle( DeiceVehicleInSim* pVehicle )
{
	if(pVehicle )
	{
		if(m_nDeiceCount>0)
		{
			pVehicle->SetServType(DeiceVehicleInSim::DeIce);
			pVehicle->SetServFluid( min(pVehicle->getFluidAvaiable(),m_dDeiceFluidUnHandled) );
			m_dDeiceFluidUnHandled -= pVehicle->mdServFluid;
			m_nDeiceCount--;
		}
		else if(m_nAntiIceCount>0)
		{
			pVehicle->SetServType(DeiceVehicleInSim::AntiIce);
			pVehicle->SetServFluid( min(pVehicle->getFluidAvaiable(),m_dAntiFliudUnHandled) );
			m_dAntiFliudUnHandled -= pVehicle->mdServFluid;
			m_nAntiIceCount--;
		}
		AddServiceVehicle(pVehicle);
	}
}

DeiceVehicleInSim* DeiceVehicleServiceRequest::getVehicle( int idx ) const
{
	AirsideVehicleInSim* pVehicle = m_vServiceVehicle.at(idx);
	if(pVehicle && pVehicle->GetVehicleBaseType() == VehicleType_DeicingTruck)
	{
		return (DeiceVehicleInSim*)pVehicle;
	}
	return NULL;
}

void DeiceVehicleServiceRequest::flightCallDeice( const ElapsedTime& t )
{
	for(int i=0;i<getVehicleCount();i++)
	{
		DeiceVehicleInSim* pVehicle = getVehicle(i);
		if(pVehicle && pVehicle->mServType == DeiceVehicleInSim::DeIce)
		{
			pVehicle->ServiceFlight(t);
		}
	}
}

void DeiceVehicleServiceRequest::flightCallAntiIce( const ElapsedTime& t )
{
	for(int i=0;i<getVehicleCount();i++)
	{
		DeiceVehicleInSim* pVehicle = getVehicle(i);
		if(pVehicle && pVehicle->mServType == DeiceVehicleInSim::AntiIce)
		{
			pVehicle->ServiceFlight(t);
		}
	}
}

int DeiceVehicleServiceRequest::geVehicleIndex( AirsideVehicleInSim* pVehicle ) const
{
	for(int i=0;i<getVehicleCount();i++)
	{
		if(getVehicle(i) == pVehicle)
			return i;
	}
	return 0;
}

bool DeiceVehicleServiceRequest::IsCompleteService()
{
	return (m_nDeiceCount+ m_nAntiIceCount <=0) && (m_vServiceVehicle.size()==0);
}