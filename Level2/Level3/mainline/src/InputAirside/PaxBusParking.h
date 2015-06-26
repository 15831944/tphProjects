#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "../Common/Path2008.h"
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;

enum Gatetype
{	
	depgate,
	arrgate
};
class INPUTAIRSIDE_API CPaxBusParking : public DBElement
{
public:
	CPaxBusParking();
	virtual ~CPaxBusParking();

public:
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

private:
	CString m_strGate;
	Gatetype m_enumGateType;
	int m_nPathID;
	CPath2008 m_path;

public:
	void SaveData(int nParentID);
	void SetGate(CString strGate);
	CString GetGate();

	void SetGatetype(Gatetype enumGateType);
	Gatetype GetGatetype();

	const CPath2008& GetPath()const;
	CPath2008& getPath();
	void SetPath(const CPath2008& _path);
	void UpdatePath();
	
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
};

class INPUTAIRSIDE_API CPaxBusParkingList : public DBElementCollection<CPaxBusParking>
{
public:
	CPaxBusParkingList();
	virtual ~CPaxBusParkingList();
protected:
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
	

public:
	size_t GetItemCount();
	CPaxBusParking *GetItemByIndex(size_t nIndex);

	static void ExportPaxBusParkings(CAirsideExportFile& exportFile);
	static void ImportPaxBusParkings(CAirsideImportFile& importFile);
private:
	//dll symbols export
	void DoNotCall();
};