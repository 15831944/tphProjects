#ifndef VISITOR_H
#define VISITOR_H

#include "engine\person.h"
//#include "arctools\general\containr.h"
#include "engine\ConvergePair.h"
#include "VisitorTerminalBehavior.h"
//Person neither arrival nor departing. abstract class.
class PaxVisitor : public Person
{
	friend class VisitorTerminalBehavior;

protected:

	BOOL m_bOwnerInitialized; //owner flow mobile element won't be initialized when construct 
							  // this is only set when attached.

	int m_nOwnerGroupSize;


	// visit time
	ElapsedTime visitTime;
//	ElapsedTime m_visitTime;	// from Sender, let's not care about this for now.

    // from Greeter, passenger index that a greeter has. will used for all type.
	// base on this information, whey greeter meet passenger this information is use to 
	// find the m_pOwnerPax.
	int m_nOwnerID;


public:
	
    PaxVisitor( MobLogEntry& _entry, CARCportEngine* _pEngine );
    virtual ~PaxVisitor () {};

	
	// in:
	// _bBackup: if the log is backwards.
    virtual void writeLogEntry (ElapsedTime time, bool _bBackup, bool bOffset = true );

	
	// Dumps all tracks to the log and updates ElementDescription record
	// called at death of mobile element
	// must repeat once for each member of group
	void flushLog( ElapsedTime _time, bool bmissflight = false);

	// new person for group follower
	virtual void newGroupFollower( void);
	void DeleteGroupFollower(PaxVisitor* pVisitor);
	void NewConvergePair(long _iPassengerId,bool _ConvergeTag=false);
	void DeleteConvergePair(long _iPaxId,bool _IsMyWantDele); //delete local pair.
	void DeleteGreetPool(long _iFirId,long _iSecoId);
	
    int  GetOwnerId(void) {return m_nOwnerID;}
	Passenger* GetOwner(void);

	void initOwnerPassenger( Passenger* _pPax );
	const ElapsedTime& getVisiteTime()const {return visitTime;}

	void MakeVisitorSyncAvailable(const ElapsedTime& curTime);
	

	//set tray group item that count > 1 destination
	//nRadiu is host radius
	void SetTrayGroupItemAndFollowerDesitination(Point _HostPt,Point _DesitinationPt,int nRadius);
	void writeVisitorLog( ElapsedTime time,Point _OwnerPreLocation, bool _bBackup, bool bOffset = true );
	void InitVisitorAngle();
private:
	enum theVisitorLogNum
	{
		firstLog = 0,
		secondLog,
	};
	void writeCurrentLogItem(Point _OwnerPreLocation, theVisitorLogNum logNum = secondLog);
	void addAngleLogItem(Point _OwnerPreLocation);
};

#endif
