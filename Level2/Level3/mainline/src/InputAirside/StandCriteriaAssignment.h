#pragma once
#include "Common/FLT_CNST.H"
#include "InputAirsideAPI.h"

class Stand;
class CADORecordset;
//////////////////////FlightTypeStandAssignment
class INPUTAIRSIDE_API FlightTypeStandAssignment
{
public:
	FlightTypeStandAssignment();
	~FlightTypeStandAssignment();
public:
	void setLeadInLineID(int nLeadInLineID) {m_nLeadInLineID = nLeadInLineID;}
	int getLeadInLineID(){return m_nLeadInLineID;}

	void setLeadOutLineID(int nLeadOutLineID) {m_nLeadOutLineID = nLeadOutLineID;}
	int getLeadOutLineID(){return m_nLeadOutLineID;}

	void SetAirportDatabase(CAirportDatabase* pAirportDatabase){ m_pAirportDB = pAirportDatabase; }

	void SetFlightType(const FlightConstraint& strFlightType);
	FlightConstraint& GetFlightConstraint(){ return m_FlightType; }
	const FlightConstraint& GetFlightConstraint()const{ return m_FlightType; }
	CString GetFlightTypeScreenString()const;
	//CString GetFlightTypeName(){ return m_strFlightTypeName; }
	
	void ReadData(CADORecordset& adoRecordset);
	void SaveData();
	void UpdateData();
	void DeleteData();
private:
	int m_nID;
	int m_nLeadInLineID;
	int m_nLeadOutLineID;
	FlightConstraint m_FlightType;
	//CString m_strFlightTypeName;
	CAirportDatabase* m_pAirportDB;
};
/////////////////////////StandCriteriaAssignment
class INPUTAIRSIDE_API StandCriteriaAssignment
{
public:
	StandCriteriaAssignment(void);
	~StandCriteriaAssignment(void);
public:
	void SaveData(int nProjID);
	void DeleteData();

	void setStandID(int nStandID) {m_nStandId = nStandID;}
	int getStandID() {return m_nStandId;}

	void setProjID(int nProjID) {m_nPrjID = nProjID;}
	int getProJID(){return m_nPrjID;}

	int getCount(){return (int)m_vFlightTypeStandAssignmentList.size();}
	FlightTypeStandAssignment* getItem(int nIndex)
	{
		ASSERT(nIndex >= 0 && nIndex < getCount());
		return m_vFlightTypeStandAssignmentList[nIndex];
	}
	void addItem(FlightTypeStandAssignment* pFlightType){m_vFlightTypeStandAssignmentList.push_back(pFlightType);}
	void removeItem(int nIndex);
	void removeItem(FlightTypeStandAssignment* pFlightType);
	void clear();

	void SetAirportDatabase(CAirportDatabase* pAirportDatabase){ m_pAirportDB = pAirportDatabase;}
	void SortFlightType();
private:
	int m_nStandId;
	int m_nPrjID;
	std::vector<FlightTypeStandAssignment*>m_vFlightTypeStandAssignmentList;
	std::vector<FlightTypeStandAssignment*>m_vDelFlightTypeStandAssignmentList;
	CAirportDatabase* m_pAirportDB;
};
/////////////////////StandCriteriaAssignmentList
class INPUTAIRSIDE_API StandCriteriaAssignmentList
{
public:
	StandCriteriaAssignmentList();
	~StandCriteriaAssignmentList();

	void ReadData(int nPrjID);
	void SaveData(int nPrjID);

	int getCount(){return (int)m_vStandCriteriaAssignmentList.size();}
	StandCriteriaAssignment* getItem(int nIndex)
	{
		ASSERT(nIndex >= 0 && nIndex < getCount());
		return m_vStandCriteriaAssignmentList[nIndex];
	}

	StandCriteriaAssignment* fit(Stand* pStand);
	StandCriteriaAssignment* findItem(int nStandID);
	void removeItem(int nIndex);
	void removeItem(StandCriteriaAssignment* pStandCriteria);
	void addItem(StandCriteriaAssignment* pStandCriteria){m_vStandCriteriaAssignmentList.push_back(pStandCriteria);}

	void SetAirportDatabase(CAirportDatabase* pAirportDatabase){ m_pAirportDB = pAirportDatabase;}
	bool IsExistInStandCriteriaList(StandCriteriaAssignment* pStandCriteria);

	void SortFlightType();
protected:
private:
	std::vector<StandCriteriaAssignment*>m_vStandCriteriaAssignmentList;
	std::vector<StandCriteriaAssignment*>m_vDelStandCriteriaAssignmentList;
	CAirportDatabase* m_pAirportDB;
};