#include "StdAfx.h"

#include "FlightGroundRouteFinderStrategy.h"
#include "TaxiwayResource.h"
#include "AirsideFlightInSim.h"

//------------------------------------------------------------
// 
bool FlightGroundRouteFinderStrategy::SegCanServeFlight( FlightGroundRouteDirectSegInSim* pSeg, AirsideFlightInSim* pFlight ) const
{
	return pSeg->GetRouteSegInSim()->CanServeFlight(pFlight);
}
bool FlightGroundRouteFinderStrategy::IsRouteUndoubted( FlightGroundRouteDirectSegInSim* pSeg, AirsideFlightInSim* pFlight ) const
{
	return SegCanServeFlight(pSeg, pFlight);
}
double FlightGroundRouteFinderStrategy::EvalueWeight( FlightGroundRouteDirectSegInSim* pSeg, AirsideFlightInSim* pFlight ) const
{
	return pSeg->GetLength();
}
//------------------------------------------------------------


//------------------------------------------------------------
// 
SegExcludedFinderStrategy::SegExcludedFinderStrategy()
{

}

SegExcludedFinderStrategy::SegExcludedFinderStrategy( const SegList& segList )
: m_segList(segList)
{

}

SegExcludedFinderStrategy::SegExcludedFinderStrategy( FlightGroundRouteDirectSegInSim* pSeg )
{
	m_segList.push_back(pSeg);
}

void SegExcludedFinderStrategy::AddExcludedSeg( FlightGroundRouteDirectSegInSim* pSeg )
{
	if (std::find(m_segList.begin(), m_segList.end(), pSeg) == m_segList.end())
		m_segList.push_back(pSeg);
}

void SegExcludedFinderStrategy::RemoveExcludedSeg( FlightGroundRouteDirectSegInSim* pSeg )
{
	SegList::iterator ite = std::find(m_segList.begin(), m_segList.end(), pSeg);
	if (ite != m_segList.end())
		m_segList.erase(ite);
}

bool SegExcludedFinderStrategy::SegCanServeFlight( FlightGroundRouteDirectSegInSim* pSeg, AirsideFlightInSim* pFlight ) const
{
	return std::find(m_segList.begin(), m_segList.end(), pSeg) == m_segList.end()
		&& FlightGroundRouteFinderStrategy::SegCanServeFlight(pSeg, pFlight);
}
//------------------------------------------------------------

//------------------------------------------------------------
// 
// bool SpeedConstraintFindStrategy::SegCanServeFlight( FlightGroundRouteDirectSegInSim* pSeg, AirsideFlightInSim* pFlight ) const
// {
// 	
// }
bool SpeedConstraintFindStrategy::IsRouteUndoubted( FlightGroundRouteDirectSegInSim* pSeg, AirsideFlightInSim* pFlight ) const
{
	if (!pSeg->GetRouteSegInSim()->CanServeFlight(pFlight))
		return false;

	// check taxiway speed constraint
	if (pSeg->IsKindof(typeof(TaxiwayDirectSegInSim)))
	{
		TaxiwayDirectSegInSim* pTaxiDirSeg = (TaxiwayDirectSegInSim*)pSeg;
		double dConstrainedSpeed;
		if (pTaxiDirSeg->GetTaxiwaySeg()->GetTaxiwayConstrainedSpeed(pFlight, dConstrainedSpeed) && dConstrainedSpeed<m_dNormalSpeed)
		{
			return false;
		}
	}
	return true;
}
double SpeedConstraintFindStrategy::EvalueWeight( FlightGroundRouteDirectSegInSim* pSeg, AirsideFlightInSim* pFlight ) const
{
	double dLength = pSeg->GetLength();
	// check taxiway speed constraint
	if (pSeg->IsKindof(typeof(TaxiwayDirectSegInSim)))
	{
		TaxiwayDirectSegInSim* pTaxiDirSeg = (TaxiwayDirectSegInSim*)pSeg;
		double dConstrainedSpeed;
		if (pTaxiDirSeg->GetTaxiwaySeg()->GetTaxiwayConstrainedSpeed(pFlight, dConstrainedSpeed) && dConstrainedSpeed<m_dNormalSpeed)
		{
			return dLength*m_dNormalSpeed/dConstrainedSpeed;
		}
	}
	return dLength;
}
//------------------------------------------------------------
