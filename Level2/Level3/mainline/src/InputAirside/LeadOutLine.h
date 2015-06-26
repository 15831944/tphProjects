#pragma once
#include "../Common/path2008.h"
#include "InputAirsideAPI.h"

class CADORecordset;

class INPUTAIRSIDE_API LeadOutLine
{
	friend class LeadOutLineList;
public:
	LeadOutLine(void);
	~LeadOutLine(void);
public:

	CPath2008 GetPath()const;
	CPath2008& getPath();
	void SetPath(const CPath2008& path){ m_path = path; }

	CString GetName()const;
	void SetName(const CString& str){ m_strName = str; }

	void CopyData(const LeadOutLine& otherLine);
	void ReadPath();

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
	void SaveData(int nParentID);

};


class INPUTAIRSIDE_API LeadOutLineList 
{
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
public:
	void CopyData(const LeadOutLineList& otherList);

	int GetItemCount()const; 
	LeadOutLine& ItemAt(int idx);
	const LeadOutLine& ItemAt(int idx)const;

	void AddNewItem(const LeadOutLine& newLine);
	void InitItemsName();

	void DeleteItem(int idx);

	void ReadData(int nParentID);
	void SaveData(int nParentID);
	void DeleteData();

	LeadOutLine* GetItemByID(int nID);



protected:
	void ReadDataList(int nParentID, std::vector<LeadOutLine>& vList);	


protected:
	std::vector<LeadOutLine> m_vDataList;
};
