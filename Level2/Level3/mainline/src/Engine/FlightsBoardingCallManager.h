#pragma once
#include "Common/elaptime.h"
#include <CommonData/procid.h>

class FlightItemBoardingCallMgr;
class ProcessorList;
class FlightSchedule;
class Flight;
class FlightData;
class InputTerminal;
class BoardingCallEvent;
class CSinglePaxTypeFlow;
class FlightsBoardingCallManager
{
public:
	FlightsBoardingCallManager();
	virtual ~FlightsBoardingCallManager(void);

	void Initialize(const ProcessorList *p_procs, const FlightSchedule *pSchedule, 
		const FlightData *p_data, InputTerminal* _pInTerm);
	void ResetContext();

	void ScheduleBoardingCallEvents(const Flight* pFlight, const ElapsedTime& actualDepTime,const ElapsedTime& depTime);
	void ScheduleFltWithoutStandBoardingCallEvents();
	void ScheduleBoardingCallEventsDirectly();

	FlightItemBoardingCallMgr* GetFlightItemBoardingCallMgr(const Flight* pFlight);
	void ScheduleSecondBoardingCallEvents();


protected:
	void LoadDefaultBoardingCalls(const ProcessorList *p_procs, const FlightData *p_data, InputTerminal* _pInTerm);
	void LoadDefaultBoardingCallEventToFltItem(const Flight* pFlight, BoardingCallEvent* pEvent);

	//int GetFlightIndex(const Flight* pFlight); 

	bool CheckBoardingCallValid(const Flight* pFlight,const HoldingArea* pHoldArea,InputTerminal* _pInTerm);
	bool GetProcIDInSubFlowList(const ProcessorID& ProcID,std::vector<ProcessorID>& ProIDList,InputTerminal* _pInTerm);
	bool IsSubFlowInPaxFlow(const CSinglePaxTypeFlow& pSingleFlow,const std::vector<ProcessorID>& ProIDList);

protected:
	const FlightSchedule						*m_pFlightSchedule;
	std::vector<FlightItemBoardingCallMgr*>		m_vectFltItemBoardingCallMgr;
};
