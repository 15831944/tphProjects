#pragma once
#include "OnboardAircraftElementInSim.h"
#include "../Common/Pollygon2008.h"
class EntryPointInSim;
class CSeatRow;
class OnboardSeatInSim;
class GroundInSim;
class OnboardElementSpace;
class OnboardSeatGroupInSim;

class OnboardSeatRowInSim : public OnboardAircraftElementInSim
{
public:
	OnboardSeatRowInSim(CSeatRow* pSeatRow,OnboardFlightInSim* pFlightInSim);
	~OnboardSeatRowInSim();

public:
	EntryPointInSim* GetEntryPoint(const OnboardSeatInSim* pSeatInSim)const;
	CSeatRow* GetSeatRowInput()const;

	void AddSeatInSim(OnboardSeatInSim* pSeatInSim);
	int GetCount()const;
	OnboardSeatInSim* GetSeatInSim(int idx)const;
	OnboardSeatInSim* front()const;
	OnboardSeatInSim* back()const;
	void SetSeatGroupInSim(OnboardSeatGroupInSim* pSeatGroupInSim);
	OnboardSeatGroupInSim* GetSeatGroupInSim()const;


	void AddEntryPoint(EntryPointInSim* pEntryInSim);
	bool empty()const;
	CString GetType()const {return _T("Seat Row");}

	std::vector<OnboardSeatInSim *> GetSeatNeedGivePlace(OnboardSeatInSim* pSeat, EntryPointInSim *pEntryPoint);
	
	//get the entry sequence of seat, the first one has the highest priority
	std::vector<OnboardSeatInSim *> GetSequenceOfSeat(EntryPointInSim *pEntryPoint);
	virtual void CalculateSpace();

	void wakeupPax(const ElapsedTime& p_time,EntryPointInSim *pEntryPoint);
	bool GetLocation(CPoint2008& location)const;
	void CreateEntryPoint();

	bool CanLeadToEntryPoint(OnboardSeatInSim* pSeat,EntryPointInSim *pEntryPoint);

	virtual int GetResourceID() const;
protected:
	void CreatePollygon();
	void ClearSpace();
 	GroundInSim* GetGround();

	//front seat row
	void CreatFrontRowEntryPoint();
	//other seat row
	void CreateOtherRowEntryPoint();

	void CreateEntryPointWithoutSeatGroup();
	void CreateEntryPointWithSeatGroup();
protected:
	std::vector<EntryPointInSim*> m_vEntryPoint;
	CSeatRow* m_pSeatRow;
	std::vector<OnboardSeatInSim*> m_vSeatInSim;
	OnboardElementSpace* m_pElementSpace;
	OnboardSeatGroupInSim* m_pSeatGroupInSim;
};