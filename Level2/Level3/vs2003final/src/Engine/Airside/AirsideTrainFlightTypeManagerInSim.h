#pragma once
class CTrainingFlightTypeManage;
class RunwayExitInSim;
class AirsideFlightInSim;
class RunwayResourceManager;
class CTrainingTimeRangeManage;

class AirsideTrainFlightTypeManagerInSim
{
public:
	AirsideTrainFlightTypeManagerInSim(CTrainingFlightTypeManage* pTrainFlightTypeManager);
	~AirsideTrainFlightTypeManagerInSim(void);

	RunwayExitInSim* GetTakeoffPosition( AirsideFlightInSim* pFlight, RunwayResourceManager * pRunwayRes );

private:
	CTrainingTimeRangeManage* getTimeItem(AirsideFlightInSim* pFlight);
private:
	CTrainingFlightTypeManage* m_pTrainFlightTypeManager;
};
