#pragma once

#include "InputAirsideAPI.h"
#include "AirsideImportExportManager.h"
#include "common/AirportDatabase.h"
#include "TowOffStandAssignmentData.h"
#include "..\Database\DBElementCollection_Impl.h"


class ARCFlight;
class INPUTAIRSIDE_API CTowOffStandAssignmentDataList : public DBElementCollection<CTowOffStandAssignmentData>
{
public:
	CTowOffStandAssignmentDataList(TowOffDataContentNonNull NullOrNonNull = ContentNull);
	~CTowOffStandAssignmentDataList(void);

	//DBElementCollection
	virtual void GetSelectElementSQL(int nProjectID,CString& strSQL) const;
	virtual void ReadData(int nParentID);
// 	virtual void SaveData(int nProjID);
 
	void SetAirportDatabase(CAirportDatabase* pAirportDB);

	CTowOffStandAssignmentData* GetFlightFitData(ARCFlight* pflight, const ALTObjectID& standID);
	CTowOffStandAssignmentData* GetFlightTowoffData(ARCFlight* pflight, const ALTObjectID& standID);
	void GetFlightTowoffDataList(ARCFlight* pFlight, const ALTObjectID& standName, std::map<CString,CTowOffStandAssignmentData*>& mapDataList);
public:
	static void ExportTowOffStandAssignment(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB);
	static void ImportTowOffStandAssignment(CAirsideImportFile& importFile);

private:
	CAirportDatabase* m_pAirportDatabase;
};
