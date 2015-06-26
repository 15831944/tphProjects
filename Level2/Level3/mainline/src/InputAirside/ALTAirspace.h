#pragma once
#include "ALTObject.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API ALTAirspace

{
public:
	ALTAirspace();
	~ALTAirspace(void);

public:
	static void GetWaypointList(int nAirspaceID, ALTObjectUIDList& IDList);
	static void GetSectorList(int nAirspaceID, ALTObjectUIDList& IDList);
	static void GetHoldList(int nAirspaceID, ALTObjectUIDList& IDList);
	static void GetAirRouteList(int nAirspaceID, ALTObjectUIDList& IDList);
	static void GetSectorObjList(int nAirspaceID, ALTObjectList& vObjectList);


public:
	void LoadAirspace(int nAirSpaceID);
	ALTObject *GetObjectByID(ALTOBJECT_TYPE enumObjType, int nID);

public:
	ALTObjectList* getWaypointList();
	ALTObjectList* getAirRouteList();
	ALTObjectList* getAirHoldList();
	ALTObjectList* getAirSectorList();

	const ALTObject* GetWaypoint(const CGuid& guid) const;
	const ALTObject* GetAirRoute(const CGuid& guid) const;
	const ALTObject* GetAirHold(const CGuid& guid) const;
	const ALTObject* GetAirSector(const CGuid& guid) const;

protected:
	void LoadWayPoint(int nAirSpaceID);
	void LoadAirRoute(int nAirSpaceID);
	void LoadAirHold(int nAirSpaceID);
	void LoadAirSector(int nAirSpaceID);

	const ALTObject *GetObject(const ALTObjectList&, const CGuid& guid) const;
	ALTObject *GetObject(ALTObjectList&, int nID);

protected:
	ALTObjectList m_lstWaypoint;
	ALTObjectList m_lstAirRoute;
	ALTObjectList m_lstAirHold;
	ALTObjectList m_lstAirSector;



	
};
