#pragma once
#include "Common\ARCVector.h"
#include "CorridorGraphMgr.h"
#include "afxtempl.h"

class AirsideFlightInSim;
class COnboardFlight;
class COnBoardAnalysisCandidates;
class Flight;
class CAircaftLayOut;
class OnboardSeatInSim;
class CDoor;
class ElapsedTime;
class Person;
class OnboardDoorInSim;
class OnboardStairInSim;
class OnboardElevatorInSim;
class OnboardEscalatorInSim;
class OnboardCorridorInSim;
class OnboardSeatRowInSim;
class OnboardSeatGroupInSim;
class EntryPointInSim;
class OnboardDeckGroundInSim;
class GroundInSim;
class CSeatGroup;
class OnboardWallInSim;
class OnboardCellGraph;
class SeatAssignmentInSim;
class CFlightPaxCabinAssign;
class CSeatingPreferenceList;
class CFlightSeatTypeDefine;

class CarryonVolumeInSim;
class FlightCarryonPriorityInSim;
class TargetLocationVariableInSim;
class CFlightCarryonStoragePriorities;
class FlightDoorOperationalSpec;
class DoorAssignmentInSim;
class CARCportEngine;
class OnboardDoorLog;
class DoorOperationInSim;
class OnboardFlightSeatBlock;

enum SeatAssignmentType;

class OnboardFlightInSim
{
	enum Enum_FlightOpType
	{
		Enum_FlightOpType_UNKOWN = 0,
		Enum_FlightOpType_ARR,
		Enum_FlightOpType_DEP,



		Enum_FlightOpType_COUNT
	};
public:
	OnboardFlightInSim(Flight *pFlight,bool bArrival, COnboardFlight *pOnboardFlight);
	~OnboardFlightInSim(void);
public:
	void FlightTerminated(const ElapsedTime& eTime );

	bool IsSameFlight(Flight *pFlight,bool bArrival);
	Flight* GetFlightInput()const{return m_pFlight;}

	bool getShortestPath(const ARCVector3& sourcePoint, const ARCVector3& destPoint, CorridorGraphVertexList& _shortestPath);
	bool getShortestPath(const OnboardAircraftElementInSim* pEntryElementInSim,const ARCVector3& entryPos,const OnboardAircraftElementInSim* pExitElementInSim,const ARCVector3& exitPos,CorridorGraphVertexList& _shortestPath);

	COnboardFlight* GetOnboardFlight()const{return m_pOnboardFlight;}

	void InitSeatAssignment(CSeatingPreferenceList* pSeatingPreferenceList, CFlightPaxCabinAssign* pOnboardPaxCabin,CFlightSeatTypeDefine* pOnboardSeat,SeatAssignmentType emType);
	void InitDoorAssignment(FlightDoorOperationalSpec* pDoorOperationSpc,OnboardFlightSeatBlock* pFlightSeatBlock);
	CarryonVolumeInSim * GetCarryonVolume() const;
	void SetCarryonVolume(CarryonVolumeInSim * pVolume);

	FlightCarryonPriorityInSim *GetCarryonPriority() const;
	void SetCarryonPriority(CFlightCarryonStoragePriorities *pPriority);

	TargetLocationVariableInSim * GetTargetLocation() const;
	void SetTargetLocation(TargetLocationVariableInSim * pTargetLocation);

	bool SetOnboardDoorConnect(const ARCVector2& vDir,const CPoint2008& locationPt,const CLine2008& line);

	void GenerateCloseDoorEvent(const ElapsedTime& time);
	bool ProcessCloseDoor(const ElapsedTime& time);
	bool ReadyCloseDoor()const;

	bool IsAllPaxOnBoard();
public:
	OnboardDoorInSim* GetEntryDoor() const;
	OnboardDoorInSim* GetDoor(Person* pPerson,OnboardSeatInSim* pSeatInSim)const;
	OnboardDoorInSim* GetExitDoor() const;
	OnboardSeatInSim* GetSeat(Person* pPerson,CString& strMessage);

	void GetPaxListInFlight(std::vector<Person*>& vPaxList);

	std::vector<EntryPointInSim* >& GetSeatEntryPoint();

	void WakeupPassengerDeplane(const ElapsedTime& tTime);

	bool IsSeatsAvailable(int nSeatCount);
	CAircaftLayOut* GeLayout()const{ return m_pLayout;}

	GroundInSim* GetGroundInSim(CDeck* pDeck);

	std::vector<OnboardSeatInSim *>* GetOnboardSeatList() {return &m_vSeatInSim;}
	std::vector<OnboardDoorInSim *>* GetOnboardDoorList() {return &m_vDoorInSim;}
	std::vector<OnboardElevatorInSim*>* GetOnboardElevatorList() {return &m_vElevatorInSim;}
	std::vector<OnboardStairInSim*>* GetOnboardStairList() {return &m_vStairInSim;}
	std::vector<OnboardEscalatorInSim*>* GetOnboardEscalatorList() {return &m_vEscalatorInSim;}
	std::vector<OnboardCorridorInSim*>* GetOnboardCorridorList() {return &m_vCorridorInSim;}
	std::vector<OnboardSeatRowInSim*>* GetOnboardSeatRowList() {return &m_vSeatRowInSim;}
	std::vector<OnboardSeatGroupInSim*>* GetOnboardSeatGroupList(){return &m_vSeatGroupInSim;}
	std::vector<OnboardWallInSim*>* GetOnboardWallList(){return &m_vWallInSim;}
	std::vector<OnboardStorageInSim*>* GetOnboardStorageList(){return &m_vStorageInSim;}
public:
	CString getFlightID() const;
	OnboardDeckGroundInSim* GetDeckInSim(int iDeck);

	OnboardCellGraph *getCellGraph() const;

	OnboardStorageInSim* GetOnboardStorage(GroundInSim* pGroundInSim,const ARCVector3& pt);

	//dReachDist, reach distance
	std::vector<OnboardStorageInSim*> GetCanReachStorageDevice(GroundInSim* pGroundInSim,const ARCVector3& pt, double dReachDist = 150);


	bool IsInitialized() const{return m_bInitialized;}
	void SetInitialized(bool bInit = true){m_bInitialized = bInit;}

	void SetEngine(CARCportEngine* pEngine){m_pArcportEngine = pEngine;}
	CARCportEngine* GetEngine()const {return m_pArcportEngine;}

	//register the passenger to this onboard flight
	//if the pax already exists in the flight, return false
	bool RegisterPax(int nPaxID);
	//if passenger is not existing on the flight, return false
	bool DeRegisterPax(int nPaxID);


	void SetWaitAirsideFlight(AirsideFlightInSim* pFlightInSim){m_pWaitAirsideFlight = pFlightInSim;}
	void WriteDoorCloseLog(OnboardDoorLog* pLog);
	void GetFitestDoor(const DoorOperationInSim* pDoorOperationInSim,std::vector<OnboardDoorInSim*>& vDoorList)const;
	void ClearPaxFromHisSeat(int nPaxID, ElapsedTime eTime);

protected:
	void Initialize();
	void CalculateCorridorIntersection();
	OnboardSeatRowInSim* GetSeatRowInSim(CSeatRow* pSeatRow)const;
	OnboardSeatGroupInSim* GetSeatGroupInSim(CSeatGroup* pSeatGroup)const;
	void CalculateSeatRowSpace();
	void CalculateSeatGroupSpace();
	void CalculateSeatRowEntrypoint();
	void CalculateStorageDeviceEntrypoin();

	void InitDeckGround();
	OnboardDeckGroundInSim* GetDeckInSim(CDeck* pDeck);
	void setOnboardCellBarrier();

protected:
	Flight *m_pFlight;

	Enum_FlightOpType m_enumOP;

	COnboardFlight *m_pOnboardFlight;
	CAircaftLayOut* m_pLayout;

	std::vector<OnboardSeatInSim *> m_vSeatInSim;
	std::vector<OnboardDoorInSim *> m_vDoorInSim;
	std::vector<OnboardElevatorInSim*> m_vElevatorInSim;
	std::vector<OnboardStairInSim*> m_vStairInSim;
	std::vector<OnboardEscalatorInSim*> m_vEscalatorInSim;
	std::vector<OnboardCorridorInSim*> m_vCorridorInSim;
	std::vector<OnboardSeatRowInSim*> m_vSeatRowInSim;
	std::vector<OnboardSeatGroupInSim*> m_vSeatGroupInSim;
	std::vector<OnboardWallInSim*> m_vWallInSim;
	std::vector<OnboardStorageInSim*> m_vStorageInSim;

	std::vector<EntryPointInSim *> m_vSeatEntryPoint;

	CorridorGraphMgr* m_pCorridorMgr;

	OnboardCellGraph *m_pCellGraph;
	//std::vector<OnboardDeckGroundInSim*> m_vGroundInSimList;

	SeatAssignmentInSim* m_pSeatAssignmentInSim;
	DoorAssignmentInSim* m_pDoorAssignmentInSim;

	CarryonVolumeInSim *m_pCarryonVolume;

	FlightCarryonPriorityInSim *m_pStorageDevicePriority;

	TargetLocationVariableInSim *m_pTargetLocation;

	bool m_bInitialized;

	CARCportEngine*	m_pArcportEngine;

	bool m_bGenerateCloseDoorEvent;//true, generate; false, does not

	//record person and seat relationship
	CMap<Person*,Person*,OnboardSeatInSim*,OnboardSeatInSim*> m_mapObjects;

	AirsideFlightInSim*	m_pWaitAirsideFlight;
	std::vector<int> m_vRegisterPax;
};

class OnboardFlightInSimList
{

public:
	OnboardFlightInSimList(COnBoardAnalysisCandidates* pFlightCandiates);
	~OnboardFlightInSimList();

	void TerminateAll(const ElapsedTime& t );

public:
	OnboardFlightInSim *GetOnboardFlightInSim(AirsideFlightInSim* pFlight, bool bArrival);
	OnboardFlightInSim *GetExistedOnboardFlightInSim(Flight* pFlight, bool bArrival);
protected:
	std::vector<OnboardFlightInSim *> m_vOnboardFlight;

	COnBoardAnalysisCandidates* m_pFlightCandiates;
private:
};
