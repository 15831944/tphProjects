#pragma once
#include "AirsideImportExportManager.h"
#include "common/AirportDatabase.h"
#include "FlightServicingRequirement.h"
#include "Common/FLT_CNST.H"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CFlightServicingRequirementList : public DBElementCollection<CFlightServicingRequirement>
{
public:
	CFlightServicingRequirementList(void);
	virtual ~CFlightServicingRequirementList(void);

	//DBElementCollection
	virtual void GetSelectElementSQL(int nProjectID,CString& strSQL) const;
	virtual void ReadData(int nProjectID, CVehicleSpecifications* pVehicleSpec);
	virtual void SaveData(int nProjID);
	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset, CVehicleSpecifications* pVehicleSpec);

	void SetAirportDatabase(CAirportDatabase* pAirportDB);

public:
	static void ExportFlightServiceRequirement(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB);
	static void ImportFlightServiceRequirement(CAirsideImportFile& importFile);

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
protected:
	CAirportDatabase* m_pAirportDatabase;
};
