#ifndef BAGGAGE_PROCESSOR_H
#define BAGGAGE_PROCESSOR_H

#include "common\containr.h"
#include "common\template.h"
#include "engine\lineproc.h"
#include "results\bagentry.h"
#include "BaggageProcUtil.h"
#include "common\pollygon.h"
#include "engine\barrier.h"

class ProcessorIDList;
// all baggage's log is write one time. and then this baggage is been removed from reclaim
// while  adding capacity property to reclaim, we need to know the actual bag's leave time.
class BagStillOnReclaim
{
public:
	BagStillOnReclaim(int nID, ElapsedTime sTime, int bagCount)
		:id(nID), time(sTime), nBagCount(bagCount)
	{}
	
	PersonID id;//bag id
	ElapsedTime time;//actual leave time
	int nBagCount;//bag's group item count

};

typedef std::vector< BagStillOnReclaim> BagStillOnRecliamState;
typedef BagStillOnRecliamState::iterator BagStillOnRecliamIter;
typedef BagStillOnRecliamState::const_iterator BagStillOnRecliamConsIter;


class RoutingOption;

typedef long PersonID;

//This class describe features of the processor bag.
class ENGINE_TRANSFER BaggageProcessor : public LineProcessor
{
protected:
    //a pointer to a processor distribution
    ProcessorDistribution *bypassProcessors;

    //a list of bags
    UnsortedContainer<BagLogEntry> bagContainer;    

    //It retrieves the user input capacity of bags for a aircraft
    int capacity;

	//handle checkbas moving logic and handle how pax pickup his bags
	BaggageProcUtil m_bagUtil;


	// baggage's moving speed on baggageproc
	DistanceUnit m_dSpeed;

	// entry point of baggage when it arrvial at baggageprocessor
	Point m_ptBagFirstEntryPos;

	// entry index  of baggage in BaggageProcUtil m_bagUtil when it arrvial at baggageprocessor
	int m_iBagFirstEntryPosIdx;

	// the max time when person wait for his bag or bag wait for his owner
	long m_lMaxWaitTime;

	//the path check baggage moving on
	Path m_pathBagMovingOn;

	// barrier in baggage proc
	Barrier m_barrier;
	
	std::vector<PersonID> m_vOwners;
	std::vector<PersonID> m_vBaggages;

	std::vector<PersonID> m_listApproachBaggage;
	BagStillOnRecliamState m_vectBagStillOnReclaim;

	ProcessorIDList* m_pHostId;

	//if it is baggage processor, save the random index of service point
	//int m_nRandomIndexOfServicePoint;
private:
	std::vector<PersonID> GetMyBaggage( Passenger* _pOwner );
	Person* GetMyOwner( PersonID _nBaggage );

	// generate baggage moving logs on baggage device 
	void GenerateBaggageLogsOnMovingPath( Person* _pOwner, PersonID _nBaggage , bool _bBaggageComeFirst, const ElapsedTime& _curTime);

public:
    BaggageProcessor () ;
    ~BaggageProcessor ();

	const ProcessorIDList* GetHostIdList(void) const ;
	void SetHostIdList(ProcessorIDList* _vProcsID);

	long GetMaxWaitForTime()const { return m_lMaxWaitTime;	}
	//Pollygon* GetInternalPolygon()  { return &m_internalPolygon;	};
	//Pollygon* GetExternalPolygon()  { return &m_externalPolygon;	};
	//Path* GetTrueServicePath() { return &m_trueServicePath;	};

	//virtual Point GetPipeExitPoint( int iCurFloor, CString& _curTime,Point& outPoint,TerminalMobElementBehavior *terminalMob) ;
    //Returns bag processor type.
	virtual Path* GetBarrierPath(){ return GetBarrier()->serviceLocationPath(); }
	virtual Path* GetPathBagMovingOn(){ return &m_pathBagMovingOn;	};
	Barrier* GetBarrier(){ return &m_barrier;	}
    virtual int getProcessorType (void) const { return BaggageProc; };

    //Returns bag processor name.
    virtual const char *getProcessorName (void) const { return "BAGGAGE"; };

	virtual void initServiceLocation (int pathCount, const Point *pointList);

    void initSpecificMisc (const MiscData *miscData);

    void beginService (Person *person, ElapsedTime curTime);

    ProcessorDistribution *getBypassProcessors (void) const { return bypassProcessors; };

	virtual bool isVacant( Person* pPerson  = NULL) const;

	void WakeupWaitingBaggageInPrevProc(  const ElapsedTime& time );

	void addBaggageIntoApproachList(const Person* pBaggage);

	void removeBaggageFromApproachList(const Person* pBaggage);
	void updateBaggageOnReclaimState(const ElapsedTime& curTime);
   

	// get next index and next point in BaggageProcUtil m_bagUtil according to current index
	int GetNextPosOnBaggage( int _iCurrentPosIdx , Point& _ptNextPos ) const ;

	//init BaggageProcUtil m_bagUtil 
	void InitBaggageDeviceData();

	//erase person from current index ,and add person in next index
	void MovePersonToNextPos( int _iCurrentPosIdx, PersonID _nPerson );

	void SetMovingSpeed( DistanceUnit _dSpeed ){ m_dSpeed = _dSpeed;	};
	DistanceUnit GetMovingSpeed() const { return m_dSpeed;	};

	// the owner stand on currentpos to pickup its bags
	void PickupBags( Passenger* _pOwner , int _iCurrentPos, BAGS& _vBags );
	void EraseBagAt( int _iBagPosIdx, PersonID _nBag );


	//void InitInternalPolygon( int _iCount, const Point* _pPointList );

	virtual Point AcquireServiceLocation (Person * _pPerson );

	// Returns service m_location.
	virtual Point GetServiceLocation() const;
	
	// Get Entry Point of the processor,
	// might be inconstraint, or service m_location.
	//virtual Point GetEntryPoint(int _iCurFloor);

	int intersects (const Point& p1, const Point& p2);
	int contains (const Point& pnt);

	virtual void DoOffset( DistanceUnit _dx, DistanceUnit _dy );
	virtual void Rotate( DistanceUnit _degree );
	virtual void Mirror( const Path* _p);

	bool IfPersonStillExsit( long _lPersonToCheck );

	virtual bool CopyOtherData(Processor* _pDestProc);

	virtual Processor* CreateNewProc();

protected:

	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile)const;



	
};

#endif





















