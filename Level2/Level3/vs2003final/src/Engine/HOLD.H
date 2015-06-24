#ifndef HOLDING_AREA_H
#define HOLDING_AREA_H

#include "common\template.h"
#include "common\containr.h"
#include "engine\process.h"
#include "inputs\pax_cnst.h"
#include "inputs\MultiMobConstraint.h"
#include <list>
#include <utility>
typedef std::pair< Person*, ProcessorID> PersonDestidPair;
class CAirsidePaxBusInSim ;
class CorralServiceEvent;

class ENGINE_TRANSFER HoldingArea : public Processor
{
protected:

    double m_dArea;		// Area of the polygon
    DistanceUnit radius;

	//
	int m_iStageID;
	bool m_bApplyServiceTimeAfterGreeting;

	//flag to mark service after full feature
	bool m_bCorralService;
	//the number of person serviced per time
	//seconds
	//int m_nTimeToService;
	ElapsedTime m_eTimeToService;
	
	//ElapsedTime m_eLastPaxEntryTime;

	//bool		m_bCannotHoldNextPax;
	CorralServiceEvent *m_pCorralServiceEvent;

    

	//
	std::list< PersonDestidPair > m_waitForLoaderList;
    //current servering bus 
	std::vector<CAirsidePaxBusInSim*> m_serverBus ;

public:
	BOOL IsCanTakeOnFlight(Passenger* _pax) ;
	void RegisterServerBus(CAirsidePaxBusInSim* _bus) ;
	void LogoutServerBus(CAirsidePaxBusInSim* _bus) ;
public:
    HoldingArea ();;

    //copy constructor.
    HoldingArea (HoldingArea& aProc) { *this = aProc; };
	//virtual Point GetPipeExitPoint( int iCurFloor, CString& _curTime,Point& outPoint,TerminalMobElementBehavior *terminalMob) ;
    //Returns processor type.
    virtual int getProcessorType (void) const { return HoldAreaProc; };

    //Returns processor name.
    virtual const char *getProcessorName (void) const { return "HOLD_AREA"; };

    void initSpecificMisc (const MiscData *miscData);

    virtual void initQueue (int fixed, int pathCount, const Point *pointList);
    virtual void initServiceLocation (int pathCount, const Point *pointList);

    // Returns service m_location.
    virtual Point GetServiceLocation() const;

    void beginService (Person *person, ElapsedTime curTime);

    virtual void makeAvailable (const Person *aPerson, ElapsedTime availTime, bool _bNoDelay );

    int getPaxOfType (const CMobileElemConstraint& aConst) const;

    int releasePaxToAirside(const CMobileElemConstraint& aConst ,int cout ,ElapsedTime time) ;

    void releasePax (const CMultiMobConstraint *multiConst, float proportion, ElapsedTime releaseTime);

    void releasePax (const CMobileElemConstraint& aConst, float proportion, ElapsedTime releaseTime);

    void releasePax (const CMobileElemConstraint& aConst, int count,  ElapsedTime releaseTime);

    void releasePax (const CMobileElemConstraint& aConst, ElapsedTime releaseTime);

    int wipe (ElapsedTime currentTime);

    DistanceUnit getRadius (void) const { return radius; };

    void setRadius (DistanceUnit rad) { radius = rad; };

    double getDensity (void);

	int getStageID()const { return m_iStageID;	}

	bool getApplyServiceTimeAfterGreeting(){ return m_bApplyServiceTimeAfterGreeting; }

	// release person who wait in area because of the roster reason in Person::SelectProc
	void releaseWaitInAreaPax( const ElapsedTime& releaseTime , Processor* _pBackToProc );
	
	// release bag to loader
	bool TryToReleaseBagToLoader( const ElapsedTime& releaseTime, const ProcessorID& _backToProcID );

	void storeBagtoHolding( Person* _person, const ProcessorID& _destProcID );

	// Clean all sim engine related data, for the new sim process
	virtual void CleanData();

	virtual Processor* CreateNewProc();

private:
	bool PersonShouldWaitOnStage( Person *_pPerson , const ElapsedTime& _currentTime )const;	
	BOOL HasBusServer(int FlightID) ;
public:
	CAirsidePaxBusInSim* GetCanServerBus(Person* _pax) ;

	bool BeginCorralService(ElapsedTime curTime);
	void ClearCorralEvent(CorralServiceEvent *pEvent);
protected:
	bool IsCorralService() const;

	bool CorralService(Person *person, ElapsedTime curTime);
	ElapsedTime GetCorralServiceTime(ElapsedTime curTime);
	void leaveQueue (const Person *aPerson, ElapsedTime p_time, bool _bNoDelay );
	virtual bool isVacant( Person* pPerson  = NULL) const;


	//std::vector<Person *> m_vPersonWaitService;
	//void AddPersonToWaitServiceList(Person *pPerson);
	//int GetPersonWaitServiceGroupSize();
	//Person *GetHeadPersonInQueue();
	void processPerson (Person *aPerson, ElapsedTime p_time, bool _bStation );
	bool IsAllPersonsReadyForService();

};

#endif
