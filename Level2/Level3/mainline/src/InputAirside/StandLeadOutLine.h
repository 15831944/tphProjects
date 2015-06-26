#pragma once
#include "../Common/path2008.h"
#include "InputAirsideAPI.h"

class CADORecordset;
class CAirsideExportFile;
class CAirsideImportFile;

class INPUTAIRSIDE_API StandLeadOutLine 
{

friend class Stand;
friend class StandLeadOutLineList;

public:
	StandLeadOutLine(void);
	~StandLeadOutLine(void);

public:

	CPath2008 GetPath()const;
	CPath2008& getPath();
	void SetPath(CPath2008 path);

	BOOL IsPushBack()const;
	void SetPushBack(BOOL b);

	BOOL IsSetReleasePoint()const;
	void toSetReleasePoint(BOOL bSet);

	CString GetName()const;
	void SetName(const CString& str);

	void CopyData(const StandLeadOutLine& otherLine);
	void ReadPath();

	int GetID()const;
	void SetID(int nID);

	void SetReleasePoint(const CPoint2008& pt);
	const CPoint2008 GetReleasePoint()const;
	DistanceUnit GetDistance()const;

	BOOL PathPointMoreThanOne() const;

protected:
	int m_nID;
	CPath2008 m_path;
	int m_nPathID;
	BOOL m_bPushBack;
	CString m_strName;
	DistanceUnit m_dlt;
	BOOL m_bSetReleasePoint;

public:
	void GetInsertSQL(int nParentID,CString& strSQL) const;
	void GetUpdateSQL(CString& strSQL) const;
	void GetDeleteSQL(CString& strSQL) const;
	void InitFromDBRecordset( CADORecordset& recordset );

	void DeleteData();
	void SaveData(int nStandID);

	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);

};


class INPUTAIRSIDE_API StandLeadOutLineList 
{
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
public:
	void CopyData(const StandLeadOutLineList& otherList);

	size_t GetItemCount()const; 
	StandLeadOutLine& ItemAt(size_t i);
	const StandLeadOutLine& ItemAt(size_t i)const;
	
	void AddNewItem(const StandLeadOutLine& newLine);
	void InitItemsName();

	void DeleteItem(int idx);

	void ReadData(int nParentID);
	void SaveData(int nParentID);
	void DeleteData();

	StandLeadOutLine* GetItemByID(int nID);

	BOOL IsValid() const;


protected:
	void ReadDataList(int nParentID, std::vector<StandLeadOutLine>& vList);	

protected:
	std::vector<StandLeadOutLine> m_vDataList;
};