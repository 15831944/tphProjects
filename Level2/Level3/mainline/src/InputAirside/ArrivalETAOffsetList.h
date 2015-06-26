#pragma once
#include "../Database/DBElementCollection_Impl.h"
#include "ArrivalETAOffset.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CArrivalETAOffsetList :
	public DBElementCollection<CArrivalETAOffset>
{
public:
	CArrivalETAOffsetList(void);
	virtual ~CArrivalETAOffsetList(void);

public:
	//DBElementCollection
	virtual void GetSelectElementSQL(int nProjectID,CString& strSQL) const;
	virtual void ReadData(int nProjectID);
	virtual void SaveData(int nParentID);
	virtual void DeleteItemFromIndex(size_t nIndex);
	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	void SetAirportDatabase(CAirportDatabase* pAirportDB){ m_pAirportDatabase = pAirportDB; }

public:
	static void ExportArrivalETAOffset(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB);
	static void ImportArrivalETAOffset(CAirsideImportFile& importFile);

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

	CArrivalETAOffset * FindBestMatchETAOffset(const AirsideFlightDescStruct& fltDesc)const;

	void SortByFlightType();

	CAirportDatabase * GetAirportDB()const{ return m_pAirportDatabase; }
private:
	CAirportDatabase* m_pAirportDatabase;
};
