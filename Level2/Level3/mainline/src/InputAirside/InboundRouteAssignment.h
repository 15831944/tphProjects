#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "inboundroute.h"
#include <InputAirside/RouteAssignment.h>
#include "InputAirsideAPI.h"

class CAirsideImportFile;
class CAirsideExportFile;


class INPUTAIRSIDE_API CInboundRouteAssignment : public DBElementCollection<CInboundRoute>
{
public:
	CInboundRouteAssignment(void);
	virtual ~CInboundRouteAssignment(void);

public:
	void SetRouteAssignmentStatus(ROUTEASSIGNMENTSTATUS raStatus);
	ROUTEASSIGNMENTSTATUS GetRouteAssignmentStatus();

	//DBElementCollection
	virtual void GetSelectElementSQL(int nProjectID,CString& strSQL) const;
	virtual void ReadData(int nProjectID);
	virtual void SaveData(int nProjID);
	virtual void DeleteItemFromIndex(size_t nIndex);
	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nProjID, CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

	static void ExportInboundRouteAssignment(CAirsideExportFile& exportFile);
	static void ImportInboundRouteAssignment(CAirsideImportFile& importFile);

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

public:
	ROUTEASSIGNMENTSTATUS m_nRouteAssignmentStatus; //1:shortest path to stand  2:time shortest path to stand  3:preferred route
private:
	int TimeRangeID ;
		void DelAllItemFromDB();
public: 
	void SetTimeRangeID(int Id) {TimeRangeID = Id ;};

    void DeleteData() ;
};