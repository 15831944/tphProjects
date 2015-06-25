#include "StdAfx.h"
#include ".\baggagetrainservicerequest.h"

BaggageTrainServiceRequest::BaggageTrainServiceRequest(void)
{
	m_nBaggageCount = 0;
	m_nBaggageLeft = 0;
}

BaggageTrainServiceRequest::~BaggageTrainServiceRequest(void)
{
}

void BaggageTrainServiceRequest::setBaggageCount( int nTotalBagNeedToHandle )
{
	m_nBaggageCount = nTotalBagNeedToHandle;
}

void BaggageTrainServiceRequest::setUnserviceBaggageCount( int nLeft )
{
	m_nBaggageLeft = nLeft;
}

bool BaggageTrainServiceRequest::IsCompleteService()
{
	if (m_nBaggageLeft >0 )
		return false;


	return VehicleServiceRequest::IsCompleteService();
	//return m_nBaggageLeft == 0;
}

enumVehicleBaseType BaggageTrainServiceRequest::GetType() const
{
	return VehicleType_BaggageTug;
}

FlightOperation BaggageTrainServiceRequest::getFltOperation() const
{
	return m_enumFltOperation;
}

void BaggageTrainServiceRequest::setFltOperation( FlightOperation enumOp )
{
	m_enumFltOperation = enumOp;
}

int BaggageTrainServiceRequest::GetBaggageLeft() const
{
	return m_nBaggageLeft;
}

//bool BaggageTrainServiceRequest::IsArrival()
//{
//	return m_enumFltOperation == ARRIVAL_OPERATION;
//}



void BaggageTrainServiceRequest::SetTermProcList( std::vector<Processor *>& vTermProcs )
{
	m_vTermProcs = vTermProcs;
}

void BaggageTrainServiceRequest::SetPakringSpotList( std::vector<CBagCartsParkingSpotInSim *>& vParkingSpot )
{
	m_vParkingSpot = vParkingSpot;
}

Processor * BaggageTrainServiceRequest::getTermPocessor()
{
	if(m_vTermProcs.size())
	{
		return m_vTermProcs.at(0);
	}
	return NULL;
}

CBagCartsParkingSpotInSim * BaggageTrainServiceRequest::getParkingSpot()
{
	if(m_vParkingSpot.size())
	{
		return m_vParkingSpot.at(0);
	}
	return NULL;
}



