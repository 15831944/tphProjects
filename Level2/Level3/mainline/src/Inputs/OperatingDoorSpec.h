#pragma once
#include "Common/FLT_CNST.H"
#include <vector>
#include "../Common/ALTObjectID.h"

class CADORecordset;

class StandOperatingDoorData
{
public:
	StandOperatingDoorData(void);
	~StandOperatingDoorData(void);

	int GetID();
	void SetID(int nID);

	int GetStandID();
	void SetStandID(int nID){ m_nStandID = nID;}

	const ALTObjectID& GetStandName();
	void SetStandName(const ALTObjectID& altName);

	int GetOpDoorCount();
	int GetOpDoorID(int nIdx);
	void GetOpDoors(std::vector<int>& vDoors);

	void AddOpDoor(int nID);
	void DelOpDoor(int nID);
	void SetAirportDatabase(CAirportDatabase* pAirportDatabase){ m_pAirportDB = pAirportDatabase; }

	void ReadData(CADORecordset& recordset);
	void SaveData(int nParentID);
	void UpdateData(int nParentID,const CString& strDoors);
	void DeleteData();

private:
	int m_nID; //database key

	int m_nStandID;
	ALTObjectID m_standName;
	std::vector<int> m_vOpDoors;
	CAirportDatabase* m_pAirportDB;

};

//////////////////////////////////
class FltOperatingDoorData
{
public:
	FltOperatingDoorData(CAirportDatabase* pAirportDatabase);
	~FltOperatingDoorData(void);

	int GetID();
	void SetID(int nID);

	const FlightConstraint& GetFltType();
	void SetFltType(const FlightConstraint& fltType);

	int GetStandDoorCount();
	StandOperatingDoorData* GetStandDoorData(int nIdx);

	void AddStandDoor(StandOperatingDoorData* pData);
	void DelStandData(StandOperatingDoorData* pData);

	void ReadData(CADORecordset& recordset);
	void SaveData();
	void UpdateData();
	void DeleteData();

private:
	int m_nID; //database key
	FlightConstraint m_FltType;
	CAirportDatabase* m_pAirportDB;
	std::vector<StandOperatingDoorData*> m_vStandOpDoorData;
	std::vector<StandOperatingDoorData*> m_vDelStandOpDoorData;
};

/////////////////////////////////////
class OperatingDoorSpec
{
public:
	OperatingDoorSpec(void);
	~OperatingDoorSpec(void);

	int GetFltDataCount();
	FltOperatingDoorData* GetFltDoorData(int nIdx);

	void AddFlightOpDoor(FltOperatingDoorData* pData);
	void SetAirportDatabase(CAirportDatabase* pAirportDatabase){ m_pAirportDB = pAirportDatabase; }
	void DelFltData(FltOperatingDoorData* pData);

	void ReadData();
	void SaveData();

private:
	std::vector<FltOperatingDoorData*> m_vFltData;
	std::vector<FltOperatingDoorData*> m_vDelFltData;
	CAirportDatabase* m_pAirportDB;
};
