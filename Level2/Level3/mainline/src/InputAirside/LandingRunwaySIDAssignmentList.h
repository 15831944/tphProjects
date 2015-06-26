#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "InputAirsideAPI.h"

class CLandingRunwaySIDAssignment;
class CAirsideImportFile;
class CAirsideExportFile;
class CFlightUseSIDList;
class CAirportDatabase;

class INPUTAIRSIDE_API CLandingRunwaySIDAssignmentList : public DBElementCollection<CLandingRunwaySIDAssignment>
{
public:
	typedef DBElementCollection<CLandingRunwaySIDAssignment> BaseClass;
	CLandingRunwaySIDAssignmentList();
	virtual ~CLandingRunwaySIDAssignmentList(void);

public:
	void GetItemByRunwayIDAndMarking(int nRunwayID, int nMarking, CLandingRunwaySIDAssignment ** ppLandingRunwaySIDAssignment);
	BOOL IsConflictTime(int nRunwayID, CFlightUseSIDList * pFlightUseSIDList);
	void DeleteItemByRunwayID(int nRunwayID);

	//DBElementCollection
	virtual void GetSelectElementSQL(int nProjectID,CString& strSQL) const;
	virtual void ReadData(int nProjectID);
	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	void SetAirportDatabase(CAirportDatabase* pAirportDB){ m_pAirportDatabase = pAirportDB; }

public:
	static void ExportLandingRunwaySIDAssignment(CAirsideExportFile& exportFile);
	static void ImportLandingRunwaySIDAssignment(CAirsideImportFile& importFile);

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
protected:
	CAirportDatabase* m_pAirportDatabase;

};