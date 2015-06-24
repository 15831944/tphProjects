#include "StdAfx.h"
#include "TaxiwaySegConstraintsProperties.h"
#include <algorithm>
#include "AirsideFlightInSim.h"
#include "TaxiwayResource.h"
#include <limits>
#include <common/CARCUnit.h>
//---------------------------------------------------------------------------------------------
//WeightWingspanConstraintsInSim
void WeightWingspanConstraintsInSim::addItem(TaixwayConstraintType type, int nValue)
{
	if (!isExist(type))
		m_mapWeightWingspanCons.insert( std::make_pair( type, nValue ) );
}

bool WeightWingspanConstraintsInSim::isExist(TaixwayConstraintType type) const
{
	std::map< TaixwayConstraintType, int >::const_iterator iter = m_mapWeightWingspanCons.find(type);
	return iter != m_mapWeightWingspanCons.end();

}

bool WeightWingspanConstraintsInSim::canServe(const AirsideFlightInSim *pFlight) const
{
	TaixwayConstraintType type= WeightConstraint;
	if (isExist(type))
	{	
		double dWeight = pFlight->GetWeight();
		std::map< TaixwayConstraintType, int >::const_iterator iter = m_mapWeightWingspanCons.find(type);
	
		if(static_cast<int>(dWeight) > CARCWeightUnit::ConvertWeight(AU_WEIGHT_POUND,DEFAULT_DATABASE_WEIGHT_UNIT,iter->second) )
			return false;
	}

	type= WingSpanConstraint;
	if (isExist(type))
	{	
		double dWingSpan =  pFlight->GetWingspan();
		std::map< TaixwayConstraintType, int >::const_iterator iter = m_mapWeightWingspanCons.find(type);

		if(static_cast<int>(dWingSpan) > CARCLengthUnit::ConvertLength(AU_LENGTH_METER,DEFAULT_DATABASE_LENGTH_UNIT,iter->second) )
			return false;
	}
	return true;
}


//---------------------------------------------------------------------------------------------
//FlightTypeRestrictionsInSim

bool FlightTypeRestrictionsInSim::isExist(FlightConstraint& cons)
{
//	std::vector< FlightConstraint>::iterator iter = 
//		std::find(m_vectFltTypeCons.begin(), m_vectFltTypeCons.end(), cons);

//	return iter != m_vectFltTypeCons.end();
	ASSERT(0);
	return false;
}

void FlightTypeRestrictionsInSim::addItem(FlightConstraint& cons)
{
	//if (!isExist(cons))
	m_vectFltTypeCons.push_back(cons);
}

bool FlightTypeRestrictionsInSim::canServe(const AirsideFlightInSim* pFlight) const
{
	if(m_vectFltTypeCons.size() == 0)
		return true;

	std::vector< FlightConstraint>::const_iterator iter = m_vectFltTypeCons.begin();
	for(;iter != m_vectFltTypeCons.end(); ++iter)
	{
		if (pFlight->fits(*iter))
			return false;
	}

	return true;
}


//---------------------------------------------------------------------------------------------
//DirectionalityConstraintsInSim

void DirectionalityConstraintsInSim::SetDirectionConstraintType(DirectionalityConstraintType emDir)
{
	m_emDirection = emDir;
}

void DirectionalityConstraintsInSim::SetBearingDegree(double dValue)
{
	m_dBearingDegree = dValue; 
}

bool DirectionalityConstraintsInSim::canServe(DirectionalityConstraintType emType, double dBearingDegree)
{
	return true;
	//TODO:

	////if(emType == Direction_Bearing)
	////	return m_dBearingDegree >= dBearingDegree;
	////else
	////	return m_emDirection == emType;

}

//////////////////////////////////////////////////////////////////////////
//
CTaxiwayClosureConstraint::CTaxiwayClosureConstraint()
{

}
CTaxiwayClosureConstraint::~CTaxiwayClosureConstraint()
{

}
void CTaxiwayClosureConstraint::addItem(ElapsedTime epStartTime,ElapsedTime epEndTime)
{
	m_vTimeConstraint.push_back(ClosureTimeRage(epStartTime,epEndTime));
}

bool CTaxiwayClosureConstraint::canServe(const AirsideFlightInSim *pFlight) const
{
	std::vector<ClosureTimeRage>::const_iterator iter= m_vTimeConstraint.begin();

	for (;iter != m_vTimeConstraint.end();++iter)
	{
		if((*iter).TimeInClosureRange(pFlight->GetTime()))
			return false;
	}

	return true;
}

//------------------------------------------------------------
// TaxiSpeedConstraintsInSim
bool TaxiSpeedConstraintsInSim::GetConstrainedSpeed( const AirsideFlightInSim* pFlight, double& dSpeed ) const
{
	bool bFound = false;
	dSpeed = (std::numeric_limits<double>::max)();

	dataset_type::const_iterator ite = m_vTaxiSpeedConstraints.begin();
	dataset_type::const_iterator iteEn = m_vTaxiSpeedConstraints.end();
	for (;ite!=iteEn;++ite)
	{
		if(pFlight->fits(ite->first) && ite->second<dSpeed) // find the most restrict speed constraint
		{
			dSpeed = ite->second;
			bFound = true;
		}
	}

	return bFound;
}

void TaxiSpeedConstraintsInSim::addItem( const FlightConstraint& cons, double dSpeed )
{
	addItem(value_type(cons, dSpeed));
}

void TaxiSpeedConstraintsInSim::addItem( const value_type& item )
{
	m_vTaxiSpeedConstraints.push_back(item);
}
//------------------------------------------------------------

//---------------------------------------------------------------------------------------------
//TaxiwaySegConstraintsProperties
TaxiwaySegConstraintsProperties::TaxiwaySegConstraintsProperties(void)
{

}

TaxiwaySegConstraintsProperties::~TaxiwaySegConstraintsProperties(void)
{
}


//TODO:
bool TaxiwaySegConstraintsProperties::canServe(const AirsideFlightInSim *pFlight) const
{
	TaixwayConstraintType type = WeightConstraint;
	int nValue = 0;
	if(!m_weightWingspanCons.canServe(pFlight))
		return false;

	if(!m_fltTypeCons.canServe(pFlight))
		return false;


	//not implement
	DirectionalityConstraintType dir = Direction_Bearing;
	double degree;
	if(dir == Direction_Bearing)
		degree = 30.0;

	//if(!m_directionCons.canServe(dir, degree))
	//	return false;
	
	if (!m_closureConstraint.canServe(pFlight))
		return false;


	return true;
}

bool TaxiwaySegConstraintsProperties::GetTaxiwayConstrainedSpeed( const AirsideFlightInSim* pFlight, double& dSpeed ) const
{
	return m_taxiSpeedConstraints.GetConstrainedSpeed(pFlight, dSpeed);
}

void TaxiwaySegConstraintsProperties::AddWeightWingspanConstraint(TaixwayConstraintType type, int nValue)
{
	m_weightWingspanCons.addItem(type, nValue);
}

void TaxiwaySegConstraintsProperties::AddFlightTypeRestrictions(FlightConstraint& fltCons)
{
	m_fltTypeCons.addItem(fltCons);
}

void TaxiwaySegConstraintsProperties::SetDirectionConstraintType(DirectionalityConstraintType emDir)
{
	m_directionCons.SetDirectionConstraintType(emDir);
}

void TaxiwaySegConstraintsProperties::SetBearingDegree(double dValue)
{
	m_directionCons.SetBearingDegree(dValue);
}
void TaxiwaySegConstraintsProperties::AddClosureTimeConstraint(ElapsedTime epStartTime,ElapsedTime epEndTime)
{
	m_closureConstraint.addItem(epStartTime,epEndTime);
}

void TaxiwaySegConstraintsProperties::AddTaxiSpeedConstraint( const FlightConstraint& cons, double dSpeed )
{
	m_taxiSpeedConstraints.addItem(cons, dSpeed);
}

