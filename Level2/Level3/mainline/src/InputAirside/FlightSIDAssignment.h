#pragma once
#include "common/elaptime.h"
#include "Common/FLT_CNST.H"
#include <vector>
#include "InputAirsideAPI.h"

class CADORecordset;
/////////////////SID percentage/////////////////////////
class INPUTAIRSIDE_API SIDPercentage
{
public:
	SIDPercentage(void);
	~SIDPercentage(void);

	void ReadData(CADORecordset& recordset);
	void SaveData(int nParentID);
	void UpdateData(int nParentID);
	void DeleteData();

	int GetID() { return m_nID; }
	int GetSIDID() { return m_nSIDID; }
	void SetSIDID(int nSIDID){ m_nSIDID = nSIDID; }
	void SetPercent(int nPercent){ m_nPercent = nPercent; }
	int GetPercent(){ return m_nPercent; }

private:
	int m_nID;
	int m_nSIDID;
	int m_nPercent;

};
/////////////////Flight sid assignment///////////////////
class INPUTAIRSIDE_API FlightSIDAssignment
{
public:
	FlightSIDAssignment(void);
	~FlightSIDAssignment(void);

	void ReadData(CADORecordset& recordset);
	void SaveData(int nPrjID);
	void UpdateData(int nPrjID);
	void DeleteData();

	int GetSIDCount();
	SIDPercentage* GetSIDItem(int nIndex);

	void DeleteSIDItem(SIDPercentage* pSIDItem);
	void AddSIDItem(SIDPercentage* pSIDItem);

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
	std::vector<SIDPercentage*> m_vSIDPercentageList;
	std::vector<SIDPercentage*> m_vSIDPercentageDelList;
};

/////////////////Flight sid assignment list///////////////////
class INPUTAIRSIDE_API FlightSIDAssignmentList
{
public:
	FlightSIDAssignmentList(void);
	~FlightSIDAssignmentList(void);

	void ReadData(int nPrjID);
	void SaveData();
	void UpdateData();
	void DeleteData();

	int GetSIDCount();
	FlightSIDAssignment* GetSIDItem(int nIndex);

	void DeleteSIDItem(FlightSIDAssignment* pSIDItem);
	void AddSIDItem(FlightSIDAssignment* pSIDItem);

	void SetAirportDatabase(CAirportDatabase* pAirportDatabase){ m_pAirportDB = pAirportDatabase;}

	void Sort();

private:
	std::vector<FlightSIDAssignment*> m_vFlightSIDAssignmentList;
	std::vector<FlightSIDAssignment*> m_vFlightSIDAssignmentDelList;
	int m_nPrjID;
	CAirportDatabase* m_pAirportDB;

};

