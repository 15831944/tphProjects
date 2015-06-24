#ifndef PERSON_H
#define PERSON_H

#include "common\fsstream.h"
#include "common\template.h"
#include "common\term_bin.h"
#include "common\states.h"
#include "inputs\pax_db.h"
#include "engine\mobile.h"
#include "engine\proclist.h"
#include "results\paxentry.h"
#include "inputs\TimePointOnSideWalk.h"
#include "PersonOnStation.h"
#include "inputs\SinglePaxTypeFlow.h"
#include "engine\FlowChannelList.h"
#include "engine\Convergepair.h"
#include "engine\GroupInfo.h"

#include "ProcessorOccupantsState.h"

#include <Inputs/FlowItemEx.h>
#include <vector>
#include <map>

#define MAX_TRACK_COUNT 3500
#define MAX_GROUP       7
#define GROUP_OFFSET	30.0

class ConvergePair;
class AirsideFlightInSim;
class CAirsidePaxBusInSim;
class BridgeConnector;

class CSimFlowSync;
class CSubFlow;
class CPaxDestDiagnoseInfo;
class CARCportEngine;
class TerminalMobElementBehavior;

enum { NORMAL_MODE, FROM_GATE_MODE, TO_GATE_MODE };
enum{ NonChannel = 0,ChannelStart =1, ChannelEnd =2 };
enum ArrDepBridgeState {NonState,ArrBridge,DepBridge};
class Person : public MobileElement
{
	friend class TerminalMobElementBehavior;
	friend class TurnaroundPaxTerminalBehavior;

protected:
    // Mobile Element tracking log.
    MobLogEntry m_logEntry;

    // Contains carrier, flight ID, and passenger classification.
    CMobileElemConstraint m_type;

    //Index of gate used by Person.
    int m_nGate;

    //Index of bag carosel used by Person.
    int m_nBagCarosel;             

	Terminal* m_pTerm;
	CARCportEngine *m_pEngine;

	std::vector<Person*> m_vBags;// all my bags

	int m_ilightIndex;

	//The positional deviation data;
	// if for sideStep, it stores distribution as value always random
	// if for inStep, a Person's value is generally constant
	const ProbabilityDistribution* m_pSideStep;

	DistanceUnit m_in;

	DistanceUnit m_spacing;

	  

public:
	// Group infomation.
	CGroupInfo* m_pGroupInfo;

	//For group moving.
	static float * far m_pRotation;

	std::vector<ConvergePair *> m_vpConvergePair;
public:
	CARCportEngine *getEngine();
	const CARCportEngine *getEngine() const;
	
	// check if is passenger for visitor person.
	bool IsHuman();

    MobLogEntry& getLogEntry( void ) { return m_logEntry;}

	// Used to determine if the gate is used as arrival purpose.
	virtual bool CheckAndTurnArrState();

    static void initSpacing (void);
	int GetCurFlightIndex();

	void SetTypeInputTeminal(Terminal* pTerm) {m_type.SetInputTerminal((InputTerminal*)pTerm);}

    Person (MobLogEntry & p_entry, CARCportEngine* _pEngine );
    virtual ~Person ();


	//void SetTerminal( Terminal* _pTerm ){ m_pTerm = _pTerm; }
	const Terminal* GetTerminal() const;
	Terminal* GetTerminal();
    CMobileElemConstraint initType (int p_flight, char p_mode);

	int processTerminalModeMovement(ElapsedTime p_time,bool bNoLog);

    virtual int move (ElapsedTime p_time,bool bNoLog);

    //The "interruption" method that removes any scheduled event and writes the Person's current location to the log.
    //This function is used for direction changes.
    void setCurrentPosition (ElapsedTime moveTime);

    //Returns reequired gap between Persons m_in queues.
   virtual DistanceUnit getInStep (void) const;

    //Returns a random sideways movement deviation.
    DistanceUnit getSideStep (void) const;

    //Returns person "type".
    virtual const CMobileElemConstraint &getType (void) const { return m_type; }
   
	virtual const CMobileElemConstraint &getArrivalType(void) const {return m_type;}
    
    //Gets the group size associated with a person.
    virtual int GetAdminGroupSize () const { return m_pGroupInfo->GetAdminGroupSize(); }
	void SetAdminGroupSize( int _size ) { m_pGroupInfo->SetAdminGroupSize( _size ); }

	//Get the size of those people who walk together.
	int GetActiveGroupSize() const { return m_pGroupInfo->GetActiveGroupSize(); }

	// Get the size of those people who walk together, include visitor.
	virtual int GetActiveTotalSize() const { return GetActiveGroupSize(); }

    //Returns gate index.
    int getGateIndex (void) const { return m_nGate; }

	int getBagCarosel (void) const { return m_nBagCarosel; }

    //Method.
    Point offsetCoords (Point p1, Point p2, float dir, DistanceUnit dist);

    //It offsets the points m_in a list of tracks by a direction and distance.
    void adjustCoordinates (MobEventStruct *trackList, int count, float direction, DistanceUnit distance);
	void adjustCoordinates (std::vector<MobEventStruct>& trackList, int count, float direction, DistanceUnit distance );
	void adjustCoordinates (std::vector<MobEventStruct>& trackList, int _beginPos, int _endPos, float direction, DistanceUnit distance );

	// Repeat last log item in the m_logEntry with new destination set by former code
	// this is useful for PaxVisitor to make a quick shift
	// 
	// NOTE: to use this method correctly, look at the implementation.
	void RepeatLastLogWithNewDest();

	//It writes next track to ElementTrack log.
	// in:
	// _bBackup: if the log is backwards.
    virtual void writeLog (ElapsedTime time, bool _bBackup, bool bOffset = true );

    // Dumps all tracks to the log and updates ElementDescription record.
    // It is called at death of passenger.
    // It must repeat once for each member of group
    virtual void flushLog (ElapsedTime p_time, bool bmissflight = false) = 0;

	virtual void regetAttatchedNopax()  { m_type.getNonpaxCount().init(); 	}
	virtual CString getPersonErrorMsg( void ) const;

	//Generates the next movement event for the receiver based on its current state and adds it to the event list.
	// when m_bEvacuationnWhenInPipe is true, shall not generate any event.
    void generateEvent (ElapsedTime eventTime,bool bNoLog );
	
	// new person for group follower
	virtual void newGroupFollower( void) { /*void*/ };

		
// 	// Sets Element's destination
// 	virtual void setDestination( Point p );


	// copy pax event log
	void copyPassengerEventLog( long _lBeginIdx, long _lEndIdx, const MobLogEntry& _srcLogEntry );
	
	// flush group follower's log
	virtual void flushGroupFollowerLog( const ElapsedTime& time, float dir, int _iGroupIndex );

	// generate avoid density event
	void generateAvoidDensityEvent( const AreaIntersectPoint& _intersectPoint, int _iRealState, const Point& _ptRealDest ,bool bNoLog);

	virtual void SetFollowerArrivalDoor(bool bArrivalDoor);

/////mobile element position interface
	//Returns current coordinates
	bool getAirsidePoint (CPoint2008& pt) const;
	bool getTerminalPoint(Point& pt)const;
	bool getOnboardPoint(ARCVector3& pt)const;

	//Returns current destination.
	bool getAirsideDest (CPoint2008& pt) const;
	bool getTerminalDest(Point& pt) const;
	bool getOnboardDest(ARCVector3& pt)const;
	// get pure location
	bool getAirsidePureLocation(CPoint2008& pt)const;
	bool getTerminalPureLocation(Point& pt)const;

	ElapsedTime moveTime (void) const;
	ElapsedTime moveTime( DistanceUnit _dExtraSpeed, bool _bExtra ) const;

	//Sets Element's destination
	virtual void setAirsideDestination (const CPoint2008& p);
	virtual void setTerminalDestination(const Point& p);

	virtual void setTerminalLocation( const Point& _ptLocation );
	virtual void setAirsideLocation( const CPoint2008& _ptLocation );
//write entry terminal and exit terminal log
	virtual void writeTerminalBeginLogEntry(ElapsedTime curTime);
	virtual void writeTerminalEndLogEntry(ElapsedTime curTime);
	//Sets current event type.
	virtual void setState (short newState);
protected:
	//// base on what the next processor, creat the log for the passenger.
	//void ProcessPipe( Processor* _pNextProc, ElapsedTime& _curTime, CFlowItemEx* _pPrevFlowItem );

	// Give a start point, end point, find the pipe list should flow.
	std::vector<CFlowItemEx> AutomaicGetPipe( Point _ptStart, Point _ptEnd );
};

#endif
