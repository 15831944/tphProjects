#pragma once
#include "..\Database\DBElementCollection_Impl.h"

class CLandingRunwayStarAssignment;
class CAirsideImportFile;
class CAirsideExportFile;
class CFlightUseStarList;
class CAirportDatabase;

class INPUTAIRSIDE_API CLandingRunwayStarAssignmentList : public DBElementCollection<CLandingRunwayStarAssignment>
{
public:
	typedef DBElementCollection<CLandingRunwayStarAssignment> BaseClass;
	CLandingRunwayStarAssignmentList();
	virtual ~CLandingRunwayStarAssignmentList(void);

public:
	void GetItemByRunwayIDAndMarking(int nRunwayID, int nMarking, CLandingRunwayStarAssignment ** ppLandingRunwayStarAssignment);
	BOOL IsConflictTime(int nRunwayID, CFlightUseStarList * pFlightUseStarList);
	void DeleteItemByRunwayID(int nRunwayID);

	//DBElementCollection
	virtual void GetSelectElementSQL(int nProjectID,CString& strSQL) const;
	virtual void ReadData(int nProjectID);
	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	void SetAirportDatabase(CAirportDatabase* pAirportDB){ m_pAirportDatabase = pAirportDB; }

public:
	static void ExportLandingRunwayStarAssignment(CAirsideExportFile& exportFile);
	static void ImportLandingRunwayStarAssignment(CAirsideImportFile& importFile);

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
	CAirportDatabase* m_pAirportDatabase;

};
