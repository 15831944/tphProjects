#pragma once
#include "landsidestretchinsim.h"
#include "LaneParkingSpot.h"
#include "LandsidePaxSericeInSim.h"

/*class LandsideLaneNodeList;*/
class LaneSegInSim;
class LandsideResourceManager;
class LandsideTaxiQueue;
class PaxLandsideBehavior;
class LandsideSimulation;
class LandsideQueueSystemProcess;
class LandsideBaseBehavior;




class IUpdateOrderingVehicle
{
public:
	virtual void _UpdateOrderingVehicle(CARCportEngine* pEngine)=0;
	virtual void UpdateOrderingVehicle(const ElapsedTime& t){ Event* pE = new UpdateOrdingServiceVehiclesEvent(this); pE->setTime(t); pE->addEvent(); }

	class UpdateOrdingServiceVehiclesEvent : public Event
	{
	public:
		UpdateOrdingServiceVehiclesEvent(IUpdateOrderingVehicle* pHost){ m_pHost = pHost; }
		virtual int process(CARCportEngine *pEngine){  m_pHost->_UpdateOrderingVehicle(pEngine); return 1; }
		virtual int kill (void){ return 1; }
		//It returns event's name
		virtual const char *getTypeName (void) const { return _T("Update Ordering Service Vehicle"); }
		//It returns event type
		virtual int getEventType (void) const { return NormalEvent; }
	protected:
		IUpdateOrderingVehicle * m_pHost;
	};
};



class LandsideTaxiQueueInSim :
	public LandsideResourceInSim, public LandsideLayoutObjectInSim , public LandsidePaxSericeInSim, public IUpdateOrderingVehicle
{
public:
	LandsideTaxiQueueInSim(LandsideTaxiQueue *pQ,bool bLeftDrive);
	virtual ~LandsideTaxiQueueInSim();

	virtual void InitRelateWithOtherObject(LandsideResourceManager* pRes);
	virtual void InitGraphNodes(LandsideRouteGraph* pGraph)const{}
	virtual void InitGraphLink(LandsideRouteGraph* pGraph)const{}



	virtual LandsideTaxiQueueInSim* toTaxiQueue(){ return this; }
	LandsideResourceInSim* IsResource(){ return this; }
	virtual CString print()const;
	
	
	LandsideLaneNodeInSim* GetExitNode(LandsideLaneInSim* pLane);
	LandsideLayoutObjectInSim* getLayoutObject() const;

	LandsideTaxiQueue* GetTaxiQInput()const{return (LandsideTaxiQueue*)getInput(); }
	LandsideInterfaceParkingSpotsGroup& getParkingSpot(){ return mParkingSpots; }

	virtual CPath2008 GetQueue();
	virtual bool CanStartService(){return true;}

	//vehicle is coming to service
	void addServiceVehicle(LandsideVehicleInSim* pV);
	void removeServiceVehicle(LandsideVehicleInSim* pV);
	void AddWaitingVehicle(LandsideVehicleInSim* pTaxi);
	void DelWaitingVehicle(LandsideVehicleInSim* pTaxi);


	//update to check need to order more vehicles
	void _UpdateOrderingVehicle(CARCportEngine* pEngine);

	void NoticeWaitingPax(const ElapsedTime &time, LandsideVehicleInSim* pTaxi, LandsideSimulation* pSimulation);
	virtual void PassengerMoveInto(PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime);
	virtual void PassengerMoveOut(PaxLandsideBehavior *pPaxBehvior, ElapsedTime eTime);

	bool PaxTakeOnBus( PaxLandsideBehavior *pBehavior, LandsideSimulation *pSimulation, const ElapsedTime& eTime );
	void PaxLeaveQueue( const ElapsedTime& eTime );

private:
	std::vector<LandsideLaneEntry*> m_vLaneEntries;
	std::vector<LandsideLaneExit*> m_vLaneExits;

	VehicleList m_waitingtaxi;

	LandsideResourceManager* m_pAllRes;
	VehicleList m_vServiceVehicles;

	LandsideInterfaceParkingSpotsGroup mParkingSpots;
	LandsideQueueSystemProcess* m_queueProcessSys;
	CPath2008 m_waitingPath;

};
