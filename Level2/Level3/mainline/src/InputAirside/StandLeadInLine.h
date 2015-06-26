#pragma once
#include "../Common/path2008.h"
#include "InputAirsideAPI.h"

class StandLeadOutLine;
class CADORecordset;
class CAirsideExportFile;
class CAirsideImportFile;

class INPUTAIRSIDE_API StandLeadInLine 	
{
	friend class Stand;
	friend class StandLeadInLineList;
public:
	StandLeadInLine(void);
	~StandLeadInLine(void);

public:
	CPath2008 GetPath()const;
	CPath2008& getPath();
	void SetPath(const CPath2008& _path);

	CString GetName()const;
	void SetName(const CString& str);

	void CopyData(const StandLeadInLine& otherLine);

	int GetID()const;
	void SetID(int nID);

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

	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);

	void DeleteData();
	void SaveData(int nStandID);

	void ReadPath();

	BOOL PathPointMoreThanOne() const;

public:


};



class INPUTAIRSIDE_API StandLeadInLineList 
{
public:
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
	void CopyData(const StandLeadInLineList& otherList);
	
	size_t GetItemCount()const; 
	StandLeadInLine& ItemAt(size_t i);
	const StandLeadInLine& ItemAt(size_t i)const;

	void AddNewItem(const StandLeadInLine& newLine);
	void DeleteItem(int idx);
	void InitItemsName();

	void ReadData(int nParentID);
	void SaveData(int nParentID);
	void DeleteData();

	StandLeadInLine* GetItemByID(int nID);

	BOOL IsValid() const;

protected:
	void ReadDataList(int nParentID, std::vector<StandLeadInLine>& vList);

protected:
	std::vector<StandLeadInLine> m_vDataList;
};
