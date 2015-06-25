// GroupLeaderInfo.h: interface for the CGroupLeaderInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GROUPLEADERINFO_H__C75987B6_85B3_4928_9826_4B0E8A89B2B4__INCLUDED_)
#define AFX_GROUPLEADERINFO_H__C75987B6_85B3_4928_9826_4B0E8A89B2B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "GroupInfo.h"
#include "mobile.h"

class CGroupLeaderInfo : public CGroupInfo  
{
protected:
	// the id list of the group follower
	std::vector<long> m_vGroupFollowerId;
	
	// now is attatching with leader person*
	MobileElementList   m_pGroupFollowerList;

	// true = the followers walk together
	bool m_bInGroup;

public:
	virtual bool IsSameGroup(Person* pPerson);

	CGroupLeaderInfo( int _nGroupSize, Person* _pGroupLeader );

	virtual ~CGroupLeaderInfo();


	//check follower if in tray host
	PaxVisitor* GetFollowerInTrayHost();
	//Remove person from group
	void RemoveFollower(Person* _pPerson);
	// Add person in the group.
	void AddFollower( Person* _pPerson, Person* _pGroupLeader );

	// Set the destination for all the follower, will adjust the corrd.
	virtual void SetFollowerDestination( const Point& _ptCurrent, const Point& _ptDestination, float* _pRotation );

	// Repeat every followers' last log item in log entry with new destination set by former code
	// this is useful for PaxVisitor to make a quick shift
	// 
	// NOTE: to use this method correctly, look at void Person::RepeatLastLogWithNewDest().
	virtual void RepeatFollowersLastLogWithNewDest();

	virtual void SetFollowerWithOwner(bool bWithOwner);
	virtual void SetFollowerInTray(bool bInTray);

	virtual void setFollowerArrivalDoor(bool bArrivalDoor);
	virtual void setEntryTime(const ElapsedTime& tEntryTime);
	// write log for all the follower if they are active grouped
	// in:
	// _bBackup: if the log is backwards.
    virtual void WriteLogForFollower (ElapsedTime time, bool _bBackup, bool bOffset = true );

	// IN_GROUP --> OUT_GROUP: Generate event for all the followers
	// OUT_GROUP --> JOIN_GROUP: Check if all member ready
	virtual enum EVENT_OPERATION_DECISTION ProcessEvent( ElapsedTime eventTime, bool _bDisallowGroupInNextProc );

	// check if all members are ready.
	bool AllReadytoRegroup();

	//Get the size of those people who walk together.
	virtual int GetActiveGroupSize() const;

	virtual bool IsFollower() const { return false; }

	// Flush log for the follower.
	virtual void FlushLogforFollower( ElapsedTime _time );
	
	void ProcessLoaderProc(ElapsedTime eventTime,Processor *pLoaderProc ,Processor* pHoldAreaProc );

	int GetGroupSize();
	//person in group position
	virtual int GetPosition(Person* pPerson);

	//search follower position
	int SearchPostion(Person* pFollower);

	void SetInGroup(bool group) {m_bInGroup = group;}
	bool isInGroup(){	return m_bInGroup; }

	MobileElementList& GetFollowerList(){	return m_pGroupFollowerList; }

	//set tray group item that count > 1 destination
	//nRadiu is host radius
	virtual void SetTrayGroupItemFollowerDesitination(Point _HostPt,int nRadius);
};

#endif // !defined(AFX_GROUPLEADERINFO_H__C75987B6_85B3_4928_9826_4B0E8A89B2B4__INCLUDED_)
