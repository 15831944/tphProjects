#pragma once
class CARCportEngine;
class OnboardFlightInSimList;
class InputOnboard;
class Flight;
class OnboardFlightInSim;
class ElapsedTime;
class AirsideFlightInSim;
class COnboardPaxCabinAssign;
class CSeatingPreferenceList;
class COnboardSeatTypeDefine;
class CSeatingAssignmentCabinList;
class CarryonVolumeInSim;
class TargetLocationVariableInSim;
class CarryonStoragePrioritiesInSim;
class DoorOperationalSpecification;
class OnboardFlightSeatBlockContainr;
class ProjectCommon;

class OnboardSimulation
{
public:
	OnboardSimulation(CARCportEngine *_pEngine,InputOnboard *pInputOnboard);
	~OnboardSimulation(void);

public:
	void Initialize();

	//return onboard flight
	//fltCons,flight's ocnstraint
	//nFlightIndex, 
	//if has no, return NULL
	OnboardFlightInSim *GetOnboardFlightInSim(AirsideFlightInSim* pFlight, bool bArrival);
	bool ExistedOnboardFlightInSim(AirsideFlightInSim* pFlight,bool bArrival = true);

	void FlightTerminated(Flight* pFlight, bool bArrival,const ElapsedTime& eTime);
	

	void InitializeFlight(OnboardFlightInSim* pOnboardFlightInSim);


	//flush passenger logs
	void clearPerson(const ElapsedTime& t); 
protected:
	void Clear();


protected:
	CARCportEngine *m_pEngine;
	InputOnboard *m_pInputOnboard;
	OnboardFlightInSimList* m_lstOnboardFlightInSim;

	COnboardPaxCabinAssign* m_pOnboardCabinAssign;//check passenger constraint fit
	CSeatingPreferenceList*	m_pSeatPreferenceList;	//user define data for preference
	COnboardSeatTypeDefine* m_pOnboardSeatType;//user define data for seat group property
	CSeatingAssignmentCabinList* m_pSeatAssignmentStrategy;	


	CarryonVolumeInSim *m_pCarryonVolume;
	TargetLocationVariableInSim *m_pTagetLocation;

	CarryonStoragePrioritiesInSim *m_pCarryonPriority;

	DoorOperationalSpecification* m_pArrDoorOperationSpc;
	DoorOperationalSpecification* m_pDepDoorOperationSpc;

	OnboardFlightSeatBlockContainr* m_pFlightSeatBlockManager;
	ProjectCommon*		m_pProjectCom;
};
