#pragma once
#include "LandsidePaxVehicleInSim.h"
#include "..\LandsideVehicleEntryInfo.h"
#include "State_LandsideVehicle.h"



class LandsidePrivateVehicleInSim : public LandsidePaxVehicleInSim
{
public:
	virtual LandsidePrivateVehicleInSim*  toPrivate(){ return this; }

	LandsidePrivateVehicleInSim(const PaxVehicleEntryInfo& entryInfo);
	virtual bool InitBirth(CARCportEngine *_pEngine);

	virtual void SuccessParkInCurb( LandsideCurbSideInSim*pCurb ,LaneParkingSpot* spot);
	virtual void SuccessParkInLotSpot(LandsideParkingSpotInSim* plot);

	virtual bool ProceedToNextFcObject(CARCportEngine* pEngine);

	virtual void OnFailPickPaxAtCurb(LandsideCurbSideInSim* pCurb, LaneParkingSpot* spot, CARCportEngine* pEngine);

	bool isArrival()const;
	bool isDepLongTerm(LandsideLayoutObjectInSim* pObj);
	bool isDepShortTerm(LandsideLayoutObjectInSim* pObj);


	void CallPaxGetOn( CARCportEngine* pEngine  ,LandsideResourceInSim* pRes );
	bool HavePaxNeedGetOn( CARCportEngine* pEngine , LandsideResourceInSim* pRes ) const;

	bool AllPaxOnVehicle(CARCportEngine* pEngine);
};



//State Parking at curbside
class State_PickPaxAtCurbsidePrivate : public State_LandsideVehicle<LandsidePrivateVehicleInSim> 
{
public:
	State_PickPaxAtCurbsidePrivate(LandsidePrivateVehicleInSim* pV,LandsideCurbSideInSim*pCurb,LaneParkingSpot* spot);
	virtual const char* getDesc()const{ return _T("Arr Parking at Curbside"); }

	virtual void Entry(CARCportEngine* pEngine);
	virtual void Execute(CARCportEngine* pEngine);	
	virtual void Exit(CARCportEngine* pEngine);
protected:
	LandsideCurbSideInSim* m_pCurb;
	State_ServiceTimer m_waitTimer;
	LaneParkingSpot* m_spot;
};


//////////////////////////////////////////////////////////////////////////
class State_ParkingAtCellPhoneLotPrivate : public State_LandsideVehicle<LandsidePrivateVehicleInSim>
{
public:
	State_ParkingAtCellPhoneLotPrivate(LandsidePrivateVehicleInSim* pV,LandsideParkingSpotInSim* pSpot)
		:State_LandsideVehicle<LandsidePrivateVehicleInSim>(pV){ mpSpot = pSpot;  }
	
	virtual const char* getDesc()const{ return _T("Wait In CellPhone Lot"); }
	virtual void Execute(CARCportEngine* pEngine);
protected:
	LandsideParkingSpotInSim* mpSpot;
};
//////////////////////////////////////////////////////////////////////////
class State_ParkingInLotSpotPrivateDep : public State_LandsideVehicle<LandsidePrivateVehicleInSim>
{
public:
	enum Parking_Type{ _depshort, _deplongterm , _other , };
	State_ParkingInLotSpotPrivateDep(LandsidePrivateVehicleInSim* pV,LandsideParkingSpotInSim* pSpot);
	virtual const char* getDesc()const{ return _T("Park In Lot Spot"); }
	virtual void Entry(CARCportEngine* pEngine);
	virtual void Execute(CARCportEngine* pEngine);
protected:
	LandsideParkingSpotInSim* m_pSpot;
	State_ServiceTimer m_WaitVisitorTimer;
    Parking_Type m_ptype;	
};


class State_ParkingInLotSpotPrivateArr : public State_LandsideVehicle<LandsidePrivateVehicleInSim>
{
public:
	State_ParkingInLotSpotPrivateArr(LandsidePrivateVehicleInSim* pV,LandsideParkingSpotInSim* pSpot)
	:State_LandsideVehicle<LandsidePrivateVehicleInSim>(pV)
	{
		m_pSpot = pSpot;
	}
	virtual const char* getDesc()const{ return _T("Park In Lot Spot"); }
	virtual void Entry(CARCportEngine* pEngine);
	virtual void Execute(CARCportEngine* pEngine);
protected:
	LandsideParkingSpotInSim* m_pSpot;
};