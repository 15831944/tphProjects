#pragma once
#include "Common/FLT_CNST.H"
#include <vector>
#include "../Common/ALTObjectID.h"
#include "../Common/ACTypeDoor.h"

class CADORecordset;

class StandOperatingDoorData
{
public:
	StandOperatingDoorData(void);
	~StandOperatingDoorData(void);

    class OperationDoor
    {
    public:
        OperationDoor();
        ~OperationDoor();
        CString GetDoorString()const;
        CString GetHandTypeString()const;
        void readDataVersion1(ArctermFile& p_file);
        void parseString(const CString& strOp, ACTYPEDOORLIST* pAcDoors);
        void writeData(ArctermFile& p_file);
        bool operator == (const OperationDoor& doorOp)const;

        CString GetDoorName(){ return m_doorName; }
        void SetDoorName(CString strDoorName){ m_doorName = strDoorName; }

        void SetHandType(ACTypeDoor::DoorDir handType){ m_iHandType = handType; }
        ACTypeDoor::DoorDir GetHandType(){ return m_iHandType; }
    private:
        CString m_doorName;
        ACTypeDoor::DoorDir m_iHandType;
    };

	int GetID();
	void SetID(int nID);

	int GetStandID();
	void SetStandID(int nID){ m_nStandID = nID;}

	const ALTObjectID& GetStandName();
	void SetStandName(const ALTObjectID& altName);

	void AddDoorOperation(ACTypeDoor* pDoor);
	bool GetDoorOperation(ACTypeDoor* pDoor,std::vector<OperationDoor>& vDoorOp);
	int GetOpDoorCount();
    std::vector<OperationDoor>* GetDoorOpList(){ return &m_vOpDoors; }

 	void DelOpDoor(OperationDoor* pDoorOp);
	bool findOpDoor(OperationDoor* pDoorOp);
	void SetAirportDatabase(CAirportDatabase* pAirportDatabase){ m_pAirportDB = pAirportDatabase; }
	void SetAircraftType(const CString& strActype) {m_strActype = strActype;}

	void ReadData(CADORecordset& recordset);
	void SaveData(int nParentID);
	void UpdateData(int nParentID,const CString& strDoors);
	void DeleteData();
private: 
	void DoCompatible(const CString& strDoors,ACTYPEDOORLIST* pDoorList);
	ACTypeDoor* GetAcTypeDoor(int nID,ACTYPEDOORLIST* pDoorList)const;
private:
	int m_nID; //database key

	int m_nStandID;
	CString m_strActype;
	ALTObjectID m_standName;
	CAirportDatabase* m_pAirportDB;
	std::vector<OperationDoor> m_vOpDoors;
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
