#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "ALTObject.h"
#include "InputAirsideAPI.h"


class CAirsideExportFile;
class CAirsideImportFile;

class INPUTAIRSIDE_API CDirectRouting : public DBElement
{
public:
	CDirectRouting();
	virtual ~CDirectRouting();

public:
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

private:
	int m_nARWaypointStartID;
	int m_nARWaypointEndID;
	int m_nARWaypointMaxID;

	//	int			m_nProjectID;

public:
	void SetARWaypointStartID(int nARWaypointStartID){m_nARWaypointStartID = nARWaypointStartID;}
	int GetARWaypointStartID(){return m_nARWaypointStartID;}

	void SetARWaypointEndID(int nARWaypointEndID){m_nARWaypointEndID = nARWaypointEndID;}
	int GetARWaypointEndID(){return m_nARWaypointEndID;}

	void SetARWaypointMaxID(int nARWaypointMaxID){m_nARWaypointMaxID = nARWaypointMaxID;}
	int GetARWaypointMaxID(){return m_nARWaypointMaxID;}
	
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
};

class INPUTAIRSIDE_API CDirectRoutingList : public DBElementCollection<CDirectRouting>
{
public:
	typedef DBElementCollection<CDirectRouting> BaseClass;
	CDirectRoutingList();
	virtual ~CDirectRoutingList();
protected:
	//	virtual void InitFromDBRecordset(CADORecordset& recordset) {}
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;

public:
	static void ExportDirectRoutings(CAirsideExportFile& exportFile);
	static void ImportDirectRoutings(CAirsideImportFile& importFile);
private:
	//export dll symbols
	void DoNotCall();
};