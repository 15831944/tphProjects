#pragma once
class LandsideFacilityLayoutObject;
enum ALTOBJECT_TYPE;
#include <common/containr.h>

class LandsideRouteGraph;
class LandsideResourceManager;
class LandsideResourceInSim;
class StretchSpeedControlData;
class LandsideVehicleInSim;
class CFacilityBehavior;
class LandsidePaxTypeLinkageInSim;
class ElapsedTime;
class ALTObjectID;
class LandsideParkingLotInSim;
class LandsideExternalNodeInSim;
class LandsideCurbSideInSim;
class LandsideTaxiQueueInSim;
class LandsideTaxiPoolInSim;
class LandsideBusStationInSim;
class VBDistributionObject;
class CHierachyName;
class VBDistributionObjectServiceTime;

class LandsideLayoutObjectInSim
{
public:
	LandsideLayoutObjectInSim(LandsideFacilityLayoutObject* pInput);
	virtual ~LandsideLayoutObjectInSim(void);

	//dynamic cast
	virtual LandsideParkingLotInSim* toParkLot(){ return NULL; }
	virtual LandsideExternalNodeInSim* toExtNode(){ return NULL; }
	virtual LandsideCurbSideInSim* toCurbSide(){ return NULL; }
	virtual LandsideTaxiQueueInSim* toTaxiQueue(){ return NULL; }
	virtual LandsideTaxiPoolInSim* toTaxiPool(){ return NULL; }
	virtual LandsideBusStationInSim* toBusSation(){ return NULL; }


	virtual LandsideResourceInSim* IsResource(){ return NULL; }
	virtual void InitRelateWithOtherObject(LandsideResourceManager* pRes)=0;
	virtual void InitGraphNodes(LandsideRouteGraph* pGraph)const=0;
	virtual void InitGraphLink(LandsideRouteGraph* pGraph)const=0;

	virtual bool IsHaveCapacityAndFull(LandsideVehicleInSim*pVeh)const;
	virtual bool IsHaveCapacity(LandsideVehicleInSim*pVeh)const;


	LandsideFacilityLayoutObject* getInput()const{ return m_pInput; }
	int getID()const;
	ALTOBJECT_TYPE getType()const;	
	const ALTObjectID& getName()const; 
	
	
	void SetSpeedControlData(StretchSpeedControlData* pConst){ m_pSpeedControl = pConst; }
	StretchSpeedControlData* GetSpeedControlData(){ return m_pSpeedControl; }

	void SetServiceTime(VBDistributionObject* pServiceT){ m_pServiceTime = pServiceT; }
	ElapsedTime GetServiceTime(const CHierachyName& vehicleType)const;
	

	void SetBehavior(CFacilityBehavior* pBeh); 
	CFacilityBehavior* GetBehavior();

	LandsidePaxTypeLinkageInSim * getPaxLeadToLinkage() const;
	bool CanLeadTo(const ElapsedTime& eTime, LandsideLayoutObjectInSim *pObject)const;

protected:
	LandsidePaxTypeLinkageInSim *m_pPaxLeadToLinkage;

protected:
	StretchSpeedControlData* m_pSpeedControl;
	CFacilityBehavior* m_pBehavior;
	LandsideFacilityLayoutObject* m_pInput;
	VBDistributionObject* m_pServiceTime;
	VBDistributionObject* GetServiceTime(){ return m_pServiceTime; }

};


class LandsideLayoutObjectInSimList : public UnsortedContainer<LandsideLayoutObjectInSim>
{
public:

protected:
};