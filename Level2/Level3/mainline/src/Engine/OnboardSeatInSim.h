#pragma once
#include "OnboardAircraftElementInSim.h"
#include "../Common/Pollygon2008.h"
#include "../InputOnboard/SeatTypeSpecification.h"

class CSeat;
class ARCVector3;
class Person;
class PaxVisitor;
class EntryPointInSim;
class OnboardSeatRowInSim;
class OnboardSeatGroupInSim;
class GroundInSim;
class OnboardElementSpace;
class OnboardDoorInSim;

class OnboardSeatInSim : public OnboardAircraftElementInSim
{
public:
	OnboardSeatInSim(CSeat *pSeat,OnboardFlightInSim* pFlightInSim);
	~OnboardSeatInSim(void);
public:
	virtual CString GetType()const;

	virtual int GetResourceID() const;

	BOOL IsAssigned() const;
	
	//check the pax has been seated or not 
	//if yes, return TRUE
	//else return FALSE
	BOOL IsPaxSit();
	
	BOOL AssignToPerson(Person *pPerson);

	BOOL GetPosition(ARCVector3& seatPos) const;

	BOOL GetFrontPosition(ARCVector3& entryPos)const;
	OnboardCellInSim* GetOnboardCellInSim();

	Person* GetSittingPerson();//
	void PersonLeave();

	Person* GetAsignedPerson()const{ return m_pAssigndPerson; }

	EntryPointInSim* GetEntryPoint()const;
	virtual OnboardCellInSim *GetEntryCell() const;
	void SetEntryPoint(EntryPointInSim* pEntryPointInSim){m_pEntryPointInSim = pEntryPointInSim;}

	void SetSeatRowInSim(OnboardSeatRowInSim* pSeatRowInSim){m_pSeatRowInSim = pSeatRowInSim;}
	void SetSeatGroupInSim(OnboardSeatGroupInSim* pSeatGroupInSim){m_pSeatGroupInSim = pSeatGroupInSim;}

	OnboardSeatRowInSim* GetSeatRowInSim() {return m_pSeatRowInSim;}
	OnboardSeatGroupInSim* GetSeatGroupInSime()const {return m_pSeatGroupInSim;}

	bool GetLocation(CPoint2008& location)const;
	
	EntryPointInSim* GetEntryPointWithSeatRowAndSeatGroup();
	//seat with group entry point cell
	EntryPointInSim* GetIntersectionNodeWithSeatGroupPolygon();
	EntryPointInSim* GetfrontRowEntryPoint(const CPoint2008& dir);


 	GroundInSim* GetGround();

	double GetRotation()const;

	ARCVector3 GetRandomPoint()const;
	//visitor flush log
	void visitorFlushLog(const ElapsedTime& eTime);
	void visitorWriteLog(const ElapsedTime& eTime);
	void addVisitor(PaxVisitor* pVisitor);
	void clearVisitor();

	void AddSeatType(OnboardSeatType emType);
	bool fitSeatType(OnboardSeatType emType)const;

	double GetAvailableSpace() const;
	BOOL ClearPaxFromSeat();


protected:
	virtual void CalculateSpace();
	void ClearSpace();


 	void CreatePollygon();

	//create entry point
	void CreateEntryPoint();
	EntryPointInSim* FindEntryPoint();

public:
	CSeat *m_pSeat;//seat object

	//person that assign to seat
	//leave seat entry point will set null
	Person *m_pPerson;

protected:
	mutable EntryPointInSim* m_pEntryPointInSim;
	OnboardSeatRowInSim* m_pSeatRowInSim;
	OnboardSeatGroupInSim* m_pSeatGroupInSim;
	Person * m_pAssigndPerson;
	OnboardElementSpace* m_pElementSpace;

	std::vector<PaxVisitor*> m_pVisitorList;

	std::vector<OnboardSeatType> m_vSeatType;

	double m_dCarryonLoad;
};

