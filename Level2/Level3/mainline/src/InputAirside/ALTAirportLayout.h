#pragma once

#include "ALTAirport.h"
#include ".\filettaxiway.h"
#include "./IntersectionNodesInAirport.h"
#include "AirsidePaxBusParkSpot.h"
#include "AirsideBagCartParkSpot.h"

class INPUTAIRSIDE_API ALTAirportLayout : public ALTAirport
{
public:
protected:

	void LoadTaxiway();
	void LoadRunway();
	void LoadHeliport();
	void LoadStand();
	void LoadDeicePad();
	void LoadStretch();
	void LoadRoadIntersection();
	void LoadPoolParking();
	void LoadTrafficLight();
	void LoadTollGate();
	void LoadStopSign();
	void LoadYieldSign();
	void LoadCircleStretch();
	void LoadStartPosition();
	void LoadMeetingPoint();

	void ClearData();

public:
	//the airport id exists
	void LoadData();
	Runway *GetRunwayByID(int nRunwayID);
	Taxiway* GetTaxiwayByID(int nTaxiwayID);
	ALTObject *GetObjectByID(int nObjectID);		

protected:
	std::vector<Runway::RefPtr > m_vRunway;
	std::vector<Taxiway::RefPtr > m_vTaxiway;
	std::vector<Heliport::RefPtr > m_vHeliport;
	std::vector<Stand::RefPtr > m_vStand	;
	std::vector<DeicePad::RefPtr > m_vDeicePad	;
	std::vector<Stretch::RefPtr > m_vStretch	;
	std::vector<Intersections::RefPtr > m_vRoadIntersection	;
	std::vector<VehiclePoolParking::RefPtr > m_vPoolParking	;
	std::vector<TrafficLight::RefPtr > m_vTrafficLight	;
	std::vector<TollGate::RefPtr > m_vTollGate	;
	std::vector<StopSign::RefPtr > m_vStopSign	;
	std::vector<YieldSign::RefPtr > m_vYieldSign	;
	std::vector<CircleStretch::RefPtr > m_vCircleStretch	;
	std::vector<CStartPosition::RefPtr > m_vStartPosition	;
	std::vector<CMeetingPoint::RefPtr > m_vMeetingPoint	;
	std::vector<AirsidePaxBusParkSpot::RefPtr > m_vPaxBusSpot	;
	std::vector<AirsideBagCartParkSpot::RefPtr > m_vBagCartSpot	;
public:
	FilletTaxiwaysInAirport m_vFilletTaxiways;
	IntersectionNodesInAirport m_nodelist;

	//the object exists in the airport, but now only taxiway and runway, if need, add
	//std::vector<ALTObject *> m_vObjects;


	template <class ClassType > 
	static	ClassType *TemplateGetObject(const CGuid& guid, std::vector<typename ClassType::RefPtr >& vObjects)
	{
		std::vector<ClassType::RefPtr>::iterator iter = vObjects.begin();

		for (; iter != vObjects.end(); ++iter)
		{
			if((*iter).get() &&  (*iter).get()->getGuid() == guid)
				return (ClassType *)(*iter).get();
		}
		return NULL;
	};

	template <class ClassType > 
	static void TemplateGetObjectList(std::vector<void *>& vData, std::vector<typename ClassType::RefPtr >& vObjects)
	{
		std::vector<ClassType::RefPtr>::iterator iter = vObjects.begin();

		for (; iter != vObjects.end(); ++iter)
		{
			vData.push_back((*iter).get());
		}
	};
public:
	template <class DataType>
	bool QueryDataObjectList(std::vector<DataType*>& vData)
	{
		std::vector<void*> vPtrs;
		if (GetObjectList(DataType::getTypeGUID(),vPtrs))
		{
			vData.reserve(vData.size() + vPtrs.size());
			for (std::vector<void*>::iterator ite = vPtrs.begin();ite!=vPtrs.end();ite++)
			{
				vData.push_back((DataType*)*ite);
			}
			return true;
		}
		return false;
	}
public:

	ALTObject *GetObject(const CGuid& guid, const GUID& class_guid);

	bool GetObjectList(const GUID& class_guid,std::vector<void*>& vData);

	void GetEntireALTObjectList(ALTObjectList& objList);
};














