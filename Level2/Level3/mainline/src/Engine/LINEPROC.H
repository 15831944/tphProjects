#ifndef LINE_PROCESSOR_H
#define LINE_PROCESSOR_H

#include "common\containr.h"
#include "engine\process.h"

class LineProcessor : public Processor
{
protected:
    //occupancy flag
	std::vector<int> m_vOccupiedID;		// -1 = non occupied.

    //The records of service points which compose a path.
    Path m_serviceCoords;


protected:
    void openLineProc (ElapsedTime p_time);

	// give a person id, found index of service location, return -1 for not found.
	int GetIndexOfServiceLocation( int _nIdPerson ) const;


public:
    LineProcessor () {}
    LineProcessor (const LineProcessor& aProc) { *this = aProc; }
	virtual ~LineProcessor (){};

	//virtual Point GetPipeExitPoint( int iCurFloor, CString& _curTime,Point& outPoint,TerminalMobElementBehavior *terminalMob) ;
    //Returns processor type.
    virtual int getProcessorType (void) const { return LineProc; }

    //Returns processor name.
    virtual const char *getProcessorName (void) const { return "LINE"; }

    void initServiceLocation (int pathCount, const Point *pointList);

    void initQueue (int fixed, int pathCount, const Point *pointList);

    //It randomly returns the index of an open service point.
    int getRandomPoint (void) const;

    // Returns service m_location.
    virtual Point GetServiceLocation() const;

	// Acquire service m_location.
	virtual Point AcquireServiceLocation( Person* _pPerson  );

//	Point getNextServiceLocation(Person *){
//		return m_serviceCoords.getPoint (random(servicePoints));
//	}

    virtual void balanceQueues (ElapsedTime curTime) {};

    virtual void makeAvailable (const Person *aPerson, ElapsedTime availTime, bool _bNoDelay);

    virtual void processPerson (Person *aPerson, ElapsedTime arriveTime, bool _bNoDelay);

    //Returns TRUE if the processor can serve a client.
    int isAvailable (void) const;

	// do offset for all the position related data. be careful with the derived class.
	virtual void DoOffset( DistanceUnit _xOffset, DistanceUnit _yOffset );
	virtual void Rotate( DistanceUnit _degree );

	// release person who wait in area because of the roster reason in Person::SelectProc
	void releaseWaitInAreaPax( const ElapsedTime& releaseTime , Processor* _pBackToProc );

	
	virtual void beginService (Person *aPerson, ElapsedTime curTime);
	
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;

	virtual Processor* CreateNewProc();


	virtual void UpdateMinMax();



};

#endif
