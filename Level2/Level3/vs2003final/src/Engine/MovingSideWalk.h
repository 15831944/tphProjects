// MovingSideWalk.h: interface for the MovingSideWalk class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOVINGSIDEWALK_H__0BF430F6_ED66_4D2A_8B32_9266D602E9C0__INCLUDED_)
#define AFX_MOVINGSIDEWALK_H__0BF430F6_ED66_4D2A_8B32_9266D602E9C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PROCESS.H"

#include "common\Pollygon.h"
#include "inputs\TimePointOnSideWalk.h"
#include <CommonData/ExtraProcProp.h>


class Pollygon;
class CTimePoint;

class MovingSideWalk : public Processor, public MovingSideWalkProp
{
protected:
	
public:
	MovingSideWalk();
	virtual ~MovingSideWalk();

public:

	// check if this processor's population is exceed the capacity.
	//virtual bool IsMyQueueExceedCapacity() const;

	// refresh the related data before engine start;
	virtual void RefreshRelatedData( InputTerminal* _pInTerm );

    // Returns ProcessorClassType corresponding to proc type.
    virtual int getProcessorType (void) const { return MovingSideWalkProc; };

	virtual ExtraProcProp* GetExtraProp() { return this; }

    //Returns processor name. It will be override by subclasses.
    //Without special specific, define a processor name value as "Point". 
    virtual const char *getProcessorName (void) const { return "MOVING_SIDE_WALK"; };

    // Initializes service location(s).
    // It must test count according to type of processor.
    virtual void initServiceLocation (int pathCount, const Point *pointList);

    // It is called by Person::processServerArrival
    // The different processor types serve clients in different ways.
    virtual void beginService (Person *aPerson, ElapsedTime curTime);

    // Returns service m_location.
	virtual Point AcquireServiceLocation( Person* _pPerson  );

	//get all region this sidewalk covered
	void GetSideWalkCoveredRegion( POLLYGONVECTOR& _regions ) const;

	// test if this sidewalk is contained by the specific region
	bool IsContainedBy( POLLYGONVECTOR& _regions ) const;

	DistanceUnit GetMovingSpeed()const;
	DistanceUnit GetWidth()const;
	void CaculatePointTime( PTONSIDEWALK& _vResult, int nPipeIndex,int _iStartIdx, int _iEndIdx ,DistanceUnit _dStartDistance,DistanceUnit _dEndDistance  )const;

	virtual Processor* CreateNewProc();

private:
	
	void GetSiglePollygon( Point& _first,Point& _second, DistanceUnit _width, Pollygon& _resultPllygon) const;
	
	

/*

    // Allocates and initializes fixed or non-fixed queue.
    virtual void initQueue (int fixed, int pathCount, const Point *pointList);

    virtual void initMiscData ( bool _bDisallowGroup, int visitors, int count, const GroupIndex *gates );

    virtual void initSpecificMisc (const MiscData *miscData) {};

    virtual int readMiscInfo (ArctermFile& dataFile) { return TRUE; }

    // Sets processor index and relative event index.
    virtual void setIndex (int ndx) { index = ndx; logEntry.setIndex (ndx); }

    // Simple accessor for coordinate.
    virtual Point getServicePoint (int ndx) const { return location.getPoint(ndx); }

    // Returns count of service location points.
    virtual int serviceLocationLength (void) const { return location.getCount(); }

	//Returns service location path
	virtual Path* serviceLocationPath(void) { return &location; }
	


    // Returns processor's TransitionRow.
    virtual ProcessorDistribution *getNextDestinations(const PassengerConstraint &p_type);

    // Adds Person to the processor's occupants list.
    virtual void addPerson (Person *aPerson);

    // Removes Person from the processor's occupants list.
	virtual void removePerson (const Person *aPerson);

    // It is called by Person::processServerArrival
    // The different processor types serve clients in different ways.
    virtual void beginService (Person *aPerson, ElapsedTime curTime);
 
    // It resets the server to allow the processor to process the next client and releases the next Person waiting in its queue.   
    virtual void makeAvailable (const Person *aPerson, ElapsedTime availTime, bool _bNoDelay );

    //It updates Person's state based on current state & processor data
    virtual void getNextState (int& state) const;

    //It sets Person's next destination based on state & processor data
    virtual void getNextLocation (Person *aPerson);

    // Returns TRUE if the processor can serve a client (implicating open).
    virtual int isAvailable (void) const { return (isOpen || isClosing) && available; }

    // Returns TRUE if the processor cannot serve a client.
    virtual int closing (void) const { return isClosing; }

    virtual void balanceQueues (ElapsedTime curTime);

    // Kills all passengers stranded in queue at end of simulation.
    // Returns a count of the stranded passengers.
    virtual int clearQueue (ElapsedTime curTime);

    //It updates queue to reflect Person leaving head.
    virtual void leaveQueue (const Person *aPerson, ElapsedTime p_time, bool _bNoDelay );

    //It starts p_person moving and relays advance event to next pax in line.
    virtual void moveThroughQueue (Person *p_person, ElapsedTime p_time);

    // It tests Person's position and Processor's state to see if Person can advance to service location and begin service.
    virtual void processPerson (Person *aPerson, ElapsedTime p_time, bool _bNoDelay ) const;

    //Only and always returns true.
    virtual int writeExtraFields (ArctermFile& miscDataFile) const{ return TRUE; }

    virtual void closeStation (ElapsedTime p_time);

	// do offset for all the position related data. be careful with the derived class.
	virtual void DoOffset( DistanceUnit _xOffset, DistanceUnit _yOffset );

	virtual void Rotate( DistanceUnit _degree );
	
	virtual int ReadProcessor(ArctermFile& procFile);

	// Clean all sim engine related data, for the new sim process
	virtual void CleanData();

*/
	public:
		virtual void UpdateMinMax();

};

#endif // !defined(AFX_MOVINGSIDEWALK_H__0BF430F6_ED66_4D2A_8B32_9266D602E9C0__INCLUDED_)
