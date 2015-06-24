#ifndef PASSENGER_H
#define PASSENGER_H

#include "common\fsstream.h"
#include "common\template.h"
#include "common\containr.h"
#include "engine\person.h"
#include "engine\visitor.h"
#include "engine\ConvergePair.h"

class HoldingArea ;
class TrayHost;
class RetailProcessor;

//----------------------------------------------------------------------------
//Summary:
//		passenger shopping in retail processor
//----------------------------------------------------------------------------
struct PaxShoppingCondition
{
	RetailProcessor* m_pRetailProc;
	std::map<int,int> m_productMap;
};

class RetailPaxShoppingCondition
{
public:
	RetailPaxShoppingCondition()
	{

	}
	~RetailPaxShoppingCondition();
	void AddEntry(RetailProcessor* pRetailProc,int nProductID,int nCount);
	int GetCount()const;
	PaxShoppingCondition* FindEntry(int ndx)const;
	void DeleteEntry(RetailProcessor* pRetailProc);
	void ClearEntry();

private:
	std::vector<PaxShoppingCondition*> m_vRetailShoppingCondition;
};
//----------------------------------------------------------------------------------

class Passenger : public Person
{
protected:
	int m_iBagsCount;
	RetailPaxShoppingCondition* m_pRetailPaxShopping;
    //the plan which the passenger on 

public:
    
	//Visitors (greeter or sender) who attached to the pax.
	std::vector<PaxVisitor*> m_pVisitorList;
	//all trayhost visitor list
	std::vector<PaxVisitor*> m_pTrayVisitorList;

	//visitor index
	std::vector<long> m_vVisIndex;
	
	PaxVisitor* GetVisitorItemByType(int nTypeIndex);

    Passenger (MobLogEntry& p_entry, CARCportEngine* _pEngine );
    virtual ~Passenger ();;

	virtual void setTerminalDestination(const Point& p);

	// Get group and visitor size
	virtual int GetActiveTotalSize() const;
	

    //It writes the element's current location, state to the log file.
	// in:
	// _bBackup: if the log is backwards.
    virtual void writeLogEntry (ElapsedTime time, bool _bBackup, bool bOffset = true );

    //It tests last track to see if flight was missed.
    // If so, it is entered into the list of late passengers.
    void testForLate (ElapsedTime curTime, long _lPaxID );

    // Dumps all tracks to the log and updates ElementDescription record.
    // It is called at death of passenger and must repeat once for each member of group.
    // It also tests for late passengers
    virtual void flushLog (ElapsedTime currentTime, bool bmissflight = false);

	// test if the person is my visitor
	bool IsMyVisitor( Person* _pPerson )const;
	
	bool IsMyVisitor( long _lPerson )const;


	int GetBagsCount()const { return m_iBagsCount;	};
	
//	virtual int getAttatchedNopaxCount() const;
	virtual void regetAttatchedNopax();
	
	// new person for group follower
	virtual void newGroupFollower( void );

	//set visitor list and follower arrival door
	virtual void SetFollowerArrivalDoor(bool bArrivalDoor);

	void NewConvergePair(long _iVisitorId,long _iVisitorType);
	void DeleteConvergePair(long _iVisitorId,bool _IsMyWantDele);
    void DeleteGreetPool(long _iFirId,long _iSecoId);

	std::vector<Person*>& GetBags(){ return m_vBags;}


	void AttachCarryon(PaxVisitor *pCarryon);
	 virtual DistanceUnit getInStep (void) const;

	void MakePassengerSyncAvailable(const ElapsedTime& curTime);
	//-------------------------------------------------------------
	//Summary:
	//		passenger methods of retail store shopping
	//-------------------------------------------------------------
	void AddEntry(RetailProcessor* pRetailProc,int nProductID,int nCount);
	void ClearEntry();
	void CheckOutShopping(RetailProcessor* pRetailProc,const ElapsedTime& curTime);
	void CancelShopping( RetailProcessor* pRetailProc, const ElapsedTime& curTime );
	//-----------------------------------------------------------------
};

#endif
