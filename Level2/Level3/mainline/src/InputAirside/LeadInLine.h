#pragma once
#include "../Common/path2008.h"
#include "InputAirsideAPI.h"

class CADORecordset;

class INPUTAIRSIDE_API LeadInLine
{
		friend class LeadInLineList;
public:
	LeadInLine(void);
	~LeadInLine(void);

public:
	CPath2008 GetPath()const;
	CPath2008& getPath();
	void SetPath(const CPath2008& _path);

	CString GetName()const;
	void SetName(const CString& str){ m_strName = str; }

	void CopyData(const LeadInLine& otherLine);

	int GetID()const;
	void SetID(int nID){ m_nID = nID; }


protected:
	int m_nID;
	CPath2008 m_path;
	int m_nPathID;
	CString m_strName;

public:
	void GetInsertSQL(int nParentID,CString& strSQL) const;
	void GetUpdateSQL(CString& strSQL) const;
	void GetDeleteSQL(CString& strSQL) const;
	void InitFromDBRecordset( CADORecordset& recordset );

	void DeleteData();
	void SaveData(int nStandID);

	void ReadPath();
};

class INPUTAIRSIDE_API LeadInLineList 
{
public:
	virtual void GetSelectElementSQL(int nParentID, CString& strSQL) const;
	void CopyData(const LeadInLineList& otherList);

	size_t GetItemCount()const; 
	LeadInLine& ItemAt(int nIdx);
	const LeadInLine& ItemAt(int nIdx)const;

	void AddNewItem(const LeadInLine& newLine);
	void DeleteItem(int idx);
	void InitItemsName();

	void ReadData(int nParentID);
	void SaveData(int nParentID);
	void DeleteData();

	LeadInLine* GetItemByID(int nID);


protected:
	void ReadDataList(int nParentID, std::vector<LeadInLine>& vList);

protected:
	std::vector<LeadInLine> m_vDataList;
};

