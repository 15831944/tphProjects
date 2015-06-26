#pragma once

#include <vector>
#include "InputAirsideAPI.h"

class GroundRoute;

typedef std::vector<GroundRoute*> GroundRouteVector;
typedef std::vector<GroundRoute*>::iterator GroundRouteIter;

class INPUTAIRSIDE_API GroundRoutes
{
public:
	GroundRoutes(int nAirportID);
	~GroundRoutes();

	int GetAirportID();

	void ReadData();
	void SaveData();
	void VerifyData();

	int GetCount();
	GroundRoute* GetItem(int nIndex);
	GroundRoute* FindItem(int nID);
	BOOL IsItemExist(GroundRoute* pGroundRoute);

	void AddItem(GroundRoute* pGroundRoute);
	void DeleteItem(GroundRoute* pGroundRoute);


private:
	int m_nAirportID;

	GroundRouteVector m_vectGroundRoute;
	GroundRouteVector m_vectDelGroundRoute;
};
