#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "FlightTypeRestriction.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CFlightTypeRestrictionList :public DBElementCollection<CFlightTypeRestriction>
{
public:
	CFlightTypeRestrictionList(void);
	 ~CFlightTypeRestrictionList(void);

private:
	int m_nProjID;
	CAirportDatabase* m_pAirportDatabase;
public:	
	void SetProjID(int nProjID);
	int  GetProjID() const;
	void SetAirportDatabase(CAirportDatabase* pAirportDatabase);
	virtual void GetSelectElementSQL(int nParentID, CString &strSQL);
	virtual void ReadData(int nProjectID);
	void InitFromDBRecordset(CADORecordset& recordset);	
	static void ImportTaxiwayFlightTypeRestriction(CAirsideImportFile& importFile);
	static void ExportTaxiwayFlightTypeRestriction(CAirsideExportFile& exportFile);
};
