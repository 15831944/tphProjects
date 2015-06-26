#pragma once
#include "../Common/path2008.h"
#include "LeadInLine.h"
#include "LeadOutLine.h"

class CADORecordset;

class INPUTAIRSIDE_API ParkingLotExit
{
	friend class ParkingLotExitList;
public:
	ParkingLotExit(void);
	~ParkingLotExit(void);

public:
	CPath2008 GetPath()const;
	CPath2008& getPath();
	void SetPath(const CPath2008& _path);

	CString GetName()const;
	void SetName(const CString& str){ m_strName = str; }

	int GetID()const;
	void SetID(int nID){ m_nID = nID; }
	ParkingLotExit& operator= (const ParkingLotExit& otherLine);

public:
	void GetInsertSQL(int nParentID,CString& strSQL) const;
	void GetUpdateSQL(CString& strSQL) const;
	void GetDeleteSQL(CString& strSQL) const;
	void InitFromDBRecordset( CADORecordset& recordset );

	void DeleteData();
	void SaveData(int nStandID);

	void ReadPath();

	LeadInLineList& GetLeadInLineList();
	LeadOutLineList& GetLeadOutLineList();

protected:
	int m_nID;
	CPath2008 m_path;
	int m_nPathID;
	CString m_strName;

	LeadInLineList m_vLeadInLines;
	LeadOutLineList m_vLeadOutLines;
};

class INPUTAIRSIDE_API ParkingLotExitList 
{

public:
	ParkingLotExitList();
	~ParkingLotExitList();

	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
	void CopyData(const ParkingLotExitList& otherList);

	int GetItemCount()const; 
	ParkingLotExit* ItemAt(int nIdx);

	void AddNewItem(ParkingLotExit* newLine);
	void DeleteItem(int idx);
	void InitItemsName();

	void ReadData(int nParentID);
	void SaveData(int nParentID);
	void DeleteData();

	void ClearList();

	ParkingLotExit* GetItemByID(int nID);

private:
	std::vector<ParkingLotExit*> m_vExitList;
	std::vector<ParkingLotExit*> m_vDelExitList;
};