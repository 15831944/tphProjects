#pragma once

namespace airside_engine
{

class FlightList;
class FlightEvent;
class SimFlight;

class ConflictAdjustment
{
public:
	ConflictAdjustment(FlightList& flights);
	void Run();

private:
	FlightList& m_flights;

	void AdjustConflict(SimFlight& flight1, SimFlight& flight2);
	void CheckAndResolveConflict(FlightEvent& segment1StartEvent, FlightEvent& segment1EndEvent,
								 FlightEvent& segment2StartEvent, FlightEvent& segment2EndEvent);
};

}