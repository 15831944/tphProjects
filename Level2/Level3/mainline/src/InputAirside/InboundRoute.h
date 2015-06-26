#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "InboundRouteHead.h"
#include "InputAirsideAPI.h"

class CAirsideImportFile;
class CAirsideExportFile;
#define INBOUD_ROUTE  0x0110 
class INPUTAIRSIDE_API CInboundRoute : public DBElementCollection<CInboundRouteHead>
{
public:
	CInboundRoute(void);
	virtual ~CInboundRoute(void);

public:
	void SetInboundRouteAssignmentID(int nInboundRouteAssignment);
	int GetInboundRouteAssignmentID();
	void SetRunwayID(int nRunwayID);
	int GetRunwayID();
	void SetMarkingIndexID(int nMarkingIndexID);
	int GetMarkingIndexID();
	//void SetTaxiwayID(int nTaxiwayID);
	//int GetTaxiwayID();
	void SetStandGroupID(int nStandGroupID);
	int GetStandGroupID();
	void SetRunwayExitID(int nRunwayExitID);
	int GetRunwayExitID();
	void SetRunwayAllExitFlag(BOOL bRunwayAllExitFlag);
	BOOL GetRunwayAllExitFlag();
	//DBElementCollection
	virtual void SaveData(int nID);
	virtual void DeleteData();
	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
   
public:
	void DeleteDataFromDB();

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
 
private:
	int m_nInboundRouteAssignmentID;
	int m_nRunwayID;
	int m_nMarkingIndexID;
	//int m_nTaxiwayID;
	int m_nRunwayExitID;
	int m_nStandGroupID;
	BOOL m_bRunwayExitAll;   //TRUE:all exit  FALSE:not all exit


};
