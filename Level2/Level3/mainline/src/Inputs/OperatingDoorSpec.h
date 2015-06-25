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

	struct OperationDoor
	{
		CString GetDoorString()const
		{
			CString strOp;
			if (m_iHandType == ACTypeDoor::LeftHand)
			{
				strOp.Format(_T("L%d"),m_nID);
			}
			else if (m_iHandType == ACTypeDoor::RightHand)
			{
				strOp.Format(_T("R%d"),m_nID);
			}

			return strOp;
		}

		CString GetHandTypeString()const
		{
			if (m_iHandType == ACTypeDoor::LeftHand)
			{
				return CString("Left");
			}
			else if (m_iHandType == ACTypeDoor::RightHand)
			{
				return CString("Right");
			}

			return CString("Left");
		}
		void ParseString(const CString& strOp)
		{
			CString strLeft;
			CString strRight;
			strLeft = strOp.Left(1);
			strRight = strOp.Right(1);
			if (strLeft.Compare("L") == 0)
			{
				m_iHandType = ACTypeDoor::LeftHand;
			}
			else
			{
				m_iHandType = ACTypeDoor::RightHand;
			}

			m_nID = atoi(strRight);
		}

		bool operator == (const OperationDoor& doorOp)const
		{
			if (doorOp.m_nID != m_nID)
			{
				return false;
			}
			
			if (doorOp.m_iHandType != m_iHandType)
			{
				return false;
			}
		
			return true;
		}

		int m_nID;
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
// 	int GetOpDoorID(int nIdx);
// 	void GetOpDoors(std::vector<int>& vDoors);
// 
// 	void AddOpDoor(int nID);
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
//	std::vector<int> m_vOpDoors;
	CAirportDatabase* m_pAirportDB;

public:
	std::vector<OperationDoor> m_vOpDoors;;

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
