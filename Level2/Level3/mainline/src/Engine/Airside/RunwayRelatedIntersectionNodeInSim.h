#pragma once

#include "../../InputAirside/ALT_BIN.h"
#include "../../Common/elaptime.h"
#include "../../Common/Point2008.h"
#include "CommonInSim.h"

class CRunwaySystem;
class LogicRunwayInSim;
class AirsideFlightInSim;

enum RWYRELATION{
	LANDING_LANDING,
	LANDING_TAKEOFF,
	TAKEOFF_TAKEOFF
};

class RunwayRelatedIntersectionNodeInSim
{
private:
	struct RunwayRelatedIntersectionNodeOccupyInstance
	{
		AirsideFlightInSim* m_pOccFlight;
		AirsideMobileElementMode m_eMode;
		ElapsedTime m_tOccTime;

		bool operator < (const RunwayRelatedIntersectionNodeOccupyInstance& _occupancy) const
		{
			return m_tOccTime <= _occupancy.m_tOccTime;
		}
	};

public:
	RunwayRelatedIntersectionNodeInSim(CRunwaySystem* pRunwaySystem, LogicRunwayInSim* pRunway1, LogicRunwayInSim* pRunway2, AIRCRAFTOPERATIONTYPE eACOpType);
	~RunwayRelatedIntersectionNodeInSim(void);

	void SetPosition(const CPoint2008& point){ m_Position = point; }
	void SetIntersectType(RunwayIntersectionType eType){ m_PosType = eType; }

	LogicRunwayInSim* GetOpRunway1(){ return m_pRunway1; }
	LogicRunwayInSim* GetOpRunway2(){ return m_pRunway2; }

	RWYRELATION GetOperationType(){ return m_ACOpType; }

	const CPoint2008& GetPosition(){ return m_Position; }
	RunwayIntersectionType GetIntersectionType(){ return m_PosType; }

	ElapsedTime GetFlightDelayTime(AirsideFlightInSim* pOpFlight, ElapsedTime tTimeAtPoint);

	bool IsSameNode(const LogicRunwayInSim* pRwy1,const LogicRunwayInSim* pRwy2, const CPoint2008& pos, AIRCRAFTOPERATIONTYPE eOpType);

private:
	AIRCRAFTOPERATIONTYPE GetAcOperationType( AirsideMobileElementMode eTrailMode, AirsideMobileElementMode eLeadMode);
	void AddNewInstance(AirsideFlightInSim* pOccFlight, AirsideMobileElementMode eMode, ElapsedTime tOccTime);

	
private:
	LogicRunwayInSim* m_pRunway1;
	LogicRunwayInSim* m_pRunway2;
	CPoint2008 m_Position;

	RWYRELATION m_ACOpType;
	RunwayIntersectionType m_PosType;

	std::vector<RunwayRelatedIntersectionNodeOccupyInstance> m_vOccupyTable;

	CRunwaySystem* m_pRunwaySystem;
};
