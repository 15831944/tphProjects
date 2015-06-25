// GroupInfo.h: interface for the CGroupInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GROUPINFO_H__2D54DE5A_0C88_4ADD_924F_B70CDF26F6E9__INCLUDED_)
#define AFX_GROUPINFO_H__2D54DE5A_0C88_4ADD_924F_B70CDF26F6E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\point.h"

enum EVENT_OPERATION_DECISTION { CONTINURE_GENERATE_EVENT, NOT_GENERATE_EVENT, REMOVE_FROM_PROCESSOR_AND_NO_EVENT };

class CGroupInfo  
{
protected:
	// the current size of this group
	int m_nGroupSize;

	// when group member waiting for regrouping.
	bool m_bWaitingForRegrouping;

	// need for both leader info ( generate event for follower ) and follower.
	Person* m_pGroupLeader;

	Processor *m_pNextProc;
public:
	void SetNextProc(Processor *pNextProc);
public:
	
	virtual bool IsSameGroup(Person* pPerson) = 0;

	CGroupInfo( int _nGroupSize );
	
	virtual ~CGroupInfo(); 

	void SetGroupLeader( Person* _pPerson ){ m_pGroupLeader = _pPerson; }

	Person* GetGroupLeader(){ return m_pGroupLeader; }
	
    // Set & Get the group size
	virtual int GetAdminGroupSize() const { return m_nGroupSize; }

	// Set the group size
	void SetAdminGroupSize( int _size ) { m_nGroupSize = _size; }

	// set & get group leader id
	virtual void setGroupLeaderId( long _lId ) {}
	virtual long getGroupLeaderId( void ) const { return -1; }

	// Set the destination for all the follower, will adjust the corrd.
	virtual void SetFollowerDestination( const Point& _ptCurrent, const Point& _ptDestination, float* _pRotation ) = 0;

	// Repeat every followers' last log item in log entry with new destination set by former code
	// this is useful for PaxVisitor to make a quick shift
	// 
	// NOTE: to use this method correctly, look at void Person::RepeatLastLogWithNewDest().
	virtual void RepeatFollowersLastLogWithNewDest() = 0;

	//person in group position
	virtual int GetPosition(Person* pPerson) = 0;
	// write log for all the follower if they are active grouped
	// in:
	// _bBackup: if the log is backwards.
    virtual void WriteLogForFollower (ElapsedTime time, bool _bBackup, bool bOffset = true ) = 0;

	// When disassemble the group, leader will generate event for the follower.
	// When reassembble the group, leader will setback the disassemble flag
	virtual enum EVENT_OPERATION_DECISTION ProcessEvent( ElapsedTime eventTime, bool _bDisallowGroupInNextProc ) = 0;

	//Get the size of those people who walk together.
	virtual int GetActiveGroupSize() const = 0;

	virtual bool IsFollower() const = 0;

	// Flush log for the follower.
	virtual void FlushLogforFollower( ElapsedTime _time ) = 0;

	
	bool IsWaitingForRegrouping(){ return m_bWaitingForRegrouping; }

	//Group leader and follower is walk with owner 
	virtual void SetFollowerWithOwner(bool bWithOwner) = 0;
	virtual void SetFollowerInTray(bool bInTray) = 0;

	//Group leader and follower arrival door
	virtual void setFollowerArrivalDoor(bool bArrivalDoor) = 0;
	virtual void setEntryTime(const ElapsedTime& tEntryTime) = 0;

	//set tray group item that count > 1 destination
	//nRadiu is host radius
	virtual void SetTrayGroupItemFollowerDesitination(Point _HostPt,int nRadius) = 0;
};

#endif // !defined(AFX_GROUPINFO_H__2D54DE5A_0C88_4ADD_924F_B70CDF26F6E9__INCLUDED_)
