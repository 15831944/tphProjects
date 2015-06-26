#ifndef _VEHICLETAGITEM_HEADER_
#define _VEHICLETAGITEM_HEADER_

#pragma once
#include "VehicleType.h"
#include "AirsideImportExportManager.h"
#include "VehicleSpecificationItem.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CVehicleTagItem
{
public:
	//CVehicleTagItem(void);
	CVehicleTagItem(int nProjID);
	~CVehicleTagItem(void);

	void ReadData(int nID);
	void SaveData(int nParentID);
	void DeleteData(void);
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

	static void GetVehicleTagItemList(int nParentID,std::vector<int> & vrVehicleTagItemID);

	int GetParentID(void);
	int GetID(void);
protected:
	int m_nID;
	int m_nParentID;
	int m_nProjID;
public:
	CVehicleSpecificationItem m_vehicleItem;
	BOOL m_bShowTags;
	DWORD m_dwTagInfo; //bit mask to determine what tag info to show
};

#endif  
