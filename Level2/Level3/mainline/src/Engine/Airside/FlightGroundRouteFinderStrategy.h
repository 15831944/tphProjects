#pragma once

class AirsideFlightInSim;
class FlightGroundRouteDirectSegInSim;


// flight ground route find strategy
// 1. this strategy is used to decide the route finder's detailed behaviors
// 2. you can derive from this class to implement more behavior
// 3. to apply your own strategy class,
//    use CFlightGroundRouteFinder::ApplyStrategy(...), CFlightGroundRouteFinder::RestoreBasicStrategy(...)
//    or CFlightGroundRouteFinder::StrategyApplier
class FlightGroundRouteFinderStrategy
{
public:
	// check whether the route segment can serve the flight
	// if not, when initializing the route network graph, the segment will not be added into the graph
	// NOTE: pSeg and pFlight won't be NULL, you do not need to check this
	// -> TO SEE: void CFlightGroundRouteFinder::InitBoostGraph(...)
	virtual bool SegCanServeFlight(FlightGroundRouteDirectSegInSim* pSeg, AirsideFlightInSim* pFlight) const;

	// check whether the segment has some indeterminate condition
	// which makes the segment not the best choice for the flight
	// for example, the taxiway speed constraint.
	// here it works the same as SegCanServeFlight, but you can add more to your derived class
	// NOTE: pSeg and pFlight won't be NULL, you do not need to check this
	// TO SEE: bool CFlightGroundRouteFinder::GetRoute(...)
	virtual bool IsRouteUndoubted(FlightGroundRouteDirectSegInSim* pSeg, AirsideFlightInSim* pFlight) const;

	// evaluate the segment's weight
	// normally, it is just the length of segment
	// NOTE: pSeg and pFlight won't be NULL, you do not need to check this
	// TO SEE: bool CFlightGroundRouteFinder::GetRoute(...)
	virtual double EvalueWeight(FlightGroundRouteDirectSegInSim* pSeg, AirsideFlightInSim* pFlight) const;
	
};

class SegExcludedFinderStrategy : public FlightGroundRouteFinderStrategy
{
public:
	typedef std::vector<FlightGroundRouteDirectSegInSim*> SegList;

	SegExcludedFinderStrategy();
	SegExcludedFinderStrategy(const SegList& segList);
	SegExcludedFinderStrategy(FlightGroundRouteDirectSegInSim* pSeg);

	void AddExcludedSeg(FlightGroundRouteDirectSegInSim* pSeg);
	void RemoveExcludedSeg(FlightGroundRouteDirectSegInSim* pSeg);

	virtual bool SegCanServeFlight(FlightGroundRouteDirectSegInSim* pSeg, AirsideFlightInSim* pFlight) const;

private:
	SegList m_segList;
};

class SpeedConstraintFindStrategy : public FlightGroundRouteFinderStrategy
{
public:
	SpeedConstraintFindStrategy(double dNormalSpeed)
		: m_dNormalSpeed(dNormalSpeed)
	{

	}

// 	virtual bool SegCanServeFlight(FlightGroundRouteDirectSegInSim* pSeg, AirsideFlightInSim* pFlight) const;
	virtual bool IsRouteUndoubted(FlightGroundRouteDirectSegInSim* pSeg, AirsideFlightInSim* pFlight) const;
	virtual double EvalueWeight(FlightGroundRouteDirectSegInSim* pSeg, AirsideFlightInSim* pFlight) const;

private:
	double m_dNormalSpeed;
};