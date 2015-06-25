#pragma once
#include "airsidebaggagetuginsim.h"

enum FlightOperation;
class BaggageTrainServiceRequest;
class CBagCartsParkingSpotInSim;
class CARCportEngine;
class AirsideBaggageCartInSim;
class Processor;
class CMobileElemConstraint;



class AirsideBaggageTrainInSim :
	public AirsideBaggageTugInSim
{
public:
	AirsideBaggageTrainInSim(int id,
		int nPrjID,
		CVehicleSpecificationItem *pVehicleSpecItem,
		CVehicleSpecificationItem * pCartsSpecifications,
		CARCportEngine *_pEngine);
	~AirsideBaggageTrainInSim(void);

public:
	int GetVehicleCapacity();

	void SetServiceCount(int  nCount);

	void SetServiceFlight(AirsideFlightInSim* pFlight, double vStuffPercent,BaggageTrainServiceRequest& request );
	void GetNextCommand();
	CARCportEngine *getEngine();

	CBagCartsParkingSpotInSim * getBagCartsSpotInSim() const;

	FlightOperation getFlightOperation() const;

	//initialize the baggage train with the full baggage carts
	void Initialize(int& nNextVehicleUnqiueID);

	ElapsedTime GetServiceTimePerBag()const;
public:
	void UnloadBaggageToLoader(ElapsedTime time);

	bool CanServe(const CMobileElemConstraint& mobCons);
	void LoadBaggageFromPusher(ElapsedTime time);
	void TransferTheBag(Person *pBag, const ElapsedTime& eTime,ElapsedTime& retTime);


	virtual int ArriveAtStand(ElapsedTime time);

	void LoadBagFromFlight(ElapsedTime time) ;
	void UnloadBaggageFromCart(ElapsedTime time);
public:
	
	//vehicle moves
	//after this cart finished load or unload baggage, the baggage train would move and the next cart move to the service point
	void MoveToNextCart(const ElapsedTime& eTime);


	AirsideBaggageCartInSim *getCurBaggageCart();

	virtual void SetPosition(const CPoint2008& pos);
	virtual void SetSpeed(double speed);

	virtual void InitLogEntry(VehiclePoolInSim*pool, OutputAirside * pOut);
	virtual void FlushLog(const ElapsedTime& endTime);
	virtual void WirteLog(const CPoint2008& p, const double speed, const ElapsedTime& t, bool bPushBack = false);
protected:

	//if the baggage are all oon board
	//the vehicle starts to move
	bool IsReadyToGo();
	bool isLastCart()const;
	void LeaveStandService(const ElapsedTime& leaveTime);
protected:
	int m_curCartIndex;;
	std::vector<AirsideBaggageCartInSim *> m_vCartAssigned;


protected:
	CVehicleSpecificationItem * m_pCartsSpecifications;


	//the baggage count serviced by this train
	int m_nBagCount;

	int m_nBagLoaded;

	FlightOperation m_enumFltOperation;
	CBagCartsParkingSpotInSim * m_pBagCartsSpotInSim;

	CARCportEngine *m_pEngine;

	std::vector<AirsideBaggageCartInSim *> m_vBagCarts;

	int m_nProjID;
	
	//the terminal processor that the train services
	Processor *m_pTermProc;

	//trace in length;
	CPath2008 m_trace;

};
