#pragma once
#include "onboardaircraftelementinsim.h"
#include "../Common/Pollygon2008.h"
#include "PaxCellLocation.h"
#include "../Common/elaptime.h"


class CDoor;
class ARCVector3;
class GroundInSim;
class OnboardFlightInSim;
class OnboardElementSpace;
class EntryPointInSim;
class ElapsedTime;
class DoorOperationalData;
class DoorOperationInSim;
class OnboardCloseDoorEvent;

class OnboardDoorInSim :
	public OnboardAircraftElementInSim
{
public:
	OnboardDoorInSim(CDoor *pDoor,OnboardFlightInSim* pFlightInSim);
	~OnboardDoorInSim(void);
public:

	BOOL GetPosition(ARCVector3& doorPos) const;
	CPoint2008 GetLocation()const;
	virtual CString GetType()const;

	virtual OnboardCellInSim *GetEntryCell() const;

	virtual int GetResourceID() const;

	bool IsAvailable(PaxOnboardBaseBehavior* pPax);

	void AddWaitingPax(PaxOnboardBaseBehavior* pPax, const ElapsedTime& eTime);
	void ReleaseNextPax(const ElapsedTime& eTime);
	void DeletePaxWait(PaxOnboardBaseBehavior *pPax);

	PaxCellLocation getEntryLocation() const;

	virtual void NotifyPaxLeave(OnboardCellInSim *pCell, const ElapsedTime& eTime );
	
	//check the door is clear
	bool  CheckDoorClear(PaxOnboardBaseBehavior *pBehavior, const ElapsedTime& eTime );

	bool ConnectValid();
	void GenerateCloseDoorEvent(const ElapsedTime& time);
	bool ProcessCloseDoor(const ElapsedTime& time);

	void SetOperationData( DoorOperationInSim* pData );
	DoorOperationInSim* GetOperationData()const{return m_pDoorOperation;}

	void SetConnectionPos(const CPoint2008& ptConnect){m_ptConnect = ptConnect;}
	const CPoint2008& GetConnectionPos()const{return m_ptConnect;}

	void SetGroundPos(const CPoint2008& ptGround){m_ptGround = ptGround;}
	const CPoint2008& GetGroundPos()const{return m_ptGround;}

protected:
	virtual void CalculateSpace();
	GroundInSim* GetGround();
	void ClearSpace();
	void CreatePollygon();
	//create entry point
	void CreateEntryPoint();

	PaxCellLocation m_entryLocation;

	std::vector<PaxOnboardBaseBehavior*> m_vPaxWaitting;

	void WriteCloseDoorLog(const ElapsedTime& time);
public:
	CDoor *m_pDoor;//seat object
	OnboardElementSpace* m_pElementSpace;
	EntryPointInSim* m_pEntryPointInSim;

	DoorOperationInSim* m_pDoorOperation;
	CPoint2008	m_ptConnect;
	CPoint2008	m_ptGround;


	ElapsedTime	m_tStartClost;
	ElapsedTime	m_tCloseTime;
	bool m_bClose;
	OnboardCloseDoorEvent* m_pCloseDoorEvent;
};
