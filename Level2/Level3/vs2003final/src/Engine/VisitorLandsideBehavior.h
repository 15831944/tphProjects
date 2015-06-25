#pragma once
#include "landsidebasebehavior.h"

class PaxLandsideBehavior;

class VisitorLandsideBehavior :
	public LandsideBaseBehavior
{
public:

~VisitorLandsideBehavior(void);
	VisitorLandsideBehavior(Person* p);
		
	virtual int performanceMove(ElapsedTime p_time,bool bNoLog);
	
	virtual void InitializeBehavior();
	virtual void flushLog (ElapsedTime p_time, bool bmissflight = false);
	//
	void UpdateEntryTime( const ElapsedTime& eEntryTime );
	//write visitor log himself
	int WriteLogEntry(ElapsedTime time, int ntype,bool _bBackup = false, bool bOffset  = true);

	//Sets Element's destination
	virtual void setDestination (const ARCVector3& p,MobDir emWalk = FORWARD);

protected:
	void writeFollowerLog(ElapsedTime time, bool _bBackup = false, bool bOffset  = true );
	


public:
	//pax write visitor log
	bool WriteVisitorTrack(const ARCVector3& pax_pre_location,PaxLandsideBehavior* pPax,ElapsedTime time, bool _bBackup = false, bool bOffset  = true );
protected:
	//pax write visitor log
	void writeCurrentLogItem(PaxLandsideBehavior* pOwnerPax, Point _OwnerPreLocation, theVisitorLogNum logNum = secondLog,MobDir emWalk = FORWARD);
	void RepeatFollowersLastLogWithNewDest();
	void RepeatLastLogWithNewDest(PaxVisitor* pVisitor);

};
