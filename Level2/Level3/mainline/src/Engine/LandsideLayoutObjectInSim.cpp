#include "StdAfx.h"
#include ".\landsidelayoutobjectinsim.h"
#include "landside\LandsideStretch.h"
#include "Common\ARCUnit.h"
#include "landside\SpeedControlData.h"
#include "landside/FacilityBehavior.h"
#include "LandsidePaxTypeLinkageInSimManager.h"
#include "Landside/VehicleBehaviorDistributions.h"





LandsideLayoutObjectInSim::LandsideLayoutObjectInSim( LandsideFacilityLayoutObject* pInput ) :m_pInput(pInput)
{
	m_pSpeedControl = NULL;
	m_pBehavior  = NULL;
	m_pPaxLeadToLinkage = new LandsidePaxTypeLinkageInSim();
	m_pServiceTime = NULL;
}


LandsideLayoutObjectInSim::~LandsideLayoutObjectInSim( void )
{
	delete m_pPaxLeadToLinkage;
}

ALTOBJECT_TYPE LandsideLayoutObjectInSim::getType() const
{
	return m_pInput->GetType();
}

//double LandsideLayoutObjectInSim::GetSpeedConstrain() const
//{
//	double dSpd = ARCMath::DISTANCE_INFINITE;
//	if(m_pSpeedControl )
//	{
//		return m_pSpeedControl->GetStretchSpeed();
//	}
//	return dSpd;
//}

bool LandsideLayoutObjectInSim::IsHaveCapacity(LandsideVehicleInSim*pVeh) const
{
	if(m_pBehavior){
		return m_pBehavior->isCapacityLimit();
	}
	return false;
}

bool LandsideLayoutObjectInSim::IsHaveCapacityAndFull( LandsideVehicleInSim*pVeh ) const
{
	return IsHaveCapacity(pVeh)&&false;
}

int LandsideLayoutObjectInSim::getID() const
{
	if(getInput()){
		return m_pInput->getID();
	}
	return -1;
}

void LandsideLayoutObjectInSim::SetBehavior( CFacilityBehavior* pBeh )
{
	m_pBehavior = pBeh;
}

CFacilityBehavior* LandsideLayoutObjectInSim::GetBehavior()
{
	return m_pBehavior;
}
bool LandsideLayoutObjectInSim::CanLeadTo(const ElapsedTime& eTime, LandsideLayoutObjectInSim *pObject ) const
{
	//here it could be a bus station or parking lot
	//the bus station is embedded in the parking lot sometimes, so there are connected, 
	//if passenger wants take bus to parking lot, could go through the bus station
	if(m_pPaxLeadToLinkage)
		return m_pPaxLeadToLinkage->CanLeadTo(eTime,pObject->getInput()->getName());

	return false;
}
LandsidePaxTypeLinkageInSim * LandsideLayoutObjectInSim::getPaxLeadToLinkage() const
{
	return m_pPaxLeadToLinkage;
}

const ALTObjectID& LandsideLayoutObjectInSim::getName() const
{
	return m_pInput->getName();
}

ElapsedTime LandsideLayoutObjectInSim::GetServiceTime( const CHierachyName& vehicleType ) const
{
	if(m_pServiceTime)
	{
		if(VBDistributionObjectServiceTime* pObjSerTime = m_pServiceTime->FindMatchType(vehicleType))
		{
			return pObjSerTime->getRandomTime();
		}
	}
	return ElapsedTime(0L);
}