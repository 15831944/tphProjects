#pragma once
#include "../Database/DBElementCollection.h"
class CServicingRequirement;
class CAirsideExportFile;
class CAirsideImportFile;

class CServicingRequirementList : public DBElementCollection<CServicingRequirement>
{
public:
	CServicingRequirementList(void);
	virtual ~CServicingRequirementList(void);

public:
	//DBElementCollection
	virtual void GetSelectElementSQL(int nProjectID,CString& strSQL) const;

	virtual void ReadData(int nProjectID);
	void InitFromDBRecordset(CADORecordset& recordset);

public:
	static void ExportServicingRequirement(CAirsideExportFile& exportFile);
	static void ImportServicingRequirement(CAirsideImportFile& importFile);

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
};
