#pragma once

#include "Inputs\MobileElemConstraint.h"
#include "Common\ALTObjectID.h"
#include "Common\containr.h"
#include "Common\TimeRange.h"
#include "LandsideTerminalLinkageInSim.h"
#include "Common\HierachyName.h"


class NonResidentVehicleTypeContainer;
class LandsidePaxTypeLinkageInSim;
class LandsideFacilityLayoutObjectList;

class ResidentVehicleRoute;
class NonResidentVehicleTypePlan;
class NonResidentVehicleTypeData;

class LandsidePaxTypeLinkageTimeItemInSim;
class LandsidePaxTypeLinkageInSim;
class LandsideFacilityLayoutObjectList;
class LandsidePaxVehicleLinkageInSim;
class ResidentVehicleRouteFlow;
class LandsideFacilityObject;


//based on the pax type
//the landside object the pax could move to
//retrieve from non-resident vehicel flow

class LandsidePaxTypeLinkageInSimManager
{
public:
	LandsidePaxTypeLinkageInSimManager(void);
	~LandsidePaxTypeLinkageInSimManager(void);

public:
	void Initialize(NonResidentVehicleTypeContainer *pNonResidentFlow, const LandsideFacilityLayoutObjectList& layoutObjectList);


	LandsidePaxTypeLinkageInSim* getBestFitItem(const CMobileElemConstraint& paxType);

	LandsidePaxVehicleLinkageInSim* getBestFitItem(const CMobileElemConstraint& paxType, const CHierachyName& vehicleType);


protected:
	LandsidePaxTypeLinkageInSim* getEqualItem(const CMobileElemConstraint& paxType);
	LandsidePaxVehicleLinkageInSim* getPaxVehicleItem(const CHierachyName& vehicleType,const CMobileElemConstraint& paxType);
	void ProcessRouteData(ResidentVehicleRoute *pOriginalRoute, ResidentVehicleRouteFlow* pVehicleRouteFlow,std::vector<LandsideFacilityObject> vFacilityObject,LandsidePaxTypeLinkageTimeItemInSim *pTimeRange, const LandsideFacilityLayoutObjectList& layoutObjectList );
	void ProcessTimeData(NonResidentVehicleTypePlan *pOrginalTimePlan, LandsidePaxTypeLinkageInSim *pPaxTypeLinkage, const LandsideFacilityLayoutObjectList& layoutObjectList );
	void ProcessPaxTypeData(NonResidentVehicleTypeData *pVehicleTypeData, const LandsideFacilityLayoutObjectList& layoutObjectList );

protected:
	SortedContainer<LandsidePaxTypeLinkageInSim> m_vPaxTypeLinkage;
	SortedContainer<LandsidePaxVehicleLinkageInSim> m_vPaxVehicleLinkage;

};

class LandsidePaxTypeLinkageInSim
{
public:
	LandsidePaxTypeLinkageInSim();
	LandsidePaxTypeLinkageInSim(const CMobileElemConstraint& paxType);
	~LandsidePaxTypeLinkageInSim();

public:
	CMobileElemConstraint *getPaxType();

	void AddItem(LandsidePaxTypeLinkageTimeItemInSim * pTimeItem){m_vTimeRangeLinkage.push_back(pTimeItem);}

public:
	bool operator <(const LandsidePaxTypeLinkageInSim& _anotherLinkage) const;

	bool GetAllObjectCanLeadTo( const ElapsedTime& eTime, std::vector<LandsideObjectLinkageItemInSim>& vAllObject) const;

	bool GetAllObjectCanLeadTo( std::vector<LandsideObjectLinkageItemInSim>& vAllObject) const;

	bool CanLeadTo(const ElapsedTime& eTime, const ALTObjectID& altDestination) const;
protected:
	CMobileElemConstraint m_paxConstrait;
	//land side objects
	std::vector<LandsidePaxTypeLinkageTimeItemInSim *> m_vTimeRangeLinkage;

protected:
private:
};

class LandsidePaxVehicleLinkageInSim : public LandsidePaxTypeLinkageInSim
{
public:
	LandsidePaxVehicleLinkageInSim(const CMobileElemConstraint& paxType,const CHierachyName& vehicleType);
	~LandsidePaxVehicleLinkageInSim();

public:
	bool fit(const CMobileElemConstraint& paxType,const CHierachyName& vehicleType);
	bool Equal(const CMobileElemConstraint& paxType,const CHierachyName& vehicleType);

public:
	bool operator <(const LandsidePaxVehicleLinkageInSim& _anotherLinkage) const;

protected:
	CHierachyName m_vehicleType;
};

class LandsidePaxTypeLinkageTimeItemInSim
{
public:
	LandsidePaxTypeLinkageTimeItemInSim();
	~LandsidePaxTypeLinkageTimeItemInSim();

	LandsidePaxTypeLinkageTimeItemInSim(LandsidePaxTypeLinkageTimeItemInSim& _other);
public:
	void setTimeRange(const TimeRange& timeRange){m_timeRange = timeRange;}
	TimeRange getTimeRange(){return m_timeRange;}

	void AddObject(const ALTObjectID& objID)
	{
		m_vObject.push_back(LandsideObjectLinkageItemInSim(objID));
	}
	void AddObject(const ALTObjectID& objID, const std::vector<int>& vLevel)
	{
		m_vObject.push_back(LandsideObjectLinkageItemInSim(objID, vLevel));
	}	
	const std::vector<LandsideObjectLinkageItemInSim>& getObjects()const {return m_vObject;}

	bool CanLeadTo( const ALTObjectID& altDestination ) const;
protected:
	TimeRange m_timeRange;
	std::vector<LandsideObjectLinkageItemInSim> m_vObject;


protected:
private:
};







