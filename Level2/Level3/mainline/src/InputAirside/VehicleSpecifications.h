#pragma once

#include "VehicleSpecificationItem.h"
#include "InputAirsideAPI.h"
#include "../Common/AirportDatabase.h"

#define VehicleSize  19

class INPUTAIRSIDE_API CVehicleSpecifications: public DBElementCollection<CVehicleSpecificationItem>
{
public:
	CVehicleSpecifications(void);
	virtual ~CVehicleSpecifications(void);
	virtual void ReadData(int nParentID);
	//virtual void ReadData(int nParentID,CAirportDatabase *pAirportDatabase);
	//virtual void  SaveData(int nParentID,CAirportDatabase *pAirportDatabase);
	virtual void SaveData(int nParentID);
	//void ReadDataOperationType(int nParentID);
	//void SaveDataOperationType(int nParentID);
	//void DeleteItemOperationType(int nItem);

	/*virtual void DeleteData(CAirportDatabase *pAirportDatabase);
	virtual void SaveData(int nParentID,CAirportDatabase *pAirportDatabase);*/
protected:
	
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
	bool CheckDataList();


public:
	//void ExportData(CAirsideExportFile& exportFile);
	//void ImportData(CAirsideImportFile& importFile);
	CVehicleSpecificationItem* GetVehicleItemByName(CString strName);
	CVehicleSpecificationItem* GetVehicleItemByID(int nID);
	void GetVehicleIDByBaseType(enumVehicleBaseType type, std::vector<int>& vIDs);
	static void ExportVehicleSpec(CAirsideExportFile& exportFile);
	static void ImportVehicleSpec(CAirsideImportFile& importFile);

private:
	//dll symbols export
	void DoNotCall();
	

};
