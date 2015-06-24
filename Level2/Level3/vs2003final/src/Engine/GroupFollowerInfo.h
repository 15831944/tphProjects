// GroupFollowerInfo.h: interface for the CGroupFollowerInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GROUPFOLLOWERINFO_H__771F620C_8F50_458F_8AC3_7356E94DA43A__INCLUDED_)
#define AFX_GROUPFOLLOWERINFO_H__771F620C_8F50_458F_8AC3_7356E94DA43A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GroupInfo.h"
#include <assert.h>
#include "../Common/elaptime.h"

class CGroupFollowerInfo : public CGroupInfo  
{

public:
	CGroupFollowerInfo( int _nGroupSize );
	virtual ~CGroupFollowerInfo();

	virtual bool IsSameGroup(Person* pPerson);

	// set & get group leader id
	void SetGroupLeader( Person* _pPerson ){ m_pGroupLeader = _pPerson; }
//	void getGroupLeaderId( void ) const { return m_lGroupLeaderId; }

	//Get the size of those people who walk together.
	virtual int GetActiveGroupSize() const { return 1; }

	// write log for all the follower if they are active grouped
	// follower need not do anything for that.
	// in:
	// _bBackup: if the log is backwards.
	virtual void WriteLogForFollower (ElapsedTime time, bool _bBackup, bool bOffset = true ){}

	// OUT_GROUP --> JOIN_GROUP ask leader check if all member 
	virtual enum EVENT_OPERATION_DECISTION ProcessEvent( ElapsedTime _eventTime, bool _bDisallowGroupInNextProc );


	//person in group position
	virtual int GetPosition(Person* pPerson);

	virtual bool IsFollower() const { return true; }

	// Set the destination for all the follower, will adjust the corrd.
	virtual void SetFollowerDestination( const Point& _ptCurrent, const Point& _ptDestination, float* _pRotation ){}

	// Repeat every followers' last log item in log entry with new destination set by former code
	// this is useful for PaxVisitor to make a quick shift
	// 
	// NOTE: to use this method correctly, look at void Person::RepeatLastLogWithNewDest().
	virtual void RepeatFollowersLastLogWithNewDest() {}

	// Flush log for the follower.
	virtual void FlushLogforFollower( ElapsedTime _time ) {}

	//leader and follower if walk with owner
	virtual void SetFollowerWithOwner(bool bWithOwner){}
	virtual void SetFollowerInTray(bool bInTray){}

	virtual void setFollowerArrivalDoor(bool bArrivalDoor){}

	//set tray group item that count > 1 destination
	//nRadiu is host radius
	virtual void SetTrayGroupItemFollowerDesitination(Point _HostPt,int nRadius){}
};

#endif // !defined(AFX_GROUPFOLLOWERINFO_H__771F620C_8F50_458F_8AC3_7356E94DA43A__INCLUDED_)
