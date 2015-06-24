// Sorter.h: interface for the CSorter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SORTER_H__A7BB5AA4_49D7_46C5_B85C_D376FC2F55FA__INCLUDED_)
#define AFX_SORTER_H__A7BB5AA4_49D7_46C5_B85C_D376FC2F55FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
#include "SimpleConveyor.h"

using namespace std;

class CSorter : public CSimpleConveyor  
{
public:
	CSorter();
	virtual ~CSorter();

	virtual void beginService (Person *aPerson, ElapsedTime curTime);
	void InitEntryExitSlots();
	int GetEntryProcessor(const ProcessorID& _sourceProcsID);
	Processor* GetExitProcessor(int _nIndex);
	virtual bool isVacant( Person* pPerson  = NULL) const;
	virtual void InitData( const MiscConveyorData * _pMiscData );
	virtual void RemoveHeadPerson(const ElapsedTime& time);
	bool IsEntryVacant(int _nEntryProcsIndex);
	virtual void updateApproachingPerson( const ElapsedTime& time );
	bool _ifNeedToNotice( void ) const;
	bool CanAcceptPerson(const ProcessorID& _entryProcsID ,const ElapsedTime& _time )const;
		
private:
	virtual void getDestinationLocation( Person* aPerson );
	virtual ElapsedTime _getReleasePersonTime( void ) const;
	int GetExitProcessor(const Processor* _pExitProcessor) const;
	ElapsedTime m_timeNextStep;
protected:
	// let the conveyor step it
	virtual void stepIt( const ElapsedTime& time );
	virtual void processorStepItEvent( InputTerminal* _pInTerm, const ElapsedTime& time );
	bool IfSlotsVacant(const std::vector<int>& vSlots) const;
	bool IfAllSlotsOccupy() const;
	virtual bool releaseHeadPerson( Person* person, ElapsedTime time  );
	bool releasePersonInExitSlot(ElapsedTime time);
	//bool createStepItEventIfNeed( const ElapsedTime& time );
	void noticeEntryConveyor(const ElapsedTime& time);
	bool IsEntryProcsReleasePersonNow(const ElapsedTime& _time);
protected:
	//preserve the exit Slot's positions in Slots,
	//his elment < m_iCapacity
	std::vector<int> m_vExitSlots;

	//preserve the entry Slot's positions in Slots,
	//his elment < m_iCapacity
	// m_vEntrySlots.size() always equal to Processor::m_vSourceProcs.size()
	std::vector<int>  m_vEntrySlots;

	//processors in this vector that conveyor is proper.
	//m_vLinkedExitProcs.size() always == m_vExitSlots.size()
	std::vector<Processor*> m_vLinkedExitProcs;
	std::vector<int> m_vMobileExitSlots;

};

#endif // !defined(AFX_SORTER_H__A7BB5AA4_49D7_46C5_B85C_D376FC2F55FA__INCLUDED_)
