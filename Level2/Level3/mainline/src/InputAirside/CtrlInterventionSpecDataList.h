#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "InputAirsideAPI.h"

#include "CtrlInterventionSpecData.h"
class CAirsideImportFile;
class CAirsideExportFile;

class INPUTAIRSIDE_API CCtrlInterventionSpecDataList : public DBElementCollection<CCtrlInterventionSpecData>
{
public:
	typedef DBElementCollection<CCtrlInterventionSpecData> BaseClass;
	CCtrlInterventionSpecDataList(void);
	virtual ~CCtrlInterventionSpecDataList(void);

public:
	//DBElementCollection
	virtual void GetSelectElementSQL(int nProjectID,CString& strSQL) const;

	virtual void ReadData(int nProjectID);
	void InitFromDBRecordset(CADORecordset& recordset);

	bool IsSideStepPreGoAround(int nRunwayID, int rw_mark)const;
public:
	static void ExportCtrlInterventionSpec(CAirsideExportFile& exportFile);
	static void ImportCtrlInterventionSpec(CAirsideImportFile& importFile);

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
};
