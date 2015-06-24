#pragma once
#include "vector"
#include "Common/elaptime.h"
#include "AirsideFlightInSim.h"

class StandInSim;
class ARCFlight;


struct StandAssignRecord 
{
	AirsideFlightInSim* m_pFlight;
	ElapsedTime m_tStart;
	ElapsedTime m_tEnd;

	bool operator < ( const StandAssignRecord& _record ) const
	{
		return m_tStart < _record.m_tStart;
	}
	FLIGHT_PARKINGOP_TYPE m_OpType;
};

class CStandAssignmentRegisterInSim
{
public:
	CStandAssignmentRegisterInSim(StandInSim* pStand);
	~CStandAssignmentRegisterInSim(void);


public:
	bool IsStandIdle(ElapsedTime tStart, ElapsedTime tEnd);

	void AddRecord(StandAssignRecord _Record);
	void DeleteRecord(int Idx);
	int FindRecord(AirsideFlightInSim* pFlight, FLIGHT_PARKINGOP_TYPE _type);
	//void GetFlightOccupancyTime(AirsideFlightInSim* pFlight,ElapsedTime& tStart, ElapsedTime& tEnd);

	void StandAssignmentRecordInit(AirsideFlightInSim* pFlight, FLIGHT_PARKINGOP_TYPE _type,ElapsedTime tStart, ElapsedTime tEnd);

	void GetFlightInTimeRange(ElapsedTime tStart, ElapsedTime tEnd, std::vector<ARCFlight*>& vFlights);

private:
	std::vector<StandAssignRecord> m_vStandAssignRecords;
	StandInSim* m_pStand;
};
