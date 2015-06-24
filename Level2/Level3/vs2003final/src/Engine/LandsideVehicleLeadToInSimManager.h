#pragma once
#include "Inputs\MobileElemConstraint.h"
#include "Common\HierachyName.h"
#include "Common\ALTObjectID.h"
#include "Common\containr.h"

class NonResidentVehicleTypeContainer;
class LandsideFacilityLayoutObjectList;
class NonResidentVehicleTypeData;
class NonResidentVehicleTypePlan;
class ResidentVehicleRoute;
class CHierachyName;
class LandsideFacilityObject;
class ResidentVehicleRouteFlow;

class LandsideVehicleDirection
{
public:
	LandsideVehicleDirection();
	~LandsideVehicleDirection();

	void Insert( const CMobileElemConstraint& _paxType, const CHierachyName& vehicleType,const ALTObjectID& objName );
	bool CanLeadTo( const CMobileElemConstraint& _paxType, const CHierachyName& vehicleType,const ALTObjectID& objName)const;

	bool operator == (const LandsideVehicleDirection& vehicleDir)const;

	bool operator< (const LandsideVehicleDirection& lsDirection) const;

private:
	CMobileElemConstraint m_paxType;
	CHierachyName m_vehicleType;
	std::vector<ALTObjectID> m_vCanGoToLandsideObject;
};

class LandsideDirectionConstraint
{
public:
	LandsideDirectionConstraint(const ALTObjectID& objName);
	~LandsideDirectionConstraint();

	void Insert( const CMobileElemConstraint& _paxType, const CHierachyName& vehicleType,const ALTObjectID& objName);
	bool CanLeadTo(const CMobileElemConstraint& _paxType, const CHierachyName& vehicleType, const ALTObjectID& objName,const ALTObjectID& objDest)const;

	bool operator == (const LandsideDirectionConstraint& dirCon)const;
private:
	ALTObjectID m_landsideObject;
	SortedContainer<LandsideVehicleDirection> m_vDirection;
};

class LandsideVehicleLeadToInSimManager
{
public:
	LandsideVehicleLeadToInSimManager(void);
	~LandsideVehicleLeadToInSimManager(void);

	void Inilization(NonResidentVehicleTypeContainer *pNonResidentFlow, const LandsideFacilityLayoutObjectList& layoutObjectList);

	bool CanLeadTo(const CMobileElemConstraint& _paxType,const CHierachyName& vehicleType, const ALTObjectID& objName,const ALTObjectID& objDest)const;
	void Insert( const CMobileElemConstraint& _paxType, const CHierachyName& vehicleType,const ALTObjectID& srcName,const ALTObjectID& objName);
private:
	void ProcessPaxTypeData(NonResidentVehicleTypeData *pVehicleTypeData, const LandsideFacilityLayoutObjectList& layoutObjectList);
	void ProcessTimeData(NonResidentVehicleTypePlan *pOrginalTimePlan,const CHierachyName& vehicleType, const CMobileElemConstraint& _paxType,const LandsideFacilityLayoutObjectList& layoutObjectList);
	void ProcessRouteData(ResidentVehicleRoute *pOriginalRoute,ResidentVehicleRouteFlow* pVehicleRouteFlow,const CHierachyName& vehicleType,
							std::vector<LandsideFacilityObject> vFacilityObject,
							const CMobileElemConstraint& _paxType,
							const LandsideFacilityLayoutObjectList& layoutObjectList);
private:
	std::vector<LandsideDirectionConstraint> m_vLeadTo;
};

