#pragma once
#include "map"

class CRunwayTakeOffPosition;
class CRunwayTakeOffPositionData;
class LogicRunwayInSim;
class AirsideFlightInSim;
class RunwayExitInSim;
class CAirportDatabase;
class ElapsedTime;
class RunwayResourceManager;

enum BACKTRACKTYPE
{
	MaxBacktrack = 0,
	MinBacktrack,
	NoBacktrack
};


class CRunwayTakeoffPositionAssignInSim
{
public:
	CRunwayTakeoffPositionAssignInSim(void);
	~CRunwayTakeoffPositionAssignInSim(void);

	//RunwayExitInSim* GetTakeoffPositionByFlightType(AirsideFlightInSim* pFlight);
	RunwayExitInSim* GetTakeoffPositionByFlightType(LogicRunwayInSim* pRunway, AirsideFlightInSim* pFlight);
	void Init(int nPrjId,CAirportDatabase* pDatabase,RunwayResourceManager * pRunwayResManager);

	typedef std::vector<CRunwayTakeOffPositionData*> TakeoffPosList;

	BACKTRACKTYPE GetBacktrackTypeOfTakeoffPos( AirsideFlightInSim* pFlight);


private:
	void GetRunwayTakeoffPositions(LogicRunwayInSim* pLogicRunway);
	void ShowErrorMessageAboutTakeoffPos(RunwayResourceManager * pRunwayResManager);//if runway exit miss, show the message
	void CheckRunwayExitOfTakaoffPos(LogicRunwayInSim* pLogicRunway);
	
	std::map<LogicRunwayInSim*,TakeoffPosList> m_mapTakeoffPosList;

	CRunwayTakeOffPosition* m_pRunwayTakeoffPosition;
};
