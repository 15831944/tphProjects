#pragma once
#include "../EngineDll.h"

class RunwayInSim;
class CWaveCrossingSpecificationItem;
class HoldPositionInSim;
class AirsideFlightInSim;
class ElapsedTime;
class ENGINE_TRANSFER CRunwayCrossSpecificationInSim
{
public:
	CRunwayCrossSpecificationInSim(RunwayInSim *pRunway);
	~CRunwayCrossSpecificationInSim(void);
public:
	//function: check the runway need to do wave cross operation or not
	//return true, need to pause runway's landing or take off operation, start to cross runway operation
	//return false, do nothing
	BOOL CheckNeedWaveCross(AirsideFlightInSim *pCurFlight,const ElapsedTime& eTime);
	
	BOOL NotifyWaveCross(AirsideFlightInSim *pCurFlight,const ElapsedTime& eTime);

	void AddHoldPositionInSim(HoldPositionInSim *pHoldPositionInSim);
	void SetWaveCrossSpecificationItem(CWaveCrossingSpecificationItem * pWaveCrossSpecItem);
protected:
	BOOL CheckLandWaveCross(AirsideFlightInSim *pCurFlight,AirsideFlightInSim *pNextFlight,std::vector<AirsideFlightInSim *>& vAfterFlight,const ElapsedTime& eTime);
	BOOL CheckTakeOffWaveCross(AirsideFlightInSim *pCurFlight,AirsideFlightInSim *pNextFlight,std::vector<AirsideFlightInSim *>& vAfterFlight,const ElapsedTime& eTime);

	BOOL AddAfterFlightToRunwayWaitingFlight(std::vector<AirsideFlightInSim *>& vAfterFlight,const ElapsedTime& eEventTime);
protected:
	//the hold position pointer, user settings
	std::vector<HoldPositionInSim *> m_vHoldPositionInSim;
	//point to the runway  which have wave cross settings
	RunwayInSim *m_pRunwayInSim;
	//user settings
	CWaveCrossingSpecificationItem *m_pWaveCrossSpec;

protected:

};
