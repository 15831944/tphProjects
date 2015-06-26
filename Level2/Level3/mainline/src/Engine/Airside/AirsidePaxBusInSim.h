#pragma once
#include "airsidevehicleinsim.h"
class Passenger ;
class CVehicleSpecificationItem;
class CPaxBusParkingResourceManager;
class CARCportEngine;
class CAirsidePaxBusInSim :
	public AirsideVehicleInSim
{
public:
	CAirsidePaxBusInSim(int id,
		int nPrjID,
		CVehicleSpecificationItem *pVehicleSpecItem,
		CPaxBusParkingResourceManager * pPaxBusParkingManager,
		CARCportEngine *_pEngine);
	~CAirsidePaxBusInSim(void);


public:
	void SetServiceFlight(AirsideFlightInSim* pFlight, double vStuffPercent,bool bArrival);
    void WirteLog(const CPoint2008& p, const double speed, const ElapsedTime& t, bool bPushBack = false);

	long GetVehicleCapacity();
	void GetNextCommand();

    double m_dir ;
	int GetLoadPaxCount();
	void SetLoadPaxCount(int nPaxCount);
	bool IsServiceArrival();
	ElapsedTime m_preTime ;
	double m_preSpeed ;
	

	bool GetTimeOfLastPerson(ElapsedTime& t)const
	{
		if(m_currentpaxNum>0){
			t = m_tLastPersonOnBus;
			return true;
		}
		return false;
	}
	ElapsedTime GetServiceTimePerPerson()const;

	CARCportEngine *getEngine();
protected:
	CARCportEngine *m_pEngine;
	int m_nCapacity;

	bool m_bServiceArrival;

	int m_nPaxLoad;
	//the time of each pax takeoff bus
	ElapsedTime m_takeoff_sepeed ;
     
	int m_currentpaxNum ;
	ElapsedTime m_tLastPersonOnBus;  //time of last person take on this bus

	CPaxBusParkingResourceManager * m_pPaxBusParkingManager;
	CPoint2008 prePos;

protected:
	std::vector<Person*> m_pax ;
protected:
	void GeneratePaxTakeOffEvent(int _pax_state,ElapsedTime time) ;
public:
	void TakeOffPassenger(ElapsedTime time) ;
	void TakeOnPasseneger(Person* _pax,ElapsedTime time) ;
	void CancelTakeOnPassenger(Person* _pax,ElapsedTime t);

	BOOL TryTakePaxLeave(const ElapsedTime&t);
	BOOL IsFull()  ;
    double getDirect(CPoint2008 _curPoint);
	int ArriveAtStand(ElapsedTime time) ;
protected:
	void NoticePaxTakeOffFlight(ElapsedTime time) ;
public:
	void BusMoving(ElapsedTime time) ;
protected:
	void NoticeHoldingAreaBusLeave() ;
public:
	void AddPassenger(Person* _pax) ;
	bool havePerson(const Person* pPerson)const;//check pax bus if have this person
	void handlePersonDeath(Person* pPerson);
};
