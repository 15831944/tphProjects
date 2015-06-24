#pragma once
#include "..\core\State.h"
#include "Common\elaptime.h"
#include "..\SEvent.H"

//
class LandsideVehicleInSim;
class IState_LandsideVehicle : virtual public IState
{
public:
	virtual void Entry(CARCportEngine* pEngine){ Execute(pEngine); }
	static ElapsedTime curTime();
	virtual LandsideVehicleInSim* getVehicle()const =0;
	IStateMachine* getMachine()const; 
};


template<class T>
class State_LandsideVehicle : public IState_LandsideVehicle
{
public:
	State_LandsideVehicle(T* pV){ m_pOwner = pV; }
	LandsideVehicleInSim* getVehicle()const{ return m_pOwner; }
protected:
	T* m_pOwner;
};



class State_ServiceTimer : public SEvent
{
public:
	State_ServiceTimer();
	bool isEnded()const{ return m_bEnd; }	
	void StartTimer(IState* pState, const ElapsedTime& endT );
	inline void SetEnd(bool b){ m_bEnd =b; }
protected:
	IState * m_pState;
	bool m_bEnd;
	virtual int getEventType (void) const { return NormalEvent; }
	virtual const char *getTypeName (void) const{ return _T("Service Time Ended"); }
	virtual void doProcess(CARCportEngine* pEngine );
};


//class State_OnBirthLandsideVehicle : public State_LandsideVehicle<LandsideVehicleInSim>
//{
//public:
//	State_OnBirthLandsideVehicle(LandsideVehicleInSim* pV)
//		:State_LandsideVehicle<LandsideVehicleInSim>(pV){}
//
//	virtual const char* getDesc()const{ return _T("On Birth"); }
//	virtual void Entry(CARCportEngine* pEngine);
//};
