#pragma once
#include "common/elaptime.h"
#include "Common/FLT_CNST.H"
#include <vector>
#include "InputAirsideAPI.h"

class CADORecordset;
/////////////////Star percentage/////////////////////////
class INPUTAIRSIDE_API StarPercentage
{
public:
	StarPercentage(void);
	~StarPercentage(void);

	void ReadData(CADORecordset& recordset);
	void SaveData(int nParentID);
	void UpdateData(int nParentID);
	void DeleteData();

	int GetID() { return m_nID; }
	int GetStarID() { return m_nStarID; }
	void SetStarID(int nStarID){ m_nStarID = nStarID; }
	void SetPercent(int nPercent){ m_nPercent = nPercent; }
	int GetPercent(){ return m_nPercent; }

private:
	int m_nID;
	int m_nStarID;
	int m_nPercent;

};
/////////////////Flight star assignment///////////////////
class INPUTAIRSIDE_API FlightStarAssignment
{
public:
	FlightStarAssignment(void);
	~FlightStarAssignment(void);

	void ReadData(CADORecordset& recordset);
	void SaveData(int nPrjID);
	void UpdateData(int nPrjID);
	void DeleteData();

	int GetSTARCount();
	StarPercentage* GetStarItem(int nIndex);

	void DeleteStarItem(StarPercentage* pStarItem);
	void AddStarItem(StarPercentage* pStarItem);

	void SetAirportDatabase(CAirportDatabase* pAirportDatabase){ m_pAirportDB = pAirportDatabase; }

	int GetID(){ return m_nID;}
	void setFlightType(const CString& strFlightType);
	FlightConstraint GetFlightConstraint(){ return m_FlightType; }
	CString GetFlightTypeName(){ return m_strFlightTypeName; }

	void SetStartTime(ElapsedTime tTime){ m_tStartTime = tTime; }
	void SetEndTime(ElapsedTime tTime){ m_tEndTime = tTime; }
	ElapsedTime GetStartTime(){ return m_tStartTime; }
	ElapsedTime GetEndTime(){ return m_tEndTime; }

private:
	int m_nID;
	FlightConstraint m_FlightType;
	CString m_strFlightTypeName;
	ElapsedTime m_tStartTime;
	ElapsedTime m_tEndTime;
	CAirportDatabase* m_pAirportDB;
	std::vector<StarPercentage*> m_vStarPercentageList;
	std::vector<StarPercentage*> m_vStarPercentageDelList;
};

/////////////////Flight star assignment list///////////////////
class INPUTAIRSIDE_API FlightStarAssignmentList
{
public:
	FlightStarAssignmentList(void);
	~FlightStarAssignmentList(void);

	void ReadData(int nPrjID);
	void SaveData();
	void UpdateData();
	void DeleteData();

	int GetSTARCount();
	FlightStarAssignment* GetStarItem(int nIndex);

	void DeleteStarItem(FlightStarAssignment* pStarItem);
	void AddStarItem(FlightStarAssignment* pStarItem);

	void SetAirportDatabase(CAirportDatabase* pAirportDatabase){ m_pAirportDB = pAirportDatabase;}

	void Sort();
private:
	std::vector<FlightStarAssignment*> m_vFlightStarAssignmentList;
	std::vector<FlightStarAssignment*> m_vFlightStarAssignmentDelList;
	int m_nPrjID;
	CAirportDatabase* m_pAirportDB;

};

